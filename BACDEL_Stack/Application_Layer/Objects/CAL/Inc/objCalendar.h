/*
 * objCalendar.h
 *
 *  Created on: Nov 6, 2019
 *      Author: Pranav.Phadatare
 */

#ifndef OBJCALENDAR_H_
#define OBJCALENDAR_H_

#ifdef BACDEL_OBJ_CAL

/** include header files */
#include "bacDELDef.h"
#include "propertydef.h"

/**
*
* NAME: Calendar
*
* DESCRIPTION
* BACnet schedule object structure with all properties.
*
*/
typedef struct Calendar
{
    /** numeric code that is used to identify the object*/
    Pr_BACnetObjId_t			m_stObjectID;

    /** name for the object */
    Pr_BACnetCharStr_t			m_stObjName;

    /** value of this property shall be Calendar */
    Pr_BACnetObjType_t		    m_stObjectType;

    /* either an individual date, range of dates, or month/week-of-month/day-of-week specification */
    Pr_ListOfBACnetCalendarEntry_t   m_stDateList;

    /** current value of calendar: TRUE if current date is in the Date_List else FALSE */
    Pr_BACnetBool_t			    m_stPresentValue;

	/** dummy present vlaue */
    Pr_BACnetBool_t             m_stBackupPresentValue;

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

	#if ((defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_B))
	/** the summary variable which describes the GEI and GAS service status */
    //int32_t						m_i32GetSummary;
	#endif

	#ifdef OPTIONAL_PROPERTY
    /** name of an object profile to which this object conforms */
	Pr_BACnetCharStr_t			m_stProfileName;

	/** description about the object is a string of printable characters
	whose content is not restricted */
	Pr_BACnetCharStr_t			m_stDescription;
	#endif

	/** this flag bit is added to for managing callback registration */
	bool						bAppCallBack;

	/** for dynamic object add/delete */
    struct Calendar	*pstNext;
	struct Calendar *pstLAST;
}Calendar_t;

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
	int32_t **pProprietary);

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
bool Calendar_Get_Present_Value(ulong32_t ul32BaseAddr, void *pVirtualDev);

#endif /* CAL object */


#endif /* OBJCALENDAR_H_ */
