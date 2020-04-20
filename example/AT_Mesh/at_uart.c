#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/light_ll/light_ll.h"
#include "proj_lib/light_ll/light_frame.h"
#include "proj_lib/ble_ll/service.h"
#include "vendor/common/rtc.h"
#include "proj/drivers/flash.h"
#include "vendor/common/common.h"
#include "vendor/common/scene.h"
#include "proj_lib/ble_ll/blueLight.h"
#include "proj/mcu/register.h"

#include "proj/drivers/uart.h"

#define UART_DATA_LEN    44      // data max 252
typedef struct{
    u32 len;        // data max 252
    u8 data[UART_DATA_LEN];
}uart_data_t;
STATIC_ASSERT((sizeof(uart_data_t) % 16) == 0);

uart_data_t T_txdata_user;
uart_data_t T_txdata_buf;      // not for user

uart_data_t T_rxdata_user;
uart_data_t T_rxdata_buf;   // data max 252, user must copy rxdata to other Ram,but not use directly
unsigned char uart_rx_true;

void at_uart_init()
{
	//note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
    uart_recbuff_init((unsigned short *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
	uart_gpio_set(UART_TX_PIN, UART_RX_PIN);// uart tx/rx pin set

	uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

	//baud rate: 115200
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	uart_init(9, 13, PARITY_NONE, STOP_BIT_ONE);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	uart_init(249, 9, PARITY_NONE, STOP_BIT_ONE);
	#endif
	
	uart_dma_enable(1, 1);	//uart data in hardware buffer moved by dma, so we need enable them first

	irq_set_mask(FLD_IRQ_DMA_EN);
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);		//uart Rx/Tx dma irq enable
	reg_dma_rx_rdy0 = FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX;
}

void at_uart_irq()
{
    static unsigned char enterRXIrq,enterTXIrq;
    unsigned char irqS = uart_IRQSourceGet();
    if(irqS & FLD_DMA_CHN_UART_RX){
        uart_rx_true = 1;
        enterRXIrq++;
    }
    
    if(irqS & FLD_DMA_CHN_UART_TX){
        uart_clr_tx_busy_flag();
        enterTXIrq++;
    }
}

void at_uart_loop()
{
    if(uart_rx_true){
        uart_rx_true = 0;
        u32 rx_len = T_rxdata_buf.len + 4 > sizeof(T_rxdata_user) ? sizeof(T_rxdata_user) : T_rxdata_buf.len + 4;
        memcpy(&T_rxdata_user, &T_rxdata_buf, rx_len);

		at_data_process(T_rxdata_user.data, T_rxdata_user.len);
        //uart_Send((u8 *)(&T_rxdata_user));
    }
}

void at_print(char * str)
{
    T_txdata_user.len = 0;
	while(*str)
	{
		T_txdata_user.data[T_txdata_user.len] = *str++;
		T_txdata_user.len += 1;
		if(T_txdata_user.len == UART_DATA_LEN)
		{
			uart_Send(&T_txdata_user);
			T_txdata_user.len = 0;
			WaitMs(20);
		}
	}

	if(T_txdata_user.len)
	{
		uart_Send(&T_txdata_user);
		T_txdata_user.len = 0;
		WaitMs(20);
	}
}

unsigned char hextab[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void at_print_hexstr(char * data, u32 len)
{
	unsigned char buf[128] = { 0 };
	for(int i =0; i < len; i ++)
	{
		buf[i*3] = hextab[(data[i] >> 4)];
		buf[i*3 +1] = hextab[(data[i]&0xf)];
		buf[i*3 +2] = ' ';
	}
	at_print((char*)buf);
}

void at_send(char * data, u32 len)
{
	while(len > UART_DATA_LEN)
	{
		memcpy(T_txdata_user.data, data,  UART_DATA_LEN);
		data += UART_DATA_LEN;
		len -= UART_DATA_LEN;

		T_txdata_user.len = UART_DATA_LEN;

		uart_Send(&T_txdata_user);
		T_txdata_user.len = 0;
		WaitMs(20);
	}

	if(len > 0)
	{
		memcpy(T_txdata_user.data, data,  len);
		T_txdata_user.len = len;
		uart_Send(&T_txdata_user);
		T_txdata_user.len = 0;
		WaitMs(20);
	}
}