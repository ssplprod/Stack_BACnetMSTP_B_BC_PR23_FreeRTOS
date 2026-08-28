/*
 * objNotificationClass.h
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
*   objNotificationClass.h
*
*	AUTHORS
*	Heramb Joshi
*
*   DESCRIPTION
*   This file contains structures and function definitions related to
*   notification class object type.
*
*******************************************************************************/
#ifndef OBJNOTIFICATIONCLASS_H_
#define OBJNOTIFICATIONCLASS_H_

#ifdef BACDEL_OBJ_NC

/**	Include Files */
#include "bacDELDef.h"
#include "propertydef.h"

/**
*
* NAME: Notification Class
*
* DESCRIPTION
* BACnet notification class object structure with all properties.
*
*/
typedef struct Notificationclass
{
    /** numeric code that is used to identify the object */
    Pr_BACnetObjId_t			m_stObjectID;

    /** name for the object */
    Pr_BACnetCharStr_t			m_stObjName;

    /** value of this property shall be Notification Class */
    Pr_BACnetObjType_t			m_stObjectType;

    /** the notification class to be used when handling and generating event
	notifications for this object */
    Pr_BACnetUnsigned32_t		m_stNotificationClass;

	/** priorty to be used for event notification */
	Pr_BACnetNotifyPriority_t 	m_stPriority;

    /** convey three flags that separately represent whether acknowledgment
	shall be required in notifications generated for To-OFFNORMAL, TO-FAULT
	and TO-NORMAL events */
    Pr_BACnetBitStr_t			m_stAckRequired;

	/** convey the recipient/destinations for the notifications */
	Pr_ListOfBACnetDestination_t	m_stRecipientList;

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

	#ifdef OPTIONAL_PROPERTY
	/** name of an object profile to which this object conforms */
	Pr_BACnetCharStr_t			m_stProfileName;

	/** description about the object is a string of printable characters
	whose content is not restricted */
	Pr_BACnetCharStr_t			m_stDescription;

	//kv
	Pr_BACnetBitStr_t			m_stStatusFlag;

	Pr_BACnetEventState_t		m_stEventState;

	Pr_BACnetReliability_t		m_stReliability;

	Pr_BACnetBool_t				m_stEventDetectionEnable;

	Pr_BACnetBitStr_t			m_stEventEnable;
	Pr_BACnetBitStr_t			m_stAckedTransitions;
	Pr_BACnetNotifyType_t		m_stNotifyType;
	Pr_BACnetEventTimeStamp_t   m_stEventTimeStamps;

	#endif

	/** this flag bit is added to for managing callback registration */
	bool						bAppCallBack;

#if ((defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_B))

	/** The Summary variable which describes the GEI and GAS service status */
    int32_t						m_i32GetSummary;

#endif

	/** for dynamic object add/delete */
	struct Notificationclass *pstNext;
	struct Notificationclass  *pstLAST;
}Notificationclass_t;

/**
*
* DISCREPTION
* Function is used to assign property list for notification class object.
* this list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] pointer to required property list
* @param pOptional [Out] pointer to optional property list
* @param pProprietary [Out] pointer to proprietary property list
*
*/
void Notification_Class_Property_Lists(
	int32_t **pRequired,
	int32_t **pOptional,
	int32_t **pProprietary);

#endif /* NC object */

#endif /* OBJNOTIFICATIONCLASS_H_ */
