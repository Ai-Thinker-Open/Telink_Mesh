/********************************************************************************************************
 * @file     flash.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#include "../tl_common.h"
#include "../drivers/spi.h"
#include "flash.h"
#include "spi_i.h"
#include "../mcu/watchdog_i.h"

static inline int flash_is_busy(){		// should be called by _attribute_ram_code_ function
	return mspi_read() & 0x01;				//  the busy bit, pls check flash spec
}

_attribute_ram_code_ static void flash_send_cmd(u8 cmd){
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

_attribute_ram_code_ static void flash_send_addr(u32 addr){
	mspi_write((u8)(addr>>16));
	mspi_wait();
	mspi_write((u8)(addr>>8));
	mspi_wait();
	mspi_write((u8)(addr));
	mspi_wait();
}

//  make this a asynchorous version
_attribute_ram_code_ static void flash_wait_done()
{
	sleep_us(100); 
	flash_send_cmd(FLASH_READ_STATUS_CMD);

	int i;
	for(i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
	mspi_high();
}

_attribute_ram_code_ void flash_erase_sector(u32 addr){
	u8 r = irq_disable();
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_SECT_ERASE_CMD);
	flash_send_addr(addr);	
	mspi_high();
	flash_wait_done();
	irq_restore(r);
}

//  Note: differrent size or type may use differrent command of block erase.
#if 0
_attribute_ram_code_ void flash_erase_block(u32 addr){
	u8 r = irq_disable();
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_BLK_ERASE_CMD);
	flash_send_addr(addr);	
	mspi_high();
	flash_wait_done();
	irq_restore(r);
}
#endif

_attribute_ram_code_ void flash_write_page_256(u32 addr, u32 len, u8 *buf){
	u8 r = irq_disable();
	// important:  buf must not reside at flash, such as constant string.  If that case, pls copy to memory first before write 
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_CMD);
	flash_send_addr(addr);
	
	u32 i;
	for(i = 0; i < len; ++i){
		mspi_write(buf[i]);		/* write data */
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();
	irq_restore(r);
}

#define PAGE_SIZE     256
#if (__PROJECT_OTA_BOOT__)
_attribute_ram_code_
#endif
void flash_write_page(u32 addr, u32 len, u8 *buf){
    u32 len_empty = PAGE_SIZE - (u8)addr;
    while(len){
        if(len >= len_empty){
            flash_write_page_256(addr, len_empty, buf);
            len -= len_empty;
            addr += len_empty;
            buf += len_empty;
            len_empty = PAGE_SIZE;
        }else{
            flash_write_page_256(addr, len, buf);
            len = 0;
        }
    }
}

_attribute_ram_code_ void flash_read_page(u32 addr, u32 len, u8 *buf){
	u8 r = irq_disable();
	flash_send_cmd(FLASH_READ_CMD);
	flash_send_addr(addr);	
	
	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
	mspi_wait();
	/* get data */
	for(int i = 0; i < len; ++i){
		*buf++ = mspi_get();
		mspi_wait();
	}
	mspi_high();
	irq_restore(r);
}

#if (__PROJECT_LIGHT_SWITCH__)
#define FLASH_PROTECT_ENABLE    0
#else
#define FLASH_PROTECT_ENABLE    0
#endif

u8 flash_protect_en = FLASH_PROTECT_ENABLE;

#if (!__PROJECT_OTA_BOOT__)
_attribute_ram_code_ u32 flash_get_jedec_id(){
    #if (__PROJECT_MASTER_LIGHT_8266__ || __PROJECT_MASTER_LIGHT_8267__)
    return FLASH_ID_MD25D40;       // save ram
    #else
	u8 r = irq_disable();
	flash_send_cmd(FLASH_GET_JEDEC_ID);
	u8 manufacturer = mspi_read();
	u8 mem_type = mspi_read();
	u8 cap_id = mspi_read();
	mspi_high();
	irq_restore(r);
	return (u32)((manufacturer << 24 | mem_type << 16 | cap_id));
	#endif
}

#if 0
#define FLASH_GET_KGD       0x4B

_attribute_ram_code_ u16 flash_get_KGD(){
	u8 r = irq_disable();
	flash_send_cmd(FLASH_GET_KGD);
	u16 kgd;
	kgd = mspi_read();
	kgd = (kgd << 8) + mspi_read();
	mspi_high();
	irq_restore(r);
	return kgd;
}
#endif

#if (FLASH_PROTECT_ENABLE)
STATIC_ASSERT(FLASH_ADR_MAC >= 0x70000);
#endif

FLASH_ADDRESS_EXTERN;
extern u32  ota_program_offset;

static u32 flash_id = 0;
static u16 T_flash_status = -1;

void flash_get_id(){
    flash_id = flash_get_jedec_id();
}

_attribute_ram_code_ u16 flash_status_read(){
    #if (__PROJECT_MASTER_LIGHT_8266__ || __PROJECT_MASTER_LIGHT_8267__)
    return 0;       // save ram
    #else
    u16 status = 0;
	u8 r = irq_disable();

	flash_send_cmd(FLASH_READ_STATUS_CMD);
	for(int i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
    status = mspi_read();
	mspi_wait();

	if(FLASH_ID_GD25Q40 == flash_id){
    	flash_send_cmd(FLASH_READ_STATUS_CMD1);
        status = status + (mspi_read() << 8);
    	mspi_wait();
	}
	
	mspi_high();

	irq_restore(r);
	return status;
	#endif
}
_attribute_ram_code_ void flash_status_write(u16 status){
    #if (__PROJECT_MASTER_LIGHT_8266__ || __PROJECT_MASTER_LIGHT_8267__)
    // save ram
    #else
	u8 r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_STATUS_CMD);
	mspi_write((u8)status);
	mspi_wait();
	if(FLASH_ID_GD25Q40 == flash_id){
    	mspi_write((u8)(status >> 8));
    	mspi_wait();
	}
	
	mspi_high();
	flash_wait_done();

	irq_restore(r);
	#endif
}

int flash_protect_GD25Q40B(u8 idx){
	u8 r = irq_disable();
	int ret = 0;
	u16 status;
    #if 1   // read status before write
	T_flash_status = status = flash_status_read();
	
	status &= (u16)(~((u16)(0x7c) | (BIT(6) << 8)));
	#else   // use default 0
	status = 0;
	#endif
	
	if(FLASH_NONE == idx){
	
	}else if(FLASH_GD25Q40_0x00000_0x1ffff == idx){
        status |= (u16)(0b01010 << 2);              //0x0028;//
	}else if(FLASH_GD25Q40_0x00000_0x3ffff == idx){
        status |= (u16)(0b01011 << 2);              //0x002c;//
	}else if(FLASH_GD25Q40_0x00000_0x6ffff == idx){
        status |= (u16)((0b00001 << 2) | (BIT(6)<< 8));   //0x4004;//
	}else if(FLASH_GD25Q40_0x40000_0x7ffff == idx){
        status |= (u16)(0b00011 << 2);              //0x000c;//
    }else{
        ret = -1;
    	irq_restore(r);
    	return ret;
    }

    if(T_flash_status == status){
        ret = 0;
    	irq_restore(r);
    	return ret;
    }
    
    flash_status_write(status);

	#if 1   // check
	sleep_us(100);
	T_flash_status = flash_status_read();
	if((T_flash_status & ((u16)((u16)(0x7c) | (BIT(6) << 8))))
	 != (status & ((u16)((u16)(0x7c) | (BIT(6) << 8))))){
        ret = -1;
	}
	#endif
	
	irq_restore(r);
	return ret;
}

int flash_protect_MD25D40(u8 idx){
	u8 r = irq_disable();
	int ret = 0;
	u16 status;
    #if 1   // read status before write
	T_flash_status = status = flash_status_read();
	
	status &= (u16)(~((u16)(0x1c)));
	#else   // use default 0
	status = 0;
	#endif
	
	if(FLASH_NONE == idx){
	
	}else if(FLASH_MD25D40_0x00000_0x3ffff == idx){
        status |= (u16)(0b110 << 2);              //0x0018;//
	}else if(FLASH_MD25D40_0x00000_0x6ffff == idx){
        status |= (u16)(0b100 << 2);              //0x0010;//
	}else{
        ret = -1;
    	irq_restore(r);
    	return ret;
    }

    if(T_flash_status == status){
        ret = 0;
    	irq_restore(r);
    	return ret;
    }
    
    flash_status_write(status);

	#if 1   // check
	sleep_us(100);
	T_flash_status = flash_status_read();
	if((T_flash_status & ((u16)(0x1c)))
	 != (status & ((u16)(0x7c)))){
        ret = -1;
	}
	#endif
	
	irq_restore(r);
	return ret;
}

int flash_protect_cmd(u8 idx){
    int ret = 0;
    if(FLASH_ID_GD25Q40 == flash_id){
        ret = flash_protect_GD25Q40B(idx);
    }else if(FLASH_ID_MD25D40 == flash_id){
        ret = flash_protect_MD25D40(idx);
    }else{
        ret = -1;
    }

    return ret;
}

int flash_protect(u8 idx){
    if(0 == flash_protect_en){
        return 0;
    }
    
    for(int i = 0; i < 5; ++i){
		if(0 == flash_protect_cmd(idx)){
			return 0;
		}
	}

	return -1;
}

int flash_protect_disable(){
    return flash_protect(FLASH_NONE);
}

int flash_protect_up256k(){
    if(FLASH_ID_GD25Q40 == flash_id){
        return flash_protect(FLASH_GD25Q40_0x00000_0x3ffff);
    }else if(FLASH_ID_MD25D40 == flash_id){
        return flash_protect(FLASH_MD25D40_0x00000_0x3ffff);
    }

    return -1;
}

int flash_protect_down256k(){
    if(FLASH_ID_GD25Q40 == flash_id){
        return flash_protect(FLASH_GD25Q40_0x40000_0x7ffff);
    }

    return -1;
}

int flash_protect_8266_normal(){
    if(FLASH_ID_GD25Q40 == flash_id){
        return flash_protect(FLASH_GD25Q40_0x00000_0x1ffff);
    }else if(FLASH_ID_MD25D40 == flash_id){
        if(0x30000 == flash_adr_mac){           // can not be protect
            return flash_protect(FLASH_NONE);
        }else{
            return flash_protect(FLASH_MD25D40_0x00000_0x3ffff);
        }
    }

    return -1;
}

int flash_protect_8267_normal(){    
    if(FLASH_ID_GD25Q40 == flash_id){
        if(0 == ota_program_offset){
            if(0x30000 == flash_adr_mac){
                return flash_protect(FLASH_GD25Q40_0x40000_0x7ffff);
            }else if(0x70000 == flash_adr_mac){
                return flash_protect(FLASH_GD25Q40_0x00000_0x6ffff);
            }
        }else{
            return flash_protect(FLASH_GD25Q40_0x00000_0x1ffff);
        }
    }else if(FLASH_ID_MD25D40 == flash_id){
        return flash_protect(FLASH_MD25D40_0x00000_0x6ffff);
    }

    return -1;
}

#if 0
int flash_protect_8269_normal(){        // same with 8267
    return flash_protect_8267_normal();
}
#endif

int flash_protect_8267_OTA_clr_flag(){
    if(FLASH_ID_GD25Q40 == flash_id){
        if(0 == ota_program_offset){
            return flash_protect(FLASH_GD25Q40_0x00000_0x3ffff);
        }else{
            return flash_protect(FLASH_GD25Q40_0x40000_0x7ffff);
        }
    }else if(FLASH_ID_MD25D40 == flash_id){
        return flash_protect(FLASH_NONE);
    }

    return -1;
}

int flash_unprotect_OTA_start(){
    if(FLASH_ID_GD25Q40 == flash_id){
        if(0x70000 == flash_adr_mac){
            #if (MCU_CORE_TYPE == MCU_CORE_8266)
            // keep normal
            #else
            return flash_protect(FLASH_NONE);
            #endif
        }else{
            // keep normal
        }
    }else if(FLASH_ID_MD25D40 == flash_id){
        return flash_protect(FLASH_NONE);
    }

    return -1;
}

int flash_protect_OTA_copy(){
    if(FLASH_ID_GD25Q40 == flash_id){
        return flash_protect(FLASH_GD25Q40_0x40000_0x7ffff);
    }else if(FLASH_ID_MD25D40 == flash_id){
        return flash_protect(FLASH_NONE);
    }

    return -1;
}

int flash_protect_OTA_data_erase(){
    #if (MCU_CORE_TYPE == MCU_CORE_8266)
    return flash_protect_8266_normal();
    #else
    return flash_protect(FLASH_NONE);
    #endif

    return -1;
}


void flash_protect_debug(){
#if 1
    static u8 T_protect_idx = 0;
    if(T_protect_idx){
        if(T_protect_idx == 0xff){
            T_protect_idx = 0;       // disprotect
        }
        flash_protect(T_protect_idx);
        T_protect_idx = 0;
    }

    #if (!__PROJECT_LIGHT_SWITCH__ && !__PROJECT_LPN__ && !__PROJECT_8263_SWITCH__ && !__PROJECT_8368_SWITCH__)
	static u32 tick;
	if (clock_time_exceed (tick, 40000))
	{
    	tick = clock_time();
    	static u8 st = 0;
    	u8 s = (!(gpio_read (GPIO_PD4))) || (!(gpio_read(GPIO_PD5))) || (!(gpio_read(GPIO_PC5))) || (!(gpio_read(GPIO_PC6)));
    	if ((!st) & s)
    	{
    	static u32 T_button;  T_button++;
            flash_protect(0);   // disprotect
    	}
    	st = s;
	}
	#endif
#endif    
}

#endif

