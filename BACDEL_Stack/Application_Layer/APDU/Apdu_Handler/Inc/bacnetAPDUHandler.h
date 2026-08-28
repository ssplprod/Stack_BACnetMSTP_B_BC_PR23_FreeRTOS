/**********************************************************************************
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
*   SoftDEL Systems Ltd.						india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*	bacnetAPDUHandler.h
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of APDU layer header.
*
**********************************************************************************/

#ifndef APDU_H
#define APDU_H

/** header files */
#include "bacDELDef.h"
#include "bacnetStackMgmt.h"
//#include "pduServiceStructure.h"

/** compile as C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* generic unconfirmed function handler */
/* Suitable to handle the following services: */
/* I_Am, Who_Is, Unconfirmed_COV_Notification, I_Have, */
/* Unconfirmed_Event_Notification, Unconfirmed_Private_Transfer, */
/* Unconfirmed_Text_Message, Time_Synchronization, Who_Has, */
/* UTC_Time_Synchronization */
typedef BACNET_RETURN_TYPE (*Unconfirmed_Function_t) (
		processInfo_t *pstProcQInfo,
		uint8_t *pu8ServiceRequest,
		int32_t i32ServiceLen);

/* generic confirmed function handler */
/* Suitable to handle the following services: */
/* Acknowledge_Alarm, Confirmed_COV_Notification, */
/* Confirmed_Event_Notification, Get_Alarm_Summary, */
/* Get_Enrollment_Summary_Handler, Get_Event_Information, */
/* Subscribe_COV_Handler, Subscribe_COV_Property, */
/* Life_Safety_Operation, Atomic_Read_File, */
/* Confirmed_Atomic_Write_File, Add_List_Element, */
/* Remove_List_Element, Create_Object_Handler, */
/* Delete_Object_Handler, Read_Property, */
/* Read_Property_Conditional, Read_Property_Multiple, Read_Range, */
/* Write_Property, Write_Property_Multiple, */
/* Device_Communication_Control, Confirmed_Private_Transfer, */
/* Confirmed_Text_Message, Reinitialize_Device, */
/* VT_Open, VT_Close, VT_Data_Handler, */
/* Authenticate, Request_Key */
typedef BACNET_RETURN_TYPE (*Confirmed_Function_t) (
		processInfo_t *pstProcQInfo,
		uint8_t *pu8ServiceRequest,
		int32_t i32ServiceLen);

/* generic confirmed simple ack function handler */
typedef BACNET_RETURN_TYPE (*Confirmed_Simple_ACK_Function_t) (
        BACnetAddress_t * pstSrcAddr,
        uint8_t u8InvokeId);

/* generic confirmed ack function handler */
typedef BACNET_RETURN_TYPE (*Confirmed_ACK_Function_t) (
        processInfo_t *pstProcQInfo,
		uint8_t *pu8ServiceRequest,
		int32_t i32ServiceLen );

/* generic error reply function */
typedef BACNET_RETURN_TYPE (*Error_Function_t) (
        processInfo_t *pstProcQInfo,
		uint8_t *pu8ServiceRequest,
		int32_t i32ServiceLen );

/* generic abort reply function */
typedef BACNET_RETURN_TYPE (
        *Abort_Function_t) (
        BACnetAddress_t * pstSrcAddr,
        uint8_t u8InvokeId,
        uint8_t u8AbortReason,
        bool server);

/* generic reject reply function */
typedef BACNET_RETURN_TYPE (
        *Reject_Function_t) (
        BACnetAddress_t * pstSrcAddr,
        uint8_t u8InvokeId,
        uint8_t u8RejectReason);

/**
*                                                                    
* DESCRIPTION:
* Function to decode the APDU header.
* This function will call appropriate service handler after decoding APDU header.
* Decoded data will be filled in process queue node.
*
* @param pu8APDUReq		[in]      APDU data to be decoded
* @param u16APDULen		[in]      APDU data length
* @param pstProcQInfo	[in/out]  Pointer to process queue node
*
* @return  [out]  BACNET_RETURN_TYPE enumerations
* This function is called from process (Px) thread:
* - BACDEL_SUCCESS will trigger Tx thread to send response
* - BACDEL_CONTINUE will do nothing
* - BACDEL_ERROR or any other value will free Px-Q node
*
*/
BACNET_RETURN_TYPE APDU_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8APDUReq,
    uint16_t u16APDULen);

/**
*                                                                    
* DESCRIPTION:
* Function to encode the APDU data, header plus variable data.
* This function will call appropriate service encode handler after encoding APDU header.
*
* @param pstProcQInfo	[in]  Pointer to process queue node
* @return  [out]  BACNET_RETURN_TYPE enumerations
* BACDEL_SUCCESS on success else any other error code
*
*/
BACNET_RETURN_TYPE APDU_Encode_Handler(processInfo_t *pstProcQInfo);

/** 
*
* DESCRIPTION
* Function to set complex ACK handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Confirmed_Ack_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Confirmed_ACK_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set simple ACK handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Confirmed_Simple_Ack_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Confirmed_Simple_ACK_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set un-recognized service request handler
*
* @param fpFunction  [in]  Function pointer for handler
* @return [out] void
*
*/
void APDU_Set_Unrecognized_Service_Handler(
    Confirmed_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set confirmed service request handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Confirmed_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Confirmed_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set un-confirmed service request handler
*
* @param eServiceChoice  [in]  Un-confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Unconfirmed_Handler(
    BACNET_UNCONFIRMED_SERVICE eServiceChoice,
    Unconfirmed_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set error ACK handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Error_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Error_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set abort ACK handler
*
* @param fpFunction [in]  Function pointer for handler
* @return [out] void
*
*/
void APDU_Set_Abort_Handler(
    Abort_Function_t fpFunction);

/** 
*
* DESCRIPTION
* Function to set reject ACK handler
*
* @param fpFunction [in]  Function pointer for handler
* @return [out] void
*
*/
void APDU_Set_Reject_Handler(
    Reject_Function_t fpFunction);
	
/**
*
* DESCRIPTION
* Function to decode fixed parameters of Apdu of confirm service request.
*
* @param u8ReqAPDU			[in]      Apdu request data
* @param u16APDULen			[in]      Apdu request data length
* @param pProcQIndex		[in/out]  Process queue node to fill decoded data
* @param pu8ServiceRequest  [in/out]  Returns address variable part of Apdu
* @param pi16ServiceReqLen	[in/out]  Remaining length of apdu data to decode
*
* @return u16PduLen [out] Length of decoded apdu request data
*
*/
uint16_t APDU_Decode_Confirmed_Service_Request(
processInfo_t *pstProcQInfo,
uint8_t *u8ReqAPDU,
uint16_t u16APDULen,
uint8_t **pu8ServiceRequest,
int16_t *pi16ServiceReqLen);

/**
*
* DESCRIPTION
* Function to check if duplicate requests is received from client devices.
* Criteria for duplicate request:
* 1. Invoke id
* 2. Service type
* 3. Client device address
* in received request should match.
*
* @param 		[in]   No parameter (Void)
* @return bool  [out]  True if duplicate request else False.
*
*/
bool APDU_Check_If_Duplicate_Request(void);	

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* APDU_H */
