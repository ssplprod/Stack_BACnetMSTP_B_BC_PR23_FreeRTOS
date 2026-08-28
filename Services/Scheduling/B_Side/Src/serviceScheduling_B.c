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
*   3rd Floor, Pentagon P4,                                http://www.softdel.com
*    Magarpatta City, Hadapsar
*    Pune - 411 028
*
******************************************************************************/
/**
*********************************************************************************
*
*   FILE
*      ServiceSchedule.c
*
*   AUTHORS
*
*
*   DESCRIPTION
*        schedule service function.
*
*   RELEASE HISTORY
*        DATE            NAME                DESCRIPTION
*        11/09/2012      Heramb Joshi        File Created.
*
*
*********************************************************************************/

#ifdef BACDEL_OBJ_SDL

#include "serviceScheduling_B.h"
#include "PropertyParsingAndInitialization.h"
#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"
#include "propertyValueRead.h"
#include "pduDateTime.h"
#include "propertyValueWrite.h"
#include "propertyValidations.h"
#include "bacnetInitiateServiceMgmt.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "propertyClearValues.h"


/**
*
*DESCRIPTION
*   This fun Gets Present Vaule of schedule object
*   evaluate the current value ofthe Weekly_Schedule array element for the current day.
*   Schedule Internal and External
*@param ul32BaseAddr -  Base address of schedule object
*
*
*/
void Get_PresentValue_ForSchedule(ulong32_t ul32BaseAddr, void *pInData, int8_t i8ArrayIndex,
	void *pVirtualDev, bool bChangeReliability)
{
    Pr_BACnetDateRange_t         stEffectivePeriod = {0};
    Pr_ListOfBACnetDailySchedule_t   *pstWeeklySchedule = {0};
    BACnetDate_t            stSyetemDate = {0};
    BACnetTime_t            stSyetemTime = {0};
    AnyValue_t              *pstSHDLPresentValue = NULL;
    bool                    bOutofservice = 0;
    ulong32_t               ul32OffsetAddr = 0;
    PROP_ACCESS_TYPE        ePermission = BACNET_DEFAULT;
    void                    *pvDummy = NULL;
    uint32_t                PropertyPtr = 0;
    bacnetip_arguments_t    stServiceArgs = {0};
    uint32_t                i32Priority = 0 ;
    ListOfBACnetDevObjPropRef_t            *pstListOfBACnetDevObjPropReff = NULL;
    Pr_ListOfBACnetDevObjPropRef_t 	    *pstListOfObjectPropertyReference = NULL;
    BACnetDevObjPropRef_t   *pstDevObjPropRef = NULL;
    Pr_ListOfBACnetSpecialEvent_t          *pstExceptionSchedule = NULL;
    AnyValue_t                      *pstScheduleDefault = NULL;
    int32_t         i32ErrorStatus  =    RELIABILITY_NO_FAULT_DETECTED;
    TriggerTimeNPriority_t* pstTriggerTime = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Get_PresentValue_ForSchedule: entry \r\n");
	#endif

    /* Get Present Value offset */
    GET_SHDL_OFFSET(OBJECT_SCHEDULE, m_stPresentValue, ul32OffsetAddr);
    pstSHDLPresentValue =(void *)(ul32BaseAddr+ul32OffsetAddr);

    /* get address of stListOfBACnetDevObjPropReff prop of schedule object */
    GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stListOfObjectPropertyReference,ul32OffsetAddr);
    pstListOfObjectPropertyReference = (void *)(ul32BaseAddr+ul32OffsetAddr);

    /* is present value writable */
    bOutofservice = IsOutofSerivceFlagTrue(ul32BaseAddr, OBJECT_SCHEDULE);

    /* Get Effective period of Schedule Object */
    GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stEffectivePeriod,ul32OffsetAddr);
    memcpy(&(stEffectivePeriod),(void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(Pr_BACnetDateRange_t));

    /* Get current day */
    GetDevice_DateTime(&stSyetemDate, &stSyetemTime, pVirtualDev);

    /* Set Trigger time to next day 00:00:00.0 */
    /* Update Trigger Time for Schedule Object */
    GET_SHDL_OFFSET(OBJECT_SCHEDULE, m_stTimeNPriority, ul32OffsetAddr);
    pstTriggerTime = (TriggerTimeNPriority_t *)(ul32BaseAddr+ul32OffsetAddr);

    memset(&pstTriggerTime->m_stTriggerTime, 0, sizeof(BACnetTime_t));

    pstTriggerTime->m_stTriggerDate = stSyetemDate;
    DateTime_Add_Days(1, &pstTriggerTime->m_stTriggerDate);

    /* Get current Time, Check If current day is within Effective period */
    if(Is_Active_DateRange(&stEffectivePeriod.m_stDateRange, &stSyetemDate))
    {
        /*Present_Value property is decoupled from the internal calculations when Out_Of_Service is TRUE*/
        if(TRUE == bOutofservice && (((BACNET_PROPERTY_VALUE*)pInData)->m_ContextSpecific == 0))
        {
            memcpy(&(pstSHDLPresentValue->m_stValue), (BACNET_PROPERTY_VALUE *)pInData,
				sizeof(BACNET_PROPERTY_VALUE));
            i32ErrorStatus  =    RELIABILITY_NO_FAULT_DETECTED;
        }
        else
        {
            /* Calculate Schedule Present Value for the current day */

            GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stExceptionSchedule,ul32OffsetAddr);
            pstExceptionSchedule = (Pr_ListOfBACnetSpecialEvent_t *)(ul32BaseAddr+ul32OffsetAddr);

            /* get weekly schedule offset */
            GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stWeeklySchedule,ul32OffsetAddr);
            pstWeeklySchedule = (Pr_ListOfBACnetDailySchedule_t *)(ul32BaseAddr+ul32OffsetAddr);

            /* get Schedule Default Offset */
            GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stScheduleDefault,ul32OffsetAddr);
            pstScheduleDefault = (AnyValue_t *)(ul32BaseAddr+ul32OffsetAddr);

            if(IS_ConfigurationError(pstListOfObjectPropertyReference->m_pstListOfBACnetDevObjPropReff,
                                        pstExceptionSchedule->m_pstSplEvent,  pstWeeklySchedule,
                                        pstScheduleDefault,  pVirtualDev))
            {
                /* Get present value from Exception Schedule*/
                if(!SetSDLPresentValExcp(pstSHDLPresentValue, pstExceptionSchedule->m_pstSplEvent, ul32BaseAddr, pVirtualDev))
                {
                    /* Get present value from stWeeklySchedule*/
                    if(pstWeeklySchedule->m_stTimeValue[stSyetemDate.m_u8Wday -1].bIsUsed)
                    {
                        SetSDLPresentValWeekly(pstSHDLPresentValue, &(pstWeeklySchedule->m_stTimeValue[stSyetemDate.m_u8Wday -1]),
                                                ul32BaseAddr, pVirtualDev);
                    }
                    else
                    {
                        /* Assign prop default value to Present value of object*/
                        memcpy(&(pstSHDLPresentValue->m_stValue), &(pstScheduleDefault->m_stValue),
                            sizeof(BACNET_PROPERTY_VALUE));
                    }
                }
            }
            else
            {
               /* set relibility error code "configuration error" */
                i32ErrorStatus = RELIABILITY_CONFIGURATION_ERROR;
            }

			/* application has returned bacdel other in callback,
			this means validation failed in application.
			though simple ack will be sent reliability needs to be
			put in fault */
			if(TRUE == bChangeReliability)
			{
				/* set relibility error code "configuration error" */
                i32ErrorStatus = RELIABILITY_CONFIGURATION_ERROR;
			}

            ePermission = GetDefndPropAccess(OBJECT_SCHEDULE, PROP_RELIABILITY,ePermission);

            ePermission = Validate_PropertySupport(OBJECT_SCHEDULE, PROP_RELIABILITY,
                                                    ePermission);

			UpdateCommonProperty(OBJECT_SCHEDULE, (void*)ul32BaseAddr,
				PROP_RELIABILITY , -1, &i32ErrorStatus, ePermission,
				&PropertyPtr, &pvDummy, pVirtualDev,false,false,false);
        }

        if(i32ErrorStatus == RELIABILITY_NO_FAULT_DETECTED)
        {
            pstListOfBACnetDevObjPropReff = pstListOfObjectPropertyReference->m_pstListOfBACnetDevObjPropReff;

            /* Get Priority for writing */
            GET_PRIORITYFORWRT_OFFSET(OBJECT_SCHEDULE, m_stPriorityForWriting.m_u32Val, ul32OffsetAddr);
            memcpy(&i32Priority, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint32_t));

            /*  write Present_Value to all members of the List_Of_Object_Property_References property.*/
            while(NULL != pstListOfBACnetDevObjPropReff)
            {
                pstDevObjPropRef = &(pstListOfBACnetDevObjPropReff->m_stDevObjPropRef);

                /*Scheduling-External-B*/
                if(((virtualDevData_t*)pVirtualDev)->m_stDevObject.m_stObjectID.m_u32ObjId !=
					pstDevObjPropRef->m_u32DeviceInstace && pstDevObjPropRef->m_eDeviceType != 0)
                {
                    /*write Property on other Device*/
                    if(SchedulingExternal(pstDevObjPropRef,
                        &stServiceArgs,
                        pstSHDLPresentValue,
                        i32Priority,
                        pVirtualDev))
                    {
                        /* Call A side Interface*/
                        /* FIXME--- ADD DADR*/
						BACDEL_Generate_Aside_Request( ((virtualDevData_t*)pVirtualDev)->m_u8DvSADR ,
                            &stServiceArgs, NULL);
                    }
                }
                /* Scheduling-Internal-B*/
                else
                {
                    SchedulingInternal(pstDevObjPropRef, pVirtualDev, pstSHDLPresentValue, i32Priority);

                } /* Scheduling-Internal-B*/

                pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;
            } /* End of While */
        }
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Get_PresentValue_ForSchedule: exit \r\n");
	#endif
} /* Get_PresentValue_ForSchedule */


/**
*
*DESCRIPTION
*evaluate the current value ofthe Weekly_Schedule array element for the current day and current time
*
*@param pstSHDLPresentValue -  Pointer to Present Value of schedule object.
*@paramp pstTimeValue - TimeValue structure of WeeklySchedule .
*
*/
void SetSDLPresentValWeekly(AnyValue_t *pstSHDLPresentValue,
                             BACnetTimeValue_t  *pstTimeValue,
                             ulong32_t          ul32BaseAddr,
							 void *pVirtualDev)
{
    BACnetTime_t        stLocalTime = {0};
    BACnetTime_t        stTempTime = {0};
    BACnetTime_t        stNullTime = {0};
    BACnetTime_t        stTriggeredTime = {0};
    BACnetDate_t        stDate = {0};
    BACnetDate_t        stTempDate = {0};
    AnyValue_t  stScheduleDefault = {0};
    bool                bIsNullVal  = false;
    ulong32_t           ul32OffsetAddr = 0;
    BACnetTimeValue_t   *pstTempTimeValue = NULL;
    TriggerTimeNPriority_t *pstTriggerTime = {0};
    BACnetTimeValue_t   *pstHeadTimeValue = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	SetSDLPresentValWeekly: entry \r\n");
	#endif

    GetDevice_DateTime(&stDate, &(stLocalTime), pVirtualDev);
    stTempDate = stDate;

    stTriggeredTime = GetNextTriggerTimeWeekly(pstTimeValue, stLocalTime);

    if(stTriggeredTime.m_u8Hour == 25)
    {
        memset(&stTriggeredTime, 0, sizeof(BACnetTime_t));
        DateTime_Add_Days(1, &stTempDate);
    }

    /* Update Trigger Time for Schedule Object */
    GET_SHDL_OFFSET(OBJECT_SCHEDULE, m_stTimeNPriority, ul32OffsetAddr);
    pstTriggerTime = (TriggerTimeNPriority_t *)(ul32BaseAddr+ul32OffsetAddr);

    /* On same day Update Trigger Time,
        if weekly trigger Time is smaller than Current trigger Time */
    if(0 > DateTime_Compare_Time(&stTriggeredTime, &pstTriggerTime->m_stTriggerTime) &&
        (0 == DateTime_Compare_Date(&stTempDate, &pstTriggerTime->m_stTriggerDate)) )
    {
        pstTriggerTime->m_stTriggerTime = stTriggeredTime;
        pstTriggerTime->m_stTriggerDate = stTempDate;
    }
    /* Update if Weekly Date is smaller than Current Trigger Date */
    else if(0 > DateTime_Compare_Date(&stTempDate, &pstTriggerTime->m_stTriggerDate))
    {
        pstTriggerTime->m_stTriggerTime = stTriggeredTime;
        pstTriggerTime->m_stTriggerDate = stTempDate;
    }

    while (NULL != pstTimeValue)
    {
        /* Check Event Time is less than System Time */
        if(0 <= DateTime_Compare_Time(&(stLocalTime), &(pstTimeValue->m_stTime)) )
        {
            /* Create List for TimeValues within the current time */
            if(pstTempTimeValue ==  NULL)
            {
                pstTempTimeValue = OSAL_Malloc(sizeof(BACnetTimeValue_t), __FILE__, __FUNCTION__, __LINE__);
				if(NULL == pstTempTimeValue)
				{
					#if(defined DEBUG_PRINTF && DL_0)
					Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
					SetSDLPresentValWeekly: malloc error \r\n");
					#endif
					return;
				}
                pstHeadTimeValue = pstTempTimeValue;
            }
            else
            {
                pstTempTimeValue->m_pstNext =
                    OSAL_Malloc(sizeof(BACnetTimeValue_t), __FILE__, __FUNCTION__, __LINE__);
				if(NULL == pstTempTimeValue->m_pstNext)
				{
					#if(defined DEBUG_PRINTF && DL_0)
					Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
					SetSDLPresentValWeekly: malloc error \r\n");
					#endif
					/* free allocated memory */
					Clear_TimeValues_List(&pstHeadTimeValue);
					return;
				}
                pstTempTimeValue = pstTempTimeValue->m_pstNext;
            }

            memcpy(pstTempTimeValue, pstTimeValue, sizeof(BACnetTimeValue_t));
            pstTempTimeValue->m_pstNext = NULL;

            /* Find NULL value with Biggest Time */
            if(0 < DateTime_Compare_Time(&(pstTimeValue->m_stTime), &(stNullTime)) &&
                BACNET_APPLICATION_TAG_NULL ==  pstTimeValue->m_stPropValue.m_TagType)
            {
                memcpy(&(stNullTime), &(pstTimeValue->m_stTime), sizeof(BACnetTime_t));
                bIsNullVal = true;
            }
        }
        pstTimeValue = pstTimeValue->m_pstNext;
    }

    /* Delete TimeValue before time of NULL value */
    if(bIsNullVal)
    {
        pstTempTimeValue = pstHeadTimeValue;
        pstTimeValue = pstHeadTimeValue;

        while(NULL != pstTempTimeValue)
        {
            /* Delete TimeValue before of NULL Value Time */
            if( 0 <= DateTime_Compare_Time(&(stNullTime), &(pstTimeValue->m_stTime)) )
            {
                if(pstTempTimeValue == pstHeadTimeValue)
                {
                    pstHeadTimeValue = pstTempTimeValue->m_pstNext;
                    pstTimeValue = pstTempTimeValue->m_pstNext;
                    OSAL_Free(pstTempTimeValue, __FILE__, __FUNCTION__, __LINE__);
                    pstTempTimeValue = pstHeadTimeValue;
                }
                else
                {
                    pstTimeValue->m_pstNext = pstTempTimeValue->m_pstNext;
                    OSAL_Free(pstTempTimeValue, __FILE__, __FUNCTION__, __LINE__);
                    pstTempTimeValue = pstTimeValue->m_pstNext;
                }
            }
            else
            {
                pstTimeValue = pstTempTimeValue;
                pstTempTimeValue = pstTempTimeValue->m_pstNext;
            }
        }
    }

    pstTimeValue = pstHeadTimeValue;
    memset(&stTempTime, 0, sizeof(BACnetTime_t));

    /*is current time is with in schedule time*/
    /*positive if time1 is after time2. negative if time1 is before time2.*/
    while (NULL != pstTimeValue)
    {
        /* Find TimeValue with Biggest Time ( So near to System time) */
        if(0 <= DateTime_Compare_Time(&(pstTimeValue->m_stTime), &(stTempTime)) &&
            BACNET_APPLICATION_TAG_NULL !=  pstTimeValue->m_stPropValue.m_TagType)
        {
            /* Update present value*/
            pstTempTimeValue = pstTimeValue;

            memcpy(&(stTempTime), &(pstTimeValue->m_stTime), sizeof(BACnetTime_t));
        }

        pstTimeValue = pstTimeValue->m_pstNext;
    }


    /* As no time match found(Not in scheule time) assigned the value of schedule_default to present value*/
    if(pstTempTimeValue)
    {
        memcpy(&(pstSHDLPresentValue->m_stValue), &(pstTempTimeValue->m_stPropValue), sizeof(BACNET_PROPERTY_VALUE));
    }
    else
    {
        GET_SHDL_OFFSET(OBJECT_SCHEDULE, m_stScheduleDefault, ul32OffsetAddr);

        memcpy(&(stScheduleDefault), (void *)(ul32BaseAddr+ul32OffsetAddr),
            sizeof(AnyValue_t));

        /* Copy data into property offset address */
        memcpy(pstSHDLPresentValue, &stScheduleDefault, sizeof(AnyValue_t));
    }

    pstTempTimeValue = pstHeadTimeValue;
    while(pstTempTimeValue != NULL)
    {
        pstHeadTimeValue = pstTempTimeValue->m_pstNext;
        OSAL_Free(pstTempTimeValue, __FILE__, __FUNCTION__, __LINE__);
        pstTempTimeValue = pstHeadTimeValue;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	SetSDLPresentValWeekly: exit \r\n");
	#endif
}





bool SetSDLPresentValExcp(AnyValue_t *pstSHDLPresentValue,
                        ListOfSpecialEvent_t    *pstSplEvent,
                        ulong32_t          ul32BaseAddr,
                        void *pVirtualDev)
{
	/* local variables */
    ActiveExcepTimeVal_t    *pstListofActExcepTimeVal = NULL;
    ActiveExcepTimeVal_t    *pstTempListofActExcepTimeVal = NULL;
    ActiveExcepTimeVal_t    *pstFirstListofActExcepTimeVal = NULL;
    BACnetTimeValue_t       *pstTimeValue = NULL;
    uint32_t                u32EventPriority = 16;
    BACnetTimeValue_t       stListOfTimeValues = {0};
    BACnetDate_t            stSyetemDate = {0};
    BACnetTime_t            stSyetemTime = {0};
    BACnetDate_t            stTempDate = {0};
    BACnetTime_t            stTempTime = {0};
    TriggerTimeNPriority_t *pstTriggerTime;
    bool bstatus= false;
    ulong32_t  ul32OffsetAddr = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	SetSDLPresentValExcp: entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pVirtualDev)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"APDU: SetSDLPresentValExcp: null input pointer \r\n");
		#endif
        return false;
	}
	else if(NULL == pstSplEvent)
	{
		/* exception schedule is empty, no need to proceed further */
		return false;
	}

    GetDevice_DateTime(&stSyetemDate, &stSyetemTime, pVirtualDev);
    stTempDate = stSyetemDate;

    /* this function give whether excepation schedule is in effect.
        and if it is then return stListOfTimeValues with priority*/
    pstListofActExcepTimeVal = Get_Excpt_Event_InEffect(pstSplEvent, &stListOfTimeValues, pVirtualDev);

    if(NULL != pstListofActExcepTimeVal)
        bstatus = true;

    /* Calculate next trigger time */
    stTempTime = GetNextTriggerTimeExcp(pstListofActExcepTimeVal, stSyetemTime);

    if(stTempTime.m_u8Hour == 25)
    {
        memset(&stTempTime, 0, sizeof(BACnetTime_t));
        DateTime_Add_Days(1, &stTempDate);
    }

    /* Update Trigger Time for Schedule Object */
    GET_SHDL_OFFSET(OBJECT_SCHEDULE, m_stTimeNPriority, ul32OffsetAddr);
    pstTriggerTime = (TriggerTimeNPriority_t *)(ul32BaseAddr+ul32OffsetAddr);
    pstTriggerTime->m_stTriggerTime = stTempTime;
    pstTriggerTime->m_stTriggerDate = stTempDate;


    /* upto this we got all event in within range of date, Date, weekNday....
        now sort time which before system time */
    if(NULL != pstListofActExcepTimeVal)
    {
        pstFirstListofActExcepTimeVal = pstListofActExcepTimeVal;
        pstTempListofActExcepTimeVal = pstListofActExcepTimeVal;

        while(NULL != pstListofActExcepTimeVal)
        {
            /* remove all element which contain time which is more than system time */
            pstTimeValue = &(pstListofActExcepTimeVal->m_stListOfTimeValues);

            if(0 > DateTime_Compare_Time(&(stSyetemTime), &(pstTimeValue->m_stTime)) ||
                (BACNET_APPLICATION_TAG_NULL == pstTimeValue->m_stPropValue.m_TagType))
            {
                if(pstFirstListofActExcepTimeVal == pstListofActExcepTimeVal)
                {
                    pstFirstListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
                    pstTempListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;

                    OSAL_Free(pstListofActExcepTimeVal,  __FILE__, __FUNCTION__, __LINE__);
                    pstListofActExcepTimeVal = pstTempListofActExcepTimeVal;
                }
                else
                {
                    pstTempListofActExcepTimeVal->m_pstNext = pstListofActExcepTimeVal->m_pstNext;

                    OSAL_Free(pstListofActExcepTimeVal,  __FILE__, __FUNCTION__, __LINE__);
                    pstListofActExcepTimeVal = pstTempListofActExcepTimeVal->m_pstNext;
                }
            }
            else if(NULL != pstListofActExcepTimeVal)
            {
                pstTempListofActExcepTimeVal = pstListofActExcepTimeVal;
                pstListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
            }
        }

        if(NULL == pstFirstListofActExcepTimeVal)
        {
            return false;
        }

        pstTempListofActExcepTimeVal = pstFirstListofActExcepTimeVal;

        /* Find highest priority event */
        u32EventPriority = 16;
        while(NULL != pstTempListofActExcepTimeVal)
        {
            if(u32EventPriority > pstTempListofActExcepTimeVal->m_u32EventPriority)
            {
                u32EventPriority = pstTempListofActExcepTimeVal->m_u32EventPriority;
            }

            pstTempListofActExcepTimeVal = pstTempListofActExcepTimeVal->m_pstNext;
        }

        pstTempListofActExcepTimeVal = pstFirstListofActExcepTimeVal;
        memset(&stTempTime, 0, sizeof(BACnetTime_t));

        while(NULL != pstTempListofActExcepTimeVal )
        {
            /* If Highest priority and Time is Highest in List  */
            if(u32EventPriority == pstTempListofActExcepTimeVal->m_u32EventPriority &&
               0 >= DateTime_Compare_Time(&stTempTime, &pstTempListofActExcepTimeVal->m_stListOfTimeValues.m_stTime ))
            {
                stTempTime = pstTempListofActExcepTimeVal->m_stListOfTimeValues.m_stTime;
                pstListofActExcepTimeVal = pstTempListofActExcepTimeVal;
            }
            pstTempListofActExcepTimeVal = pstTempListofActExcepTimeVal->m_pstNext;
        }

        /* update present value */
        if(NULL != pstListofActExcepTimeVal)
        {
            pstTimeValue = &(pstListofActExcepTimeVal->m_stListOfTimeValues);
            memcpy(&(pstSHDLPresentValue->m_stValue), &(pstTimeValue->m_stPropValue), sizeof(BACNET_PROPERTY_VALUE));
            bstatus = true;
        }

        /* free memory for Active Event List */
        pstListofActExcepTimeVal = pstFirstListofActExcepTimeVal;
        while(NULL != pstListofActExcepTimeVal)
        {
            pstTempListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
            OSAL_Free(pstListofActExcepTimeVal,  __FILE__, __FUNCTION__, __LINE__);
            pstListofActExcepTimeVal = pstTempListofActExcepTimeVal;
        }
		/* function exit */
		#if(defined DEBUG_PRINTF && DL_3)
		Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
		SetSDLPresentValExcp: exit \r\n");
		#endif
        return bstatus;
    }
    else
    {
		/* function exit */
		#if(defined DEBUG_PRINTF && DL_3)
		Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
		SetSDLPresentValExcp: exit \r\n");
		#endif
        return false;
    }
 }

/* this function will return list of TimeValues and priority value with in active peroid of date */
ActiveExcepTimeVal_t *Get_Excpt_Event_InEffect(ListOfSpecialEvent_t    *pstSplEvent,
                                            BACnetTimeValue_t *pstListOfTimeValues,
                                            void *pVirtualDev)
{
    BACnetDate_t            stSyetemDate = {0};
    BACnetTime_t            stSyetemTime = {0};
    ActiveExcepTimeVal_t   *pstListofActExcepTimeVal = NULL;
    ActiveExcepTimeVal_t   *pstHeadActExcepTimeVal = NULL;
    ActiveExcepTimeVal_t   *pstTmpActExcepTimeVal = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Get_Excpt_Event_InEffect: entry \r\n");
	#endif

    GetDevice_DateTime(&stSyetemDate, &stSyetemTime, pVirtualDev);

    /* Wait till the mutex is released */
    //Osal_Wait_Mutex(hMtxPropException, INFINITE);

    while(NULL != pstSplEvent)
    {
        pstTmpActExcepTimeVal = NULL;

        /* check is current day */
        if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_DATE)
        {
            if(IsValidCurrentDate(&stSyetemDate, &pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDate,EXACT))
            {
                pstTmpActExcepTimeVal =
                    FillActiveExcepTimeVal(&(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues),
                        pstSplEvent->m_stListSpecialEvent.m_u32EventPriority, stSyetemTime);

            }
        }
        /* check with in date of range */
        else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_DATE_RANGE)
        {
            if(Is_Active_DateRange(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange,
                &stSyetemDate))
            {
                pstTmpActExcepTimeVal =
                    FillActiveExcepTimeVal(&(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues),
                        pstSplEvent->m_stListSpecialEvent.m_u32EventPriority, stSyetemTime);

            }
        }
        /* check for week n day */
        else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_WEEK_N_DAY)
        {
            if(IsWeekInDay(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay, &stSyetemDate))
            {
                pstTmpActExcepTimeVal =
                    FillActiveExcepTimeVal(&(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues),
                        pstSplEvent->m_stListSpecialEvent.m_u32EventPriority, stSyetemTime);

            }
        }
		#ifdef BACDEL_OBJ_CAL
        /* for calendar reff */
        else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_CAL_REFF)
        {
            ulong32_t ul32OffsetAddr = 0 ;
            ulong32_t ul32BaseAddr = 0;
            bool bCalPresentVal = 0;
            void * pvObjectBasePtr = NULL;

            /* get calendar object */
            pvObjectBasePtr = Find_Object(OBJECT_CALENDAR,
                            pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stCalReff.m_u32ObjId,
                            pVirtualDev);

            ul32BaseAddr = (ulong32_t )pvObjectBasePtr;

            if(NULL != pvObjectBasePtr)
            {
                /* Get Present Value of  Calender object */
                GET_OBJCALPROP_OFFSET(OBJECT_CALENDAR, m_stPresentValue.m_bVal, ul32OffsetAddr);
                memcpy( &bCalPresentVal, (bool *)( ul32OffsetAddr + ul32BaseAddr), sizeof(bool));

                if(bCalPresentVal)
                {
                    pstTmpActExcepTimeVal =
                        FillActiveExcepTimeVal(&(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues),
                            pstSplEvent->m_stListSpecialEvent.m_u32EventPriority, stSyetemTime);
                }
            }
        }
		#endif /* CAL */

        if(NULL == pstHeadActExcepTimeVal)
        {
            pstHeadActExcepTimeVal  = pstTmpActExcepTimeVal;
        }
        else
        {
            pstListofActExcepTimeVal = pstHeadActExcepTimeVal;

            while(NULL != pstListofActExcepTimeVal->m_pstNext)
            {
                pstListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
            }

            pstListofActExcepTimeVal->m_pstNext = pstTmpActExcepTimeVal;
        }

        pstSplEvent = pstSplEvent->m_pstNext;
    }
    /* release the mutex */
    //Osal_Release_Mutex(hMtxPropException);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Get_Excpt_Event_InEffect: exit \r\n");
	#endif

   return pstHeadActExcepTimeVal;
}

/**
*
* DESCRIPTION
* Function to clear property values of type list of active exception time values.
*
* @param ppFreeActExecpTimeValue [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ActiveExcepTimeValues_List(ActiveExcepTimeVal_t **ppFreeActExecpTimeValue)
{
	/* local variables */
	void *pvTemp = NULL;
    ActiveExcepTimeVal_t   *pstActExecpTimeValue = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: Clear_ActiveExcepTimeValues_List: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == ppFreeActExecpTimeValue)
        return;
	if(NULL == *ppFreeActExecpTimeValue)
        return;

	/* get the base pointer of link list */
    pstActExecpTimeValue = *ppFreeActExecpTimeValue;
	*ppFreeActExecpTimeValue = NULL;

    while(NULL != pstActExecpTimeValue)
    {
		/* save next */
        pvTemp = pstActExecpTimeValue->m_pstNext;
		/* free allocated memory */
        OSAL_Free(pstActExecpTimeValue, __FILE__, __FUNCTION__, __LINE__);
		/* move to next node */
        pstActExecpTimeValue = pvTemp;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: Clear_ActiveExcepTimeValues_List: exit \r\n");
	#endif
    return;
}

/* */
ActiveExcepTimeVal_t *FillActiveExcepTimeVal(BACnetTimeValue_t *pstListOfTimeValues,
                                             uint32_t  u32EventPriority,
                                             BACnetTime_t  stSyetemTime)
{
    ActiveExcepTimeVal_t   *pstListofActExcepTimeVal = NULL;
    ActiveExcepTimeVal_t   *pstTempListofActExcepTimeVal = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	FillActiveExcepTimeVal: entry \r\n");
	#endif

    /* If List of TimeValues is Not filled */
    if(pstListOfTimeValues->bIsUsed != 1)
        return pstListofActExcepTimeVal;


    while(NULL != pstListOfTimeValues)
    {
        /* If NULL PropValue and Time is equal or less than Current time,
            then list neglect all Values already filled */
        if(pstListOfTimeValues->m_stPropValue.m_TagType == BACNET_APPLICATION_TAG_NULL &&
            0 <= DateTime_Compare_Time(&(stSyetemTime), &(pstListOfTimeValues->m_stTime)))
        {
            while(NULL != pstListofActExcepTimeVal)
            {
                pstTempListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
                OSAL_Free(pstListofActExcepTimeVal, __FILE__, __FUNCTION__, __LINE__);
                pstListofActExcepTimeVal = pstTempListofActExcepTimeVal;
            }

            pstListofActExcepTimeVal =  NULL;
            pstTempListofActExcepTimeVal = NULL;
        }
        else
        {
            /* Assign memory to save List of Active Time Values */
            if(NULL == pstListofActExcepTimeVal)
            {
                pstListofActExcepTimeVal =
                    OSAL_Malloc(sizeof(ActiveExcepTimeVal_t),__FILE__,__FUNCTION__, __LINE__);
				if(NULL == pstListofActExcepTimeVal)
				{
					#if(defined DEBUG_PRINTF && DL_0)
					Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
					FillActiveExcepTimeVal: malloc error \r\n");
					#endif
					return NULL;
				}
                pstTempListofActExcepTimeVal = pstListofActExcepTimeVal;
            }
            else
            {
                pstListofActExcepTimeVal->m_pstNext =
                    OSAL_Malloc(sizeof(ActiveExcepTimeVal_t),__FILE__,__FUNCTION__, __LINE__);
				if(NULL == pstListofActExcepTimeVal->m_pstNext)
				{
					#if(defined DEBUG_PRINTF && DL_0)
					Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
					FillActiveExcepTimeVal: malloc error \r\n");
					#endif
					/* free allocated memory */
					Clear_ActiveExcepTimeValues_List(&pstTempListofActExcepTimeVal);
					return NULL;
				}
                pstListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
            }

            pstListofActExcepTimeVal->m_pstNext = NULL;

            /* Update Active Event List */
            memcpy( &(pstListofActExcepTimeVal->m_stListOfTimeValues), pstListOfTimeValues, sizeof(BACnetTimeValue_t));
            pstListofActExcepTimeVal->m_u32EventPriority = u32EventPriority;
            pstListofActExcepTimeVal->m_stListOfTimeValues.m_pstNext = NULL;
        }

        pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	FillActiveExcepTimeVal: exit \r\n");
	#endif

    return pstTempListofActExcepTimeVal;
}



BACnetTime_t GetNextTriggerTimeExcp(ActiveExcepTimeVal_t *pstListofActExcepTimeVal, BACnetTime_t stSyetemTime)
{
    BACnetTime_t            stTempTime = {0};
    stTempTime.m_u8Hour = 25;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	GetNextTriggerTimeExcp: entry \r\n");
	#endif

    while(NULL != pstListofActExcepTimeVal)
    {
        /* We Have to Find nxtTime to SystemTime for trigger,
            IF +ve EventTime is More than SystemTime */
        if(0 < DateTime_Compare_Time(&(pstListofActExcepTimeVal->m_stListOfTimeValues.m_stTime), &stSyetemTime))
        {
            /* We have to find smallest time in List as Trigger Time,
                If +ve, Trigger Time is More than EventTime */
            if(0 < DateTime_Compare_Time(&stTempTime, &(pstListofActExcepTimeVal->m_stListOfTimeValues.m_stTime)))
            {
                memcpy(&stTempTime, &(pstListofActExcepTimeVal->m_stListOfTimeValues.m_stTime), sizeof(BACnetTime_t));
            }
        }
        pstListofActExcepTimeVal = pstListofActExcepTimeVal->m_pstNext;
    }
    /*if(stTempTime.hour == 25)
        stTempTime.hour = 0;*/
	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	GetNextTriggerTimeExcp: exit \r\n");
	#endif
    return stTempTime;
}

BACnetTime_t GetNextTriggerTimeWeekly(BACnetTimeValue_t   *pstListOfTimeValues,BACnetTime_t stSyetemTime)
{
    BACnetTime_t            stTempTime = {0};
    stTempTime.m_u8Hour = 25;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	GetNextTriggerTimeWeekly: entry \r\n");
	#endif

    while(NULL != pstListOfTimeValues)
    {
        if(0 < DateTime_Compare_Time(&(pstListOfTimeValues->m_stTime), &stSyetemTime))
        {
            if(0 < DateTime_Compare_Time(&stTempTime, &(pstListOfTimeValues->m_stTime)))
                memcpy(&stTempTime, &(pstListOfTimeValues->m_stTime), sizeof(BACnetTime_t));
        }
        pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	GetNextTriggerTimeWeekly: exit \r\n");
	#endif
    return stTempTime;
}


#ifdef NEW_RP_WP_INTERFACE
/** function to convert and fill schedule property value to initiate WP-A */
void Fill_WpA_Property_Value(
	AnyValue_t *pstSdlPresentValue,
	BACNET_DATA_TYPE eDataType,
	void **ppvPropValue)
{
	/* local variables */
	void *pvData = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Fill_WpA_Property_Value: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pstSdlPresentValue || NULL == ppvPropValue)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Fill_WpA_Property_Value: null input pointer \r\n");
		#endif
		return;
	}

	/* fill property value as per data type */
	switch(eDataType)
	{
	case BACNET_DT_NULL:
		/* nothing to encode */
		break;

	case BACNET_DT_UNSIGNED:
	case BACNET_DT_UNSIGNED8:
	case BACNET_DT_UNSIGNED16:
	case BACNET_DT_UNSIGNED32:
		{
			/* save unsigned value */
			Pr_BACnetUnsigned32_t *pstUintVal;
			pstUintVal = (Pr_BACnetUnsigned32_t *)OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstUintVal)
			{
				/* save value */
				pstUintVal->m_u32Val = pstSdlPresentValue->m_stValue.uValue.m_Unsigned_Int;
				pvData = pstUintVal;
			}
		}
		break;

	case BACNET_DT_INTEGER:
		{
			/* save signed value */
			Pr_BACnetSigned32_t *pstIntVal;
			pstIntVal = (Pr_BACnetSigned32_t *)OSAL_Malloc(sizeof(Pr_BACnetSigned32_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstIntVal)
			{
				/* save value */
				pstIntVal->m_i32Val = pstSdlPresentValue->m_stValue.uValue.m_Signed_Int;
				pvData = pstIntVal;
			}
		}break;

	case BACNET_DT_ENUM:
		{
			/* save enumerated value */
			Pr_BinaryEnumPV_t *pstEnumVal;
			pstEnumVal = (Pr_BinaryEnumPV_t *)OSAL_Malloc(sizeof(Pr_BinaryEnumPV_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstEnumVal)
			{
				/* save value */
				pstEnumVal->m_eVal = pstSdlPresentValue->m_stValue.uValue.m_Enumerated;
				pvData = pstEnumVal;
			}
		}
		break;

	case BACNET_DT_ENUM_NEW:
		{
			/* save enumerated value */
			Pr_BACnetEnum_t *pstEnumVal;
			pstEnumVal = (Pr_BACnetEnum_t *)OSAL_Malloc(sizeof(Pr_BACnetEnum_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstEnumVal)
			{
				/* save value */
				pstEnumVal->m_eVal = pstSdlPresentValue->m_stValue.uValue.m_Enumerated;
				pvData = pstEnumVal;
			}
		}
		break;

	case BACNET_DT_BOOLEAN:
		{
			/* save boolean value */
			Pr_BACnetBool_t *pstBoolVal;
			pstBoolVal = (Pr_BACnetBool_t *)OSAL_Malloc(sizeof(Pr_BACnetBool_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstBoolVal)
			{
				/* save value */
				pstBoolVal->m_bVal = pstSdlPresentValue->m_stValue.uValue.m_Boolean;
				pvData = pstBoolVal;
			}
		}
		break;

	case BACNET_DT_REAL:
		{
			/* save real value */
			Pr_BACnetReal_t *pstRealVal;
			pstRealVal = (Pr_BACnetReal_t *)OSAL_Malloc(sizeof(Pr_BACnetReal_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstRealVal)
			{
				/* save value */
				pstRealVal->m_fVal = pstSdlPresentValue->m_stValue.uValue.m_Real;
				pvData = pstRealVal;
			}
		}
		break;

	case BACNET_DT_DOUBLE:
		{
			/* save double value */
			Pr_BACnetDouble_t *pstDoubleVal;
			pstDoubleVal = (Pr_BACnetDouble_t *)OSAL_Malloc(sizeof(Pr_BACnetDouble_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstDoubleVal)
			{
				/* save value */
				pstDoubleVal->m_dVal = pstSdlPresentValue->m_stValue.uValue.m_Double;
				pvData = pstDoubleVal;
			}
		}
		break;

	default:
		/* do nothing */
		break;
	}// switch ends

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Fill_WpA_Property_Value: exit \r\n");
	#endif

	/* save the pointer and return */
	*ppvPropValue = pvData;
	return;
}
#endif /* NEW_RP_WP_INTERFACE */


/**
*
*DESCRIPTION
* get Service Args to schedule property of other Device
*
*@param  pstDevObjPropRef -  pointer to "Device Object Property Reference"
*@paramp pstServiceArgs - Structure to access the data of BACnet IP packet. .
**@paramp i32Priority -  priority at which the referenced properties are commanded
*/
BACNET_RETURN_TYPE SchedulingInternal(BACnetDevObjPropRef_t  *pstDevObjPropRef,
                        virtualDevData_t *pVirtualDev,
                        AnyValue_t   *pstSHDLPresentValue,
                        uint32_t              i32Priority)
{
    BACNET_CONF_DATA stWpData = {0};
    BACNET_RETURN_TYPE eRetValue = BACDEL_ERROR;
	BACnetAddress_t stAddress = {0};
	BACnetDateTime_t stTimeStamp = {0};

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	SchedulingInternal: entry \r\n");
	#endif

	stWpData.eObjectType = pstDevObjPropRef->m_eObjectType;
    stWpData.u32ObjectInstance = pstDevObjPropRef->m_u32ObjId;
    stWpData.eObjectProperty = pstDevObjPropRef->m_ePropertyIdentifier;
    stWpData.u32ArrayIndex = pstDevObjPropRef->m_u32ArrayIndex;
    stWpData.bArrIndxPresent = pstDevObjPropRef->m_bArrIndxPresent;
    stWpData.u8Priority = (uint8_t)i32Priority;

    /* Assigned 50 bytes memory to Write property Value */
    stWpData.pu8PropValueBuffer = (uint8_t*) OSAL_Malloc( 50, __FILE__, __FUNCTION__, __LINE__);

    if(NULL == stWpData.pu8PropValueBuffer)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR,
		"APDU: SchedulingInternal: malloc error \r\n");
		#endif
        return BACDEL_MALLOC_ERROR;
	}

    stWpData.i32ApplicationDataLen =
        BACApp_Encode_Application_Data(stWpData.pu8PropValueBuffer, &(pstSHDLPresentValue->m_stValue));

    /* update present value of object referred by ManiplVariableReff*/
    //eRetValue = Write_Object_Property( pVirtualDev, &stWpData);
	eRetValue = WP_B_Request_Parser(pVirtualDev, &stWpData, &stAddress,
		&stTimeStamp, THREADPOOL_COUNT);

    OSAL_Free(stWpData.pu8PropValueBuffer,  __FILE__, __FUNCTION__, __LINE__);
    stWpData.pu8PropValueBuffer = NULL;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	SchedulingInternal: exit \r\n");
	#endif

    return(eRetValue);
}



/**
*
*DESCRIPTION
* get Service Args to schedule property of other Device
*
*@param  pstDevObjPropRef -  pointer to "Device Object Property Reference"
*@paramp pstServiceArgs - Structure to access the data of BACnet IP packet. .
**@paramp i32Priority -  priority at which the referenced properties are commanded
*/
bool SchedulingExternal(BACnetDevObjPropRef_t   *pstDevObjPropRef,
                        bacnetip_arguments_t    *pstServiceArgs,
                        AnyValue_t              *pstSHDLPresentValue,
                        uint32_t                i32Priority,
                        void                    *pVirtualDev)
{
    bool bFlag = false;
	BACNET_DATA_TYPE eDataType = BACNET_DT_NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	SchedulingExternal: entry \r\n");
	#endif

   	BACDEL_Set_Default_Parameters(pstServiceArgs, BACDEL_REQ_WP);

    do
    {
        if(bFlag == true || (BACDEL_SUCCESS == BACDEL_Get_Device_ID_Or_Address(
                                                &(pstDevObjPropRef->m_u32DeviceInstace),
                                                &pstServiceArgs->m_stDestBACnetAddr, true)))
        {
            /* Get the Object type to be read */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice
				.m_stWP_Request.m_eObjectType = pstDevObjPropRef->m_eObjectType;
            /* Get the instance number to be read */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice
				.m_stWP_Request.m_u32ObjectInstance = pstDevObjPropRef->m_u32ObjId;
            /* Get property number to be read */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stWP_Request.m_eObjectProperty = pstDevObjPropRef->m_ePropertyIdentifier;
            /* If property is an array get the array index to be read */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWP_Request.m_u32ArrayIndex = pstDevObjPropRef->m_u32ArrayIndex;
            if(ARRAY_INDEX_PRESENT == pstDevObjPropRef->m_bArrIndxPresent)
                pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stWP_Request.m_u8ArrayIndexPresent = ARRAY_INDEX_PRESENT;
           /* Update Priority */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stWP_Request.m_u32Priority = i32Priority;

#ifdef OLD_RP_WP_INTERFACE
            /* Copy Schedule Present value into WP request */
            memcpy(&(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWP_Request.m_stPropertyValue),&(pstSHDLPresentValue->m_stValue), sizeof(BACNET_PROPERTY_VALUE));
            /* Scheduling support Primitive Data type only */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWP_Request.m_stPropertyValue.m_ContextSpecific = -1;
            /* Scheduling support Primitive Data type only */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWP_Request.m_stPropertyValue.m_ContextTag = -1;
            /* Scheduling support Primitive Data type only */
            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWP_Request.m_stPropertyValue.m_StartTag = -1;

#elif defined NEW_RP_WP_INTERFACE
			/* save data type */
			/* Convert App Tag Type To Data Type */
			eDataType = Convert_AppTagType_To_DataType(pstSHDLPresentValue->m_stValue.m_TagType);

			pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stWP_Request.m_eDataType = eDataType;
			/* get the pointer of property value */
			Fill_WpA_Property_Value(pstSHDLPresentValue, eDataType,
				&pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stWP_Request.m_pvPropVal);

#elif defined CONSOLE_APPLICATION
            /* Convert Value to String format */
            /* FIXME: Only primitive Datatypes supported */
            Convert_AppTag_To_String(&pi8PropVal,
                       &pstSHDLPresentValue->m_stValue,
					   (uint8_t)pstDevObjPropRef->m_u16ObjectType,
                       MAX_PROP_SUPPORTED, //pstDevObjPropRef->m_ePropertyIdentifier,
					   pstDevObjPropRef->m_u32ArrayIndex,
					   &i32StringLen, pstDevObjPropRef->m_bArrIndxPresent, false);

            /* copy value to WP-A Request structure */
            Strcopy(&(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stWP_Request.m_i8PropValue[0]),
                pi8PropVal, MAX_APPLN_PROP_VAL_STR);
#else
#endif
            return true;
        }
        else
        {
             Send_WHO_IS(pVirtualDev,
                            (pstDevObjPropRef->m_u32DeviceInstace),
                            (pstDevObjPropRef->m_u32DeviceInstace),
                            false,
					        0,
                            DESTINATION_IS_GLOBAL_BROADCAST);
            Osal_Sleep(100);
            bFlag = true;
        }
    }while(BACDEL_SUCCESS == BACDEL_Get_Device_ID_Or_Address(&(pstDevObjPropRef->m_u32DeviceInstace),
                        &pstServiceArgs->m_stDestBACnetAddr, true));

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	SchedulingExternal: exit \r\n");
	#endif

    return false;
}


bool IS_ConfigurationError(ListOfBACnetDevObjPropRef_t     *pstListOfBACnetDevObjPropReff,
                           ListOfSpecialEvent_t       *pstSplEvent,
                           Pr_ListOfBACnetDailySchedule_t            *pstWeeklySchedule,
                           AnyValue_t               *pstScheduleDefault,
						   void *pVirtualDev)
{
    BACNET_PROPERTY_ID eObjectProperty;
    uint32_t u32arryaIndex = BACNET_ARRAY_ALL;
    BACNET_APPLICATION_TAG    eDataTypeDORP = BACNET_APPLICATION_TAG_NULL;
     bool bIsDataTypeSameDOPR = true;
    bool bIsDataTypeSameEXCP = true;
    bool bIsDataTypeSameWEEK = true;
    uint8_t TagTypeEXCP = BACNET_APPLICATION_TAG_NULL;
    uint8_t TagTypeWEEK = BACNET_APPLICATION_TAG_NULL;
    uint8_t TagTypeDEFAULT = BACNET_APPLICATION_TAG_NULL;
    BACnetTimeValue_t   *pstListOfTimeValues = NULL;
    BACNET_APPLICATION_TAG eTagType = TAG_NOT_SUPPORTED;
    int32_t i32count = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	IS_ConfigurationError: entry \r\n");
	#endif

    /* Validate Object Property reference has Propeties of same DataType */
    while(NULL != pstListOfBACnetDevObjPropReff)
    {
        /* get array index*/
        u32arryaIndex =  pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_u32ArrayIndex;
        if(ARRAY_INDEX_PRESENT != pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bArrIndxPresent)
            u32arryaIndex = BACNET_ARRAY_ALL;
        /* get Property ID */
        eObjectProperty = pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_ePropertyIdentifier;

            /* Get Tag for Property */
		eTagType = GetPropertyTag(pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_eObjectType,
                        eObjectProperty, u32arryaIndex,
                        pstListOfBACnetDevObjPropReff->m_stDevObjPropRef.m_bArrIndxPresent);

            if(eTagType > BACNET_APPLICATION_TAG_REAL &&
                eTagType != BACNET_APPLICATION_TAG_ENUMERATED)
            {
                return false;
            }

            if(BACNET_APPLICATION_TAG_NULL == eDataTypeDORP)
            {
                /* Set Property DataType 1st Time */
                eDataTypeDORP = eTagType;
            }
            else if(eDataTypeDORP != eTagType)
            {
                bIsDataTypeSameDOPR = false;
                return false;
            }

            /* get next object */
            pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;
            bIsDataTypeSameDOPR = true;
    }

    /* Validate All Entries in Exception schedule, Weekly schedule and Schedule Defeult are same DataType */
    if(bIsDataTypeSameDOPR )
    {
        /* check for exception schedule */
        while(NULL != pstSplEvent)
        {
            pstListOfTimeValues = &(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues);

            /* If This entry is empty then no need to validate */
            if(pstListOfTimeValues->bIsUsed)
            {
                while(NULL != pstListOfTimeValues)
                {
                    if(TagTypeEXCP == BACNET_APPLICATION_TAG_NULL)
                        TagTypeEXCP = pstListOfTimeValues->m_stPropValue.m_TagType;

                    if( TagTypeEXCP != pstListOfTimeValues->m_stPropValue.m_TagType &&
                        BACNET_APPLICATION_TAG_NULL  != pstListOfTimeValues->m_stPropValue.m_TagType)
                    {
                        bIsDataTypeSameEXCP = false;
                        break;
                    }

                    pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
                    bIsDataTypeSameEXCP = true;
                }
            }

            if(false == bIsDataTypeSameEXCP)
                break;

            pstSplEvent = pstSplEvent->m_pstNext;
        } // End of While for Exception schedule

        /* Validate ObjProp Tag with ExcptShdl Tag */
        if(eDataTypeDORP != TagTypeEXCP &&
            TagTypeEXCP != BACNET_APPLICATION_TAG_NULL &&
            eDataTypeDORP != BACNET_APPLICATION_TAG_NULL)
            bIsDataTypeSameEXCP = false;

        if(bIsDataTypeSameEXCP)
        {
            for(i32count = 0; i32count < BACNET_ARRAY_OF_SEVEN; i32count++)
            {
                pstListOfTimeValues =  &(pstWeeklySchedule->m_stTimeValue[i32count]);

                /* If This entry is empty then no need to validate */
                if(pstListOfTimeValues->bIsUsed)
                {
                    while(NULL != pstListOfTimeValues)
                    {
                        if(TagTypeWEEK == BACNET_APPLICATION_TAG_NULL)
                            TagTypeWEEK = pstListOfTimeValues->m_stPropValue.m_TagType;

                        if(TagTypeWEEK != pstListOfTimeValues->m_stPropValue.m_TagType &&
                            BACNET_APPLICATION_TAG_NULL != pstListOfTimeValues->m_stPropValue.m_TagType)
                        {
                            bIsDataTypeSameDOPR = false;
                            return false;
                        }
                        pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
                        bIsDataTypeSameWEEK = true;
                    }
                }
                /*if(false == bIsDataTypeSameWEEK)
                    break;*/
            } // End of for of Weekly Schedule

            /* Validate ObjProp Tag with WeeklyShdl Tag */
            if(eDataTypeDORP != TagTypeWEEK &&
                TagTypeWEEK != BACNET_APPLICATION_TAG_NULL &&
                eDataTypeDORP != BACNET_APPLICATION_TAG_NULL)
                bIsDataTypeSameWEEK = false;

            if(bIsDataTypeSameWEEK)
            {
                if(NULL != pstScheduleDefault)
                {
                    TagTypeDEFAULT = pstScheduleDefault->m_stValue.m_TagType;

                    /* Validate ObjProp Tag with ShdlDefault Tag */
                    if(eDataTypeDORP != TagTypeDEFAULT && TagTypeDEFAULT != BACNET_APPLICATION_TAG_NULL
                        && eDataTypeDORP != BACNET_APPLICATION_TAG_NULL)
                    {
                        return false;
                    }
                    else
                    {
                        //SameWeekExcpDeflt = true;
                    }
                }
            }
            else /* weekly schedule tag type match not found */
            {
                 return false;
            }
        }
        else /* Exception schedule tag type match not found */
        {
             return false;
        }
    }   /* Device Obj Prop reff data type match not found */

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	IS_ConfigurationError: exit \r\n");
	#endif
    return true;
}





/**
*
*DESCRIPTION
*   This is 1sec. Timer call back fucntion.
*
*@param lpParam [in] This is not used.
*@param TimerOrWaitFired [in] <TBD>
*
*@return VOID.
*/

TIMER_CALLBACK SCHEDULE_TIMER(void* lpParam, uint8_t TimerOrWaitFired)
{

    ulong32_t               ul32OffsetAddr = 0 ;
    ulong32_t               ul32BaseAddr = 0;
    BACnetDate_t            stSyetemDate = {0};
    BACnetTime_t            stSyetemTime = {0};
    BACnetTime_t            stCalTriggerTime = {0};
    BACnetTime_t            stMidNightTime = {0};
    BACNET_PROPERTY_VALUE   *pstValue = NULL;
    PROP_ACCESS_TYPE        ePermission;
    Schedule_t		        *pstSchedule = NULL;
    void                    *pvVirtualDev = NULL;
    uint32_t                PropertyPtr = 0;
    void                    *pvDummy = NULL;
    BACnet_Device_Struct_t* pstHostDevice = NULL;
	#ifdef BACDEL_OBJ_CAL
    Calendar_t  *pstObjCalendar = NULL;
    bool bCalPresentVal = 0;
	#endif
    Pr_BACnetDateRange_t         stEffectivePeriod = {0};
    TriggerTimeNPriority_t* pstTriggerTime = NULL;
	static bool bSchdlTimerFlag = false;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	SCHEDULE_TIMER: entry \r\n");
	#endif

	/* avoid concurrent execution of multiple timer instances */
	if(true == bSchdlTimerFlag)
	{
		/* return from function */
		#ifdef __linux
		return NULL;
		#else
		return NULL;
		#endif
	}
	/* set the flag */
	bSchdlTimerFlag = true;

    pstHostDevice = &gstHostDevice;
    stCalTriggerTime.m_u8Sec = 5;


    while(NULL != pstHostDevice)
    {
        pvVirtualDev = pstHostDevice->m_pstDeviceStruct;
		if(NULL == pvVirtualDev)
			break;
        pstSchedule =  ((virtualDevData_t*)pvVirtualDev)->m_stSupportedObjects.m_pstSchedule;

        while(NULL != pstSchedule)
        {
            ul32BaseAddr = (ulong32_t)pstSchedule;

            GetDevice_DateTime(&stSyetemDate,&stSyetemTime, pvVirtualDev);

			/* check for reliability */
			if(RELIABILITY_NO_FAULT_DETECTED != pstSchedule->m_stReliability.m_eReliabilty)
			{
				/* move to next node */
				pstSchedule = pstSchedule->pstNext;
				continue;
			}

            GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stEffectivePeriod,ul32OffsetAddr);
            /* Copy data into property offset address */
            memcpy(&(stEffectivePeriod),(void *)(ul32BaseAddr+ul32OffsetAddr),
                sizeof(Pr_BACnetDateRange_t));

            /* Get current Time */
            if(Is_Active_DateRange(&stEffectivePeriod.m_stDateRange, &stSyetemDate))
            {
                /* Update Trigger Time for Schedule Object */
                GET_SHDL_OFFSET(OBJECT_SCHEDULE, m_stTimeNPriority, ul32OffsetAddr);
                pstTriggerTime = (TriggerTimeNPriority_t *)(ul32BaseAddr+ul32OffsetAddr);

                /* +ve if, TriggerTime is already passed to CurrentTime && Its Trigger Time is current time */
                if(0 <= DateTime_Compare_Time(&stSyetemTime, &pstTriggerTime->m_stTriggerTime) &&
                    (0 <= DateTime_Compare_Date(&stSyetemDate, &pstTriggerTime->m_stTriggerDate)) )
                 {
                    GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stPresentValue.m_stValue,ul32OffsetAddr);
                    pstValue = (void *)(ul32BaseAddr+ul32OffsetAddr);


                    UpdateCommonProperty(OBJECT_SCHEDULE, (void*)ul32BaseAddr,PROP_PRESENT_VALUE, -1, pstValue,
                                        ePermission,&PropertyPtr, &pvDummy, pvVirtualDev,
                                        false,false,false);
                }
            }
            pstSchedule = pstSchedule->pstNext;
        }/* End of inner while loop */
        /* at Midnight trigger Calendar and schedule object Present value */
        if(0 <= DateTime_Compare_Time(&stSyetemTime,&stMidNightTime))
        {
            if(0 <= DateTime_Compare_Time(&stCalTriggerTime,&stSyetemTime))
            {
				#ifdef BACDEL_OBJ_CAL
                /* calendar object */
                pstObjCalendar = ((virtualDevData_t*)pvVirtualDev)->m_stSupportedObjects.m_pstCalendar;
                while(NULL != pstObjCalendar)
                {
                    ul32BaseAddr = (ulong32_t)pstObjCalendar;

                    UpdateCommonProperty(OBJECT_CALENDAR, (void*)ul32BaseAddr,PROP_PRESENT_VALUE, -1, &bCalPresentVal,
                                    ePermission,&PropertyPtr, &pvDummy, pvVirtualDev,
                                    false,false,false);

                     pstObjCalendar = pstObjCalendar->pstNext;
                }
				#endif
                /* schedule object */
                pstSchedule = ((virtualDevData_t*)pvVirtualDev)->m_stSupportedObjects.m_pstSchedule;
                while(NULL != pstSchedule)
                {
                    ul32BaseAddr = (ulong32_t)pstSchedule;
                    GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stPresentValue.m_stValue,ul32OffsetAddr);
                    pstValue = (void *)(ul32BaseAddr+ul32OffsetAddr);


                    UpdateCommonProperty(OBJECT_SCHEDULE, (void*)ul32BaseAddr,PROP_PRESENT_VALUE, -1, pstValue,
                                ePermission,&PropertyPtr, &pvDummy, pvVirtualDev,
                                false,false,false);

                    pstSchedule = pstSchedule->pstNext;
                }
            } /* inner if */
        }/* outer if */
        pstHostDevice = pstHostDevice->pstNextDevice;
    }/*  End of outer while loop */

	/* reset the flag */
	bSchdlTimerFlag = false;

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	SCHEDULE_TIMER: exit \r\n");
	#endif
//#ifdef __linux
	return NULL;
//#endif
}

/** function to trigger present value of schedule object whenever device
	date_time changes due to time_sync or utc_time_sync */
void Trigger_Schedule_Object(virtualDevData_t *pVirtualDev)
{
	/* local variables */
	ulong32_t ul32OffsetAddr = 0 ;
    ulong32_t ul32BaseAddr = 0;
    BACNET_PROPERTY_VALUE *pstValue = NULL;
    PROP_ACCESS_TYPE ePermission = 0;
    Schedule_t	*pstSchedule = NULL;
    uint32_t u32PropertyPtr = 0;
    void *pvDummy = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Trigger_Schedule_Object: entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pVirtualDev)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Trigger_Schedule_Object: null input pointer \r\n");
		#endif
		return;
	}

	/* get 1st node of schedule object linklist */
    pstSchedule =  pVirtualDev->m_stSupportedObjects.m_pstSchedule;

    while(NULL != pstSchedule)
    {
		/* check for reliability */
		if(RELIABILITY_NO_FAULT_DETECTED != pstSchedule->m_stReliability.m_eReliabilty)
		{
			/* move to next node */
			pstSchedule = pstSchedule->pstNext;
			continue;
		}
		/* convert base address */
		ul32BaseAddr = (ulong32_t)pstSchedule;

		/* get present value offset of schedule obj */
        GET_SHDL_OFFSET(OBJECT_SCHEDULE,m_stPresentValue.m_stValue,ul32OffsetAddr);
        pstValue = (void *)(ul32BaseAddr+ul32OffsetAddr);

		/* call update common property */
        UpdateCommonProperty(OBJECT_SCHEDULE, (void *)pstSchedule,
				PROP_PRESENT_VALUE, -1, pstValue,
				ePermission, &u32PropertyPtr, &pvDummy, pVirtualDev,
				false,false,false);

		/* move to next schedule object */
		pstSchedule = pstSchedule->pstNext;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Trigger_Schedule_Object: exit \r\n");
	#endif
}

#ifdef BACDEL_OBJ_CAL
/** function to trigger present value of calendar object whenever device
	date_time changes due to time_sync or utc_time_sync */
void Trigger_Calendar_Object(virtualDevData_t *pVirtualDev)
{
	/* local variables */
    PROP_ACCESS_TYPE ePermission = 0;
    uint32_t u32PropertyPtr = 0;
    void *pvDummy = NULL;
    Calendar_t *pstObjCalendar = NULL;
	bool bCalPresentVal = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Trigger_Calendar_Object: entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pVirtualDev)
	{
		/* invalid pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Trigger_Calendar_Object: null input pointer \r\n");
		#endif
		return;
	}

	/* get 1st node of schedule object linklist */
    pstObjCalendar =  pVirtualDev->m_stSupportedObjects.m_pstCalendar;

    while(NULL != pstObjCalendar)
    {
		/* call update common property */
        UpdateCommonProperty(OBJECT_CALENDAR, (void *)pstObjCalendar,
				PROP_PRESENT_VALUE, -1, &bCalPresentVal,
				ePermission, &u32PropertyPtr, &pvDummy, pVirtualDev,
				false,false,false);

		/* move to next calendar object */
		pstObjCalendar = pstObjCalendar->pstNext;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Trigger_Calendar_Object: exit \r\n");
	#endif
}
#endif /* CAL */

#endif /* BACDEL_OBJ_SDL */
