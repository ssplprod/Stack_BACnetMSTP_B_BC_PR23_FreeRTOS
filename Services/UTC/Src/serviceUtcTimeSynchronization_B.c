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
*   SoftDEL Systems Ltd.                             india@softdel.com      
*   3rd Floor, Pentagon P4,                          http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*   serviceUtcTimeSynchronization_B.c
*                                                                      
*   AUTHORS                                                                     
*	Pratham M., Sarvesh J.    
*                                                                         
*   DESCRIPTION                                                            
*   The B device interprets Utc Time Synchronization messages from the A 
*	device & shall update its local representation of time and date by 
*	subtracting the value of the 'UTC_Offset' property of the Device object 
*	from the 'Time' parameter and taking the 'Daylight_Savings_Status' 
*	property of the Device object into account as appropriate to the 
*	locality. 
*	This change shall be reflected in the Local_Time and Local_Date properties 
*	of the Device object.
*                                                                         
*    
******************************************************************************/

#ifdef BACDEL_SER_DM_UTC_B

/* header files */
#include "serviceUtcTimeSynchronization_B.h"
#include "bacDELDef.h"
#include "osalFreeRTOS.h"
#include "miscMiscellaneous.h"
#include "pduDateTime.h"

/* to include scheduling functionality */
#ifdef BACDEL_OBJ_SDL
#include "serviceScheduling_B.h"
#endif

/**
*                                                                    
* DESCRIPTION                                                                          
* This function updates the local_date & local_time properties
* of the device object depending upon UTC offset & Day light saving
* properties.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pstProcQInfo	[in]	Pointer to the instance of processQ that 
*								contains the decoded parameters.
* @param u8ThreadNo		[in]    thread number
*                                   
* @return	[out]	false as this is unconfirmed service.
*	
*/
bool UtcTimeSync_B_Request_Parser(
	virtualDevData_t *pVirtualDev, 
	processInfo_t *pstProcQInfo,
	uint8_t u8ThreadNo)
{
	/* local variables */
	utc_time_sync_request_t *pstTimeSyncData = NULL;
	utc_time_sync_request_t *pstUTCTimeSyncReqData = NULL;
	uint32_t u32ErrCode = 0;
	BACNET_ERROR_CLASS eErrClass = ERROR_CLASS_DEVICE;
	BACNET_PDU_TYPE ePduType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
	BACNET_RETURN_TYPE eRetType = BACDEL_SUCCESS;
	BACnetAddress_t stRmtDvAddr = {0};
	utc_time_sync_request_t stUtcDateTime = {0};
	utc_data_t stUtcData = {0};
	uint32_t u32CallbackId = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	UtcTimeSync_B_Request_Parser: entry \r\n");
	#endif

	/* null check for input pointers */
	if(NULL == pstProcQInfo || NULL == pVirtualDev)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		UtcTimeSync_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* get service request data */
	pstTimeSyncData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
	if(NULL == pstTimeSyncData)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		UtcTimeSync_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* validate the received date & time values */
	if(!Validate_DateTime(&pstTimeSyncData->m_stDate, &pstTimeSyncData->m_stTime))
	{
		/* free allocated servcie data */
		OSAL_Free(pstTimeSyncData,__FILE__,__FUNCTION__,__LINE__);		
		return FALSE;
	}
	/* copy date & time */
	memcpy(&stUtcDateTime.m_stDate, &pstTimeSyncData->m_stDate, sizeof(BACnetDate_t));
	memcpy(&stUtcDateTime.m_stTime, &pstTimeSyncData->m_stTime, sizeof(BACnetTime_t));

	/* get utc offset property value */
	stUtcData.m_i32UtcOffset = pVirtualDev->m_stDevObject.m_stUTCOffset.m_i32Val;

	/* get the day light saving status */
	stUtcData.m_bDayLightStatus = pVirtualDev->m_stDevObject.m_stDaylightSavingStatus.m_bVal;

	#if (!STACK_CUSTOMIZE_UTC_TIMESYNC_B || CALLBACK_BEFORE_EXECUTION != CALLBACK_CONFIG_UTC_TIME_SYNC)
	/* adjust UTC offset */
	if(stUtcData.m_i32UtcOffset)
	{
		/* subtract the utc offset from received service data */
		DateTime_Subtract_UTC_Offset(&stUtcDateTime, stUtcData.m_i32UtcOffset);
	}
	/* adjust day light saving status */
    if(stUtcData.m_bDayLightStatus)
	{
		/* day light saving is on so add daylight saving hrs or mins to time */
		/* 1hr is 60 mins */
        DateTime_Add_Minutes(&stUtcDateTime, DAY_LIGHT_SAVING_MINS);
	}
	#endif 

	#if (CALLBACK_BEFORE_EXECUTION == CALLBACK_CONFIG_UTC_TIME_SYNC || \
		CALLBACK_AFTER_EXECUTION == CALLBACK_CONFIG_UTC_TIME_SYNC)

	/* Allocate buffer for service data */
	pstUTCTimeSyncReqData =  (utc_time_sync_request_t *)OSAL_Malloc(
		sizeof(utc_time_sync_request_t),  __FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstUTCTimeSyncReqData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		UtcTimeSync_B_Request_Parser : Malloc failed \r\n");
		#endif
		/* free allocated servcie data */
		OSAL_Free(pstTimeSyncData, __FILE__,__FUNCTION__,__LINE__);
		return FALSE;
    }
	/* copy data to request structure for callback */
	memcpy(pstUTCTimeSyncReqData, &stUtcDateTime, sizeof(utc_time_sync_request_t));
	
	/* copy address */
	memcpy(&stRmtDvAddr, &pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
		sizeof(BACnetAddress_t));	
	#endif

	#if (CALLBACK_BEFORE_EXECUTION == CALLBACK_CONFIG_UTC_TIME_SYNC)
	/* If callback function is registered, then pass the data. */
	if(NULL == afpApplicationFunctionCb[APP_CB_UTC_TIME_SYNC])
	{
		#ifdef DEBUG_PRINT
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		UtcTimeSync_B_Request_Parser: Callback not registered \r\n");
		#endif
		/* free allocated servcie data */
		OSAL_Free(pstTimeSyncData, __FILE__,__FUNCTION__,__LINE__);
		OSAL_Free(pstUTCTimeSyncReqData, __FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	}

	/* generate a new callback id */
	u32CallbackId = Generate_Callback_ID();

	/* Inform to the Application(Caller) about time sync request */
	eRetType = afpApplicationFunctionCb[APP_CB_UTC_TIME_SYNC](
		pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId, /* device id */
		u32CallbackId,
		SERVICE_SUPPORTED_UTC_TIME_SYNCHRONIZATION,
		&eErrClass,
		&u32ErrCode,
		&ePduType,
		pstUTCTimeSyncReqData, /* request data */
		NULL,
		&stUtcData, /* utc offset & day light saving status */
		&stRmtDvAddr,
		(BACnetDateTime_t*)pstUTCTimeSyncReqData);

	if(BACDEL_SUCCESS == eRetType)
	{
		/* free allocated servcie data */
		OSAL_Free(pstUTCTimeSyncReqData,__FILE__,__FUNCTION__,__LINE__);
	}
	else
	{
		/* free allocated servcie data */
		OSAL_Free(pstTimeSyncData,__FILE__,__FUNCTION__,__LINE__);
		OSAL_Free(pstUTCTimeSyncReqData,__FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	}
	#endif /* CALLBACK_BEFORE_EXECUTION */
	
	/* update device local date & local time properties */
	Update_Dv_Local_DateTime_Properties(pVirtualDev, 
		&stUtcDateTime.m_stTime,
		&stUtcDateTime.m_stDate,
		false);

	/* trigger calendar & schedule object for scheduling service */
	#ifdef BACDEL_OBJ_CAL
	Trigger_Calendar_Object(pVirtualDev);
	#endif
	#ifdef BACDEL_OBJ_SDL
	Trigger_Schedule_Object(pVirtualDev);
	#endif
	
	/* Since we don't have to return any value, we free the process Queue
       location, free the service request data & return false */
    //Exit_Routine_To_Error_Of_Unconfirmed_Request(pstProcQInfo);
	OSAL_Free(pstTimeSyncData,__FILE__,__FUNCTION__,__LINE__);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	UtcTimeSync_B_Request_Parser: exit \r\n");
	#endif
	return FALSE;
}

#endif /* BACDEL_SER_DM_UTC_B */
