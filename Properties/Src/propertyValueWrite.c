/*************************************************************************
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
*   SoftDEL Systems Ltd.                     india@softdel.com         
*   3rd Floor, Pentagon P4,                  http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - propertyValueWrite.c
*
*   RELEASE HISTORY
*      
*     DATE         NAME                DESCRIPTION
*
*   25/07/2011     Prashant Badgujar   File Created & Added generic write API.
*   20/07/2011     M.Venu              Redesigned WriteCommonProperties function
*                                      logic using macro definations
*   26/07/2011     M.Venu              Added GetPropertyTag, GetPriorityArrTag,
*                                      Convert_AppTag_WriteProperty ,
*                                      ValidateSpecialPropertyTag,ValidateWriteTag
*                                      Write_Object_Property functions.
*   27/07/2011     M.Venu              Updated Convert_AppTag_WriteProperty function 
*                                      with all allowed Data Tags
*   03/08/2011     M.Venu              Link list support given for list of int 
*                                      and list of char data type write functionality. 
*   04/08/2011     M.Venu              Allowed special tag validation for DateTime
*                                      
*	17/08/2011	   Prashant 		   Adding support for device in Convert_AppTag_WriteProperty API.
*   10/09/2011     M.Venu              Defect Fix R2-5
*   16/09/2011     M.Venu              For enumerated data types value change
*                                      comprision removed with -0x30 as did
*                                      atoi in objproperty.c file at the time
*                                      of Add property functionality
*	16/09/2011		Prashant		   Adding HW Interface check before doing Write Operation
*   16/09/2011      M.Venu             Read and Add Common property function 
*                                      calls replaced with new generic function calls
*   20/09/2011      M.Venu             1. Updated Priority Array functionality 
*                                      2. Allowd write NULL Tag value for present
*                                         value property of all objects
*   20/09/2011      M.Venu             1. made Default value for g_i32ArrayIndex as 15
*   22/09/2011      M.Venu             Supported ROP_ELAPSED_ACTIVE_TIME functionality
*   23/09/2011      Ashish Verma       Modified the charater string structure 
*                                      from BACNET_CHARACTER_STRING to
*                                      Pr_BACnetCharStr_t.
*   29/12/11        Harshal M.         Added function CheckPropertyValRange
*   04/09/12        Heramb Joshi       Added context specific write property support
*************************************************************************/

#include "osalFreeRTOS.h"

#include "propertyValueWrite.h"
#include "propertyGenricHandler.h"
#include "propertyDataType.h"
#include "propertyValueRead.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "bacDELApi.h"
#include "miscMiscellaneous.h"
#include "bacDELDeviceConfig.h"
#include "propertyValidations.h"
#include "propertyClearValues.h"
#include "propertyValidations.h"
#include "propertyWpCallback.h"
#include "propertyTagType.h"
#ifdef BACDEL_SER_DS_COV_B
#include "serviceChangeOfValue_B.h"
#endif

#ifdef BACDEL_OBJ_SDL
#include "serviceScheduling_B.h"
#endif

/* include for A-side services */
#if (defined INITIATE_SERVICE_ENABLED || defined BACDEL_SER_DS_WP_B)
#include "bacnetInitiateServiceMgmt.h"
#endif

/* Used to identify UpdateCommonProperty function call is from 
   recursive or from actual write functionality */
bool    g_bWriteRequest    = false;

/** global variable for stack initialization status */
extern bool g_bBACnetStackInitFlag;
extern DB_t SMCfg;

/**
*
* DESCRIPTION
* Function returns property tag-type based on property ID and object ID.
*
* @param eObjectType		[in] Object Type
* @param eObjectProperty	[in] Property Id
* @param u32ArrayIndex		[in] Array index value
* @param bArrIndxPresent	[in] Array index flag
*
* @return BACNET_APPLICATION_TAG  [out] Tag-type value
*
*/
BACNET_APPLICATION_TAG GetPropertyTag(
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
    /** laocal variables */
    BACNET_REMAPPED_OBJECT_TYPE  eRemapObjType = REMAPPED_MAX_BACNET_OBJECT_TYPE;
    BACNET_APPLICATION_TAG ePropTAGtype = TAG_NOT_SUPPORTED;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:GetPropertyTag : Entry \r\n");
	#endif

    /** Remap object id as per Default Access type table */
    eRemapObjType = Remap_Objct_Id(eObjectType);

    /* check the array index value */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
        u32ArrayIndex = BACNET_ARRAY_ALL;

    if(eRemapObjType != REMAPPED_MAX_BACNET_OBJECT_TYPE &&
        eObjectProperty < MAX_PROP_SUPPORTED )
    {
        /* Get the Property access type depending upon device type & property  type */
        ePropTAGtype = au8PropertyTagType[eObjectProperty][eRemapObjType];

        if((u32ArrayIndex == 0) &&
            (ePropTAGtype == BACNET_APPLICATION_TAG_LISTCHAR ||
            ePropTAGtype == BACNET_APPLICATION_TAG_LISTUNSIGN) )
        {
            ePropTAGtype = BACNET_APPLICATION_TAG_UNSIGNED_INT;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetPropertyTag : Exit \r\n");
	#endif

	/* return tag type value */
    return (ePropTAGtype);
}

/**
*
* DESCRIPTION
* Function returns property data-type based on property ID and object ID.
*
* @param eObjectType		[in] Object Type
* @param eObjectProperty	[in] Property Id
* @param u32ArrayIndex		[in] Array index value
* @param bArrIndxPresent	[in] Array index flag
*
* @return BACNET_DATA_TYPE  [out] Data-type value
*
*/
BACNET_DATA_TYPE GetPropertyDataType(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex,
    bool bArrIndxPresent)
{    
    /** local variables */
    BACNET_REMAPPED_OBJECT_TYPE  eRemapObjType = REMAPPED_MAX_BACNET_OBJECT_TYPE;
    BACNET_DATA_TYPE ePropDataType = BACNET_DT_MAX;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetPropertyDataType : Entry \r\n");
	#endif

    /** remap object type value */
    eRemapObjType = Remap_Objct_Id(eObjectType);

    /* check the array index value */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
        u32ArrayIndex = BACNET_ARRAY_ALL;

    if(eRemapObjType != REMAPPED_MAX_BACNET_OBJECT_TYPE &&
        eObjectProperty < MAX_PROP_SUPPORTED )
    {
        /* get the data type of property */
        ePropDataType = au8PropertyDataType[eObjectProperty][eRemapObjType];

		/* if array index is 0, return unsigned data type */
        if(0 == u32ArrayIndex)
        {
			// TODO 
            //ePropDataType = BACNET_DT_UNSIGNED;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetPropertyDataType : Exit \r\n");
	#endif

	/* return data type value */
    return ePropDataType;
}


/**
*
* Description:
*   Check Range of input Proprty Value & other limits as required by the device.
*
*@param   pvObjectPrt       [In]  Pointer for Object Base address.
*@param   ObjType           [In]  Object Type.
*@param   ObjInstance       [In]  Object instance number.
*@param   ObjProperty       [In]  Object Property Identifier.
*@param   pstAppDataValue   [In]  Property value.
*@param   pvContextValue    [In]  Property value of constructed property.
*@param   u32ArrayIndex     [In]  Array index.
*@param   u32DevID          [in]  Device Id.
*@param   bArrIndxPresent   [in]  Indicates if array index is present.
*@param   bIsReqFromApp		[In]  if true, request is received from application.
*@param	  eDataType			[In]  data type
*
*@return  [out]  BACNET_ERROR_CODE.
*
*/
BACNET_ERROR_CODE CheckPropertyValRange(void *pvObjectPrt, uint32_t ObjType,
                             uint32_t ObjInstance, BACNET_PROPERTY_ID ObjProperty, 
                             BACNET_PROPERTY_VALUE *pstAppDataValue,
                             void  *pvContextValue,
                             uint32_t u32ArrayIndex, uint32_t u32DevID,
                             bool bArrIndxPresent, bool bIsReqFromApp, 
                             void *pVirtualDev,
							 BACNET_DATA_TYPE eDataType)
{
    /* local variables */
    ulong32_t ul32BaseAddr = 0;     /* For Base struct Addr */
    ulong32_t ul32OffsetAddr = 0;   /* For property offset address */
    uint32_t u32Val = 0;
    BACNET_PROPERTY_VALUE *appTempDataValue = NULL;
    uint32_t u32Cnt = 0;    /* used for counting */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	uint32_t u32FirstFailedNo = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: CheckPropertyValRange: Entry \r\n");
	#endif

    /* Get the Object Base Address */
    ul32BaseAddr = (ulong32_t )pvObjectPrt;

    /* NOTE : it is assumed that tag validation is done before calling this function. */

    /* switch to type of property */
    switch(ObjProperty)
    {
        case PROP_PRESENT_VALUE:
        case PROP_RELINQUISH_DEFAULT:
        {
            /* check range as per object type */
            switch(ObjType)
            {
#ifdef OPTIONAL_PROPERTY
            case OBJECT_ANALOG_INPUT:
            case OBJECT_ANALOG_OUTPUT:
#ifdef BACDEL_PR14
			case OBJECT_ANALOG_VALUE:
#endif
                {
                    ulong32_t ul32MinOffsetAddr = 0 ;  /* For Data offset Addr */
                    ulong32_t ul32MaxOffsetAddr = 0 ;  /* For Data offset Addr */
                    Float_t fMinPresentVal = 0.0;
                    Float_t fMaxPresentVal = 0.0;

                    /* Get the Min & Max Present Values */
                    GET_MIN_MAX_PV_OFFSET(ObjType, ul32MinOffsetAddr, 							
                        ul32MaxOffsetAddr);   
                    memcpy(&fMinPresentVal,(void *)(ul32BaseAddr+ul32MinOffsetAddr), 
                        sizeof(Float_t));
                    memcpy(&fMaxPresentVal,(void *)(ul32BaseAddr+ul32MaxOffsetAddr), 
                        sizeof(Float_t));                 

                    /* Check if the value to write is withing range */
                    if(((fMinPresentVal <= pstAppDataValue->uValue.m_Real) && 
                        (fMaxPresentVal >= pstAppDataValue->uValue.m_Real)) || 
                        pstAppDataValue->m_TagType == BACNET_APPLICATION_TAG_NULL)
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:CheckPropertyValRange :"
                            "Value to write is withing range\r\n");
						#endif
                        return MAX_BACNET_ERROR_CODE;
                    }
                    else 
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                            "Value to write is Out Of Range\r\n");
						#endif
                        return ERROR_CODE_VALUE_OUT_OF_RANGE;
                    }
                }break;
#endif /* OPTIONAL_PROPERTY */

            case OBJECT_BINARY_INPUT:
            case OBJECT_BINARY_OUTPUT:
            case OBJECT_BINARY_VALUE:
                {
                    /* Check Binary Present Value is "1" or "0" */
                    if((pstAppDataValue->m_TagType == BACNET_APPLICATION_TAG_ENUMERATED) &&
                       ((pstAppDataValue->uValue.m_Enumerated > MAX_BINARY_PV) || 
                       (pstAppDataValue->uValue.m_Enumerated < MIN_BINARY_PV)))
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                            "Value to write is Out Of Range\r\n");
						#endif
                        return ERROR_CODE_VALUE_OUT_OF_RANGE;
                    }
                }break;

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
            case OBJECT_MULTI_STATE_INPUT:
            case OBJECT_MULTI_STATE_OUTPUT:
            case OBJECT_MULTI_STATE_VALUE:
                {
                    /* Check Number of states for given Object type and instance */
                    uint32_t NumOfStates = 0;

                    GET_MULTISTATE_PROP_OFFSET(ObjType, 
                        m_stNumberOfStates.m_u32Val, ul32OffsetAddr);
                    memcpy((void *)&NumOfStates, (void *)(ul32BaseAddr+ul32OffsetAddr), 
                        sizeof(uint32_t));

                    if((pstAppDataValue->m_TagType == BACNET_APPLICATION_TAG_UNSIGNED_INT) &&
                        ((pstAppDataValue->uValue.m_Unsigned_Int > NumOfStates) ||
                        (0 == pstAppDataValue->uValue.m_Unsigned_Int)))
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                            "Value to write is Out Of Range\r\n");
						#endif
                        return ERROR_CODE_VALUE_OUT_OF_RANGE;
                    }
                }break;
#endif /* Multistate objects */

#ifdef BACDEL_OBJ_SDL
            case OBJECT_SCHEDULE:
                {
                    /* check data type */
                    if(pstAppDataValue->m_TagType > BACNET_APPLICATION_TAG_REAL &&
                        pstAppDataValue->m_TagType != BACNET_APPLICATION_TAG_ENUMERATED)
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                            "Datatype is not supported by object.\r\n");
						#endif
                        return ERROR_CODE_DATATYPE_NOT_SUPPORTED;
                    }
                }break;
#endif /* BACDEL_OBJ_SDL */

            /* default case */
            default:
                break;
            }//switch (switch(ObjType)) ends.
        }
        break;
		
		case PROP_OBJECT_IDENTIFIER:
		{
			/* Check if the value to write is withing range */
			if((DEV_ID_MAX_VALUE < pstAppDataValue->uValue.m_stObject_Id.m_u32ObjInstance) ||
			(DEV_ID_MIN_VALUE > pstAppDataValue->uValue.m_stObject_Id.m_u32ObjInstance))
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
				"Value to write is Out Of Range\r\n");
				#endif
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
					
		}
		break;
		
        case PROP_OBJECT_NAME:
        {
            uint16_t u16ObjType = 0;
            uint32_t u32ObjId = 0;
			if(pstAppDataValue->uValue.m_stCharacter_String.m_u32StrLen > 0)
			{
            if(BACDEL_SUCCESS != Check_Object_Name(u32DevID, 
                pstAppDataValue->uValue.m_stCharacter_String.m_pu8CharStr, &u16ObjType, &u32ObjId))
            {
                if(u16ObjType != (uint16_t)ObjType || u32ObjId != ObjInstance)
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "duplicate object \r\n");
					#endif
                    return ERROR_CODE_DUPLICATE_NAME;
                }
            }
			}
            if(pstAppDataValue->uValue.m_stCharacter_String.m_u32StrLen > MAX_CHARACTER_STRING_BYTES ||
                    pstAppDataValue->uValue.m_stCharacter_String.m_u32StrLen < 1)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "input value is out of range. \r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
            if(Validate_Charstring_Encoding(pstAppDataValue->uValue.m_stCharacter_String.m_u8Encoding,
				pstAppDataValue->uValue.m_stCharacter_String.m_u16CodePage))
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "character set is not supported by device. \r\n");
				#endif
                return ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED;
            }
        }break;

		case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
		{
			if(!Validate_ObjectTypes_Supported(u32DevID, pvContextValue))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}
		break;

		case PROP_PROTOCOL_SERVICES_SUPPORTED:
		{
			if(!Validate_Services_Supported(u32DevID, pvContextValue))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}
		break;

        case PROP_MAX_APDU_LENGTH_ACCEPTED:
        {
            uint32_t u32MaxAPDULen = 0;

            u32MaxAPDULen = pstAppDataValue->uValue.m_Unsigned_Int;

			if(!Validate_Max_APDU_Length(u32MaxAPDULen,pVirtualDev))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
        }break;

		case PROP_NUMBER_OF_APDU_RETRIES:
		{
			if(pstAppDataValue->uValue.m_Unsigned_Int > 255)
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
				" no of apdu retries should be less than 255. \r\n");
				#endif
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}break;

		case PROP_APDU_TIMEOUT:
		{
            /* get device object pointer */
            devObject_t *pstDevObj = NULL;
            pstDevObj = pvObjectPrt;

            /* read the no of apdu retries property */
            if(NULL != pvObjectPrt)
                u32Val = pstDevObj->m_stNumOfAPDURetries.m_u32Val;
            else
                return ERROR_CODE_INTERNAL_ERROR;

            /* check value */
			if((pstAppDataValue->uValue.m_Unsigned_Int % 1000)
				||
               (pstAppDataValue->uValue.m_Unsigned_Int < APDU_TIMEOUT ||
                pstAppDataValue->uValue.m_Unsigned_Int > UINT16_MAX))
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
				" apdu timeout value out of range. \r\n");
				#endif
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}break;

		case PROP_SEGMENTATION_SUPPORTED:
		{
			if((pstAppDataValue->m_TagType == BACNET_APPLICATION_TAG_ENUMERATED) &&
               (pstAppDataValue->uValue.m_Enumerated >= MAX_BACNET_SEGMENTATION))
            {
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }

		}break;

#ifdef SEGMENTATION_SUPPORTED
		case PROP_APDU_SEGMENT_TIMEOUT:
		{
            /* get device object pointer */
            devObject_t *pstDevObj = NULL;
            pstDevObj = pvObjectPrt;

            /* read the no of apdu retries property */
            if(NULL != pvObjectPrt)
                u32Val = pstDevObj->m_stNumOfAPDURetries.m_u32Val;
            else
                return ERROR_CODE_INTERNAL_ERROR;

            /* check value */
			if((pstAppDataValue->uValue.m_Unsigned_Int % 1000)
				||
               (pstAppDataValue->uValue.m_Unsigned_Int < APDU_SEGMENT_TIMEOUT ||
                pstAppDataValue->uValue.m_Unsigned_Int > UINT16_MAX))
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
				" apdu segment timeout value out of range. \r\n");
				#endif
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}break;

        case PROP_MAX_SEGMENTS_ACCEPTED:
        {
            if(pstAppDataValue->uValue.m_Unsigned_Int == BACNET_ZERO ||
                pstAppDataValue->uValue.m_Unsigned_Int > MAX_SEGMENTS_RX)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                "max segments accepted value out of range \r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }break;
#endif /* SEGMENTATION_SUPPORTED */


#ifdef BACDEL_SER_DM_UTC_B
        case PROP_UTC_OFFSET:
		{
			if(pstAppDataValue->uValue.m_Signed_Int < -780 ||
				pstAppDataValue->uValue.m_Signed_Int > 780)
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "utc is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}break;
#endif /* BACDEL_SER_DM_UTC_B */
        case PROP_MAX_MASTER:    //SK
		{
			switch(ObjType)
			{
			case OBJECT_DEVICE:
			case OBJECT_NETWORK_PORT:
				if(pstAppDataValue->uValue.m_Unsigned_Int > BN_DEVICE_MAX_MASTER /*||
					pstAppDataValue->uValue.m_Unsigned_Int < MSTP_GetMacId(NULL)*/)
				{
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}
			}
			break;
		}
		case PROP_MAX_INFO_FRAMES:
		{
			switch(ObjType)
			{
			case OBJECT_DEVICE:
			case OBJECT_NETWORK_PORT:
					if(pstAppDataValue->uValue.m_Unsigned_Int > MAX_INFO_FRAMES_HIGH_LIMIT ||
						pstAppDataValue->uValue.m_Unsigned_Int <= MAX_INFO_FRAMES_LOW_LIMIT)
					{
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
					}
				}
				break;
		}

        case PROP_POLARITY:// all binary objects
        #ifdef BACDEL_SER_AE_EN_B
        case PROP_ALARM_VALUE://BI & BV-enum
        #endif
        {
            /* Check Binary Object polarity is "1" or "0" */
            if((pstAppDataValue->m_TagType == BACNET_APPLICATION_TAG_ENUMERATED) &&
               ((pstAppDataValue->uValue.m_Enumerated > MAX_BINARY_PV) || 
               (pstAppDataValue->uValue.m_Enumerated < MIN_BINARY_PV)))
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "Value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }
        break;

        case PROP_NOTIFICATION_CLASS:
        {
            if(pstAppDataValue->uValue.m_Unsigned_Int >= BACNET_MAX_INSTANCE)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "Value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }break;

        case PROP_NOTIFY_TYPE:
        {
            if(pstAppDataValue->uValue.m_Enumerated >= MAX_NOTIFY_TYPE)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "Value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }break;

		case PROP_EVENT_ENABLE:
		case PROP_ACK_REQUIRED:
		{
			if(pstAppDataValue->uValue.m_stBit_String.m_u8UnusedBits != 5 || 
				pstAppDataValue->uValue.m_stBit_String.m_u8ByteCnt != 1)
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "bitstring value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}break;

		case PROP_STATUS_FLAGS:
		{
			/* check total no of bits */
			if((pstAppDataValue->uValue.m_stBit_String.m_u8UnusedBits != 4 ||
				pstAppDataValue->uValue.m_stBit_String.m_u8ByteCnt != 1))
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "Statuc flag value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
		}
		break;

		case PROP_EVENT_STATE:
		{
            /* check Event State Value */
            if((pstAppDataValue->m_TagType == BACNET_APPLICATION_TAG_ENUMERATED) &&
               (pstAppDataValue->uValue.m_Enumerated >= MAX_EVENT_STATE))
            {
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
		}
		break;

#ifdef BACDEL_SER_AE_EN_B
		case PROP_LIMIT_ENABLE:
		{
			if(pstAppDataValue->uValue.m_stBit_String.m_u8UnusedBits != 6 ||
				pstAppDataValue->uValue.m_stBit_String.m_u8ByteCnt != 1)
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "bitstring value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}break;

        case PROP_FEEDBACK_VALUE:// bo-enum, mso-uint
        {
            appTempDataValue = pstAppDataValue;

            if(ObjType == OBJECT_MULTI_STATE_OUTPUT)
            {
				#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
                /* check if received value is within range */
                GET_MULTISTATE_PROP_OFFSET(ObjType, m_stNumberOfStates.m_u32Val, ul32OffsetAddr);
                /* Copy data into property offset address */
                memcpy(&u32Val, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint32_t));             

                while(NULL != appTempDataValue)
                {
                    if(u32Val < appTempDataValue->uValue.m_Unsigned_Int ||
                        0 >= appTempDataValue->uValue.m_Unsigned_Int)
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                        "Value to write is Out Of Range\r\n");
						#endif
                        return ERROR_CODE_VALUE_OUT_OF_RANGE;
                    } 
                    appTempDataValue = appTempDataValue->pstNextPropVal;
                }
				#else
				/* do nothing */;
				#endif
            }
            else if(ObjType == OBJECT_BINARY_OUTPUT)
            {
                if(appTempDataValue->uValue.m_Enumerated != MIN_BINARY_PV &&
                   appTempDataValue->uValue.m_Enumerated != MAX_BINARY_PV)
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                        "Value to write is Out Of Range\r\n");
					#endif
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
                }
            }
        }
        break;

        case PROP_ALARM_VALUES://msi, msv both uint //bsv - bitstring array
        case PROP_FAULT_VALUES://msi, msv both uint
        {
            appTempDataValue = pstAppDataValue;

            switch(ObjType)
            {
            case OBJECT_MULTI_STATE_INPUT:
            case OBJECT_MULTI_STATE_VALUE:
                {
					ListOfUnsigned_t *pstUintValueList = NULL;
					pstUintValueList = pvContextValue;

					if(NULL == pstUintValueList)
					{
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
					}

                    while(NULL != pstUintValueList)
                    {
						if(pstUintValueList->m_u32Value == 0 || 
							pstUintValueList->m_u32Value > g_stStackMaxLimits.m_u32MaxStateText)
                        {
							#ifdef DEBUG_PRINTF
                            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                            "Value to write is Out Of Range\r\n");
							#endif
                            return ERROR_CODE_VALUE_OUT_OF_RANGE;
                        } 
						pstUintValueList = pstUintValueList->m_pstNext;
                        /* increment count */
                        u32Cnt++;
                    }
                }break;

            /* default case */
            default:
                break;
            }

            /* if no of values exceed max allowed list / array limit, return error */
            if(u32Cnt > g_stStackMaxLimits.m_u32MaxAFValuesList)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                "max values limit exceeded.\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            } 
        }break;
#endif /* BACDEL_SER_AE_EN_B */

#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B || defined BACDEL_OBJ_SDL)
		case PROP_RELIABILITY:
		{
			switch(ObjType)
			{
			case OBJECT_ANALOG_INPUT:

				if(pstAppDataValue->uValue.m_Enumerated == RELIABILITY_NO_OUTPUT ||
				   pstAppDataValue->uValue.m_Enumerated == RELIABILITY_PROCESS_ERROR ||
				   pstAppDataValue->uValue.m_Enumerated == RELIABILITY_MULTI_STATE_FAULT ||
				   pstAppDataValue->uValue.m_Enumerated == RELIABILITY_CONFIGURATION_ERROR ||
				   pstAppDataValue->uValue.m_Enumerated >= RELIABILITY_MEMBER_FAULT)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity value is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_ANALOG_OUTPUT:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_OPEN_LOOP &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_SHORTED_LOOP &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_OUTPUT &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE)
				{
						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
							"reliablity is Out Of Range\r\n");
						#endif
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_ANALOG_VALUE:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_OVER_RANGE &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNDER_RANGE &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_BINARY_INPUT:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_SENSOR &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_OPEN_LOOP &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_SHORTED_LOOP &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_BINARY_OUTPUT:
			case OBJECT_MULTI_STATE_OUTPUT:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_OUTPUT &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_OPEN_LOOP &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_SHORTED_LOOP &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_BINARY_VALUE:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_MULTI_STATE_INPUT:
				if(pstAppDataValue->uValue.m_Enumerated == RELIABILITY_NO_OUTPUT ||
				   pstAppDataValue->uValue.m_Enumerated == RELIABILITY_PROCESS_ERROR ||
				   pstAppDataValue->uValue.m_Enumerated == RELIABILITY_CONFIGURATION_ERROR ||
				   pstAppDataValue->uValue.m_Enumerated >= RELIABILITY_MEMBER_FAULT)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
			case OBJECT_MULTI_STATE_VALUE:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_MULTI_STATE_FAULT)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
#if (defined BACDEL_OBJ_SDL || defined BACDEL_OBJ_EL)
            case OBJECT_SCHEDULE:
				if(pstAppDataValue->uValue.m_Enumerated != RELIABILITY_NO_FAULT_DETECTED &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_UNRELIABLE_OTHER &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_COMMUNICATION_FAILURE &&
				   pstAppDataValue->uValue.m_Enumerated != RELIABILITY_CONFIGURATION_ERROR)
				{
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
						"reliablity is Out Of Range\r\n");
					#endif
					return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}break;
#endif

#ifdef BACDEL_OBJ_NP
 			case OBJECT_NETWORK_PORT:
 				if(pstAppDataValue->uValue.m_Enumerated >= MAX_RELIABILITY)
 				{
 					#if(defined DEBUG_PRINTF && DL_1)
 					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: CheckPropertyValRange: "
 						"reliablity value is Out Of Range\r\n");
 					#endif
 					return ERROR_CODE_VALUE_OUT_OF_RANGE;
 				}break;
#endif

			default:
				break;
			}//switch(ObjType) ends.
		}break;
#endif /* RELIABLITY */
#ifdef BACDEL_OBJ_NP
		case PROP_NETWORK_NUMBER:
		{
			/* allowed range is 0 to 65534 */
			if(pstAppDataValue->uValue.m_Unsigned_Int > (UINT16_MAX - 1))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}
		break;

		case PROP_APDU_LENGTH:
		{
			/* check apdu length as per allowed values */
			if(!Validate_APDU_Length(pstAppDataValue->uValue.m_Unsigned_Int))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}
		break;

		case PROP_FD_BBMD_ADDRESS:
		{
			/* validate host n port value */
			eErrorCode = Validate_HostNPort((Pr_BACnetHostNPort_t *)pvContextValue);
			return eErrorCode;
		}
		break;

		case PROP_FD_SUBSCRIPTION_LIFETIME:
		{
			/* allowed range is 0 to 65535 */
			if(pstAppDataValue->uValue.m_Unsigned_Int > UINT16_MAX)
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}
		break;
#endif /* NP */
#ifdef BACDEL_PR18
        case PROP_CURRENT_COMMAND_PRIORITY:
        {
            if((pstAppDataValue->uValue.m_Unsigned_Int < BACNET_MIN_PRIORITY) ||
				(pstAppDataValue->uValue.m_Unsigned_Int > BACNET_MAX_PRIORITY))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
        }break;
#endif	/* PR18 */

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
        case PROP_NUMBER_OF_STATES:
        {
            if(pstAppDataValue->uValue.m_Unsigned_Int > g_stStackMaxLimits.m_u32MaxStateText ||
                pstAppDataValue->uValue.m_Unsigned_Int <= 0)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                        "bitstring value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }
        break;
#endif /* MSI - MSO - MSV */

#ifdef OPTIONAL_PROPERTY
#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
        case PROP_STATE_TEXT:
        {
            if(BACNET_APPLICATION_TAG_UNSIGNED_INT == pstAppDataValue->m_TagType)
            {
                /* invalid size for array */
                if(pstAppDataValue->uValue.m_Unsigned_Int > g_stStackMaxLimits.m_u32MaxStateText ||
                    pstAppDataValue->uValue.m_Unsigned_Int == 0)
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "max state-text limit exceeded.. \r\n");
					#endif
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
                }
            }
            else
            {
                appTempDataValue = pstAppDataValue;

                while(NULL != appTempDataValue)
                {
                    /* check if character string length is not exceeded */
                    if(appTempDataValue->uValue.m_stCharacter_String.m_u32StrLen >
                        MAX_CHARACTER_STRING_BYTES)
                        return ERROR_CODE_VALUE_OUT_OF_RANGE;
                    /* check the character encoding */
                    if(Validate_Charstring_Encoding(appTempDataValue->uValue.m_stCharacter_String.m_u8Encoding,
						appTempDataValue->uValue.m_stCharacter_String.m_u16CodePage))
                    {
                        return ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED;
                    }
                    /* move to next value */
                    appTempDataValue = appTempDataValue->pstNextPropVal;
                    /* increment count by 1 */
                    u32Cnt++;
                }

                /* if no of values exceed max allowed states, return error */
                if(u32Cnt > g_stStackMaxLimits.m_u32MaxStateText)
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }
        break;
#endif /* MSI - MSO - MSV */

#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
        case PROP_CHANGE_OF_STATE_COUNT:
        case PROP_ELAPSED_ACTIVE_TIME:
        {
            if(pstAppDataValue->uValue.m_Unsigned_Int != BACNET_ZERO)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                "only zero values is valid. \r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
			
			/* check property is elapsed active time */
			if(PROP_ELAPSED_ACTIVE_TIME == ObjProperty && 
			BACNET_ZERO == pstAppDataValue->uValue.m_Unsigned_Int)
			{
			    /* Verify is write data is same as currntly avail data
				   Logic: Read property value in void pointer and based on property
				   Data type typecast and compare with input data. Retrun True on 
				   same data , false for not same */
				   g_bWriteRequest = true;				
			}
        }break;
#endif /* BI - BO - BV */
#endif /* OPTIONAL_PROPERTY */

#ifdef BACDEL_OBJ_NC
		case PROP_RECIPIENT_LIST:
		{
			/* check NULL pointer */
			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}

			/* validate destination list value */
			eErrorCode = Validate_Destination_List(
				(ListOfBACnetDestination_t *)pvContextValue, &u32FirstFailedNo);
			return eErrorCode;
		}
		break;

		case PROP_PRIORITY:
		{
			/* check NULL pointer */
			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}

			/* validate priority property value */
			eErrorCode = Validate_NotifyPriority_Array(
				(Pr_BACnetNotifyPriority_t *)pvContextValue, 
				bArrIndxPresent, u32ArrayIndex);
			return eErrorCode;
		}
		break;
#endif /* BACDEL_OBJ_NC */

#if (defined BACDEL_OBJ_SDL )
        case PROP_PRIORITY_FOR_WRITING:
        {
            if( pstAppDataValue->uValue.m_Unsigned_Int <= 0 ||
                pstAppDataValue->uValue.m_Unsigned_Int > BACNET_MAX_PRIORITY)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                        "Prop priority-for-writing to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }
        break;
#endif

#ifdef BACDEL_OBJ_SDL
        case PROP_EXCEPTION_SCHEDULE:
        {
            ListOfSpecialEvent_t *pstSplEvent = NULL;

			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}

            if(bArrIndxPresent && 0 == u32ArrayIndex)
            {
                
				if((*((uint32_t *)(pvContextValue))) > g_stStackMaxLimits.m_u32MaxExSchdList)
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;                
            }
			else
			{
				pstSplEvent = pvContextValue;
				eErrorCode = Validate_SpecialEvent_List(pstSplEvent, &u32FirstFailedNo,
					bArrIndxPresent, u32ArrayIndex);
				return eErrorCode;
			}
        }
        break;

        case PROP_SCHEDULE_DEFAULT:
        {
            if(pstAppDataValue->m_TagType > BACNET_APPLICATION_TAG_REAL &&
                pstAppDataValue->m_TagType != BACNET_APPLICATION_TAG_ENUMERATED)
            {
                return ERROR_CODE_DATATYPE_NOT_SUPPORTED;
            }
        }
        break;

		case PROP_EFFECTIVE_PERIOD:
		{
			/* Local Variables */
			BACnetDateRange_t *pstDateRange = NULL;

			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}
			/* get the value */
			pstDateRange = pvContextValue;

			/* validate the date-range values */
			if(!Validate_Date_Combinations(&pstDateRange->m_stStartDate) || 
				!Validate_Date_Combinations(&pstDateRange->m_stEndDate))
			{
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
			else if((!Is_Any_Date(&pstDateRange->m_stStartDate) && 
				!Validate_Date(&pstDateRange->m_stStartDate)) 
				||
				(!Is_Any_Date(&pstDateRange->m_stEndDate) && 
				!Validate_Date(&pstDateRange->m_stEndDate)))
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
			else if(Validate_Date(&pstDateRange->m_stStartDate) && 
				Validate_Date(&pstDateRange->m_stEndDate))
			{
				/* compares two dates & return error if date1 is after date2 */
				if(DateTime_Compare_Date(&pstDateRange->m_stStartDate, 
					&pstDateRange->m_stEndDate) > BACNET_ZERO)
				{
            		return ERROR_CODE_VALUE_OUT_OF_RANGE;
				}
			}			
		}
		break;

		case PROP_WEEKLY_SCHEDULE:
		{
			Pr_ListOfBACnetDailySchedule_t *pstWeeklySchdl = NULL;

			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}

			pstWeeklySchdl = pvContextValue;
			eErrorCode = Validate_DailySchedule_Array(pstWeeklySchdl, 
				bArrIndxPresent, u32ArrayIndex);
			return eErrorCode;
		}
		break;

		case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
		{
			ListOfBACnetDevObjPropRef_t *pstListObjPropRef = NULL;

			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}

			pstListObjPropRef = pvContextValue;
			eErrorCode = Validate_DevObjPropRef_List(pstListObjPropRef, &u32FirstFailedNo);
			return eErrorCode;
		}
		break;
#endif /* BACDEL_OBJ_SDL */

#ifdef BACDEL_OBJ_CAL
        case PROP_DATE_LIST :
        {
        	/* Local Variables */
        	ListOfBACnetCalendarEntry_t *pstCalList = NULL;

			if(NULL == pvContextValue)
			{
				return ERROR_CODE_INVALID_DATA_TYPE;
			}
            /* get the value */
            pstCalList = ((ListOfBACnetCalendarEntry_t *)pvContextValue);

			eErrorCode = Validate_CalenderEntry_List(pstCalList, &u32FirstFailedNo);
			return eErrorCode;
        }
        break;
#endif /* BACDEL_OBJ_CAL */

        case PROP_UNITS:
        case PROP_OUTPUT_UNITS:
        case PROP_CONTROLLED_VARIABLE_UNITS:
        case PROP_PROPORTIONAL_CONSTANT_UNITS:
        case PROP_INTEGRAL_CONSTANT_UNITS:
        case PROP_DERIVATIVE_CONSTANT_UNITS:
        {
            if(pstAppDataValue->uValue.m_Enumerated >= MAX_UNITS)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                "not a valid units value. \r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }break;

#if (defined OPTIONAL_PROPERTY && (defined BACDEL_OBJ_AI ))
        case PROP_UPDATE_INTERVAL:
        {
            u32Val = pstAppDataValue->uValue.m_Unsigned_Int/1000;

            if(u32Val < 1 && pstAppDataValue->uValue.m_Unsigned_Int != 0)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "Value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }break;
#endif

        case PROP_MAX_PRES_VALUE :
        {
#ifdef OPTIONAL_PROPERTY		
            Float_t fMinPresVal = 0;
            if(OBJECT_ACCUMULATOR != ObjType)
            {
                GET_MIN_PV_RESOLUTION_OFFSET(ObjType, m_stMinPresValue.m_fVal ,ul32OffsetAddr);						
                memcpy(&fMinPresVal,(void *)(ul32BaseAddr+ul32OffsetAddr),sizeof(Float_t));

                if(fMinPresVal > pstAppDataValue->uValue.m_Real)
				{
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                        "Value to write is Out Of Range\r\n");
					#endif
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
                }
            }
#endif			
        }
        break;

#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV)
        case PROP_MIN_PRES_VALUE :
        {
            Float_t fMaxPresVal = 0;
            
            GET_MAX_PV_VALUE_OFFSET(ObjType, m_stMaxPresValue,ul32OffsetAddr);
            memcpy(&fMaxPresVal,(void *)(ul32BaseAddr+ul32OffsetAddr),sizeof(Float_t));

            if(fMaxPresVal < pstAppDataValue->uValue.m_Real)
			{
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:CheckPropertyValRange :"
                    "Value to write is Out Of Range\r\n");
				#endif
                return ERROR_CODE_VALUE_OUT_OF_RANGE;
            }
        }
        break;
#endif

#ifdef BACDEL_SER_AE_EN_B
		case PROP_HIGH_LIMIT:
        {
			/* locals */
			PropertyValue_u uLowLimit = {0};
			int32_t i32Len = 0;

			/* get low limit value */
			GET_HL_LL_VALUE_OFFSET(ObjType, m_stLowLimit, ul32OffsetAddr, i32Len);
			memcpy(&uLowLimit, (void *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

			switch(ObjType)
			{
				case OBJECT_ACCUMULATOR:
				{
					if(uLowLimit.u32Val > pstAppDataValue->uValue.m_Unsigned_Int)
					{
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
					}
				}
				break;

				case OBJECT_ANALOG_INPUT:
				case OBJECT_ANALOG_OUTPUT:
				case OBJECT_ANALOG_VALUE:
				{
					if(uLowLimit.fVal > pstAppDataValue->uValue.m_Real)
					{
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
					}
				}
				break;

				/* default case - do nothing */
				default:
					break;
			}//switch ends.
		}
		break;

		case PROP_LOW_LIMIT:
        {
			/* locals */
			PropertyValue_u uHighLimit = {0};
			int32_t i32Len = 0;

			/* get high limit value */
			GET_HL_LL_VALUE_OFFSET(ObjType, m_stHighLimit, ul32OffsetAddr, i32Len);
			memcpy(&uHighLimit, (void *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

			switch(ObjType)
			{
				case OBJECT_ACCUMULATOR:
				{
					if(pstAppDataValue->uValue.m_Unsigned_Int > uHighLimit.u32Val)
					{
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
					}
				}
				break;

				case OBJECT_ANALOG_INPUT:
				case OBJECT_ANALOG_OUTPUT:
				case OBJECT_ANALOG_VALUE:
				{
					if(pstAppDataValue->uValue.m_Real > uHighLimit.fVal)
					{
						return ERROR_CODE_VALUE_OUT_OF_RANGE;
					}
				}
				break;

				 /* default case - do nothing */
				default:
					break;
			}//switch ends.
        }
        break;
#endif /* BACDEL_SER_AE_EN_B */
		case PROP_FAULT_LOW_LIMIT:   //PR-19
		        {
					/* locals */
					PropertyValue_u uFaultHighLimit = {0};
					int32_t i32Len = 0;

					/* get high limit value */
					GET_FHL_FLL_VALUE_OFFSET(ObjType, m_stFaultHighLimit, ul32OffsetAddr, i32Len);
					memcpy(&uFaultHighLimit, (void *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

					switch(ObjType)
					{
						case OBJECT_ANALOG_INPUT:
						case OBJECT_ANALOG_VALUE:
						{
							if(pstAppDataValue->uValue.m_Real > uFaultHighLimit.fVal)
							{
								return ERROR_CODE_VALUE_OUT_OF_RANGE;
							}
						}
					}
		        }
					break;
		case PROP_FAULT_HIGH_LIMIT:
				        {
							PropertyValue_u uFaultLowLimit = {0};
							int32_t i32Len = 0;
							/* get high limit value */
							GET_FHL_FLL_VALUE_OFFSET(ObjType, m_stFaultLowLimit, ul32OffsetAddr, i32Len);
							memcpy(&uFaultLowLimit, (void *)(ul32BaseAddr+ul32OffsetAddr), i32Len);
							switch(ObjType)
							{
								case OBJECT_ANALOG_INPUT:
								case OBJECT_ANALOG_VALUE:
								{
									if(uFaultLowLimit.fVal > pstAppDataValue->uValue.m_Real)
									{
										return ERROR_CODE_VALUE_OUT_OF_RANGE;
									}
								}
							}
				        }
							break;
		case PROP_SYSTEM_STATUS:
		{
			/* validate system status */		
			if(pstAppDataValue->uValue.m_Enumerated >= MAX_DEVICE_STATUS)
			{
				return ERROR_CODE_VALUE_OUT_OF_RANGE;
			}
		}
		break;

        default:
        {
            /* Check Range as per DataType supported */
            switch(pstAppDataValue->m_TagType)
            {
                /* check length & character encoding */
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
                if(pstAppDataValue->uValue.m_stCharacter_String.m_u32StrLen > MAX_CHARACTER_STRING_BYTES)
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
                else if(Validate_Charstring_Encoding(pstAppDataValue->uValue.m_stCharacter_String.m_u8Encoding,
					pstAppDataValue->uValue.m_stCharacter_String.m_u16CodePage))
                    return ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED;
                break;
                /* check length */
            case BACNET_APPLICATION_TAG_BIT_STRING:
				if(pstAppDataValue->uValue.m_stBit_String.m_u8ByteCnt > MAX_BITSTRING_BYTES 
                    || 
					pstAppDataValue->uValue.m_stBit_String.m_u8UnusedBits == 8)
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
                break;
                /* check length */
            case BACNET_APPLICATION_TAG_OCTET_STRING:
				if(pstAppDataValue->uValue.m_stOctet_String.m_u32OctetCount > MAX_OCTET_STRING_BYTES)
                    return ERROR_CODE_VALUE_OUT_OF_RANGE;
                break;
                /* default case - do nothing */
            default:
                break;
            }//switch ends.
        }
        break;
    }//switch(ObjProperty) ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: CheckPropertyValRange: Exit \r\n");
	#endif
    return MAX_BACNET_ERROR_CODE;
}

/** Api to check if property is writable or not, even though its access type 
    states that it is read only. */
/** returns true if writable */
bool Is_Property_Writable(BACNET_PROPERTY_ID ePropId, BACNET_OBJECT_TYPE eObjTYpe, 
                     void *pvObjectPrt, void *pVirtualDev)
{
    /* local variables */
    bool bVal = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Is_Property_Writable : Entry \r\n");
	#endif

    /* null check input pointers */
    if(NULL == pvObjectPrt || NULL == pVirtualDev)
        return bVal;

    /* Added this for AI,BI,MSI objects wen OutOfService is TRUE */
	if((ePropId == PROP_PRESENT_VALUE ) &&
       (IsOutofSerivceFlagTrue((ulong32_t)pvObjectPrt, eObjTYpe)))
       bVal = true;
    else
        bVal = false;

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Is_Property_Writable : Exit \r\n");
	#endif
    return bVal;
}

/** function to remove values from list type properties */
void RemovePreviousElement(BACNET_CONF_DATA *pWpData, void *pvObject)
{
    /* local variables */
    ulong32_t ul32OffsetAddr = 0 ;
    ulong32_t ul32BaseAddr = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    RemovePreviousElement : Entry \r\n");
	#endif

    /* check input pointers */
    if(NULL == pvObject || NULL == pWpData)
        return;

    /* check the array index */
    /* NOTE: list type properties can only be cleared when array index is -1 */
    /* i.e. no array index present in request */
    if(ARRAY_INDEX_ABSENT != pWpData->bArrIndxPresent)
        return;

    /* get object base address */
    ul32BaseAddr = (ulong32_t )pvObject;

    /* switch to type of property */
    switch(pWpData->eObjectProperty)
    {
#ifdef BACDEL_OBJ_NC
        case PROP_RECIPIENT_LIST:
        {
            Pr_ListOfBACnetDestination_t *pstRecipientList = NULL;
            GET_NC_OBJ_PROP_OFFSET(pWpData->eObjectType, m_stRecipientList, ul32OffsetAddr);
            /* copy the data to property offset address */
            pstRecipientList = (Pr_ListOfBACnetDestination_t *)(ul32BaseAddr+ul32OffsetAddr);
            /* clear the list */
            Clear_Destination_List(&pstRecipientList->m_pstNCRecepient);
        }
        break;
#endif /* BACDEL_OBJ_NC */

#ifdef BACDEL_SER_AE_EN_B
        case PROP_ALARM_VALUES: // for MSI, MSV, BSV
        case PROP_FAULT_VALUES: // for MSI, MSV
        {
            switch(pWpData->eObjectType)
            {
            case OBJECT_MULTI_STATE_INPUT:
            case OBJECT_MULTI_STATE_VALUE:
                {
                    Pr_ListOfUnsigned_t *pstAlarmFaultValues = NULL;               

                    /* get the data offset address */
                    if(pWpData->eObjectProperty == PROP_ALARM_VALUES)
                        GET_ALARM_VALUES_OFFSET(pWpData->eObjectType,
                        m_stAlarmValues, ul32OffsetAddr);
                    if(pWpData->eObjectProperty == PROP_FAULT_VALUES)
                        GET_FAULT_VALUES_OFFSET(pWpData->eObjectType,
                        m_stFaultValues, ul32OffsetAddr);

                    /* save base address & then clear */
                    pstAlarmFaultValues = (void*)(ul32OffsetAddr+ul32BaseAddr);
                 
                    /* clear the list */
                    Clear_UnsignedInt_List(&pstAlarmFaultValues->m_pstUnsignVal);
                }
                break;
            default:
                break;
            }//switch ends.
        }
        break;
#endif /* BACDEL_SER_AE_EN_B */

#ifdef BACDEL_OBJ_SDL
        case PROP_EXCEPTION_SCHEDULE:
        {
            Pr_ListOfBACnetSpecialEvent_t *pstExceptionSchedule = NULL;
            GET_SHDL_OFFSET(pWpData->eObjectType,m_stExceptionSchedule,ul32OffsetAddr);
            
            pstExceptionSchedule = (void *)(ul32BaseAddr+ul32OffsetAddr);
			/* Note: when WP is received with no array index previous list is cleared 
			   in UpdateCommonProperty() function */

        }
        break;

        case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:        
        {   
            Pr_ListOfBACnetDevObjPropRef_t 	*pstListOfObjectPropertyReference = NULL;

            /* copy the data to property offset address */
            GET_SHDL_OFFSET(pWpData->eObjectType,m_stListOfObjectPropertyReference,ul32OffsetAddr);	

            /* save base address & then clear */
			pstListOfObjectPropertyReference = (Pr_ListOfBACnetDevObjPropRef_t *)
                (ul32BaseAddr+ul32OffsetAddr);
           
            /* clear the list */
            Clear_DevObjPropReff_List(&pstListOfObjectPropertyReference->m_pstListOfBACnetDevObjPropReff);
        }
        break;
#endif /* BACDEL_OBJ_SDL */

#ifdef BACDEL_OBJ_CAL
        case PROP_DATE_LIST:
        {
            Pr_ListOfBACnetCalendarEntry_t   *pstDateList = NULL;

            /* copy the data to property offset address */
            GET_OBJCALPROP_OFFSET(OBJECT_CALENDAR,m_stDateList,ul32OffsetAddr);

            /* save base address & then clear */
            pstDateList = (Pr_ListOfBACnetCalendarEntry_t*)(ul32BaseAddr+ul32OffsetAddr);

            /* clear the list */
            Clear_CalendarEntry_List(&pstDateList->m_pstListOfCalendar);
        }
        break;
#endif /* BACDEL_OBJ_CAL */

        /* default case - do nothing */
        default:
           break;
    }//switch ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    RemovePreviousElement : Exit \r\n");
	#endif
    return;
}


/** function to validate if property supports writting null value */
BACNET_ERROR_CODE ValidateNullTag(
  BACNET_PROPERTY_ID ePropId,
  BACNET_OBJECT_TYPE eObjectType,
  PROP_ACCESS_TYPE eAccessType)
{
	/* local variables */
	BACNET_ERROR_CODE eErrCode = MAX_BACNET_ERROR_CODE;

	/* check as per property & object type */
    switch(ePropId)
    {
	/* properties supporting null datatype */
    case PROP_SCHEDULE_DEFAULT:
    case PROP_CLIENT_COV_INCREMENT:
	break;

	/* PV */
    case PROP_PRESENT_VALUE:
	{
		switch(eObjectType)
		{
		/* output type object */
		case OBJECT_ANALOG_OUTPUT:
		case OBJECT_BINARY_OUTPUT:
		case OBJECT_MULTI_STATE_OUTPUT:
			/* PV of output type objects is commandable */
		break;

		/* value type object */
		case OBJECT_ANALOG_VALUE:
		case OBJECT_BINARY_VALUE:
		case OBJECT_MULTI_STATE_VALUE:
		{
			/* if not commandable, return error */
			if(eAccessType != COMMANDABLE_PROP)
			{
				eErrCode = ERROR_CODE_INVALID_DATA_TYPE; 
			}
		}
		break;

		/* for all other object types, return error */
		default: 
			eErrCode = ERROR_CODE_INVALID_DATA_TYPE; 
		break;
		}
	}
    break;

	/* default case */
    default: 
		/* return error */
		eErrCode = ERROR_CODE_INVALID_DATA_TYPE;
		break;
    }

	/* return value */
    return eErrCode;
}

/**
*   Function to check if property of array type is empty.
*
*   @param ePropId [in] property id.
*   @param eObjectType [in] object type.
*   @returns false if empty datatype is allowed.
*
*/
bool ValidateEmptyTag(
	BACNET_PROPERTY_ID eObjectProperty,
	BACNET_OBJECT_TYPE eObjectType)
{
	/* local variables */
	bool bRetType = TRUE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    ValidateEmptyTag : Entry \r\n"); 
	#endif

	switch(eObjectProperty)
	{
	case PROP_ALARM_VALUES:
	{

	}
	break;

	case PROP_FAULT_VALUES:
	{

	}
	break;

	case PROP_SETPOINT_REFERENCE:
	{
		bRetType = false;
	}
	break;

	default:
		break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    ValidateEmptyTag : Exit \r\n"); 
	#endif
	return bRetType;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function to copy list of recipient info in COV subscription.
*    
* @param pstDestCovRcpt		 [in] address to destination pointer.
* @param pstSrcCovRcpt		 [in] source pointer.
*                                   
* @return BACNET_RETURN_TYPE [out] either success or error.
*	
*/
BACNET_RETURN_TYPE Copy_COVRecipeint_Info(
	CovRecipientInfo_t **pstDestCovRcpt,
	CovRecipientInfo_t *pstSrcCovRcpt)
{
	/* local variables */
	bool bFlag = TRUE;
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;
	CovRecipientInfo_t *pstCovRcpt = NULL;
	CovRecipientInfo_t *pstBasePtr = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Copy_COVRecipeint_Info : Entry \r\n");
	#endif

	/* if source pointer is NULL, return without error */
	if(NULL == pstSrcCovRcpt)
	{
		return BACDEL_SUCCESS;
	}

	/* if address of destination pointer is NULL */
	if(NULL ==  pstDestCovRcpt)
	{
		eRetVal = BACDEL_INVALID_INPUT_PARAMETER;
	}	
	
	else
	{
		while(NULL != pstSrcCovRcpt && (NULL != pstCovRcpt || bFlag))
		{
			if(bFlag)
			{
				/* allocate memory to base node */
				pstBasePtr = OSAL_Malloc(sizeof(CovRecipientInfo_t),
					__FILE__,__FUNCTION__,__LINE__);
				/* reset flag */
				bFlag = FALSE;
				/* update the pointer */
				pstCovRcpt = pstBasePtr;
			}
			else
			{
				/* allocate memory from next node */
				pstCovRcpt->m_pstNext = OSAL_Malloc(sizeof(CovRecipientInfo_t),
					__FILE__,__FUNCTION__,__LINE__);
				/* move to new node */
				pstCovRcpt = pstCovRcpt->m_pstNext;
			}

			if(NULL != pstCovRcpt)
			{
				/* copy data */				
				memcpy(pstCovRcpt, pstSrcCovRcpt, sizeof(CovRecipientInfo_t));			
				/* make next pointer NULL */
				pstCovRcpt->m_pstNext = NULL;			
			}
			else
			{
				/* free allocated memories */
				eRetVal = BACDEL_MALLOC_ERROR;
				break;
			}

			/* move to next node */
			pstSrcCovRcpt = pstSrcCovRcpt->m_pstNext;
		}

		/* save base node to destination */
		if(NULL != pstBasePtr)
		{
			/* copy base data */
			*pstDestCovRcpt = pstBasePtr;
		}

	}
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Copy_COVRecipeint_Info : Exit \r\n");
	#endif
	return eRetVal;
}

/**
*
* DESCRIPTION                                                                          
* This API function to copy the property value from pointer, 
* received from application as per the data type.
*
* @param pvPropVal [in] pointer to property value to be copied.
* @param eDataType [in] data type of property.
* @param pvOutData [in] pointer to copy out data.
*
* @return BACNET_RETURN_TYPE [out] either success or error.
*
*/
BACNET_RETURN_TYPE Copy_PropVal_AsPer_DataType(
	void *pvPropVal, 
	BACNET_DATA_TYPE eDataType,
	void **pvOutData)
{
	/* local variable */
	void *pvData = NULL;
	uint32_t u32Count = 0;
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;
	bool bFlag = TRUE;
	bool bBrkLoop = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Copy_PropVal_AsPer_DataType : Entry \r\n");
	#endif

	/* NULL check for pointer */
	if(NULL == pvOutData)
	{
		return BACDEL_ERROR;
	}
	/* empty and null datatype can have null pointer */
	else if(NULL == pvPropVal && eDataType != BACNET_DT_EMPTY && eDataType != BACNET_DT_NULL)
	{
		return BACDEL_ERROR;
	}

	/* reset the pointer */
	*pvOutData = NULL;

	/* switch as per data type */
	switch(eDataType)
	{
	case BACNET_DT_NULL:
	{
		/* do nothing */
	}
	break;

	case BACNET_DT_EMPTY:
	{
		/* do nothing */
	}
	break;

	case BACNET_DT_BOOLEAN:
	{
		/* local variables */
		Pr_BACnetBool_t *pstBval = NULL;
		Pr_BACnetBool_t *pstInBval = NULL;

		/* copy to temp buffer */
		pstInBval = (Pr_BACnetBool_t *)pvPropVal;

		/* allocate memory to save real pointer */
		pstBval = OSAL_Malloc(sizeof(Pr_BACnetBool_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstBval)
		{
			/* copy the data */
			pstBval->m_bVal = pstInBval->m_bVal;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBval;
	}
	break;

	case BACNET_DT_ENUM:
	{
		/* local variables */
		Pr_BinaryEnumPV_t *pstEnum = NULL;
		Pr_BinaryEnumPV_t *pstInEnum  = NULL;

		/* copy to temp buffer */
		pstInEnum = (Pr_BinaryEnumPV_t *)pvPropVal;

		/* allocate memory to save enum pointer */
		pstEnum = OSAL_Malloc(sizeof(Pr_BinaryEnumPV_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstEnum)
		{
			/* copy the data */
			pstEnum->m_eVal = pstInEnum->m_eVal;
			pstEnum->m_ePVPolarity = POLARITY_NORMAL;
			pstEnum->m_pstNext = NULL;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstEnum;
	}
	break;

	case BACNET_DT_ENUM_NEW:
	{
		/* local variables */
		Pr_BACnetEnum_t *pstEumNew  = NULL;
		Pr_BACnetEnum_t *pstInEumNew = NULL;

		/* copy to temp buffer */
		pstInEumNew = (Pr_BACnetEnum_t *)pvPropVal;

		/* allocate memory to save enum pointer */
		pstEumNew = OSAL_Malloc(sizeof(Pr_BACnetEnum_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstEumNew)
		{
			/* copy the data */
			pstEumNew->m_eVal = pstInEumNew->m_eVal;
		}
		else 
		{
			eRetVal = BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstEumNew;
	}
	break;

	case BACNET_DT_UNSIGNED32:
	case BACNET_DT_UNSIGNED:
	{
		/* local variables */
		Pr_BACnetUnsigned32_t *pstUnsigned32 = NULL;
		Pr_BACnetUnsigned32_t *pstInUnsigned32 = NULL;

		/* copy to temp buffer */
		pstInUnsigned32 = (Pr_BACnetUnsigned32_t *)pvPropVal;

		/* allocate memory to save unsigned32 pointer */
		pstUnsigned32 = OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstUnsigned32)
		{
			/* copy the data */
			pstUnsigned32->m_u32Val = pstInUnsigned32->m_u32Val;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstUnsigned32;
	}
	break;

	case BACNET_DT_UNSIGNED16:
	{
		/* local variables */
		Pr_BACnetUnsigned16_t *pstUnsigned16 = NULL;
		Pr_BACnetUnsigned16_t *pstInUnsigned16 = NULL;

		/* copy to temp buffer */
		pstInUnsigned16 = (Pr_BACnetUnsigned16_t *)pvPropVal;

		/* allocate memory to save unsigned16 pointer */
		pstUnsigned16 = OSAL_Malloc(sizeof(Pr_BACnetUnsigned16_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstUnsigned16)
		{
			/* copy the data */
			pstUnsigned16->m_u16Val = pstInUnsigned16->m_u16Val;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstUnsigned16;
	}
	break;

	case BACNET_DT_UNSIGNED8:
	{
		/* local variables */
		Pr_BACnetUnsigned8_t *pstUnsigned8 = NULL;
		Pr_BACnetUnsigned8_t *pstInUnsigned8 = NULL;

		/* copy to temp buffer */
		pstInUnsigned8 = (Pr_BACnetUnsigned8_t *)pvPropVal;

		/* allocate memory to save unsigned8 pointer */
		pstUnsigned8 = OSAL_Malloc(sizeof(Pr_BACnetUnsigned8_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstUnsigned8)
		{
			/* copy the data */
			pstUnsigned8->m_u8Val = pstInUnsigned8->m_u8Val;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstUnsigned8;
	}
	break;

	case BACNET_DT_INTEGER:
	{
		/* local variables */
		Pr_BACnetSigned32_t *pstInteger = NULL;
		Pr_BACnetSigned32_t *pstInInteger = NULL;

		/* copy to temp buffer */
		pstInInteger = (Pr_BACnetSigned32_t *)pvPropVal;

		/* allocate memory to save integer pointer */
		pstInteger = OSAL_Malloc(sizeof(Pr_BACnetSigned32_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstInteger)
		{
			/* copy the data */
			pstInteger->m_i32Val = pstInInteger->m_i32Val;
		}	
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstInteger;
	}
	break;

	case BACNET_DT_REAL:
	{
		/* local variables */
		Pr_BACnetReal_t *pstReal = NULL;
		Pr_BACnetReal_t *pstInReal = NULL;

		/* copy to temp buffer */
		pstInReal = (Pr_BACnetReal_t *)pvPropVal;

		/* allocate memory to save real pointer */
		pstReal = OSAL_Malloc(sizeof(Pr_BACnetReal_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstReal)
		{
			/* copy the data */
			pstReal->m_fVal = pstInReal->m_fVal;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstReal;
	}
	break;

	case BACNET_DT_DOUBLE:
	{
		/* local variables */
		Pr_BACnetDouble_t *pstDouble = NULL;
		Pr_BACnetDouble_t *pstInDouble = NULL;

		/* copy to local buffer */
		pstInDouble = (Pr_BACnetDouble_t *)pvPropVal;

		/* allocate memory to save real pointer */
		pstDouble = OSAL_Malloc(sizeof(Pr_BACnetDouble_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDouble)
		{
			/* copy the data */
			pstDouble->m_dVal = pstInDouble->m_dVal;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}
		/* save the pointer to be returned */
		pvData = (void *)pstDouble;
	}
	break;

	case BACNET_DT_CHARSTRING:
	{
		/* local variables */
		Pr_BACnetCharStr_t *pstCharstr = NULL;
		Pr_BACnetCharStr_t *pstInCharstr = NULL;

		/* copy to local buffer */
		pstInCharstr = (Pr_BACnetCharStr_t *)pvPropVal;

		/* allocate memory to save char string pointer */
		pstCharstr = OSAL_Malloc(sizeof(Pr_BACnetCharStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstCharstr)
		{
			/* copy the data */
			memcpy(&pstCharstr->m_stCHARString, &pstInCharstr->m_stCHARString,
				sizeof(BACnetCharStr_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstCharstr;
	}
	break;

	case BACNET_DT_TIME:
	{
		/* local variables */
		Pr_BACnetTime_t *pstTimedef = NULL;
		Pr_BACnetTime_t *pstInTimedef = NULL;

		/* copy to local buffer */
		pstInTimedef = (Pr_BACnetTime_t *)pvPropVal;

		/* allocate memory to save time deff pointer */
		pstTimedef = OSAL_Malloc(sizeof(Pr_BACnetTime_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstTimedef)
		{
			/* copy the data */
			memcpy(&pstTimedef->m_Time_val, &pstInTimedef->m_Time_val,
				sizeof(BACnetTime_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstTimedef;
	}
	break;

	case BACNET_DT_DATE:
	{
		/* local variables */
		Pr_BACnetDate_t *pstDate = NULL;
		Pr_BACnetDate_t *pstInDate = NULL;

		/* copy to local buffer */
		pstInDate = (Pr_BACnetDate_t *)pvPropVal;

		/* allocate memory to save date pointer */
		pstDate = OSAL_Malloc(sizeof(Pr_BACnetDate_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDate)
		{
			/* copy the data */
			memcpy(&pstDate->m_Date_val, &pstInDate->m_Date_val,
				sizeof(BACnetDate_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstDate;
	}
	break;

	case BACNET_DT_DATETIME:
	{
		/* local variables */
		Pr_BACnetDateTime_t *pstDateTime = NULL;
		Pr_BACnetDateTime_t *pstInDateTime = NULL;

		/* copy to local buffer */
		pstInDateTime = (Pr_BACnetDateTime_t *)pvPropVal;

		/* allocate memory to save date-time pointer */
		pstDateTime = OSAL_Malloc(sizeof(Pr_BACnetDateTime_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDateTime)
		{
			/* copy the data */
			memcpy(&pstDateTime->m_stDateTime, &pstInDateTime->m_stDateTime,
				sizeof(BACnetDateTime_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstDateTime;
	}
	break;

	case BACNET_DT_OBJECTID:
	{
		/* local variables */
		Pr_BACnetObjId_t *pstObjId = NULL;
		Pr_BACnetObjId_t *pstInObjId = NULL;

		/* copy to local buffer */
		pstInObjId = (Pr_BACnetObjId_t *)pvPropVal;

		/* allocate memory to save object-id pointer */
		pstObjId = OSAL_Malloc(sizeof(Pr_BACnetObjId_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstObjId)
		{
			/* copy the data */
			pstObjId->m_u32ObjId = pstInObjId->m_u32ObjId;
			pstObjId->m_eObjectType = pstInObjId->m_eObjectType;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjId;
	}
	break;

	case BACNET_DT_OBJTYPE:
	{
		/* local variables */
		Pr_BACnetObjType_t *pstObjType = NULL;
		Pr_BACnetObjType_t *pstInObjType = NULL;

		/* copy to local buffer */
		pstInObjType = (Pr_BACnetObjType_t *)pvPropVal;

		/* allocate memory to save object-id pointer */
		pstObjType = OSAL_Malloc(sizeof(Pr_BACnetObjType_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstObjType)
		{
			/* copy the data */
			pstObjType->m_eObjectType = pstInObjType->m_eObjectType;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjType;
	}
	break;
	case BACNET_DT_BDT_ENTRY:
	{
		/* local variables */
		Pr_BACnetBDTEntry_t *pstBDTEntry = NULL;
		Pr_BACnetBDTEntry_t *pstInBDTEntry = NULL;

		/* copy to local pointer */
		pstInBDTEntry = (Pr_BACnetBDTEntry_t *)pvPropVal;

		/* allocate memory to save bdt value pointer */
		pstBDTEntry = OSAL_Malloc(sizeof(Pr_BACnetBDTEntry_t),
			__FILE__, __FUNCTION__, __LINE__);

		if(NULL != pstBDTEntry)
		{
			/* copy data */
			memcpy(&pstBDTEntry->m_stBDTEntry, &pstInBDTEntry->m_stBDTEntry,
				sizeof(BACnetBDTEntry_t));
		}
		else
		{
			eRetVal = BACDEL_MALLOC_ERROR;
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBDTEntry;
	}
	break;

	case BACNET_DT_BDT_ENTRY_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetBDTEntry_t *pstBDTEntryList = NULL;
		Pr_ListOfBACnetBDTEntry_t *pstInBDTEntryList = NULL;
		ListOfBACnetBDTEntry_t *pstBDTEntry = NULL;
		ListOfBACnetBDTEntry_t *pstInBDTEntry = NULL;

		/* copy to local pointer */
		pstInBDTEntryList = (Pr_ListOfBACnetBDTEntry_t *)pvPropVal;
		pstInBDTEntry = pstInBDTEntryList->m_pstBDTEntryList;

		/* allocate memory to save bdt enrty list/array pointer */
		pstBDTEntryList = OSAL_Malloc(sizeof(Pr_ListOfBACnetBDTEntry_t), __FILE__, __FUNCTION__, __LINE__);
		if(NULL != pstBDTEntryList)
		{
			while(NULL != pstInBDTEntry && (NULL != pstBDTEntry || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstBDTEntryList->m_pstBDTEntryList = OSAL_Malloc(sizeof(ListOfBACnetBDTEntry_t),
						__FILE__, __FUNCTION__, __LINE__);
					/* reset flag */
					bFlag = FALSE;
					/* update the pointer */
					pstBDTEntry = pstBDTEntryList->m_pstBDTEntryList;
				}
				else
				{
					/* allocate memory */
					pstBDTEntry->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetBDTEntry_t),
						__FILE__, __FUNCTION__, __LINE__);
					/* move to new node */
					pstBDTEntry = pstBDTEntry->m_pstNext;
				}

				if(NULL != pstBDTEntry)
				{
					/* copy data */
					memcpy(pstBDTEntry, pstInBDTEntry, sizeof(ListOfBACnetBDTEntry_t));
					pstBDTEntry->m_pstNext = NULL;
					/* increment the count */
					pstBDTEntryList->m_u32ArraySize++;
				}
				else
				{
					/* free allocated memories */
					Clear_BdtEntry_List(&pstBDTEntryList->m_pstBDTEntryList);
					OSAL_Free(pstBDTEntryList, __FILE__, __FUNCTION__, __LINE__);
					pstBDTEntryList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR;
					break;
				}

				/* move to next node */
				pstInBDTEntry = pstInBDTEntry->m_pstNext;
			}
		}
		else
		{
			eRetVal = BACDEL_MALLOC_ERROR;
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBDTEntryList;
	}
	break;
	case BACNET_DT_BACNETDEVSTAT:
	{
		/* local variables */
		Pr_BACnetDevStatus_t *pstDevStat = NULL;
		Pr_BACnetDevStatus_t *pstInDevStat = NULL;

		/* copy to local buffer */
		pstInDevStat = (Pr_BACnetDevStatus_t *)pvPropVal;

		/* allocate memory to save dev-status pointer */
		pstDevStat = OSAL_Malloc(sizeof(Pr_BACnetDevStatus_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDevStat)
		{
			/* copy the data */
			pstDevStat->m_eDeviceStatus = pstInDevStat->m_eDeviceStatus;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}
		/* save the pointer to be returned */
		pvData = (void *)pstDevStat;
	}
	break;

	case BACNET_DT_SERVICES_SUPPORTED:
	{
		/* local variables */
		Pr_BACnetServicesSupported_t *pstSerSupported = NULL;
		Pr_BACnetServicesSupported_t *pstInSerSupported = NULL;

		/* copy to local buffer */
		pstInSerSupported = (Pr_BACnetServicesSupported_t *)pvPropVal;

		/* allocate memory to save service support pointer */
		pstSerSupported = OSAL_Malloc(sizeof(Pr_BACnetServicesSupported_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstSerSupported)
		{
			/* copy the data */
			memcpy(&pstSerSupported->m_stServiceSupport,
				&pstInSerSupported->m_stServiceSupport,
				sizeof(BACnetServicesSupported_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstSerSupported;
	}
	break;

	case BACNET_DT_OBJECT_TYPE_SUPPORTED:
	{
		/* local variables */
		Pr_BACnetObjectTypesSupported_t *pstObjSupported = NULL;
		Pr_BACnetObjectTypesSupported_t *pstInObjSupported = NULL;

		/* copy to local buffer */
		pstInObjSupported = (Pr_BACnetObjectTypesSupported_t *)pvPropVal;

		/* allocate memory to save objtype support pointer */
		pstObjSupported = OSAL_Malloc(sizeof(Pr_BACnetObjectTypesSupported_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstObjSupported)
		{
			/* copy the data */
			memcpy(&pstObjSupported->m_stObjectSupport,
				&pstInObjSupported->m_stObjectSupport,
				sizeof(BACnetServicesSupported_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjSupported;
	}
	break;

	case BACNET_DT_BACNETSEG:
	{
		/* local variables */
		Pr_BACnetSegmentation_t *pstSegSupp = NULL;
		Pr_BACnetSegmentation_t *pstInSegSupp = NULL;

		/* copy to local buffer */
		pstInSegSupp = (Pr_BACnetSegmentation_t *)pvPropVal;

		/* allocate memory to save segmentation pointer */
		pstSegSupp = OSAL_Malloc(sizeof(Pr_BACnetSegmentation_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstSegSupp)
		{
			/* copy the data */
			pstSegSupp->m_eSegmentationSupport = pstInSegSupp->m_eSegmentationSupport;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}
		/* save the pointer to be returned */
		pvData = (void *)pstSegSupp;
	}
	break;

	case BACNET_DT_TIMESTAMP:
	{
		/* local variables */
		Pr_BACnetTimeStamp_t *pstTimStamp = NULL;
		Pr_BACnetTimeStamp_t *pstInTimStamp = NULL;

		/* copy to local variables */
		pstInTimStamp = (Pr_BACnetTimeStamp_t *)pvPropVal;

		/* allocate memory to save timestamp pointer */
		pstTimStamp = OSAL_Malloc(sizeof(Pr_BACnetTimeStamp_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstTimStamp)
		{
			/* copy the data */
			memcpy(&pstTimStamp->m_stTimeStamp.m_stTimeStamp, 
				&pstInTimStamp->m_stTimeStamp.m_stTimeStamp,
				sizeof(timeStamp_u));
			pstTimStamp->m_stTimeStamp.m_eTimeStampType = pstInTimStamp->m_stTimeStamp.m_eTimeStampType;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstTimStamp;
	}
	break;

	case BACNET_DT_PRIORITY_ARRAY:
	{
		/* local variables */
		Pr_BACnetPriorityArray_t *pstBinPrioArr = NULL;
		Pr_BACnetPriorityArray_t *pstInBinPrioArr = NULL;

		/* copy to local buffer */
		pstInBinPrioArr = (Pr_BACnetPriorityArray_t *)pvPropVal;

		/* allocate memory to save binary priority array pointer */
		pstBinPrioArr = OSAL_Malloc(sizeof(Pr_BACnetPriorityArray_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstBinPrioArr)
		{
			memcpy(&pstBinPrioArr->m_bTagType, &pstInBinPrioArr->m_bTagType,
				sizeof(pstBinPrioArr->m_bTagType));
			memcpy(&pstBinPrioArr->m_uValue, &pstInBinPrioArr->m_uValue,
				sizeof(pstBinPrioArr->m_uValue));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBinPrioArr;
	}
	break;

	case BACNET_DT_BITSTRING_NEW:
	{
		/* local variables */
		Pr_BACnetBITStr_t *pstNewBitStr = NULL;
		Pr_BACnetBITStr_t *pstInNewBitStr = NULL;

		/* copy to local buffer */
		pstInNewBitStr = (Pr_BACnetBITStr_t *)pvPropVal;

		/* allocate memory to save bitstring pointer */
		pstNewBitStr = OSAL_Malloc(sizeof(Pr_BACnetBITStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstNewBitStr)
		{
			memcpy(&pstNewBitStr->m_stBitString, &pstInNewBitStr->m_stBitString,
				sizeof(BACnetBITStr_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstNewBitStr;
	}
	break;


	case BACNET_DT_OCTETSTRING:
	{
		/* local variables */
		Pr_BACnetOctetStr_t *pstOctStr = NULL;
		Pr_BACnetOctetStr_t *pstInOctStr = NULL;

		/* copy to local buffer */
		pstInOctStr = (Pr_BACnetOctetStr_t *)pvPropVal;

		/* allocate memory to save octetstring pointer */
		pstOctStr = OSAL_Malloc(sizeof(Pr_BACnetOctetStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstOctStr)
		{
			memcpy(&pstOctStr->m_stOctetString, &pstInOctStr->m_stOctetString,
				sizeof(BACnetOctetStr_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstOctStr;
	}
	break;

	case BACNET_DT_BITSTRING:
	{
		/* local variables */
		Pr_BACnetBitStr_t *pstBitstr = NULL;
		Pr_BACnetBitStr_t *pstInBitstr = NULL;

		/* copy to local buffer */
		pstInBitstr = (Pr_BACnetBitStr_t *)pvPropVal;
		 
		/* allocate memory to save bitstring pointer */
		pstBitstr = OSAL_Malloc(sizeof(Pr_BACnetBitStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstBitstr)
		{
			memcpy(pstBitstr->m_stBitStr.m_u8TransBits, pstInBitstr->m_stBitStr.m_u8TransBits,
				MIN_BITSTRING_BYTES);
			pstBitstr->m_stBitStr.m_u8ByteCnt = pstInBitstr->m_stBitStr.m_u8ByteCnt;
			pstBitstr->m_stBitStr.m_u8UnusedBits = pstInBitstr->m_stBitStr.m_u8UnusedBits;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBitstr;
	}
	break;

	case BACNET_DT_TIMESTAMP_ARRAY:
	{
		/* local variables */
		Pr_BACnetEventTimeStamp_t *pstTimeStamp = NULL;
		Pr_BACnetEventTimeStamp_t *pstInTimeStamp = NULL;

		/* copy to local buffer */
		pstInTimeStamp = (Pr_BACnetEventTimeStamp_t *)pvPropVal;
	 
		/* allocate memory to save bitstring pointer */
		pstTimeStamp = OSAL_Malloc(sizeof(Pr_BACnetEventTimeStamp_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstTimeStamp)
		{
			memcpy(&pstTimeStamp->m_eTimeStampType, &pstInTimeStamp->m_eTimeStampType,
				sizeof(pstTimeStamp->m_eTimeStampType));
			memcpy(&pstTimeStamp->m_uEvntStamp, &pstInTimeStamp->m_uEvntStamp,
				sizeof(pstTimeStamp->m_uEvntStamp));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstTimeStamp;
	}
	break;

	case BACNET_DT_BACKUPSTATE:
	{
		/* local variables */
		Pr_BACnetBackupState_t *pstBackupState = NULL;
		Pr_BACnetBackupState_t *pstInBackupState = NULL;

		/* copy to local buffer */
		pstInBackupState = (Pr_BACnetBackupState_t *)pvPropVal;
	 
		/* allocate memory to save bitstring pointer */
		pstBackupState = OSAL_Malloc(sizeof(Pr_BACnetBackupState_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstBackupState)
		{
			pstBackupState->m_eBackupState = pstInBackupState->m_eBackupState;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBackupState;
	}
	break;

	case BACNET_DT_CHARSTRING_ARRAY:
	{
		/* local variables */
		Pr_ListOfCharStr_t *pstStrArr;
		Pr_ListOfCharStr_t *pstInStrArr;
		ListOfCharStr_t *pstCharStr = NULL;
		ListOfCharStr_t *pstInCharStr = NULL;

		/* copy to local buffer */
		pstInStrArr = (Pr_ListOfCharStr_t *)pvPropVal;
		pstInCharStr = pstInStrArr->m_stStringVal.m_pstNext;

		/* allocate memory to save charstring array pointer */
		pstStrArr = OSAL_Malloc(sizeof(Pr_ListOfCharStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstStrArr)
		{
			/* copy the first node */
			memcpy(&pstStrArr->m_stStringVal, &pstInStrArr->m_stStringVal, sizeof(ListOfCharStr_t));
			/* as 1st member is static, if count is 0 do increment anything */
			if(pstInStrArr->m_u32Count)
			{
				pstStrArr->m_u32Count++;
			}
			pstCharStr = &pstStrArr->m_stStringVal;

			while(NULL != pstInCharStr && NULL != pstCharStr)
			{
				/* allocate memory for next node */
				pstCharStr->m_pstNext = OSAL_Malloc(sizeof(ListOfCharStr_t),
						__FILE__,__FUNCTION__,__LINE__);
				
				if(NULL != pstCharStr->m_pstNext)
				{
					/* move to new node */
					pstCharStr = pstCharStr->m_pstNext;
					/* copy data */
					memcpy(pstCharStr, pstInCharStr, sizeof(ListOfCharStr_t));
					pstCharStr->m_pstNext = NULL;

					/* increment the count */
					pstStrArr->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_CharString_List(&pstStrArr->m_stStringVal.m_pstNext);
					OSAL_Free(pstStrArr, __FILE__,__FUNCTION__,__LINE__);
					pstStrArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInCharStr = pstInCharStr->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstStrArr;
	}
	break;

	case BACNET_DT_OBJECTID_ARRAY:
	{
		/* local variables */
		Pr_ListOfObjId_t *pstObjIDArr = NULL;
		Pr_ListOfObjId_t *pstInObjIDArr = NULL;
		ListOfObjId_t *pstArrObjId = NULL;
		ListOfObjId_t *pstInArrObjId = NULL;

		/* copy to local buffer */
		pstInObjIDArr = (Pr_ListOfObjId_t *)pvPropVal;
		pstInArrObjId = pstInObjIDArr->m_pstArrayObjId;

		/* allocate memory to save charstring array pointer */
		pstObjIDArr = OSAL_Malloc(sizeof(Pr_ListOfObjId_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstObjIDArr)
		{
			while(NULL != pstInArrObjId && (NULL != pstArrObjId || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstObjIDArr->m_pstArrayObjId = OSAL_Malloc(sizeof(ListOfObjId_t),
						__FILE__,__FUNCTION__,__LINE__);					
					/* reset flag */
					bFlag = FALSE;
					pstArrObjId = pstObjIDArr->m_pstArrayObjId;
				}
				else
				{
					/* allocate memory */
					pstArrObjId->m_pstNext = OSAL_Malloc(sizeof(ListOfObjId_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstArrObjId = pstArrObjId->m_pstNext;					
				}

				if(NULL != pstArrObjId)
				{
					/* copy the data */
					pstArrObjId->m_stObjectId.m_eObjectType = pstInArrObjId->m_stObjectId.m_eObjectType;
					pstArrObjId->m_stObjectId.m_u32ObjId = pstInArrObjId->m_stObjectId.m_u32ObjId;
					pstArrObjId->m_pstNext = NULL;

					/* increment the count */
					pstObjIDArr->m_u32ObjCount++;
				}
				else
				{
					/* free allocated memories */
					Clear_ObjectID_List(&pstObjIDArr->m_pstArrayObjId);
					OSAL_Free(pstObjIDArr, __FILE__,__FUNCTION__,__LINE__);
					pstObjIDArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInArrObjId = pstInArrObjId->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjIDArr;
	}
	break;

	case BACNET_DT_OPTCHARSTRING_ARRAY:
	case BACNET_DT_OPTIONAL_CHARSTRING:
	{
		/* local variables */
		Pr_ListOfOptCharStr_t *pstOptStrArr = NULL;
		Pr_ListOfOptCharStr_t *pstInOptStrArr = NULL;
		ListOfOptCharStr_t *pstOptCharStr = NULL;
		ListOfOptCharStr_t *pstInOptCharStr = NULL;
	    
		/* copy to local buffer */
		pstInOptStrArr = (Pr_ListOfOptCharStr_t *)pvPropVal;
		pstInOptCharStr = pstInOptStrArr->m_pstOptCharStr;

		/* allocate memory to save Optional Character String array pointer */
		pstOptStrArr = OSAL_Malloc(sizeof(Pr_ListOfOptCharStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstOptStrArr)
		{
			while(NULL != pstInOptCharStr && (NULL != pstOptCharStr || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstOptStrArr->m_pstOptCharStr = OSAL_Malloc(
						sizeof(ListOfOptCharStr_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = false;
					pstOptCharStr = pstOptStrArr->m_pstOptCharStr; 
				}
				else
				{
					/* allocate memory */
					pstOptCharStr->m_pstNext = OSAL_Malloc(
						sizeof(ListOfOptCharStr_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstOptCharStr = pstOptCharStr->m_pstNext;
				}

				if(NULL != pstOptCharStr)
				{
					/* copy the first node */
					memcpy(pstOptCharStr, pstInOptCharStr, 
						sizeof(ListOfOptCharStr_t));
					pstOptCharStr->m_pstNext = NULL;

					/* increment the count */
					pstOptStrArr->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_OptionalCharString_List(&pstOptStrArr->m_pstOptCharStr);
					OSAL_Free(pstOptStrArr, __FILE__,__FUNCTION__,__LINE__);
					pstOptStrArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* break if single node is present */
				if(BACNET_DT_OPTIONAL_CHARSTRING == eDataType)
				{
					break;
				}
				/* move to next node */
				pstInOptCharStr = pstInOptCharStr->m_pstNext;
			}
		}
		else 
		{
			eRetVal = BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstOptStrArr;
	}
	break;
		
	case BACNET_DT_DEVOBJREFF_LIST:
	case BACNET_DT_DEVOBJREFF_ARRAY:
	{
		/* local variables */
		Pr_ListOfBACnetDevObjRef_t *pstObjReffArr = NULL;
		Pr_ListOfBACnetDevObjRef_t *pstInObjReffArr = NULL;
		ListOfBACnetDevObjRef_t *pstDevObjReff = NULL;
		ListOfBACnetDevObjRef_t *pstInDevObjReff = NULL;

		/* copy to local buffer */
		pstInObjReffArr = (Pr_ListOfBACnetDevObjRef_t *)pvPropVal;
		pstInDevObjReff = pstInObjReffArr->m_pstListOfDevObjReff;

		/* allocate memory to save Device Object Reference array/List pointer */
		pstObjReffArr = OSAL_Malloc(
			sizeof(Pr_ListOfBACnetDevObjRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstObjReffArr)
		{
			while(NULL != pstInDevObjReff && (NULL != pstDevObjReff || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstObjReffArr->m_pstListOfDevObjReff = OSAL_Malloc(
						sizeof(ListOfBACnetDevObjRef_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = false;
					pstDevObjReff = pstObjReffArr->m_pstListOfDevObjReff; 
				}
				else
				{
					/* allocate memory */
					pstDevObjReff->m_pstNext = OSAL_Malloc(
						sizeof(ListOfBACnetDevObjRef_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstDevObjReff = pstDevObjReff->m_pstNext;
				}

				if(NULL != pstDevObjReff)
				{
					/* copy the first node */
					memcpy(pstDevObjReff, pstInDevObjReff, sizeof(ListOfBACnetDevObjRef_t));
					pstDevObjReff->m_pstNext = NULL;

					/* increment the count */
					pstObjReffArr->m_u32ArraySize++;
				}
				else
				{
					/* free allocated memories */
					Clear_DevObjRef_List(&pstObjReffArr->m_pstListOfDevObjReff);
					OSAL_Free(pstObjReffArr, __FILE__,__FUNCTION__,__LINE__);
					pstObjReffArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInDevObjReff = pstInDevObjReff->m_pstNext;
			}
		}
		else 
		{
			eRetVal = BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjReffArr;
	}
	break;

	case BACNET_DT_DEVOBJPROPREFF_LIST:
	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
	{
		/* local variables */
		Pr_ListOfBACnetDevObjPropRef_t *pstObjPropRefArr = NULL;
		Pr_ListOfBACnetDevObjPropRef_t *pstInObjPropRefArr = NULL;
		ListOfBACnetDevObjPropRef_t *pstDevObjPropReff = NULL;
		ListOfBACnetDevObjPropRef_t *pstInDevObjPropReff = NULL;

		/* copy to local buffer */
		pstInObjPropRefArr = (Pr_ListOfBACnetDevObjPropRef_t *)pvPropVal;
		pstInDevObjPropReff = 
			pstInObjPropRefArr->m_pstListOfBACnetDevObjPropReff;

		/* allocate memory to save Device Object Property Reff. array/List pointer */
		pstObjPropRefArr = OSAL_Malloc(sizeof
			(Pr_ListOfBACnetDevObjPropRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstObjPropRefArr)
		{
			while(NULL != pstInDevObjPropReff && (NULL != pstDevObjPropReff || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstObjPropRefArr->m_pstListOfBACnetDevObjPropReff = 
						OSAL_Malloc(sizeof(ListOfBACnetDevObjPropRef_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = false;
					pstDevObjPropReff = pstObjPropRefArr->m_pstListOfBACnetDevObjPropReff;
				}
				else
				{
					/* allocate memory */
					pstDevObjPropReff->m_pstNext = 
						OSAL_Malloc(sizeof(ListOfBACnetDevObjPropRef_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstDevObjPropReff = pstDevObjPropReff->m_pstNext;
				}

				if(NULL != pstDevObjPropReff)
				{
					/* copy data */
					memcpy(pstDevObjPropReff, pstInDevObjPropReff, 
						sizeof(ListOfBACnetDevObjPropRef_t));
					pstDevObjPropReff->m_pstNext = NULL;
					
					/* increment the count */
					pstObjPropRefArr->m_u32ArraySize++;
				}
				else
				{
					/* free allocated memories */
					Clear_DevObjPropReff_List(&pstObjPropRefArr->m_pstListOfBACnetDevObjPropReff);
					OSAL_Free(pstObjPropRefArr, __FILE__,__FUNCTION__,__LINE__);
					pstObjPropRefArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInDevObjPropReff = pstInDevObjPropReff->m_pstNext;
			}
		}
		else 
		{
			eRetVal = BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjPropRefArr;
	}
	break;

	case BACNET_DT_DESTINATION_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetDestination_t *pstRecipList = NULL;
		Pr_ListOfBACnetDestination_t *pstInRecipList = NULL;
		ListOfBACnetDestination_t *pstRecipient = NULL;
		ListOfBACnetDestination_t *pstInRecipient = NULL;

		/* copy to local buffer */
		pstInRecipList = (Pr_ListOfBACnetDestination_t *)pvPropVal;
		pstInRecipient = pstInRecipList->m_pstNCRecepient;

		/* allocate memory to save Recipient List pointer */
		pstRecipList = OSAL_Malloc(sizeof
			(Pr_ListOfBACnetDestination_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstRecipList)
		{
			while(NULL != pstInRecipient && (NULL != pstRecipient || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstRecipList->m_pstNCRecepient = OSAL_Malloc(sizeof(ListOfBACnetDestination_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = false;
					pstRecipient = pstRecipList->m_pstNCRecepient;
				}
				else
				{
					/* allocate memory */
					pstRecipient->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetDestination_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstRecipient = pstRecipient->m_pstNext;
				}

				if(NULL != pstRecipient)
				{
					/* copy data */
					memcpy(pstRecipient, pstInRecipient, sizeof(ListOfBACnetDestination_t));
					pstRecipient->m_pstNext = NULL;

					/* increment the count */
					pstRecipList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_Destination_List(&pstRecipList->m_pstNCRecepient);
					OSAL_Free(pstRecipList, __FILE__,__FUNCTION__,__LINE__);
					pstRecipList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInRecipient = pstInRecipient->m_pstNext;
			}
			/* save the count */
			//pstRecipList->m_u32Count = u32Count;
		}
		else 
		{
			eRetVal = BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstRecipList;
	}
	break;

	case BACNET_DT_EVENTPARAMETERS: // TODO - incomplete case 
	{
		/* local variables */
		Pr_BACnetEventParameter_t *pstEveParam = NULL;
		Pr_BACnetEventParameter_t *pstInEveParam = NULL;
		
		/* copy to local buffer */
		pstInEveParam = (Pr_BACnetEventParameter_t *)pvPropVal;

		/* allocate memory to save Event Parameters pointer*/
		pstEveParam = OSAL_Malloc(sizeof
			(Pr_BACnetEventParameter_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstEveParam)
		{
			/* copy data */
			memcpy(pstEveParam, pstInEveParam, sizeof(Pr_BACnetEventParameter_t));
			// TODO - copy internal lists
			if(EVENT_CHANGE_OF_STATE == pstInEveParam->m_stEventParam.m_eEventType)
			{
				ChangeOfState_t	stCngState = {0};				
				ChangeOfState_t *pstInCngState = NULL;
				ListOfBACnetPropertyStates_t *pstPropState = NULL;
				ListOfBACnetPropertyStates_t *pstInPropState = NULL;
				void *pvTemp = NULL;

				pstInCngState = &pstInEveParam->m_stEventParam.BACnetEventParameter_u.
					m_stCngState;
				pstInPropState = pstInCngState->m_pstListOfValues;
				
				/* save time delay value */
				stCngState.m_u32Timedelay = pstInEveParam->m_stEventParam.BACnetEventParameter_u.
					m_stCngState.m_u32Timedelay;

				while(NULL != pstInPropState && (NULL != pstPropState || bFlag))
				{
					if(bFlag)
					{
						/* allocate memory */
						stCngState.m_pstListOfValues = OSAL_Malloc
							(sizeof(ListOfBACnetPropertyStates_t),
							__FILE__,__FUNCTION__,__LINE__);
						/* reset flag & new node address */
						bFlag = false;
						pstPropState = stCngState.m_pstListOfValues;
					}
					else
					{
						/* allocate memory */
						pstPropState->m_pstNext = OSAL_Malloc(sizeof
							(ListOfBACnetPropertyStates_t),__FILE__,__FUNCTION__,__LINE__);
						/* move to new node */
						pstPropState = pstPropState->m_pstNext;
					}

					if(NULL != pstPropState)
					{
						/* copy data */
						memcpy(pstPropState, pstInPropState,
							sizeof(ListOfBACnetPropertyStates_t));
						pstPropState->m_pstNext = NULL;

						/* increment the count */
						u32Count++;
					}
					else
					{
						/* free allocated memories */
						while(NULL != stCngState.m_pstListOfValues)
						{
							pvTemp = stCngState.m_pstListOfValues->m_pstNext;
							OSAL_Free(stCngState.m_pstListOfValues,
								__FILE__,__FUNCTION__,__LINE__);
							stCngState.m_pstListOfValues = pvTemp;
						}						
						eRetVal = BACDEL_MALLOC_ERROR; 
						break;
					}
					/* move to next node */
					pstInPropState = pstInPropState->m_pstNext;
				}

				/* memcopy change of state */
				memcpy(&pstEveParam->m_stEventParam.BACnetEventParameter_u.m_stCngState,
					&stCngState, sizeof(ChangeOfState_t));
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstEveParam;
	}
	break;

	case BACNET_DT_DEVOBJPROPREF:
	{
		/* local variables */
		Pr_BACnetDevObjPropRef_t *pstDevObjPropRef = NULL;
		Pr_BACnetDevObjPropRef_t *pstInDevObjPropRef = NULL;

		/* copy to local buffer */
		pstInDevObjPropRef = (Pr_BACnetDevObjPropRef_t *)pvPropVal;

		/* allocate memory to save devobjpropreff pointer */
		pstDevObjPropRef = OSAL_Malloc(sizeof(Pr_BACnetDevObjPropRef_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDevObjPropRef)
		{
			/* copy data */
			memcpy(pstDevObjPropRef, pstInDevObjPropRef, sizeof(Pr_BACnetDevObjPropRef_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstDevObjPropRef;
	}
	break;

	case BACNET_DT_NOTIFICATIONPRIORITY:
	{
		/* local variables */
		Pr_BACnetNotifyPriority_t *pstNotiPriority = NULL;
		Pr_BACnetNotifyPriority_t *pstInNotiPriority = NULL;

		/* copy to local buffer */
		pstInNotiPriority = (Pr_BACnetNotifyPriority_t *)pvPropVal;

		/* allocate memory to save notification priority pointer */
		pstNotiPriority = OSAL_Malloc(sizeof(Pr_BACnetNotifyPriority_t),__FILE__,
			__FUNCTION__,__LINE__);

		if(NULL != pstNotiPriority)
		{
			/* copy data */
			memcpy(pstNotiPriority->m_u32Value, pstInNotiPriority->m_u32Value,
				sizeof(pstNotiPriority->m_u32Value));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstNotiPriority;

	}
	break;

	case BACNET_DT_DATERANGE:
	{
		/* local variables */
		Pr_BACnetDateRange_t *pstEffectPeriod = NULL;
		Pr_BACnetDateRange_t *pstInEffectPeriod = NULL;

		/* copy to local buffer */
		pstInEffectPeriod = (Pr_BACnetDateRange_t *)pvPropVal;

		/* allocate memory to save effective period pointer */
		pstEffectPeriod = OSAL_Malloc(sizeof(Pr_BACnetDateRange_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstEffectPeriod)
		{
			/* copy data */
			memcpy(&pstEffectPeriod->m_stDateRange, &pstInEffectPeriod->m_stDateRange,
				sizeof(BACnetDateRange_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstEffectPeriod;
	}
	break;

	case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
	{
		/* local variables */
		AnyValue_t *pstSchedulePresDef = NULL;
		AnyValue_t *pstInSchedulePresDef = NULL;

		/* copy to local buffer */
		pstInSchedulePresDef = (AnyValue_t *)pvPropVal;

		/* allocate memory to save eschedule present default pointer */
		pstSchedulePresDef = OSAL_Malloc(sizeof(AnyValue_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstSchedulePresDef)
		{
			/* copy data */
			pstSchedulePresDef->m_stValue.m_ContextSpecific = -1;
			pstSchedulePresDef->m_stValue.m_ContextTag = -1;
			pstSchedulePresDef->m_stValue.m_StartTag = -1;
			pstSchedulePresDef->m_stValue.m_TagType = pstInSchedulePresDef->m_stValue.m_TagType;
			memcpy(&pstSchedulePresDef->m_stValue.uValue, &pstInSchedulePresDef->m_stValue.uValue,
				sizeof(pstSchedulePresDef->m_stValue.uValue));
			pstSchedulePresDef->m_stValue.pstNextPropVal = NULL;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstSchedulePresDef;
	}
	break;

	case BACNET_DT_CLIENTCOVINCREMENT:
	{
		/* local variables */
		Pr_BACnetClientCOV_t *pstClientCovInc = NULL;
		Pr_BACnetClientCOV_t *pstInClientCovInc = NULL;

		/* copy to local buffer */
		pstInClientCovInc = (Pr_BACnetClientCOV_t *)pvPropVal;

		/* allocate memory to save client cov increment pointer */
		pstClientCovInc = OSAL_Malloc(sizeof(Pr_BACnetClientCOV_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstClientCovInc)
		{
			/* copy data */
			pstClientCovInc->m_stClientCOV.m_fVal = pstInClientCovInc->m_stClientCOV.m_fVal;
			pstClientCovInc->m_stClientCOV.m_eAppTagtype = pstInClientCovInc->m_stClientCOV
				.m_eAppTagtype;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstClientCovInc;
	}
	break;

	case BACNET_DT_OBJPROPREF:
	{
		/* local variables */
		Pr_BACnetObjPropRef_t *pstObjPropRef = NULL;
		Pr_BACnetObjPropRef_t *pstInObjPropRef = NULL;

		/* copy to local buffer */
		pstInObjPropRef = (Pr_BACnetObjPropRef_t *)pvPropVal;

		/* allocate memory to save object prop reference pointer */
		pstObjPropRef = OSAL_Malloc(sizeof(Pr_BACnetObjPropRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstObjPropRef)
		{
			/* copy data */
			memcpy(pstObjPropRef, pstInObjPropRef, sizeof(Pr_BACnetObjPropRef_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstObjPropRef;
	}
	break;

	case BACNET_DT_SETPOINTREF:
	{
		/* local variables */
		Pr_BACnetSetpointRef_t *pstSetptRef = NULL;
		Pr_BACnetSetpointRef_t *pstInSetptRef = NULL;

		/* copy to local buffer */
		pstInSetptRef = (Pr_BACnetSetpointRef_t *)pvPropVal;

		/* allocate memory to save setpoint reference pointer */
		pstSetptRef = OSAL_Malloc(sizeof(Pr_BACnetSetpointRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstSetptRef)
		{
			/* copy data */
			memcpy(pstSetptRef, pstInSetptRef, sizeof(Pr_BACnetSetpointRef_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstSetptRef;
	}
	break;

	case BACNET_DT_DEVOBJREFF:
	{
		/* local variables */
		Pr_BACnetDevObjRef_t *pstDevObjRef = NULL;
		Pr_BACnetDevObjRef_t *pstInDevObjRef = NULL;

		/* copy to local buffer */
		pstInDevObjRef = (Pr_BACnetDevObjRef_t *)pvPropVal;

		/* allocate memory to save device object reference pointer */
		pstDevObjRef = OSAL_Malloc(sizeof(Pr_BACnetDevObjRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDevObjRef)
		{
			/* copy data */
			memcpy(pstDevObjRef, pstInDevObjRef, sizeof(Pr_BACnetDevObjRef_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstDevObjRef;
	}
	break;

	case BACNET_DT_SHEDLEVEL:
	{
		/* local variables */
		Pr_BACnetShedLevel_t *pstShedLev = NULL;
		Pr_BACnetShedLevel_t *pstInShedLev = NULL;

		/* copy to local buffer */
		pstInShedLev = (Pr_BACnetShedLevel_t *)pvPropVal;

		/* allocate memory to save sched level pointer */
		pstShedLev = OSAL_Malloc(sizeof(Pr_BACnetShedLevel_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstShedLev)
		{
			/* copy data */
			memcpy(&pstShedLev->m_stShedLevel, &pstInShedLev->m_stShedLevel,
				sizeof(BACnetShedLevel_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstShedLev;
	}
	break;

	case BACNET_DT_UNSIGNED_LIST:
	case BACNET_DT_UNSIGNED_ARRAY:
	{
		Pr_ListOfUnsigned_t *pstUnsignListArr = NULL;
		Pr_ListOfUnsigned_t *pstInUnsignListArr = NULL;
		ListOfUnsigned_t *pstIntList = NULL;
		ListOfUnsigned_t *pstInIntList = NULL;

		/* copy to local buffer */
		pstInUnsignListArr = (Pr_ListOfUnsigned_t *)pvPropVal;
		pstInIntList = pstInUnsignListArr->m_pstUnsignVal;

		/* allocate memory to save unsigned list/array pointer */
		pstUnsignListArr = OSAL_Malloc(sizeof(Pr_ListOfUnsigned_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstUnsignListArr)
		{
			while(NULL != pstInIntList && (NULL != pstIntList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstUnsignListArr->m_pstUnsignVal = OSAL_Malloc(sizeof(ListOfUnsigned_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* reset flag */
					bFlag = FALSE;
					/* update the pointer */
					pstIntList = pstUnsignListArr->m_pstUnsignVal;
				}
				else
				{
					/* allocate memory */
					pstIntList->m_pstNext = OSAL_Malloc(sizeof(ListOfUnsigned_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstIntList = pstIntList->m_pstNext;
				}

				if(NULL != pstIntList)
				{
					/* copy data */
					pstIntList->m_u32Value = pstInIntList->m_u32Value;
					pstIntList->m_pstNext = NULL;

					/* increment the count */
					pstUnsignListArr->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_UnsignedInt_List(&pstUnsignListArr->m_pstUnsignVal);
					OSAL_Free(pstUnsignListArr, __FILE__,__FUNCTION__,__LINE__);
					pstUnsignListArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInIntList = pstInIntList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstUnsignListArr;
	}
	break;

	case BACNET_DT_BACNETVTCLASS:
	{
		/* local variables */
		Pr_BACnetVTClass_t *pstVTClass = NULL;
		Pr_BACnetVTClass_t *pstInVTClass = NULL;
		Pr_BACnetVTClass_t *pstBaseNode = NULL;

		/* copy to local buffer */
		pstInVTClass = (Pr_BACnetVTClass_t *)pvPropVal;

		while(NULL != pstInVTClass && (NULL != pstVTClass || bFlag))
		{
			if(bFlag)
			{
				/* allocate memory */
				pstVTClass = OSAL_Malloc(sizeof(Pr_BACnetVTClass_t),
					__FILE__,__FUNCTION__,__LINE__);
				pstBaseNode = pstVTClass;
				/* reset flag */
				bFlag = FALSE;
			}
			else
			{
				/* allocate memory */
				pstVTClass->m_pstNext = OSAL_Malloc(sizeof(Pr_BACnetVTClass_t),
					__FILE__,__FUNCTION__,__LINE__);
				/* move to new node */
				pstVTClass = pstVTClass->m_pstNext;
			}

			if(NULL != pstVTClass)
			{
				/* copy the data */
				pstVTClass->m_eVTClassSupport = pstInVTClass->m_eVTClassSupport;
				pstVTClass->m_pstNext = NULL;
			}
			else
			{
				/* free allocated memories */
				Clear_VtClass_List(&pstBaseNode);
				pstBaseNode = NULL;
				eRetVal = BACDEL_MALLOC_ERROR; 
				break;
			}

			/* move to next node */
			pstInVTClass = pstInVTClass->m_pstNext;
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBaseNode;
	}
	break;

	case BACNET_DT_BACNETVTSESS:
	{
		/* local variables */
		Pr_ListOfBACnetVTSession_t *pstVTSess = NULL;
		Pr_ListOfBACnetVTSession_t *pstInVTSess = NULL;
		ListOfBACnetVTSession_t *pstTempVTSessionList = NULL;
		ListOfBACnetVTSession_t *pstInTempVTSessionList = NULL;

		/* copy to local buffer */
		pstInVTSess = (Pr_ListOfBACnetVTSession_t *)pvPropVal;
		pstInTempVTSessionList = pstInVTSess->m_pstListOfVTSession;

		/* allocate memory to save list */
		pstVTSess = OSAL_Malloc(sizeof(Pr_ListOfBACnetVTSession_t),
					__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstVTSess)
		{
			while(NULL != pstInTempVTSessionList && (NULL != pstTempVTSessionList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstVTSess->m_pstListOfVTSession = OSAL_Malloc(sizeof(ListOfBACnetVTSession_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* reset flag */
					bFlag = FALSE;
					pstTempVTSessionList = pstVTSess->m_pstListOfVTSession;
				}
				else
				{
					/* allocate memory */
					pstTempVTSessionList->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetVTSession_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstTempVTSessionList = pstTempVTSessionList->m_pstNext;
				}

				if(NULL != pstTempVTSessionList)
				{
					/* copy the data */
					memcpy(pstTempVTSessionList, pstInTempVTSessionList, sizeof(ListOfBACnetVTSession_t));
					pstTempVTSessionList->m_pstNext = NULL;

					/* increment counter */
					pstVTSess->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_VtSessions_List(&pstVTSess->m_pstListOfVTSession);
					OSAL_Free(pstVTSess, __FILE__,__FUNCTION__,__LINE__);
					pstVTSess = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInTempVTSessionList = pstInTempVTSessionList->m_pstNext;
			}
		}
		else 
		{
			eRetVal = BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstVTSess;
	}
	break;

	case BACNET_DT_ADDRESSBINDING_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetAddrBinding_t *pstAddBind = NULL;
		Pr_ListOfBACnetAddrBinding_t *pstInAddBind = NULL;
		BACnetAddrBinding_t *pstTempAddBind = NULL; 
		BACnetAddrBinding_t *pstInTempAddBind = NULL;

		/* copy to local buffer */
		pstInAddBind = (Pr_ListOfBACnetAddrBinding_t *)pvPropVal;
		pstInTempAddBind = pstInAddBind->m_stAddBinding.pstNext;

		/* allocate memory to save address binding pointer */
		pstAddBind = OSAL_Malloc(sizeof(Pr_ListOfBACnetAddrBinding_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstAddBind)
		{
			/* copy the first node */
			memcpy(&pstAddBind->m_stAddBinding, &pstInAddBind->m_stAddBinding, sizeof(BACnetAddrBinding_t));
			pstAddBind->m_u32Count++;
			pstTempAddBind = &pstAddBind->m_stAddBinding;

			while(NULL != pstInTempAddBind && NULL != pstTempAddBind)
			{
				/* allocate memory for next node */
				pstTempAddBind->pstNext = OSAL_Malloc(sizeof(BACnetAddrBinding_t),
						__FILE__,__FUNCTION__,__LINE__);
				
				if(NULL != pstTempAddBind->pstNext)
				{
					/* move to new node */
					pstTempAddBind = pstTempAddBind->pstNext;
					/* copy data */
					memcpy(pstTempAddBind, pstInTempAddBind, sizeof(BACnetAddrBinding_t));
					pstTempAddBind->pstNext = NULL;

					/* increment the count */
					pstAddBind->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_AddressBinding_List(&pstAddBind->m_stAddBinding.pstNext, false);
					OSAL_Free(pstAddBind, __FILE__,__FUNCTION__,__LINE__);
					pstAddBind = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInTempAddBind = pstInTempAddBind->pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstAddBind;
	}
	break;

	case BACNET_DT_BOOLEAN_ARRAY:
	{
		/* local variables */
		Pr_ListOfBoolen_t *pstBoolArr = NULL;
		Pr_ListOfBoolen_t *pstInBoolArr = NULL;
		ListOfBoolen_t *pstBoolList = NULL;
		ListOfBoolen_t *pstInBoolList = NULL;

		/* copy to local buffer */
		pstInBoolArr = (Pr_ListOfBoolen_t *)pvPropVal;
		pstInBoolList = pstInBoolArr->m_pstBoolList;

		/* allocate memory to save boolean array pointer */
		pstBoolArr = OSAL_Malloc(sizeof(Pr_ListOfBoolen_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstBoolArr)
		{
			while(NULL != pstInBoolList && (NULL != pstBoolList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstBoolArr->m_pstBoolList = OSAL_Malloc(
						sizeof(ListOfBoolen_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstBoolList = pstBoolArr->m_pstBoolList; 
				}
				else
				{
					/* allocate memory */
					pstBoolList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfBoolen_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstBoolList = pstBoolList->m_pstNext;
				}

				if(NULL != pstBoolList)
				{
					/* copy the data */
					pstBoolList->m_bVal = pstInBoolList->m_bVal;
					pstBoolList->m_pstNext = NULL;

					/* increment the count */
					pstBoolArr->m_u32BoolCount++;
				}
				else
				{
					/* free allocated memories */
					Clear_Boolean_List(&pstBoolArr->m_pstBoolList);
					OSAL_Free(pstBoolArr, __FILE__,__FUNCTION__,__LINE__);
					pstBoolArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInBoolList = pstInBoolList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBoolArr;
	}
	break;

	case BACNET_DT_BITSTRING_ARRAY:
	{
		/* local variable */
		Pr_ListOfBitStr_t *pstStrList = NULL;
		Pr_ListOfBitStr_t *pstInStrList = NULL;
		ListOfBitStr_t * pstBitStrList = NULL;
		ListOfBitStr_t * pstInBitStrList = NULL;

		/* copy to local buffer */
		pstInStrList = (Pr_ListOfBitStr_t *)pvPropVal;
		pstInBitStrList = pstInStrList->m_pstBitString;

		/* allocate memory to save bit string array pointer */
		pstStrList = OSAL_Malloc(sizeof(Pr_ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstStrList)
		{
			while(NULL != pstInBitStrList && (NULL != pstBitStrList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstStrList->m_pstBitString = OSAL_Malloc(
						sizeof(ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstBitStrList = pstStrList->m_pstBitString; 
				}
				else
				{
					/* allocate memory */
					pstBitStrList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstBitStrList = pstBitStrList->m_pstNext;
				}

				if(NULL != pstBitStrList)
				{
					/* copy the data */
					memcpy(pstBitStrList, pstInBitStrList, sizeof(ListOfBitStr_t));
					pstBitStrList->m_pstNext = NULL;

					/* increment the count */
					pstStrList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_BitString_List(&pstStrList->m_pstBitString);
					OSAL_Free(pstStrList, __FILE__,__FUNCTION__,__LINE__);
					pstStrList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInBitStrList = pstInBitStrList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstStrList;
	}
	break;

	case BACNET_DT_ENUM_LIST:
	{
		/* local variables */
		Pr_ListOfEnum_t *pstBacEnumList = NULL;
		Pr_ListOfEnum_t *pstInBacEnumList = NULL;
		ListOfEnum_t *pstEnumList = NULL;
		ListOfEnum_t *pstInEnumList = NULL;

		/* copy to local buffer */
		pstInBacEnumList = (Pr_ListOfEnum_t *)pvPropVal;   //received list contains count and list
		pstInEnumList = pstInBacEnumList->m_pstEnumList;    // assign received list 

		/* allocate memory to save charstring array pointer */
		pstBacEnumList = OSAL_Malloc(sizeof(Pr_ListOfEnum_t),__FILE__,__FUNCTION__,__LINE__); //create list
		if(NULL != pstBacEnumList)
		{
			while(NULL != pstInEnumList && (NULL != pstEnumList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstBacEnumList->m_pstEnumList = OSAL_Malloc(
						sizeof(ListOfEnum_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstEnumList = pstBacEnumList->m_pstEnumList; 
				}
				else
				{
					/* allocate memory */
					pstEnumList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfEnum_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstEnumList = pstEnumList->m_pstNext;
				}

				if(NULL != pstEnumList)
				{
					/* copy the data */
					pstEnumList->m_eVal = pstInEnumList->m_eVal;
					pstEnumList->m_pstNext = NULL;

					/* increment the count */
					pstBacEnumList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_Enumeration_List(&pstBacEnumList->m_pstEnumList);
					OSAL_Free(pstBacEnumList, __FILE__,__FUNCTION__,__LINE__);
					pstBacEnumList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInEnumList = pstInEnumList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBacEnumList;
	}
	break;

	case BACNET_DT_RECIPIENT_LIST:
	{
		/* local variables */
		Pr_ListOfBACnetRecipient_t *pstRecipient = NULL;
		Pr_ListOfBACnetRecipient_t *pstInRecipient = NULL;
		ListOfBACnetRecipient_t *pstTmpRecipient = NULL;
		ListOfBACnetRecipient_t *pstInTmpRecipient = NULL;
		
		/* copy to local buffer */
		pstInRecipient = (Pr_ListOfBACnetRecipient_t *)pvPropVal;
		pstInTmpRecipient = pstInRecipient->m_pstListOfRecipient;

		/* allocate memory to save charstring array pointer */
		pstRecipient = OSAL_Malloc(sizeof(Pr_ListOfBACnetRecipient_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstRecipient)
		{
			while(NULL != pstInTmpRecipient && (NULL != pstTmpRecipient || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstRecipient->m_pstListOfRecipient = OSAL_Malloc(sizeof(ListOfBACnetRecipient_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* reset flag */
					bFlag = FALSE;
					pstTmpRecipient = pstRecipient->m_pstListOfRecipient;
				}
				else
				{
					/* allocate memory */
					pstTmpRecipient->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetRecipient_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstTmpRecipient = pstTmpRecipient->m_pstNext;
				}

				if(NULL != pstTmpRecipient)
				{
					/* copy the data */
					memcpy(pstTmpRecipient, pstInTmpRecipient, sizeof(ListOfBACnetRecipient_t));
					pstTmpRecipient->m_pstNext = NULL;

					/* increment the count */
					pstRecipient->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_Recipient_List(&pstRecipient->m_pstListOfRecipient);
					OSAL_Free(pstRecipient, __FILE__,__FUNCTION__,__LINE__);
					pstRecipient = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInTmpRecipient = pstInTmpRecipient->m_pstNext;
			}
		}

		/* save the pointer to be returned */
		pvData = (void *)pstRecipient;
	}
	break;

	case BACNET_DT_DATELIST:
	{
		/* local variables */
		Pr_ListOfBACnetCalendarEntry_t *pstDateList = NULL;
		Pr_ListOfBACnetCalendarEntry_t *pstInDateList = NULL;
		ListOfBACnetCalendarEntry_t *pstCalendarList = NULL;
		ListOfBACnetCalendarEntry_t *pstInCalendarList = NULL;

		/* copy to local buffer */
		pstInDateList = (Pr_ListOfBACnetCalendarEntry_t *)pvPropVal;
		pstInCalendarList = pstInDateList->m_pstListOfCalendar;

		/* allocate memory to save date list pointer */
		pstDateList = OSAL_Malloc(sizeof(Pr_ListOfBACnetCalendarEntry_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstDateList)
		{
			while(NULL != pstInCalendarList && (NULL != pstCalendarList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstDateList->m_pstListOfCalendar = OSAL_Malloc(
						sizeof(ListOfBACnetCalendarEntry_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstCalendarList = pstDateList->m_pstListOfCalendar; 
				}
				else
				{
					/* allocate memory */
					pstCalendarList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfBACnetCalendarEntry_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstCalendarList = pstCalendarList->m_pstNext;
				}

				if(NULL != pstCalendarList)
				{
					/* copy the node data */
					memcpy(pstCalendarList, pstInCalendarList, sizeof(ListOfBACnetCalendarEntry_t));
					pstCalendarList->m_pstNext = NULL;

					/* increment the count */
					pstDateList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_CalendarEntry_List(&pstDateList->m_pstListOfCalendar);
					OSAL_Free(pstDateList, __FILE__,__FUNCTION__,__LINE__);
					pstDateList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInCalendarList = pstInCalendarList->m_pstNext;
			}
			/* save the count */
			//pstDateList->m_u32Count = u32Count;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstDateList;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_DAILYSCHEDULE:
	{
		/* local variables */
		Pr_BACnetDailySchedule_t *pstWeekSchedule = NULL;
		Pr_BACnetDailySchedule_t *pstInWeekSchedule = NULL;
		BACnetTimeValue_t *pstTimeValue = NULL;
		BACnetTimeValue_t *pstInTimeValue = NULL;

		/* copy to local buffer */
		pstInWeekSchedule = (Pr_BACnetDailySchedule_t *)pvPropVal;

		/* allocate memory to save weekly schedule pointer */
		pstWeekSchedule = OSAL_Malloc(sizeof(Pr_BACnetDailySchedule_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstWeekSchedule)
		{
			/* copy data if flag is true */
			if(pstInWeekSchedule->m_stTimeValue.bIsUsed)
			{
				/* set the flag */
				pstWeekSchedule->m_stTimeValue.bIsUsed = TRUE;				

				/* copy 1st node data */
				memcpy(&pstWeekSchedule->m_stTimeValue.m_stPropValue,
					&pstInWeekSchedule->m_stTimeValue.m_stPropValue,
					sizeof(BACNET_PROPERTY_VALUE));
				memcpy(&pstWeekSchedule->m_stTimeValue.m_stTime,
					&pstInWeekSchedule->m_stTimeValue.m_stTime,
					sizeof(BACnetTime_t));

				/* 1st pointer of output */
				pstTimeValue = &pstWeekSchedule->m_stTimeValue;
				/* 1st pointer of input */
				pstInTimeValue = pstInWeekSchedule->m_stTimeValue.m_pstNext;

				while(NULL != pstInTimeValue && NULL != pstTimeValue)
				{
					/* allocate memory */
					pstTimeValue->m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),
						__FILE__,__FUNCTION__,__LINE__);
					if(NULL != pstTimeValue->m_pstNext)
					{
						/* move to new node */
						pstTimeValue = pstTimeValue->m_pstNext;
						/* copy data */
						memcpy(&pstTimeValue->m_stPropValue, &pstInTimeValue->m_stPropValue,
							sizeof(BACNET_PROPERTY_VALUE));
						memcpy(&pstTimeValue->m_stTime, &pstInTimeValue->m_stTime,
							sizeof(BACnetTime_t));
						pstTimeValue->m_pstNext = NULL;
					}
					else
					{
						/* free allocated memories */
						Clear_TimeValues_List(&pstWeekSchedule->m_stTimeValue.m_pstNext);
						memset(&pstWeekSchedule->m_stTimeValue, 0, sizeof(BACnetTimeValue_t));
						OSAL_Free(pstWeekSchedule, __FILE__,__FUNCTION__,__LINE__);
						pstWeekSchedule = NULL;
						eRetVal = BACDEL_MALLOC_ERROR; 						
						break;
					}

					/* move to next node */
					pstInTimeValue = pstInTimeValue->m_pstNext;
				}
			}			
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstWeekSchedule;
	}
	break;

	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	{
		/* local variables */
		Pr_ListOfBACnetDailySchedule_t *pstWeekSchedule = NULL;
		Pr_ListOfBACnetDailySchedule_t *pstInWeekSchedule = NULL;
		BACnetTimeValue_t *pstTimeValue = NULL;
		BACnetTimeValue_t *pstInTimeValue = NULL;
		uint8_t u8Count = 0;

		/* copy to local buffer */
		pstInWeekSchedule = (Pr_ListOfBACnetDailySchedule_t *)pvPropVal;

		/* allocate memory to save weekly schedule pointer */
		pstWeekSchedule = OSAL_Malloc(sizeof(Pr_ListOfBACnetDailySchedule_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstWeekSchedule)
		{
			for(u8Count = 0; u8Count < BACNET_ARRAY_OF_SEVEN; u8Count++)
			{
				/* copy data if flag is true */
				if(pstInWeekSchedule->m_stTimeValue[u8Count].bIsUsed)
				{
					/* set the flag */
					pstWeekSchedule->m_stTimeValue[u8Count].bIsUsed = TRUE;

					/* 1st pointer of input */
					pstInTimeValue = pstInWeekSchedule->m_stTimeValue[u8Count].m_pstNext;

					/* copy 1st node data */
					memcpy(&pstWeekSchedule->m_stTimeValue[u8Count].m_stPropValue,
						&pstInWeekSchedule->m_stTimeValue[u8Count].m_stPropValue,
						sizeof(BACNET_PROPERTY_VALUE));
					memcpy(&pstWeekSchedule->m_stTimeValue[u8Count].m_stTime,
						&pstInWeekSchedule->m_stTimeValue[u8Count].m_stTime,
						sizeof(BACnetTime_t));

					/* 1st pointer of output */
					pstTimeValue = &pstWeekSchedule->m_stTimeValue[u8Count];

					while(NULL != pstInTimeValue && NULL != pstTimeValue)
					{
						/* allocate memory */
						pstTimeValue->m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),
							__FILE__,__FUNCTION__,__LINE__);
						if(NULL != pstTimeValue->m_pstNext)
						{
							/* move to new node */
							pstTimeValue = pstTimeValue->m_pstNext;
							/* copy data */
							memcpy(&pstTimeValue->m_stPropValue, &pstInTimeValue->m_stPropValue,
								sizeof(BACNET_PROPERTY_VALUE));
							memcpy(&pstTimeValue->m_stTime, &pstInTimeValue->m_stTime,
								sizeof(BACnetTime_t));
							pstTimeValue->m_pstNext = NULL;
						}
						else
						{
							/* free allocated memories */
							Clear_WeeklyShdl_List(pstWeekSchedule);
							OSAL_Free(pstWeekSchedule, __FILE__,__FUNCTION__,__LINE__);
							pstWeekSchedule = NULL;
							eRetVal = BACDEL_MALLOC_ERROR; 
							/* set count to exit outer loop */
							u8Count = BACNET_ARRAY_OF_SEVEN;
							break;
						}

						/* move to next node */
						pstInTimeValue = pstInTimeValue->m_pstNext;
					}
				}
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstWeekSchedule;

	}
	break;

	case BACNET_DT_SPECIALEVENT_ARRAY:
	case BACNET_DT_SPECIALEVENT:
	{
		/* local variables */
		Pr_ListOfBACnetSpecialEvent_t *pstExceSchedule = NULL;
		Pr_ListOfBACnetSpecialEvent_t *pstInExceSchedule = NULL;
		ListOfSpecialEvent_t *pstEventList = NULL;
		ListOfSpecialEvent_t *pstInEventList = NULL;
		BACnetTimeValue_t *pstTimeValue = NULL;
		BACnetTimeValue_t *pstInTimeValue = NULL;

		/* copy to local buffer */
		pstInExceSchedule = (Pr_ListOfBACnetSpecialEvent_t *)pvPropVal;
		pstInEventList = pstInExceSchedule->m_pstSplEvent;

		/* allocate memory to save charstring array pointer */
		pstExceSchedule = OSAL_Malloc(sizeof(Pr_ListOfBACnetSpecialEvent_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstExceSchedule)
		{
			while(NULL != pstInEventList && (NULL != pstEventList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstExceSchedule->m_pstSplEvent = OSAL_Malloc(
						sizeof(ListOfSpecialEvent_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstEventList = pstExceSchedule->m_pstSplEvent; 
				}
				else
				{
					/* allocate memory */
					pstEventList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfSpecialEvent_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstEventList = pstEventList->m_pstNext;
				}

				if(NULL != pstEventList)
				{
					/* copy the node data */
					memcpy(pstEventList, pstInEventList, sizeof(ListOfSpecialEvent_t));
					pstEventList->m_pstNext = NULL;

					/* 1st pointer of input */
					pstInTimeValue = pstEventList->m_stListSpecialEvent.m_stListOfTimeValues.m_pstNext;
					/* 1st pointer of output */
					pstTimeValue = &pstEventList->m_stListSpecialEvent.m_stListOfTimeValues;

					/* increment the count */
					pstExceSchedule->m_u32Count++;

					while(NULL != pstInTimeValue && NULL != pstTimeValue)
					{
						/* allocate memory */
						pstTimeValue->m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),
							__FILE__,__FUNCTION__,__LINE__);
						if(NULL != pstTimeValue->m_pstNext)
						{
							/* move to new node */
							pstTimeValue = pstTimeValue->m_pstNext;
							/* copy data */
							memcpy(&pstTimeValue->m_stPropValue, &pstInTimeValue->m_stPropValue,
								sizeof(BACNET_PROPERTY_VALUE));
							memcpy(&pstTimeValue->m_stTime, &pstInTimeValue->m_stTime,
								sizeof(BACnetTime_t));
							pstTimeValue->m_pstNext = NULL;
						}
						else
						{
							/* free allocated memories */
							Clear_ExceptionShdl_List(&pstExceSchedule->m_pstSplEvent);
							OSAL_Free(pstExceSchedule, __FILE__,__FUNCTION__,__LINE__);
							pstExceSchedule = NULL;
							eRetVal = BACDEL_MALLOC_ERROR; 
							/* set flag to exit outer loop */
							bBrkLoop = TRUE;
							break;
						}

						/* move to next node */
						pstInTimeValue = pstInTimeValue->m_pstNext;
					}
				}
				else
				{
					/* free allocated memories */
					Clear_ExceptionShdl_List(&pstExceSchedule->m_pstSplEvent);
					OSAL_Free(pstExceSchedule, __FILE__,__FUNCTION__,__LINE__);
					pstExceSchedule = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* break loop if flag is set */
				if(bBrkLoop)
				{
					break;
				}

				/* break if single node */
				if(BACNET_DT_SPECIALEVENT == eDataType)
				{
					break;
				}
				/* move to next node */
				pstInEventList = pstInEventList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstExceSchedule;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_COVSUB:
	{
		/* local variables */
		Pr_ListOfBACnetCovSubs_t *pstActCovList = NULL;
		Pr_ListOfBACnetCovSubs_t *pstInActCovList = NULL;
		ListOfBACnetCovSubs_t *pstCOVSub = NULL;
		ListOfBACnetCovSubs_t *pstInCOVSub = NULL;

		/* copy to local buffer */
		pstInActCovList = (Pr_ListOfBACnetCovSubs_t *)pvPropVal;
		pstInCOVSub = pstInActCovList->m_pstCOVSubscribe;

		/* allocate memory to save charstring array pointer */
		pstActCovList = OSAL_Malloc(sizeof(Pr_ListOfBACnetCovSubs_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstActCovList)
		{
			while(NULL != pstInCOVSub && (NULL != pstCOVSub || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory to save first node once */
					pstActCovList->m_pstCOVSubscribe = OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t),
						__FILE__,__FUNCTION__,__LINE__);
					bFlag = FALSE;
					pstCOVSub = pstActCovList->m_pstCOVSubscribe;
				}
				else
				{
					/* allocate memory */
					pstCOVSub->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t),
						__FILE__,__FUNCTION__,__LINE__);
					pstCOVSub = pstCOVSub->m_pstNext;
				}

				if(NULL != pstCOVSub)
				{
					/* copy data */
					memcpy(pstCOVSub, pstInCOVSub, sizeof(ListOfBACnetCovSubs_t));
					pstCOVSub->m_pstNext = NULL;
					pstCOVSub->m_pstSubscriberInfo = NULL;
					pstCOVSub->m_stCOVPropertyElem.m_pstNext = NULL;
					pstCOVSub->m_stCOVPropertyElem.m_pvCurrentValue = NULL;
					pstCOVSub->m_stCOVPropertyElem.m_pvStoredValue = NULL;

					/* copy COV recipient list */
					eRetVal = Copy_COVRecipeint_Info(&pstCOVSub->m_pstSubscriberInfo, pstInCOVSub->m_pstSubscriberInfo);
					if(BACDEL_SUCCESS != eRetVal)
					{
						/* free allocated memories */
						Clear_ActiveCovSubs_List(&pstActCovList->m_pstCOVSubscribe);
						OSAL_Free(pstActCovList, __FILE__,__FUNCTION__,__LINE__);
						pstActCovList = NULL;
						eRetVal = BACDEL_MALLOC_ERROR; 
						break;
					}

					/* increment the count */
					pstActCovList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_ActiveCovSubs_List(&pstActCovList->m_pstCOVSubscribe);
					OSAL_Free(pstActCovList, __FILE__,__FUNCTION__,__LINE__);
					pstActCovList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInCOVSub = pstInCOVSub->m_pstNext;
			}
			//pstActCovList->m_u32Count = u32Count;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstActCovList;
	}
	break;

	case BACNET_DT_AUTHENTICATION_FACTOR:
	{
		/* local variables */
		Pr_BACnetAuFactor_t *pstAutheFact = NULL;
		Pr_BACnetAuFactor_t *pstInAutheFact = NULL;

		/* copy to local buffer */
		pstInAutheFact = (Pr_BACnetAuFactor_t *)pvPropVal;

		/* allocate memory to save Authentication factor pointer*/
		pstAutheFact = OSAL_Malloc(sizeof
			(Pr_BACnetAuFactor_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstAutheFact)
		{
			/* copy data */
			memcpy(&pstAutheFact->m_stAuFactor, &pstInAutheFact->m_stAuFactor, 
				sizeof(BACnetAuFactor_t));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstAutheFact;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_LOGBUFFER_TRENDLOG:
	{
		/* local variables */
		Pr_ListOfBACnetLogRecord_t *pstTrendLogBuffList = NULL;
		Pr_ListOfBACnetLogRecord_t *pstInTrendLogBuffList = NULL;
		ListOfBACnetLogRecord_t *pstTrendLogBuff = NULL;
		ListOfBACnetLogRecord_t *pstInTrendLogBuff = NULL;

		/* copy to local buffer */
		pstInTrendLogBuffList = (Pr_ListOfBACnetLogRecord_t *)pvPropVal;
		pstInTrendLogBuff = pstInTrendLogBuffList->m_pstLogRecord;

		/* allocate memory to save Trend Log Buffer List pointer */
		pstTrendLogBuffList = OSAL_Malloc(sizeof(Pr_ListOfBACnetLogRecord_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstTrendLogBuffList)
		{
		  	while(NULL != pstInTrendLogBuff && (NULL != pstTrendLogBuff || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
			  		pstTrendLogBuffList->m_pstLogRecord= OSAL_Malloc(sizeof(ListOfBACnetLogRecord_t),
						__FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstTrendLogBuff = pstTrendLogBuffList->m_pstLogRecord;
				}
				else
			  	{
			  		/* allocate memory */
			  		pstTrendLogBuff->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetLogRecord_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstTrendLogBuff = pstTrendLogBuff->m_pstNext;
			   	}
               
				if(NULL != pstTrendLogBuff)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstTrendLogBuff, pstInTrendLogBuff, sizeof(ListOfBACnetLogRecord_t));
					pstTrendLogBuff->m_pstNext = NULL;

					/* increment the count */
					pstTrendLogBuffList->m_u32Count++;
				}
				else
		  	   	{
		  	   		/* free allocated memories */
					Clear_TrendLogRecord_List(&pstTrendLogBuffList->m_pstLogRecord);
					OSAL_Free(pstTrendLogBuffList, __FILE__,__FUNCTION__,__LINE__);
					pstTrendLogBuffList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

				/* move to next node */
				pstInTrendLogBuff = pstInTrendLogBuff->m_pstNext;
			  }
			 /* save the count */
			 //pstTrendLogBuffList->m_u32Count = u32Count;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstTrendLogBuffList;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_ACTION_LIST_ARRAY:
	case BACNET_DT_ACTION_LIST:
	{
		/* local variable */
		bool bInsideFlag = TRUE;
		bool bErrorFlag = FALSE;
		Pr_ListOfBACnetActionList_t *pstActList = NULL;
		Pr_ListOfBACnetActionList_t *pstInActList = NULL;
		ListOfBACnetActionList_t *pstActionList = NULL;
		ListOfBACnetActionList_t *pstInActionList = NULL;
		ListOfBACnetActionCommand_t *pstActCmdList = NULL;
		ListOfBACnetActionCommand_t *pstInActCmdList = NULL;

		/* copy to local buffer */
		pstInActList = (Pr_ListOfBACnetActionList_t *)pvPropVal;
		pstInActionList = pstInActList->m_pstArrayActCmd;

		/* allocate memory to save action list array pointer */
		pstActList = OSAL_Malloc(sizeof(Pr_ListOfBACnetActionList_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstActList)
		{
			while(NULL != pstInActionList && (NULL != pstActionList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstActList->m_pstArrayActCmd = OSAL_Malloc(
						sizeof(ListOfBACnetActionList_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstActionList = pstActList->m_pstArrayActCmd; 
				}
				else
				{
					/* allocate memory */
					pstActionList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfBACnetActionList_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstActionList = pstActionList->m_pstNext;
				}

				if(NULL != pstActionList)
				{
					/* copy the data */
					pstInActCmdList = pstInActionList->m_pstActionCommand;
					while(NULL != pstInActCmdList && (NULL != pstActCmdList || bInsideFlag))
					{
						if(bInsideFlag)
						{
							/* allocate memory */
							pstActionList->m_pstActionCommand = OSAL_Malloc(
								sizeof(ListOfBACnetActionCommand_t),__FILE__,__FUNCTION__,__LINE__);
							/* reset flag & new node address */
							bInsideFlag = FALSE;
							pstActCmdList = pstActionList->m_pstActionCommand; 
						}
						else
						{
							/* allocate memory */
							pstActCmdList->m_pstNext = OSAL_Malloc(
								sizeof(ListOfBACnetActionCommand_t),__FILE__,__FUNCTION__,__LINE__);
							/* move to new node */
							pstActCmdList = pstActCmdList->m_pstNext;
						}

						if(NULL != pstActCmdList)
						{
							/* copy data */
		  	   				memcpy(pstActCmdList, pstInActCmdList, sizeof(ListOfBACnetActionCommand_t));
							pstActCmdList->m_stPropPointer.m_pvConstrProp = NULL;
							Copy_PropVal_AsPer_DataType(pstInActCmdList->m_stPropPointer.m_pvConstrProp,
								pstInActCmdList->m_stPropPointer.m_eData_Type,
								&pstActCmdList->m_stPropPointer.m_pvConstrProp);
							pstActCmdList->m_pstNext = NULL;
						}
						else
						{
							/* free allocated memories */
							Clear_Action_List(&pstActList->m_pstArrayActCmd);
							OSAL_Free(pstActList, __FILE__,__FUNCTION__,__LINE__);
							pstActList = NULL;
							eRetVal = BACDEL_MALLOC_ERROR; 
							bErrorFlag = TRUE;
							break;
						}

						/* move to next node */
						pstInActCmdList = pstInActCmdList->m_pstNext;
					}// inner while loop

					if(bErrorFlag)
					{
						/* exit from outer while loop */
						break;
					}
					
					/* copy the data */
					pstActionList->m_bTerminateList = pstInActionList->m_bTerminateList;
					pstActionList->m_pstNext = NULL;

					/* increment the count */
					pstActList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_Action_List(&pstActList->m_pstArrayActCmd);
					OSAL_Free(pstActList, __FILE__,__FUNCTION__,__LINE__);
					pstActList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* break if single node */
				if(BACNET_DT_ACTION_LIST == eDataType)
				{
					break;
				}

				/* move to next node */
				pstInActionList = pstInActionList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstActList;
	}
	break;

	case BACNET_DT_READACCESSSPECS_LIST:
	{
		/* local variables */
		Pr_ListOfReadAccessSpecs_t *pstReadAccSpecif = NULL;
		Pr_ListOfReadAccessSpecs_t *pstInReadAccSpecif = NULL;
		ListOfReadAccessSpecs_t *pstReadAccSpecList = NULL;
		ListOfReadAccessSpecs_t *pstInReadAccSpecList = NULL;

		/* copy to local buffer */
		pstInReadAccSpecif = (Pr_ListOfReadAccessSpecs_t *)pvPropVal;
		pstInReadAccSpecList = pstInReadAccSpecif->m_pstGpOfObj;

		/* allocate memory to save Read Access Specification array pointer */
		pstReadAccSpecif = OSAL_Malloc(sizeof
			(Pr_ListOfReadAccessSpecs_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstReadAccSpecif)
		{
			while(NULL != pstInReadAccSpecList && (NULL != pstReadAccSpecList || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
		  	  		/* allocate memory */
		  	  		pstReadAccSpecif->m_pstGpOfObj = OSAL_Malloc(sizeof(ListOfReadAccessSpecs_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	  		/* reset flag & new node address */
		  	  		bFlag = false;
		  	  		pstReadAccSpecList = pstReadAccSpecif->m_pstGpOfObj;
		  	  	}
		  	  	else
		  	  	{
		  	  		/* allocate memory */
		  	  		pstReadAccSpecList->m_pstNext = OSAL_Malloc(sizeof(ListOfReadAccessSpecs_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	  		/* move to new node */
		  	  		pstReadAccSpecList = pstReadAccSpecList->m_pstNext;
		  	  	}
             
		  		if(NULL != pstReadAccSpecList)
		  		{
		  			/* copy data */
		  			memcpy(pstReadAccSpecList, pstInReadAccSpecList, sizeof(ListOfReadAccessSpecs_t));
		  			pstReadAccSpecList->m_pstNext = NULL;
					// TODO - copy internal list
					pstReadAccSpecList->m_stlistOfPropRef.m_pstNext = NULL;
            
		  			/* increment the count */
		  			pstReadAccSpecif->m_u32Count++;
		  		}
		  		else
		  		{
		  			/* free allocated memories */
					Clear_ReadAccessSpecs_List(&pstReadAccSpecif->m_pstGpOfObj);
					OSAL_Free(pstReadAccSpecif, __FILE__,__FUNCTION__,__LINE__);
					pstReadAccSpecif = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  		}
            
		  		/* move to next node */
		  		pstInReadAccSpecList = pstInReadAccSpecList->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstReadAccSpecif;
	}
	break;

	case BACNET_DT_READACCESSRESULT_LIST:
	{
		/* local variables */
		Pr_ListOfReadAccessResult_t *pstAccResList = NULL;
		Pr_ListOfReadAccessResult_t *pstInAccResList = NULL;
		ListOfReadAccessResult_t *pstReadAcceRes = NULL;
		ListOfReadAccessResult_t *pstInReadAcceRes = NULL;

		/* copy to local buffer */
		pstInAccResList = (Pr_ListOfReadAccessResult_t *)pvPropVal;
		pstInReadAcceRes = pstInAccResList->m_pstGpObjResults;

		/* allocate memory to save Read Access Result List pointer */
		pstAccResList = OSAL_Malloc(sizeof
			(Pr_ListOfReadAccessResult_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstAccResList)
		{
			while(NULL != pstInReadAcceRes && (NULL != pstReadAcceRes || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstAccResList->m_pstGpObjResults = OSAL_Malloc(sizeof(ListOfReadAccessResult_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & save node address */
					bFlag = false;
					pstReadAcceRes = pstAccResList->m_pstGpObjResults;
				}
				else
				{
					/* allocate memory */
					pstReadAcceRes->m_pstNext = OSAL_Malloc(sizeof(ListOfReadAccessResult_t),
						__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstReadAcceRes = pstReadAcceRes->m_pstNext;
				}

				if(NULL != pstReadAcceRes)
				{
					/* copy data */
					memcpy(pstReadAcceRes, pstInReadAcceRes, sizeof(ListOfReadAccessResult_t));
					pstReadAcceRes->m_pstNext = NULL;
					// TODO - copy internal list
					pstReadAcceRes->m_stListOfResults.m_pstNext = NULL;

					/* increment the count */
					pstAccResList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_ReadAccessResult_List(&pstAccResList->m_pstGpObjResults);
					OSAL_Free(pstAccResList, __FILE__,__FUNCTION__,__LINE__);
					pstAccResList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
				}

				/* move to next node */
				pstInReadAcceRes = pstInReadAcceRes->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstAccResList;
	}
	break;

	case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:
	case BACNET_DT_AU_FACTOR_FORMAT:
	{
		/* local variables */
		Pr_ListOfBACnetAuFactorFormat_t *pstAutheFactFormArr = NULL;
		Pr_ListOfBACnetAuFactorFormat_t *pstInAutheFactFormArr = NULL;
		ListOfBACnetAuFactorFormat_t *pstAutheFactForm = NULL;
		ListOfBACnetAuFactorFormat_t *pstInAutheFactForm = NULL;

		/* copy to local buffer */
		pstInAutheFactFormArr = (Pr_ListOfBACnetAuFactorFormat_t *)pvPropVal;
		pstInAutheFactForm = pstInAutheFactFormArr->m_pstAuFactFormatList;

		/* allocate memory to save Authentication factor Format array pointer */
		pstAutheFactFormArr = OSAL_Malloc(sizeof
			(Pr_ListOfBACnetAuFactorFormat_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstAutheFactFormArr)
		{
		  	while(NULL != pstInAutheFactForm && (NULL != pstAutheFactForm || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
					/* allocate memory */
					pstAutheFactFormArr->m_pstAuFactFormatList = OSAL_Malloc(
						sizeof(ListOfBACnetAuFactorFormat_t),__FILE__,__FUNCTION__,
						__LINE__);
					/* reset flag & new node address */
		  	  		bFlag = false;
		  	  		pstAutheFactForm = pstAutheFactFormArr->m_pstAuFactFormatList;
		  	  	}
		  	  	else
		  	  	{
		  	  		/* allocate memory */
					pstAutheFactForm->m_pstNext = OSAL_Malloc(sizeof
		  	  			(ListOfBACnetAuFactorFormat_t),__FILE__,__FUNCTION__,__LINE__);
		  	  		/* move to new node */
					pstAutheFactForm = pstAutheFactForm->m_pstNext;
				}

				if(NULL != pstAutheFactForm)
		  		{
		  			/* copy data */
		  			memcpy(pstAutheFactForm, pstInAutheFactForm, 
		  				sizeof(ListOfBACnetAuFactorFormat_t));
					pstAutheFactForm->m_pstNext = NULL;

					/* increment the count */
		  			pstAutheFactFormArr->m_u32ArraySize++;
		  		}
		  		else
		  		{
		  			/* free allocated memories */
					Clear_AuFactorFormat_List(&pstAutheFactFormArr->m_pstAuFactFormatList);
					OSAL_Free(pstAutheFactFormArr, __FILE__,__FUNCTION__,__LINE__);
					pstAutheFactFormArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  		}

				/* break if single node */
				if(BACNET_DT_AU_FACTOR_FORMAT == eDataType)
				{
					break;
				}
				/* move to next node */
		  		pstInAutheFactForm = pstInAutheFactForm->m_pstNext;
		  	}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstAutheFactFormArr;
	}
	break;

	case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
	case BACNET_DT_AUTHENTICATION_POLICY:
	{
		/* local variables */
		Pr_ListOfBACnetAuPolicy_t *pstAuthePolicyArr = NULL;
		Pr_ListOfBACnetAuPolicy_t *pstInAuthePolicyArr = NULL;
		ListOfBACnetAuPolicy_t *pstAuthePolicy = NULL;
		ListOfBACnetAuPolicy_t *pstInAuthePolicy = NULL;

		/* copy to local buffer */
		pstInAuthePolicyArr = (Pr_ListOfBACnetAuPolicy_t *)pvPropVal;
		pstInAuthePolicy = pstInAuthePolicyArr->m_pstAuPolicy;

		/* allocate memory to save Authentication Policy array pointer */
		pstAuthePolicyArr = OSAL_Malloc(sizeof
			(Pr_ListOfBACnetAuPolicy_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstAuthePolicyArr)
		{
		  	while(NULL != pstInAuthePolicy && (NULL != pstAuthePolicy || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
			  		pstAuthePolicyArr->m_pstAuPolicy = OSAL_Malloc(
			  			sizeof(ListOfBACnetAuPolicy_t),__FILE__,__FUNCTION__,
			  			__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
			  		pstAuthePolicy = pstAuthePolicyArr->m_pstAuPolicy;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstAuthePolicy->m_pstNext = OSAL_Malloc(sizeof
		  	   			(ListOfBACnetAuPolicy_t),__FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstAuthePolicy = pstAuthePolicy->m_pstNext;
			   	}
               
			   	if(NULL != pstAuthePolicy)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstAuthePolicy, pstInAuthePolicy, 
		  	   			sizeof(ListOfBACnetAuPolicy_t));
			   		pstAuthePolicy->m_pstNext = NULL;
					// TODO - copy internal list
					pstAuthePolicy->m_stPolicy.m_pstNext = NULL;

			   		/* increment the count */
			   		pstAuthePolicyArr->m_u32ArraySize++;
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_AuPolicy_List(&pstAuthePolicyArr->m_pstAuPolicy);
					OSAL_Free(pstAuthePolicyArr, __FILE__,__FUNCTION__,__LINE__);
					pstAuthePolicyArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

				/* break if single node */
				if(BACNET_DT_AUTHENTICATION_POLICY == eDataType)
				{
					break;
				}
			   	/* move to next node */
			   	pstInAuthePolicy = pstInAuthePolicy->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstAuthePolicyArr;
	}
	break;

	case BACNET_DT_ACCESS_RULE_ARRAY:
	case BACNET_DT_ACCESS_RULE:
	{
		/* local variables */
		Pr_ListOfBACnetAccessRule_t *pstBacAccRulArr = NULL;
		Pr_ListOfBACnetAccessRule_t *pstInBacAccRulArr = NULL;
		ListOfBACnetAccessRule_t *pstAccRul = NULL;
		ListOfBACnetAccessRule_t *pstInAccRul = NULL;

		/* copy to local buffer */
		pstInBacAccRulArr = (Pr_ListOfBACnetAccessRule_t *)pvPropVal;
		pstInAccRul = pstInBacAccRulArr->m_pstAccessRuleArray;

		/* allocate memory to save array obj id pointer */
		pstBacAccRulArr = OSAL_Malloc(sizeof(Pr_ListOfBACnetAccessRule_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstBacAccRulArr)
		{
			while(NULL != pstInAccRul && (NULL != pstAccRul || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
					pstBacAccRulArr->m_pstAccessRuleArray = OSAL_Malloc(sizeof(ListOfBACnetAccessRule_t),
						__FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstAccRul = pstBacAccRulArr->m_pstAccessRuleArray;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstAccRul->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetAccessRule_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstAccRul = pstAccRul->m_pstNext;
			   	}
               
			   	if(NULL != pstAccRul)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstAccRul, pstInAccRul, sizeof(ListOfBACnetAccessRule_t));
			   		pstAccRul->m_pstNext = NULL;

			   		/* increment the count */
					pstBacAccRulArr->m_u32ArraySize++;
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_AccessRule_List(&pstBacAccRulArr->m_pstAccessRuleArray);
					OSAL_Free(pstBacAccRulArr, __FILE__,__FUNCTION__,__LINE__);
					pstBacAccRulArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

				/* break if single node */
				if(BACNET_DT_ACCESS_RULE == eDataType)
				{
					break;
				}
			   	/* move to next node */
				pstInAccRul = pstInAccRul->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBacAccRulArr;
	}
	break;

	case BACNET_DT_CRED_AU_FACTOR_ARRAY:
	case BACNET_DT_CRED_AU_FACTOR:
	{
		/* local variables */
		Pr_ListOfBACnetCredAuFactor_t *pstBacCredAuFactArr = NULL;
		Pr_ListOfBACnetCredAuFactor_t *pstInBacCredAuFactArr = NULL;
		ListOfBACnetCredAuFactor_t *pstCredAuFact = NULL;
		ListOfBACnetCredAuFactor_t *pstInCredAuFact = NULL;

		/* copy to local buffer */
		pstInBacCredAuFactArr = (Pr_ListOfBACnetCredAuFactor_t *)pvPropVal;
		pstInCredAuFact = pstInBacCredAuFactArr->m_pstCredAuFactArray;

		/* allocate memory to save array obj id pointer */
		pstBacCredAuFactArr = OSAL_Malloc(sizeof(Pr_ListOfBACnetCredAuFactor_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstBacCredAuFactArr)
		{
			while(NULL != pstInCredAuFact && (NULL != pstCredAuFact || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
					pstBacCredAuFactArr->m_pstCredAuFactArray = OSAL_Malloc(sizeof(ListOfBACnetCredAuFactor_t),
						__FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstCredAuFact = pstBacCredAuFactArr->m_pstCredAuFactArray;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstCredAuFact->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetCredAuFactor_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstCredAuFact = pstCredAuFact->m_pstNext;
			   	}
               
			   	if(NULL != pstCredAuFact)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstCredAuFact, pstInCredAuFact, sizeof(ListOfBACnetCredAuFactor_t));
			   		pstCredAuFact->m_pstNext = NULL;

			   		/* increment the count */
					pstBacCredAuFactArr->m_u32ArraySize++;
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_CredentialAuFactor_List(&pstBacCredAuFactArr->m_pstCredAuFactArray);
					OSAL_Free(pstBacCredAuFactArr, __FILE__,__FUNCTION__,__LINE__);
					pstBacCredAuFactArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

				/* break if single node */
				if(BACNET_DT_CRED_AU_FACTOR == eDataType)
				{
					break;
				}
			   	/* move to next node */
				pstInCredAuFact = pstInCredAuFact->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBacCredAuFactArr;
	}
	break;


	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS:
	{
		/* local variables */
		Pr_ListOfBACnetAssignedAccessRights_t *pstBacAsgnAccsRtArr = NULL;
		Pr_ListOfBACnetAssignedAccessRights_t *pstInBacAsgnAccsRtArr = NULL;
		ListOfBACnetAssignedAccessRights_t *pstAsgnAccsRt = NULL;
		ListOfBACnetAssignedAccessRights_t *pstInAsgnAccsRt = NULL;

		/* copy to local buffer */
		pstInBacAsgnAccsRtArr = (Pr_ListOfBACnetAssignedAccessRights_t *)pvPropVal;
		pstInAsgnAccsRt = pstInBacAsgnAccsRtArr->m_pstAsngdAccessArray;

		/* allocate memory to save array obj id pointer */
		pstBacAsgnAccsRtArr = OSAL_Malloc(sizeof(Pr_ListOfBACnetAssignedAccessRights_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstBacAsgnAccsRtArr)
		{
			while(NULL != pstInAsgnAccsRt && (NULL != pstAsgnAccsRt || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
					pstBacAsgnAccsRtArr->m_pstAsngdAccessArray = OSAL_Malloc(sizeof(ListOfBACnetAssignedAccessRights_t),
						__FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstAsgnAccsRt = pstBacAsgnAccsRtArr->m_pstAsngdAccessArray;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstAsgnAccsRt->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetAssignedAccessRights_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstAsgnAccsRt = pstAsgnAccsRt->m_pstNext;
			   	}
               
			   	if(NULL != pstAsgnAccsRt)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstAsgnAccsRt, pstInAsgnAccsRt, sizeof(ListOfBACnetAssignedAccessRights_t));
			   		pstAsgnAccsRt->m_pstNext = NULL;

			   		/* increment the count */
					pstBacAsgnAccsRtArr->m_u32ArraySize++;
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_AssignedAccessRights_List(&pstBacAsgnAccsRtArr->m_pstAsngdAccessArray);
					OSAL_Free(pstBacAsgnAccsRtArr, __FILE__,__FUNCTION__,__LINE__);
					pstBacAsgnAccsRtArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

				/* break if single node */
				if(BACNET_DT_ASSIGNED_ACCESS_RIGHTS == eDataType)
				{
					break;
				}
			   	/* move to next node */
				pstInAsgnAccsRt = pstInAsgnAccsRt->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBacAsgnAccsRtArr;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_LOGBUFFER_TLM:
	{
		/* local variables */
		Pr_ListOfBACnetLogMultipleRecord_t *pstBacLogMulRecArr = NULL;
		Pr_ListOfBACnetLogMultipleRecord_t *pstInBacLogMulRecArr = NULL;
		ListOfBACnetLogMultipleRecord_t *pstLogMulRec = NULL;
		ListOfBACnetLogMultipleRecord_t *pstInLogMulRec = NULL;
		BACnetLogData_t *pstTempInData = NULL;
		BACnetLogData_t *pstTempData = NULL;

		/* copy to local buffer */
		pstInBacLogMulRecArr = (Pr_ListOfBACnetLogMultipleRecord_t *)pvPropVal;
		pstInLogMulRec = pstInBacLogMulRecArr->m_pstLogMultipleRecord;

		/* allocate memory to save array obj id pointer */
		pstBacLogMulRecArr = OSAL_Malloc(sizeof(Pr_ListOfBACnetLogMultipleRecord_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstBacLogMulRecArr)
		{
			while(NULL != pstInLogMulRec && (NULL != pstLogMulRec || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
					pstBacLogMulRecArr->m_pstLogMultipleRecord = 
						OSAL_Malloc(sizeof(ListOfBACnetLogMultipleRecord_t), __FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstLogMulRec = pstBacLogMulRecArr->m_pstLogMultipleRecord;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstLogMulRec->m_pstNext = 
						OSAL_Malloc(sizeof(ListOfBACnetLogMultipleRecord_t), __FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstLogMulRec = pstLogMulRec->m_pstNext;
			   	}
               
			   	if(NULL != pstLogMulRec)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstLogMulRec, pstInLogMulRec, sizeof(ListOfBACnetLogMultipleRecord_t));
			   		pstLogMulRec->m_pstNext = NULL;
					// TODO - copy internal list 
					pstTempData = &pstLogMulRec->m_stlogData;
					pstTempInData = &pstInLogMulRec->m_stlogData;
					while(NULL != pstTempInData && NULL != pstTempInData->m_pstNext)
					{
						/* allocate memory */
						pstTempData->m_pstNext = OSAL_Malloc(sizeof(BACnetLogData_t), __FILE__,__FUNCTION__,__LINE__);
						if(NULL == pstTempData->m_pstNext)
						{
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
							Copy_PropVal_AsPer_DataType: malloc failed \r\n");
							#endif
							return BACDEL_MALLOC_ERROR;				
						}
						
						/* copy data */
						memcpy(pstTempData->m_pstNext, pstTempInData->m_pstNext, sizeof(BACnetLogData_t));
						
						/* move to next node */
						pstTempInData = pstTempInData->m_pstNext;
						pstTempData = pstTempData->m_pstNext;		
					}
					//pstLogMulRec->m_stlogData.m_pstNext = NULL;

			   		/* increment the count */
					//u32Count++;
					pstBacLogMulRecArr->m_u32Count++;
					
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_TrendLogMultipleRecord_List(&pstBacLogMulRecArr->m_pstLogMultipleRecord);
					OSAL_Free(pstBacLogMulRecArr, __FILE__,__FUNCTION__,__LINE__);
					pstBacLogMulRecArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

			   	/* move to next node */
				pstInLogMulRec = pstInLogMulRec->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBacLogMulRecArr;
	}
	break;

	case BACNET_DT_LOGBUFFER_EL:
	{
		/* local variables */
		Pr_ListOfBACnetEventLogRecord_t *pstBacEveLogRecArr = NULL;
		Pr_ListOfBACnetEventLogRecord_t *pstInBacEveLogRecArr = NULL;
		ListOfBACnetEventLogRecord_t *pstEveLogRec = NULL;
		ListOfBACnetEventLogRecord_t *pstInEveLogRec = NULL;

		/* copy to local buffer */
		pstInBacEveLogRecArr = (Pr_ListOfBACnetEventLogRecord_t *)pvPropVal;
		pstInEveLogRec = pstInBacEveLogRecArr->m_pstEventLogRecord;

		/* allocate memory to save array obj id pointer */
		pstBacEveLogRecArr = OSAL_Malloc(sizeof(Pr_ListOfBACnetEventLogRecord_t),__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstBacEveLogRecArr)
		{
			while(NULL != pstInEveLogRec && (NULL != pstEveLogRec || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
					pstBacEveLogRecArr->m_pstEventLogRecord = 
						OSAL_Malloc(sizeof(ListOfBACnetEventLogRecord_t), __FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstEveLogRec = pstBacEveLogRecArr->m_pstEventLogRecord;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstEveLogRec->m_pstNext = 
						OSAL_Malloc(sizeof(ListOfBACnetEventLogRecord_t), __FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
			   		pstEveLogRec = pstEveLogRec->m_pstNext;
			   	}
               
			   	if(NULL != pstEveLogRec)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstEveLogRec, pstInEveLogRec, sizeof(ListOfBACnetEventLogRecord_t));
			   		pstEveLogRec->m_pstNext = NULL;

			   		/* increment the count */
					u32Count++;
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_EventLogRecord_List(&pstBacEveLogRecArr->m_pstEventLogRecord);
					OSAL_Free(pstBacEveLogRecArr, __FILE__,__FUNCTION__,__LINE__);
					pstBacEveLogRecArr = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

			   	/* move to next node */
				pstInEveLogRec = pstInEveLogRec->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstBacEveLogRecArr;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

#ifdef BACDEL_PR12
#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_PROPERTYACCESSRESULT:
	case BACNET_DT_PROPERTYACCESSRESULT_ARRAY:
	{
		/* Local Variables */	
		Pr_ListOfBACnetPropAccessRslt_t *pstPropAccResList  = NULL;
		Pr_ListOfBACnetPropAccessRslt_t *pstInPropAccResList  = NULL;
		ListOfBACnetPropAccessRslt_t *pstPropAccRes = NULL;
		ListOfBACnetPropAccessRslt_t *pstInPropAccRes = NULL;

		/* copy to local buffer */
		pstInPropAccResList = (Pr_ListOfBACnetPropAccessRslt_t *)pvPropVal;
		pstInPropAccRes = pstInPropAccResList->m_pstPropAccRslt;

		/* allocate memory to save property access result list pointer */
		pstPropAccResList = OSAL_Malloc(sizeof(Pr_ListOfBACnetPropAccessRslt_t),
			__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstPropAccResList)
		{
			while(NULL != pstInPropAccRes && (NULL != pstPropAccRes || bFlag))
			{
	  	  		if(bFlag)
	  	  		{
		  			/* allocate memory */
					pstPropAccResList->m_pstPropAccRslt = 
						OSAL_Malloc(sizeof(ListOfBACnetPropAccessRslt_t), 
						__FILE__,__FUNCTION__,__LINE__);
		  			/* reset flag & new node address */
	  	   			bFlag = false;
					pstPropAccRes = pstPropAccResList->m_pstPropAccRslt;
		   		}
		  		else
		  		{
		  			/* allocate memory */
		  			pstPropAccRes->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetPropAccessRslt_t),
						__FILE__,__FUNCTION__,__LINE__);
	  	   			/* move to new node */
					pstPropAccRes = pstPropAccRes->m_pstNext;
		   		}
               
		   		if(NULL != pstPropAccRes)
	  	   		{
	  	   			/* copy data */
	  	   			memcpy(pstPropAccRes, pstInPropAccRes, 
						sizeof(ListOfBACnetPropAccessRslt_t));
					pstPropAccRes->m_pstNext = NULL;

		   			/* increment the count */
					pstPropAccResList->m_u32ArraySize++;
		   		}
		   		else
	  	   		{
	  	   			/* free allocated memories */
					Clear_PropAccessResult_List(&pstPropAccResList->m_pstPropAccRslt);
					OSAL_Free(pstPropAccResList, __FILE__,__FUNCTION__,__LINE__);
					pstPropAccResList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
	  	   		}

				/* break if single node */
				if(BACNET_DT_PROPERTYACCESSRESULT == eDataType)
				{
					break;
				}
		   		/* move to next node */
				pstInPropAccRes = pstInPropAccRes->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstPropAccResList;
	}
	break;

	case BACNET_DT_NW_SECURITY_POLICY:
	case BACNET_DT_NW_SECURITY_POLICY_ARRAY:
	{
		/* Local Variables */	
		Pr_ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcyList  = NULL;
		Pr_ListOfBACnetNwSecurityPolicy_t *pstInNetSecPlcyList  = NULL;
		ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcy  = NULL;
		ListOfBACnetNwSecurityPolicy_t *pstInNetSecPlcy  = NULL;

		/* copy to local buffer */
		pstInNetSecPlcyList = (Pr_ListOfBACnetNwSecurityPolicy_t *)pvPropVal;
		pstInNetSecPlcy = pstInNetSecPlcyList->m_pstSecurityPolicyArr;

		/* allocate memory to save network security policy list pointer */
		pstNetSecPlcyList = OSAL_Malloc(sizeof(Pr_ListOfBACnetNwSecurityPolicy_t),
			__FILE__,__FUNCTION__,__LINE__);

		if(NULL != pstNetSecPlcyList)
		{
			while(NULL != pstInNetSecPlcy && (NULL != pstNetSecPlcy || bFlag))
			{
		  	  	if(bFlag)
		  	  	{
			  		/* allocate memory */
					pstNetSecPlcyList->m_pstSecurityPolicyArr = 
						OSAL_Malloc(sizeof(ListOfBACnetNwSecurityPolicy_t), 
						__FILE__,__FUNCTION__,__LINE__);
			  		/* reset flag & new node address */
		  	   		bFlag = false;
					pstNetSecPlcy = pstNetSecPlcyList->m_pstSecurityPolicyArr;
			   	}
			  	else
			  	{
			  		/* allocate memory */
			  		pstNetSecPlcy->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetNwSecurityPolicy_t),
						__FILE__,__FUNCTION__,__LINE__);
		  	   		/* move to new node */
					pstNetSecPlcy = pstNetSecPlcy->m_pstNext;
			   	}
               
			   	if(NULL != pstNetSecPlcy)
		  	   	{
		  	   		/* copy data */
		  	   		memcpy(pstNetSecPlcy, pstInNetSecPlcy, 
						sizeof(ListOfBACnetNwSecurityPolicy_t));
					pstNetSecPlcy->m_pstNext = NULL;

			   		/* increment the count */
					pstNetSecPlcyList->m_u32ArraySize++;
			   	}
			   	else
		  	   	{
		  	   		/* free allocated memories */
					Clear_NwSecurityPolicy_List(&pstNetSecPlcyList->m_pstSecurityPolicyArr);
					OSAL_Free(pstNetSecPlcyList, __FILE__,__FUNCTION__,__LINE__);
					pstNetSecPlcyList = NULL;
					eRetVal = BACDEL_MALLOC_ERROR; 
					break;
		  	   	}

				/* break if single node */
				if(BACNET_DT_NW_SECURITY_POLICY == eDataType)
				{
					break;
				}
			   	/* move to next node */
				pstInNetSecPlcy = pstInNetSecPlcy->m_pstNext;
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstNetSecPlcyList;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_ADDRESSBINDING:
    {
		/* Local Variables */
		Pr_BACnetAddrBinding_t *pstAddBinding = NULL;
		Pr_BACnetAddrBinding_t *pstInAddBinding = NULL;

		/* copy to local buffer */
		pstInAddBinding = (Pr_BACnetAddrBinding_t *)pvPropVal;

		/* allocate memory to save security Key sets pointer */
		pstAddBinding = OSAL_Malloc(sizeof(Pr_BACnetAddrBinding_t),__FILE__,
			__FUNCTION__,__LINE__);

		if(NULL != pstAddBinding)
		{
			/* copy data */
			memcpy(&pstAddBinding->m_stAddBinding, &pstInAddBinding->m_stAddBinding,
				sizeof(BACnetAddrBinding_t));
			pstAddBinding->m_stAddBinding.pstNext = NULL;
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}
		/* save the pointer to be returned */
		pvData = (void *)pstAddBinding;
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_SECURITY_KEYSET:
	case BACNET_DT_SECURITY_KEYSET_ARRAY:
	{
		/* Local Variables */
		int32_t i32Size = 0;
		void *pvSecKeySetList = NULL;

		/* get structure size */
		if(BACNET_DT_SECURITY_KEYSET == eDataType)
		{
			i32Size = sizeof(Pr_BACnetSecurityKeySet_t);
		}
		else if(BACNET_DT_SECURITY_KEYSET_ARRAY == eDataType)
		{
			i32Size = sizeof(Pr_ListOfBACnetSecurityKeySet_t);
		}

		/* allocate memory to save security Key sets list pointer */
		pvSecKeySetList = OSAL_Malloc(i32Size, __FILE__, __FUNCTION__, __LINE__);

		/* check for NULL pointer */
		if(NULL != pvSecKeySetList)
		{
			/* get structure size */
			if(BACNET_DT_SECURITY_KEYSET == eDataType)
			{
				/* copy data */
				memcpy(&((Pr_BACnetSecurityKeySet_t *)pvSecKeySetList)->m_stSecurityKetSet, 
					&((Pr_BACnetSecurityKeySet_t *)pvPropVal)->m_stSecurityKetSet, 
					sizeof((Pr_BACnetSecurityKeySet_t *)pvSecKeySetList)->m_stSecurityKetSet);
			}
			else if(BACNET_DT_SECURITY_KEYSET_ARRAY == eDataType)
			{
				/* copy data */
				memcpy(&((Pr_ListOfBACnetSecurityKeySet_t *)pvSecKeySetList)->m_stSecurityKetSet, 
					&((Pr_ListOfBACnetSecurityKeySet_t *)pvPropVal)->m_stSecurityKetSet, 
					sizeof((Pr_ListOfBACnetSecurityKeySet_t *)pvSecKeySetList)->m_stSecurityKetSet);
			}
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = pvSecKeySetList;
	}
	break;
#endif /* !PROFILE_BOD_ONLY */
#endif /* PR12 */

#if (defined BACDEL_PR12 || defined BACDEL_PR14)
	case BACNET_DT_EVENT_MSG_TEXT:
	{
		/* local variables */
		Pr_BACnetEventMsgText_t *pstEventMsgTxt = NULL;
		Pr_BACnetEventMsgText_t *pstInEventMsgTxt = NULL;

		/* copy to local buffer */
		pstInEventMsgTxt = (Pr_BACnetEventMsgText_t *)pvPropVal;

		/* allocate memory to save event msg text pointer */
		pstEventMsgTxt = OSAL_Malloc(sizeof(Pr_BACnetEventMsgText_t),__FILE__,
			__FUNCTION__,__LINE__);

		if(NULL != pstEventMsgTxt)
		{
			/* copy data */
			memcpy(pstEventMsgTxt->m_stEventMsgText, pstInEventMsgTxt->m_stEventMsgText,
				sizeof(pstEventMsgTxt->m_stEventMsgText));
		}
		else 
		{
			eRetVal =  BACDEL_MALLOC_ERROR; 
		}

		/* save the pointer to be returned */
		pvData = (void *)pstEventMsgTxt;
	}
	break;
#endif /* (PR12 || PR14) */

	default:
	{
		/* data type not supported */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Copy_PropVal_AsPer_DataType : \
			Data type not supported ");
		#endif
		eRetVal = BACDEL_INITIATOR_CHOICE_ERROR;
		pvData = NULL;
	}
	break;
	} /* switch ends */

	/* check data copy status */
	if(eRetVal != BACDEL_SUCCESS)
	{
		/* Note : in case of error, memories should be freed in respective cases */
		pvData = NULL;
	}

	/* copy input pointer */
	*pvOutData = pvData;

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Copy_PropVal_AsPer_DataType : Exit \r\n");
	#endif

	return eRetVal;
}

/**
*
* DESCRIPTION
* Function reads WP callback config value based on property ID and object ID.
*
* @param eObjectType		[in] Object Type
* @param eObjectProperty	[in] Property Id
*
* @return BACNET_CALLBACK_CONFIG_TYPE  [out] Callback config value
*
*/
BACNET_CALLBACK_CONFIG_TYPE GetPropertyWpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty)
{    
    /* local variables */
    BACNET_REMAPPED_OBJECT_TYPE  eRemapObjType = REMAPPED_MAX_BACNET_OBJECT_TYPE;
	BACNET_CALLBACK_CONFIG_TYPE ePropCBType = MAX_CALLBACK_CONFIG_TYPE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	GetPropertyWpCallback : Entry \r\n");
	#endif

    /* remap object type value */
    eRemapObjType = Remap_Objct_Id(eObjectType);

    if(eRemapObjType != REMAPPED_MAX_BACNET_OBJECT_TYPE &&
        eObjectProperty < MAX_PROP_SUPPORTED)
    {
        /* get the callback type of property */
        //ePropCBType = au8WpCallback[eObjectProperty][eRemapObjType];
			ePropCBType = CALLBACK_CONFIG_REQUIRED;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	GetPropertyWpCallback : Exit \r\n");
	#endif

	/* return data type value */
    return ePropCBType;
}

///**
//*
//* DESCRIPTION
//* Function writes WP callback config value based on property ID and object ID.
//*
//* @param eObjectType		[in] Object Type
//* @param eObjectProperty	[in] Property Id
//* @param ePropCBType		[in] Property callback value
//*
//* @return BACNET_CALLBACK_CONFIG_TYPE  [out] Callback config value
//*
//*/
BACNET_CALLBACK_CONFIG_TYPE SetPropertyWpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty,
	BACNET_CALLBACK_CONFIG_TYPE ePropCBType)
{    
    /* local variables */
    BACNET_REMAPPED_OBJECT_TYPE  eRemapObjType = REMAPPED_MAX_BACNET_OBJECT_TYPE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"SetPropertyWpCallback : Entry \r\n");
	#endif

    /* remap object type value */
    eRemapObjType = Remap_Objct_Id(eObjectType);

    if(eRemapObjType != REMAPPED_MAX_BACNET_OBJECT_TYPE &&
        eObjectProperty < MAX_PROP_SUPPORTED)
    {
        /* set the callback type of property */
        //au8WpCallback[eObjectProperty][eRemapObjType] = ePropCBType;
		//ePropCBType = CALLBACK_CONFIG_NOT_REQUIRED;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"SetPropertyWpCallback : Exit \r\n");
	#endif

	/* return data type value */
    return ePropCBType;
}

/**
*
* DESCRIPTION                                                                          
* This API function to copy the property value from pointer, 
* received from application as per the data type.
*
* @param eDataType		   [in] data type of property.
* @param pvInData		   [in] input data pointer.
* @param pvOutData		   [out]pointer to copy out data.
* @param i32ArrayIdx	   [in] array index 

* @return MAX_BACNET_ERROR_CODE on success
*
*/
BACNET_ERROR_CODE Convert_PropVal_AsPer_DataType(
	BACNET_DATA_TYPE eDataType, 
	void *pvInData, 
	void **pvOutData, 
	int32_t i32ArrayIdx, 
	bool bArrayIndexPresent,
	BACNET_PROPERTY_VALUE *pstPropVal,
	bool *pbWriteNULL,
	bool bCreateCopy)
{
	/* local variables */
	void *pvData = NULL;
	void *pvTempData = NULL;
	BACNET_ERROR_CODE eErrCode = MAX_BACNET_ERROR_CODE;
	bool bFlag = TRUE;
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Convert_PropVal_AsPer_DataType : Entry \r\n");
	#endif

	/* null check for input parameters */
	if(NULL == pvOutData || NULL == pstPropVal || NULL == pbWriteNULL)
	{
		return ERROR_CODE_INCONSISTENT_PARAMETERS;
	}
	/* empty and null datatype can have null pointer */
	else if(NULL == pvInData && eDataType != BACNET_DT_EMPTY && eDataType != BACNET_DT_NULL)
	{
		return ERROR_CODE_INVALID_DATA_TYPE;
	}

	/* set default values */
	memset(pstPropVal, 0, sizeof(BACNET_PROPERTY_VALUE));
	*pvOutData = NULL;

	/* switch as per data type */
	switch(eDataType)
	{
	case BACNET_DT_NULL:
	{
		/* get address & copy data */
		pvData = &pstPropVal->uValue;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_NULL;
	}
	break;

	case BACNET_DT_EMPTY:
	{
		/* do nothing */
	}
	break;

	case BACNET_DT_BOOLEAN:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetBool_t *)pvInData)->m_bVal;
		pstPropVal->uValue.m_Boolean = ((Pr_BACnetBool_t *)pvInData)->m_bVal;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_BOOLEAN;
	}
	break;

	case BACNET_DT_UNSIGNED:		
	case BACNET_DT_UNSIGNED32:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetUnsigned32_t *)pvInData)->m_u32Val;
		pstPropVal->uValue.m_Unsigned_Int = ((Pr_BACnetUnsigned32_t *)pvInData)->m_u32Val;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_UNSIGNED_INT;
	}
	break;

	case BACNET_DT_UNSIGNED8:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetUnsigned8_t *)pvInData)->m_u8Val;
		pstPropVal->uValue.m_Unsigned_Int = ((Pr_BACnetUnsigned8_t *)pvInData)->m_u8Val;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_UNSIGNED_INT;
	}
	break;

	case BACNET_DT_UNSIGNED16:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetUnsigned16_t *)pvInData)->m_u16Val;
		pstPropVal->uValue.m_Unsigned_Int = ((Pr_BACnetUnsigned16_t *)pvInData)->m_u16Val;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_UNSIGNED_INT;
	}
	break;

	case BACNET_DT_INTEGER:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetSigned32_t *)pvInData)->m_i32Val;
		pstPropVal->uValue.m_Signed_Int = ((Pr_BACnetSigned32_t *)pvInData)->m_i32Val;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_SIGNED_INT;
	}
	break;

	case BACNET_DT_REAL:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetReal_t *)pvInData)->m_fVal;
		pstPropVal->uValue.m_Real = ((Pr_BACnetReal_t *)pvInData)->m_fVal;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_REAL;
	}
	break;

	case BACNET_DT_DOUBLE:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetDouble_t *)pvInData)->m_dVal;
		pstPropVal->uValue.m_Double = ((Pr_BACnetDouble_t *)pvInData)->m_dVal;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_DOUBLE;
	}
	break;

	case BACNET_DT_ENUM:
	{
		/* get address & copy data */
		pvData = &((Pr_BinaryEnumPV_t *)pvInData)->m_eVal;
		pstPropVal->uValue.m_Enumerated = ((Pr_BinaryEnumPV_t *)pvInData)->m_eVal;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_ENUMERATED;
	}
	break;

	case BACNET_DT_ENUM_NEW:
	case BACNET_DT_OBJTYPE:
	case BACNET_DT_BACKUPSTATE:
	case BACNET_DT_BACNETDEVSTAT:
	case BACNET_DT_BACNETSEG:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetEnum_t *)pvInData)->m_eVal;
		pstPropVal->uValue.m_Enumerated = ((Pr_BACnetEnum_t *)pvInData)->m_eVal;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_ENUMERATED;
	}
	break;

	case BACNET_DT_OCTETSTRING:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetOctetStr_t *)pvInData)->m_stOctetString;
		memcpy(&pstPropVal->uValue.m_stOctet_String, 
			&((Pr_BACnetOctetStr_t *)pvInData)->m_stOctetString,
			sizeof(BACnetOctetStr_t));
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_OCTET_STRING;
	}
	break;

	case BACNET_DT_BITSTRING:
	{
		/* locals */
		Pr_BACnetBitStr_t *pstBitStr = (Pr_BACnetBitStr_t *)pvInData;
		uint8_t u8Value = 0;

		/* get address & copy data */
		pvData = (Pr_BACnetBitStr_t *)pvInData;

		/* reverse bits */
		pstBitStr->m_stBitStr.m_u8TransBits[0] = 
			Byte_Reverse_Bits(pstBitStr->m_stBitStr.m_u8TransBits[0]);

		/* save unused bits & byte count */
		pstPropVal->uValue.m_stBit_String.m_u8ByteCnt = 
			pstBitStr->m_stBitStr.m_u8ByteCnt;
		pstPropVal->uValue.m_stBit_String.m_u8UnusedBits = 
			pstBitStr->m_stBitStr.m_u8UnusedBits;

		/* copy bits */
		for(u8Value = 0; u8Value < MIN_BITSTRING_BYTES; u8Value++)
		{
			pstPropVal->uValue.m_stBit_String.m_u8TransBits[u8Value] =
				pstBitStr->m_stBitStr.m_u8TransBits[u8Value];				
		}

		/* save tag */
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_BIT_STRING;		
	}
	break;

	case BACNET_DT_BITSTRING_NEW:
	{
		/* locals */
		Pr_BACnetBITStr_t *pstBitStr = (Pr_BACnetBITStr_t *)pvInData;
		uint8_t u8Value = 0;

		/* get address & copy data */
		pvData = &((Pr_BACnetBitStr_t *)pvInData)->m_stBitStr;
		/* reverse bits */
		pstBitStr->m_stBitString.m_u8TransBits[0] = 
			Byte_Reverse_Bits(pstBitStr->m_stBitString.m_u8TransBits[0]);

		/* save unused bits & byte count */
		pstPropVal->uValue.m_stBit_String.m_u8ByteCnt = 
			pstBitStr->m_stBitString.m_u8ByteCnt;
		pstPropVal->uValue.m_stBit_String.m_u8UnusedBits = 
			pstBitStr->m_stBitString.m_u8UnusedBits;

		/* copy bits */
		for(u8Value = 0; u8Value < MAX_BITSTRING_BYTES; u8Value++)
		{
			pstPropVal->uValue.m_stBit_String.m_u8TransBits[u8Value] =
				pstBitStr->m_stBitString.m_u8TransBits[u8Value];				
		}

		/* save tag */
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_BIT_STRING;		
	}
	break;

	case BACNET_DT_CHARSTRING:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetCharStr_t *)pvInData)->m_stCHARString;
		memcpy(&pstPropVal->uValue.m_stCharacter_String, 
			&((Pr_BACnetCharStr_t *)pvInData)->m_stCHARString,
			sizeof(BACnetCharStr_t));		
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_CHARACTER_STRING;
	}
	break;

	case BACNET_DT_DATE:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetDate_t *)pvInData)->m_Date_val;
		memcpy(&pstPropVal->uValue.m_stDate, 
			&((Pr_BACnetDate_t *)pvInData)->m_Date_val,
			sizeof(BACnetDate_t));		
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_DATE;
	}
	break;

	case BACNET_DT_TIME:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetTime_t *)pvInData)->m_Time_val;
		memcpy(&pstPropVal->uValue.m_stTime, 
			&((Pr_BACnetTime_t *)pvInData)->m_Time_val,
			sizeof(BACnetTime_t));
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_TIME;
	}
	break;

	case BACNET_DT_DATETIME:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetDateTime_t *)pvInData)->m_stDateTime;
	}
	break;

	case BACNET_DT_TIMESTAMP:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetTimeStamp_t *)pvInData)->m_stTimeStamp;
	}
	break;

	case BACNET_DT_OBJECTID:
	{
		/* get address & copy data */
		pvData = ((Pr_BACnetObjId_t *)pvInData);
		pstPropVal->uValue.m_stObject_Id.m_eObjType =
		((Pr_BACnetObjId_t *)pvInData)->m_eObjectType;
		pstPropVal->uValue.m_stObject_Id.m_u32ObjInstance =
		((Pr_BACnetObjId_t *)pvInData)->m_u32ObjId;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_OBJECT_ID;
	}
	break;

	case BACNET_DT_CLIENTCOVINCREMENT:
	{
		/* locals */
		Pr_BACnetClientCOV_t *pstCovIncr = (Pr_BACnetClientCOV_t *)pvInData;

		/* get address & copy data */
		pvData = &pstCovIncr->m_stClientCOV.m_fVal;
		pstPropVal->uValue.m_Real = pstCovIncr->m_stClientCOV.m_fVal;
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_REAL;

		/* update tag if null datatype */
		if(BACNET_APPLICATION_TAG_NULL == pstCovIncr->m_stClientCOV.m_eAppTagtype)
		{
			*pbWriteNULL = TRUE;
			pstPropVal->m_TagType = BACNET_APPLICATION_TAG_NULL;
		}
	}
	break;

	case BACNET_DT_SERVICES_SUPPORTED:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetServicesSupported_t *)pvInData)->m_stServiceSupport;
	}
	break;

	case BACNET_DT_OBJECT_TYPE_SUPPORTED:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetObjectTypesSupported_t *)pvInData)->m_stObjectSupport;
	}
	break;

	case BACNET_DT_NOTIFICATIONPRIORITY:
	{
		/* get address & copy data */
		pvData = ((Pr_BACnetNotifyPriority_t *)pvInData);
	}
	break;

	case BACNET_DT_OBJPROPREF:
	{
		/* get address & copy data */
		pvData = ((Pr_BACnetObjPropRef_t *)pvInData);
	}
	break;

	case BACNET_DT_SETPOINTREF:
	{
		/* locals */
		Pr_BACnetSetpointRef_t *pstSetPtRef = (Pr_BACnetSetpointRef_t *)pvInData;
		
		/* get address & copy data */
		if(!pstSetPtRef->m_stSptRef.m_bDataFlag)
		{
			/* save null pointer */
			pvData = NULL;
		}
		else
		{
			/* save property value pointer */
			pvData = ((Pr_BACnetSetpointRef_t *)pvInData);
		}
	}
	break;

	case BACNET_DT_DEVOBJPROPREF:
	{
		/* get address & copy data */
		pvData = ((Pr_BACnetDevObjPropRef_t *)pvInData);
	}
	break;

	case BACNET_DT_DATERANGE:
	{
		/* get address & copy data */
		pvData = &((Pr_BACnetDateRange_t *)pvInData)->m_stDateRange;
	}
	break;
	
	case BACNET_DT_CHARSTRING_ARRAY:
	{
		/* local */
		Pr_ListOfCharStr_t *pstStrArr = NULL;
		ListOfCharStr_t *pstInCharStr = NULL;
		BACNET_PROPERTY_VALUE *pstBkpPropVal = NULL;

		/* get address & copy data */
		pstStrArr = (Pr_ListOfCharStr_t *)pvInData;

		/* backup pointer */
		pstBkpPropVal = pstPropVal;

		/* check array index */
		if(0 == i32ArrayIdx)
		{
			/* save pointer for unsigned value */
			pvData = &pstStrArr->m_u32Count;
			pstPropVal->uValue.m_Unsigned_Int = pstStrArr->m_u32Count;
			pstPropVal->m_TagType = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			break;
		}

		/* copy first node */
		memcpy(&pstPropVal->uValue.m_stCharacter_String, 
			&pstStrArr->m_stStringVal.m_stCharStr,
			sizeof(BACnetCharStr_t));
		pstPropVal->m_TagType = BACNET_APPLICATION_TAG_CHARACTER_STRING;

		if(i32ArrayIdx > 0)
		{
			/* save pointer for unsigned value */
			pvData = (BACNET_PROPERTY_VALUE *)pstBkpPropVal;
			/* break the case as only one value is expected */
			break;
		}

		/* get pointer to 2nd node */
		pstInCharStr = pstStrArr->m_stStringVal.m_pstNext;

		while(NULL != pstInCharStr && NULL != pstPropVal)
		{
			/* malloc memory to next node */
			pstPropVal->pstNextPropVal = OSAL_Malloc(sizeof(BACNET_PROPERTY_VALUE),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstPropVal->pstNextPropVal)
			{
				eErrCode = ERROR_CODE_OUT_OF_MEMORY;
				break;
			}

			/* move to new node */
			pstPropVal = pstPropVal->pstNextPropVal;

			/* copy data */
			memcpy(&pstPropVal->uValue.m_stCharacter_String, &pstInCharStr->m_stCharStr, 
				sizeof(BACnetCharStr_t));
			pstPropVal->m_TagType = BACNET_APPLICATION_TAG_CHARACTER_STRING;
			pstPropVal->pstNextPropVal = NULL;					

			/* move to next node */
			pstInCharStr = pstInCharStr->m_pstNext;
		}			

		/* save the pointer */
		pvData = (BACNET_PROPERTY_VALUE *)pstBkpPropVal;
	}
	break;
	case BACNET_DT_BITSTRING_ARRAY:
	{
		/* local variable */
		Pr_ListOfBitStr_t *pstStrList = NULL;
		Pr_ListOfBitStr_t *pstInStrList = NULL;
		ListOfBitStr_t * pstBitStrList = NULL;
		ListOfBitStr_t * pstInBitStrList = NULL;

		/* copy to local buffer */
		pstInStrList = (Pr_ListOfBitStr_t *)pvInData;
		pstInBitStrList = pstInStrList->m_pstBitString;

		/* allocate memory to save bit string array pointer */
		pstStrList = OSAL_Malloc(sizeof(Pr_ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL != pstStrList)
		{
			while(NULL != pstInBitStrList && (NULL != pstBitStrList || bFlag))
			{
				if(bFlag)
				{
					/* allocate memory */
					pstStrList->m_pstBitString = OSAL_Malloc(
						sizeof(ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
					/* reset flag & new node address */
					bFlag = FALSE;
					pstBitStrList = pstStrList->m_pstBitString;
				}
				else
				{
					/* allocate memory */
					pstBitStrList->m_pstNext = OSAL_Malloc(
						sizeof(ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
					/* move to new node */
					pstBitStrList = pstBitStrList->m_pstNext;
				}

				if(NULL != pstBitStrList)
				{
					/* copy the data */
					memcpy(pstBitStrList, pstInBitStrList, sizeof(ListOfBitStr_t));
					pstBitStrList->m_pstNext = NULL;

					/* increment the count */
					pstStrList->m_u32Count++;
				}
				else
				{
					/* free allocated memories */
					Clear_BitString_List(&pstStrList->m_pstBitString);
					OSAL_Free(pstStrList, __FILE__,__FUNCTION__,__LINE__);
					pstStrList = NULL;
					eErrCode = ERROR_CODE_OUT_OF_MEMORY;
					break;
				}

				/* move to next node */
				pstInBitStrList = pstInBitStrList->m_pstNext;
			}
		}
		else
		{
			eErrCode = ERROR_CODE_OUT_OF_MEMORY;
		}

		/* save the pointer to be returned */
		pvData = (void *)pstStrList;
	}
	break;
	case BACNET_DT_DATELIST:
	{
		/* locals */
		Pr_ListOfBACnetCalendarEntry_t *pstCalEntryList = NULL;

		/* get address & copy data */
		pstCalEntryList = (Pr_ListOfBACnetCalendarEntry_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetCalendarEntry_t *)pvTempData)->m_pstListOfCalendar;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstCalEntryList->m_pstListOfCalendar;
		}
	}
	break;	

	case BACNET_DT_DESTINATION_LIST:
	{
		/* locals */
		Pr_ListOfBACnetDestination_t *pstDestinationList = NULL;

		/* get address & copy data */
		pstDestinationList = (Pr_ListOfBACnetDestination_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetDestination_t *)pvTempData)->m_pstNCRecepient;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstDestinationList->m_pstNCRecepient;
		}
	}
	break;

	case BACNET_DT_DEVOBJPROPREFF_LIST:
	case BACNET_DT_DEVOBJPROPREFF_ARRAY :
	{
		/* locals */
		Pr_ListOfBACnetDevObjPropRef_t *pstDevObjPrRefList = NULL;

		/* get address & copy data */
		pstDevObjPrRefList = (Pr_ListOfBACnetDevObjPropRef_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetDevObjPropRef_t *)pvTempData)->m_pstListOfBACnetDevObjPropReff;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstDevObjPrRefList->m_pstListOfBACnetDevObjPropReff;
		}
	}
	break;

	case BACNET_DT_UNSIGNED_LIST:
	{
		/* locals */
		Pr_ListOfUnsigned_t *pstUintList = NULL;

		/* get address & copy data */
		pstUintList = (Pr_ListOfUnsigned_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfUnsigned_t *)pvTempData)->m_pstUnsignVal;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstUintList->m_pstUnsignVal;
		}
	}
	break;

#ifndef PROFILE_BOD_ONLY
	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	{
		/* locals */
		Pr_ListOfBACnetDailySchedule_t *pstDailySchedule = NULL;

		/* get address & copy data */
		pstDailySchedule = (Pr_ListOfBACnetDailySchedule_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetDailySchedule_t *)pvTempData);
		}
		else
		{
			/* update the pointers */
			pvData = pstDailySchedule;			
		}
	}
	break;

	case BACNET_DT_SPECIALEVENT_ARRAY:
	{
		/* locals */
		Pr_ListOfBACnetSpecialEvent_t *pstSpecialEventList = NULL;

		/* get address & copy data */
		pstSpecialEventList = (Pr_ListOfBACnetSpecialEvent_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetSpecialEvent_t *)pvTempData)->m_pstSplEvent;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstSpecialEventList->m_pstSplEvent;
		}
	}
	break;
#endif /* !PROFILE_BOD_ONLY */

	case BACNET_DT_EVENTPARAMETERS:
	{
		/* locals */
		Pr_BACnetEventParameter_t *pstEventParameter = NULL;

		/* get address & copy data */
		pstEventParameter = (Pr_BACnetEventParameter_t *)pvInData;
		
		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_BACnetEventParameter_t *)pvTempData);
		}
		else
		{
			/* update the pointers */
			pvData = pstEventParameter;
		}
	}
	break;	
	
	case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
	{
		/* locals */
		AnyValue_t *pstAnyValue = NULL;

		/* get address & copy data */
		pstAnyValue = (AnyValue_t *)pvInData;
		pvData = pstPropVal;
		memcpy(pstPropVal, &pstAnyValue->m_stValue,	sizeof(BACNET_PROPERTY_VALUE));
	}
	break;

	case BACNET_DT_OBJECTID_ARRAY:
	{
		/* local */
		Pr_ListOfObjId_t *pstObjIdArr = NULL;

		/* get address & copy data */
		pstObjIdArr = (Pr_ListOfObjId_t *)pvInData;
	
		/* check array index */
		if(0 == i32ArrayIdx)
		{
			/* save pointer for unsigned value */
			pvData = &pstObjIdArr->m_u32ObjCount;
			pstPropVal->uValue.m_Unsigned_Int = pstObjIdArr->m_u32ObjCount;
			pstPropVal->m_TagType = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			break;
		}

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfObjId_t *)pvTempData)->m_pstArrayObjId;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstObjIdArr->m_pstArrayObjId;
		}
	}
	break;

	case BACNET_DT_OPTCHARSTRING_ARRAY:
	{
		/* local */
		Pr_ListOfOptCharStr_t *pstOptCharStrArr = NULL;

		/* get address & copy data */
		pstOptCharStrArr = (Pr_ListOfOptCharStr_t *)pvInData;
	
		/* check array index */
		if(0 == i32ArrayIdx)
		{
			/* save pointer for unsigned value */
			pvData = &pstOptCharStrArr->m_u32Count;
			pstPropVal->uValue.m_Unsigned_Int = pstOptCharStrArr->m_u32Count;
			pstPropVal->m_TagType = BACNET_APPLICATION_TAG_UNSIGNED_INT;
			break;
		}

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfOptCharStr_t *)pvTempData)->m_pstOptCharStr;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstOptCharStrArr->m_pstOptCharStr;
		}
	}
	break;

#ifdef BACDEL_OBJ_NP
	case BACNET_DT_OCTETSTRING_ARRAY:
	{
		/* locals */
		Pr_ListOfOctetStr_t *pstOctetStrList = NULL;

		/* get address & copy data */
		pstOctetStrList = (Pr_ListOfOctetStr_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfOctetStr_t *)pvTempData)->m_pstOctetStrVal;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstOctetStrList->m_pstOctetStrVal;
		}
	}
	break;

	case BACNET_DT_BDT_ENTRY_LIST:
	{
		/* locals */
		Pr_ListOfBACnetBDTEntry_t *pstBdtList = NULL;

		/* get address & copy data */
		pstBdtList = (Pr_ListOfBACnetBDTEntry_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetBDTEntry_t *)pvTempData)->m_pstBDTEntryList;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstBdtList->m_pstBDTEntryList;
		}
	}
	break;

	case BACNET_DT_FDT_ENTRY_LIST:
	{
		/* locals */
		Pr_ListOfBACnetFDTEntry_t *pstFdtList = NULL;

		/* get address & copy data */
		pstFdtList = (Pr_ListOfBACnetFDTEntry_t *)pvInData;

		if(bCreateCopy)
		{
			/* create a copy */
			Copy_PropVal_AsPer_DataType(pvInData, eDataType, &pvTempData);
			pvData = ((Pr_ListOfBACnetFDTEntry_t *)pvTempData)->m_pstFDTEntryList;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* update the pointers */
			pvData = pstFdtList->m_pstFDTEntryList;
		}
	}
	break;

	case BACNET_DT_HOSTNPORT:
	{
		/* get address & copy data */
		pvData = ((Pr_BACnetHostNPort_t *)pvInData);
	}
	break;
#endif /* NP */
	case BACNET_DT_PRIORITY_ARRAY:
	case BACNET_DT_RECIPIENT_LIST:
	default:
	{
		/* datatype not supported */
		eErrCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
	}
	break;
	}

	/* update pointer */
	*pvOutData = pvData;

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Convert_PropVal_AsPer_DataType : Exit \r\n");
	#endif
	return eErrCode;
}

/**
*
* DESCRIPTION                                                                          
* Function to validate read only property.
*
* @param eObjType  [in] object type.
* @param eDevProp  [in] device property.
*
* @return True if read-only property 
*
*/
bool Validate_ReadOnlyProperty(
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID eDevProp)
{
	/* local variable */
	bool bReturnVal = TRUE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Validate_ReadOnlyProperty : Entry \r\n");
	#endif

	/* check if device object */
    if(eObjType == OBJECT_DEVICE)
    {
        /* device object properties */
        switch(eDevProp)
        {
		case PROP_OBJECT_TYPE:
		case PROP_OBJECT_IDENTIFIER:
        case PROP_DEVICE_ADDRESS_BINDING:
        case PROP_OBJECT_LIST:
		case PROP_VENDOR_NAME:
		case PROP_VENDOR_IDENTIFIER:
		case PROP_PROTOCOL_VERSION:
		case PROP_PROTOCOL_REVISION:
		case PROP_PROTOCOL_SERVICES_SUPPORTED:
		case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
		case PROP_DATABASE_REVISION:
		case PROP_SEGMENTATION_SUPPORTED:
		case PROP_MAX_APDU_LENGTH_ACCEPTED:
#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)
		case PROP_LOCAL_DATE:
		case PROP_LOCAL_TIME:
#endif
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
		case PROP_ACTIVE_COV_SUBSCRIPTIONS:
#endif
		{
			;//do nothing
			break;
		}

        default:
		{
			/* reset flag */
            bReturnVal = FALSE;
			break;
		}
        }//switch ends 
    }
#ifdef BACDEL_OBJ_NP
	/* check if network port object object */
	else if(OBJECT_NETWORK_PORT == eObjType)
	{
		/* network port object properties */
		switch(eDevProp)
		{
			case PROP_OBJECT_NAME:
			case PROP_DESCRIPTION:
			case PROP_PROFILE_NAME:
			case PROP_RELIABILITY:
			case PROP_OUT_OF_SERVICE:
			case PROP_NETWORK_NUMBER:
			case PROP_LINK_SPEED:
			case PROP_MAC_ADDRESS:
			case PROP_IP_DNS_SERVER:
			case PROP_IP_DEFAULT_GATEWAY:
			case PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE:
			case PROP_BBMD_FOREIGN_DEVICE_TABLE:
			case PROP_BBMD_ACCEPT_FD_REGISTRATIONS:
			case PROP_FD_BBMD_ADDRESS:
			case PROP_FD_SUBSCRIPTION_LIFETIME:
			{
				/* reset flag */
				bReturnVal = FALSE;
				break;
			}

			default:
			{
				;//do nothing
				break;
			}
		}//switch ends
	}
#endif /* NP */
    else
    {
        /* other objects properties */
        switch(eDevProp)
        {
		case PROP_OBJECT_TYPE:
		case PROP_OBJECT_IDENTIFIER:
		case PROP_EVENT_STATE:
		case PROP_STATUS_FLAGS:
		case PROP_PRIORITY_ARRAY:
#ifdef OPTIONAL_PROPERTY
		case PROP_CHANGE_OF_STATE_TIME:
		case PROP_TIME_OF_STATE_COUNT_RESET:
#endif
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || (defined BACDEL_SER_AE_GEI_B))
        case PROP_EVENT_TIME_STAMPS:
#endif
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_A))
		case PROP_ACKED_TRANSITIONS:
#endif
#ifdef BACDEL_OBJ_EE
		case PROP_EVENT_TYPE:
#endif
		{
			;//do nothing
			break;
		}

		case PROP_NOTIFICATION_CLASS:
		{
			/* read only for NC object */
			if(OBJECT_NOTIFICATION_CLASS != eObjType)
			{
				/* reset flag */
				bReturnVal = FALSE;
			}
			break;
		}

		case PROP_PRESENT_VALUE:
		{
			/* read only for calender object */
			if(OBJECT_CALENDAR != eObjType)
			{
				/* reset flag */
				bReturnVal = FALSE;
			}
			break;
		}

        default:
		{
			/* reset flag */
            bReturnVal = FALSE;
			break;
		}
        }//switch ends 
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Validate_ReadOnlyProperty : Exit \r\n");
	#endif
	return bReturnVal;
}

/**
*
* DESCRIPTION                                                                          
* Function to validate read only properties.
* These properties can be updated only before stack initialization.
*
* @param eObjType  [in] object type.
* @param eDevProp  [in] device property.
*
* @return True if property can be written. 
*
*/
bool Is_ReadOnly_Property_Writable(
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID eDevProp)
{
	/* local variable */
	bool bReturnVal = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Is_ReadOnly_Property_Writable: Entry \r\n");
	#endif

	/* check if stack is already running */
	if(TRUE == g_bBACnetStackInitFlag)
	{
		/* stack already initialized */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
		Is_ReadOnly_Property_Writable: Stack already initialized \r\n");
		#endif
		return bReturnVal;
	}

	/* check if device object */
    if(eObjType == OBJECT_DEVICE)
    {
        /* device object properties */
        switch(eDevProp)
        {
		case PROP_PROTOCOL_SERVICES_SUPPORTED:
		case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
		case PROP_SEGMENTATION_SUPPORTED:
		case PROP_MAX_APDU_LENGTH_ACCEPTED:
		{
			/* set the flag */
			bReturnVal = TRUE;
			break;
		}

		default:
		{
			;//do nothing
			break;
		}
		}// switch ends
	}
	else
	{
        /* other objects properties */
        switch(eDevProp)
        {
			default:
			{
				;//do nothing
				break;
			}
		}// switch ends
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Is_ReadOnly_Property_Writable: Exit \r\n");
	#endif
	return bReturnVal;
}

/**
*
* DESCRIPTION                                                                          
* Function to set property value of the device.
*
* @param eObjType		   [in] object type.
* @param u32ObjectID	   [in] device identifier.
* @param pVirtualDev	   [in] virtual device data.
* @param eDevProp	       [in] device property.
* @param pvWritePropVal    [in] data value pointer.
* @param bArrayIndexPresent[in] flag for array index.
* @param eDataType         [in] property data type.
*
* @return MAX_BACNET_ERROR_CODE on success
*/
BACNET_ERROR_CODE Set_Property_Data(
	uint32_t u32DevId,
	BACNET_OBJECT_TYPE eObjType,
	uint32_t u32ObjectID,
	virtualDevData_t *pVirtualDev,
	BACNET_PROPERTY_ID eDevProp,
	void *pvWritePropVal,
	bool bArrayIndexPresent,
	int32_t i32ArrayIndex,
	uint8_t u8Priority,
	BACNET_DATA_TYPE eDataType)
{
	/* local variables */
	void *pvData = NULL;
	void * pvObjectPrt = NULL;
	BACNET_PROPERTY_VALUE stPropVal = {0};
	BACNET_CONF_DATA stWpData = {0};
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	PROP_ACCESS_TYPE eAccessType = BACNET_DEFAULT;
	void *pvPropAdd = NULL;
	uint32_t u32PropertyAdd = 0;
	ulong32_t ul32OffsetAddr = 0;
	ulong32_t ul32BaseAddr = 0;
	bool bWriteNull = false;
	bool bSameDataCopy = false;
	bool bStatus = false;
	BACNET_RETURN_TYPE eRetType = BACDEL_SUCCESS;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Set_Property_Data : Entry \r\n");
	#endif

	/* Find the object */
    pvObjectPrt = Find_Object(eObjType, u32ObjectID, pVirtualDev);
    if(NULL == pvObjectPrt)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Set_Property_Data: \
        Object not found. \r\n"); 
		#endif
        return ERROR_CODE_UNKNOWN_OBJECT;
    }

	/* get object base address */
    ul32BaseAddr = (ulong32_t)pvObjectPrt;

	/* check for given property support */
	eAccessType = GetDefndPropAccess(eObjType, eDevProp, BACNET_DEFAULT);
	if(NOT_SUPPORTED == eAccessType)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Set_Property_Data: \
        Property not found. \r\n"); 
		#endif
        return ERROR_CODE_UNKNOWN_PROPERTY;
	}

    /* Get the property access type for given object & property. */
	eAccessType = GetAsignPropAccess(eObjType, eDevProp, pvObjectPrt, 
		eAccessType, true, &ul32OffsetAddr);		// false -> true
    eAccessType = Validate_PropertySupport(eObjType, eDevProp, eAccessType);
    if(NOT_SUPPORTED == eAccessType)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Set_Property_Data: \
        Property not found. \r\n"); 
		#endif
        return ERROR_CODE_UNKNOWN_PROPERTY;
    }

	/* validate read only property */
	if(Validate_ReadOnlyProperty(eObjType, eDevProp) && 
		!Is_ReadOnly_Property_Writable(eObjType, eDevProp))
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Set_Property_Data: \
        Property write access denied. \r\n"); 
		#endif
        return ERROR_CODE_WRITE_ACCESS_DENIED;
    }

	/* validate array index */
	eErrorCode = ValidateArrayIndex(eDevProp, eObjType, i32ArrayIndex, 
		pvObjectPrt, bArrayIndexPresent, ul32OffsetAddr);
	if(ERROR_CODE_OTHER != eErrorCode)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Set_Property_Data : Invalid array index \r\n");
		#endif
		return eErrorCode;
	}

	/* set default priority */
	if(BACNET_NO_PRIORITY == u8Priority)
	{
		/* set max priority */
		u8Priority = BACNET_MAX_PRIORITY;
	}
	else if(u8Priority > BACNET_MAX_PRIORITY)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Set_Property_Data : Invalid priority \r\n");
		#endif
		return ERROR_CODE_PARAMETER_OUT_OF_RANGE;
	}	
   
	/* validate data type */
	eErrorCode = Validate_DataType(eDevProp, eObjType, eDataType, eAccessType);
	if(MAX_BACNET_ERROR_CODE != eErrorCode)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Set_Property_Data : Invalid property datatype \r\n");
		#endif
		return eErrorCode;
	}

	/* function to get the pointer as per data type */
	eErrorCode = Convert_PropVal_AsPer_DataType(eDataType, 
		pvWritePropVal, &pvData, i32ArrayIndex,
		bArrayIndexPresent, &stPropVal, &bWriteNull, TRUE);

	if(MAX_BACNET_ERROR_CODE != eErrorCode)
	{
		/* free memory for property values */
		Clear_Property_Value(stPropVal.pstNextPropVal);		
		Clear_Internal_ListType_Properties(&pvData, eDataType);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Set_Property_Data : Could not copy property value \r\n");
		#endif
		return eErrorCode;
	}

	/* check Value is within range for writing, check character set, etc. */
    /* skip list properties with empty value */
	eErrorCode = CheckPropertyValRange(pvObjectPrt, eObjType, 
		u32ObjectID, eDevProp, &stPropVal, pvData, i32ArrayIndex,
		pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId, 
		bArrayIndexPresent, 
		TRUE, pVirtualDev, 
		eDataType);

	if(eErrorCode != MAX_BACNET_ERROR_CODE && BACNET_DT_EMPTY != eDataType)
	{       
		/* free memory for property values */
		Clear_Property_Value(stPropVal.pstNextPropVal);
		Clear_Internal_ListType_Properties(&pvData, eDataType);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Set_Property_Data : Error Code \r\n");
		#endif
		return eErrorCode;
	}
	/* reset error code */
	eErrorCode = MAX_BACNET_ERROR_CODE;

	/* set the default array index */
	if(ARRAY_INDEX_PRESENT != bArrayIndexPresent)
	{
		/* set no array index value */
		i32ArrayIndex = BACNET_ARRAY_ALL;
	}

	/* update array index for commandable property  */
	if(eDevProp == PROP_PRESENT_VALUE &&  
         COMMANDABLE_PROP == eAccessType)
    {
        /* update array index in commandable PV */
        i32ArrayIndex = u8Priority - 1;
    } 

	/* copy parameters */
	stWpData.u32ObjectInstance = u32ObjectID;
	stWpData.eObjectType = eObjType;
	stWpData.eObjectProperty = eDevProp;
	stWpData.pvReadPropValue = (void *)(ul32OffsetAddr + ul32BaseAddr);
	stWpData.u32ArrayIndex = i32ArrayIndex;
	stWpData.bArrIndxPresent = bArrayIndexPresent;
	stWpData.eData_Type = eDataType;
	stWpData.u8Priority = u8Priority;

	/* remove previous list elements for list type properties */
	RemovePreviousElement(&stWpData, pvObjectPrt);
    if(stWpData.bErrorStatus)
    {
		/* free memory for property values */
		Clear_Property_Value(stPropVal.pstNextPropVal);
		Clear_Internal_ListType_Properties(&pvData, eDataType);
        return ERROR_CODE_INTERNAL_ERROR;
    }

	/* get actual property data type */
	stWpData.eData_Type = GetPropertyDataType(stWpData.eObjectType, 
		stWpData.eObjectProperty, BACNET_ARRAY_ALL, FALSE);

	/* compare received and actual value. */
	WP_B_Compare_PropVal_AsPer_DataType(
		&bSameDataCopy, &bWriteNull, &stWpData, 
		pvData, stPropVal.m_TagType, eDataType);

	/* update property value only if it is different than previous */
	if(!bSameDataCopy)
	{
		/* for device object */
		if(eObjType == OBJECT_DEVICE)
		{
			/* update device object property values */
			bStatus = Dv_Write_Prop(&stWpData, pVirtualDev, pvData, eAccessType);

			if((bStatus == true) && ((eDevProp == PROP_OBJECT_NAME) || 
				(eDevProp == PROP_OBJECT_IDENTIFIER)) && (false == bSameDataCopy) )
			{
				/* update database revision property value by 1 */
				Update_Database_Revision_Value(pVirtualDev);
			}
		}
		#if (defined BACDEL_OBJ_NP /*&& defined BACDEL_PR18*/)
		else if(OBJECT_NETWORK_PORT == eObjType)
		{
			/* update network port object property values */
			/* send recursive call flag as true to avoid changing dependent properties */
			bStatus = NetworkPort_Write_Property(stWpData.eObjectProperty,
				stWpData.u32ArrayIndex,	stWpData.bArrIndxPresent,
				TRUE, stWpData.u8Priority, (void *)pVirtualDev, pvData,
				eAccessType, pvObjectPrt, &pvPropAdd, FALSE);

			if((bStatus == true) && ((eDevProp == PROP_OBJECT_NAME) ||
				(eDevProp == PROP_OBJECT_IDENTIFIER)) && (false == bSameDataCopy))
			{
				/* update database revision property value by 1 */
				Update_Database_Revision_Value(pVirtualDev);
			}
		}
		#endif /* NP */
		else
		{
			/* update the property value */
			eRetType = UpdateCommonProperty(eObjType, pvObjectPrt, 
				eDevProp, i32ArrayIndex, pvData, eAccessType, &u32PropertyAdd,
				&pvPropAdd, pVirtualDev, FALSE, FALSE, bWriteNull);

			if( (eRetType == BACDEL_SUCCESS) && 
				((eDevProp == PROP_OBJECT_NAME) || 
				(eDevProp ==  PROP_OBJECT_IDENTIFIER)) &&
				(false == bSameDataCopy) )
			{
				/* update database revision property value by 1 */
				Update_Database_Revision_Value(pVirtualDev);
			}
		}
	}

	/* free memories for property values */
	Clear_Property_Value(stPropVal.pstNextPropVal);
	if(BACNET_DT_DAILYSCHEDULE_ARRAY == eDataType || BACNET_DT_EVENTPARAMETERS == eDataType)
	{
		OSAL_Free(pvData, __FILE__, __FUNCTION__, __LINE__ );
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Set_Property_Data : Exit \r\n");
	#endif
	return eErrorCode;
}

/**
*
* DESCRIPTION                                                                          
* Function to get current count of number of elements in 
* array type property.
*
* @param pvData			   [in]  pointer having property offset.
* @param eDataType	   [in/out]  property data type.
* @param pstUnsignedVal   [out]  unsigned structure.
*
* @returns MAX_BACNET_ERROR_CODE on success
*
*/
BACNET_ERROR_CODE Get_Array_DataType_Count(
	BACNET_DATA_TYPE *peDataType,
	void *pvData,
	Pr_BACnetUnsigned32_t *pstUnsignedVal)
{
	/* local variables */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Get_Array_DataType_Count : Entry \r\n");
	#endif

	/* null check for input pointers */
	if(NULL == peDataType || NULL == pstUnsignedVal || NULL == pvData)
	{
		return ERROR_CODE_INCONSISTENT_PARAMETERS;
	}

	/* switch as per data type */
	switch(*peDataType)
	{
	case BACNET_DT_TIMESTAMP_ARRAY:
	case BACNET_DT_NOTIFICATIONPRIORITY:
	{
		/* fixed size array */
		pstUnsignedVal->m_u32Val = BACNET_ARRAY_OF_THREE;
	}
	break;

	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	{
		/* fixed size array */
		pstUnsignedVal->m_u32Val = BACNET_ARRAY_OF_SEVEN;
	}
	break;

	case BACNET_DT_OBJECTID_ARRAY:
	{
		/* variable size array */
		pstUnsignedVal->m_u32Val = ((Pr_ListOfObjId_t *)pvData)->m_u32ObjCount;
	}
	break;

	case BACNET_DT_CHARSTRING_ARRAY:
	{
		/* variable size array */
		pstUnsignedVal->m_u32Val = ((Pr_ListOfCharStr_t *)pvData)->m_u32Count;
	}
	break;	

	case BACNET_DT_SPECIALEVENT_ARRAY:
	{
		/* variable size array */
		pstUnsignedVal->m_u32Val = ((Pr_ListOfBACnetSpecialEvent_t *)pvData)->m_u32Count;
	}
	break;

	default:
	{
		/* property not an array type */
		eErrorCode = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
	}
	break;
	}

	if(MAX_BACNET_ERROR_CODE == eErrorCode)
	{
		/* update datatype to unsigned 32 */
		*peDataType = BACNET_DT_UNSIGNED32;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Get_Array_DataType_Count : Exit \r\n");
	#endif
	return eErrorCode;
}

/**
*
* DESCRIPTION                                                                          
* Function to validate received data type against stack data type.
*
* @param eDevProp    [in] device property.
* @param eObjType    [in] object type.
* @param eDataType	 [in] property data type.
*
* @returns MAX_BACNET_ERROR_CODE on success
*
*/
BACNET_ERROR_CODE Validate_DataType(
	BACNET_PROPERTY_ID eDevProp,
	BACNET_OBJECT_TYPE eObjType,
	BACNET_DATA_TYPE eDataType,
	PROP_ACCESS_TYPE eAccessType)
{
	/* local variables */
	BACNET_DATA_TYPE eStackDT = BACNET_DT_MAX;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Validate_DataType : Entry \r\n");
	#endif

	/* get stack data type for property */
	eStackDT = GetPropertyDataType(eObjType, eDevProp, BACNET_ARRAY_ALL, FALSE);

	/* stack data type and data type received from application are not same */
	if(eDataType != eStackDT)
	{
		/* null data type */
		if(BACNET_DT_NULL == eDataType)
		{
			/* check for null data type */
			eErrorCode = ValidateNullTag(eDevProp, eObjType, eAccessType);
		}
		/* empty data type */
		else if(BACNET_DT_EMPTY == eDataType)
		{
			if(ValidateEmptyTag(eDevProp, eObjType))
			{
				/* check for list type property having empty data type */
				eErrorCode = ValidateListTypeProperty(eDevProp, eObjType);			
			}
		}
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Validate_DataType : Exit \r\n");
	#endif
	return eErrorCode;
}

/** 
*
* DESCRIPTION
* Compare received and actual value.
* 
* @param  pWpData         [in]     Confirm service data pointer
* @param  pvNewPropVal    [in]     Context data pointer
* @param  eAppTagType     [in]     Application tag number
* @param  eOrgDataType    [in]	   actual data type of decoded value
*
* @return pbSameDataCopy  [out]    If same data is received then return 
*								   True else false
* @return pbWriteNULL     [out]    If NULL data is received then return 
*								   True else false
*/
void WP_B_Compare_PropVal_AsPer_DataType(
	bool *pbSameDataCopy,
	bool *pbWriteNULL,
	BACNET_CONF_DATA *pWpData,
	void *pvNewPropVal,
	BACNET_APPLICATION_TAG eAppTagType,
	BACNET_DATA_TYPE eOrgDataType)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    WP_B_Compare_PV : Entry \r\n");
	#endif

	/* null check for input parameters */
	if(NULL == pWpData || NULL == pvNewPropVal || 
		NULL == pbSameDataCopy || NULL == pbWriteNULL)
	{
		return;
	}

	/* set default values */
	*pbSameDataCopy = FALSE;
	*pbWriteNULL = FALSE;

	/* check as per datatype */
	switch(pWpData->eData_Type)
	{
	case BACNET_DT_NULL:
    {
        /* Ideally This property is not writable */
        pWpData->bErrorStatus = TRUE;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = ERROR_CODE_WRITE_ACCESS_DENIED;
    }
    break;

    case BACNET_DT_BOOLEAN:
    {
        /* local data */
		bool *pbBoolVal = (bool *)pvNewPropVal;
		Pr_BACnetBool_t *pstBoolValue = pWpData->pvReadPropValue;

		/* Compare with existing data, if true update same flag */
        if(pstBoolValue->m_bVal == *pbBoolVal)
		{
            *pbSameDataCopy = TRUE;
		}
    }
    break;

    case BACNET_DT_UNSIGNED32:
	case BACNET_DT_UNSIGNED:
    {
        /* local variables */
		uint32_t *pu32UnsignedVal = (uint32_t *)pvNewPropVal;
		Pr_BACnetUnsigned32_t *pstUnsignedValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
		if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }

        /* Compare with existing data, if true update same flag */
		if((pstUnsignedValue->m_u32Val == *pu32UnsignedVal)	&&
            (eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
            *pbSameDataCopy = TRUE;
		}
    }
    break;
	
	case BACNET_DT_UNSIGNED16:
    {       
        /* local variables */
		uint16_t *pu16UnsignedVal = (uint16_t *)pvNewPropVal;
		Pr_BACnetUnsigned16_t *pstUnsignedValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }

        /* Compare with existing data, if true update same flag */
		if((pstUnsignedValue->m_u16Val == *pu16UnsignedVal) &&
           (eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
            *pbSameDataCopy = TRUE;  
		}
    }
    break;

	case BACNET_DT_UNSIGNED8:
    {     
		/* local variables */
		uint8_t *pu8UnsignedVal = (uint8_t *)pvNewPropVal;
		Pr_BACnetUnsigned8_t *pstUnsignedValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }

        /* Compare with existing data, if true update same flag */
		if((pstUnsignedValue->m_u8Val == *pu8UnsignedVal) &&
           (eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
            *pbSameDataCopy = TRUE;                
		}
    }
    break;

    case BACNET_DT_INTEGER:
    {
		/* local variables */
		int32_t *pi32SignedVal = (int32_t *)pvNewPropVal;
		Pr_BACnetSigned32_t *pstSignedValue = pWpData->pvReadPropValue;
          
		/* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }

        /* Compare with existing data, if true update same flag */
		if((pstSignedValue->m_i32Val == *pi32SignedVal) &&
           (eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
            *pbSameDataCopy = TRUE;                
		}
    }
    break;

    case BACNET_DT_REAL:
    {      
        /* local variables */
		Float_t *pfRealVal = (Float_t *)pvNewPropVal;
		Pr_BACnetReal_t *pstRealValue = pWpData->pvReadPropValue;

		/* Allow NULL To be written for present value data type & client cov */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE || 
			 pWpData->eObjectProperty == PROP_CLIENT_COV_INCREMENT))
        {
            *pbWriteNULL = TRUE;
        }

        /* Compare with existing data, if true update same flag */
        if((pstRealValue->m_fVal == *pfRealVal) && 
			(eAppTagType != BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty != PROP_CLIENT_COV_INCREMENT))
		{
            *pbSameDataCopy = TRUE;                
		}
    }
    break;

	case BACNET_DT_DOUBLE:
    {
       /* local variables */
		Double_t *pdDoubleVal = (Double_t *)pvNewPropVal;
		Pr_BACnetDouble_t *pstDoubleValue = pWpData->pvReadPropValue;

		/* Allow NULL To be written for present value data type & client cov */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }

        /* Compare with existing data, if true update same flag */
        if((pstDoubleValue->m_dVal == *pdDoubleVal) && 
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
            *pbSameDataCopy = TRUE;                
		}

    }
    break;

    case BACNET_DT_OCTETSTRING:
    {
		/* local variable */
		Pr_BACnetOctetStr_t *pstOctStrValue = pWpData->pvReadPropValue;
		BACnetOctetStr_t *pstOctStrVal = (BACnetOctetStr_t *)pvNewPropVal;

        /* Allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }

		/* Compare with existing data, if true update same flag */
        if(!memcmp(&pstOctStrValue->m_stOctetString, pstOctStrVal,
			sizeof(BACnetOctetStr_t)) && 
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
			/* same value */
            *pbSameDataCopy = TRUE;
		}
    }
    break;

    case BACNET_DT_CHARSTRING:
    {
		/* local variable */
		Pr_BACnetCharStr_t *pstCharValue = pWpData->pvReadPropValue;
		BACnetCharStr_t *pstCharVal = (BACnetCharStr_t *)pvNewPropVal;

        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
                *pbWriteNULL = TRUE;
        }                
		/* Compare with existing data, if true update same flag */
        if(!memcmp(pstCharValue->m_stCHARString.m_pu8CharStr, 
			pstCharVal->m_pu8CharStr,
			(pstCharVal->m_u32StrLen > pstCharValue->m_stCHARString.m_u32StrLen ?
			 pstCharVal->m_u32StrLen : pstCharValue->m_stCHARString.m_u32StrLen)) && 
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
			/* same value */
            *pbSameDataCopy = TRUE;
		} 
    }
    break;
    case BACNET_DT_BITSTRING_NEW:
    {
    	// local variables
    	Pr_BACnetBITStr_t *pstBitStr = pWpData->pvReadPropValue;
    	Pr_BACnetBITStr_t *pstBitValue = (Pr_BACnetBITStr_t*)pvNewPropVal;

    	 //allow NULL To be written for present value data type
    	if((eAppTagType == BACNET_APPLICATION_TAG_NULL) &&
    			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
    	{
    		*pbWriteNULL = TRUE;
    	}

    	 //Compare with existing data, if true update same flag
    	if(!memcmp(&pstBitStr->m_stBitString, &pstBitValue->m_stBitString,
    			sizeof(BACnetBITStr_t)) &&
    			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
    	{
    		 //same value
    		*pbSameDataCopy = TRUE;
    	}
    }
    break;

    case BACNET_DT_BITSTRING:
    {
        /* Allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }
    }
    break;

    case BACNET_DT_BITSTRING_ARRAY:
    {
    	// local variables
    	Pr_ListOfBitStr_t *pstBitStr = pWpData->pvReadPropValue;
    	BACnetBITStr_t    *pstBitValue = (BACnetBITStr_t*)pvNewPropVal;

		 //allow NULL To be written for present value data type
		if((eAppTagType == BACNET_APPLICATION_TAG_NULL) &&
				(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
		{
			*pbWriteNULL = TRUE;
		}

		 //Compare with existing data, if true update same flag
		if(!memcmp(&pstBitStr->m_pstBitString->m_stBitString.m_u8TransBits, &pstBitValue->m_u8TransBits,
				sizeof(BACnetBITStr_t)) &&
				(eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
			 //same value
			*pbSameDataCopy = TRUE;
		}

    }
    break;

    case BACNET_DT_ENUM:
    {
		/* local variables */
		BACNET_BINARY_PV *peEnumVal = (BACNET_BINARY_PV *)pvNewPropVal;
		Pr_BinaryEnumPV_t *pstEnumValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }                
                        
        /* Compare with existing data, if true update same flag */
		if((pstEnumValue->m_eVal == *peEnumVal) && 
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
        {
			*pbSameDataCopy = TRUE;
        }
    }
    break;

	case BACNET_DT_ENUM_NEW:
	case BACNET_DT_BACKUPSTATE:
	case BACNET_DT_BACNETDEVSTAT:
	case BACNET_DT_BACNETSEG:
    {
		/* local variables */
		enum_t *peEnumVal = (enum_t *)pvNewPropVal;
		Pr_BACnetEnum_t *pstEnumValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }  

        /* Compare with existing data, if true update same flag */
        if((pstEnumValue->m_eVal == *peEnumVal) &&
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
        {
            *pbSameDataCopy = TRUE;
        }
    }
    break;
	

	case BACNET_DT_DATE:
    {
		/* local variables */
		BACnetDate_t *pstDateVal = (BACnetDate_t *)pvNewPropVal;
		Pr_BACnetDate_t *pstDateValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }  

        /* Compare with existing data, if true update same flag */
        if(!memcmp(&pstDateValue->m_Date_val, pstDateVal,
			sizeof(BACnetDate_t)) &&
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
			/* same value */
            *pbSameDataCopy = TRUE;
		}       
    }
    break;

    case BACNET_DT_TIME:
    {
		/* local variables */
		BACnetTime_t *pstTimeVal = (BACnetTime_t *)pvNewPropVal;
		Pr_BACnetTime_t *pstTimeValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }  

        /* Compare with existing data, if true update same flag */
        if(!memcmp(&pstTimeValue->m_Time_val,pstTimeVal,
			sizeof(BACnetTime_t)) &&
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
		{
			/* same value */
            *pbSameDataCopy = TRUE;
		}
    }
    break;

    case BACNET_DT_OBJECTID:
    {
		/* local variables */
		BACnetObjId_t *pu32ObjId = (BACnetObjId_t *)pvNewPropVal;
		Pr_BACnetObjId_t *pstObjIdValue = pWpData->pvReadPropValue;

        /* allow NULL To be written for present value data type */
        if((eAppTagType == BACNET_APPLICATION_TAG_NULL) && 
			(pWpData->eObjectProperty == PROP_PRESENT_VALUE))
        {
            *pbWriteNULL = TRUE;
        }  

        /* Compare with existing data, if true update same flag */
		if((pstObjIdValue->m_u32ObjId == pu32ObjId->m_u32ObjInstance && 
			pstObjIdValue->m_eObjectType == pu32ObjId->m_eObjType) &&
			(eAppTagType != BACNET_APPLICATION_TAG_NULL))
        {
            *pbSameDataCopy = TRUE;
        }
    }
    break;

	case BACNET_DT_CLIENTCOVINCREMENT:
    {
		/* allow NULL To be written for client cov increment data type */
        if(eAppTagType == BACNET_APPLICATION_TAG_NULL)
        {
            *pbWriteNULL = TRUE;
        }  
    }
    break;

    case BACNET_DT_PRIORITY_ARRAY:
    {

    }
    break;

	case BACNET_DT_OPTCHARSTRING_ARRAY:
	{
		/* allowed null value to be written for alarm/fault values */
		if(BACNET_DT_NULL == eOrgDataType)
		{
			*pbWriteNULL = TRUE;
		}
	}
	break;

	case BACNET_DT_DATETIME:
    case BACNET_DT_UNSIGNED_LIST:
    case BACNET_DT_CHARSTRING_ARRAY:
    case BACNET_DT_TIMESTAMP:
	case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
	case BACNET_DT_DEVOBJPROPREFF_LIST:
	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	case BACNET_DT_SPECIALEVENT_ARRAY:
	case BACNET_DT_DEVOBJPROPREF:
	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
	case BACNET_DT_DESTINATION_LIST:
	case BACNET_DT_EVENTPARAMETERS:
	case BACNET_DT_DATELIST:
	case BACNET_DT_OBJPROPREF:
	case BACNET_DT_SETPOINTREF:
	case BACNET_DT_DATERANGE:
	case BACNET_DT_SCALE:
	case BACNET_DT_PRESCALE:
	case BACNET_DT_SERVICES_SUPPORTED:
	case BACNET_DT_OBJECT_TYPE_SUPPORTED:
	case BACNET_DT_OBJECTID_ARRAY:
	case BACNET_DT_NOTIFICATIONPRIORITY:
	case BACNET_DT_ACCRECORD:
	case BACNET_DT_RECIPIENT_LIST:
	#ifdef BACDEL_OBJ_NP
	case BACNET_DT_OCTETSTRING_ARRAY:
	case BACNET_DT_BDT_ENTRY_LIST:
	case BACNET_DT_HOSTNPORT:
	#endif
    {
		;//do nothing 
    }
    break;

	default:
    {
        /* This is invalid datatype */
        pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
    }
    break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    WP_B_Compare_PV : Exit \r\n");
	#endif
}

/**
* 
* Function to Set NULL value to internal pointer as per data types.
* 
* @param peDataType [in] base pointer of data type.
* @param ppPropValue [in] base pointer to free Memory.
* @return - void;
*
*/
void Set_Null_Value_AsPer_DataType(
   BACNET_DATA_TYPE eDataType, 
   void *pvPropValue)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Set_Null_Value_AsPer_DataType: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pvPropValue)
		return;

	/* switch as per data type */
	switch(eDataType)
	{
	case BACNET_DT_DATELIST:
	{
		/* typecast & set pointer to null */
		Pr_ListOfBACnetCalendarEntry_t *pDateList = NULL;
		pDateList = (Pr_ListOfBACnetCalendarEntry_t *)pvPropValue;
		pDateList->m_pstListOfCalendar = NULL;
	}
	break;

	case BACNET_DT_DESTINATION_LIST:
	{
		/* typecast & set pointer to null */
		Pr_ListOfBACnetDestination_t *pDest = NULL;
		pDest = (Pr_ListOfBACnetDestination_t *)pvPropValue;
		pDest->m_pstNCRecepient = NULL;

	}
	break;

	case BACNET_DT_DEVOBJPROPREFF_LIST:
	{
		/* typecast & set pointer to null */
		Pr_ListOfBACnetDevObjPropRef_t *pObjPropRefList = NULL;
		pObjPropRefList = (Pr_ListOfBACnetDevObjPropRef_t *)pvPropValue;
		pObjPropRefList->m_pstListOfBACnetDevObjPropReff = NULL;
	}
	break;

	case BACNET_DT_UNSIGNED_LIST:
	{
		/* typecast & set pointer to null */
		Pr_ListOfUnsigned_t *pUnsignArr = NULL;
		pUnsignArr = (Pr_ListOfUnsigned_t *)pvPropValue;	
		pUnsignArr->m_pstUnsignVal = NULL;
	}
	break;

	case BACNET_DT_SPECIALEVENT_ARRAY:
	{
		/* typecast & set pointer to null */
		Pr_ListOfBACnetSpecialEvent_t *pExceSchedule = NULL;
		pExceSchedule = (Pr_ListOfBACnetSpecialEvent_t *)pvPropValue;
		pExceSchedule->m_pstSplEvent = NULL;
	}
	break;

	case BACNET_DT_OBJECTID_ARRAY:
	{
		/* typecast & set pointer to null */
		Pr_ListOfObjId_t *pObjIdArray = NULL;
		pObjIdArray = (Pr_ListOfObjId_t *)pvPropValue;
		pObjIdArray->m_pstArrayObjId = NULL;
	} 
	break; 

	case BACNET_DT_OPTCHARSTRING_ARRAY:
	{
		/* typecast & set pointer to null */
		Pr_ListOfOptCharStr_t *pOptCharArray = NULL;
		pOptCharArray = (Pr_ListOfOptCharStr_t *)pvPropValue;
		pOptCharArray->m_pstOptCharStr = NULL;
	} 
	break;
	#ifdef BACDEL_OBJ_NP
	case BACNET_DT_BDT_ENTRY_LIST:
	{
		/* typecast & set pointer to null */
		Pr_ListOfBACnetBDTEntry_t *pBdtEntryList = NULL;
		pBdtEntryList = (Pr_ListOfBACnetBDTEntry_t *)pvPropValue;
		pBdtEntryList->m_pstBDTEntryList = NULL;
	}
	break;
	#endif /* NP */
	default:			
		break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Set_Null_Value_AsPer_DataType: exit \r\n");
	#endif	
}
/**
*
* DESCRIPTION
* This fucntion performs below functionalities
* - callback to application
* - compare existing stack value with received new value
* - call function to update stack memory
*
* @param pWpData	  [in] WP service data
* @param pvObjectPrt  [in] Object base address
* @param appDataValue [in] Property value in primitive data format
* @param pvPropValue  [in] Property value to be copied in stack memory
* @param pvAppValue   [in] Property value to be sent to application
*
* @return void  [out] No Return value
*
*/
static void WP_B_Convert_AppTag_WriteProperty(
	BACNET_CONF_DATA *pWpData,
	void *pvObjectPrt,
	BACNET_PROPERTY_VALUE *appDataValue,
	PROP_ACCESS_TYPE eAccessType,
	virtualDevData_t *pVirtualDev,
	void *pvPropValue,
	ulong32_t ul32PropOffstAddr,
	BACnetAddress_t *pstRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp,
	uint8_t u8ThreadNo,
	void *pvAppValue)
{
    /** local variables */
	void *pvTemp = NULL;
    bool bStatus = false; 
    bool bSameDataCopy = false;
	bool bOutOfService = false;
	bool bWriteNULL = false;
    ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0 ;
	ulong32_t ul32BackupPVOffset = 0;
	uint32_t u32PropertyAdd = 0;
	int32_t i32Len = 0;
	int32_t i32ArrayIndex = BACNET_ARRAY_ALL;
	BACNET_DATA_TYPE eDataTypeBkp = BACNET_DT_NULL;
    BACNET_RETURN_TYPE eRetType = BACDEL_ERROR;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_ConvertAppTag_WriteProperty : Entry \r\n");
	#endif

    /* get object base address */
    ul32BaseAddr = (ulong32_t)pvObjectPrt;
	/* get property value base address */
	pWpData->pvReadPropValue = (void*)(ul32PropOffstAddr + ul32BaseAddr);

	/* backup decoded data-type value */
	eDataTypeBkp = pWpData->eData_Type;
	/* get data type */
	pWpData->eData_Type = GetPropertyDataType(pWpData->eObjectType, 
		pWpData->eObjectProperty, BACNET_ARRAY_ALL, FALSE);

    /* get the array index */
    if(ARRAY_INDEX_PRESENT == pWpData->bArrIndxPresent)
	{
		/* update array index value */
        i32ArrayIndex = pWpData->u32ArrayIndex;
	}

	/* update array index for commandable property  */
	if(pWpData->eObjectProperty == PROP_PRESENT_VALUE &&  
         COMMANDABLE_PROP == eAccessType)
    {
        /* update array index in commandable PV */
        i32ArrayIndex = (pWpData->u8Priority) - 1;
    }	

	/* save pointers */
//	pvCmdPropValue = pvPropValue;

	/* compare values */
	WP_B_Compare_PropVal_AsPer_DataType(&bSameDataCopy, 
		&bWriteNULL, pWpData, 
		pvPropValue, appDataValue->m_TagType, eDataTypeBkp);	 

	/* clear pointer value to avoid freeing of memory & crash */
	pWpData->pvReadPropValue = NULL;

	/* get the value of out of service flag */
	bOutOfService = IsOutofSerivceFlagTrue(ul32BaseAddr, pWpData->eObjectType);

	/* update pointers for list type properties */
	Set_Null_Value_AsPer_DataType(eDataTypeBkp, pvAppValue);
	/* update property value */
#ifdef BACDEL_PR18
	if(pWpData->eObjectType != OBJECT_DEVICE && pWpData->eObjectType != OBJECT_NETWORK_PORT)
#else
	if(pWpData->eObjectType != OBJECT_DEVICE)
#endif
	{
		if(!bSameDataCopy || COMMANDABLE_PROP == eAccessType)
		{
			#ifndef backup_pv_logic
			/* if Out-of-Service changes TRUE to FALSE */
			/* this code ensures that PV is not restored to original value i.e. backup PV */
			if((bOutOfService) && 
				(pWpData->eObjectProperty == PROP_OUT_OF_SERVICE) &&
				(*((bool *)pvPropValue) == 0))
			{
				/* get BackupPresent value offset */
				GET_PRESENTVAL_OFFSET(pWpData->eObjectType, 									
					m_stBackupPresentValue, ul32BackupPVOffset, i32Len);

				/* get Present value property offset */
				GET_PRESENTVAL_OFFSET(pWpData->eObjectType,						
				m_stPresentValue, ul32OffsetAddr, i32Len);

				/* copy present value in backup value  */
				memcpy((void *)(ul32BackupPVOffset + ul32BaseAddr),
					(void *)(ul32OffsetAddr + ul32BaseAddr), i32Len);
			}
			#endif /* */

			/* call the function to update/write the property */
			eRetType = UpdateCommonProperty(pWpData->eObjectType, pvObjectPrt,
				pWpData->eObjectProperty, i32ArrayIndex, pvPropValue, eAccessType,
				&u32PropertyAdd, &pvTemp, pVirtualDev, false, false, bWriteNULL);

			/* check if database-revision needs to be updated */
			if((eRetType == BACDEL_SUCCESS) && 
				((PROP_OBJECT_NAME == pWpData->eObjectProperty) || 
				(PROP_OBJECT_IDENTIFIER == pWpData->eObjectProperty)) &&
				(false == bSameDataCopy))
			{
				/* update database revision property value by 1 */
				Update_Database_Revision_Value(pVirtualDev);
			}
		}
	}
	#if (defined BACDEL_OBJ_NP && defined BACDEL_PR18)
	else if(pWpData->eObjectType == OBJECT_NETWORK_PORT)
	{
		if(!bSameDataCopy)
		{
			/* update networkport object property values */
			bStatus = NetworkPort_Write_Property(pWpData->eObjectProperty, pWpData->u32ArrayIndex,
				pWpData->bArrIndxPresent, FALSE, pWpData->u8Priority, (void *)pVirtualDev,
				pvPropValue, eAccessType, pvObjectPrt, &pvTemp, FALSE);

			/* check if database-revision needs to be updated */
			if((true == bStatus) &&
				((PROP_OBJECT_NAME == pWpData->eObjectProperty) ||
				(PROP_OBJECT_IDENTIFIER == pWpData->eObjectProperty)) &&
				(false == bSameDataCopy))
			{
				/* update database revision property value by 1 */
				Update_Database_Revision_Value(pVirtualDev);
			}
		}
	}
	#endif /* NP */
	else
	{
		if(!bSameDataCopy)
		{
			/* update device object property */
			bStatus = Dv_Write_Prop(pWpData, pVirtualDev, pvPropValue, eAccessType);

			/* check if database-revision needs to be updated */
			if((true == bStatus) && 
				((PROP_OBJECT_NAME == pWpData->eObjectProperty) || 
				(PROP_OBJECT_IDENTIFIER == pWpData->eObjectProperty)) && 
				(false == bSameDataCopy) )
			{
				/* update database revision property value by 1 */
				Update_Database_Revision_Value(pVirtualDev);
			}
		}
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_ConvertAppTag_WriteProperty : Exit \r\n");
	#endif
}

#if (defined BACDEL_SER_DS_WP_B || defined BACDEL_SER_DS_WPM_B)
/**
*                                                                    
* DESCRIPTION                                                                          
* This function to read property of any object.
*    
* @param pVirtualDev	[in]	  virtual device data.
* @param pWpData		[in]      pointer to save property information.
* @param pstRmtDvAddr   [in]	  remote device address
* @param pstTimeStamp   [in]	  time stamp at which request received 
* @param u8ThreadNo     [in]	  thread number
*                                   
* @return TRUE/FALSE	[out]	  true or false.
*	
*/
BACNET_RETURN_TYPE WP_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	BACNET_CONF_DATA *pWpData,
	BACnetAddress_t *pstRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp,
	uint8_t u8ThreadNo)
{
	/** local variables */
    PROP_ACCESS_TYPE eAccessType =  NOT_SUPPORTED;   
	BACNET_PROPERTY_VALUE appDataValue = {0};
    void  *pvappValue = NULL;
    int32_t i32Len = 0;
    void *pvObjectPrt = NULL;
    bool bIsEmptyListValue = false;
	ulong32_t ul32OffsetAddr = 0 ;
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER;
    PROP_ACCESS_TYPE eDfndAccessType = BACNET_DEFAULT;
	void *pGetPropVal = NULL;
	bool bWriteNull = false;
	int32_t i32ArrayIndex = 0;
	uint32_t u32FirstFailedElement = 0;
	BACNET_DATA_TYPE eDataType = BACNET_DT_EMPTY;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Request_Parser : Entry \r\n");
	#endif   

	/* null check for input pointers */
	if(NULL == pVirtualDev || NULL == pWpData)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : null pointers \r\n");
		#endif
		return BACDEL_ERROR;
	}
	else if(NULL == pWpData->pu8PropValueBuffer)
	{
		pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_DEVICE;
        pWpData->eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return BACDEL_ERROR;
	}

	/* initialise property value structure */
    /* NOTE : donot change this initialization value */
    memset(&appDataValue, -1, sizeof(BACNET_PROPERTY_VALUE));
    /* init next to null - default */
    appDataValue.pstNextPropVal = NULL;

    /* Check the object type. If device get Device Object base else if object then traverse the
    object list to get object base pointer */

    /** Find The Object Instance Existance  */
    pvObjectPrt = Find_Object(pWpData->eObjectType, 
		pWpData->u32ObjectInstance,pVirtualDev);
    if(pvObjectPrt == NULL)
    {
        /* Free received Property Value buffer */
        OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
        pWpData->pu8PropValueBuffer = NULL;
        pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_OBJECT;
        pWpData->eErrorCode = ERROR_CODE_UNKNOWN_OBJECT;

		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : Error Code \r\n");
		#endif
        return BACDEL_ERROR;
    }

    /** Check if the property is supported in stack */
    eAccessType = GetDefndPropAccess(pWpData->eObjectType, 
		pWpData->eObjectProperty, NOT_SUPPORTED);

    eDfndAccessType = eAccessType;
    if(NOT_SUPPORTED == eAccessType)
    {
        /* Free received Property Value buffer */
        OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
        pWpData->pu8PropValueBuffer = NULL;

        /* Unknown Property  */
        pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;    

		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : Error Code \r\n");
		#endif
        return BACDEL_ERROR;
    }


    /* Check Assigned Access type of while Adding property */
    eAccessType = GetAsignPropAccess(pWpData->eObjectType, 			
		pWpData->eObjectProperty, pvObjectPrt, BACNET_DEFAULT, 
		false, &ul32OffsetAddr);
		
    if(NOT_SUPPORTED == eAccessType)
    {
        /* Free received Property Value buffer */
        OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
        pWpData->pu8PropValueBuffer = NULL;
        /* Unknown Property  */
        pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;  

		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : Error Code  \r\n");
		#endif
        return BACDEL_ERROR;
    }
    else if(READ_ONLY == eAccessType)
    {
        /* check if property can be written even if it is read only */
        if(Is_Property_Writable(pWpData->eObjectProperty, pWpData->eObjectType,
            pvObjectPrt, pVirtualDev))
        {
        	; /* do nothing */
        }
        else
        {
            /* Free received Property Value buffer */
            OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
            pWpData->pu8PropValueBuffer = NULL;
            /* Write Access Denied */
            pWpData->bErrorStatus = true;
            pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
            pWpData->eErrorCode = ERROR_CODE_WRITE_ACCESS_DENIED;

			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Request_Parser : Error Code\r\n");
			#endif
            return BACDEL_ERROR;
        }
    }
	/* property access is "READ" do not allow to it write from application or network */
    else if(READ == eAccessType)
    {
        /* Free received Property Value buffer */
        OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
        pWpData->pu8PropValueBuffer = NULL;
        /* Write Access Denied */
        pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = ERROR_CODE_WRITE_ACCESS_DENIED;

		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : Access set to READ \r\n");
		#endif
        return BACDEL_ERROR;
    }
	/* property access is "COMMANDABLE_PROP" allow it to be written from application or network */
	else if(COMMANDABLE_PROP == eAccessType)
	{
		; /* do nothing */
	}
    else
    {
		/* return error if is request received from n/w and property is not writable */
		if(FALSE == pWpData->bIsApplicationReq && READ_WRITE != eAccessType)
		{
			/* Free received Property Value buffer */
			OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
			pWpData->pu8PropValueBuffer = NULL;
			/* Write Access Denied */
			pWpData->bErrorStatus = true;
			pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
			pWpData->eErrorCode = ERROR_CODE_WRITE_ACCESS_DENIED;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Request_Parser :Access set to READ \r\n");
			#endif
			return BACDEL_ERROR;
		}

        /* Update Original Access type of property */
		/* read any-way */
        if(eAccessType == READ_ANYWAY)
        { 
			if(COMMANDABLE_PROP == eDfndAccessType || READ_ONLY == eDfndAccessType)
			{
				eAccessType = GetDefndPropAccess(pWpData->eObjectType, pWpData->eObjectProperty, BACNET_DEFAULT);
			}
			else
			{
				eAccessType = GetDefndPropAccess(pWpData->eObjectType, pWpData->eObjectProperty, BACNET_DEFAULT);
			}
        }
		/* write any-way */
        else if(eAccessType == WRITE_ANYWAY)
        {
            if(COMMANDABLE_PROP == eDfndAccessType || READ_ONLY == eDfndAccessType)
			{
				eAccessType = GetDefndPropAccess(pWpData->eObjectType, pWpData->eObjectProperty, BACNET_DEFAULT);
			}
            else
			{
				eAccessType = GetDefndPropAccess(pWpData->eObjectType, pWpData->eObjectProperty, BACNET_DEFAULT);
			}
        }
    }

	/* validate array index */
    eErrorCode = ValidateArrayIndex(pWpData->eObjectProperty, 
		pWpData->eObjectType, pWpData->u32ArrayIndex, pvObjectPrt, 
		pWpData->bArrIndxPresent,
		ul32OffsetAddr);
	if(ERROR_CODE_OTHER != eErrorCode)
    {
		/* Free received Property Value buffer */
		OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
		pWpData->pu8PropValueBuffer = NULL;
        /* array index invalid or property not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
        WP_B_Request_Parser : ValidateArrayIndex fails \r\n");
		#endif
        /* return error code */
        pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = eErrorCode;
		return BACDEL_ERROR;
    }

	//validate priority 
	/* set default priority */
	if(BACNET_NO_PRIORITY == pWpData->u8Priority)
	{
		pWpData->u8Priority = BACNET_MAX_PRIORITY;
	}
	else if(pWpData->u8Priority > BACNET_MAX_PRIORITY)
	{
		/* Free received Property Value buffer */
		OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
		pWpData->pu8PropValueBuffer = NULL;
		/* Write Access Denied */
		pWpData->bErrorStatus = true;
		pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
		pWpData->eErrorCode = ERROR_CODE_PARAMETER_OUT_OF_RANGE;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : Invalid priority value \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/* check for empty property value */
    if(pWpData->i32ApplicationDataLen == 0 
		&& ValidateEmptyTag(pWpData->eObjectProperty, pWpData->eObjectType))
    {
       /* check if this is list type property */
       eErrorCode = ValidateListTypeProperty(pWpData->eObjectProperty,
           pWpData->eObjectType);
       if(MAX_BACNET_ERROR_CODE != eErrorCode || 
		   (BACNET_ZERO == pWpData->u32ArrayIndex && ARRAY_INDEX_PRESENT == pWpData->bArrIndxPresent))
       {
            /* Free received Property Value buffer */
            OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
            pWpData->pu8PropValueBuffer = NULL;

            /* if not list type, return error */
            pWpData->bErrorStatus = true;
            pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
            pWpData->eErrorCode = ERROR_CODE_INVALID_DATA_TYPE;
			/* if array index present & value is empty, return error */
			if(ARRAY_INDEX_PRESENT == pWpData->bArrIndxPresent)
			{
				pWpData->eErrorCode = ERROR_CODE_INCONSISTENT_PARAMETERS;
			}

			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Request_Parser : Invalid tag \r\n");
			#endif
            return BACDEL_ERROR;
       }
       /* make flag true to avoid validating tag & property value range */
       else 
	   {
		   /* set the flag */
		   bIsEmptyListValue = true;
	   }
    }

	/* check for null tag */
    if(pWpData->i32ApplicationDataLen == 1 
		&& pWpData->pu8PropValueBuffer[0] == 0)
    {
        /* check if property supports null tag value */
        eErrorCode = ValidateNullTag(pWpData->eObjectProperty,
			pWpData->eObjectType,eDfndAccessType);
        if(MAX_BACNET_ERROR_CODE != eErrorCode)
        {
            /* Free received Property Value buffer */
            OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
            pWpData->pu8PropValueBuffer = NULL;
            /* if not , return error */
            pWpData->bErrorStatus = true;
            pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
            pWpData->eErrorCode = eErrorCode;

			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Request_Parser : Invalid data type \r\n");
			#endif
            return BACDEL_ERROR;
        }
    }

	/* decode property value */
	i32Len = BACApp_Decode_Data_Type(
		pWpData->pu8PropValueBuffer,
		pWpData->i32ApplicationDataLen, 
		pWpData->eObjectProperty,
		pWpData->eObjectType,
		&pWpData->eData_Type, 
		&pGetPropVal,
		pWpData->u32ArrayIndex,
		pWpData->bArrIndxPresent,
		&u32FirstFailedElement);

	/* check for error in decoding */
	if(i32Len < 0)
	{
		/* convert to BACnet error codes */
		Convert_DT_Error_To_Error_Code(&pWpData->eErrorCode,   
			&pWpData->eErrorClass, i32Len);
	    pWpData->bErrorStatus = true;

		/* Free received Property Value buffer */
		OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
		pWpData->pu8PropValueBuffer = NULL;

		Clear_PropVal_AsPer_DataType(pWpData->eData_Type, &pGetPropVal);
		return BACDEL_ERROR;
	}

    /* Free received Property Value buffer */
    OSAL_Free(pWpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
    pWpData->pu8PropValueBuffer = NULL;

	/* get the array index */
    if(ARRAY_INDEX_PRESENT != pWpData->bArrIndxPresent)
	{
        i32ArrayIndex = BACNET_ARRAY_ALL;
	}
    else
	{
        i32ArrayIndex = pWpData->u32ArrayIndex;
	}

	/* function to get the pointer as per data type */
	eErrorCode = Convert_PropVal_AsPer_DataType(
		pWpData->eData_Type, pGetPropVal, &pvappValue,
		i32ArrayIndex,pWpData->bArrIndxPresent, 
		&appDataValue, &bWriteNull, FALSE);
	
	if(MAX_BACNET_ERROR_CODE != eErrorCode)
	{
	    pWpData->bErrorStatus = true;
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = eErrorCode;
		Clear_PropVal_AsPer_DataType(pWpData->eData_Type, &pGetPropVal);
		/* TODO - free pvappValue */
		Clear_Property_Value(appDataValue.pstNextPropVal);
		return BACDEL_ERROR;
	}

    /* Check Value is within range for writing, check character set, etc. */
    /* skip list properties with empty value */
    eErrorCode = CheckPropertyValRange(pvObjectPrt, pWpData->eObjectType, 
		pWpData->u32ObjectInstance, pWpData->eObjectProperty, 
		&appDataValue, pvappValue, pWpData->u32ArrayIndex,
		pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId, 
		pWpData->bArrIndxPresent, 
		pWpData->bIsApplicationReq, pVirtualDev, pWpData->eData_Type);

    if(eErrorCode != MAX_BACNET_ERROR_CODE && false == bIsEmptyListValue)
    {
        /* Error while decoding - a appDataValue larger than we can handle */
        pWpData->bErrorStatus = true;
		if(eErrorCode == ERROR_CODE_LOG_BUFFER_FULL)
			pWpData->eErrorClass = ERROR_CLASS_OBJECT;
		else if(eErrorCode == ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY)
			pWpData->eErrorClass = ERROR_CLASS_RESOURCES;
		else
			pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = eErrorCode;
        Clear_PropVal_AsPer_DataType(pWpData->eData_Type, &pGetPropVal);
		Clear_Property_Value(appDataValue.pstNextPropVal);
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Request_Parser : Error Code \r\n");
		#endif
        return BACDEL_ERROR;

    }

    /* only for list type of peroperty */
    if(i32Len >= 0)
    {
    	RemovePreviousElement(pWpData, pvObjectPrt);
        if(pWpData->bErrorStatus)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Request_Parser : Error Code  \r\n");
			#endif
            return BACDEL_ERROR;
        }
    }

	/* backup decoded value data-type */
	eDataType = pWpData->eData_Type;
	
    /* Update Property Value Structure */
    WP_B_Convert_AppTag_WriteProperty(
		pWpData,
		pvObjectPrt,
		&appDataValue,                             
		eAccessType, 
		pVirtualDev, 
		pvappValue,
		ul32OffsetAddr,
		pstRmtDvAddr,
		pstTimeStamp,
		u8ThreadNo,
		pGetPropVal);

	// TODO
	// Need to check timeout scenario here

    /* Free Memory for property value */
	Clear_Property_Value(appDataValue.pstNextPropVal);

	/* clear list/array type properties */
	Clear_PropVal_AsPer_DataType(eDataType, &pGetPropVal);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Request_Parser : Exit \r\n");
	#endif
    return BACDEL_SUCCESS;
}
#endif /* WP_B || WPM_B */

/********************* End of propertyValueWrite.c file *********************/
