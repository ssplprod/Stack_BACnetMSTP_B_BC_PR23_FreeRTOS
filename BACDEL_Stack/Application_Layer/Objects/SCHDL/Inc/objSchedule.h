/******************************************************************************
*
*                   Copyright (c) by SoftDEL Systems Pvt. Ltd.
*
*   This software is copyrighted by and is the sole property of SoftDEL
*   Systems Pvt. Ltd. All rights, title, ownership, or other interests in the
*   software remain the property of  SoftDEL Systems Pvt. Ltd. This software
*   may only be used in accordance with the corresponding license
*   agreement. Any unauthorized use, duplication, transmission,
*   distribution, or disclosure of this software is expressly forbidden.
*
*   This Copyright notice may not be removed or modified without prior
*   written consent of SoftDEL Systems Pvt. Ltd.
*
*   SoftDEL Systems Pvt. Ltd. reserves the right to modify this software
*   without notice.
*
*   SoftDEL Systems Pvt. Ltd.						india@softdel.com
*   3rd Floor, Pentagon P4,						http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028
*
*   FILE NAME
*   objSchedule.h
*
*	AUTHORS
*	Heramb Joshi
*
*   DESCRIPTION
*   This file contains structures and function definitions related to
*   schedule object type.
*
*******************************************************************************/

#ifndef SDL_H
#define SDL_H
#ifdef BACDEL_OBJ_SDL

/** include header files */
#include "bacDELDef.h"
#include "propertydef.h"

/**
*
* NAME: TriggerTimeNPriority
*
* DESCRIPTION
* BACnet schedule object trigger time and priority structure.
*
*/
typedef struct TriggerTimeNPriority
{
    /* date */
    BACnetDate_t            m_stTriggerDate;
    /* time */
    BACnetTime_t            m_stTriggerTime;
    /* priority */
    uint32_t                m_u32EventPriority;
}TriggerTimeNPriority_t;

typedef struct TimeValueRange
{
	/* time */
	BACnetTime_t m_stTime;
	/* value */
	uint8_t u8Value;
	/* tag */
	uint8_t u8TagType;
}TimeValueRange_t;

typedef struct BACnetWeeklySchdl
{
	TimeValueRange_t Range[6];
	uint8_t u8NumOfEntry;
	uint8_t u8WeekDay;
}BACnetWeeklySchdl_t;

/**
*
* NAME: Schedule
*
* DESCRIPTION
* BACnet schedule object structure with all properties.
*
*/
typedef struct Schedule
{
    /** numeric code that is used to identify the object */
    Pr_BACnetObjId_t		    m_stObjectID;

    /** name for the object */
    Pr_BACnetCharStr_t		    m_stObjName;

    /** value of this property shall be Schedule */
    Pr_BACnetObjType_t		    m_stObjectType;

    /** indicates the current value of the schedule, which may be any primitive datatype */
    AnyValue_t      			m_stPresentValue;

	/** dummy present vlaue */
	AnyValue_t					m_stBackupPresentValue;

    /** this property holds a default value to be used for the Present_Value property */
    AnyValue_t      			m_stScheduleDefault;

    /** represents four boolean flags that indicate the general "health" of object */
    Pr_BACnetBitStr_t			m_stStatusFlag;

    /** provides an indication of whether the present value or the operation of the
	physical input in question is "reliable" */
    Pr_BACnetReliability_t		m_stReliability;

    /** an indication whether (true) or not (false) the physical input that the object
	represents is not in service */
    Pr_BACnetBool_t			    m_stOutOfService;

    /** specifies the range of dates within which the schedule object is active */
    Pr_BACnetDateRange_t        m_stEffectivePeriod;

    /** priority at which the referenced properties are commanded only for writing */
    Pr_BACnetUnsigned32_t       m_stPriorityForWriting;

    /* specifies the list of device id, object id and id of the properties to be written with
    specific values at specific times on specific days */
	Pr_ListOfBACnetDevObjPropRef_t 	m_stListOfObjectPropertyReference;

   /* each special event describes a sequence of schedule actions that takes
    precedence over the normal day's behavior on a specific day or days */
    Pr_ListOfBACnetSpecialEvent_t   m_stExceptionSchedule;

    /* describe the sequence of schedule actions on each day of the week */
    Pr_ListOfBACnetDailySchedule_t	m_stWeeklySchedule;

    /* used to get next trigger time in scheduling */
    TriggerTimeNPriority_t      m_stTimeNPriority;

    Pr_BACnetCharStr_t			m_stDescription;

	#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object.
	the Object_Name, Object_Type, Object_Identifier, and Property_List properties
	are not included in the list */
    Pr_BACnetPropertyList_t		m_stPropertyList;
	#endif

	#ifdef BACDEL_SER_DS_COV_B
	/** cov variable which describes the cov service status */
	int32_t						m_i32ObjCOVSupport;
	#endif

	//kv
	Pr_BACnetBool_t				m_stEventDetectionEnable;
	Pr_BACnetUnsigned32_t		m_stNotificationClass;
	Pr_BACnetBitStr_t			m_stEventEnable;
	Pr_BACnetEventState_t		m_stEventState;
	Pr_BACnetBitStr_t			m_stAckedTransitions;
	Pr_BACnetNotifyType_t		m_stNotifyType;
	Pr_BACnetEventTimeStamp_t   m_stEventTimeStamps;

	/** this flag bit is added to for managing callback registration */
	bool						bAppCallBack;

#if ((defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_B))

	/** The Summary variable which describes the GEI and GAS service status */
    int32_t						m_i32GetSummary;

#endif
	/** for dynamic object add/delete */
    struct Schedule *pstNext;

}Schedule_t;

/**
*
* DISCREPTION
* Function is used to assign property list for schedule object.
* this list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] pointer to required property list
* @param pOptional [Out] pointer to optional property list
* @param pProprietary [Out] pointer to proprietary property list
*
*/
void Schedule_Property_Lists(
	int32_t **pRequired,
	int32_t **pOptional,
	int32_t **pProprietary);

#endif /* SDL object */
#endif /* SDL_H */

/******************************** end of header file *************************/
