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
*   SoftDEL Systems Ltd.                                india@softdel.com      
*   3rd Floor, Pentagon P4,                             http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*      serviceEventNotification_B.c
*                                                                      
*   AUTHORS                                                                     
*       Pratham N. Murkute
*                                                                         
*   DESCRIPTION                                                            
*        Initiate & Encode the confirmed & unconfirmed event notifications. 
*                                                                         
*   RELEASE HISTORY                                                                 
*        DATE           NAME                DESCRIPTION

*********************************************************************************/

#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)

#include "serviceEventNotification_B.h"
#include "bacDELDef.h" 

#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"
#include "propertyValueRead.h"
#include "propertyValueWrite.h"
#include "propertyClearValues.h"

/* to include MSTP layer functions */

#include "datalinkMSTP.h"


#ifdef BACDEL_SER_AE_AA_B
/** global struct for saved notifications list */
SavedNotificationslist_t g_stSavedEventNotifications = {0};
#endif

/** global variable for sequence no */
uint32_t g_u32SeqNo = 0;

/** external global variables */
/** global variable for device virtual network no. */
extern uint16_t g_u16VirtualNWNo;

/** global variable for device local network no. */
extern uint16_t g_u16LocalNWNo;

extern QueueHandle_t Cov_EventQueue;

extern DB_t SMCfg;
#ifdef BACDEL_PR14
/** function to properties reported in CHANGE_OF_RELIABILITY 
notifications according to object types */
static BACNET_RETURN_TYPE Change_Of_Reliability_Prop_Values(
	BACNET_OBJECT_TYPE eObjectType,
	ChangeOfReliabilityNP_t *pstChangeOfReliability,
	EventNotificationMsgQ_t *pstMsgQData);
#endif

/**
*                                                                    
*DESCRIPTION                                                                          
*   Creates the struct to be passed to event notification thread.
*    
*@param eObjectType			[in]	Type of object whose present value has changed.
*@param	pvObjBaseAddr		[in]	Base address of the object
*@param eEventStateTo		[in]	Future value of event state
*@param uPropValueData		[in]	property value
*@param	u32TimeDelay		[in]	Time delay value				
*@param bFlag				[in]	To check for any condition ..... not used for now
*
*@return pointer to the struct to be passed to fill the lisklist.
*                                                                      
**/
static void *Create_EventNotification_MsgQ_Struct(BACNET_OBJECT_TYPE	eObjectType,
											void *				pvObjBaseAddr,
											BACNET_EVENT_STATE	eEventStateTo,
											BACNET_EVENT_STATE	eEventStateFrom,
											BACNET_RELIABILITY	eReliability,
											PropertyValue_u		*puPropValueData,
											void *				pvVirtualDev)
{
    /* local variables */
    EventNotificationMsgQ_t *pstMsgQueData = NULL;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
    Create_EventStateUpdate_MsgQue_Struct: entry \r\n");
	#endif
	
    /* Assign memory to the pointer */
    pstMsgQueData = OSAL_Malloc(sizeof(EventNotificationMsgQ_t),  __FILE__, __FUNCTION__, __LINE__);
    if(NULL == pstMsgQueData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, 
		"APDU: Create_EventStateUpdate_MsgQue_Struct: malloc failed \r\n");
		#endif
        return NULL;
    }

	/* type of object */
	pstMsgQueData->m_eObjectType = eObjectType;
	/* base address of the object */
	pstMsgQueData->m_pvObjBaseAddr = pvObjBaseAddr;
	/* future event state of the object */
	pstMsgQueData->m_eEventStateTo = eEventStateTo;
	/* current event state of the object */
	pstMsgQueData->m_eEventStateFrom = eEventStateFrom;
	/* current reliability of the object */
	pstMsgQueData->m_eReliability = eReliability;
	/* present value of the object that has changed just now */
	memcpy(&pstMsgQueData->m_uPropValue, puPropValueData,
		sizeof(PropertyValue_u));
	/* Pointer to itself to free it later on */
	pstMsgQueData->m_pvSelfAddress = pstMsgQueData;
	/* virtual device address */
	pstMsgQueData->m_pvVirtualDev = pvVirtualDev;

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
    Create_EventStateUpdate_MsgQue_Struct: exit \r\n");
	#endif

    return (pstMsgQueData);
}

/**
*
* DESCRIPTION
* Function to check if two event notifications are to be generated.
*
* @param eObjectType [in]   object type.
* @param eFromState	 [in]	current event state.
* @param eToState	 [in]	next event state.
* @returns           [out]	true if 2 notifications are to be sent.
*
**/
bool IsDoubleNotification(BACNET_OBJECT_TYPE eObjectType,
		BACNET_EVENT_STATE eFromState, BACNET_EVENT_STATE eToState)
{
	/* return value */
	bool bReturnValue = FALSE;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	IsDoubleNotification: entry \r\n");
	#endif

	switch(eObjectType)
	{
		case OBJECT_ANALOG_INPUT:
		case OBJECT_ANALOG_OUTPUT:
		case OBJECT_ANALOG_VALUE:
		{
			if((EVENT_STATE_HIGH_LIMIT == eToState && 
				EVENT_STATE_LOW_LIMIT == eFromState) 
				||
			   (EVENT_STATE_HIGH_LIMIT == eFromState && 
			    EVENT_STATE_LOW_LIMIT == eToState))
			{
				/* send two notifications */
				bReturnValue = TRUE;
			}
			#ifdef BACDEL_PR14
			else if((EVENT_STATE_HIGH_LIMIT == eToState ||
			    EVENT_STATE_LOW_LIMIT == eToState ||
				EVENT_STATE_OFFNORMAL == eToState) && 
				EVENT_STATE_FAULT == eFromState)
			{
				/* send two notifications */
				bReturnValue = TRUE;
			}
			#endif
		}break;

		case OBJECT_BINARY_INPUT:
		case OBJECT_BINARY_OUTPUT:
		case OBJECT_BINARY_VALUE:
		case OBJECT_MULTI_STATE_INPUT:
		case OBJECT_MULTI_STATE_OUTPUT:
		case OBJECT_MULTI_STATE_VALUE:
		{
			if(EVENT_STATE_OFFNORMAL == eToState && 
				EVENT_STATE_FAULT == eFromState)
			{
				/* send two notifications */
				bReturnValue = TRUE;
			}
		}break;

	default:
		/* send only 1 notification */
		bReturnValue = FALSE;
		break;
	}//switch ends

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	IsDoubleNotification: exit \r\n");
	#endif

	return bReturnValue;
}

#ifdef BACDEL_PR14
/** 
* 
* DESCRIPTION  
* Properties Reported in CHANGE_OF_RELIABILITY Notifications 
* according to object types .
*    
* @param eObjectType			[in]  Object type.
* @param pstChangeOfReliability [in]  Pointer to struct ChangeOfReliabilityNP_t.
* @param pstMsgQData			[in]  Pointer to struct EventNotificationMsgQ_t frm msgQ.
*
* @return pointer to the struct to be passed to fill the lisklist.
*                                                                      
*/
static BACNET_RETURN_TYPE Change_Of_Reliability_Prop_Values(
	BACNET_OBJECT_TYPE eObjectType,
	ChangeOfReliabilityNP_t *pstChangeOfReliability,
	EventNotificationMsgQ_t *pstMsgQData)
{
	/* local variables */
	/* base address of initiating object */
	ulong32_t ul32BaseAddr = 0;
	/* offset address for property values */
	ulong32_t ul32OffsetAddr = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
    Change_Of_Reliability_Prop_Values: entry \r\n");
	#endif

	/* validate input pointer */
	if(NULL == pstChangeOfReliability)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Change_Of_Reliability_Prop_Values: Null pointer. \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

	/* get the base address of object */
    ul32BaseAddr = (ulong32_t)pstMsgQData->m_pvObjBaseAddr;

	switch(eObjectType)
	{

		#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV)
		case OBJECT_ANALOG_INPUT:
		case OBJECT_ANALOG_OUTPUT:
		case OBJECT_ANALOG_VALUE:
		{
			/* local variables */
			Pr_BACnetReal_t *pstRealPtr = NULL;

			/* present value */
			/* allocate memory */
			pstRealPtr = OSAL_Malloc(sizeof(Pr_BACnetReal_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstRealPtr)
			{
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* assign real value */
			pstRealPtr->m_fVal = pstMsgQData->m_uPropValue.fVal;
			pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = pstRealPtr;
			/* save data type */
			pstChangeOfReliability->m_stPropertyValue.m_eData_Type =
				BACNET_DT_REAL;
			/* save object property */
			pstChangeOfReliability->m_stPropertyValue.m_eObjectProperty =
				PROP_PRESENT_VALUE;
			/* save default priority */
			pstChangeOfReliability->m_stPropertyValue.m_u8Priority =
				BACNET_NO_PRIORITY;
			/* save default array index */
			pstChangeOfReliability->m_stPropertyValue.m_bIsArrayIndxPresent = FALSE;
		}
		break;
		#endif /* AI || AO || AV */

		#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BV)
		case OBJECT_BINARY_INPUT:
		case OBJECT_BINARY_VALUE:
		{
			/* local variables */
			Pr_BACnetBinaryPV_t *pstBinaryPvPtr = NULL;

			/* present value */
			/* allocate memory */
			pstBinaryPvPtr = OSAL_Malloc(sizeof(Pr_BACnetBinaryPV_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstBinaryPvPtr)
			{
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* assign character string value */
			pstBinaryPvPtr->m_eBinaryPv = pstMsgQData->m_uPropValue.eVal;
			pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = pstBinaryPvPtr;
			/* save data type */
			pstChangeOfReliability->m_stPropertyValue.m_eData_Type =
				BACNET_DT_ENUM_NEW;
			/* save object property */
			pstChangeOfReliability->m_stPropertyValue.m_eObjectProperty =
				PROP_PRESENT_VALUE;
			/* save default priority */
			pstChangeOfReliability->m_stPropertyValue.m_u8Priority =
				BACNET_NO_PRIORITY;
			/* save default array index */
			pstChangeOfReliability->m_stPropertyValue.m_bIsArrayIndxPresent = FALSE;
		}
		break;
		#endif /* BI || BV */

		#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSV)
		case OBJECT_MULTI_STATE_INPUT:
		case OBJECT_MULTI_STATE_VALUE:
		{
			/* local variables */
			Pr_BACnetUnsigned32_t *pstUnsignedPtr = NULL;
			/* allocate memory */
			pstUnsignedPtr = OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstUnsignedPtr)
			{
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* assign value */
			pstUnsignedPtr->m_u32Val = pstMsgQData->m_uPropValue.u32Val;
			pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = pstUnsignedPtr;
			/* save data type */
			pstChangeOfReliability->m_stPropertyValue.m_eData_Type =
				BACNET_DT_UNSIGNED32;
			/* save object property */
			pstChangeOfReliability->m_stPropertyValue.m_eObjectProperty =
				PROP_PRESENT_VALUE;
			/* save default priority */
			pstChangeOfReliability->m_stPropertyValue.m_u8Priority =
				BACNET_NO_PRIORITY;
			/* save default array index */
			pstChangeOfReliability->m_stPropertyValue.m_bIsArrayIndxPresent = FALSE;
		}
		break;
		#endif /* MSI || MSV */

		#ifdef BACDEL_OBJ_BO
		case OBJECT_BINARY_OUTPUT:
		{
			/* local variables */
			Pr_BACnetBinaryPV_t *pstBinaryPvPtr = NULL;
			property_value_t *pstPropValPtr = NULL;
			BACNET_BINARY_PV eFeedbackValue = MAX_BINARY_PV;

			/* allocate memory for present value */
			pstBinaryPvPtr = OSAL_Malloc(sizeof(Pr_BACnetBinaryPV_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstBinaryPvPtr)
			{
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* assign value */
			pstBinaryPvPtr->m_eBinaryPv = pstMsgQData->m_uPropValue.eVal;
			pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = pstBinaryPvPtr;
			/* save data type */
			pstChangeOfReliability->m_stPropertyValue.m_eData_Type =
				BACNET_DT_ENUM_NEW;
			/* save object property */
			pstChangeOfReliability->m_stPropertyValue.m_eObjectProperty =
				PROP_PRESENT_VALUE;
			/* save default priority */
			pstChangeOfReliability->m_stPropertyValue.m_u8Priority =
				BACNET_NO_PRIORITY;
			/* save default array index */
			pstChangeOfReliability->m_stPropertyValue.m_bIsArrayIndxPresent = FALSE;

			/* allocate memory for feedback value */
			pstBinaryPvPtr = OSAL_Malloc(sizeof(Pr_BACnetBinaryPV_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstBinaryPvPtr)
			{
				/* free allocated memories */
				OSAL_Free(pstChangeOfReliability->m_stPropertyValue.m_pvPropVal, 
					__FILE__, __FUNCTION__, __LINE__);
				pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = NULL;
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* allocate memory to next property value */
			pstPropValPtr = OSAL_Malloc(sizeof(property_value_t), __FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstPropValPtr)
			{
				/* free allocated memories */
				OSAL_Free(pstChangeOfReliability->m_stPropertyValue.m_pvPropVal, 
					__FILE__, __FUNCTION__, __LINE__);
				OSAL_Free(pstBinaryPvPtr, __FILE__, __FUNCTION__, __LINE__);
				pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = NULL;
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* get offset of feedback value & save value */
			GET_FEEDBACKVAL_OFFSET(eObjectType, m_stFeedbackValue, ul32OffsetAddr);
			memcpy((void *)&eFeedbackValue, (BACNET_BINARY_PV *)(ul32BaseAddr+ul32OffsetAddr), 
				sizeof(BACNET_BINARY_PV));
			/* save property value */
			pstBinaryPvPtr->m_eBinaryPv = eFeedbackValue;
			/* assign value */
			pstPropValPtr->m_pvPropVal = pstBinaryPvPtr;
			/* save data type */
			pstPropValPtr->m_eData_Type = BACNET_DT_ENUM_NEW;
			/* save object property */
			pstPropValPtr->m_eObjectProperty = PROP_FEEDBACK_VALUE;
			/* save default priority */
			pstPropValPtr->m_u8Priority = BACNET_NO_PRIORITY;
			/* save default array index */
			pstPropValPtr->m_bIsArrayIndxPresent = FALSE;
			/* assigne feedback value to next pointer of change of reliability */
			pstChangeOfReliability->m_stPropertyValue.m_pstNextVal = pstPropValPtr;
		}
		break;
		#endif /* BO object */

		#ifdef BACDEL_OBJ_MSO
		case OBJECT_MULTI_STATE_OUTPUT:
		{
			/* local variables */
			Pr_BACnetUnsigned32_t *pstUnsignedPtr = NULL;
			property_value_t *pstPropValPtr = NULL;
			uint32_t u32FeedbackValue = 0;

			/* present value */
			/* allocate memory */
			pstUnsignedPtr = OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstUnsignedPtr)
			{
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* assign value */
			pstUnsignedPtr->m_u32Val = pstMsgQData->m_uPropValue.u32Val;
			pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = pstUnsignedPtr;
			/* save data type */
			pstChangeOfReliability->m_stPropertyValue.m_eData_Type =
				BACNET_DT_UNSIGNED;
			/* save object property */
			pstChangeOfReliability->m_stPropertyValue.m_eObjectProperty =
				PROP_PRESENT_VALUE;
			/* save default priority */
			pstChangeOfReliability->m_stPropertyValue.m_u8Priority =
				BACNET_NO_PRIORITY;
			/* save default array index */
			pstChangeOfReliability->m_stPropertyValue.m_bIsArrayIndxPresent = FALSE;

			/* allocate memory for feedback value */
			pstUnsignedPtr = OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstUnsignedPtr)
			{
				/* free allocated memories */
				OSAL_Free(pstChangeOfReliability->m_stPropertyValue.m_pvPropVal, 
					__FILE__, __FUNCTION__, __LINE__);
				pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = NULL;
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* allocate memory to next property value */
			pstPropValPtr = OSAL_Malloc(sizeof(property_value_t), __FILE__, __FUNCTION__, __LINE__);
			/* validate input pointer */
			if(NULL == pstPropValPtr)
			{
				/* free allocated memories */
				OSAL_Free(pstChangeOfReliability->m_stPropertyValue.m_pvPropVal, 
					__FILE__, __FUNCTION__, __LINE__);
				OSAL_Free(pstUnsignedPtr, __FILE__, __FUNCTION__, __LINE__);
				pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = NULL;
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
				Change_Of_Reliability_Prop_Values: Malloc failed. \r\n");
				#endif
				return BACDEL_MALLOC_ERROR;
			}
			/* get offset of feedback value & save value */
			GET_FEEDBACKVAL_OFFSET(eObjectType, m_stFeedbackValue , ul32OffsetAddr);
			memcpy((void *)&u32FeedbackValue, (uint32_t *)(ul32BaseAddr+ul32OffsetAddr), 
				sizeof(uint32_t));
			/* save property value */
			pstUnsignedPtr->m_u32Val = u32FeedbackValue;
			/* assign value */
			pstPropValPtr->m_pvPropVal = pstUnsignedPtr;
			/* save data type */
			pstPropValPtr->m_eData_Type = BACNET_DT_UNSIGNED32;
			/* save object property */
			pstPropValPtr->m_eObjectProperty = PROP_FEEDBACK_VALUE;
			/* save default priority */
			pstPropValPtr->m_u8Priority = BACNET_NO_PRIORITY;
			/* save default array index */
			pstPropValPtr->m_bIsArrayIndxPresent = FALSE;
			/* assigne feedback value to next pointer of change of reliability */
			pstChangeOfReliability->m_stPropertyValue.m_pstNextVal = pstPropValPtr;
		}
		break;
		#endif /* MSO object */

		#if((defined BACDEL_OBJ_SDL || defined BACDEL_OBJ_TL) && defined BACDEL_PR14)
		case OBJECT_SCHEDULE:
		{
			/* no property values to fill */
			pstChangeOfReliability->m_stPropertyValue.m_eData_Type = BACNET_DT_EMPTY;
			pstChangeOfReliability->m_stPropertyValue.m_pvPropVal = NULL;
			pstChangeOfReliability->m_stPropertyValue.m_pstNextVal = NULL;
		}
		break;
		#endif /* PR14 */

		default:
		{
			; // do nothing
		}
		break;
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
    Change_Of_Reliability_Prop_Values: exit \r\n");
	#endif
    return BACDEL_SUCCESS;
}
#endif /* PR14 */

/**
*                                                                    
*DESCRIPTION                                                                          
*   Fills the event values in pstENdata depending on object type.
*    
*@param pstENdata [out] structure to store event values.
*@param pstMsgQData [in] Pointer to struct EventNotificationMsgQ_t frm msgQ.  
*
*@return void.
*                                                                      
*/
static void Get_Event_Value(void *pvMsgQData, BacnetEnPropElem_t *pstENdata)
{
	/* base address of initiating object */
	ulong32_t ul32BaseAddr = 0;
	/* offset address for property values */
	ulong32_t ul32OffsetAddr = 0;
	/* bacnet object type */
	BACNET_OBJECT_TYPE eObjectType;
	/* msgQ data */
	EventNotificationMsgQ_t *pstMsgQData = NULL;
    /* sizeof */
    int32_t i32Len = 0;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Get_Event_Value: entry \r\n");
	#endif

	/* Get MsgQ data pointer */
	pstMsgQData = pvMsgQData;

	/* Get the object base address */
    ul32BaseAddr = (ulong32_t )pstMsgQData->m_pvObjBaseAddr;

	/* Get the object type */
	eObjectType = pstMsgQData->m_eObjectType;

	/* set default event type to MAX_EVENT_TYPE */
	pstENdata->m_eEventType = MAX_EVENT_TYPE;

	switch(eObjectType)
	{
		#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV)
		case OBJECT_ANALOG_INPUT:
		case OBJECT_ANALOG_OUTPUT:
		case OBJECT_ANALOG_VALUE:
		{
			/* local variables */
			OutOfRangeNP_t *pstOutOfRange = NULL;
			#ifdef BACDEL_PR14
			ChangeOfReliabilityNP_t *pstChangeOfReliability = NULL;

			if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo ||
				EVENT_STATE_FAULT == pstMsgQData->m_eEventStateFrom)
			{
				pstChangeOfReliability = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability;
				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_RELIABILITY;
				/* fill event values */
				Change_Of_Reliability_Prop_Values(pstENdata->m_eObjectType, 
					pstChangeOfReliability, pstMsgQData);
				/* Reliability */
				pstChangeOfReliability->m_eReliabilty = pstMsgQData->m_eReliability;
				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfReliability->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
			else
			#endif /* PR14 */
			{
				pstOutOfRange = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stOutOfRange;

				/* fill event type */
				pstENdata->m_eEventType = EVENT_OUT_OF_RANGE;

				/* fill event values */
				/* present value */
				pstOutOfRange->m_fExceedingValue = 
					pstMsgQData->m_uPropValue.fVal;
				/* high limit */
				if(EVENT_STATE_HIGH_LIMIT == pstMsgQData->m_eEventStateTo ||
					EVENT_STATE_HIGH_LIMIT == pstMsgQData->m_eEventStateFrom)
				{
					/* get offset of high limit & save value */
					GET_HL_LL_VALUE_OFFSET(eObjectType, m_stHighLimit ,ul32OffsetAddr,i32Len);
					memcpy(&pstOutOfRange->m_fExceededLimit, 
						(Float_t *)(ul32BaseAddr+ul32OffsetAddr), 
						i32Len);
				}
				/* low limit */
				else if(EVENT_STATE_LOW_LIMIT == pstMsgQData->m_eEventStateTo ||
					EVENT_STATE_LOW_LIMIT == pstMsgQData->m_eEventStateFrom)
				{
					/* get offset of low limit & save value */
					GET_HL_LL_VALUE_OFFSET(eObjectType, m_stLowLimit ,ul32OffsetAddr,i32Len);
					memcpy(&pstOutOfRange->m_fExceededLimit, 
						(Float_t *)(ul32BaseAddr+ul32OffsetAddr), 
						i32Len);
				}
				else pstOutOfRange->m_fExceededLimit = 0.0;

				/* deadband */
				/* get offset of deadband & save value */
				GET_DEADBAND_COV_INCR_VAL_OFFSET(eObjectType, 
					m_stDeadBand,ul32OffsetAddr, i32Len);
				memcpy(&pstOutOfRange->m_fDeadband, 
					(Float_t *)(ul32BaseAddr+ul32OffsetAddr), 
					i32Len);

				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstOutOfRange->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
		}
		break;
		#endif /* Analog || PC */

		#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BV)
		case OBJECT_BINARY_INPUT:
		case OBJECT_BINARY_VALUE:
		{
			/* local variables */
			ChangeOfStateNP_t *pstChangeOfState = NULL;
			#ifdef BACDEL_PR14
			ChangeOfReliabilityNP_t *pstChangeOfReliability = NULL;

			if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo ||
				EVENT_STATE_FAULT == pstMsgQData->m_eEventStateFrom)
			{
				pstChangeOfReliability = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability;
				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_RELIABILITY;
				/* fill event values */
				Change_Of_Reliability_Prop_Values(pstENdata->m_eObjectType, 
					pstChangeOfReliability, pstMsgQData);
				/* Reliability */
				pstChangeOfReliability->m_eReliabilty = pstMsgQData->m_eReliability;
				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfReliability->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
			else
			#endif /* PR14 */
			{
				pstChangeOfState = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeState;

				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_STATE;

				/* fill event values */
				/* present value */
				pstChangeOfState->m_uNewState.BACnetPropertyStates_U.m_UnsignedValue = 
					pstMsgQData->m_uPropValue.eVal;
				/* save tag type of value that denotes its prop_state */
				pstChangeOfState->m_uNewState.m_ePropState = PROP_STATE_BINARY_VALUE;

				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfState->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
		}
		break;
		#endif /* BI || BV */

		#ifdef BACDEL_OBJ_BO
		case OBJECT_BINARY_OUTPUT:
		{
			/* local variables */
			CommandFailureNP_t *pstCmdFailure = NULL;
			#ifdef BACDEL_PR14
			ChangeOfReliabilityNP_t *pstChangeOfReliability = NULL;

			if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo ||
				EVENT_STATE_FAULT == pstMsgQData->m_eEventStateFrom)
			{
				pstChangeOfReliability = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability;
				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_RELIABILITY;
				/* fill event values */
				Change_Of_Reliability_Prop_Values(pstENdata->m_eObjectType, 
					pstChangeOfReliability, pstMsgQData);
				/* Reliability */
				pstChangeOfReliability->m_eReliabilty = pstMsgQData->m_eReliability;
				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfReliability->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
			else
			#endif /* PR14 */
			{
				pstCmdFailure = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stCmdFailure;

				/* fill event type */
				pstENdata->m_eEventType = EVENT_COMMAND_FAILURE;

				/* fill event values */
				/* present value */
				pstCmdFailure->m_stCommandValue.eVal = 
					pstMsgQData->m_uPropValue.eVal;
				/* save app tag type for encoding */
				pstCmdFailure->m_u8Apptag = BACNET_APPLICATION_TAG_ENUMERATED;

				/* feedback value */
				/* get offset of feedback value & save value */
				GET_FEEDBACKVAL_OFFSET(eObjectType,m_stFeedbackValue , 
					ul32OffsetAddr);
				memcpy(&pstCmdFailure->m_stFeedbackValue.eVal,
					(BACNET_BINARY_PV *)(ul32BaseAddr+ul32OffsetAddr), 
					sizeof(BACNET_BINARY_PV));

				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstCmdFailure->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
		}
		break;
		#endif /* BO */

		#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSV)
		case OBJECT_MULTI_STATE_INPUT:
		case OBJECT_MULTI_STATE_VALUE:
		{
			/* local variables */
			ChangeOfStateNP_t *pstChangeOfState = NULL;
			#ifdef BACDEL_PR14
			ChangeOfReliabilityNP_t *pstChangeOfReliability = NULL;
			/*pstChangeOfState = &pstENdata->m_stEventValues.
				BACnetNotificationParameters_u.m_stChangeState;*/

			if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo ||
				EVENT_STATE_FAULT == pstMsgQData->m_eEventStateFrom)
			{
				pstChangeOfReliability = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability;
				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_RELIABILITY;
				/* fill event values */
				Change_Of_Reliability_Prop_Values(pstENdata->m_eObjectType, 
					pstChangeOfReliability, pstMsgQData);
				/* Reliability */
				pstChangeOfReliability->m_eReliabilty = pstMsgQData->m_eReliability;
				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfReliability->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
			else
			#endif /* PR14 */
			{
				pstChangeOfState = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeState;

				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_STATE;

				/* fill event values */
				/* present value */
				pstChangeOfState->m_uNewState.BACnetPropertyStates_U.m_UnsignedValue = 
					pstMsgQData->m_uPropValue.u32Val;
				/* save tag type of value that denotes its prop_state */
				pstChangeOfState->m_uNewState.m_ePropState = PROP_STATE_UNSIGNED_VALUE;

				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfState->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
		}
		break;
		#endif /* MSI || MSV */

		#ifdef BACDEL_OBJ_MSO
		case OBJECT_MULTI_STATE_OUTPUT:
		{
			/* local variables */
			CommandFailureNP_t *pstCmdFailure = NULL;
			#ifdef BACDEL_PR14
			ChangeOfReliabilityNP_t *pstChangeOfReliability = NULL;
			
			if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo ||
				EVENT_STATE_FAULT == pstMsgQData->m_eEventStateFrom)
			{
				pstChangeOfReliability = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability;
				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_RELIABILITY;
				/* fill event values */
				Change_Of_Reliability_Prop_Values(pstENdata->m_eObjectType, 
					pstChangeOfReliability, pstMsgQData);
				/* Reliability */
				pstChangeOfReliability->m_eReliabilty = pstMsgQData->m_eReliability;
				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfReliability->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
			else
			#endif /* PR14 */
			{
				pstCmdFailure = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stCmdFailure;

				/* fill event type */
				pstENdata->m_eEventType = EVENT_COMMAND_FAILURE;

				/* fill event values */
				/* present value */
				pstCmdFailure->m_stCommandValue.u32Val = 
					pstMsgQData->m_uPropValue.u32Val;
				/* save app tag type for encoding */
				pstCmdFailure->m_u8Apptag = BACNET_APPLICATION_TAG_UNSIGNED_INT;

				/* feedback value */
				/* get offset of feedback value & save value */
				GET_FEEDBACKVAL_OFFSET(eObjectType,m_stFeedbackValue , 
					ul32OffsetAddr);
				memcpy(&pstCmdFailure->m_stFeedbackValue.u32Val,
					(uint32_t *)(ul32BaseAddr+ul32OffsetAddr), 
					sizeof(uint32_t));

				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstCmdFailure->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
		}
		break;
		#endif /* MSO */

		#if (defined BACDEL_OBJ_SDL && defined BACDEL_PR14)
		case OBJECT_SCHEDULE:
        {
			/* local variables */
			#ifdef BACDEL_PR14
			ChangeOfReliabilityNP_t *pstChangeOfReliability = NULL;

			if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo ||
				EVENT_STATE_FAULT == pstMsgQData->m_eEventStateFrom)
			{
				/* get value */
				pstChangeOfReliability = &pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability;
				/* fill event type */
				pstENdata->m_eEventType = EVENT_CHANGE_OF_RELIABILITY;
				/* fill event values */
				Change_Of_Reliability_Prop_Values(pstENdata->m_eObjectType, 
					pstChangeOfReliability, pstMsgQData);
				/* Reliability */
				pstChangeOfReliability->m_eReliabilty = pstMsgQData->m_eReliability;
				/* status flag */
				/* get offset of status flag & save value */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_stBitStr, ul32OffsetAddr);
				memcpy(&pstChangeOfReliability->m_stStatusFlag, 
					(BACnetBitStr_t *)(ul32BaseAddr+ul32OffsetAddr),
					sizeof(BACnetBitStr_t));
			}
			else
			#endif /* PR14 */
			{
				; // schedule object can't generate other events */
			}
		}
        break;
        #endif /* SDL */

		default:
			/* default case debug message */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: Get_Event_Value : \
										Invalid Object Type \r\n");
			#endif
			break;
	}//end of switch

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Get_Event_Value: exit \r\n");
	#endif

	return;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Api store all event notification parameters to be encoded & returns pointer
*	of the structure. 
*    
*@param pstVirtualDev [in] virtual device pointer.
*@param pstMsgQData [in] Pointer to struct EventNotificationMsgQ_t frm msgQ.  
*@param pstNCRecepient [in] pointer to recipient info from recipient list.  
*@param pstNotificationClassObj [in] Pointer to NC object.  
*
*@return BACNET_RETURN_TYPE.
*                                                                      
*/
static void *Get_Event_Notification_Parameters(virtualDevData_t *pstVirtualDev, 
										Notificationclass_t *pstNotificationClassObj,
										ListOfBACnetDestination_t *pstNCRecepient,
										void *pvMsgQData)
{
	/* pointer to store the event notification data */
	BacnetEnPropElem_t *pstENdata = NULL;
	/* base address of initiating object */
	ulong32_t ul32BaseAddr = 0;
	/* offset address for property values */
	ulong32_t ul32OffsetAddr = 0;
	/* for testing bits */
	uint8_t	u8Result = 0; 
	/* msgQ data */
	EventNotificationMsgQ_t *pstMsgQData = NULL;
	#ifdef BACDEL_PR14
	/* for event message text config */
	Pr_BACnetEventMsgText_t *pstEvntMsgTextConfig = NULL;
	#else
	/* default string from object */
	NotificationString_t *pstNotificationMsg = NULL;
	#endif

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
    Get_Event_Notification_Parameters: entry \r\n");
	#endif

	/* Get MsgQ data pointer */
	pstMsgQData = pvMsgQData;

	if(NULL == pstVirtualDev || NULL == pstNotificationClassObj ||
		NULL == pstNCRecepient || NULL == pstMsgQData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Get_Event_Notification_Parameters: null pointers \r\n");
		#endif
		return NULL;
	}

	/* Get the object base address */
    ul32BaseAddr = (ulong32_t )pstMsgQData->m_pvObjBaseAddr;

	pstENdata = OSAL_Malloc(sizeof(BacnetEnPropElem_t), __FILE__, __FUNCTION__, __LINE__);
	if(NULL != pstENdata)
	{
		/* initialize memory to zero */
		memset(pstENdata, 0, sizeof(BacnetEnPropElem_t));

		/* start filling the data */	
		/* device type */
		pstENdata->m_eDeviceType = pstVirtualDev->m_stDevObject.m_stObjectID.m_eObjectType;
		/* device id */
		pstENdata->m_u32DeviceId = pstVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId;

		/* object type */
		pstENdata->m_eObjectType = pstMsgQData->m_eObjectType;
		/* object id */
        GET_OBJCOMMON_OFFSET(pstMsgQData->m_eObjectType, m_stObjectID.m_u32ObjId, ul32OffsetAddr);
		memcpy(&pstENdata->m_u32ObjId,
			(void *)(ul32BaseAddr+ul32OffsetAddr),sizeof(uint32_t));

		#ifndef BACDEL_PR14
		/* character message */
		GET_INTRINSIC_OFFSET(pstMsgQData->m_eObjectType, 
			m_stNotificationMsg, ul32OffsetAddr);
		pstNotificationMsg = (void *)(ul32BaseAddr+ul32OffsetAddr);
		pstENdata->m_CharString.m_u8Encoding = pstNotificationMsg->m_Encoding;
		pstENdata->m_CharString.m_u32StrLen = pstNotificationMsg->m_u32StrLen;
		memcpy (&pstENdata->m_CharString.m_pu8CharStr, &pstNotificationMsg->m_pu8CharStr, 
					pstNotificationMsg->m_u32StrLen);
		#endif

		/* from state */
		pstENdata->m_eFromState = pstMsgQData->m_eEventStateFrom;
		/* to state */
		pstENdata->m_eToState = pstMsgQData->m_eEventStateTo;

		/* notify type */
		GET_INTRINSIC_OFFSET(pstMsgQData->m_eObjectType,
			m_stNotifyType.m_eNotifyType ,ul32OffsetAddr);
		memcpy(&pstENdata->m_eNotifyType,
			(void *)(ul32BaseAddr+ul32OffsetAddr),sizeof(BACNET_NOTIFY_TYPE));

		/* notification class */
		pstENdata->m_u32NotifyClass = pstNotificationClassObj->m_stNotificationClass.m_u32Val;
		/* process id */
		pstENdata->m_u32ProcessId = pstNCRecepient->m_stProcessId;
		
		/* ack required & priority */
		if(EVENT_STATE_HIGH_LIMIT == pstMsgQData->m_eEventStateTo || 
			EVENT_STATE_LOW_LIMIT == pstMsgQData->m_eEventStateTo ||
			EVENT_STATE_OFFNORMAL == pstMsgQData->m_eEventStateTo ||
			EVENT_STATE_LIFE_SAFETY_ALARM == pstMsgQData->m_eEventStateTo)
		{
			/* read ack-required property value */
			u8Result = BIT_VALUE_CHECK(pstNotificationClassObj->m_stAckRequired.
						m_stBitStr.m_u8TransBits[0],TRANSITION_TO_OFFNORMAL);
			/* save priority */
			pstENdata->m_u8Priority = 
				pstNotificationClassObj->m_stPriority.m_u32Value[TRANSITION_TO_OFFNORMAL];
            #ifdef BACDEL_PR14
			/* update event notification message */
			memcpy(&pstENdata->m_CharString, &pstEvntMsgTextConfig->m_stEventMsgText[TRANSITION_TO_OFFNORMAL], 
				sizeof(BACnetCharStr_t));
			#endif
		}
		else if(EVENT_STATE_NORMAL == pstMsgQData->m_eEventStateTo)
		{
			/* read ack-required property value */
			u8Result = BIT_VALUE_CHECK(pstNotificationClassObj->m_stAckRequired.
						m_stBitStr.m_u8TransBits[0],TRANSITION_TO_NORMAL);
			/* save priority */
			pstENdata->m_u8Priority = 
				pstNotificationClassObj->m_stPriority.m_u32Value[TRANSITION_TO_NORMAL];
			#ifdef BACDEL_PR14
			/* update event notification message */
			memcpy(&pstENdata->m_CharString, &pstEvntMsgTextConfig->m_stEventMsgText[TRANSITION_TO_NORMAL], 
				sizeof(BACnetCharStr_t));
			#endif
		}
		else if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo)
		{
			/* read ack-required property value */
			u8Result = BIT_VALUE_CHECK(pstNotificationClassObj->m_stAckRequired.
						m_stBitStr.m_u8TransBits[0],TRANSITION_TO_FAULT);
			/* save priority */
			pstENdata->m_u8Priority = 
				pstNotificationClassObj->m_stPriority.m_u32Value[TRANSITION_TO_FAULT];
			#ifdef BACDEL_PR14
			/* update event notification message */
			memcpy(&pstENdata->m_CharString, &pstEvntMsgTextConfig->m_stEventMsgText[TRANSITION_TO_FAULT], 
				sizeof(BACnetCharStr_t));
			#endif
		}
		else;

		/* save ack-required value */
		if(u8Result)
		{
			pstENdata->m_bAckReq = TRUE;
		}

		/* call the function to store event type & event values */
		Get_Event_Value(pstMsgQData, pstENdata);

		/* return the pointer */
		#if(defined DEBUG_PRINTF && DL_3)
		Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
		Get_Event_Notification_Parameters: exit \r\n");
		#endif

		return ((void *)pstENdata);
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
    Get_Event_Notification_Parameters: exit \r\n");
	#endif

	return NULL;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   this function updates the event_time_stamp & acked_transition properties while 
*	sending the event notification.
*    
*@param eObjectType [in]  type of object.
*@param eTransitionBit [in] transition to normal, offnormal or fault.
*@param pstDateTime [in] current system date_time.
*@param pvObjBaseAddr [in] object base address.
*@param pstVirtualDev [in] pointer to instance of virtual device data.
*@param u32SeqNo [in] sequence no if time stamp is sequence no.
*
*@return void. 
*                                                                      
*/
static void Update_Dependent_Properties(bool bIsConfirmed,
							   BACNET_EVENT_TRANSITION_BITS eTransitionBit,
							   timeStamp_u *pstTimeStamp,
							   void *pvObjBaseAddr,
							   virtualDevData_t *pstVirtualDev,
							   BacnetEnPropElem_t *pstENdata,
                               uint32_t u32SeqNo)
{
	/* local variables */
	ulong32_t ul32BaseAddr = 0;   /* For Base struct Addr */
	ulong32_t ul32OffsetAddr = 0; /* For Data offset Addr */
	void *pvData = NULL; /* data pointer to send address of data location */
	void *pvDummy = NULL;
	uint32_t u32PropertyAdd = 0;
	Pr_BACnetBitStr_t stBitString = {0}; /* for acked transition */
	PROP_ACCESS_TYPE eAccessType = BACNET_DEFAULT; /* property access type */
	BACNET_OBJECT_TYPE eObjectType = 0;
//    uint32_t u32Val = 0;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: UpdateDependentProperties: entry \r\n");
	#endif

    /* NOTE: the input pointers are null checked & passed to this function */

	/* get the object base address */
	ul32BaseAddr = (ulong32_t)pvObjBaseAddr;
	/* get type of object */
	eObjectType = pstENdata->m_eObjectType;

	/* EVENT_TIME_STAMP */

#ifdef STAMP_AS_SEQUENCE
    /* if tim stamp is sequence no update the value */
	pstTimeStamp->m_u32SeqNo = u32SeqNo;
#endif

	/* data to be written to time_stamp */
	pvData = pstTimeStamp;

	/* call UpdateCommonProperty function to change event_time_stamp */
	UpdateCommonProperty(eObjectType,
						 pvObjBaseAddr, 
						 PROP_EVENT_TIME_STAMPS, 
						 (eTransitionBit+1)/* adding 1 for array index */, 
                         pvData, eAccessType,
						 &u32PropertyAdd, &pvDummy, pstVirtualDev,
						 false, false, false);

	/* ACKED_TRANSITION */
	if(/*bIsConfirmed && */pstENdata->m_bAckReq)
	{
		/* Update property offset */
		GET_INTRINSIC_OFFSET(eObjectType, m_stAckedTransitions, 
			ul32OffsetAddr);
		memcpy(&stBitString,(void *)(ul32BaseAddr+ul32OffsetAddr),
			sizeof(Pr_BACnetBitStr_t));

		/* clear the respective bit */
		BIT_CLEAR(stBitString.m_stBitStr.m_u8TransBits[0], eTransitionBit);

		/* data to be written to ack_transition */
		pvData = &stBitString;

		/* call UpdateCommonProperty function to change ack_transition */
		UpdateCommonProperty(eObjectType, 
							 pvObjBaseAddr, 
							 PROP_ACKED_TRANSITIONS, 
							 -1, pvData, eAccessType,
							 &u32PropertyAdd, &pvDummy, pstVirtualDev,
							 false, false, false);
	}

#ifdef BACDEL_PR14
	/* EVENT_MESSAGE_TEXTS */
	/* Update property Access Type offset & get access type value */

	/* data to be written to message text */
	pvData = (void *)&pstENdata->m_CharString;

	/* call UpdateCommonProperty function to change event_message_text */
	UpdateCommonProperty(eObjectType,
						 pvObjBaseAddr, 
						 PROP_EVENT_MESSAGE_TEXTS, 
						 (eTransitionBit+1)/* adding 1 for array index */,
						 pvData, eAccessType,
						 &u32PropertyAdd, &pvDummy, pstVirtualDev,
						 false, false, false);
#endif /* PR14 */

    /* update other dependent properties as per object type */
    switch(eObjectType)
    {
        /* default case - do nothing */
        default:
            break;
    }

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: UpdateDependentProperties: exit \r\n");
	#endif

	return;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Saves the event notification in linklist. 
*    
*@param pstENdata [in] event notification data.
*@return BACNET_RETURN_TYPE.
*                                                                      
*/
#ifdef BACDEL_SER_AE_AA_B
static BACNET_RETURN_TYPE Save_Event_Notification(BacnetEnPropElem_t *pstENdata)
{
	/* local variables */
	SavedEventNotifications_t *pstLocalNode = NULL;
	bool bReplace = FALSE;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Save_Event_Notification: entry \r\n");
	#endif

	if(NULL == pstENdata)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Save_Event_Notification: null input pointer \r\n");
		#endif

		return BACDEL_ERROR;
	}

	/* Wait till the mutex is released */
	Osal_Wait_Mutex(g_stSavedEventNotifications.hMtxSavedNotification, INFINITE); 

	/* get 1st node of linklist */
	pstLocalNode = g_stSavedEventNotifications.pstRootNode;

	/* traverse the linklist */
	while(pstLocalNode != NULL)
	{
		if((pstENdata->m_u32DeviceId == pstLocalNode->m_stENdata.m_u32DeviceId) &&
			(pstENdata->m_eObjectType == pstLocalNode->m_stENdata.m_eObjectType) &&
		   (pstENdata->m_u32ObjId == pstLocalNode->m_stENdata.m_u32ObjId))
		{
			switch(pstLocalNode->m_stENdata.m_eToState)
			{
			case EVENT_STATE_NORMAL:
				if(pstENdata->m_eToState == EVENT_STATE_NORMAL)
					bReplace = TRUE;
				break;
			case EVENT_STATE_FAULT:
				if(pstENdata->m_eToState == EVENT_STATE_FAULT)
					bReplace = TRUE;
				break;
			case EVENT_STATE_OFFNORMAL:
				if(pstENdata->m_eToState == EVENT_STATE_OFFNORMAL)
					bReplace = TRUE;
				break;
			case EVENT_STATE_HIGH_LIMIT:
			case EVENT_STATE_LOW_LIMIT:
				if(pstENdata->m_eToState == EVENT_STATE_OFFNORMAL ||
				   pstENdata->m_eToState == EVENT_STATE_HIGH_LIMIT ||
				   pstENdata->m_eToState == EVENT_STATE_LOW_LIMIT)
					bReplace = TRUE;
				break;
			case EVENT_STATE_LIFE_SAFETY_ALARM:
				if(pstENdata->m_eToState == EVENT_STATE_OFFNORMAL ||
				   pstENdata->m_eToState == EVENT_STATE_LIFE_SAFETY_ALARM)
				    bReplace = TRUE;
				break;
			default:
				bReplace = FALSE;
				break;
			}//switch ends.
			if(bReplace)
			{
				/* match found, replace the node data */
				memcpy(&pstLocalNode->m_stENdata, pstENdata, sizeof(BacnetEnPropElem_t));
				/* event values are not required in Ack-Notification */
				memset(&pstLocalNode->m_stENdata.m_stEventValues, 0,
					sizeof(BACnetNotificationParameters_t));
				break;
			}
		}//if ends
		/* move to next node */
		pstLocalNode = pstLocalNode->m_pstNext;
	}

	/* node not found in linklist, so create new node */
	if(NULL == pstLocalNode)
	{
		/* allocate memory */
		pstLocalNode = OSAL_Malloc(sizeof(SavedEventNotifications_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstLocalNode)
		{
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
			Save_Event_Notification: malloc failed \r\n");
			#endif

			/* release the mutex */
			Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);
			return BACDEL_MALLOC_ERROR;
		}
		/* initialize memory */
		memset(pstLocalNode,0,sizeof(SavedEventNotifications_t));
		/* copy the data */
		memcpy(&pstLocalNode->m_stENdata, pstENdata, sizeof(BacnetEnPropElem_t));
		/* event values are not required in Ack-Notification */
		memset(&pstLocalNode->m_stENdata.m_stEventValues, 0, 
			sizeof(BACnetNotificationParameters_t));
		/* now maintain the links in linklist */
		if(g_stSavedEventNotifications.pstRootNode == NULL)
			/* update the root node address only when 1st node for the list is created */
			g_stSavedEventNotifications.pstRootNode = pstLocalNode;
		else
			/* create the list here */
			g_stSavedEventNotifications.pstLastNode->m_pstNext = pstLocalNode;
		/* update the current/last node address */
		g_stSavedEventNotifications.pstLastNode = pstLocalNode;
	}

	/* release the mutex */
	Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Save_Event_Notification: exit \r\n");
	#endif

	return BACDEL_SUCCESS;
}
#endif /* BACDEL_SER_AE_AA_B */

/**
*                                                                    
*DESCRIPTION                                                                          
*   Sends the event notification to recipient based on the value received. 
*    
*@param u32Value [in] not used yet.
*@param pstMsgQueData [in] Pointer to struct EventNotificationMsgQ_t.  
*
*@return nothing.
*                                                                      
*/
void Send_Notification(bacnetip_arguments_t *pstIpArgs,
					   virtualDevData_t *pVirtualDev,
					   ListOfBACnetDestination_t *pstNCRecepient,
					   BacnetEnPropElem_t *pstENdata)
{
	/* local variable */
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	int32_t i32Lowrange = -1;
	int32_t i32Highrange = -1;
	bool bIsNwMsg = FALSE;
	bool bDontBreakLoop = FALSE;
	uint8_t u8Count = 0;
	bool bIsRouter = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Send_Notification: entry \r\n");
	#endif

	/* validate input parameters */
	if(NULL == pVirtualDev || NULL == pstNCRecepient || NULL == pstENdata)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Send_Notification: null input data pointers. \r\n");
		#endif
		return;
	}

	/* clear any previous data */
	memset(pstIpArgs, 0x00, sizeof(bacnetip_arguments_t));

	/* send the notifications */
	/* Initialize all the required parameters */
	pstIpArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
		m_stEvent_Notification.m_pvEventValues = (void *)pstENdata;
	/* get the process id */
	pstIpArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
		m_stEvent_Notification.m_u32ProcessIdentifier = pstENdata->m_u32ProcessId;
	/* set m_i8SendSimpleAck to false */
	pstIpArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
		m_stEvent_Notification.m_u8SendSimpleAck = FALSE;
    /* update the destination type */
    pstIpArgs->m_stNPDUData.m_eDestinationType = 
        pstNCRecepient->m_stRecipient.m_eDestinationType;

	/* set to default value */
	pstIpArgs->m_u32DeviceID = BACNET_MAX_INSTANCE;

	/* switch to the recipient type */
	switch(pstNCRecepient->m_stRecipient.m_eDestinationType)
	{
	case DESTINATION_IS_IP_ADDR:
		/* copy the address */
		memcpy(&pstIpArgs->m_stDestBACnetAddr,
			&pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress, 
			sizeof(BACnetAddress_t));
		break;

	case DESTINATION_IS_LOCAL_BROADCAST:
		/* update bvlc to braodcast */
		pstIpArgs->m_eBvlcFunctionType = BVLC_ORIGINAL_BROADCAST_NPDU;
		/* update destination type */
		pstIpArgs->m_stNPDUData.m_eDestinationType = DESTINATION_IS_LOCAL_BROADCAST;
		/* get this device address & update it for local broadcast */

		/* set broadcast MSTP address */
		MSTP_GetBroadcastMacId(DESTINATION_IS_LOCAL_BROADCAST, &pstIpArgs->m_stDestBACnetAddr);
		break;

	case DESTINATION_IS_GLOBAL_BROADCAST:
		/* update bvlc to braodcast */
		pstIpArgs->m_eBvlcFunctionType = BVLC_ORIGINAL_BROADCAST_NPDU;
		/* update destination type */
		pstIpArgs->m_stNPDUData.m_eDestinationType = DESTINATION_IS_GLOBAL_BROADCAST;
		/* get this device address & update it for global broadcast */

		/* set broadcast MSTP address */
		MSTP_GetBroadcastMacId(DESTINATION_IS_GLOBAL_BROADCAST, &pstIpArgs->m_stDestBACnetAddr);
		break;

	case DESTINATION_IS_DEVICE_ID:
		/* get the address of destination from device address binding
			by matching its device id */
		pstIpArgs->m_u32DeviceID = 
			pstNCRecepient->m_stRecipient.BACnetRecipient_u.
			m_stObjId.m_u32ObjId;
		eReturnVal = BACDEL_Get_Device_ID_Or_Address
						(&pstIpArgs->m_u32DeviceID,
						&pstIpArgs->m_stDestBACnetAddr, true);
		/* set parameters for sending who is */
		i32Lowrange = pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stObjId.m_u32ObjId;
		i32Highrange = pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stObjId.m_u32ObjId;
		bIsNwMsg = FALSE;
		/* update bvlc to unicast */
		pstIpArgs->m_eBvlcFunctionType = BVLC_ORIGINAL_UNICAST_NPDU;
        pstIpArgs->m_stNPDUData.m_eDestinationType = DESTINATION_IS_UNICAST;

		break;

	case DESTINATION_IS_REMOTE_BROADCAST:
		/* update bvlc to braodcast */
		pstIpArgs->m_eBvlcFunctionType = BVLC_ORIGINAL_BROADCAST_NPDU;
		/* Note: do not add break statement here */
	case DESTINATION_IS_MSTP_DEVICE:
		/* check if ip address is available */
		eReturnVal = BACDEL_Get_Router_Address_From_Network_No
			(pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net, 
			&pstIpArgs->m_stDestBACnetAddr);
		if(BACDEL_SUCCESS == eReturnVal)
	    {
			/* router address found, proceed to send event */
		}
		else
		{
			/* router address not found, send who is router */
			bIsRouter = TRUE;
		}
        /* copy the dnet & dadr of recipient */
		for(u8Count = 0; u8Count < pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len ; 
            u8Count++)
		{
			/* copy address */
            pstIpArgs->m_stDestBACnetAddr.u8DvDadr[u8Count] = 
			pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[u8Count];
		}
        pstIpArgs->m_stDestBACnetAddr.u8dlen = 
			pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len;
        /* copy the dnet */
		pstIpArgs->m_stDestBACnetAddr.u16net = pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net;
        /* reset u8Count */
        u8Count = 0;
		/* set parameters for sending who is router to network */
		i32Lowrange = -1;
		i32Highrange = -1;
		bIsNwMsg = TRUE;
		bIsRouter = TRUE;
		break;

	case DESTINATION_IS_UNRECOGNIZED:
	default:
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DESTINATION_INVALID, "APDU: \
		Send_Notification: destination type not supported. \r\n");
		#endif
		eReturnVal = BACDEL_ERROR;
		break;
	}//switch ends

	#ifdef SUPPORT_MULTIPLE_DEVICE
	/* send local broadcast on local n/w of VR */
	/* applicable for both VR and VDs */
	if(DESTINATION_IS_REMOTE_BROADCAST == pstNCRecepient->m_stRecipient.m_eDestinationType &&
		pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net == g_u16LocalNWNo)
	{
		/* set broadcast IP address */
		BIP_GetDstBrdcstAddr(DESTINATION_IS_LOCAL_BROADCAST, &pstIpArgs->m_stDestBACnetAddr);
		/* set network number */
		pstIpArgs->m_stDestBACnetAddr.u16net = BACNET_ZERO;
	}
	/* send unicast on local network */
	else if(DESTINATION_IS_MSTP_DEVICE == pstNCRecepient->m_stRecipient.m_eDestinationType &&
		pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net == g_u16LocalNWNo)
	{
		/* copy the address */
		memcpy(&pstIpArgs->m_stDestBACnetAddr,
			&pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress,
			sizeof(BACnetAddress_t));
		/* set network number */
		pstIpArgs->m_stDestBACnetAddr.u16net = BACNET_ZERO;
	}
	/* check if this is virtual device */
	/* change destination address for virtual device */
	else if(BACNET_ZERO != pVirtualDev->m_u16SNET)
	{
		/* discard local broadcast on virtual n/w */
		if(DESTINATION_IS_LOCAL_BROADCAST == pstNCRecepient->m_stRecipient.m_eDestinationType ||
			(DESTINATION_IS_REMOTE_BROADCAST == pstNCRecepient->m_stRecipient.m_eDestinationType &&
			pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net == g_u16VirtualNWNo))
		{
			/* no need to send notification, set flag to discard notification */
			/* free notification data */
			OSAL_Free(pstENdata, __FILE__, __FUNCTION__, __LINE__);
			return;
		}
		else
		{
			; /* do nothing */
		}
	}
	else
	{
		/* discard local broadcast on virtual n/w */
		if(DESTINATION_IS_REMOTE_BROADCAST == pstNCRecepient->m_stRecipient.m_eDestinationType &&
			pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net == g_u16VirtualNWNo)
		{
			/* no need to send notification, set flag to discard notification */
			/* free notification data */
			OSAL_Free(pstENdata, __FILE__, __FUNCTION__, __LINE__);
			return;
		}
		else
		{
			; /* do nothing */
		}
	}
	#endif /* */

	do{
		if(BACDEL_SUCCESS == eReturnVal)
		{
			if(pstIpArgs->m_eBvlcFunctionType == BVLC_ORIGINAL_BROADCAST_NPDU && 
			   TRUE == pstNCRecepient->m_bIssueConfirmedNotification)
			{
				/* free notification data */
				OSAL_Free(pstENdata, __FILE__, __FUNCTION__, __LINE__);
				/* do not broad cast if confirmed notification */
				return;
			}
			/* get the notification type */
			if(TRUE == pstNCRecepient->m_bIssueConfirmedNotification)
			{
				/* set the service support to confirmed */
				pstIpArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stEvent_Notification.m_u8IsConfirmedNotification = TRUE;
				pstIpArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport =
					SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION;
			}
			else
				/* set the service support to unconfirmed */
				pstIpArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport =
					SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION;
			/* generate the request */
			BACDEL_Generate_Aside_Request(pVirtualDev->m_u8DvSADR, pstIpArgs, NULL);
			/* break the loop */
			bDontBreakLoop = FALSE;
		}
		else
		{
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, eReturnVal, "APDU: \
			Send_Notification: destination address not found, sending who is. \r\n");
			#endif

			/* send who is with the corresponding device id */
			Send_WHO_IS(pVirtualDev, i32Lowrange, i32Highrange, bIsNwMsg,
				pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net,
				DESTINATION_IS_GLOBAL_BROADCAST);
			Osal_Sleep(500); //vks
			if(!bIsRouter)
			{
				/* check if the device was binded to device address binding */
				eReturnVal = BACDEL_Get_Device_ID_Or_Address
							(&pstIpArgs->m_u32DeviceID,
							&pstIpArgs->m_stDestBACnetAddr, true);
			}
			else
			{
				/* check if router address was binded & ip address is available */
				eReturnVal = BACDEL_Get_Router_Address_From_Network_No
					(pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net,
					&pstIpArgs->m_stDestBACnetAddr);
			}
			if(BACDEL_SUCCESS == eReturnVal)
				/* device found, send notification */
				bDontBreakLoop = TRUE;
			else
			{
				#if(defined DEBUG_PRINTF && DL_1)
				Print_DebugMsg(DEBUG_LEVEL1, eReturnVal, "APDU: \
				Send_Notification: destination address still not found, \
				sending who is again. \r\n");
				#endif

				/* send who is with the corresponding device id again */
				Send_WHO_IS(pVirtualDev, i32Lowrange, i32Highrange, bIsNwMsg,
					pstNCRecepient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net,
					DESTINATION_IS_GLOBAL_BROADCAST);
				/* break the loop */
				bDontBreakLoop = FALSE;
			}
		}
		/* increment count to break the loop in case anything goes wrong */
		if(u8Count)
			bDontBreakLoop = FALSE;
		else
			u8Count++;
	}while(bDontBreakLoop);

	/* notification was not sent, free the resources allocated */
	if(BACDEL_SUCCESS != eReturnVal)
	{
		/* free notification data */
		if(NULL != pstENdata)
		{
			#ifdef BACDEL_PR14
			/* free change of reliability property value memory */
			if(EVENT_CHANGE_OF_RELIABILITY == pstENdata->m_eEventType)
			{
				/* free pointer from 1st property value */
				Clear_PropVal_AsPer_DataType(pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability.
					m_stPropertyValue.m_eData_Type, 
					&pstENdata->m_stEventValues.
					BACnetNotificationParameters_u.m_stChangeOfReliability.
					m_stPropertyValue.m_pvPropVal);
				/* free the notification data */
				Clear_Prop_Val(&pstENdata->m_stEventValues.BACnetNotificationParameters_u.
					m_stChangeOfReliability.m_stPropertyValue.m_pstNextVal);
			}
			#endif
			/* free event notification data */
			OSAL_Free(pstENdata, __FILE__, __FUNCTION__, __LINE__);
		}
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Send_Notification: exit \r\n");
	#endif
	return;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Generates event notification based on the value received. 
*    
*@param u32Value [in] not used yet.
*@param pstMsgQueData [in] Pointer to struct EventNotificationMsgQ_t.  
*
*@return BACNET_RETURN_TYPE.
*                                                                      
*/
BACNET_RETURN_TYPE Send_Event_Notification(uint32_t u32Value, void *pstMsgQueData)
{
    /* local variables */
	EventNotificationMsgQ_t *pstMsgQData = NULL; /* msgQ data */
	virtualDevData_t *pstVirtualDev = NULL; /* virtual device ptr */
	ListOfBACnetDestination_t *pstNCRecepient = NULL; /* recipient info */
	Notificationclass_t	*pstNotificationClassObj = NULL; /* NC obj ptr */

	BacnetEnPropElem_t *pstENdata = NULL; /* notification parameters */
	timeStamp_u uTimestamp = {0}; /* to save date & time */
	BACNET_EVENT_TRANSITION_BITS eTransitionBit = MAX_BACNET_EVENT_TRANSITION;

	bacnetip_arguments_t stServiceArgs = {0}; /* to send event notification */

	ulong32_t ul32BaseAddr = 0; /* For Base struct Addr */
	ulong32_t ul32OffsetAddr = 0; /* For Data offset Addr */
	uint32_t u32NotifyClassINITobj = 0;
	uint32_t u32NotifyClassNCobj = 0;
	int32_t i32DiffFromTime = 0; /* to get time difference */
	int32_t i32DiffToTime = 0; /* to get time difference */
	uint8_t	u8Result = 0; /* for testing bits */

	bool bSaveNotification = TRUE; /* to check if notification is to be saved */
	bool bFirstRecipient = TRUE; /* to keep track of 1st recipient */
	BACNET_RETURN_TYPE eReturnVal = BACDEL_ERROR;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Send_Event_Notification: entry \r\n");
	#endif

    /* null check input pointer */
    if(pstMsgQueData == NULL)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
        Send_Event_Notification: null pointer \r\n");
		#endif

        return BACDEL_ERROR;
	}

	/* get the msgQ pointer */
	pstMsgQData = pstMsgQueData;
	/* get the virtual device struct */
    pstVirtualDev = (virtualDevData_t *)pstMsgQData->m_pvVirtualDev;
	/* Get the object base address */
    ul32BaseAddr = (ulong32_t )pstMsgQData->m_pvObjBaseAddr;

    /* check pointers */
	if(NULL == pstVirtualDev || 0 == ul32BaseAddr)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
        Send_Event_Notification: null pointer \r\n");
		#endif

		return BACDEL_ERROR;
	}

	 /* Update property offset */
    GET_NCPROP_OFFSET(pstMsgQData->m_eObjectType,m_stNotificationClass.m_u32Val,ul32OffsetAddr);
    /* Copy data into local variable from property offset address */
    memcpy(&u32NotifyClassINITobj,(void *)(ul32BaseAddr+ul32OffsetAddr),sizeof(uint32_t));

	/* get the struct of 1st NC object from device obj */
	pstNotificationClassObj = pstVirtualDev->m_stSupportedObjects.m_pstNotificationClass;
	while(NULL != pstNotificationClassObj)
	{
		u32NotifyClassNCobj = pstNotificationClassObj->m_stNotificationClass.m_u32Val;
		if(u32NotifyClassNCobj == u32NotifyClassINITobj)
		{
			/* get the recipient list struct */
			pstNCRecepient = pstNotificationClassObj->m_stRecipientList.m_pstNCRecepient;
			/* break only if non null pointer is found */
			if(NULL != pstNCRecepient)
				break;
		}
		/* move to next notification class object */
		pstNotificationClassObj = pstNotificationClassObj->pstNext;
	}

    /* recipient list not found */
	if(NULL == pstNCRecepient)
	{
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: Send_Event_Notification: \
        Recipient List Empty. \r\n");
		#endif

		return BACDEL_ERROR;
	}

	/* get current device date & time */
	GetDevice_DateTime(&uTimestamp.m_stDateTime.m_stDate, 
        &uTimestamp.m_stDateTime.m_stTime, 
        pstVirtualDev);

	/* traverse the recipient list & send event notifications */
	while(NULL != pstNCRecepient)
	{
		/* check if the To-state transition bit is set */
		if(EVENT_STATE_HIGH_LIMIT == pstMsgQData->m_eEventStateTo || 
			EVENT_STATE_LOW_LIMIT == pstMsgQData->m_eEventStateTo ||
			EVENT_STATE_OFFNORMAL == pstMsgQData->m_eEventStateTo ||
			EVENT_STATE_LIFE_SAFETY_ALARM == pstMsgQData->m_eEventStateTo)
		{
			eTransitionBit = TRANSITION_TO_OFFNORMAL;
			u8Result = BIT_VALUE_CHECK(pstNCRecepient->m_stTransitions.
				m_u8TransBits[0],TRANSITION_TO_OFFNORMAL);
		}
		else if(EVENT_STATE_NORMAL == pstMsgQData->m_eEventStateTo)
		{
			eTransitionBit = TRANSITION_TO_NORMAL;
			u8Result = BIT_VALUE_CHECK(pstNCRecepient->m_stTransitions.
				m_u8TransBits[0],TRANSITION_TO_NORMAL);
		}
		else if(EVENT_STATE_FAULT == pstMsgQData->m_eEventStateTo)
		{
			eTransitionBit = TRANSITION_TO_FAULT;
			u8Result = BIT_VALUE_CHECK(pstNCRecepient->m_stTransitions.
				m_u8TransBits[0],TRANSITION_TO_FAULT);
		}
		else; /* error case */

		if(u8Result)
		{
			/* reset u8Result */
			u8Result = 0;
			/* compare from time */
			i32DiffFromTime = DateTime_Compare_Time(&pstNCRecepient->m_stFromTime, 
				&uTimestamp.m_stDateTime.m_stTime);
			/* compare to time */
			i32DiffToTime = DateTime_Compare_Time(&pstNCRecepient->m_stToTime, 
				&uTimestamp.m_stDateTime.m_stTime);
			if((i32DiffFromTime == 0 || i32DiffFromTime < 0) && 
				(i32DiffToTime == 0 || i32DiffToTime > 0))
			{
				/* valid time found, now check for valid day */
				switch(uTimestamp.m_stDateTime.m_stDate.m_u8Wday)
				{
				case BACNET_WEEKDAY_MONDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_MONDAY);
					break;
				case BACNET_WEEKDAY_TUESDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_TUESDAY);
					break;
				case BACNET_WEEKDAY_WEDNESDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_WEDNESDAY);
					break;
				case BACNET_WEEKDAY_THURSDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_THURSDAY);
					break;
				case BACNET_WEEKDAY_FRIDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_FRIDAY);
					break;
				case BACNET_WEEKDAY_SATURDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_SATURDAY);
					break;
				case BACNET_WEEKDAY_SUNDAY:
					u8Result = BIT_VALUE_CHECK
						(pstNCRecepient->m_stDaysOfWeek.m_u8TransBits[0],
						BACNET_DAYS_OF_WEEK_SUNDAY);
					break;
				default:
					u8Result = 0;
					break;
				}//end of switch
			}//end of time differences
		}//end of if(u8Result)

		if(u8Result)
		{
			/* get notification service parameters */
			pstENdata = Get_Event_Notification_Parameters(pstVirtualDev, 
									pstNotificationClassObj,
									pstNCRecepient,
									pstMsgQData);
			if(NULL == pstENdata)
			{
				#if(defined DEBUG_PRINTF && DL_2)
				Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
				Send_Event_Notification: Get_Event_Notification_Parameters returned null \r\n");
				#endif

				return BACDEL_ERROR;
			}
			/* save time_stamp in pstENdata */
#ifdef STAMP_AS_DATETIME
			memcpy(&pstENdata->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stDate, 
				&uTimestamp.m_stDateTime.m_stDate, sizeof(BACnetDate_t));
			memcpy(&pstENdata->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime, 
				&uTimestamp.m_stDateTime.m_stTime, sizeof(BACnetTime_t));
#elif defined STAMP_AS_TIME
			memcpy(&pstENdata->m_stTimeStamp.m_stTimeStamp.m_stTime, 
				&uTimestamp.m_stTime, sizeof(BACnetTime_t));
#elif defined STAMP_AS_SEQUENCE
			memcpy(&pstENdata->m_stTimeStamp.m_stTimeStamp.m_u32SeqNo, 
				&g_u32SeqNo, sizeof(uint32_t));
#endif
			if(bFirstRecipient)
			{
				/* update the dependent properties */
				Update_Dependent_Properties(pstNCRecepient->m_bIssueConfirmedNotification,
										  eTransitionBit,
										  &uTimestamp,
										  pstMsgQData->m_pvObjBaseAddr,
										  pstVirtualDev,
										  pstENdata,
                                          g_u32SeqNo);
				/* reset 1st recipient flag */
				bFirstRecipient = FALSE;


			}

			#ifdef BACDEL_SER_AE_AA_B
			if(bSaveNotification && pstENdata->m_bAckReq)
			{
				/* call function to save the notification in linklist */
				eReturnVal = Save_Event_Notification(pstENdata);
				if(BACDEL_SUCCESS == eReturnVal)
				{
					/* reset 1st save notification flag */
					bSaveNotification = FALSE;
				}
				else
				{
					;
					/* save notification failed */
					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, eReturnVal, "APDU: \
					Send_Event_Notification: Save_Event_Notification returns error. \r\n");
					#endif
			    }
			}
			#endif

			/* send the notification */
			Send_Notification(&stServiceArgs, pstVirtualDev, pstNCRecepient, pstENdata);

		}//end of if(u8Result)
		
		/* move to next recipient */
		pstNCRecepient = pstNCRecepient->m_pstNext;
	}//end of while loop

	/* Free the malloced memories */
    OSAL_Free(pstMsgQData->m_pvSelfAddress,  __FILE__, __FUNCTION__, __LINE__);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Send_Event_Notification: exit \r\n");
	#endif

	return BACDEL_SUCCESS;
}


/**
*                                                                    
*DESCRIPTION                                                                          
*   Encodes the Event Notification to be send. 
*    
*@param pstProcQIndex [in]  Pointer to processInfo_t structure.
*@param pu8APDUResp   [out] Pointer to Transmit buffer.
*@param i32APDULen    [out] Transmit buffer fill Index
*
*@return Number of Bytes consumed. 
*                                                                      
*/
int32_t Event_Notification_Encoder(processInfo_t *pstProcQInfo, uint8_t *pu8APDUResp,
									int32_t i32APDULen)
{
	/* notification parameters */
	BacnetEnPropElem_t *pstENdata = NULL; 

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Event_Notification_Encoder: entry \r\n");
	#endif

    /* get the request data */
	pstENdata = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
    if(NULL == pstENdata)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Event_Notification_Encoder: NULL Input Pointers \r\n");
		#endif
        return 0;
    }

	/* START ENCODING */
	/* Encode Process ID */
    i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen],TAG_NO_0,
							pstENdata->m_u32ProcessId);

	/* Encode Device ID */
    i32APDULen += Encode_Context_Object_Id(&pu8APDUResp[i32APDULen], TAG_NO_1, 
							(int32_t)pstENdata->m_eDeviceType, 
							pstENdata->m_u32DeviceId);

    /* Encode Object ID */
    i32APDULen += Encode_Context_Object_Id(&pu8APDUResp[i32APDULen], TAG_NO_2,
							(int32_t)pstENdata->m_eObjectType, 
							pstENdata->m_u32ObjId);

	/* Encode TimeStamp */
	i32APDULen += Encode_Time_Stamp(&pstENdata->m_stTimeStamp,
		&pu8APDUResp[i32APDULen], TAG_NO_3);

	/* Encode Notification Class */
	i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen],TAG_NO_4,
							pstENdata->m_u32NotifyClass);

	/* Encode Priority */
	i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen],TAG_NO_5,
							pstENdata->m_u8Priority);

	/* Encode Event Type */
	i32APDULen += Encode_Context_Enumerated(&pu8APDUResp[i32APDULen],TAG_NO_6,
							pstENdata->m_eEventType);

	/* check if string is available - this is optional */
	if(BACNET_ZERO != pstENdata->m_CharString.m_u32StrLen)
		/* Encode Message Text - optional */
		i32APDULen += Encode_Context_Character_String(&pu8APDUResp[i32APDULen],TAG_NO_7,
							&pstENdata->m_CharString);

	/* Encode Notify Type */
	i32APDULen += Encode_Context_Enumerated(&pu8APDUResp[i32APDULen],TAG_NO_8,
							pstENdata->m_eNotifyType);

	/* encode ack_required & from_state only if notify type is alarm or event */
	/* refer clause 13.8.1.11 & 13.8.1.12 */
	if(NOTIFY_ACK_NOTIFICATION != pstENdata->m_eNotifyType)
	{
		/* Encode Ack Required */
		i32APDULen += Encode_Context_Boolean(&pu8APDUResp[i32APDULen],TAG_NO_9,
								pstENdata->m_bAckReq);

		/* Encode From State */
		i32APDULen += Encode_Context_Enumerated(&pu8APDUResp[i32APDULen],TAG_NO_10,
								pstENdata->m_eFromState);
	}

	/* Encode To State */
	i32APDULen += Encode_Context_Enumerated(&pu8APDUResp[i32APDULen],TAG_NO_11,
							pstENdata->m_eToState);

	/* encode event_values only if notify type is alarm or event */
	/* refer clause 13.8.1.14 */
	if(NOTIFY_ACK_NOTIFICATION != pstENdata->m_eNotifyType)
	{
		/* Encode Event Values */
		i32APDULen += Encode_Notification_Parameters
								(&pu8APDUResp[i32APDULen], pstENdata);
	}
	/* END ENCODING */

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Event_Notification_Encoder: exit \r\n");
	#endif

	/* return encoded length */
	return i32APDULen;
}

/**
*************************************************************************************
*DESCRIPTION
*   This API will decide if Event_Notification is to be generated or not.
*	
*@param	pvObjBaseAddr	[in]	base address of object.
*@param	eObjectType		[in]	type of object.
*@param	eEventStateTo	[in]	new event_state of object.
8@param	eEventStateFrom	[in]	old event_state of object.
*
*@return				[out]	TRUE if notification is to be generated, else FALSE.
*
*************************************************************************************/
bool Check_For_Event_Notification_Generation(void *pvObjBaseAddr, 
											 BACNET_OBJECT_TYPE eObjectType,
											 BACNET_EVENT_STATE *eEventStateTo,
											 BACNET_EVENT_STATE *eEventStateFrom)
{
    /* local variables */
	ulong32_t	ul32BaseAddr = 0;		/* for object base address */
	ulong32_t	ul32OffsetAddr = 0;		/* for data offset address */
	uint8_t		u8Result = 0;			/* for testing bits */
	uint8_t		u8BitString = 0;		/* for storing bit string value */
	uint8_t		u8BitStringEE = 0;		/* for storing bit string value of event enable */

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Check_For_Event_Notification_Generation: entry \r\n");
	#endif

	/* get the base address */
	ul32BaseAddr = (ulong32_t)pvObjBaseAddr;				
	/* Get event_enable property offset & Copy data into local variable from offset address */
	GET_INTRINSIC_OFFSET(eObjectType,m_stEventEnable.m_stBitStr.m_u8TransBits[0], ul32OffsetAddr);
	memcpy(&u8BitStringEE, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint8_t)); 

    /* check if from & to states are same */
	if(*eEventStateTo == *eEventStateFrom)
	{
        if(eObjectType != OBJECT_TRENDLOG && eObjectType != OBJECT_CHARACTERSTRING_VALUE
		   )
		{
            return FALSE;
		}
        else if(eObjectType == OBJECT_TRENDLOG)
		{
            if(*eEventStateTo == EVENT_STATE_FAULT)
                return FALSE;
		}
	}

    /* switch to type of object */
	switch(eObjectType)
	{
		/*	Bit 0 = offnormal
			Bit 1 = fault
			Bit 2 = normal */
		case OBJECT_ANALOG_INPUT:
		case OBJECT_ANALOG_OUTPUT:
		case OBJECT_ANALOG_VALUE:
			if(*eEventStateTo == EVENT_STATE_LOW_LIMIT)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_OFFNORMAL);
				if(u8Result == 1)
				{
					u8BitString = 0;
					/* Get limit_enable property offset & Copy data into local variable from offset address */
					GET_LIMITENABLE_OFFSET(eObjectType, m_stLimitEnable.m_stBitStr.m_u8TransBits[0], 
						ul32OffsetAddr);
					memcpy(&u8BitString, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), 
						sizeof(uint8_t)); 
					if(u8BitString == LIMIT_ENABLE_NONE)
						return FALSE;
					else /*	Bit 0 = low limit -- Bit 1 = high limit */
					{						
						/* only low limit is enable */
						if(u8BitString == LIMIT_ENABLE_LOW && *eEventStateFrom == EVENT_STATE_HIGH_LIMIT)
						{
							/* send only normal to low notification, discard high to normal */
							*eEventStateFrom = EVENT_STATE_NORMAL;
							if(BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL))
								return TRUE;
							else
								return FALSE;
						}
						/* only high limit is enable */
						else if(u8BitString == LIMIT_ENABLE_HIGH && *eEventStateFrom == EVENT_STATE_HIGH_LIMIT)
						{
							/* send only high to normal notification, discard normal to low */
							*eEventStateTo = EVENT_STATE_NORMAL;
							if(BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL))
								return TRUE;
							else
								return FALSE;
						}
						/* low limit is enable */
						if(u8BitString == LIMIT_ENABLE_LOW || u8BitString == LIMIT_ENABLE_BOTH)
							return TRUE;
					}
				}
			}
			else if(*eEventStateTo == EVENT_STATE_HIGH_LIMIT)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_OFFNORMAL);
				if(u8Result == 1)
				{
					u8BitString = 0;
					/* Get limit_enable property offset & Copy data into local variable from offset address */
					GET_LIMITENABLE_OFFSET(eObjectType, m_stLimitEnable.m_stBitStr.m_u8TransBits[0], 
						ul32OffsetAddr);
					memcpy(&u8BitString, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), 
						sizeof(uint8_t)); 
					if(u8BitString == LIMIT_ENABLE_NONE)
						return FALSE;
					else /*	Bit 0 = low limit -- Bit 1 = high limit */
					{
						/* only high limit is enable */
						if(u8BitString == LIMIT_ENABLE_HIGH && *eEventStateFrom == EVENT_STATE_LOW_LIMIT)
						{
							/* send only normal to low notification, discard high to normal */
							*eEventStateFrom = EVENT_STATE_NORMAL;
							if(BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL))
								return TRUE;
							else
								return FALSE;
						}
						/* only low limit is enable */
						else if(u8BitString == LIMIT_ENABLE_LOW && *eEventStateFrom == EVENT_STATE_LOW_LIMIT)
						{
							/* send only low to normal notification, discard normal to high */
							*eEventStateTo = EVENT_STATE_NORMAL;
							if(BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL))
								return TRUE;
							else
								return FALSE;
						}
						/* high limit is enable */
						if(u8BitString == LIMIT_ENABLE_HIGH || u8BitString == LIMIT_ENABLE_BOTH)
							return TRUE;
					}
				}
			}
			else if(*eEventStateTo == EVENT_STATE_NORMAL)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL);
				if(u8Result == 1)
				{
					if(*eEventStateFrom == EVENT_STATE_FAULT)
						return TRUE;
					u8BitString = 0;
					/* Get limit_enable property offset & Copy data into local variable from offset address */
					GET_LIMITENABLE_OFFSET(eObjectType, m_stLimitEnable.m_stBitStr.m_u8TransBits[0], 
						ul32OffsetAddr);
					memcpy(&u8BitString, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), 
						sizeof(uint8_t)); 
				
					if(u8BitString == LIMIT_ENABLE_NONE)
						return FALSE;
					else 
					{
						/* low limit is enable */
						if((u8BitString == LIMIT_ENABLE_LOW || u8BitString == LIMIT_ENABLE_BOTH) && *eEventStateFrom == EVENT_STATE_LOW_LIMIT)
							return TRUE;
						/* high limit is enable */
						else if((u8BitString == LIMIT_ENABLE_HIGH || u8BitString == LIMIT_ENABLE_BOTH) && *eEventStateFrom == EVENT_STATE_HIGH_LIMIT)
							return TRUE;
					}
				}
			}
			else if(*eEventStateTo == EVENT_STATE_FAULT)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_FAULT);
				if(u8Result == 1)
					return TRUE;
			}
			else; /* Other state - ideally should not be there for analog objects */
			break;

		case OBJECT_BINARY_INPUT:
		case OBJECT_BINARY_OUTPUT:
		case OBJECT_BINARY_VALUE:
		case OBJECT_MULTI_STATE_INPUT:
		case OBJECT_MULTI_STATE_OUTPUT:
		case OBJECT_MULTI_STATE_VALUE:

			if(*eEventStateTo == EVENT_STATE_OFFNORMAL)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_OFFNORMAL);
				if(u8Result == 1)
					return TRUE;
			}
			else if(*eEventStateTo == EVENT_STATE_NORMAL)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL);
				if(u8Result == 1)
					return TRUE;
			}
			else if(*eEventStateTo == EVENT_STATE_FAULT)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_FAULT);
				if(u8Result == 1)
					return TRUE;
			}
			else;	/* Other state - ideally should not be there */
			break;

        #if (defined BACDEL_OBJ_EL \
			|| (defined BACDEL_OBJ_SDL && defined BACDEL_PR14))
		#if (defined BACDEL_OBJ_SDL && defined BACDEL_PR14)
		case OBJECT_SCHEDULE:
		#endif
            if(*eEventStateTo == EVENT_STATE_NORMAL)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_NORMAL);
				if(u8Result == 1)
					return TRUE;
			}
			else if(*eEventStateTo == EVENT_STATE_FAULT)
			{
				u8Result = BIT_VALUE_CHECK(u8BitStringEE,TRANSITION_TO_FAULT);
				if(u8Result == 1)
					return TRUE;
			}
            /* offnormal state is not applicable to trend log object */
            else;
            break;
        #endif

		default:
			/* default case debug message */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
			Check_For_Event_Notification_Generation : Invalid Object Type \r\n");
			#endif
			break;
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Check_For_Event_Notification_Generation: exit \r\n");
	#endif

	return FALSE;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Posts the struct to be passed to event notification thread.
*	- check if 2 notifications are to be generated or not.
*    
*@param eObjectType		[in]	Type of object whose property value has changed.
*@param	pvObject		[in]	Base address of the object
*@param eEventStateFrom	[in]	current event state
*@param eEventStateTo	[in]	Future value of event state
*@param uPropValueData	[in]	property value
*@param	pVirtualDev		[in]	virtual device instance				
*
*@return nothing.
*                                                                      
**/
void Post_Event_Notification(BACNET_OBJECT_TYPE	eObjectType,
							void *				pvObjBaseAddr,
							BACNET_EVENT_STATE	eEventStateTo,
							BACNET_EVENT_STATE	eEventStateFrom,
							BACNET_RELIABILITY	eReliability,
							PropertyValue_u		*puPropValueData,
							void *				pvVirtualDev)
{
    /* local variables */
    Post_Thread_Msg_t postThreadMsg = {0};
    void *pvMsgQdata = NULL;
	virtualDevData_t *pVirtualDev = NULL;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Post_Event_Notification: entry \r\n");
	#endif

    /* return back if invalid pointers received */
	if(NULL == pvVirtualDev || NULL == pvObjBaseAddr)
		return;

	/* get the virtual device pointer */
	pVirtualDev = pvVirtualDev;

	/* create the msgQ structure */
	pvMsgQdata = Create_EventNotification_MsgQ_Struct(eObjectType, pvObjBaseAddr, 
		eEventStateTo, eEventStateFrom, eReliability, puPropValueData, pVirtualDev);
	if(NULL != pvMsgQdata)
	{
		#ifdef SUPPORT_MULTIPLE_DEVICE
		postThreadMsg.idThread = gstHostDevice.m_pstDeviceStruct->m_dwNotificationThreadID;
		#else
		/* post the message to the notification thread */
		postThreadMsg.idThread = pVirtualDev->m_dwNotificationThreadID;
		#endif
		postThreadMsg.MsgType = MSGQ_TYPE;
		postThreadMsg.wParam = (unsigned int)0;
		postThreadMsg.lParam = (void*)pvMsgQdata;

		xQueueSend(Cov_EventQueue, &postThreadMsg, (TickType_t)100);
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Post_Event_Notification: exit \r\n");
	#endif

	return;
}

#endif /* BACDEL_SER_AE_EN_B */
