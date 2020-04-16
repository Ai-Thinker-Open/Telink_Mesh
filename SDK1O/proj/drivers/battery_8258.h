/********************************************************************************************************
 * @file     battery_8258.h 
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

#if(MCU_CORE_TYPE == MCU_CORE_8258)

#define	LOW_BATT_FLG					0x5A

#define VBAT_ALRAM_THRES_MV				2000   // 2000 mV low battery alarm



void battery_set_detect_enable (int en);
int  battery_get_detect_enable (void);

int app_battery_power_check(u16 alram_vol_mv);

extern u32 lowBattDet_tick;

#endif /* APP_BATTDET_H_ */
