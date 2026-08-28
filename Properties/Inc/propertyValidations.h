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
*   File Name - propertyValidations.h
*
*   RELEASE HISTORY
*
*	DATE				NAME					DESCRIPTION
*	5th March 2013		Pratham N. Murkute		Created file & moved api's.
*
****************************************************************************/

#ifndef PROP_VALIDATE_H
#define PROP_VALIDATE_H

/** header file includes */
#include "propertydef.h"

/** function returns the property access type if property is supported
when USER_DEFINED_ACCESS_TYPE macro is enabled. */
PROP_ACCESS_TYPE Validate_PropertySupport(
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID eDevProp, 
	PROP_ACCESS_TYPE eAccessType);

/** function to validate values in Week N Day datatype */
bool Validate_WeekNDay(
	BACNET_MONTH eMonth,
	BACNET_WEEK_OF_MONTH eWeekOfMonth,
	BACNET_WEEKDAY eWeekNDay);

/** function validates if proper date value of schedule is received. */
bool Validate_Date_Combinations(
	BACnetDate_t *pstDate);

/** api to check if current date is between given date range */
bool Is_Active_DateRange(
	BACnetDateRange_t *pstDateRange, 
	BACnetDate_t *pstSyetemDate);

/** api to check if current date */
bool Is_Active_Date(
	BACnetDate_t *pstDate, 
	BACnetDate_t *pstSyetemDate);

/* function to check if current week-n-day value */
bool IsWeekInDay(WeekNDay_t *pstWeekNDay, 
	BACnetDate_t *pstSystemDate);

/** function validates deferent combinations of time */ 
bool Validate_Time_Combinations(
	BACnetTime_t *pstTime);

/**	function validates max apdu length values */
bool Validate_Max_APDU_Length(uint32_t u32ApduLength,void *pVirtualDev);

/** validates parameters for daily schedule array datatype */
BACNET_ERROR_CODE Validate_DailySchedule_Array(
	Pr_ListOfBACnetDailySchedule_t *pstWeeklySchedule, 
	bool bArrIndxPresent,
	uint32_t u32ArrayIndex);

/** validates parameters for DevObjPropRef data type */
BACNET_ERROR_CODE Validate_DevObjPropRef(
	BACnetDevObjPropRef_t *pstDevObjPropRef);

/** validates parameters for event parameters datatype */
BACNET_ERROR_CODE Validate_EventParameters(
	BACnetEventParameter_t *pstEventParameter);

/** validate parameters for calendar entry list datatype */
BACNET_ERROR_CODE Validate_CalenderEntry_List(
	ListOfBACnetCalendarEntry_t *pstDateList, 
	uint32_t *pu32FirstFaildElementNo);

/** validates parameters for destination list datatype */
BACNET_ERROR_CODE Validate_Destination_List(
	ListOfBACnetDestination_t *pstRecepientList, 
	uint32_t *pu32FirstFaildElementNo);

/** validates parameters for special event datatype */
BACNET_ERROR_CODE Validate_SpecialEvent(
	ListOfSpecialEvent_t *pstSplEvent,
	uint32_t *pu32FirstFaildElementNo);

/** validates parameters for special event list datatype */
BACNET_ERROR_CODE Validate_SpecialEvent_List(
	ListOfSpecialEvent_t  *pstListOfSpecialEvent,
	uint32_t *pu32FirstFaildElementNo,
	bool bArrIndxPresent,
	uint32_t u32ArrayIndex);

/** validate parameters for Device Object Property Ref. list datatype */
BACNET_ERROR_CODE Validate_DevObjPropRef_List(
	ListOfBACnetDevObjPropRef_t *pstListObjPropRef, 
	uint32_t *pu32FirstFaildElementNo);

/** validates parameters for Notification Priority array datatype */
BACNET_ERROR_CODE Validate_NotifyPriority_Array(
	Pr_BACnetNotifyPriority_t *pstNotifyPriority, 
	bool bArrIndxPresent,
	uint32_t u32ArrayIndex);

/** validates parameters for Configuration file property */
BACNET_ERROR_CODE Validate_ConfigurationFile(
	ListOfObjId_t *pstObjIdList);

/** validates parameters for Last Restore Time property */
BACNET_ERROR_CODE Validate_TimeStamp(
	BACnetTimeStamp_t *pstTimeStamp);

/* function to check whether service type is supported or not */
bool Check_Service_Support(
	int32_t i32DevId,
	BACNET_SERVICES_SUPPORTED eServiceType)	;

/* validate protocol service supported data type */
bool Validate_Services_Supported(
	int32_t i32DevId,
	void *pvWritePropVal);

/* validate protocol object types supported data type */
bool Validate_ObjectTypes_Supported(
	int32_t i32DevId,
	void *pvWritePropVal);

bool Validate_APDU_Length(uint32_t u32ApduLength);

/** function validates bbmd bdt entry list */
BACNET_ERROR_CODE Validate_BdtEntry_List(
	ListOfBACnetBDTEntry_t *pstBdtList,
	uint32_t *pu32FirstFaildElementNo,
	BACNET_CONFIRMED_SERVICE eServiceChoice);

/** function validates bbmd fdt entry list */
BACNET_ERROR_CODE Validate_FdtEntry_List(
	ListOfBACnetFDTEntry_t *pstFdtList,
	uint32_t *pu32FirstFaildElementNo);

/** function validates host n port value */
BACNET_ERROR_CODE Validate_HostNPort(
	Pr_BACnetHostNPort_t *pstHostNPort);

bool Is_ActiveDate(BACnetDateRange_t *pstDateRange, BACnetDate_t *pstSyetemDate);	

#ifdef BACDEL_OBJ_NP
/** function validates apdu length value of network port object */
bool Validate_APDU_Length(uint32_t u32ApduLength);

/** function validates bbmd bdt entry list */
BACNET_ERROR_CODE Validate_BdtEntry_List(
	ListOfBACnetBDTEntry_t *pstBdtList,
	uint32_t *pu32FirstFaildElementNo,
	BACNET_CONFIRMED_SERVICE eServiceChoice);

/** function validates bbmd fdt entry list */
BACNET_ERROR_CODE Validate_FdtEntry_List(
	ListOfBACnetFDTEntry_t *pstFdtList,
	uint32_t *pu32FirstFaildElementNo);

/** function validates host n port value */
BACNET_ERROR_CODE Validate_HostNPort(
	Pr_BACnetHostNPort_t *pstHostNPort);
#endif /* NP */
#endif /* PROP_VALIDATE_H */
