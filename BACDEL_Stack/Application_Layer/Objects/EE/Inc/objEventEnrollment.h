/*
 * objEventEnrollment.h
 *
 *  Created on: Nov 7, 2019
 *      Author: Pranav.Phadatare
 */
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
*   objEventEnrollment.h
*
*	AUTHORS
*	Pratham Murkute
*
*   DESCRIPTION
*   This file contains structures and function definitions related to
*   event enrollment object type.
*
*******************************************************************************/
#ifndef OBJEVENTENROLLMENT_H_
#define OBJEVENTENROLLMENT_H_
#ifdef BACDEL_OBJ_EE

/** include header files */
#include "bacDELDef.h"
#include "propertydef.h"

/**
*
* NAME: Event Enrollment
*
* DESCRIPTION
* BACnet event enrollment object structure with all properties.
*
*/
typedef struct EventEnrollment
{
	/** numeric code that is used to identify the object */
    Pr_BACnetObjId_t			m_stObjectID;

	/** name for the object */
    Pr_BACnetCharStr_t			m_stObjName;

	/** value of this property shall be EVENT ENROLLMENT */
    Pr_BACnetObjType_t			m_stObjectType;

	/** indicates the type of event algorithm */
	Pr_BACnetEventType_t		m_stEventype;

	/** indicates notifications generated is an events or alarms */
	Pr_BACnetNotifyType_t		m_stNotifyType;

	/** determines the algorithm used to monitor the referenced object and
	provides the parameter values needed for this algorithm */
	Pr_BACnetEventParameter_t	m_stEventParameter;

	/** particular object and property referenced by this event enrollment object */
	Pr_BACnetDevObjPropRef_t 	m_stObjectPropertyReference;

	/** contains the current state of the event */
	Pr_BACnetEventState_t		m_stEventState;

	/** whether notifications are enabled for TO-OFFNORMAL, TO-FAULT and TO-NORMAL transitions */
	Pr_BACnetBitStr_t			m_stEventEnable;

	/** convey three separate flags that each indicate whether the most recent
	TO-OFFNORMAL, TO-FAULT or TO-NORMAL event transitions have been acknowledged */
	Pr_BACnetBitStr_t			m_stAckedTransitions;

    /** notification class to be used when handling and generating event
	notifications for this object */
    Pr_BACnetUnsigned32_t		m_stNotificationClass;

	/** convey the times of the last event notifications for TO-OFFNORMAL, TO-FAULT
	and TO-NORMAL events respectively */
	Pr_BACnetEventTimeStamp_t   m_stEventTimeStamps;

	/** this flag bit is added to inform that:
	if TRUE = event type, event parameter & object property refrence contain
	valid values.
	if FALSE = event parameter or object property refrence have inconsistent
	parameters with respect to event type */
    bool						bObjectEnable;

	#if (STACK_CONFIG_SUPPORT_EXTERNAL_ALARMS)
	/** this property is used to store the present value received from application
	this is used in external algorithmic timer to match the value after
	time delay */
	AnyValue_t      			m_stExternalValue;

	/** This property is used to store the feedback value received from application
	This is used in command failure algorithm to match with present value for
	generation of external event algorithm */
	AnyValue_t      			m_stExternalFeedbackValue;

	/** indicates the current status flags value of external object */
	Pr_BACnetBitStr_t			m_stExternalStatusFlag;
	#endif /* */

	#ifdef BACDEL_SER_DS_COV_B
	/** cov variable which describes the cov service status */
	int32_t						m_i32ObjCOVSupport;
	#endif

	#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object.
	the Object_Name, Object_Type, Object_Identifier, and Property_List properties
	are not included in the list */
	Pr_BACnetPropertyList_t		m_stPropertyList;

	/** represents four boolean flags that indicate the general "health" of an
	event enrollment object */
    Pr_BACnetBitStr_t			m_stStatusFlag;

	/** provides an indication of whether the object or the operation of
	the physical input in question is "reliable" as far as the bacnet
    device or operator can determine and, if not, why? **/
    Pr_BACnetReliability_t		m_stReliability;

	#ifdef OPTIONAL_PROPERTY
	/** indicates the type of fault algorithm */
	Pr_BACnetFaultType_t		m_stFaultType;

	/* determines the algorithm used to monitor the referenced object and
	provides the parameter values needed for this algorithm */
	Pr_BACnetFaultParameter_t	m_stFaultParameter;
	#endif
	#endif /* PR14 */

	#ifdef BACDEL_SER_AE_EN_B
	/** the char string that the object will send in event notification
	whenever it is generated. this string is to be set using an
	exported api */
    NotificationString_t		m_stNotificationMsg;
	#endif

	#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_PR14)
	/** an indication whether (TRUE) or not (FALSE) object is configured for
	intrinsic reporting */
    Pr_BACnetBool_t				m_stEventDetectionEnable;

	/** an indication whether (TRUE) or not (FALSE) detection of offnormal condition to be
	dynamically suppressed */
    Pr_BACnetBool_t				m_stEventAlgorithmInhibit;

	#ifdef OPTIONAL_PROPERTY
	/** in order to allow a different time delay to be used for TONORMAL event transitions, the
	Time_Delay_Normal property is added to all event initiating objects */
    Pr_BACnetUnsigned32_t		m_stTimeDelayNormal;
	#endif
	#endif /* */

	#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B || defined BACDEL_PR14)
	/** reliability evaluation inhibit */
    Pr_BACnetBool_t				m_stReliabilityEvaluationInhibit;

	/** as reliability-evalution-inhibit is true,
	bRelEvalInhibitBackup = false indicates that change event state to normal even
	if present value matches to alarm/fault value in UpdateEventState_On_Reliablity() function
    for all applicable objects */
    bool						m_bBackupRelEvalInhibit;

	/** event message text */
    Pr_BACnetEventMsgText_t		m_stEventMsgText;

	/** event message text config  */
    Pr_BACnetEventMsgText_t		m_stEventMsgTextConfig;
	#endif

	#if ((defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_B))
	/** the summary variable which describes the GEI and GAS service status */
    int32_t						m_i32GetSummary;
	#endif

	#ifdef OPTIONAL_PROPERTY
	/** name of an object profile to which this object conforms */
	Pr_BACnetCharStr_t			m_stProfileName;

	/** description about the object is a string of printable characters
	whose content is not restricted */
	Pr_BACnetCharStr_t			m_stDescription;
	#endif

#ifdef ALGORITHMIC_REPORTING
		/** list of EE object instances in which properties of this object are
		referenced for internal algorithmic reporting */
		Pr_ListofEEObjPropRef_t		m_stEEObjectlist;
#endif
	/* this flag bit is added to for managing callback registration */
	bool						bAppCallBack;

	/** for dynamic object add/delete */
	struct EventEnrollment *pstNext;
	struct EventEnrollment *pstLAST;
}EventEnrollment_t;

/**
*
* DISCREPTION
* Function is used to assign property list for trend log object.
* this list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] pointer to required property list
* @param pOptional [Out] pointer to optional property list
* @param pProprietary [Out] pointer to proprietary property list
*
*/
void Event_Enrollment_Property_Lists(
	int32_t **pRequired,
	int32_t **pOptional,
	int32_t **pProprietary);

#endif /* EE object */


#endif /* OBJEVENTENROLLMENT_H_ */
