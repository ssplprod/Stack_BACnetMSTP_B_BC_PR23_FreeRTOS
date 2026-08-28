/******************************************************************************
*
*            Copyright (c) by SoftDEL Systems Pvt. Ltd.               
*
*  This software is copyrighted by and is the sole property of 
*  SoftDEL Systems Pvt. Ltd.
*  All rights, title, ownership, or other interests         
*  in the software remain the property of  SoftDEL Systems Pvt. Ltd.
*  This software may only be used in accordance with the corresponding        
*  license agreement.  Any unauthorized use, duplication, transmission,  
*  distribution, or disclosure of this software is expressly forbidden.  
*
*  This Copyright notice may not be removed or modified without prior    
*  Written consent of SoftDEL Systems Pvt. Ltd.
*
*  SoftDEL Systems Pvt. Ltd. reserves the right to modify this software
*  Without notice.
*
*  SoftDEL Systems Pvt. Ltd.                      info@softdel.com         
*  3rd Floor, Pentagon P4,                        http://www.softdel.com   
*  Magarpatta City, Hadapsar                                                
*  Pune - 411 028
*
*  FILE
*  serviceAcknowledgeAlarm_B.c
*
*  AUTHORS                                                                     
*  Pratham N. Murkute
*
*  DESCRIPTION
*  B-side service processes acknowledgments of previously transmitted  
*  alarm/event notifications. 
* 
*********************************************************************************/


#if (defined BACDEL_SER_AE_AA_B && defined BACDEL_OBJ_NC)

/* header files */
#include "serviceAcknowledgeAlarm_B.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "propertyValueRead.h"
#include "bacnetDeviceMgmtInterface.h"
#include "propertyGenricHandler.h"
#include "bacnetInitiateServiceMgmt.h"
#include "miscMiscellaneous.h"

/* include MSTP layer functions */
#include "datalinkMSTP.h"

extern DB_t SMCfg;

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is default handler when a Acknowledge Alarm request is
* received.
*    
* @param pu8ServiceRequest		[in]    Data received for decoding.
* @param i32ServiceLen			[in]    Length of service request.
* @param pstProcQInfo			[out]	Pointer to the instance of processQ & will 
*										contain the decoded parameters.
*                                   
* @return BACNET_RETURN_TYPE	[out]	success or suitable error code.
*	
*/
BACNET_RETURN_TYPE AckAlarm_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
    /* local variables */
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	ack_alarm_request_t *pstAckAlarmData = NULL;
	int32_t i32Len = 0;		
	int32_t i32DecodeLen = 0;
	uint8_t u8TagNumber = 0;
	uint32_t u32LenValue = 0;
	uint32_t u32Value = 0;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AckAlarm_B_Decode_Handler: Entry\r\n");
	#endif
   
	/* null check for input parameters */
    if(NULL == pstProcQInfo || NULL == pu8ServiceRequest)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		AckAlarm_B_Decode_Handler: NULL Input Pointers\r\n");
		#endif
        return BACDEL_ERROR;
	}

    /* Allocate buffer for read property data */
    pstAckAlarmData = (ack_alarm_request_t *)OSAL_Malloc
		( sizeof(ack_alarm_request_t),  __FILE__, __FUNCTION__, __LINE__);

    if(NULL == pstAckAlarmData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		AckAlarm_B_Decode_Handler: Malloc failed \r\n");
		#endif

		/* Set Error for this command */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY);
		return BACDEL_SUCCESS;
    }
	/* assign allocated memory to service request data */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstAckAlarmData;

	/* Must have at always 6 tags & minimum service length of 25 */
	if(i32ServiceLen < MINIMUM_SERVICE_DATA_LENGTH_AA) 
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
		AckAlarm_B_Decode_Handler : Truncated data received \r\n");
		#endif

		/* Set Error for this command */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_MISSING_REQUIRED_PARAMETER);
		return BACDEL_SUCCESS;
	}

		/* START DECODING */

	/* Extract the process identifier tag */
	i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
		&u8TagNumber, &u32LenValue);
	/* If tag is not of type process id return error */
	if(u8TagNumber != TAG_NO_0)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
		AckAlarm_B_Decode_Handler: process tag u8TagNumber = %d is not \
		valid\r\n", u8TagNumber);
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_REJECT, BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
		return BACDEL_SUCCESS;
	}
	/* Decode process Id of the notifier */
	i32Len += Decode_Unsigned(&pu8ServiceRequest[i32Len], u32LenValue,
		&pstAckAlarmData->m_u32ProcessIdentifier);

	/* Extract Event Initiating Object Identifier tag */
	i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
		&u8TagNumber, &u32LenValue);
	/* If tag is not of type Object ID return error */
	if(u8TagNumber != TAG_NO_1)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
		AckAlarm_B_Decode_Handler: object tag u8TagNumber = %d is not \
		valid\r\n", u8TagNumber);
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_REJECT, BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
		return BACDEL_SUCCESS;
	}
	/* Decode the event object Identifier */
	i32Len += Decode_Object_Id(&pu8ServiceRequest[i32Len],
		(uint32_t *)&pstAckAlarmData->m_eInitiatingObjectType,
		&pstAckAlarmData->m_u32InitiatingObjectId); 

	/* Extract Event State Acknowledged */
	i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
		&u8TagNumber, &u32LenValue);
	/* If tag is not of type Event State Acknowledged return error */
	if(u8TagNumber != TAG_NO_2)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
		AckAlarm_B_Decode_Handler: Event State tag u8TagNumber = %d is \
		not valid\r\n", u8TagNumber);
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_REJECT, BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
		return BACDEL_SUCCESS;
	}
	/* Decode Event State Acknowledged */
	i32Len += Decode_Enumerated(&pu8ServiceRequest[i32Len], u32LenValue,
		(uint32_t *)&pstAckAlarmData->m_eAcknowledgedState);

	/* Extract Time Stamp value */
	i32DecodeLen = Decode_Time_Stamp(&pstAckAlarmData->m_stTimeStamp, 
		&pu8ServiceRequest[i32Len], TAG_NO_3);
	if(i32DecodeLen < 0)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
		AckAlarm_B_Decode_Handler: time stamp decode error \r\n");
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_REJECT, BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
		return BACDEL_SUCCESS;
	}
	/* add to total decoded length */
	i32Len += i32DecodeLen;

	/* Extract acknowledgement source - message text */
	i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
			&u8TagNumber, &u32LenValue);
	/* If tag is not of type acknowledgement source return error */
	if(u8TagNumber == TAG_NO_4)
	{
		/* As per Addendum 135-2010af PR13, donot discard if character set not 
				supported or if length is more */
		/* decode even if length exceeds mac-char-string length of device */
		u32Value = u32LenValue > MAX_SUPPORTED_CHRSTR_LEN ? MAX_SUPPORTED_CHRSTR_LEN : u32LenValue;
		if(CHARACTER_IBM_MS_DBCS == pu8ServiceRequest[i32Len])
		{
			/* code page takes 2 bytes, so add 2 */
			u32Value += 2; 
		}
		/* Decode message text */
		Decode_Character_String(&pu8ServiceRequest[i32Len], u32Value,
			&pstAckAlarmData->m_AcknowledgementSource);
		/* add to total length */
		i32Len += u32LenValue;
	}

	/* Extract Time of Acknowledgement - time_stamp */
	i32DecodeLen = Decode_Time_Stamp(&pstAckAlarmData->m_stAcknowledgementTime, 
		&pu8ServiceRequest[i32Len], TAG_NO_5);
	if(i32DecodeLen < 0)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
		AckAlarm_B_Decode_Handler: time stamp decode error \r\n");
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_REJECT, BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
		return BACDEL_SUCCESS;
	}
	/* add to total decoded length */
	i32Len += i32DecodeLen;

	/* END OF DECODING */

	/* check for received & decoded data length mismatch */
	if(i32Len != i32ServiceLen)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DECODING_FAILED, "APDU: \
		AckAlarm_B_Decode_Handler: Decode Error \r\n");
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR,
			ERROR_CLASS_SERVICES, ERROR_CODE_INCONSISTENT_PARAMETERS);
		return BACDEL_SUCCESS;
	}

	/* Call Object Management Layer interface */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
    if(BACDEL_CONTINUE != eReturnType)
    {
		/* Set Error to send device process queue was full */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ABORT, 
			BACNET_ZERO, ERROR_CODE_ABORT_OUT_OF_RESOURCES);

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		AckAlarm_B_Decode_Handler: Obj_MgmtLayer_Interface returns Other \r\n");
		#endif
        return BACDEL_SUCCESS;
    }

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AckAlarm_B_Decode_Handler: Exit\r\n");
	#endif
	return eReturnType;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to update the acked_transition property if time_stamp match 
* is found in acknowledge alarm service request.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pvObject		[in]	object base address.
* @param eObjectType	[in]    type of object.
* @param eTransitionBit	[in]	transition bit to set i.e. bit no in byte
*								to be set.
*                                   
* @returns void.
*	
*/
static void AA_B_Update_Acked_Transition(
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_EVENT_TRANSITION_BITS eTransitionBit,
	virtualDevData_t *pVirtualDev,
	void *pvObject)
{
    /* local variables */
	Pr_BACnetBitStr_t stBitString = {0};
	void *pvDummy = NULL;
	uint32_t u32PropertyAdd = 0;
	PROP_ACCESS_TYPE eAccessType = NOT_SUPPORTED;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AA_B_Update_Acked_Transition: entry \r\n");
	#endif

	/* get the Base Address */
    ul32BaseAddr = (ulong32_t )pvObject;

	/* Update property offset */
	GET_INTRINSIC_OFFSET(eObjectType, m_stAckedTransitions, 
		ul32OffsetAddr);
	memcpy(&stBitString,(void *)(ul32BaseAddr+ul32OffsetAddr),
		sizeof(Pr_BACnetBitStr_t));

	/* Update property Access Type offset & get access type value */
	/* get default access type */
	eAccessType = GetDefndPropAccess(eObjectType, PROP_ACKED_TRANSITIONS, BACNET_DEFAULT);

	/* set the respective bit */
	BIT_SET(stBitString.m_stBitStr.m_u8TransBits[0], eTransitionBit);

	/* call UpdateCommonProperty function to change ack_transition */
	UpdateCommonProperty(eObjectType, 
		pvObject, 
		PROP_ACKED_TRANSITIONS, 
		-1, &stBitString, eAccessType,
		&u32PropertyAdd, &pvDummy, pVirtualDev,
		false, false, false);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AA_B_Update_Acked_Transition: exit \r\n");
	#endif
    return;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to validate received timestamp value.
*    
* @param pstAckAlarmData	[in]    decoded ack alarm request data.
* @param pvObject			[in]	object base address.
* @param eObjectType		[in]    type of object.
* @param eTransitionBit		[in]	transition bit used as index for timestamp.
*                                   
* @returns					[out]	true if match found else false.
*	
*/
static bool AA_B_Validate_Time_Stamp(
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_EVENT_TRANSITION_BITS eTransitionBit,
	ack_alarm_request_t *pstAckAlarmData,
	void *pvObject)
{
    /* local variables */
	BACnetDateTime_t stDateTime = {0};
	#ifdef STAMP_AS_SEQUENCE
	uint32_t u32SeqNo = 0;
	#endif
	int32_t i32DiffTime = 0;
	int32_t i32DiffDate = 0;
	ulong32_t ul32BaseAddr = 0;
	ulong32_t ul32OffsetAddr = 0;
	bool bMatchFlag = FALSE;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AA_B_Validate_Time_Stamp: entry \r\n");
	#endif

	/* get the Base Address */
    ul32BaseAddr = (ulong32_t )pvObject;

	/* get time stamp value of object depending upon state being acknowledged */
	/* eTransitionBit - is used as array index for time stamp */
	#if defined STAMP_AS_DATETIME

	/* Update property offset */
	GET_INTRINSIC_OFFSET(eObjectType,m_stEventTimeStamps.m_uEvntStamp
        [eTransitionBit].m_stDateTime, ul32OffsetAddr);
	/* Copy data from property offset address */
	memcpy(&stDateTime, (BACnetDateTime_t *)(ul32BaseAddr+ul32OffsetAddr),
				sizeof(BACnetDateTime_t));

	/* compare from time */
	i32DiffTime = DateTime_Compare_Time(&stDateTime.m_stTime, 
		&pstAckAlarmData->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime);
	if(0 == i32DiffTime)
	{
		/* compare from date if time matches */
		i32DiffDate = DateTime_Compare_Date(&stDateTime.m_stDate,
			&pstAckAlarmData->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stDate);
	}
	if(0 == i32DiffTime && 0 == i32DiffDate)
	{
		/* time stamp value matches */
		bMatchFlag = TRUE;
	}

	#elif defined STAMP_AS_TIME

	/* Update property offset */
	GET_INTRINSIC_OFFSET(eObjectType,m_stEventTimeStamps.m_uEvntStamp
		[eTransitionBit].m_stTime, ul32OffsetAddr);
	/* Copy data from property offset address */
	memcpy(&stDateTime.m_stTime, (BACnetTime_t *)(ul32BaseAddr+ul32OffsetAddr),
				sizeof(BACnetTime_t)); 

	/* compare from time */
	i32DiffTime = DateTime_Compare_Time(&stDateTime.m_stTime, 
		&pstAckAlarmData->m_stTimeStamp.m_stDateTime.m_stTime);
	if(0 == i32DiffTime)
	{
		/* time stamp value matches */
		bMatchFlag = TRUE;
	}

	#elif defined STAMP_AS_SEQUENCE

	/* Update property offset */
	GET_INTRINSIC_OFFSET(eObjectType,m_stEventTimeStamps.m_uEvntStamp
		[eTransitionBit].m_u32SeqNo, ul32OffsetAddr);
	/* Copy data from property offset address */
	memcpy(&u32SeqNo, (uint32_t *)(ul32BaseAddr+ul32OffsetAddr),
				sizeof(uint32_t)); 

	/* compare seq. no. */
	if(u32SeqNo == pstAckAlarmData->m_stTimeStamp.m_u32SeqNo)
	{
		/* time stamp value matches */
		bMatchFlag = TRUE;
	}

	#endif /* */

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AA_B_Validate_Time_Stamp: exit \r\n");
	#endif
	return bMatchFlag;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function sends ack_notification to all the recipients of 
* given notification class & then clears acked notification form 
* linklist.
*    
* @param pvSavedNotificationNode	[in]	pointer of saved notification in linklist.
* @param peErrCode					[out]	returns error code.
* @param peErrClass					[out]	returns error class.
* @param pVirtualDev                [in]    virtual device pointer.
* @param pstENData                  [in]    event nnotification data.
*                                   
* @return BACNET_RETURN_TYPE  		[out]	success or suitable error code..
*	
*/
static BACNET_RETURN_TYPE AA_B_Send_Ack_Notification(
	void *pvSavedNotificationNode,
	BACNET_ERROR_CODE *peErrCode,
	BACNET_ERROR_CLASS *peErrClass, 
	virtualDevData_t *pVirtualDev,
	BacnetEnPropElem_t *pstENData)
{
	/* local variables */
	BacnetEnPropElem_t *pstEnNewdata = NULL;
	bacnetip_arguments_t stServiceArgs = {0}; 
	Notificationclass_t	*pstNotificationClassObj = NULL; 
	ListOfBACnetDestination_t *pstNCRecepient = NULL; 
    BACnetDateTime_t stDateTime = {0};

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AA_B_Send_Ack_Notification: entry \r\n");
	#endif

    /* null check input pointers */
	if(NULL == pvSavedNotificationNode || NULL == pVirtualDev)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		AA_B_Send_Ack_Notification: null input pointer of InitiateQ. \r\n");
		#endif

		*peErrClass = ERROR_CLASS_SERVICES;
		*peErrCode = ERROR_CODE_INTERNAL_ERROR;
		return BACDEL_ERROR;
	}

	/* update its notify type */
	pstENData->m_eNotifyType = NOTIFY_ACK_NOTIFICATION;
	/* make ack_required to false */
	pstENData->m_bAckReq = FALSE;

	/* get the struct of 1st NC object from device obj */
	pstNotificationClassObj = pVirtualDev->m_stSupportedObjects.m_pstNotificationClass;
	while(NULL != pstNotificationClassObj)
	{
		if(pstENData->m_u32NotifyClass == 
			pstNotificationClassObj->m_stNotificationClass.m_u32Val)
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

	if(NULL == pstNCRecepient)
	{
        /* NC object not found or recipient list is empty */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_LIST_ELEMENT_NOT_FOUND, "APDU: \
		AA_B_Send_Ack_Notification: Recipient List Empty. \r\n");
		#endif

		*peErrClass = ERROR_CLASS_SERVICES;
		*peErrCode = ERROR_CODE_LIST_ELEMENT_NOT_FOUND;
		return BACDEL_ERROR;
	}

	#if (defined STAMP_AS_DATETIME || defined STAMP_AS_TIME)
    /* get current device date & time */
	GetDevice_DateTime(&stDateTime.m_stDate, &stDateTime.m_stTime,
			pVirtualDev);
	#elif defined STAMP_AS_SEQUENCE
    /* get the sequence no */
    g_u32SeqNo++;
	if(g_u32SeqNo > UINT16_MAX)
	{
		g_u32SeqNo = 1;
	}
	#endif

	/* traverse the recipient list & send event notifications */
	while(NULL != pstNCRecepient)
	{
		/* allocate memory */
		pstEnNewdata = OSAL_Malloc(sizeof(BacnetEnPropElem_t), 
			__FILE__, __FUNCTION__, __LINE__);
		if(NULL == pstEnNewdata)
		{
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
			AA_B_Send_Ack_Notification: malloc error. \r\n");
			#endif

			*peErrClass = ERROR_CLASS_RESOURCES;
			*peErrCode = ERROR_CODE_OUT_OF_MEMORY;
			return BACDEL_MALLOC_ERROR;
		}

		/* copy the data */
		memcpy(pstEnNewdata, pstENData, sizeof(BacnetEnPropElem_t));
		/* update the respective process id for the recipient */
		pstEnNewdata->m_u32ProcessId = pstNCRecepient->m_stProcessId;

        /* update the time_stamp in pstEnNewdata */
		#ifdef STAMP_AS_DATETIME
        /* date time value */
		memcpy(&pstEnNewdata->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stDate, 
			&stDateTime.m_stDate, sizeof(BACnetDate_t));
		memcpy(&pstEnNewdata->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime, 
			&stDateTime.m_stTime, sizeof(BACnetTime_t));
		#elif defined STAMP_AS_TIME
        /* time value */
		memcpy(&pstEnNewdata->m_stTimeStamp.m_stTimeStamp.m_stTime, 
			&stDateTime.m_stTime, sizeof(BACnetTime_t));
		#elif defined STAMP_AS_SEQUENCE
        /* sequence no */
		memcpy(&pstEnNewdata->m_stTimeStamp.m_stTimeStamp.m_u32SeqNo, 
			&g_u32SeqNo, sizeof(uint32_t));
		#endif

		/* send the notification */
		Send_Notification(&stServiceArgs, pVirtualDev, pstNCRecepient, pstEnNewdata);

		/* move to next recipient */
		pstNCRecepient = pstNCRecepient->m_pstNext;
	}//while ends.

    /* commented to fix bug id : 2902, for re_acknowledgement */
    /* test case : 135.1-2009i-17 - 9.1.1.X1 */

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AA_B_Send_Ack_Notification: exit \r\n");
	#endif
	return BACDEL_SUCCESS;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function finds the previously saved notification data in 
* saved notifications linklist.
* If the data in ack_alarm request & event_notification matches, then 
* the notification is again sent to all recipients with notify type equal 
* to ack_notification and without ack_required, from_state & event_values.
*    
* @param pstAckAlarmData	[in]    decoded ack alarm request data.
* @param peErrCode			[out]	returns error code.
* @param peErrClass			[out]	returns error class.
* @param pstdata			[out]	returns saved event notification data.
* @param u32DevID           [in]    device identfier
*                                   
* @return					[out]	savedNotificationNode instance from linklist on success.
*	
*/
static void * AA_B_Find_AckAlarm_Match(
	ack_alarm_request_t *pstAckAlarmData,
	BACNET_ERROR_CODE *peErrCode,
	BACNET_ERROR_CLASS *peErrClass,
	BacnetEnPropElem_t *pstdata,
	uint32_t u32DevID)
{
	/* local variables */
	BacnetEnPropElem_t *pstENdata = NULL;
	BacnetEnPropElem_t *pstTemp = NULL;
	bool bToStateIsValid = TRUE;
	SavedEventNotifications_t *pstLocalNode = NULL;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AA_B_Find_AckAlarm_Match: entry \r\n");
	#endif

	/* Wait till the mutex is released */
	Osal_Wait_Mutex(g_stSavedEventNotifications.hMtxSavedNotification, INFINITE); 

	/* get 1st node of linklist */
	pstLocalNode = g_stSavedEventNotifications.pstRootNode;

	if(NULL == pstLocalNode)
	{
		*peErrClass = ERROR_CLASS_SERVICES;
		*peErrCode = ERROR_CODE_OTHER;

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OTHER, "APDU: \
		AA_B_Find_AckAlarm_Match: no saved notifications in link list. \r\n");
		#endif

		/* release the mutex */
		Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);
		return NULL;
	}

	/* traverse the linklist */
	while(pstLocalNode != NULL)
	{
		/* get notification data pointer */
		pstENdata = &pstLocalNode->m_stENdata;
		/* now check process id & event initiating object id & type */
		if((pstENdata->m_eObjectType == pstAckAlarmData->m_eInitiatingObjectType) &&
			(pstENdata->m_u32ObjId == pstAckAlarmData->m_u32InitiatingObjectId) &&
           (pstENdata->m_u32DeviceId == u32DevID))
		{
			/* reset bToStateIsValid to true */
			bToStateIsValid = TRUE;
			/* saved notification data found */
			/* switch to ToState */
			switch(pstENdata->m_eToState)
			{
			case EVENT_STATE_NORMAL:
				if(pstAckAlarmData->m_eAcknowledgedState != EVENT_STATE_NORMAL)
					bToStateIsValid = FALSE;
				break;
			case EVENT_STATE_FAULT:
				if(pstAckAlarmData->m_eAcknowledgedState != EVENT_STATE_FAULT)
					bToStateIsValid = FALSE;
				break;
			case EVENT_STATE_OFFNORMAL:
				if(pstAckAlarmData->m_eAcknowledgedState != EVENT_STATE_OFFNORMAL)
					bToStateIsValid = FALSE;
				break;
			case EVENT_STATE_HIGH_LIMIT:
				if(pstAckAlarmData->m_eAcknowledgedState == EVENT_STATE_OFFNORMAL ||
				   pstAckAlarmData->m_eAcknowledgedState == EVENT_STATE_HIGH_LIMIT)
				{
					; /* do nothing */
				}
				else
					bToStateIsValid = FALSE;
				break;
			case EVENT_STATE_LOW_LIMIT:
				if(pstAckAlarmData->m_eAcknowledgedState == EVENT_STATE_OFFNORMAL ||
				   pstAckAlarmData->m_eAcknowledgedState == EVENT_STATE_LOW_LIMIT)
				{
					; /* do nothing */
				}
				else
					bToStateIsValid = FALSE;
				break;
			case EVENT_STATE_LIFE_SAFETY_ALARM:
				if(pstAckAlarmData->m_eAcknowledgedState == EVENT_STATE_OFFNORMAL ||
				   pstAckAlarmData->m_eAcknowledgedState == EVENT_STATE_LIFE_SAFETY_ALARM)
				{
					; /* do nothing */
				}
				else
					bToStateIsValid = FALSE;
				break;
			default:
				bToStateIsValid = FALSE;
				break;
			}//switch ends
			if(!bToStateIsValid)
				/* save the pointer */
				pstTemp = pstENdata;
			else
			{
				#if(defined DEBUG_PRINTF && DL_2)
				Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
				AA_B_Find_AckAlarm_Match: found saved notification \r\n");
				#endif

				memcpy(pstdata, pstENdata, sizeof(BacnetEnPropElem_t));
				/* release the mutex */
				Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);
				return (void *)pstLocalNode;
			}
		}
		/* move to next node */
		pstLocalNode = pstLocalNode->m_pstNext;
	}

    /* object with active event state found, but received invalid event state */
	if(!bToStateIsValid && NULL != pstTemp)
	{
		*peErrClass = ERROR_CLASS_SERVICES;
		*peErrCode = ERROR_CODE_INVALID_EVENT_STATE;

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_PARAMETER_VALUE, "APDU: \
		AA_B_Find_AckAlarm_Match: invalid to state received. \r\n");
		#endif

		/* release the mutex */
		Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);
		return NULL;
	}

	/* set error code as no match found in list for specified object */
	*peErrClass = ERROR_CLASS_OBJECT;
	//*peErrCode = ERROR_CODE_OTHER;
	*peErrCode = ERROR_CODE_NO_ALARM_CONFIGURED;

	/* release the mutex */
	Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AA_B_Find_AckAlarm_Match: exit \r\n");
	#endif
	return NULL;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is to validate received acknowledge alarm request & do 
* necessary actions after successful validation.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pstProcQInfo	[in]	Pointer to service data.
* @param u8ThreadNo     [in]    thread number
*                                   
* @return				[out]	true or false.
*	
*/
bool AckAlarm_B_Request_Parser(
	virtualDevData_t *pVirtualDev, 
	processInfo_t *pstProcQInfo, 
	uint8_t u8ThreadNo)
{
    /* local variables */
	ack_alarm_request_t *pstAckAlarmData = NULL;
	BacnetEnPropElem_t stENdata = {0};
	bool bMatchFlag = FALSE;
	BACNET_OBJECT_TYPE eObjectType = MAX_BACNET_OBJECT_TYPE;
	BACNET_EVENT_TRANSITION_BITS eTransitionBit = 0;	
	BACNET_RETURN_TYPE eReturnType = BACDEL_ERROR;
	uint32_t u32ErrCode = 0;
	BACNET_ERROR_CLASS eErrClass = MAX_BACNET_ERROR_CLASS;
	void *pvSavedNotification = NULL;		
	void *pvObject = NULL;	
	#ifdef SUPPORT_MULTIPLE_DEVICE
	Post_Thread_Msg_t stThreadMsg = {0};
	#endif
	
    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AckAlarm_B_Request_Parser: entry \r\n");
	#endif

	/* null check for input parameters */
	if(NULL == pstProcQInfo || NULL == pVirtualDev)
	{
		/* This should not occur ideally */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		AckAlarm_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* Default vaue for AA-B response */
	pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_SIMPLE_ACK;

    /* get the service request data */
	pstAckAlarmData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	if(NULL == pstAckAlarmData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		AckAlarm_B_Request_Parser: Null service data pointer \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_RESOURCES, ERROR_CODE_OTHER);
		return TRUE;
	}

	/* get type of object */
	eObjectType = pstAckAlarmData->m_eInitiatingObjectType;
	pvObject = Find_Object(eObjectType, pstAckAlarmData->m_u32InitiatingObjectId, 
		pVirtualDev);
	if(NULL == pvObject)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_NOT_PRESENT, "APDU: \
		AckAlarm_B_Request_Parser: Null input pointers \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_OBJECT, ERROR_CODE_UNKNOWN_OBJECT);
		return TRUE;
	}

	/* get the type of transition from event state being acknowledged */
	if(EVENT_STATE_NORMAL == pstAckAlarmData->m_eAcknowledgedState)
		eTransitionBit = TRANSITION_TO_NORMAL;
	else if(EVENT_STATE_FAULT == pstAckAlarmData->m_eAcknowledgedState)
		eTransitionBit = TRANSITION_TO_FAULT;
	else if(EVENT_STATE_OFFNORMAL == pstAckAlarmData->m_eAcknowledgedState ||
		EVENT_STATE_LOW_LIMIT == pstAckAlarmData->m_eAcknowledgedState ||
		EVENT_STATE_HIGH_LIMIT == pstAckAlarmData->m_eAcknowledgedState ||
		EVENT_STATE_LIFE_SAFETY_ALARM == pstAckAlarmData->m_eAcknowledgedState)
		eTransitionBit = TRANSITION_TO_OFFNORMAL;
	else 
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_PARAMETER_VALUE, "APDU: \
		AckAlarm_B_Request_Parser: Invalid event state received \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_EVENT_STATE);
		return TRUE; 
	}

    /* find the saved notification for this event */
	pvSavedNotification = AA_B_Find_AckAlarm_Match(pstAckAlarmData, (BACNET_ERROR_CODE *)&u32ErrCode, 
		&eErrClass, &stENdata, pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId);
	if(NULL == pvSavedNotification)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INCONSISTENT_PARAMETERS, "APDU: \
		AckAlarm_B_Request_Parser: AA_B_Find_AckAlarm_Match returns null with \
		error code = %d \r\n", u32ErrCode);
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_ERROR, eErrClass, u32ErrCode);
		return TRUE; 
	}

	/* check if received time_stamp is valid */
	bMatchFlag = AA_B_Validate_Time_Stamp(eObjectType, eTransitionBit, 
			pstAckAlarmData, pvObject);

	if(TRUE != bMatchFlag)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_PARAMETER_VALUE, "APDU: \
		AckAlarm_B_Request_Parser: Time_stamp match not found \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_TIME_STAMP);
		return TRUE;
	}

	/* time stamp value matches, so update ack_transition property & 
		send the saved notification by changing notify type to ack_notification 
		& ack_required to false */
	/* send simple ack 1st & then the ack notifications */
	/* make state of processQ location to done */
	pstProcQInfo->m_eState = PROC_DONE;

	/* set the flag to notify master tsm to stop reply postpone */
	b_ResponseSentOnNetwork = TRUE;

	#ifndef SUPPORT_MULTIPLE_DEVICE
    /* Increment the semaphore of Tx thread to send simple ack */
    if (!OSAL_Release_Sem(g_hTxSemaphoreHandle, BACNET_ONE))
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_SEMAPHORE_ERROR, "APDU: \
		AckAlarm_B_Request_Parser: OSAL_Release_Sem Failed \r\n");
		#endif

        /* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_ERROR, ERROR_CLASS_DEVICE, ERROR_CODE_OPERATIONAL_PROBLEM);
		return TRUE;
    }

	#else
	/* Post the message to the message queue */
	stThreadMsg.idThread = gstHostDevice.m_pstDeviceStruct->m_dwTransmitThreadID;
	stThreadMsg.MsgType = MSGQ_TYPE;
	stThreadMsg.wParam = (WPARAM)0;
	stThreadMsg.lParam = (LPARAM)pstProcQInfo;
	if(!OSAL_Post_Message(&stThreadMsg))
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MESSAGE_QUEUE_ERROR, 
		"APDU: AckAlarm_B_Request_Parser: message queue post error \r\n");
		#endif
		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_ERROR, ERROR_CLASS_DEVICE, ERROR_CODE_OPERATIONAL_PROBLEM);
		return TRUE;
	}
	#endif /* */

	/* NOTE */
	/* make return value false, to avoid triggering Tx thread to send simple ack
	   as this is already sent in AA_B_Send_Ack_Notification() */

	/* set the bit corresponding to the state being acknowledged in
		acked_transition property */
	AA_B_Update_Acked_Transition(eObjectType, eTransitionBit, pVirtualDev, pvObject);	

	/* send ack_notification, updates its notify type to ACK_NOTIFICATION
		& ack_required to FALSE and resend with same pdu type */
	eReturnType = AA_B_Send_Ack_Notification(pvSavedNotification, (BACNET_ERROR_CODE *)&u32ErrCode, 
		&eErrClass, pVirtualDev, &stENdata);
	if(BACDEL_SUCCESS != eReturnType)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		AckAlarm_B_Request_Parser: AA_B_Send_Ack_Notification returned \
		error code = %d\r\n", u32ErrCode);
		#endif

		/*Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			eErrClass, u32ErrCode);
		return TRUE;*/
		/* do not send any ack, just return false & do not update 
			acked transition property */
		return FALSE;
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AckAlarm_B_Request_Parser: exit \r\n");
	#endif
	return FALSE;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Api to clear saved notification when it is either acknowledged or the 
* device to which this notification belongs is deleted.
*    
* @param u32DevID			[in] device id.
* @param bIsDeleteDevice	[in] True if api is called from delete device api. 
* @param pvInstance			[in] pointer to be deleted from the list.
*                                   
* @returns Void.
*	
*/
void AA_B_Clear_Saved_Event_Notification(
	uint32_t u32DevID, 
	bool bIsDeleteDevice,
	void *pvInstance)
{
    /* local varaibles */
    SavedEventNotifications_t *pstLocalNode = NULL;
	SavedEventNotifications_t *pstNode = NULL;
	SavedEventNotifications_t *pstPrevNode = NULL;
	SavedEventNotifications_t *pstNextNode = NULL;
    bool bNodeDeleted = FALSE;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	AA_B_Clear_Saved_Event_Notification: entry \r\n");
	#endif

    /* Wait till the mutex is released */
	Osal_Wait_Mutex(g_stSavedEventNotifications.hMtxSavedNotification, INFINITE); 

    /* get 1st node of linklist */
	pstLocalNode = g_stSavedEventNotifications.pstRootNode;

    if(bIsDeleteDevice)
    {
        /* this is call from delete device api, so delete all notifications
            that belong to the "u32DevID" */
        while(NULL != pstLocalNode)
        {
            /* save next node */
		    pstNextNode = pstLocalNode->m_pstNext;
            /* check if node is to be deleted */
            if(u32DevID == pstLocalNode->m_stENdata.m_u32DeviceId)
            {
                if(NULL == pstPrevNode && NULL == pstNextNode)
	            {
		            /* this is the only node in linklist */
		            g_stSavedEventNotifications.pstRootNode = NULL;
		            g_stSavedEventNotifications.pstLastNode = NULL;
	            }
	            else if(NULL == pstPrevNode)
	            {
		            /* this is 1st node of list */
		            g_stSavedEventNotifications.pstRootNode = pstNextNode;
	            }
	            else if(NULL == pstNextNode)
	            {
		            /* this is last node of list */
		            pstPrevNode->m_pstNext = NULL;
		            g_stSavedEventNotifications.pstLastNode = pstPrevNode;
	            }
	            else
	            {
		            /* middle node */
		            pstPrevNode->m_pstNext = pstNextNode;
	            }
	            /* free the node */
	            OSAL_Free(pstLocalNode, __FILE__, __FUNCTION__, __LINE__);
                bNodeDeleted = TRUE;
            }
            if(bNodeDeleted)
                bNodeDeleted = FALSE;
            else
                /* update previous node */
		        pstPrevNode = pstLocalNode;
		    /* move to next node */
		    pstLocalNode = pstNextNode;
        }//while ends.
    }//if ends.
    else
    {
        /* delete the notification with given pointer "pvInstance" */
        /* get address of node to be deleted */
        pstNode = pvInstance;
        if(NULL == pstNode)
		{
			/* release the mutex */
			Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);
            return;
		}
        /* traversing the list */
	    while(NULL != pstLocalNode)
	    {
		    /* save next node */
		    pstNextNode = pstLocalNode->m_pstNext;
		    if(pstNode == pstLocalNode)
			    break;
		    /* update previous node */
		    pstPrevNode = pstLocalNode;
		    /* move to next node */
		    pstLocalNode = pstNextNode;
	    }
	    if(NULL == pstPrevNode && NULL == pstNextNode)
	    {
		    /* this is the only node in linklist */
		    g_stSavedEventNotifications.pstRootNode = NULL;
		    g_stSavedEventNotifications.pstLastNode = NULL;
	    }
	    else if(NULL == pstPrevNode)
	    {
		    /* this is 1st node of list */
		    g_stSavedEventNotifications.pstRootNode = pstNextNode;
	    }
	    else if(NULL == pstNextNode)
	    {
		    /* this is last node of list */
		    pstPrevNode->m_pstNext = NULL;
		    g_stSavedEventNotifications.pstLastNode = pstPrevNode;
	    }
	    else
	    {
		    /* middle node */
		    pstPrevNode->m_pstNext = pstNextNode;
	    }
	    /* free the node */
	    OSAL_Free(pstLocalNode, __FILE__, __FUNCTION__, __LINE__);
    }//else ends.
	
	/* release the mutex */
	Osal_Release_Mutex(g_stSavedEventNotifications.hMtxSavedNotification);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	AA_B_Clear_Saved_Event_Notification: exit \r\n");
	#endif
    return;
}

#endif /* BACDEL_SER_AE_AA_B */
