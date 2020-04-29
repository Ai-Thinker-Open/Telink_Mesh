/********************************************************************************************************
 * @file     app_att.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2015
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
#include "proj/tl_common.h"

///////////////////////////  ATT  protocol  ///////////////////////////
#include "proj_lib/ble_ll/ble_common.h"
#include "proj_lib/ble_ll/att.h"
#include "proj_lib/ble_ll/gatt_uuid.h"
#include "proj_lib/ble_ll/service.h"
#include "proj_lib/ble_ll/gatt_server.h"
#include "proj_lib/ble_ll/gap_const.h"
#include "proj_lib/light_ll/light_frame.h"
#include "at_cmd.h"

#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

#define GATT_UUID_APPEARANCE 0x2a01

#define GATT_UUID_PERI_CONN_PARAM 0x2a04

#define GATT_UUID_SERVICE_CHANGE 0x2a05

#define MAX_DEV_NAME_LEN 18

#define ATT_PERMISSIONS_RDWR 03

///////////////////////////////////// ATT  HANDLER define ///////////////////////////////////////
typedef enum
{
	ATT_H_START = 0,
	//// Gap ////
	/**********************************************************************************************/
	GenericAccess_PS_H, 					//UUID: 2800, 	VALUE: uuid 1800
	GenericAccess_DeviceName_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	GenericAccess_DeviceName_DP_H,			//UUID: 2A00,   VALUE: device name
	GenericAccess_Appearance_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	GenericAccess_Appearance_DP_H,			//UUID: 2A01,	VALUE: appearance
	CONN_PARAM_CD_H,						//UUID: 2803, 	VALUE:  			Prop: Read
	CONN_PARAM_DP_H,						//UUID: 2A04,   VALUE: connParameter


	//// gatt ////
	/**********************************************************************************************/
	GenericAttribute_PS_H,					//UUID: 2800, 	VALUE: uuid 1801
	GenericAttribute_ServiceChanged_CD_H,	//UUID: 2803, 	VALUE:  			Prop: Indicate
	GenericAttribute_ServiceChanged_DP_H,   //UUID:	2A05,	VALUE: service change
	GenericAttribute_ServiceChanged_CCB_H,	//UUID: 2902,	VALUE: serviceChangeCCC


	//// device information ////
	/**********************************************************************************************/
	DeviceInformation_PS_H,					 //UUID: 2800, 	VALUE: uuid 180A
	DeviceInformation_pnpID_CD_H,			 //UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_pnpID_DP_H,			 //UUID: 2A50,	VALUE: PnPtrs

	//// SPP ////
	/**********************************************************************************************/
	SPP_PS_H, 							 //UUID: 2800, 	VALUE: telink spp service uuid

	//server to client
	SPP_SERVER_TO_CLIENT_CD_H,		     //UUID: 2803, 	VALUE:  			Prop: read | Notify
	SPP_SERVER_TO_CLIENT_DP_H,			 //UUID: telink spp s2c uuid,  VALUE: SppDataServer2ClientData
	SPP_SERVER_TO_CLIENT_CCB_H,			 //UUID: 2902, 	VALUE: SppDataServer2ClientDataCCC
	SPP_SERVER_TO_CLIENT_DESC_H,		 //UUID: 2901, 	VALUE: TelinkSPPS2CDescriptor

	ATT_END_H,

}ATT_HANDLE;

typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

static const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

static const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

static const u16 reportRefUUID = GATT_UUID_REPORT_REF;

static const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

static const u16 userdesc_UUID	= GATT_UUID_CHAR_USER_DESC;

static const u16 serviceChangeUUID = 0x2a05;

static const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

static const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

static const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

static const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

static const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;

static const u16 my_appearanceUIID = GATT_UUID_APPEARANCE;

static const u16 my_periConnParamUUID = GATT_UUID_PERI_CONN_PARAM;

static const u16 my_appearance = GAP_APPEARE_UNKNOWN;

static const gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

static const u16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;

static u16 serviceChangeVal[2] = {0};

static u8 serviceChangeCCC[2] = {0,0};

#define MY_DEV_NAME                        "Ai-Thinker"
extern  u8 ble_devName[];

static const u8 my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};



//////////////////////// OTA  ////////////////////////////////////////////////////
static const  u8 my_OtaUUID[16]					    = TELINK_SPP_DATA_OTA;
//static const  u8 my_OtaServiceUUID[16]				= TELINK_OTA_UUID_SERVICE;
static u8 my_OtaData 						        = 0x00;
static const  u8 my_OtaName[] 						= {'O', 'T', 'A'};

////////////////////// SPP ////////////////////////////////////
static const u8 TelinkSppServiceUUID[16]	      	    = TELINK_SPP_UUID_SERVICE;
static const u8 TelinkSppDataServer2ClientUUID[16]      = TELINK_SPP_DATA_SERVER2CLIENT;
static const u8 TelinkSppDataClient2ServerUUID[16]      = TELINK_SPP_DATA_CLIENT2SERVER;

static const u8 TelinkSppData222[16] = {0x15,0x19,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00} ;

static const u8 my_SppDataProp		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY;
static const u8 m_SppDataProp		= CHAR_PROP_INDICATE;

// Spp data from Server to Client characteristic variables
static u8 SppDataServer2ClientDataCCC[2]  				= {0};

//this array will not used for sending data(directly calling HandleValueNotify API), so cut array length from 20 to 1, saving some SRAM

static u8 SppData_1[1] 					= {0};  //SppDataServer2ClientData[20]
// Spp data from Client to Server characteristic variables
//this array will not used for receiving data(data processed by Attribute Write CallBack function), so cut array length from 20 to 1, saving some SRAM
static u8 SppData_2[1] 					= {0};  //SppDataClient2ServerData[20]


//SPP data descriptor
static const u8 Telink_Descriptor_1[] 		 		= "Ai-Thinker SPP: Module<->Phone";

//// GAP attribute values
static const u8 my_devNameCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const u8 my_appearanceCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
	U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
static const u8 my_periConnParamCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
	U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};


//// GATT attribute values
static const u8 my_serviceChangeCharVal[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};


//// device Information  attribute values
static const u8 my_PnCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_pnpID_DP_H), U16_HI(DeviceInformation_pnpID_DP_H),
	U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
};


//// Telink spp  attribute values
static const u8 TelinkSppData_1[19] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY,
	U16_LO(SPP_SERVER_TO_CLIENT_DP_H), 
	U16_HI(SPP_SERVER_TO_CLIENT_DP_H), 
	TELINK_SPP_DATA_SERVER2CLIENT
};

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 handle;
	u8 handle1;
	u8 value;
}rf_packet_att_write_t;

char buff[64] = {0};
int module_onReceiveData(rf_packet_att_write_t *p)
{
	u8 len = p->l2capLen - 3;
	u_sprintf(buff, "\r\n+DATA:%d,", len);
	at_print(buff);

	at_send((char*)&p->value, len);

	at_print("\r\n");

	//bls_att_pushNotifyData(SPP_SERVER_TO_CLIENT_DP_H, (u8*)&p->value, len); //release

	light_notify((u8*)&p->value, len, (u8*)&p->value);

	return 0;
}


u8  ble_devName[] = {'M', 'e', 's', 'h', ' ', 'n', 'a', 'm', 'e'};

u8 mr_SppDataPairData[20] = { 0 };

static const u16 my_SppServiceUUID = 0x7890;
static const u16 my_SppServiceUUID1 = 0x7891;
static const u16 my_SppServiceUUID2 = 0x7892;

attribute_t my_Attributes[] = {

	{13, 0,0,0,0,0},	// total num of attribute

	//0001 - 0007  gap
	{7,2,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,2,sizeof(my_devNameCharVal),sizeof(my_devNameCharVal),(u8*)(&my_characterUUID), (u8*)(my_devNameCharVal), 0},
	{0,2,MAX_DEV_NAME_LEN,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(ble_devName), 0},
	{0,2,sizeof(my_appearanceCharVal),sizeof(my_appearanceCharVal),(u8*)(&my_characterUUID), (u8*)(my_appearanceCharVal), 0},
	{0,2,sizeof (my_appearance),sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,2,sizeof(my_periConnParamCharVal),sizeof(my_periConnParamCharVal),(u8*)(&my_characterUUID), (u8*)(my_periConnParamCharVal), 0},
	{0,2,sizeof (my_periConnParameters),sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},


	//spp
	{5,2,16,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(TelinkSppServiceUUID), 0},

	{0,2,sizeof(TelinkSppData_1),sizeof(TelinkSppData_1),(u8*)(&my_characterUUID), 		(u8*)(&TelinkSppData_1), 0},
	{0,16,sizeof (serviceChangeVal), sizeof (serviceChangeVal),(u8*)(TelinkSppDataServer2ClientUUID), 	(u8*)(&serviceChangeVal), &module_onReceiveData, 0},
	{0,2,2,2,(u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC), 0},
	{0,2,sizeof (Telink_Descriptor_1), sizeof (Telink_Descriptor_1),(u8*)(&userdesc_UUID), (u8*)(Telink_Descriptor_1), 0},

};

void my_att_init (void)
{
	bls_att_setAttributeTable ((u8 *)my_Attributes);

	u8 device_name[] = MY_DEV_NAME;
	bls_att_setDeviceName(device_name, sizeof(MY_DEV_NAME));
}