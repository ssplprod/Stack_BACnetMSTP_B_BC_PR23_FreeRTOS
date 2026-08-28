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
*   File Name - propertyClearValues.h
*
*   RELEASE HISTORY
*
*	DATE				NAME					DESCRIPTION
*	18th March 2013		Pratham N. Murkute		Created file & moved api's.
*	18th March 2013		Pratham N. Murkute		Added new api's.
*
****************************************************************************/

#ifndef PROP_CLEAR_H
#define PROP_CLEAR_H

/** header file includes */
#include "bacDELApi.h"
#include "bacDELPropertyDef.h"

/** clears memory allocated to property value in primitive datatype format */
void Clear_Property_Value(BACNET_PROPERTY_VALUE * pstPropertyVal);

/** clears property value */
void Clear_Prop_Val(property_value_t **ppPropValList);

/** clears memory allocated to context (constructed) property value */
void Clear_ContextProperty_Value(
	BACNET_DATA_TYPE ePropDataType, void *pvAppValue, 
    uint32_t u32arrayIndex, bool bArrIndxFlag);

/** clears memory allocated as per data types */
void Clear_PropVal_AsPer_DataType(
	BACNET_DATA_TYPE eDataType, 
	void **ppPropValue);

/** clears memory allocated during Rp-B, Rpm-B and Rr-b as per data types */
void Clear_Duplicate_Prop_Values(
	BACNET_DATA_TYPE eData_Type, 
	void **ppPropValue); 

/** clears memory allocated to internal lists */
/** clears memory allocated while writting properties using exported api */
void Clear_Internal_ListType_Properties(
	void **pvPropVal,
	BACNET_DATA_TYPE eDataType);

/** api to clear the global device address binding list. */
void Clear_Global_Device_Addr_Binding_List(void);

/** clears memory allocated to list of address  binding values */
void Clear_AddressBinding_List(BACnetAddrBinding_t **ppAddrBindList, bool bGlobalList);

/** clears memory allocated to list of boolean values */
void Clear_Boolean_List(ListOfBoolen_t **ppBooleanList);

/** clears memory allocated to list of bit string values */
void Clear_BitString_List(ListOfBitStr_t **ppBitStrList);

/** clears memory allocated to list of unsigned values */
void Clear_UnsignedInt_List(ListOfUnsigned_t **ppUnsignIntList);

/** clears memory allocated to list of enum values */
void Clear_Enumeration_List(ListOfEnum_t **ppEnumerationList);

/** clears memory allocated to list of character string values */
void Clear_CharString_List(ListOfCharStr_t **ppCharStrList);

/** clears memory allocated to list of recipient values */
void Clear_Recipient_List(ListOfBACnetRecipient_t **ppRecpList);

/** clears memory allocated to list of destination values */
void Clear_Destination_List(ListOfBACnetDestination_t **ppDestList);

/** clears memory allocated to list of object id values */
void Clear_ObjectID_List(ListOfObjId_t **ppObjectIDList);

/** clears memory allocated to list of device object property reference values */
void Clear_DevObjPropReff_List(ListOfBACnetDevObjPropRef_t **ppDevObjPropRefList);

/** clears memory allocated to active cov subsciption list */
void Clear_ActiveCovSubs_List(ListOfBACnetCovSubs_t **ppActCovSubList);

/** clears memory allocated to list of extended parameters in the event 
    prameters with event type EXTENDED. */
void Clear_ExtendedParameters_List(Parameters_t **ppExtParamList);

/** clears memory allocated to list of property states in the event 
    prameters with event type CHANGE OF STATE. */
void Clear_PropertyStates_List(ListOfBACnetPropertyStates_t **ppPropStatesList);

/** clears memory allocated to list of BACnet VT Class */
void Clear_VtClass_List(Pr_BACnetVTClass_t **ppVTClass);

/** clears memory allocated to list of BACnet VT Sessions */
void Clear_VtSessions_List(ListOfBACnetVTSession_t **ppVTSessionsList);

/** clears memory allocated to list of BACnet Action command type */
void Clear_ActionCmd_List(ListOfBACnetActionCommand_t **ppActionList);

/** clears memory allocated to list of BACnet Action type */
void Clear_Action_List(ListOfBACnetActionList_t **ppActionList);

/** clears memory allocated to list of Read Access Result properties type */
void Clear_GpResults_List(listOfResults_t **ppPropResultList);

/** clears memory allocated to list of bacnet Read Access Result type */
void Clear_ReadAccessResult_List(ListOfReadAccessResult_t **ppReadAccessRes);

/** clears memory allocated to list of Read Access Specification properties type */
void Clear_GpSpecs_List(ListOfBACnetPropRef_t **ppPropRefList);

/** clears memory allocated to list of bacnet Read Access Specification type */
void Clear_ReadAccessSpecs_List(ListOfReadAccessSpecs_t **ppReadAccessSpecif);

/** clears memory allocated to list of bacnet device object reference type */
void Clear_DevObjRef_List(ListOfBACnetDevObjRef_t **ppDevObjRef);

/** clears memory allocated to list of bacnet trend log record type */
void Clear_TrendLogRecord_List(ListOfBACnetLogRecord_t **ppLogRecordList);

/** clears memory allocated to list of bacnet Event log record type */
void Clear_EventLogRecord_List(ListOfBACnetEventLogRecord_t **ppEvntLogRecoList);

/** clears memory allocated to list of bacnet trend log Multiple record type */
void Clear_TrendLogMultipleRecord_List(ListOfBACnetLogMultipleRecord_t **ppTLogMultRecoList);

/** clears memory allocated to list of Log Data type properties */
void Clear_LogDataTLM_List(BACnetLogData_t **ppLogDataList);

/** clears memory allocated to Array of bacnet Authentication Factor format type */
void Clear_AuFactorFormat_List(ListOfBACnetAuFactorFormat_t **ppAuthefactFormList);

/** clears memory allocated to list of bacnet Optional Character String type */
void Clear_OptionalCharString_List(ListOfOptCharStr_t **ppOptCharStrList);

/** clears memory allocated to List of policy */
void Clear_Policy_List(Policy_t **ppPolicyList);

/** clears memory allocated to array of bacnet Authentication Policy type */
void Clear_AuPolicy_List(ListOfBACnetAuPolicy_t **ppAuthePolicyList);

/** clears memory allocated to array of bacnet Access Rule type */
void Clear_AccessRule_List(ListOfBACnetAccessRule_t **ppAccessRule);

/** clears memory allocated to array of bacnet Credential Authentication Factor */
void Clear_CredentialAuFactor_List(ListOfBACnetCredAuFactor_t **ppCredAutheFact);

/** clears memory allocated to array of bacnet Assigned Access Rights type */
void Clear_AssignedAccessRights_List(ListOfBACnetAssignedAccessRights_t **ppAssAcceRit);

#ifdef BACDEL_PR12

/** clears memory allocated to array of Network security Policy */
void Clear_NwSecurityPolicy_List(ListOfBACnetNwSecurityPolicy_t **ppNwSecPolArr);

/** clears memory allocated to List of Key Identifier */
void Clear_KeyId_List(ListOfBACnetKeyId_t **ppKeyIdList);

/** clears memory allocated to List of Security Key set */
bool Clear_SecurityKeySet_List(Pr_ListOfBACnetSecurityKeySet_t  *pstSecKeySetList);

/** clears memory allocated to List of Property Access Result */
void Clear_PropAccessResult_List(ListOfBACnetPropAccessRslt_t **ppAccessResult);

#endif /* BACDEL_PR12 */

/** clears memory allocated to notification parameters */
void Clear_Notification_Parameter(
	BACnetNotificationParameters_t *pstNotifyParameters, 
	BACNET_EVENT_TYPE eEventType);

/** clears memory allocated to List of Time-Values */
void Clear_TimeValues_List(BACnetTimeValue_t **ppFreeTimeValue);

/** clears memory allocated to array of special event */
void Clear_ExceptionShdl_List(ListOfSpecialEvent_t **ppSplEventList);

/** clears memory allocated to array of daily schedule */
void Clear_WeeklyShdl_List(Pr_ListOfBACnetDailySchedule_t *pstWeeklyList);

/** clears memory allocated to List of calendar entry */
void Clear_CalendarEntry_List(ListOfBACnetCalendarEntry_t **ppCalendarEntryList);

#ifdef BACDEL_OBJ_NP

void Clear_BdtEntry_List(ListOfBACnetBDTEntry_t **ppBdtEntryList);

void Clear_FdtEntry_List(ListOfBACnetFDTEntry_t **ppFdtEntryList);

void Clear_OctetString_List(ListOfOctetStr_t **ppOctetStrList);

#endif
#endif /* PROP_CLEAR_H */
