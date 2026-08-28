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
*   File Name - propertyValidations.c
*
*   RELEASE HISTORY
*
*	DATE				NAME					DESCRIPTION
*	5th March 2013		Pratham N. Murkute		Created file & moved api's.
*
*************************************************************************/

/* header file includes */
#include "propertyValidations.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"
#include "pduDateTime.h"
#include "pduEncodeDecode.h"
#include "objDevice.h"
#include "propertyGenricHandler.h"
#include <stdio.h>

extern DB_t SMCfg;
/***
*
* DESCRIPTION
* API to validate values in Week N Day received.
*
* @param  : values for month, week of month & weekday.
* @return : true on success else false.
*
***/
bool Validate_WeekNDay(
	BACNET_MONTH eMonth,
	BACNET_WEEK_OF_MONTH eWeekOfMonth,
	BACNET_WEEKDAY eWeekNDay)
{
	/* return value */
	bool bReturnValue = TRUE;

	/* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_WeekNDay: entry \r\n");
	#endif

	if(eMonth < BACNET_MONTH_JANUARY || eMonth > BACNET_MONTH_EVENMONTH)
		if(eMonth != BACNET_MONTH_ANYMONTH)
			bReturnValue = FALSE;

	if(eWeekOfMonth < DAYS_NUM_1_7 || eWeekOfMonth > LAST_7_DAYS_OF_MONTH)
		if(eWeekOfMonth != ANY_WEEK_OF_MONTH)
			bReturnValue = FALSE;

	if(eWeekNDay < BACNET_WEEKDAY_MONDAY || eWeekNDay > BACNET_WEEKDAY_SUNDAY)
        if(eWeekNDay != ANY_DAY_OF_WEEK)
		    bReturnValue = FALSE;

	/* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_WeekNDay: exit \r\n");
	#endif
	return bReturnValue;
}


///** validates parameters for special event datatype */
BACNET_ERROR_CODE Validate_SpecialEvent(
	ListOfSpecialEvent_t *pstSplEvent,
	uint32_t *pu32FirstFaildElementNo)
{
	/* local variables */
	BACNET_ERROR_CODE eErrCode = MAX_BACNET_ERROR_CODE;
	BACnetTimeValue_t *pstListOfTimeValues = NULL;
	BACnetTimeValue_t *pstTimeValuesNext = NULL;
	int32_t i32Diff = 0;
	uint32_t u32InnerLoop = 0;
	uint32_t u32OuterLoop = 0;
	uint32_t u32ListCount = 0;
	bool bFlag = FALSE;

	/* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_SpecialEvent: entry \r\n");
	#endif

	/* reset count */
	*pu32FirstFaildElementNo = 0;

	/* traverse list & validate */
    while(NULL != pstSplEvent)
    {
		/* increment first failed number */
		(*pu32FirstFaildElementNo)++;

		/* validate date */
        if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_DATE)
        {
            if(!Validate_Date_Combinations(&(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDate)))
            {
				eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
            }
			else if(Is_Any_Date(&(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDate)))
			{
				eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
			}
        }
        /* validate date range */
        else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_DATE_RANGE)
        {
            if( !Validate_Date_Combinations(&(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stStartDate)) ||
                !Validate_Date_Combinations(&(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stEndDate)) )
            {
                eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
                break;
            }
			/* Validate Daterange for date pattern & Special date value */
			/* daterange should not allow special date or date pattern */
			else if(
				(!Is_Any_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stStartDate) &&
				 !Validate_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stStartDate))
				 ||
				 (!Is_Any_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stEndDate) &&
				 !Validate_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stEndDate))
				 )
			{
				eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
                break;
			}
			/* check if both dates are completely specific */
			else if(Validate_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stStartDate) &&
				Validate_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stEndDate)
				)
			{
				/* compares two dates & return error if date1 is after date2 */
				if(DateTime_Compare_Date(&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stStartDate,
					&pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stDateRange.m_stEndDate) > BACNET_ZERO)
				{
					eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
					break;
				}
			}
        }
        /* validate week n day */
        else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_WEEK_N_DAY)
        {
            if(!Validate_WeekNDay(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay.m_eMonth,
                                pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay.m_eWeekOfMonth,
                                pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay.m_eWeekNDay))
            {
                eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
                break;
            }
        }
		/* calendar reference */
        else if(pstSplEvent->m_stListSpecialEvent.m_eStatusCalendar == STATUS_CAL_REFF)
        {
            if(pstSplEvent->m_stListSpecialEvent.m_stPeriod.m_stCalReff.m_u32ObjId > 4194302)
            {
				eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
                break;
            }
			else if(OBJECT_CALENDAR != pstSplEvent->m_stListSpecialEvent.m_stPeriod.
				m_stCalReff.m_eObjectType)
			{
				eErrCode = ERROR_CODE_INCONSISTENT_PARAMETERS;
                break;
			}
        }
        else
        {
			/* invalid option */
			eErrCode = ERROR_CODE_INVALID_DATA_TYPE;
			break;
        }

		/* validate list of time values */
		pstListOfTimeValues = &(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues);
		while(NULL != pstListOfTimeValues)
		{
			/* validate time value */
			if(!Validate_Time(&pstListOfTimeValues->m_stTime))
			{
				eErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				bFlag = TRUE;
				break;
			}
			/* check value datatype */
			else if(pstListOfTimeValues->m_stPropValue.m_TagType > BACNET_APPLICATION_TAG_REAL &&
				pstListOfTimeValues->m_stPropValue.m_TagType != BACNET_APPLICATION_TAG_ENUMERATED)
			{
				eErrCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
				bFlag = TRUE;
				break;
			}
			/* move to next node */
			pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
		}

        if(u32ListCount > g_stStackMaxLimits.m_u32MaxTimeValueList)
		{
			eErrCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
			break;
		}

		/* validate duplicates entries in time values list */
		pstListOfTimeValues = &(pstSplEvent->m_stListSpecialEvent.m_stListOfTimeValues);
		if(u32ListCount > BACNET_TWO)
		{
			/* nested loops */
			for(u32OuterLoop = 0; u32OuterLoop < (u32ListCount-1); u32OuterLoop++)
			{
				/* get pointer of next node */
				pstTimeValuesNext = pstListOfTimeValues->m_pstNext;
				/* break if null value */
				if(NULL == pstTimeValuesNext)
				{
					break;
				}

				for(u32InnerLoop = 0; u32InnerLoop < (u32ListCount-1); u32InnerLoop++)
				{
					/* check if same time */
					i32Diff = DateTime_Compare_Time(&pstListOfTimeValues->m_stTime,
						&pstTimeValuesNext->m_stTime);
					if(i32Diff == 0)
					{
						/* values have same time */
						eErrCode = ERROR_CODE_DUPLICATE_ENTRY;
						bFlag = TRUE;
						break;
					}

					/* move to next node */
					pstTimeValuesNext = pstTimeValuesNext->m_pstNext;
					/* break if null value */
					if(NULL == pstTimeValuesNext)
					{
						break;
					}
				}// inner for loop

				/* move to next node */
				pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
				/* break if null value */
				if(NULL == pstListOfTimeValues || bFlag)
				{
					break;
				}
			}// outer for loop
		}
		else if(BACNET_TWO == u32ListCount)
		{
			/* get pointer of next node */
			pstTimeValuesNext = pstListOfTimeValues->m_pstNext;

			/* check if same time */
			i32Diff = DateTime_Compare_Time(&pstListOfTimeValues->m_stTime,
				&pstTimeValuesNext->m_stTime);
			if(i32Diff == 0)
			{
				/* values have same time */
				eErrCode = ERROR_CODE_DUPLICATE_ENTRY;
				bFlag = TRUE;
				break;
			}
		}
		else; // do nothing

		/* reset list count, break loop if error */
		u32ListCount = 0;
		/* break loop if error */
		if(bFlag) {break;}

		/* move to next node */
        pstSplEvent = pstSplEvent->m_pstNext;
    }// while ends.

	/* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_SpecialEvent: exit \r\n");
	#endif
    return eErrCode;
}

/**
*                                                                         
* DESCRIPTION                                                              
* Function returns the property access type if property is supported
* when USER_DEFINED_ACCESS_TYPE macro is enabled.
*    
* @param eObjectType - Type of Object of which property needs to be Initialize.
* @param eDevProp - Property of object which is to be initialize.
* @param eAccessType - Access type value of property obtained from
*						GetDefndPropAccess(). same value is returned if property
*						is supported else NOT_SUPPORTED is returned.
*
* @return NOT_SUPPORTED/READ_ONLY/READ_WRITE/BACNET_DEFAULT/COMMANDABLE_PROP    
*
*/  
PROP_ACCESS_TYPE Validate_PropertySupport(
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID eDevProp, 
	PROP_ACCESS_TYPE eAccessType)
{
	/* function entry message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_PropertySupport : Entry \r\n"); 
	#endif

	/* check if device object */
    if(eObjectType == OBJECT_DEVICE)
    {
        /* device object properties */
        switch(eDevProp)
        {
#ifdef BACDEL_PR14
			case PROP_PROPERTY_LIST:
#endif
            case PROP_OBJECT_IDENTIFIER:
            case PROP_OBJECT_NAME:
            case PROP_OBJECT_TYPE:
            case PROP_SYSTEM_STATUS:
            case PROP_VENDOR_NAME:
            case PROP_VENDOR_IDENTIFIER:
            case PROP_MODEL_NAME:
            case PROP_FIRMWARE_REVISION:
            case PROP_APPLICATION_SOFTWARE_VERSION:
            case PROP_APDU_TIMEOUT:
            case PROP_NUMBER_OF_APDU_RETRIES:
            case PROP_DEVICE_ADDRESS_BINDING:
            case PROP_DATABASE_REVISION:
            case PROP_PROTOCOL_VERSION:
            case PROP_PROTOCOL_REVISION:
            case PROP_PROTOCOL_SERVICES_SUPPORTED:
            case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
            case PROP_OBJECT_LIST:
            case PROP_MAX_APDU_LENGTH_ACCEPTED:
            case PROP_SEGMENTATION_SUPPORTED:

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
			case PROP_ACTIVE_COV_SUBSCRIPTIONS:
#endif
#ifdef SEGMENTATION_SUPPORTED
			case PROP_MAX_SEGMENTS_ACCEPTED:
			case PROP_APDU_SEGMENT_TIMEOUT:
#endif
#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)
            case PROP_LOCAL_TIME:
            case PROP_LOCAL_DATE:
#endif
#ifdef BACDEL_SER_DM_UTC_B
            case PROP_UTC_OFFSET:
            case PROP_DAYLIGHT_SAVINGS_STATUS:
#endif
            break;
            case PROP_MAX_MASTER:
            {
            	break;
            }
            case PROP_MAX_INFO_FRAMES:
            {
            	break;
            }

#ifdef OPTIONAL_PROPERTY
            case PROP_LOCATION:
            case PROP_DESCRIPTION:
            case PROP_PROFILE_NAME:


#ifdef BACDEL_PR14
			case PROP_SERIAL_NUMBER:
#endif
#endif
				break;

            default:
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Validate_PropertySupport : \
                    Property Match Not Found \r\n");
				#endif
                return NOT_SUPPORTED;
        }
    }
#ifdef BACDEL_OBJ_NP
	/* check if network port object */
	else if(eObjectType == OBJECT_NETWORK_PORT)
    {
        /* network port object properties */
        switch(eDevProp)
        {
#ifdef BACDEL_PR14
			case PROP_PROPERTY_LIST:
#endif
            case PROP_OBJECT_IDENTIFIER:
            case PROP_OBJECT_NAME:
            case PROP_OBJECT_TYPE:
            case PROP_STATUS_FLAGS:
            case PROP_RELIABILITY:
            case PROP_OUT_OF_SERVICE:
            case PROP_NETWORK_TYPE:
            case PROP_PROTOCOL_LEVEL:
            case PROP_NETWORK_NUMBER:
            case PROP_NETWORK_NUMBER_QUALITY:
            case PROP_CHANGES_PENDING:
            case PROP_APDU_LENGTH:
            case PROP_LINK_SPEED:
// OPTIONAL_PROPERTY
            case PROP_DESCRIPTION:
            case PROP_PROFILE_NAME:
            case PROP_MAC_ADDRESS:
            case PROP_BACNET_IP_MODE:
            case PROP_IP_ADDRESS:
            case PROP_BACNET_IP_UDP_PORT:
            case PROP_IP_SUBNET_MASK:
			case PROP_IP_DEFAULT_GATEWAY:
            case PROP_IP_DNS_SERVER:
            case PROP_IP_DHCP_ENABLE:
            break;

			case PROP_MAX_MASTER:
			{
				break;
			}
			case PROP_MAX_INFO_FRAMES:
			{
				break;
			}

#ifdef BBMD_ENABLED
            case PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE:
            case PROP_BBMD_ACCEPT_FD_REGISTRATIONS:
            case PROP_BBMD_FOREIGN_DEVICE_TABLE:
#endif
            case PROP_FD_BBMD_ADDRESS:
            case PROP_FD_SUBSCRIPTION_LIFETIME:

				break;

            default:
				#if(defined DEBUG_PRINTF && DL_1)
                Print_DebugMsg(DEBUG_LEVEL1, BACDEL_PROPERTY_NOT_PRESENT,
				"APDU: Validate_PropertySupport: Property Match Not Found\r\n");
				#endif
                return NOT_SUPPORTED;
        }
    }
#endif /* NP */
    else
    {
        /* other objects properties */
        switch(eDevProp)
        {
            case PROP_OBJECT_IDENTIFIER:
            case PROP_OBJECT_NAME:
            case PROP_OBJECT_TYPE:
            case PROP_PRESENT_VALUE:
            case PROP_STATUS_FLAGS:
            case PROP_EVENT_STATE:
            case PROP_OUT_OF_SERVICE:
            case PROP_NUMBER_OF_STATES:
            case PROP_PRIORITY_ARRAY:
            case PROP_RELINQUISH_DEFAULT:
            case PROP_UNITS:
#ifdef BACDEL_PR14
					case PROP_PROPERTY_LIST:
#endif
#ifdef BACDEL_PR18
			case PROP_CURRENT_COMMAND_PRIORITY:
#endif
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
            case PROP_COV_INCREMENT:
#endif

#ifdef OPTIONAL_PROPERTY
            case PROP_DESCRIPTION:
            case PROP_STATE_TEXT:
            case PROP_MIN_PRES_VALUE:
            case PROP_RESOLUTION:
            case PROP_PROFILE_NAME:
            case PROP_POLARITY:
            case PROP_INACTIVE_TEXT:
            case PROP_ACTIVE_TEXT:
            case PROP_CHANGE_OF_STATE_TIME:
            case PROP_CHANGE_OF_STATE_COUNT:
            case PROP_TIME_OF_STATE_COUNT_RESET:
            case PROP_ELAPSED_ACTIVE_TIME:
            case PROP_TIME_OF_ACTIVE_TIME_RESET:
            case PROP_MINIMUM_OFF_TIME:
            case PROP_MINIMUM_ON_TIME:
            case PROP_DEVICE_TYPE:
			#if (defined BACDEL_OBJ_AI)
			case PROP_UPDATE_INTERVAL:
			#endif
#endif

#if (defined OPTIONAL_PROPERTY )
            case PROP_MAX_PRES_VALUE:
#endif 
           
#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B || defined BACDEL_OBJ_SDL)
            case PROP_RELIABILITY:
#endif

#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
            case PROP_TIME_DELAY:
			case PROP_HIGH_LIMIT:
            case PROP_LOW_LIMIT:
            case PROP_DEADBAND:
            case PROP_LIMIT_ENABLE:
            case PROP_ALARM_VALUE:
            case PROP_ALARM_VALUES:
            case PROP_FAULT_VALUES:
            case PROP_FEEDBACK_VALUE:
			case PROP_TIME_DELAY_NORMAL:
			case PROP_FAULT_HIGH_LIMIT:	
			case PROP_FAULT_LOW_LIMIT:
#endif

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ESUM_A) || (defined BACDEL_SER_AE_ESUM_B))
            case PROP_NOTIFICATION_CLASS:
#endif
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B))
            case PROP_EVENT_ENABLE:
#endif
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_A))
            case PROP_ACKED_TRANSITIONS:
#endif
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ASUM_B))
            case PROP_NOTIFY_TYPE:
#endif
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B))
            case PROP_EVENT_TIME_STAMPS:
#endif

#ifdef BACDEL_OBJ_NC
            case PROP_RECIPIENT_LIST:
			case PROP_PRIORITY:
			case PROP_ACK_REQUIRED:
#endif

#ifdef BACDEL_OBJ_CAL
            case PROP_DATE_LIST:
#endif
#ifdef BACDEL_OBJ_SDL
            case PROP_EFFECTIVE_PERIOD:
            case PROP_WEEKLY_SCHEDULE:
            case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
            case PROP_SCHEDULE_DEFAULT:
            case PROP_EXCEPTION_SCHEDULE:
#endif
#if (defined BACDEL_OBJ_SDL )
            case PROP_PRIORITY_FOR_WRITING:
#endif
				break;

            default:
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Validate_PropertySupport : \
                    Property Match Not Found\r\n");
				#endif
                return NOT_SUPPORTED;
        }
    }

	/* function exit message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_PropertySupport : Exit \r\n"); 
	#endif
    return eAccessType;
}

/***
*
*DESCRIPTION
*   This function validates if proper date value is received.
*
*@param pstDate	[in] Date received.
*@return - true on validation success else false.
*
***/
bool Validate_Date_Combinations(BACnetDate_t *pstDate)
{
	/* local variables */
	uint8_t u8MonthDays = 0;
	uint8_t u8WeekDay = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_Date_Combinations: entry \r\n");
	#endif

	if(NULL == pstDate)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: "
		"Validate_Date_Combinations: null input pointers. \r\n");
		#endif
		return FALSE;
	}

	/* validate date */
	if((pstDate->m_u16Year < 1900 || pstDate->m_u16Year > 2155) && pstDate->m_u16Year != 255)
		return FALSE;
	if((pstDate->m_u8Month < BACNET_MONTH_JANUARY || pstDate->m_u8Month > BACNET_MONTH_EVENMONTH) && pstDate->m_u8Month != 255)
		return FALSE;
	if((pstDate->m_u8Wday < BACNET_WEEKDAY_MONDAY || pstDate->m_u8Wday > BACNET_WEEKDAY_SUNDAY) && pstDate->m_u8Wday != 255)
		return FALSE;
    if((/*pstDate->m_u8Day < 0 ||*/ pstDate->m_u8Day > 34) && pstDate->m_u8Day != 255)
		return FALSE;
	/* get total days in given month for given year */
    if(pstDate->m_u16Year != 255 && (pstDate->m_u8Month != 255 && pstDate->m_u8Month < BACNET_MONTH_ODDMONTH)  &&
        (pstDate->m_u8Day != 255 && pstDate->m_u8Day < 32) && (pstDate->m_u8Wday != 255 && pstDate->m_u8Wday <= BACNET_WEEKDAY_SUNDAY))
    {
	    u8MonthDays = Month_Days(pstDate->m_u16Year, pstDate->m_u8Month);
	    if((pstDate->m_u8Day < 1 || pstDate->m_u8Day > u8MonthDays) && pstDate->m_u8Day != 255)
		    return FALSE;
	    /* now cross check if weekday is valid */
	    u8WeekDay = Day_Of_Week(pstDate->m_u16Year, pstDate->m_u8Month, pstDate->m_u8Day);
	    if((pstDate->m_u8Wday != u8WeekDay) && pstDate->m_u8Wday != 255)
		    return FALSE;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_Date_Combinations: exit \r\n");
	#endif
	return TRUE;
}

/**
*
* DESCRIPTION
*   Function returns true if current Date is same as Effective system date
*   i.e. it is valid active date same as system date.
*
* @param pstDate        [in] hold current date.
* @paramp pstSyetemDate [in] System Date .
*
*/
bool Is_Active_Date(BACnetDate_t *pstDate, BACnetDate_t *pstSyetemDate)
{
    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Is_Active_Date : Entry \r\n");
	#endif

	/* check if active date */
    if(IsValidCurrentDate(pstSyetemDate, pstDate, EXACT))
	{
        return TRUE;
	}

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Is_Active_Date : Exit \r\n");
	#endif
    return FALSE;
}

/**
*
* DESCRIPTION
*   Function returns true if current Date is in Effective Period i.e. it is
*   valid active date between given date range.
*
* @param pstDateRange [in]  hold start Date and end date.
* @paramp pstSyetemDate [in] System Date .
*
*/
bool Is_Active_DateRange(BACnetDateRange_t *pstDateRange, BACnetDate_t *pstSyetemDate)
{
    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Is_Active_DateRange : Entry \r\n");
	#endif

	/* check date range */
    if(IsValidCurrentDate(pstSyetemDate,&(pstDateRange->m_stStartDate),BEFORE)
        &&
       IsValidCurrentDate(pstSyetemDate,&(pstDateRange->m_stEndDate),AFTER))
	{
        return TRUE;
	}

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "Is_Active_DateRange : Exit \r\n");
	#endif
    return FALSE;
}

/**
*
* DESCRIPTION
* Function returns true if Week-N-Day is equal t current date.
*
* @param pstWeekNDay [in]  Week-N-Day value.
* @paramp pstSystemDate [in] System Date .
*
*/
bool IsWeekInDay(WeekNDay_t *pstWeekNDay,
	BACnetDate_t *pstSystemDate)
{
	/* local variables */
    uint8_t u8LastDayOfMonth = 0;
    bool bIsActiveMonth = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "IsWeekInDay : Entry \r\n");
	#endif

    /* check if month is odd */
    if(pstWeekNDay->m_eMonth == BACNET_MONTH_ODDMONTH)
    {
        if(pstSystemDate->m_u8Month % 2 != 0)
            bIsActiveMonth = true;
    }
    /* check if month is even */
    else if(pstWeekNDay->m_eMonth == BACNET_MONTH_EVENMONTH)
    {
        if(pstSystemDate->m_u8Month % 2 == 0)
            bIsActiveMonth = true;
    }
    /* check if any month then return true */
    else if(pstWeekNDay->m_eMonth == BACNET_MONTH_ANYMONTH)
    {
        bIsActiveMonth = true;
    }
    /* match month */
    else if(pstWeekNDay->m_eMonth < BACNET_MONTH_ODDMONTH)
    {
         if(pstWeekNDay->m_eMonth == pstSystemDate->m_u8Month)
            bIsActiveMonth = true;
    }
	/* check if active match */
    if(bIsActiveMonth)
    {
		/* 1st week */
        if(pstWeekNDay->m_eWeekOfMonth == DAYS_NUM_1_7)
        {
            if((pstSystemDate->m_u8Day < 8) &&
                (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
                return true;
            else
                return false;
        }
		/* 2nd week */
        else if(pstWeekNDay->m_eWeekOfMonth == DAYS_NUM_8_14)
        {
            if((pstSystemDate->m_u8Day > 7) && (pstSystemDate->m_u8Day < 15) &&
                (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
                return true;
            else
                return false;
        }
		/* 3rd week */
        else if(pstWeekNDay->m_eWeekOfMonth == DAYS_NUM_15_21)
        {
            if((pstSystemDate->m_u8Day > 14) && (pstSystemDate->m_u8Day < 22) &&
                (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
                return true;
            else
                return false;
        }
		/* 4th week */
        else if(pstWeekNDay->m_eWeekOfMonth == DAYS_NUM_22_28)
        {
            if((pstSystemDate->m_u8Day > 21) && (pstSystemDate->m_u8Day < 29) &&
                (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
                return true;
            else
                return false;
        }
		/* 5th week */
        else if(pstWeekNDay->m_eWeekOfMonth == DAYS_NUM_29_31)
        {
            if((pstSystemDate->m_u8Day > 28) && (pstSystemDate->m_u8Day < 32) &&
                (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
                return true;
            else
                return false;
        }
		/* last 7 days of month */
        else if(pstWeekNDay->m_eWeekOfMonth == LAST_7_DAYS_OF_MONTH)
        {
            u8LastDayOfMonth = Month_Days(pstSystemDate->m_u16Year,pstSystemDate->m_u8Month);
            if((pstSystemDate->m_u8Day > (u8LastDayOfMonth-7)) &&
				(u8LastDayOfMonth >= pstSystemDate->m_u8Day) &&
                (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
                return true;
            else
                return false;
        }
		/* any 7 days prior to last 7 days of month */
        else if(pstWeekNDay->m_eWeekOfMonth == ANY_7_DAYS_PRIOR_TO_LAST_7_DAYS_OF_MONTH)
        {
	        u8LastDayOfMonth = Month_Days(pstSystemDate->m_u16Year, pstSystemDate->m_u8Month);
	        if((pstSystemDate->m_u8Day <= (u8LastDayOfMonth-7)) &&
		      (pstSystemDate->m_u8Day > (u8LastDayOfMonth-14)) &&
	          (u8LastDayOfMonth >= pstSystemDate->m_u8Day) &&
		      (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
		      pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
		      return true;
            else
                return false;
        }
       /* any 7 days prior to last 14 days of month */
        else if(pstWeekNDay->m_eWeekOfMonth == ANY_7_DAYS_PRIOR_TO_LAST_14_DAYS_OF_MONTH)
        {
	        u8LastDayOfMonth = Month_Days(pstSystemDate->m_u16Year, pstSystemDate->m_u8Month);
	        if((pstSystemDate->m_u8Day <= (u8LastDayOfMonth-14)) &&
		      (pstSystemDate->m_u8Day > (u8LastDayOfMonth-21)) &&
	          (u8LastDayOfMonth >= pstSystemDate->m_u8Day) &&
		      (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
		      pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
		    return true;
            else
               return false;
        }
        /* any 7 days prior to last 21 days of month */
        else if(pstWeekNDay->m_eWeekOfMonth == ANY_7_DAYS_PRIOR_TO_LAST_21_DAYS_OF_MONTH)
        {
	       u8LastDayOfMonth = Month_Days(pstSystemDate->m_u16Year, pstSystemDate->m_u8Month);
	       if((pstSystemDate->m_u8Day <= (u8LastDayOfMonth-21)) &&
		     (pstSystemDate->m_u8Day > (u8LastDayOfMonth-28)) &&
	         (u8LastDayOfMonth >= pstSystemDate->m_u8Day) &&
		     (pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
		     pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK))
		   return true;
           else
              return false;
        }
		/* any week in month */
        else if(pstWeekNDay->m_eWeekOfMonth == ANY_WEEK_OF_MONTH)
        {
            if(pstWeekNDay->m_eWeekNDay == pstSystemDate->m_u8Wday ||
				pstWeekNDay->m_eWeekNDay == ANY_DAY_OF_WEEK)
                return true;
            else
                return false;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
    "IsWeekInDay : Exit \r\n");
	#endif
    return false;
}

/**
*                                                                         
* DESCRIPTION                                                              
*   function validates deferent combinations of time.
*
*/
bool Validate_Time_Combinations(BACnetTime_t *pstTime)
{
    /* local variables */
    bool bReturnVal = TRUE;

    /* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_Time_Combinations: entry \r\n");
	#endif

	if(NULL == pstTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Validate_Time_Combinations: null input pointers. \r\n");
		#endif
		return FALSE;
	}

	/* validate time */
   if((/*pstTime->m_u8Hour < 0 ||*/ pstTime->m_u8Hour > 23) && pstTime->m_u8Hour != 255)
		return FALSE;
	if((/*pstTime->m_u8Min < 0 ||*/ pstTime->m_u8Min > 59) && pstTime->m_u8Min != 255)
		return FALSE;
    if((/*pstTime->m_u8Sec < 0 ||*/ pstTime->m_u8Sec > 59) && pstTime->m_u8Sec != 255)
		return FALSE;
	if(/*pstTime->m_u8Hundredths < 0 ||*/ pstTime->m_u8Hundredths > 255)
		return FALSE;

    /* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_Time_Combinations: exit \r\n");
	#endif
	return bReturnVal;
}

/**
*
* DESCRIPTION
*   Function returns true if current Date is in Effective Period i.e. it is
*   valid active date between given date range.
*
* @param pstDateRange [in]  hold start Date and end date.
* @paramp pstSyetemDate [in] System Date .
*
*/
bool Is_ActiveDate(BACnetDateRange_t *pstDateRange, BACnetDate_t *pstSyetemDate)
{
	/* function enter */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_ActiveDate : Entry \r\n");
	#endif

	if(IsValidCurrentDate(pstSyetemDate,&(pstDateRange->m_stStartDate),BEFORE)
	&&
	IsValidCurrentDate(pstSyetemDate,&(pstDateRange->m_stEndDate),AFTER))
	return TRUE;

    #ifdef DEBUG_PRINTF
	/* function exit */
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_ActiveDate : Exit \r\n");
	#endif
	return FALSE;
}

/**
*                                                                         
*	DESCRIPTION                                                              
*   function validates max apdu length values.
*
*/
bool Validate_Max_APDU_Length(uint32_t u32ApduLength, void *pVirtualDev)
{	
	/* local variables */
	#ifdef BACDEL_OBJ_NP
	virtualDevData_t *pstVirtualDev = NULL;
	NetworkPort_t *pstNpObj = NULL;
	#endif

	if(u32ApduLength != 50 && u32ApduLength != 128 && u32ApduLength != 206 &&
		u32ApduLength != 480 && u32ApduLength != 1024 && u32ApduLength != 1476)
	{
		return FALSE;
	}
	
	#ifdef BACDEL_OBJ_NP
	/* get base address of 1st NP object */
	pstVirtualDev = (virtualDevData_t *)pVirtualDev;
	pstNpObj = pstVirtualDev->m_stSupportedObjects.m_pstNetworkPort;
	while(pstNpObj->pstNext != NULL)
	{
		/* check the length */
		if(u32ApduLength > pstNpObj->m_stAPDULength.m_u32Val)
		{
			return FALSE;
		}
		/* move to next node */
		pstNpObj = pstNpObj->pstNext;
	}
	#endif
	return TRUE;
}

/** validates parameters for daily schedule array datatype */
BACNET_ERROR_CODE Validate_DailySchedule_Array(
	Pr_ListOfBACnetDailySchedule_t *pstWeeklySchedule,
	bool bArrIndxPresent,
	uint32_t u32ArrayIndex)
{
	/* local variables */
	BACnetTimeValue_t *pstTimeValue = NULL;
	BACnetTimeValue_t *pstTimeValueNext = NULL;
	int32_t i32Diff = 0;
	uint32_t u32InnerLoop = 0;
	uint32_t u32OuterLoop = 0;
	uint32_t u32Count = 0;
	uint32_t u32ListCount = 0;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	bool bErrorFlag = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Validate_DailySchedule_Array: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstWeeklySchedule)
	{
		/* empty value is allowed, skip further validation */
		return eErrorCode;
	}
    /*Validate Array Index*/
    if(bArrIndxPresent)
    {
        if(u32ArrayIndex < 1 || u32ArrayIndex > BACNET_ARRAY_OF_SEVEN)
       {
            return ERROR_CODE_INVALID_ARRAY_INDEX;
        }
    }

	/* count no of list elements received */
	for(u32Count = 0; u32Count < BACNET_ARRAY_OF_SEVEN; u32Count++)
	{
		if(pstWeeklySchedule->m_stTimeValue[u32Count].bIsUsed == TRUE)
		{
			/* increment count */
			u32ListCount++;
		}
	}

	/* validate array index */
	if((ARRAY_INDEX_PRESENT == bArrIndxPresent && u32ArrayIndex == BACNET_ZERO)
		||
	   (ARRAY_INDEX_PRESENT == bArrIndxPresent && u32ListCount > BACNET_ONE)
	    /*||
	   (ARRAY_INDEX_ABSENT == bArrIndxPresent && u8ArrInxCount != BACNET_ARRAY_OF_SEVEN)*/)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INCONSISTENT_PARAMETERS, "APDU: \
		Validate_DailySchedule_Array: inconsistent parameters \r\n");
		#endif
		/* set error code as per test case "9.22.2.X2 Resizing a writable fixed size
		   array property" of "BTL Specified Tests-15.2.final.pdf" document */
		return ERROR_CODE_INVALID_ARRAY_INDEX;
	}

	/* validate max list count */
	u32ListCount = 0;
	for(u32Count = 0; u32Count < BACNET_ARRAY_OF_SEVEN; u32Count++)
	{
		/* check if entry is used */
		if(pstWeeklySchedule->m_stTimeValue[u32Count].bIsUsed == TRUE)
		{
			/* reset count */
			u32ListCount = 0;

			/* get pointer to 1st node */
			pstTimeValue = &(pstWeeklySchedule->m_stTimeValue[u32Count]);
			while(NULL != pstTimeValue)
			{
				/* increment Inner Loop count */
				u32ListCount++;

				/* validate time */
				if(!Validate_Time(&pstTimeValue->m_stTime))
				{
				   eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				   bErrorFlag = TRUE;
				   break;
				}

				/* Datatype validation */
				if(pstTimeValue->m_stPropValue.m_TagType > BACNET_APPLICATION_TAG_REAL &&
				   pstTimeValue->m_stPropValue.m_TagType != BACNET_APPLICATION_TAG_ENUMERATED)
				{
					eErrorCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
					bErrorFlag = TRUE;
					break;
				}
				/* move to next node */
				pstTimeValue = pstTimeValue->m_pstNext;
			}
		}

		/* break loop if error */
		if(bErrorFlag)
			break;
		/* if no of values exceed max allowed limit, return error */
		if(u32ListCount > g_stStackMaxLimits.m_u32MaxTimeValueList)
		{
			eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
			break;
		}

		/* validate duplicates entries in time values list */
		pstTimeValue = &(pstWeeklySchedule->m_stTimeValue[u32Count]);
		if(u32ListCount > BACNET_TWO)
		{
			/* nested loops */
			for(u32OuterLoop = 0; u32OuterLoop < (u32ListCount-1); u32OuterLoop++)
			{
				/* get pointer of next node */
				pstTimeValueNext = pstTimeValue->m_pstNext;
				/* break if null value */
				if(NULL == pstTimeValueNext)
				{
					break;
				}

				for(u32InnerLoop = 0; u32InnerLoop < (u32ListCount-1); u32InnerLoop++)
				{
					/* check if same time */
					i32Diff = DateTime_Compare_Time(&pstTimeValue->m_stTime,
						&pstTimeValueNext->m_stTime);
					if(i32Diff == 0)
					{
						/* values have same time */
						eErrorCode = ERROR_CODE_DUPLICATE_ENTRY;
						bErrorFlag = TRUE;
						break;
					}

					/* move to next node */
					pstTimeValueNext = pstTimeValueNext->m_pstNext;
					/* break if null value */
					if(NULL == pstTimeValueNext)
					{
						break;
					}
				}// inner for loop

				/* move to next node */
				pstTimeValue = pstTimeValue->m_pstNext;
				/* break if null value */
				if(NULL == pstTimeValue || bErrorFlag)
				{
					break;
				}
			}// outer for loop
		}
		else if(BACNET_TWO == u32ListCount)
		{
			/* get pointer of next node */
			pstTimeValueNext = pstTimeValue->m_pstNext;

			/* check if same time */
			i32Diff = DateTime_Compare_Time(&pstTimeValue->m_stTime,
				&pstTimeValueNext->m_stTime);
			if(i32Diff == 0)
			{
				/* values have same time */
				eErrorCode = ERROR_CODE_DUPLICATE_ENTRY;
				bErrorFlag = TRUE;
				printf("\r\n[DUPLICATE DETECTED]\r\n");
				printf("Day Index: %lu\r\n", u32Count);
				printf("List Count: %lu\r\n", u32ListCount);
				printf("ErrorCode Set: %d\r\n", eErrorCode);
				break;
			}
		}
		else; // do nothing

		/* reset list count, break loop if error */
		u32ListCount = 0;
		if(bErrorFlag) {break;}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Validate_DailySchedule_Array: exit \r\n");
	#endif
	printf("\r\n[Validate_DailySchedule_Array EXIT]\r\n");
	printf("Returning ErrorCode: %d\r\n", eErrorCode);
	printf("-------------------------------------\r\n");
	return eErrorCode;
}

/** validates parameters for DevObjPropRef data type */
BACNET_ERROR_CODE Validate_DevObjPropRef(
	BACnetDevObjPropRef_t *pstDevObjPropRef)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_DevObjPropRef: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstDevObjPropRef)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Validate_DevObjPropRef: Null input pointer. \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}

	/* we donot support this functionality currently */
	if(pstDevObjPropRef->m_bDeviceIdPresent)
	{
		eErrorCode = ERROR_CODE_OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED;
	}

	/* check if correct device type */
	if(pstDevObjPropRef->m_bDeviceIdPresent &&
		OBJECT_DEVICE != pstDevObjPropRef->m_eDeviceType) 
	{
		eErrorCode = ERROR_CODE_INCONSISTENT_PARAMETERS;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_DevObjPropRef: exit \r\n");
	#endif
	return eErrorCode;	
}

/** validates parameters for event parameters datatype */
BACNET_ERROR_CODE Validate_EventParameters(
	BACnetEventParameter_t *pstEventParameter)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	uint8_t u8ListCount = 0;
	bool bFlag = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_EventParameters: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstEventParameter)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Validate_EventParameters: Null input pointer. \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}

	/* validate event type */
	switch (pstEventParameter->m_eEventType)
	{
	case EVENT_CHANGE_OF_STATE:
	{
		/* local variable */
		ListOfBACnetPropertyStates_t *pstCngStateVal = NULL;
		pstCngStateVal = pstEventParameter->
			BACnetEventParameter_u.m_stCngState.m_pstListOfValues;

		while(NULL != pstCngStateVal)
		{
			/* increment count by 1 */
			u8ListCount++;

			switch(pstCngStateVal->m_stPropStateVal.m_ePropState)
			{
				/* supported datatypes */
			case PROP_STATE_BINARY_VALUE:
			case PROP_STATE_UNSIGNED_VALUE :
				break;

				/* default */
			default:
				/* set error code and flag */
				eErrorCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
				bFlag = TRUE;
				break;
			}

			/* break loop if error */
			if(bFlag) {break;} 

			/* move to next node */
			pstCngStateVal = pstCngStateVal->m_pstNext;
		}
        
		/* if no of values exceed max allowed limit, return error */
		if(u8ListCount > g_stStackMaxLimits.m_u32MaxEventParaList)
		{
			eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
		}
	}
	break;

	case EVENT_COMMAND_FAILURE:
	{
		/* local variable */
		CommandFailure_t *pstCammandFailure = NULL;
		pstCammandFailure = &pstEventParameter->BACnetEventParameter_u.m_stCmdFail;

		/* we donot support this functionality currently */
		if(pstCammandFailure->m_stFeedbackPropertyReference.m_bDeviceIdPresent)
		{
			eErrorCode = ERROR_CODE_OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED;
		}

		/* check if correct device type */
		if(pstCammandFailure->m_stFeedbackPropertyReference.m_bDeviceIdPresent &&
			OBJECT_DEVICE != pstCammandFailure->m_stFeedbackPropertyReference.m_eDeviceType) 
		{
		   eErrorCode = ERROR_CODE_INCONSISTENT_PARAMETERS;
		}
	}
	break;

	case EVENT_OUT_OF_RANGE:
	{
		; // nothing to validate
	}
	break;

	default:
	{
		/* algorithmic reporting does not support other 
		even types */
		eErrorCode = ERROR_CODE_OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED;
	}
	break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_EventParameters: exit \r\n");
	#endif
	return eErrorCode;	
}

/** validate parameters for calendar entry list datatype */
BACNET_ERROR_CODE Validate_CalenderEntry_List(
	ListOfBACnetCalendarEntry_t *pstDateList,
	uint32_t *pu32FirstFaildElementNo)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	uint8_t u8ListCount = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_CalenderEntry_List: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstDateList || NULL == pu32FirstFaildElementNo)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"Validate_Date_List: Null input pointer. \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}
	/* reset count */
	*pu32FirstFaildElementNo = 0;

	/* validate the values received */
    while(NULL != pstDateList)
    {
		/* increment first failed number */
		(*pu32FirstFaildElementNo)++;

		/* date range */
        if(pstDateList->m_eStatusCalendar == STATUS_DATE_RANGE)
        {
			/* validate the values received */
			if(!Validate_Date_Combinations(
				&pstDateList->m_stCalendar.m_stDateRange.m_stStartDate) ||
				!Validate_Date_Combinations(
				&pstDateList->m_stCalendar.m_stDateRange.m_stEndDate))
			{
				eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
			}
			/* Validate Daterange for date pattern */
			else if(
				(!Is_Any_Date(&pstDateList->m_stCalendar.
				m_stDateRange.m_stStartDate) &&
				!Validate_Date(&pstDateList->m_stCalendar.
				m_stDateRange.m_stStartDate))
				||
				(!Is_Any_Date(&pstDateList->m_stCalendar.
				m_stDateRange.m_stEndDate) &&
				!Validate_Date(&pstDateList->m_stCalendar.
				m_stDateRange.m_stEndDate))
				)
			{
				eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
			}
			/* check if both dates are completely specific */
			else if(Validate_Date(&pstDateList->m_stCalendar.
				m_stDateRange.m_stStartDate) &&
				Validate_Date(&pstDateList->m_stCalendar.
				m_stDateRange.m_stEndDate))
			{
				/* compares two dates & return error if date1 is after date2 */
    			if(DateTime_Compare_Date(&pstDateList->m_stCalendar.
					m_stDateRange.m_stStartDate,
					&pstDateList->m_stCalendar.m_stDateRange.m_stEndDate) > BACNET_ZERO)
				{
					eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
					break;
				}
			}
        }

		/* date */
        else if(pstDateList->m_eStatusCalendar == STATUS_DATE)
        {
            /* validate the values received */
			if(!Validate_Date_Combinations(&pstDateList->m_stCalendar.m_stDate))
			{
				eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
			}
			else if(Is_Any_Date(&pstDateList->m_stCalendar.m_stDate))
			{
				eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
			}
        }

		/* week n day */
        else if(pstDateList->m_eStatusCalendar == STATUS_WEEK_N_DAY)
        {
            if(!Validate_WeekNDay(pstDateList->m_stCalendar.m_stWeekNDay.m_eMonth,
				pstDateList->m_stCalendar.m_stWeekNDay.m_eWeekOfMonth,
				pstDateList->m_stCalendar.m_stWeekNDay.m_eWeekNDay))
			{
				eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
				break;
			}
        }

		/* increment count by 1 */
		u8ListCount++;
		/* move to next node */
        pstDateList = pstDateList->m_pstNext;
    }

	/* validate max list count */
    if(u8ListCount > g_stStackMaxLimits.m_u32MaxDateList)
	{
		eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_CalenderEntry_List: exit \r\n");
	#endif
	return eErrorCode;
}

/** validates parameters for destination list datatype */
BACNET_ERROR_CODE Validate_Destination_List(
	ListOfBACnetDestination_t *pstRecepientList,
	uint32_t *pu32FirstFaildElementNo)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	uint8_t u8ListCount = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_Destination_List: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstRecepientList || NULL == pu32FirstFaildElementNo)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"Validate_Destination_List: Null input pointer. \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}
	/* reset count */
	*pu32FirstFaildElementNo = 0;

	while(NULL != pstRecepientList)
	{
		/* increment first failed number */
		(*pu32FirstFaildElementNo)++;

		/* validate from time */
		if(!Validate_Time(&pstRecepientList->m_stFromTime))
		{
			eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
			break;
		}

		/* validate to time */
		if(!Validate_Time(&pstRecepientList->m_stToTime))
		{
			eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
			break;
		}

		/* check no of bits in days of week and transitions */
		if(pstRecepientList->m_stDaysOfWeek.m_u8UnusedBits != BACNET_ONE ||
			pstRecepientList->m_stTransitions.m_u8UnusedBits != 5)
		{
			eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
			break;
		}

		/* compares two time & return error if From time is after To time */
		if(DateTime_Compare_Time(&pstRecepientList->m_stFromTime,
			&pstRecepientList->m_stToTime) > BACNET_ZERO)
		{
			eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
			break;
		}

		/* reverse bits in byte */
		/* Note that in stack bit-string bit are used in reverse manner */
		pstRecepientList->m_stDaysOfWeek.m_u8TransBits[0] =
			Byte_Reverse_Bits(
			pstRecepientList->m_stDaysOfWeek.m_u8TransBits[0]);
		pstRecepientList->m_stTransitions.m_u8TransBits[0] =
			Byte_Reverse_Bits(
			pstRecepientList->m_stTransitions.m_u8TransBits[0]);

		/* update destination type */
		if(DESTINATION_IS_IP_ADDR ==
			pstRecepientList->m_stRecipient.m_eDestinationType)
		{
			/* */
			if(!pstRecepientList->m_stRecipient.BACnetRecipient_u.
				m_stAddress.u16net)
		    {
				if(pstRecepientList->m_stRecipient.BACnetRecipient_u.
					m_stAddress.u8mac_len)
				{
				    /* if mac is not 0 then this is destination ip address */
				    /* set destination type */
				    pstRecepientList->m_stRecipient.m_eDestinationType =
					    DESTINATION_IS_IP_ADDR;
				}
			    else
				{
				    /* if mac len is 0, then this is local broabcast address */
				    /* set destination type */
				    pstRecepientList->m_stRecipient.m_eDestinationType =
					    DESTINATION_IS_LOCAL_BROADCAST;
				}
		    }
		    else if(pstRecepientList->m_stRecipient.BACnetRecipient_u.
				m_stAddress.u16net ==
			    BACNET_GLOBAL_BROADCAST_NETWORK_NO)
		    {
				if(pstRecepientList->m_stRecipient.BACnetRecipient_u.
					m_stAddress.u8mac_len)
				{
				    /* if mac is not 0 then this is ?? */
				    /* set destination type */
				    pstRecepientList->m_stRecipient.m_eDestinationType =
					    DESTINATION_IS_UNRECOGNIZED;
				}
			    else
				{
				    /* if mac len is 0, then this is global broabcast address */
				    /* set destination type */
				    pstRecepientList->m_stRecipient.m_eDestinationType =
					    DESTINATION_IS_GLOBAL_BROADCAST;
				}
		    }
		    else
		    {
				if(pstRecepientList->m_stRecipient.BACnetRecipient_u.
					m_stAddress.u8mac_len)
				{
				    /* if mac is not 0 then this is mstp destination
				    with its mac address */
				    /* set destination type */
				    pstRecepientList->m_stRecipient.m_eDestinationType =
					    DESTINATION_IS_MSTP_DEVICE;
				}
			    else
				{
				    /* if mac len is 0, then this is remote broabcast address */
				    /* set destination type */
				    pstRecepientList->m_stRecipient.m_eDestinationType =
					    DESTINATION_IS_REMOTE_BROADCAST;
				}
		    }
		}
		else if(DESTINATION_IS_DEVICE_ID ==
			pstRecepientList->m_stRecipient.m_eDestinationType)
		{
			if(OBJECT_DEVICE != pstRecepientList->m_stRecipient.
				BACnetRecipient_u.m_stObjId.m_eObjectType)
			{
				eErrorCode = ERROR_CODE_INCONSISTENT_PARAMETERS;
				break;
			}
		}

		/* increment count by 1 */
		u8ListCount++;
		/* move to next node */
    	pstRecepientList = pstRecepientList->m_pstNext;
	}

	/* if no of values exceed max allowed Limit, return error */
    if(u8ListCount > g_stStackMaxLimits.m_u32MaxDestinationList)
	{
		eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_Destination_List: exit \r\n");
	#endif
	return eErrorCode;
}

///** validates parameters for special event list datatype */
BACNET_ERROR_CODE Validate_SpecialEvent_List(
	ListOfSpecialEvent_t  *pstListOfSpecialEvent,
	uint32_t *pu32FirstFaildElementNo,
	bool bArrIndxPresent,
	uint32_t u32ArrayIndex)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	BACnetTimeValue_t *pstListOfTimeValues = NULL;
	uint8_t u8ExpSchLenCnt = 0;
	uint8_t u8ListCount = 0;

	/* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_SpecialEvent_List: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstListOfSpecialEvent || NULL == pu32FirstFaildElementNo)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"Validate_SpecialEvent_List: Null input pointer. \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}
	/* reset count */
	*pu32FirstFaildElementNo = 0;

	/* validate special event values */
    eErrorCode = Validate_SpecialEvent(pstListOfSpecialEvent, pu32FirstFaildElementNo);
	if(MAX_BACNET_ERROR_CODE != eErrorCode)
    {
		/* reset pointer to avoid further processing */
		pstListOfSpecialEvent = NULL;
    }
	else
	{
		/* reset count */
		*pu32FirstFaildElementNo = 0;
	}

	while(NULL != pstListOfSpecialEvent)
	{
		/* increment first failed number */
		(*pu32FirstFaildElementNo)++;

		pstListOfTimeValues = &(pstListOfSpecialEvent->m_stListSpecialEvent.
			m_stListOfTimeValues);
		while(NULL != pstListOfTimeValues)
		{
			/* increment Inner Loop count */
			u8ListCount++;
			/* move to next node */
			pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
		}
		/* if no of values in time list exceed max allowed states, return error */
		if(u8ListCount > g_stStackMaxLimits.m_u32MaxTimeValueList)
		{
			eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
			break;
		}
		u8ListCount = 0; // reset count
		/* increment count */
        u8ExpSchLenCnt++;
        /* move to next node */
        pstListOfSpecialEvent = pstListOfSpecialEvent->m_pstNext;
	}

	/* if no of values exceed max allowed states, return error */
    if(u8ExpSchLenCnt > g_stStackMaxLimits.m_u32MaxExSchdList)
	{
		eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
	}

	/* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_SpecialEvent_List: exit \r\n");
	#endif
	return eErrorCode;
}

///** validate parameters for Device Object Property Ref. list datatype */
BACNET_ERROR_CODE Validate_DevObjPropRef_List(
	ListOfBACnetDevObjPropRef_t *pstListObjPropRef,
	uint32_t *pu32FirstFaildElementNo)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	uint8_t u8ListCount = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_DevObjPropRef_List: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstListObjPropRef || NULL == pu32FirstFaildElementNo)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"Validate_DevObjPropRef_List: Null input pointer. \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}
	/* reset count */
	*pu32FirstFaildElementNo = 0;

	/* validate the values received */
    while(NULL != pstListObjPropRef)
    {
		/* increment first failed number */
		(*pu32FirstFaildElementNo)++;

		/* increment count by 1 */
		u8ListCount++;

		/* check if correct device type */
		if(pstListObjPropRef->m_stDevObjPropRef.m_bDeviceIdPresent &&
			OBJECT_DEVICE != pstListObjPropRef->m_stDevObjPropRef.m_eDeviceType)
		{
		   eErrorCode = ERROR_CODE_INCONSISTENT_PARAMETERS;
		   break;
		}

		/* move to next node */
        pstListObjPropRef = pstListObjPropRef->m_pstNext;
    }

	/* validate max list count */
    if(u8ListCount > g_stStackMaxLimits.m_u32MaxDevObjPropRefList)
	{
		/* limit exceeded */
		eErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_CalenderEntry_List: exit \r\n");
	#endif
	return eErrorCode;
}

/** validates parameters for Notification Priority array datatype */
BACNET_ERROR_CODE Validate_NotifyPriority_Array(
	Pr_BACnetNotifyPriority_t *pstNotifyPriority,
	bool bArrIndxPresent,
	uint32_t u32ArrayIndex)
{
	/* local variables */
	uint8_t u8Count = 0;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_NotifyPriority_Array: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstNotifyPriority)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"Validate_NotifyPriority_Array: Null input pointer. \r\n");
		#endif
		return ERROR_CODE_INTERNAL_ERROR;
	}

	/* validate array index */
	if(ARRAY_INDEX_PRESENT == bArrIndxPresent && u32ArrayIndex == BACNET_ZERO)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: "
		"Validate_NotifyPriority_Array: cannot resize array \r\n");
		#endif
		return ERROR_CODE_INCONSISTENT_PARAMETERS;
	}

	/* validate value */
	for(u8Count = 0; u8Count < BACNET_ARRAY_OF_THREE; u8Count++)
	{
		/* validate priority value */
		if(pstNotifyPriority->m_u32Value[u8Count] > UINT8_MAX)
		{
			eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
			break;
		}
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"Validate_NotifyPriority_Array: exit \r\n");
	#endif
	return eErrorCode;
}

/** validates parameters for Last Restore Time property */
BACNET_ERROR_CODE Validate_TimeStamp(BACnetTimeStamp_t *pstTimeStamp)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_TimeStamp: entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstTimeStamp)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Validate_TimeStamp: Null input pointer. \r\n");
		#endif
		return ERROR_CODE_INTERNAL_ERROR;
	}

	/* timestamp is time */
	if(TIMESTAMP_TYPE_TIME == pstTimeStamp->m_eTimeStampType)
	{
		/* validate time */
	}
	/* timestamp is sequence no */
	else if(TIMESTAMP_TYPE_SEQUENCE_NO == pstTimeStamp->m_eTimeStampType)
	{
		/* validate sequence no */
		if(pstTimeStamp->m_stTimeStamp.m_u32SeqNo > UINT16_MAX)
		{
			eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
		}
	}
	/* timestamp is datetime */
	else if(TIMESTAMP_TYPE_DATETIME == pstTimeStamp->m_eTimeStampType)
	{
		/* validate datetime */
	}
	else
	{
		/* ideally this case should not occur */
		;//dummy statement
	}

	/* validate with default timestamp support in stack */
	/* check if correct timestamp is received */
	#ifdef STAMP_AS_TIME
	#else
	/* timestamp is datetime */
	if(TIMESTAMP_TYPE_DATETIME != pstTimeStamp->m_eTimeStampType)
	{
		eErrorCode = ERROR_CODE_INVALID_TIME_STAMP;
	}
	#endif

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_TimeStamp: exit \r\n");
	#endif
	return eErrorCode;
}

/**
*
* DESCRIPTION
* Function to check whether service type is supported or not.
*
* @param eServiceType [in]  type of service.
* @return bool [out] true if supported else false.
*
*/
bool Check_Service_Support(
	int32_t i32DevId, 
	BACNET_SERVICES_SUPPORTED eServiceType)
{
	/* local variables */
    virtualDevData_t *pVirtualDev = NULL; 
	bool bRetVal = FALSE;
	uint8_t u8ByteNo = 0;
	uint8_t u8BitNo = 0;
	uint8_t u8ByteValue = 0;
	ulong32_t ul32OffsetAddr = 0;
	
	/* function entry message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Check_Service_Support: Invalid Object type \r\n");
	#endif

    /* validate inputs */
	if(eServiceType >= MAX_BACNET_SERVICES_SUPPORTED)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
	    Check_Service_Support: Invalid Service  \r\n");
		#endif
		return bRetVal;
	}

	if(i32DevId < 0)
	{
		/* Check service support in stack */
		/* get the service size */
		/* Note : used to check service support in stack */
		if(0 == ul32OffsetAddr)
		{
			/* service type not supported */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
			Check_Service_Support: service type is invalid \r\n");
			#endif
			bRetVal = FALSE;
		}
		else 
		{
			/* service type is supported */
			bRetVal = TRUE;
		}
	}
	else
	{
		/* check if this is Host Device */
		pVirtualDev = Find_In_Host_Device_List(i32DevId);
		if(NULL == pVirtualDev)
        {
            /* device not found */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			Check_Service_Support: unknown device \r\n");
			#endif
            bRetVal = FALSE;
        }
		else
		{
			/* get the byte and bit number */
			u8ByteNo = (uint8_t)(eServiceType / 8);
			u8ByteValue = pVirtualDev->m_stDevObject.m_stServicesSupported.
				m_stServiceSupport.Byte[u8ByteNo];
			u8BitNo = eServiceType % 8;
			/* check bit value */
			bRetVal = BIT_VALUE_CHECK(u8ByteValue, u8BitNo);
		}
	}

	/* function exit message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Check_Service_Support: exit \r\n");
	#endif
	return bRetVal;
}

/**
*
* DESCRIPTION
* Function to validate service supported data type.
*
* @param i32DevId [in] Device Id.
* @param pvWritePropVal	[in] Pointer to property value.
*
* @return bool [out] true on success else false.
*
*/
bool Validate_Services_Supported(
	int32_t i32DevId,
	void *pvWritePropVal)
{
	/* local variables */
	bool bRetVal = TRUE;
	uint8_t u8ByteNo = 0;
	uint8_t u8BitNo = 0;
	uint8_t u8ByteValue = 0;
	uint8_t u8Mask = 0;
	uint8_t u8Result = 0;
	BACnetServicesSupported_t *pstServiceSupported = NULL;
	BACNET_SERVICES_SUPPORTED eServiceType = MAX_BACNET_SERVICES_SUPPORTED;

	/* function entry message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Validate_Services_Supported: Invalid Object type \r\n");
	#endif

	/* check input pointer */
	if(NULL == pvWritePropVal)
	{
		/* null pointer */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Validate_Services_Supported: Null input pointer. \r\n");
		#endif
		return FALSE;
	}

	/* typecast the pointer */
	pstServiceSupported = (BACnetServicesSupported_t *)pvWritePropVal;

	/* check value of each bit in each byte */
	for(u8ByteNo = 0; u8ByteNo < MAX_SERVICE_SUPPORT_BYTE; u8ByteNo++)
	{
		/* get byte value */
		u8ByteValue = pstServiceSupported->Byte[u8ByteNo];
		/* set the mask */
		u8Mask = 0x01;

		/* check bits in byte */
		for(u8BitNo = 0; u8BitNo < 8; u8BitNo++)
		{
			/* get bit value */
			u8Result = u8ByteValue & u8Mask;
			/* if bit value is 1, check support in stack */
			if(u8Result)
			{
				/* get service type */
				eServiceType = ((u8ByteNo * 8) + (u8BitNo));
				/* check service support in stack */
				bRetVal = Check_Service_Support(BACNET_NEGATIVE_ONE, eServiceType);
				/* break the loop if not supported */
				if(!bRetVal) 
				{
					break;
				}
			}
			/* shift the mask */
			u8Mask = u8Mask << 1;
		}

		/* break the loop if not supported */
		if(!bRetVal) 
		{ 
			break;
		}
	}

	/* function exit message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Validate_Services_Supported: exit \r\n");
	#endif
	return bRetVal;
}

/**
*
* DESCRIPTION
* Function to validate object types supported data type.
*
* @param i32DevId [in] Device Id.
* @param pvWritePropVal	[in] Pointer to property value.
*
* @return bool [out] true on success else false.
*
*/
bool Validate_ObjectTypes_Supported(
	int32_t i32DevId,
	void *pvWritePropVal)
{
	/* local variables */
	bool bRetVal = TRUE;
	uint8_t u8ByteNo = 0;
	uint8_t u8BitNo = 0;
	uint8_t u8ByteValue = 0;
	uint8_t u8Mask = 0;
	uint8_t u8Result = 0;
	BACnetObjectTypesSupported_t *pstObjTypesSupported = NULL;
	BACNET_OBJECT_TYPE eObjType = MAX_BACNET_OBJECT_TYPE;

	/* function entry message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Validate_ObjectTypes_Supported: Invalid Object type \r\n");
	#endif

	/* check input pointer */
	if(NULL == pvWritePropVal)
	{
		/* null pointer */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Validate_ObjectTypes_Supported: Null input pointer. \r\n");
		#endif
		return FALSE;
	}

	/* typecast the pointer */
	pstObjTypesSupported = (BACnetObjectTypesSupported_t *)pvWritePropVal;

	/* check value of each bit in each byte */
	for(u8ByteNo = 0; u8ByteNo < MAX_OBJECT_TYPE_BYTE; u8ByteNo++)
	{
		/* get byte value */
		u8ByteValue = pstObjTypesSupported->Byte[u8ByteNo];
		/* set the mask */
		u8Mask = 0x01;

		/* check bits in byte */
		for(u8BitNo = 0; u8BitNo < 8; u8BitNo++)
		{
			/* get bit value */
			u8Result = u8ByteValue & u8Mask;
			/* if bit value is 1, check support in stack */
			if(u8Result)
			{
				/* get object type */
				eObjType = ((u8ByteNo * 8) + (u8BitNo));
				/* check object type support in stack */
				bRetVal = BACDEL_Check_Object_Support(BACNET_NEGATIVE_ONE, eObjType);
				/* break the loop if not supported */
				if(!bRetVal) 
				{
					break;
				}
			}
			/* shift the mask */
			u8Mask = u8Mask << 1;
		}

		/* break the loop if not supported */
		if(!bRetVal) 
		{ 
			break;
		}
	}

	/* function exit message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Validate_ObjectTypes_Supported: exit \r\n");
	#endif
	return bRetVal;
}
#ifdef BACDEL_OBJ_NP
/**
*
*	DESCRIPTION
*   function validates apdu length value of network port object.
*
*/
bool Validate_APDU_Length(uint32_t u32ApduLength)
{
	/* local variables */

	/* check with standard values */
	if(u32ApduLength != 50 && u32ApduLength != 128 && u32ApduLength != 206 &&
		u32ApduLength != 480 && u32ApduLength != 1024 && u32ApduLength != 1476)
	{
		return FALSE;
	}

	/* return success */
	return TRUE;
}

/** function validates host n port value */
BACNET_ERROR_CODE Validate_HostNPort(
	Pr_BACnetHostNPort_t *pstHostNPort)
{
	/* local variable */
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: Validate_HostNPort: Entry \r\n");
	#endif

	/* null check input pointer */
    if(NULL == pstHostNPort)
	{
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
			"APDU: Validate_HostNPort: Invalid pointer \r\n");
		#endif
		eErrorCode = ERROR_CODE_INTERNAL_ERROR;
		return eErrorCode;
	}

	/* check data type */
	if(BACNET_DT_NULL_CONTEXT == pstHostNPort->m_stHostNPort.m_stHostAddress.m_eDataType ||
		BACNET_DT_CHARSTRING == pstHostNPort->m_stHostNPort.m_stHostAddress.m_eDataType)
	{
		/* data type not supported */
		eErrorCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
	}

	/* check ip address length */
	else if(pstHostNPort->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.
		m_stIPAddress.m_u32OctetCount != MAX_IP_LEN)
	{
		/* incorrect ip length */
		eErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: Validate_HostNPort: Exit \r\n");
	#endif
	return eErrorCode;
}
#endif /* NP */

/*************************** end of propertyValidations.c file ****************************/
