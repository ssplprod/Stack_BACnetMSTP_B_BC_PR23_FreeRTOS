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
*   SoftDEL Systems Ltd.                            india@softdel.com         
*   3rd Floor, Pentagon P4,                         http://www.softdel.com  
*    Magarpatta City, Hadapsar
*    Pune - 411 028                                
*                                                                         
*   FILE
*   serviceReadPropertyMultiple_B.h
*                                                                      
*   AUTHORS                                                                     
*   Harshal Mangale
*                                                                         
*   DESCRIPTION
*   The ReadPropertyMultiple service is used by a client BACnet-user to 
*	request the values of one or more specified properties of one or more 
*	BACnet Objects.
*   This file include function for RPM-B service. 
*                                                                         
*
**********************************************************************************/

#ifndef RPM_B_H
#define RPM_B_H
#ifdef BACDEL_SER_DS_RPM_B

/* header files */
#include "objDevice.h"

/**
*                                                                    
* DESCRIPTION
* This function is default handler when a read property multiple 
* request is received.
*    
* @param pstProcQInfo		 [in]	 Pointer to save decoded data.
* @param pu8ServiceRequest	 [in]    Data received for decoding.
* @param i32ServiceLen		 [in]    Length of service request.
*                                   
* @return BACNET_RETURN_TYPE [out]   Success or suitable error code.
*	
*/
BACNET_RETURN_TYPE RPM_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);

/**
*
* DESCRIPTION
* Function to encode the RPM-B acknowledgement. 
*
* @param pstProcQInfo	[in]	Pointer to service data
* @param pu8APDUResp	[in]    APDU buffer to save encoded data.
* @param i32APDULen		[in]    Length of encoded data in buffer.
*
* @return i32APDULen    [out]   Total length after encoding.
*
*/
int32_t RPM_B_Ack_Encoder(                    
    processInfo_t *pstProcQInfo,
    uint8_t **pu8APDUResp,
    int32_t i32APDULen);

/** */
int32_t RPM_Ack_Encode_Apdu_Object_Property(
    uint8_t * pu8APDUResp,
    BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex,
    bool bArrIndxPresent);

/** */
int32_t RPM_Ack_Encode_Apdu_Object_Property_value(
    uint8_t * pu8APDUResp,
    Bacnet_Multiple_Data_t * pstRpmData ,
    uint32_t u32SegSupport,
    uint16_t u16MaxApduLen);

/** */
int32_t RPM_Ack_Encode_Apdu_Object_Property_error(
    uint8_t * pu8APDUResp,
    BACNET_ERROR_CLASS eErrorClass,
    BACNET_ERROR_CODE eErrorCode);

/** */
int32_t RPM_Ack_Encode_Apdu_Object_End(
    uint8_t * pu8APDUResp);

/**
*
* DESCRIPTION                                                                          
* Function to validate and execute read multiple property service.
*    
* @param pVirtualDev	[in]  virtual device data.
* @param pstProcQInfo   [in]  rpm-b service data 
* @param u8ThreadNo     [in]  thread number
*                                   
* @return [out] true or false.
*
*/
bool RPM_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pstProcQInfo,	
	uint8_t u8ThreadNo);
	
/**
*
* DESCRIPTION
* Function to free allocated memories in RPM service parser.
*
* @param pstRpmResp		 [in]  pointer to RPM response structure.
* @param pstMultipleData [in]  pointer to RPM service structure.
*
* @return void.
*
*/
void RPM_B_Free_Service_Data(
	rpm_response_t *pstRpmResp,
	Bacnet_Multiple_Data_t *pstMultipleData);	

#endif /* BACDEL_SER_DS_RPM_B */
#endif /* RPM_B_H */
