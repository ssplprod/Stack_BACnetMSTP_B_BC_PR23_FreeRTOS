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
******************************************************************************/
/****************************************************************************** 
*                                                                         
*   FILE
*       datetime.c  
*   AUTHORS                                                                             
*	    Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*       Manipulate BACnet Date and Time values.
*
*   RELEASE HISTORY
*	DATE        NAME            DESCRIPTION
*   05/08/2011  Ashish Verma    File Created
*
******************************************************************************/

#include "bacDELDef.h"
#include "bacDELApi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "pduDateTime.h"
#include "pduEncodeDecode.h"
#include "objDevice.h"

extern RTC_HandleTypeDef hrtc;
/* USER CODE BEGIN 0 */
typedef struct DT {
	/* Year value */
	uint32_t	m_u16Year;      /* AD */
	/* Month value */
	uint32_t		m_u8Month;      /* 1=Jan */
	/* Day value */
	uint32_t		m_u8Day;        /* 1..31 */
	/* Weekday value */
	uint32_t		m_u8Wday;       /* 1=Monday-7=Sunday */
	/* Hour value */
	uint32_t		m_u8Hour;
	/* Minutes value */
	uint32_t		m_u8Min;
	/* Second value */
	uint32_t		m_u8Sec;
	/* Hundredths value */
	uint32_t		m_u8Hundredths;
	
}DT_t;
DT_t st_Rtc1;
extern DT_t dt;
/* BACnet Date */
/* year = years since 1900 */
/* month 1=Jan */
/* day = day of month 1..31 */
/* wday 1=Monday...7=Sunday */

/* Wildcards:
  A value of X'FF' in any of the four octets 
  shall indicate that the value is unspecified.
  If all four octets = X'FF', the corresponding
  time or date may be interpreted as "any" or "don't care"
*/

/** 
*	 
*	// below structure is defined in time.h file
*	struct tm
*	{
*		int    tm_sec   Seconds [0,59].
*		int    tm_min   Minutes [0,59].
*		int    tm_hour  Hour [0,23].
*		int    tm_mday  Day of month [1,31].
*		int    tm_mon   Month of year [0,11].
*		int    tm_year  Years since 1900.
*		int    tm_wday  Day of week [0,6] (Sunday =0).
*		int    tm_yday  Day of year [0,365].
*		int    tm_isdst Daylight Savings flag.
*	}
*	typedef __time64_t time_t;      //time value 
*
**/
#ifdef BACDEL_SER_DM_TS_B
/**
*
* DESCRIPTION
* Function to receive auto response for TimeSync. service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_TimeSync
(
uint32_t u32DevId,
uint32_t u32CallbackId,
BACNET_SERVICES_SUPPORTED eServiceType,
BACNET_ERROR_CLASS *peErrorClass,
uint32_t *pu32ErrorCode,
BACNET_PDU_TYPE *pePduType,
void *pvServiceData,
void *pvServiceResp,
void *pvOtherData,
BACnetAddress_t *pvRmtDvAddr,
BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE	eRetVal = BACDEL_SUCCESS;
	time_sync_request_t *pstTimeSyncReq = NULL;
	RTC_TimeTypeDef _Time;
	RTC_DateTypeDef _Date;
	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* get pointer value */
	pstTimeSyncReq = (time_sync_request_t *)pvServiceData;

	dt.m_u16Year =  pstTimeSyncReq->m_stDate.m_u16Year;
	dt.m_u8Month =  pstTimeSyncReq->m_stDate.m_u8Month,
	dt.m_u8Day =  pstTimeSyncReq->m_stDate.m_u8Day,
	dt.m_u8Wday =	pstTimeSyncReq->m_stDate.m_u8Wday,
	dt.m_u8Hour =	pstTimeSyncReq->m_stTime.m_u8Hour,
	dt.m_u8Min =	pstTimeSyncReq->m_stTime.m_u8Min,
	dt.m_u8Sec =	pstTimeSyncReq->m_stTime.m_u8Sec,
	dt.m_u8Hundredths =	pstTimeSyncReq->m_stTime.m_u8Hundredths;

	//Set RTC Time
	_Time.Hours = dt.m_u8Hour;
	_Time.Minutes = dt.m_u8Min;
	_Time.Seconds = dt.m_u8Sec;
	_Time.SecondFraction = dt.m_u8Hundredths;
	_Time.SubSeconds = 0;
	_Time.TimeFormat = RTC_HOURFORMAT_24;
	_Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	_Time.StoreOperation = RTC_STOREOPERATION_RESET;

	if(HAL_RTC_SetTime(&hrtc, &_Time, RTC_FORMAT_BIN) != HAL_OK )
		HAL_RTC_SetTime(&hrtc, &_Time, RTC_FORMAT_BIN);

	//Set RTC Date
	_Date.Year = (uint8_t)(dt.m_u16Year % 100);
	_Date.Month = dt.m_u8Month;
	_Date.Date = dt.m_u8Day;
	_Date.WeekDay = dt.m_u8Wday;

	if(HAL_RTC_SetDate(&hrtc,&_Date,RTC_FORMAT_BIN)!= HAL_OK)
		HAL_RTC_SetDate(&hrtc, &_Date, RTC_FORMAT_BIN);

	/* return value */
	return eRetVal;

}
#endif

#ifdef BACDEL_SER_DM_UTC_B
/**
*
* DESCRIPTION
* Function to receive auto response for UTC TimeSync. service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_UTC_TimeSync
(
uint32_t u32DevId,
uint32_t u32CallbackId,
BACNET_SERVICES_SUPPORTED eServiceType,
BACNET_ERROR_CLASS *peErrorClass,
uint32_t *pu32ErrorCode,
BACNET_PDU_TYPE *pePduType,
void *pvServiceData,
void *pvServiceResp,
void *pvOtherData,
BACnetAddress_t *pvRmtDvAddr,
BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */
	utc_time_sync_request_t *pstUTCTimeSyncReq = NULL;
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;
	RTC_TimeTypeDef _Time;
	RTC_DateTypeDef _Date;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* get pointer value */
	pstUTCTimeSyncReq = (utc_time_sync_request_t *)pvServiceData;

	//Set RTC Time
	_Time.Hours = pstUTCTimeSyncReq->m_stTime.m_u8Hour;
	_Time.Minutes = pstUTCTimeSyncReq->m_stTime.m_u8Min;
	_Time.Seconds = pstUTCTimeSyncReq->m_stTime.m_u8Sec;
	_Time.SecondFraction = 0;
	_Time.SubSeconds = 0;
	_Time.TimeFormat = RTC_HOURFORMAT_24;
	_Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	_Time.StoreOperation = RTC_STOREOPERATION_RESET;

	if(HAL_RTC_SetTime(&hrtc, &_Time, RTC_FORMAT_BIN) != HAL_OK )
		HAL_RTC_SetTime(&hrtc, &_Time, RTC_FORMAT_BIN);

	//Set RTC Date
	_Date.Year = (uint8_t)((pstUTCTimeSyncReq->m_stDate.m_u16Year) % 100);
	_Date.Month = pstUTCTimeSyncReq->m_stDate.m_u8Month;
	_Date.Date = pstUTCTimeSyncReq->m_stDate.m_u8Day;
	_Date.WeekDay = pstUTCTimeSyncReq->m_stDate.m_u8Wday;

	if(HAL_RTC_SetDate(&hrtc,&_Date,RTC_FORMAT_BIN)!= HAL_OK)
		HAL_RTC_SetDate(&hrtc, &_Date, RTC_FORMAT_BIN);

	/* return value */
	return eRetVal;
}
#endif

/** function to store the current system local date-time into BACnetDate_t & BACnetTime_t format */
char *GetSystem_DateTime(BACnetDate_t *pstLocalDate, BACnetTime_t *pstLocalTime)
{
	RTC_TimeTypeDef _Time;
	RTC_DateTypeDef _Date;
	
	HAL_RTC_GetTime(&hrtc, &_Time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &_Date, RTC_FORMAT_BIN);

	pstLocalDate->m_u8Day				= _Date.Date;// st_Rtc1.m_u8Day;			/* store date */
	pstLocalDate->m_u8Wday				= _Date.WeekDay;//st_Rtc1.m_u8Wday;			/* store week day */
	pstLocalDate->m_u8Month				= _Date.Month;//st_Rtc1.m_u8Month;			/* store month */
	pstLocalDate->m_u16Year		 		= _Date.Year + 2000; //st_Rtc1.m_u16Year;			/* store year */
	
	pstLocalTime->m_u8Hour		 		= _Time.Hours;//st_Rtc1.m_u8Hour;			/* store hours */
	pstLocalTime->m_u8Min		 		= _Time.Minutes;//st_Rtc1.m_u8Min;			/* store minutes */
	pstLocalTime->m_u8Sec		 		= _Time.Seconds;//st_Rtc1.m_u8Sec;			/* store seconds */
	pstLocalTime->m_u8Hundredths		= _Time.SecondFraction;//st_Rtc1.m_u8Hundredths;	/* system does not return milliseconds so bacnet default is X'FF */
	
	return(NULL);
}

/**
*
*DESCRIPTION
*   Converts System Time structure into BACnet datetime type
*
*@param tm [in] System Time structure
*@param stDateTime [in/out] BACnet datetime structure
*
*@return VOID [out] No return 
*/
//VOID Convert_SystemTimeToBACnet(struct tm *tm, Pr_BACnetDateTime_t *stDateTime)
void Convert_SystemTimeToBACnet(void *pvTime, Pr_BACnetDateTime_t *pstDateTime)                   
{

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Convert_SystemTimeToBACnet : Entry \r\n"); 
	#endif

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Convert_SystemTimeToBACnet :"
        "Exit\r\n"); 
	#endif
}

/**
*
*DESCRIPTION
*   Converts BACnet datetime structure into System time structure
*
*@param tm [in/out] System Time structure
*@param stDateTime [in] BACnet datetime structure
*
*@return VOID [out] No return 
*/
//VOID Convert_BACnetTimeToSystem( Pr_BACnetDateTime_t *pstDateTime ,struct tm *tm)
void Convert_BACnetTimeToSystem(void *pvTime, Pr_BACnetDateTime_t *pstDateTime)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Convert_BACnetTimeToSystem : Entry \r\n"); 
	#endif

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Convert_BACnetTimeToSystem :"
        "Exit\r\n"); 
	#endif
}

/**
*
*DECRIPTION
*   This function checks if a year is a leap year or not.
*
*@param u16Year [in] Year that needs to be validated.
*@return TRUE/FALSE
*
*/
static bool Is_Leap_Year(uint16_t u16Year)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Is_Leap_Year:Entry\r\n");
	#endif

    if ((u16Year % 4) == 0 && ((u16Year % 100) != 0 || (u16Year % 400) == 0))
        return (true);
    else
        return (false);
}

/**
*
*DECRIPTION
*   This function provides the number of days in a month.
*
*@param u16Year [in] Year that needs to be validated.
*@param u8Month [in] Month that needs to be validated.
*@return Number of days in that month.
*
*/
uint8_t Month_Days(uint16_t u16Year, uint8_t u8Month)
{
    /* note: start with a zero in the first element to save us from a
       month - 1 calculation in the lookup */
    int32_t month_days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	/* function entry */
	#ifdef DEBUG_PRINTF
 	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:uint8_t Month_Days:Entry...number of days in a month\r\n");
	#endif

    /* February */
    if ((u8Month == 2) && Is_Leap_Year(u16Year))
        return 29;
    else if (u8Month >= 1 && u8Month <= 12)
        return (uint8_t) month_days[u8Month];
    else
        return 0;
}


static uint32_t Days_Since_Epoch(uint16_t u16Year, uint8_t u8Month,
                                 uint8_t u8Day)
{
    uint32_t u32Days = 0;   	/* return value */
    uint8_t u8MonthDays = 0;    /* days in a month */
    uint16_t u16Years = 0;  	/* loop counter for years */
    uint8_t u8Months = 0;   	/* loop counter for months */

    u8MonthDays = Month_Days(u16Year, u8Month);

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:uint32_t Days_Since_Epoch:Entry\r\n");
	#endif

    if ((u16Year >= 1900) && (u8MonthDays) && (u8Day >= 1) &&
        (u8Day <= u8MonthDays))
    {
        for (u16Years = 1900; u16Years < u16Year; u16Years++)
        {
            u32Days += 365;
            if (Is_Leap_Year(u16Years))
                u32Days++;
        }
        for (u8Months = 1; u8Months < u8Month; u8Months++)
        {
            u32Days += Month_Days(u16Years, u8Months);
        }
        u32Days += (u8Day - 1);
    }
    return (u32Days);
}

static void Days_Since_Epoch_Into_YMD(uint32_t u32Days, uint16_t *pu16Year, 
                                      uint8_t *pu8Month, uint8_t *pu8Day)
{
    uint16_t u16Year = 1900;
    uint8_t u8Month = 1;
    uint8_t u8Day = 1;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Days_Since_Epoch_Into_YMD:Entry\r\n");
	#endif

    while (u32Days >= 365)
    {
        if ((Is_Leap_Year(u16Year)) && (u32Days == 365))
            break;
        u32Days -= 365;
        if (Is_Leap_Year(u16Year))
            --u32Days;
        u16Year++;
    }

    while (u32Days >= (uint32_t) Month_Days(u16Year, u8Month))
    {
        u32Days -= Month_Days(u16Year, u8Month);
        u8Month++;
    }

    u8Day = (uint8_t) (u8Day + u32Days);

    if (pu16Year)
        *pu16Year = u16Year;
    if (pu8Month)
        *pu8Month = u8Month;
    if (pu8Day)
        *pu8Day = u8Day;

    return;
}


/**
*
*DECRIPTION
*   This function provides the day of the week.
*   e.g. Jan 1, 1900 is a Monday 
*        wday 1=Monday...7=Sunday
*
*@param u16Year [in] Year that needs to be validated.
*@param u8Month [in] Month that needs to be validated.
*@param u8Day [in] Day that needs to be validated.
*@return The day of that week.
*
*/
uint8_t Day_Of_Week(uint16_t u16Year, uint8_t u8Month, uint8_t u8Day)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:uint8_t Day_Of_Week:Entry\r\n");
	#endif

    return (uint8_t) ((Days_Since_Epoch(u16Year, u8Month, u8Day) % 7) + 1);
}


/**
*
*DECRIPTION
*   This function compares two dates.
*
*@param date1 [in] Date that needs to be compared.
*@param date2 [in] Date that needs to be compared.
*@return 0        if the date1 is same as date2.
*        positive if date1 is after date2.
*        negative if date1 is before date2.
*
*/
int32_t DateTime_Compare_Date(BACnetDate_t *pstdate1, BACnetDate_t *pstdate2)
{
	/* Local variables */
    int32_t i32Diff = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Compare_Date:Entry\r\n");
	#endif

    if (pstdate1 && pstdate2)
    {
        i32Diff = (int32_t) pstdate1->m_u16Year - (int32_t) pstdate2->m_u16Year;
        if (i32Diff == 0)
        {
            i32Diff = (int32_t) pstdate1->m_u8Month - (int32_t) pstdate2->m_u8Month;
            if (i32Diff == 0)
            {
                i32Diff = (int32_t) pstdate1->m_u8Day - (int32_t) pstdate2->m_u8Day;
            }
        }
    }

    return i32Diff;
}


/**
*
*DECRIPTION
*   This function compares two time.
*
*@param time1 [in] Time that needs to be compared.
*@param time2 [in] Time that needs to be compared.
*@return 0        if the time1 is same as time2.
*        positive if time1 is after time2.
*        negative if time1 is before time2.
*
*/
int32_t DateTime_Compare_Time(BACnetTime_t *psttime1, BACnetTime_t *psttime2)
{
    int32_t i32Diff = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Compare_Time:Entry\r\n");
	#endif

    if (psttime1 && psttime2)
    {
        i32Diff = (int32_t) psttime1->m_u8Hour - (int32_t) psttime2->m_u8Hour;
        if (i32Diff == 0)
        {
            i32Diff = (int32_t) psttime1->m_u8Min - (int32_t) psttime2->m_u8Min;
            if (i32Diff == 0)
            {
                i32Diff = (int32_t) psttime1->m_u8Sec - (int32_t) psttime2->m_u8Sec;
                if (i32Diff == 0)
                {
                    i32Diff = (int32_t) psttime1->m_u8Hundredths 
                        - (int32_t) psttime2->m_u8Hundredths;
                }
            }
        }
    }

    return i32Diff;
}


/**
*
*DECRIPTION
*   This function compares two Date & Time.
*
*@param datetime1 [in] Date & Time that needs to be compared.
*@param datetime2 [in] Date & Time that needs to be compared.
*@return 0        if the datetime1 is same as datetime2.
*        positive if datetime1 is after datetime2.
*        negative if datetime1 is before datetime2.
*
*/
int32_t DateTime_Compare(BACnetDateTime_t *pstdatetime1,
                         BACnetDateTime_t *pstdatetime2)
{
    int32_t i32Diff = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Compare:Entry\r\n");
	#endif

	i32Diff = DateTime_Compare_Date(&pstdatetime1->m_stDate, &pstdatetime2->m_stDate);
    if(i32Diff == 0)
    {
        i32Diff =
			DateTime_Compare_Time(&pstdatetime1->m_stTime, &pstdatetime2->m_stTime);
    }

    return i32Diff;
}

/**
*
*DECRIPTION
*   This function compares two Date & Time.
*
*@return zero     if the datetime1 is same as datetime2.
*        positive if datetime1 is after datetime2.
*        negative if datetime1 is before datetime2.
*
*/
int32_t DateTime_Compare_DateTime(
        BACnetDate_t *pstDate1, BACnetTime_t *pstTime1,
                BACnetDate_t *pstDate2, BACnetTime_t *pstTime2)
{
    int32_t i32Diff = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    DateTime_Compare_DateTime: Entry \r\n");
	#endif

    i32Diff = DateTime_Compare_Date(pstDate1, pstDate2);
    if(i32Diff == 0)
        i32Diff = DateTime_Compare_Time(pstTime1, pstTime2);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    DateTime_Compare_DateTime: Exit \r\n");
	#endif

    return i32Diff;
}

/**
*
*DECRIPTION
*   This function copies source Date to destination Date.
*
*@param dest_date [in] Destination where the date is to be copied.
*@param src_date [in] Source from where the date is to be copied.
*@return VOID.
*
*/
void DateTime_Copy_Date(BACnetDate_t *pstDest_Date, BACnetDate_t *pstSrc_Date)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Copy_Date:Entry\r\n");
	#endif

    if (pstDest_Date && pstSrc_Date)
    {
        pstDest_Date->m_u16Year = pstSrc_Date->m_u16Year;
        pstDest_Date->m_u8Month = pstSrc_Date->m_u8Month;
        pstDest_Date->m_u8Day   = pstSrc_Date->m_u8Day;
        pstDest_Date->m_u8Wday  = pstSrc_Date->m_u8Wday;
    }
}

/**
*
*DECRIPTION
*   This function copies source time to destination time.
*
*@param dest_time [in] Destination where the time is to be copied.
*@param src_time [in] Source from where the time is to be copied.
*@return VOID.
*
*/
void DateTime_Copy_Time(BACnetTime_t *pstDest_Time, BACnetTime_t *pstSrc_Time)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Copy_Time:Entry\r\n");
	#endif

    if (pstDest_Time && pstSrc_Time) {
        pstDest_Time->m_u8Hour = pstSrc_Time->m_u8Hour;
        pstDest_Time->m_u8Min = pstSrc_Time->m_u8Min;
        pstDest_Time->m_u8Sec = pstSrc_Time->m_u8Sec;
        pstDest_Time->m_u8Hundredths = pstSrc_Time->m_u8Hundredths;
    }
}

/**
*
*DECRIPTION
*   This function copies source date & time to destination date & time.
*
*@param dest_datetime [in] Destination where the date & time is to be copied.
*@param src_datetime [in] Source from where the date & time is to be copied.
*@return VOID.
*
*/
void DateTime_Copy(BACnetDateTime_t *pstDest_DateTime, 
                   BACnetDateTime_t *pstSrc_DateTime)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Copy:Entry\r\n");
	#endif

	DateTime_Copy_Time(&pstDest_DateTime->m_stTime, &pstSrc_DateTime->m_stTime);
	DateTime_Copy_Date(&pstDest_DateTime->m_stDate, &pstSrc_DateTime->m_stDate);
}

/**
*
*DECRIPTION
*   This function sets date.
*
*@param bdate [out] Format in which the date is to be set.
*@param u16Year [in] Year that needs to be set.
*@param u8Month [in] Month that needs to be set.
*@param u8Day [in] Day that needs to be set.
*@return VOID.
*
*/
void DateTime_Set_Date(BACnetDate_t *pstBDate, uint16_t u16Year, uint8_t u8Month,
                       uint8_t u8Day)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Set_Date:Entry\r\n");
	#endif

    if (pstBDate)
    {
        pstBDate->m_u16Year = u16Year;
        pstBDate->m_u8Month = u8Month;
        pstBDate->m_u8Day = u8Day;
        pstBDate->m_u8Wday = Day_Of_Week(u16Year, u8Month, u8Day);
    }
}

void DateTime_Set_Time(BACnetTime_t *pstBTime, uint8_t u8Hour, uint8_t u8Minute,
                       uint8_t u8Seconds, uint8_t u8Hundredths)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Set_Time:Entry\r\n");
	#endif

    if (pstBTime)
    {
        pstBTime->m_u8Hour = u8Hour;
        pstBTime->m_u8Min = u8Minute;
        pstBTime->m_u8Sec = u8Seconds;
        pstBTime->m_u8Hundredths = u8Hundredths;
    }
}

void DateTime_Set(BACnetDateTime_t *pstBDateTime, BACnetDate_t *pstBDate,
                  BACnetTime_t *pstBTime)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Set:Entry\r\n");
	#endif

    if (pstBDate && pstBTime && pstBDateTime)
    {
		pstBDateTime->m_stTime.m_u8Hour = pstBTime->m_u8Hour;
        pstBDateTime->m_stTime.m_u8Min = pstBTime->m_u8Min;
        pstBDateTime->m_stTime.m_u8Sec = pstBTime->m_u8Sec;
        pstBDateTime->m_stTime.m_u8Hundredths = pstBTime->m_u8Hundredths;
		pstBDateTime->m_stDate.m_u16Year = pstBDate->m_u16Year;
        pstBDateTime->m_stDate.m_u8Month = pstBDate->m_u8Month;
        pstBDateTime->m_stDate.m_u8Day = pstBDate->m_u8Day;
        pstBDateTime->m_stDate.m_u8Wday = pstBDate->m_u8Wday;
    }
}

void DateTime_Set_Values(BACnetDateTime_t *pstBDateTime, uint16_t u16Year,
                         uint8_t u8Month, uint8_t u8Day, uint8_t u8Hour,
                         uint8_t u8Minute, uint8_t u8Seconds, uint8_t u8Hundredths)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Set_Values:Entry\r\n");
	#endif

    if (pstBDateTime) {
		pstBDateTime->m_stDate.m_u16Year = u16Year;
        pstBDateTime->m_stDate.m_u8Month = u8Month;
        pstBDateTime->m_stDate.m_u8Day = u8Day;
        pstBDateTime->m_stDate.m_u8Wday = Day_Of_Week(u16Year, u8Month, u8Day);
		pstBDateTime->m_stTime.m_u8Hour = u8Hour;
        pstBDateTime->m_stTime.m_u8Min = u8Minute;
        pstBDateTime->m_stTime.m_u8Sec = u8Seconds;
        pstBDateTime->m_stTime.m_u8Hundredths = u8Hundredths;
    }
}

static uint32_t Seconds_Since_Midnight(uint8_t u8Hours, uint8_t u8Minutes,
                                       uint8_t u8Seconds)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Seconds_Since_Midnight:Entry\r\n");
	#endif

    return ((u8Hours * 60 * 60) + (u8Minutes * 60) + u8Seconds);
}

static void Seconds_Since_Midnight_Into_HMS(uint32_t u32Seconds, uint8_t *pu8Hours,
                                            uint8_t *pu8Minutes,
                                            uint8_t *pu8Seconds)
{
    uint8_t u8Hour = 0;
    uint8_t u8Minute = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Seconds_Since_Midnight_Into_HMS:Entry\r\n");
	#endif

    u8Hour = (uint8_t) (u32Seconds / (60 * 60));
    u32Seconds -= (u8Hour * 60 * 60);
    u8Minute = (uint8_t) (u32Seconds / 60);
    u32Seconds -= (u8Minute * 60);

	if(u8Hour > 23)
		u8Hour -= 24;

    if (pu8Hours)
        *pu8Hours = u8Hour;
    if (pu8Minutes)
        *pu8Minutes = u8Minute;
    if (pu8Seconds)
        *pu8Seconds = (uint8_t) u32Seconds;
}
#if (defined BACDEL_SER_DM_UTC_A || defined BACDEL_SER_DM_UTC_B)
void DateTime_Add_Minutes(utc_time_sync_request_t *pstBDateTime, uint32_t u32Minutes)
{
    uint32_t u32BDateTime_Minutes = 0;
    uint32_t u32BDateTime_Days = 0;
    uint32_t u32Days = 0;
    
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Add_Minutes:Entry\r\n");
	#endif
    
    /* convert pstBDateTime to u8Seconds and days */
    u32BDateTime_Minutes =
		Seconds_Since_Midnight(pstBDateTime->m_stTime.m_u8Hour, pstBDateTime->m_stTime.m_u8Min,
        pstBDateTime->m_stTime.m_u8Sec) / 60;
    u32BDateTime_Days =
		Days_Since_Epoch(pstBDateTime->m_stDate.m_u16Year, pstBDateTime->m_stDate.m_u8Month,
        pstBDateTime->m_stDate.m_u8Day);

    /* add */
    u32Days = u32Minutes / (24 * 60);
    u32BDateTime_Days += u32Days;
    u32Minutes -= (u32Days * 24 * 60);
    u32BDateTime_Minutes += u32Minutes;
    u32Days = u32BDateTime_Minutes / (24 * 60);
    u32BDateTime_Days += u32Days;

    /* convert pstBDateTime from u8Seconds and days */
    Seconds_Since_Midnight_Into_HMS(u32BDateTime_Minutes * 60,
		&pstBDateTime->m_stTime.m_u8Hour, &pstBDateTime->m_stTime.m_u8Min,
        &pstBDateTime->m_stTime.m_u8Sec);
	Days_Since_Epoch_Into_YMD(u32BDateTime_Days, &pstBDateTime->m_stDate.m_u16Year,
        &pstBDateTime->m_stDate.m_u8Month, &pstBDateTime->m_stDate.m_u8Day);
	pstBDateTime->m_stDate.m_u8Wday = Day_Of_Week(pstBDateTime->m_stDate.m_u16Year,
        pstBDateTime->m_stDate.m_u8Month, pstBDateTime->m_stDate.m_u8Day);
}
#endif
bool DateTime_WildCard(BACnetDateTime_t *pstBDateTime)
{
    bool bWildCard_Present = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_WildCard:Entry\r\n");
	#endif

    if (pstBDateTime)
    {
		if ((pstBDateTime->m_stDate.m_u16Year == (1900 + 0xFF)) &&
			(pstBDateTime->m_stDate.m_u8Month == 0xFF) && (pstBDateTime->m_stDate.m_u8Day == 0xFF) &&
            (pstBDateTime->m_stDate.m_u8Wday == 0xFF) && (pstBDateTime->m_stTime.m_u8Hour == 0xFF) &&
			(pstBDateTime->m_stTime.m_u8Min == 0xFF) && (pstBDateTime->m_stTime.m_u8Sec == 0xFF) &&
            (pstBDateTime->m_stTime.m_u8Hundredths == 0xFF))
        {
            bWildCard_Present = true;
        }
    }

    return bWildCard_Present;
}

/* Returns true if any type of wildcard is present except for day of week on it's own. */
bool DateTime_Wildcard_Present(BACnetDateTime_t *pstBDateTime)
{
    bool bWildCard_Present = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Wildcard_Present:Entry\r\n");
	#endif

    if (pstBDateTime)
    {
		if ((pstBDateTime->m_stDate.m_u16Year == (1900 + 0xFF)) ||
			(pstBDateTime->m_stDate.m_u8Month > 12) || (pstBDateTime->m_stDate.m_u8Day > 31) ||
            (pstBDateTime->m_stTime.m_u8Hour == 0xFF) || (pstBDateTime->m_stTime.m_u8Min == 0xFF) ||
            (pstBDateTime->m_stTime.m_u8Sec == 0xFF) ||
            (pstBDateTime->m_stTime.m_u8Hundredths == 0xFF))
        {
            bWildCard_Present = true;
        }
    }

    return bWildCard_Present;
}

void DateTime_Date_Wildcard_Set(BACnetDate_t *pstBDate)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Date_Wildcard_Set:Entry\r\n");
	#endif

    if (pstBDate)
    {
        pstBDate->m_u16Year = 1900 + 0xFF;
        pstBDate->m_u8Month = 0xFF;
        pstBDate->m_u8Day   = 0xFF;
        pstBDate->m_u8Wday  = 0xFF;
    }
}

void DateTime_Time_Wildcard_Set(BACnetTime_t *pstBTime)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Time_Wildcard_Set:Entry\r\n");
	#endif

    if (pstBTime)
    {
        pstBTime->m_u8Hour = 0xFF;
        pstBTime->m_u8Min = 0xFF;
        pstBTime->m_u8Sec = 0xFF;
        pstBTime->m_u8Hundredths = 0xFF;
    }
}

void DateTime_Wildcard_Set(BACnetDateTime_t *pstBDateTime)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DateTime_Wildcard_Set:Entry\r\n");
	#endif

    if (pstBDateTime)
    {
		DateTime_Date_Wildcard_Set(&pstBDateTime->m_stDate);
        DateTime_Time_Wildcard_Set(&pstBDateTime->m_stTime);
    }
}

int32_t BACAppEncode_DateTime(uint8_t *pu8APDU, BACnetDateTime_t *pstValue)
{
    int32_t i32Len = 0;
    int32_t i32APDU_Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BACAppEncode_DateTime:Entry\r\n");
	#endif

    if (pu8APDU && pstValue) {
		i32Len = Encode_Application_Date(&pu8APDU[0], &pstValue->m_stDate);
        i32APDU_Len += i32Len;

        i32Len =
            Encode_Application_Time(&pu8APDU[i32APDU_Len], &pstValue->m_stTime);
        i32APDU_Len += i32Len;
    }
    return i32APDU_Len;
}


int32_t BACAppEncode_Context_DateTime(uint8_t *pu8APDU, uint8_t u8Tag_Number,
									  BACnetDateTime_t *pstValue)
{
    int32_t i32Len = 0;
    int32_t i32APDU_Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BACAppEncode_Context_DateTime:Entry\r\n");
	#endif

    if (pu8APDU && pstValue) {
        i32Len = Encode_Opening_Tag(&pu8APDU[i32APDU_Len], u8Tag_Number);
        i32APDU_Len += i32Len;

        i32Len = BACAppEncode_DateTime(&pu8APDU[i32APDU_Len], pstValue);
        i32APDU_Len += i32Len;

        i32Len = Encode_Closing_Tag(&pu8APDU[i32APDU_Len], u8Tag_Number);
        i32APDU_Len += i32Len;
    }
    return i32APDU_Len;
}

int32_t BACAppDecode_DateTime(uint8_t *pu8APDU, BACnetDateTime_t *pstValue)
{
    int32_t i32Len = 0;
    int32_t i32Section_Len;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BACAppDecode_DateTime:Entry\r\n");
	#endif

    if (-1 == (i32Section_Len =
		Decode_Application_Date(&pu8APDU[i32Len], &pstValue->m_stDate)))
    {
        return -1;
    }
    i32Len+= i32Section_Len;

    if (-1 == (i32Section_Len =
            Decode_Application_Time(&pu8APDU[i32Len], &pstValue->m_stTime)))
    {
        return -1;
    }

    i32Len += i32Section_Len;

    return i32Len;
}

int32_t BACAppDecode_Context_DateTime(uint8_t *pu8APDU, uint8_t u8Tag_Number,
									  BACnetDateTime_t *pstValue)
{
    int32_t i32APDU_Len = 0;
    int32_t i32Len;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BACAppDecode_Context_DateTime:Entry\r\n");
	#endif

    if (Decode_Is_Opening_Tag_Number(&pu8APDU[i32APDU_Len], u8Tag_Number))
    {
        i32APDU_Len++;
    } 
    else
    {
        return -1;
    }

    if (-1 == (i32Len = BACAppDecode_DateTime(&pu8APDU[i32APDU_Len], pstValue)))
    {
        return -1;
    }
    else
    {
        i32APDU_Len += i32Len;
    }

    if (Decode_Is_Closing_Tag_Number(&pu8APDU[i32APDU_Len], u8Tag_Number))
    {
        i32APDU_Len++;
    } 
    else
    {
        return -1;
    }
    return i32APDU_Len;
}

#ifdef BACDEL_SER_DM_UTC_B
/* function to subtract utc offset from time value 
		use for utc time sync service */
void DateTime_Subtract_UTC_Offset(utc_time_sync_request_t *pstBDateTime, int32_t i32UtcOffset)
{
	int32_t i32UtcSec = 0;
	int32_t i32OffsetSec = 0;
    int32_t i32Days = 0; // adjust days
	uint32_t u32DaysSinceEpoch = 0;
	/* variables for calculations */
	uint32_t u32ActualSec = 0;
	uint32_t u32ActualDays = 0;

	/* FIXME : it is assumed that utc offset is always between -780 to +780 only */
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
					DateTime_Subtract_UTC_Offset: entry \r\n");
	#endif

	/* get utc ofset mins converted to secs */
	i32OffsetSec = (i32UtcOffset*60);

    /* convert to minutes since midnight */
    i32UtcSec = Seconds_Since_Midnight(pstBDateTime->m_stTime.m_u8Hour,
		pstBDateTime->m_stTime.m_u8Min, pstBDateTime->m_stTime.m_u8Sec);

	/* convert to total days since epoch */
	u32DaysSinceEpoch = Days_Since_Epoch(pstBDateTime->m_stDate.m_u16Year,
		pstBDateTime->m_stDate.m_u8Month, pstBDateTime->m_stDate.m_u8Day);

	/* add - subtract the utc difference */
	i32UtcSec -= i32OffsetSec;

	/* NOTE : (24*60*60) = seconds in 1 day */
	if(i32UtcSec < 0)
	{
		/* secs are -ve so find actual sec by adding to (24*60*60) */
		u32ActualSec = (24*60*60) + i32UtcSec;
		/* subtract 1 day */
		i32Days -= 1;
	}
	else
	{
		/* if sec are more than (24*60*60), add 1 day & find actual sec */
		if(i32UtcSec >= (24*60*60))
			u32ActualSec = i32UtcSec - (24*60*60);
		else
			u32ActualSec = i32UtcSec;
		/* add 1 day if needed */
		i32Days = i32UtcSec / (24*60*60);
	}

	/* calculate days */
	u32ActualDays += u32DaysSinceEpoch;
	u32ActualDays += i32Days;

    /* convert seconds to HMS */
    Seconds_Since_Midnight_Into_HMS(u32ActualSec, &pstBDateTime->m_stTime.m_u8Hour,
		&pstBDateTime->m_stTime.m_u8Min, &pstBDateTime->m_stTime.m_u8Sec);

	/* convert days to YMD */
	Days_Since_Epoch_Into_YMD(u32ActualDays, &pstBDateTime->m_stDate.m_u16Year,
		&pstBDateTime->m_stDate.m_u8Month, &pstBDateTime->m_stDate.m_u8Day);
	/* get the week day */
	pstBDateTime->m_stDate.m_u8Wday = Day_Of_Week(pstBDateTime->m_stDate.m_u16Year,
		pstBDateTime->m_stDate.m_u8Month, pstBDateTime->m_stDate.m_u8Day);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	DateTime_Subtract_UTC_Offset: exit \r\n");
	#endif
}
#endif /* BACDEL_SER_DM_UTC_B */

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) 
/* function to get difference between system datetime & datetime 
	received in time sync requests. */
void TimeSync_Get_DateTime_Diff(BACnetDate_t *pstTsDate, BACnetTime_t *pstTsTime,
								int32_t *pi32SecDiff, int32_t *pi32DaysDiff)
{
	/* to save system time */
	BACnetTime_t stSysTime = {0}; 
	/* to save system date */
	BACnetDate_t stSysDate = {0}; 
	/* local variables for sys date time */
    uint32_t u32SysDays = 0;
	uint32_t u32SysSec = 0;
	/* local variables for time sync date time */
	uint32_t u32TsDays = 0;
	uint32_t u32TsSec = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	TimeSync_Get_DateTime_Diff: entry \r\n");
	#endif

	if(NULL == pstTsDate || NULL == pstTsTime || 
	   NULL == pi32SecDiff || NULL == pi32DaysDiff )
	   return;

	/* get sys date_time parameters */
	/* get the system date_time */
	GetSystem_DateTime(&stSysDate, &stSysTime);
	/* calculate seconds since midnight */
	u32SysSec = Seconds_Since_Midnight(stSysTime.m_u8Hour, 
		stSysTime.m_u8Min, stSysTime.m_u8Sec);
	/* calculate days since epoch */
	u32SysDays = Days_Since_Epoch(stSysDate.m_u16Year,
		stSysDate.m_u8Month, stSysDate.m_u8Day);

	/* get time sync date_time parameters */
	/* calculate seconds since midnight */
	u32TsSec = Seconds_Since_Midnight(pstTsTime->m_u8Hour, 
		pstTsTime->m_u8Min, pstTsTime->m_u8Sec);
	/* calculate days since epoch */
	u32TsDays = Days_Since_Epoch(pstTsDate->m_u16Year,
		pstTsDate->m_u8Month, pstTsDate->m_u8Day);

	/* calculate & save the date time differences in virtual device */
	*pi32SecDiff = u32SysSec - u32TsSec;
	*pi32DaysDiff = u32SysDays - u32TsDays;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	TimeSync_Get_DateTime_Diff: exit \r\n");
	#endif
}
#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)  */

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function validates if proper date value is received.
*    
*@param pstDate	[in] Date received.                                   
*@return - true on validation success else false.
*	
***/
bool Validate_Date(BACnetDate_t *pstDate)
{
	/* local variables */
	uint8_t u8MonthDays = 0;
	uint8_t u8WeekDay = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_Date: entry \r\n");
	#endif

	if(NULL == pstDate)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Validate_Date: null input pointers. \r\n");
		#endif
		return FALSE;
	}

	/* validate date */
	if(pstDate->m_u16Year < 1900 || pstDate->m_u16Year > 2155)
		return FALSE;
	if(pstDate->m_u8Month < BACNET_MONTH_JANUARY || pstDate->m_u8Month > BACNET_MONTH_DECEMBER)
		return FALSE;
	if(pstDate->m_u8Wday < BACNET_WEEKDAY_MONDAY || pstDate->m_u8Wday > BACNET_WEEKDAY_SUNDAY)
		return FALSE;
	/* get total days in given month for given year */
	u8MonthDays = Month_Days(pstDate->m_u16Year, pstDate->m_u8Month);
	if(pstDate->m_u8Day < BACNET_ONE || pstDate->m_u8Day > u8MonthDays)
		return FALSE;
	/* now cross check if weekday is valid */
	u8WeekDay = Day_Of_Week(pstDate->m_u16Year, pstDate->m_u8Month, pstDate->m_u8Day);
	if(pstDate->m_u8Wday != u8WeekDay)
		return FALSE;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_Date: exit \r\n");
	#endif

	return TRUE;
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function validates if proper time value is received.
*    
*@param pstTime	[in] Time received.                                   
*@return - true on validation success else false.
*	
***/
bool Validate_Time(BACnetTime_t *pstTime)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_Time: entry \r\n");
	#endif

	if(NULL == pstTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Validate_Time: null input pointers. \r\n");
		#endif

		return FALSE;
	}

	/* validate time */
	if(/*pstTime->m_u8Hour < 0 ||*/ pstTime->m_u8Hour > 23)
		return FALSE;
	if(/*pstTime->m_u8Min < 0 ||*/ pstTime->m_u8Min > 59)
		return FALSE;
	if(/*pstTime->m_u8Sec < 0 ||*/ pstTime->m_u8Sec > 59)
		return FALSE;
	if(/*pstTime->m_u8Hundredths < 0 ||*/ pstTime->m_u8Hundredths > 99)
		return FALSE;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_Time: exit \r\n");
	#endif

	return TRUE;
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function validates if proper date & time values are received.
*    
*@param pstDate	[in] Date received.
*@param pstTime	[in] Time received.
*                                   
*@return - true on validation success else false.
*	
***/
bool Validate_DateTime(BACnetDate_t *pstDate, BACnetTime_t *pstTime)
{
	/* local variables */
	bool bReturnValue = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_DateTime: entry \r\n");
	#endif

	if(NULL == pstDate || NULL == pstTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Validate_DateTime: null input pointers. \r\n");
		#endif

		return bReturnValue;
	}

	/* validate date & time */
	if(Validate_Date(pstDate) && Validate_Time(pstTime))
		bReturnValue = TRUE;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Validate_DateTime: exit \r\n");
	#endif

	return bReturnValue;
}

/** function to get the device date_time */
void GetDevice_DateTime(BACnetDate_t *pstDeviceDate, BACnetTime_t *pstDeviceTime,
								 void *pvVirtualDev)
{
	/* to save system time */
	BACnetTime_t stSysTime = {0}; 
	/* to save system date */
	BACnetDate_t stSysDate = {0}; 
	/* to get date time differences */
	int32_t i32SecDiff = 0;
	int32_t i32DayDiff = 0;
	/* virtual device instance */
	virtualDevData_t *pVirtualDev = NULL;
	/* local variables for sys date time */
    int32_t i32SysDays = 0;
	int32_t i32SysSec = 0;
	/* variables for calculations */
	uint32_t u32ActualSec = 0;
	uint32_t u32ActualDays = 0;
	/* to adjust days in case needed */
	int32_t i32Days = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	GetDevice_DateTime: Entry \r\n");
	#endif

	/* get virtual device pointer */
	pVirtualDev = pvVirtualDev;
	if(NULL == pVirtualDev || NULL == pstDeviceDate || NULL == pstDeviceTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		GetDevice_DateTime: Null input pointers \r\n");
		#endif

		return;
	}

	/* get the system date_time */
	GetSystem_DateTime(&stSysDate, &stSysTime);

	/* get the time sync differences from virtual device */
	i32SecDiff = pVirtualDev->m_i32Sec_Diff;
	i32DayDiff = pVirtualDev->m_i32Days_Diff;

	if(!i32SecDiff && ! i32DayDiff)
	{
		/* no need to update device date time, send the sys date_time */
		/* copy date & time */
		memcpy(pstDeviceDate, &stSysDate, sizeof(BACnetDate_t));
		memcpy(pstDeviceTime, &stSysTime, sizeof(BACnetTime_t));
	}
	else
	{
		/* get secs since midnight */
		i32SysSec = Seconds_Since_Midnight(stSysTime.m_u8Hour, 
			stSysTime.m_u8Min, stSysTime.m_u8Sec);
		/* get days since epoch */
		i32SysDays = Days_Since_Epoch(stSysDate.m_u16Year,
			stSysDate.m_u8Month, stSysDate.m_u8Day);

		/* add - subtract the difference */
		i32SysSec -= i32SecDiff;
		i32SysDays -= i32DayDiff;

		/* NOTE : (24*60*60) = seconds in 1 day */
		if(i32SysSec < 0)
		{
			/* secs are -ve so find actual sec by adding to (24*60*60) */
			u32ActualSec = (24*60*60) + i32SysSec;
			/* subtract 1 day */
			i32Days -= 1;
		}
		else
		{
			/* if sec are more than (24*60*60), add 1 day & find actual sec */
			if(i32SysSec >= (24*60*60))
				u32ActualSec = i32SysSec - (24*60*60);
			else
				u32ActualSec = i32SysSec;
			/* add 1 day if needed */
			i32Days = i32SysSec / (24*60*60);
		}

		/* convert seconds to HMS */
		Seconds_Since_Midnight_Into_HMS(u32ActualSec,
									&pstDeviceTime->m_u8Hour, 
									&pstDeviceTime->m_u8Min,
									&pstDeviceTime->m_u8Sec);

		/* calculate days */
		u32ActualDays = i32SysDays + i32Days;

		/* convert days to YMD */
		Days_Since_Epoch_Into_YMD(u32ActualDays, &pstDeviceDate->m_u16Year, 
			&pstDeviceDate->m_u8Month, &pstDeviceDate->m_u8Day);
		/* get the week day */
		pstDeviceDate->m_u8Wday = Day_Of_Week(pstDeviceDate->m_u16Year,
			pstDeviceDate->m_u8Month, pstDeviceDate->m_u8Day);
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	GetDevice_DateTime: Exit \r\n");
	#endif

	return;
}

/** Api to add days in given date */
bool DateTime_Add_Days(uint32_t u32DayCnt, BACnetDate_t *pstInDate)
{
    int32_t i32SysDays = 0;

    /* get days since epoch */
	i32SysDays = Days_Since_Epoch(pstInDate->m_u16Year, pstInDate->m_u8Month, pstInDate->m_u8Day);

    i32SysDays += u32DayCnt;

    Days_Since_Epoch_Into_YMD( i32SysDays, &pstInDate->m_u16Year, 
		&pstInDate->m_u8Month, &pstInDate->m_u8Day);

    pstInDate->m_u8Wday = Day_Of_Week(pstInDate->m_u16Year,
			pstInDate->m_u8Month, pstInDate->m_u8Day);

    return true;
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function validates if "ANY" date value is received.
*    
*@param pstDate	[in] Date received.                                   
*@return - true on validation success else false.
*	
***/
bool Is_Any_Date(BACnetDate_t *pstDate)
{
    /* local variables */
    bool bReturnVal = TRUE;

	/* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_Any_Date: entry \r\n");
	#endif

	if(NULL == pstDate)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Is_Any_Date: null input pointers. \r\n");
		#endif

		return FALSE;
	}

	/* validate date */
	if(pstDate->m_u16Year != 0xFF)
		bReturnVal = FALSE;
	if(pstDate->m_u8Month != 0xFF)
		bReturnVal = FALSE;
	if(pstDate->m_u8Day != 0xFF)
		bReturnVal = FALSE;
	if(pstDate->m_u8Wday != 0xFF)
		bReturnVal = FALSE;

    /* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_Any_Date: exit \r\n");
	#endif

	return bReturnVal;
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function validates if "ANY" time value is received.
*    
*@param pstTime	[in] Time received.                                   
*@return - true on validation success else false.
*	
***/
bool Is_Any_Time(BACnetTime_t *pstTime)
{
    /* local variables */
    bool bReturnVal = TRUE;

    /* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_Any_Time: entry \r\n");
	#endif

	if(NULL == pstTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Is_Any_Time: null input pointers. \r\n");
		#endif

		return FALSE;
	}

	/* validate time */
	if(pstTime->m_u8Hour != 0xFF)
		bReturnVal = FALSE;
	if(pstTime->m_u8Min != 0xFF)
		bReturnVal = FALSE;
	if(pstTime->m_u8Sec != 0xFF)
		bReturnVal = FALSE;
	if(pstTime->m_u8Hundredths != 0xFF)
		bReturnVal = FALSE;

    /* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_Any_Time: exit \r\n");
	#endif

	return bReturnVal;
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function validates if "ANY" date & time values are received.
*    
*@param pstDate	[in] Date received.
*@param pstTime	[in] Time received.
*                                   
*@return - true on validation success else false.
*	
***/
bool Is_Any_DateTime(BACnetDate_t *pstDate, BACnetTime_t *pstTime)
{
	/* local variables */
	bool bReturnValue = FALSE;

    /* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_Any_DateTime: entry \r\n");
	#endif

	if(NULL == pstDate || NULL == pstTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Is_Any_DateTime: null input pointers. \r\n");
		#endif

		return bReturnValue;
	}

	/* validate date & time */
	if(Is_Any_Date(pstDate) && Is_Any_Time(pstTime))
		bReturnValue = TRUE;

    /* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Is_Any_DateTime: exit \r\n");
	#endif

	return bReturnValue;
}

/*** 
*
* DESCRIPTION
* Function to convert date time value in to total second value.
*
* @param pstDateTime	[in] DateTime of device.
* @returns uint32_t		[out] void.
*
***/
uint32_t Convert_DateTime_InToSecond(Pr_BACnetDateTime_t *pstDateTime)
{
	/* local variables for sys date time */
	uint32_t u32Sec = 0;
	uint32_t u32Days = 0;
	uint32_t u32TotalSec = 0;
	
	/* calculate seconds since midnight */
	u32Sec = Seconds_Since_Midnight(pstDateTime->m_stDateTime.m_stTime.m_u8Hour,
	pstDateTime->m_stDateTime.m_stTime.m_u8Min,
	pstDateTime->m_stDateTime.m_stTime.m_u8Sec);
	
	/* calculate days since epoch */
	u32Days = Days_Since_Epoch(pstDateTime->m_stDateTime.m_stDate.m_u16Year,
	pstDateTime->m_stDateTime.m_stDate.m_u8Month,
	pstDateTime->m_stDateTime.m_stDate.m_u8Day);

	/* calculate & save the date time differences in virtual device */
	u32TotalSec = (u32Days * 24 * 60 * 60)+(u32Sec) ;
	return u32TotalSec;
}


/** This Api is for validation of Date in schedule and calendar object*/
bool IsValidCurrentDate(BACnetDate_t *pstDeviceDate, BACnetDate_t *pstdate2, uint8_t u8Compare)
{
    switch(u8Compare)
    {
        case BEFORE:
        {   /* to Varify if DateValue is before Device Date */
            if(pstdate2->m_u16Year == 255 || pstdate2->m_u16Year <= pstDeviceDate->m_u16Year)
            {   /* DateValue-year is before DeviceDate-year */
                if(pstdate2->m_u16Year < pstDeviceDate->m_u16Year && 
                    pstdate2->m_u16Year != 255 && 
                    pstdate2->m_u8Month < BACNET_MONTH_ODDMONTH &&
                    pstdate2->m_u8Day < 32)
                    return true;
                /* Valid if DateValue-Month is more than 14(even Month) i.e Any month
                         or DateValue-Month is More/Equal than January and less than DeviceDate-Month
                         or if DateValue-Month is 13(odd) then DeviceDate-Month must be Odd Month 
                         or if DateValue-Month is 14(even) then DeviceDate-Month must be even Month */
                if( pstdate2->m_u8Month > BACNET_MONTH_EVENMONTH || 
                    (pstdate2->m_u8Month <= pstDeviceDate->m_u8Month && pstdate2->m_u8Month >= BACNET_MONTH_JANUARY)|| 
                   (pstdate2->m_u8Month == BACNET_MONTH_ODDMONTH && (pstDeviceDate->m_u8Month%2)) ||
                   (pstdate2->m_u8Month == BACNET_MONTH_EVENMONTH && !(pstDeviceDate->m_u8Month%2)))
                    {   /* DateValue-Month is before DeviceDate-Month */
                        if(pstdate2->m_u8Month < pstDeviceDate->m_u8Month && pstdate2->m_u8Day < 32)
                            return true;
                        /* Valid if DateValue-Day is more than 34(even Day)
                         or DateValue-Day is less/Equal than DeviceDate-Day
                         or if DateValue-Day is 33(odd) then DeviceDate-Day must be Odd Day 
                         or if DateValue-Day is 34(even) then DeviceDate-Day must be even Day
                         or if DateValue-Day is 32(Last Day) then DeviceDate-Day must be Last Day of Month */
                        if( pstdate2->m_u8Day > 34 || 
                            (pstdate2->m_u8Day <= pstDeviceDate->m_u8Day  && pstdate2->m_u8Day > 0)|| 
                            (pstdate2->m_u8Day == 33 && (pstDeviceDate->m_u8Day%2)) ||
                            (pstdate2->m_u8Day == 34 && !(pstDeviceDate->m_u8Day%2)) ||
                            (pstdate2->m_u8Day == 32 && (pstDeviceDate->m_u8Day == Month_Days(pstDeviceDate->m_u16Year, pstDeviceDate->m_u8Month))))
                        {                            
                             return true;                            
                        }
                    }
            }
        }
        break;
        case AFTER:
        {   /* to Varify if DateValue is after Device Date */
            if(pstdate2->m_u16Year == 255 || pstdate2->m_u16Year >= pstDeviceDate->m_u16Year)
            {   /* DateValue-year is after DeviceDate-year */
                if(pstdate2->m_u16Year > pstDeviceDate->m_u16Year &&
                    pstdate2->m_u8Month < BACNET_MONTH_ODDMONTH &&
                    pstdate2->m_u8Day < 32)
                    return true;
                /* Valid if DateValue-Month is more than 14(even Month)i.e Any month
                         or DateValue-Month is More/Equal than DeviceDate-Month and less than Odd Month
                         or if DateValue-Month is 13(odd) then DeviceDate-Month must be Odd Month 
                         or if DateValue-Month is 14(even) then DeviceDate-Month must be even Month */
                if( pstdate2->m_u8Month > BACNET_MONTH_EVENMONTH || 
                    (pstdate2->m_u8Month >= pstDeviceDate->m_u8Month && pstdate2->m_u8Month < BACNET_MONTH_ODDMONTH) || 
                   (pstdate2->m_u8Month == BACNET_MONTH_ODDMONTH && (pstDeviceDate->m_u8Month%2)) ||
                   (pstdate2->m_u8Month == BACNET_MONTH_EVENMONTH && !(pstDeviceDate->m_u8Month%2)))
                    {    /* DateValue-Month is after DeviceDate-Month */
                        if(pstdate2->m_u8Month > pstDeviceDate->m_u8Month &&
                            pstdate2->m_u8Month < BACNET_MONTH_ODDMONTH &&
                            pstdate2->m_u8Day < 32)
                            return true;
                        /* Valid if DateValue-Day is more than 34(even Day)
                         or DateValue-Day is More/Equal than DeviceDate-Day
                         or if DateValue-Day is 33(odd) then DeviceDate-Day must be Odd Day 
                         or if DateValue-Day is 34(even) then DeviceDate-Day must be even Day
                         or if DateValue-Day is 32(Last Day) then DeviceDate-Day must be Last Day of Month */
                        if( pstdate2->m_u8Day > 34 || 
                            (pstdate2->m_u8Day >= pstDeviceDate->m_u8Day && pstdate2->m_u8Day < 32) || 
                            (pstdate2->m_u8Day == 33 && (pstDeviceDate->m_u8Day%2)) ||
                            (pstdate2->m_u8Day == 34 && !(pstDeviceDate->m_u8Day%2)) ||
                            (pstdate2->m_u8Day == 32 && (pstDeviceDate->m_u8Day == Month_Days(pstDeviceDate->m_u16Year, pstDeviceDate->m_u8Month))))
                        {
                            return true;                          
                        }
                    }
            }
        }
        break;
        case EXACT:
        {   /* to Varify if DateValue, DeviceDate is same */
            if(pstdate2->m_u16Year == 255 || pstdate2->m_u16Year == pstDeviceDate->m_u16Year)
            {
                /* Valid if DateValue-Month is more than 14(even Month)i.e Any month
                         or DateValue-Month is Equal to DeviceDate-Month 
                         or if DateValue-Month is 13(odd) then DeviceDate-Month must be Odd Month 
                         or if DateValue-Month is 14(even) then DeviceDate-Month must be even Month */
                if( pstdate2->m_u8Month > BACNET_MONTH_EVENMONTH || 
                    pstdate2->m_u8Month == pstDeviceDate->m_u8Month || 
                   (pstdate2->m_u8Month == BACNET_MONTH_ODDMONTH && (pstDeviceDate->m_u8Month%2)) ||
                   (pstdate2->m_u8Month == BACNET_MONTH_EVENMONTH && !(pstDeviceDate->m_u8Month%2)))
                    {
                        /* Valid if DateValue-Day is more than 34(even Day)
                         or DateValue-Day is Equal to DeviceDate-Day
                         or if DateValue-Day is 33(odd) then DeviceDate-Day must be Odd Day 
                         or if DateValue-Day is 34(even) then DeviceDate-Day must be even Day
                         or if DateValue-Day is 32(Last Day) then DeviceDate-Day must be Last Day of Month */
                        if( pstdate2->m_u8Day > 34 || 
                            pstdate2->m_u8Day == pstDeviceDate->m_u8Day || 
                            (pstdate2->m_u8Day == 33 && (pstDeviceDate->m_u8Day%2)) ||
                            (pstdate2->m_u8Day == 34 && !(pstDeviceDate->m_u8Day%2)) ||
                            (pstdate2->m_u8Day == 32 && (pstDeviceDate->m_u8Day == Month_Days(pstDeviceDate->m_u16Year, pstDeviceDate->m_u8Month))))
                        {   /* if DateValue-wday, DeviceDate-wday is same*/
                            if(pstdate2->m_u8Wday == pstDeviceDate->m_u8Wday || pstdate2->m_u8Wday > BACNET_WEEKDAY_SUNDAY)
                            {
                                return true;
                            }
                        }
                    }
            }
        }
        break;
    }
    return false;
}
