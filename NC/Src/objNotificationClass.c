/*
 * objNotificationClass.c
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
*   objNotificationClass.c
*
*	AUTHORS
*	Heramb Joshi
*
*   DESCRIPTION
*   This file contains structures and function definitions related to
*   notification class object type.
*
*******************************************************************************/

#ifdef BACDEL_OBJ_NC

/** include header files */
#include "objNotificationClass.h"

/** required properties */
int32_t ai32NotificationClassReqProp[MAX_PROPERTY_LIST] =
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
	PROP_NOTIFICATION_CLASS,
	PROP_PRIORITY,
	PROP_ACK_REQUIRED,
	PROP_RECIPIENT_LIST,
    ENDOFPROPLIST
};

/** optional properties */
int32_t ai32NotificationClassOptProp[MAX_PROPERTY_LIST] =
{
	PROP_DESCRIPTION,
    ENDOFPROPLIST
};

/** proprietary properties */
int32_t ai32NotificationClassProProp[MAX_PROPERTY_LIST] =
{
    ENDOFPROPLIST
};

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
	int32_t **pProprietary)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Notification_Class_Property_Lists: Entry \r\n");
	#endif

	/* save array pointers */
	if(pRequired)
	{
		/* required properties */
		*pRequired = ai32NotificationClassReqProp;
	}
	if(pOptional)
	{
		/* optional properties */
		*pOptional = ai32NotificationClassOptProp;
	}
	if(pProprietary)
	{
		/* proprietary properties */
		*pProprietary = ai32NotificationClassProProp;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Notification_Class_Property_Lists: Exit \r\n");
	#endif
	return;
}

#endif /* NC object */

/******************************** end of source file *************************/



