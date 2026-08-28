/******************************************************************************
*                                                                        
*                   Copyright (c) by SoftDEL Systems Ltd.
*
*   This software is copyrighted by and is the sole property of SoftDEL
*   Systems Ltd. All rights, title, ownership, or other interests in the 
*   software remain the property of  SoftDEL Systems Ltd. This software 
*   may only be used in accordance with the corresponding license 
*   agreement. Any unauthorized use, duplication, transmission,
*   distribution, or disclosure of this software is expressly forbidden.
*
*   This Copyright notice may not be removed or modified without prior
*   written consent of SoftDEL Systems Ltd.
*
*   SoftDEL Systems Ltd. reserves the right to modify this software
*   without notice.
*
*   SoftDEL Systems Ltd.                        india@softdel.com
*   3rd Floor, Pentagon P4,                     http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028
*
*   FILE
*
*   AUTHORS
*   Harshal Mangale, M. Venu
*
*   DESCRIPTION
*	This file contains general functions common to all BACnet services.
*
******************************************************************************/

#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

/* header file */
#include "bacDELDef.h"
#include "bacDELApi.h"
#include "pduDataType.h"
#include "bacnetStackMgmt.h"

/* macros */
#define TAG_ERROR_NOTIFY 0

#define I_AM_BUFF_SIZE  30
/* Application Function Handlers */
extern HardWare_Interface_t afpApplicationFunction[MAX_HW_APP_FUNC];

/** An Array of Application callback functions */
extern App_AutoResp_Interface_t afpApplicationFunctionCb[MAX_APP_CALLBACK_FUN];

/** An array of application internal callback response functions */
extern App_Interface_t fpAppLayerCbFunction;
/* structure to set various max limits of stack */
extern BACApp_MaxLimits_t g_stStackMaxLimits;
#ifdef BBMD_ENABLED
extern uint8_t gu8IamMPDU[I_AM_BUFF_SIZE];
#endif /* BBMD Enable*/
/*****************************************************************************/

#if (defined(BACDEL_SER_DM_DDB_A) || defined(BACDEL_SER_DM_DDB_B) || \
    defined(BACDEL_SER_DM_DOB_A) || defined(BACDEL_SER_DM_DOB_B))

/* To include DDB-B service */
#if (defined(BACDEL_SER_DM_DOB_A) || defined(BACDEL_SER_DM_DOB_B))

/*
 *Name - obj_add_binding_t
 *
 *DESCRIPTION - Stores the I-Have responses received.
 */
typedef struct OBJECT_ADDRESS_BINDING
{
    /** Structure to store DOB data */
    dob_i_have_t m_stDOBData;
    /** To accomodate duplicate values */
    int8_t i8NumOfDuplicates;
    /** Points to the next location to store DOB data */
    struct OBJECT_ADDRESS_BINDING *m_pstNext;
}obj_add_binding_t;

#endif /* BACDEL_SER_DM_DOB_B  || BACDEL_SER_DM_DOB_A */
/*****************************************************************************/

/*****************************************************************************/

/* To store decoded I-AM-Router-To-Network data */
typedef struct iAmRouterToNetwork
{
    /* To store Network No. */
    uint16_t			m_u16NetworkNo;
    /*Ip Address of Router */
	BACnetAddress_t	m_stAddress;
	/** Next node in list */
	struct iAmRouterToNetwork *pstNext;
}IamRouterToNwBindData_t;

/*****************************************************************************/

/*                                                                       
 * Check the received device instance range with the Device Instance
 */
BACNET_RETURN_TYPE Check_Device_Address_Range(
	ddb_who_is_t *pstDevAddrRange, 
	uint32_t u32DevID);

#endif /* (defined(BACDEL_SER_DM_DDB_A) || defined(BACDEL_SER_DM_DDB_B) || \
    defined(BACDEL_SER_DM_DOB_A) || defined(BACDEL_SER_DM_DOB_B)) */

/*****************************************************************************/

/*
 * Allocate dynamic memory & copies data from Process queue. The size of 
 * memory to be allocate depends on Service choice.
 */
BACNET_RETURN_TYPE Copy_Process_Queue_Data(processInfo_t *pstProcQInfo,
                                         processInfo_t *pstSrcProcQInfo);

/*****************************************************************************/

/**
*
* DESCRIPTION
* Function converts the service choice type to service supported type.
*
* @param u8ServiceChoice [in] confirmed or un-confirmed service type.
* @param ePduType		 [in] pdu type.
*
* @return [out] BACNET_SERVICES_SUPPORTED enum value.
*
*/
BACNET_SERVICES_SUPPORTED Map_With_Services_Supported(
	uint8_t u8ServiceChoice, 
	BACNET_PDU_TYPE ePduType);

int32_t Decode_ObjId_Prop_ArrIdx( uint8_t * pu8ReqBuffer, int16_t i16ReqLen,
                                BACNET_CONF_DATA * pstReqData);

int32_t Encode_ObjId_Prop_ArrIdx(
                uint8_t * pu8EncodeBuffer,
                BACNET_CONF_DATA * pstEncodeParam);

void Free_Service_Memory(Bacnet_Multiple_Data_t *pstMPSData);

void Clear_Process_Info(processInfo_t* pstProcessQData);

void Exit_Routine_To_Error_Of_Unconfirmed_Request(processInfo_t *pstProcQIndex);


void Exit_Routine_To_Error_Of_Confirmed_Request(processInfo_t *pstProcQIndex,
                                                int32_t i32PDUType, 
                                                BACNET_ERROR_CLASS eErrorClass,
                                                BACNET_ERROR_CODE eErrorCode);

int32_t Encode_MPS_ObjId_OTag(
                uint8_t * pu8APDUResp,
                Bacnet_Multiple_Data_t * pstRpmData);

int32_t Encode_MPS_property_ArrIdx(
                uint8_t * pu8EncodeBuffer,
                BACNET_PROPERTY_ID eObjectProperty,
                uint32_t u32ArrayIndex,
                bool bArrIndxPresent);

/* decode the object portion of the service request only */
int32_t Decode_MPS_object_id(
                uint8_t * u8APDUReq,
                uint32_t u32APDULen,
                Bacnet_Multiple_Data_t * pstRpmData);

/* decode the object property portion of the service request only */
int32_t Decode_MPS_property_ArrIdx(
                uint8_t * u8APDUReq,
                uint32_t u32APDULen,
                Bacnet_Multiple_Data_t * pstRpmData);

/**  This function is called whenever local date & local time properties 
	of device object are read. This function sets date & time properties 
	from calculated date & time. */
void Update_Dv_Local_DateTime_Properties(void *pvVirtualDev,
										 BACnetTime_t *pstTime,
										 BACnetDate_t *pstDate,
										 bool bFlag);

#ifdef SUPPORT_MULTIPLE_DEVICE
/** function copies device address from virtual device to process info. */
void Copy_Device_Address(void *pvProcQInfo, void *pvVirtualDev);
#endif

/** This function is used to free b-side service data */
void Clear_Bside_ServiceData(BACNET_SERVICES_SUPPORTED eServiceSupport, 
	void **ppvServiceData);

/** This function converts service type enum to auto-response callback type enum */
BACAPP_CALLBACK_FUN_CHOICE Convert_ServiceType_To_CallbackType(
	BACNET_SERVICES_SUPPORTED eServiceSupport);

/**
*
* DESCRIPTION
*   function to validate the supported character set encoding.
*
*/
BACNET_RETURN_TYPE Validate_Charstring_Encoding(BACNET_CHARACTER_STRING_ENCODING eEncoding,
		uint16_t u16CodePage);

/*****************************************************************************/
/** structure for decoding list & array type properties */
typedef struct {
	BACNET_OBJECT_TYPE      m_eObjectType;
	/** Object Instance*/     
    uint32_t                m_u32ObjId; 
    /* Prop id */
    BACNET_PROPERTY_ID      m_ePropId;
	/* Property data type */
	BACNET_DATA_TYPE	  	m_eDataType;
    /* property array index */ 
    uint32_t                m_u32ArrayIndex;
    /* indicates if array index is present or absent */
    bool                    m_bArrIndxPresent;
	/* Length of buffer */
	int32_t             	m_i32ApplicationDataLen;
	/* Pointer to Encoded Property Value */
	uint8_t             	*m_pu8PropValueBuffer;
    /* list element */
    void                    *pvListElement;
}ListManipulation_t;

/*****************************************************************************/
/**
*
* DESCRIPTION
* Function to genrates callback-id for B-side callbacks to be 
* sent to application.
*
* @returns valid or invalid callback id no.
* Note: 
* 0 = invalid callback id
* 1 to INT32_MAX = valid callback id
* value > INT32_MAX = special values (unused values for now)
*
*/
uint32_t Generate_Callback_ID(void);

/** Api to free memory allocated to save COV notification data */
void Free_COV_Notification_Data(void *pvCovData);

/**
*                                                                    
* DESCRIPTION:
* Function is used to assign service error/abort/reject to Apdu.
*
* @param i32PDUType     [in]  PDU type of response
* @param eErrorClass	[in]  Error class for error response
* @param eErrorCode		[in]  Error code, Abort reason, Reject reason
* @param bServer		[in]  Abort from server or client
* @param pstProcQInfo  [out]  Pointer to request parameter structure
*
*/
void Service_Error_Handler(
    int32_t i32PDUType,
    BACNET_ERROR_CLASS  eErrorClass,
    BACNET_ERROR_CODE   eErrorCode,
    processInfo_t *pstProcQInfo,
    bool bServer);

/**
*
* DESCRIPTION  
* This function set error code as per pdu type.
*
* @param pstProcQInfo  [in/out] process Q data.
* @param ePDUType      [in]		pdu type
* @param eErrorClass   [in]		error class
* @param u32ErrorCode  [in]		error code
* @return void
*					  
*/
void Set_Error_Parameters_Of_Confirmed_Req(
	processInfo_t *pstProcQInfo,
	BACNET_PDU_TYPE ePDUType,
	BACNET_ERROR_CLASS eErrorClass,
	uint32_t u32ErrorCode);

#ifdef BACDEL_SER_DM_DDB_B
/**
*
* DECRIPTION
* Function sends an I-AM request or I-Am-Router-To-Network.
*
* @param pvVirtualDev	 [in] Virtual device data
* @param bIsNwLayerMsg   [in] flag to send i-am-router-to-network
* @param eDestType		 [in] Destination type
*
* @return void
*
*/
void Send_I_AM(void *pvVirtualDev,
	bool bIsNwLayerMsg, 
	DESTINATION_TYPE eDestType);
#endif /* DDB-B */

#ifdef BACDEL_SER_DM_DDB_A
/**
*
* DECRIPTION
* Function sends an Who-Is request or Who-Is-Router-To-Network.
*
* @param pvVirtualDev			[in] Virtual device data
* @param bIsNwLayerMsg			[in] flag to send i-am-router-to-network
* @param eDestType				[in] Destination type
* @param i32DevRangeLowLimit	[in] Low limit for who-is
* @param i32DevRangeHighLimit	[in] High limit for who-is
* @param u32NetworkNo			[in] Network number for who-is router
*
* @return [ou] success or error
*
*/
BACNET_RETURN_TYPE Send_WHO_IS(void *pvVirtualDev, 
	int32_t i32DevRangeLowLimit, int32_t i32DevRangeHighLimit,
	bool bIsNwLayerMsg, uint32_t u32NetworkNo, 
    DESTINATION_TYPE eDestType);
#endif /* DDB-A */

#ifdef BBMD_ENABLED

/**
*
* DECRIPTION
* Function update a Forwarded I-AM request buffer.
*
* @param pu8IamBuffer  			[in] Pointer of buffer
* @param u8DataLength			[in] Number of data bytes
*
* @return [none]
*
*/
void UpdateForwardedI_AM_Data(uint8_t *pu8IamBuffer,
		uint8_t u8DataLength);
#endif /* BBMD_ENABLED */

#if (defined BACDEL_SER_AE_GEI_A || defined BACDEL_SER_AE_GEI_B)
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will free memory allocated to save GEI ACK data.
*    
* @param pstASUMData [in]   GEI Ack data.
* @return			 [out]	void / returns nothing	
*                                                                      
*/
void GEI_Free_Ack_Data(event_info_response_t *pstEventInfoData);
#endif /* GEI_A || GEI_B */
#endif /* SERVICE_HANDLER_H */
