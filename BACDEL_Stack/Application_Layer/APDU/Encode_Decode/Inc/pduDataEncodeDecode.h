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
*   SoftDEL Systems Ltd.                     		    india@softdel.com         
*   3rd Floor, Pentagon P4,								http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   	
*
*	File Name - device.c
*
*   RELEASE HISTORY                                                                                                                           
*	DATE        NAME                    DESCRIPTION
*   29/06/2011	Harshal Mangale			File Created
*   29/06/2011  Harshal Mangale         property value encoding
*                                       according to its datatype
*   28/07/2011  M.Venu                  Input parameter modified to 
*                                       allow array index of property
*                                       in BACApp_Encode_Data_Type function
*   23/09/2011  Ashish verma            Changed the Character string structure
*                                       from BACNET_CHARACTER_STRING to 
*                                       Pr_BACnetCharStr_t.
*   10/09/2011  Ashish Verma            Added BACApp_Decode_COV_data() to decode
*                                       read ack frame for ActiveCOVSubscription.
******************************************************************************/
#ifndef DATA_ENCODE_DECODE_H
#define DATA_ENCODE_DECODE_H

#include "bacDELDef.h"
#include "bacDELApi.h"
#include <stdio.h>
#include "pduDataType.h"
#include "pduDateTime.h"

/* To check if the APDU length has crossed the MAX APDU value */
#define APDU_ERROR_LENGTH (MAX_APDU_LENGTH_ACCEPTED)

/* Errors returned by function BACApp_Encode_Data_Type() */
#define DATA_TYPE_NOT_SUPPORTED (-1)
#define SEGMENTATION_NOT_SUPPORTED (-2)
#define APDU_DATA_BUFFER_SIZE_EXCEEDED (-3)

/**********************************************************
* Function Declaration
***********************************************************/
    int32_t BACApp_Encode_data(
        uint8_t * pu8ApduBuf,
        BACNET_PROPERTY_VALUE * value,
        BACNET_PROPERTY_ID  eObjectProperty);

    int32_t BACApp_Decode_Data(
        uint8_t * apdu,
        uint8_t tag_data_type,
        uint32_t len_value_type,
        BACNET_PROPERTY_VALUE * value);

    int32_t BACApp_Decode_Application_Data(
        uint8_t * apdu,
        uint32_t u32Max_Apdu_Len,
        BACNET_PROPERTY_VALUE * value,
        BACNET_PROPERTY_ID  ePropID,
        BACNET_OBJECT_TYPE  eObjType);

	/* not defined yet */
    bool BACApp_Decode_Application_Data_Safe(
        uint8_t * new_apdu,
        uint32_t new_apdu_len,
        BACNET_PROPERTY_VALUE * value);

    int32_t BACApp_Encode_Application_Data(
        uint8_t * apdu,
        BACNET_PROPERTY_VALUE * value);

    int32_t BACApp_Encode_Data_Type(
        uint8_t * apdu, int32_t u32MaxInBufSize,
        void * application_data,
        BACNET_DATA_TYPE eData_Type, uint32_t u32ArrayIndex, 
        int32_t i32APDULenFilled,
        uint32_t u32SegSupport, bool bArrIndxPresent,
        uint16_t u16MaxApduLen, uint16_t *pu16NodeLenArr, 
		int32_t *pu32NodeCnt, int32_t i32NodeLenArrSize);

	/* not defined yet */
	int32_t BACAapp_Decode_Context_Data(
        uint8_t * apdu,
        uint32_t max_apdu_len,
        BACNET_PROPERTY_VALUE * value,
        BACNET_PROPERTY_ID eProperty);

	/* not defined yet */
    int32_t BACApp_Encode_Context_Data(
        uint8_t * apdu,
        BACNET_PROPERTY_VALUE * value,
        BACNET_PROPERTY_ID eProperty);

    int32_t BACApp_Encode_Context_Data_Value(
        uint8_t * apdu,
        uint8_t context_tag_number,
        BACNET_PROPERTY_VALUE * value);

    BACNET_APPLICATION_TAG BACApp_Prop_Context_Tag(
        BACNET_PROPERTY_ID eProperty,
        BACNET_OBJECT_TYPE eObjType,
        uint8_t i32Tag_Number,
        uint8_t i8Start_Tag, uint8_t u8Tag);

    /* returns the length of data between an opening tag and a closing tag.
       Expects that the first octet contain the opening tag.
       Include a value property identifier for context specific data
       such as the value received in a WriteProperty request */
    int32_t BACApp_Verify_Data_Len(
        uint8_t * apdu,
        uint32_t max_apdu_len,
        BACNET_PROPERTY_ID eProperty);

    int32_t BACApp_Decode_Data_Len(
        uint8_t * apdu,
        uint8_t tag_data_type,
        uint32_t len_value_type);

    int32_t BACApp_Decode_Application_Data_Len(
        uint8_t * apdu,
        uint32_t max_apdu_len);

    int32_t BACApp_Decode_Context_Data_Len(
        uint8_t * apdu,
        uint32_t max_apdu_len,
        BACNET_PROPERTY_ID eProperty);

    bool ConvertInStr_To_AppTag(
        BACNET_APPLICATION_TAG tag_number,
        int8_t *pu8PropertyVal,
        BACNET_PROPERTY_VALUE * pstPropVal);
	
	bool BACApp_Decode_Check_Property(BACNET_PROPERTY_ID  ePropID,
	BACNET_OBJECT_TYPE  eObjType);	

#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_SER_AE_EN_B || \
     defined BACDEL_SER_AE_AA_B || defined BACDEL_OBJ_EL)

#define TIMESTAMP_INVALID_TAG (-1)
#define TIMESTAMP_DECODE_ERROR (-2)

/** function to decode time stamp for event notification & ack alarm services. */
int32_t Decode_Time_Stamp(BACnetTimeStamp_t *pstTimeStamp, uint8_t *pu8APDU, uint8_t u8TagNo);

/** function to encode time stamp for event notification & ack alarm services. */
int32_t Encode_Time_Stamp(BACnetTimeStamp_t *pstTimeStamp, uint8_t *pu8APDU, uint8_t u8TagNo);

#endif 

#if (defined BACDEL_SER_AE_EN_B || defined BACDEL_OBJ_EL)

    /** function to encode notification parameters i.e. event values */
    int32_t Encode_Notification_Parameters(
        uint8_t *pu8APDU, 
        BacnetEnPropElem_t *pstENdata);

#endif

	/** Function to decode data as per property data-type */
	int32_t BACApp_Decode_Data_Type(
		uint8_t *pu8Apdu, uint32_t u32ApduDataLen,
		BACNET_PROPERTY_ID ePropID, BACNET_OBJECT_TYPE eObjType,
		BACNET_DATA_TYPE *peDataType, void **ppPropValue,
		uint32_t u32ArrayIndex, bool bArrayIndexFlag,
		uint32_t *pu32FirstFailedElement);


#endif /* DATA_ENCODE_DECODE_H */
