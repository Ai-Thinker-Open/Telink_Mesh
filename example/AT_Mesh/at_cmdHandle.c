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

#include "at_cmd.h"

#include "tinyFlash/tinyFlash.h"

#define AT_VERSION "V0.1"

#define STORAGE_NAME 1
#define STORAGE_BAUD 2
#define STORAGE_ATE  3
#define STORAGE_MODE 4

extern u8 baud_buf[];
extern  const u8 tbl_scanRsp[];
extern u8 my_scanRsp[32];
extern u8 ATE;
extern u8  mac_public[6];


u8 HEX2BYTE(u8 hex_ch)
{
	if (hex_ch >= '0' && hex_ch <= '9')
	{
		return hex_ch - '0';
	}

	if (hex_ch >= 'a' && hex_ch <= 'f')
	{
		return hex_ch - 'a' + 10;
	}

	if (hex_ch >= 'A' && hex_ch <= 'F')
	{
		return hex_ch - 'A' + 10;
	}

	return 0x00;
}

u8 HEX2BIN(u8 * p_hexstr, u8 * p_binstr)
{
	u8 bin_len = 0;
	u8 hex_len = strlen((char *)p_hexstr);
	u8 index = 0;

	if (hex_len % 2 == 1)
	{
		hex_len -= 1;
	}

	bin_len = hex_len / 2;
	for(index = 0; index < hex_len; index+=2)
	{
		p_binstr[index/2] = ((HEX2BYTE(p_hexstr[index]) << 4) & 0xF0) + HEX2BYTE(p_hexstr[index + 1]); 
	}

	return bin_len;
}

u16 HEX2U16(u8 * p_hexstr) //将16进制字符串转换成U16类型的整数
{
	u8 hexStr_len = strlen((char *)p_hexstr);
	u16 numBer = 0;
	u8 index = 0;

	for(index = 0; index < hexStr_len; index++)
	{
		numBer <<=4;
		numBer += HEX2BYTE(p_hexstr[index]);
	}

	return numBer;
}

u16 STR2U16(u8 * p_hexstr) //将10进制字符串转换成U16类型的整数
{
	u8 hexStr_len = strlen((char *)p_hexstr);
	u16 numBer = 0;
	u8 index = 0;

	for(index = 0; index < hexStr_len; index++)
	{
		numBer *= 10;
		numBer += HEX2BYTE(p_hexstr[index]);
	}

	return numBer;
}

static unsigned char atCmd_ATE0(char *pbuf,  int mode, int lenth)
{
	// ATE = 0;
	// tinyFlash_Write(STORAGE_ATE, &ATE, 1);
	return 0;
}

static unsigned char atCmd_ATE1(char *pbuf,  int mode, int lenth)
{
	// ATE = 1;
	// tinyFlash_Write(STORAGE_ATE, &ATE, 1);
	return 0;
}

static unsigned char atCmd_GMR(char *pbuf,  int mode, int lenth)
{
	at_print("\r\n+VER:"AT_VERSION);
	return 0;
}

static unsigned char atCmd_Reset(char *pbuf,  int mode, int lenth)
{
	at_print("\r\nOK\r\n");
	light_sw_reboot();
	return 0;
}

static unsigned char atCmd_Sleep(char *pbuf,  int mode, int lenth)
{
	// at_print("\r\nOK\r\n");

	// gpio_setup_up_down_resistor(UART_RX_PIN, PM_PIN_PULLDOWN_100K);
	// cpu_set_gpio_wakeup (UART_RX_PIN, Level_Low, 1); 

	// cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
	return 0;
}

static unsigned char atCmd_Restore(char *pbuf,  int mode, int lenth)
{
	//tinyFlash_Format();
	at_print("\r\nOK\r\n");
	factory_reset();
	light_sw_reboot();
	return 0;
}

static unsigned char atCmd_Baud(char *pbuf,  int mode, int lenth)
{
	// if(mode == AT_CMD_MODE_READ)
	// {
	// 	at_print("\r\n+BAUD:");
	// 	at_print_buf[0] = baud_buf[0] + '0';
	// 	at_send(at_print_buf, 1);

	// 	return 0;
	// }

	// if(mode == AT_CMD_MODE_SET)
	// {
	// 	if((pbuf[0] >= '0') && (pbuf[0] <= '9'))
	// 	{
	// 		pbuf[0] -= '0';
	// 		tinyFlash_Write(STORAGE_BAUD, (unsigned char*)pbuf, 1);
	// 		return 0;
	// 	}
	// 	else
	// 	{
	// 		return 2;
	// 	}
	// }
	return 1;
}

static unsigned char atCmd_Name(char *pbuf,  int mode, int lenth)
{
	if(mode == AT_CMD_MODE_READ)
	{
		// memset(at_print_buf, 0, 64);

		// at_print("\r\n+NAME:");

		// if(my_scanRsp[1] == 0x09) //客户自定义的蓝牙设备名称
		// {
		// 	memcpy(at_print_buf, my_scanRsp+2, my_scanRsp[0] -1);
		// }
		// else
		// {
		// 	memcpy(at_print_buf, tbl_scanRsp+2, 10);
		// }
		
		// at_print((char*)at_print_buf);

		return 0;
	}

	if(mode == AT_CMD_MODE_SET)
	{
		tinyFlash_Write(3, (unsigned char*)pbuf, lenth);
		return 0;
	}
	return 1;
}

static unsigned char atCmd_Mac(char *pbuf,  int mode, int lenth)
{
	// if(mode == AT_CMD_MODE_READ)
	// {
	// 	at_print("\r\n+MAC:");
	// 	u_sprintf((char*)at_print_buf, "%X%X%X%X%X%X", mac_public[5], mac_public[4], mac_public[3], mac_public[2], mac_public[1], mac_public[0] );
	// 	at_print((char*)at_print_buf);
	// 	return 0;
	// }

	// if(mode == AT_CMD_MODE_SET)
	// {
	// 	if(lenth != 12) 
	// 	{
	// 		at_print("len error\r\n");
	// 		return 2;
	// 	}

	// 	for(lenth = 0; lenth < 12; lenth ++)
	// 	{
	// 		if(((pbuf[lenth] >= '0') && (pbuf[lenth] <= '9')) || ((pbuf[lenth] >= 'A') && (pbuf[lenth] <= 'F')))
	// 		{
	// 			if((pbuf[lenth] >= '0') && (pbuf[lenth] <= '9'))
	// 			{
	// 				pbuf[lenth] -= '0';
	// 			}
	// 			else
	// 			{
	// 				pbuf[lenth] -= 'A';
	// 				pbuf[lenth] += 0x0A;
	// 			}

	// 			if(lenth%2)
	// 			{
	// 				pbuf[lenth/2] = (pbuf[lenth-1] << 4) | pbuf[lenth];
	// 			}
	// 		}
	// 		else
	// 		{
	// 			return 2;
	// 		}
	// 	}

	// 	pbuf[6] = pbuf[0];
	// 	pbuf[0] = pbuf[5];
	// 	pbuf[5] = pbuf[6];

	// 	pbuf[6] = pbuf[1];
	// 	pbuf[1] = pbuf[4];
	// 	pbuf[4] = pbuf[6];

	// 	pbuf[6] = pbuf[2];
	// 	pbuf[2] = pbuf[3];
	// 	pbuf[3] = pbuf[6];

	// 	flash_erase_sector (CFG_ADR_MAC);
	// 	flash_write_page (CFG_ADR_MAC, 8, (unsigned char*)pbuf);
		
	// 	return 0;
	// }

	return 1;
}

extern u8 	slave_link_connected;
static unsigned char atCmd_State(char *pbuf,  int mode, int lenth)
{
	// if(device_in_connection_state ==0)
	// {
	// 	at_print("\r\n+STATE:0");
	// }
	// else
	// {
	// 	at_print("\r\n+STATE:1");
	// }
	return 0;
}


//AT+SEND=46,4646464646546\r\n
static unsigned char atCmd_Send(char *pbuf,  int mode, int lenth)
{
	char * tmp = strstr(pbuf,",");
    if(tmp == NULL)
    {
        return AT_RESULT_CODE_ERROR;
    }

	tmp[0] = 0; tmp++;

	u16 addr_dst = HEX2U16(pbuf);  //获取目的地址

	pbuf = tmp;

	tmp = strstr(pbuf,",");
    if(tmp == NULL)
    {
        return AT_RESULT_CODE_ERROR;
    }
	tmp[0] = 0; tmp++;

	u16 data_len = STR2U16(pbuf); //获取数据长度

	pbuf[0] = 0; //handle 0：module 2 module，handle 1：modul 2 app

	if(addr_dst == 0xfff0) //发送到手机的数据
	{
		if(slave_link_connected) //本模块已经与手机连接
		{
			extern u8 mesh_notify_enc_enable;
			mesh_notify_enc_enable = 0;

			send_data_to_app(tmp, data_len, device_address);

			return 0;
		}
		else
		{
			pbuf[0] = 1; // 委托别的模块发送数据到APP
			addr_dst = 0xFFFF;//将地址设置为广播地址(因为不知道目前是哪个模块与手机连接)
		}
	}

	#if 1
	while(data_len > 0)
	{
		if(data_len > 16)
		{
			at_mesh_tx_cmd(addr_dst, tmp, 16, pbuf[0]);
			data_len -= 16;
			tmp +=16;
		}
		else
		{
			at_mesh_tx_cmd(addr_dst, tmp, data_len, pbuf[0]);
			data_len = 0;
		}
		sleep_us(150 * 1000);
	}
	#else
	static int cmd_sno = 0;
	cmd_sno = clock_time() + device_address;

	tmp -= 3;
	tmp[0] = 0x3f|0xc0;
	tmp[1] = data_len;
	mesh_push_user_command(cmd_sno++, addr_dst, tmp, 13);
	#endif

	if(addr_dst == 0xffff) return 0;
	return 0xff;
}

//用于测试开发板
static unsigned char atCmd_Board_test(char *pbuf,  int mode, int lenth)
{

}

static unsigned char atCmd_MeshName(char *pbuf,  int mode, int lenth)
{
	extern u8 pair_config_mesh_name[17];

	if(mode == AT_CMD_MODE_READ)
	{
		get_mesh_name(pair_config_mesh_name);//读取MeshName
		char buf[64] = {0};
		u_sprintf(buf,"+NETNAME:%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
		pair_config_mesh_name[0],
		pair_config_mesh_name[1],
		pair_config_mesh_name[2],
		pair_config_mesh_name[3],
		pair_config_mesh_name[4],
		pair_config_mesh_name[5],
		pair_config_mesh_name[6],
		pair_config_mesh_name[7],
		pair_config_mesh_name[8],
		pair_config_mesh_name[9],
		pair_config_mesh_name[10],
		pair_config_mesh_name[11],
		pair_config_mesh_name[12],
		pair_config_mesh_name[13],
		pair_config_mesh_name[14],
		pair_config_mesh_name[15]
		);
		
		at_print(buf);
		return 0;
	}
	else if(mode == AT_CMD_MODE_SET)
	{
		memset(pair_config_mesh_name, 0, 17);
		memcpy(pair_config_mesh_name, pbuf, lenth);
		set_mesh_name(pair_config_mesh_name);
		return 0;
	}
}

static unsigned char atCmd_MeshPwd(char *pbuf,  int mode, int lenth)
{
	extern u8 pair_config_mesh_pwd[17];

	if(mode == AT_CMD_MODE_READ)
	{
		//不允许读取MeshPwd
		return 2;
	}
	else if(mode == AT_CMD_MODE_SET)
	{
		memset(pair_config_mesh_pwd, 0, 17);
		memcpy(pair_config_mesh_pwd, pbuf, lenth);
		set_mesh_pwd(pair_config_mesh_pwd);
		return 0;
	}

	return 2;	
}


static unsigned char atCmd_Addr(char *pbuf,  int mode, int lenth)
{
	if(mode == AT_CMD_MODE_SET)
	{
		device_address = HEX2U16(pbuf);
		tinyFlash_Write(2, &device_address, 2); //存储ADDR
		return 0;
	}
	else if(mode == AT_CMD_MODE_READ)
	{
		char buff[32] = { 0 };
		u_sprintf(buff,"+ADDR:%04x",device_address);
		at_print(buff);
		return 0;
	}
	return 2;
}

extern u8 g_ttl;
static unsigned char atCmd_Ttl(char *pbuf,  int mode, int lenth)
{
	if(mode == AT_CMD_MODE_SET)
	{
		u8 ttl =  pbuf[0] - '0';
		if(ttl > 8) return 2;

		g_ttl = ttl;
		tinyFlash_Write(3, &g_ttl, 1); //存储ADDR

		extern void	rf_link_set_max_relay (u8 num);
		rf_link_set_max_relay (g_ttl);

		return 0;
	}
	else if(mode == AT_CMD_MODE_READ)
	{
		char buff[32] = { 0 };
		u_sprintf(buff,"+TTL:%d",g_ttl);
		at_print(buff);
		return 0;
	}
	return 2;
}

static unsigned char atCmd_Adv(char *pbuf,  int mode, int lenth)
{
	extern u8 send_adv_flag;

	if(mode == AT_CMD_MODE_SET)
	{
		send_adv_flag =  pbuf[0] == '0'?0:1;
		tinyFlash_Write(1, &send_adv_flag, 1); //存储ADV标志
		return 0;
	}
	else if(mode == AT_CMD_MODE_READ)
	{
		char buff[32] = { 0 };
		u_sprintf(buff,"+ADV:%d",send_adv_flag);
		at_print(buff);
		return 0;
	}
	return 2;
}

_at_command_t gAtCmdTb_writeRead[] =
{ 
	{ "BAUD", 	atCmd_Baud,	"Set/Read BT Baud\r\n"},
	{ "NAME", 	atCmd_Name,	"Set/Read BT Name\r\n"},
	{ "MAC", 	atCmd_Mac,	"Set/Read BT MAC\r\n"},
	{ "MESHNAME",atCmd_MeshName,"Set/Read BT MAC\r\n"},
	{ "MESHPWD",atCmd_MeshPwd,	"Set/Read BT MAC\r\n"},
	{ "ADDR", 	atCmd_Addr,	"Set/Read BT MAC\r\n"},
	{ "TTL", 	atCmd_Ttl,	"Set/Read Ttl\r\n"},
	{ "ADV", 	atCmd_Adv,	"Set/Read Adv\r\n"},
	{ "SEND", 	atCmd_Send, "Send data to phone\r\n"},
	{0, 	0,	0}
};

_at_command_t gAtCmdTb_exe[] =
{
	{ "1", 		atCmd_ATE1, "ATE1\r\n"},  //ATE1
	{ "0", 		atCmd_ATE0, "ATE0\r\n"},  //ATE0
	{ "GMR", 	atCmd_GMR,  "GMR\r\n"}, 
	{ "RST", 	atCmd_Reset, "RESET\r\n"}, 
	{ "SLEEP", 	atCmd_Sleep, "Sleep\r\n"}, 	
	{ "RESTORE",atCmd_Restore,"RESTORE\r\n"},
	{ "STATE",  atCmd_State,  "State\r\n"},
	{ "BTEST",  atCmd_Board_test,"Board_test\r\n"},
	{0, 	0,	0}
};
