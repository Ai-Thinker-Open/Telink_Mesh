/********************************************************************************************************
 * @file     user_config.h 
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

#pragma once

#include "chip_type_project.h"

#if (__PROJECT_BLE_MASTER__)
	#include "../ble_master/app_config.h"
#elif (__PROJECT_BEACON_DETECTOR__)
	#include "../ble_dongle_beacon/app_config.h"
#elif (__PROJECT_LIGHT_8266__)
	#include "../light_8266/light.h"
#elif (__PROJECT_LIGHT_8267__)
	#include "../light_8267/light.h"
#elif (__PROJECT_LIGHT_8269__)
	#include "../light_8269/light.h"
#elif (__PROJECT_LIGHT_8258__)
	#include "light.h"
#elif (__PROJECT_LIGHT_8267_UART__)
	#include "../light_8267_uart/light.h"
#elif (__PROJECT_LIGHT_GATEWAY__)
    #if (__PROJECT_CHIP_TYPE_SEL__ == PROJECT_CHIP_8266)
    #include "../light_gateway/light_gateway_8266.h"
    #elif (__PROJECT_CHIP_TYPE_SEL__ == PROJECT_CHIP_8267)
	#include "../light_gateway/light_gateway_8267.h"
    #elif (__PROJECT_CHIP_TYPE_SEL__ == PROJECT_CHIP_8258)
	#include "../light_gateway/light_gateway_8258.h"
	#endif
#elif (__PROJECT_LIGHT_NO_MESH__)
	#include "../light_no_mesh/light.h"
#elif (__PROJECT_MASTER_LIGHT_8266__)
	#include "../master_light/light_8266.h"
#elif (__PROJECT_MASTER_LIGHT_8267__)
	#include "../master_light/light_8267.h"
#elif (__PROJECT_8266_MESH_CONFIG__)
	#include "../8266_mesh_config/app_config.h"
#elif (__PROJECT_8263_SWITCH__)
	#include "../8263_switch/app_config.h"
#elif (__PROJECT_8368_SWITCH__)
	#include "../8368_switch/app_config.h"
#elif (__PROJECT_MOTIONSENSOR_8267__)
	#include "../motionsensor_8267/app_config.h"
#elif (__PROJECT_LIGHT_SWITCH__)
	#include "../light_switch/light_switch.h"
#elif (__PROJECT_LPN__)
	#include "../light_lpn/light_lpn.h"
#elif (__PROJECT_OTA_BOOT__)
	#include "../ota_boot/app_config.h"
#elif (__PROJECT_OTA_MASTER__)
	#include "../ota_master/app_config.h"
#elif (__PROJECT_MONITOR_8266__)
	#include "../monitor_8266/app_config.h"
#elif (__PROJECT_EMI__)
	#include "../emi/main_emi.h"
#else
	#include "user_config_common.h"
#endif

#define	FLASH_SECTOR_SIZE       (4096)

#define	FLASH_ADDRESS_DEFINE						\
		u32 flash_adr_mac;							\
		u32 flash_adr_pairing;						\
		u32 flash_adr_dev_grp_adr;			        \
		u32 flash_adr_lum;							\
		u32 flash_adr_ota_master;					\
		u32 flash_adr_reset_cnt;					\
		u32 flash_adr_alarm;					    \
		u32 flash_adr_scene;					    \
		u32 flash_adr_user_data;					    \
		u32 flash_adr_light_new_fw;
#define FLASH_ADDRESS_CONFIG								\
		flash_adr_mac 			= FLASH_ADR_MAC;			\
		flash_adr_pairing 		= FLASH_ADR_PAIRING;		\
		flash_adr_dev_grp_adr   = FLASH_ADR_DEV_GRP_ADR;    \
		flash_adr_lum 			= FLASH_ADR_LUM;			\
		flash_adr_ota_master 	= FLASH_ADR_OTA_NEW_FW;     \
		flash_adr_reset_cnt 	= FLASH_ADR_RESET_CNT;      \
		flash_adr_alarm 	    = FLASH_ADR_ALARM;          \
		flash_adr_scene 	    = FLASH_ADR_SCENE;          \
		flash_adr_user_data   = FLASH_ADR_USER_DATA;          \
        flash_adr_light_new_fw  = FLASH_ADR_LIGHT_NEW_FW;          
#define FLASH_ADDRESS_EXTERN						\
		extern u32 flash_adr_mac;					\
		extern u32 flash_adr_pairing;				\
		extern u32 flash_adr_dev_grp_adr;		    \
		extern u32 flash_adr_lum;					\
		extern u32 flash_adr_ota_master;            \
		extern u32 flash_adr_reset_cnt;             \
		extern u32 flash_adr_alarm;                 \
		extern u32 flash_adr_scene;                 \
		extern u32 flash_adr_user_data;                 \
		extern u32 flash_adr_light_new_fw;

#if 1//(__PROJECT_LIGHT_8267__ || __PROJECT_LIGHT_8269__)
enum{
	TYPE_TLK_MESH		    = 0x000000A3,// don't change, must same with telink mesh SDK
	TYPE_SIG_MESH		    = 0x0000003A,// don't change, must same with telink mesh SDK
	TYPE_TLK_BLE_SDK	    = 0x000000C3,// don't change, must same with telink mesh SDK
	TYPE_TLK_ZIGBEE 	    = 0x0000003C,// don't change, must same with telink mesh SDK
    TYPE_DUAL_MODE_STANDBY  = 0x00000065,// dual mode state was standby to be selected
	TYPE_DUAL_MODE_RECOVER 	= 0x00000056,// don't change, must same with zigbee SDK, recover from zigbee.
};
// 0xA3: Telink mesh    0x3A: SIG mesh
#define			FLASH_ADR_MESH_TYPE_FLAG	0x73000	// don't change, must same with SIG mesh SDK
#endif

#define IBEACON_ENABLE                      0
#if(IBEACON_ENABLE)
// unit: 100ms
#define 		IBEACON_INTERVAL         	5
#define			FLASH_ADR_BEACON_MAJOR		0x75000
#define			FLASH_ADR_BEACON_MINOR		0x75010
#define			FLASH_ADR_BEACON_UUID		0x75020
#define			FLASH_ADR_BEACON_COMPANYID	0x75030
#endif
#define			FLASH_ADR_MAC				0x76000
#define			FLASH_ADR_DC				0x76010
#define			FLASH_ADR_TP_LOW		    0x76011
#define			FLASH_ADR_TP_HIGH		    0x76012
#define			CFG_ADR_DUAL_MODE_EN		0x76080

#define			FLASH_ADR_PAIRING			0x77000
#define			FLASH_ADR_LUM				0x78000
#define			FLASH_ADR_DEV_GRP_ADR	    0x79000
#define			FLASH_ADR_RESET_CNT			0x7A000
#define			FLASH_ADR_ALARM			    0x7B000
#define			FLASH_ADR_SCENE			    0x7C000
#define			FLASH_ADR_USER_DATA		    0x7D000
//////vendor define here from 0x7D000 to 0x7FFFF...
//////vendor define here ...
#define			FLASH_ADR_PAR_MAX			0x80000

// begin with 0x1F000, resv address, don't change
#define			FLASH_ADR_OTA_BOOT			0x1F000
#define			FLASH_ADR_OTA_NEW_FW		0x20000
#define			FLASH_ADR_OTA_READY_FLAG	0x3F000		
#define         FLASH_ADR_LIGHT_NEW_FW      0x40000
#define         ERASE_SECTORS_FOR_OTA       32

#define 		GATEWAY_OTA_OTHER_FW_ADR	(0x21000)	// shall not use 0x20000 for 8267

// note !!!DEVICE_NAME max 13 bytes  
#define DEVICE_NAME 'T', 'e', 'l', 'i', 'n', 'k', ' ', 't', 'L', 'i', 'g' ,'h', 't'

#ifndef ADV_UUID
#define ADV_UUID                            0
#endif
#ifndef PA_ENABLE
#define PA_ENABLE                           0
#endif

#if(PA_ENABLE)
#ifndef PA_TXEN_PIN
#define PA_TXEN_PIN                         GPIO_PD4	// same with GPIO_GP10
#endif

#ifndef PA_RXEN_PIN
#define PA_RXEN_PIN                         GPIO_PD5	// same with GPIO_GP11
#endif
#endif

#ifndef PASSIVE_EN
#define PASSIVE_EN                          0
#endif

#ifndef BQB_EN
#define BQB_EN                          	0
#endif

#ifndef PM_DEEPSLEEP_RETENTION_ENABLE
#define PM_DEEPSLEEP_RETENTION_ENABLE       0
#endif

#if PROVISIONING_ENABLE
#define MESH_PAIR_ENABLE                    1	// must 1
#else
#define MESH_PAIR_ENABLE                    0	// user can set 0 or 1
#endif

// if MESH_PAIR_ENABLE is 1, SW_NO_PAIR_ENABLE must be 0.
#if MESH_PAIR_ENABLE
#define SW_NO_PAIR_ENABLE					0	// must 0
#else
#define SW_NO_PAIR_ENABLE					0
#endif

enum{
    PA_TX   = 0,
    PA_RX   = 1,
    PA_OFF  = 2,
};

#if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#define			ANA_REG_DC							0x8A
#else
#define			ANA_REG_DC							0x81
#endif

/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////

#ifndef HCI_ACCESS          // for gateway
#define HCI_ACCESS		HCI_USE_USB
#endif

#define PAIR_VALID_FLAG						0xFA
//u8 advData[] = {0x02, 0x01, 0x05};
//0~max_mesh_name_len bytes  (strlen(advData) + strlen(MESH_NAME) + sizeof(ll_adv_private_t))<=31
#define MESH_NAME							"at_mesh"
//max 16 bytes
#define MESH_PWD							"123456"
//max 16 bytes
#define MESH_PWD_ENCODE_SK					"0123456789ABCDEF"
//max 16 bytes, random data from master
#define MESH_LTK							0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf

#define OUT_OF_MESH						    "out_of_mesh"

#include "../../version.h"

