/*
 * objCalendar.c
 *
 *  Created on: Nov 6, 2019
 *      Author: Pranav.Phadatare
 */

#ifdef BACDEL_OBJ_CAL

/** include header files */
#include "objCalendar.h"
#include "propertyGenricHandler.h"
#include "propertyValidations.h"
#include "pduDateTime.h"

/** required properties */
int32_t ai32CalendarReqProp[MAX_PROPERTY_LIST] =
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_DATE_LIST,
    ENDOFPROPLIST
};

/** optional properties */
int32_t ai32CalendarOptProp[MAX_PROPERTY_LIST] =
{
	PROP_DESCRIPTION,
    ENDOFPROPLIST
};

/** proprietary properties */
int32_t ai32CalendarProProp[MAX_PROPERTY_LIST] =
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
void Calendar_Property_Lists(
	int32_t **pRequired,
	int32_t **pOptional,
	int32_t **pProprietary)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Calendar_Property_Lists: Entry \r\n");
	#endif

	/* save array pointers */
	if(pRequired)
	{
		/* required properties */
		*pRequired = ai32CalendarReqProp;
	}
	if(pOptional)
	{
		/* optional properties */
		*pOptional = ai32CalendarOptProp;
	}
	if(pProprietary)
	{
		/* proprietary properties */
		*pProprietary = ai32CalendarProProp;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Calendar_Property_Lists: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* Function reads date list of calendar object and calculates the
* present value.
*
* @param ul32BaseAddr [in] base address of object
* @param pVirtualDev  [in] virtual device data
*
* @return [out] true or false. true is calendar is active
*
*/
bool Calendar_Get_Present_Value(ulong32_t ul32BaseAddr, void *pVirtualDev)
{
	/* local variables */
    BACnetDate_t stSystemDate = {0};
    BACnetTime_t stSystemTime = {0};
	bool bRetValue = false;
    ulong32_t ul32OffsetAddr = 0;
	Pr_ListOfBACnetCalendarEntry_t *pstDateList = {0};
    ListOfBACnetCalendarEntry_t *pstListOfCalendar = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Calendar_Get_Present_Value: Entry \r\n");
	#endif

	/* read the current date list of calendar object */
    GET_OBJCALPROP_OFFSET(OBJECT_CALENDAR, m_stDateList ,ul32OffsetAddr);
    pstDateList = (Pr_ListOfBACnetCalendarEntry_t *)(ul32BaseAddr + ul32OffsetAddr);
    pstListOfCalendar = pstDateList->m_pstListOfCalendar;

	/* get the current system date-time */
    GetDevice_DateTime(&stSystemDate, &stSystemTime, pVirtualDev);

	/* traverse date list 1 by 1 */
    while(NULL != pstListOfCalendar)
    {
		/* check calendar entry type */
        if(pstListOfCalendar->m_eStatusCalendar == STATUS_DATE)
        {
			/* check if active date */
			if(Is_Active_Date(&pstListOfCalendar->m_stCalendar.m_stDate, &stSystemDate))
			{
				/* set the flag */
                bRetValue = true;
				break;
			}
        }
        else if(pstListOfCalendar->m_eStatusCalendar == STATUS_DATE_RANGE)
        {
			/* check if active date range */
            if(Is_Active_DateRange(&pstListOfCalendar->m_stCalendar.m_stDateRange, &stSystemDate))
			{
				/* set the flag */
                bRetValue = true;
				break;
			}
        }
        else
        {
			/* check if active week n day */
            if(IsWeekInDay(&pstListOfCalendar->m_stCalendar.m_stWeekNDay, &stSystemDate))
            {
				/* set the flag */
                bRetValue = true;
				break;
			}
        }

		/* move to next node */
        pstListOfCalendar = pstListOfCalendar->m_pstNext;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Calendar_Get_Present_Value: Exit \r\n");
	#endif
    return bRetValue;
}

#endif /* CAL object */

/******************************** end of source file *************************/
