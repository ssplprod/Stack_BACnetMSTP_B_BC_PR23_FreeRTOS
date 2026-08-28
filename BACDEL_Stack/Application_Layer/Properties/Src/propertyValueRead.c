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
*   File Name - propertyvalueread.c
*
*   RELEASE HISTORY
*      
*     DATE         NAME                DESCRIPTION
*
*   12/07/2011     Prashant Badgujar   File Created & Added generic read API.
*   13/07/2011     M.Venu              Updated Binary Input and Output
*                                      structure members as per std.coding style
*   14/07/2011     M.Venu              Customized Read logic from property based
*                                      functions to unique functions
*   15/07/2011     M.Venu              Added common properties of Binary and
*                                      Device objects
*   18/07/2011     M.Venu              Included Polarity datat type relavent 
*                                      changes
*   18/07/2011     Prashant Badgujar   Changed return value of  Dv_Read_Prop 
*                                      in Read_Object_Property API
*   20/07/2011     M.Venu              Redesigned ReadCommonProperties function 
*                                      logic using macro definations
*   22/07/2011     M.Venu              Added Read Functionality for 
*                                      Multi state objects
*   26/07/2011     M.Venu              Updated with GET_BINARYONLY_OFFSET
*   27/07/2011     M.Venu              1. Replaced Addition of Base and offset 
*                                      pointer is replaced with a variable with
*                                      already added
*                                      2. Read_Common_Properties function direct
*                                      return replaced with a varaible
*  28/07/2011      M.Venu              Modified Read_Object_Property for 
*                                      independent of object type to read 
*  28/07/2011      M.Venu              Added ValidateAccessAndIndex function 
*                                      and Array Index validation done
*  01/08/2011	   Prashant Badgujar   Adding AI,AO & AV support in Find_Object API
*  03/08/2011	   Prashant Badgujar   Removing static from ValidateAccessAndIndex API
*									   & making GetDefndPropAccess generic for device &
*									   objects.
*  03/08/2011      M.Venu              1. Added IsNoIndex, ValidateAccessAndIndex functions
*                                      2.Index based validation done for each property
*  04/08/2011      Prashant			   Adding AI,AO & AV support in Present Value.
*  04/08/2011      Prashant            Addingg missing properties in Read_Common_Properties
*                                      API for AI, AO & AV.
*   04/08/2011     M.Venu               Updated Status text add property with 
*                                      the new macro GET_STATETEXT_OFFSET & 
*                                      supported for DateTime type properties
*   05/08/2011     M.Venu               Supported for EventTimeStamp property with
*                                      hardcode values for read functionality test.
*   29/08/2011     M.Venu               Outofservice comparision with TRUE replaced
*                                      with "'1".With out this present value 
*                                       property not able to write
*   16/09/2011     M.Venu                1. Removed Status flag local fill up
*                                      2. Replaced Outofservice validation with generic function
*                                      3. Added Generic function GetPresentValOffset
*	16/09/2011	   Prashant			   Adding HW Interface checks before performing 
*                                      Read Operation.
*   16/09/2011     M.Venu              Made changes to make ReadCommonProperty 
*                                      and ReadPresentValue functions generic
*   20/09/2011     M.Venu              Replaced GET_ANALOG_RELINQUISH_OFFSET macro with
*                                      uinque macro for all objects
*   20/09/2011     M.Venu              Removed Test code for Date & Time type properties
*   22/09/2011     M.Venu              Added GetPolarity function and supported 
*                                      PROP_ELAPSED_ACTIVE_TIME functionality
*
*
*************************************************************************/
#include "propertyValueRead.h"
#include "propertyGenricHandler.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "bacDELApi.h"
#include "propertyValueWrite.h"
#include "propertyValidations.h"
#include <string.h>

/**
*DESCRIPTION
*   Based on Object Type and Object Instance ID, this API 
*   Returns object pointer after traversing object linked list
*
*@param eObjectType [in] Object Type
*@param u32InObjID [in] Object Instance ID
*@param pVirtualDev [in] Pointer to Virtual Device struct
*
*@return void * [out] Object pointer of specific object Id, if not found
*                     returns NULL 
*
*/
void *Find_Object(BACNET_OBJECT_TYPE  eObjectType,
                         uint32_t u32InObjID,
                         virtualDevData_t *pVirtualDev)
{
    void *pvObject = NULL;
    void *pvObjectNxtptr = NULL;
    ulong32_t ul32BaseAddr = 0;     /* For Base struct Addr */
    ulong32_t ul32OffsetAddr = 0 ;  /* For Data offset Addr */
    uint32_t u32ObjectId = 0;       /* For Object ID */
    
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Find_Object : Entry \r\n"); 
	#endif

    if(NULL == pVirtualDev)
        return NULL;

    /* Based on Object Type */
    switch(eObjectType)
    {
#ifdef BACDEL_OBJ_AI
        case OBJECT_ANALOG_INPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstAnalogInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_AO
        case OBJECT_ANALOG_OUTPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstAnalogOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_AV
        case OBJECT_ANALOG_VALUE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstAnalogValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BI
        case OBJECT_BINARY_INPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstBinaryInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BO
        case OBJECT_BINARY_OUTPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstBinaryOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BV
        case OBJECT_BINARY_VALUE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstBinaryValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSI
        case OBJECT_MULTI_STATE_INPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstMSInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSO
        case OBJECT_MULTI_STATE_OUTPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstMSOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSV
        case OBJECT_MULTI_STATE_VALUE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstMSValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_DEV
        case OBJECT_DEVICE:
        {
            if(u32InObjID == pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId ||
               u32InObjID == SPECIAL_DEVICE_OBJECT)
            {
                pvObject = (void *) &(pVirtualDev->m_stDevObject);
                return pvObject;
            }
        }
        break;
#endif
#ifdef BACDEL_OBJ_NC
        case OBJECT_NOTIFICATION_CLASS:
        {
			pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstNotificationClass;
        }
        break;
#endif
#ifdef BACDEL_OBJ_CAL
		case OBJECT_CALENDAR:
        {
			pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstCalendar;
        }
        break;
#endif
#ifdef BACDEL_OBJ_SDL
		case OBJECT_SCHEDULE:
        {
			pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstSchedule;
        }
        break;
#endif
#ifdef BACDEL_OBJ_NP
		case OBJECT_NETWORK_PORT:
        {
        	if(NULL != pVirtualDev->m_stSupportedObjects.m_pstNetworkPort)
			{
				if(u32InObjID == pVirtualDev->m_stSupportedObjects.m_pstNetworkPort->
					m_stObjectID.m_u32ObjId || u32InObjID == BACNET_MAX_INSTANCE)
				{
					pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstNetworkPort;
					return pvObject;
				}
			}
        }
        break;

#endif

       default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Find_Object :\
                The ObjectType %d with ID %d Not Found,returned NULL Pointer\r\n",
                eObjectType,u32InObjID);
			#endif
            pvObject = NULL;
        }
    }/* End of Switch Case */
    
    if(pvObject == NULL)
        return pvObject;
    
    /* Traverse the object list for particular object type */
    while(pvObject != NULL)
    {
        /* Get the Base Address */
        ul32BaseAddr = (ulong32_t )pvObject;
        
        /* Check if object instance of Read property data matched
                            with device object BI instances */        
        GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId , 
                                                    ul32OffsetAddr);

        memcpy((void *)&u32ObjectId, (void *)(ul32BaseAddr+ul32OffsetAddr),
                                                 sizeof(uint32_t));
        /* Validate Object Id */
        if(u32InObjID == u32ObjectId)
        {
            /* Object Type matched */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Find_Object : Object Type %d Found with \
                                    the given ID %d \r\n",eObjectType,u32InObjID);
			#endif
            break;
        }
        else
        {
            /* Move to Next object Pointer */
           GET_OBJCOMMON_OFFSET(eObjectType, pstNext , ul32OffsetAddr);   
            
            /* Copy address of next element stored in pstNext pointer */
            memcpy(&pvObjectNxtptr, (void *)(ul32BaseAddr+ul32OffsetAddr),
                                                 sizeof(void *));
            pvObject = pvObjectNxtptr;
        }
    }
    
	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Find_Object : Exit \r\n"); 
	#endif

    /* Retrun object pointer */
    return pvObject;
}

/**
*DESCRIPTION
*   Based on Object Type, Return Data Type used for Encode Tag
*   Note: Add cases for new object type support
*
*@param eObjectType [in] Object Type
*@param peDataType  [in/out] Encoding Tag Data Type
*
*@return void  [out] No Return 
*
*/
void UpdateValuePropDataType(BACNET_OBJECT_TYPE eObjectType,
                                    BACNET_DATA_TYPE *peDataType)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:UpdateValuePropDataType : Entry \r\n");     
	#endif

    /* Update Data type for Tag at encoding time */
    switch(eObjectType)
    {
        case OBJECT_ANALOG_OUTPUT:
        case OBJECT_ANALOG_VALUE:
            *peDataType = BACNET_DT_REAL;
            break;

        case OBJECT_BINARY_INPUT:
        case OBJECT_BINARY_OUTPUT:
        case OBJECT_BINARY_VALUE:
            *peDataType = BACNET_DT_ENUM;
            break;

        case OBJECT_MULTI_STATE_INPUT:
        case OBJECT_MULTI_STATE_OUTPUT:
        case OBJECT_MULTI_STATE_VALUE:
            *peDataType = BACNET_DT_UNSIGNED;
            break;
        default :
        {
            /* default data type is NULL */
        	*peDataType = BACNET_DT_NULL;
        }
        break;
    }/* End of Switch*/

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:UpdateValuePropDataType : Exit \r\n");     
	#endif
}

/**
*DESCRIPTION
*   Based on Object Type, Reads Present value of object
*
*@param eObjectType [in] Object Type
*@param ul32BaseAddr  [in] Object Base Memory address
*@param ul32OffsetAddr [in] Object Present value property offset
*@param  pstRpData [in/out] Pointer to Read property structure
*
*@return void  [out] No Return 
*
*/
static void ReadPresentValueProperty(BACNET_OBJECT_TYPE eObjectType,
                                     ulong32_t ul32BaseAddr,
                                     ulong32_t ul32OffsetAddr, 
                                     BACNET_DATA_TYPE *peData_Type,
                                     void **pvReadPropValue,
                                     void *pVirtualDev)
{
    /* local variables */
    ulong32_t ul32TotalAddr = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    ReadPresentValueProperty : Entry \r\n");     
	#endif

    /* calculate the actual offset address */
    ul32TotalAddr = ul32BaseAddr+ul32OffsetAddr;

    /* return the total calculated address */
    *pvReadPropValue = (void *)ul32TotalAddr;

    /* Update Data type for Tag at encoding time */ 
    switch(eObjectType)
    {
        case OBJECT_ANALOG_INPUT:
        case OBJECT_ANALOG_OUTPUT:
        case OBJECT_ANALOG_VALUE:
        {
            *peData_Type = BACNET_DT_REAL;
        }
        break;
#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
        case OBJECT_BINARY_INPUT:
        case OBJECT_BINARY_OUTPUT:
        case OBJECT_BINARY_VALUE:
        {
            /* Assign Pointer for Present Value */
            *peData_Type = BACNET_DT_ENUM;
            /* Update polarity value in PV structure for encoding */
          //  Update_PV_Polarity(ul32BaseAddr, eObjectType);    <@>
        }
        break;
#endif

        case OBJECT_MULTI_STATE_INPUT:
        case OBJECT_MULTI_STATE_OUTPUT:
        case OBJECT_MULTI_STATE_VALUE:
        {
            *peData_Type = BACNET_DT_UNSIGNED;
        }
        break;

#ifdef BACDEL_OBJ_CAL
        case OBJECT_CALENDAR:
        {
            *peData_Type = BACNET_DT_BOOLEAN;
        }
        break;
#endif

#ifdef BACDEL_OBJ_SDL
        case OBJECT_SCHEDULE:
        {
            *peData_Type = BACNET_DT_SCHEDULE_PRESENT_DEFAULT;
        }
        break;
#endif
        default:
        {
            /* default data type is NULL */
            *peData_Type = BACNET_DT_NULL;
            *pvReadPropValue = NULL;
        }
        break;
    }/* End of Switch*/

    /* Explicitly changing Write flag as false, It only need to be true
    at the time of write validation */
    g_bWriteRequest = false;

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    ReadPresentValueProperty : Exit \r\n");     
	#endif
}

/**
*DESCRIPTION
*   Validates is array index is empty or not
*
*@param u32ArrayIndex        [in]  Array index
*@param bIsArrayIndexPresent [in]  Indicates if array index is present or not.
*@return BACNET_ERROR_CODE   [out] ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY
*
*/
BACNET_ERROR_CODE IsNoIndex(uint32_t u32ArrayIndex, bool bIsArrayIndexPresent)
{
    /* Function Return Type */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER; 

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:IsNoIndex : Entry \r\n"); 
	#endif

    /* Validate Array Index */
    if(bIsArrayIndexPresent)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:IsNoIndex :"
            "ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY\r\n"); 
		#endif

        eErrorCode = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:IsNoIndex : Exit \r\n"); 
	#endif
    return eErrorCode;
}


/**
*DESCRIPTION
*   Based on object type & property type, validate the access type and 
*   and array index.
*   - api returns the aoffset address of property base structure.
*
*@param eObjectProperty     [in] Property Type
*@param eObjType            [in] Object Type
*@param pvObject            [in] Object base address 
*@param u32ArrayIndex       [in] Array Index to validate
*@param pul32PropOffsetAddr [out]   Returns base address of property struct.
*@param bIsArrayIndexPresent [in]   indicates if array index was present in request.
*
*@return BACNET_ERROR_CODE [out] ERROR_CODE_OTHER on success.
*
*/
BACNET_ERROR_CODE ValidateAccessAndIndex(
                BACNET_PROPERTY_ID eObjectProperty, BACNET_OBJECT_TYPE eObjType,
                uint32_t u32ArrayIndex, void *pvObject,
                ulong32_t *pul32PropOffsetAddr, bool bIsArrayIndexPresent)
{
    /* Function Return Type */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER; 
    /* Offset address of the object property */
    ulong32_t ul32OffsetAddr = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ValidateAccessAndIndex: Entry \r\n");
	#endif

    /* NOTE: do no validate pvObject for null value here */

    /* validate access type */
    eErrorCode = ValidateAccessType(eObjectProperty, eObjType, 
        pvObject, &ul32OffsetAddr);

    if(ERROR_CODE_OTHER == eErrorCode)
    {
        /* validate array index */
        eErrorCode = ValidateArrayIndex(eObjectProperty, eObjType, 
            u32ArrayIndex, pvObject, bIsArrayIndexPresent, ul32OffsetAddr);
    }

    /* return the property base address */
    if(NULL != pul32PropOffsetAddr)
        *pul32PropOffsetAddr = ul32OffsetAddr;

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:ValidateAccessAndIndex : Exit \r\n");
	#endif
    return eErrorCode;
}
                                             

/**
*
*DESCRIPTION
*   Based on object type & property type, validates array index.
*
*@param eObjectProperty     [in]    Property Type
*@param eObjType            [in]    Object Type
*@param pvObject            [in]    Object base address 
*@param u32ArrayIndex       [in]    Array Index
*
*@return BACNET_ERROR_CODE  [out]   ERROR_CODE_OTHER on success.
*
*/
BACNET_ERROR_CODE ValidateArrayIndex(
                BACNET_PROPERTY_ID eObjectProperty,
                BACNET_OBJECT_TYPE eObjType,
                uint32_t u32ArrayIndex,
                void *pvObject, bool bIsArrayIndexPresent,
				ulong32_t ul32PropOffsetAddr)
{
    /* Function Return Type */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER; 
    /* Offset address of the object property */
    ulong32_t ul32BaseAddr = 0;
    ulong32_t ul32OffsetAddr = 0;
    /* Used for validating number of states */
    uint32_t u32Count = 0;
    /* Device object struct */
    devObject_t *pstDevObj = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ValidateArrayIndex: entry \r\n");     
	#endif

    /* get the object base address */
    ul32BaseAddr = (ulong32_t)pvObject;

    /* array type properties require array index validations */
    switch(eObjectProperty)
    {
        case PROP_OBJECT_LIST:
        {
            /* check input pointer */
            if(NULL == pvObject)
            {
                eErrorCode = ERROR_CODE_INTERNAL_ERROR;
                break;
            }
            /* get device base address */
            pstDevObj = pvObject;
            u32Count = pstDevObj->m_stObjectIDList.m_u32ObjCount;
            /* validate array index */
            if(u32ArrayIndex > u32Count && bIsArrayIndexPresent)
                eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
        }
        break;

#ifdef OPTIONAL_PROPERTY
        case PROP_SLAVE_PROXY_ENABLE:
        {
            /* TODO */
            /* validate array index */
        }
        break;

        case PROP_AUTO_SLAVE_DISCOVERY:
        {
            /* TODO */
            /* validate array index */
        }
        break;

        /* state text */
#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
        case PROP_STATE_TEXT:
        {
            /* check input pointer */
            if(NULL == pvObject)
            {
                eErrorCode = ERROR_CODE_INTERNAL_ERROR;
                break;
            }
            /* get offset */
            GET_MULTISTATE_PROP_OFFSET(eObjType, m_stNumberOfStates.m_u32Val,
                ul32OffsetAddr);
            memcpy((void *)&u32Count,(void *)(ul32BaseAddr+ul32OffsetAddr),
                sizeof(uint32_t));
            /* validate array index */
            if(u32ArrayIndex > u32Count && bIsArrayIndexPresent)
                eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
        }
        break;
#endif /* Multistate objects */

#endif /* OPTIONAL_PROPERTY */

        /* priority array */
        case PROP_PRIORITY_ARRAY:
        {
            /* validate array index */
            if(u32ArrayIndex > MAX_PRIORITY_ARRAY_COUNT && bIsArrayIndexPresent)
                eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
        }
        break;

		/* event time stamp & event message texts */
		case PROP_EVENT_TIME_STAMPS:
        case PROP_EVENT_MESSAGE_TEXTS:
#ifdef BACDEL_OBJ_NC
        /* priority for NC object */
        case PROP_PRIORITY:
#endif
		{
			/* validate array index */
            if(u32ArrayIndex > BACNET_ARRAY_OF_THREE && bIsArrayIndexPresent)
                eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
		}
		break;

#ifdef BACDEL_OBJ_SDL
        /* weekly schedule */
        case PROP_WEEKLY_SCHEDULE:
        {
            /* validate array index */
            if(u32ArrayIndex > BACNET_ARRAY_OF_SEVEN && bIsArrayIndexPresent)
                eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
        }
        break;

        /* exception schedule */
        case PROP_EXCEPTION_SCHEDULE:
        {
            Pr_ListOfBACnetSpecialEvent_t *pstExceptionSchedule = NULL;
            ListOfSpecialEvent_t *pstSplEvent = NULL; 

            /* check input pointer */
            if(NULL == pvObject)
            {
                eErrorCode = ERROR_CODE_INTERNAL_ERROR;
                break;
            }
            /* get offset */
            GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stExceptionSchedule,ul32OffsetAddr);
            pstExceptionSchedule = (void *)(ul32BaseAddr+ul32OffsetAddr);
            pstSplEvent = pstExceptionSchedule->m_pstSplEvent;
            /* count */
            while(NULL != pstSplEvent)
            {
                u32Count++;
                pstSplEvent = pstSplEvent->m_pstNext;
            }
            /* validate array index */
            if(u32ArrayIndex > u32Count && bIsArrayIndexPresent)
                eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
        }
        break;
#endif /* BACDEL_OBJ_SDL */

#ifdef BACDEL_SER_AE_EN_B
        case PROP_ALARM_VALUES:
        {
            /* check as per object type */
            switch(eObjType)
            {

            /* chceck for no index */
            default:
                /* validate array index */
                eErrorCode = IsNoIndex(u32ArrayIndex, bIsArrayIndexPresent);
                break;
            }//switch ends.
        }
        break;

		case PROP_FAULT_VALUES:
        {
            /* check as per object type */
            switch(eObjType)
            {

            /* chceck for no index */
            default: 
                /* validate array index */
                eErrorCode = IsNoIndex(u32ArrayIndex, bIsArrayIndexPresent);
                break; 
            }//switch ends. 
        } 
        break; 
#endif /* BACDEL_SER_AE_EN_B */
#ifdef BACDEL_PR14
		case PROP_PROPERTY_LIST:
        {
			/* local variable */
			Pr_BACnetPropertyList_t *pstPropertyList = NULL;
            /* check input pointer */
            if(NULL == pvObject)
            {
                eErrorCode = ERROR_CODE_INTERNAL_ERROR;
                break;
            }

			/* get property list property address */
			pstPropertyList = (void *)(ul32BaseAddr + ul32PropOffsetAddr);
			/* get count value of property list property */
			u32Count = pstPropertyList->m_u32PropCount;
			/* validate array index */
			if(u32ArrayIndex > u32Count && bIsArrayIndexPresent)
			{
				eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
			}
        }
		break;
#endif /* PR14 */

#ifdef BACDEL_OBJ_NP
		/* subordinate list */
        case PROP_IP_DNS_SERVER:
        {
			/* local variable */
            Pr_ListOfOctetStr_t *pstIpDnsSrvList = NULL;
            /* check input pointer */
            if(NULL == pvObject)
            {
                eErrorCode = ERROR_CODE_INTERNAL_ERROR;
                break;
            }

            /* get ip dns server property address */
            pstIpDnsSrvList = (Pr_ListOfOctetStr_t *)(ul32BaseAddr + ul32PropOffsetAddr);
            /* get count value of property list property */
			u32Count = pstIpDnsSrvList->m_u32Count;
			/* validate array index */
			if(u32ArrayIndex > u32Count && bIsArrayIndexPresent)
			{
				eErrorCode = ERROR_CODE_INVALID_ARRAY_INDEX;
			}
        }
        break;
#endif /* NP */
        /* for all other properties array index should be -1 (default value) */
        default:
        {
            /* validate array index */
            eErrorCode = IsNoIndex(u32ArrayIndex, bIsArrayIndexPresent);
        }
        break;
    }//switch ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ValidateArrayIndex: exit \r\n");
	#endif
    return eErrorCode;
}

/**
*
*DESCRIPTION
*   Based on object type & property type, validates array index.
*
*@param eObjectProperty     [in]    Property Type
*@param eObjType            [in]    Object Type
*@param pvObject            [in]    Object base address 
*@param pul32PropOffsetAddr [out]   Returns base address of property struct.
*
*@return BACNET_ERROR_CODE  [out]   ERROR_CODE_OTHER on success.
*
*/
BACNET_ERROR_CODE ValidateAccessType(
                BACNET_PROPERTY_ID eObjectProperty,
                BACNET_OBJECT_TYPE eObjType,
                void *pvObject,
                ulong32_t *pul32PropOffsetAddr)
{
    /* function return value */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER; 
    /* Offset address of the object property */
    ulong32_t ul32OffsetAddr = 0;
    /* property permission */
    PROP_ACCESS_TYPE ePremission = BACNET_DEFAULT;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ValidateAccessType: entry \r\n");
	#endif

    /* FIXME: write an api to get ul32OffsetAddr when USER_DEFINED_ACCESS_TYPE is
        not defined */

    /* get the access type of property for given object type */
    ePremission = GetAsignPropAccess(eObjType, eObjectProperty, pvObject, 
        BACNET_DEFAULT, false, &ul32OffsetAddr);
    if(NOT_SUPPORTED == ePremission)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: ValidateAccessType:\
        property is not supported. \r\n");
		#endif
        /* return unknown property */
        eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;  
    }

    /* return property base address */
    if(NULL != pul32PropOffsetAddr)
        *pul32PropOffsetAddr = ul32OffsetAddr;

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ValidateAccessType: exit \r\n");
	#endif
    return eErrorCode;
}

#ifndef PROFILE_BAWS_ONLY
/**
*DESCRIPTION
*   This API Read Common Properties and fill the Response buffer with the data
*   Note: Add cases for new property Add support
*   
*@param pstRpData [in/out] Read Property data structure which 
*       will give object type to read & carry application data.
*@param pvObject [in] Pointer to void - Accept Any Type of Object
*
*@return BACNET_RETURN_TYPE [out] BACDEL_SUCCESS-> If Property found
*                                 BACDEL_ERROR-> If property not found
*
*/
BACNET_ERROR_CODE Read_Common_Properties(BACNET_OBJECT_TYPE eObjectType,
                                         BACNET_PROPERTY_ID eObjectProperty ,
                                         uint32_t u32ArrayIndex,
                                         BACNET_DATA_TYPE *peData_Type ,
                                         void **pvReadPropValue, 
                                         void *pvObject,
										 void *pVirtualDev,
                                         bool bArrayIndexPresent)
{
    /* locals */
    ulong32_t ul32BaseAddr = 0;     /* For Base struct Addr */
    ulong32_t ul32OffsetAddr = 0 ;  /* For Data offset Addr */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Read_Common_Properties : Entry \r\n"); 
	#endif

    /* Get the Structure Base Address */
    ul32BaseAddr = (ulong32_t )pvObject;

    /* validate the array index & access type */
    eErrorCode = ValidateAccessAndIndex(eObjectProperty, eObjectType, u32ArrayIndex, 
        pvObject, &ul32OffsetAddr, bArrayIndexPresent);
    if(ERROR_CODE_OTHER != eErrorCode)
    {
        /* array index invalid or property not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
        Read_Common_Properties : ValidateAccessAndIndex fails. \r\n");
		#endif
        /* return error code */
        return eErrorCode;
    }

    /** Based on Object Property 
        Note: Assume that property existance care has been taken properly */
	/** Note: Assumed Every Property have Access Type as its first element
              ul32OffsetAddr can be used as Property Base Address */
    switch(eObjectProperty)
    {
        /* For Object Identifier */
        case PROP_OBJECT_IDENTIFIER:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_OBJECTID;
        }
        break;

        /* Object Name */
        case PROP_OBJECT_NAME:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_CHARSTRING;
        }
        break;

		/* Object type */
        case PROP_OBJECT_TYPE:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_OBJTYPE;
        }
        break;
        
        /* For Present Value */
        case PROP_PRESENT_VALUE:
        {
            /* Read Present value based on Object Type (i.e. Data Type) */
            ReadPresentValueProperty(eObjectType, ul32BaseAddr, ul32OffsetAddr, 
                peData_Type, pvReadPropValue, pVirtualDev);
        }
        break;

        /* For Object Status Flags */
        case PROP_STATUS_FLAGS:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_BITSTRING;
        }
        break;

        case PROP_EVENT_STATE:
        case PROP_POLARITY:
        case PROP_UNITS:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_ENUM_NEW;
        }
        break;
        
        /* For Object Out of service */
        case PROP_OUT_OF_SERVICE:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_BOOLEAN;
        }
        break;

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
        /* For No of States */
        case PROP_NUMBER_OF_STATES:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_UNSIGNED;
        }
        break;
#endif
        
        /* For Object priority array */
        case PROP_PRIORITY_ARRAY:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_PRIORITY_ARRAY; 
        }
        break;        
        
        /* For Object relinquish default value */
        case PROP_RELINQUISH_DEFAULT:
        {
			/* Update Data type for Tag at encoding time */
            UpdateValuePropDataType( eObjectType , peData_Type);
        }
        break;

#ifdef BACDEL_PR18
		case PROP_CURRENT_COMMAND_PRIORITY:
		{
			/* Update Data type for Tag at encoding time */
			*peData_Type = BACNET_DT_OPTIONAL_UNSIGNED;
		}
		break;
#endif

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
		/* For COV Increment */
        case PROP_COV_INCREMENT:
        {
            switch(eObjectType)
            {
				
				default:
					/* Update Data type for Tag at encoding time */
					*peData_Type = BACNET_DT_REAL;
				break;
            }
        }
        break;        
#endif /* (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B) */

#ifdef OPTIONAL_PROPERTY
        case PROP_DESCRIPTION:
        case PROP_DEVICE_TYPE:
        case PROP_PROFILE_NAME:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_CHARSTRING;
        }
        break;

#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV)
		case PROP_MIN_PRES_VALUE:
        case PROP_RESOLUTION:
		{
			/* save data type as per object type */
			switch (eObjectType)
			{
				
				default:
					/* Update Data type for Tag at encoding time */
					*peData_Type = BACNET_DT_REAL;
				break;
			}
		}
        break;

#endif /* AI - AO */

#if (defined BACDEL_OBJ_AI )
        case PROP_UPDATE_INTERVAL:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_UNSIGNED;
        }
        break;
#endif
        
#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
		/* For Object  Active / Inactive Text */
        case PROP_INACTIVE_TEXT:
        case PROP_ACTIVE_TEXT:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_CHARSTRING;
        }
        break;        
        
        case PROP_CHANGE_OF_STATE_TIME:
        case PROP_TIME_OF_STATE_COUNT_RESET:
        case PROP_TIME_OF_ACTIVE_TIME_RESET:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_DATETIME;                  
        }
        break;
       
        /* For Object present state change count  */
        case PROP_CHANGE_OF_STATE_COUNT:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_UNSIGNED16;
        }
        break;
#endif /* BI - BO - BV */

        /* For Object present active state change elapse time */
        case PROP_ELAPSED_ACTIVE_TIME:
        {
			uint32_t u32PropertyPtr = 0;
			void *pvDummy = NULL;
			PROP_ACCESS_TYPE ePermission = 0;

			/* Get offset of property Access Type */
			GET_BINARYONLY_OFFSET(eObjectType,m_stElaspsedActiveTime.stActiveElapseTime,
			ul32OffsetAddr);

			/* Set value of property Access Type */
			memcpy((int8_t *)&ePermission,(void *)(ul32BaseAddr+ul32OffsetAddr),
			sizeof(PROP_ACCESS_TYPE));

			/* call update common propperty to Set Point value */
			UpdateCommonProperty(eObjectType, pvObject, PROP_ELAPSED_ACTIVE_TIME, u32ArrayIndex, NULL, ePermission,
			&u32PropertyPtr, &pvDummy, pVirtualDev, false, false, false);

			/* Update Data type for Tag at encoding time */
			*peData_Type = BACNET_DT_UNSIGNED32;
        }
        break;

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
         /* For Multistate Text */
        case PROP_STATE_TEXT:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_CHARSTRING_ARRAY;
        }
        break; 
#endif /* MSI - MSO - MSV */

        /* For minimum on / off time for the Object to be inactive state */
        case PROP_MINIMUM_OFF_TIME:
        case PROP_MINIMUM_ON_TIME:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_UNSIGNED32;
        }
        break;
#endif /* OPTIONAL_PROPERTY */     

#if (defined OPTIONAL_PROPERTY )
        case PROP_MAX_PRES_VALUE:
        {

				*peData_Type = BACNET_DT_REAL;
        }
        break;
#endif

#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B || defined BACDEL_OBJ_SDL)
        /* For Object Reliability */
        case PROP_RELIABILITY:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_ENUM_NEW;
        }
        break; 
#ifdef BACDEL_PR14
		/* For Object Reliability evaluation inhibit */
        case PROP_RELIABILITY_EVALUATION_INHIBIT:
        {
            /* Update Data type for Tag at encoding time */
			*peData_Type = BACNET_DT_BOOLEAN;
        }
        break; 
#endif
#endif
        
#ifdef BACDEL_SER_AE_EN_B

        /* For Object minimum time delay for present value to generate
         alarm/event */
        case PROP_TIME_DELAY:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_UNSIGNED;
        }
        break;

		case PROP_HIGH_LIMIT:
        {
            /* Update Data type as per object type */     
            switch(eObjectType)
            {
                /* unsigned value */
				case OBJECT_ACCUMULATOR:
					*peData_Type = BACNET_DT_UNSIGNED32;
                break;
                /* default is real value */
				default:
					*peData_Type = BACNET_DT_REAL;
                break;
            }
        }
        break;

        case PROP_LOW_LIMIT:
        {
            /* Update Data type as per object type */     
            switch(eObjectType)
            {
                /* unsigned value */
				case OBJECT_ACCUMULATOR:
					*peData_Type = BACNET_DT_UNSIGNED32;
                break;
                /* default is real value */
				default:
					*peData_Type = BACNET_DT_REAL;
                break;
            }
        }
        break;

        case PROP_DEADBAND:
        {
            /* Update Data type as per object type */     
            switch(eObjectType)
            {
                /* default is real value */
				default:
					*peData_Type = BACNET_DT_REAL;
                break;
            }
        }
        break;

        case PROP_LIMIT_ENABLE:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_BITSTRING;
        }
        break;

        /* For Object Feed back value after write */
        case PROP_FEEDBACK_VALUE:
        {
            UpdateValuePropDataType( eObjectType , peData_Type);
        }
        break;

        /* For Object Alarm values property after */
        case PROP_ALARM_VALUES:
        {
            /* return data type on the basis of object type */
            switch(eObjectType)
            {
                default :
        	        /* Update Data type for Tag at encoding time */
        	        *peData_Type = BACNET_DT_UNSIGNED_LIST;
                break;
            }
        }
        break;

        /* For Object Feed back value after write */
        case PROP_ALARM_VALUE:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_ENUM;
        }
        break;
        
        /* For Object Alarm values property after */
        case PROP_FAULT_VALUES:
        {
            /* return data type on the basis of object type */
            switch(eObjectType)
            {

                default :
        	        /* Update Data type for Tag at encoding time */
        	        *peData_Type = BACNET_DT_UNSIGNED_LIST;
                break;
            }
        }
        break;

		/* For Event message texts & textconfig property */
		case PROP_EVENT_MESSAGE_TEXTS:
#ifdef BACDEL_PR14
		case PROP_EVENT_MESSAGE_TEXTS_CONFIG:
#endif
        {
            /* Update Data type for Tag at encoding time */
			*peData_Type = BACNET_DT_EVENT_MSG_TEXT;
        }
        break; 
#endif /* BACDEL_SER_AE_EN_B */

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ESUM_A) || (defined BACDEL_SER_AE_ESUM_B))
        /* For Object Notification class */
        case PROP_NOTIFICATION_CLASS:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_UNSIGNED;
        }
        break;
#endif 
		/* ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) ||
	(defined BACDEL_SER_AE_ESUM_A) || (defined BACDEL_SER_AE_ESUM_B)) */

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B))        
        /* To read Object Event Enable */
        case PROP_EVENT_ENABLE:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_BITSTRING;
        }
        break;
#endif /* ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B)) */
        
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_A))
        /* To read Object Ackonowledge transactions */
        case PROP_ACKED_TRANSITIONS:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_BITSTRING;
		}
        break;
#endif 

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ASUM_B))        
        /* For Object Notification type */
        case PROP_NOTIFY_TYPE:
        {
            /* Update Data type for Tag at encoding time */
            *peData_Type = BACNET_DT_ENUM_NEW;
        }
        break;
#endif 
		/* ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || 
	(defined BACDEL_SER_AE_ASUM_B)) */

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B))		        
        /* For Object Event Time Stamps
        Note: It return 3 bytes of arry with the datetime variable for
        following three event types 
        TO-OFFNORMAL
        TO-FAULT
        TO-NORMAL */
        case PROP_EVENT_TIME_STAMPS:
        {
			/* Update Data type for Tag at encoding time */
			*peData_Type = BACNET_DT_TIMESTAMP_ARRAY;
        }
        break;
#endif 
		/* ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B)) */

#ifdef BACDEL_OBJ_NC
		case PROP_PRIORITY:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_NOTIFICATIONPRIORITY;
        }
        break;

		case PROP_ACK_REQUIRED:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_BITSTRING;
        }
        break;

		case PROP_RECIPIENT_LIST:
        {
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_DESTINATION_LIST;
        }
        break;
#endif /* BACDEL_OBJ_NC */

#ifdef BACDEL_OBJ_CAL
        case PROP_DATE_LIST:
        {
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_DATELIST;
        }
        break;
#endif /* BACDEL_OBJ_CAL */

#ifdef BACDEL_OBJ_SDL
        case PROP_EFFECTIVE_PERIOD:
        {			
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_DATERANGE;
        }
        break;

        case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
        {
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_DEVOBJPROPREFF_LIST;
        }
        break;

        case PROP_WEEKLY_SCHEDULE:
        {
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_DAILYSCHEDULE_ARRAY;
        }
        break;

        case PROP_EXCEPTION_SCHEDULE:
        { 
                /* Update Data type for Tag at encoding time */        
                *peData_Type = BACNET_DT_SPECIALEVENT_ARRAY;
        }
        break;

        case PROP_SCHEDULE_DEFAULT:
        {			
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_SCHEDULE_PRESENT_DEFAULT;
        }
        break;
#endif /* BACDEL_OBJ_SDL */

#if (defined BACDEL_OBJ_SDL)
        case PROP_PRIORITY_FOR_WRITING:
        {			
            /* Update Data type for Tag at encoding time */        
            *peData_Type = BACNET_DT_UNSIGNED32;
        }
        break;
#endif

#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_PR14) 
		case PROP_EVENT_DETECTION_ENABLE:
		case PROP_EVENT_ALGORITHM_INHIBIT:
        {
            /* Update Data type for Tag at encoding time */           
            *peData_Type = BACNET_DT_BOOLEAN;
        }
        break;
#ifdef OPTIONAL_PROPERTY
		/* For time delay normal */
		case PROP_TIME_DELAY_NORMAL:
        {
            /* Update Data type for Tag at encoding time */           
			*peData_Type = BACNET_DT_UNSIGNED;
        }
        break;
#endif
#endif /* EN-B && PR14 */

        default:
        {
            /* No Access Type support */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Read_Common_Properties :"
                "ObjectType %d with PropID %d Not Found\r\n",
                eObjectType, eObjectProperty); 
			#endif

            /* Retrun Special case */
            eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;
        }
        break;
    }
    
    /* Updated void pointer only if there is no any error while reading */
    if(eErrorCode == ERROR_CODE_OTHER) 
    {
        if((PROP_PRESENT_VALUE != eObjectProperty)&&
            (PROP_ELAPSED_ACTIVE_TIME != eObjectProperty))
        {
            *pvReadPropValue = 
                (void *)(ul32BaseAddr+ul32OffsetAddr);
        }
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Read_Common_Properties: Exit \r\n");     
	#endif
    return eErrorCode;
 }
#endif /* !PROFILE_BAWS_ONLY */

/**
* DESCRIPTION
* function returns false if the property can't be read using RP or RPM service.
*
* @param ePropId    [in]   property id.
* @param eObjType   [in]   object type.
* @return bRetVal   [out]  true if readable else false.
*
*/
bool IsPropertyReadable(
    BACNET_PROPERTY_ID ePropId, 
    BACNET_OBJECT_TYPE eObjType)
{
    /* function return value */
    bool bRetVal = true; 

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: IsPropertyReadable: Entry \r\n"); 
	#endif
    
    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: IsPropertyReadable: Exit \r\n"); 
	#endif
    return bRetVal;
}

#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
BACNET_POLARITY GetPolarity(ulong32_t ul32BaseAddr, 
                            BACNET_OBJECT_TYPE eObjectType)
{
   
   BACNET_POLARITY  ePolarity = POLARITY_NORMAL;
   ulong32_t        ul32OffsetAddr = 0;
   ulong32_t        ul32TotalAddr = 0;

   /* function entry */
   #ifdef DEBUG_PRINTF
   Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:GetPolarity : Entry \r\n"); 
   #endif
   
   if(OBJECT_BINARY_INPUT == eObjectType || OBJECT_BINARY_OUTPUT == eObjectType)
   {
        /* Note: ul32OffsetAddr Data going to change here */
        GET_POLARITY_OFFSET(eObjectType,m_stPolarity.m_ePolarity,
                                                ul32OffsetAddr);
        ul32TotalAddr = ul32BaseAddr+ul32OffsetAddr;
        memcpy((void *)&ePolarity, 
               (void *)(ul32TotalAddr), 
               sizeof(BACNET_POLARITY));
    }
    else
    {
        /* There is no polarity for binary value. Even if ask for
           polarity by default it must be normal */
        ePolarity = 0;
    }
   /* function exit */
   #ifdef DEBUG_PRINTF
   Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:GetPolarity : Exit \r\n");
   #endif
   return ePolarity;
}


BACNET_POLARITY Update_PV_Polarity(ulong32_t ul32BaseAddr, 
                            BACNET_OBJECT_TYPE eObjectType)
{
    ulong32_t ul32OffsetAddr = 0 ;  /* For Data offset Addr */
    BACNET_POLARITY  ePolarity = POLARITY_NORMAL;

    /* If OS changes effect of polarity For BI and BO changes */
    if(eObjectType == OBJECT_BINARY_INPUT || eObjectType == OBJECT_BINARY_OUTPUT )
    {
        /* Get Present Value Polarity offset */
        GET_BINARYONLY_OFFSET(eObjectType,m_stPresentValue.m_ePVPolarity ,
                                             ul32OffsetAddr);

        /* Polarity works Only if Out of service is FALSE */
        if(!IsOutofSerivceFlagTrue(ul32BaseAddr, eObjectType))
        {
            /* If polarity is in POLARITY_REVERSE mode */
            if(GetPolarity(ul32BaseAddr, eObjectType))
            {
                ePolarity = POLARITY_REVERSE;
            }
        }/* End of Outofservice validation */

        /* Copy data into property offset address */
        memcpy((void *)(ul32BaseAddr+ul32OffsetAddr), &ePolarity,
                           sizeof(BACNET_POLARITY));
    }

    return ePolarity;
}
#endif /* BI - BO - BV */



/**
*                                                                         
*DESCRIPTION                                                                          
*        This API fills in the required parameters in RP data structure necessary for 
*reading object property.
*    
*@param u32ObjId The[In] The object Identifier for which RP request to be sent.
*@param u8ObjectType[In] The type of object for which RP request to be sent.
*@param u8PropId[In] The Object property field which is to be read.
*@param pRpData [Out] The output pointer containing required filled in contains of RP 
*                   data
*
*@return None
*/
void Generate_RPB_Data(uint32_t u32ObjId, 
                       uint8_t u8ObjectType, 
                       uint8_t u8PropId, 
                       int32_t i32ArrayIndex,  
                       BACNET_CONF_DATA *pRpData,
                       bool bIsArrayIndexPresent)
{   
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Generate_RPB_Data : Entry \r\n"); 
	#endif

    memset(pRpData, 0, sizeof(BACNET_CONF_DATA));
    pRpData->eObjectType = u8ObjectType;
    pRpData->u32ObjectInstance = u32ObjId;
    pRpData->eObjectProperty = u8PropId;
    pRpData->u32ArrayIndex = i32ArrayIndex; 
    if(ARRAY_INDEX_PRESENT == bIsArrayIndexPresent)
        pRpData->bArrIndxPresent = ARRAY_INDEX_PRESENT;
    pRpData->eErrorClass = ERROR_CLASS_DEVICE;
    pRpData->eErrorCode = ERROR_CODE_OTHER;

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Generate_RPB_Data : Exit \r\n"); 
	#endif
}

/**
*                                                                         
*DESCRIPTION
*   This API Reads all properties of all object types and 
*   returns the property data type & base address.
*   
*@param eObjectType [in] Object type.
*@param eObjectProperty [in] Property Id.
*@param i32ArrayIndex [in] Property array index to be read.
*@param pvObject [in] Pointer to void - Accept Any Type of Object.
*@param pVirtualDev [in] Virtual device pointer.
*@param u32ObjId [in] Id of object. NOTE :this parameter is of no use for now.
*
*@param peData_Type [out] returns the property data type.
*@param pvReadPropValue [out] returns the base address of property struct.
*
*@return BACNET_ERROR_CODE [out] Error code other on success, else respective
*                                   error code.
*/
BACNET_ERROR_CODE Read_All_Properties(BACNET_OBJECT_TYPE eObjectType,
          BACNET_PROPERTY_ID eObjectProperty, uint32_t u32ArrayIndex,
          BACNET_DATA_TYPE *peData_Type, void **pvReadPropValue, 
          void *pvObject, void *pVirtualDev, uint32_t u32ObjId,
          bool bArrayIndexPresent)
{
    /* Local Variables */
    BACNET_ERROR_CODE   eErrorCode = ERROR_CODE_OTHER;
	BACNET_CONF_DATA	stRpData = {0};

    /* Function entry debug message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Read_All_Properties : Entry \r\n"); 
	#endif

    /* Check for input pointers */
    if(NULL == pVirtualDev || NULL == pvObject || NULL == pvReadPropValue
        || NULL == peData_Type)
        return ERROR_CODE_INTERNAL_ERROR;

    switch(eObjectType)
    {
    case OBJECT_DEVICE:
        /* Fill stRpData to read device prop */
		stRpData.eObjectProperty = eObjectProperty;
		stRpData.u32ArrayIndex = u32ArrayIndex;
        stRpData.bArrIndxPresent = bArrayIndexPresent;
		stRpData.eObjectType = eObjectType;
		stRpData.u32ObjectInstance = u32ObjId;
		/* Read device object property */
		eErrorCode = Dv_Read_Prop(&stRpData, pVirtualDev);
        /* Get property base address */
		*pvReadPropValue = stRpData.pvReadPropValue;
         /* Get property  data type */
		*peData_Type = stRpData.eData_Type;
        break;

    default:
#ifndef PROFILE_BAWS_ONLY
        /* All other object's properties can be read with this api */
        eErrorCode = Read_Common_Properties(eObjectType, eObjectProperty,
					u32ArrayIndex, peData_Type, pvReadPropValue,
					pvObject, pVirtualDev, bArrayIndexPresent);
#endif
        break;
    }

    /* Function exit debug message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Read_All_Properties : Exit \r\n"); 
	#endif
    return eErrorCode;
}

/**
*   Api to check if property is of type list or array of list.
*
*   @param ePropId [in] property id.
*   @param eObjectType [in] object type.
*   @returns error if property is not list type.
*
*/
BACNET_ERROR_CODE ValidateListTypeProperty(
    BACNET_PROPERTY_ID ePropId, BACNET_OBJECT_TYPE eObjectType)
{
    /* Local Variables */
    BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

    /* Function entry debug message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    ValidateListTypeProperty : Entry \r\n"); 
	#endif

    switch(ePropId)
    {
        /* Device */
    case PROP_VT_CLASSES_SUPPORTED:
    case PROP_ACTIVE_VT_SESSIONS:
    case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
    case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
    case PROP_DEVICE_ADDRESS_BINDING:
    case PROP_ACTIVE_COV_SUBSCRIPTIONS:
    case PROP_MANUAL_SLAVE_ADDRESS_BINDING:
    case PROP_SLAVE_ADDRESS_BINDING:
    case PROP_RESTART_NOTIFICATION_RECIPIENTS:
        /* Calendar */
    case PROP_DATE_LIST:
        /* Notification */
    case PROP_RECIPIENT_LIST:
        /* Schedule */
    case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
    case PROP_WEEKLY_SCHEDULE:      /* array of list */
    case PROP_EXCEPTION_SCHEDULE:   /* array of list */
        /* Trend Log */
    case PROP_LOG_BUFFER:
		/* Life Safety Point - Zone */
	case PROP_MEMBER_OF:
	case PROP_ZONE_MEMBERS:
	case PROP_LIFE_SAFETY_ALARM_VALUES:
		/* Group */
    case PROP_LIST_OF_GROUP_MEMBERS:
        break;

        /* Properties with multiple data type depending on object type */
        /* list type for - MSI & MSV */
        /* array type for - BSV */
    case PROP_ALARM_VALUES:
	{
		switch(eObjectType)
		{
			/* property is list type for below objects */
			case OBJECT_MULTI_STATE_INPUT:
			case OBJECT_MULTI_STATE_VALUE:
			case OBJECT_ACCESS_ZONE:
			case OBJECT_ACCESS_DOOR:
			case OBJECT_LIFE_SAFETY_POINT:
			case OBJECT_LIFE_SAFETY_ZONE:
				break;
			/* property is array type or not supported for all other objects */
			default:
				eErrorCode = ERROR_CODE_PROPERTY_IS_NOT_A_LIST; 
				break;
		}
	}
	break;

		/* Properties with multiple data type depending on object type */
    case PROP_FAULT_VALUES:
	{
		switch(eObjectType)
		{
			/* property is list type for below objects */
			case OBJECT_MULTI_STATE_INPUT:
			case OBJECT_MULTI_STATE_VALUE:
			case OBJECT_ACCESS_DOOR:
			case OBJECT_LIFE_SAFETY_POINT:
			case OBJECT_LIFE_SAFETY_ZONE:
				break;
			/* property is array type or not supported for all other objects */
			default:
				eErrorCode = ERROR_CODE_PROPERTY_IS_NOT_A_LIST; 
				break;
		}
	}
	break;

        /* Default */
    default:
        eErrorCode = ERROR_CODE_PROPERTY_IS_NOT_A_LIST;
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
        ValidateListTypeProperty : this is not a list or array of list type property. \r\n");
		#endif
        break;
    }

    /* Function exit debug message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    ValidateListTypeProperty : Exit \r\n"); 
	#endif
    return eErrorCode;
}

/**
*******************************************************************************
*                                                                         
*DESCRIPTION                                                                          
*   This function is used to find single property in property list
*   as per Required, Optional or All properties
*    
*@param pstPropertyList     [in]    Reference to the structure which holds list of
*                                   Required, Optional, and Proprietary object
*                                   properties with their counts.
*@param eSpecialProperty    [in]    special property identifier(ALL,REQUIRED,OPTIONAL)
*@param u32Index            [in]    index of property list
*
*@return    Property identifier of property present at given index in list
*
******************************************************************************/
BACNET_PROPERTY_ID RPM_Object_Property(
    special_property_list_t *pstPropertyList,
    BACNET_PROPERTY_ID eSpecialProperty,
    uint32_t u32Index)
{
    int32_t i32Property = -1;  /* return value */
    uint32_t u32Required, u32Optional, u32Proprietary;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Object_Property: entry \r\n");
	#endif

    u32Required = pstPropertyList->Required.count;
    u32Optional = pstPropertyList->Optional.count;
    u32Proprietary = pstPropertyList->Proprietary.count;
    
    if (eSpecialProperty == PROP_ALL) 
    {
        if (u32Index < u32Required) 
        {
            i32Property = pstPropertyList->Required.pList[u32Index];
        } 
        else if (u32Index < (u32Required + u32Optional)) 
        {
            u32Index -= u32Required;
            i32Property = pstPropertyList->Optional.pList[u32Index];
        } 
        else if (u32Index < (u32Required + u32Optional + u32Proprietary)) 
        {
            u32Index -= (u32Required + u32Optional);
            i32Property = pstPropertyList->Proprietary.pList[u32Index];
        }
    } 
    else if (eSpecialProperty == PROP_REQUIRED) 
    {
        if (u32Index < u32Required) 
        {
            i32Property = pstPropertyList->Required.pList[u32Index];
        }
    } 
    else if (eSpecialProperty == PROP_OPTIONAL) 
    {
        if (u32Index < u32Optional) 
        {
            i32Property = pstPropertyList->Optional.pList[u32Index];
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Object_Property: exit \r\n");
	#endif
    return (BACNET_PROPERTY_ID) i32Property;
}

/**
*******************************************************************************
*                                                                         
*DESCRIPTION                                                                          
*   This function is used to find single property in property list
*   as per Required, Optional or All properties
*    
*@param pstPropertyList     [in]    Reference to the structure which holds list of
*                                   Required, Optional, and Proprietary object
*                                   properties with their counts.
*@param eSpecialProperty    [in]   special property identifier(ALL,REQUIRED,OPTIONAL)
*
*@return    count of properties present in a list
*
******************************************************************************/
uint32_t RPM_Object_Property_Count(
    special_property_list_t *pstPropertyList,
    BACNET_PROPERTY_ID eSpecialProperty)
{
    uint32_t u32PropCnt = 0; /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Object_Property_Count: entry \r\n");
	#endif

    if(NULL == pstPropertyList)
        return 0;

    if (eSpecialProperty == PROP_ALL) 
    {
        u32PropCnt =
            pstPropertyList->Required.count + pstPropertyList->Optional.count +
            pstPropertyList->Proprietary.count;
    } 
    else if (eSpecialProperty == PROP_REQUIRED) 
    {
        u32PropCnt = pstPropertyList->Required.count;
    } 
    else if (eSpecialProperty == PROP_OPTIONAL) 
    {
        u32PropCnt = pstPropertyList->Optional.count;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Object_Property_Count: exit \r\n");
	#endif
    return u32PropCnt;
}

/**
*
* DESCRIPTION
* Function reads RP callback config value based on property ID and object ID.
*
* @param eObjectType		[in] Object Type
* @param eObjectProperty	[in] Property Id
*
* @return BACNET_CALLBACK_CONFIG_TYPE  [out] Callback config value
*
*/
BACNET_CALLBACK_CONFIG_TYPE GetPropertyRpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty)
{    
    /* local variables */
    BACNET_REMAPPED_OBJECT_TYPE  eRemapObjType = REMAPPED_MAX_BACNET_OBJECT_TYPE;
    BACNET_CALLBACK_CONFIG_TYPE ePropCBType = MAX_CALLBACK_CONFIG_TYPE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	GetPropertyRpCallback : Entry \r\n");
	#endif

    /* remap object type value */
    eRemapObjType = Remap_Objct_Id(eObjectType);

    if(eRemapObjType != REMAPPED_MAX_BACNET_OBJECT_TYPE &&
        eObjectProperty < MAX_PROP_SUPPORTED)
    {
        /* get the callback type of property */
        //ePropCBType = au8RpCallback[eObjectProperty][eRemapObjType];
			ePropCBType = CALLBACK_CONFIG_REQUIRED;//CALLBACK_CONFIG_NOT_REQUIRED;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	GetPropertyRpCallback : Exit \r\n");
	#endif

	/* return data type value */
    return ePropCBType;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to copy list type or array (array of N) type properties. 
*    
* @param pvSrcPtr		 [in]	Pointer of source list
* @param eData_Type		 [in]	property data type
* @param pvReadPropValue [out]	pointer of destination list. 
*                                   
* @return MAX_BACNET_ERROR_CODE on success.
*	
*/
BACNET_ERROR_CODE Copy_List_Type_properties(
	void *pvSrcPtr,
	BACNET_DATA_TYPE eDataType,
	void **pvReadPropValue
	)
{
	/* local parameters */
	BACNET_ERROR_CODE eErrCode = MAX_BACNET_ERROR_CODE;
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Copy_List_Type_properties: Entry \r\n");     
	#endif

	/* check datatype */
	switch(eDataType)
	{
	case BACNET_DT_COVSUB:
	case BACNET_DT_RECIPIENT_LIST:
	case BACNET_DT_ADDRESSBINDING_LIST:
	case BACNET_DT_OBJECTID_ARRAY: 
	case BACNET_DT_CHARSTRING_ARRAY: 
	case BACNET_DT_UNSIGNED_LIST:
	case BACNET_DT_DESTINATION_LIST: 
	case BACNET_DT_DATELIST:	
	case BACNET_DT_DEVOBJPROPREFF_LIST: 
	case BACNET_DT_SPECIALEVENT_ARRAY: 
	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	
	case BACNET_DT_LOGBUFFER_TRENDLOG:
	case BACNET_DT_LOGBUFFER_TLM:
	case BACNET_DT_LOGBUFFER_EL:
	case BACNET_DT_BOOLEAN_ARRAY:	
	case BACNET_DT_READACCESSSPECS_LIST:
	case BACNET_DT_READACCESSRESULT_LIST:
	case BACNET_DT_ENUM_LIST:
	case BACNET_DT_DEVOBJREFF_LIST:
	case BACNET_DT_DEVOBJREFF_ARRAY:
	case BACNET_DT_UNSIGNED_ARRAY:
	case BACNET_DT_PRIORITY_ARRAY:
	case BACNET_DT_BITSTRING_ARRAY:	
	case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:	
	case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
	case BACNET_DT_ACCESS_RULE_ARRAY:
	case BACNET_DT_CRED_AU_FACTOR_ARRAY:
	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
	case BACNET_DT_OPTCHARSTRING_ARRAY:	
	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
	{
		/* property is list or array type */
		/* create a copy of property value */
		eRetVal = Copy_PropVal_AsPer_DataType(pvSrcPtr, eDataType, pvReadPropValue);  
		if(BACDEL_SUCCESS != eRetVal)
		{
			/* set pointer to null */
			*pvReadPropValue = NULL;
			eErrCode = ERROR_CODE_OUT_OF_MEMORY;
		}
	}
	break;

	default:
	{
		/* property is not list or array type */
		/* no need to create copy */
		;
	}
	break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Copy_List_Type_properties: Exit \r\n");     
	#endif
	return eErrCode;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to handle special cases depending upon property type.
* These propetries are mainly updated when a read request is received.
*    
* @param eObjectType		[in]	  object type.
* @param eObjectProperty	[in/out]  property ID.
* @param pvObject			[in]	  base addrees of object.
* @param ul32OffsetAddr		[in]	  offset addrees/ property address. 
* @param pVirtualDev		[in]	  virtual device data.
* @param peData_Type		[out]	  property data type
* @param pvReadPropValue	[out]	  pointer to save property value. 
*                                   
* @return MAX_BACNET_ERROR_CODE	on success.
*	
*/
BACNET_ERROR_CODE Update_Special_Property(
	BACNET_OBJECT_TYPE eObjectType,
	uint32_t u32ObjectInstance,
	BACNET_PROPERTY_ID eObjectProperty,
	void *pvObject,
	ulong32_t ul32PropOffsetAddr,
	void *pVirtualDev,
	BACNET_DATA_TYPE *peData_Type,
	void **pvReadPropValue)	
{
	/* local variables */
	ulong32_t ul32BaseAddr = 0;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:\
	Update_Special_Property : Entry \r\n");     
	#endif

	/* check input pointers */
	if(NULL == pvObject || NULL == pVirtualDev || 
		NULL == pvReadPropValue || NULL == peData_Type)
	{
		return ERROR_CODE_INTERNAL_ERROR;
	}

	 /* Get the Structure Base Address */
    ul32BaseAddr = (ulong32_t )pvObject;	

	/* handling based on property id */
	switch(eObjectProperty)
    {              
        /* For Present Value */
        case PROP_PRESENT_VALUE:
        {
			#ifdef optimize_no_need_to_use_function
            /* Read Present value based on Object Type (i.e. Data Type) */
             ReadPresentValueProperty(eObjectType, ul32BaseAddr, ul32PropOffsetAddr, 		
                peData_Type, pvReadPropValue, pVirtualDev);
			#else
			/* save the offset */
			*pvReadPropValue = (void *)(ul32BaseAddr + ul32PropOffsetAddr);
			/* update polarity for binary object */
			#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
			if(OBJECT_BINARY_INPUT == eObjectType || OBJECT_BINARY_OUTPUT == eObjectType ||
				OBJECT_BINARY_VALUE == eObjectType)
			{				
				/* Update polarity value in PV structure for encoding */
				Update_PV_Polarity(ul32BaseAddr, eObjectType);   
			}
			#endif
			#endif
        }
        break;             
        
        /* For Object relinquish default value */
        case PROP_RELINQUISH_DEFAULT:
        {
			/* Update Data type for Tag at encoding time */
            UpdateValuePropDataType( eObjectType , peData_Type);
        }
        break;
    
#ifdef BACDEL_SER_AE_EN_B
        /* For Object Feed back value after write */
        case PROP_FEEDBACK_VALUE:
        {
            UpdateValuePropDataType( eObjectType , peData_Type);
        }
        break;
#endif /* BACDEL_SER_AE_EN_B */


#ifdef BACDEL_OBJ_DEV
#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)
       case PROP_LOCAL_TIME:
       {
		   /* to save local time */
		   BACnetTime_t stLocalTime = {0};
		   BACnetDate_t stLocalDate = {0};

		   /* Update Device object local date & local time property every 
		   time any of these property is read */
		   GetDevice_DateTime(&stLocalDate, &stLocalTime, pVirtualDev);  
		   Update_Dv_Local_DateTime_Properties(pVirtualDev, &stLocalTime, &stLocalDate, false);
        }
        break;

        case PROP_LOCAL_DATE:
        {
			/* to save local date */
			BACnetDate_t stLocalDate = {0};
			BACnetTime_t stLocalTime = {0};

			/* Update Device object local date & local time property every 
			time any of these property is read */
			GetDevice_DateTime(&stLocalDate, &stLocalTime, pVirtualDev);    
			Update_Dv_Local_DateTime_Properties(pVirtualDev, &stLocalTime, &stLocalDate, false);
        }
        break;

#ifdef BACDEL_PR14
		case PROP_PROPERTY_LIST:
		{
			/* change data-type of property-list property */
			*peData_Type = BACNET_DT_PROPERTY_LIST;
		}
		break;
#endif /* PR14 */
#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */
#endif /* BACDEL_OBJ_DEV */

		case PROP_ELAPSED_ACTIVE_TIME:
		{
			/* change data-type of property-list property */
			Read_All_Properties(eObjectType, eObjectProperty, 
			-1, peData_Type, pvReadPropValue, pvObject, pVirtualDev,
			u32ObjectInstance, false);
		}
		break;
		
        default:
        {
			/* do nothing */
        }
        break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:\
	Update_Special_Property : Exit \r\n");     
	#endif
	return eErrorCode;
}


/**
*                                                                    
* DESCRIPTION                                                                          
* This function to validate property of RP/RPM request.
*    
* @param pVirtualDev	[in]	  virtual device data.
* @param pstRpData		[in/out]  request pointer to save error.
*                                   
* @return TRUE/FALSE	[out]	  FALSE - If any error occur in request.
*								  TRUE  - If success.
*	
*/
bool Validate_ObjId_PropId_ArrayIndx(
	BACNET_CONF_DATA *pstCnfrmData,
	virtualDevData_t *pVirtualDev)
{
	/* local variable */
	BACNET_ERROR_CODE eErrCode = MAX_BACNET_ERROR_CODE;
	PROP_ACCESS_TYPE eAccessType = BACNET_DEFAULT;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_ObjId_PropId_ArrayIndx: Entry \r\n");     
	#endif

	/* check input pointers */
	if(NULL == pVirtualDev || NULL == pstCnfrmData)
	{
		/* This should not occur ideally */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Validate_ObjId_PropId_ArrayIndx: Null input pointers \r\n");
		#endif
		if(NULL != pstCnfrmData)
		{
			pstCnfrmData->eErrorCode = ERROR_CODE_INTERNAL_ERROR;
			pstCnfrmData->eErrorClass = ERROR_CLASS_DEVICE;
			pstCnfrmData->bErrorStatus = true;
		}
		return FALSE;
	}
	
	/* Find Object in the device object list */
	  pstCnfrmData->pvObjectAddr = Find_Object(pstCnfrmData->eObjectType, 
		pstCnfrmData->u32ObjectInstance, pVirtualDev);
    if(NULL == pstCnfrmData->pvObjectAddr)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Validate_ObjId_PropId_ArrayIndx : Object Not Found \r\n");
		#endif
        pstCnfrmData->eErrorCode = ERROR_CODE_UNKNOWN_OBJECT;
        pstCnfrmData->eErrorClass = ERROR_CLASS_OBJECT;
        pstCnfrmData->bErrorStatus = true;
        return FALSE;
    }	
	/* if device ID is special value(4194303) then set device ID as IUT device ID as per clause 9.18.1.3 */
	else if((OBJECT_DEVICE == pstCnfrmData->eObjectType) && 
		(SPECIAL_DEVICE_OBJECT == pstCnfrmData->u32ObjectInstance))
	{
		pstCnfrmData->u32ObjectInstance = pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId;
	}

	#ifdef BACDEL_OBJ_NP
	/* if special value(4194303) then set network port object instance no. */
	else if((OBJECT_NETWORK_PORT == pstCnfrmData->eObjectType) &&
		(BACNET_MAX_INSTANCE == pstCnfrmData->u32ObjectInstance))
	{
		pstCnfrmData->u32ObjectInstance = pVirtualDev->m_stSupportedObjects.m_pstNetworkPort->m_stObjectID.m_u32ObjId;
	}
	#endif
    /* Check Is Object support given property or not */
	eAccessType = GetDefndPropAccess(pstCnfrmData->eObjectType,
		pstCnfrmData->eObjectProperty, BACNET_DEFAULT);
	if(NOT_SUPPORTED == eAccessType)
    {
        /* Error Debug Logging done at end of switch case */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Validate_ObjId_PropId_ArrayIndx : Property not supported in stack \r\n");
		#endif
        pstCnfrmData->eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;
        pstCnfrmData->eErrorClass = ERROR_CLASS_PROPERTY;
        pstCnfrmData->bErrorStatus = true;
        return FALSE;
    }

	/* validate access type for property, calculate offset address */
	eErrCode = ValidateAccessType(pstCnfrmData->eObjectProperty, 
		pstCnfrmData->eObjectType, pstCnfrmData->pvObjectAddr, 
		&pstCnfrmData->ulPropOffsetAddr);
	if(ERROR_CODE_OTHER != eErrCode)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Validate_ObjId_PropId_ArrayIndx : Property not supported in object \r\n");
		#endif
		pstCnfrmData->eErrorCode = eErrCode;
        pstCnfrmData->eErrorClass = ERROR_CLASS_PROPERTY;
        pstCnfrmData->bErrorStatus = true;
        return FALSE;
	}	

	/* check if property is readable using RP or RPM */
    /* log buffer property is readable only using read range service */
	if(!IsPropertyReadable(pstCnfrmData->eObjectProperty, pstCnfrmData->eObjectType))
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Validate_ObjId_PropId_ArrayIndx : Property is not readable \r\n");
		#endif
		pstCnfrmData->eErrorCode = ERROR_CODE_READ_ACCESS_DENIED;
		pstCnfrmData->eErrorClass = ERROR_CLASS_PROPERTY;
		pstCnfrmData->bErrorStatus = true;
		return FALSE;
	}
	
	/* validate array index */
	eErrCode = ValidateArrayIndex(pstCnfrmData->eObjectProperty,
		pstCnfrmData->eObjectType, pstCnfrmData->u32ArrayIndex,
		pstCnfrmData->pvObjectAddr, pstCnfrmData->bArrIndxPresent,
		pstCnfrmData->ulPropOffsetAddr);
	if(ERROR_CODE_OTHER != eErrCode)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Validate_ObjId_PropId_ArrayIndx : Invalid array index \r\n");
		#endif
		pstCnfrmData->eErrorCode = eErrCode;
        pstCnfrmData->eErrorClass = ERROR_CLASS_PROPERTY;
        pstCnfrmData->bErrorStatus = true;
        return FALSE;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_ObjId_PropId_ArrayIndx: Exit \r\n");     
	#endif
	return TRUE;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function to get property values base address for encoding 
* for RP/RPM property value response.
* Function creates duplicate copy of property if array or list type.
* All special handling related to properties are handled in this function.
*    
* @param pVirtualDev	[in]	  virtual device data.
* @param pstReqData	    [in/out]  RP/RPM request data pointer to save value/error
*                                   
*/
void Read_All_Property_Value(
	BACNET_CONF_DATA *pstReqData,
	virtualDevData_t *pVirtualDev)
{
	/* local varaiables */
	ulong32_t ul32BaseAddr = 0;
	void *pvSrcPtr = NULL;
	BACNET_ERROR_CODE eErrCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Read_All_Property_Value: Entry \r\n");     
	#endif

	/* check input pointers */
	if(NULL == pVirtualDev || NULL == pstReqData)
	{
		/* This should not occur ideally */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Read_All_Property_Value: Null input pointers \r\n");
		#endif

		/* set error */
		if(NULL != pstReqData)
		{
			pstReqData->eErrorCode = ERROR_CODE_INTERNAL_ERROR;
			pstReqData->eErrorClass = ERROR_CLASS_DEVICE;
			pstReqData->bErrorStatus = true;
		}
		return;
	}

	/* get the object base address */
	ul32BaseAddr = (ulong32_t)pstReqData->pvObjectAddr;	

	/* get property datatype */
	pstReqData->eData_Type = GetPropertyDataType(     
		pstReqData->eObjectType,
		pstReqData->eObjectProperty, 
		pstReqData->u32ArrayIndex, 
		pstReqData->bArrIndxPresent);

	/* update properties that need to be update for RP request */	
	eErrCode = Update_Special_Property(
		pstReqData->eObjectType,
		pstReqData->u32ObjectInstance,
		pstReqData->eObjectProperty, 
		pstReqData->pvObjectAddr, 
		pstReqData->ulPropOffsetAddr, pVirtualDev, 
		&pstReqData->eData_Type, &pvSrcPtr);

	if(eErrCode != MAX_BACNET_ERROR_CODE)
	{
		/* set error */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:\
		Read_All_Property_Value : Update_Special_Property failed \r\n");
		#endif
		pstReqData->eErrorCode = eErrCode;
		pstReqData->eErrorClass = ERROR_CLASS_PROPERTY;
		pstReqData->bErrorStatus = TRUE;
		return;			
	}			

	/* if property other than PV and elapsed-active-time */
	if((PROP_PRESENT_VALUE != pstReqData->eObjectProperty))
	{
		/* save the property address */
		pvSrcPtr = (void *)(ul32BaseAddr + pstReqData->ulPropOffsetAddr);

		/* update offset for address binding property */
		if(PROP_DEVICE_ADDRESS_BINDING == pstReqData->eObjectProperty)
		{
			/* take device address binding property from global structure */
			pvSrcPtr = &g_stDevAddBinding;
		}
	}

	/* set default property address */
	pstReqData->pvReadPropValue = pvSrcPtr;

	/* if property is of list type, whole list is duplicated and new pointer 
	   will be assigned to RP structure */
	eErrCode = Copy_List_Type_properties(pvSrcPtr, pstReqData->eData_Type,
		&pstReqData->pvReadPropValue);

	if(eErrCode != MAX_BACNET_ERROR_CODE)
	{
		/* set error */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:\
		Read_All_Property_Value : Copy_List_Type_properties failed \r\n");
		#endif
		pstReqData->eErrorCode = eErrCode;
		pstReqData->eErrorClass = ERROR_CLASS_RESOURCES;
		pstReqData->bErrorStatus = TRUE;
		return;			
	}

	/* function Exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Read_All_Property_Value: Entry \r\n");     
	#endif 
	return;
}


/********************* End of propertyValueRead.c File *****************/
