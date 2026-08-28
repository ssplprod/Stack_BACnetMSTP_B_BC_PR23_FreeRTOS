/*************************************************************************
*
*            Copyright (c) by SoftDEL Systems Pvt. Ltd.               
*
*  This software is copyrighted by and is the sole property of 
*  SoftDEL Systems Pvt. Ltd.
*  All rights, title, ownership, or other interests         
*  in the software remain the property of  SoftDEL Systems Pvt. Ltd.  This
*  software may only be used in accordance with the corresponding        
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
*************************************************************************/
#ifndef DATE_TIME_H
#define DATE_TIME_H

#include "bacDELapi.h"
#include "bacDELDef.h"
#include "propertydef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
	
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
	);
	
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
	);

//	/*	function that give current system local time-date in 
//		required bacnet date-time format or structures */
	char *GetSystem_DateTime(BACnetDate_t *pstLocalDate, BACnetTime_t *pstLocalTime);

	/** function to get the device date_time */
	void GetDevice_DateTime(BACnetDate_t *pstDeviceDate, BACnetTime_t *pstDeviceTime,
								 void *pvVirtualDev);

    /** Converts BACnet datetime structure into System time structure */
    //void Convert_BACnetTimeToSystem( Pr_BACnetDateTime_t *pstDateTime ,struct tm *tm);
    void Convert_BACnetTimeToSystem(void *pvTime, Pr_BACnetDateTime_t *pstDateTime);

    /** Converts System Time structure into BACnet datetime type */
    //void Convert_SystemTimeToBACnet(struct tm *tm, Pr_BACnetDateTime_t *pstDateTime);
    void Convert_SystemTimeToBACnet(void *pvTime, Pr_BACnetDateTime_t *pstDateTime);    

    /** function to add days in date */
    bool DateTime_Add_Days(uint32_t u32DayCnt, BACnetDate_t *pstInDate);

    /* utility initialization functions */
    void DateTime_Set_Date(
        BACnetDate_t * bdate,
        uint16_t year,
        uint8_t month,
        uint8_t day);
    void DateTime_Set_Time(
        BACnetTime_t * btime,
        uint8_t hour,
        uint8_t minute,
        uint8_t seconds,
        uint8_t hundredths);
    void DateTime_Set(
		BACnetDateTime_t * bdatetime,
        BACnetDate_t * bdate,
        BACnetTime_t * btime);
    void DateTime_Set_Values(
        BACnetDateTime_t * bdatetime,
        uint16_t year,
        uint8_t month,
        uint8_t day,
        uint8_t hour,
        uint8_t minute,
        uint8_t seconds,
        uint8_t hundredths);

    /* utility comparison functions:
       if the date/times are the same, return is 0
       if date1 is before date2, returns negative
       if date1 is after date2, returns positive */
    int32_t DateTime_Compare_Date(
        BACnetDate_t * date1,
        BACnetDate_t * date2);
    int32_t DateTime_Compare_Time(
        BACnetTime_t * time1,
        BACnetTime_t * time2);
    int32_t DateTime_Compare(
        BACnetDateTime_t * datetime1,
        BACnetDateTime_t * datetime2);
    int32_t DateTime_Compare_DateTime(
        BACnetDate_t *pstDate1, BACnetTime_t *pstTime1,
        BACnetDate_t *pstDate2, BACnetTime_t *pstTime2);

    uint8_t Day_Of_Week(uint16_t u16Year, uint8_t u8Month, uint8_t u8Day);

    /* utility copy functions */
    void DateTime_Copy_Date(
        BACnetDate_t * date1,
        BACnetDate_t * date2);
    void DateTime_Copy_Time(
        BACnetTime_t * time1,
        BACnetTime_t * time2);
    void DateTime_Copy(
        BACnetDateTime_t * datetime1,
        BACnetDateTime_t * datetime2);
#if (defined BACDEL_SER_DM_UTC_A || defined BACDEL_SER_DM_UTC_B)
    /* utility add function */
    void DateTime_Add_Minutes(
        utc_time_sync_request_t *bdatetime,
        uint32_t minutes);
#endif
    /* date and time wildcards */
    bool DateTime_WildCard(
        BACnetDateTime_t * bdatetime);
    bool DateTime_Wildcard_Present(
        BACnetDateTime_t * bdatetime);
    void DateTime_Wildcard_Set(
        BACnetDateTime_t * bdatetime);
    void DateTime_Date_Wildcard_Set(
        BACnetDate_t * bdate);
    void DateTime_Time_Wildcard_Set(
        BACnetTime_t * btime);

    int32_t BACAppEncode_DateTime(
        uint8_t * apdu,
        BACnetDateTime_t * value);

    int32_t BACAppEncode_Context_DateTime(
        uint8_t * apdu,
        uint8_t tag_number,
        BACnetDateTime_t * value);

    int32_t BACAppDecode_DateTime(
        uint8_t * apdu,
        BACnetDateTime_t * value);

    int32_t BACAppDecode_Context_DateTime(
        uint8_t * apdu,
        uint8_t tag_number,
        BACnetDateTime_t * value);

    uint8_t Month_Days(uint16_t u16Year, uint8_t u8Month);

	#ifdef BACDEL_SER_DM_UTC_B
	/* function to subtract utc offset from time value 
		use for utc time sync service */
	void DateTime_Subtract_UTC_Offset(
		utc_time_sync_request_t *pstBDateTime, 
		int32_t i32UtcOffset);
	#endif

	#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) 
	/* function to get difference between system datetime & datetime 
		received in time sync requests. */
	void TimeSync_Get_DateTime_Diff(
		BACnetDate_t *pstTsDate, BACnetTime_t *pstTsTime,
		int32_t *pi32SecDiff, int32_t *pi32DaysDiff);
	#endif

    /* below api's return true on success */
	/* function validates if proper date value is received. */
	bool Validate_Date(BACnetDate_t *pstDate);
	/* function validates if proper time value is received. */
	bool Validate_Time(BACnetTime_t *pstTime);
	/* function validates if proper date & time values are received. */
	bool Validate_DateTime(BACnetDate_t *pstDate, BACnetTime_t *pstTime);

    /* below api's return true on success */
    /* function to check if any time value is received */
    bool Is_Any_Time(BACnetTime_t *pstTime);
    /* function to check if any date value is received */
    bool Is_Any_Date(BACnetDate_t *pstDate);
    /* function to check if any date_time value is received */
    bool Is_Any_DateTime(BACnetDate_t *pstDate, BACnetTime_t *pstTime);
	/* Function to convert date time value in to total second value */
	uint32_t Convert_DateTime_InToSecond(Pr_BACnetDateTime_t *pstDateTime);
	

    /** Function to calculate time change value for trending service.
        - api calculates time change & saves the value in 
          (m_fTimeChange) of virtual device. */
    bool IsValidCurrentDate(BACnetDate_t *pstdate1, BACnetDate_t *pstdate2,uint8_t u8Compare);	
	int32_t Get_DateTime_Diff(Pr_BACnetDateTime_t stTsDateTime, Pr_BACnetDateTime_t stDevDateTime);	
    #define BEFORE 0
    #define AFTER 1
    #define EXACT 2

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DATE_TIME_H */
