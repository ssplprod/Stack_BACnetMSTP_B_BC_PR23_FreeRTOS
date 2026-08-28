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
*   objSchedule.c
*
*	AUTHORS
*	Heramb Joshi
*
*   DESCRIPTION
*   This file contains structures and function definitions related to
*   schedule object type.
*
*******************************************************************************/

#ifdef BACDEL_OBJ_SDL

/** include header files */
#include "objSchedule.h"

/** required properties */
const int32_t ai32ScheduleReqProp[MAX_PROPERTY_LIST] =
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_EFFECTIVE_PERIOD,
    PROP_SCHEDULE_DEFAULT,
    PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES,
    PROP_PRIORITY_FOR_WRITING,
    PROP_STATUS_FLAGS,
    PROP_RELIABILITY,
    PROP_OUT_OF_SERVICE,
	ENDOFPROPLIST
};

/** optional properties */
const int32_t ai32ScheduleOptProp[MAX_PROPERTY_LIST] =
{
	PROP_DESCRIPTION,
    PROP_WEEKLY_SCHEDULE,
    PROP_EXCEPTION_SCHEDULE,
    ENDOFPROPLIST
};

/** proprietary properties */
const int32_t ai32ScheduleProProp[MAX_PROPERTY_LIST] =
{
    ENDOFPROPLIST
};

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
	int32_t **pProprietary)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Schedule_Property_Lists: Entry \r\n");
	#endif

	/* save array pointers */
	if(pRequired)
	{
		/* required properties */
		*pRequired = (int32_t*)ai32ScheduleReqProp;
	}
	if(pOptional)
	{
		/* optional properties */
		*pOptional = (int32_t*)ai32ScheduleOptProp;
	}
	if(pProprietary)
	{
		/* proprietary properties */
		*pProprietary = (int32_t*)ai32ScheduleProProp;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Schedule_Property_Lists: Exit \r\n");
	#endif
	return;
}

#endif /* SDL object */

/******************************** end of source file *************************/
