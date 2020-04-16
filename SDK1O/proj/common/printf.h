/********************************************************************************************************
 * @file     printf.h 
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

#ifndef MYPRINTF_H
#define MYPRINTF_H

//////////////////// PRINT DEBUG INFO ///////////////////////
#if (__TL_LIB_8263__ || MCU_CORE_TYPE == MCU_CORE_8263 || __TL_LIB_8368__ || MCU_CORE_TYPE == MCU_CORE_8368)
#define PRINT_DEBUG_INFO                    0
#else
#define PRINT_DEBUG_INFO                    1
#endif

#if PRINT_DEBUG_INFO

#define PRINT_BAUD_RATE             		1000000
#define DEBUG_INFO_TX_PIN           		GPIO_PA0
//#define PULL_WAKEUP_SRC_PA0         		PM_PIN_PULLUP_1M

#endif
/////////////////////////////////////////////////////////////



#if (PRINT_DEBUG_INFO)

void my_array_printf(char*data, int len);
int mini_printf(const char *format, ...);
void PrintHex(u8 x);

#define printf			mini_printf
#define	printfArray		arrayPrint
//#define	logPrint		mini_printf
//#define	logPrintArray	arrayPrint
#define	arrayPrint(arrayAddr,len)					\
{													\
	mini_printf("\n*********************************\n");		\
	unsigned char	i = 0;							\
	do{												\
		mini_printf(" %x",((unsigned char *)arrayAddr)[i++]);						\
	}while(i<len);										\
	mini_printf("\n*********************************\n");		\
}
#else
#define printf
#define	printfArray
#define	PrintHex
#endif

//#define debugBuffer (*(volatile unsigned char (*)[40])(0x8095d8))
#endif

