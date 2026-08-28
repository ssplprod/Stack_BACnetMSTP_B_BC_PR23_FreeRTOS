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

/** header files */
#include "serviceGenericHandler.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "pduServiceStructure.h"
#include "objDevice.h"
#include "propertyGenricHandler.h"
#include "propertyClearValues.h"
#include "pduAbort.h"
#include "pduReject.h"
#include "bacnetNPDUHandler.h"

/** Global variables */
/* To access process queue */
extern osMutexId_t m_hProcessDataQMtxLockHandle;
extern uint8_t MSTP_GetBroadcastMacId(DESTINATION_TYPE eDestType,
	BACnetAddress_t *pstAddress);
extern DB_t SMCfg;
#ifdef BBMD_ENABLED
uint8_t gu8IamMPDU[I_AM_BUFF_SIZE]={0};
extern uint32_t Swap_Unsigned32_Bytes(uint32_t u32Value);
#endif /* BBMD_ENABLED*/
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
	BACNET_PDU_TYPE ePduType)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Map_With_Services_Supported: Entry \r\n");
	#endif

	/* check for confirmed services */
    if((ePduType == PDU_TYPE_CONFIRMED_SERVICE_REQUEST) ||
        (ePduType == PDU_TYPE_SIMPLE_ACK) ||
        (ePduType == PDU_TYPE_COMPLEX_ACK))
    {
        switch(u8ServiceChoice)
        {
        case SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM:
            return(SERVICE_SUPPORTED_ACKNOWLEDGE_ALARM);
            
        case SERVICE_CONFIRMED_COV_NOTIFICATION:
            return(SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION);
            
        case SERVICE_CONFIRMED_EVENT_NOTIFICATION:
            return(SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION);
            
        case SERVICE_CONFIRMED_GET_ALARM_SUMMARY:
            return(SERVICE_SUPPORTED_GET_ALARM_SUMMARY);
            
        case SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY:
            return(SERVICE_SUPPORTED_GET_ENROLLMENT_SUMMARY);
            
        case SERVICE_CONFIRMED_GET_EVENT_INFORMATION:
            return(SERVICE_SUPPORTED_GET_EVENT_INFORMATION);
            
        case SERVICE_CONFIRMED_SUBSCRIBE_COV:
            return(SERVICE_SUPPORTED_SUBSCRIBE_COV);
            
        case SERVICE_CONFIRMED_SUBSCRIBE_COV_PROPERTY:
            return(SERVICE_SUPPORTED_SUBSCRIBE_COV_PROPERTY);
            
        case SERVICE_CONFIRMED_LIFE_SAFETY_OPERATION:
            return(SERVICE_SUPPORTED_LIFE_SAFETY_OPERATION); 
            
        case SERVICE_CONFIRMED_ATOMIC_READ_FILE:
            return(SERVICE_SUPPORTED_ATOMIC_READ_FILE);
            
        case SERVICE_CONFIRMED_ATOMIC_WRITE_FILE:
            return(SERVICE_SUPPORTED_ATOMIC_WRITE_FILE);
            
        case SERVICE_CONFIRMED_ADD_LIST_ELEMENT:
            return(SERVICE_SUPPORTED_ADD_LIST_ELEMENT);
            
        case SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT:
            return(SERVICE_SUPPORTED_REMOVE_LIST_ELEMENT);
            
        case SERVICE_CONFIRMED_CREATE_OBJECT:
            return(SERVICE_SUPPORTED_CREATE_OBJECT);
            
        case SERVICE_CONFIRMED_DELETE_OBJECT:
            return(SERVICE_SUPPORTED_DELETE_OBJECT);
            
        case SERVICE_CONFIRMED_READ_PROPERTY:
            return(SERVICE_SUPPORTED_READ_PROPERTY);
            
        case SERVICE_CONFIRMED_READ_PROP_CONDITIONAL:
            return(SERVICE_SUPPORTED_READ_PROP_CONDITIONAL);
            
        case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
            return(SERVICE_SUPPORTED_READ_PROP_MULTIPLE);
            
        case SERVICE_CONFIRMED_READ_RANGE:
            return(SERVICE_SUPPORTED_READ_RANGE);
            
        case SERVICE_CONFIRMED_WRITE_PROPERTY:
            return(SERVICE_SUPPORTED_WRITE_PROPERTY);
            
        case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
            return(SERVICE_SUPPORTED_WRITE_PROP_MULTIPLE);
            
        case SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL:
            return(SERVICE_SUPPORTED_DEVICE_COMMUNICATION_CONTROL);
            
        case SERVICE_CONFIRMED_PRIVATE_TRANSFER:
            return(SERVICE_SUPPORTED_CONFIRMED_PRIVATE_TRANSFER);
            
        case SERVICE_CONFIRMED_TEXT_MESSAGE:
            return(SERVICE_SUPPORTED_CONFIRMED_TEXT_MESSAGE);
            
        case SERVICE_CONFIRMED_REINITIALIZE_DEVICE:
            return(SERVICE_SUPPORTED_REINITIALIZE_DEVICE);
            
        case SERVICE_CONFIRMED_VT_OPEN:
            return(SERVICE_SUPPORTED_VT_OPEN);
            
        case SERVICE_CONFIRMED_VT_CLOSE:
            return(SERVICE_SUPPORTED_VT_CLOSE);
            
        case SERVICE_CONFIRMED_VT_DATA:
            return(SERVICE_SUPPORTED_VT_DATA);
            
        case SERVICE_CONFIRMED_AUTHENTICATE:
            return(SERVICE_SUPPORTED_AUTHENTICATE);
            
        case SERVICE_CONFIRMED_REQUEST_KEY:
            return(SERVICE_SUPPORTED_REQUEST_KEY);
            
        default:
            break;
        }
    }
	/* check for un-confirmed services */
    else if(ePduType == PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST)
    {
        switch(u8ServiceChoice)
        {
        case SERVICE_UNCONFIRMED_I_AM:
            return(SERVICE_SUPPORTED_I_AM);
            
        case SERVICE_UNCONFIRMED_I_HAVE:
            return(SERVICE_SUPPORTED_I_HAVE);
            
        case SERVICE_UNCONFIRMED_COV_NOTIFICATION:
            return(SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION);
            
        case SERVICE_UNCONFIRMED_EVENT_NOTIFICATION:
            return(SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION);
            
        case SERVICE_UNCONFIRMED_PRIVATE_TRANSFER:
            return(SERVICE_SUPPORTED_UNCONFIRMED_PRIVATE_TRANSFER);
            
        case SERVICE_UNCONFIRMED_TEXT_MESSAGE:
            return(SERVICE_SUPPORTED_UNCONFIRMED_TEXT_MESSAGE);
            
        case SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION:
            return(SERVICE_SUPPORTED_TIME_SYNCHRONIZATION);
            
        case SERVICE_UNCONFIRMED_WHO_HAS:
            return(SERVICE_SUPPORTED_WHO_HAS);
            
        case SERVICE_UNCONFIRMED_WHO_IS:
            return(SERVICE_SUPPORTED_WHO_IS);
            
        case SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION:
            return(SERVICE_SUPPORTED_UTC_TIME_SYNCHRONIZATION);
            break;
		#ifdef BACDEL_PR14
		case SERVICE_UNCONFIRMED_WRITE_GROUP:
			return(SERVICE_SUPPORTED_WRITE_GROUP);
            break;
		#endif
        default:
            break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Map_With_Services_Supported: Exit \r\n");
	#endif
    return(MAX_BACNET_SERVICES_SUPPORTED);
}


/*****************************************************************************/
/* To Include/Exclude support to services like RP & WP */
#if ((defined BACDEL_SER_DS_RP_A) || (defined BACDEL_SER_DS_RP_B) \
    || (defined BACDEL_SER_DS_WP_A) || (defined BACDEL_SER_DS_WPM_A)\
    || (defined BACDEL_SER_DS_RPM_B))

/** 
*******************************************************************************
*
*DESCRIPTION:
*   Function to decode received request parameters,
*   Decodes Object Id, Property ID and Array Index for processing
*   RP-B, RP-Ack, WP-B, WPM-Error-Ack service.
*
*   Single Read/write command
*        SD 0    Object_ID
*        SD 1    Prop_ID
*        SD 2    Arr_Index    (Optional)
*
*@param pu8ReqBuffer    [in]    Received request buffer
*@param i16ReqLen       [in]    Remaning decode length of request
*@param pstReqData      [out]   Structure to store parameters of request
*
*@return    i32Len      Total decoded length of received request Or
*                       Abort/ Reject if decoding failed
*
*******************************************************************************/
int32_t Decode_ObjId_Prop_ArrIdx( uint8_t * pu8ReqBuffer, int16_t i16ReqLen,
                                BACNET_CONF_DATA * pstReqData)
{
    int32_t i32Len = 0;
    uint32_t u32LenValueType = 0;
    uint32_t u32Type = 0;  /* for decoding */
    uint32_t u32Property = 0;      /* for decoding */
    uint32_t u32ArrayValue = 0;   /* for decoding */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_ObjId_Prop_ArrIdx : entry \r\n");
	#endif

    /* Tag 0: Object Identifier */
    if(i32Len > i16ReqLen)
    {
        pstReqData->eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Decode_ObjId_Prop_ArrIdx: \
		missing parameter\r\n");
		#endif

        return BACNET_STATUS_REJECT;
    }
    else if(!Decode_Context_Tag(&pu8ReqBuffer[i32Len++], TAG_NO_0, &u32LenValueType))
    {
        pstReqData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Decode_ObjId_Prop_ArrIdx: \
		Invalid tag\r\n");
		#endif

        return BACNET_STATUS_REJECT;
    }

    if( DATA_LEN_4 != u32LenValueType )
    {
        pstReqData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Decode_ObjId_Prop_ArrIdx: \
		Invalid tag\r\n");
		#endif

        return BACNET_STATUS_REJECT;
    }
    i32Len += Decode_Object_Id(&pu8ReqBuffer[i32Len], &u32Type, &pstReqData->u32ObjectInstance);
    pstReqData->eObjectType = (BACNET_OBJECT_TYPE) u32Type;
    

    /* Tag 1: Property ID */
    if(i32Len > i16ReqLen)
    {
        pstReqData->eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Decode_ObjId_Prop_ArrIdx: \
		missing parameter\r\n");
		#endif

        return BACNET_STATUS_REJECT;
    }
    else if(!Decode_Context_Tag(&pu8ReqBuffer[i32Len++], TAG_NO_1, &u32LenValueType))
    {
        pstReqData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Decode_ObjId_Prop_ArrIdx: \
		invalid tag\r\n");
		#endif

        return BACNET_STATUS_REJECT;
    }
    i32Len += Decode_Enumerated(&pu8ReqBuffer[i32Len], u32LenValueType, &u32Property);

    pstReqData->eObjectProperty = (BACNET_PROPERTY_ID) u32Property;
    

    /* Tag 2: Optional Array Index */
    /* note: decode without incrementing len so we can check for opening tag */
    if( i32Len < i16ReqLen && 
        (Decode_Context_Tag(&pu8ReqBuffer[i32Len], TAG_NO_2, &u32LenValueType)) ) 
    {
        i32Len++;

        if( i32Len < i16ReqLen ) 
        {
            i32Len +=
                Decode_Unsigned(&pu8ReqBuffer[i32Len], u32LenValueType, &u32ArrayValue);
            
            pstReqData->u32ArrayIndex = u32ArrayValue;
            pstReqData->bArrIndxPresent = ARRAY_INDEX_PRESENT;
        } 
        else 
        {
            pstReqData->bArrIndxPresent = ARRAY_INDEX_ABSENT;
            pstReqData->eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Decode_ObjId_Prop_ArrIdx: \
				missing parameter \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }
    } 
    else
    {
        pstReqData->u32ArrayIndex = BACNET_ARRAY_ALL;
        pstReqData->bArrIndxPresent = ARRAY_INDEX_ABSENT;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_ObjId_Prop_ArrIdx returns: exit \r\n");
	#endif

    return i32Len;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   Function to encode received request parameters,
*   Encodes Object Id, Property ID and Array Index to send 
*   RP-A, RP-Ack, WP-A, WPM-Error-Ack service.
*
*   Single Read/write command
*        SD 0    Object_ID
*        SD 1    Prop_ID
*        SD 2    Arr_Index    (Optional)
*
*@param pstEncodeParam       [in]    Structure of parameters of read property request
*@param pu8EncodeBuffer     [out]   Array to store encoded data
*
*@return    u16EncodeLen    total length of the apdu encoded
*
*******************************************************************************/
int32_t Encode_ObjId_Prop_ArrIdx(
    uint8_t * pu8EncodeBuffer,
    BACNET_CONF_DATA * pstEncodeParam)
{
    int32_t i32Len = 0;        /* length of each encoding */
    int32_t i32EncodeLen = 0;   /* total length of the apdu, return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_ObjId_Prop_ArrIdx: entry \r\n");
	#endif

    if (pu8EncodeBuffer) 
    {
        /* Encode Object Identifier */
        i32Len =
            Encode_Context_Object_Id(&pu8EncodeBuffer[i32EncodeLen], TAG_NO_0, 
                (int32_t)pstEncodeParam->eObjectType, pstEncodeParam->u32ObjectInstance);
        i32EncodeLen += i32Len;

        /* Encode Property Identifier */
        i32Len =
            Encode_Context_Enumerated(&pu8EncodeBuffer[i32EncodeLen], TAG_NO_1,
            pstEncodeParam->eObjectProperty);
        i32EncodeLen += i32Len;

        /* context 2 array index is optional */
        if (ARRAY_INDEX_PRESENT == pstEncodeParam->bArrIndxPresent)
        {
            i32Len =
                Encode_Context_Unsigned(&pu8EncodeBuffer[i32EncodeLen], TAG_NO_2,
                pstEncodeParam->u32ArrayIndex);
            i32EncodeLen += i32Len;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_ObjId_Prop_ArrIdx : exit \r\n");
	#endif

    return i32EncodeLen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   encode the object Id portion of the response
*
*@param pu8APDUResp [out]   Response buffer
*@param pstRpmData  [in]    Structure of RPM request parameters
*
*@return    i32APDULen  Total encoded length of rsponse
*
*******************************************************************************/
int32_t Encode_MPS_ObjId_OTag(
    uint8_t * pu8APDUResp,
    Bacnet_Multiple_Data_t * pstRpmData)
{
    int32_t i32APDULen = 0;   /* total length of the apdu, return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_MPS_ObjId_OTag: entry \r\n");
	#endif
	
    if (pu8APDUResp) 
    {
        /* Tag 0: objectIdentifier */
        i32APDULen =
            Encode_Context_Object_Id(&pu8APDUResp[0], TAG_NO_0, (int32_t)pstRpmData->m_stServiceData.eObjectType,
            pstRpmData->m_stServiceData.u32ObjectInstance);
        
        /* Tag 1: Opening Tag for listOfProperty Refference */
        i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_1);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_MPS_ObjId_OTag: exit \r\n");
	#endif

    return i32APDULen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   encode the object Property and Array Index portion of the response
*
*@param pu8APDUResp     [out]   Response buffer
*@param eObjectProperty [in]    Property Identifier
*@param i32ArrayIndex   [in]    Array Index 
*
*@return    i32APDULen  Total encoded length of rsponse
*
*******************************************************************************/
int32_t Encode_MPS_property_ArrIdx(
    uint8_t * pu8EncodeBuffer,
    BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex, bool bArrIndxPresent)
{
    int32_t i32APDULen = 0;   /* total length of the apdu, return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_MPS_property_ArrIdx: entry \r\n");
	#endif
	
    if (pu8EncodeBuffer) 
    {
        /* Tag 0: propertyIdentifier */
        i32APDULen = Encode_Context_Enumerated(&pu8EncodeBuffer[0],
                                TAG_NO_0, eObjectProperty);
        
        /* Tag 1: optional propertyArrayIndex */
        if (ARRAY_INDEX_PRESENT == bArrIndxPresent)
            i32APDULen +=
                Encode_Context_Unsigned(&pu8EncodeBuffer[i32APDULen],
                         TAG_NO_1, u32ArrayIndex);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_MPS_property_ArrIdx: exit \r\n");
	#endif
	
    return i32APDULen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   decode the object Id portion of the service request only. 
*   Rject if tags are wrong or missing/incomplete
*
*@param pu8APDUReq  [in]    Received RPM-B request
*@param u32APDULen  [in]    Length of received RPM-b request
*@param pstRpmData  [out]   Structure to store parameters of RPM request
*
*@return    u32Len                  Total decoded length of received request
*           BACNET_STATUS_REJECT    If tags are wrong or missing/incomplete
*
*******************************************************************************/
int32_t Decode_MPS_object_id(
    uint8_t * pu8APDUReq,
    uint32_t u32APDULen,
    Bacnet_Multiple_Data_t * pstRpmData)
{
    int32_t i32Len = 0;
    uint32_t u32Type = 0;  /* for decoding */
    uint32_t u32LenValueType = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_MPS_object_id : entry \r\n");
	#endif
	
    /* check for value pointers */
    if (pu8APDUReq && u32APDULen && pstRpmData) 
    {
        if (u32APDULen < 5) 
        {     /* Must be at least 2 tags and an object id */
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_object_id : missing parameter \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }

        /* Tag 0: Object ID */
        if(!Decode_Context_Tag(&pu8APDUReq[i32Len++], TAG_NO_0, &u32LenValueType))
        {
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_object_id : invalid tag \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }
        if( DATA_LEN_4 != u32LenValueType )
        {
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_object_id : invalid tag \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }
        i32Len +=  Decode_Object_Id(&pu8APDUReq[i32Len], &u32Type,
                             &pstRpmData->m_stServiceData.u32ObjectInstance);
        pstRpmData->m_stServiceData.eObjectType = (BACNET_OBJECT_TYPE) u32Type;
        
        /* Tag 1: sequence of Read Access Specification */
        if (!Decode_Is_Opening_Tag_Number(&pu8APDUReq[i32Len], TAG_NO_1))
        {
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_object_id : invalid tag \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }
        i32Len++;  /* opening tag is only one octet */
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_MPS_object_id : exit\r\n");
	#endif

    return i32Len;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   decode the object property portion of the service request only
*   BACnetPropertyReference ::= SEQUENCE 
*   {
*        propertyIdentifier [0] BACnetPropertyIdentifier,
*        propertyArrayIndex [1] Unsigned OPTIONAL
*        -- used only with array datatype
*        -- if omitted with an array the entire array is referenced
*    }
*
*@param pu8APDUReq  [in]    Received RPM-B request
*@param u32APDULen  [in]    Length of received RPM-b request
*@param pstRpmData  [out]   Structure to store parameters of RPM request
*
*@return    u32Len                  Total decoded length of received request
*           BACNET_STATUS_REJECT    If tags are wrong or missing/incomplete
*
*******************************************************************************/
int32_t Decode_MPS_property_ArrIdx(
    uint8_t * pu8APDUReq,
    uint32_t u32APDULen,
    Bacnet_Multiple_Data_t * pstRpmData)
{
    int32_t i32Len = 0;
    uint32_t u32OptionLen = 0;
    uint8_t u8TagNumber = 0;
    uint32_t u32LenValueType = 0;
    uint32_t u32Property = 0;      /* for decoding */
    uint32_t u32ArrayValue = 0;   /* for decoding */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_MPS_property_ArrIdx: entry \r\n");
	#endif

    /* check for valid pointers */
    if (pu8APDUReq && u32APDULen && pstRpmData) 
	{
        /* Tag 0: propertyIdentifier */
        if(!Decode_Context_Tag(&pu8APDUReq[i32Len++], TAG_NO_0, &u32LenValueType))
		{
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_property_ArrIdx: invalid tag \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }

        /* Should be at least the uint32_t value + 1 tag left */
        if ((i32Len + u32LenValueType) >= u32APDULen)
		{
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_property_ArrIdx: missing parameter \r\n");
			#endif

            return BACNET_STATUS_REJECT;
        }
        i32Len += Decode_Enumerated(&pu8APDUReq[i32Len], u32LenValueType, &u32Property);

        pstRpmData->m_stServiceData.eObjectProperty = (BACNET_PROPERTY_ID) u32Property;
        
		/* Assume most probable outcome */
        pstRpmData->m_stServiceData.u32ArrayIndex = BACNET_ARRAY_ALL;
        pstRpmData->m_stServiceData.bArrIndxPresent = ARRAY_INDEX_ABSENT;
        /* Tag 1: Optional propertyArrayIndex */
        if (IS_CONTEXT_SPECIFIC(pu8APDUReq[i32Len]) && !IS_CLOSING_TAG(pu8APDUReq[i32Len]))
		{
            u32OptionLen =
                (unsigned) Decode_Tag_Number_And_Value(&pu8APDUReq[i32Len], &u8TagNumber,
                &u32LenValueType);
            if (u8TagNumber == TAG_NO_1) 
			{
            	i32Len += u32OptionLen;
                /* Should be at least the uint32_t array index + 1 tag left */
                if ((i32Len + u32LenValueType) >= u32APDULen)
                {
                    pstRpmData->m_stServiceData.eErrorCode =
                        ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_MPS_property_ArrIdx: missing parameter \r\n");
					#endif

                    return BACNET_STATUS_REJECT;
                }
                i32Len +=
                    Decode_Unsigned(&pu8APDUReq[i32Len], u32LenValueType, &u32ArrayValue);
                pstRpmData->m_stServiceData.u32ArrayIndex = u32ArrayValue;
                pstRpmData->m_stServiceData.bArrIndxPresent = ARRAY_INDEX_PRESENT;
            }
        }
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_MPS_property_ArrIdx: exit \r\n");
	#endif

    return i32Len;
} /* SERVICE_HANDLER_H */

#endif

///*****************************************************************************/


/*****************************************************************************/
/* To Include/Exclude support to services like DDB & DOB */
#if (defined(BACDEL_SER_DM_DDB_A) || defined(BACDEL_SER_DM_DDB_B) || \
    defined(BACDEL_SER_DM_DOB_A) || defined(BACDEL_SER_DM_DOB_B))
/**
*
*DESCRIPTION
*   Check the received device instance range with the Device Instance
* 
*@param pstDeviceAddressRange [in] Has the stored High Limit & 
*                                   Low Limit Range.
*@param u32DeviceInstanceID [in] Has the Local Device Instance ID.
*@return BACDEL_SUCCESS/BACDEL_ERROR
*
*/
BACNET_RETURN_TYPE Check_Device_Address_Range(
	ddb_who_is_t *pstDevAddrRange, 
	uint32_t u32DevID)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_OUT_OF_RANGE_ERROR;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Check_Device_Address_Range: entry \r\n");
	#endif
	
    if((((uint32_t)pstDevAddrRange->m_i32DevRangeLowLimit <= u32DevID) && 
       ((uint32_t)pstDevAddrRange->m_i32DevRangeHighLimit >= u32DevID)) ||
       ((BACNET_PARAMETER_ABSENT == pstDevAddrRange->m_i32DevRangeLowLimit) &&
       (BACNET_PARAMETER_ABSENT == pstDevAddrRange->m_i32DevRangeHighLimit)))
    {
		/* validation success */
        eRetVal = BACDEL_SUCCESS;
    }

	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Check_Device_Address_Range: exit \r\n");
	#endif
    return eRetVal;
}

#endif /* #if (defined(BACDEL_SER_DM_DDB_A) || defined(BACDEL_SER_DM_DDB_B) || \
    defined(BACDEL_SER_DM_DOB_A) || defined(BACDEL_SER_DM_DOB_B) */
/*****************************************************************************/


/**
*******************************************************************************
*                                                                         
*DESCRIPTION                                                                          
*   This function Frees Memory of RPM service Data Link list starting from
*   Received pstRpmData pointer
*    
*@param pstRpmData   [in]    structure of decoded service parameters
*                                                                      
******************************************************************************/
void Free_Service_Memory(Bacnet_Multiple_Data_t *pstMPSData)
{
    Bacnet_Multiple_Data_t *pstTmpMPSData = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Free_Service_Memory: entry \r\n");
	#endif
	
    while(pstMPSData != NULL)
    {
        pstTmpMPSData = pstMPSData->m_pstMDNext;

        if(NULL != pstMPSData->m_stServiceData.pu8PropValueBuffer)
            OSAL_Free(pstMPSData->m_stServiceData.pu8PropValueBuffer,  __FILE__, __FUNCTION__, __LINE__);
        
        pstMPSData->m_stServiceData.pu8PropValueBuffer = NULL;

        OSAL_Free(pstMPSData,  __FILE__, __FUNCTION__, __LINE__);
        pstMPSData = NULL;
        pstMPSData = pstTmpMPSData;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Free_Service_Memory: exit \r\n");
	#endif
	
}

/**
*******************************************************************************
*
*DESCRIPTION
*   This function frees memory allocated for all services in processInfo_t struct
*
*   This function is used when 
*   1.  response is received in PROCESS_QUEUE after initiating RP-A, WP-A,RPM-A,
*       WPM-A, Notification for Confirm/Unconfirm COV_SUB
*   2.  Clearing Request Parameters from INITIATE_QUEUE after RESPONSE_RECEIVED
*   
*@param pstProcessQData   [in]  Pointer to Process queue location to be free
*
******************************************************************************/
void Clear_Process_Info(processInfo_t* pstProcessQData)
{
	/* local variables */
	BACNET_SERVICES_SUPPORTED eServiceSupported = MAX_BACNET_SERVICES_SUPPORTED;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Process_Info: entry \r\n");
	#endif
	
	if(	NULL != pstProcessQData &&
		NULL != pstProcessQData->m_stProcessData.m_stAPDU.m_pvServiceRequestData)
	{
		/* map the service supported */
		eServiceSupported = Map_With_Services_Supported
			(pstProcessQData->m_stProcessData.m_stAPDU.m_u8ServiceChoice,
			pstProcessQData->m_stProcessData.m_ePDUType);

		switch(eServiceSupported)
		{

		default:
			/* Free the Memory allocated to the Pointer */
			OSAL_Free(pstProcessQData->m_stProcessData.m_stAPDU.
					m_pvServiceRequestData, __FILE__, __FUNCTION__, __LINE__);
			break;
		}// end of switch

		/* Set service data pointer to NULL and all parameters to 0 */
		pstProcessQData->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Process_Info: exit\r\n");
	#endif
}


/**
*
*DECRIPTION
*   This function is called when an error has occurred. It free's the allocated
*   heap memory and makes the Process Queue location avaliable for storing next
*   request.
*
*@param pstProcQIndex [in] Process Queue Index
*@return void
*
*/
void Exit_Routine_To_Error_Of_Unconfirmed_Request(processInfo_t *pstProcQIndex)
{
    /* Lock the Process Queue for accquiring an avaliable queue 
     * location */
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackMisc:Exit_Routine_To_Error_Of_Unconfirmed_Request:Entry\r\n");
	#endif

    if(Osal_Wait_Mutex(m_hProcessDataQMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackMisc:Exit_Routine_To_Error_Of_Unconfirmed_Request:Wait on ProcData queue mutex Failed\r\n");
		#endif

		return;
	}

    if(pstProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData)
    {
        OSAL_Free(pstProcQIndex->m_stProcessData.m_stAPDU.
            m_pvServiceRequestData,  __FILE__, __FUNCTION__, __LINE__);
        pstProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
    }

	/* Update process queue status to free the queue location */
    //pstProcQIndex->m_eState = PROC_INIT;

    /* Unlock Mutex */
	Osal_Release_Mutex(m_hProcessDataQMtxLockHandle);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackMisc:Exit_Routine_To_Error_Of_Unconfirmed_Request:Exit\r\n");
	#endif
}                                    
       

/**
*
*DECRIPTION
*   This function is called when an error has occurred for a confirmed request.
*   It free's the allocated heap memory, updates Error/Reject/Abort response 
*   for this request and updates process queue status to send response in
*   transmit thread.
*
*@param pstProcQIndex [in] Process Queue Index
*@param i32PDUType [in] Specfies the PDU type.
*@param eErrorClass [in] Specifies the error class.
*@param eErrorCode [in] Specifies the error type.
*@return void
*
*/
void Exit_Routine_To_Error_Of_Confirmed_Request(processInfo_t *pstProcQIndex,
                                                int32_t i32PDUType, 
                                                BACNET_ERROR_CLASS eErrorClass,
                                                BACNET_ERROR_CODE eErrorCode)
{
	/* local variables */
    BACNET_SERVICES_SUPPORTED eServiceSupport = MAX_BACNET_SERVICES_SUPPORTED;

    /* Lock the Process Queue for accquiring an avaliable queue 
     * location */
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackMisc:Exit_Routine_To_Error_Of_Confirmed_Request:Entry\r\n");
	#endif
	 
    if(Osal_Wait_Mutex(m_hProcessDataQMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackMisc:Exit_Routine_To_Error_Of_Confirmed_Request:wait on ProcData queue mutex failed\r\n");
		#endif

		return;
	}

	/* get the service type */
	eServiceSupport = pstProcQIndex->m_stProcessData.m_eServiceSupported;

	/* clear service data */
	Clear_Bside_ServiceData(eServiceSupport, &pstProcQIndex->m_stProcessData.m_stAPDU.
		m_pvServiceRequestData);

    /* Update Error/Reject/Abort response for this request */
    Service_Error_Handler(i32PDUType, eErrorClass, eErrorCode, pstProcQIndex, TRUE);  

    /* Update process queue status to send response in transmit
     * thread */ 
    //pstProcQIndex->m_eState = PROC_DONE;
   
	/* Unlock Mutex */
    Osal_Release_Mutex(m_hProcessDataQMtxLockHandle);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackMisc: \
			Exit_Routine_To_Error_Of_Confirmed_Request: Exit \r\n");
	#endif
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function is called whenever local date & local time properties
*	of device object are read & whenever TS & UTC requests comes.
*	This function sets date & time properties from received date & time.
*    
*@param pVirtualDev	[in]  Instance of virtual device to which device 
*						  device object belongs.
*@param bFlag		[in]  not used yet.						  
*@param pstTime		[in]  input pointer to time value.
*@param pstDate		[in]  input pointer to date value.
*
*@returns void.
*	
***/
void Update_Dv_Local_DateTime_Properties(void *pvVirtualDev,
										 BACnetTime_t *pstTime,
										 BACnetDate_t *pstDate,
										 bool bFlag)
{
	/* local variable */
	virtualDevData_t *pVirtualDev = NULL;

	/* function entry debug msg */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Update_Dv_Local_DateTime_Properties: Entry \r\n");
	#endif

	/* get virtual device pointer */
	pVirtualDev = pvVirtualDev;

	if(NULL == pVirtualDev || NULL == pstTime || NULL == pstDate)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
		Update_Dv_Local_DateTime_Properties: Null input pointer \r\n");
		#endif

		return;
	}

	#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)

	/* update the local date */
	pVirtualDev->m_stDevObject.m_stLocalDate.
		m_Date_val.m_u8Day = pstDate->m_u8Day;
	pVirtualDev->m_stDevObject.m_stLocalDate.
		m_Date_val.m_u8Month = pstDate->m_u8Month;
	pVirtualDev->m_stDevObject.m_stLocalDate.
		m_Date_val.m_u8Wday = pstDate->m_u8Wday;
	pVirtualDev->m_stDevObject.m_stLocalDate.
		m_Date_val.m_u16Year = pstDate->m_u16Year;

	/* upadte the local time */
	pVirtualDev->m_stDevObject.m_stLocalTime.
		m_Time_val.m_u8Hour = pstTime->m_u8Hour;
	pVirtualDev->m_stDevObject.m_stLocalTime.
		m_Time_val.m_u8Hundredths = pstTime->m_u8Hundredths;
	pVirtualDev->m_stDevObject.m_stLocalTime.
		m_Time_val.m_u8Min = pstTime->m_u8Min;
	pVirtualDev->m_stDevObject.m_stLocalTime.
		m_Time_val.m_u8Sec = pstTime->m_u8Sec;

	#endif
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Update_Dv_Local_DateTime_Properties: Exit \r\n");
	#endif
}

/**
*
* DESCRIPTION
*   function to validate the supported character set encoding.
*
*/
BACNET_RETURN_TYPE Validate_Charstring_Encoding(
	BACNET_CHARACTER_STRING_ENCODING eEncoding,
	uint16_t u16CodePage)
{
	/* locals */
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;

	/* switch as per encoding type */
	switch(eEncoding)
	{
	case CHARACTER_IBM_MS_DBCS:
		switch(u16CodePage)
		{
		case 850: /* Western Europe - ANSI */
		case 932: /* Japanese Industrial Standard - JIS */
			break;
		default:
			eReturnVal = BACDEL_CODE_PAGE_NOT_SUPPORTED;
			break;
		}
		break;

	case CHARACTER_ANSI_X34:
	case CHARACTER_ISO_8859_1:
	case CHARACTER_UCS2:
		break;

		/* default - encoding not supported */
	default:
		eReturnVal = BACDEL_CHARACTER_SET_NOT_SUPPORTED;
		break;
	}

	/* return the value */
	return eReturnVal;
}

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
uint32_t Generate_Callback_ID(void)
{
    /* local varaibles */
    uint32_t u32NewCallbackId = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
   	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Generate_Callback_ID: Entry \r\n");
	#endif

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Generate_Callback_ID: Exit \r\n");
	#endif

    /* return new callback id value */
    return u32NewCallbackId;
} 

/**
*
* DESCRIPTION  
* This function is used to free b-side service data.
*
* @param pvServiceData		[in] destination address.
* @param eServiceSupport	[in] servcie type.
*
* @return void
*
*/
void Clear_Bside_ServiceData(
	BACNET_SERVICES_SUPPORTED eServiceSupport, 
	void **ppvServiceData)
{
	/* local variables */
	void *pvServiceData = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Clear_Bside_ServiceData: Entry\r\n");
	#endif

	/* check input pointers */
	if(NULL == ppvServiceData || NULL == *ppvServiceData)
	{
		return;
	}

	/* get the pointer */
	pvServiceData = *ppvServiceData;
	*ppvServiceData = NULL;

	/* free memory as per service type */
	switch(eServiceSupport)
    {
		#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
        case SERVICE_SUPPORTED_SUBSCRIBE_COV:
        case SERVICE_SUPPORTED_SUBSCRIBE_COV_PROPERTY:
        {
			/* locals */
            ListOfBACnetCovSubs_t *pstCOVPSubscribe = NULL;
            CovPropElement_t *pstCOVPropElem = NULL;
            CovPropElement_t *pstPropElemNxt = NULL;
            CovRecipientInfo_t *m_pstSubscriberInfo = NULL;

			/* convert to service data */
            pstCOVPSubscribe = (ListOfBACnetCovSubs_t *)pvServiceData;

			/* free property element data */
            pstCOVPropElem = pstCOVPSubscribe->m_stCOVPropertyElem.m_pstNext;
            while(pstCOVPropElem != NULL)
            {
                pstPropElemNxt = pstCOVPropElem->m_pstNext;
                OSAL_Free(pstCOVPropElem, __FILE__, __FUNCTION__, __LINE__);
                pstCOVPropElem = pstPropElemNxt;
            }

            /* free subsriber  info */
            m_pstSubscriberInfo = pstCOVPSubscribe->m_pstSubscriberInfo;
            OSAL_Free(m_pstSubscriberInfo,  __FILE__, __FUNCTION__, __LINE__);
            pstCOVPSubscribe->m_pstSubscriberInfo = NULL;
        }
        break;
		#endif /* COV-B || COVP-B */





		default:
			/* do nothing */
			break;
    }

	/* free memory */
	OSAL_Free(pvServiceData,  __FILE__, __FUNCTION__, __LINE__);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer:\
	Clear_Bside_ServiceData: Exit \r\n");
	#endif
	return;
}

/**       //KV
*
* DESCRIPTION
* This function converts service type enum to auto-response callback type enum.
*
* @param eServiceSupport [in] servcie type.
* @return eCallback  [out] enumeration for auto-responses type
*
*/
BACAPP_CALLBACK_FUN_CHOICE Convert_ServiceType_To_CallbackType(
	BACNET_SERVICES_SUPPORTED eServiceSupport)
{
	/* local variables */
	BACAPP_CALLBACK_FUN_CHOICE eCallbackType = MAX_APP_CALLBACK_FUN;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: "
	"Convert_ServiceType_To_CallbackType: Entry \r\n");
	#endif

	switch(eServiceSupport)
	{
		#ifdef BACDEL_SER_DS_RP_B
		case SERVICE_SUPPORTED_READ_PROPERTY:
			eCallbackType = APP_CB_READ_PROPERTY;
		break;
		#endif /* RP-B */

		#ifdef BACDEL_SER_DS_RPM_B
		case SERVICE_SUPPORTED_READ_PROP_MULTIPLE:
			eCallbackType = APP_CB_READ_PROPERTY_MULTIPLE;
		break;
		#endif /* RPM-B */

		#ifdef BACDEL_SER_DS_WP_B
		case SERVICE_SUPPORTED_WRITE_PROPERTY:
			eCallbackType = APP_CB_WRITE_PROPERTY;
		break;
		#endif /* WP-B */

		#ifdef BACDEL_SER_DS_WPM_B
		case SERVICE_SUPPORTED_WRITE_PROP_MULTIPLE:
			eCallbackType = APP_CB_WRITE_PROPERTY_MULTIPLE;
		break;
		#endif

			#ifdef BACDEL_SER_DS_COV_B
		case SERVICE_SUPPORTED_SUBSCRIBE_COV:
			eCallbackType = APP_CB_SUBSCRIBE_COV;
		break;
		#endif /* COV-B */


		#ifdef BACDEL_SER_AE_AA_B
		case SERVICE_SUPPORTED_ACKNOWLEDGE_ALARM:
			eCallbackType = APP_CB_ACKNOWLEDGE_ALARM;
		break;
		#endif /* AA-B */

		#ifdef BACDEL_SER_AE_GEI_B
		case SERVICE_SUPPORTED_GET_EVENT_INFORMATION:
			eCallbackType = APP_CB_GET_EVENT_INFORMATION;
		break;
		#endif /* GEI-B */

		#ifdef BACDEL_SER_DM_DCC_B
		case SERVICE_SUPPORTED_DEVICE_COMMUNICATION_CONTROL:
			eCallbackType = APP_CB_DEVICE_COMMUNICATION_CONTROL;
		break;
		#endif /* DCC-B */

		#ifdef BACDEL_SER_DM_RD_B
		case SERVICE_SUPPORTED_REINITIALIZE_DEVICE:
			eCallbackType = APP_CB_REINITIALIZE_DEVICE;
		break;
		#endif /* RD-B */

		#ifdef BACDEL_SER_DM_TS_B
		case SERVICE_SUPPORTED_TIME_SYNCHRONIZATION:
			eCallbackType = APP_CB_TIME_SYNC;
		break;
		#endif /* TS-B */

		#ifdef BACDEL_SER_DM_UTC_B
		case SERVICE_SUPPORTED_UTC_TIME_SYNCHRONIZATION:
			eCallbackType = APP_CB_UTC_TIME_SYNC;
		break;
		#endif /* UTC-B */

		#ifdef BACDEL_SER_DM_DDB_B
		case SERVICE_SUPPORTED_WHO_IS:
			eCallbackType = APP_CB_WHO_IS;
		break;
		#endif /* DDB-B */

		#ifdef BACDEL_SER_DM_DOB_B
		case SERVICE_SUPPORTED_WHO_HAS:
			eCallbackType = APP_CB_WHO_HAS;
		break;
		#endif /* DOB-B */

		#ifdef BACDEL_SER_DM_DDB_A
		case SERVICE_SUPPORTED_I_AM:
			eCallbackType = APP_CB_I_AM;
		break;
		#endif /* DDB-A */

		#ifdef BACDEL_PR23
		#ifdef BACDEL_SER_DM_DDA_A
			case SERVICE_SUPPORTED_WHO_AM_I:
				eCallbackType = APP_CB_WHO_AM_I;
				break;
		#endif /* DDA-A */
		#endif /* BACDEL_PR23 */
		default:
		break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: "
	"Convert_ServiceType_To_CallbackType: Exit \r\n");
	#endif
	return eCallbackType;
}

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
	uint32_t u32ErrorCode)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Set_Error_Parameters_Of_Confirmed_Req: Entry \r\n");
	#endif

	/* check pdu type */
	if( PDU_TYPE_ERROR != ePDUType && PDU_TYPE_ABORT !=  ePDUType && 
		PDU_TYPE_REJECT != ePDUType)
	{
		/* invalid pdu type, send abort */
		pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
		pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = ABORT_REASON_OTHER;
		pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
	}
	else 
	{
		/* set server flag true */
		pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
		/* check error code max limit */
		if(u32ErrorCode > UINT16_MAX)
		{
			/* save pdu type */
			pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
			/* save error code */
			pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = ABORT_REASON_OTHER;			
		}
		else
		{
			/* check if abort pdu type */
			if(PDU_TYPE_ABORT == ePDUType)
			{
				/* save pdu type */
				pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
				pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = u32ErrorCode;
			}
			/* check if reject pdu type */
			else if(PDU_TYPE_REJECT == ePDUType)
			{
				/* save pdu type */
				pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_REJECT;
				pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = u32ErrorCode;
			}
			/* check if error pdu type */
			else if(PDU_TYPE_ERROR == ePDUType)
			{
				/* check max limit for error class */
				if(eErrorClass > UINT16_MAX)
				{
					/* save pdu type */
					pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
					/* save error code */
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = ABORT_REASON_OTHER;
				}
				else
				{
					/* save pdu type */
					pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ERROR;
					/* save error class and code */
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = u32ErrorCode;
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass = eErrorClass;
				}
			}
		}
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Set_Error_Parameters_Of_Confirmed_Req: Exit \r\n");
	#endif
}


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
    bool bServer)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Service_Error_Handler: Entry \r\n");
	#endif

	/* set the server or client flag */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = bServer;

	/* check PDU type */
    if (i32PDUType == BACNET_STATUS_ERROR) 
    {
		/* set parameters for error response */
        pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ERROR;
        pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass = eErrorClass;
        pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = eErrorCode;
    }
    else if (i32PDUType == BACNET_STATUS_ABORT)  
    {
		/* set parameters for abort response */
        pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
        pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
			Abort_Convert_Error_Code(eErrorCode);
    }
    else if (i32PDUType == BACNET_STATUS_REJECT )
    {
		/* set parameters for reject response */
        pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_REJECT;
        pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
			Reject_Convert_Error_Code(eErrorCode);
    }
    else
	{
		; // dummy statement
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: Service_Error_Handler: \
		Undefined PDU type \r\n");
		#endif
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Service_Error_Handler: Exit \r\n");
	#endif
}

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
	DESTINATION_TYPE eDestType)
{
	/* local variables */
	bacnetip_arguments_t stServiceArgs = {0};
	virtualDevData_t *pstDeviceStruct = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Send_I_AM : Entry \r\n");
	#endif

    /* NULL check the pointer */
    if(NULL == pvVirtualDev)
    {
        return;
    }
	pstDeviceStruct = (virtualDevData_t *)pvVirtualDev;

    /* Assign all parameters required to send I-AM */
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_eServiceSupport =
        SERVICE_SUPPORTED_I_AM;
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stI_AM_Request.
		m_u32DeviceInstance = pstDeviceStruct->m_stDevObject.m_stObjectID.
        m_u32ObjId;
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stI_AM_Request.
		m_u32VendorID = pstDeviceStruct->m_stDevObject.m_stVendorId.m_u16Val;
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stI_AM_Request.
		m_u16MaxAPDUlengthAccepted = pstDeviceStruct->m_stDevObject.
        m_stMaxAPDULenAccepted.m_u16Val;
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stI_AM_Request.
		m_eSegmentationSupported = pstDeviceStruct->m_stDevObject.
        m_stSegmentationSupport.m_eSegmentationSupport;

    /* Set destination type */
    stServiceArgs.m_stNPDUData.m_eDestinationType = eDestType;

    /* save device id */
    stServiceArgs.m_u32DeviceID = pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId;

    /* Set Destination AS Global Broadcast */

    {
    	MSTP_GetBroadcastMacId(eDestType, &stServiceArgs.m_stDestBACnetAddr);
	}

	if(bIsNwLayerMsg)
	{
		/* Network Layer message update the values. */
		stServiceArgs.m_stNPDUData.m_bIsNwLayerMsg = TRUE;
		stServiceArgs.m_stNPDUData.m_eMsgType =
			NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK;
	}

    /* Generate the request */
    BACDEL_Generate_Aside_Request(pstDeviceStruct->m_u8DvSADR, &stServiceArgs, NULL);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Send_I_AM : Exit \r\n");
	#endif
}
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
    DESTINATION_TYPE eDestType)
{
    /* local variables */
    initiator_response_t stInitRet = {0};
	bacnetip_arguments_t stServiceArgs = {0};
	virtualDevData_t *pstDeviceStruct = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Send_WHO_IS : Entry \r\n");
	#endif

    /* If NULL Send whois from 1st Host device */
	pstDeviceStruct = (virtualDevData_t *)pvVirtualDev;
    if(NULL == pstDeviceStruct)
        pstDeviceStruct = gstHostDevice.m_pstDeviceStruct;

    /* Assign all parameters required to send WHO-IS */
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stWHO_IS_Request.
        m_i32DevRangeLowLimit = i32DevRangeLowLimit;
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stWHO_IS_Request.
        m_i32DevRangeHighLimit = i32DevRangeHighLimit;
    stServiceArgs.m_stNPDUData.m_stAPDUData.m_eServiceSupport =
        SERVICE_SUPPORTED_WHO_IS;

    /* Set destination type */
    stServiceArgs.m_stNPDUData.m_eDestinationType = eDestType;

    /* save device id */
    stServiceArgs.m_u32DeviceID = pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId;

    /* Set Destination AS Global Broadcast */

	/* Set broadcast MSTP address */
	MSTP_GetBroadcastMacId(eDestType, &stServiceArgs.m_stDestBACnetAddr);

	if(bIsNwLayerMsg)
	{
		/* Network Layer message update the values. */
		stServiceArgs.m_stNPDUData.m_bIsNwLayerMsg = TRUE;
		stServiceArgs.m_stNPDUData.m_eMsgType =
			NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK;
		stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_u16RtrNetNumber = (uint16_t)u32NetworkNo;
	}

    /* Generate the request */
    stInitRet = BACDEL_Generate_Aside_Request( pstDeviceStruct->m_u8DvSADR, &stServiceArgs, NULL);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Send_WHO_IS : Exit \r\n");
	#endif
	return stInitRet.m_eErrorCode;
}
#endif /* DDB_A */

/**
*                                                                    
* DESCRIPTION                                                                          
* Api to free memory allocated to save COV notification data
*    
* @param pvCovData			[in] new cov subscription data.
* @returns Void.
*	
*/
void Free_COV_Notification_Data(void *pvCovData)
{
    /* local variables */
    ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;
    CovRecipientInfo_t *pstSubscriberInfo = NULL;
    CovPropElement_t *pstCOVPropertyElem = NULL;
    void *pvTemp = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Free_COV_Notification_Data: Entry \r\n");
	#endif

    /* null check i/p pointer */
    if(NULL == pvCovData)
        return;
    else
        pstCOVSubscribe = pvCovData;

    /* get data pointers */
    pstSubscriberInfo = pstCOVSubscribe->m_pstSubscriberInfo;
    pstCOVPropertyElem = &pstCOVSubscribe->m_stCOVPropertyElem;

    /* free subscribe info */
    pstCOVSubscribe->m_pstSubscriberInfo = NULL;
    while(NULL != pstSubscriberInfo)
    {
        pvTemp = pstSubscriberInfo->m_pstNext;
        OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
        pstSubscriberInfo = pvTemp;
    }

    /* free property value of 1st property */
    if(NULL != pstCOVPropertyElem->m_pvStoredValue)
        OSAL_Free(pstCOVPropertyElem->m_pvStoredValue, __FILE__, __FUNCTION__, __LINE__);
    pstCOVPropertyElem->m_pvStoredValue = NULL;

    /* free property related data */
    pstCOVPropertyElem = pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext;
    while(NULL != pstCOVPropertyElem)
    {
        /* save next */
        pvTemp = pstCOVPropertyElem->m_pstNext;
        /* free property value */
        if(NULL != pstCOVPropertyElem->m_pvStoredValue)
            OSAL_Free(pstCOVPropertyElem->m_pvStoredValue, __FILE__, __FUNCTION__, __LINE__);
        pstCOVPropertyElem->m_pvStoredValue = NULL;
        /* free property structure data */
        OSAL_Free(pstCOVPropertyElem, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstCOVPropertyElem = pvTemp;
    }
    pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext = NULL;

    /* free COV notification data itself */
    OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Free_COV_Notification_Data: Exit\r\n");
	#endif

    return;
}

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
void GEI_Free_Ack_Data(event_info_response_t *pstEventInfoData)
{
	/* local variables */
	event_summary_t *pstTemp = NULL;
	event_summary_t *pstNext = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "GEI_Free_Ack_Data: entry \r\n");
	#endif

	/* null check for inpuit parameters */
	if(NULL == pstEventInfoData)
	{
		return;
	}

	/* get the base node of list */
	pstTemp = pstEventInfoData->m_pstEventSummaryList;

	while(NULL != pstTemp)
	{
		/* move to next node */
		pstNext = pstTemp->m_pstNextSummary;

		/* free memory */
		OSAL_Free(pstTemp,  __FILE__, __FUNCTION__, __LINE__);

		/* restore the data */
		pstTemp = pstNext;
	}

	/* free base structure */
	OSAL_Free(pstEventInfoData,  __FILE__, __FUNCTION__, __LINE__);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "GEI_Free_Ack_Data: exit \r\n");
	#endif
}
#endif /* GEI_A || GEI_B */

/*************** end of file **************/
