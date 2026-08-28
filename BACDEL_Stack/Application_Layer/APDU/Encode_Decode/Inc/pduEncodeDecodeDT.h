/********************************************************************************
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
*   File Name - pduEncodeDecodeDT.c
*
*   AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION                                                            
*   This file includes encoding and decoding of all property data-types 
*	supported by stack.
*
*********************************************************************************/

#ifndef DT_ENCODE_DECODE_H
#define DT_ENCODE_DECODE_H

/** Header files */
#include "propertydef.h"

/** Compile as a C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
*  Macros Definations
********************************************************************************/

/** Macros */
#define DT_ERR_INVALID_TAG				-1
#define DT_ERR_INVALID_TAG_LENGTH		-2
#define DT_ERR_INVALID_DATA_TYPE		-3
#define DT_ERR_INVALID_INPUTS			-4
#define DT_ERR_DATA_TYPE_NOT_SUPPORTED	-5
#define DT_ERR_VALUE_OUT_OF_RANGE		-6
#define DT_ERR_MALLOC_FAILED			-7
#define DT_ERR_8	-8
#define DT_ERR_9	-9
#define DT_ERR_10	-10


/*******************************************************************************
*  Decoding Function Declarations
********************************************************************************/
int32_t DT_Decode_BdtEntry_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	ListOfBACnetBDTEntry_t **pstBdtEntryList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);
int32_t DT_Decode_FdtEntry_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	ListOfBACnetFDTEntry_t **pstFdtEntryList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);
int32_t DT_Decode_HostNPort(
	uint8_t *pu8Apdu,
	BACnetHostNPort_t *pstHostNPort);
/** Function to decode date-time data type */
int32_t DT_Decode_DateTime(
	uint8_t *pu8Apdu, 
	BACnetDate_t *pstDate, 
	BACnetTime_t *pstTime);

/** Function to decode time-stamp data type */
int32_t DT_Decode_TimeStamp(
	uint8_t *pu8Apdu,
	timeStamp_u *puTimeStamp, 
	BACNET_TIMESTAMP_TYPE *peTimeStampType);

/** Function to decode array-of-object-id data type */
int32_t DT_Decode_ObjectID_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfObjId_t **pstObjIdList,
	uint32_t *pu32Count);

/** Function to decode array-of-bool data type */
int32_t DT_Decode_Boolean_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBoolen_t **pstBoolList,
	uint32_t *pu32Count);

/** Function to decode address data type */
int32_t DT_Decode_Address(
	uint8_t *pu8Apdu, 
	BACnetAddress_t *pstAddress);

/** Function to decode Address binding data type */
int32_t DT_Decode_AddressBinding(
	uint8_t *pu8Apdu, 
	BACnetAddrBinding_t *pstAddrBind);

/** Function to decode arrar-of-object-id data type */
int32_t DT_Decode_AddressBinding_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	BACnetAddrBinding_t *pstAddrBindList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode list of VT-Class data type */
int32_t DT_Decode_VtClass_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	Pr_BACnetVTClass_t **pstVtClassList,
	uint32_t *pu32Count);

/** Function to decode list of VT-session data type */
int32_t DT_Decode_VtSession_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetVTSession_t **pstVtSessionList,
	uint32_t *pu32Count);

/** Function to decode recipient data type */
int32_t DT_Decode_Recipient(
	uint8_t *pu8Apdu, 
	BACnetRecipient_t *pstRecipient);

/** Function to decode list of list of recipients data type */
int32_t DT_Decode_Recipient_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetRecipient_t **pstRecipientList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

int32_t DT_Decode_RecipientProcess(
	uint8_t *pu8Apdu, 
	BACnetRecipient_t *pstRecipient,
	uint32_t *pu32ProcessId);

/** Function to decode object property reference data type */
int32_t DT_Decode_ObjPropReff(
	uint8_t *pu8Apdu, 
	BACnetObjPropRef_t *pstObjPropRef);

/** Function to decode cov-subscription data type */
int32_t DT_Decode_CovSubscription(
	uint8_t *pu8Apdu, 
	ListOfBACnetCovSubs_t *pstCovSubscription);

/** Function to decode list of cov-subscription data type */
int32_t DT_Decode_CovSubscription_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetCovSubs_t **pstCovSubscription, 
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode array of time-stamp data type */
int32_t DT_Decode_TimeStamp_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetEventTimeStamp_t *pstTimeStamp);

/** Function to decode priority array data type */
int32_t DT_Decode_Priority_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetPriorityArray_t *pstPriorityArray);

/** Function to decode list-of-unsigned data type */
int32_t DT_Decode_Unsigned_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfUnsigned_t **pstUnitList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode BACnetClientCOV data type */
int32_t DT_Decode_ClientCOVIncrement(
	uint8_t *pu8Apdu,
	BACnetClientCOV_t *pstClientCOV);

/** Function to decode BACnetDateRange data type */
int32_t DT_Decode_DateRange(
	uint8_t *pu8Apdu,
	BACnetDateRange_t *pstDateRange);

/** Function to decode List of BACnetCalendarEntry data type */
int32_t DT_Decode_CalenderEntry_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetCalendarEntry_t **pstCalenderList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode device object property reference data type */
int32_t DT_Decode_DevObjPropReff(
	uint8_t *pu8Apdu, 
	BACnetDevObjPropRef_t *pstDevObjPropRef);

/** Function to decode set point reference data type. */
int32_t DT_Decode_SetPtReff(
	uint8_t *pu8Apdu, 
	BACnetSetpointRef_t *pstSetPtReff);

/** Function to decode list of BACnetDestination data type. */
int32_t DT_Decode_Destination_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetDestination_t **pstRecepientList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode list/array of BACnetDeviceObjectPropertyReference data type */
int32_t DT_Decode_DevObjPropRef_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetDevObjPropRef_t **pstDevObjPropReffList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode array of BACnetDailySchedule data type */
int32_t DT_Decode_DailySchedule_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_ListOfBACnetDailySchedule_t *pstDailySchedule,
	BACNET_PROPERTY_ID ePropID);

/** Function to decode array of BACnetSpecialEvent data type */
int32_t DT_Decode_SpecialEvent_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfSpecialEvent_t **pstListOfSpecialEvent,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID);

/** Function to decode list of BACnetLogRecord data type */
int32_t DT_Decode_LogRecord_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetLogRecord_t **pstLogRecordList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement);

/** Function to decode list of EventParameters data type */
int32_t DT_Decode_EventParameters(
	uint8_t *pu8Apdu, 
	BACnetEventParameter_t *pstEventParameter,
	uint32_t u32Max_Apdu_Len
	);

/** Function to decode array of Bitstring data type */
int32_t DT_Decode_BitString_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBitStr_t **pstBacnet_BitStringList,
	uint32_t *pu32Count);

/** Function to decode array of Optional characterstring */
int32_t DT_Decode_OpCharString_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfOptCharStr_t **pstOptCharStrList,
	uint32_t *pu32Count);

/** Function to deocode 'Any' datatype */
int32_t DT_Decode_AnyValue(uint8_t *pu8Apdu,
	BACNET_PROPERTY_VALUE *pstAnyValue);

/** Function to deocode device object reference dataype */
int32_t DT_Decode_DevObjReff(
	uint8_t *pu8Apdu, 
	BACnetDevObjRef_t *pstDevObjRef);

/** Function to decode list/array of BACnetDeviceObjectReference datatype */
int32_t DT_Decode_DevObjRef_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetDevObjRef_t **pstListOfDevObjReff,
	uint32_t *pu32Count);

/** Function to decode bacnet shed level data type */
int32_t DT_Decode_ShedLevel(
	uint8_t *pu8Apdu, 							
	Pr_BACnetShedLevel_t *pstBacnetShedLevel);

/** Function to decode list of BACnetLogMultipleRecord data type */
int32_t DT_Decode_LogMultipleRecord_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetLogMultipleRecord_t **pstLogMultipleRecord,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID);

/** Function to decode datatype - enum-list */
int32_t DT_Decode_Enum_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfEnum_t **pstEnumList,
	uint32_t *pu32Count);

/** Function to decode datatype - enum-list */
int32_t DT_Decode_EventLogRecord(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetEventLogRecord_t **pstEventLogRecord,
	uint32_t *pu32Count,
	uint32_t u32APDULen,
	BACNET_PROPERTY_ID ePropID);

/** Function to decode characterString array data type */
int32_t DT_Decode_CharString_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfCharStr_t *pstCharStrList,
	uint32_t *pu32Count);

/** Function to decode List of ReadAccessSpecification data type */
int32_t DT_Decode_ReadAccessSpecs_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfReadAccessSpecs_t **pstGpOfObj,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID);

/** Function to decode List of ReadAccessResult data type */
int32_t DT_Decode_ReadAccessResult_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfReadAccessResult_t **pstGpObjResults,
	uint32_t *pu32Count,	
	BACNET_PROPERTY_ID ePropID);

/** Function to decode NotificationPriority (BACnetARRAY[3] of Unsigned) */
int32_t DT_Decode_NotificationPriority(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetNotifyPriority_t *pstNotifyPriority);

/** Function to decode List of BACnetActionList data type */
int32_t DT_Decode_Action_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetActionList_t **pstAction_List,
	uint32_t *pu32Count,	
	BACNET_PROPERTY_ID ePropID);

/** Function to decode BACnetAuthenticationFactor datatype */
int32_t DT_Decode_AuthenticationFactor(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	BACnetAuFactor_t *pstAuthFactor);

/** Function to decode array of BACnetAuthenticationFactorFormat datatype */
int32_t DT_Decode_AuthFactorFormat_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAuFactorFormat_t **pstAuthFactFormat_Array,
	uint32_t *pu32Count);

/** Function to decode array of BACnetCredentialAuthenticationFactor datatype */
int32_t DT_Decode_CredentialAuthFactor_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetCredAuFactor_t **pstCredentialAuthFactor,
	uint32_t *pu32Count);

/** Function to decode list of BACnetAuthenticationPolicy datatype */
int32_t DT_Decode_AuthenticationPolicy_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAuPolicy_t **pstAuthPolicy,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID);

/** Function to deocode array of BACnetAssignedAccessRights datatype */
int32_t DT_Decode_AssAccessRights_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAssignedAccessRights_t **pstAccesssRights,
	uint32_t *pu32Count);

/** Function to deocode array of BACnetAccessRule datatype */
int32_t DT_Decode_AccessRule_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAccessRule_t **pstAccessRule,
	uint32_t *pu32Count);

/* Function to decode BACnetPropertyValue datatype */
int32_t DT_Decode_PropertyValue(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen,
	property_value_t *pstPropVal,
	BACNET_OBJECT_TYPE eObjType);

/* Function to decode list of BACnetPropertyValue datatype */
int32_t DT_Decode_PropValue_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	property_value_t *pstPropValueList,
	BACNET_OBJECT_TYPE eObjType,
	uint32_t *pu32Count);

#ifdef BACDEL_PR12

/** Function to decode Network security policy data type. */
int32_t DT_Decode_NwSecurityPolicy(
	uint8_t *pu8Apdu, 
	ListOfBACnetNwSecurityPolicy_t *pstSecurityPolicy);

/** Function to decode Array of Network security policy data type. */
int32_t DT_Decode_NwSecurityPolicy_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetNwSecurityPolicy_t **pstSecurityPolicyArr,
	uint32_t *pu32Count);

/** Function to decode Key ID policy data type. */
int32_t DT_Decode_KeyID(
	uint8_t *pu8Apdu, 
	ListOfBACnetKeyId_t *pstKeyID);

/** Function to decode list of Key ID datatype */
int32_t DT_Decode_KeyID_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetKeyId_t *pstKeyIdList,
	uint32_t *pu32Count);

/** Function to decode Security Key set datatype */
int32_t DT_Decode_SecurityKeySet(
	uint8_t *pu8Apdu, 
	BACnetSecurityKeySet_t *pstSecurityKeySet, 
	uint32_t u32MaxApduLen,
	BACNET_PROPERTY_ID ePropID);

/** Function to decode Security Key set array datatype */
int32_t DT_Decode_SecurityKeySet_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_ListOfBACnetSecurityKeySet_t *pstKeySetArray, 
	BACNET_PROPERTY_ID ePropID);

/** Function to decode Property Access Result datatype */
int32_t DT_Decode_PropertyAccessResult(
	uint8_t *pu8Apdu, 
	ListOfBACnetPropAccessRslt_t *pstAccessResult,
	uint32_t u32ApduLen);

/** Function to decode Array of Property Access Result datatype */
int32_t DT_Decode_PropertyAccessResult_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetPropAccessRslt_t **pstPropAccessResult,
	uint32_t *pu32Count);

/** Function to decode Array of Event Message Text datatype */
int32_t DT_Decode_EveMsgTxt_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetEventMsgText_t *pstEveMsgText);

#endif /* BACDEL_PR12 */


/*******************************************************************************
*  Encoding Function Declarations
********************************************************************************/

/** function to encode time stamp data type */
int32_t DT_Encode_TimeStamp(
	uint8_t *uAPDU,
	timeStamp_u *puTimeStamp, 
	BACNET_TIMESTAMP_TYPE eTimeStampType);

/** function to encode time stamp array data type */
int32_t DT_Encode_TimeStamp_Array(
	uint8_t *uAPDU,
	Pr_BACnetEventTimeStamp_t *puTimeStampArr);

/** function to encode unsigned list data type */
int32_t DT_Encode_Unsigned_List(
	uint8_t *uAPDU, 
	Pr_ListOfUnsigned_t *puUnsigList,
	int32_t u32MaxBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize);

/** function to encode device object property reference data type */
int32_t DT_Encode_DevObjPropReff(
	uint8_t *uAPDU, 
	BACnetDevObjPropRef_t *puDevObjPropReff);

/** function to encode device objecty reference data type */
int32_t DT_Encode_DevObjReff(
	uint8_t *uAPDU, 
	BACnetDevObjRef_t *puDevObjReff);

/** function to encode device object reference list data type */
int32_t DT_Encode_DevObjReff_List(
	uint8_t *uAPDU, 
	Pr_ListOfBACnetDevObjRef_t *puDevObjReffList, 
	int32_t u32MaxBufSize);

/** function to encode device object reference array data type */
int32_t DT_Encode_DevObjReff_Array(
	uint8_t *uAPDU,
	Pr_ListOfBACnetDevObjRef_t *puDevObjReffArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/* Function to Encode single node of special event array */
int32_t DT_Encode_SpecialEvent(
	uint8_t *u8TempAPDU,
	BACnetSpecialEvent_t *pstSpecialEvent);

/* Function to Encode single node of optional character string */
int32_t DT_Encode_OptCharString(
	uint8_t *u8TempAPDU,
	ListOfOptCharStr_t *pstOptCharStr);

/* Function to Encode single node of optional character string */
int32_t DT_Encode_ActionList(
	uint8_t *u8TempAPDU,
	ListOfBACnetActionList_t *pstArrayActCmd);

/** function to encode shed level data type */
int32_t DT_Encode_ShedLevel(
	uint8_t *uAPDU, 
	Pr_BACnetShedLevel_t *puShedLevel);

/** function to encode TL log buffer data type */
int32_t DT_Encode_TrendLogRec_List(
	uint8_t *uAPDU, 
	Pr_ListOfBACnetLogRecord_t *puLogRecord,
	int32_t u32MaxBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize);

/** function to encode EL log buffer data type */
int32_t DT_Encode_EventLogRec_List(
	uint8_t *uAPDU, 
	Pr_ListOfBACnetEventLogRecord_t *puLogRecord,
	int32_t u32MaxInBufSize);

/** function to encode TLM log data list data type */
int32_t DT_Encode_LogData(
	uint8_t *uAPDU, 
	ListOfBACnetLogMultipleRecord_t *puLogData,
	int32_t u32MaxInBufSize);

int32_t DT_Encode_TrendLogMultRec_List(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetLogMultipleRecord_t *puTreLogMultRec,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize
	);

/** function to encode event message text array data type */
int32_t DT_Encode_EveMsgTxt_Array(
	uint8_t *uAPDU,
	Pr_BACnetEventMsgText_t *puEveMsgTxtArr);

/** function to encode event message text data type */
int32_t DT_Encode_EveMsgTxt(
	uint8_t *uAPDU, 
	Pr_BACnetEventMsgText_t *puEveMsgTxt);

/** function to encode authentication factor data type */
int32_t DT_Encode_AuthFactor(
	uint8_t *uAPDU, 
	BACnetAuFactor_t *puAuthFactor);

/** function to encode authentication factor format data type */
int32_t DT_Encode_AuthFactForm(
	uint8_t *uAPDU, 
	ListOfBACnetAuFactorFormat_t *puAuthFactForm);

/** function to encode authentication factor format array data type */
int32_t DT_Encode_AuthFactForm_Array(
	uint8_t *uAPDU,
	Pr_ListOfBACnetAuFactorFormat_t *puAuthFactFormArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/** function to encode authentication policy data type */
int32_t DT_Encode_AuthPolicy(
	uint8_t *uAPDU,
	ListOfBACnetAuPolicy_t *puAuthPolicy,
	int32_t u32MaxInBufSize);

/** function to encode authentication policy array data type */
int32_t DT_Encode_AuthPolicy_Array(
	uint8_t *uAPDU,
	Pr_ListOfBACnetAuPolicy_t *puAuthPolicyArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/** function to encode access rule data type */
int32_t DT_Encode_AccessRule(
	uint8_t *uAPDU, 
	ListOfBACnetAccessRule_t *puAccRule);

/** function to encode access rule array data type */
int32_t DT_Encode_AccessRule_Array(
	uint8_t *uAPDU,
	Pr_ListOfBACnetAccessRule_t *puAccRuleArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/** function to encode credential authentication factor data type */
int32_t DT_Encode_CredAutheFact(
	uint8_t *uAPDU, 
	ListOfBACnetCredAuFactor_t *puCredAutehFact);

/** function to encode credentioal authentication factor array data type */
int32_t DT_Encode_CredAutheFact_Array(uint8_t *uAPDU,
	Pr_ListOfBACnetCredAuFactor_t *puCredAuFactArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/** function to encode assigned access rigths data type */
int32_t DT_Encode_AssiAccRight(
	uint8_t *uAPDU, 
	ListOfBACnetAssignedAccessRights_t *puAssiAccRight);

/** function to encode assigned access rights array data type */
int32_t DT_Encode_AssiAccRight_Array(uint8_t *uAPDU,
	Pr_ListOfBACnetAssignedAccessRights_t *puAssiAccRight,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

#ifdef BACDEL_PR12

/* Function to encode Bacnet key identifier*/
int32_t DT_Encode_KeyIdentifier(
	uint8_t *pu8Apdu, 
	ListOfBACnetKeyId_t *pstKeyIdentify);

/* Function to encode Bacnet key identifier Arr*/
int32_t DT_Encode_KeyIdentifier_List(
	uint8_t *pu8Apdu, 
	ListOfBACnetKeyId_t *pstKeyIdentify,
	int32_t u32MaxInBufSize);

/* Function to encode BACnet security keyset*/
int32_t DT_Encode_SecurityKeySet(
	uint8_t *u8TempAPDU,
	BACnetSecurityKeySet_t *pstSecKeySet,
	int32_t u32MaxInBufSize);

/* Function to encode BACnet security keyset array type*/
int32_t DT_Encode_SecurityKeySet_Array(
	uint8_t *u8TempAPDU,
	Pr_ListOfBACnetSecurityKeySet_t *pstSecKeySet,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/* Function to encode BACnet Network Security Policy */
int32_t DT_Encode_NwSecurityPolicy(
	uint8_t *u8TempAPDU,
	ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcyArr);

/* Function to encode BACnet Network Security Policy Array type*/
int32_t DT_Encode_NwSecurityPolicy_Array(
	uint8_t *u8TempAPDU,
	Pr_ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcy,
	int32_t u32MaxInBufSize, 
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/* Function to encode BACnet Property Access Result data type*/
int32_t DT_Encode_PropAccessResult(
	uint8_t *u8TempAPDU,
	ListOfBACnetPropAccessRslt_t *pstPropAccResArr,
	int32_t u32MaxInBufSize,
	uint32_t u32SegSupport,
	uint16_t u16MaxApduLen);

/* Function to encode BACnet Property Access Result Array type*/
int32_t DT_Encode_PropAccessResult_Array(
	uint8_t *u8TempAPDU,
	Pr_ListOfBACnetPropAccessRslt_t *pstPropAccRes,
	int32_t u32MaxInBufSize,
	uint32_t u32SegSupport,
	uint16_t u16MaxApduLen,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

#endif /* BACDEL_PR12 */

/* Function to encode BACnet address data type */
int32_t DT_Encode_Address(
	uint8_t *pu8Apdu, 
	BACnetAddress_t *pstAddress);

/* Function to encode BACnet recipient data type  */
int32_t DT_Encode_Recipient(
	uint8_t *pu8Apdu, 
	BACnetRecipient_t *pstRecipient);

/* Function to encode property value */
int32_t DT_Encode_PropertyValue(
	property_value_t *pstPropVal, 
	uint8_t *pu8ApduData,
	uint32_t u32MaxInBufSize);

#ifdef BACDEL_PR14
                                                                          
/* Function to Encode property list data type */
int32_t DT_Encode_PropertyList(
	uint8_t *pu8Apdu,
	Pr_BACnetPropertyList_t *pstPropertyList,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

#endif /* BACDEL_PR14 */



/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode List of BACnetActionCommand.
*    
* @param pu8APDU		   [in]	data to be decoded.
* @param u32ApduLen		   [out]received data length.
* @param pstAction_List    [out]pointer to save list of BACnetActionList.
* @param pu32Count		   [out]to save no of elements in array.
* @param ePropID		   [in]parameter to be passed to a function
								which verifies the length between opening and closing tags.                                  
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Action_Command(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetActionCommand_t **pstAction_Command,
	BACNET_PROPERTY_ID ePropID
	);
	
/***
*
* DESCRIPTION
* Function to decode Bacnet Calender Entry.
*
* @param pu8APDU			[in]	data to be decoded.
* @param pstCalenderEntry	[out]	pointer to save single calender Entry.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_BACnetCalenderEntry(
uint8_t *pu8Apdu,
BACnetCalendarEntry_t *pstCalenderEntry);

/***
*
* DESCRIPTION
* Function to decode single node of special event.
*
* @param pu8APDU			[in]	data to be decoded.
* @param pstSpecialEvent	[out]	pointer to save special event.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_BACnetSpecialEvent(
	uint8_t *pu8Apdu,
	BACnetSpecialEvent_t *pstSpecialEvent,
	uint32_t u32ApduLen,
	BACNET_PROPERTY_ID ePropID);

/***
*
* DESCRIPTION
* Function to decode single node of bacnet log record. (for TL object).
*
* @param pu8APDU			[in]	data to be decoded.
* @param pstLogRecord	[out]	pointer to save log record.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_EventLogData(
	uint8_t *pu8Apdu,
	EventlogDatam_t *pstEventLog,
	uint32_t u32APDULen,
	BACNET_PROPERTY_ID ePropID
	);

/***
*
* DESCRIPTION
* Function to decode single node of bacnet log data (for TLM object).
*
* @param pu8APDU		[in]	data to be decoded.
* @param BACnetLogData_t		[out]	pointer to save log data.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_LogData(
	uint8_t *pu8Apdu,
	BACnetLogData_t *pstLogData,
	uint32_t u32APDULen,
	BACNET_PROPERTY_ID ePropID
	);

/***
*
* DESCRIPTION
* Function to decode single node of bacnet log record. (for TL object).
*
* @param pu8APDU			[in]	data to be decoded.
* @param pstLogRecord	[out]	pointer to save log record.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_LogRecord(
	uint8_t *pu8Apdu,
	BACnetLogRecord_t *pstLogRecord
	);

/***
*
* DESCRIPTION
* Function to decode policy list.
*
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstPolicy    [out]	pointer to save list of enum values.
* @param pu32Count	  [out]	to save no of elements in array.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Policy_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	Policy_t *pstPolicy);


/***
*
* DESCRIPTION
* Function to decode priority array value as per tag type.
*
* @param pu8APDU		 [in]	data to be decoded.
* @param pbTagType		 [out]	to save the data type.
* @param pstPropValue	 [out]	pointer to save decoded data.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_PriorityValue(
	uint8_t *pu8Apdu,
	bool_t *pbTagType,
	PropertyValue_u *pstPropValue);

/***
*
* DESCRIPTION
* Function to decode List of property references.
*
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstlistOfPropReff   [out]	pointer to save list of property references.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_PropertyRef_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	ListOfBACnetPropRef_t *pstlistOfPropReff,
	uint32_t u32ApduDataLen);
	
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode listOfResults field .
*    
* @param pu8APDU		[in]  data to be decoded.
* @param u32ApduLen		[on]  length to be decoded.
* @param pstResults     [out] pointer to save listOfResults data.
* @param u16ObjectType	[in]  object type decoded in objectIdentifier.
* @param u32ObjId    	[in]  object id decoded in objectIdentifier.
* @param ePropID		[in]  parameter to be passed to a function
							  which verifies the length between opening and closing tags.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Results_list(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	listOfResults_t *pstResults,
	uint16_t u16ObjectType,
	uint32_t u32ObjId,
	BACNET_PROPERTY_ID ePropID);
	

/***
*
* DESCRIPTION
* Function to decode BACnet Time-Value data type.
*
* @param pu8APDU [in]	data to be decoded.
* @param pstTimeValue [out]	pointer to save time-value .
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_TimeValue(
	uint8_t *pu8Apdu,
	BACnetTimeValue_t *pstTimeValue);

/***
*
* DESCRIPTION
* Function to decode BACnet Time-Value list data type.
*
* @param pu8APDU [in]	data to be decoded.
* @param pstTimeValue [out]	pointer to save time-value .
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_TimeValue_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	BACnetTimeValue_t *pstTimeValueList);



int32_t DT_Encode_HostNPort(
	uint8_t *pu8Apdu,
	BACnetHostNPort_t *pstHostNPort);

int32_t DT_Encode_BdtEntry(
	uint8_t *pu8Apdu,
	BACnetBDTEntry_t *pstBDTEntry);

int32_t DT_Encode_BdtEntry_List(
	uint8_t *pu8Apdu,
	ListOfBACnetBDTEntry_t *pstBDTEntryList,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize);

int32_t DT_Encode_FdtEntry(
	uint8_t *pu8Apdu,
	BACnetFDTEntry_t *pstFDTEntry);

int32_t DT_Encode_FdtEntry_List(
	uint8_t *pu8Apdu,
	ListOfBACnetFDTEntry_t *pstFDTEntryList,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize);

int32_t DT_Decode_OptionalUnsigned(
	uint8_t *pu8Apdu,
	BACnetOptionalUnsigned_t *pstOptionalUnsigned);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DT_ENCODE_DECODE_H */
