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
*   SoftDEL Systems Ltd.                                 india@softdel.com
*   3rd Floor, Pentagon P4,                              http://www.softdel.com
*    Magarpatta City, Hadapsar
*    Pune - 411 028       
*
*    File Name - device.c
*
*   RELEASE HISTORY
*   DATE        NAME              DESCRIPTION
*   29/06/2011  Harshal Mangale   File Created
*   29/06/2011  Harshal Mangale   property value encoding
*                                 according to its datatype
*   12-07-2011  M.Venu            1. Included Enum and Bit 
*                                    String data type cases for 
*                                    encoding application data
*   13-07-2011  M.Venu            1.Added BACNET_DT_TIMESTAMP_ARRAY
*                                   BACNET_DT_PRIORITY_ARRAY
*   22-07-2011  M.Venu            1.Updated BACNET_DT_PRIORITY_ARRAY 
*                                   for Multistate objects
*                                 2.Added BACNET_DT_UNSIGNED_ARRAY &
*                                   BACNET_DT_CHARSTRING_ARRAY Encode Tag
*   26-07-2011  M.Venu            Updated BACApp_Decode_Application_Data 
*                                 function for multiple Tag Support
*   27-07-2011  M.Venu            Encoding logic changed for Bit String,
*                                 List of Unsigned and Character strings
*   28/07/2011  M.Venu            Input parameter modified to 
*                                 allow array index of property
*                                 in BACApp_Encode_Data_Type function
*   03/08/2011  M.Venu            Modified encoding logic for Bit string,
*                                 List of unsigned and list of char
*                                 data type properties
*   04/08/2011  M.Venu            Encoding done as per change in structure 
*                                 Event time stamp deta type
*   05/08/2011  M.Venu            Supported Encoding for EventTimeStamp 
*                                 property with
*   30/08/2011  M.Venu            Resolved bug fix of changing outofsevice
*                                 property value
*   20/09/2011  M.Venu            Encoding for BACNET_DT_UNSIGNED16 
*                                 data type done with unsigned 16 data type
*   23/09/2011  Ashish Verma      Modified BACNET_CHARACTER_STRING
*                                 structure to Pr_BACnetCharStr_t in 
*                                 BACApp_Encode_Data_Type().    
*   27/09/2011  Ashish Verma      Modified ConvertInStr_To_AppTag
*                                 function to fix defect 23 of release2 
*   11/10/2011  Ashish Verma      Added BACApp_Decode_COV_data() to decode 
*                                 read ack frame for ActiveCOVSubscription 
*                                 property.
*   04/09/2012  Heramb Joshi      Added BACApp_Decode_Context_Data() for Context 
*                                 specific property decode.
*	03/03/2020  Pranav Phadatare  Adding support to Fault Algorithm- Out of Range
*								  Modified fucntions-
*								  Encode_Notification_Parameters()
*
******************************************************************************/
#include "bacDELDef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pduEncodeDecode.h"
#include "pduEncodeDecodeDT.h"
#include "pduDataType.h"
#include "pduDataEncodeDecode.h"
#include "propertyValidations.h"
#include "objDevice.h"
#include "miscMiscellaneous.h"
#include "propertyClearValues.h"
#include "propertyValueRead.h"
#include "propertyValueWrite.h"


/** Global variable for Device Local Network No. */
extern uint16_t g_u16LocalNWNo;


/** @file bacapp.c  Utilities for the Bacnet_Property_Value */

int32_t BACApp_Encode_Application_Data(
    uint8_t * apdu,
    BACNET_PROPERTY_VALUE * value)
{
    int32_t i32APDU_Len = 0;   /* total length of the apdu, return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_Application_Data: Entry \r\n");
	#endif

    if (value && apdu) {
        switch (value->m_TagType) {
#if defined (BACAPP_NULL)
            case BACNET_APPLICATION_TAG_NULL:
                apdu[0] = value->m_TagType;
                i32APDU_Len++;
                break;
#endif
#if defined (BACAPP_BOOLEAN)
            case BACNET_APPLICATION_TAG_BOOLEAN:
                i32APDU_Len =
                    Encode_Application_Boolean(&apdu[0], value->uValue.m_Boolean);
                break;
#endif
#if defined (BACAPP_UNSIGNED)
            case BACNET_APPLICATION_TAG_UNSIGNED_INT:
                i32APDU_Len =
                    Encode_Application_Unsigned(&apdu[0],
                    value->uValue.m_Unsigned_Int);
                break;
#endif
#if defined (BACAPP_SIGNED)
            case BACNET_APPLICATION_TAG_SIGNED_INT:
                i32APDU_Len =
                    Encode_Application_Signed(&apdu[0],
                    value->uValue.m_Signed_Int);
                break;
#endif
#if defined (BACAPP_REAL)
            case BACNET_APPLICATION_TAG_REAL:
                i32APDU_Len = Encode_Application_Real(&apdu[0], value->uValue.m_Real);
                break;
#endif
#if defined (BACAPP_DOUBLE)
            case BACNET_APPLICATION_TAG_DOUBLE:
                i32APDU_Len =
                    Encode_Application_Double(&apdu[0], value->uValue.m_Double);
                break;
#endif
#if defined (BACAPP_OCTET_STRING)
            case BACNET_APPLICATION_TAG_OCTET_STRING:
                i32APDU_Len =
                    Encode_Application_Octet_String(&apdu[0],
                    &value->uValue.m_stOctet_String);
                break;
#endif
#if defined (BACAPP_CHARACTER_STRING)
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
                i32APDU_Len =
                    Encode_Application_Character_String(&apdu[0],
                    &value->uValue.m_stCharacter_String);
                break;
#endif
#if defined (BACAPP_BIT_STRING)
            case BACNET_APPLICATION_TAG_BIT_STRING:
                i32APDU_Len =
                    Encode_Application_BITstring(&apdu[0],
                    &value->uValue.m_stBit_String);
                break;
#endif
#if defined (BACAPP_ENUMERATED)
            case BACNET_APPLICATION_TAG_ENUMERATED:
                i32APDU_Len =
                    Encode_Application_Enumerated(&apdu[0],
                    value->uValue.m_Enumerated);
                break;
#endif
#if defined (BACAPP_DATE)
            case BACNET_APPLICATION_TAG_DATE:
                i32APDU_Len =
                    Encode_Application_Date(&apdu[0], &value->uValue.m_stDate);
                break;
#endif
#if defined (BACAPP_TIME)
            case BACNET_APPLICATION_TAG_TIME:
                i32APDU_Len =
                    Encode_Application_Time(&apdu[0], &value->uValue.m_stTime);
                break;
#endif
#if defined (BACAPP_OBJECT_ID)
            case BACNET_APPLICATION_TAG_OBJECT_ID:
                i32APDU_Len =
                    Encode_Application_Object_Id(&apdu[0],
					(int32_t) value->uValue.m_stObject_Id.m_eObjType,
                    value->uValue.m_stObject_Id.m_u32ObjInstance);
                break;
#endif
            case BACNET_APPLICATION_TAG_LISTUNSIGN:
                while(NULL != value)
                {
                    i32APDU_Len += Encode_Application_Unsigned(&apdu[i32APDU_Len], 
                        value->uValue.m_Unsigned_Int);
                    value = value->pstNextPropVal;
                }
                break;
            case BACNET_APPLICATION_TAG_LISTCHAR:
                while(NULL != value)
                {
                    i32APDU_Len += 
                        Encode_Application_Character_String(&apdu[i32APDU_Len],
                        &value->uValue.m_stCharacter_String);
                    value = value->pstNextPropVal;
                }
                break;
            default:
                break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_Application_Data: Exit \r\n");
	#endif

    return i32APDU_Len;
}

/* decode the data and store it into value.
   Return the number of octets consumed. */
int32_t BACApp_Decode_Data(
    uint8_t * apdu,
    uint8_t eTag_Data_Type,
    uint32_t i32Len_Value_Type,
    BACNET_PROPERTY_VALUE * value)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Data: Entry \r\n");
	#endif

    if (apdu && value) 
    {
        switch (eTag_Data_Type) 
        {
#if defined (BACAPP_NULL)
            case BACNET_APPLICATION_TAG_NULL:
                /* nothing else to do */
                break;
#endif
#if defined (BACAPP_BOOLEAN)
            case BACNET_APPLICATION_TAG_BOOLEAN:
                value->uValue.m_Boolean = Decode_Boolean(i32Len_Value_Type);
                break;
#endif
#if defined (BACAPP_UNSIGNED)
            case BACNET_APPLICATION_TAG_UNSIGNED_INT:
                i32Len =
                    Decode_Unsigned(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_Unsigned_Int);
                break;
#endif
#if defined (BACAPP_SIGNED)
            case BACNET_APPLICATION_TAG_SIGNED_INT:
                i32Len =
                    Decode_Signed(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_Signed_Int);
                break;
#endif
#if defined (BACAPP_REAL)
            case BACNET_APPLICATION_TAG_REAL:
                i32Len =
                    Decode_Real_Safe(&apdu[0], i32Len_Value_Type,
                    &(value->uValue.m_Real));
                break;
#endif
#if defined (BACAPP_DOUBLE)
            case BACNET_APPLICATION_TAG_DOUBLE:
                i32Len =
                    Decode_Double_Safe(&apdu[0], i32Len_Value_Type,
                    &(value->uValue.m_Double));
                break;
#endif
#if defined (BACAPP_OCTET_STRING)
            case BACNET_APPLICATION_TAG_OCTET_STRING:
                i32Len =
                    Decode_OCTET_String(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_stOctet_String);
                break;
#endif
#if defined (BACAPP_CHARACTER_STRING)
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
                i32Len =
                    Decode_Character_String(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_stCharacter_String);
                break;
#endif
#if defined (BACAPP_BIT_STRING)
            case BACNET_APPLICATION_TAG_BIT_STRING:
                i32Len =
                    Decode_BITstring(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_stBit_String);
                break;
#endif
#if defined (BACAPP_ENUMERATED)
            case BACNET_APPLICATION_TAG_ENUMERATED:
                i32Len =
                    Decode_Enumerated(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_Enumerated);
                break;
#endif
#if defined (BACAPP_DATE)
            case BACNET_APPLICATION_TAG_DATE:
                i32Len =
                    Decode_Date_Safe(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_stDate);
                break;
#endif
#if defined (BACAPP_TIME)
            case BACNET_APPLICATION_TAG_TIME:
                i32Len =
                    Decode_Time_Safe(&apdu[0], i32Len_Value_Type,
                    &value->uValue.m_stTime);
                break;
#endif
#if defined (BACAPP_OBJECT_ID)
            case BACNET_APPLICATION_TAG_OBJECT_ID:
                {
                    uint32_t u32object_type = 0;
                    uint32_t u32instance = 0;
                    i32Len =
                        Decode_Object_Id_Safe(&apdu[0], i32Len_Value_Type,
                        &u32object_type, &u32instance);
					value->uValue.m_stObject_Id.m_eObjType = (BACNET_OBJECT_TYPE)u32object_type;
                    value->uValue.m_stObject_Id.m_u32ObjInstance = u32instance;
                }
                break;
#endif
            default:
                break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Data: Exit \r\n");
	#endif

    return i32Len;
}


/** function to check if property datatype has 2 closing tags at the end of property value */
bool BACApp_Decode_Check_Property(BACNET_PROPERTY_ID  ePropID,
    BACNET_OBJECT_TYPE  eObjType)
{
	/* local variables */
	bool bReturnVal = false;

    switch(ePropID)
    {
        case PROP_EVENT_PARAMETERS : 
        case PROP_LOG_BUFFER:
        	bReturnVal = TRUE;
        	break;

        case PROP_PRESENT_VALUE:
            switch(eObjType)
            {
                case OBJECT_GROUP:
                	bReturnVal = TRUE;
                	break;
                default :
                	bReturnVal = FALSE;
                	break;
            }
            break;

        default:
        	bReturnVal = FALSE;
        	break;
    }

    return bReturnVal;
}

/**
*DESCRIPTION
*   This API decodes the application data & return the data value.
*
*@param apdu [in ] Application data containg Tag Number, Clss & Length/Type
*                  value.
*@param max_apdu_len [in] Application data lenght 
*@param value [out] Application data decoded value.
*
*@return i32Len Decoded data length.
*
*/
int32_t BACApp_Decode_Application_Data(
    uint8_t * apdu,
    uint32_t u32Max_Apdu_Len,
    BACNET_PROPERTY_VALUE * Avalue,
    BACNET_PROPERTY_ID  ePropID,
    BACNET_OBJECT_TYPE  eObjType)
{
    int32_t i32Len = 0;
    int32_t i32Tag_Len = 0;
    int32_t i32Decode_Len = 0;
    uint8_t i32Tag_Number = 0;
    uint32_t i32Len_Value_Type = 0;
    uint32_t u32MaxDataLen = 0;
    uint8_t u8Opening_Tag_Cnt = 0;
    // array size is dependent on max value that u8Opening_Tag_Cnt can hold
	int8_t i8Opening_Tag_Number[256] = {-1,0,0,0,0}; 
    BACNET_PROPERTY_VALUE   *psValue = NULL;
    BACNET_APPLICATION_TAG eDataTAG = MAX_BACNET_APPLICATION_TAG;
    int8_t i8Index = -1;
    bool bMallocDone = true;
    int8_t i8Value = -1;
    bool bIsUnsigned = false; /* used for BACnetAuthenticationPolicy */

	/* check input pointers */
    if(NULL == apdu  || NULL == Avalue )
    {
        return -1;
    }

	psValue = Avalue;
    psValue->pstNextPropVal = NULL;
    u32MaxDataLen = u32Max_Apdu_Len;   

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Application_Data: Entry \r\n");
	#endif

    while(u32Max_Apdu_Len)
    {
        if (IS_OPENING_TAG(apdu[i32Len])) 
        {
            i32Tag_Len = Decode_Tag_Number_And_Value(&apdu[i32Len], &i32Tag_Number,
                        &i32Len_Value_Type);

            /* Update opening Tag to notify Start of Property Value, 
                Increment Count of Opening TAG */
            u8Opening_Tag_Cnt++;
			i8Opening_Tag_Number[u8Opening_Tag_Cnt] = i32Tag_Number;

            /* Special handling for weekly schedule property & action prop for command object */
            if(ePropID == PROP_WEEKLY_SCHEDULE)
                if(!IS_CLOSING_TAG(apdu[i32Len+1]) /*&& (i8Index >= 0)*/ && !bMallocDone)
                {
                    psValue->pstNextPropVal = OSAL_Malloc(sizeof(BACNET_PROPERTY_VALUE),  
                        __FILE__, __FUNCTION__, __LINE__);
                    if(psValue->pstNextPropVal == NULL)
                        u32Max_Apdu_Len = 0; /* To make Break from loop */
                    else
                        psValue = psValue->pstNextPropVal;
                }
                else;
            /* Handling for Event Parameter property, save type of event */
            else if(PROP_EVENT_PARAMETERS == ePropID && -1 == i8Index)
                i8Value = i32Tag_Number;
            else;

            i8Index++;
            /* do not increment if tag is 4 for action prop in command object */
            if(ePropID == PROP_ACTION && eObjType == OBJECT_COMMAND && 4 == i32Tag_Number)
                i8Index--;
            psValue->m_StartTag = i8Opening_Tag_Number[u8Opening_Tag_Cnt];

            i32Len += i32Tag_Len;
            u32Max_Apdu_Len -= i32Tag_Len;
        } 
        else if (IS_CLOSING_TAG(apdu[i32Len])) 
        {
            i32Tag_Len = Decode_Tag_Number_And_Value(&apdu[i32Len], &i32Tag_Number,
                        &i32Len_Value_Type);

            /* Reset opening Tag to notify End of Property Value, 
                Decrement Count of Opening TAG */
                u8Opening_Tag_Cnt--;

            i32Len += i32Tag_Len;
            u32Max_Apdu_Len -= i32Tag_Len;

            if(PROP_ACTION == ePropID && OBJECT_COMMAND == eObjType)
            {            
                if(IS_OPENING_TAG(apdu[i32Len]) && 
				IS_OPENING_TAG(apdu[i32Len-2]))
                {
				    psValue->m_StartTag = i8Index;
                    psValue->pstNextPropVal = OSAL_Malloc(sizeof(BACNET_PROPERTY_VALUE), 
                            __FILE__, __FUNCTION__, __LINE__);
                    if(psValue->pstNextPropVal == NULL)
                        u32Max_Apdu_Len = 0; /* To make Break from loop */
                    else
                        psValue = psValue->pstNextPropVal;
                    bMallocDone = true;
                }
                else if(!u32Max_Apdu_Len && IS_OPENING_TAG(apdu[i32Len-2]) && 
				IS_CLOSING_TAG(apdu[i32Len-1]))
                {
				    psValue->m_StartTag = i8Index;
                }
            }
        } 
        else if(apdu && psValue) /* Decode Data */
        {
			/* Save start tag */
			psValue->m_StartTag = i8Opening_Tag_Number[u8Opening_Tag_Cnt];

            if(PROP_EVENT_PARAMETERS == ePropID && 1 == i8Index && TAG_NO_13 == i8Value)
                psValue->m_StartTag = i8Value;

            /* For Weekly schedule property start tag represents array index value */
            if(ePropID == PROP_WEEKLY_SCHEDULE || 
              (ePropID == PROP_ACTION && eObjType == OBJECT_COMMAND && TAG_NO_4 != psValue->m_StartTag))
                psValue->m_StartTag = i8Index;
            /* For log buffer - in TLM update the tag count value */
            else if(PROP_LOG_BUFFER == ePropID)
            {
                /* save index value & reset */
                i8Value = i8Index;
                i8Index = 0;
            }

            /* Get the Context Tag Number & Length of Data */
            i32Tag_Len =
                Decode_Tag_Number_And_Value(&apdu[i32Len], &i32Tag_Number,
                    &i32Len_Value_Type);

            if(IS_CONTEXT_SPECIFIC(apdu[i32Len]) && i32Tag_Len) 
            {
                /* context-specific tagged data */
                psValue->m_ContextSpecific = true;
                psValue->m_ContextTag = i32Tag_Number;

                eDataTAG = BACApp_Prop_Context_Tag(ePropID, eObjType, i32Tag_Number, 
                    psValue->m_StartTag, i8Value);

                /* Specail handling for BACnetAuthenticationPolicy */
                if(PROP_AUTHENTICATION_POLICY_LIST == ePropID)
                {
                    if(1 == psValue->m_ContextTag && BACNET_APPLICATION_TAG_OBJECT_ID == eDataTAG && false == bIsUnsigned)
                        bIsUnsigned = true;
                    else if(1 == psValue->m_ContextTag && BACNET_APPLICATION_TAG_OBJECT_ID == eDataTAG && true == bIsUnsigned)
                    {
                        bIsUnsigned = false;
                        eDataTAG = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    }
                }
            } 
            else if(!IS_CONTEXT_SPECIFIC(apdu[i32Len]) && i32Tag_Len)
            {
                /* Application Sepcific Tag Data */
                psValue->m_ContextSpecific = false;
                psValue->m_ContextTag = 0;

                eDataTAG = i32Tag_Number;
            }

            i32Len += i32Tag_Len;

			/* save tag type */
            psValue->m_TagType = eDataTAG;

            /* Decode and save Data as per Application Tag */
            i32Decode_Len =
                BACApp_Decode_Data(&apdu[i32Len], eDataTAG, i32Len_Value_Type,
                    psValue);

            if(i32Decode_Len)
            {
                i32Len += i32Decode_Len;
            } 
            else if(eDataTAG == BACNET_APPLICATION_TAG_NULL ||
                    eDataTAG == BACNET_APPLICATION_TAG_BOOLEAN)
            {
				/* special handling for context specific boolean tags */
				if(psValue->m_ContextSpecific == true &&
					eDataTAG == BACNET_APPLICATION_TAG_BOOLEAN)
				{
					u32Max_Apdu_Len--;
					psValue->uValue.m_Boolean = apdu[i32Len++];
				}
                /* Nothing to do */
                psValue->m_TagType = eDataTAG;
            }
            else if((eDataTAG == BACNET_APPLICATION_TAG_CHARACTER_STRING ||
                    eDataTAG == BACNET_APPLICATION_TAG_OCTET_STRING ||
                    eDataTAG == BACNET_APPLICATION_TAG_BIT_STRING) 
                    && i32Decode_Len == 0
                    && i32Len_Value_Type != 0)
            {
                /* -ve Datalen means Error in decoding */
                i32Len = -1;
                u32Max_Apdu_Len = 0; /* To make Break from loop */
            }
            /* do nothing since nothing was present to decode */
            else if((eDataTAG == BACNET_APPLICATION_TAG_CHARACTER_STRING ||
                    eDataTAG == BACNET_APPLICATION_TAG_OCTET_STRING ||
                    eDataTAG == BACNET_APPLICATION_TAG_BIT_STRING) 
                    && i32Decode_Len == 0
                    && i32Len_Value_Type == 0)
			{
				;/* do nothing */
			}
            else
            {
                psValue->m_TagType = MAX_BACNET_APPLICATION_TAG;
            }

            /* Calculate length of data to be decoded */
            u32Max_Apdu_Len -= (i32Tag_Len + i32Decode_Len);

            if((int32_t)u32Max_Apdu_Len < 0 || u32Max_Apdu_Len > u32MaxDataLen)
            {
                /* -ve Datalen means Error in decoding */
                i32Len = -1;
                u32Max_Apdu_Len = 0; /* To make Break from loop */
            }
            else if(u32Max_Apdu_Len > 0 && (psValue->m_TagType != MAX_BACNET_APPLICATION_TAG) )
            {
				/* For Last byte having closing TAG no need to MAlloc */
				if(u32Max_Apdu_Len == 1 && IS_CLOSING_TAG(apdu[i32Len]))
				{	
					; /* do nothing */
				}
				else if(u32Max_Apdu_Len == 2 && 
                    BACApp_Decode_Check_Property(ePropID,eObjType) &&
                    IS_CLOSING_TAG(apdu[i32Len]))
				{
					;/* do nothing */
				}
                else if(u32Max_Apdu_Len == 3 && (ePropID == PROP_EVENT_PARAMETERS) &&
                    (IS_OPENING_TAG(apdu[i32Len]) || IS_CLOSING_TAG(apdu[i32Len])))
				{
					;/* do nothing */
				}
                else if(u32Max_Apdu_Len == 3 && (ePropID == PROP_LOG_BUFFER) && 
                    IS_CLOSING_TAG(apdu[i32Len]))
				{
					;/* do nothing */
				}
                else if((ePropID == PROP_WEEKLY_SCHEDULE) && 
                    IS_OPENING_TAG(apdu[i32Len+1]) && IS_CLOSING_TAG(apdu[i32Len+2]) &&
                    IS_CLOSING_TAG(apdu[i32Len]))
                    bMallocDone = false;
				else
				{
                    /* allocate memory for next data to be decoded */
					psValue->pstNextPropVal = OSAL_Malloc(sizeof(BACNET_PROPERTY_VALUE), 
                        __FILE__, __FUNCTION__, __LINE__);
	                if(psValue->pstNextPropVal == NULL)
	                    u32Max_Apdu_Len = 0; /* To make Break from loop */
	                else
	                    psValue = psValue->pstNextPropVal;
                    bMallocDone = true;
				}
            }
        }
        else
        {
            u32Max_Apdu_Len--;// Error packet
        }
	}// end of while loop

    psValue->pstNextPropVal = NULL;

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Application_Data: Exit \r\n");
	#endif

    return i32Len;
}

/* Decode the data and
   return the number of octets consumed. */
int32_t BACApp_Decode_Data_Len(
    uint8_t * apdu,
    uint8_t eTag_Data_Type,
    uint32_t i32Len_Value_Type)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Data_Len: Entry \r\n");
	#endif

    if (apdu) {
        switch (eTag_Data_Type) {
            case BACNET_APPLICATION_TAG_NULL:
                break;
            case BACNET_APPLICATION_TAG_BOOLEAN:
                break;
            case BACNET_APPLICATION_TAG_UNSIGNED_INT:
            case BACNET_APPLICATION_TAG_SIGNED_INT:
            case BACNET_APPLICATION_TAG_REAL:
            case BACNET_APPLICATION_TAG_DOUBLE:
            case BACNET_APPLICATION_TAG_OCTET_STRING:
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
            case BACNET_APPLICATION_TAG_BIT_STRING:
            case BACNET_APPLICATION_TAG_ENUMERATED:
            case BACNET_APPLICATION_TAG_DATE:
            case BACNET_APPLICATION_TAG_TIME:
            case BACNET_APPLICATION_TAG_OBJECT_ID:
                i32Len = (int32_t) i32Len_Value_Type;
                break;
            default:
                break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Data_Len: Exit \r\n");
	#endif

    return i32Len;
}

int32_t BACApp_Decode_Application_Data_Len(
    uint8_t * apdu,
    uint32_t max_apdu_len)
{
    int32_t i32Len = 0;
    int32_t i32Tag_Len = 0;
    int32_t i32Decode_Len = 0;
    uint8_t i32Tag_Number = 0;
    uint32_t i32Len_Value_Type = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Application_Data_Len: Entry \r\n");
	#endif

    /* FIXME: use max_apdu_len! */
    //max_apdu_len = max_apdu_len;
    if (apdu && !IS_CONTEXT_SPECIFIC(*apdu)) 
    {
        i32Tag_Len = Decode_Tag_Number_And_Value(&apdu[0], &i32Tag_Number,
            &i32Len_Value_Type);
        if (i32Tag_Len) 
        {
            i32Len += i32Tag_Len;
            i32Decode_Len =  BACApp_Decode_Data_Len(&apdu[i32Len], i32Tag_Number, 
                                i32Len_Value_Type);
            i32Len += i32Decode_Len;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Application_Data_Len: Exit \r\n");
	#endif

    return i32Len;
}

int32_t BACApp_Encode_Context_Data_Value(
    uint8_t * apdu,
    uint8_t context_tag_number,
    BACNET_PROPERTY_VALUE * psValue)
{
    int32_t i32APDU_Len = 0;   /* total length of the apdu, return psValue */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_Context_Data_Value: Entry \r\n");
	#endif

    if (psValue && apdu) {
        switch (psValue->m_TagType) {
#if defined (BACAPP_NULL)
            case BACNET_APPLICATION_TAG_NULL:
                i32APDU_Len = Encode_Context_Null(&apdu[0], context_tag_number);
                break;
#endif
#if defined (BACAPP_BOOLEAN)
            case BACNET_APPLICATION_TAG_BOOLEAN:
                i32APDU_Len =
                    Encode_Context_Boolean(&apdu[0], context_tag_number,
                    psValue->uValue.m_Boolean);
                break;
#endif
#if defined (BACAPP_UNSIGNED)
            case BACNET_APPLICATION_TAG_UNSIGNED_INT:
                i32APDU_Len =
                    Encode_Context_Unsigned(&apdu[0], context_tag_number,
                    psValue->uValue.m_Unsigned_Int);
                break;
#endif
#if defined (BACAPP_SIGNED)
            case BACNET_APPLICATION_TAG_SIGNED_INT:
                i32APDU_Len =
                    Encode_Context_Signed(&apdu[0], context_tag_number,
                    psValue->uValue.m_Signed_Int);
                break;
#endif
#if defined (BACAPP_REAL)
            case BACNET_APPLICATION_TAG_REAL:
                i32APDU_Len =
                    Encode_Context_Real(&apdu[0], context_tag_number,
                    psValue->uValue.m_Real);
                break;
#endif
#if defined (BACAPP_DOUBLE)
            case BACNET_APPLICATION_TAG_DOUBLE:
                i32APDU_Len =
                    Encode_Context_Double(&apdu[0], context_tag_number,
                    psValue->uValue.m_Double);
                break;
#endif
#if defined (BACAPP_OCTET_STRING)
            case BACNET_APPLICATION_TAG_OCTET_STRING:
                i32APDU_Len =
                    Encode_Context_Octet_String(&apdu[0], context_tag_number,
                    &psValue->uValue.m_stOctet_String);
                break;
#endif
#if defined (BACAPP_CHARACTER_STRING)
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
                i32APDU_Len =
                    Encode_Context_Character_String(&apdu[0],
                    context_tag_number, &psValue->uValue.m_stCharacter_String);
                break;
#endif
#if defined (BACAPP_BIT_STRING)
            case BACNET_APPLICATION_TAG_BIT_STRING:
                i32APDU_Len =
                    Encode_Context_BITString(&apdu[0], context_tag_number,
                    &psValue->uValue.m_stBit_String);
                break;
#endif
#if defined (BACAPP_ENUMERATED)
            case BACNET_APPLICATION_TAG_ENUMERATED:
                i32APDU_Len =
                    Encode_Context_Enumerated(&apdu[0], context_tag_number,
                    psValue->uValue.m_Enumerated);
                break;
#endif
#if defined (BACAPP_DATE)
            case BACNET_APPLICATION_TAG_DATE:
                i32APDU_Len =
                    Encode_Context_Date(&apdu[0], context_tag_number,
                    &psValue->uValue.m_stDate);
                break;
#endif
#if defined (BACAPP_TIME)
            case BACNET_APPLICATION_TAG_TIME:
                i32APDU_Len =
                    Encode_Context_Time(&apdu[0], context_tag_number,
                    &psValue->uValue.m_stTime);
                break;
#endif
#if defined (BACAPP_OBJECT_ID)
            case BACNET_APPLICATION_TAG_OBJECT_ID:
                i32APDU_Len =
                    Encode_Context_Object_Id(&apdu[0], context_tag_number,
					(int32_t) psValue->uValue.m_stObject_Id.m_eObjType,
                    psValue->uValue.m_stObject_Id.m_u32ObjInstance);
                break;
#endif
            default:
                break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_Context_Data_Value: Exit \r\n");
	#endif

    return i32APDU_Len;
}


/* returns the application tag type for context tags in log buffer property */
static BACNET_APPLICATION_TAG BACApp_Prop_Decode_LogBuffer_Tags(
    BACNET_OBJECT_TYPE eObjType,
    int8_t i32Tag_Number,
    int8_t i8Start_Tag, int8_t u8Value)
{
    /* default tag value */
    BACNET_APPLICATION_TAG eTag = MAX_BACNET_APPLICATION_TAG;

    switch(eObjType)
    {
        /* default case - invalid value received */
		default:
			break;
    }

    /* return the tag value */
    return eTag;
}

/* returns the application tag type for context tags in event parameters property */
static BACNET_APPLICATION_TAG 
    BACApp_Prop_Decode_EventParameters_Tags(
        int8_t i32Tag_Number, 
        int8_t i8Start_Tag, int8_t u8Value)
{
    /* default tag value */
    BACNET_APPLICATION_TAG eTag = MAX_BACNET_APPLICATION_TAG;

    switch(u8Value)
    {
        /* decoding for below event types */
    case EVENT_OUT_OF_RANGE:
    case EVENT_DOUBLE_OUT_OF_RANGE:
    case EVENT_SIGNED_OUT_OF_RANGE:
    case EVENT_UNSIGNED_OUT_OF_RANGE:
    case EVENT_UNSIGNED_RANGE:
	    switch (i32Tag_Number) 
        {
            /* time delay */
            case 0:
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    break;
            /* low limit */
		    case 1:
                if(5 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_REAL;
			    else if (11 == i8Start_Tag || 16 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                else if(14 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_DOUBLE;
                else if(15 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_SIGNED_INT;
                break;
            /* high limit */
		    case 2:
			    if(5 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_REAL;
			    else if (11 == i8Start_Tag || 16 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                else if(14 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_DOUBLE;
                else if(15 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_SIGNED_INT;
			    break;
            /* deadband */
		    case 3:
			    if(5 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_REAL;
                else if(14 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_DOUBLE;
			    else if (16 == i8Start_Tag || 15 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    break;
            /* ideally this is invalid case */
		    default:
                break;
	    }
        break;

        /* buffer ready */
    case EVENT_BUFFER_READY:
        /* all tags are unsigned */
        eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
        break;

        /* change of state */
    case EVENT_CHANGE_OF_STATE:
        switch(i32Tag_Number) 
        {
            /* time delay or boolean type list of value */
            case 0:
                /* note: unsigned is sent for boolean as encoding is similar */
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                break;
            /* unsigned type list of values */
            case 11:
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                break;
            /* all other list of values are enumerated */
            default:
                eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                break;
        }
        break;

        /* command failure */
    case EVENT_COMMAND_FAILURE:
        switch (i32Tag_Number)
        {
            /* time delay or object id */
            case 0:
			    if(3 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    else if(1 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
            /* property id */
		    case 1:
                eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                break;
            /* optional property array index */
		    case 2:
			    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    break;
            /* optional device id */
		    case 3:
			    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
		    default:
                break;
        }
        break;

        /* floating limit */
    case EVENT_FLOATING_LIMIT:
        switch (i32Tag_Number)
        {
            /* time delay or object id */
            case 0:
			    if(4 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    else if(1 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
            /* property id */
		    case 1:
                eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                break;
            /* low diff limit or optional property array index */
		    case 2:
			    if (4 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_REAL;
			    else if (1 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    break;
            /* high diff limit or optional device id */
		    case 3:
			    if (4 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_REAL;
			    else if (1 == i8Start_Tag)
				    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
            /* deadband */
            case 4:
                eTag = BACNET_APPLICATION_TAG_REAL;
                break;
		    default:
                break;
        }
        break;

        /* decoding for below event types */
    case EVENT_CHANGE_OF_BITSTRING:
    case EVENT_CHANGE_OF_CHARACTERSTRING:
    case EVENT_CHANGE_OF_STATUS_FLAGS:
        switch (i32Tag_Number) 
        {
            /* time delay */
            case 0:
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			    break;
            /* bitmask or status flag */ 
            case 1:
                if(0 == i8Start_Tag || 18 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_BIT_STRING;
                break;
            /* ideally this is invalid case */
		    default:
                break;
	    }
        break;

        /* change of value */
    case EVENT_CHANGE_OF_VALUE:
        switch (i32Tag_Number) 
        {
            /* time delay or bitmask */
            case 0:
                if(2 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                else if(1 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_BIT_STRING;
			    break;
            /* reff prop increment */
            case 1:
                eTag = BACNET_APPLICATION_TAG_REAL;
                break;
            /* ideally this is invalid case */
		    default:
                break;
	    }
        break;

        /* change of life safety */
    case EVENT_CHANGE_OF_LIFE_SAFETY:
        switch (i32Tag_Number) 
        {
            /* time delay or object id in mode_prop_reff */
            case 0:
                if(8 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                else if(3 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
            /* property id in mode_prop_reff */
            case 1:
                eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                break;
            /* array index in mode_prop_reff */
            case 2:
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                break;
            /* device id in mode_prop_reff */
            case 3:
                eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                break;
            /* ideally this is invalid case */
		    default:
                break;
	    }
        break;

        /* extended event type */
    case EVENT_EXTENDED:
        switch (i32Tag_Number) 
        {
            /* time delay or object id in parameters */
            case 0:
                if(9 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                else if(0 == i8Start_Tag)
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
            /* property id in parameters */
            case 1:
                eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                break;
            /* array index in parameters */
            case 2:
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                break;
            /* device id in parameters */
            case 3:
                eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                break;
            /* ideally this is invalid case */
		    default:
                break;
	    }
        break;

        /* access event */
    case EVENT_ACCESS_EVENT:
        switch (i32Tag_Number) 
        {
            /* object id or device id */
            case 0:
            case 3:
                eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
			    break;
            /* property id */
            case 1:
                eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                break;
            /* array index */
            case 2:
                eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                break; 
            /* ideally this is invalid case */ 
		    default: 
                break; 
	    } 
        break; 

        /* default - no tag data available */
    default:
        break;
    }

    /* return the tag value */
    return eTag;
}

/* returns the application tag type for context tags in present value property */
static BACNET_APPLICATION_TAG BACApp_Prop_Decode_PV_Tags(
    BACNET_OBJECT_TYPE eObjType,
    int8_t i32Tag_Number,
    int8_t i8Start_Tag, int8_t u8Value)
{
    /* default tag value */
    BACNET_APPLICATION_TAG eTag = MAX_BACNET_APPLICATION_TAG;

    switch(eObjType)
    {
        /* group object tag values */
        case OBJECT_GROUP:
            switch (i32Tag_Number)
            {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 3:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                default:
                    break;
            }                
            break;

        /* global group object tag values */
        case OBJECT_GLOBAL_GROUP:
            switch (i32Tag_Number) 
            {
                case 0:
                case 3:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 1:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                default:
                    break;
            }                
        break;

        /* credential data input object tag values */
        case OBJECT_CREDENTIAL_DATA_INPUT:
            switch (i32Tag_Number) 
            {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 1:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_OCTET_STRING;
                    break;
                default:
                    break;
            }
        break;

        /* default - no tag data available */
        default:
            break;
    }

    /* return the tag value */
    return eTag;
}

/* returns the fixed tag type for certain context tagged properties */
/*
 * @param   eProperty     [in]  Property ID.
 * @param   eObjType      [in]  Object type.
 * @param   i32Tag_Number [in]  Context tag no.
 * @param   i8Start_Tag   [in]  Start tag no.
 * @param   u8Value       [in]  Special tag value.
 *
 * @return  eTag         [out]  Application tag value.
 *
 * NOTE: for Special tag (u8Tag)
 *      1. represents event type in case of event parameters property.
 *      2. represents start tag count in case of log buffer property.    
 *
 */
BACNET_APPLICATION_TAG BACApp_Prop_Context_Tag(
    BACNET_PROPERTY_ID eProperty,
    BACNET_OBJECT_TYPE eObjType,
    uint8_t i32Tag_Number,
    uint8_t i8Start_Tag, uint8_t u8Value)
{
    /* default tag value */
    BACNET_APPLICATION_TAG eTag = MAX_BACNET_APPLICATION_TAG;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Prop_Context_Tag: Entry \r\n");
	#endif

    /* switch as per property id */
    switch (eProperty) 
    {
        case PROP_EVENT_TIME_STAMPS:
        case PROP_LAST_RESTORE_TIME:
        case PROP_TIME_OF_DEVICE_RESTART:
            switch (i32Tag_Number) {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_TIME;
                    break;
                case 1:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
				default:
                    break;
			}
            break;

		case PROP_RECIPIENT_LIST:
        case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
        case PROP_RESTART_NOTIFICATION_RECIPIENTS:
        case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
		case PROP_COVU_RECIPIENTS:
			switch (i32Tag_Number) {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
				default:
                    break;
			}
			break;

		case PROP_ACTIVE_COV_SUBSCRIPTIONS:
			switch (i32Tag_Number) {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
				case 1:
					if(0 == i8Start_Tag)
						eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
					else if (1 == i8Start_Tag)
						eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
				case 2:
					if(1 == i8Start_Tag)
						eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
					else
						eTag = BACNET_APPLICATION_TAG_BOOLEAN;
                    break;
				case 3:
					eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
				case 4:
					eTag = BACNET_APPLICATION_TAG_REAL;
                    break;
				default:
                    break;
			}
			break;

		case PROP_EVENT_PARAMETERS:
            /* get tag as per the event type */
            eTag = BACApp_Prop_Decode_EventParameters_Tags(
                i32Tag_Number, i8Start_Tag, u8Value);
			break;

		case PROP_DATE_LIST:
			switch (i32Tag_Number) {
                case 0:
					eTag = BACNET_APPLICATION_TAG_DATE;
                    break;
				case 2:
					eTag = BACNET_APPLICATION_TAG_OCTET_STRING;
					break;
				default:
                    break;
			}
			break;

		case PROP_EXCEPTION_SCHEDULE:
            switch (i32Tag_Number) {
                case 1:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 3:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                case 0:        /* calendarEntry: abstract syntax + context */
                    if(0 == i8Start_Tag)
                         eTag = BACNET_APPLICATION_TAG_DATE;
                    break;
                case 2:        /* list of BACnetTimeValue: abstract syntax */
                    if(0 == i8Start_Tag)
                         eTag = BACNET_APPLICATION_TAG_OCTET_STRING;
                    break;

                default:
                    break;
            }
            break;

        case PROP_OBJECT_PROPERTY_REFERENCE:
        case PROP_LOG_DEVICE_OBJECT_PROPERTY:
        case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
        case PROP_MANIPULATED_VARIABLE_REFERENCE:
        case PROP_CONTROLLED_VARIABLE_REFERENCE:
        case PROP_SETPOINT_REFERENCE:
        case PROP_INPUT_REFERENCE:
        case PROP_GROUP_MEMBERS: 
            switch (i32Tag_Number) {
                case 0:        /* Object ID */
                case 3:        /* Device ID */
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 1:        /* Property ID */
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 2:        /* Array index */
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                default:
                    break;
            }
            break;

        case PROP_ACTUAL_SHED_LEVEL:
        case PROP_REQUESTED_SHED_LEVEL:
        case PROP_EXPECTED_SHED_LEVEL:
            switch (i32Tag_Number) {
                case 0:
                case 1:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_REAL;
                    break;
                default:
                    break;
            }
            break;

        case PROP_ACTION:
            switch (i32Tag_Number) {
                case 0:
                case 1:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 3:
                case 5:
                case 6:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                case 7:
                case 8:
                    eTag = BACNET_APPLICATION_TAG_BOOLEAN;
                    break;
                case 4:        /* propertyValue: abstract syntax */
                default:
                    break;
            }
            break;

        case PROP_SUBORDINATE_LIST:
        case PROP_MEMBER_OF:
        case PROP_ZONE_MEMBERS:
        case PROP_DOOR_MEMBERS:
        case PROP_CREDENTIALS_IN_ZONE:
        case PROP_ENTRY_POINTS:
        case PROP_EXIT_POINTS:
        case PROP_MEMBERS:
        case PROP_CREDENTIALS:
        case PROP_ACCESS_DOORS:
        case PROP_ACCESS_EVENT_CREDENTIAL:
        case PROP_ZONE_TO:
        case PROP_ZONE_FROM:
        case PROP_LAST_CREDENTIAL_ADDED:
        case PROP_LAST_CREDENTIAL_REMOVED:
        case PROP_ACCOMPANIMENT:
        case PROP_BELONGS_TO:
        case PROP_LAST_ACCESS_POINT:
            /* BACnetARRAY[N] of BACnetDeviceObjectReference */
            switch (i32Tag_Number) {
                case 0:        /* Optional Device ID */
                case 1:        /* Object ID */
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                default:
                    break;
            }
            break;

        case PROP_LOG_BUFFER:
            /* get tag as per the object type */
            eTag = BACApp_Prop_Decode_LogBuffer_Tags(eObjType, 
                i32Tag_Number, i8Start_Tag, u8Value);
            break;

        case PROP_SCALE:
            switch (i32Tag_Number) {
                case 0:       
                    eTag = BACNET_APPLICATION_TAG_REAL;
                    break;
                case 1:
                    eTag = BACNET_APPLICATION_TAG_SIGNED_INT;
                    break;
                default:
                    break;
            }
            break;

        case PROP_PRESCALE:                 
            /* all values are unsigned */
            eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
            break;

        case PROP_LOGGING_RECORD:
            switch (i32Tag_Number) {
                case 1:
                case 2:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                 case 3:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                default:
                    break;
            }
            break;

        case PROP_LIST_OF_GROUP_MEMBERS:
            switch (i32Tag_Number) {
                case 0:
                    if (1 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    else
                        eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 1:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                default:
                    break;
            }
            break;

        case PROP_PRESENT_VALUE:
            /* get the tag values as per object type */
            eTag = BACApp_Prop_Decode_PV_Tags(eObjType, i32Tag_Number, 
                i8Start_Tag, u8Value);
            break;

        case PROP_SUPPORTED_FORMATS:
        {
            switch (i32Tag_Number) 
            {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 1:
                case 2:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                default:
                    break;
            }
        }
        break;

        case PROP_AUTHENTICATION_FACTORS:
        case PROP_ACCESS_EVENT_AUTHENTICATION_FACTOR:
        {
            switch (i32Tag_Number) 
            {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                case 1:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_OCTET_STRING;
                    break;
                default:
                    break;
            }
        }
        break;

        case PROP_ASSIGNED_ACCESS_RIGHTS:
            switch (i32Tag_Number) 
            {
                case 0:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                case 1:
                    if(0 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    else
                        eTag = BACNET_APPLICATION_TAG_BOOLEAN;
                    break;
                default:
                    break;
            }
        break;

        case PROP_POSITIVE_ACCESS_RULES:
        case PROP_NEGATIVE_ACCESS_RULES:
            switch (i32Tag_Number) {
                /* object id in time_range or location & enum for time_range_specifier */
                case 0:
                    if(1 == i8Start_Tag || 3 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    else
                        eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                /* prop id in time range or location */ 
                case 1:    
                    if(1 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    else if(3 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                /* array index in time_range or location & enum for location_specifier */
                case 2:
                    if(1 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    else
                        eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                /* object id in time_range */
                case 3:
                    if(1 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                /* enable */
                case 4:
                    eTag = BACNET_APPLICATION_TAG_BOOLEAN;
                    break;
                /* invalid tag */
                default:
                    break;
            }
            break;

        case PROP_NETWORK_ACCESS_SECURITY_POLICIES:
            switch (i32Tag_Number) {
                /* port id */
                case 0:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                /* security level */ 
                case 1:
                    eTag = BACNET_APPLICATION_TAG_ENUMERATED;
                    break;
                /* invalid tag */
                default:
                    break;
            }
            break;

        case PROP_KEY_SETS:
            switch (i32Tag_Number) {
                /* key revision & algorithm */
                case 0:
                /* key id */
                case 1:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
                    break;
                /* invalid tag */
                default:
                    break;
            }
            break;

        case PROP_AUTHENTICATION_POLICY_LIST :
            switch (i32Tag_Number) {
                /* key revision & algorithm */
                case 0:
                    eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    break;
                /* key id */
                case 1:
                    if(0 == i8Start_Tag)
                        eTag = BACNET_APPLICATION_TAG_OBJECT_ID;
                    else
                        eTag = BACNET_APPLICATION_TAG_BOOLEAN;
                    break;
                case 2:
                    eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT; 
                    break; 
                /* invalid tag */ 
                default: 
                    break; 
            } 
            break;

		case PROP_UPDATE_TIME:
		case PROP_ACCESS_EVENT_TIME:
			{
				if(eObjType == OBJECT_CREDENTIAL_DATA_INPUT || 
					(eObjType == OBJECT_ACCESS_POINT &&
					eProperty == PROP_ACCESS_EVENT_TIME))
				{
					switch (i32Tag_Number) {
					/* key revision & algorithm */
					case 0:
						eTag = BACNET_APPLICATION_TAG_TIME;
						break;
					/* key id */
					case 1:
						eTag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
						break;
					case 2:
						eTag = BACNET_APPLICATION_TAG_DATETIME; 
						break; 
					/* invalid tag */ 
					default: 
						break; 
					}
				}
			}
			break;
 
        /* default case - do nothing */
        default:
            break;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Prop_Context_Tag: Exit \r\n");
	#endif

    return eTag;
}

int32_t BACApp_Decode_Context_Data_Len(
    uint8_t * apdu,
    uint32_t max_apdu_len,
    BACNET_PROPERTY_ID eProperty)
{
    int32_t i32APDU_Len = 0, i32Len = 0;
    int32_t i32Tag_Len = 0;
    uint8_t i32Tag_Number = 0;
    uint32_t i32Len_Value_Type = 0;
    uint8_t u8Tag = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Context_Data_Len: Entry \r\n");
	#endif

    /* FIXME: use max_apdu_len! */
    //max_apdu_len = max_apdu_len;
    if (apdu && IS_CONTEXT_SPECIFIC(*apdu)) {
        i32Tag_Len =
            Decode_Tag_Number_And_Value(&apdu[0], &i32Tag_Number,
            &i32Len_Value_Type);
        if (i32Tag_Len) {
            i32APDU_Len = i32Tag_Len;
            u8Tag = BACApp_Prop_Context_Tag(eProperty, 0, i32Tag_Number, 0, 0);
            if (u8Tag < MAX_BACNET_APPLICATION_TAG) {
                i32Len =
                    BACApp_Decode_Data_Len(&apdu[i32APDU_Len], u8Tag,
                    i32Len_Value_Type);
                i32APDU_Len += i32Len;
            } else {
                i32APDU_Len += i32Len_Value_Type;
            }
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Decode_Context_Data_Len: Exit \r\n");
	#endif

    return i32APDU_Len;
}


/** function to encode data as per app_tag & property data_type */
int32_t BACApp_Encode_data(
    uint8_t * pu8ApduBuf,
    BACNET_PROPERTY_VALUE * psValue,
    BACNET_PROPERTY_ID  eObjectProperty)
{
    int32_t i32ApduLen = 0;   /* total length of the apdu, return psValue */
    int8_t i8OpenTag[256] = {-1,0,0,0,0};
    int8_t i8OpenTagCnt = -1;
    int8_t i8InternTag = -1;
    uint32_t u32ItemCount = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_data: Entry \r\n");
	#endif

    if(NULL == psValue || NULL == pu8ApduBuf)
        return 0;

    memset(i8OpenTag, -1, sizeof(i8OpenTag));

    while(NULL != psValue)
    {
    	 u32ItemCount ++;
        /* Encode Opening TAG and store is Value */
        //if(psValue->m_StartTag >= 0 && -3 != psValue->m_ContextSpecific)
		if( (psValue->m_StartTag >= 0 && -3 != psValue->m_ContextSpecific)
			||	(-3 == psValue->m_ContextSpecific && PROP_EVENT_PARAMETERS == eObjectProperty
				 && (8 == psValue->m_StartTag || 13 == psValue->m_StartTag))
		  )
        {
            if(i8OpenTagCnt == -1 || (i8OpenTag[i8OpenTagCnt] != psValue->m_StartTag && i8OpenTagCnt >= 0) )
            {
                i8OpenTagCnt++;
                i8OpenTag[i8OpenTagCnt] = psValue->m_StartTag;
                i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], psValue->m_StartTag);
            }
        }

        /* Encode Internal Opening TAG (For DateRange) and store is Value */
        if(psValue->m_ContextSpecific >= 0 && i8InternTag != psValue->m_ContextSpecific)
        {
            i8InternTag = psValue->m_ContextSpecific;
            i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], psValue->m_ContextSpecific);
        }
        else if(-3 == psValue->m_ContextSpecific)
        {
            //i8InternTag = getInternTag(eObjectProperty);
            if(PROP_AUTHENTICATION_POLICY_LIST != eObjectProperty)
			{
				if(PROP_EVENT_PARAMETERS == eObjectProperty &&
					(0 == psValue->m_StartTag || 8 == psValue->m_StartTag ||
					13 == psValue->m_StartTag || 17 == psValue->m_StartTag))
				{
					i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], psValue->m_ContextTag);
					i8InternTag = psValue->m_ContextTag;    /* special handling for Event parameters */
					psValue->m_ContextTag = -1;
				}
				else
				{
					i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], psValue->m_StartTag);
					/* Increase opening tag count */
					i8OpenTagCnt++;
					i8OpenTag[i8OpenTagCnt] = psValue->m_StartTag;
				}
			}
            else
			{
				i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 0);
				/* Increase opening tag count */
				i8OpenTagCnt++;
				i8OpenTag[i8OpenTagCnt] = psValue->m_StartTag;
			}
            if(PROP_AUTHENTICATION_POLICY_LIST == eObjectProperty && 0 == psValue->m_StartTag)
                i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 0);
        }

        /* Encode Opening TAG for Active COV Subscription */
        if(PROP_ACTIVE_COV_SUBSCRIPTIONS == eObjectProperty &&
            psValue->m_ContextTag == 1 && psValue->m_ContextSpecific == 0 )
        {
            i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], psValue->m_ContextTag);
            psValue->m_ContextTag = -1;
        }

        /* Encode Opening TAG for present value of channel object */
		if((PROP_PRESENT_VALUE == eObjectProperty) &&
			(psValue->m_ContextSpecific == -4) && (psValue->m_TagType == 9))
		{
			i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 0);
		}

        /* Encode Property Value as per Application or Context specific */
        if (psValue->m_ContextTag >= 0)
        {
            i32ApduLen +=
                BACApp_Encode_Context_Data_Value(&pu8ApduBuf[i32ApduLen], psValue->m_ContextTag, psValue);
        }
        else
        {
            i32ApduLen += BACApp_Encode_Application_Data(&pu8ApduBuf[i32ApduLen], psValue);
        }

        /* Encode Closing TAG for Active COV Subscription */
        if(PROP_ACTIVE_COV_SUBSCRIPTIONS == eObjectProperty &&
            psValue->m_ContextTag == -2 && psValue->m_ContextSpecific == 0 )
        {
            i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 1);
            psValue->m_ContextTag = -1;
        }
        else if(PROP_AUTHENTICATION_POLICY_LIST == eObjectProperty)
        {
            if(0 == psValue->m_StartTag && 1 == psValue->m_ContextTag && 2 == psValue->m_TagType
                &&  -3 == psValue->pstNextPropVal->m_ContextSpecific)
                psValue->pstNextPropVal->m_StartTag = 1;

            if(psValue->m_ContextSpecific == -3)
            {
                i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 0);
                if(1 == psValue->m_StartTag)
                {
                    psValue->m_StartTag = 0;
                    i8OpenTag[i8OpenTagCnt] = -1;
                    i8OpenTagCnt--;
                }
            }
        }

        /* Encode Closing TAG, if m_ContextSpecific is -1 and InternTAG is not -1*/
        if( (psValue->m_ContextSpecific < 0) && i8InternTag != -1 )
        {
            i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8InternTag);
            i8InternTag = -1;
        }
        /* if m_ContextSpecific is differnet than next Internal TAG */
        else if(psValue->pstNextPropVal != NULL && i8InternTag != -1)
        {
            if (psValue->m_ContextSpecific != psValue->pstNextPropVal->m_ContextSpecific &&
                psValue->pstNextPropVal->m_ContextSpecific != -2)
            {
                i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8InternTag);
                i8InternTag = -1;
            }
        }


        /* Encode Closing TAG, if Start TAG is -1 */
        if( (psValue->m_StartTag < 0) && i8OpenTagCnt >= 0)
        {
            if(i8OpenTag[i8OpenTagCnt] != -1 )
            {
                i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8OpenTag[i8OpenTagCnt]);
                i8OpenTag[i8OpenTagCnt] = -1;
                i8OpenTagCnt--;
            }
        }
        /* if Start TAG is differnet than next Start TAG */
        else if(psValue->pstNextPropVal != NULL && i8OpenTagCnt >= 0)
        {
        		if(psValue->m_StartTag != psValue->pstNextPropVal->m_StartTag &&  //v14
					psValue->pstNextPropVal->m_StartTag != -2 && i8OpenTag[i8OpenTagCnt] != -1 &&
					-3 != psValue->pstNextPropVal->m_ContextSpecific)
				{
					i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8OpenTag[i8OpenTagCnt]);
					i8OpenTag[i8OpenTagCnt] = -1;
					i8OpenTagCnt--;
				}
				else if(PROP_EVENT_PARAMETERS == eObjectProperty &&
					psValue->m_StartTag != psValue->pstNextPropVal->m_StartTag &&
					-3 == psValue->pstNextPropVal->m_ContextSpecific && -3 == psValue->m_ContextSpecific)
				{
					i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8OpenTag[i8OpenTagCnt]);
					i8OpenTag[i8OpenTagCnt] = -1;
					i8OpenTagCnt--;
				}
				#ifdef BACDEL_PR18
				else if(PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE == eObjectProperty &&
					psValue->m_StartTag != psValue->pstNextPropVal->m_StartTag && 2 == psValue->m_TagType)
				{
					i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8OpenTag[i8OpenTagCnt]);
					i8OpenTag[i8OpenTagCnt] = -1;
					i8OpenTagCnt--;
				}
				#endif
        }

		/* Encode closing TAG for  present value of channel object */
       if((PROP_PRESENT_VALUE == eObjectProperty) && (NULL == psValue->pstNextPropVal) &&
		   (-4 == psValue->m_ContextSpecific))
        {
            i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 0);
        }

		/* special handling for change-of-life-safety (event parameters) if
		list-of-life-safety-alarm-values and list-of-alarm-values both are empty */
		if(PROP_EVENT_PARAMETERS == eObjectProperty)
		{
			if(8 == psValue->m_StartTag && 2 == psValue->m_TagType && NULL != psValue->pstNextPropVal &&
				8 == psValue->pstNextPropVal->m_StartTag && 3 == psValue->pstNextPropVal->m_ContextSpecific
				&& 12 == psValue->pstNextPropVal->m_TagType)
			{
				i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 1);
				i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 1);
				i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 2);
				i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 2);
			}
			else if((8 == psValue->m_StartTag && 2 == psValue->m_TagType && NULL != psValue->pstNextPropVal &&
				2 == psValue->pstNextPropVal->m_StartTag && -3 == psValue->pstNextPropVal->m_ContextSpecific
				&& 9 == psValue->pstNextPropVal->m_TagType) ||
				(17 == psValue->m_StartTag && NULL == psValue->pstNextPropVal && 2 == psValue->m_TagType))
			{
				i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 1);
				i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 1);
			}
			else if((1 == psValue->m_StartTag && 9 == psValue->m_TagType && NULL != psValue->pstNextPropVal &&
				8 == psValue->pstNextPropVal->m_StartTag && 3 == psValue->pstNextPropVal->m_ContextSpecific
				&& 12 == psValue->pstNextPropVal->m_TagType) ||
				(0 == psValue->m_StartTag && 2 == u32ItemCount && NULL == psValue->pstNextPropVal && 8 == psValue->m_TagType &&
				1 == psValue->m_ContextTag))
			{
				i32ApduLen += Encode_Opening_Tag(&pu8ApduBuf[i32ApduLen], 2);
				i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], 2);
			}
		}

        psValue = psValue->pstNextPropVal;
    }

    /* Encode Closing TAG, if Internal TAG is -1 or differnet than next Internal TAG */
    if( i8InternTag != -1 )
    {
        i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8InternTag);

        i8InternTag = -1;
    }


    /* Encode Closing TAG, if Start TAG is -1 or differnet than next Start TAG */
    while( i8OpenTagCnt >= 0)
    {
        if(i8OpenTag[i8OpenTagCnt] != -1)
        {
            i32ApduLen += Encode_Closing_Tag(&pu8ApduBuf[i32ApduLen], i8OpenTag[i8OpenTagCnt]);
            i8OpenTag[i8OpenTagCnt] = -1;
        }
        i8OpenTagCnt--;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_data: Exit \r\n");
	#endif

    return i32ApduLen;
}


/* returns the length of data between an opening tag and a closing tag.
   Expects that the first octet contain the opening tag.
   Include a value property identifier for context specific data
   such as the value received in a WriteProperty request */
int32_t BACApp_Verify_Data_Len(
    uint8_t * apdu,
    uint32_t max_apdu_len,
    BACNET_PROPERTY_ID eProperty)
{
    int32_t i32Len = 0;
    int32_t total_len = 0;
    int32_t i32APDU_Len = 0;
    uint8_t i32Tag_Number = 0;
    uint8_t u8Opening_Tag_Number = 0;
    uint8_t u8Opening_Tag_Number_Counter = 0;
    uint32_t u32Value = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Verify_Data_Len: Entry \r\n");
	#endif

    if (IS_OPENING_TAG(apdu[0])) 
    {
        /* Decode first Opening Tag */
        i32Len = Decode_Tag_Number_And_Value(&apdu[i32APDU_Len], &i32Tag_Number, &u32Value);
        
        i32APDU_Len += i32Len;
        u8Opening_Tag_Number = i32Tag_Number;
        u8Opening_Tag_Number_Counter = 1;
        
        while (u8Opening_Tag_Number_Counter) 
        {
            if (IS_OPENING_TAG(apdu[i32APDU_Len])) 
            {
                i32Len = Decode_Tag_Number_And_Value(&apdu[i32APDU_Len], &i32Tag_Number,
                    &u32Value);
                if (i32Tag_Number == u8Opening_Tag_Number)
                    u8Opening_Tag_Number_Counter++;
            } 
            else if (IS_CLOSING_TAG(apdu[i32APDU_Len])) 
            {
                i32Len = Decode_Tag_Number_And_Value(&apdu[i32APDU_Len], &i32Tag_Number,
                    &u32Value);
                if (i32Tag_Number == u8Opening_Tag_Number)
                    u8Opening_Tag_Number_Counter--;
            } 
            else if (IS_CONTEXT_SPECIFIC(apdu[i32APDU_Len])) 
            {
                /* context-specific tagged data */
                i32Len = BACApp_Decode_Context_Data_Len(&apdu[i32APDU_Len],
                    max_apdu_len - i32APDU_Len, eProperty);
            } 
            else 
            {
                /* application tagged data */
                i32Len = BACApp_Decode_Application_Data_Len(&apdu[i32APDU_Len],
                    max_apdu_len - i32APDU_Len);
            }

            i32APDU_Len += i32Len;

            if (u8Opening_Tag_Number_Counter) 
            {
                if (i32Len > 0) 
                {
                    total_len += i32Len;
                } 
                else 
                {
                    /* error: i32Len is not incrementing */
                    total_len = BACNET_STATUS_ERROR;
                    break;
                }
            }

            if ((unsigned) i32APDU_Len > max_apdu_len) 
            {
                /* error: exceeding our buffer limit */
                total_len = BACNET_STATUS_ERROR;
                break;
            }
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Verify_Data_Len: Exit \r\n");
	#endif

    return total_len;
}

/**
*******************************************************************************
*                                                                    
*DESCRIPTION                                                                          
*   Function to Encode property value according to its DataType.
*   The value is validated against the allowed Max APDU Length.
*    
*@param apdu                [out]   Apdu buffer to stroe encoded Data
*@param application_data    [in]    pointer to structure of property
*@param eData_Type          [in]    Data type of property
*@param u32SegSupport       [in]    Segmentation support of device
*@param bArrIndxPresent     [in]    Indicates if array index is present or not
*@param u16MaxApduLen       [in]    Max apdu length of device
*
*@return    Length of encoded Data
*                                                                      
******************************************************************************/
int32_t BACApp_Encode_Data_Type(uint8_t * apdu, int32_t u32MaxInBufSize, void * application_data,
    BACNET_DATA_TYPE eData_Type, uint32_t u32ArrayIndex,
    int32_t i32APDULenFilled, uint32_t u32SegSupport, bool bArrIndxPresent,
    uint16_t u16MaxApduLen, uint16_t *pu16NodeLenArr, int32_t *pu32NodeCnt, 
	int32_t i32NodeLenArrSize)
    
{
    int32_t i32APDU_Len = 0;   /* total length of the apdu, return value */
    uint32_t u32Index = 0, u32MaxVal =0;
	int32_t i32DecodeLen = 0;
    BACNET_APPLICATION_TAG eAppTag = 0; /* For Application Tag */
	uint8_t u8TempAPDU[MAX_APDU_BUFFER + 100] = {0}; /* 100 bytes for additional buffer to avoid crash */
	bool bReadRangeFlag = FALSE;
	int32_t i32PrevLen = 0;    

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_Data_Type: Entry \r\n");
	#endif

	/* encode as per data-type */
    if (application_data && apdu) 
    {
        switch (eData_Type) 
        {
			case BACNET_DT_EMPTY :
				break;

            case BACNET_DT_NULL:
                u8TempAPDU[0] = 0;
                i32APDU_Len++;
                break;

            case BACNET_DT_BOOLEAN:
                i32APDU_Len =
                    Encode_Application_Boolean(&u8TempAPDU[0], 
                            ((Pr_BACnetBool_t *) application_data)->m_bVal);
                break;

            case BACNET_DT_ENUM:
            {
                BACNET_BINARY_PV     eTmpBinaryVal;
                
                eTmpBinaryVal = ((Pr_BinaryEnumPV_t *) application_data)->m_eVal;
                
                /* Check polarity for Binary Present Value property */
                if(((Pr_BinaryEnumPV_t *) application_data)->m_ePVPolarity == POLARITY_REVERSE)
                {
                    if(eTmpBinaryVal == BINARY_ACTIVE)
                        eTmpBinaryVal = BINARY_INACTIVE;
                    else
                        eTmpBinaryVal = BINARY_ACTIVE;
                }

                i32APDU_Len =
                    Encode_Application_Enumerated(&u8TempAPDU[0], eTmpBinaryVal);
            }
            break;

            case BACNET_DT_ENUM_NEW:
            {
                BACNET_EVENT_STATE eValue = 0;
                /* NOTE : all enumerated properties are memory mapped with event state struct */
                eValue = ((Pr_BACnetEventState_t *)application_data)->m_eEventState;

                i32APDU_Len =
                    Encode_Application_Enumerated(&u8TempAPDU[0], eValue);
            }break;

            /* For Binary Output Priority Array */    
            case BACNET_DT_PRIORITY_ARRAY:
            {
                Pr_BACnetPriorityArray_t *pstPriorityArray = NULL;
                pstPriorityArray = application_data;

                /* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    u32MaxVal = MAX_PRIORITY_ARRAY_COUNT;
                    u32Index = 0;
                }
                /* Send response with the given index data */
                else
                {
                    u32MaxVal = u32ArrayIndex;
                    u32Index = u32ArrayIndex-1;
                }
                /* Zero Indicates the total length of priority array */
                if(u32ArrayIndex == 0 && ARRAY_INDEX_PRESENT == bArrIndxPresent)
                {
                    i32APDU_Len =
                    Encode_Application_Unsigned(&u8TempAPDU[0],MAX_PRIORITY_ARRAY_COUNT);
                }
                else
                {   
                    /* For all 16 Priority elements */
                    for(;u32Index<u32MaxVal; u32Index++)
                    {
                         /* To send error in case the value to be reported is 
                          * greater than the APDU limit.
                          */
                        if(i32APDU_Len < APDU_ERROR_LENGTH)
                        {
                            eAppTag = pstPriorityArray->m_bTagType[u32Index];
                            /* Check Tag Type */
                            switch(eAppTag)
                            {
								/* Encode Boolean tag */
								case BACNET_APPLICATION_TAG_BOOLEAN:
								{
									i32APDU_Len += Encode_Application_Boolean(&u8TempAPDU[i32APDU_Len], 
										pstPriorityArray->m_uValue.m_bVal[u32Index]);
								}
								break;
                                /*  Encode Enumerated Tag */
                                case  BACNET_APPLICATION_TAG_ENUMERATED:
                                {
                                    i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len], 
										pstPriorityArray->m_uValue.m_eVal[u32Index] );
                                }
                                break;
                                /* Encode Unsigned Tag */
                                case BACNET_APPLICATION_TAG_UNSIGNED_INT:
                                {
                                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
										pstPriorityArray->m_uValue.m_u32Val[u32Index]);
                                }
                                break;
                                /* Encode Real Tag */
                                case BACNET_APPLICATION_TAG_REAL:
								{
									i32APDU_Len += Encode_Application_Real(&u8TempAPDU[i32APDU_Len],
									pstPriorityArray->m_uValue.m_fVal[u32Index]);
								}
                                break;
								/* Encode signed Tag */
                                case BACNET_APPLICATION_TAG_SIGNED_INT:
                                {
                                    i32APDU_Len += Encode_Application_Signed(&u8TempAPDU[i32APDU_Len], 
                                        pstPriorityArray->m_uValue.m_i32IntVal[u32Index]);
                                }
                                break;

                                default: /*BACNET_APPLICATION_TAG_NULL */ 
                                {
                                     /* Encode Null Tag */
                                    i32APDU_Len += Encode_Application_Null(&u8TempAPDU[i32APDU_Len]);
                                }
                                break;
                            }/* End of Switch */
                        }
                    }/* End of for loop */
                }/* End of else condition */
            }
            break;

			case BACNET_DT_UNSIGNED8:
                i32APDU_Len =
                    Encode_Application_Unsigned(&u8TempAPDU[0],
                            ((Pr_BACnetUnsigned8_t *) application_data)->m_u8Val);
                break;
            
            case BACNET_DT_UNSIGNED16:
                i32APDU_Len =
                    Encode_Application_Unsigned(&u8TempAPDU[0],
                            ((Pr_BACnetUnsigned16_t *) application_data)->m_u16Val);
                break;

            case BACNET_DT_UNSIGNED:
            case BACNET_DT_UNSIGNED32:
                i32APDU_Len =
                    Encode_Application_Unsigned(&u8TempAPDU[0],
                            ((Pr_BACnetUnsigned32_t *) application_data)->m_u32Val);
                break;

            case BACNET_DT_INTEGER:
                i32APDU_Len =
                    Encode_Application_Signed(&u8TempAPDU[0],
                            ((Pr_BACnetSigned32_t *) application_data)->m_i32Val);
                break;

            case BACNET_DT_REAL:
                i32APDU_Len = Encode_Application_Real(&u8TempAPDU[0], 
                    ((Pr_BACnetReal_t *) application_data)->m_fVal);
                break;
        	case BACNET_DT_REAL_ARRAY: //SK
			{
//				Pr_BACnetArrReal_t *pstLinkspeeds = NULL;
				u32MaxVal = BACNET_ARRAY_OF_THREE;

				/* If none send , response with all */

			 for(;u32Index<u32MaxVal; u32Index++)
			 {
				i32APDU_Len += Encode_Application_Real(&u8TempAPDU[i32APDU_Len],
						 ((Pr_BACnetArrReal_t *)application_data)->m_fVal[u32Index]);
			 }
			}
			break;
            case BACNET_DT_DOUBLE:
                i32APDU_Len = Encode_Application_Double(&u8TempAPDU[0], 
                    ((Pr_BACnetDouble_t *) application_data)->m_dVal);
                break;                

            case BACNET_DT_CHARSTRING:
            {
                Pr_BACnetCharStr_t *pstCharString = NULL;
                pstCharString = application_data;

                /* encode string */
                i32APDU_Len =
                    Encode_Application_Character_String(&u8TempAPDU[0], 
                    &pstCharString->m_stCHARString);
            }
            break;

            case BACNET_DT_TIME:
                i32APDU_Len =
                    Encode_Application_Time(&u8TempAPDU[0], 
                        &((Pr_BACnetTime_t *) application_data)->m_Time_val);
                break;

            case BACNET_DT_DATE:
                i32APDU_Len =
                    Encode_Application_Date(&u8TempAPDU[0], 
                            &((Pr_BACnetDate_t *) application_data)->m_Date_val);
                break;

            case BACNET_DT_OBJECTID:
                i32APDU_Len =
                    Encode_Application_Object_Id(&u8TempAPDU[0],
						(int32_t)((Pr_BACnetObjId_t *)application_data)->m_eObjectType,
                        ((Pr_BACnetObjId_t *)application_data)->m_u32ObjId);
                break;

            case BACNET_DT_OBJTYPE:
                i32APDU_Len =
                    Encode_Application_Enumerated(&u8TempAPDU[0],
                        ((Pr_BACnetObjType_t*)application_data)->m_eObjectType);
                break;

            case BACNET_DT_BACNETDEVSTAT:
                i32APDU_Len = Encode_Application_Enumerated(&u8TempAPDU[0], 
                    ((Pr_BACnetDevStatus_t*)application_data)->m_eDeviceStatus);
                break;
                
            case BACNET_DT_BITSTRING:
            {
				Pr_BACnetBitStr_t *pstBitString = NULL;
				pstBitString = application_data;

				i32APDU_Len = Encode_Application_Bitstring_small(&u8TempAPDU[0],
					&pstBitString->m_stBitStr);
            }break;

            case BACNET_DT_BITSTRING_NEW:
            {
                Pr_BACnetBITStr_t *pstBitString = NULL;
				pstBitString = application_data;
             
				i32APDU_Len = Encode_Application_BITstring(&u8TempAPDU[0], 
					&pstBitString->m_stBitString);

            }break;

            case BACNET_DT_OCTETSTRING:
            {
                Pr_BACnetOctetStr_t *pstOctetString = NULL;
                pstOctetString = application_data;

				/* encode octet string */
				i32APDU_Len = Encode_Application_Octet_String(&u8TempAPDU[0],
					&pstOctetString->m_stOctetString);
            }
            break;

            case BACNET_DT_DATETIME:
            {
                i32APDU_Len = Encode_Application_Date(&u8TempAPDU[0], 
                    &((Pr_BACnetDateTime_t *)application_data)->m_stDateTime.m_stDate);
                i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len], 
                    &((Pr_BACnetDateTime_t *)application_data)->m_stDateTime.m_stTime);
            }
            break;

            case BACNET_DT_TIMESTAMP_ARRAY:
            {
                Pr_BACnetEventTimeStamp_t *pstTimeStampArr = NULL;
                pstTimeStampArr = (Pr_BACnetEventTimeStamp_t *)application_data;

				if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
				{
					i32APDU_Len = DT_Encode_TimeStamp_Array(&u8TempAPDU[0],
						 pstTimeStampArr);
				}
                else if(u32ArrayIndex == 0)
				{
					i32APDU_Len = Encode_Application_Unsigned
								(&u8TempAPDU[0], BACNET_ARRAY_OF_THREE);
				}
				else
				{
					i32APDU_Len = DT_Encode_TimeStamp(&u8TempAPDU[0], 
							&pstTimeStampArr->m_uEvntStamp[u32ArrayIndex - 1],
							 pstTimeStampArr->m_eTimeStampType[u32ArrayIndex - 1]);
				}
            }
            break;
			case BACNET_DT_OCTETSTRING_ARRAY:
			{
				/* local variables */
				Pr_ListOfOctetStr_t *pstOctetStrArr = NULL;
				ListOfOctetStr_t *pstOctetStr = NULL;
				uint8_t u8ByteCnt = 0;

				/* get the value */
				pstOctetStrArr = (Pr_ListOfOctetStr_t *)application_data;
				pstOctetStr = pstOctetStrArr->m_pstOctetStrVal;

                /* if no index, send response with all entries */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    while(NULL != pstOctetStr && (i32APDU_Len < u32MaxInBufSize))
                    {
						/* encode value */
                        i32APDU_Len += Encode_Application_Octet_String(&u8TempAPDU[i32APDU_Len],
							&pstOctetStr->stOctetString);
                        /* move to next value */
                        pstOctetStr = pstOctetStr->m_pstNext;
                    }
                }
                else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
						&u8TempAPDU[i32APDU_Len], pstOctetStrArr->m_u32Count);
                }
                else
                {
					/* traverse till the node to be encoded */
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstOctetStr)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next real value */
                        pstOctetStr = pstOctetStr->m_pstNext;
                    }
                    if(NULL != pstOctetStr)
                    {
                        /* encode value */
                        i32APDU_Len += Encode_Application_Octet_String(&u8TempAPDU[0],
							&pstOctetStr->stOctetString);
                    }
                }
			}
			break;
			case BACNET_DT_HOSTNPORT:
			{
				/* local variables */
				Pr_BACnetHostNPort_t *pstHostNPort = NULL;
                pstHostNPort = (Pr_BACnetHostNPort_t *)application_data;
				if(NULL != pstHostNPort)
				{
					/* encode host n port */
					i32APDU_Len += DT_Encode_HostNPort(&u8TempAPDU[0],
						&pstHostNPort->m_stHostNPort);
				}
			}
			break;

			case BACNET_DT_BDT_ENTRY:
			{
				/* local variables */
				Pr_BACnetBDTEntry_t *pstBDTEntry = NULL;
                pstBDTEntry = (Pr_BACnetBDTEntry_t *)application_data;
				if(NULL != pstBDTEntry)
				{
					/* encode BDT entry */
					i32APDU_Len += DT_Encode_BdtEntry(&u8TempAPDU[0],
						&pstBDTEntry->m_stBDTEntry);
				}
			}
			break;

			case BACNET_DT_BDT_ENTRY_LIST:
			{
				/* local variables */
				Pr_ListOfBACnetBDTEntry_t  *pstBDTEntryList = NULL;
				pstBDTEntryList = (Pr_ListOfBACnetBDTEntry_t *)application_data;
				if(NULL != pstBDTEntryList)
				{
					/* encode BDT entry */
					i32APDU_Len += DT_Encode_BdtEntry_List(&u8TempAPDU[0],
						pstBDTEntryList->m_pstBDTEntryList, u32MaxInBufSize,
						pu16NodeLenArr, pu32NodeCnt, i32NodeLenArrSize);
				}
			}
			break;

			case BACNET_DT_FDT_ENTRY:
			{
				/* local variables */
				Pr_BACnetFDTEntry_t *pstFDTEntry = NULL;
                pstFDTEntry = (Pr_BACnetFDTEntry_t *)application_data;
				if(NULL != pstFDTEntry)
				{
					/* encode FDT entry */
					i32APDU_Len += DT_Encode_FdtEntry(&u8TempAPDU[0],
						&pstFDTEntry->m_stFDTEntry);
				}
			}
			break;

			case BACNET_DT_FDT_ENTRY_LIST:
			{
				/* local variables */
				Pr_ListOfBACnetFDTEntry_t  *pstFDTEntryList = NULL;
				pstFDTEntryList = (Pr_ListOfBACnetFDTEntry_t *)application_data;
				if(NULL != pstFDTEntryList)
				{
					/* encode FDT entry list */
					i32APDU_Len += DT_Encode_FdtEntry_List(&u8TempAPDU[0],
						pstFDTEntryList->m_pstFDTEntryList, u32MaxInBufSize,
						pu16NodeLenArr, pu32NodeCnt, i32NodeLenArrSize);
				}
			}
			break;

            case BACNET_DT_OBJECT_TYPE_SUPPORTED:
            {
                Pr_BACnetObjectTypesSupported_t *pstObjSupport = NULL;

                pstObjSupport = application_data;

                /* Encode Bit String */
                i32APDU_Len = 
                    Encode_Application_Byte(&u8TempAPDU[0], 
                                &pstObjSupport->m_stObjectSupport.Byte[0],
                                OBJECT_SUPPORTED_UNUSED_BITS,
                                MAX_OBJECT_TYPE_BYTE);
            }break;

            case BACNET_DT_SERVICES_SUPPORTED:
            {
                Pr_BACnetServicesSupported_t *pstSerSupport = NULL;

                pstSerSupport = application_data;

                /* Encode Bit String */
                i32APDU_Len = 
                    Encode_Application_Byte(&u8TempAPDU[0], 
                                &pstSerSupport->m_stServiceSupport.Byte[0],
                                SERVICE_SUPPORTED_UNUSED_BITS,
                                MAX_SERVICE_SUPPORT_BYTE);
            }
            break;

            case BACNET_DT_OBJECTID_ARRAY: 
            {
				/* locals */
				Pr_ListOfObjId_t *pstObjectIdArray = NULL;
				ListOfObjId_t *pstObjIDList = NULL;
				uint8_t u8ByteCnt = 0;

				/* get the pointer */
                pstObjectIdArray = (Pr_ListOfObjId_t *)application_data;
				if(NULL != pstObjectIdArray)
				{
					pstObjIDList = pstObjectIdArray->m_pstArrayObjId;
				}

				/* check array index */
				if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
				{
					/* encode complete list */
					while((pstObjIDList != NULL) && (i32APDU_Len < u32MaxInBufSize))
					{
						i32APDU_Len +=
							Encode_Application_Object_Id(&u8TempAPDU[i32APDU_Len],
							(int32_t)pstObjIDList->m_stObjectId.m_eObjectType, 
							pstObjIDList->m_stObjectId.m_u32ObjId);

						/* move to next value */
						pstObjIDList = pstObjIDList->m_pstNext;
					}
				}
                else if(u32ArrayIndex == 0)
				{
					/* encode count */
					i32APDU_Len = Encode_Application_Unsigned
						(&u8TempAPDU[0], pstObjectIdArray->m_u32ObjCount);
				}
				else
				{
					/* encode requested data */
					for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstObjIDList)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next value */
                        pstObjIDList = pstObjIDList->m_pstNext;
                    }

					if(NULL != pstObjIDList)
					{
						/* encode object id */
						i32APDU_Len = Encode_Application_Object_Id(&u8TempAPDU[0],
							(int32_t)pstObjIDList->m_stObjectId.m_eObjectType,
							pstObjIDList->m_stObjectId.m_u32ObjId);
					}
				}
            }
            break;

			case BACNET_DT_BACKUPSTATE:
				i32APDU_Len =
                    Encode_Application_Enumerated(&u8TempAPDU[0],
                            ((Pr_BACnetBackupState_t*)application_data)->m_eBackupState);
			break;

            case BACNET_DT_BACNETSEG: 
                i32APDU_Len =
                    Encode_Application_Enumerated(&u8TempAPDU[0],
                            ((Pr_BACnetSegmentation_t*)application_data)->m_eSegmentationSupport);
            break;
            
            case BACNET_DT_UNSIGNED_ARRAY:
            {
                ListOfUnsigned_t *pstBackupIntList = NULL;

                /* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    u32MaxVal = 0;
                }
                /* Send response with the given index data */
                else
                {
                    u32MaxVal = u32ArrayIndex;
                } 
                if( NULL != ((Pr_ListOfUnsigned_t *)application_data)->m_pstUnsignVal)
                {
                    /* Send First string for All request or for index 1 */
                    if((u32MaxVal == 0)||(u32MaxVal == 1))
                    {
                        i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len],
                          ((Pr_ListOfUnsigned_t *)application_data)->m_pstUnsignVal->m_u32Value);
                    }
                    pstBackupIntList = (ListOfUnsigned_t *)
                                ((Pr_ListOfUnsigned_t *) application_data)->m_pstUnsignVal->m_pstNext;
                    
                    /* Request for All strings or other than first string */
                    if((u32MaxVal == 0)||(u32MaxVal != 1))
                    {   
                        /* Indicates one (zero index already sent) */                 
                        u32Index++;
                        /* To send error in case the value to be reported is 
                         * greater than the APDU limit.
                         */
                        while((pstBackupIntList != NULL) && ( i32APDU_Len < u32MaxInBufSize))
                        {             
                            /* Send only when it matches  or send all */
                            if((u32Index == u32MaxVal-1)||(u32MaxVal == 0))
                            {
                                i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len],
                                pstBackupIntList->m_u32Value);
                            }
                            pstBackupIntList = (ListOfUnsigned_t *)pstBackupIntList->m_pstNext;
                            u32Index++;
                            
                        }
                    }
                }
            }
            break;

			/* For Unsigned List values property */
            case BACNET_DT_UNSIGNED_LIST:
            {
				/* Local Variables */
				Pr_ListOfUnsigned_t *pstUnsigList = NULL;
	
				/* get the value */
                pstUnsigList = (Pr_ListOfUnsigned_t *)application_data;

				i32APDU_Len = DT_Encode_Unsigned_List(&u8TempAPDU[0],
						 pstUnsigList, u32MaxInBufSize, pu16NodeLenArr, 
						 pu32NodeCnt, i32NodeLenArrSize);
            }
			break;

            /* for state text / bit text property */
            case BACNET_DT_CHARSTRING_ARRAY:
            {
                Pr_ListOfCharStr_t *pstCharStrArray = NULL;
                ListOfCharStr_t *pstStringVal = NULL;
                uint8_t u8ByteCnt = 0;

                /* get the value */
                pstCharStrArray = application_data;
                pstStringVal = &pstCharStrArray->m_stStringVal;

                /* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    while(NULL != pstStringVal && (i32APDU_Len < u32MaxInBufSize))
                    {
						/* as 1st member is static, if count is 0 do not encode anything */
						if(!pstCharStrArray->m_u32Count)
							break;
							
                        /* encode char string value */
                        i32APDU_Len +=
                            Encode_Application_Character_String(&u8TempAPDU[i32APDU_Len], 
                                    &pstStringVal->m_stCharStr);
                        /* move to next char string value */
                        pstStringVal = pstStringVal->m_pstNext;
                    }
                }
                else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
                        &u8TempAPDU[i32APDU_Len], pstCharStrArray->m_u32Count);
                }
                else
                {
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstStringVal)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next char string value */
                        pstStringVal = pstStringVal->m_pstNext;
                    }
                    if(NULL != pstStringVal)
                    {
                        /* encode char string value */
                        i32APDU_Len +=
                            Encode_Application_Character_String(&u8TempAPDU[i32APDU_Len], 
                                    &pstStringVal->m_stCharStr);
                    }
                }
            }break;

            case BACNET_DT_COVSUB: 
            {
                Pr_ListOfBACnetCovSubs_t *pstActCOVLst= NULL;
                ListOfBACnetCovSubs_t *pstActCOVSub = NULL;
                CovPropElement_t *pstPropRef = NULL;
                CovRecipientInfo_t *pstRecipient = NULL;
                BACnetOctetStr_t stMACAdrss = {0};
                uint8_t u8LoopCnt =0;
				#ifdef __FREERTOS__
				BACnetAddress_t m_stAddress = {0};
				uint8_t u8EndCount = 0;
				#endif
                
                pstActCOVLst = application_data;
                pstActCOVSub = pstActCOVLst->m_pstCOVSubscribe;
                if(NULL != pstActCOVSub)
                {
                    pstPropRef = &(pstActCOVSub->m_stCOVPropertyElem);
                    pstRecipient = pstActCOVSub->m_pstSubscriberInfo;
                }

				/* if encode function is called from ReadRange-B service 
				   make node count of nodes to 0 and set the flag */
				if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
				{
					*pu32NodeCnt = 0;
					bReadRangeFlag = TRUE;
				}

                /* Check value is Active COV is subsribed */
                /* To send error in case the value to be reported is 
                 * greater than the APDU limit.
                 */
                while((NULL != pstActCOVSub) && (NULL != pstPropRef) && (NULL != pstRecipient)
                    && (i32APDU_Len < u32MaxInBufSize))
                {
					/* save previous encoded data lenth */
					i32PrevLen = i32APDU_Len;
                    /* Dont encode recipient of Local broadcast */
                    if(pstRecipient->m_stAddress.u8IpAddrs[3] != 0xFF)
                {
                    /* Encode Opening Tag for New Subscription */ 
                    i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);

                    /* Encode Opening Tag for Recipient Process */ 
                    i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);

                    /* Encode Opening Tag for network-number, MAC-address */ 
                    i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);


                    /* Encode Network Number */
                    if(pstRecipient->m_stAddress.u16net)
                        i32APDU_Len +=
                            Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], pstRecipient->m_stAddress.u16net);
                    else
                        i32APDU_Len +=
                            Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], g_u16LocalNWNo);

					stMACAdrss.m_u32OctetCount = pstRecipient->m_stAddress.u8mac_len;

					for(u8LoopCnt=0; u8LoopCnt <=stMACAdrss.m_u32OctetCount; u8LoopCnt++)
                    {
						stMACAdrss.m_ai8OctetStr[u8LoopCnt] = pstRecipient->m_stAddress.u8IpAddrs[u8LoopCnt];
                    }

                    /* Encode MAC address */
                    i32APDU_Len += Encode_Application_Octet_String(&u8TempAPDU[i32APDU_Len], &stMACAdrss);

                    /* Encode the application closing tag */
                    i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);

                    /* Encode the application closing tag */
                    i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);

                    /* Encode Process Identifier */
                    i32APDU_Len +=
                        Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
                                                pstRecipient->m_u32ProcessId );

                    /* Encode the application closing tag */
                    i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);


                    /* Encode Opening Tag for network-number, MAC-address */ 
                    i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);

                    /* Encode Object Identifier */
                    i32APDU_Len +=
                        Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_0, 
												(int32_t)pstActCOVSub->m_stObjId.m_eObjectType, 
                                                pstActCOVSub->m_stObjId.m_u32ObjId);

                    /* Encode Property Identifier */
                    i32APDU_Len +=
                        Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
                                                    pstPropRef->m_ePropertyId);

                    /* context 2 array index is optional */
                    if (ARRAY_INDEX_PRESENT == pstPropRef->m_bArrIndxresent) 
                    {
                        i32APDU_Len +=
                            Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                                                    pstPropRef->m_u32PropertyArrayIndex);
                    }

                    /* Encode the application closing tag */
                    i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);


                    /* Encode Issued Confirmed Notification */
                    i32APDU_Len +=
                        Encode_Context_Boolean(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                                        pstRecipient->m_bIssueConfirmedNotification);

                    /* Encode Time Remaining */
                    i32APDU_Len +=
                        Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
                                                pstRecipient->m_u32TimeRemaining );

					if(BACNET_DT_REAL == pstActCOVSub->m_stCOVPropertyElem.m_eData_Type)
                    {
						i32APDU_Len +=
                            Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_4,
								pstRecipient->m_stPropertyValue.uValue.m_Real);						
                    }
                }
                    if( NULL != pstRecipient->m_pstNext)
                        pstRecipient = pstRecipient->m_pstNext;
                    else
                    {
                        pstActCOVSub = pstActCOVSub->m_pstNext;
                        if(NULL != pstActCOVSub)
                        {
                            pstPropRef = &pstActCOVSub->m_stCOVPropertyElem;
                            pstRecipient = pstActCOVSub->m_pstSubscriberInfo;
                        }
                    }

					/* if encode function is called from ReadRange-B service, save the length of
					   each node in array */
					if(bReadRangeFlag)
					{
						if(*pu32NodeCnt < i32NodeLenArrSize)
						{
							pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32APDU_Len - i32PrevLen);
							(*pu32NodeCnt)++;
						}
					}
                }// End of while(pstActCOVSub)
            }
            break;

            case BACNET_DT_ADDRESSBINDING_LIST: 
            {
                Pr_ListOfBACnetAddrBinding_t *pstDevAddrBind;
				BACnetAddrBinding_t *pstAddBindData;
                BACnetOctetStr_t stMACAdrss = {0};
                uint8_t u8LoopCnt = 0;

                pstDevAddrBind = application_data;
				pstAddBindData = &pstDevAddrBind->m_stAddBinding;

				/* if encode function is called from ReadRange-B service 
				   make node count of nodes to 0 and set the flag */
				if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
				{
					*pu32NodeCnt = 0;
					bReadRangeFlag = TRUE;
				}

               while(NULL != pstAddBindData)
			   {
                    if(i32APDU_Len < u32MaxInBufSize)
                    {
						/* save previous encoded data length */
						i32PrevLen = i32APDU_Len;
                        /* Encode Object Identifier */
                        i32APDU_Len +=
                        Encode_Application_Object_Id(&u8TempAPDU[i32APDU_Len],
								(int32_t)pstAddBindData->m_eObjectType,
                                pstAddBindData->m_u32ObjId);

                        /* Encode Network Number */
                        i32APDU_Len +=
                            Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
									pstAddBindData->m_stAddress.u16net);

                        if(pstAddBindData->m_stAddress.u8dlen)
                        {
                            /* Get Mac Address & Mac length */
							stMACAdrss.m_u32OctetCount = pstAddBindData->m_stAddress.u8dlen;
							for(u8LoopCnt=0; u8LoopCnt <= stMACAdrss.m_u32OctetCount; u8LoopCnt++)
							{
								stMACAdrss.m_ai8OctetStr[u8LoopCnt] = 
								    pstAddBindData->m_stAddress.u8DvDadr[u8LoopCnt];
							}
                        }
                        else
                        {
						    /* Get Ip Address & Port No */
							stMACAdrss.m_u32OctetCount = pstAddBindData->m_stAddress.u8mac_len;
							for( u8LoopCnt=0; u8LoopCnt <= stMACAdrss.m_u32OctetCount; u8LoopCnt++)
							{
								stMACAdrss.m_ai8OctetStr[u8LoopCnt] = 
								    pstAddBindData->m_stAddress.u8IpAddrs[u8LoopCnt];
							}
                        }

                        /* Encode MAC address */
                        i32APDU_Len += Encode_Application_Octet_String
							(&u8TempAPDU[i32APDU_Len], &stMACAdrss);
                    }
					pstAddBindData = pstAddBindData->pstNext;

					/* if encode function is called from ReadRange-B service, save the length of
					   each node in array */
					if(bReadRangeFlag)
					{
						if(*pu32NodeCnt < i32NodeLenArrSize)
						{
							pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32APDU_Len - i32PrevLen);
							(*pu32NodeCnt)++;
						}
					}
                }// End of while loop
            }
            break;

            case BACNET_DT_RECIPIENT_LIST: 
            {
                Pr_ListOfBACnetRecipient_t *pstRecipient = NULL;
				ListOfBACnetRecipient_t *pstListOfBACnetRecipient = NULL;
                BACnetOctetStr_t stMACAdrss = {0};
                uint8_t u8LoopCnt = 0;

				/* get the data */
                pstRecipient = application_data;
				pstListOfBACnetRecipient = pstRecipient->m_pstListOfRecipient;

				/* if encode function is called from ReadRange-B service 
				   make node count of nodes to 0 and set the flag */
				if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
				{
					*pu32NodeCnt = 0;
					bReadRangeFlag = TRUE;
				}

                while((NULL != pstListOfBACnetRecipient) && (i32APDU_Len < u32MaxInBufSize))
                {
					/* save previous encoded data length */
					i32PrevLen = i32APDU_Len;
                    /* Check MAC Len is non zero */
					if(pstListOfBACnetRecipient->m_stRecipient.m_eDestinationType ==
						DESTINATION_IS_IP_ADDR)
                    {
						/* encode opening tag - 1E */
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
                        /* Encode Network Number */
                        i32APDU_Len += Encode_Application_Unsigned
							(&u8TempAPDU[i32APDU_Len], pstListOfBACnetRecipient->m_stRecipient.
							BACnetRecipient_u.m_stAddress.u16net);
						/* set the mac length */
						stMACAdrss.m_u32OctetCount = pstListOfBACnetRecipient->m_stRecipient.
							BACnetRecipient_u.m_stAddress.u8mac_len;
						/* copy address */
						for(u8LoopCnt=0; u8LoopCnt <= stMACAdrss.m_u32OctetCount; u8LoopCnt++)
                        {
							stMACAdrss.m_ai8OctetStr[u8LoopCnt] = pstListOfBACnetRecipient->m_stRecipient.
								BACnetRecipient_u.m_stAddress.u8IpAddrs[u8LoopCnt];
                        }
                        /* Encode MAC address */
                        i32APDU_Len += Encode_Application_Octet_String
							(&u8TempAPDU[i32APDU_Len], &stMACAdrss);
						/* encode closing tag - 1F */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
                    }
                    else
                    {
                        /* Encode Object Identifier */
                        i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 
							TAG_NO_0, (int32_t)pstListOfBACnetRecipient->m_stRecipient.
							BACnetRecipient_u.m_stObjId.m_eObjectType,
							pstListOfBACnetRecipient->m_stRecipient.BACnetRecipient_u.
							m_stObjId.m_u32ObjId);
                    }
                    /* Assign next recipient node */
					pstListOfBACnetRecipient = pstListOfBACnetRecipient->m_pstNext;

					/* if encode function is called from ReadRange-B service, save the length of
					   each node in array */
					if(bReadRangeFlag)
					{
						if(*pu32NodeCnt < i32NodeLenArrSize)
						{
							pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32APDU_Len - i32PrevLen);
							(*pu32NodeCnt)++;
						}
					}
                }// End of while(pstRecipient)
            }
            break;

            case BACNET_DT_BOOLEAN_ARRAY:
            {
				Pr_ListOfBoolen_t *pstBoolArray = NULL;
                ListOfBoolen_t *pstBoolList = NULL;
				uint8_t u8ByteCnt = 0;

                pstBoolArray = application_data;
				pstBoolList = pstBoolArray->m_pstBoolList;
                
				/* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
					/* To send error in case the value to be reported is 
					* greater than the APDU limit.
					*/
					while((NULL != pstBoolList) && (i32APDU_Len < u32MaxInBufSize))
					{
						/* Encode value */
						i32APDU_Len +=
                        Encode_Application_Boolean(&u8TempAPDU[i32APDU_Len], pstBoolList->m_bVal);

						/* Get pointer to next value */
						pstBoolList = pstBoolList->m_pstNext;
					}
				}
				else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
                        &u8TempAPDU[i32APDU_Len], pstBoolArray->m_u32BoolCount);
                }
                else
                {
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstBoolList)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next boolean value */
                        pstBoolList = pstBoolList->m_pstNext;
                    }
                    if(NULL != pstBoolList)
                    {
                        /* encode boolean value */
                        i32APDU_Len +=
                        Encode_Application_Boolean(&u8TempAPDU[i32APDU_Len], pstBoolList->m_bVal);
                    }
                }
            }
			break;

			case BACNET_DT_DEVOBJPROPREF:
            {
                Pr_BACnetDevObjPropRef_t 	*pstObjPropReffEncode;
				pstObjPropReffEncode = (Pr_BACnetDevObjPropRef_t *)application_data;

                /* object id */
                i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 0,
					(int32_t)pstObjPropReffEncode->m_stDevObjPropReff.m_eObjectType,
					pstObjPropReffEncode->m_stDevObjPropReff.m_u32ObjId);
				/* Encode Property Identifier */            
				i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
					pstObjPropReffEncode->m_stDevObjPropReff.m_ePropertyIdentifier);
				/* array index */
                if(ARRAY_INDEX_PRESENT == pstObjPropReffEncode->m_stDevObjPropReff.m_bArrIndxPresent)
                {
                    i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
						TAG_NO_2, pstObjPropReffEncode->m_stDevObjPropReff.m_u32ArrayIndex);
                }

				/* device id & type */
				if(true == pstObjPropReffEncode->m_stDevObjPropReff.m_bDeviceIdPresent)
				{
					i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 
						TAG_NO_3, (int32_t)OBJECT_DEVICE, 
						pstObjPropReffEncode->m_stDevObjPropReff.m_u32DeviceInstace);
				}
            }break;

			case BACNET_DT_EVENTPARAMETERS:
            {
                Pr_BACnetEventParameter_t *pstEventParaEncode;
				pstEventParaEncode = application_data;

				switch(pstEventParaEncode->m_stEventParam.m_eEventType)
				{
				case EVENT_CHANGE_OF_BITSTRING:
					{
                        ListOfBitStr_t  *pstBitStrList = NULL;

                        /* Encode Opening Tag for change of bitstring */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);
                        /* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stCngBitstring.m_u32Timedelay);
                        /* Encode bit mask */
						i32APDU_Len += Encode_Context_BITString(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							&pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stCngBitstring.m_stBitmask.
                            m_stBitString);
                        /* Encode Opening Tag for list of  of bitstring */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
                        /* Encode list of bitstring values */
                        pstBitStrList = pstEventParaEncode->m_stEventParam
							.BACnetEventParameter_u.m_stCngBitstring.m_pstBitStrList;
                        while((NULL != pstBitStrList) && (i32APDU_Len < u32MaxInBufSize))
                        {
						    i32APDU_Len += Encode_Application_BITstring(&u8TempAPDU[i32APDU_Len],
                                &pstBitStrList->m_stBitString);
                            pstBitStrList = pstBitStrList->m_pstNext;
                        }
                        /* Encode Closing Tag for list of  of bitstring */ 
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
                        /* Encode Closing Tag for change of bitstring */ 
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);
					}
					break;

					case EVENT_OUT_OF_RANGE:
					{
						/* Encode Opening Tag for out-of-range */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_5);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stOutRange.m_u32Timedelay);                
						/* Encode low-limit */ 
						i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stOutRange.m_fLow_Limit);
						/* Encode high-limit */ 
						i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stOutRange.m_fHigh_Limit);
						/* Encode  deadband */ 
						i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
						pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
						.m_stOutRange.m_fDeadband);
						 /* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_5);
					}
					break;

					case EVENT_CHANGE_OF_VALUE:
					{
						/* Encode Opening Tag for change-of-value*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stCngValue.m_u32Timedelay);                
                        /* Encode Opening Tag for cov criteria */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
                        if(!pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCngValue.m_i8CriteriaType)
                        {
                            /* Encode bit mask */ 
						    i32APDU_Len += Encode_Context_BITString(&u8TempAPDU[i32APDU_Len], 
                                TAG_NO_0, &pstEventParaEncode->m_stEventParam
								.BACnetEventParameter_u.m_stCngValue.m_stCovCriteria
								.m_stBitmask.m_stBitString);
                        }
                        else
                        {
                            /* Encode referenced-property-increment */ 
						    i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], 
                                TAG_NO_1, pstEventParaEncode->m_stEventParam
								.BACnetEventParameter_u.m_stCngValue.m_stCovCriteria
								.m_fRefPropIncr);
                        }
                        /* Encode Closing Tag for cov criteria */ 
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
					}
					break;

					case EVENT_CHANGE_OF_STATE:
					{
                        ListOfBACnetPropertyStates_t *pstListOfValues = NULL;
						/* Encode Opening Tag for change-of-state*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCngState.m_u32Timedelay);                
						/* Encode  list-of-values */ 
                        i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_1);
                        pstListOfValues = pstEventParaEncode->m_stEventParam
							.BACnetEventParameter_u.m_stCngState.m_pstListOfValues;
                        while(NULL != pstListOfValues)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                                pstListOfValues->m_stPropStateVal.m_ePropState,
                                pstListOfValues->m_stPropStateVal.BACnetPropertyStates_U.
                                m_BinaryValue);
                            pstListOfValues = pstListOfValues->m_pstNext;
                        }
                        i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_1);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
					}
					break;

					case EVENT_COMMAND_FAILURE:
					{
						/* Encode Opening Tag for command-failure*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_3);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
								pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
								.m_stCmdFail.m_u32Timedelay); 
						/* Encode Opening Tag for feedback-property-reference*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode Object Identifier */
						i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							(int32_t)pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCmdFail.m_stFeedbackPropertyReference.m_eObjectType,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCmdFail.m_stFeedbackPropertyReference.m_u32ObjId);
						 /* Encode Property Identifier */
						i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
									pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
									.m_stCmdFail.m_stFeedbackPropertyReference.m_ePropertyIdentifier);
                        if(ARRAY_INDEX_PRESENT == pstEventParaEncode->m_stEventParam
							.BACnetEventParameter_u.m_stCmdFail.m_stFeedbackPropertyReference
							.m_bArrIndxPresent)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                                pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
								.m_stCmdFail.m_stFeedbackPropertyReference.m_u32ArrayIndex);
                        }
						/* encode device ID */
                        if(true == pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stCmdFail.
                            m_stFeedbackPropertyReference.m_bDeviceIdPresent)
                        {
                            i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
                                (int32_t)pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stCmdFail.
								m_stFeedbackPropertyReference.m_eDeviceType,
                                pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stCmdFail.
                                m_stFeedbackPropertyReference.m_u32DeviceInstace);
                        }
						 /* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_3);
					}
					break;

					case EVENT_FLOATING_LIMIT:
					{
						/* Encode Opening Tag for floating-limit*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_4);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stFlotLimit.m_u32Timedelay); 
						/* Encode Opening Tag for setpoint-reference*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode Object Identifier */
						i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							(int32_t)pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stFlotLimit.m_stSetpointReference.m_eObjectType,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stFlotLimit.m_stSetpointReference.m_u32ObjId);
						 /* Encode Property Identifier */
						i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
									pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
									.m_stFlotLimit.m_stSetpointReference.m_ePropertyIdentifier);
                        /* encode optional array index */
                        if(ARRAY_INDEX_PRESENT == pstEventParaEncode->m_stEventParam
							.BACnetEventParameter_u.m_stFlotLimit.m_stSetpointReference.m_bArrIndxPresent)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
							    pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stFlotLimit.
                                m_stSetpointReference.m_u32ArrayIndex); 
                        }

						 /* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode low-diff-limit */
						i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stFlotLimit.m_fLow_Diff_Limit);
						 /* Encode high-diff-limit */
						i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
								pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
								.m_stFlotLimit.m_fHigh_Diff_Limit);
						 /* Encode deadband */
						i32APDU_Len +=  Encode_Context_Real(&u8TempAPDU[i32APDU_Len], TAG_NO_4,
								pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
								.m_stFlotLimit.m_fDeadband);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_4);
					}
					break;

					case EVENT_CHANGE_OF_LIFE_SAFETY:
					{
                        ListOfEnum_t *pstAlarmValues = NULL;
						/* Encode Opening Tag for change-of-life-safety*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_8);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCngLifeSafety.m_u32Timedelay); 
						/* Encode Opening Tag for list-of-life-safety-alarm-values*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Get pointer add in local vari */
						pstAlarmValues = pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngLifeSafety.m_pstListOfLifeSafetyAlarmValues;
						while((NULL != pstAlarmValues) && (i32APDU_Len < u32MaxInBufSize))
						{
                            /* Alarm Enum Value */
							i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len],
                                pstAlarmValues->m_eVal);
							/* Get pointer to next value */
							pstAlarmValues = pstAlarmValues->m_pstNext;
						}						
						 /* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode Opening Tag for list-of-alarm-values*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
						/* Get pointer add in local vari */
						pstAlarmValues = pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngLifeSafety.m_pstListOfAlarmValues;
						while((NULL != pstAlarmValues) && (i32APDU_Len < u32MaxInBufSize))
						{
							i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len],
                                pstAlarmValues->m_eVal);
							/* Get pointer to next value */
							pstAlarmValues = pstAlarmValues->m_pstNext;
						}						
						 /* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
						/* Encode Opening Tag for mode-property-reference*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_3);
						/* Encode Object Identifier */
						i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							(int32_t)pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCngLifeSafety.m_stModePropReff.m_eObjectType,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCngLifeSafety.m_stModePropReff.m_u32ObjId);
							/* Encode Object Identifier */
						i32APDU_Len += Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stCngLifeSafety.m_stModePropReff.m_ePropertyIdentifier);
                        /* encode optional array index */
                        if(ARRAY_INDEX_PRESENT == pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngLifeSafety.m_stModePropReff.m_bArrIndxPresent)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                                TAG_NO_2, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                                m_stCngLifeSafety.m_stModePropReff.m_u32ArrayIndex); 
                        }

						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_3);
						 /* Encode the application closing tag */	
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_8);
					}
					break;

                    /* TODO - encoding is incomplete */
					case EVENT_EXTENDED:
					{
						/* Encode Opening Tag for extended*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_9);
						/* Encode vendor id */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u
							.m_stExtended.m_u32VendorId);
						/* Encode extended Event Type */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stExtended.
							m_u32ExtendedEventType);
						/* Encode Opening Tag for parameters */
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode parameters - TODO */
						/* Encode the application closing tag */	
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode the application closing tag */	
						i32APDU_Len +=Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_9);
					}
					break;

					case EVENT_BUFFER_READY:
					{		
						/* Encode Opening Tag for buffer-ready*/
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_10);
						/* Encode notification-threshold*/
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stBuffReady.
                            m_u32NotificationThreshold);
						/* Encode previous-notification-count*/
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stBuffReady.
                            m_u32PreviousNotificationCount);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_10);
					}
					break;

                    case EVENT_UNSIGNED_RANGE:
                    {
                        /* Encode Opening Tag for unsigned range */
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_11);
						/* Encode time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stUnsiRange.m_u32Timedelay);
                        /* Encode low limit */
                        i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stUnsiRange.m_u32LowLimit);
                        /* Encode high limit */
                        i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stUnsiRange.m_u32HighLimit);
						/* Encode the closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_11);
                    }
                    break;

                    case EVENT_DOUBLE_OUT_OF_RANGE:
					{
						/* Encode Opening Tag for double-out-of-range */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_14);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_0, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stDoubleOutofRange.m_u32Timedelay);                
						/* Encode low-limit */ 
						i32APDU_Len += Encode_Context_Double(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_1, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stDoubleOutofRange.m_dLow_Limit);
						/* Encode high-limit */ 
						i32APDU_Len += Encode_Context_Double(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_2, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stDoubleOutofRange.m_dHigh_Limit);
						/* Encode  deadband */ 
						i32APDU_Len += Encode_Context_Double(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_3, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stDoubleOutofRange.m_dDeadband);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_14);
					}
					break;

                    case EVENT_SIGNED_OUT_OF_RANGE:
					{
						/* Encode Opening Tag for signed-out-of-range */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_15);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_0, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stSignedOutofRange.m_u32Timedelay);                
						/* Encode low-limit */ 
						i32APDU_Len += Encode_Context_Signed(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_1, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stSignedOutofRange.m_i32Low_Limit);
						/* Encode high-limit */ 
						i32APDU_Len += Encode_Context_Signed(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_2, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stSignedOutofRange.m_i32High_Limit);
						/* Encode  deadband */ 
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_3, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stSignedOutofRange.m_u32Deadband);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_15);
					}
					break;

                    case EVENT_UNSIGNED_OUT_OF_RANGE:
					{
						/* Encode Opening Tag for unsigned-out-of-range */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_16);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stUnSignedOutofRange.m_u32Timedelay);                
						/* Encode low-limit */ 
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_1, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stUnSignedOutofRange.m_u32Low_Limit);
						/* Encode high-limit */ 
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_2, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stUnSignedOutofRange.m_u32High_Limit);
						/* Encode  deadband */ 
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                            TAG_NO_3, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                        m_stUnSignedOutofRange.m_u32Deadband);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_16);
					}
					break;

                    case EVENT_ACCESS_EVENT:
                    {
                        ListOfEnum_t *pstAccessEvent = NULL;
                        /* Encode Opening Tag for access-event */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_13);
                        /* Encode Opening Tag for list-of-access-event */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);
                        /* encode list of access events */
                        pstAccessEvent = pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stAccessEvent.m_pstAccessEventList;
                        while((NULL != pstAccessEvent) && (i32APDU_Len < u32MaxInBufSize))
                        {
                            i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len],
                                pstAccessEvent->m_eVal);
                            pstAccessEvent = pstAccessEvent->m_pstNext;
                        }
                        /* Encode closing tag for list-of-access-event */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);
                        /* Encode Opening Tag for access-event-time-reference */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
                        /* encode Object Identifier */
						i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							(int32_t)pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stAccessEvent.
							m_stAccessEventTimeReff.m_eObjectType,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stAccessEvent.
                            m_stAccessEventTimeReff.m_u32ObjId);
						/* encode property identifier */
						i32APDU_Len += Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.m_stAccessEvent.
                            m_stAccessEventTimeReff.m_ePropertyIdentifier);
                        /* encode optional array index */
                        if(ARRAY_INDEX_PRESENT == pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stAccessEvent.m_stAccessEventTimeReff.m_bArrIndxPresent)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], 
                                TAG_NO_2, pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                                m_stAccessEvent.m_stAccessEventTimeReff.m_u32ArrayIndex); 
                        }

                        /* encode optional device id */
						i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
							(int32_t)pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
							m_stAccessEvent.m_stAccessEventTimeReff.m_eDeviceType,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stAccessEvent.m_stAccessEventTimeReff.m_u32DeviceInstace);

                        /* Encode closing tag for access-event-time-reference */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_13);
                    }
                    break;

                    case EVENT_CHANGE_OF_CHARACTERSTRING:
					{
                        ListOfCharStr_t  *pstAlarmValues = NULL;
						/* Encode Opening Tag for change of character string */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_17);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngCharString.m_u32Timedelay);
                        /* Encode opening tag for list-of-alarm-values */
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
                        /* encode list of alarm values */
                        pstAlarmValues = pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngCharString.m_pstListOfAlarmValues;
                        while((NULL != pstAlarmValues) && (i32APDU_Len < u32MaxInBufSize))
                        {
                            i32APDU_Len += Encode_Application_Character_String(
                                &u8TempAPDU[i32APDU_Len], &pstAlarmValues->m_stCharStr);
                            pstAlarmValues = pstAlarmValues->m_pstNext;
                        }
                        /* Encode closing tag for list-of-alarm-values */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_17);
					}
					break;

                    case EVENT_CHANGE_OF_STATUS_FLAGS:
					{
						/* Encode Opening Tag for change of status flag */ 
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_18);
						/* Encode Time delay */
						i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
							pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngStatusFlag.m_u32Timedelay);                
						/* Encode status flag */ 
                        i32APDU_Len += Encode_Context_BITSTRING(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							&pstEventParaEncode->m_stEventParam.BACnetEventParameter_u.
                            m_stCngStatusFlag.m_stStatusFlag);
						/* Encode the application closing tag */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_18);
					}
					break;

                    default:
                        break;
				}//event para switch
            }
			break;

#ifdef BACDEL_OBJ_CAL
            case BACNET_DT_DATELIST:
            {
                Pr_ListOfBACnetCalendarEntry_t   *pstDateList;
                ListOfBACnetCalendarEntry_t        *pstListOfCalendar;		        
                BACnetOctetStr_t     stBACNET_OCTET_STRING = {0};

                pstDateList = application_data;
                /*get Calendar Entry to be encode*/
                pstListOfCalendar = pstDateList->m_pstListOfCalendar;

				/* if encode function is called from ReadRange-B service
				   make node count of nodes to 0 and set the flag */
				if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
				{
					*pu32NodeCnt = 0;
					bReadRangeFlag = TRUE;
				}
                while(NULL != pstListOfCalendar && i32APDU_Len < u32MaxInBufSize)
                {
					/* save previous encoded data length */
					i32PrevLen = i32APDU_Len;
					/*Date*/
                    if(pstListOfCalendar->m_eStatusCalendar == STATUS_DATE)
                    {   /*Encode date*/
                        i32APDU_Len += Encode_Context_Date(&u8TempAPDU[i32APDU_Len], TAG_NO_0, 
                          &(pstListOfCalendar->m_stCalendar.m_stDate));
                    }
                    /*Date Range*/
                    else if(pstListOfCalendar->m_eStatusCalendar == STATUS_DATE_RANGE)
                    {   /*encode opening tag*/
                        i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_1);
                        /*Encode start date*/
                        i32APDU_Len += Encode_Application_Date(&u8TempAPDU[i32APDU_Len], &(pstListOfCalendar->m_stCalendar.m_stDateRange.m_stStartDate));
                        /*Encode End Date*/
                        i32APDU_Len += Encode_Application_Date(&u8TempAPDU[i32APDU_Len], &(pstListOfCalendar->m_stCalendar.m_stDateRange.m_stEndDate));
                        /*Encode closeing tag*/
                        i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_1);
                    }
                    /*Week n Day*/
                    else if(pstListOfCalendar->m_eStatusCalendar == STATUS_WEEK_N_DAY)
                    {
						stBACNET_OCTET_STRING.m_ai8OctetStr[0] = pstListOfCalendar->m_stCalendar.m_stWeekNDay.m_eMonth;
						stBACNET_OCTET_STRING.m_ai8OctetStr[1] = pstListOfCalendar->m_stCalendar.m_stWeekNDay.m_eWeekOfMonth;
						stBACNET_OCTET_STRING.m_ai8OctetStr[2] = pstListOfCalendar->m_stCalendar.m_stWeekNDay.m_eWeekNDay;
						stBACNET_OCTET_STRING.m_u32OctetCount = 3;
                        i32APDU_Len += Encode_Context_Octet_String(&u8TempAPDU[i32APDU_Len],TAG_NO_2,
                            &stBACNET_OCTET_STRING);
                    }
                    /*get next data*/
                    pstListOfCalendar = pstListOfCalendar->m_pstNext;

					/* if encode function is called from ReadRange-B service, save the length of
					   each node in array */
					if(bReadRangeFlag)
					{
						if(*pu32NodeCnt < i32NodeLenArrSize)
						{
							pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32APDU_Len - i32PrevLen);
							(*pu32NodeCnt)++;
						}
					}
                }
            }
			break;
#endif	/* BACDEL_OBJ_CAL */

#if (defined BACDEL_OBJ_SDL)
            case BACNET_DT_DATERANGE:
            {
                Pr_BACnetDateRange_t       *pstEffectivePeriod;
                pstEffectivePeriod =   application_data;

                i32APDU_Len += Encode_Application_Date(&u8TempAPDU[i32APDU_Len], &(pstEffectivePeriod->m_stDateRange.m_stStartDate));
                        /*Encode End Date*/
                i32APDU_Len += Encode_Application_Date(&u8TempAPDU[i32APDU_Len], &(pstEffectivePeriod->m_stDateRange.m_stEndDate));

            }
            break;

			case BACNET_DT_DEVOBJPROPREFF_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetDevObjPropRef_t    *pstListOfObjectPropertyReference;
                ListOfBACnetDevObjPropRef_t        *pstListOfBACnetDevObjPropReff;
				uint8_t u8ByteCnt = 0;

				/* get the value */
                pstListOfObjectPropertyReference = (Pr_ListOfBACnetDevObjPropRef_t*)application_data;
                pstListOfBACnetDevObjPropReff = pstListOfObjectPropertyReference->m_pstListOfBACnetDevObjPropReff;

				/* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
					/* To send error in case the value to be reported is 
					* greater than the APDU limit. */
					/* encode all elements in list & check max length is not exceeded */
					while((NULL != pstListOfBACnetDevObjPropReff) && (i32APDU_Len < u32MaxInBufSize))
					{
						i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 0,
							(int32_t)pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_eObjectType,
							pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ObjId);

						/* Encode Property Identifier */            
						i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_ePropertyIdentifier);
						/* check for array */
						if(ARRAY_INDEX_PRESENT == pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bArrIndxPresent)
						{
							i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
							pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ArrayIndex);
						}	
#ifndef INTRL_SHED_ONLY
						/* check for Device ID */
						if(true == pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bDeviceIdPresent)
						{
							i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
								(int32_t)OBJECT_DEVICE,
							pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32DeviceInstace);
						}
#endif
						pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;
					}
				}
				else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
                        &u8TempAPDU[i32APDU_Len], pstListOfObjectPropertyReference->m_u32ArraySize);
                }
				else
                {
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstListOfBACnetDevObjPropReff)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next boolean value */
                        pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;
                    }
                    if(NULL != pstListOfBACnetDevObjPropReff)
                    {
                        i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 0,
						(int32_t)pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_eObjectType,
                        pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ObjId);

						/* Encode Property Identifier */            
						i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
								pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_ePropertyIdentifier);
						/* check for array */
						if(ARRAY_INDEX_PRESENT == pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bArrIndxPresent)
						{
							i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
							pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ArrayIndex);
						}	
#ifndef INTRL_SHED_ONLY
						/* check for Device ID */
						if(true == pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bDeviceIdPresent)
						{
							i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
								(int32_t)OBJECT_DEVICE,
							pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32DeviceInstace);
						}
#endif
						pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;
					}
                }
			}
			break;

            case BACNET_DT_DEVOBJPROPREFF_LIST:
            {
                Pr_ListOfBACnetDevObjPropRef_t    *pstListOfObjectPropertyReference;
                ListOfBACnetDevObjPropRef_t        *pstListOfBACnetDevObjPropReff;

                pstListOfObjectPropertyReference =   application_data;
                pstListOfBACnetDevObjPropReff = pstListOfObjectPropertyReference->m_pstListOfBACnetDevObjPropReff;

				/* if encode function is called from ReadRange-B service 
				   make node count of nodes to 0 and set the flag */
				if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
				{
					*pu32NodeCnt = 0;
					bReadRangeFlag = TRUE;
				}

                while(NULL != pstListOfBACnetDevObjPropReff && i32APDU_Len < u32MaxInBufSize)
                {
					/* save previous encoded data length */
					i32PrevLen = i32APDU_Len;
                    i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 0,
						(int32_t)pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_eObjectType,
                        pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ObjId);

					/* Encode Property Identifier */            
                    i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
								pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_ePropertyIdentifier);
                    /* check for array */
                    if(ARRAY_INDEX_PRESENT == pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bArrIndxPresent)
                    {
                        i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                        pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ArrayIndex);
                    }	
#ifndef INTRL_SHED_ONLY
                    /* check for Device ID */
                    if(true == pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bDeviceIdPresent)
                    {
			            i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
						(int32_t)OBJECT_DEVICE,
                        pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32DeviceInstace);
                    }
#endif
                    pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;

					/* if encode function is called from ReadRange-B service, save the length of
					   each node in array */
					if(bReadRangeFlag)
					{
						if(*pu32NodeCnt < i32NodeLenArrSize)
						{
							pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32APDU_Len - i32PrevLen);
							(*pu32NodeCnt)++;
						}
					}
                }
            }
            break;

			/* case added for encoding single node of weekly schedule */
			case BACNET_DT_DAILYSCHEDULE:
            {
                Pr_BACnetDailySchedule_t *pstDaySchedule = NULL;
                BACnetTimeValue_t *pstScheduleTimeValue = NULL;

                pstDaySchedule = (Pr_BACnetDailySchedule_t *)application_data;
                /* Encode Opening Tag */
		        i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_0);

                pstScheduleTimeValue = &(pstDaySchedule->m_stTimeValue);
                if(pstScheduleTimeValue->bIsUsed)
                {
                    while(NULL != pstScheduleTimeValue && i32APDU_Len < u32MaxInBufSize)
                    {
                        i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len], 
                                        &(pstScheduleTimeValue->m_stTime));
                            
                        i32APDU_Len += BACApp_Encode_Application_Data(&u8TempAPDU[i32APDU_Len],
                                        &(pstScheduleTimeValue->m_stPropValue));

                        pstScheduleTimeValue = pstScheduleTimeValue->m_pstNext;
                    }
                }

                /* Encode the application closing tag */
		        i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_0);                
            }
            break;

            case BACNET_DT_DAILYSCHEDULE_ARRAY:
            {
                Pr_ListOfBACnetDailySchedule_t   *pstWeeklySchedule;
                int32_t                 i32count = 0;
                int32_t                 i32Startcount = 0;
                int32_t                 i32ArrayCount = BACNET_ARRAY_OF_SEVEN-1;
                BACnetTimeValue_t       *pstScheduleTimeValue;

                pstWeeklySchedule =     application_data;

                if(0 == u32ArrayIndex && ARRAY_INDEX_PRESENT == bArrIndxPresent)
                {
                     i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], BACNET_ARRAY_OF_SEVEN);
                }
                else
                {
                    if(0 < u32ArrayIndex && BACNET_ARRAY_OF_SEVEN >= u32ArrayIndex && ARRAY_INDEX_PRESENT == bArrIndxPresent)
                    {
                        i32ArrayCount = u32ArrayIndex-1;
                        i32Startcount = u32ArrayIndex-1;
                        i32count = u32ArrayIndex - 2;
                    }
                    else
                    {
                        i32ArrayCount = BACNET_ARRAY_OF_SEVEN-1;
                        i32Startcount = 0;
                    }

                    for(i32count = i32Startcount; i32count <= i32ArrayCount; i32count++)
                    {
                        /* Encode Opening Tag */
		                i32APDU_Len +=Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_0);

                        pstScheduleTimeValue = &(pstWeeklySchedule->m_stTimeValue[i32count]);
                        if(pstScheduleTimeValue->bIsUsed)
                        {
                            while(NULL != pstScheduleTimeValue && i32APDU_Len < u32MaxInBufSize)
                            {
                                i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len], 
                                               &(pstScheduleTimeValue->m_stTime));
                            
                                i32APDU_Len += BACApp_Encode_Application_Data(&u8TempAPDU[i32APDU_Len],
                                               &(pstScheduleTimeValue->m_stPropValue));

                                pstScheduleTimeValue = pstScheduleTimeValue->m_pstNext;
                            }
                        }
                        /* Encode the application closing tag */
		                i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_0);
                    } 
                }
            }
            break;

			/* case added for encoding single node of special event array*/
			case BACNET_DT_SPECIALEVENT:
            {
                Pr_ListOfBACnetSpecialEvent_t  *pstExceptionSchedule = NULL;
				ListOfSpecialEvent_t  *pstSpecialEvent = NULL;

                pstExceptionSchedule = (Pr_ListOfBACnetSpecialEvent_t *)application_data;
				if(NULL != pstExceptionSchedule)
					pstSpecialEvent = pstExceptionSchedule->m_pstSplEvent;
				if(NULL != pstSpecialEvent)
				{
					i32APDU_Len +=  DT_Encode_SpecialEvent(&u8TempAPDU[0], 
						&pstSpecialEvent->m_stListSpecialEvent);
				}
            }
            break;

            case BACNET_DT_SPECIALEVENT_ARRAY:
            {
                Pr_ListOfBACnetSpecialEvent_t      *pstExceptionSchedule = NULL;
                ListOfSpecialEvent_t            *pstSplEvent = NULL;
                BACnetTimeValue_t               *pstListOfTimeValues = NULL;
                BACnetOctetStr_t             	stBACNET_OCTET_STRING = {0};
                uint32_t u32arraycount = 0;


                pstExceptionSchedule = application_data;
                pstSplEvent = pstExceptionSchedule->m_pstSplEvent;

                if(0 == u32ArrayIndex && ARRAY_INDEX_PRESENT == bArrIndxPresent)
                {
                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
                                    pstExceptionSchedule->m_u32Count);
                }
                else
                {
                    if(ARRAY_INDEX_PRESENT == bArrIndxPresent)
                    {              
                        u32arraycount = 1;
                        while(u32ArrayIndex != u32arraycount && NULL != pstSplEvent)
                        {
                            u32arraycount++;
                            pstSplEvent = pstSplEvent->m_pstNext;
                        }
                        if(NULL == pstSplEvent)
                        {
                        }
                     }

                    while(NULL!= pstSplEvent && i32APDU_Len < u32MaxInBufSize)
                    {   /* period */
                        if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar  != STATUS_CAL_REFF)
                        {
                            i32APDU_Len +=Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_0);                        
                            if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar  == STATUS_DATE)
                            {   /*Encode date*/
                                i32APDU_Len += Encode_Context_Date(&u8TempAPDU[i32APDU_Len], TAG_NO_0, 
                                    &(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDate));
                            }
                            /*Date Range*/
                            else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar  == STATUS_DATE_RANGE)
                            {   /*encode opening tag*/
                                i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_1);
                                /*Encode start date*/
                                i32APDU_Len += Encode_Application_Date(&u8TempAPDU[i32APDU_Len], &(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stStartDate));
                                /*Encode End Date*/
                                i32APDU_Len += Encode_Application_Date(&u8TempAPDU[i32APDU_Len], &(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stEndDate));
                                /*Encode closeing tag*/
                                i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_1);
                            }
                            /*Week n Day*/
                            else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_WEEK_N_DAY)
                            {
								stBACNET_OCTET_STRING.m_ai8OctetStr[0] = pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay.m_eMonth;
								stBACNET_OCTET_STRING.m_ai8OctetStr[1] = pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay.m_eWeekOfMonth;
								stBACNET_OCTET_STRING.m_ai8OctetStr[2] = pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay.m_eWeekNDay;
								stBACNET_OCTET_STRING.m_u32OctetCount = 3;
                                 i32APDU_Len += Encode_Context_Octet_String(&u8TempAPDU[i32APDU_Len],TAG_NO_2,
                                    &stBACNET_OCTET_STRING);
                            }
                            i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_0);
                        }
                        else
                        {   /* Calendar reference */
                            i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len],TAG_NO_1,
								(int32_t)pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stCalReff.m_eObjectType,
                                pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stCalReff.m_u32ObjId);
                        }
                        /* List of bacnetTimeValues */
                        i32APDU_Len +=Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_2);
                        pstListOfTimeValues = &(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues);
                        if(pstListOfTimeValues->bIsUsed)
                        {
                            while(NULL != pstListOfTimeValues)
                            {  
                            
                                /* Time */
                                i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len], 
                                               &(pstListOfTimeValues->m_stTime));
                                /* Values */
                                i32APDU_Len += BACApp_Encode_Application_Data(&u8TempAPDU[i32APDU_Len],
                                               &(pstListOfTimeValues->m_stPropValue));

                                pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
                            }
                        }
                        i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len],TAG_NO_2);
                        /* Event Priority */
                        i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_3,
                            pstSplEvent->m_stListSpecialEvent.m_u32EventPriority);
                        if(ARRAY_INDEX_ABSENT == bArrIndxPresent)
                        { 
                            pstSplEvent = pstSplEvent->m_pstNext;
                        }
                        else
                            break;
                    }
                }
            }
            break;

            case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
            {
                AnyValue_t   *pstPresentDefaultValue;              
                pstPresentDefaultValue =   application_data;                
                i32APDU_Len += BACApp_Encode_Application_Data(&u8TempAPDU[i32APDU_Len],
					&(pstPresentDefaultValue->m_stValue));
            }
            break;
#endif	/* #if (defined BACDEL_OBJ_SDL) */

            case BACNET_DT_TIMESTAMP: 
			{
                /* get the time stamp value to encode */
                Pr_BACnetTimeStamp_t *pstTimeStamp = NULL;
                pstTimeStamp = (Pr_BACnetTimeStamp_t *)application_data;

				i32APDU_Len = DT_Encode_TimeStamp(&u8TempAPDU[0], 
					&pstTimeStamp->m_stTimeStamp.m_stTimeStamp,
					pstTimeStamp->m_stTimeStamp.m_eTimeStampType); 
			}
			break;
			
			/* unsigned array of 3 for property - priority in NC obj */
			case BACNET_DT_NOTIFICATIONPRIORITY:
            {
                uint8_t u8Count = 0;
                /* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    /* Send First string for All request or for index 1 */
                    for(u8Count = 0; u8Count < BACNET_ARRAY_OF_THREE; u8Count++)
                    {
                        i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len],
                          ((Pr_BACnetNotifyPriority_t *)application_data)->m_u32Value[u8Count]);
                    }
                }
                else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len],
                        BACNET_ARRAY_OF_THREE);
                }
                else
                {
                    /* encode value @ given array index */
                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len],
                          ((Pr_BACnetNotifyPriority_t *)application_data)->
                          m_u32Value[u32ArrayIndex-1]);
                }
            }
            break;

			/* Recipient list for NC object */
			case BACNET_DT_DESTINATION_LIST:
			{
				Pr_ListOfBACnetDestination_t *pstRecipient = NULL;
				ListOfBACnetDestination_t *pstNCRecepient;
				BACnetOctetStr_t stMACAdrss = {0};
				uint8_t u8LoopCnt = 0;

				pstRecipient = (Pr_ListOfBACnetDestination_t *)application_data;
				pstNCRecepient = pstRecipient->m_pstNCRecepient;

				/* if encode function is called from ReadRange-B service 
				   make node count of nodes to 0 and set the flag */
				if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
				{
					*pu32NodeCnt = 0;
					bReadRangeFlag = TRUE;
				}
				/* traverse the list of destinations and encode i.e. recipient list */
				while((pstNCRecepient != NULL) && (i32APDU_Len < u32MaxInBufSize))
				{
					/* save previous encoded data length */
					i32PrevLen = i32APDU_Len;
					/* encode days of week */
					i32APDU_Len += Encode_Application_Bitstring_small(&u8TempAPDU[i32APDU_Len], 
						&pstNCRecepient->m_stDaysOfWeek);

					/* encode from time */
					i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len],
										&pstNCRecepient->m_stFromTime);
					/* encode To time */
					i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len],
										&pstNCRecepient->m_stToTime);

					/* check if recipient is device id */
                    if(DESTINATION_IS_DEVICE_ID == 
						pstNCRecepient->m_stRecipient.m_eDestinationType)
					{
						/* encode the object id & type */
						i32APDU_Len += Encode_Context_Object_Id
								(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
								(int32_t)pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stObjId.m_eObjectType,
								pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stObjId.m_u32ObjId);
					}
					else
					{
						/* encode opening tag for network-no & MAC address */
						i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
						/* encode network number */
						i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
							pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net);
						/* */
						stMACAdrss.m_u32OctetCount = pstNCRecepient->m_stRecipient.BACnetRecipient_u.
							m_stAddress.u8mac_len;
						for( u8LoopCnt=0; u8LoopCnt <=stMACAdrss.m_u32OctetCount; u8LoopCnt++)
						{
							stMACAdrss.m_ai8OctetStr[u8LoopCnt] = pstNCRecepient->m_stRecipient.
							BACnetRecipient_u.m_stAddress.u8IpAddrs[u8LoopCnt];
						}
						/* encode MAC address */
						i32APDU_Len += Encode_Application_Octet_String
							(&u8TempAPDU[i32APDU_Len], &stMACAdrss);
						/* encode closing tag for network-no & MAC address */
						i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
					}
					
					/* encode process id */
					i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len],
										pstNCRecepient->m_stProcessId);

					/* encode issue confirmed notification */
					i32APDU_Len += Encode_Application_Boolean(&u8TempAPDU[i32APDU_Len], 
								pstNCRecepient->m_bIssueConfirmedNotification);

					/* encode transition */
					i32APDU_Len += Encode_Application_Bitstring_small(&u8TempAPDU[i32APDU_Len], 
						&pstNCRecepient->m_stTransitions);

					/* move to next destination in list */
					pstNCRecepient = pstNCRecepient->m_pstNext;

					/* if encode function is called from ReadRange-B service, save the length of
					   each node in array */
					if(bReadRangeFlag)
					{
						if(*pu32NodeCnt < i32NodeLenArrSize)
						{
							pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32APDU_Len - i32PrevLen);
							(*pu32NodeCnt)++;
						}
					}
				}
			}break;

#ifndef PROFILE_BOD_ONLY
            case BACNET_DT_LOGBUFFER_TRENDLOG:
            {
				/* Local Variables */
                Pr_ListOfBACnetLogRecord_t *pstLogRecord = NULL;

				/* get the value */
                pstLogRecord = (Pr_ListOfBACnetLogRecord_t *)application_data;

				i32APDU_Len = 
					DT_Encode_TrendLogRec_List(&u8TempAPDU[0], 
											  pstLogRecord, 
											  u32MaxInBufSize,
											  pu16NodeLenArr,
											  pu32NodeCnt,
											  i32NodeLenArrSize);
            }
            break;
#endif /* !PROFILE_BOD_ONLY */

            case BACNET_DT_CLIENTCOVINCREMENT:
            {
                Pr_BACnetClientCOV_t *pstClienCov = NULL;
                pstClienCov = (Pr_BACnetClientCOV_t *)application_data;
                if(BACNET_APPLICATION_TAG_NULL == pstClienCov->m_stClientCOV.m_eAppTagtype)
                {
                    /* encode null value */
                    u8TempAPDU[0] = 0;
                    i32APDU_Len++;
                }
                else
                {
                    /* encode the real data_type value */
                    i32APDU_Len = Encode_Application_Real
                            (&u8TempAPDU[0], pstClienCov->m_stClientCOV.m_fVal);
                }
            }break;

            case BACNET_DT_OBJPROPREF:
            {
                Pr_BACnetObjPropRef_t 	*pstObjPropReffEncode;
				pstObjPropReffEncode = application_data;
                if(NULL != pstObjPropReffEncode)
                {
                    /* object id */
                    i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 0,
						(int32_t)pstObjPropReffEncode->m_stObjPropRef.m_eObjectType,
						pstObjPropReffEncode->m_stObjPropRef.m_u32ObjId);
				    /* Encode Property Identifier */            
				    i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
						pstObjPropReffEncode->m_stObjPropRef.m_ePropertyIdentifier);
				    /* array index */
                    if(ARRAY_INDEX_PRESENT == pstObjPropReffEncode->m_stObjPropRef.m_bArrIndxPresent)
                    {
                        i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                            pstObjPropReffEncode->m_stObjPropRef.m_u32ArrayIndex);
                    }
                }
            }
            break;

            case BACNET_DT_SETPOINTREF:
            {
                Pr_BACnetSetpointRef_t 	*pstSetPtReffEncode;
                pstSetPtReffEncode = application_data;

                if(NULL != pstSetPtReffEncode && pstSetPtReffEncode->m_stSptRef.m_bDataFlag == true)
                {  
                    i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);
                    /* object id */
                    i32APDU_Len += Encode_Context_Object_Id(&u8TempAPDU[i32APDU_Len], 0,
						(int32_t)pstSetPtReffEncode->m_stSptRef.m_stObjPropRef.m_eObjectType,
                        pstSetPtReffEncode->m_stSptRef.m_stObjPropRef.m_u32ObjId);
				    /* Encode Property Identifier */            
				    i32APDU_Len +=   Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
						pstSetPtReffEncode->m_stSptRef.m_stObjPropRef.m_ePropertyIdentifier);
				    /* array index */
                    if(ARRAY_INDEX_PRESENT == pstSetPtReffEncode->m_stSptRef.m_stObjPropRef.m_bArrIndxPresent)
                    {
                        i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                            pstSetPtReffEncode->m_stSptRef.m_stObjPropRef.m_u32ArrayIndex);
                    }
                   i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_0);
                }
            }
            break;



            case BACNET_DT_BITSTRING_ARRAY:
            {
                Pr_ListOfBitStr_t *pstBitStrArr = NULL;
                ListOfBitStr_t *pstBitString = NULL;
                uint8_t u8ByteCnt = 0;

                /* get the value */
                pstBitStrArr = application_data;
                pstBitString = pstBitStrArr->m_pstBitString;

                /* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    while(NULL != pstBitString && (i32APDU_Len < u32MaxInBufSize))
                    {
						/* encode value */
                        i32APDU_Len += Encode_Application_BITstring(&u8TempAPDU[i32APDU_Len], 
							&pstBitString->m_stBitString);
                        /* move to next bitstring value */
                        pstBitString = pstBitString->m_pstNext;
                    }
                }
                else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
                        &u8TempAPDU[i32APDU_Len], pstBitStrArr->m_u32Count);
                }
                else
                {
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstBitString)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next bitstring value */
                        pstBitString = pstBitString->m_pstNext;
                    }
                    if(NULL != pstBitString)
                    {
                        /* encode value */
                        i32APDU_Len = Encode_Application_BITstring(&u8TempAPDU[0], 
							&pstBitString->m_stBitString);
                    }
                }
            }break;

            case BACNET_DT_BACNETVTCLASS:
            {
                Pr_BACnetVTClass_t *pstVTClassSupported;

                pstVTClassSupported = application_data;

                do
                {
                    i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len], 
                        pstVTClassSupported->m_eVTClassSupport );

                    pstVTClassSupported = (Pr_BACnetVTClass_t*)pstVTClassSupported->m_pstNext;

                }while(NULL != pstVTClassSupported);
            }
            break;

            case BACNET_DT_BACNETVTSESS:
            {
                Pr_ListOfBACnetVTSession_t *pstActiveVTSessions = NULL;
				ListOfBACnetVTSession_t *pstListOfBACnetVTSession = NULL;
                BACnetOctetStr_t stMACAdrss = {0};
                uint8_t u8LoopCnt = 0;

				/* get the data */
                pstActiveVTSessions = application_data;
				pstListOfBACnetVTSession = pstActiveVTSessions->m_pstListOfVTSession;

                while((NULL != pstListOfBACnetVTSession) && (i32APDU_Len < u32MaxInBufSize))
                {
                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
						pstListOfBACnetVTSession->m_stVTSession.m_u8LocalVTSessionId );
                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
						pstListOfBACnetVTSession->m_stVTSession.m_u8RemoteVTSessionId );
                    i32APDU_Len += Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
						pstListOfBACnetVTSession->m_stVTSession.m_stRemoteVTAddress.u16net );

					stMACAdrss.m_u32OctetCount = pstListOfBACnetVTSession->m_stVTSession.
						m_stRemoteVTAddress.u8mac_len;
					for( u8LoopCnt=0; u8LoopCnt <= stMACAdrss.m_u32OctetCount; u8LoopCnt++)
					{
						stMACAdrss.m_ai8OctetStr[u8LoopCnt] = 
							pstListOfBACnetVTSession->m_stVTSession.
							m_stRemoteVTAddress.u8IpAddrs[u8LoopCnt];
					}

					/* encode MAC address */
					i32APDU_Len += Encode_Application_Octet_String(&u8TempAPDU[i32APDU_Len], &stMACAdrss);

					pstListOfBACnetVTSession = pstListOfBACnetVTSession->m_pstNext;
                }
            }
            break;

			case BACNET_DT_OPTIONAL_CHARSTRING:
            {
                ListOfOptCharStr_t   *pstOptCharStr = NULL;
                
                pstOptCharStr = ((Pr_ListOfOptCharStr_t*)application_data)->m_pstOptCharStr;

				/* encode optional character string */
				i32APDU_Len += DT_Encode_OptCharString(&u8TempAPDU[i32APDU_Len], pstOptCharStr);
			}
            break;
            
            case BACNET_DT_OPTCHARSTRING_ARRAY :
            {
                ListOfOptCharStr_t   *pstOptCharStr = NULL;
                uint8_t     u8ByteCnt = 0;
                pstOptCharStr = ((Pr_ListOfOptCharStr_t*)application_data)->m_pstOptCharStr;
                

                /* If none send , response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
                    while(NULL != pstOptCharStr && (i32APDU_Len < u32MaxInBufSize))
                    {
						/* encode optional character string */
						i32APDU_Len += DT_Encode_OptCharString(&u8TempAPDU[i32APDU_Len], pstOptCharStr);
                        
                        /* move to next element */ 
                        pstOptCharStr = pstOptCharStr->m_pstNext;
                    }
                }
                else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
                        &u8TempAPDU[i32APDU_Len], ((Pr_ListOfOptCharStr_t*)application_data)->m_u32Count);
                }
                else
                {
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstOptCharStr)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next bitstring value */
                        pstOptCharStr = pstOptCharStr->m_pstNext;
                    }
                    if(NULL != pstOptCharStr)
                    {
                        if(BACNET_APPLICATION_TAG_CHARACTER_STRING == pstOptCharStr->m_i8AppTag)
                        {
                            i32APDU_Len += Encode_Application_Character_String(
                                &u8TempAPDU[i32APDU_Len], &pstOptCharStr->m_stCharStr);
                        }
                        else if(BACNET_APPLICATION_TAG_NULL == pstOptCharStr->m_i8AppTag)
                        {
                            i32APDU_Len += Encode_Application_Null(&u8TempAPDU[i32APDU_Len]);
                        }
                        else;
                    }
                }
            }
            break;

			case BACNET_DT_ENUM_LIST: 
            {
				Pr_ListOfEnum_t *pstEnumList = NULL;
                ListOfEnum_t *pstEnumValue = NULL;
                pstEnumList = (Pr_ListOfEnum_t *)application_data;
				pstEnumValue = pstEnumList->m_pstEnumList;
                
                /* encode all elements in list & check max length is not exceeded */
                while((NULL != pstEnumValue) && (i32APDU_Len < u32MaxInBufSize))
                {
                    i32APDU_Len +=
						Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len], pstEnumValue->m_eVal);
                    pstEnumValue = pstEnumValue->m_pstNext;
                }
            }
            break;

			case BACNET_DT_DEVOBJREFF_LIST:
			{
				/* Local Variables */
				Pr_ListOfBACnetDevObjRef_t *pstDevObjReffList = NULL;
				/* get the value */
                pstDevObjReffList = (Pr_ListOfBACnetDevObjRef_t *)application_data;

                i32APDU_Len = DT_Encode_DevObjReff_List(&u8TempAPDU[0], 
					pstDevObjReffList, u32MaxInBufSize); 
			}
			break;

			/* unsigned array of 3 for property - Event_Msg_Text in LSP obj */	
			case BACNET_DT_EVENT_MSG_TEXT:
            {
				/* Local Variables */
				Pr_BACnetEventMsgText_t *pstEventMsgText = NULL;

				/* get the value */
				pstEventMsgText = (Pr_BACnetEventMsgText_t *)application_data;

                /* If none send, response with all */
                if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
                {
					i32APDU_Len = DT_Encode_EveMsgTxt_Array(&u8TempAPDU[0], pstEventMsgText);
                }
                else if(0 == u32ArrayIndex)
                {
                    /* encode the array size count */
                    i32APDU_Len = Encode_Application_Unsigned(
						&u8TempAPDU[i32APDU_Len], BACNET_ARRAY_OF_THREE);
                }
                else
                {
                    /* encode value @ given array index */
                    i32APDU_Len = Encode_Application_Character_String(&u8TempAPDU[0], 
						&pstEventMsgText->m_stEventMsgText[u32ArrayIndex-1]);
                }
            }
            break;

            case BACNET_DT_READACCESSSPECS_LIST:
            {
				/* Local Variables */
				Pr_ListOfReadAccessSpecs_t  *pstReadAccSpecif = NULL;
                ListOfReadAccessSpecs_t           *pstGpOfObj       = NULL;
                ListOfBACnetPropRef_t    *pstlistOfPropRef = NULL;
				
				/* Get Value */
				pstReadAccSpecif = (Pr_ListOfReadAccessSpecs_t*)application_data;
                pstGpOfObj = pstReadAccSpecif->m_pstGpOfObj;
                
				/* encode all elements in list & check max length is not exceeded */
                while(NULL != pstGpOfObj && (i32APDU_Len < u32MaxInBufSize))
                {   
					/* Encode obj id */
                    i32APDU_Len += Encode_Context_Object_Id(
						&u8TempAPDU[i32APDU_Len], TAG_NO_0,
						(int32_t)pstGpOfObj->m_eObjectType,
                         pstGpOfObj->m_u32ObjId);

                    pstlistOfPropRef = &(pstGpOfObj->m_stlistOfPropRef);
					/* encode opening tag for BACnet Read Access Specification */
                    i32APDU_Len +=Encode_Opening_Tag(
						&u8TempAPDU[i32APDU_Len],TAG_NO_1);  

					/* encode all elements in list & check max length is not exceeded */
                    while(NULL != pstlistOfPropRef && (i32APDU_Len < u32MaxInBufSize))
                    {   
                        /* Encode Property id */
                        i32APDU_Len +=   Encode_Context_Enumerated(
							&u8TempAPDU[i32APDU_Len], TAG_NO_0,
                             pstlistOfPropRef->m_ePropertyIdentifier);
                        /* check for array */
                        if(pstlistOfPropRef->m_bIsArrayIndxPresent == ARRAY_INDEX_PRESENT)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(
								&u8TempAPDU[i32APDU_Len], TAG_NO_1,
                                pstlistOfPropRef->m_u32ArrayIndex);
                        }
                        pstlistOfPropRef = pstlistOfPropRef->m_pstNext;
                    }
					/* encode closing tag for BACnet Read Access Specification */
                    i32APDU_Len +=Encode_Closing_Tag(
						&u8TempAPDU[i32APDU_Len],TAG_NO_1);

					/* Move to Next Link List Value */
                    pstGpOfObj = pstGpOfObj->m_pstNext;
                }
            }
            break;

            case BACNET_DT_READACCESSRESULT_LIST:
            {
				/* Local Variables */
				Pr_ListOfReadAccessResult_t  *pstReadAccRes = NULL;
                ListOfReadAccessResult_t      *pstGpObjResults  = NULL;
                listOfResults_t     *pstListOfResults = NULL;

				/* Get Value */
				pstReadAccRes = (Pr_ListOfReadAccessResult_t*)application_data;
                pstGpObjResults = pstReadAccRes->m_pstGpObjResults;

				/* encode all elements in list & check max length is not exceeded */
                while(NULL != pstGpObjResults && (i32APDU_Len < u32MaxInBufSize))
                {
                    /* Encode Object Id  */
                    i32APDU_Len += Encode_Context_Object_Id(
						&u8TempAPDU[i32APDU_Len], TAG_NO_0,
						(int32_t)pstGpObjResults->m_eObjectType,
                         pstGpObjResults->m_u32ObjId);

                    pstListOfResults = &(pstGpObjResults->m_stListOfResults); 
					/* encode opening tag for BACnet Read Access Result */
                    i32APDU_Len +=Encode_Opening_Tag(
						&u8TempAPDU[i32APDU_Len],TAG_NO_1);
					/* encode all elements in list & check max length is not exceeded */
                    while(NULL != pstListOfResults && 
												(i32APDU_Len < u32MaxInBufSize))
                    {
                        /* Encode Property id */
                        i32APDU_Len +=   Encode_Context_Enumerated(
							&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                            pstListOfResults->m_ePropertyIdentifier);
                        /* check for array */
                        if(ARRAY_INDEX_PRESENT == 
											pstListOfResults->m_bArrIndxFlag)
                        {   /* Encode array */
                            i32APDU_Len += Encode_Context_Unsigned(
								&u8TempAPDU[i32APDU_Len], TAG_NO_3,
                                pstListOfResults->m_u32ArrayIndex);
                        }
                        /* Encode Property value */
                        if(pstListOfResults->m_u32UnionMember == 3)
                        {
							/* encode opening tag for property Value */
                            i32APDU_Len += Encode_Opening_Tag(
								&u8TempAPDU[i32APDU_Len],TAG_NO_4);
                            /* Encode Property value */
                            i32APDU_Len += BACApp_Encode_Data_Type(
								&u8TempAPDU[i32APDU_Len], 
                                MAX_APDU_LENGTH_ACCEPTED, 
                                pstListOfResults->m_stReadResult.m_stPropPointer
									.m_pvConstrProp, 
                                pstListOfResults->m_stReadResult.m_stPropPointer
									.m_eData_Type, 
                                pstListOfResults->m_u32ArrayIndex,
                                i32APDU_Len, u32SegSupport,pstListOfResults
									->m_bArrIndxFlag,u16MaxApduLen, NULL, NULL, 0);
							/* encode Closing tag for property Value */
                            i32APDU_Len +=Encode_Closing_Tag(
								&u8TempAPDU[i32APDU_Len],TAG_NO_4);
                        }
                         /* Encode Error received */
                        else if(pstListOfResults->m_u32UnionMember == 2)
                        {
							/* encode Opening tag for property Access Error */
                            i32APDU_Len += Encode_Opening_Tag(
								&u8TempAPDU[i32APDU_Len],TAG_NO_5);
                            /* Encode error class */
                            i32APDU_Len += Encode_Application_Enumerated(
								&u8TempAPDU[i32APDU_Len],
                                pstListOfResults->m_stReadResult.m_stError
								.m_eErrorClass);
                            /* Encode error code */
                            i32APDU_Len += Encode_Application_Enumerated(
								&u8TempAPDU[i32APDU_Len],
                                 pstListOfResults->m_stReadResult.m_stError
								 .m_eErrorCode);
							/* encode Closing tag for property Access Error */
                            i32APDU_Len +=Encode_Closing_Tag(
								&u8TempAPDU[i32APDU_Len],TAG_NO_5);
                        }
                        /* get next property to encode */
                        pstListOfResults = pstListOfResults->m_pstNext;
                    }
					/* encode Closing tag for BACnet Read Access Result */
                    i32APDU_Len +=Encode_Closing_Tag(
						&u8TempAPDU[i32APDU_Len],TAG_NO_1);

					 /* get next result to encode */
                    pstGpObjResults = pstGpObjResults->m_pstNext;
                }
            }
            break;

			case BACNET_DT_OPTIONAL_UNSIGNED:
			{
				/* local variables */
				Pr_BACnetOptionalUnsigned_t *pstOptionalUnsigned = NULL;
                pstOptionalUnsigned = (Pr_BACnetOptionalUnsigned_t *)application_data;
				if(NULL != pstOptionalUnsigned)
				{
					/* encode optional unsigned value */
					i32APDU_Len += DT_Encode_OptionalUnsigned(&u8TempAPDU[0],
						&pstOptionalUnsigned->m_stOptionalUnsigned);
				}
			}
			break;

#ifndef PROFILE_BOD_ONLY
			/* case to encode single node of action list */
			case BACNET_DT_ACTION_LIST:
			{
				Pr_ListOfBACnetActionList_t *pstAction = NULL;
                ListOfBACnetActionList_t  *pstArrayActCmd = NULL;

				/* Get value */
                pstAction = (Pr_ListOfBACnetActionList_t *)application_data;
                pstArrayActCmd = pstAction->m_pstArrayActCmd;

				i32APDU_Len = DT_Encode_ActionList(&u8TempAPDU[i32APDU_Len], pstArrayActCmd);
			}
			break;

            case BACNET_DT_ACTION_LIST_ARRAY:
            {
				/* Local Variables */
                Pr_ListOfBACnetActionList_t      *pstAction      = NULL;
                ListOfBACnetActionList_t  *pstArrayActCmd   = NULL;
                ListOfBACnetActionCommand_t   *pstLstActCmd = NULL;
                bool bFlagIsArray = false;
                uint8_t u8ByteCnt = 0;

				/* Get value */
                pstAction = (Pr_ListOfBACnetActionList_t *)application_data;
                pstArrayActCmd = pstAction->m_pstArrayActCmd;
                
                if(0 == u32ArrayIndex && bArrIndxPresent == ARRAY_INDEX_PRESENT)
                {
                    /* encode the array size count */
                    i32APDU_Len += Encode_Application_Unsigned(
                        &u8TempAPDU[i32APDU_Len],pstAction->m_pstArrayActCmd->
						m_pstActionCommand->m_u32ArrayIndex);
                    break;
                }
                else if(BACNET_ARRAY_ALL == u32ArrayIndex)
                {
                    bFlagIsArray = true;
                }
                else
                {
                    for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
                    {
                        if(NULL == pstArrayActCmd)
                        {
                            i32APDU_Len = 0;
                            break;
                        }
                        /* move to next value */
                        pstArrayActCmd = pstArrayActCmd->m_pstNext;
                    }
                    if(NULL != pstArrayActCmd)
                    {
                        bFlagIsArray = false;
                    }
                }

				/* encode all elements in list & check max length is not exceeded */
                while((NULL != pstArrayActCmd) && (i32APDU_Len < u32MaxInBufSize))
                {
                    pstLstActCmd = pstArrayActCmd->m_pstActionCommand;
                    /* encoding opening tag of Action List*/
                    i32APDU_Len += Encode_Opening_Tag(
						&u8TempAPDU[i32APDU_Len], TAG_NO_0);

					/* encode all elements in list & check max length is not exceeded */
                    while((NULL != pstLstActCmd) && (i32APDU_Len < u32MaxInBufSize))
                    {
                        /* check for Device ID */
						if(pstLstActCmd->m_eDeviceType == OBJECT_DEVICE)
                        {
                            i32APDU_Len += Encode_Context_Object_Id(
								&u8TempAPDU[i32APDU_Len], TAG_NO_0,
								(int32_t)pstLstActCmd->m_eDeviceType,
                                pstLstActCmd->m_u32DevId);
                        }
                        /* check for Object ID */
                        i32APDU_Len += Encode_Context_Object_Id(
							&u8TempAPDU[i32APDU_Len], TAG_NO_1,
							(int32_t)pstLstActCmd->m_eObjectType,
                             pstLstActCmd->m_u32ObjId);

				        /* Encode Property Identifier */            
                        i32APDU_Len += Encode_Context_Enumerated(
							&u8TempAPDU[i32APDU_Len], TAG_NO_2,
                             pstLstActCmd->m_ePropertyIdentifier);
                        /* Check for array */
                        if(pstLstActCmd->m_bArrIndxFlag)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(
								&u8TempAPDU[i32APDU_Len], TAG_NO_3,
                                pstLstActCmd->m_u32ArrayIndex);
                        }
						/* encode Opening tag for property Value */
                        i32APDU_Len +=Encode_Opening_Tag(
							&u8TempAPDU[i32APDU_Len],TAG_NO_4); 
						 /* Encode Property Values */
						i32DecodeLen = BACApp_Encode_Data_Type(
							&u8TempAPDU[i32APDU_Len],
							(u32MaxInBufSize - i32APDU_Len),
							pstLstActCmd->m_stPropPointer.m_pvConstrProp,
							pstLstActCmd->m_stPropPointer.m_eData_Type,
							0, 0, u32SegSupport, false, u16MaxApduLen, NULL, NULL, 0);
						if(i32DecodeLen < 0)
						{
							//TODO - Error Need to be Handle
						}
						else
						{
							i32APDU_Len += i32DecodeLen;
						}

						/* encode Closing tag for property Value */
                        i32APDU_Len += Encode_Closing_Tag(
							&u8TempAPDU[i32APDU_Len],TAG_NO_4);

                        /* priority [5] Unsigned (1..16) OPTIONAL */
                        if(pstArrayActCmd->m_pstActionCommand->m_bPriorityFlag)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(
								&u8TempAPDU[i32APDU_Len], TAG_NO_5,
                                 pstLstActCmd->m_u32Priority);
                        }
                        /* postDelay [6] Unsigned OPTIONAL */
                        if(pstArrayActCmd->m_pstActionCommand->m_bPostDelayFlag)
                        {
                            i32APDU_Len += Encode_Context_Unsigned(
								&u8TempAPDU[i32APDU_Len], TAG_NO_6,
                                 pstLstActCmd->m_u32PostDelay);
                        }
                        /* Encode QuitOnFailure */
                        i32APDU_Len += Encode_Context_Boolean(
							&u8TempAPDU[i32APDU_Len], TAG_NO_7,
                             pstLstActCmd->m_bQuitOnFailure);
                        /* Encode WriteSuccessful */
                        i32APDU_Len += Encode_Context_Boolean(
							&u8TempAPDU[i32APDU_Len], TAG_NO_8,
                             pstLstActCmd->m_bWriteSuccess);

						/* Move to next Action Command Value */
                        pstLstActCmd = pstLstActCmd->m_pstNext;
                    }

					/* encoding Closing tag of Action List */
                    i32APDU_Len += Encode_Closing_Tag(
						&u8TempAPDU[i32APDU_Len], TAG_NO_0);

					/* Move to next Action Value */
                    pstArrayActCmd = pstArrayActCmd->m_pstNext;
                    if(bFlagIsArray == false)
                            break;
                }
            }
            break;
#endif /* !PROFILE_BOD_ONLY */

			case BACNET_DT_DEVOBJREFF :
			{
				/* Local Variables */
				Pr_BACnetDevObjRef_t * pstDevObjReff = NULL;
				/* get the value */
				pstDevObjReff = (Pr_BACnetDevObjRef_t *)application_data;

				i32APDU_Len = DT_Encode_DevObjReff(&u8TempAPDU[0], &pstDevObjReff->m_stDeviceObjReff);				
			}
			break;

			case BACNET_DT_DEVOBJREFF_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetDevObjRef_t *pstDevObjReffArr = NULL;
				/* get the value */
                pstDevObjReffArr = (Pr_ListOfBACnetDevObjRef_t *)application_data;

                i32APDU_Len = DT_Encode_DevObjReff_Array(&u8TempAPDU[0], 
					pstDevObjReffArr, u32MaxInBufSize,
					u32ArrayIndex, bArrIndxPresent); 
			}
			break;

			case BACNET_DT_SHEDLEVEL:
			{
				/* Local Variables */
				Pr_BACnetShedLevel_t *pstShedLevel = NULL;			
				/* get the value */
                pstShedLevel = (Pr_BACnetShedLevel_t *)application_data;

                i32APDU_Len = DT_Encode_ShedLevel(&u8TempAPDU[0], pstShedLevel); 
			}
			break;
	
#ifndef PROFILE_BOD_ONLY
			case BACNET_DT_LOGBUFFER_EL:
			{
				/* Local Variables */
				Pr_ListOfBACnetEventLogRecord_t *pstEventLogRec = NULL;				
				/* get the value */
                pstEventLogRec = (Pr_ListOfBACnetEventLogRecord_t *)application_data;

                i32APDU_Len = DT_Encode_EventLogRec_List(&u8TempAPDU[0], 
				     pstEventLogRec, u32MaxInBufSize); 
			}
			break;

			case BACNET_DT_LOGBUFFER_TLM:
			{
				/* Local Variables */
				Pr_ListOfBACnetLogMultipleRecord_t *pstTreLogMultRec = NULL;				
				/* get the value */
                pstTreLogMultRec = (Pr_ListOfBACnetLogMultipleRecord_t *)application_data;

                //i32APDU_Len = DT_Encode_TrendLogMultRec_List(&u8TempAPDU[0],
                //pstTreLogMultRec, u32MaxInBufSize);
					
				i32APDU_Len = DT_Encode_TrendLogMultRec_List(&u8TempAPDU[0],
					pstTreLogMultRec,
					u32MaxInBufSize,
					pu16NodeLenArr,
					pu32NodeCnt,
					i32NodeLenArrSize);
			}
			break;
#endif /* !PROFILE_BOD_ONLY */

			case BACNET_DT_AUTHENTICATION_FACTOR:
			{
				/* Local Variables */
				Pr_BACnetAuFactor_t *pstAuthFactor = NULL;
				/* get the value */
                pstAuthFactor = (Pr_BACnetAuFactor_t *)application_data;

                i32APDU_Len = DT_Encode_AuthFactor(&u8TempAPDU[0], &pstAuthFactor->m_stAuFactor); 
			}
			break;

			/* case added for encoding single node of authentication factor format array */
			case BACNET_DT_AU_FACTOR_FORMAT:
			{
				/* Local Variables */
				Pr_ListOfBACnetAuFactorFormat_t *pstAuthFactFormArr = NULL;				
				/* get the value */
                pstAuthFactFormArr = (Pr_ListOfBACnetAuFactorFormat_t *)application_data;
				if(NULL != pstAuthFactFormArr)
				{
					i32APDU_Len = DT_Encode_AuthFactForm(&u8TempAPDU[0], pstAuthFactFormArr->m_pstAuFactFormatList);
				}
			}
			break;

			case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetAuFactorFormat_t *pstAuthFactFormArr = NULL;				
				/* get the value */
                pstAuthFactFormArr = 
					(Pr_ListOfBACnetAuFactorFormat_t *)application_data;

                i32APDU_Len = DT_Encode_AuthFactForm_Array(&u8TempAPDU[0], 
					pstAuthFactFormArr, u32MaxInBufSize,
					u32ArrayIndex, bArrIndxPresent); 
			}
			break;

			case BACNET_DT_AUTHENTICATION_POLICY:
			{
				/* Local Variables */
				Pr_ListOfBACnetAuPolicy_t *pstAuthPolicyArr = NULL;				
				/* get the value */
                pstAuthPolicyArr = (Pr_ListOfBACnetAuPolicy_t *)application_data;
				if(NULL != pstAuthPolicyArr)
				{
					i32APDU_Len = DT_Encode_AuthPolicy(&u8TempAPDU[0], pstAuthPolicyArr->m_pstAuPolicy,
						u32MaxInBufSize);
				}
			}
			break;

			case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetAuPolicy_t *pstAuthPolicyArr = NULL;				
				/* get the value */
                pstAuthPolicyArr = (Pr_ListOfBACnetAuPolicy_t *)application_data;

                i32APDU_Len = DT_Encode_AuthPolicy_Array(&u8TempAPDU[0], 
					pstAuthPolicyArr, u32MaxInBufSize, 
					u32ArrayIndex, bArrIndxPresent); 
			}
			break;

			/* case added for encoding single node of access rule */
			case BACNET_DT_ACCESS_RULE:
			{
				/* Local Variables */
				Pr_ListOfBACnetAccessRule_t *pstAccRuleArr = NULL;				
				/* get the value */
                pstAccRuleArr = (Pr_ListOfBACnetAccessRule_t *)application_data;                 
				if(NULL != pstAccRuleArr)
				{
					i32APDU_Len = DT_Encode_AccessRule(&u8TempAPDU[0], pstAccRuleArr->m_pstAccessRuleArray);
				}
			}
			break;	

			case BACNET_DT_ACCESS_RULE_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetAccessRule_t *pstAccRuleArr = NULL;				
				/* get the value */
                pstAccRuleArr = (Pr_ListOfBACnetAccessRule_t *)application_data;

                i32APDU_Len = DT_Encode_AccessRule_Array(&u8TempAPDU[0], 
					pstAccRuleArr, u32MaxInBufSize,
					u32ArrayIndex, bArrIndxPresent); 
			}
			break;

			/* case added for encoding single node of credential au factor */
			case BACNET_DT_CRED_AU_FACTOR:
			{
				/* Local Variables */
				Pr_ListOfBACnetCredAuFactor_t *pstCredAuFactArr = NULL;				
				/* get the value */
                pstCredAuFactArr = (Pr_ListOfBACnetCredAuFactor_t *)application_data;
				if(NULL != pstCredAuFactArr)
				{
					i32APDU_Len = DT_Encode_CredAutheFact(&u8TempAPDU[0], 
						pstCredAuFactArr->m_pstCredAuFactArray); 
				}
			}
			break;
			
			case BACNET_DT_CRED_AU_FACTOR_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetCredAuFactor_t *pstCredAuFactArr = NULL;				
				/* get the value */
                pstCredAuFactArr = (Pr_ListOfBACnetCredAuFactor_t *)application_data;

                i32APDU_Len = DT_Encode_CredAutheFact_Array(&u8TempAPDU[0], 
					pstCredAuFactArr, u32MaxInBufSize,
					u32ArrayIndex, bArrIndxPresent); 
			}
			break;

			/* case added for encoding single node of assigned access rights */
			case BACNET_DT_ASSIGNED_ACCESS_RIGHTS:
			{
				/* Local Variables */
				Pr_ListOfBACnetAssignedAccessRights_t *pstAssiAccRight = NULL;				
				/* get the value */
                pstAssiAccRight = (Pr_ListOfBACnetAssignedAccessRights_t *)application_data;

				if(NULL != pstAssiAccRight)
				{
					i32APDU_Len = DT_Encode_AssiAccRight(&u8TempAPDU[0], pstAssiAccRight->m_pstAsngdAccessArray); 
				}
			}
			break;

			case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetAssignedAccessRights_t *pstAssiAccRight = NULL;				
				/* get the value */
                pstAssiAccRight = (Pr_ListOfBACnetAssignedAccessRights_t *)application_data;

                i32APDU_Len = DT_Encode_AssiAccRight_Array(&u8TempAPDU[0], 
					pstAssiAccRight, u32MaxInBufSize,
					u32ArrayIndex, bArrIndxPresent); 
			}
			break;

#ifdef BACDEL_PR12

#ifndef PROFILE_BOD_ONLY
			/* case added for encoding single node of property access result array */
			case BACNET_DT_PROPERTYACCESSRESULT:
			{
				/* Local Variables */	
				Pr_ListOfBACnetPropAccessRslt_t *pstPropAccRes  = NULL;
				pstPropAccRes = (Pr_ListOfBACnetPropAccessRslt_t *)application_data;
				if(NULL != pstPropAccRes)
				{								
					/* encode property access result */
					i32APDU_Len = DT_Encode_PropAccessResult(&u8TempAPDU[0], pstPropAccRes->m_pstPropAccRslt,
					u32MaxInBufSize, u32SegSupport, u16MaxApduLen);
				}
			}
			break;

			case BACNET_DT_PROPERTYACCESSRESULT_ARRAY:
			{
				/* Local Variables */	
				Pr_ListOfBACnetPropAccessRslt_t *pstPropAccRes  = NULL;
				pstPropAccRes = (Pr_ListOfBACnetPropAccessRslt_t *)application_data;

				/* encode property access result array */
				i32APDU_Len = DT_Encode_PropAccessResult_Array(&u8TempAPDU[0],
					pstPropAccRes, u32MaxInBufSize, u32SegSupport, u16MaxApduLen,
					u32ArrayIndex, bArrIndxPresent);
			}
			break;

			/* case added for encoding single node of network security policy */
			case BACNET_DT_NW_SECURITY_POLICY:
			{
				/* Local Variables */	
				Pr_ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcy  = NULL;
				pstNetSecPlcy = (Pr_ListOfBACnetNwSecurityPolicy_t *)application_data;
				if(NULL != pstNetSecPlcy)
				{
					/* encode property network security policy */
					i32APDU_Len = DT_Encode_NwSecurityPolicy(&u8TempAPDU[0],
						pstNetSecPlcy->m_pstSecurityPolicyArr);							
				}
			}
			break;

			case BACNET_DT_NW_SECURITY_POLICY_ARRAY:
			{
				/* Local Variables */	
				Pr_ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcy  = NULL;
				pstNetSecPlcy = (Pr_ListOfBACnetNwSecurityPolicy_t *)application_data;

				/* encode property network security policy */
				i32APDU_Len += DT_Encode_NwSecurityPolicy_Array(&u8TempAPDU[0],
					pstNetSecPlcy, u32MaxInBufSize,
					u32ArrayIndex, bArrIndxPresent);							
			}
			break;
#endif /* !PROFILE_BOD_ONLY */

			case BACNET_DT_ADDRESSBINDING:
            {
				/* Local Variables */
				Pr_BACnetAddrBinding_t *pstDevAddrBind = NULL;
				BACnetAddrBinding_t *pstAddBindData;
                BACnetOctetStr_t stMACAdrss = {0};
                uint8_t u8LoopCnt = 0;

                pstDevAddrBind = (Pr_BACnetAddrBinding_t *)application_data;
				pstAddBindData = &pstDevAddrBind->m_stAddBinding;
       
                /* Encode Object Identifier */
                i32APDU_Len +=
                Encode_Application_Object_Id(&u8TempAPDU[i32APDU_Len],
						(int32_t)pstAddBindData->m_eObjectType,
                        pstAddBindData->m_u32ObjId);

                /* Encode Network Number */
                i32APDU_Len +=
                    Encode_Application_Unsigned(&u8TempAPDU[i32APDU_Len], 
							pstAddBindData->m_stAddress.u16net);

                if(pstAddBindData->m_stAddress.u8dlen)
                {
                    /* Get Mac Address & Mac length */
					stMACAdrss.m_u32OctetCount = pstAddBindData->m_stAddress.u8dlen;
					for(u8LoopCnt=0; u8LoopCnt <= stMACAdrss.m_u32OctetCount; u8LoopCnt++)
					{
						stMACAdrss.m_ai8OctetStr[u8LoopCnt] = 
						    pstAddBindData->m_stAddress.u8DvDadr[u8LoopCnt];
					}
                }
                else
                {
				    /* Get Ip Address & Port No */
					stMACAdrss.m_u32OctetCount = pstAddBindData->m_stAddress.u8mac_len;
					for( u8LoopCnt=0; u8LoopCnt <= stMACAdrss.m_u32OctetCount; u8LoopCnt++)
					{
						stMACAdrss.m_ai8OctetStr[u8LoopCnt] = 
						    pstAddBindData->m_stAddress.u8IpAddrs[u8LoopCnt];
					}
                }

                /* Encode MAC address */
                i32APDU_Len += Encode_Application_Octet_String
					(&u8TempAPDU[i32APDU_Len], &stMACAdrss);
            }
            break;

#ifndef PROFILE_BOD_ONLY
			/* case added for encoding single node of Key sets */
			case BACNET_DT_SECURITY_KEYSET:
			{
				/* Local Variables */
				Pr_BACnetSecurityKeySet_t  *pstSecKeySet = NULL;
				pstSecKeySet = (Pr_BACnetSecurityKeySet_t *)application_data;
				if(NULL != pstSecKeySet)
				{
					i32APDU_Len += DT_Encode_SecurityKeySet(&u8TempAPDU[0],
						&pstSecKeySet->m_stSecurityKetSet, u32MaxInBufSize);
				}
			}
			break;

			case BACNET_DT_SECURITY_KEYSET_ARRAY:
			{
				/* Local Variables */
				Pr_ListOfBACnetSecurityKeySet_t  *pstSecKeySet = NULL;
				pstSecKeySet = (Pr_ListOfBACnetSecurityKeySet_t *)application_data;
	
				i32APDU_Len += DT_Encode_SecurityKeySet_Array(
					&u8TempAPDU[0], pstSecKeySet,
					u32MaxInBufSize, u32ArrayIndex,
					bArrIndxPresent);
			}
			break;
#endif /* !PROFILE_BOD_ONLY */

#endif /* BACDEL_PR12 */

#ifdef BACDEL_PR14

			case BACNET_DT_PROPERTY_LIST: 
            {
				/* local variables */
				Pr_BACnetPropertyList_t *pstPropertyList = NULL;
				/* get pointer of property list */
                pstPropertyList = (Pr_BACnetPropertyList_t *)application_data;
				/* check pointer */
				if(NULL != pstPropertyList)
				{
					i32APDU_Len += DT_Encode_PropertyList(
						&u8TempAPDU[0],	pstPropertyList,
						u32MaxInBufSize, u32ArrayIndex,
						bArrIndxPresent);
				}
            }
            break;

#endif /* BACDEL_PR14 */

				/* default case */
            default:
                i32APDU_Len = -1;
                break;
        }
    }
	/* if null datatype, encode value directly, no need to check pointer */
	else if(eData_Type == BACNET_DT_NULL)
	{
		u8TempAPDU[0] = 0;
        i32APDU_Len++;
	}

    /* DataType is not supported */
	if(i32APDU_Len == -1 || NULL == apdu)
	{
        return DATA_TYPE_NOT_SUPPORTED;
	}

    /* Check if the APDU length allowed by Device is not exceeded. */
    if((i32APDULenFilled+i32APDU_Len) <= u16MaxApduLen)
    {
		/* check if buffer size is not exceeded */
		if (i32APDU_Len <= ((MAX_APDU_LENGTH_ACCEPTED-i32APDULenFilled) - 0 /* 0 is offset */)) 
		{
			/* copy the data */	
			memcpy(apdu, u8TempAPDU, i32APDU_Len);
		}
		else
		{
            /* apdu buffer size exceeded */
			return APDU_DATA_BUFFER_SIZE_EXCEEDED;
		}
    }
    else
    {
#ifdef SEGMENTATION_SUPPORTED
		if(SEGMENTATION_TRANSMIT == u32SegSupport || SEGMENTATION_BOTH == u32SegSupport)
		{
			/* check if buffer size is not exceeded */
			if (i32APDU_Len <= ((u32MaxInBufSize-i32APDULenFilled) - 0 /* 0 is offset */)) 
			{
				/* copy the data */	
				memcpy(apdu, u8TempAPDU, i32APDU_Len);
			}
			else
			{
				/* apdu buffer size exceeded */
				return APDU_DATA_BUFFER_SIZE_EXCEEDED;
			}
		}
        else return SEGMENTATION_NOT_SUPPORTED;	/* segmentation is not supported by host device */
#else
		return SEGMENTATION_NOT_SUPPORTED;
#endif 
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: BACApp_Encode_Data_Type: Exit \r\n");
	#endif

    return i32APDU_Len;
}


#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_SER_AE_EN_B || defined BACDEL_SER_AE_AA_B)
/***
*
*DESCRIPTION
*   This function will decode the Time Stamp for event notification & acknowledge
*	alarm services.
*
*@param pu8APDU			[in]    The contents of service request.
*@param pstTimeStamp	[out]	Structure pointer to hold received time_stamp data.
*								struct of type BACnetTimeStamp_t.
*
*@return	-	Returns no of bytes decoded on success else -1 or -2 on error.
*
***/
int32_t Decode_Time_Stamp(BACnetTimeStamp_t *pstTimeStamp, uint8_t *pu8APDU, uint8_t u8TagNo)
{
	int32_t i32Len = 0;			/* to store length */
	uint8_t u8TagNumber = 0;	/* to decode & save tag no */
	uint32_t u32LenValue = 0;	/* to decode & save */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time_Stamp: entry \r\n");
	#endif

	/* Extract Time Stamp tag */
	/* Tag 3E: Opening context tag */
	Decode_Tag_Number_And_Value(&pu8APDU[i32Len],&u8TagNumber, &u32LenValue);
	/* If tag is not of type time stamp return error */
	if(u8TagNumber == u8TagNo &&
		Decode_Is_Opening_Tag_Number(&pu8APDU[i32Len++],u8TagNo))
	{
		/* Tag 2E: Opening context tag */
		if (!Decode_Is_Opening_Tag_Number(&pu8APDU[i32Len], TAG_NO_2))
		{
			if(Decode_Is_Context_Tag(&pu8APDU[i32Len], TAG_NO_0) == TRUE)
			{
				i32Len++; /* context tag for time decoded so increment by 1 */
				/* time stamp is time only */
				i32Len += Decode_Time(&pu8APDU[i32Len],
					&pstTimeStamp->m_stTimeStamp.m_stTime);
				pstTimeStamp->m_eTimeStampType = TIMESTAMP_TYPE_TIME;
			}
			else if(Decode_Is_Context_Tag(&pu8APDU[i32Len], TAG_NO_1) == TRUE)
			{
				/* time stamp is sequence number */
				i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len],&u8TagNumber, &u32LenValue);
				i32Len += Decode_Unsigned(&pu8APDU[i32Len], u32LenValue,
					&pstTimeStamp->m_stTimeStamp.m_u32SeqNo);
				pstTimeStamp->m_eTimeStampType = TIMESTAMP_TYPE_SEQUENCE_NO;
			}
			else
			{
				/* invalid tag received, time stamp is not time or seq no. */
				return TIMESTAMP_INVALID_TAG;
			}
		}
		else
		{
			i32Len++; /* opening tag 2E decoded so increment by 1 */
			/* time stamp is date-time */
			pstTimeStamp->m_eTimeStampType = TIMESTAMP_TYPE_DATETIME;
			i32Len += Decode_Application_Date(&pu8APDU[i32Len],
				&pstTimeStamp->m_stTimeStamp.m_stDateTime.m_stDate);
			if(i32Len < 0)
				return TIMESTAMP_DECODE_ERROR;
			i32Len += Decode_Application_Time(&pu8APDU[i32Len],
				&pstTimeStamp->m_stTimeStamp.m_stDateTime.m_stTime);
			if(i32Len < 0)
				return TIMESTAMP_DECODE_ERROR;
			/* Tag 2F: Closing context tag */
			if (!Decode_Is_Closing_Tag_Number(&pu8APDU[i32Len++], TAG_NO_2))
			{
				/* invalid tag received return error */
				return TIMESTAMP_INVALID_TAG;
			}
		}
	}
	else
	{
		/* invalid tag received return error */
		return TIMESTAMP_INVALID_TAG;
	}
	/* Tag 3F: Closing context tag */
	if (!Decode_Is_Closing_Tag_Number(&pu8APDU[i32Len++], u8TagNo))
	{
		/* invalid tag received return error */
		return TIMESTAMP_INVALID_TAG;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time_Stamp: exit \r\n");
	#endif

	return i32Len;
}

/***
*
*DESCRIPTION
*   This function will encode the Time Stamp for event notification & acknowledge
*	alarm services.
*
*@param pu8APDU			[out]   Buffer to save encoded time_stamp value.
*@param pstTimeStamp	[in]	Structure pointer that holds time_stamp data.
*								struct of type BACnetTimeStamp_t.
*
*@return	-	Returns no of bytes encoded.
*
***/
int32_t Encode_Time_Stamp(BACnetTimeStamp_t *pstTimeStamp, uint8_t *pu8APDU, uint8_t u8TagNo)
{
	int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Time_Stamp: entry \r\n");
	#endif

	/* encode opening tag - xE */
	i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], u8TagNo);

#ifdef STAMP_AS_DATETIME
	/* encode date_time */
	i32Len += Encode_Application_StampDateTime(&pu8APDU[i32Len],
		&pstTimeStamp->m_stTimeStamp.m_stDateTime.m_stTime,
		&pstTimeStamp->m_stTimeStamp.m_stDateTime.m_stDate);
#elif defined STAMP_AS_TIME
	/* encode time */
	i32Len += Encode_Application_StampTime(&pu8APDU[i32Len],
		&pstTimeStamp->m_stTimeStamp.m_stTime);
#elif defined STAMP_AS_SEQUENCE
	/* encode sequence no */
	i32Len += Encode_Application_StampUnsigned(&pu8APDU[i32Len],
		pstTimeStamp->m_stTimeStamp.m_u32SeqNo);
#endif /* STAMP_AS_DATETIME */

	/* encode closing tag - xF */
	i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], u8TagNo);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Time_Stamp: exit \r\n");
	#endif

	/* return encoded length */
	return i32Len;
}

#endif /* BACDEL_SER_AE_EN_A || BACDEL_SER_AE_EN_B || BACDEL_SER_AE_AA_B*/


/***
*
* DESCRIPTION
* Function to encode notification parameters i.e event values.
*
* @param pu8ServiceRequest	[out]   Buffer to hold encoded byte data.
* @param pstENdata			[in]	Structure pointer that hold notification
*                                   parameters decoding.
*
* @return - Returns no of bytes encoded.
*
*/
#if (defined BACDEL_SER_AE_EN_B || defined BACDEL_OBJ_EL)
int32_t Encode_Notification_Parameters(
    uint8_t *pu8APDU,
    BacnetEnPropElem_t *pstENdata)
{
    /* local variables */
	int32_t i32Len = 0;
	BACNET_EVENT_TYPE eEventType = MAX_EVENT_TYPE;
    BACNET_PROPERTY_VALUE stValue = {0};
    Parameters_t *pstParameters = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Encode_Notification_Parameters: entry \r\n");
	#endif

	/* encode opening tag - CE */
	i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_12);

	/* get the type of event */
	eEventType = pstENdata->m_eEventType;

	/* switch to event type */
	switch(eEventType)
	{
	case EVENT_CHANGE_OF_STATE:
	{
		/* encode opening tag - 1E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_1);
		/* encode new state */
		/* encode opening tag - 0E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_0);
		if(PROP_STATE_BOOLEAN_VALUE == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stChangeState.m_uNewState.m_ePropState)
		{
			/* encode boolean value */
			i32Len += Encode_Context_Boolean(&pu8APDU[i32Len], PROP_STATE_BOOLEAN_VALUE,
							pstENdata->m_stEventValues.BACnetNotificationParameters_u.
							m_stChangeState.m_uNewState.BACnetPropertyStates_U.m_BooleanValue);
		}
		else if(PROP_STATE_UNSIGNED_VALUE == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stChangeState.m_uNewState.m_ePropState)
		{
			/* encode unsigned value */
			i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], PROP_STATE_UNSIGNED_VALUE,
							pstENdata->m_stEventValues.BACnetNotificationParameters_u.
							m_stChangeState.m_uNewState.BACnetPropertyStates_U.m_UnsignedValue);
		}
		else
		{
			/* encode enum value */
			i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len],
							pstENdata->m_stEventValues.BACnetNotificationParameters_u.
							m_stChangeState.m_uNewState.m_ePropState,
							pstENdata->m_stEventValues.BACnetNotificationParameters_u.
							m_stChangeState.m_uNewState.BACnetPropertyStates_U.m_UnsignedValue);
		}
		/* encode closing tag - 0F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_0);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stChangeState.m_stStatusFlag);
		/* encode closing tag - 1F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_1);
		break;
	}

	case EVENT_COMMAND_FAILURE:
	{
		/* encode opening tag - 3E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_3);
		/* encode command value */
		/* encode opening tag - 0E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_0);
		if(BACNET_APPLICATION_TAG_BOOLEAN == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stCmdFailure.m_u8Apptag)
		{
			/* encode boolean value */
			i32Len += Encode_Application_Boolean(&pu8APDU[i32Len],(bool)
						pstENdata->m_stEventValues.BACnetNotificationParameters_u.
						m_stCmdFailure.m_stCommandValue.eVal);
		}
		else if(BACNET_APPLICATION_TAG_UNSIGNED_INT == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stCmdFailure.m_u8Apptag)
		{
			/* encode unsigned value */
			i32Len += Encode_Application_Unsigned(&pu8APDU[i32Len],
						pstENdata->m_stEventValues.BACnetNotificationParameters_u.
						m_stCmdFailure.m_stCommandValue.u32Val);
		}
		else if(BACNET_APPLICATION_TAG_ENUMERATED == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stCmdFailure.m_u8Apptag)
		{
			/* encode enum value */
			i32Len += Encode_Application_Enumerated(&pu8APDU[i32Len],
						pstENdata->m_stEventValues.BACnetNotificationParameters_u.
						m_stCmdFailure.m_stCommandValue.eVal);
		}
		else;
		/* encode closing tag - 0F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_0);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stCmdFailure.m_stStatusFlag);
		/* encode feedback value */
		/* encode opening tag - 2E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_2);
		/* TODO - needs to be worked on to send feed back value for EE objects
			during algorithmic reporting */
		if(BACNET_APPLICATION_TAG_BOOLEAN == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stCmdFailure.m_u8Apptag)
		{
			/* encode boolean value */
			i32Len += Encode_Application_Boolean(&pu8APDU[i32Len],(bool)
						pstENdata->m_stEventValues.BACnetNotificationParameters_u.
						m_stCmdFailure.m_stFeedbackValue.eVal);
		}
		else if(BACNET_APPLICATION_TAG_UNSIGNED_INT == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stCmdFailure.m_u8Apptag)
		{
			/* encode unsigned value */
			i32Len += Encode_Application_Unsigned(&pu8APDU[i32Len],
						pstENdata->m_stEventValues.BACnetNotificationParameters_u.
						m_stCmdFailure.m_stFeedbackValue.u32Val);
		}
		else if(BACNET_APPLICATION_TAG_ENUMERATED == pstENdata->m_stEventValues.
			BACnetNotificationParameters_u.m_stCmdFailure.m_u8Apptag)
		{
			/* encode enum value */
			i32Len += Encode_Application_Enumerated(&pu8APDU[i32Len],
						pstENdata->m_stEventValues.BACnetNotificationParameters_u.
						m_stCmdFailure.m_stFeedbackValue.eVal);
		}
		else;
		/* encode closing tag - 2F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_2);
		/* encode closing tag - 3F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_3);
		break;
	}

	case EVENT_OUT_OF_RANGE:
	{
		/* encode opening tag - 5E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_5);
		/* encode exceeding value */
		i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_0,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stOutOfRange.m_fExceedingValue);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stOutOfRange.m_stStatusFlag);
		/* encode deadband */
		i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_2,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stOutOfRange.m_fDeadband);
		/* encode exceeded limit */
		i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_3,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stOutOfRange.m_fExceededLimit);
		/* encode closing tag - 5F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_5);
		break;
	}

	case EVENT_BUFFER_READY:
	{
		/* encode opening tag - AE */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_10);
		/* encode opening tag - 0E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_0);
		/* encode buffer property object id */
		i32Len += Encode_Context_Object_Id(&pu8APDU[i32Len],TAG_NO_0,
			(int32_t)pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
			m_stBufferProperty.m_eObjectType,
			pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
			m_stBufferProperty.m_u32ObjId);
		/* encode buffer property id */
		i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len], TAG_NO_1,
			pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
			m_stBufferProperty.m_ePropertyIdentifier);
		/* encode buffer property array index if present */
		if(pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
			m_stBufferProperty.m_bArrIndxPresent == ARRAY_INDEX_PRESENT)
		{
			/* encode array index */
			i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_2,
				pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
				m_stBufferProperty.m_u32ArrayIndex);
		}
		/* encode buffer property device id, i.e. the device whose object has this prop */

		/* encode closing tag - 0F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_0);
		/* encode previous notification value i.e last notify record */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_1,
			pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
			m_u32PreviousNotification);
		/* encode current notification value i.e total record count */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_2,
			pstENdata->m_stEventValues.BACnetNotificationParameters_u.m_stBufferReady.
			m_u32CurrentNotification);
		/* encode closing tag - AF */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_10);
		break;
	}

    case EVENT_FLOATING_LIMIT:
	{
        /* encode opening tag - 4E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_4);
		/* encode reference value */
		i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_0,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stFlotLimit.m_fReferenceValue);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stFlotLimit.m_stStatusFlag);
		/* encode set point value */
		i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_2,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stFlotLimit.m_fSetPointValue);
		/* encode error limit */
		i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_3,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stFlotLimit.m_fErrorLimit);
		/* encode closing tag - 4F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_4);
        break;
	}

	case EVENT_CHANGE_OF_BITSTRING:
	{
        /* encode opening tag - 0E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_0);
		/* encode reference bitstring value */
		i32Len += Encode_Context_BITString(&pu8APDU[i32Len], TAG_NO_0,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stChangeBitstring.m_stRefBitString);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stChangeBitstring.m_stStatusFlag);
		/* encode closing tag - 0F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_0);
        break;
	}

    case EVENT_UNSIGNED_RANGE:
	{
        /* encode opening tag - BE */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_11);
		/* encode exceeding value */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_0,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stUnsignedRange.m_u32ExceedingValue);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
						&pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stUnsignedRange.m_stStatusFlag);
		/* encode exceeded limit value */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_2,
						pstENdata->m_stEventValues.
						BACnetNotificationParameters_u.m_stUnsignedRange.m_u32ExceededLimit);
		/* encode closing tag - BF */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_11);
        break;
	}

    case EVENT_DOUBLE_OUT_OF_RANGE:
	{
		/* encode opening tag - DE */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_14);
		/* encode exceeding value */
		i32Len += Encode_Context_Double(&pu8APDU[i32Len], TAG_NO_0,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stDoubleOutOfRange.m_dExceedingValue);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
				&pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stDoubleOutOfRange.m_stStatusFlag);
		/* encode deadband */
		i32Len += Encode_Context_Double(&pu8APDU[i32Len], TAG_NO_2,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stDoubleOutOfRange.m_dDeadband);
		/* encode exceeded limit */
		i32Len += Encode_Context_Double(&pu8APDU[i32Len], TAG_NO_3,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stDoubleOutOfRange.m_dExceededLimit);
		/* encode closing tag - DF */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_14);
		break;
	}

    case EVENT_SIGNED_OUT_OF_RANGE:
	{
		/* encode opening tag - 15E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_15);
		/* encode exceeding value */
		i32Len += Encode_Context_Signed(&pu8APDU[i32Len], TAG_NO_0,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stSignedOutOfRange.m_i32ExceedingValue);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
				&pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stSignedOutOfRange.m_stStatusFlag);
		/* encode deadband */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_2,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stSignedOutOfRange.m_u32Deadband);
		/* encode exceeded limit */
		i32Len += Encode_Context_Signed(&pu8APDU[i32Len], TAG_NO_3,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stSignedOutOfRange.m_i32ExceededLimit);
		/* encode closing tag - 15F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_15);
		break;
	}

	case EVENT_UNSIGNED_OUT_OF_RANGE:
	{
		/* encode opening tag - 16E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_16);
		/* encode exceeding value */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_0,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stUnSgndOutOfRange.m_u32ExceedingValue);
		/* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
				&pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stUnSgndOutOfRange.m_stStatusFlag);
		/* encode deadband */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_2,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stUnSgndOutOfRange.m_u32Deadband);
		/* encode exceeded limit */
		i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_3,
				pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stUnSgndOutOfRange.m_u32ExceededLimit);
		/* encode closing tag - 16F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_16);
		break;
	}

    case EVENT_CHANGE_OF_CHARACTERSTRING:
	{
        /* encode opening tag - 17E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_17);
        /* encode changed value */
        i32Len += Encode_Context_Character_String(&pu8APDU[i32Len], TAG_NO_0,
                &pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeCharString.m_stChangedValue);
        /* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
			    &pstENdata->m_stEventValues.
			    BACnetNotificationParameters_u.m_stChangeCharString.m_stStatusFlag);
        /* encode alarm value */
        i32Len += Encode_Context_Character_String(&pu8APDU[i32Len], TAG_NO_2,
                &pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeCharString.m_stAlarmValue);
        /* encode closing tag - 17F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_17);
        break;
	}

    case EVENT_CHANGE_OF_STATUS_FLAGS:
	{
        /* encode opening tag - 18E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_18);
        /* encode present value */
        /* save the value in required format */
        /* FIXME - need to change this logic if required */
        stValue.m_TagType = pstENdata->m_stEventValues.
			    BACnetNotificationParameters_u.m_stChangeStatusFlags.m_u8AppTag;
        memcpy(&stValue.uValue, &pstENdata->m_stEventValues.BACnetNotificationParameters_u.
            m_stChangeStatusFlags.m_uPresentValue, 10/*sizeof(stValue.uValue)*/);
        /* set other tags to default */
        stValue.m_ContextSpecific = -1;
        stValue.m_ContextTag = -1;
        stValue.m_StartTag = -1;
        stValue.pstNextPropVal = NULL;
        /* encode opening tag - 0E */
        i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_0);
        /* NOTE: can pass any value to PropId parameter as v r only
            encoding primitive datatypes here */
        i32Len += BACApp_Encode_data(&pu8APDU[i32Len], &stValue, 0);
        /* encode closing tag - 0F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_0);
        /* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
			    &pstENdata->m_stEventValues.
			    BACnetNotificationParameters_u.m_stChangeStatusFlags.m_stStatusFlag);
        /* encode closing tag - 18F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_18);
        break;
	}

	case EVENT_CHANGE_OF_VALUE:
	{
        /* encode opening tag - 2E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_2);
        /* encode opening tag - 0E */
        i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_0);
        if(TAG_NO_0 == pstENdata->m_stEventValues.BACnetNotificationParameters_u.
            m_stChangeValue.m_i8ValueType)
        {
            /* encode changed bits */
            i32Len += Encode_Context_BITString(&pu8APDU[i32Len], TAG_NO_0,
                &pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeValue.uNewValue.m_stChangedBits);
        }
        else
        {
            /* encode changed value */
            i32Len += Encode_Context_Real(&pu8APDU[i32Len], TAG_NO_1,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeValue.uNewValue.m_fChangedValue);
        }
        /* encode closing tag - 0F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_0);
        /* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
			    &pstENdata->m_stEventValues.
			    BACnetNotificationParameters_u.m_stChangeValue.m_stStatusFlag);
        /* encode closing tag - 2F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_2);
        break;
	}

    case EVENT_CHANGE_OF_LIFE_SAFETY:
	{
        /* encode opening tag - 8E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_8);
        /* encode new state */
        i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len], TAG_NO_0,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeLifeSafety.m_eNewState);
        /* encode new mode */
        i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len], TAG_NO_1,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeLifeSafety.m_eNewMode);
        /* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_2,
			    &pstENdata->m_stEventValues.
			    BACnetNotificationParameters_u.m_stChangeLifeSafety.m_stStatusFlag);
        /* encode operation expected */
        i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len], TAG_NO_3,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stChangeLifeSafety.m_eOperationExpected);
        /* encode closing tag - 8F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_8);
        break;
	}

	case EVENT_EXTENDED:
	{
        /* encode opening tag - 9E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_9);
        /* encode vendor id */
        i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_0,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stExtended.m_u16VendorId);
        /* encode extended event type */
        i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_0,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stExtended.m_u32ExtendedEventType);
        /* encode opening tag - 2E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_2);
        /* parameters encoding */
        pstParameters = pstENdata->m_stEventValues.BACnetNotificationParameters_u.
            m_stExtended.m_pstParameters;
        while(NULL != pstParameters)
        {
            /* save the value in required format */
            /* FIXME - need to change this logic if required */
            stValue.m_TagType = pstParameters->m_u8ParameterType;
            memcpy(&stValue.uValue, &pstParameters->m_stParameters,
                10/*sizeof(stValue.uValue)*/);
            /* set other tags to default */
            stValue.m_ContextSpecific = -1;
            stValue.m_ContextTag = -1;
            stValue.m_StartTag = -1;
            stValue.pstNextPropVal = NULL;
            /* NOTE: can pass any value to PropId parameter as v r only
                encoding primitive datatypes here */
            i32Len += BACApp_Encode_data(&pu8APDU[i32Len], &stValue, 0);
            /* move to next parameter */
            pstParameters = pstParameters->m_pstNext;
        }
        /* encode closing tag - 2F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_2);
        /* encode closing tag - 9F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_9);
        break;
	}

    case EVENT_ACCESS_EVENT:
	{
        /* encode opening tag - DE */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_13);
        /* encode access event */
        i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len], TAG_NO_0,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stAccessEvent.m_eAccessEvent);
        /* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
			    &pstENdata->m_stEventValues.
			    BACnetNotificationParameters_u.m_stAccessEvent.m_stStatusFlag);
        /* encode access event tag */
        i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_2,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stAccessEvent.m_u32EventTag);
        /* encode access event time */
        i32Len += Encode_Time_Stamp(&pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stAccessEvent.m_stEventTime,
                &pu8APDU[i32Len], TAG_NO_3);
        /* encode opening tag - 4E */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_4);
        /* encode access credential */
        /* encode device id - optional */
        i32Len += Encode_Context_Object_Id(&pu8APDU[i32Len], TAG_NO_0,
                (int32_t)pstENdata->m_stEventValues.BACnetNotificationParameters_u.
				m_stAccessEvent.m_AccessCredential.m_eDeviceType,
                pstENdata->m_stEventValues.BACnetNotificationParameters_u.
                m_stAccessEvent.m_AccessCredential.m_u32DeviceId);
        /* encode object id */
        i32Len += Encode_Context_Object_Id(&pu8APDU[i32Len], TAG_NO_1,
                (int32_t)pstENdata->m_stEventValues.BACnetNotificationParameters_u.
				m_stAccessEvent.m_AccessCredential.m_eObjectType,
                pstENdata->m_stEventValues.BACnetNotificationParameters_u.
                m_stAccessEvent.m_AccessCredential.m_u32ObjId);
        /* encode closing tag - 4F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_4);
        /* encode authentication factor */
        if(TRUE == pstENdata->m_stEventValues.BACnetNotificationParameters_u.
                m_stAccessEvent.m_bAuFactorPresent)
        {
            /* encode opening tag - 5E */
		    i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_5);
            /* encode format type */
            i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len], TAG_NO_0,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stAccessEvent.m_stAuFactor.m_eFormatType);
            /* encode format class */
            i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_1,
                pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stAccessEvent.m_stAuFactor.m_u32FormatClass);
            /* encode octet string value */
            i32Len += Encode_Context_Octet_String(&pu8APDU[i32Len], TAG_NO_2,
                &pstENdata->m_stEventValues.
                BACnetNotificationParameters_u.m_stAccessEvent.m_stAuFactor.m_stOctetStrValue);
            /* encode closing tag - 5F */
		    i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_5);
        }
        /* encode closing tag - DF */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_13);
        break;
	}

#ifdef FAULT_ALGORITHM
    case EVENT_CHANGE_OF_RELIABILITY:
    {
		Pr_BACnetReal_t *PresentValue=NULL;
		float present_Value=0;
		PresentValue = (Pr_BACnetReal_t*)(pstENdata->m_stEventValues.BACnetNotificationParameters_u.
				m_stChangeOfReliability.m_stPropertyValue.m_pvPropVal);
		present_Value = PresentValue->m_fVal;

        /* encode opening tag - 13F */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_19);

		/* encode reliability flag */
		i32Len += Encode_Context_Enumerated(&pu8APDU[i32Len],TAG_NO_0,
				pstENdata->m_stEventValues.BACnetNotificationParameters_u.
				m_stChangeOfReliability.m_eReliabilty);

        /* encode status flag */
		i32Len += Encode_Context_BITSTRING(&pu8APDU[i32Len], TAG_NO_1,
			    &pstENdata->m_stEventValues.BACnetNotificationParameters_u.
				m_stChangeOfReliability.m_stStatusFlag);

		/* Tag 4: propertyValue */
		/* Encode the application opening tag */
		i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_2);

	    /* Encode the context unsigned Property-ID Value */
	    i32Len += Encode_Context_Unsigned(&pu8APDU[i32Len], TAG_NO_0, pstENdata->m_stEventValues.
	    		BACnetNotificationParameters_u.m_stChangeOfReliability.m_stPropertyValue.m_eObjectProperty);

		/* Encode the application opening tag */
	    i32Len += Encode_Opening_Tag(&pu8APDU[i32Len], TAG_NO_2);

	    /* Encode the application Real present value */
	    i32Len += Encode_Application_Real(&pu8APDU[i32Len],present_Value);

	    /* Encode the application closing tag */
	    i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_2);

	    /* Encode the application closing tag */
	    i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_2);

        /* encode closing tag - 13F */
		i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_19);
		break;
    }
#endif /*FAULT_ALGORITHM*/

	default:
        /* These Event Types are currently not supported */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: "
        "Encode_Notification_Parameters: event type encoding not supported. \r\n");
		#endif
		break;
	}//end of switch

	/* encode closing tag - CF */
	i32Len += Encode_Closing_Tag(&pu8APDU[i32Len], TAG_NO_12);

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Encode_Notification_Parameters: exit \r\n");
	#endif

	/* return encoded length */
	return i32Len;
}
#endif /* BACDEL_SER_AE_EN_B || BACDEL_OBJ_EL */

/**
* 
* DESCRIPTION
* Function to decode data as per property data-type.
*
* @ param pu8Apdu			[in]  data to be decoded.
* @ param u32ApduDataLen	[in]  length of data to be decoded. 
* @ param ePropID			[in]  property id.
* @ param eObjType			[in]  object type.
* @ param u32ArrayIndex		[in]  array index value if present.
* @ param bArrayIndexFlag	[in]  indicates if array index is present or not.
* @ param peDataType		[out] pointer to save data type.
* @ param ppPropValue		[out] double pointer to save actual property value pointer.
*
* @ return [out] returns no of bytes decoded on success else -ve error value.
* @ return [out] error coded returned as per "DT_ERR_ ..." defined in 
*				"pduEncodeDecodeDT.h" file 
*
*/
int32_t BACApp_Decode_Data_Type(
    uint8_t *pu8Apdu, uint32_t u32ApduDataLen,
    BACNET_PROPERTY_ID ePropID, BACNET_OBJECT_TYPE eObjType,
	BACNET_DATA_TYPE *peDataType, void **ppPropValue,
	uint32_t u32ArrayIndex, bool bArrayIndexFlag,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	void *pvData = NULL;
	//void *pvTemp = NULL;

	uint32_t u32DataValue = 0;
	uint32_t u32Count = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	BACApp_Decode_Data_Type: entry \r\n");
	#endif

	//return 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == peDataType || NULL == ppPropValue)
	{
		/* invalid input pointers */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		BACApp_Decode_Data_Type: null input pointers \r\n");
		#endif
		/* fill return values */
		if(NULL != peDataType)
			peDataType = NULL;
		if(NULL != ppPropValue)
			*ppPropValue = NULL;
		return i32Len;
	}

	/* initialize output variables with default values */
	*ppPropValue = NULL;
	*peDataType = BACNET_DT_EMPTY;

	// TODO - get the data type value
	*peDataType = GetPropertyDataType(eObjType, ePropID, u32ArrayIndex, bArrayIndexFlag);

	/* if array index is 0, decode unsigned value */
	if(ARRAY_INDEX_PRESENT == bArrayIndexFlag && 0 == u32ArrayIndex)
		*peDataType = BACNET_DT_UNSIGNED;

	/* check raw data length specifies empty value */
	if(0 == u32ApduDataLen)
		*peDataType = BACNET_DT_EMPTY;
	/* check raw data length specifies null value */
	else if(1 == u32ApduDataLen && pu8Apdu[i32Len] == 0x00)
		*peDataType = BACNET_DT_NULL;

	/* decode ar per data type */
	switch(*peDataType)
	{
	case BACNET_DT_EMPTY:
		break;

	case BACNET_DT_NULL:
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_NULL != u8TagNo || 0 != u32LenValueType)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* update the pointer value */
		pvData = NULL;
	}
	break;

	case BACNET_DT_BOOLEAN:
	{
		/* local variables */
		Pr_BACnetBool_t *pstBoolVal = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BOOLEAN != u8TagNo ||
			(1 != u32LenValueType && 0 != u32LenValueType))
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstBoolVal = (Pr_BACnetBool_t *)OSAL_Malloc(sizeof(Pr_BACnetBool_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstBoolVal)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
			pstBoolVal->m_bVal = Decode_Boolean(u32LenValueType);

		/* update the pointer value */
		pvData = (void *)pstBoolVal;
	}
	break;

	case BACNET_DT_UNSIGNED8:
	case BACNET_DT_UNSIGNED16:
	{
		/* change data type to unsigned 32 */
		*peDataType = BACNET_DT_UNSIGNED;
	}
	// Note : do not add break here

	case BACNET_DT_UNSIGNED:
	case BACNET_DT_UNSIGNED32:
	{
		/* local variables */
		Pr_BACnetUnsigned32_t *pstUint32 = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo || u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstUint32 = (Pr_BACnetUnsigned32_t *)OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstUint32)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, &u32DataValue);
			i32Len += i32DecodeLen;
			pstUint32->m_u32Val = u32DataValue;
		}

		/* update the pointer value */
		pvData = (void *)pstUint32;
	}
	break;

	case BACNET_DT_INTEGER:
	{
		/* local variables */
		Pr_BACnetSigned32_t *pstInt32 = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_SIGNED_INT != u8TagNo || u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstInt32 = (Pr_BACnetSigned32_t *)OSAL_Malloc(sizeof(Pr_BACnetSigned32_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstInt32)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Signed(&pu8Apdu[i32Len], u32LenValueType, (int32_t *)&u32DataValue);
			i32Len += i32DecodeLen;
			pstInt32->m_i32Val = u32DataValue;
		}

		/* update the pointer value */
		pvData = (void *)pstInt32;
	}
	break;

	/* NOTE : all enumerated data-types have same memory mapping, hence using single case */
	case BACNET_DT_ENUM_NEW:
	case BACNET_DT_OBJTYPE:
	case BACNET_DT_BACNETDEVSTAT:
	case BACNET_DT_BACKUPSTATE:
	case BACNET_DT_BACNETSEG:
	{
		/* local variables */
		Pr_BACnetEnum_t *pstEnum = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_ENUMERATED != u8TagNo || u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstEnum = (Pr_BACnetEnum_t *)OSAL_Malloc(sizeof(Pr_BACnetEnum_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstEnum)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, &u32DataValue);
			i32Len += i32DecodeLen;
			pstEnum->m_eVal = (enum_t)u32DataValue;
		}

		/* update the pointer value */
		pvData = (void *)pstEnum;
	}
	break;

	case BACNET_DT_ENUM:
	{
		/* local variables */
		Pr_BinaryEnumPV_t *pstEnum = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_ENUMERATED != u8TagNo || u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstEnum = (Pr_BinaryEnumPV_t *)OSAL_Malloc(sizeof(Pr_BinaryEnumPV_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstEnum)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, &u32DataValue);
			i32Len += i32DecodeLen;
			pstEnum->m_eVal = (BACNET_BINARY_PV)u32DataValue;
			pstEnum->m_ePVPolarity = POLARITY_NORMAL;
			pstEnum->m_pstNext = NULL;
		}

		/* update the pointer value */
		pvData = (void *)pstEnum;
	}
	break;

	case BACNET_DT_REAL:
	{
		/* local variables */
		Pr_BACnetReal_t *pstReal = NULL;
		Float_t fDataValue = 0.0;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_REAL != u8TagNo || u32LenValueType != 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstReal = (Pr_BACnetReal_t *)OSAL_Malloc(sizeof(Pr_BACnetReal_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstReal)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Real(&pu8Apdu[i32Len], &fDataValue);
			i32Len += i32DecodeLen;
			pstReal->m_fVal = fDataValue;
		}

		/* update the pointer value */
		pvData = (void *)pstReal;
	}
	break;

	case BACNET_DT_DOUBLE:
	{
		/* local variables */
		Pr_BACnetDouble_t *pstDouble = NULL;
		Double_t dDataValue = 0.0;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_DOUBLE != u8TagNo || u32LenValueType != 8)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstDouble = (Pr_BACnetDouble_t *)OSAL_Malloc(sizeof(Pr_BACnetDouble_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstDouble)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Double(&pu8Apdu[i32Len], &dDataValue);
			i32Len += i32DecodeLen;
			pstDouble->m_dVal = dDataValue;
		}

		/* update the pointer value */
		pvData = (void *)pstDouble;
	}
	break;

	case BACNET_DT_DATE:
	{
		/* local variables */
		Pr_BACnetDate_t *pstDate = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_DATE != u8TagNo || u32LenValueType != 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstDate = (Pr_BACnetDate_t *)OSAL_Malloc(sizeof(Pr_BACnetDate_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstDate)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Date(&pu8Apdu[i32Len], &pstDate->m_Date_val);
			i32Len += i32DecodeLen;
		}

		/* update the pointer value */
		pvData = (void *)pstDate;
	}
	break;

	case BACNET_DT_TIME:
	{
		/* local variables */
		Pr_BACnetTime_t *pstTime = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_TIME != u8TagNo || u32LenValueType != 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstTime = (Pr_BACnetTime_t *)OSAL_Malloc(sizeof(Pr_BACnetTime_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstTime)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Time(&pu8Apdu[i32Len], &pstTime->m_Time_val);
			i32Len += i32DecodeLen;
		}

		/* update the pointer value */
		pvData = (void *)pstTime;
	}
	break;

	case BACNET_DT_OBJECTID:
	{
		/* local variables */
		Pr_BACnetObjId_t *pstObjId = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_OBJECT_ID != u8TagNo || u32LenValueType != 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstObjId = (Pr_BACnetObjId_t *)OSAL_Malloc(sizeof(Pr_BACnetObjId_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstObjId)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&pstObjId->m_eObjectType, &pstObjId->m_u32ObjId);
			i32Len += i32DecodeLen;
		}

		/* update the pointer value */
		pvData = (void *)pstObjId;
	}
	break;

	case BACNET_DT_CHARSTRING:
	{
		/* local variables */
		Pr_BACnetCharStr_t *pstCharString = NULL;
		uint32_t u32MaxStrLen = 0;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_CHARACTER_STRING != u8TagNo)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstCharString = (Pr_BACnetCharStr_t *)OSAL_Malloc(sizeof(Pr_BACnetCharStr_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstCharString)
		{
			i32Len = DT_ERR_MALLOC_FAILED;
			break;
		}
		else
		{
			i32DecodeLen = Decode_Character_String(&pu8Apdu[i32Len], u32LenValueType, 
				&pstCharString->m_stCHARString);
			i32Len += i32DecodeLen;
		}

		/* check max char string length */
		u32MaxStrLen = ((pstCharString->m_stCHARString.m_u8Encoding == CHARACTER_IBM_MS_DBCS) ?
			(MAX_SUPPORTED_CHRSTR_LEN +2) : MAX_SUPPORTED_CHRSTR_LEN);
		if(u32LenValueType > u32MaxStrLen)
		{
			/* free allocated memory & set error */
			OSAL_Free(pstCharString, __FILE__,__FUNCTION__,__LINE__);
			pstCharString = NULL;			
			i32Len = DT_ERR_VALUE_OUT_OF_RANGE;
		}
		/* update the pointer value */
		pvData = (void *)pstCharString;
	}
	break;

	case BACNET_DT_OCTETSTRING:
	{
		/* local variables */
		Pr_BACnetOctetStr_t *pstOctetString = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_OCTET_STRING != u8TagNo)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstOctetString = (Pr_BACnetOctetStr_t *)OSAL_Malloc(sizeof(Pr_BACnetOctetStr_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstOctetString)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len], u32LenValueType, 
				&pstOctetString->m_stOctetString);
			i32Len += i32DecodeLen;
		}

		/* update the pointer value */
		pvData = (void *)pstOctetString;
	}
	break;

	case BACNET_DT_BITSTRING:
	{
		/* local variables */
		Pr_BACnetBitStr_t *pstBitString = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstBitString = (Pr_BACnetBitStr_t *)OSAL_Malloc(sizeof(Pr_BACnetBitStr_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstBitString)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_BitString_Small(&pu8Apdu[i32Len], u32LenValueType, 
				&pstBitString->m_stBitStr);
			i32Len += i32DecodeLen;
		}

		/* update the pointer value */
		pvData = (void *)pstBitString;
	}
	break;

	case BACNET_DT_BITSTRING_NEW:
	{
		/* local variables */
		Pr_BACnetBITStr_t *pstBitString = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* allocate memory & decode value */
		pstBitString = (Pr_BACnetBITStr_t *)OSAL_Malloc(sizeof(Pr_BACnetBITStr_t), __FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstBitString)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType, 
				&pstBitString->m_stBitString);
			i32Len += i32DecodeLen;
		}

		/* update the pointer value */
		pvData = (void *)pstBitString;
	}
	break;

	case BACNET_DT_DATETIME:
	{
		/* local variables */
		BACnetDateTime_t stDateTime = {0};
		Pr_BACnetDateTime_t *pstDateTime = NULL;

		/* decode the application tag */
		i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len], 
			&stDateTime.m_stDate, &stDateTime.m_stTime);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDateTime = (Pr_BACnetDateTime_t *)OSAL_Malloc(sizeof(Pr_BACnetDateTime_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstDateTime)
				i32Len = DT_ERR_MALLOC_FAILED;
			else
			{
				memcpy(&pstDateTime->m_stDateTime, &stDateTime, sizeof(BACnetDateTime_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDateTime;
	}
	break;

	case BACNET_DT_TIMESTAMP:
	{
		/* local variables */
		Pr_BACnetTimeStamp_t stTimeStamp = {0};
		Pr_BACnetTimeStamp_t *pstTimeStamp = NULL;

		/* decode the application tag */
		i32DecodeLen = DT_Decode_TimeStamp(&pu8Apdu[i32Len], 
			&stTimeStamp.m_stTimeStamp.m_stTimeStamp, 
			&stTimeStamp.m_stTimeStamp.m_eTimeStampType);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstTimeStamp = (Pr_BACnetTimeStamp_t *)OSAL_Malloc(sizeof(Pr_BACnetTimeStamp_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstTimeStamp)
				i32Len = DT_ERR_MALLOC_FAILED;
			else
			{
				memcpy(pstTimeStamp, &stTimeStamp, sizeof(Pr_BACnetTimeStamp_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstTimeStamp;
	}
	break;

	case BACNET_DT_OBJECT_TYPE_SUPPORTED:
	{
		/* local variables */
		BACnetBITStr_t stBitString = {0};
		Pr_BACnetObjectTypesSupported_t *pstObjSupport = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* decode value */
		i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType, &stBitString);
		i32Len += i32DecodeLen;

		/* allocate memory & copy value */
		pstObjSupport = (Pr_BACnetObjectTypesSupported_t *)OSAL_Malloc(sizeof(Pr_BACnetObjectTypesSupported_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstObjSupport)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			for(u32Count = 0; u32Count < MAX_OBJECT_TYPE_BYTE; u32Count++)
			{
				/* reverse the bits in bytes */
				pstObjSupport->m_stObjectSupport.Byte[u32Count] = 
					Byte_Reverse_Bits(stBitString.m_u8TransBits[u32Count]);
			}
		}

		/* update the pointer value */
		pvData = (void *)pstObjSupport;
	}
	break;

	case BACNET_DT_SERVICES_SUPPORTED:
	{
		/* local variables */
		BACnetBITStr_t stBitString = {0};
		Pr_BACnetServicesSupported_t *pstSerSupport = NULL;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			break;
		}

		/* decode value */
		i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType, &stBitString);
		i32Len += i32DecodeLen;

		/* allocate memory & copy value */
		pstSerSupport = (Pr_BACnetServicesSupported_t *)OSAL_Malloc(sizeof(Pr_BACnetServicesSupported_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstSerSupport)
			i32Len = DT_ERR_MALLOC_FAILED;
		else
		{
			for(u32Count = 0; u32Count < MAX_SERVICE_SUPPORT_BYTE; u32Count++)
			{
				/* reverse the bits in bytes */
				pstSerSupport->m_stServiceSupport.Byte[u32Count] = 
					Byte_Reverse_Bits(stBitString.m_u8TransBits[u32Count]);
			}
		}

		/* update the pointer value */
		pvData = (void *)pstSerSupport;
	}
	break;

	case BACNET_DT_OBJECTID_ARRAY:
    {
		/* local variables */
		Pr_ListOfObjId_t *pstListObjectID = NULL;
        ListOfObjId_t *pstObjectIdList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_ObjectID_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstObjectIdList, &u32Count);

		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstListObjectID = (Pr_ListOfObjId_t *)OSAL_Malloc(sizeof(Pr_ListOfObjId_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstListObjectID)
			{
				/* clear allocated memories & return error */
				Clear_ObjectID_List(&pstObjectIdList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstListObjectID->m_pstArrayObjId = pstObjectIdList;
				/* save list count */
				pstListObjectID->m_u32ObjCount = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstListObjectID;
	}
	break;

	case BACNET_DT_ADDRESSBINDING_LIST:
    {
		/* local variables */
        Pr_ListOfBACnetAddrBinding_t *pstAddrBindList = NULL;
		BACnetAddrBinding_t stAddBinding = {0};

		/* decode the application tag */
		i32DecodeLen = DT_Decode_AddressBinding_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stAddBinding, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstAddrBindList = (Pr_ListOfBACnetAddrBinding_t *)OSAL_Malloc(sizeof(Pr_ListOfBACnetAddrBinding_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstAddrBindList)
			{
				/* clear allocated memories & return error */
				Clear_AddressBinding_List(&stAddBinding.pstNext, false);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(&pstAddrBindList->m_stAddBinding, &stAddBinding, sizeof(BACnetAddrBinding_t));
				pstAddrBindList->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstAddrBindList;
	}
	break;

	case BACNET_DT_BACNETVTCLASS:
	{
		/* local variables */
		Pr_BACnetVTClass_t *pstVTClass = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_VtClass_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstVTClass, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}

		/* update the pointer value */
		i32Len += i32DecodeLen;
		pvData = (void *)pstVTClass;
	}
	break;

	case BACNET_DT_BACNETVTSESS:
	{
		/* local variables */
		Pr_ListOfBACnetVTSession_t *pstVTSession = NULL;
		ListOfBACnetVTSession_t *pstVTSessionList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_VtSession_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstVTSessionList, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}		
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstVTSession = (Pr_ListOfBACnetVTSession_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetVTSession_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstVTSession)
			{
				/* clear allocated memories & return error */
				Clear_VtSessions_List(&pstVTSessionList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstVTSession->m_pstListOfVTSession = pstVTSessionList;
				/* save list count */
				pstVTSession->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstVTSession;

	}
	break;

	case BACNET_DT_RECIPIENT_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetRecipient_t *pstRecipient = NULL;
		ListOfBACnetRecipient_t *pstRecipientList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_Recipient_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstRecipientList, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstRecipient = (Pr_ListOfBACnetRecipient_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetRecipient_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstRecipient)
			{
				/* clear allocated memories & return error */
				Clear_Recipient_List(&pstRecipientList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstRecipient->m_pstListOfRecipient = pstRecipientList;
				/* save list count */
				pstRecipient->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstRecipient;
	}
	break;

	case BACNET_DT_BOOLEAN_ARRAY:
	{
		/* local variables */
		Pr_ListOfBoolen_t *pstBoolArray = NULL;
		ListOfBoolen_t *pstBoolList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_Boolean_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstBoolList, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & save pointer */
			pstBoolArray = (Pr_ListOfBoolen_t *)OSAL_Malloc(sizeof(Pr_ListOfBoolen_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstBoolArray)
			{
				/* clear allocated memories & return error */
				Clear_Boolean_List(&pstBoolList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBoolArray->m_pstBoolList = pstBoolList;
				pstBoolArray->m_u32BoolCount = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBoolArray;

	}
	break;

	case BACNET_DT_COVSUB:
	{
		/* local variables */
		Pr_ListOfBACnetCovSubs_t *pstActCovList= NULL;
        ListOfBACnetCovSubs_t *pstCovSubs = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_CovSubscription_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstCovSubs, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & save pointer */
			pstActCovList = (Pr_ListOfBACnetCovSubs_t *)OSAL_Malloc(sizeof(Pr_ListOfBACnetCovSubs_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstActCovList)
			{
				/* clear allocated memories & return error */
				Clear_ActiveCovSubs_List(&pstCovSubs);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstActCovList->m_pstCOVSubscribe = pstCovSubs;
				pstActCovList->m_hcovsublistMtxLock = NULL;
				pstActCovList->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstActCovList;
	}
	break;

	case BACNET_DT_TIMESTAMP_ARRAY:
	{
		/* local variables */
		Pr_BACnetEventTimeStamp_t stTimeStamp = {0};
		Pr_BACnetEventTimeStamp_t *pstTimeStampArray = NULL;

		/* decode the application tag */
		i32DecodeLen = DT_Decode_TimeStamp_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stTimeStamp);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstTimeStampArray = (Pr_BACnetEventTimeStamp_t *)OSAL_Malloc(sizeof(Pr_BACnetEventTimeStamp_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstTimeStampArray)
				i32Len = DT_ERR_MALLOC_FAILED;
			else
			{
				memcpy(pstTimeStampArray, &stTimeStamp, sizeof(Pr_BACnetEventTimeStamp_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstTimeStampArray;
	}
	break;

	case BACNET_DT_PRIORITY_ARRAY:
    {
		/* local variables */
		Pr_BACnetPriorityArray_t stPriorityArray = {0};
        Pr_BACnetPriorityArray_t *pstPriorityArray = NULL;

		/* decode the application tag */
		i32DecodeLen = DT_Decode_Priority_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stPriorityArray);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstPriorityArray = (Pr_BACnetPriorityArray_t *)OSAL_Malloc(sizeof(Pr_BACnetPriorityArray_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstPriorityArray)
				i32Len = DT_ERR_MALLOC_FAILED;
			else
			{
				memcpy(pstPriorityArray, &stPriorityArray, sizeof(Pr_BACnetPriorityArray_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstPriorityArray;
	}
	break;

	case BACNET_DT_UNSIGNED_ARRAY:
	case BACNET_DT_UNSIGNED_LIST:
	{
		/* local variables */
		Pr_ListOfUnsigned_t *pstUintList = NULL;
		ListOfUnsigned_t *pstUintValueList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_Unsigned_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstUintValueList, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & save pointer */
			pstUintList = (Pr_ListOfUnsigned_t *)OSAL_Malloc(sizeof(Pr_ListOfUnsigned_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstUintList)
			{
				/* clear allocated memories & return error */
				Clear_UnsignedInt_List(&pstUintValueList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstUintList->m_pstUnsignVal = pstUintValueList;
				pstUintList->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstUintList;
	}
	break;
	case BACNET_DT_CLIENTCOVINCREMENT:
	{
		Pr_BACnetClientCOV_t *pstClientCOV = NULL;
		Pr_BACnetClientCOV_t stClientCOV = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_ClientCOVIncrement(&pu8Apdu[i32Len], &stClientCOV.m_stClientCOV);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstClientCOV = (Pr_BACnetClientCOV_t *)OSAL_Malloc(sizeof(Pr_BACnetClientCOV_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstClientCOV)
			{
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(pstClientCOV, &stClientCOV, sizeof(Pr_BACnetClientCOV_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstClientCOV;	
	}
	break;

    case BACNET_DT_DATERANGE:
	{
		BACnetDateRange_t  stDateRange = {0};
		Pr_BACnetDateRange_t *pstDateRangeProp = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_DateRange(&pu8Apdu[i32Len], &stDateRange);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDateRangeProp = (Pr_BACnetDateRange_t *)OSAL_Malloc(sizeof(Pr_BACnetDateRange_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstDateRangeProp)
			{
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(&pstDateRangeProp->m_stDateRange, 
					&stDateRange, sizeof(BACnetDateRange_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDateRangeProp;	
	}
	break;

    case BACNET_DT_DATELIST:
	{
		/* local variables */
        Pr_ListOfBACnetCalendarEntry_t *pstDateList = NULL;		
		ListOfBACnetCalendarEntry_t *pstCalenderList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_CalenderEntry_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstCalenderList, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDateList = (Pr_ListOfBACnetCalendarEntry_t *)OSAL_Malloc(sizeof(Pr_ListOfBACnetCalendarEntry_t),
				__FILE__,__FUNCTION__,__LINE__);
						
			if(NULL == pstDateList)
			{
				/* clear allocated memories & return error */
				Clear_CalendarEntry_List(&pstCalenderList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstDateList->m_pstListOfCalendar = pstCalenderList;								
				pstDateList->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDateList;
	}
	break;

	case BACNET_DT_OBJPROPREF:
	{
		Pr_BACnetObjPropRef_t *pstObjePropReff = NULL;
		Pr_BACnetObjPropRef_t stObjePropReff = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_ObjPropReff(&pu8Apdu[i32Len], &stObjePropReff.m_stObjPropRef);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstObjePropReff = (Pr_BACnetObjPropRef_t *)
				OSAL_Malloc(sizeof(Pr_BACnetObjPropRef_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstObjePropReff)
			{
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(pstObjePropReff, 
					&stObjePropReff, sizeof(Pr_BACnetObjPropRef_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstObjePropReff;
	}
	break;

    case BACNET_DT_DEVOBJPROPREF:
	{
		Pr_BACnetDevObjPropRef_t *pstDevObjPropRef = NULL;
		Pr_BACnetDevObjPropRef_t stDevObjPropRef = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_DevObjPropReff(&pu8Apdu[i32Len], &stDevObjPropRef.m_stDevObjPropReff);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDevObjPropRef = (Pr_BACnetDevObjPropRef_t *)
				OSAL_Malloc(sizeof(Pr_BACnetDevObjPropRef_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstDevObjPropRef)
			{
				/* clear allocated memories & return error */
				// TODO
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(pstDevObjPropRef, 
					&stDevObjPropRef, sizeof(Pr_BACnetDevObjPropRef_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDevObjPropRef;
	}
	break;

	case BACNET_DT_SETPOINTREF:
	{
		Pr_BACnetSetpointRef_t *pstSetPtReff = NULL;
		Pr_BACnetSetpointRef_t stSetPtReff = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_SetPtReff(&pu8Apdu[i32Len], &stSetPtReff.m_stSptRef);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstSetPtReff = (Pr_BACnetSetpointRef_t *)OSAL_Malloc(sizeof(Pr_BACnetSetpointRef_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstSetPtReff)
			{
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(pstSetPtReff, &stSetPtReff, sizeof(Pr_BACnetSetpointRef_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstSetPtReff;
	}
	break;

	case BACNET_DT_DESTINATION_LIST:
	{
		Pr_ListOfBACnetDestination_t *pstDestinations = NULL;
		ListOfBACnetDestination_t *pstRecepientList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_Destination_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstRecepientList, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDestinations = (Pr_ListOfBACnetDestination_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetDestination_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstDestinations)
			{
				/* clear allocated memories & return error */
				Clear_Destination_List(&pstRecepientList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstDestinations->m_pstNCRecepient = pstRecepientList;								
				pstDestinations->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDestinations;
	}
	break;

	case BACNET_DT_DEVOBJPROPREFF_LIST:
	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
	{
		Pr_ListOfBACnetDevObjPropRef_t *pstListOfDevObjPropReff = NULL;
		ListOfBACnetDevObjPropRef_t *pstDevObjPropReffList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_DevObjPropRef_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstDevObjPropReffList, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstListOfDevObjPropReff = (Pr_ListOfBACnetDevObjPropRef_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetDevObjPropRef_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstListOfDevObjPropReff)
			{
				/* clear allocated memories & return error */
				Clear_DevObjPropReff_List(&pstDevObjPropReffList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstListOfDevObjPropReff->m_pstListOfBACnetDevObjPropReff = 
					pstDevObjPropReffList;	
				pstListOfDevObjPropReff->m_u32ArraySize = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstListOfDevObjPropReff;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	{
		Pr_ListOfBACnetDailySchedule_t *pstWeeklySchedule = NULL;
		Pr_ListOfBACnetDailySchedule_t stWeeklySchedule = {0};

		/* decode the application tag */
		i32DecodeLen = DT_Decode_DailySchedule_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stWeeklySchedule, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstWeeklySchedule = (Pr_ListOfBACnetDailySchedule_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetDailySchedule_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstWeeklySchedule)
			{
				/* clear allocated memories */
				Clear_WeeklyShdl_List(&stWeeklySchedule);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(pstWeeklySchedule, &stWeeklySchedule, sizeof(Pr_ListOfBACnetDailySchedule_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstWeeklySchedule;
	}
	break;	

	case BACNET_DT_SPECIALEVENT_ARRAY:
	{
		Pr_ListOfBACnetSpecialEvent_t *pstBACnetListOfSpecialEvent = NULL;
		ListOfSpecialEvent_t *pstListOfSpecialEvent = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_SpecialEvent_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstListOfSpecialEvent, &u32Count, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetListOfSpecialEvent = (Pr_ListOfBACnetSpecialEvent_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetSpecialEvent_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetListOfSpecialEvent)
			{
				/* clear allocated memories & return error */
				Clear_ExceptionShdl_List(&pstListOfSpecialEvent);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetListOfSpecialEvent->m_pstSplEvent = 
					pstListOfSpecialEvent;	
				pstBACnetListOfSpecialEvent->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetListOfSpecialEvent;
	}
	break;

	case BACNET_DT_LOGBUFFER_TRENDLOG:
	{
		Pr_ListOfBACnetLogRecord_t *pstBACnetLogRecord = NULL;
		ListOfBACnetLogRecord_t *pstLogRecordList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_LogRecord_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstLogRecordList, &u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetLogRecord = (Pr_ListOfBACnetLogRecord_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetLogRecord_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetLogRecord)
			{
				/* clear allocated memories & return error */
				Clear_TrendLogRecord_List(&pstLogRecordList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetLogRecord->m_pstLogRecord = 
					pstLogRecordList;				
				pstBACnetLogRecord->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetLogRecord;	
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_EVENTPARAMETERS:
	{
		Pr_BACnetEventParameter_t *pstBACnetEventParameter = NULL;
		Pr_BACnetEventParameter_t stBACnetEventParameter = {0};

		/* decode the data */
		i32DecodeLen = DT_Decode_EventParameters(&pu8Apdu[i32Len], 
			&stBACnetEventParameter.m_stEventParam, u32ApduDataLen);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetEventParameter = (Pr_BACnetEventParameter_t *)
				OSAL_Malloc(sizeof(Pr_BACnetEventParameter_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstBACnetEventParameter)
			{
				/* clear allocated memories & return error */
				// TODO
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(pstBACnetEventParameter, 
					&stBACnetEventParameter, sizeof(Pr_BACnetEventParameter_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetEventParameter;
	}
	break;

	case BACNET_DT_BITSTRING_ARRAY:
	{
		Pr_ListOfBitStr_t	*pstBitStringList = NULL;
		ListOfBitStr_t *pstBacnet_BitStringList = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_BitString_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstBacnet_BitStringList, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBitStringList = (Pr_ListOfBitStr_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBitStr_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBitStringList)
			{
				/* clear allocated memories & return error */
				Clear_BitString_List(&pstBacnet_BitStringList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBitStringList->m_pstBitString = 
					pstBacnet_BitStringList;
				pstBitStringList->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBitStringList;
	}
	break;
	case BACNET_DT_OPTIONAL_UNSIGNED:
	{
		/* local variables */
		Pr_BACnetOptionalUnsigned_t *pstOptionalUnsigned = NULL;
		Pr_BACnetOptionalUnsigned_t stOptionalUnsigned = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_OptionalUnsigned(&pu8Apdu[i32Len],
			&stOptionalUnsigned.m_stOptionalUnsigned);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstOptionalUnsigned = (Pr_BACnetOptionalUnsigned_t *)
				OSAL_Malloc(sizeof(Pr_BACnetOptionalUnsigned_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstOptionalUnsigned)
			{
				/* clear allocated memories if any & return error */
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				/* save data */
				pstOptionalUnsigned->m_stOptionalUnsigned.m_eDataType =
					stOptionalUnsigned.m_stOptionalUnsigned.m_eDataType;
				pstOptionalUnsigned->m_stOptionalUnsigned.m_u32Val =
					stOptionalUnsigned.m_stOptionalUnsigned.m_u32Val;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstOptionalUnsigned;
	}
	break;
	case BACNET_DT_OPTCHARSTRING_ARRAY:
	{
		Pr_ListOfOptCharStr_t *pstOptcharStringArr = NULL;
		ListOfOptCharStr_t *pstOptCharStrList = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_OpCharString_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstOptCharStrList, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstOptcharStringArr = (Pr_ListOfOptCharStr_t *)
				OSAL_Malloc(sizeof(Pr_ListOfOptCharStr_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstOptcharStringArr)
			{
				/* clear allocated memories & return error */
				Clear_OptionalCharString_List(&pstOptCharStrList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstOptcharStringArr->m_pstOptCharStr = 
					pstOptCharStrList;
				pstOptcharStringArr->m_u32Count = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstOptcharStringArr;
	}
	break;

	case BACNET_DT_DEVOBJREFF_ARRAY:
	case BACNET_DT_DEVOBJREFF_LIST:
	{
		Pr_ListOfBACnetDevObjRef_t *pstListOfBACnetDevObjReff = NULL;
		ListOfBACnetDevObjRef_t *pstListOfDevObjReff = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_DevObjRef_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstListOfDevObjReff, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstListOfBACnetDevObjReff = (Pr_ListOfBACnetDevObjRef_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetDevObjRef_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstListOfBACnetDevObjReff)
			{
				/* clear allocated memories & return error */
				Clear_DevObjRef_List(&pstListOfDevObjReff);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstListOfBACnetDevObjReff->m_pstListOfDevObjReff = 
					pstListOfDevObjReff;
				pstListOfBACnetDevObjReff->m_u32ArraySize = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstListOfBACnetDevObjReff;
	
	}
	break;

	case BACNET_DT_SHEDLEVEL:
	{
		Pr_BACnetShedLevel_t *pstBacnetShedLevel = NULL;
		Pr_BACnetShedLevel_t stBacnetShedLevel = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_ShedLevel(&pu8Apdu[i32Len],&stBacnetShedLevel);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBacnetShedLevel = (Pr_BACnetShedLevel_t *)OSAL_Malloc(sizeof(Pr_BACnetShedLevel_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstBacnetShedLevel)
			{
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(pstBacnetShedLevel, &stBacnetShedLevel, sizeof(Pr_BACnetShedLevel_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBacnetShedLevel;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_LOGBUFFER_TLM:
	{
		Pr_ListOfBACnetLogMultipleRecord_t *pstBACnetLogMultipleRecord = NULL;
		ListOfBACnetLogMultipleRecord_t *pstLogMultipleRecord = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_LogMultipleRecord_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstLogMultipleRecord, &u32Count, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetLogMultipleRecord = (Pr_ListOfBACnetLogMultipleRecord_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetLogMultipleRecord_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetLogMultipleRecord)
			{
				/* clear allocated memories & return error */
				Clear_TrendLogMultipleRecord_List(&pstLogMultipleRecord);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetLogMultipleRecord->m_pstLogMultipleRecord = 
					pstLogMultipleRecord;				
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetLogMultipleRecord;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_ENUM_LIST:
	{
		Pr_ListOfEnum_t *pstBacnetEnumList = NULL;
		ListOfEnum_t *pstEnumList = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_Enum_List(&pu8Apdu[i32Len],u32ApduDataLen, 
			&pstEnumList, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBacnetEnumList = (Pr_ListOfEnum_t *)OSAL_Malloc(sizeof(Pr_ListOfEnum_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBacnetEnumList)
			{
				/* clear allocated memories & return error */
				Clear_Enumeration_List(&pstEnumList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBacnetEnumList->m_pstEnumList = 	pstEnumList;
				pstBacnetEnumList->m_u32Count = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBacnetEnumList;	
	}
	break;

	case BACNET_DT_LOGBUFFER_EL:
	{
		Pr_ListOfBACnetEventLogRecord_t *pstBACnetEventLogRecord = NULL;
		ListOfBACnetEventLogRecord_t *pstEventLogRecord = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_EventLogRecord(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstEventLogRecord, &u32Count, u32ApduDataLen,ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetEventLogRecord = (Pr_ListOfBACnetEventLogRecord_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetEventLogRecord_t),	__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetEventLogRecord)
			{
				/* clear allocated memories & return error */
				Clear_EventLogRecord_List(&pstEventLogRecord);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetEventLogRecord->m_pstEventLogRecord = pstEventLogRecord;				
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetEventLogRecord;	
	}
	break;

	case BACNET_DT_CHARSTRING_ARRAY:
	{
		Pr_ListOfCharStr_t *pstCharStringList = NULL;
		ListOfCharStr_t stBacnetCharStrList = {0};

		/* decode the data */
		i32DecodeLen = DT_Decode_CharString_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stBacnetCharStrList, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstCharStringList = (Pr_ListOfCharStr_t *)OSAL_Malloc(sizeof(Pr_ListOfCharStr_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstCharStringList)
			{
				/* clear allocated memories & return error */
				Clear_CharString_List(&stBacnetCharStrList.m_pstNext);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(&pstCharStringList->m_stStringVal, &stBacnetCharStrList, 
					sizeof(ListOfCharStr_t));				
				pstCharStringList->m_u32Count = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstCharStringList;
	}
	break;

	case BACNET_DT_READACCESSSPECS_LIST:
	{
		Pr_ListOfReadAccessSpecs_t *pstReadAccessSpecif = NULL;
		ListOfReadAccessSpecs_t			*pstGpOfObj = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_ReadAccessSpecs_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstGpOfObj, &u32Count, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstReadAccessSpecif = (Pr_ListOfReadAccessSpecs_t *)OSAL_Malloc(sizeof(Pr_ListOfReadAccessSpecs_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstReadAccessSpecif)
			{
				/* clear allocated memories & return error */
				Clear_ReadAccessSpecs_List(&pstGpOfObj);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstReadAccessSpecif->m_pstGpOfObj = pstGpOfObj;				
				pstReadAccessSpecif->m_u32Count = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstReadAccessSpecif;
	}
	break;

	case BACNET_DT_READACCESSRESULT_LIST:
	{
		Pr_ListOfReadAccessResult_t *pstReadAccessResult = NULL;
		ListOfReadAccessResult_t *pstGpObjResults = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_ReadAccessResult_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstGpObjResults, &u32Count, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstReadAccessResult = (Pr_ListOfReadAccessResult_t *)OSAL_Malloc(sizeof(Pr_ListOfReadAccessResult_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstReadAccessResult)
			{
				/* clear allocated memories & return error */
				Clear_ReadAccessResult_List(&pstGpObjResults);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstReadAccessResult->m_pstGpObjResults = pstGpObjResults;				
				pstReadAccessResult->m_u32Count = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstReadAccessResult;
	}
	break;

	case BACNET_DT_NOTIFICATIONPRIORITY:
	{
		/* local variables */
		Pr_BACnetNotifyPriority_t stNotifyPriority = {0};
		Pr_BACnetNotifyPriority_t *pstNotifyPriority = NULL;

		/* decode the application tag */
		i32DecodeLen = DT_Decode_NotificationPriority(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stNotifyPriority);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstNotifyPriority = (Pr_BACnetNotifyPriority_t *)
				OSAL_Malloc(sizeof(Pr_BACnetNotifyPriority_t),	__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstNotifyPriority)
			{
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(pstNotifyPriority, &stNotifyPriority,
					sizeof(Pr_BACnetNotifyPriority_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstNotifyPriority;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_ACTION_LIST_ARRAY:
	{
		Pr_ListOfBACnetActionList_t *pstBACnetActionList = NULL;
		ListOfBACnetActionList_t *pstArrayOfActionCommand = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_Action_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstArrayOfActionCommand, &u32Count, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetActionList = (Pr_ListOfBACnetActionList_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetActionList_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetActionList)
			{
				/* clear allocated memories & return error */
				Clear_Action_List(&pstArrayOfActionCommand);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetActionList->m_pstArrayActCmd = pstArrayOfActionCommand;				
				pstBACnetActionList->m_u32Count = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstBACnetActionList;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_AUTHENTICATION_FACTOR:
	{
		Pr_BACnetAuFactor_t *pstBACnetAuthenticationFactor = NULL;
		BACnetAuFactor_t stAuthenticationFactor = {0};

		/* decode the authentication factor */
		i32DecodeLen = DT_Decode_AuthenticationFactor(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stAuthenticationFactor);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetAuthenticationFactor = (Pr_BACnetAuFactor_t *)
				OSAL_Malloc(sizeof(Pr_BACnetAuFactor_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstBACnetAuthenticationFactor)
			{
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(&pstBACnetAuthenticationFactor->m_stAuFactor,
					&stAuthenticationFactor, sizeof(BACnetAuFactor_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetAuthenticationFactor;
	}
	break;

	case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:
	{
		Pr_ListOfBACnetAuFactorFormat_t *pstBACnetAuthFacFormat = NULL;
		ListOfBACnetAuFactorFormat_t *pstAuthFactorFormat_Array = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_AuthFactorFormat_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstAuthFactorFormat_Array, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetAuthFacFormat = (Pr_ListOfBACnetAuFactorFormat_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetAuFactorFormat_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetAuthFacFormat)
			{
				/* clear allocated memories & return error */
				Clear_AuFactorFormat_List(&pstAuthFactorFormat_Array);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetAuthFacFormat->m_pstAuFactFormatList = pstAuthFactorFormat_Array;				
				pstBACnetAuthFacFormat->m_u32ArraySize = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstBACnetAuthFacFormat;
	}
	break;

	case BACNET_DT_CRED_AU_FACTOR_ARRAY:
	{
		Pr_ListOfBACnetCredAuFactor_t *pstBACnetCredAuFactor = NULL;
		ListOfBACnetCredAuFactor_t *pstCredentialAuthFactor = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_CredentialAuthFactor_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstCredentialAuthFactor, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetCredAuFactor = (Pr_ListOfBACnetCredAuFactor_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetCredAuFactor_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetCredAuFactor)
			{
				/* clear allocated memories & return error */
				Clear_CredentialAuFactor_List(&pstCredentialAuthFactor);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetCredAuFactor->m_pstCredAuFactArray = pstCredentialAuthFactor;				
				pstBACnetCredAuFactor->m_u32ArraySize = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstBACnetCredAuFactor;
	}
	break;

	case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
	{
		Pr_ListOfBACnetAuPolicy_t *pstBACnetAuthPolicy = NULL;
		ListOfBACnetAuPolicy_t *pstAuthPolicy = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_AuthenticationPolicy_List(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstAuthPolicy, &u32Count, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetAuthPolicy = (Pr_ListOfBACnetAuPolicy_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetAuPolicy_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetAuthPolicy)
			{
				/* clear allocated memories & return error */
				Clear_AuPolicy_List(&pstAuthPolicy);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetAuthPolicy->m_pstAuPolicy = pstAuthPolicy;				
				pstBACnetAuthPolicy->m_u32ArraySize = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstBACnetAuthPolicy;
	}
	break;

	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
	{
		Pr_ListOfBACnetAssignedAccessRights_t *pstBACnetAssAccessRights = NULL;
		ListOfBACnetAssignedAccessRights_t *pstAsngdAccessArray = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_AssAccessRights_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstAsngdAccessArray, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetAssAccessRights = (Pr_ListOfBACnetAssignedAccessRights_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetAssignedAccessRights_t),
				__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetAssAccessRights)
			{
				/* clear allocated memories & return error */
				Clear_AssignedAccessRights_List(&pstAsngdAccessArray);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetAssAccessRights->m_pstAsngdAccessArray = pstAsngdAccessArray;				
				pstBACnetAssAccessRights->m_u32ArraySize = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstBACnetAssAccessRights;
	}
	break;

	case BACNET_DT_ACCESS_RULE_ARRAY:
	{
		Pr_ListOfBACnetAccessRule_t *pstBACnetAccessRules = NULL;
		ListOfBACnetAccessRule_t	    *pstAccessRule_Array = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_AccessRule_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstAccessRule_Array, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetAccessRules = (Pr_ListOfBACnetAccessRule_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetAccessRule_t),	__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetAccessRules)
			{
				/* clear allocated memories & return error */
				Clear_AccessRule_List(&pstAccessRule_Array);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetAccessRules->m_pstAccessRuleArray = pstAccessRule_Array;				
				pstBACnetAccessRules->m_u32ArraySize = u32Count;
			}
		}
		
		/* update the pointer value */
		pvData = (void *)pstBACnetAccessRules;
	}
	break;

	case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
	{
		/* local variables */
		AnyValue_t *pstAny = NULL;
		BACNET_PROPERTY_VALUE stAnyValue = {0};
		
		/* decode data */
		i32DecodeLen = DT_Decode_AnyValue(&pu8Apdu[i32Len], &stAnyValue);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;			
			/* allocate memory & decode value */
			pstAny = (AnyValue_t *)OSAL_Malloc(sizeof(AnyValue_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstAny)
			{
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				/* copy data */
				memcpy(&pstAny->m_stValue, &stAnyValue, sizeof(BACNET_PROPERTY_VALUE));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstAny;
	}
	break;

	case BACNET_DT_DEVOBJREFF:
	{
		Pr_BACnetDevObjRef_t *pstDevObjRef = NULL;
		Pr_BACnetDevObjRef_t stDevObjRef = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_DevObjReff(&pu8Apdu[i32Len], &stDevObjRef.m_stDeviceObjReff);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDevObjRef = (Pr_BACnetDevObjRef_t *)
				OSAL_Malloc(sizeof(Pr_BACnetDevObjRef_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstDevObjRef)
			{
				/* clear allocated memories & return error */
				// TODO
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(pstDevObjRef, 
					&stDevObjRef, sizeof(Pr_BACnetDevObjRef_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDevObjRef;
	}
	break;

#ifdef BACDEL_PR12

	case BACNET_DT_ADDRESSBINDING:
	{
		Pr_BACnetAddrBinding_t *pstDevAddressBind = NULL;
		Pr_BACnetAddrBinding_t stDevAddBind = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_AddressBinding(&pu8Apdu[i32Len], &stDevAddBind.m_stAddBinding);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstDevAddressBind = (Pr_BACnetAddrBinding_t *)OSAL_Malloc(sizeof(Pr_BACnetAddrBinding_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstDevAddressBind)
			{
				i32Len = DT_ERR_MALLOC_FAILED;				
			}
			else
			{
				memcpy(&pstDevAddressBind->m_stAddBinding, 
					&stDevAddBind.m_stAddBinding, sizeof(BACnetAddrBinding_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstDevAddressBind;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_NW_SECURITY_POLICY_ARRAY:
	{
		Pr_ListOfBACnetNwSecurityPolicy_t *pstBACnetNwSecurityPolicy = NULL;
		ListOfBACnetNwSecurityPolicy_t *pstNwSecurityPolicy = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_NwSecurityPolicy_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstNwSecurityPolicy, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetNwSecurityPolicy = (Pr_ListOfBACnetNwSecurityPolicy_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetNwSecurityPolicy_t),	__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetNwSecurityPolicy)
			{
				/* clear allocated memories & return error */
				Clear_NwSecurityPolicy_List(&pstNwSecurityPolicy);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetNwSecurityPolicy->m_pstSecurityPolicyArr = pstNwSecurityPolicy;				
				pstBACnetNwSecurityPolicy->m_u32ArraySize = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetNwSecurityPolicy;
	}
	break;

	case BACNET_DT_SECURITY_KEYSET_ARRAY:
	{
		Pr_ListOfBACnetSecurityKeySet_t *pstSecurityKeySet = NULL;
		Pr_ListOfBACnetSecurityKeySet_t stSecKeySet = {0};

		/* decode the application tag */
		i32DecodeLen = DT_Decode_SecurityKeySet_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stSecKeySet, ePropID);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstSecurityKeySet = (Pr_ListOfBACnetSecurityKeySet_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetSecurityKeySet_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstSecurityKeySet)
			{
				/* clear allocated memories */
				Clear_SecurityKeySet_List(&stSecKeySet);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				memcpy(pstSecurityKeySet, &stSecKeySet, sizeof(Pr_ListOfBACnetSecurityKeySet_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstSecurityKeySet;
	}
	break;	

	case BACNET_DT_PROPERTYACCESSRESULT_ARRAY:
	{
		Pr_ListOfBACnetPropAccessRslt_t *pstBACnetPropAccResult = NULL;
		ListOfBACnetPropAccessRslt_t *pstAccessResult = NULL;

		/* decode the data */
		i32DecodeLen = DT_Decode_PropertyAccessResult_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &pstAccessResult, &u32Count);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBACnetPropAccResult = (Pr_ListOfBACnetPropAccessRslt_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetPropAccessRslt_t),	__FILE__,__FUNCTION__,__LINE__);			

			if(NULL == pstBACnetPropAccResult)
			{
				/* clear allocated memories & return error */
				Clear_PropAccessResult_List(&pstAccessResult);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				pstBACnetPropAccResult->m_pstPropAccRslt = pstAccessResult;				
				pstBACnetPropAccResult->m_u32ArraySize = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBACnetPropAccResult;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_EVENT_MSG_TEXT:
	{
		/* local variables */
		Pr_BACnetEventMsgText_t stEveMsgText = {0};
		Pr_BACnetEventMsgText_t *pstMsgTextArray = NULL;

		/* decode the application tag */
		i32DecodeLen = DT_Decode_EveMsgTxt_Array(&pu8Apdu[i32Len], 
			u32ApduDataLen, &stEveMsgText);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstMsgTextArray = (Pr_BACnetEventMsgText_t *)OSAL_Malloc(sizeof(Pr_BACnetEventMsgText_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstMsgTextArray)
				i32Len = DT_ERR_MALLOC_FAILED;
			else
			{
				memcpy(pstMsgTextArray, &stEveMsgText, sizeof(Pr_BACnetEventMsgText_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstMsgTextArray;
	}
	break;

#endif /* BACDEL_PR12 */
	case BACNET_DT_BDT_ENTRY_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetBDTEntry_t *pstBdtEntry = NULL;
		ListOfBACnetBDTEntry_t *pstBdtEntryList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_BdtEntry_List(&pu8Apdu[i32Len],
			u32ApduDataLen, &pstBdtEntryList,
			&u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstBdtEntry = (Pr_ListOfBACnetBDTEntry_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetBDTEntry_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstBdtEntry)
			{
				/* clear allocated memories if any & return error */
				Clear_BdtEntry_List(&pstBdtEntryList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				/* save data */
				pstBdtEntry->m_pstBDTEntryList = pstBdtEntryList;
				pstBdtEntry->m_u32ArraySize = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstBdtEntry;
	}
	break;

	case BACNET_DT_FDT_ENTRY_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetFDTEntry_t *pstFdtEntry = NULL;
		ListOfBACnetFDTEntry_t *pstFdtEntryList = NULL;

		/* decode data */
		i32DecodeLen = DT_Decode_FdtEntry_List(&pu8Apdu[i32Len],
			u32ApduDataLen, &pstFdtEntryList,
			&u32Count, pu32FirstFailedElement);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstFdtEntry = (Pr_ListOfBACnetFDTEntry_t *)
				OSAL_Malloc(sizeof(Pr_ListOfBACnetFDTEntry_t),
				__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstFdtEntry)
			{
				/* clear allocated memories if any & return error */
				Clear_FdtEntry_List(&pstFdtEntryList);
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				/* save data */
				pstFdtEntry->m_pstFDTEntryList = pstFdtEntryList;
				pstFdtEntry->m_u32ArraySize = u32Count;
			}
		}

		/* update the pointer value */
		pvData = (void *)pstFdtEntry;
	}
	break;
	case BACNET_DT_HOSTNPORT:		//data type added for fdbbmd entry
	{
		/* local variables */
		Pr_BACnetHostNPort_t *pstHostNPort = NULL;
		BACnetHostNPort_t stHostNPort = {0};

		/* decode data */
		i32DecodeLen = DT_Decode_HostNPort(&pu8Apdu[i32Len],
			&stHostNPort);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */
			i32Len = i32DecodeLen;
			break;
		}
		else
		{
			i32Len += i32DecodeLen;
			/* allocate memory & copy value */
			pstHostNPort = (Pr_BACnetHostNPort_t *)
				OSAL_Malloc(sizeof(Pr_BACnetHostNPort_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstHostNPort)
			{
				/* clear allocated memories if any & return error */
				i32Len = DT_ERR_MALLOC_FAILED;
			}
			else
			{
				/* save data */
				memcpy(&pstHostNPort->m_stHostNPort, &stHostNPort,
					sizeof(BACnetHostNPort_t));
			}
		}

		/* update the pointer value */
		pvData = (void *)pstHostNPort;
	}
	break;


	default:
	{
		/* default case - return as empty data-type */
		*peDataType = BACNET_DT_EMPTY;
		pvData = NULL;
		i32Len = u32ApduDataLen;
	}
	break;

	}//switch ends

	/* if decoded data length is less that zero, return error */
	if(i32Len < 0)
	{
		/* Note: memory should be freed in individual cases */
		/* return error */
		*peDataType = BACNET_DT_MAX;
		pvData = NULL;
	}
	/* if decoded data length is less that zero, return error */
	else if(i32Len != (int32_t)u32ApduDataLen)
	{
		/* Note: memory should be freed in individual cases */
		/* return error */
		if(*peDataType == BACNET_DT_DAILYSCHEDULE_ARRAY)
		    i32Len = DT_ERR_VALUE_OUT_OF_RANGE;
		else
		    i32Len = DT_ERR_INVALID_DATA_TYPE;
		Clear_PropVal_AsPer_DataType(*peDataType, &pvData);
		*peDataType = BACNET_DT_MAX;
		pvData = NULL;
	}

	/* return the pointer containing decoded data */
	*ppPropValue = pvData;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	BACApp_Decode_Data_Type: exit \r\n");
	#endif
	
	/* return the decoded data length */
	return i32Len;
}



