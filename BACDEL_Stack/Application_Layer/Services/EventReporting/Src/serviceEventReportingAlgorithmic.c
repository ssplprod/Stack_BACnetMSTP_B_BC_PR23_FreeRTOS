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
*
*   FILE
*	 serviceEventReportingAlgorithmic.c
*                                                                      
*   AUTHORS
*		Pratham N. Murkute
*                                                                         
*   DESCRIPTION
*		Algorithmic Event Notification Reporting.
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE			NAME					DESCRIPTION
*   30/oct/2012		Pratham N. Murkute		Created file & added api's.
*
******************************************************************************/

#if (defined ALGORITHMIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_EE && defined BACDEL_OBJ_NC)

/* include header files */
#include "serviceEventReportingAlgorithmic.h"

#include "serviceEventNotification_B.h"

#include "propertyGenricHandler.h"
#include "Debug.h"
#include "propertyValueRead.h"
#include "propertyValueWrite.h"
#include "pduDataEncodeDecode.h"
//#include "ExternalAlgorithmicAlarms.h"

/** global structure for algorithmic reporting linklist  - defination */
AlgorithmicLinkList_t g_stAlgorithmicReporting = {0};

ExtAlgoEventStateLinkList_t g_stExternalAlgorithmic = {0};

/***
*
*	DESCRIPTION
*		This function enables or disables EE object for 
*	algorithmic reporting.
*   
*@param pvObjAddr 	 	[in] Base address of EE object.
*@param bValue 		 	[in] enable or disable flag.
*@param pvVirtualDev 	[in] virtual device pointer.
*@param bCngEventState 	[in] change event state flag.
*@return nothing.
*
***/
static void EnableDisable_EE_Object(void *pvObjAddr, bool bValue, void *pvVirtualDev, bool bCngEventState)
{
	#if(defined DEBUG_PRINTF && DL_2)
	/* EE object pointer */
	EventEnrollment_t *pstEE = NULL;
	#endif
	/* to save offset address */
	ulong32_t ul32BaseAddr = 0; 
    ulong32_t ul32OffsetAddr = 0;
	/* dummy pointer */
	void *pvDummy =  NULL;
    /* to write event enable property */
    Pr_BACnetBitStr_t stBitString = {0};
    uint32_t u32PropertyAdd = 0;
    PROP_ACCESS_TYPE eAccessType = BACNET_DEFAULT;
	BACNET_EVENT_STATE eEventState = EVENT_STATE_NORMAL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	EnableDisable_EE_Object: entry \r\n");
	#endif

    if(NULL == pvObjAddr)
        return;

	#if(defined DEBUG_PRINTF && DL_2)
	/* get base address of object */
	pstEE = pvObjAddr;
	#endif

	/* convert the obj base address */
    ul32BaseAddr = (ulong32_t )pvObjAddr;

	/* if false then disable */
	if(!bValue)
	{
		/* event type, event parameter & obj prop ref validation failed */
		/* so as per clause 13.3
			If any of the parameters required to process the event algorithm are 
			missing or inconsistent with the Event_Type, the Event Enrollment object 
			shall become disabled and all the bit flags in the Event_Enable 
			property shall be cleared. */

		/* make object enable to false */
		GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, bObjectEnable, ul32OffsetAddr);
		pvDummy = (void *)(ul32BaseAddr+ul32OffsetAddr);
		*((bool *)pvDummy) = FALSE;

		/* clear all bits of event enable property */
        stBitString.m_stBitStr.m_u8ByteCnt = 1;
        stBitString.m_stBitStr.m_u8UnusedBits =  5;

        #ifdef USER_DEFINED_ACCESS_TYPE		               
		/* get property access type value */
		GET_INTRINSIC_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stEventEnable.m_eAccessType, 
            ul32OffsetAddr);
		memcpy(&eAccessType, (void *)(ul32BaseAddr+ul32OffsetAddr), 
            sizeof(PROP_ACCESS_TYPE));
        stBitString.m_eAccessType = eAccessType;
        #endif

        /* call UpdateCommonProperty function to change event_enable */
	    UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvObjAddr, PROP_EVENT_ENABLE,
		    -1, (void *)&stBitString, eAccessType,
		    &u32PropertyAdd, &pvDummy, pvVirtualDev,
		    false, false, false);

		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
		EnableDisable_EE_Object: EE object with id %d is disabled for \
		algorithmic reporting. \r\n",
		pstEE->m_stObjectID.m_u32ObjId);
		#endif
	}
	/* if true then enable */
	else
	{
		/* make object enable to true */
		GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, bObjectEnable, ul32OffsetAddr);
		pvDummy = (void *)(ul32BaseAddr+ul32OffsetAddr);
		*((bool *)pvDummy) = TRUE;

		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
		EnableDisable_EE_Object: EE object with id %d is enables for\
		algorithmic reporting. \r\n",
		pstEE->m_stObjectID.m_u32ObjId);
		#endif
	}

	/* check whether event state should update */
	if(true == bCngEventState)
	{
		/* call UpdateCommonProperty function to change event_state to normal */
		UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvObjAddr, PROP_EVENT_STATE,
			-1, (void *)&eEventState, READ_ONLY,
			&u32PropertyAdd, &pvDummy, pvVirtualDev,
			false, false, false);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	EnableDisable_EE_Object: exit \r\n");
	#endif
	return;
}

/***
*
*	DESCRIPTION
*		This api event type, event parameter & object property ref have 
*	valid values or not.
*	- if values are valid, this EE obj is enabled for algorithmic reporting.
*	- if invalid values, then this obj is disabled & its event enable is cleared.
*   
*@param pvObjAddr 		[in] base address of EE object.
*@param pvVirtualDev 	[in] virtual device instance.
*@param bCngEventState 	[in] change event state flag.
*@param pvInRefObj 		[in] returns object address of ref property
*@param pvInFeedBkObj 	[in] returns object address of feedback property
*@param pbIsExternal 	[in] specifies if internal or external alarms.
*
*@return true if success else false.
*
***/
bool Validate_EE_Obj_Properties(void *pvObjAddr, void *pvVirtualDev, bool bCngEventState,
	void **pvInRefObj, void **pvInFeedBkObj, bool *pbIsExternal)
{
	/* EE object pointer */
	EventEnrollment_t *pstEE = NULL;
	/* return value of function */
	bool bValue = false;
	bool bIsExternal = false;
	/* to save reference property */
	BACNET_PROPERTY_ID ePropId = MAX_PROP_SUPPORTED;
	/* to save reference object type */
	BACNET_OBJECT_TYPE	eObjectType = MAX_BACNET_OBJECT_TYPE;
	/* to save property tag type */
	BACNET_APPLICATION_TAG eTagType = TAG_NOT_SUPPORTED;
	/* to save event type */
	BACNET_EVENT_TYPE eEventType = MAX_EVENT_TYPE; 
	/* to get error for array index validation */
	BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER;
    /* for reference & feedback object base address */
    void *pvRefObj = NULL;
    void *pvFeedBkObj = NULL;
	#ifdef BACDEL_PR14
	void *pvDummy = NULL;
	uint32_t u32PropertyAdd = 0;
	BACNET_RELIABILITY	eReliability = MAX_RELIABILITY;
	#endif

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Validate_EE_Obj_Properties: entry \r\n");
	#endif

	/* null check for pointers */
	if(NULL == pvObjAddr || NULL == pvInRefObj ||
		NULL == pvInFeedBkObj || NULL == pvVirtualDev || NULL == pbIsExternal)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Validate_EE_Obj_Properties: Null input pointer. \r\n");
		#endif
		return bValue;
	}
	/* get base address of object */
	pstEE = pvObjAddr;
	/* reset pointers */
	*pvInRefObj = NULL;
	*pvInFeedBkObj = NULL;
	*pbIsExternal = false;

	#ifdef BACDEL_PR14
	/* check event detection enable is true or false */
	if(!pstEE->m_stEventDetectionEnable.m_bVal)
	{
		/* algorithmic reporting is disabled in this object */
		return bValue;
	}

	/* check event algorithm inhibit is true or false */
	if(pstEE->m_stEventAlgorithmInhibit.m_bVal)
	{
		/* algorithmic reporting is disabled in this object */
		return bValue;
	}
	#endif /* PR14 */

	/* check if external algorithmic reporting is supported or not */
	bIsExternal = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bDeviceIdPresent;
	*pbIsExternal = bIsExternal;
	#if (STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS)
	if(bIsExternal)
	{
		/* save address of EE object itself */
		pvRefObj = pvObjAddr;
	}
	else
	#endif
    /* find the reference object */
	pvRefObj = Find_Object(pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_eObjectType,
        pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_u32ObjId, pvVirtualDev);

    /* check for null pointers */
    if(NULL == pvObjAddr || NULL == pvRefObj)
        return bValue;

	/* save reference object address */
	*pvInRefObj = pvRefObj;

	/* get event type */
	eEventType = pstEE->m_stEventype.m_eEventType;
	/* get object reference property */
	ePropId = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_ePropertyIdentifier;
	/* get reference object type */
	eObjectType = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_eObjectType;

	/* get property tag type */
    eTagType = GetPropertyTag(eObjectType, ePropId, 
        pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_u32ArrayIndex,
        pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bArrIndxPresent);

	/* NOTE - can validate ref obj here & also feedback obj in case of cmd fail */

	/* switch to event type */
	switch(eEventType)
	{
		case EVENT_CHANGE_OF_STATE:
		{
			if(BACNET_APPLICATION_TAG_BOOLEAN == eTagType ||
			   BACNET_APPLICATION_TAG_UNSIGNED_INT == eTagType ||
			   BACNET_APPLICATION_TAG_ENUMERATED == eTagType)
			{
				BACNET_APPLICATION_TAG eParameterTagType;
				BACNET_PROPERTY_STATES	eStateTag;
				ListOfBACnetPropertyStates_t *pstListVal = NULL;
				/* get list of values 1st pointer */
				pstListVal = pstEE->m_stEventParameter.m_stEventParam.
					BACnetEventParameter_u.m_stCngState.m_pstListOfValues;
				while(NULL != pstListVal)
				{
					/* get eStateTag */
					eStateTag = pstListVal->m_stPropStateVal.m_ePropState;
					if(eStateTag == PROP_STATE_BOOLEAN_VALUE)
						/* save prop app tag, useful further */
						pstListVal->m_stPropStateVal.m_eAppTag = 
						BACNET_APPLICATION_TAG_BOOLEAN;
					else if(eStateTag == PROP_STATE_UNSIGNED_VALUE)
						/* save prop app tag, useful further */
						pstListVal->m_stPropStateVal.m_eAppTag = 
						BACNET_APPLICATION_TAG_UNSIGNED_INT;
					else
						/* save prop app tag, useful further */
						pstListVal->m_stPropStateVal.m_eAppTag = 
						BACNET_APPLICATION_TAG_ENUMERATED;
					
					/* get the tag type from event parameters */
					eParameterTagType = pstListVal->m_stPropStateVal.m_eAppTag;
					if(eTagType == eParameterTagType)
						bValue = TRUE;
					else
					{
						bValue = FALSE;
						break; //bValue is false
					}
					/* move to next value */
					pstListVal = pstListVal->m_pstNext;
				}
			}
			else; //bValue is false
		}break;

		case EVENT_COMMAND_FAILURE:
		{
			if(BACNET_APPLICATION_TAG_BOOLEAN == eTagType ||
			   BACNET_APPLICATION_TAG_UNSIGNED_INT == eTagType ||
			   BACNET_APPLICATION_TAG_ENUMERATED == eTagType)
			{
				BACNET_PROPERTY_ID eFeedbackPropId = MAX_PROP_SUPPORTED;
				BACNET_OBJECT_TYPE	eFeedbackObjectType = MAX_BACNET_OBJECT_TYPE;
				BACNET_APPLICATION_TAG eFeedbackTagType = TAG_NOT_SUPPORTED;

				/* get feedback property */
				eFeedbackPropId = pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
					m_stCmdFail.m_stFeedbackPropertyReference.m_ePropertyIdentifier;
				/* get feedback object type */
				eFeedbackObjectType = pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
					m_stCmdFail.m_stFeedbackPropertyReference.m_eObjectType;
				/* get property tag type */
                eFeedbackTagType = GetPropertyTag(eFeedbackObjectType, eFeedbackPropId, 
                                    pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
                                    m_stCmdFail.m_stFeedbackPropertyReference.m_u32ArrayIndex,
                                    pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
                                    m_stCmdFail.m_stFeedbackPropertyReference.m_bArrIndxPresent);

				#if (STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS)
				/* check if external algorithmic reporting is configured */
				if(bIsExternal)
				{
					/* get tag value for external alarms */
					/* for external alarms, tag type has ANY value, so allow this value */
					eFeedbackTagType = eTagType;
				}
				#endif

				if(eFeedbackTagType == eTagType)
				{
					/* save the app tag type in cmdFail struct of event parameters */
					pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
						m_stCmdFail.m_u8Apptag = eTagType;
					/* TODO */
					/* NOTE - can validate that both ref prop & feedback prop id are same 
						or their tag types are same */
					/* FIXME - check if device id for reference prop & feedback prop are same 
						NOTE - do we need this validation ?? */

					/* check if external algorithmic reporting is supported or not */
					#if (STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS)
					if(bIsExternal)
					{
						/* save address of EE object itself */
						pvFeedBkObj = pvObjAddr;
					}
					else
					#endif
                    /* find the feedback object */
                    pvFeedBkObj = Find_Object(pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
						m_stCmdFail.m_stFeedbackPropertyReference.m_eObjectType,
                        pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
						m_stCmdFail.m_stFeedbackPropertyReference.m_u32ObjId,
                        pvVirtualDev);
					/* save command failure feedback object base address */
					*pvInFeedBkObj = pvFeedBkObj;

					/* validate if incorrect arry index for feedback property */
					if(!bIsExternal)
					{
						eErrorCode = ValidateAccessAndIndex(eFeedbackPropId,
							pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
							m_stCmdFail.m_stFeedbackPropertyReference.m_eObjectType,
							pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
							m_stCmdFail.m_stFeedbackPropertyReference.m_u32ArrayIndex,
							pvFeedBkObj, NULL,
							pstEE->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
							m_stCmdFail.m_stFeedbackPropertyReference.m_bArrIndxPresent);
						if(ERROR_CODE_OTHER == eErrorCode)
							bValue = TRUE;
						else; //bValue is false
					}
					else bValue = TRUE;
				}
				else; //bValue is false
			}
			else; //bValue is false
		}break;

		case EVENT_OUT_OF_RANGE:
		{
			if(BACNET_APPLICATION_TAG_REAL == eTagType)
				bValue = TRUE;
			else; //bValue is false
		}break;

		case EVENT_UNSIGNED_OUT_OF_RANGE:
		{
			if(BACNET_APPLICATION_TAG_UNSIGNED_INT == eTagType && 
				OBJECT_POSITIVE_INTEGER_VALUE == pstEE->m_stObjectPropertyReference.
				m_stDevObjPropReff.m_eObjectType)
				bValue = TRUE;
			else; //bValue is false
		}break;

		#ifdef BACDEL_PR14
		case EVENT_NONE:
		{
			bValue = TRUE;
		}break;
		#endif

		default:
			#if(defined DEBUG_PRINTF && DL_2)
			Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			Validate_EE_Obj_Properties: event type not supported by this stack \r\n");
			#endif
		break; //bValue is false
	}
	
	/* check if reference property has valid array index */
	if(bValue && !bIsExternal)
	{
		/* validate if incorrect arry index for reference property */
        eErrorCode =  ValidateAccessAndIndex(ePropId,
			pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_eObjectType,
            pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_u32ArrayIndex,
            pvRefObj, NULL,
            pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bArrIndxPresent);
		if(ERROR_CODE_OTHER != eErrorCode)
			bValue = FALSE;
	}

	#ifdef BACDEL_PR14
	/* update reliability to configuration-error as validation 
	   of request parameter failed */
	if(!bValue && !pstEE->m_stReliabilityEvaluationInhibit.m_bVal)
	{
		/* check reliability is other than config error */
		if(RELIABILITY_CONFIGURATION_ERROR != pstEE->m_stReliability.m_eReliabilty)
		{
			/* set reliability to configuration error */
			eReliability = RELIABILITY_CONFIGURATION_ERROR;
			/* call function to change reliability */
			UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvObjAddr, PROP_RELIABILITY, 
				-1, (void *)&eReliability, READ_ONLY, &u32PropertyAdd, &pvDummy,
				pvVirtualDev, false, false, false);
		}
	}
	else if(bValue && !pstEE->m_stReliabilityEvaluationInhibit.m_bVal)
	{
		/* check reliability is other than no fault detected */
		if(RELIABILITY_NO_FAULT_DETECTED != pstEE->m_stReliability.m_eReliabilty)
		{
			/* set reliability to no fault detected */
			eReliability = RELIABILITY_NO_FAULT_DETECTED;
			/* call function to change reliability */
			UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvObjAddr, PROP_RELIABILITY, 
				-1, (void *)&eReliability, READ_ONLY, &u32PropertyAdd, &pvDummy,
				pvVirtualDev, false, false, false);
		}
	}
	#endif

	/* enable or disable the EE object */
	EnableDisable_EE_Object(pvObjAddr, bValue, pvVirtualDev, bCngEventState);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Validate_EE_Obj_Properties: exit \r\n");
	#endif
	return bValue;
}

/** fills the data for node to be added in algorithmic reporting linklist */
void *Fill_Data_For_Algorithmic_Reporting(void *pvObject, 
	void *pVirtualDevive, bool bCngEventState, void *pvRefObj, void *pvFeedBkObj)
{
	/* EE object pointer */
	EventEnrollment_t *pstEE = NULL;
	/* noda data */
	AlgorithmicListNode_t *pstNode = NULL;
	/* virtual device pointer */
	virtualDevData_t *pVirtualDev = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Fill_Data_For_Algorithmic_Reporting: entry \r\n");
	#endif

	/* null check for pointers */
	if(NULL == pvObject || NULL == pVirtualDevive)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Fill_Data_For_Algorithmic_Reporting: Null input pointer. \r\n");
		#endif
		return NULL;
	}

	/* get base address of object */
	pstEE = pvObject;
	/* get virtual device pointer */
	pVirtualDev = pVirtualDevive;

	pstNode = OSAL_Malloc(sizeof(AlgorithmicListNode_t), __FILE__,__FUNCTION__,__LINE__);
    if(NULL == pstNode)
    {
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		Fill_Data_For_Algorithmic_Reporting: malloc failed \r\n");
		#endif

        return NULL;
    }
	/* initialize memory */
	//memset(pstNode,0,sizeof(AlgorithmicListNode_t));

	/* fill the data */
	pstNode->m_stNodeData.m_eObjectType = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_eObjectType;
	pstNode->m_stNodeData.m_u32ObjId = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_u32ObjId;
	pstNode->m_stNodeData.m_u32DevId = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_u32DeviceInstace;

	pstNode->m_stNodeData.m_ePropId = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_ePropertyIdentifier;
	pstNode->m_stNodeData.m_u32ArrayIndex = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_u32ArrayIndex;
    pstNode->m_stNodeData.m_bArrIndxFlag = pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bArrIndxPresent;

	pstNode->m_stNodeData.m_u32EEObjId = pstEE->m_stObjectID.m_u32ObjId;
	pstNode->m_stNodeData.m_pvEEObjAddr = pvObject;

	/* save event state */
	pstNode->m_stNodeData.m_eEventStateFrom = pstEE->m_stEventState.m_eEventState;
	pstNode->m_stNodeData.m_eEventStateTo = pstEE->m_stEventState.m_eEventState;

	/* make next pointer null */
	pstNode->m_pstNext = NULL;

	/* save virtual device pointer */
	pstNode->m_stNodeData.m_pVirtualDev = pVirtualDev;

	/* set the start timer flag to false */
	pstNode->m_stNodeData.m_bStartTimer = FALSE;

	/* check if ref obj belongs to this device or other */
	if((pstNode->m_stNodeData.m_u32DevId ==
		pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId && 
		pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bDeviceIdPresent) 
		||
		(!pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bDeviceIdPresent))
	{
		/* object belongs to this device */
		if(NULL != pvRefObj)
		{
			/* save device id  */
			pstNode->m_stNodeData.m_u32DevId = pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId;
			/* object found */
			pstNode->m_stNodeData.m_bThisDevice = TRUE;
			/* save its address */
			pstNode->m_stNodeData.m_pvRefObjAddr = pvRefObj;
			/* */
			if(EVENT_COMMAND_FAILURE == pstEE->m_stEventype.m_eEventType)
			{
				if(NULL != pvFeedBkObj)
				{
					/* save its address */
					pstNode->m_stNodeData.m_pvFbObjAddr = pvFeedBkObj;
				}
				else
				{
					/* disable the EE object */
					EnableDisable_EE_Object(pvObject, false, pVirtualDevive, bCngEventState);
					/* free the malloced memory & return null */
					OSAL_Free(pstNode, __FILE__,__FUNCTION__,__LINE__);

					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
					Fill_Data_For_Algorithmic_Reporting: feedback object not found \r\n");
					#endif
					return NULL;
				}
			}
		}
		else
		{
			/* disable the EE object */
			EnableDisable_EE_Object(pvObject, false, pVirtualDevive, bCngEventState);
			/* free the malloced memory & return null */
			OSAL_Free(pstNode, __FILE__,__FUNCTION__,__LINE__);

			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
			Fill_Data_For_Algorithmic_Reporting: reference object not found \r\n");
			#endif
			return NULL;
		}
	}
	else
	{
		// value is already false
		pstNode->m_stNodeData.m_bThisDevice = FALSE;
		/* NOTE - as we are not supporting objects from other devices,
					return null.
				if we support then do not free, do not disable EE & 
				return the pointer */
		/* disable the EE object */
		EnableDisable_EE_Object(pvObject, false, pVirtualDevive, bCngEventState);
		/* free the malloced memory & return null */
		OSAL_Free(pstNode, __FILE__,__FUNCTION__,__LINE__);

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
		Fill_Data_For_Algorithmic_Reporting: other device object \r\n");
		#endif
		return NULL;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Fill_Data_For_Algorithmic_Reporting: exit \r\n");
	#endif
	return pstNode;
}

/**	adds new node to linklist */
static void Add_New_Node(AlgorithmicListNode_t *pstNode)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Add_New_Node: entry \r\n");
	#endif

	if(g_stAlgorithmicReporting.pstFirstElem == NULL)
		/*update the root node address only when 1st node for the list is created */
		g_stAlgorithmicReporting.pstFirstElem = pstNode;
	else
		/* create the list here */
		//(*g_stAlgorithmicReporting.pstLastElem).m_pstNext = pstNode;
		g_stAlgorithmicReporting.pstLastElem->m_pstNext = pstNode;
	/* update the current node address */
	g_stAlgorithmicReporting.pstLastElem = pstNode;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Add_New_Node: exit \r\n");
	#endif

	return;
}

/**	replaces old node from linklist with new data */
static void Replace_Node(AlgorithmicListNode_t *pstLocalNode, 
						 AlgorithmicListNode_t *pstNode)
{
	/* copy the next pointer from original node to new node */
	pstNode->m_pstNext = pstLocalNode->m_pstNext;
	/* copy data from pstNode to pstLocalNode */
	memcpy(pstLocalNode, pstNode, sizeof(AlgorithmicListNode_t));
	/* free the memory */
	OSAL_Free(pstNode, __FILE__,__FUNCTION__,__LINE__);
}

/** create linklist to monitor property values for algorithmic alarms */
void Create_Algorithmic_Reporting_Linklist(void *pvListData)
{
	AlgorithmicListNode_t *pstLocalNode = NULL;
	AlgorithmicListNode_t *pstNode = NULL;
	bool bNodeAlreadyExist = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Create_Algorithmic_Reporting_Linklist: entry \r\n");
	#endif

	if(NULL == pvListData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Create_Algorithmic_Reporting_Linklist: null input pointer \r\n");
		#endif

		return;
	}
	/* get node pointer */
	pstNode = pvListData;

	/* Wait till the mutex is released */
	Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 

	/* first node in linklist should be directly added to list */
	if(g_stAlgorithmicReporting.pstFirstElem == NULL && 
		g_stAlgorithmicReporting.pstLastElem == NULL)
	{
		Add_New_Node(pstNode);
	}
	/* if 1st node exists, then we need to check if the node with same 
	object type & id already exists */
	else
	{
		/* local pointer to traverse linklist */
		pstLocalNode = g_stAlgorithmicReporting.pstFirstElem;
		/* traverse the linklist */
		while(pstLocalNode != NULL)
		{
			if(pstLocalNode->m_stNodeData.m_pvEEObjAddr == 
				pstNode->m_stNodeData.m_pvEEObjAddr	
				&&
			   pstLocalNode->m_stNodeData.m_u32EEObjId == 
			   pstNode->m_stNodeData.m_u32EEObjId)
			{
				bNodeAlreadyExist = TRUE;
				break;
			}
			/* maintain the previous node pointer */
			pstLocalNode = pstLocalNode->m_pstNext;
		}
		if(bNodeAlreadyExist == TRUE)
		{
			bNodeAlreadyExist = FALSE;
			/* replace the data of node with the new data */
			Replace_Node(pstLocalNode, pstNode);

			#if(defined DEBUG_PRINTF && DL_2)
			Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			Create_Algorithmic_Reporting_Linklist: node replaced in algorithmic linklist \r\n");
			#endif
		}
		else
		{
			/* since node with same obj address does not exist, add new node to link list */
			Add_New_Node(pstNode);
		}
	}

	/* release the mutex */
	Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Create_Algorithmic_Reporting_Linklist: exit \r\n");
	#endif

	return;
}

/* delete a node from algorithmic reporting linklist */
void Delete_Node(AlgorithmicListNode_t *pstCurrentNode, AlgorithmicListNode_t *pstPrevNode)
{
	/* local variables */
	AlgorithmicListNode_t *pstNextNode = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Delete_Node: entry \r\n");
	#endif

    if(NULL == pstCurrentNode)
        return;

	/* get the next node */
	pstNextNode = pstCurrentNode->m_pstNext;

	if(NULL == pstPrevNode && NULL == pstNextNode)
	{
		/* only 1 node in linklist */
		/* reset global pointers */
		g_stAlgorithmicReporting.pstFirstElem = NULL;
		g_stAlgorithmicReporting.pstLastElem = NULL;
	}
	if(NULL == pstPrevNode)
	{
		/* this is 1st node of linklist */ //g_stAlgorithmicReporting
		/* make next node as 1st node of linklist */
		g_stAlgorithmicReporting.pstFirstElem = pstNextNode;

	}
	else if(NULL == pstNextNode)
	{
		/* this is last node of linklist */
		pstPrevNode->m_pstNext = pstNextNode;
		/* make previous node as last node of linklist */
		g_stAlgorithmicReporting.pstLastElem = pstPrevNode;
	}
	else
	{
		/* middle node of linklist */
		pstPrevNode->m_pstNext = pstNextNode;
	}

	/* free the memory */
	OSAL_Free(pstCurrentNode, __FILE__,__FUNCTION__,__LINE__);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Delete_Node: exit \r\n");
	#endif

	return;
}

#if 0
/***
*
*	DESCRIPTION
*   
*
*
*@param
*@param
*@return 
*
***/
static int8_t Algorithmic_Fault_Notification(AlgorithmicListNode_t *pstLocalNode,
					EventEnrollment_t *pstEE)
{
	/* variable to store value of EVENT_STATE property */
	BACNET_EVENT_STATE	eEventState;
	/* to save offset address */
	ulong32_t ul32BaseAddr = 0; 
    ulong32_t ul32OffsetAddr = 0;
	/* void pointer */
	void *pvObject =  NULL;
	/* to save reliablity */
	BACNET_RELIABILITY  eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
	void *pvDummy = NULL;
	uint32_t u32PropertyAdd = 0;
	PropertyValue_u uPropertyValue = {0};
    /* hold Sizeof */
    int8_t i8Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Algorithmic_Fault_Notification: entry \r\n");
	#endif

	if(NULL == pstLocalNode || NULL == pstEE)
		return BACNET_ZERO;
	if(NULL == pstLocalNode->m_stNodeData.m_pvRefObjAddr)
		return BACNET_ZERO;

	/* get current event state of EE object */
	eEventState = pstEE->m_stEventState.m_eEventState;

	/* convert the obj base address */
    ul32BaseAddr = (ulong32_t )pstLocalNode->m_stNodeData.m_pvRefObjAddr;

	/* get the offset address of reliablity property */
	GET_RELIABILITY_OFFSET(pstLocalNode->m_stNodeData.m_eObjectType, 
		m_stReliability.m_eReliabilty, ul32OffsetAddr);
	/* copy the value */
	memcpy(&eReliabilty, (BACNET_RELIABILITY *)(ul32BaseAddr+ul32OffsetAddr),
		sizeof(BACNET_RELIABILITY));

	if(RELIABILITY_NO_FAULT_DETECTED != eReliabilty)
	{
		/* fault detected & event state is not fault */
		if(EVENT_STATE_FAULT != eEventState)
		{
			/* update from event state */
			pstLocalNode->m_stNodeData.m_eEventStateFrom = eEventState;
			/* update to event state */
			pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_FAULT;

			/* get the offset address of reliablity property */
			GET_PRESENTVAL_OFFSET(pstLocalNode->m_stNodeData.m_eObjectType, 
				m_stPresentValue, ul32OffsetAddr, i8Len);
			/* copy the value */
			memcpy(&uPropertyValue.u32Val, (void *)(ul32BaseAddr+ul32OffsetAddr), i8Len);
			/* assign present value pointer */
			pvDummy = &uPropertyValue.u32Val;
			/* call update common property to update event state
				of the event enrollment object */
			UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, 
				(void *)pstLocalNode->m_stNodeData.m_pvEEObjAddr, 
				PROP_EVENT_STATE, 
				BACNET_ARRAY_ALL, 
				(void *)&pstLocalNode->m_stNodeData.m_eEventStateTo,
				READ_ONLY,
				&u32PropertyAdd, &pvDummy, 
				pstLocalNode->m_stNodeData.m_pVirtualDev,
				false, false, false);

			#if(defined DEBUG_PRINTF && DL_2)
			Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			Algorithmic_Fault_Notification: reference object in fault state. \r\n");
			#endif

			return BACNET_ONE;
		}
		/* fault detected & event state is fault */
		else
		{
			#if(defined DEBUG_PRINTF && DL_2)
			Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			Algorithmic_Fault_Notification: reference dev in fault state. \r\n");
			#endif

			return BACNET_NEGATIVE_ONE;
		}
	}
	else;
		/* no fault detected */

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Algorithmic_Fault_Notification: exit \r\n");
	#endif

	return BACNET_ZERO;
}
#endif /* 0 */

/***
*
*	DESCRIPTION
*   This is timer for Algorithmic Reporting service. It is called every 1 second.
*
*	- scan algorithmic reporting linklist, one node at a time.
*   - read the property value of reference property.
*	- compare the value with limits as per event type.
*   - if limits are exceeded, update event state.
*   - continue with the next node.
*
*@param  lpParam            [in]    not used.
*@param  TimerOrWaitFired   [in]    not used.
*@return                    [out]   nothing.
*
***/
TIMER_CALLBACK AlgorithmicReportingTimer(void* lpParam, bool TimerOrWaitFired)
{
	/* Host Device Structure pointer contianing Device object information */
    virtualDevData_t *pVirtualDev = NULL;
	/* pointer to traverse event_enrollment object list */
	EventEnrollment_t *pstEE = NULL;
	/* to save object event type */
	BACNET_EVENT_TYPE eEventType = MAX_EVENT_TYPE;
	/* to check event_enrollment object state */
	bool bObjEnable  = false;
	/* flags for operations */
	bool bCompare = false;
	int8_t i8FaultFlag = 0;
	/* char buffer for property value */
	uint8_t cBuf[MAX_APPLN_PROP_VAL_STR] = {0};
	/* to save error code */
	BACNET_ERROR_CODE eErrCode = ERROR_CODE_OTHER;
	AlgorithmicListNode_t *pstLocalNode = NULL;
	AlgorithmicListNode_t *pstPrevNode = NULL;
	AlgorithmicListNode_t *pstNextNode = NULL;
	/* to save property values */
	BACNET_PROPERTY_VALUE stFeedbackVal = {0};
	BACNET_PROPERTY_VALUE stPropVal = {0};
	/* variable to store value of EVENT_STATE property */
	BACNET_EVENT_STATE	eEventState = MAX_EVENT_STATE;
    /* variable for double notification */
    bool bDoubleNotification = FALSE;
	static bool bAlgorithmicTimerFlag = false;
	#ifdef BACDEL_PR14
	/* reliablity */
	BACNET_RELIABILITY	eReliability = MAX_RELIABILITY;
	bool bEventAlgoInhibitBackup = FALSE;
	#endif

	/* avoid concurrent execution of multiple timer instances */
	if(true == bAlgorithmicTimerFlag)
	{
		/* return from function */
		#ifdef __linux
		return NULL;
		#else
		return;
		#endif
	}
	/* set the flag */
	bAlgorithmicTimerFlag = true;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AlgorithmicReportingTimer: entry \r\n");
	#endif

	//#if (STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS)
	/* execute external algorithmic alarms module */
	Ext_AlgorithmicAlarm_Timer(NULL, FALSE);
	//#endif

#if 0
	/* Wait till the mutex is released */
	Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE);

	/* get 1st node of linklist */
	pstLocalNode = g_stAlgorithmicReporting.pstFirstElem;

	while(NULL != pstLocalNode)
	{
		/* get host device pointer */
		//pVirtualDev = lpParam;
		pVirtualDev = pstLocalNode->m_stNodeData.m_pVirtualDev;
		if(NULL == pVirtualDev)
			break;

		/* break the loop if algorithmic reporting is disabled */
#if 0
		if(!g_stAlgorithmicReporting.bEnableAlgorithmicReporting)
			break;
#endif

        /* check if this node is to be deleted */
        if(pstLocalNode->m_stNodeData.m_u8RetryCount >= MAX_ALGORITHMIC_RETRY_COUNT)
        {
            /* disable this EE object to delete it from linklist */
            EnableDisable_EE_Object(pstLocalNode->m_stNodeData.m_pvEEObjAddr, false, 
				pVirtualDev, false);
        }

		/* get event type value */
		pstEE = pstLocalNode->m_stNodeData.m_pvEEObjAddr;

		#if (STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS)
		/* check if external algorithmic reporting is configured for this object */
		if(pstEE->m_stObjectPropertyReference.m_stDevObjPropReff.m_bDeviceIdPresent)
		{
			/* disable this EE object to delete it from linklist as it is configured'
			for external alarms */
			EnableDisable_EE_Object(pstLocalNode->m_stNodeData.m_pvEEObjAddr, false,
				pVirtualDev, false);
		}
		#endif

		/* get object state, if enable or disable */
		bObjEnable = pstEE->bObjectEnable;

		#ifdef BACDEL_PR14
		/* check if event-detection is disabled */
		if(!pstEE->m_stEventDetectionEnable.m_bVal)
		{
			/* Wait till the mutex is released */
			//Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 
			/* maintain previous node */
			pstPrevNode = pstLocalNode;
			/* move to next node in link list */
			pstLocalNode = pstLocalNode->m_pstNext;
			/* release the mutex */
			//Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);
			/* reset bCompare flag */
			bCompare = FALSE;
			continue;
		}

		/* check if event-algorithm is disabled */
		/* backup-rel-inhibit flag is used to send to normal notification and then
		disable further monitoring of this EE node, till event-algo-inhibit is enabled */
		bEventAlgoInhibitBackup = pstEE->m_bBackupRelEvalInhibit;
		if(pstEE->m_stEventAlgorithmInhibit.m_bVal && bEventAlgoInhibitBackup)
		{
			/* Wait till the mutex is released */
			//Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 
			/* maintain previous node */
			pstPrevNode = pstLocalNode;
			/* move to next node in link list */
			pstLocalNode = pstLocalNode->m_pstNext;
			/* release the mutex */
			//Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);
			/* reset bCompare flag */
			bCompare = FALSE;
			continue;
		}

		/* read current reliability of object */
		eReliability = pstEE->m_stReliability.m_eReliabilty;
		if(RELIABILITY_NO_FAULT_DETECTED != eReliability)
		{
			/* Wait till the mutex is released */
			//Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 
			/* maintain previous node */
			pstPrevNode = pstLocalNode;
			/* move to next node in link list */
			pstLocalNode = pstLocalNode->m_pstNext;
			/* release the mutex */
			//Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);
			/* reset bCompare flag */
			bCompare = FALSE;
			continue;
		}
		#endif

		if(true == bObjEnable)
		{
			if(pstLocalNode->m_stNodeData.m_bThisDevice)
			{
#if 0
				/* check FAULT flag of status flag property of reference object.
					if required generate notification for To_Fault or From_fault */
				i8FaultFlag = Algorithmic_Fault_Notification(pstLocalNode, pstEE);
#endif
				if(i8FaultFlag <= 0)
				{
					/* read the reference property value */
					eErrCode =  BACDEL_Get_Object_Property_String
									(pstLocalNode->m_stNodeData.m_u32DevId,
									(uint8_t)pstLocalNode->m_stNodeData.m_eObjectType, 
									pstLocalNode->m_stNodeData.m_u32ObjId,
									(uint8_t)pstLocalNode->m_stNodeData.m_ePropId, 
									pstLocalNode->m_stNodeData.m_u32ArrayIndex,
									(void *)cBuf, MAX_APPLN_PROP_VAL_STR,
                                    pstLocalNode->m_stNodeData.m_bArrIndxFlag);
					if(MAX_BACNET_ERROR_CODE == eErrCode)
					{
                        /* Get property AppTag */
                        stPropVal.m_TagType = GetPropertyTag(pstLocalNode->m_stNodeData.m_eObjectType, 
                            pstLocalNode->m_stNodeData.m_ePropId, pstLocalNode->m_stNodeData.m_u32ArrayIndex,
                            pstLocalNode->m_stNodeData.m_bArrIndxFlag);

                        /* Convert string to Genric Value as per AppTag */
                        ConvertInStr_To_AppTag(stPropVal.m_TagType, (int8_t *)cBuf, &stPropVal);

						/* set the flag */
						bCompare = TRUE;
						/* clear the array memory */
						memset(cBuf, 0, sizeof(cBuf));
					}
					else
                    {
                        //error
                        /* increment the retry count for this node */
                        pstLocalNode->m_stNodeData.m_u8RetryCount++;
						bCompare = FALSE;
                    }
				}//if(!bFaultFlag) ends
			}
			else
			{
#ifdef support_other_device
				/* NOTE - we are not supporting objects of other device for
					algorithmic reporting.
					this code is partially completed & copied @ the end 
					of this file */
#else
                /* increment the retry count for this node */
                pstLocalNode->m_stNodeData.m_u8RetryCount = MAX_ALGORITHMIC_RETRY_COUNT;
                bCompare = FALSE;

                /* debug message */
				#if(defined DEBUG_PRINTF && DL_1)
				Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
				AlgorithmicReportingTimer: algorithmic reporting not supported for\
				objects of other device. \r\n");
				#endif
#endif /* support_other_device */
			}

			if(bCompare)
			{
				uint32_t u32TimeDelay = 0;
				/* get event type */
				eEventType = pstEE->m_stEventype.m_eEventType;
				/* get current event state of EE object */
				eEventState = pstEE->m_stEventState.m_eEventState;
				/* save from event state in node data */
				pstLocalNode->m_stNodeData.m_eEventStateFrom = eEventState;

				switch(eEventType)
				{
				case EVENT_CHANGE_OF_STATE:
				{
					ChangeOfState_t	*pstCngState = NULL;
					ListOfBACnetPropertyStates_t *pstListVal = NULL;

					pstCngState = &pstEE->m_stEventParameter.m_stEventParam.
						BACnetEventParameter_u.m_stCngState;

					#ifdef BACDEL_PR14
					/* event algorithm inhibit is disabled i.e. true, so send ToNormal 
					notification with time delay normal interval.
					when event algorithm inhibit is disabled, backup-event-algo-inhibit is false 
					till to normal notification is sent */
					if(!bEventAlgoInhibitBackup)
					{
						/* check if current state is not normal */
						if(EVENT_STATE_NORMAL != eEventState)
						{
							/* reset timer if previous value is not normal */
							if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* event algo is disabled, need to send to normal notification */
							pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
							pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
							u32TimeDelay = pstEE->m_stTimeDelayNormal.m_u32Val;
						}
						else
						{
							pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* reset backup flag to true as notification is sent, and skip this EE 
							node till event-algo-inhibit becomes enabled i.e false */
							pstEE->m_bBackupRelEvalInhibit = TRUE;
						}
						break;
					}
					#endif

					pstListVal = pstCngState->m_pstListOfValues;
					while(NULL != pstListVal)
					{
						/* compare the memories as there are many data type */
						if(!memcmp(&stPropVal.uValue,&pstListVal->m_stPropStateVal.BACnetPropertyStates_U,
							sizeof(pstListVal->m_stPropStateVal.BACnetPropertyStates_U)))
							/* match found */
							break;
						/* move to next element */
						pstListVal = pstListVal->m_pstNext;
					}
					if(NULL != pstListVal)
					{
						/* match found */
						if(eEventState != EVENT_STATE_OFFNORMAL)
						{
							/* reset timer if previous value is not offnormal */
							if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_OFFNORMAL)
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_OFFNORMAL;
							pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
						}
						else
						{
							pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						}
					}
					else
					{
						/* match not found */
						if(eEventState != EVENT_STATE_NORMAL)
						{
							/* reset timer if previous value is not normal */
							if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
							pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
						}
						else
						{
							pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						}
					}

					#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
					/* check double notification, use time dlay normal for ToNormal state */
					if(IsDoubleNotification(pstLocalNode->m_stNodeData.m_eObjectType,
						pstLocalNode->m_stNodeData.m_eEventStateFrom,
						pstLocalNode->m_stNodeData.m_eEventStateTo))
					{
						/* get time delay value */
						u32TimeDelay = pstEE->m_stTimeDelayNormal.m_u32Val;
					}
					else 
					#endif
					{
						/* get time delay value */
						#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
						u32TimeDelay = ((EVENT_STATE_NORMAL == pstLocalNode->m_stNodeData.m_eEventStateTo)
							? pstEE->m_stTimeDelayNormal.m_u32Val : pstCngState->m_u32Timedelay);
						#else
						u32TimeDelay = pstCngState->m_u32Timedelay;
						#endif
					}
				}break;

				case EVENT_COMMAND_FAILURE:
				{
					CommandFailure_t *pstCmdFail = NULL;

					pstCmdFail = &pstEE->m_stEventParameter.m_stEventParam.
						BACnetEventParameter_u.m_stCmdFail;

					#ifdef BACDEL_PR14
					/* event algorithm inhibit is disabled i.e. true, so send ToNormal 
					notification with time delay normal interval.
					when event algorithm inhibit is disabled, backup-event-algo-inhibit is false 
					till to normal notification is sent */
					if(!bEventAlgoInhibitBackup)
					{
						/* check if current state is not normal */
						if(EVENT_STATE_NORMAL != eEventState)
						{
							/* reset timer if previous value is not normal */
							if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* event algo is disabled, need to send to normal notification */
							pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
							pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
							u32TimeDelay = pstEE->m_stTimeDelayNormal.m_u32Val;
						}
						else
						{
							pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* reset backup flag to true as notification is sent, and skip this EE 
							node till event-algo-inhibit becomes enabled i.e false */
							pstEE->m_bBackupRelEvalInhibit = TRUE;
						}
						break;
					}
					#endif

					/* clear the array memory */
					memset(cBuf, 0, sizeof(cBuf));
					/* reset the flag */
					bCompare = FALSE;
					/* read the feedback property value */
					eErrCode =  BACDEL_Get_Object_Property_String
								(pstLocalNode->m_stNodeData.m_u32DevId,
								pstCmdFail->m_stFeedbackPropertyReference.m_eObjectType, 
								pstCmdFail->m_stFeedbackPropertyReference.m_u32ObjId,
								pstCmdFail->m_stFeedbackPropertyReference.m_ePropertyIdentifier,
								pstCmdFail->m_stFeedbackPropertyReference.m_u32ArrayIndex,
								(void *)cBuf, MAX_APPLN_PROP_VAL_STR,
                                pstCmdFail->m_stFeedbackPropertyReference.m_bArrIndxPresent);
					if(MAX_BACNET_ERROR_CODE == eErrCode)
					{
                        /* Get property AppTag */
						stFeedbackVal.m_TagType = GetPropertyTag(pstCmdFail->m_stFeedbackPropertyReference.m_eObjectType, 
                                                    pstCmdFail->m_stFeedbackPropertyReference.m_ePropertyIdentifier, 
                                                    pstCmdFail->m_stFeedbackPropertyReference.m_u32ArrayIndex,
                                                    pstCmdFail->m_stFeedbackPropertyReference.m_bArrIndxPresent);

                        /* Convert string to Genric Value as per AppTag */
                        ConvertInStr_To_AppTag(stFeedbackVal.m_TagType, (int8_t *)cBuf, &stFeedbackVal);

						/* save the value */
						memcpy(&pstCmdFail->m_uFeedbackPropValue, &stFeedbackVal.uValue,
							sizeof(PropertyValue_u));

						/* set the flag */
						bCompare = TRUE;
						/* clear the array memory */
						memset(cBuf, 0, sizeof(cBuf));
					}
					else
                    {
                        //error
                        /* increment the retry count for this node */
                        pstLocalNode->m_stNodeData.m_u8RetryCount++;
						bCompare = FALSE;
                    }
					if(bCompare)
					{
						if(!memcmp(&stPropVal.uValue, &stFeedbackVal.uValue, 
							sizeof(stPropVal.uValue)))
						{
							/* prop value & feedback value is equal */
							if(eEventState != EVENT_STATE_NORMAL)
							{
								/* reset timer if previous value is not normal */
								if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
									pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
								pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
								pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
							}
							else
							{
								pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							}
						}
						else
						{
							/* prop value & feedback value is not equal */
							if(eEventState != EVENT_STATE_OFFNORMAL)
							{
								/* reset timer if previous value is not offnormal */
								if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_OFFNORMAL)
									pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
								pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_OFFNORMAL;
								pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
							}
							else
							{
								pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							}
						}
					}

					#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
					/* check double notification, use time dlay normal for ToNormal state */
					if(IsDoubleNotification(pstLocalNode->m_stNodeData.m_eObjectType,
						pstLocalNode->m_stNodeData.m_eEventStateFrom,
						pstLocalNode->m_stNodeData.m_eEventStateTo))
					{
						/* get time delay value */
						u32TimeDelay = pstEE->m_stTimeDelayNormal.m_u32Val;
					}
					else 
					#endif
					{
						/* get time delay value */
						#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
						u32TimeDelay = ((EVENT_STATE_NORMAL == pstLocalNode->m_stNodeData.m_eEventStateTo)
							? pstEE->m_stTimeDelayNormal.m_u32Val : pstCmdFail->m_u32Timedelay);
						#else
						u32TimeDelay = pstCmdFail->m_u32Timedelay;
						#endif
					}
				}break;

				case EVENT_OUT_OF_RANGE:
				{
					OutOfRange_t *pstOutRange = NULL;
					pstOutRange = &pstEE->m_stEventParameter.m_stEventParam.
						BACnetEventParameter_u.m_stOutRange;

					#ifdef BACDEL_PR14
					/* event algorithm inhibit is disabled i.e. true, so send ToNormal 
					notification with time delay normal interval.
					when event algorithm inhibit is disabled, backup-event-algo-inhibit is false 
					till to normal notification is sent */
					if(!bEventAlgoInhibitBackup)
					{
						/* check if current state is not normal */
						if(EVENT_STATE_NORMAL != eEventState)
						{
							/* reset timer if previous value is not normal */
							if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
								pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* event algo is disabled, need to send to normal notification */
							pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
							pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
							u32TimeDelay = pstEE->m_stTimeDelayNormal.m_u32Val;
						}
						else
						{
							pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* reset backup flag to true as notification is sent, and skip this EE 
							node till event-algo-inhibit becomes enabled i.e false */
							pstEE->m_bBackupRelEvalInhibit = TRUE;
						}
						break;
					}
					#endif

					/* verify that high limit & low limit are not same, if they
						are same then dont compare */
					if(pstOutRange->m_fLow_Limit == pstOutRange->m_fHigh_Limit)
					{
						pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
						pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						break;
					}

					#ifdef BACDEL_PR14
					/* check for fault to normal state */
					if(((stPropVal.uValue.m_Real >= pstOutRange->m_fLow_Limit)
						&& (stPropVal.uValue.m_Real < (pstOutRange->m_fHigh_Limit - pstOutRange->m_fDeadband))) 
						&& ((stPropVal.uValue.m_Real <= pstOutRange->m_fHigh_Limit) 
						&& (stPropVal.uValue.m_Real > (pstOutRange->m_fLow_Limit + pstOutRange->m_fDeadband)))
						&& EVENT_STATE_FAULT == eEventState)
					{
						/* reset timer if previous value is not normal */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}
					/* check for fault to offnormal state - low limit */
					else if(stPropVal.uValue.m_Real < pstOutRange->m_fLow_Limit &&
						EVENT_STATE_FAULT == eEventState)
					{
						/* reset timer for double notification & change event state to Normal
						   for TimeDelayNormal interval */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}
					/* check for fault to offnormal state - high limit */
					else if(stPropVal.uValue.m_Real > pstOutRange->m_fHigh_Limit && 
						EVENT_STATE_FAULT == eEventState)
					{
						/* reset timer for double notification & change event state to Normal
						   for TimeDelayNormal interval */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}
					else
					#endif
					/* check for to_offnormal state - low limit */
					if(stPropVal.uValue.m_Real < pstOutRange->m_fLow_Limit &&
						eEventState != EVENT_STATE_LOW_LIMIT)
					{
						/* reset timer if previous value is not low limit */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_LOW_LIMIT)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_LOW_LIMIT;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}
					/* check for to_offnormal state - high limit */
					else if(stPropVal.uValue.m_Real > pstOutRange->m_fHigh_Limit && 
						eEventState != EVENT_STATE_HIGH_LIMIT)
					{
						/* reset timer if previous value is not high limit */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_HIGH_LIMIT)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_HIGH_LIMIT;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}	
					/* check for to_normal state */
					else if((stPropVal.uValue.m_Real <= pstOutRange->m_fHigh_Limit)
						&& (stPropVal.uValue.m_Real > (pstOutRange->m_fLow_Limit + pstOutRange->m_fDeadband)) 
						&& eEventState == EVENT_STATE_LOW_LIMIT)
					{
						/* reset timer if previous value is not normal */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}
					/* check for to_normal state */
					else if((stPropVal.uValue.m_Real < (pstOutRange->m_fHigh_Limit - pstOutRange->m_fDeadband)) 
						&& (stPropVal.uValue.m_Real >= pstOutRange->m_fLow_Limit) 
						&& eEventState == EVENT_STATE_HIGH_LIMIT)
					{
						/* reset timer if previous value is not normal */
						if(pstLocalNode->m_stNodeData.m_eEventStateTo != EVENT_STATE_NORMAL)
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
						pstLocalNode->m_stNodeData.m_eEventStateTo = EVENT_STATE_NORMAL;
						pstLocalNode->m_stNodeData.m_bStartTimer = TRUE;
					}
					else
					{
						pstLocalNode->m_stNodeData.m_bStartTimer = FALSE;
						pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
					}

					#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
					/* check double notification, use time dlay normal for ToNormal state */
					if(IsDoubleNotification(pstLocalNode->m_stNodeData.m_eObjectType,
						pstLocalNode->m_stNodeData.m_eEventStateFrom,
						pstLocalNode->m_stNodeData.m_eEventStateTo))
					{
						/* get time delay value */
						u32TimeDelay = pstEE->m_stTimeDelayNormal.m_u32Val;
					}
					else 
					#endif
					{
						/* get time delay value */
						#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
						u32TimeDelay = ((EVENT_STATE_NORMAL == pstLocalNode->m_stNodeData.m_eEventStateTo)
							? pstEE->m_stTimeDelayNormal.m_u32Val : pstOutRange->m_u32Timedelay);
						#else
						u32TimeDelay = pstOutRange->m_u32Timedelay;
						#endif
					}
				}break;

				default:
                    /* increment the retry count for this node */
                    pstLocalNode->m_stNodeData.m_u8RetryCount++;
                    bCompare = FALSE;

					#if(defined DEBUG_PRINTF && DL_2)
					Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
					AlgorithmicReportingTimer: unsupported event type value. \r\n");
					#endif
					break;
				}//switch ends

				if(pstLocalNode->m_stNodeData.m_bStartTimer)
				{
					/* increment timer & count till time delay */
					pstLocalNode->m_stNodeData.m_u32TimeDelay++;
					if(pstLocalNode->m_stNodeData.m_u32TimeDelay == u32TimeDelay ||
						BACNET_ZERO == u32TimeDelay)
					{
						void *pvDummy = NULL;
						uint32_t u32PropertyAdd = 0;
						/* assign property value pointer */
						pvDummy = &stPropVal.uValue;

                        /* check if double notification */
                        if(IsDoubleNotification(pstLocalNode->m_stNodeData.m_eObjectType,
                            pstLocalNode->m_stNodeData.m_eEventStateFrom,
                            pstLocalNode->m_stNodeData.m_eEventStateTo))
						{
							/* backup to state */
							pstLocalNode->m_stNodeData.m_eBackupToState = 
                                pstLocalNode->m_stNodeData.m_eEventStateTo;
							/* change to state to normal */
							pstLocalNode->m_stNodeData.m_eEventStateTo = 
								EVENT_STATE_NORMAL;
							/* set bDoubleNotification */
							bDoubleNotification = TRUE;
						}

						/* call update common property to update event state
							of the event enrollment object */
						UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, 
							(void *)pstLocalNode->m_stNodeData.m_pvEEObjAddr, 
							PROP_EVENT_STATE, 
							BACNET_ARRAY_ALL, 
							(void *)&pstLocalNode->m_stNodeData.m_eEventStateTo,
							READ_ONLY,
							&u32PropertyAdd, &pvDummy, pVirtualDev,
							false, false, false);

                        if(bDoubleNotification)
						{
							/* reset bDoubleNotification */
							bDoubleNotification = FALSE;
							/* reset time delay for 2nd notification */
							pstLocalNode->m_stNodeData.m_u32TimeDelay = 0;
							/* update from & to state */
							pstLocalNode->m_stNodeData.m_eEventStateFrom = 
								EVENT_STATE_NORMAL;
							pstLocalNode->m_stNodeData.m_eEventStateTo = 
                                pstLocalNode->m_stNodeData.m_eBackupToState;
						}
					}
				}
			}//if(bCompare) ends
			else
			{
				if(!i8FaultFlag)
				{
					;
				   #if(defined DEBUG_PRINTF && DL_2)
				    Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
				    AlgorithmicReportingTimer: error getting reference property value. \r\n");
				    #endif
				}
			}

			/* Wait till the mutex is released */
			//Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 
			/* maintain previous node */
			pstPrevNode = pstLocalNode;
			/* move to next node in link list */
			pstLocalNode = pstLocalNode->m_pstNext;
			/* release the mutex */
			//Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);
			/* reset bCompare flag */
			bCompare = FALSE;
		}//if(bObjEnable) ends
		else //if(!bObjEnable)
		{
			/* this event enrollment object is disabled, so delete from linklist */
			/* Wait till the mutex is released */
			//Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 
			/* previous node will remain the same */
			/* get the next node */
			pstNextNode = pstLocalNode->m_pstNext;
			/* remove obj from list */
			Delete_Node(pstLocalNode, pstPrevNode);
			/* restore local node */
			pstLocalNode = pstNextNode;
			/* release the mutex */
			//Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);
            /* reset bCompare flag */
			bCompare = FALSE;
		}
	}//end of while(NULL != pstLocalNode)

	/* release the mutex */
	Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);
#endif

	/* reset the flag */
	bAlgorithmicTimerFlag = false;

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AlgorithmicReportingTimer: exit \r\n");
	#endif

#ifdef __linux
    return NULL;
#endif
}

/**	api to clear all pending algorithmic notification belonging to any device,
    whenever that device is deleted. */
void Clear_Algorithmic_Notifications(uint32_t u32DevId)
{
    /* local variables */
	AlgorithmicListNode_t *pstLocalNode = NULL;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
    Clear_Algorithmic_Notifications: entry \r\n");
	#endif

    /* Wait till the mutex is released */
    Osal_Wait_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic, INFINITE); 

    /* get 1st node in linklist */
    pstLocalNode = g_stAlgorithmicReporting.pstFirstElem;

    /* traverse & update the linklist */
    while(NULL != pstLocalNode)
    {
        /* if device matches, then update its retry count to max to 
            delete this node */
        if(u32DevId == pstLocalNode->m_stNodeData.m_u32DevId)
        {
            pstLocalNode->m_stNodeData.m_u8RetryCount = 
                MAX_ALGORITHMIC_RETRY_COUNT;
        }
        /* move to next node */
        pstLocalNode = pstLocalNode->m_pstNext;
    }

    /* release the mutex */
    Osal_Release_Mutex(g_stAlgorithmicReporting.hMtxLockAlgorithmic);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
    Clear_Algorithmic_Notifications: exit \r\n");
	#endif

    return;
}

/**
*
* DESCRIPTION
* This function is used to get present value of reference object
*
* @param  pvObjectAdd	[in]   Object base address
* @param  eObjType		[in]   Reference object type
* @param  ePropertId	[in]   Reference property id
* @param  pstPresentVal	[in]   Pointer to return present value
*
* @return bool	[out]  Success or error code.
*
*/
bool AlgorithmicAlarm_GetPropertyValue_FromRefObject(
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID ePropertId,
	void *pvObjectAdd, 
	AnyValue_t *pstPresentVal)
{
	/* local variable */
	bool bReturnVal = false;
	int32_t i32Len = 0;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	void *pvMonitorVal = NULL;
	PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_GetPropertyValue_FromRefObject: Entry \r\n");
	#endif

	/* validate input pointer */
	if(NULL == pvObjectAdd || NULL == pstPresentVal)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_GetPropertyValue_FromRefObject:"
		"Invalid input pointer\r\n");
		#endif
		return bReturnVal;
	}

	/* convert base address */
	ul32BaseAddr = (ulong32_t)pvObjectAdd;

	/* get reference property access type and offset address */
	ePermission = GetAsignPropAccess(eObjType, ePropertId, pvObjectAdd, 
	BACNET_DEFAULT, false, &ul32OffsetAddr);
	/* get the property value */
	pvMonitorVal = (void *)(ul32BaseAddr + ul32OffsetAddr);

	bReturnVal = AlgorithmicAlarm_Convert_ReferencePropertyValue(
		pvMonitorVal, eObjType, ePropertId, pstPresentVal);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_GetPropertyValue_FromRefObject: Exit \r\n");
	#endif
	return bReturnVal;
}

/**
*
* DESCRIPTION
* This function is used to add event entrollment object ID entry in 
* reference object structure which is used for internal algorithmic reporting
*
* @param  pvObjAdd			[in]   Object base address
* @param  pvVirtualDev		[in]   Virtual device base address
* @param  pstDevObjPropReff	[in]   Device object property reference property pointer 

* @return void		[out]  No return vakue.
*
*/
void AlgorithmicAlarm_AddEEObjectIdNode_InRefObject(
	void *pvObjAdd, 
	void *pvVirtualDev,
	BACnetDevObjPropRef_t *pstDevObjPropReff)
{
	/* local variables */
	void *pvRefObj = NULL;
	void *PvMonitoredVal = NULL;
	bool bAddNodeInList = false;
	uint32_t u32EEObjId = 0;
	ulong32_t ul32OffsetAddr = 0;
	ulong32_t ul32EEObjBaseAddr = 0;
	ulong32_t ul32RefObjBaseAddr = 0;
	AnyValue_t stMonitoredValue = {0};
	ListofEEObjPropRef_t *pstNewNode = NULL;
	ListofEEObjPropRef_t *pstEEObject = NULL;
	Pr_ListofEEObjPropRef_t *pstEEObjlist = NULL;
	BACNET_OBJECT_TYPE eRefObjType = MAX_BACNET_OBJECT_TYPE;
	BACNET_PROPERTY_ID eRefPropertId = MAX_PROP_SUPPORTED;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_AddEEObjectIdNode_InRefObject: Entry \r\n");
	#endif

	/* validate input pointer */
	if(NULL == pvObjAdd || NULL == pstDevObjPropReff)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_AddEEObjectIdNode_InRefObject:"
		"Invalid input pointer\r\n");
		#endif
		return;
	}

	/* find the reference object */
	pvRefObj = Find_Object(pstDevObjPropReff->m_eObjectType,
		pstDevObjPropReff->m_u32ObjId, (virtualDevData_t *)pvVirtualDev);
	/* validate pointer value */
	if(NULL == pvRefObj)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_NOT_PRESENT, 
		"APDU: AlgorithmicAlarm_AddEEObjectIdNode_InRefObject:"
		"Null input pointer\r\n");
		#endif
		return;
	}

	/* get the event enrollement object and reference object base address */
	ul32EEObjBaseAddr = (ulong32_t)pvObjAdd;
	ul32RefObjBaseAddr = (ulong32_t)pvRefObj;

	/* get reference object type and property id */
	eRefObjType = pstDevObjPropReff->m_eObjectType;
	eRefPropertId = pstDevObjPropReff->m_ePropertyIdentifier;

	/* check property is present value, currently only present value support is added  */
	if(PROP_PRESENT_VALUE != eRefPropertId)
	{
		/* */
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, 
		"APDU: AlgorithmicAlarm_AddEEObjectIdNode_InRefObject:"
		"Only present value support is added\r\n");
		#endif
		return;
	}

	/* get the base address of a list of EE object instances */ 
	GET_ALLOBJ_OFFSET(eRefObjType, m_stEEObjectlist, ul32OffsetAddr);
	pstEEObjlist = (Pr_ListofEEObjPropRef_t *)(ul32RefObjBaseAddr + ul32OffsetAddr);

	/* get offset address & copy data */
	GET_OBJCOMMON_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stObjectID.m_u32ObjId, 
		ul32OffsetAddr);
	memcpy(&u32EEObjId, (void *)(ul32EEObjBaseAddr + ul32OffsetAddr), 
		sizeof(uint32_t));

	/* check reference object algorithmic reporting status */
	if(false == pstEEObjlist->m_bIsAlgoReporting && NULL == pstEEObjlist->m_pstEEObjectList
		&& NULL == pstEEObjlist->m_pstLastNode)
	{
		/* add node in event entrollment object list of reference object */
		bAddNodeInList = true;
	}
	else
	{
		/* get event entrollment object list */
		pstEEObject = pstEEObjlist->m_pstEEObjectList;
		/* check node by node */
		while(NULL != pstEEObject)
		{
			/* check object instance and property id */
			if(pstEEObject->m_u32EEObjId == u32EEObjId && 
				pstEEObject->m_ePropertyId == eRefPropertId)
			{
				/* match found, no need to add node in list */
				return;
			}
			/* move to next node */
			pstEEObject = pstEEObject->m_pstNext;
		}

		/* match not found in event entrollment object list of reference 
		object, so add node in list */
		bAddNodeInList = true;
	}

	/* add node in event entrollment object list */
	if(bAddNodeInList)
	{
		/* allocate menory */
		pstNewNode = (ListofEEObjPropRef_t *)OSAL_Malloc(sizeof(ListofEEObjPropRef_t), 
			__FILE__, __FUNCTION__, __LINE__);
		/* validate pointer */
		if(NULL == pstNewNode)
		{
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, 
			"APDU: AlgorithmicAlarm_AddEEObjectIdNode_InRefObject:"
			"Invalid input pointer\r\n");
			#endif
			return;
		}

		/* save data */
		pstNewNode->m_u32EEObjId = u32EEObjId;
		pstNewNode->m_ePropertyId = eRefPropertId;

		/* check list is empty or not */
		if(NULL == pstEEObjlist->m_pstEEObjectList && 
			NULL == pstEEObjlist->m_pstLastNode)
		{
			/* add node in start */
			pstEEObjlist->m_pstEEObjectList = pstNewNode;
			/* set staus flag value */
			pstEEObjlist->m_bIsAlgoReporting = true;
		}
		else
		{
			/* add node in end */
			pstEEObjlist->m_pstLastNode->m_pstNext = pstNewNode;
		}

		/* update last node value */
		pstEEObjlist->m_pstLastNode = pstNewNode;

		/* get reference object present value */
		if(AlgorithmicAlarm_GetPropertyValue_FromRefObject(eRefObjType, eRefPropertId, 
			pvRefObj, &stMonitoredValue))
		{
			#if STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS
			/* save present value in event entrollment object as per extrernal device flag */
			if(pstDevObjPropReff->m_bDeviceIdPresent)
			{
				/* get property offset of external property value */
				GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stExternalValue, ul32OffsetAddr);
			}
			else
			#else
			{
				/* get property offset of internal property value */
				GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stInternalValue, ul32OffsetAddr);
			}
			#endif

			/* copy property value */
			memcpy((void *)(ul32EEObjBaseAddr + ul32OffsetAddr), &stMonitoredValue,
				sizeof(AnyValue_t));
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_AddEEObjectIdNode_InRefObject: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function detete event entrollment object id node from reference object
*
* @param  pstEEObjlist			[in]  Pointer of EE object id list from reference object
* @param  pstDeleteNode			[in]  Pointer of EE object id node to be deleted
* @param  pstPrevEEObjectNode	[in]  Pointer of previous EE object id node which need to be deleted
*
* @return void			[out] Void/nothing.
*
**/
static void AlgorithmicAlarm_CheckAndDeleteNode_FromRefObject(
	Pr_ListofEEObjPropRef_t *pstEEObjlist,
	ListofEEObjPropRef_t *pstDeleteNode,
	ListofEEObjPropRef_t *pstPrevEEObjectNode)
{
	/* local variable */
	ListofEEObjPropRef_t *pstDummyNode = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_CheckAndDeleteNode_FromRefObject: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstEEObjlist || NULL == pstDeleteNode)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_CheckAndDeleteNode_FromRefObject: Null input pointer \r\n");
		#endif
		return;
	}

	/* check if list contain only one node & delete this node */
	if(pstEEObjlist->m_pstEEObjectList == pstEEObjlist->m_pstLastNode
		&& pstDeleteNode == pstEEObjlist->m_pstEEObjectList)
	{
		OSAL_Free(pstEEObjlist->m_pstEEObjectList, __FILE__, __FUNCTION__, __LINE__);
		pstEEObjlist->m_pstEEObjectList = NULL;
		pstEEObjlist->m_pstLastNode = NULL;
		/* reset the algorithmic status flag */
		pstEEObjlist->m_bIsAlgoReporting = false;
	}
	/* delete first node of event entrollmet object id list */
	else if(pstDeleteNode == pstEEObjlist->m_pstEEObjectList)
	{		
		pstDummyNode = pstEEObjlist->m_pstEEObjectList->m_pstNext;
		OSAL_Free(pstEEObjlist->m_pstEEObjectList, __FILE__, __FUNCTION__, __LINE__);
		pstEEObjlist->m_pstEEObjectList = pstDummyNode;
	}
	/* delete last node of event entrollmet object id list */
	else if(pstDeleteNode == pstEEObjlist->m_pstLastNode)
	{
		/* validate previous node pointer */
		if(NULL != pstPrevEEObjectNode)
		{
			pstPrevEEObjectNode->m_pstNext = NULL;
			OSAL_Free(pstEEObjlist->m_pstLastNode, __FILE__, __FUNCTION__, __LINE__);
			pstEEObjlist->m_pstLastNode = pstPrevEEObjectNode;
			pstPrevEEObjectNode = NULL;
		}
	}
	/* delete middle node of event entrollmet object id list */
	else if(pstDeleteNode != pstEEObjlist->m_pstEEObjectList
		&& pstDeleteNode != pstEEObjlist->m_pstLastNode)
	{
		/* validate previous node pointer */
		if(NULL != pstPrevEEObjectNode)
		{
			pstPrevEEObjectNode->m_pstNext = pstDeleteNode->m_pstNext;
			OSAL_Free(pstDeleteNode, __FILE__, __FUNCTION__, __LINE__);
			if (pstPrevEEObjectNode->m_pstNext == NULL)
			{
				pstEEObjlist->m_pstLastNode = pstPrevEEObjectNode;
			}
		}
	}
	/* handle error condition */
	else
	{
		;	// error condition
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_CheckAndDeleteNode_FromRefObject: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function is used to remove event entrollment object ID entry from 
* reference object structure.
*
* @param  pvObjAdd			[in]   Object base address
* @param  pvVirtualDev		[in]   Virtual device base address
* @param  pstDevObjPropReff	[in]   Device object property reference property pointer 

* @return void		[out]  No return vakue.
*
*/
void AlgorithmicAlarm_RemoveEEObjectIdNode_FromRefObject(
	void *pvObjAdd,
	void *pvVirtualDev,
	BACnetDevObjPropRef_t *pstDevObjPropReff)
{
	/* local variables */
	void *pvRefObj = NULL;
	bool bAddNodeInList = false;
	uint32_t u32EEObjId = 0;
	ulong32_t ul32OffsetAddr = 0;
	ulong32_t ul32EEObjBaseAddr = 0;
	ulong32_t ul32RefObjBaseAddr = 0;
	Pr_ListofEEObjPropRef_t *pstEEObjlist = NULL;
	ListofEEObjPropRef_t *pstEEObjectNode = NULL;
	ListofEEObjPropRef_t *pstPrevEEObjectNode = NULL;
	BACNET_OBJECT_TYPE eRefObjType = MAX_BACNET_OBJECT_TYPE;
	BACNET_PROPERTY_ID eRefPropertId = MAX_PROP_SUPPORTED;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_RemoveEEObjectIdNode_FromRefObject: Entry \r\n");
	#endif

	/* validate input pointer */
	if(NULL == pvObjAdd || NULL == pstDevObjPropReff)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_RemoveEEObjectIdNode_FromRefObject:"
		"Invalid input pointer\r\n");
		#endif
		return;
	}

	/* get reference object base address */
	pvRefObj = Find_Object(pstDevObjPropReff->m_eObjectType,
		pstDevObjPropReff->m_u32ObjId, (virtualDevData_t *)pvVirtualDev);
	/* validate pointer value */
	if(NULL == pvRefObj)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_NOT_PRESENT, 
		"APDU: AlgorithmicAlarm_RemoveEEObjectIdNode_FromRefObject:"
		"Null input pointer\r\n");
		#endif
		return;
	}

	/* get the event enrollement object and reference object base address */
	ul32EEObjBaseAddr = (ulong32_t)pvObjAdd;
	ul32RefObjBaseAddr = (ulong32_t)pvRefObj;

	/* get reference object type and property id */
	eRefObjType = pstDevObjPropReff->m_eObjectType;
	eRefPropertId = pstDevObjPropReff->m_ePropertyIdentifier;

	/* get the base address of a list of EE object instances */ 
	GET_ALLOBJ_OFFSET(eRefObjType, m_stEEObjectlist, ul32OffsetAddr);
	pstEEObjlist = (Pr_ListofEEObjPropRef_t *)(ul32RefObjBaseAddr + ul32OffsetAddr);

	/* get offset address & copy data */
	GET_OBJCOMMON_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stObjectID.m_u32ObjId, 
		ul32OffsetAddr);
	memcpy(&u32EEObjId, (void *)(ul32EEObjBaseAddr + ul32OffsetAddr), 
		sizeof(uint32_t));

	/* check flag status */
	if(pstEEObjlist->m_bIsAlgoReporting)
	{
		/* get event entrollment object list */
		pstEEObjectNode = pstEEObjlist->m_pstEEObjectList;
		/* check node by node */
		while(NULL != pstEEObjectNode)
		{
			/* check object instance and property id */
			if(u32EEObjId == pstEEObjectNode->m_u32EEObjId && 
				eRefPropertId == pstEEObjectNode->m_ePropertyId)
			{
				/* match found, delete this node */
				break;
			}

			/* save node for previous references */
			pstPrevEEObjectNode = pstEEObjectNode;
			/* move to next node */
			pstEEObjectNode = pstEEObjectNode->m_pstNext;
		}
	}

	/* validate pointer value */
	if(NULL != pstEEObjectNode)
	{
		/* remove this node of event entrollment object from reference object */
		AlgorithmicAlarm_CheckAndDeleteNode_FromRefObject(
			pstEEObjlist, pstEEObjectNode, pstPrevEEObjectNode);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_RemoveEEObjectIdNode_FromRefObject: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function is used to check internal algorithmic reporting is supported by 
* object and property 
*
* @param  pvMonitorVal	[in]   Property value 
* @param  pvVirtualDev	[in]   Virtual device pointer
* @param  eObjType		[in]   Object type
* @param  eObjProp		[in]   Object property
*
* @return void	[out]  No return value.
*
*/
void AlgorithmicAlarm_CheckForEventStateChange(
	void *pvMonitorVal,
	void *pvObjAddr,
	void *pvVirtualDev,
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID eObjProp)
{
	/* local varibles */
	void *pvEEObjectBaseAdd = NULL;
	bool bIsInternal = false;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	AnyValue_t stMonitoredValue= {0};
	virtualDevData_t *pstVirtualDev = NULL;
	ListofEEObjPropRef_t *pstEEObject = NULL;
	Pr_ListofEEObjPropRef_t *pstEEObjectList = NULL;
	BACNET_RELIABILITY eReliabilty = MAX_RELIABILITY;
	Pr_BACnetDevObjPropRef_t *pstObjectPropertyReference = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_CheckForEventStateChange: Entry \r\n");
	#endif

	/* null check for pointers */
	if(NULL == pvObjAddr ||  NULL == pvVirtualDev)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_CheckForEventStateChange: Invalid input pointer\r\n");
		#endif
		return;
	}
	
	/* get address of virtual device */
	pstVirtualDev = (virtualDevData_t *)pvVirtualDev;

	 /* convert object base address */
     ul32BaseAddr = (ulong32_t )pvObjAddr;

	/* based on object type */
    switch(eObjType)
    {
		#ifdef BACDEL_OBJ_AI
        case OBJECT_ANALOG_INPUT:
		#endif
		#ifdef BACDEL_OBJ_AO
        case OBJECT_ANALOG_OUTPUT:
		#endif
		#ifdef BACDEL_OBJ_AV
        case OBJECT_ANALOG_VALUE:
		#endif
		#ifdef BACDEL_OBJ_BI
        case OBJECT_BINARY_INPUT:
		#endif
		#ifdef BACDEL_OBJ_BO
        case OBJECT_BINARY_OUTPUT:
		#endif
		#ifdef BACDEL_OBJ_BV
        case OBJECT_BINARY_VALUE:
		#endif
		#ifdef BACDEL_OBJ_MSI
        case OBJECT_MULTI_STATE_INPUT:
		#endif
		#ifdef BACDEL_OBJ_MSO
        case OBJECT_MULTI_STATE_OUTPUT:
		#endif
		#ifdef BACDEL_OBJ_MSV
        case OBJECT_MULTI_STATE_VALUE:
		#endif
		#ifdef BACDEL_OBJ_DEV
        case OBJECT_DEVICE:
		#endif
		#ifdef BACDEL_OBJ_NC
        case OBJECT_NOTIFICATION_CLASS:
		#endif
		#ifdef BACDEL_OBJ_CAL
		case OBJECT_CALENDAR:
		#endif
		#ifdef BACDEL_OBJ_SDL
		case OBJECT_SCHEDULE:
		#endif
		{
			/* local variable */
			void *pvDummy = NULL;
			uint32_t u32PropertyAdd = 0;
			BACNET_EVENT_TYPE eEventType = MAX_EVENT_TYPE;
			Pr_BACnetEventParameter_t *pstEventParam = NULL;

			/* get property offset and copy data */
			GET_ALLOBJ_OFFSET(eObjType, m_stEEObjectlist, ul32OffsetAddr); 
			pstEEObjectList = (Pr_ListofEEObjPropRef_t *)(ul32BaseAddr + ul32OffsetAddr);
			
			/* check flag value */
			if(pstEEObjectList->m_bIsAlgoReporting)
			{
				/* get list of event entrollment object id */
				pstEEObject = pstEEObjectList->m_pstEEObjectList;

				/* check node one by one */
				while(NULL != pstEEObject)
				{
					/* find the event entrollment object */
					pvEEObjectBaseAdd = Find_Object(OBJECT_EVENT_ENROLLMENT, pstEEObject->m_u32EEObjId, 
						pstVirtualDev);
					/* validate null pointer */
					if(NULL == pvEEObjectBaseAdd)
					{
						/* move to next node */
						pstEEObject = pstEEObject->m_pstNext;
						continue;
					}

					/* convert address */
					ul32BaseAddr = (ulong32_t)pvEEObjectBaseAdd;

					/* get event type value */
					eEventType = ((EventEnrollment_t *)pvEEObjectBaseAdd)->m_stEventype.m_eEventType;
					/* get property value */
					pstEventParam = &((EventEnrollment_t *)pvEEObjectBaseAdd)->m_stEventParameter;

					#ifdef BACDEL_PR14
					/* get property value */
					eReliabilty = ((EventEnrollment_t *)pvEEObjectBaseAdd)->m_stReliability.m_eReliabilty;
					/* check reliability */
					if(RELIABILITY_CONFIGURATION_ERROR != eReliabilty)
					#endif
					{
						/* check for match with monitored property */
						if(eObjProp == pstEEObject->m_ePropertyId)
						{
							/* convert monitor value as per tag type */
							if(AlgorithmicAlarm_Convert_MonitoredValue(pvMonitorVal, eObjType, eObjProp, 
								&stMonitoredValue));
							{
								/* get property offset & copy value */
								pstObjectPropertyReference = &((EventEnrollment_t *)pvEEObjectBaseAdd)->
									m_stObjectPropertyReference;

								/* validate data and save in algorithmic reporting list  */
								Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange(
									&stMonitoredValue, NULL, pvVirtualDev, OBJECT_EVENT_ENROLLMENT, 
									pstEEObject->m_u32EEObjId, 
									pstObjectPropertyReference->m_stDevObjPropReff.m_bDeviceIdPresent);
							}
						}
						/* check for match with feedback property */
						else if(EVENT_COMMAND_FAILURE == eEventType &&
							eObjProp == pstEventParam->m_stEventParam.BACnetEventParameter_u.m_stCmdFail.
							m_stFeedbackPropertyReference.m_ePropertyIdentifier &&
							eObjType == pstEventParam->m_stEventParam.BACnetEventParameter_u.m_stCmdFail.
							m_stFeedbackPropertyReference.m_eObjectType)
						{
							/* convert monitor value as per tag type */
							if(AlgorithmicAlarm_Convert_MonitoredValue(pvMonitorVal, eObjType, eObjProp, 
								&stMonitoredValue));
							{
								/* call update common property to update external feedback value */ 
								UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvEEObjectBaseAdd, 
									PROP_BACDEL_PROPRIETARY_EXTERNAL_FEEDBACK_VALUE, -1, &stMonitoredValue.m_stValue, 
									READ_WRITE, &u32PropertyAdd, &pvDummy, pvVirtualDev, false, false, false);
							}
						}
					}

					/* move to next node */
					pstEEObject = pstEEObject->m_pstNext;							
				}
			}
		}
        break;

		#ifdef BACDEL_OBJ_EE
		case OBJECT_EVENT_ENROLLMENT:
		#endif
		{	
			/* local variable */
			uint32_t u32RefObjId = 0;
			uint32_t u32EEObjId = 0;
			void *pvRefObjAddr = NULL;
			ulong32_t ul32RefObjBaseAddr = 0;
			ulong32_t ul32RefPropOffstAddr = 0;
			PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
			BACNET_PROPERTY_ID eRefPropId = MAX_PROP_SUPPORTED;
			BACNET_OBJECT_TYPE eRefObjType = MAX_ASHRAE_OBJECT_TYPE;
			bool bEventAlgoInhibitBackup = false;

			#ifdef BACDEL_PR14
			/* get reliability property offset and value */
			GET_RELIABILITY_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stReliability.m_eReliabilty, ul32OffsetAddr);
			memcpy(&eReliabilty, (void*)(ul32BaseAddr + ul32OffsetAddr), sizeof(BACNET_RELIABILITY));
			#endif
						
			/* check reliability & supported properties */
			if(
				#ifdef BACDEL_PR14
				RELIABILITY_CONFIGURATION_ERROR != eReliabilty && 
				#endif
				(PROP_EVENT_TYPE == eObjProp || 
				PROP_OBJECT_PROPERTY_REFERENCE == eObjProp || PROP_EVENT_ENABLE == eObjProp
				#ifdef BACDEL_PR14
				|| PROP_EVENT_DETECTION_ENABLE == eObjProp || PROP_EVENT_ALGORITHM_INHIBIT == eObjProp
				|| PROP_RELIABILITY_EVALUATION_INHIBIT == eObjProp
				#endif
				))
			{
				/* get offset address & copy data */
				GET_OBJCOMMON_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stObjectID.m_u32ObjId, ul32OffsetAddr);
				memcpy(&u32EEObjId, (void *)(ul32BaseAddr + ul32OffsetAddr), sizeof(uint32_t));

				/* get property offset & copy value */
				GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stObjectPropertyReference, ul32OffsetAddr);
				pstObjectPropertyReference = (Pr_BACnetDevObjPropRef_t *)(ul32BaseAddr + ul32OffsetAddr);

				/* check external device flag */
				bIsInternal = pstObjectPropertyReference->m_stDevObjPropReff.m_bDeviceIdPresent;
				/* get reference object type */
				eRefObjType = pstObjectPropertyReference->m_stDevObjPropReff.m_eObjectType;
				/* get reference object id */
				u32RefObjId = pstObjectPropertyReference->m_stDevObjPropReff.m_u32ObjId;
				/* get the reference property id */
				eRefPropId = pstObjectPropertyReference->m_stDevObjPropReff.m_ePropertyIdentifier;

				/* get the reference object base address */
				pvRefObjAddr = Find_Object(eRefObjType, u32RefObjId, pstVirtualDev);
				/* validate pointer value */
				if(NULL == pvRefObjAddr)
				{
					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_NOT_PRESENT, 
					"APDU: AlgorithmicAlarm_CheckForEventStateChange: Null pointer value\r\n");
					#endif
					break;
				}

				/* convert refernce object base address */
				ul32RefObjBaseAddr = (ulong32_t)pvRefObjAddr;

				/* get property offset and copy data */
				GET_ALLOBJ_OFFSET(eRefObjType, m_stEEObjectlist, ul32OffsetAddr); 
				pstEEObjectList = (Pr_ListofEEObjPropRef_t *)(ul32RefObjBaseAddr + ul32OffsetAddr);

				/* get reference property access type and offset address */
				ePermission = GetAsignPropAccess(eRefObjType, eRefPropId, pvRefObjAddr, 
				BACNET_DEFAULT, false, &ul32RefPropOffstAddr);
				/* get the property value */
				pvMonitorVal = (void *)(ul32RefObjBaseAddr + ul32RefPropOffstAddr);

				/* convert monitor value as per tag type */
				if(AlgorithmicAlarm_Convert_ReferencePropertyValue(pvMonitorVal, eRefObjType, eRefPropId, 
					&stMonitoredValue))
				{
					/* check algorithmic flag value */
					if(pstEEObjectList->m_bIsAlgoReporting)
					{
						/* get list of event entrollment object id */
						pstEEObject = pstEEObjectList->m_pstEEObjectList;

						/* check node one by one */
						while (NULL != pstEEObject)
						{
							/* validate property id and object id */
							if(eRefPropId == pstEEObject->m_ePropertyId &&
								u32EEObjId == pstEEObject->m_u32EEObjId)
							{
								/* add node in algorithmic reporting link list */
								Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange(&stMonitoredValue, 
									NULL, pvVirtualDev, OBJECT_EVENT_ENROLLMENT, pstEEObject->m_u32EEObjId,
									bIsInternal);
							}
							/* move to next node */
							pstEEObject = pstEEObject->m_pstNext;
						}
					}
				}
			}
		}
		break;

		default:
		{
			; //Nothing to do
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, 
			"APDU: AlgorithmicAlarm_CheckForEventStateChange: Object type error\r\n");
			#endif
		}
		break;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_CheckForEventStateChange: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function is used to convert property value from 'void' datatype to 
* 'AnyValue_t' datatype
*
* @param  pvData		[in]   Property value
* @param  eObjType		[in]   Object type
* @param  eObjProp		[in]   Object propertye
* @param  pvVirtualDev	[in]   Virtual device pointer
*
* @return bool	[out]  TRUE: indicate value converted successfully
*					   FALSE: indicate error in value conversion
*
*/
bool AlgorithmicAlarm_Convert_MonitoredValue(
	void *pvData,
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID ePropType, 
	AnyValue_t *pstMonitoredValue)
{
	/* local variable */
	bool bVal = true;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	AnyValue_t *pstInternalValue = NULL;
	BACNET_APPLICATION_TAG eApplicationTag = BACNET_APPLICATION_TAG_NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_Convert_MonitoredValue: Entry\r\n");
	#endif
	
	/* validate input pointer */
	if(NULL == pstMonitoredValue || NULL == pvData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_Convert_MonitoredValue: Invalid input pointer\r\n");
		#endif
		return false;
	}

	/* get tag value of respective property */ 
	eApplicationTag = GetPropertyTag(eObjectType, ePropType, BACNET_ARRAY_ALL, false);
	/* save tag value */
	pstMonitoredValue->m_stValue.m_TagType = eApplicationTag;

	/* save value as per tag number */
	switch(eApplicationTag)
	{
		case BACNET_APPLICATION_TAG_BOOLEAN:
		{
			/* save boolean value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Boolean, (bool *)pvData, 
				sizeof(bool));
		}
		break;

		case BACNET_APPLICATION_TAG_UNSIGNED_INT:
		{
			/* save unsigned value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Unsigned_Int, (uint32_t *)pvData, 
				sizeof(uint32_t));
		}
		break;

		case BACNET_APPLICATION_TAG_SIGNED_INT:
		{
			/* save signed value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Signed_Int, (int32_t *)pvData, 
				sizeof(int32_t));
		}
		break;

		case BACNET_APPLICATION_TAG_REAL:
		{
			/* save real value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Real, (Float_t *)pvData, 
				sizeof(Float_t));
		}
		break;

		case BACNET_APPLICATION_TAG_DOUBLE:
		{
			/* save double value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Double, (Double_t *)pvData, 
				sizeof(Double_t));
		}
		break;

		case BACNET_APPLICATION_TAG_OCTET_STRING: 
		{
			/* save octate string value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_stOctet_String, (BACnetOctetStr_t *)pvData, 
				sizeof(BACnetOctetStr_t));
		}
		break;

		case BACNET_APPLICATION_TAG_CHARACTER_STRING:
		{
			/* save character string value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_stCharacter_String, (BACnetCharStr_t *)pvData, 
				sizeof(BACnetCharStr_t));
		}
		break;

		case BACNET_APPLICATION_TAG_BIT_STRING:
		{
			/* save bit string value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_stBit_String, (BACnetBITStr_t *)pvData, 
				sizeof(BACnetBITStr_t)); 
		}
		break;

		case BACNET_APPLICATION_TAG_ENUMERATED:
		{
			/* save enumerated value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Enumerated, (uint32_t *)pvData, 
				sizeof(uint32_t));
		}
		break;

		default:
		{
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, 
			"APDU: AlgorithmicAlarm_Convert_MonitoredValue: Tag error\r\n");
			#endif
			bVal = false;
		}
		break;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_Convert_MonitoredValue: Exit\r\n");
	#endif
	return bVal;
}

/**
*
* DESCRIPTION
* This function is used to convert reference property value from 'void' datatype to 
* 'AnyValue_t' datatype
*
* @param  pvData		[in]   Property value
* @param  eObjType		[in]   Object type
* @param  eObjProp		[in]   Object propertye
* @param  pvVirtualDev	[in]   Virtual device pointer
*
* @return bool	[out]  TRUE: indicate value converted successfully
*					   FALSE: indicate error in value conversion
*
*/
bool AlgorithmicAlarm_Convert_ReferencePropertyValue(
	void *pvData,
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID ePropType, 
	AnyValue_t *pstMonitoredValue)
{
	/* local variable */
	bool bVal = true;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	AnyValue_t *pstInternalValue = NULL;
	BACNET_APPLICATION_TAG eApplicationTag = BACNET_APPLICATION_TAG_NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_Convert_ReferencePropertyValue: Entry\r\n");
	#endif
	
	/* validate input pointer */
	if(NULL == pstMonitoredValue || NULL == pvData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_Convert_ReferencePropertyValue: Invalid input pointer\r\n");
		#endif
		return false;
	}

	/* get tag value of respective property */ 
	eApplicationTag = GetPropertyTag(eObjectType, ePropType, BACNET_ARRAY_ALL, false);
	/* save tag value */
	pstMonitoredValue->m_stValue.m_TagType = eApplicationTag;

	/* save value as per tag number */
	switch(eApplicationTag)
	{
		case BACNET_APPLICATION_TAG_BOOLEAN:
		{
			/* save boolean value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Boolean, 
				&((Pr_BACnetBool_t *)pvData)->m_bVal, 
				sizeof(bool));
		}
		break;

		case BACNET_APPLICATION_TAG_UNSIGNED_INT:
		{
			/* save unsigned value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Unsigned_Int, 
				&((Pr_BACnetUnsigned32_t *)pvData)->m_u32Val, 
				sizeof(uint32_t));
		}
		break;

		case BACNET_APPLICATION_TAG_SIGNED_INT:
		{
			/* save signed value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Signed_Int, 
				&((Pr_BACnetSigned32_t *)pvData)->m_i32Val, 
				sizeof(int32_t));
		}
		break;

		case BACNET_APPLICATION_TAG_REAL:
		{
			/* save real value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Real, 
				&((Pr_BACnetReal_t *)pvData)->m_fVal, 
				sizeof(Float_t));
		}
		break;

		case BACNET_APPLICATION_TAG_DOUBLE:
		{
			/* save double value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Double, 
				&((Pr_BACnetDouble_t *)pvData)->m_dVal, 
				sizeof(Double_t));
		}
		break;

		case BACNET_APPLICATION_TAG_OCTET_STRING: 
		{
			/* save octate string value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_stOctet_String, 
				&((Pr_BACnetOctetStr_t *)pvData)->m_stOctetString, 
				sizeof(BACnetOctetStr_t));
		}
		break;

		case BACNET_APPLICATION_TAG_CHARACTER_STRING:
		{
			/* save character string value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_stCharacter_String, 
				&((Pr_BACnetCharStr_t *)pvData)->m_stCHARString, 
				sizeof(BACnetCharStr_t));
		}
		break;

		case BACNET_APPLICATION_TAG_BIT_STRING:
		{
			/* save bit string value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_stBit_String, 
				&((Pr_BACnetBITStr_t *)pvData)->m_stBitString, 
				sizeof(BACnetBITStr_t)); 
		}
		break;

		case BACNET_APPLICATION_TAG_ENUMERATED:
		{
			/* save enumerated value */
			memcpy(&pstMonitoredValue->m_stValue.uValue.m_Enumerated, 
				&((Pr_BACnetEnum_t *)pvData)->m_eVal, 
				sizeof(enum_t));
		}
		break;

		default:
		{
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, 
			"APDU: AlgorithmicAlarm_Convert_ReferencePropertyValue: Tag error\r\n");
			#endif
			bVal = false;
		}
		break;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_Convert_ReferencePropertyValue: Exit\r\n");
	#endif
	return bVal;
}

/**
*
* DESCRIPTION
* This function is used to update external feedback value when event parameter is updated 
* as command-failure event type
*
* @param  pvObject		[in]   Object base address
* @param  pvVirtualDev	[in]   Virtual device pointer
*
* @return void	[out]  No return value.
*
*/
void AlgorithmicAlarm_Update_ExternalFeedbackValue(
	void *pvObject,
	void *pvVirtualDev)
{
	/* local variable */
	void *pvRefObj = NULL;
	void *pvDummy = NULL;
	uint32_t u32PropertyAdd = 0;
	AnyValue_t stMonitoredValue = {0};

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: AlgorithmicAlarm_Update_ExternalFeedbackValue: Exit\r\n");
	#endif

	/* validate input pointer */
	if(NULL == pvObject || NULL == pvVirtualDev)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: AlgorithmicAlarm_Update_ExternalFeedbackValue: Invalid input pointer\r\n");
		#endif
		return;
	}

	/* find the reference object */
	pvRefObj = Find_Object(((EventEnrollment_t *)pvObject)->m_stEventParameter.m_stEventParam.
		BACnetEventParameter_u.m_stCmdFail.m_stFeedbackPropertyReference.m_eObjectType,
		((EventEnrollment_t *)pvObject)->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
		m_stCmdFail.m_stFeedbackPropertyReference.m_u32ObjId, 
		(virtualDevData_t *)pvVirtualDev);
	/* validate pointer value */
	if(NULL != pvRefObj)
	{
		/* get reference object present value */
		if(AlgorithmicAlarm_GetPropertyValue_FromRefObject(
			((EventEnrollment_t *)pvObject)->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
			m_stCmdFail.m_stFeedbackPropertyReference.m_eObjectType, 
			((EventEnrollment_t *)pvObject)->m_stEventParameter.m_stEventParam.BACnetEventParameter_u.
			m_stCmdFail.m_stFeedbackPropertyReference.m_ePropertyIdentifier,
			pvRefObj, &stMonitoredValue))
		{
			/* call update common property to update external feedback value */ 
			UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvObject, 
				PROP_BACDEL_PROPRIETARY_EXTERNAL_FEEDBACK_VALUE, -1, &stMonitoredValue.m_stValue, 
				READ_WRITE, &u32PropertyAdd, &pvDummy, pvVirtualDev, false, false, false);
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: AlgorithmicAlarm_Update_ExternalFeedbackValue: Exit\r\n");
	#endif
	return;
}
//#endif /* (ALGORITHMIC_REPORTING && BACDEL_SER_AE_EN_B && BACDEL_OBJ_EE && BACDEL_OBJ_NC) */

/**
*
* DESCRIPTION
* This function check if event state needs changes in EE object and
* accordingly adds data in algorithmic reporting link list.
*
* @param  pvObject		[in]  EE object base address
* @param  pvVirtualDev	[in]  Virtual device data
* @param  bIsInternal	[in]  Specifies internal or external alarms
* @param  bWriteNULL	[in]  Flag indicate that call is received fron timer
*
* @return void	[out]  No return value
*
*/
void Ext_AlgorithmicAlarm_CheckForEventStateChange(
	void *pvObject,
	void *pvVirtualDev,
	bool bIsInternal,
	bool bWriteNULL)
{
	/* local variables */
	bool bEventChange = false;
	bool bDoubleNotification = false;
	bool bNodeAlreadyAddedInList = false;
	uint32_t u32CopySize = 0;
	uint32_t u32PropertyAdd = 0;
	uint32_t u32TimeDelay = 0;
	uint32_t u32TimeDelayNormal = 0;
	BACNET_RETURN_TYPE eReturnVal = BACDEL_ERROR;
	BACNET_EVENT_STATE eEventStateTo = MAX_EVENT_STATE;
	BACNET_EVENT_STATE eEventStateFrom = MAX_EVENT_STATE;
	BACNET_EVENT_STATE eEventStateToBackup = MAX_EVENT_STATE;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	virtualDevData_t *pstVirtualDev = NULL;
	PropertyValue_u	uPropValue = {0};
	void *pvLocalData = NULL;
	void *pvDummy = NULL;
	PROP_ACCESS_TYPE eAccessType = IGNORE_ACCESS_TYPE;
	AnyValue_t * pstMonitorPropertyValue = NULL;
	BACnetDevObjPropRef_t  *pstObjectPropertyReference = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarms_CheckForEventStateChange: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pvObject || NULL == pvVirtualDev)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarms_CheckForEventStateChange: Null input pointer \r\n");
		#endif
		return;
	}

	/* get the virtual device pointer */
	pstVirtualDev = (virtualDevData_t *)pvVirtualDev;

	/* convert object base address */
	ul32BaseAddr = (ulong32_t)pvObject;

	/* check that device supporting external algorithmic reporting */
#if STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS
	if(bIsInternal)
	{
		/* get property offset of external property value */
		GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stExternalValue, ul32OffsetAddr);
	}
	/* device supporting internal algorithmic reporting */
	else
#endif
	{
		/* get property offset of internal property value */
		GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stInternalValue, ul32OffsetAddr);
	}

	/* get property value */
	pstMonitorPropertyValue = (AnyValue_t *)(ul32BaseAddr + ul32OffsetAddr);
	
	/* call function to check if event_state needs to be updated */
	eReturnVal = Ext_AlgorithmicAlarm_UpdateEventState(OBJECT_EVENT_ENROLLMENT, pvObject,
		pstMonitorPropertyValue, &bEventChange, &eEventStateTo, &eEventStateFrom, &u32TimeDelay, 
		pvVirtualDev, bIsInternal);

	/* add node in external algorithmic list */
	if(TRUE == bEventChange && BACDEL_SUCCESS == eReturnVal)
	{
		/* NOTE - bWriteNull is sent true to indicate that this 
		   is call from timer */
		if((eEventStateTo == EVENT_STATE_LOW_LIMIT ||
			eEventStateTo == EVENT_STATE_HIGH_LIMIT || 
			eEventStateTo == EVENT_STATE_OFFNORMAL) && bWriteNULL) 
		{
			/* call is from timer send two notifications */
			eEventStateToBackup = eEventStateTo;
			eEventStateTo = EVENT_STATE_NORMAL;
			bDoubleNotification = TRUE;
		}
		/* reliability != RELIABILITY_NO_FAULT_DETECTED, ES = fault, 
		   PV = greater than high limit value, now reliability changed 
		   to RELIABILITY_NO_FAULT_DETECTED, double notification should 
		   be generated Fault->Normal->HighLimit */	
		else if((EVENT_STATE_LOW_LIMIT == eEventStateTo || 
			EVENT_STATE_HIGH_LIMIT == eEventStateTo || 
			EVENT_STATE_OFFNORMAL == eEventStateTo) 
			&& !bWriteNULL)
		{
			/* set the flag */
			bNodeAlreadyAddedInList = true;
			#ifdef BACDEL_PR14
			/* get offset of time delay normal property */
			GET_TIME_DELAY_NORMAL_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stTimeDelayNormal.m_u32Val,
				ul32OffsetAddr);
			/* copy value from offset address */
			memcpy(&u32TimeDelayNormal, (uint32_t *)(ul32BaseAddr + ul32OffsetAddr),
				sizeof(uint32_t));
			#endif
			/* convert present value property required for link list creation */
			Ext_AlgorithmicAlarm_ConvertPresentValue(&uPropValue, pstMonitorPropertyValue, &u32CopySize);

			/* NOTE: check the reference property as per obj type, 
			   algorithmic timer is not dependent on this value for now */
			/* add the data to event_state linklist for monitoring */
			Ext_AlgorithmicAlarm_CreateEventStateList(PROP_PRESENT_VALUE,
				OBJECT_EVENT_ENROLLMENT, pvObject, eEventStateTo, eEventStateFrom,
				&uPropValue, u32TimeDelay, u32TimeDelayNormal,
				(void *)pstVirtualDev);
		}

		/* bWriteNull = true indicates that this call is from timer 
		   or 
		   bDoubleNotification = double notification is to be generated, 
		   Timer has executed for TimeDelayNormal time interval, so directly update 
		   event state to ToNormal state */					
		if(bWriteNULL || bDoubleNotification)
		{
			pvLocalData = &eEventStateTo;
			eAccessType = READ_ONLY;
			/* assign prop value */
			pvDummy = &uPropValue;
			/* update the event state */
			UpdateCommonProperty(OBJECT_EVENT_ENROLLMENT, pvObject, PROP_EVENT_STATE, 
				-1 , pvLocalData, eAccessType, &u32PropertyAdd, &pvDummy, 
				pvVirtualDev, false, false, false);
		}
		/* bWriteNull and bDoubleNotification = false indicate that 
		   reliability value changed to NO_FAULT_DETECTED, So add 
		   this node in intrinsic linklist for TimeDelayNormal functionality */
		else if(!bWriteNULL && !bDoubleNotification && !bNodeAlreadyAddedInList)
		{
			#ifdef BACDEL_PR14
			/* get offset of time delay normal property */
			GET_TIME_DELAY_NORMAL_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stTimeDelayNormal.m_u32Val,
				ul32OffsetAddr);
			/* copy value from offset address */
			memcpy(&u32TimeDelayNormal, (uint32_t *)(ul32BaseAddr + ul32OffsetAddr),
				sizeof(uint32_t));
			#endif
			/* convert present value property required for link list creation */
			Ext_AlgorithmicAlarm_ConvertPresentValue(&uPropValue, pstMonitorPropertyValue, &u32CopySize);

            /* NOTE: check the reference property as per obj type, 
               algorithmic timer is not dependent on this value for now */
            Ext_AlgorithmicAlarm_CreateEventStateList(PROP_PRESENT_VALUE, 
				OBJECT_EVENT_ENROLLMENT, pvObject, eEventStateTo, eEventStateFrom,
				&uPropValue, u32TimeDelay, u32TimeDelayNormal, pvVirtualDev);
		}

		/* add node in algorithmic link list */
		if(bDoubleNotification)
		{
			#ifdef BACDEL_PR14
			/* get offset of time delay normal property */
			GET_TIME_DELAY_NORMAL_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stTimeDelayNormal.m_u32Val,
				ul32OffsetAddr);
			/* copy value from offset address */
			memcpy(&u32TimeDelayNormal, (uint32_t *)(ul32BaseAddr + ul32OffsetAddr),
				sizeof(uint32_t));
			#endif
			/* convert present value property required for link list creation */
			Ext_AlgorithmicAlarm_ConvertPresentValue(&uPropValue, pstMonitorPropertyValue, &u32CopySize);

			/* add 2nd notification to intrinsic linklist */
			/* NOTE: check the reference property as per obj type, 
			   intrinsic timer is not dependent on this value for now */
			Ext_AlgorithmicAlarm_CreateEventStateList(PROP_PRESENT_VALUE, 
				OBJECT_EVENT_ENROLLMENT, pvObject, eEventStateToBackup, EVENT_STATE_NORMAL, 
				&uPropValue, u32TimeDelay, u32TimeDelayNormal, pvVirtualDev);
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarms_CheckForEventStateChange: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* Function checks if event state property needs to be updated or not depending
* upon the external property value.
* This function is called only after external property value is received from application
*
* @param  eObjectType		[in]  Object type
* @param  pvObjectAddr		[in]  EE object base address
* @param  pstMonitoredValue	[in]  Monitor property value 
* @param  pbEventChange		[in]  Flag indicate if event state needs to be updated or not
* @param  peEventStateTo	[in]  Pointer to hold the current value of event state
* @param  peEventStateFrom	[in]  Pointer to hold the new value of event state
* @param  pu32TimeDelay		[in]  Pointer to save time delay value
* @param  pvVirtualDev		[in]  Pointer of virtual device
* @param  bIsInternal		[in]  Specifies internal or external alarms
*
* @return BACNET_RETURN_TYPE	[out]  Success or suitable error code.
*
*/
BACNET_RETURN_TYPE Ext_AlgorithmicAlarm_UpdateEventState(
	BACNET_OBJECT_TYPE eObjectType,
	void *pvObjectAddr,
	AnyValue_t *pstMonitoredValue,
	bool *pbEventChange,
	BACNET_EVENT_STATE *peEventStateTo,
	BACNET_EVENT_STATE *peEventStateFrom,
	uint32_t *pu32TimeDelay, 
	void *pvVirtualDev,
	bool bIsInternal)
{
	/* local variables */
	bool bEventDetectionEnable = false;
	bool bEventAlgoInhibit = false;
	bool bEventAlgoInhibitBackup = false;
	bool bReliabilityEvaInhibit = false;
	bool bReliabilityEvaInhibitBackup = false;
	BACNET_EVENT_TYPE eEventType = MAX_EVENT_TYPE;
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	BACNET_EVENT_STATE eEventState = MAX_EVENT_STATE;
	BACNET_RELIABILITY eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
	uint32_t u32CopySize = 0;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	Pr_BACnetEventParameter_t *pstEventParameter = NULL;
	EventEnrollment_t *pstEeObject = NULL;
	void *pvFbObjAddr = NULL;
	bool bObjectEnable = false;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_UpdateEventState: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pvObjectAddr|| NULL == pstMonitoredValue || NULL == pbEventChange ||
		NULL == peEventStateTo || NULL == peEventStateFrom || NULL == pu32TimeDelay ||
		NULL == pvVirtualDev)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_UpdateEventState: Null input pointer \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

	/* convert base address of object */
	ul32BaseAddr = (ulong32_t )pvObjectAddr;
	pstEeObject = (EventEnrollment_t *)pvObjectAddr;

	#ifdef BACDEL_PR14
	/* get event detection enable property value */
	bEventDetectionEnable = pstEeObject->m_stEventDetectionEnable.m_bVal;
	/* check event detection enable is true or false */
	if(!bEventDetectionEnable)
	{
		/* external event algorithmic reporting is disabled */
		*pbEventChange = FALSE;
		return eReturnVal;
	}

	/* get event algorithm inhibit property value */
	bEventAlgoInhibit = pstEeObject->m_stEventAlgorithmInhibit.m_bVal;

	/* copy inhibit backup val into property offset address */
	bEventAlgoInhibitBackup = pstEeObject->m_bBackupEventAlgoInhibit;

	/* check if event-algorithm is disabled */
	/* backup-rel-inhibit flag is used to send to normal notification and then
	disable further monitoring of this EE node, till event-algo-inhibit is enabled */
	if(bEventAlgoInhibit && bEventAlgoInhibitBackup)
	{
		/* external event algorithmic reporting is disabled */
		*pbEventChange = FALSE;
		return eReturnVal;
	}

	/* get reliability evaluation inhibit property value */
	bReliabilityEvaInhibit = pstEeObject->m_stReliabilityEvaluationInhibit.m_bVal;

	/* copy inhibit backup val into property offset address */
	bReliabilityEvaInhibitBackup = pstEeObject->m_bBackupRelEvalInhibit;
	/* check if reliability-evaluation-inhibit is disabled */
	/* backup-reliability-evaluation-inhibit flag is used to send to normal notification 
	and then disable further monitoring of this EE node, till event-algo-inhibit is enabled */
	if(bReliabilityEvaInhibit && bReliabilityEvaInhibitBackup)
	{
		/* event algorithmic reporting is disabled */
		*pbEventChange = FALSE;
		return eReturnVal;
	}

	/* get reliability property value */
	eReliabilty = pstEeObject->m_stReliability.m_eReliabilty;
	/* check relaibility property value */
	if(RELIABILITY_NO_FAULT_DETECTED != eReliabilty)
	{
		/* reliability is in fault state */
		*pbEventChange = FALSE;
		return eReturnVal;
	}
	#else

	/* get the value of object enable flag */
	GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, bObjectEnable, ul32OffsetAddr);
	memcpy(&bObjectEnable, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(bool));

	/* event type, event parameter & obj prop ref validation failed */
	if(!bObjectEnable)
	{
		/* configuration error in EE object */
		*pbEventChange = FALSE;
		return eReturnVal;
	} 
	#endif /* PR14 */

	/* get event type property value */
	eEventType = pstEeObject->m_stEventype.m_eEventType;

	/* get event state property value */
	eEventState = pstEeObject->m_stEventState.m_eEventState;
	*peEventStateFrom = eEventState;

	#ifdef BACDEL_PR14
	/* check that reliability evalution inhibit property value is true */
	if(!bReliabilityEvaInhibitBackup)
	{
		/* check if current event state is fault */
		if(EVENT_STATE_NORMAL != eEventState)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;

			/* set bReliabilityEvaInhibitBackup value to false */
			bReliabilityEvaInhibitBackup = false;
			pstEeObject->m_bBackupRelEvalInhibit = bReliabilityEvaInhibitBackup;
			return eReturnVal;
		}
		else
		{
			/* reset backup flag to true as notification is sent, and skip this EE
			node till reliability evalution inhibit enabled i.e false */
			/* set bReliabilityEvaInhibitBackup value to false */
			bReliabilityEvaInhibitBackup = true;
			pstEeObject->m_bBackupRelEvalInhibit = bReliabilityEvaInhibitBackup;
			return eReturnVal;
		}
	}

	/* event algorithm inhibit is disabled i.e. true, so send ToNormal
	notification with time delay normal interval.
	when event algorithm inhibit is disabled, backup-event-algo-inhibit is false
	till to normal notification is sent */
	if(!bEventAlgoInhibitBackup)
	{
		/* check if current state is not normal */
		if(EVENT_STATE_NORMAL != eEventState)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;

			/* set bBackupRelEvalInhibit value to false */
			bEventAlgoInhibitBackup = false;
			pstEeObject->m_bBackupEventAlgoInhibit = bEventAlgoInhibitBackup;
			return eReturnVal;
		}
		else
		{
			/* reset backup flag to true as notification is sent, and skip this EE
			node till event-algo-inhibit becomes enabled i.e false */
			bEventAlgoInhibitBackup = true;
			pstEeObject->m_bBackupEventAlgoInhibit = bEventAlgoInhibitBackup;
			return eReturnVal;
		}
	}
	#endif

	/* get event parameter property value ponter */
	pstEventParameter = &pstEeObject->m_stEventParameter;

	/* check event type */
	switch(eEventType)
	{
	case EVENT_CHANGE_OF_STATE:
	{
		/* local variables */
		ChangeOfState_t	*pstCngState = NULL;
		ListOfBACnetPropertyStates_t *pstListVal = NULL;

		/* get change of state parameter pointer value */
		pstCngState = &pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stCngState;

		#if 0
		/* event algorithm inhibit is disabled i.e. true, so send ToNormal
		notification with time delay normal interval.
		when event algorithm inhibit is disabled, backup-event-algo-inhibit is false
		till to normal notification is sent */
		if(!bEventAlgoInhibitBackup)
		{
			/* check if current state is not normal */
			if(EVENT_STATE_NORMAL != eEventState)
			{
				/* change event state to normal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;

				/* set bBackupRelEvalInhibit value to false */
				bEventAlgoInhibitBackup = false;
				pstEeObject->bBackupRelEvalInhibit = bEventAlgoInhibitBackup;
				break;
			}
			else
			{
				/* reset backup flag to true as notification is sent, and skip this EE
				node till event-algo-inhibit becomes enabled i.e false */
				bEventAlgoInhibitBackup = true;
				pstEeObject->bBackupRelEvalInhibit = bEventAlgoInhibitBackup;
				break;
			}
		}
		#endif

		/* get property state list */
		pstListVal = pstCngState->m_pstListOfValues;
		/* compare the memories as there are many data type */
		if(Ext_AlgorithmicAlarm_CheckChangeOfStateValues(pstMonitoredValue, pstListVal))
		{
			/* match found */
			if(eEventState != EVENT_STATE_OFFNORMAL)
			{
				/* change event state to offnormal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_OFFNORMAL;
			}
		}
		else
		{
			/* match not found */
			if(eEventState != EVENT_STATE_NORMAL)
			{
				/* change event state to normal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
		}

		/* save time delay value */
		*pu32TimeDelay = pstCngState->m_u32Timedelay;
	}
	break;

	case EVENT_COMMAND_FAILURE:
	{
		/* local variables */
		uint32_t u32RefFeedbackObjId = 0;
		BACNET_OBJECT_TYPE eRefFeedbackObjType = MAX_ASHRAE_OBJECT_TYPE;
		BACNET_PROPERTY_ID eRefFeedbackPropId = MAX_PROP_SUPPORTED;
		AnyValue_t *pstMonitoredFeedbackValue = NULL;
		CommandFailure_t *pstCmdFail = NULL;
		Pr_BinaryEnumPV_t *pstFeedbackvalue = NULL;
		BACNET_BINARY_PV eFeedbackVal = BINARY_NULL;
		void *pvPropertyValue = NULL;

		/* check tag type value */
		if(BACNET_APPLICATION_TAG_ENUMERATED != pstMonitoredValue->m_stValue.m_TagType && 
			BACNET_APPLICATION_TAG_UNSIGNED_INT != pstMonitoredValue->m_stValue.m_TagType)
		{
			/* tag type value is invalid */
			*pbEventChange = FALSE;
			break;
		}

		/* get command failure parameter pointer value */
		pstCmdFail = &pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stCmdFail;

		#if 0
		/* event algorithm inhibit is disabled i.e. true, so send ToNormal
		notification with time delay normal interval.
		when event algorithm inhibit is disabled, backup-event-algo-inhibit is false
		till to normal notification is sent */
		if(!bEventAlgoInhibitBackup)
		{
			/* check if current state is not normal */
			if(EVENT_STATE_NORMAL != eEventState)
			{
				/* change event state to normal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;

				/* set bBackupRelEvalInhibit value to false */
				bEventAlgoInhibitBackup = false;
				pstEeObject->bBackupRelEvalInhibit = bEventAlgoInhibitBackup;
				break;
			}
			else
			{
				/* reset backup flag to true as notification is sent, and skip this EE
				node till event-algo-inhibit becomes enabled i.e false */
				bEventAlgoInhibitBackup = true;
				pstEeObject->bBackupRelEvalInhibit = bEventAlgoInhibitBackup;
				break;
			}
		}
		#endif

		/* get monitor feedback value offset address */
		GET_EE_OFFSET(eObjectType, m_stExternalFeedbackValue, ul32OffsetAddr);
		/* get monitor feedback value pointer */
		pstMonitoredFeedbackValue = (AnyValue_t *)(ul32BaseAddr + ul32OffsetAddr);

		/* check property tag value */
		if(pstMonitoredValue->m_stValue.m_TagType != pstMonitoredFeedbackValue->m_stValue.m_TagType)
		{
			/* values are not same datatype */
			*pbEventChange = FALSE;
			break;
		}

		/* set feedback value to zero */
		memset(&pstCmdFail->m_uFeedbackPropValue, BACNET_ZERO, sizeof(PropertyValue_u));
		/* convert and store feedback value */
		Ext_AlgorithmicAlarm_ConvertPresentValue(
			&pstCmdFail->m_uFeedbackPropValue, pstMonitoredFeedbackValue, &u32CopySize);
		/* save tag value */
		pstCmdFail->m_u8Apptag = pstMonitoredFeedbackValue->m_stValue.m_TagType;

		/* compare the values */
		if(!memcmp(&pstMonitoredValue->m_stValue.uValue, &pstMonitoredFeedbackValue->m_stValue.uValue, 
			sizeof(pstMonitoredValue->m_stValue.uValue)))
		{
			/* prop value & feedback value is equal */
			if(eEventState != EVENT_STATE_NORMAL)
			{
				/* change event state to normal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
		}
		else
		{
			/* prop value & feedback value is not equal */
			if(eEventState != EVENT_STATE_OFFNORMAL)
			{
				/* change event state to normal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_OFFNORMAL;
			}
		}

		/* save time delay value */
		*pu32TimeDelay = pstCmdFail->m_u32Timedelay;
	}
	break;

	case EVENT_OUT_OF_RANGE:
	{
		/* local variables */
		Float_t fLowLimit = 0.0, fHighLimit = 0.0, fDeadband = 0.0, fPresentValue = 0.0;

		/* check tag type value */
		if(BACNET_APPLICATION_TAG_REAL != pstMonitoredValue->m_stValue.m_TagType)
		{
			/* tag type value is invalid */
			*pbEventChange = FALSE;
			break;
		}

		/* get present value */
		fPresentValue = pstMonitoredValue->m_stValue.uValue.m_Real;
		/* get low limit value */
		fLowLimit = pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stOutRange.m_fLow_Limit;
		/* get high limit value */
		fHighLimit = pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stOutRange.m_fHigh_Limit;
		/* get deadband value */
		fDeadband = pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stOutRange.m_fDeadband;

		#if 0
		/* event algorithm inhibit is disabled i.e. true, so send ToNormal
		notification with time delay normal interval.
		when event algorithm inhibit is disabled, backup-event-algo-inhibit is false
		till to normal notification is sent */
		if(!bEventAlgoInhibitBackup)
		{
			/* check if current state is not normal */
			if(EVENT_STATE_NORMAL != eEventState)
			{
				/* change event state to normal */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;

				/* set bBackupRelEvalInhibit value to false */
				bEventAlgoInhibitBackup = false;
				pstEeObject->bBackupRelEvalInhibit = bEventAlgoInhibitBackup;
				break;
			}
			else
			{
				/* reset backup flag to true as notification is sent, and skip this EE
				node till event-algo-inhibit becomes enabled i.e false */
				bEventAlgoInhibitBackup = true;
				pstEeObject->bBackupRelEvalInhibit = bEventAlgoInhibitBackup;
				break;
			}
		}
		#endif

		/* verify that high limit & low limit are not same, if they are same then dont compare */
		if(fLowLimit == fHighLimit)
		{
			*pbEventChange = FALSE;
			break;
		}

		#ifdef BACDEL_PR14
		/* check for fault to normal state */
		if(((fPresentValue >= fLowLimit) && (fPresentValue < (fHighLimit - fDeadband)))
			&& ((fPresentValue <= fHighLimit) && (fPresentValue >(fLowLimit + fDeadband)))
			&& EVENT_STATE_FAULT == eEventState)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;
		}
		else
		#endif
		/* check for to_offnormal state - low limit */
		if(fPresentValue < fLowLimit && eEventState != EVENT_STATE_LOW_LIMIT)
		{
			/* change event state to low limit */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_LOW_LIMIT;
		}
		/* check for to_offnormal state - high limit */
		else if(fPresentValue > fHighLimit && eEventState != EVENT_STATE_HIGH_LIMIT)
		{
			/* change event state to high limit */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_HIGH_LIMIT;
		}
		/* check for to_normal state */
		else if((fPresentValue <= fHighLimit) && (fPresentValue > (fLowLimit + fDeadband))
			&& eEventState == EVENT_STATE_LOW_LIMIT)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;
		}
		/* check for to_normal state */
		else if((fPresentValue < (fHighLimit - fDeadband))	&& (fPresentValue >= fLowLimit)
			&& eEventState == EVENT_STATE_HIGH_LIMIT)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;
		}

		/* save time delay value */
		*pu32TimeDelay = pstEventParameter->m_stEventParam.BACnetEventParameter_u.
			m_stOutRange.m_u32Timedelay;
	}
	break;

	case EVENT_UNSIGNED_OUT_OF_RANGE:
	{
		/* local variables */
		uint32_t u32LowLimit = 0, u32HighLimit = 0, u32Deadband = 0, u32PresentValue = 0;
		/* get present value */
		u32PresentValue = pstMonitoredValue->m_stValue.uValue.m_Unsigned_Int;
		/* get low limit value */
		u32LowLimit = pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stUnSignedOutofRange.m_u32Low_Limit;
		/* get high limit value */
		u32HighLimit = pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stUnSignedOutofRange.m_u32High_Limit;
		/* get deadband value */
		u32Deadband = pstEventParameter->m_stEventParam.BACnetEventParameter_u.m_stUnSignedOutofRange.m_u32Deadband;

		/* verify that high limit & low limit are not same, if they are same then dont compare */
		if(u32LowLimit == u32HighLimit)
		{
			*pbEventChange = FALSE;
			break;
		}

		#ifdef BACDEL_PR14
		/* check for fault to normal state */
		if(((u32PresentValue >= u32LowLimit) && (u32PresentValue < (u32HighLimit - u32Deadband)))
			&& ((u32PresentValue <= u32HighLimit) && (u32PresentValue >(u32LowLimit + u32Deadband)))
			&& EVENT_STATE_FAULT == eEventState)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;
		}
		else
		#endif
		/* check for to_offnormal state - low limit */
		if(u32PresentValue < u32LowLimit && eEventState != EVENT_STATE_LOW_LIMIT)
		{
			/* change event state to low limit */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_LOW_LIMIT;
		}
		/* check for to_offnormal state - high limit */
		else if(u32PresentValue > u32HighLimit && eEventState != EVENT_STATE_HIGH_LIMIT)
		{
			/* change event state to high limit */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_HIGH_LIMIT;
		}
		/* check for to_normal state */
		else if((u32PresentValue <= u32HighLimit) && (u32PresentValue > (u32LowLimit + u32Deadband))
			&& eEventState == EVENT_STATE_LOW_LIMIT)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;
		}
		/* check for to_normal state */
		else if((u32PresentValue < (u32HighLimit - u32Deadband))	&& (u32PresentValue >= u32LowLimit)
			&& eEventState == EVENT_STATE_HIGH_LIMIT)
		{
			/* change event state to normal */
			*pbEventChange = TRUE;
			*peEventStateTo = EVENT_STATE_NORMAL;
		}

		/* save time delay value */
		*pu32TimeDelay = pstEventParameter->m_stEventParam.BACnetEventParameter_u.
			m_stUnSignedOutofRange.m_u32Timedelay;
	}
	break;

	default:
	{
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, 
		"APDU: Ext_AlgorithmicAlarm_UpdateEventState: Unsupported event type value. \r\n");
		#endif
	}
	break;
	}//switch ends

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_UpdateEventState: Exit \r\n");
	#endif
	return eReturnVal;
}

/**
*
* DESCRIPTION
* This function is used to convert property value from 'AnyValue_t' datatype to 
* 'PropertyValue_u' datatype.
* In external algorithmic list, property value needs to add of type  'PropertyValue_u'.
*
* @param  pstSrcValue	[in]   property value is read from here
* @param  puDestValue	[out]  converted value is stored here
* @param  pu32CopySize	[out]  size of data type copied
*
* @return void	[out]  No return value.
*
*/
void Ext_AlgorithmicAlarm_ConvertPresentValue(
	PropertyValue_u	*puDestValue,
	AnyValue_t *pstSrcValue,
	uint32_t *pu32CopySize)
{
	/* local variable */

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_ConvertPresentValue: Entry \r\n");
	#endif

	/* check NULL pointer */
	if(NULL == puDestValue || NULL == pstSrcValue || NULL == pu32CopySize)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_ConvertPresentValue: Null input pointer \r\n");
		#endif
		return;
	}

	/* set default value */
	*pu32CopySize = 0;

	/* check tag tpe and save present value */
	if(BACNET_APPLICATION_TAG_ENUMERATED == (BACNET_APPLICATION_TAG)pstSrcValue->m_stValue.m_TagType)
	{
		/* enumerated value */
		puDestValue->eVal = pstSrcValue->m_stValue.uValue.m_Enumerated;
		*pu32CopySize = sizeof(uint32_t);
	}
	else if(BACNET_APPLICATION_TAG_UNSIGNED_INT == (BACNET_APPLICATION_TAG)pstSrcValue->m_stValue.m_TagType)
	{
		/* unsigned value */
		puDestValue->u32Val = pstSrcValue->m_stValue.uValue.m_Unsigned_Int;
		*pu32CopySize = sizeof(uint32_t);
	}
	else if(BACNET_APPLICATION_TAG_REAL == (BACNET_APPLICATION_TAG)pstSrcValue->m_stValue.m_TagType)
	{
		/* real value */
		puDestValue->fVal = pstSrcValue->m_stValue.uValue.m_Real;
		*pu32CopySize = sizeof(Float_t);
	}
	else if(BACNET_APPLICATION_TAG_SIGNED_INT == (BACNET_APPLICATION_TAG)pstSrcValue->m_stValue.m_TagType)
	{
		/* signed value */
		puDestValue->i32Val = pstSrcValue->m_stValue.uValue.m_Signed_Int;
		*pu32CopySize = sizeof(int32_t);
	}
	else if(BACNET_APPLICATION_TAG_BOOLEAN == (BACNET_APPLICATION_TAG)pstSrcValue->m_stValue.m_TagType)
	{
		/* boolean value */
		puDestValue->bVal = pstSrcValue->m_stValue.uValue.m_Boolean;
		*pu32CopySize = sizeof(bool);
	}
	else if(BACNET_APPLICATION_TAG_DOUBLE == (BACNET_APPLICATION_TAG)pstSrcValue->m_stValue.m_TagType)
	{
		/* double value */
		puDestValue->dVal = pstSrcValue->m_stValue.uValue.m_Double;
		*pu32CopySize = sizeof(Double_t);
	}
	else;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_ConvertPresentValue: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function creates the linklist for objects whose external property value is 
* received from application. it then monitors the property value till time_delay (property) expires.
* If it finds that event state needs to be changed after expiry of timer, then it
* will signal out to update the event state.
*
* @param  ePropId			 [in]  Property ID
* @param  eObjectType		 [in]  Object type
* @param  pvObjBaseAddr		 [in]  Object base address
* @param  peEventStateTo	 [in]  Pointer to hold the current value of event state
* @param  peEventStateFrom	 [in]  Pointer to hold the new value of event state
* @param  puPropValue		 [in]  Property value
* @param  u32TimeDelay		 [in]  Time delay value
* @param  u32TimeDelayNormal [in]  Time delay normal value
* @param  pvVirtualDev		 [in]  Virtual device data
*
* @return BACNET_RETURN_TYPE [out]  Success or suitable error code.
*
*/
BACNET_RETURN_TYPE Ext_AlgorithmicAlarm_CreateEventStateList(
	BACNET_PROPERTY_ID ePropId,
	BACNET_OBJECT_TYPE eObjectType,
	void *pvObjBaseAddr,
	BACNET_EVENT_STATE eEventStateTo,
	BACNET_EVENT_STATE eEventStateFrom,
	PropertyValue_u *puPropValue,
	uint32_t u32TimeDelay,
	uint32_t u32TimeDelayNormal,
	void *pvVirtualDev)
{
	/* local variables  */
	BACNET_RETURN_TYPE eFuncReturn = BACDEL_SUCCESS;
	MsgQueListExt_t stData = {0};
	LinkListExt_t *pstLocalCurrentNode = NULL;
	bool bNodeAlreadyExist = FALSE;
	ulong32_t ul32BaseAddr = 0;	
	ulong32_t ul32OffsetAddr = 0;
	uint32_t u32ObjectId = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_CreateEventStateList: Entry \r\n");
	#endif
	
	/* check input pointer */
	if(NULL == pvObjBaseAddr || NULL == puPropValue || NULL == pvVirtualDev)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_CreateEventStateList: Null input pointer \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

	/* get the base address */
	ul32BaseAddr = (ulong32_t)pvObjBaseAddr;
	/* get offset address of object if of given object type & given object base address */
	GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32OffsetAddr);
	/* copy data from property offset address to u32objectid */
	memcpy(&u32ObjectId, (uint32_t *)(ul32BaseAddr + ul32OffsetAddr), sizeof(uint32_t));

	/* fill the data for the linklist node */
	stData.m_eEventStateTo = eEventStateTo;
	stData.m_eEventStateFrom = eEventStateFrom;
	stData.m_eObjectType = eObjectType;
	stData.m_u32ObjectId = u32ObjectId;
	stData.m_pvObjBaseAddr = pvObjBaseAddr;
	stData.m_ePropId = ePropId;
	stData.m_pvVirtualDev = pvVirtualDev;
	stData.m_u32DevId = ((virtualDevData_t *)pvVirtualDev)->
		m_stDevObject.m_stObjectID.m_u32ObjId;

	/* updated time delay & check if double notification */
	if(IsDoubleNotification(eObjectType, eEventStateFrom, eEventStateTo))
	{
		/* save time delay */
		#ifdef BACDEL_PR14
		stData.m_i32TimeDelay = u32TimeDelayNormal;
		#else
		stData.m_i32TimeDelay = u32TimeDelay;
		#endif
		stData.m_i32TimeDelayBackup = u32TimeDelay;
	}
	else
	{
		if(EVENT_STATE_NORMAL == eEventStateTo)
		{
			/* save time delay */
			#ifdef BACDEL_PR14
			stData.m_i32TimeDelay = u32TimeDelayNormal;
			stData.m_i32TimeDelayBackup = u32TimeDelayNormal;
			#else
			stData.m_i32TimeDelay = u32TimeDelay;
			stData.m_i32TimeDelayBackup = u32TimeDelay;
			#endif
		}
		else if(EVENT_STATE_OFFNORMAL == eEventStateTo ||
			EVENT_STATE_HIGH_LIMIT == eEventStateTo ||
			EVENT_STATE_LOW_LIMIT == eEventStateTo)
		{
			/* save time delay */
			stData.m_i32TimeDelay = u32TimeDelay;
			stData.m_i32TimeDelayBackup = u32TimeDelay;
		}
	}

	/* copy the data */
	memcpy(&stData.m_uPropValue, puPropValue, sizeof(PropertyValue_u));

	/* aquire mutex lock */
	Osal_Wait_Mutex(g_stExternalAlgorithmic.m_hExtAlgoMtxLock, INFINITE);

	/* first node in linklist should be directly added to list */
	if(g_stExternalAlgorithmic.m_pstFirstNode == NULL &&
		g_stExternalAlgorithmic.m_pstLastNode == NULL)
	{
		Ext_AlgorithmicAlarm_AddNodeToList(&stData);
	}
	/* if first node exists, then we need to check if the node with same object type & id already exists */
	else
	{
		/* local pointer to traverse linklist */
		pstLocalCurrentNode = g_stExternalAlgorithmic.m_pstFirstNode;
		/* traverse the linklist */
		while(pstLocalCurrentNode != NULL)
		{
			if(pstLocalCurrentNode->m_stListData.m_pvObjBaseAddr == stData.m_pvObjBaseAddr &&
				pstLocalCurrentNode->m_stListData.m_eObjectType == stData.m_eObjectType &&
				pstLocalCurrentNode->m_stListData.m_u32ObjectId == stData.m_u32ObjectId)
			{
				bNodeAlreadyExist = TRUE;
				break;
			}
			/* maintain the previous node pointer */
			pstLocalCurrentNode = pstLocalCurrentNode->m_pstNext;
		}

		/* check if node is already exist */
		if(bNodeAlreadyExist == TRUE)
		{
			bNodeAlreadyExist = FALSE;
			/* replace the data of node with the new data */
			memcpy(&pstLocalCurrentNode->m_stListData, &stData, sizeof(MsgQueListExt_t));
		}
		else
		{
			/* since node with same obj address does not exist, add new node to link list */
			Ext_AlgorithmicAlarm_AddNodeToList(&stData);
		}
	}

	/* release the mutex */
	Osal_Release_Mutex(g_stExternalAlgorithmic.m_hExtAlgoMtxLock);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_CreateEventStateList: Exit \r\n");
	#endif
	return eFuncReturn;
}

/**
*
* DESCRIPTION
* This function checks if node is to be deleted from external linklist
*
* @param  pstDeleteNode	[in]  Pointer of node to be deleted
* @return void			[out] Void/nothing.
*
**/
static void Ext_AlgorithmicAlarm_CheckAndDeleteThisNode(LinkListExt_t *pstDeleteNode)
{
	/* local variable */
	LinkListExt_t *pstDummyNode = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_CheckAndDeleteThisNode: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstDeleteNode)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_CheckAndDeleteThisNode: Null input pointer \r\n");
		#endif
		return;
	}

	/* check if list contain only one node & delete this node */
	if(g_stExternalAlgorithmic.m_pstFirstNode == g_stExternalAlgorithmic.m_pstLastNode
		&& pstDeleteNode == g_stExternalAlgorithmic.m_pstFirstNode)
	{
		OSAL_Free(g_stExternalAlgorithmic.m_pstFirstNode, __FILE__, __FUNCTION__, __LINE__);
		g_stExternalAlgorithmic.m_pstFirstNode = NULL;
		g_stExternalAlgorithmic.m_pstLastNode = NULL;
		g_stExternalAlgorithmic.m_pstPrevious = NULL;
	}
	/* delete first node of external algorithmic list */
	else if(pstDeleteNode == g_stExternalAlgorithmic.m_pstFirstNode)
	{		
		pstDummyNode = g_stExternalAlgorithmic.m_pstFirstNode->m_pstNext;
		OSAL_Free(g_stExternalAlgorithmic.m_pstFirstNode, __FILE__, __FUNCTION__, __LINE__);
		g_stExternalAlgorithmic.m_pstFirstNode = pstDummyNode;
	}
	/* delete last node of external algorithmic list */
	else if(pstDeleteNode == g_stExternalAlgorithmic.m_pstLastNode)
	{
		g_stExternalAlgorithmic.m_pstPrevious->m_pstNext = NULL;
		OSAL_Free(g_stExternalAlgorithmic.m_pstLastNode, __FILE__, __FUNCTION__, __LINE__);
		g_stExternalAlgorithmic.m_pstLastNode = g_stExternalAlgorithmic.m_pstPrevious;
	}
	/* delete middle node of external algorithmic list */
	else if(pstDeleteNode != g_stExternalAlgorithmic.m_pstFirstNode
		&& pstDeleteNode != g_stExternalAlgorithmic.m_pstLastNode)
	{
		g_stExternalAlgorithmic.m_pstPrevious->m_pstNext = pstDeleteNode->m_pstNext;
		OSAL_Free(pstDeleteNode, __FILE__, __FUNCTION__, __LINE__);
		if (g_stExternalAlgorithmic.m_pstPrevious->m_pstNext == NULL)
		{
			g_stExternalAlgorithmic.m_pstLastNode = g_stExternalAlgorithmic.m_pstPrevious;
		}
	}
	/* handle error condition */
	else
	{
		;	// error condition
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_CheckAndDeleteThisNode: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function is used to provide monitored value for external alarms 
* functionality.
*
* @param  pstMonitoredValue			[in]  Monitored property value
* @param  pstMonitoredStatusFlags	[in]  Monitored object status flags
* @param  pvVirtualDev				[in]  Virtual device data
* @param  eObjectType				[in]  Event enrollment object type
* @param  u32ObjectId				[in]  Instance no of event enrollment object
* @param  bIsInternal				[in]  Specifies internal or external alarms
*
* @return void	[out]  No return value
*
*/
void Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange(
	AnyValue_t *pstMonitoredValue,
	BACnetBitStr_t *pstMonitoredStatusFlags,
	void *pvVirtualDev,
	BACNET_OBJECT_TYPE eObjectType,
	uint32_t u32ObjectId,
	bool bIsInternal)
{
	/* local variables */
	bool bEventChange = false;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	void *pvObject = NULL;
	virtualDevData_t *pstVirtualDev = NULL;
	AnyValue_t *pstMonitorPropertyValue = NULL;
	Pr_BACnetBitStr_t *pstExternalStatusFlag = NULL;
	BACnetDevObjPropRef_t  *pstObjectPropertyReference = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstMonitoredValue || NULL == pvVirtualDev)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange: Null input pointer \r\n");
		#endif
		return;
	}

	/* get the virtual device pointer */
	pstVirtualDev = (virtualDevData_t *)pvVirtualDev;

	/* find the event entrollment object */
	pvObject = Find_Object(eObjectType, u32ObjectId, pstVirtualDev);
	/* check null pointer validation */
	if(NULL != pvObject)
	{
		/* convert object base address */
		ul32BaseAddr = (ulong32_t)pvObject;

#if STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS
		/* check that device supporting external reporting */
		if(bIsInternal)
		{
			/* get offset address of monitor present value property */
			GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stExternalValue, ul32OffsetAddr);
			pstMonitorPropertyValue = (AnyValue_t *)(ul32BaseAddr + ul32OffsetAddr);
			/* copy value */
			memcpy(&pstMonitorPropertyValue->m_stValue, &pstMonitoredValue->m_stValue, 
			sizeof(BACNET_PROPERTY_VALUE));

			/* get offset address of external status flags */
			if(NULL != pstMonitoredStatusFlags)
			{
				GET_EE_OFFSET(eObjectType, m_stExternalStatusFlag, ul32OffsetAddr);
				pstExternalStatusFlag = (Pr_BACnetBitStr_t *)(ul32BaseAddr + ul32OffsetAddr);
				/* copy value */
				memcpy(&pstExternalStatusFlag->m_stBitStr, pstMonitoredStatusFlags,
					sizeof(BACnetBitStr_t));
			}
		}
		/* device supporting external reporting */
		else 
#endif
		{
			/* get offset address of monitor present value property */
			GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stInternalValue, ul32OffsetAddr);
			pstMonitorPropertyValue = (AnyValue_t *)(ul32BaseAddr + ul32OffsetAddr);
			/* copy value */
			memcpy(&pstMonitorPropertyValue->m_stValue, &pstMonitoredValue->m_stValue, 
				sizeof(BACNET_PROPERTY_VALUE));
		}
		
		/* check and update event state */
		Ext_AlgorithmicAlarm_CheckForEventStateChange(
			pvObject, (void *)pstVirtualDev, bIsInternal,
			false);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* This function is used to add new node in external algorithmic link list
*
* @param  pstData	[in]   Pointer of data need to add in link list
* @return void		[out]  No return vakue.
*
*/
static void Ext_AlgorithmicAlarm_AddNodeToList(MsgQueListExt_t *pstData)
{
	/* local variables */
	LinkListExt_t *pstTempNode = NULL;
	ExtAlgoEventStateLinkList_t *pstExtAlgoList = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_AddNodeToList: Entry \r\n");
	#endif

	/* allocate memory */
	pstTempNode = OSAL_Malloc(sizeof(LinkListExt_t), __FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstTempNode)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_AddNodeToList: Memory allocation failed\r\n");
		#endif
		return;
	}
	/* next of newely created node is always NULL */
	pstTempNode->m_pstNext = NULL;

	/* store data for that node */
	memcpy(&pstTempNode->m_stListData, pstData, sizeof(MsgQueListExt_t));

	/* get external algorithmic link list pointer */
	pstExtAlgoList = &g_stExternalAlgorithmic;

	/* check first node is null */
	if(pstExtAlgoList->m_pstFirstNode == NULL)
	{
		/* update the root node address only when 1st node for the list is created */
		pstExtAlgoList->m_pstFirstNode = pstTempNode;
	}
	else
	{
		/* create the list here */
		pstExtAlgoList->m_pstLastNode->m_pstNext = pstTempNode;
	}

	/* update the last node address */
	pstExtAlgoList->m_pstLastNode = pstTempNode;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_AddNodeToList: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* Function checks if property state and its values from event parameters 
* matches with external value received from application.
*
* @param  pstPropValue	[in]  External property value
* @param  pstListVal	[in]  Change of state event values
*
* @return bool	[out]  True indicate change of state values and external 
*					   value received from application are same.
*
*/
static bool Ext_AlgorithmicAlarm_CheckChangeOfStateValues( 
	AnyValue_t *pstPropValue, 
	ListOfBACnetPropertyStates_t *pstListVal)
{
	/* local variable */
	bool bReturnVal = false;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_CheckChangeOfStateValues: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstPropValue || NULL == pstListVal)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
		"APDU: Ext_AlgorithmicAlarm_CheckChangeOfStateValues: Null input pointer \r\n");
		#endif
		return false;
	}
	/* check value one by one */
	while(NULL != pstListVal)
	{
		/* check if enumerated value */
		if(PROP_STATE_BOOLEAN_VALUE != pstListVal->m_stPropStateVal.m_ePropState &&
			PROP_STATE_UNSIGNED_VALUE != pstListVal->m_stPropStateVal.m_ePropState &&
			BACNET_APPLICATION_TAG_ENUMERATED  == (BACNET_APPLICATION_TAG)pstPropValue->m_stValue.m_TagType &&
			(pstListVal->m_stPropStateVal.BACnetPropertyStates_U.m_BinaryValue == 
			pstPropValue->m_stValue.uValue.m_Enumerated))
		{
			/* set return value */
			bReturnVal = true;
			break;
		}
		/* check if unsigned value */
		else if(PROP_STATE_UNSIGNED_VALUE == pstListVal->m_stPropStateVal.m_ePropState &&
			BACNET_APPLICATION_TAG_UNSIGNED_INT == (BACNET_APPLICATION_TAG)pstPropValue->m_stValue.m_TagType &&
			(pstListVal->m_stPropStateVal.BACnetPropertyStates_U.m_UnsignedValue ==
			pstPropValue->m_stValue.uValue.m_Unsigned_Int))
		{
			/* set return value */
			bReturnVal = true;
			break;
		}
		/* check if boolean value */
		else if(PROP_STATE_BOOLEAN_VALUE == pstListVal->m_stPropStateVal.m_ePropState &&
			BACNET_APPLICATION_TAG_BOOLEAN == (BACNET_APPLICATION_TAG)pstPropValue->m_stValue.m_TagType &&
			(pstListVal->m_stPropStateVal.BACnetPropertyStates_U.m_BooleanValue ==
			pstPropValue->m_stValue.uValue.m_Boolean))
		{
			/* set return value */
			bReturnVal = true;
			break;
		}
		else;

		/* move to next element */
		pstListVal = pstListVal->m_pstNext;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_CheckChangeOfStateValues: Exit \r\n");
	#endif
	return bReturnVal;
}

/**
*
* DESCRIPTION
* This is timer for external algorithmic reporting service. It is called every 1 second.
*
* Scan entire external algorithmic reporting linklist.
* - decrement time_delay by 1 second for every node in linklist.
* - if time_delay becomes zero check actual present value & present value in node matches
*     - if it matches, update event state / reliablity (as applicable)
*     - else delete the node.
* - repeate same for every node in list.
*
*/
//TIMER_CALLBACK Ext_AlgorithmicAlarm_Timer(
void Ext_AlgorithmicAlarm_Timer(
	void* lpParam,
	bool TimerOrWaitFired)
{
	/* local variables */
	bool bDeleteNode = FALSE;
	bool bUpdateEventState = FALSE;
	bool bUpdateReliability = FALSE;
	bool bDoubleNotification = FALSE;
	static bool bExternalAlgorithmTimerFlag = false;
	uint32_t u32CopySize = 0;
	uint32_t u32PropertyAdd = 0;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	void *pvDummy = NULL;
	LinkListExt_t *pstLocalCurrentNode = NULL;
	LinkListExt_t *pstDeleteNode = NULL;
	virtualDevData_t *pVirtualDev = NULL;
	PropertyValue_u uPropVal = {0}; 
	AnyValue_t *pstExternalValue = NULL;
	AnyValue_t *pstInternalValue = NULL;
	BACNET_RELIABILITY  eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
	Pr_BACnetDevObjPropRef_t  *pstObjectPropertyReference = NULL;
	bool bIsExternal = false;

	/* avoid concurrent execution of multiple timer instances */
	if(true == bExternalAlgorithmTimerFlag)
	{
		/* return from function */
		#ifdef __linux
		return NULL;
		#else
		return;
		#endif
	}
	/* set the flag */
	bExternalAlgorithmTimerFlag = true;

	/* timer entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_Timer: Entry \r\n");
	#endif

	/* wait till the mutex handle is released */
	Osal_Wait_Mutex(g_stExternalAlgorithmic.m_hExtAlgoMtxLock, INFINITE);

	/* maintain the previous node pointer : assign to first node at start */
	g_stExternalAlgorithmic.m_pstPrevious = g_stExternalAlgorithmic.m_pstFirstNode;

	/* get the 1st node pointer */
	pstLocalCurrentNode = g_stExternalAlgorithmic.m_pstFirstNode;

	while(NULL != pstLocalCurrentNode)
	{
		/* get host device address */
		if(NULL != pstLocalCurrentNode->m_stListData.m_pvVirtualDev)
			pVirtualDev = pstLocalCurrentNode->m_stListData.m_pvVirtualDev;
		else break;

		/* decrement time_delay by 1 : as this timer callback function is called every 1 sec */
		pstLocalCurrentNode->m_stListData.m_i32TimeDelay -= 1;
		if(pstLocalCurrentNode->m_stListData.m_i32TimeDelay <= 0)
		{
			/* get object base address */
			ul32BaseAddr = (ulong32_t)(pstLocalCurrentNode->m_stListData.m_pvObjBaseAddr);

			/* get proerty offset and value */
			GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stObjectPropertyReference, ul32OffsetAddr);
			pstObjectPropertyReference = (Pr_BACnetDevObjPropRef_t *)(ul32BaseAddr + ul32OffsetAddr);
			memcpy(&bIsExternal, &pstObjectPropertyReference->m_stDevObjPropReff.m_bDeviceIdPresent, 
				sizeof(bool));
			/* check that if device is supporting external algorithmic reporting */
			#if STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS
			if(bIsExternal)
			{
				/* get offset address of present value */
				GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stExternalValue, ul32OffsetAddr);
				/* get present value */
				pstExternalValue = (AnyValue_t *)(ul32BaseAddr + ul32OffsetAddr);
				/* convert present value */
				Ext_AlgorithmicAlarm_ConvertPresentValue(&uPropVal, pstExternalValue, &u32CopySize);
			}
			/* device is supporting internal algorithmic reporting */
			else
			#else
			{
				/* get offset address of present value */
				GET_EE_OFFSET(OBJECT_EVENT_ENROLLMENT, m_stInternalValue, ul32OffsetAddr);
				/* get present value */
				pstInternalValue = (AnyValue_t *)(ul32BaseAddr + ul32OffsetAddr);
				/* convert present value */
				Ext_AlgorithmicAlarm_ConvertPresentValue(&uPropVal, pstInternalValue, &u32CopySize);
			}
			#endif

			/* compare the values */
			if(!memcmp(&uPropVal, &pstLocalCurrentNode->m_stListData.m_uPropValue, u32CopySize))
			{
				/* update event state */
				bUpdateEventState = TRUE;
				if(EVENT_STATE_FAULT ==
					pstLocalCurrentNode->m_stListData.m_eEventStateTo)
				{
					/* update reliablity property */
					bUpdateReliability = TRUE;
					eReliabilty = RELIABILITY_MULTI_STATE_FAULT;
					/* no need to write event state as it will be written
					recursively when reliability is written */
					bUpdateEventState = FALSE;
				}
				else if(EVENT_STATE_FAULT ==
					pstLocalCurrentNode->m_stListData.m_eEventStateFrom)
				{
					/* update reliablity property */
					bUpdateReliability = TRUE;
					eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					/* no need to write event state as it will be written
					recursively when reliability is written */
					bUpdateEventState = FALSE;
				}
				else;
			}

			/* check event state need to update */
			if(bUpdateEventState)
			{
				/* reset bUpdateEventState */
				bUpdateEventState = FALSE;
				/* assign present value pointer */
				pvDummy = &pstLocalCurrentNode->m_stListData.m_uPropValue;
				/* check if double notification */
				if(IsDoubleNotification(pstLocalCurrentNode->m_stListData.m_eObjectType,
					pstLocalCurrentNode->m_stListData.m_eEventStateFrom,
					pstLocalCurrentNode->m_stListData.m_eEventStateTo))
				{
					/* backup to state */
					pstLocalCurrentNode->m_stListData.m_eBackupToState =
						pstLocalCurrentNode->m_stListData.m_eEventStateTo;
					/* change to state to normal */
					pstLocalCurrentNode->m_stListData.m_eEventStateTo =
						EVENT_STATE_NORMAL;
					/* set bDoubleNotification */
					bDoubleNotification = TRUE;
				}

				/* call UpdateCommonProperty function to change event_state */
				UpdateCommonProperty(pstLocalCurrentNode->m_stListData.m_eObjectType,
					(void *)ul32BaseAddr,
					PROP_EVENT_STATE,
					-1,
					(void *)&pstLocalCurrentNode->m_stListData.m_eEventStateTo,
					READ_ONLY,
					&u32PropertyAdd, &pvDummy, pVirtualDev,
					false, false, false);

				if(bDoubleNotification)
				{
					/* reset bDoubleNotification */
					bDoubleNotification = FALSE;
					/* restore time delay for 2nd notification */
					pstLocalCurrentNode->m_stListData.m_i32TimeDelay =
						pstLocalCurrentNode->m_stListData.m_i32TimeDelayBackup;
					/* update from & to state */
					pstLocalCurrentNode->m_stListData.m_eEventStateFrom =
						EVENT_STATE_NORMAL;
					pstLocalCurrentNode->m_stListData.m_eEventStateTo =
						pstLocalCurrentNode->m_stListData.m_eBackupToState;
				}
				else
				{
					/* delete this node */
					bDeleteNode = TRUE;
					pstDeleteNode = pstLocalCurrentNode;
				}
			}
			/* check reliability need to update */
			else if(bUpdateReliability)
			{
				/* reset bUpdateReliability */
				bUpdateReliability = FALSE;
				/* check if double notification is required */
				if(IsDoubleNotification(pstLocalCurrentNode->m_stListData.m_eObjectType,
					pstLocalCurrentNode->m_stListData.m_eEventStateFrom,
					pstLocalCurrentNode->m_stListData.m_eEventStateTo))
				{
					/* set bDoubleNotification */
					bDoubleNotification = TRUE;
				}

				/* assign value to pointer */
				pvDummy = &pstLocalCurrentNode->m_stListData.m_uPropValue;
				/* call UpdateCommonProperty function to change event_state */
				UpdateCommonProperty(pstLocalCurrentNode->m_stListData.m_eObjectType,
					(void *)ul32BaseAddr,
					PROP_RELIABILITY,
					-1,
					(void *)&eReliabilty,
					READ_ONLY,
					&u32PropertyAdd, &pvDummy, pVirtualDev,
					false, false, true);

				/* NOTE - bWriteNull is sent true to indicate that this
				is call from timer */
				if(bDoubleNotification)
				{
					/* reset bDoubleNotification */
					bDoubleNotification = FALSE;
				}
				else
				{
					/* delete this node */
					bDeleteNode = TRUE;
					pstDeleteNode = pstLocalCurrentNode;
				}
			}
			else
			{
				/* time_delay <= 0 & present value got changed, or object was deleted.
				no need to monitor this node further, delete it. */
				/* delete this node */
				bDeleteNode = TRUE;
				pstDeleteNode = pstLocalCurrentNode;
			}
		}//end of if time_delay <= 0

		if(bDeleteNode == FALSE)
		{
			/* maintain the previous node pointer */
			g_stExternalAlgorithmic.m_pstPrevious = pstLocalCurrentNode;
			/* move to next node in linklist */
			pstLocalCurrentNode = pstLocalCurrentNode->m_pstNext;
		}
		else
		{
			/* if current node is deleted, then no need to update previous node pointer */
			bDeleteNode = FALSE;
			/* first move to next node in linklist */
			pstLocalCurrentNode = pstLocalCurrentNode->m_pstNext;
			/* now delete node */
			Ext_AlgorithmicAlarm_CheckAndDeleteThisNode(pstDeleteNode);
		}
	}//end of while

	/* maintain the previous node pointer */
	g_stExternalAlgorithmic.m_pstPrevious = NULL;

	/* release the mutex */
	Osal_Release_Mutex(g_stExternalAlgorithmic.m_hExtAlgoMtxLock);

	/* reset the flag */
	bExternalAlgorithmTimerFlag = false;

	/* timer exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: Ext_AlgorithmicAlarm_Timer: Exit \r\n");
	#endif

	#ifdef __linux
	return NULL;
	#endif
}
#endif /* (ALGORITHMIC_REPORTING && BACDEL_SER_AE_EN_B && BACDEL_OBJ_EE && BACDEL_OBJ_NC) */

/*************** end of file ***************/
