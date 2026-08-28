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
*   File Name - propertyGenricHandler.h
*
*   RELEASE HISTORY                                                       
*      
*   DATE            NAME                 DESCRIPTION
*
*   12/07/2011      Prashant Badgujar    File Created & Added generic 
*                                        BACDEL_Add_Property API.
*   14-07-2011      M.Venu               Added "GetObjectInstancePtr"
*                                        "UpdateCommonProperty"
*   15-07/2011      Prashant Badgujar    Added BACNET_REMAPPED_OBJECT_TYPE
*   15-07-2011      M.Venu               Added GetDefndPropAccess function protorype
*   19-07-2011      M.Venu               Added Macro definations for returning 
*                                        offset Address of common properties
*   22-07-2011      M.Venu               Added GET_DEVICETYPE_OFFSET,
*                                        GET_AFVLUES_OFFSET macro definations
*                                        and Removed GetObjectInstancePtr
*   26-07-2011      M.Venu               Added GET_MULTISTATE_PROP_OFFSET Defination
*   2707-2011       M.Venu               Changed access type arg type to enum 
*                                        for UpdateCommonProperty function
*	01-08-2011		Prashant Badgujar	 Adding AI,AO & AV
*	04-08-2011		Prashant Badgujar	 Adding GET_ANALOGPROP_OFFSET,
*										 GET_MIN_PV_RESOLUTION_OFFSET,GET_COV_OFFSET
*										 macros.
*   04-08-2011      M.Venu               Added GET_MULTISTATE_PROP_OFFSET macro &
*                                        & Removed Relinquish default for Binary Input
*   24-08-2011      Prashant             Adding GET_DEVPROP_OFFSET Macro.
*   29-08-2011      Prashant             Adding GET_OBJECTID_OFFSET Macro.
*	31-08-2011		Prashant			 Adding GET_UPDATEINTERVAL_OFFSET Macro.
*   16-09-2011      M.Venu               Added GetPresentValOffset function
*	16-09-2011		Prashant			 Changing BACDEL_Add_Property & UpdateCommonProperty prototype
*										 
*   16-09-2011      M.Venu               Replaced AddCommonProperty with 
*                                        UpdateCommonProperty
*	20-09-2011		Prashant Badgujar	 Adding GET_OBJECT_TYPE_PROPERTY_OFFSET Macro.
*   20-09-2011      M.Venu               Made unique macro for Get Relinquish 
*                                        default property offset macro with all 
*                                        supported objects
*   21-09-2011      M.Venu               Added GetPolarity, IsOutofSerivceFlagTrue
*                                        function prototype
*	22-09-2011	    Prashant Badgujar	 Removing GET_ANALOG_PRESENTVAL_OFFSET macro 
*   22/09/2011      M.Venu               Added Convert_SystemTimeToBACnet ,
*                                        Convert_BACnetTimeToSystem functions
*   03-04-2013      Pratham N. M.        Sorting of offset macros as per use,
*                                        commenting to function declarations.
*   03-03-2020      Pranav Phadatare     Added GET_FAULT_HL_LL_VALUE_OFFSET function
*
*************************************************************************/

#ifndef OBJ_PROPERTY
#define OBJ_PROPERTY

/** include header files */
#include "bacDELDef.h"
#include "bacDELApi.h"
#include "propertydef.h"
#include "pduServiceStructure.h"

/** global variable used for ... */
extern bool g_bWriteRequest ;


/****************************************************************************
 **** Macros Common For All Objects.
 ****************************************************************************/
/** Defination to get offset of any property common to All objects */            
#define GET_ALLOBJ_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_DEVICE: \
offset = offsetof(devObject_t, member); \
break; \
case OBJECT_ANALOG_INPUT:\
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT:\
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE:\
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
case OBJECT_CALENDAR:\
offset = offsetof(Calendar_t, member); \
break; \
case OBJECT_NOTIFICATION_CLASS: \
offset = offsetof(Notificationclass_t, member); \
break; \
case OBJECT_NETWORK_PORT: \
offset = offsetof(NetworkPort_t, member); \
break; \
default: \
offset = -1; \
break;}\
}while(0);

/** Defination to get size ( in bytes ) of structure, of any object type */
#define GET_OBJECT_SIZE(ObjType,offset) \
do{switch(ObjType) \
{ \
case OBJECT_DEVICE: \
offset = sizeof(devObject_t); \
break; \
case OBJECT_ANALOG_INPUT: \
offset = sizeof(analoginput_t); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = sizeof(analogoutput_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = sizeof(analogvalue_t); \
break; \
case OBJECT_BINARY_INPUT: \
offset = sizeof(BinaryInput_t); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = sizeof(BinaryOutput_t); \
break; \
case OBJECT_BINARY_VALUE: \
offset = sizeof(BinaryValue_t); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = sizeof(MultistateInput_t); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = sizeof(MultistateOutput_t); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = sizeof(MultistateValue_t); \
break; \
case OBJECT_SCHEDULE: \
offset = sizeof(Schedule_t); \
break; \
case OBJECT_CALENDAR:\
offset = sizeof(Calendar_t); \
break; \
case OBJECT_NOTIFICATION_CLASS: \
offset = sizeof(Notificationclass_t); \
break; \
case OBJECT_NETWORK_PORT: \
offset = sizeof(NetworkPort_t); \
break; \
default: \
offset = 0; \
break;}\
}while(0);


/** Defination to get base address of All objects */
#define GET_OBJBASE_OFFSET(ObjType, offset) \
do{switch(ObjType) \
{ \
case OBJECT_DEVICE: \
offset = offsetof(supportedObj_t, m_pstDevObject); \
break; \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(supportedObj_t, m_pstAnalogInput); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(supportedObj_t, m_pstAnalogOutput); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(supportedObj_t, m_pstAnalogValue); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(supportedObj_t, m_pstBinaryInput); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(supportedObj_t, m_pstBinaryOutput); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(supportedObj_t, m_pstBinaryValue); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(supportedObj_t, m_pstMSInput); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(supportedObj_t, m_pstMSOutput); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(supportedObj_t, m_pstMSValue); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(supportedObj_t, m_pstSchedule); \
break; \
case OBJECT_CALENDAR:\
offset = offsetof(supportedObj_t, m_pstCalendar); \
break; \
case OBJECT_NOTIFICATION_CLASS: \
offset = offsetof(supportedObj_t, m_pstNotificationClass); \
break; \
case OBJECT_EVENT_ENROLLMENT: \
offset = offsetof(supportedObj_t, m_pstEventEnrollment); \
break; \
case OBJECT_NETWORK_PORT: \
offset = offsetof(supportedObj_t, m_pstNetworkPort); \
break; \
default: \
offset = -1; \
break;}\
}while(0);

/** Defination to get offset of any property common to All objects except Device */
/** properties are - obj name, obj id, obj type, profile name, description,
	                 bAppCallBack , pstNext, m_i32ObjCOVSupport */
#define GET_OBJCOMMON_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
case OBJECT_CALENDAR:\
offset = offsetof(Calendar_t, member); \
break; \
case OBJECT_NOTIFICATION_CLASS: \
offset = offsetof(Notificationclass_t, member); \
break; \
case  OBJECT_NETWORK_PORT: \
offset = offsetof(NetworkPort_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


/****************************************************************************
 **** Object Specific Macros.
 ****************************************************************************/

/** Defination to get offset of Device object properties */
#define GET_DEVPROP_OFFSET(member, offset) \
{ \
offset = offsetof(devObject_t, member); \
}

#ifdef BACDEL_OBJ_NP
/** Defination to get offset of network port object properties */
#define GET_NETWORK_PORT_OFFSET(member, offset) \
{ \
offset = offsetof(NetworkPort_t, member); \
}
#endif

#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)            
/** Defination to get offset of Binary object properties  */
#define GET_BINARYONLY_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* BI - BO - BV */

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
/** Defination to get offset of MultiState object properties */
#define GET_MULTISTATE_PROP_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* MSI - MSO - MSV */

#ifdef BACDEL_OBJ_NC
/** Defination to get offset of Notification class object properties */
#define GET_NC_OBJ_PROP_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_NOTIFICATION_CLASS: \
offset = offsetof(Notificationclass_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif

#ifdef BACDEL_PR14
/** Defination to get offset of event algorithm inhibit backup property */
#define GET_EVENTALGOINHIBIT_BACKUP_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_EVENT_ENROLLMENT: \
offset = offsetof(EventEnrollment_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif
#if (defined OPTIONAL_PROPERTY && defined BACDEL_PR14)
/** Definition to get offset of Time Delay Normal property */
#define GET_TIME_DELAY_NORMAL_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY && PR14 */
#ifdef BACDEL_OBJ_CAL
/** Defination to get offset of Calendar object properties	*/
#define GET_OBJCALPROP_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_CALENDAR:\
offset = offsetof(Calendar_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif

#ifdef BACDEL_OBJ_SDL
/** Defination to get offset of Schedule object properties	*/
#define GET_SHDL_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif
/****************************************************************************
 **** Property Specific Macros.
 ****************************************************************************/

/** Defination to get offset of present value property common to below objects */
#define GET_PRESENTVAL_OFFSET(ObjType, member, offset, i32Len) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member.m_eVal); \
i32Len = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member.m_eVal); \
i32Len = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member.m_eVal); \
i32Len = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member.m_u32Val); \
i32Len = sizeof(uint32_t); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member.m_u32Val); \
i32Len = sizeof(uint32_t); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member.m_u32Val); \
i32Len = sizeof(uint32_t); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member.m_stValue); \
i32Len = sizeof(BACNET_PROPERTY_VALUE); \
break; \
case OBJECT_CALENDAR: \
offset = offsetof(Calendar_t, member.m_bVal); \
i32Len = sizeof(bool); \
break;\
default: \
offset = -1; \
i32Len = 0; \
break; }\
}while(0);

/** Defination to get offset of access type & active cov value of PV property */
#define GET_PV_ELEMENT_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
case OBJECT_CALENDAR: \
offset = offsetof(Calendar_t, member); \
break;\
default: \
offset = -1; \
break; }\
}while(0);

#ifdef OPTIONAL_PROPERTY
/** Defination to get offset of min-max present value */
#define GET_MIN_MAX_PV_OFFSET(ObjType, offsetMin, offsetMax)\
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offsetMin = offsetof(analoginput_t, m_stMinPresValue.m_fVal); \
offsetMax = offsetof(analoginput_t, m_stMaxPresValue.m_fVal); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offsetMin = offsetof(analogoutput_t, m_stMinPresValue.m_fVal); \
offsetMax = offsetof(analogoutput_t, m_stMaxPresValue.m_fVal); \
break; \
case OBJECT_ANALOG_VALUE: \
offsetMin = offsetof(analogvalue_t, m_stMinPresValue.m_fVal); \
offsetMax = offsetof(analogvalue_t, m_stMaxPresValue.m_fVal); \
break; \
default: \
offsetMin = -1; \
offsetMax = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */

/** Defination to get offset of Relinquish default property value */
#define GET_RELINQUISH_VAL_OFFSET(ObjType, member, offsetval, i32size) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_OUTPUT : \
offsetval = offsetof(analogoutput_t, member.m_fVal); \
i32size = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offsetval = offsetof(analogvalue_t, member.m_fVal); \
i32size = sizeof(Float_t); \
break; \
case OBJECT_BINARY_OUTPUT: \
offsetval = offsetof(BinaryOutput_t, member.m_eVal); \
i32size = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_BINARY_VALUE: \
offsetval = offsetof(BinaryValue_t, member.m_eVal); \
i32size = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offsetval = offsetof(MultistateOutput_t, member.m_u32Val); \
i32size = sizeof(uint32_t); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offsetval = offsetof(MultistateValue_t, member.m_u32Val); \
i32size = sizeof(uint32_t); \
break; \
default: \
offsetval = -1; \
i32size = 0; \
break; }\
}while(0);

/** Defination to get offset of access type & active cov of Relinquish default property */
#define GET_RELINQUISH_OFFSET(ObjType, member, offsetval) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_OUTPUT: \
offsetval = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offsetval = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offsetval = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offsetval = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offsetval = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offsetval = offsetof(MultistateValue_t, member); \
break; \
default: \
offsetval = -1; \
break; }\
}while(0);

/** Defination to get offset of Priority array property */
#define GET_PRIORITY_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/** Defination to get offset of Status Flag property */
#define GET_STATUSFLAG_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


#ifdef OPTIONAL_PROPERTY
/** Defination to get offset of Out of Service property */
#define GET_OOS_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */


#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO)                    
/** Defination to get offset of polarity property */
#define GET_POLARITY_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif

/** Defination to get offset of Notification Class Property	*/
#define GET_NCPROP_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{\
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_NOTIFICATION_CLASS: \
offset = offsetof(Notificationclass_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/** Defination to get offset of common properties for intrinsic reporting */
/** properties : event state, event enable, acked transition, 
                 notify type, event time stamp, event message text, 
                 m_i32GetSummary */
#define GET_INTRINSIC_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_NOTIFICATION_CLASS: \
offset = offsetof(Notificationclass_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

#ifdef OPTIONAL_PROPERTY
/** Defination to get offset of event state property */
#define GET_EVENTSTATE_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */

#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B)
/** Defination to get offset of Reliability property */
#define GET_RELIABILITY_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */

/** Definition to get offset of Time Delay property */
#define GET_TIME_DELAY_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

#ifdef OPTIONAL_PROPERTY
/** Defination to get offset of Min PV & Resolution property */
#define GET_MIN_PV_RESOLUTION_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */

#ifdef OPTIONAL_PROPERTY
/** Defination to get offset of Max PV property value */
#define GET_MAX_PV_VALUE_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member.m_fVal); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member.m_fVal); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member.m_fVal); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */

#ifdef OPTIONAL_PROPERTY
/** Defination to get offset of Max PV property access type, active cov, 
    property struct base address */
#define GET_MAX_PV_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* OPTIONAL_PROPERTY */


/** Defination to get offset of Update Interval property */
#define GET_UPDATEINTERVAL_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/** Defination to get offset of Device Type property */
#define GET_DEVICETYPE_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break;\
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

#if (defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
/** Defination to get offset of Minimum On & Off Time property */
#define GET_MIN_ON_OFF_TIME_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


#endif /* BO - BV */

#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BV)
/** Defination to get offset of Alarm value property */
#define GET_ALARMVALUE_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* BI - BV */

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSV)
/** Defination to get offset of Fault values property */
#define GET_FAULT_VALUES_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/** Defination to get offset of Alarm values property */
#define GET_ALARM_VALUES_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* MSI - MSV - CSV */

#if (defined BACDEL_OBJ_BO || defined BACDEL_OBJ_MSO)
/** Defination to get offset of Feedback value property  */
#define GET_FEEDBACKVAL_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member.m_eVal); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member.m_u32Val); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


/** Defination to get offset of mAXmASTER value property  */  //SK
#define GET_MAX_MASTER_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_NETWORK_PORT: \
offset = offsetof(NetworkPort_t, member.m_u32Val); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


/** Defination to get offset of access type, active cov, base address of 
	Feedback value property */
#define GET_FEEDBACKVAL_OFFSET_VALUE(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* BO - MSO */

/* Defination to get offset for Deadband and cov increment prop access type & active cov */
#define GET_DEADBAND_COV_INCR_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/* Defination to get offset of Deadband and cov increment prop value */
#define GET_DEADBAND_COV_INCR_VAL_OFFSET(ObjType, member, offset , i32Len) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
default: \
offset = -1; \
i32Len = 0; \
break; }\
}while(0);

/** Defination to get offset of Units Property */
#define GET_UNITS_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/** Defination to get offset of Limit Enable Property */
#define GET_LIMITENABLE_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);

/** Defination to get offset of Fault High Limit & Fault Low Limit property values */
#define GET_FAULT_HL_LL_VALUE_OFFSET(ObjType, member, offset, i32Len) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
default: \
offset = -1; \
i32Len = 0; \
break; }\
}while(0);

/** Defination to get offset of High Limit & Low Limit property values */
#define GET_HL_LL_VALUE_OFFSET(ObjType, member, offset, i32Len) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
default: \
offset = -1; \
i32Len = 0; \
break; }\
}while(0);

/** Defination to get offset of High Limit & Low Limit property access type,
    active cov & struct base address */
#define GET_HL_LL_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


/** Defination to get offset of fault High Limit & Low Limit property values */ //PR-19
#define GET_FHL_FLL_VALUE_OFFSET(ObjType, member, offset, i32Len) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member.m_fVal); \
i32Len = sizeof(Float_t); \
break; \
default: \
offset = -1; \
i32Len = 0; \
break; }\
}while(0);
/** Defination to get offset of FAULT High Limit & Low Limit property access type,
    active cov & struct base address */
#define GET_FHL_FLL_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#if (defined BACDEL_OBJ_SDL)
/* offset only for Priority for writing prop */
#define GET_PRIORITYFORWRT_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_SCHEDULE: \
offset = offsetof(Schedule_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);
#endif /* SDL - LOOP */

/****************************************************************************
 **** Service Specific Macros.
 ****************************************************************************/

/** Defination to get size ( in bytes ) of structure, of any service type */
#define GET_SERVICE_SIZE(ServType, offset) \
do{switch(ServType) \
{ \
case SERVICE_SUPPORTED_READ_PROPERTY: \
offset = sizeof(rp_request_t); \
break; \
case SERVICE_SUPPORTED_WRITE_PROPERTY: \
offset = sizeof(wp_request_t); \
break; \
case SERVICE_SUPPORTED_DEVICE_COMMUNICATION_CONTROL: \
offset = sizeof(dcc_request_t); \
break; \
case SERVICE_SUPPORTED_TIME_SYNCHRONIZATION: \
offset = sizeof(time_sync_request_t); \
break; \
case SERVICE_SUPPORTED_I_AM: \
offset = sizeof(ddb_i_am_t); \
break; \
case SERVICE_SUPPORTED_I_HAVE: \
offset = sizeof(dob_i_have_t); \
break; \
case SERVICE_SUPPORTED_WHO_HAS: \
offset = sizeof(dob_who_has_t); \
break; \
case SERVICE_SUPPORTED_WHO_IS: \
offset = sizeof(ddb_who_is_t); \
break; \
default: \
offset = 0; \
break;} \
}while(0);

/** offset for Reference property for Intrinsic alarms */
#define GET_REF_PROP_VAL_OFFSET(ObjType, offset, i32size) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, m_stPresentValue.m_fVal); \
i32size = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, m_stPresentValue.m_fVal); \
i32size = sizeof(Float_t); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, m_stPresentValue.m_fVal); \
i32size = sizeof(Float_t); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, m_stPresentValue.m_eVal); \
i32size = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, m_stPresentValue.m_eVal); \
i32size = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, m_stPresentValue.m_eVal); \
i32size = sizeof(BACNET_BINARY_PV); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, m_stPresentValue.m_u32Val); \
i32size = sizeof(uint32_t); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, m_stPresentValue.m_u32Val); \
i32size = sizeof(uint32_t); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, m_stPresentValue.m_u32Val); \
i32size = sizeof(uint32_t); \
break; \
default: \
offset = -1; \
/* default value is 0, donot change */ \
i32size = 0; \
break; }\
}while(0);

/** offset for GAS, GIE & GES services */
#define GET_GETSUMMARY_OFFSET(ObjType, member, offset) \
do{switch(ObjType) \
{ \
case OBJECT_ANALOG_INPUT: \
offset = offsetof(analoginput_t, member); \
break; \
case OBJECT_ANALOG_OUTPUT: \
offset = offsetof(analogoutput_t, member); \
break; \
case OBJECT_ANALOG_VALUE: \
offset = offsetof(analogvalue_t, member); \
break; \
case OBJECT_BINARY_INPUT: \
offset = offsetof(BinaryInput_t, member); \
break; \
case OBJECT_BINARY_OUTPUT: \
offset = offsetof(BinaryOutput_t, member); \
break; \
case OBJECT_BINARY_VALUE: \
offset = offsetof(BinaryValue_t, member); \
break; \
case OBJECT_MULTI_STATE_INPUT: \
offset = offsetof(MultistateInput_t, member); \
break; \
case OBJECT_MULTI_STATE_OUTPUT: \
offset = offsetof(MultistateOutput_t, member); \
break; \
case OBJECT_MULTI_STATE_VALUE: \
offset = offsetof(MultistateValue_t, member); \
break; \
case  OBJECT_EVENT_ENROLLMENT: \
offset = offsetof(EventEnrollment_t, member); \
break; \
default: \
offset = -1; \
break; }\
}while(0);


/****************************************************************************
 **** Enum for Remapped Object type & Macro's.
 ****************************************************************************/

/** used to get property access type & tag type values from array 
    as per object type. */
typedef enum {
    REMAPPED_OBJECT_DEVICE = 0,
    REMAPPED_OBJECT_ANALOG_INPUT = 1,
    REMAPPED_OBJECT_ANALOG_OUTPUT = 2,
    REMAPPED_OBJECT_ANALOG_VALUE = 3,
    REMAPPED_OBJECT_BINARY_INPUT = 4,
    REMAPPED_OBJECT_BINARY_OUTPUT = 5,
    REMAPPED_OBJECT_BINARY_VALUE = 6,
    REMAPPED_OBJECT_MULTI_STATE_INPUT = 7,
    REMAPPED_OBJECT_MULTI_STATE_OUTPUT = 8,
    REMAPPED_OBJECT_MULTI_STATE_VALUE = 9,
	REMAPPED_OBJECT_NOTIFICATION_CLASS = 10,
	REMAPPED_OBJECT_EVENT_ENROLLMENT = 11,
    REMAPPED_OBJECT_CALENDAR = 12,
    REMAPPED_OBJECT_SCHEDULE = 13,
	REMAPPED_OBJECT_FILE = 14,
    REMAPPED_OBJECT_TRENDLOG = 15,
    REMAPPED_OBJECT_LOOP = 16,
    REMAPPED_OBJECT_ACCUMULATOR = 17,
    REMAPPED_OBJECT_BITSTRING_VALUE =18,
	REMAPPED_OBJECT_OCTETSTRING_VALUE = 19,
    REMAPPED_OBJECT_INTEGER_VALUE = 20,
    REMAPPED_OBJECT_POSITIVE_INTEGER_VALUE = 21,
    REMAPPED_OBJECT_LARGE_ANALOG_VALUE = 22,
    REMAPPED_OBJECT_DATE_VALUE = 23,
    REMAPPED_OBJECT_DATE_PATTERN_VALUE = 24,
    REMAPPED_OBJECT_TIME_VALUE = 25,
    REMAPPED_OBJECT_TIME_PATTERN_VALUE = 26,
    REMAPPED_OBJECT_DATETIME_VALUE = 27,
    REMAPPED_OBJECT_DATETIME_PATTERN_VALUE = 28,
    REMAPPED_OBJECT_CHARACTERSTRING_VALUE = 29,
    REMAPPED_OBJECT_AVERAGING = 30,
    REMAPPED_OBJECT_COMMAND = 31,
    REMAPPED_OBJECT_GROUP = 32,
    REMAPPED_OBJECT_GLOBAL_GROUP = 33,
    REMAPPED_OBJECT_PULSE_CONVERTER = 34,
    REMAPPED_OBJECT_TREND_LOG_MULTIPLE = 35,
    REMAPPED_OBJECT_EVENT_LOG = 36,
    REMAPPED_OBJECT_PROGRAM = 37,
    REMAPPED_OBJECT_STRUCTURED_VIEW = 38,
    REMAPPED_OBJECT_LOAD_CONTROL = 39,
    REMAPPED_OBJECT_LIFE_SAFETY_POINT = 40,
    REMAPPED_OBJECT_LIFE_SAFETY_ZONE = 41,
    REMAPPED_OBJECT_CREDENTIAL_DATA_INPUT = 42,
    REMAPPED_OBJECT_ACCESS_DOOR = 43,
    REMAPPED_OBJECT_ACCESS_POINT = 44,
    REMAPPED_OBJECT_ACCESS_ZONE = 45,
    REMAPPED_OBJECT_ACCESS_USER = 46,
    REMAPPED_OBJECT_ACCESS_RIGHTS = 47,
    REMAPPED_OBJECT_ACCESS_CREDENTIAL = 48, 
    REMAPPED_OBJECT_NETWORK_SECURITY = 49,

	#ifdef BACDEL_PR18
	REMAPPED_OBJECT_BINARY_LIGHTING_OUTPUT = 54,
	REMAPPED_OBJECT_TIMER = 55,
	REMAPPED_OBJECT_ELEVATOR_GROUP = 56,
	REMAPPED_OBJECT_LIFT = 57,
	REMAPPED_OBJECT_ESCALATOR = 58,
	REMAPPED_OBJECT_NETWORK_PORT = 59,
	#endif
    /** Add values here ... */
    REMAPPED_MAX_BACNET_OBJECT_TYPE = 1024
        /* Enumerated values 0-127 are reserved for definition by ASHRAE. */
        /* Enumerated values 128-1023 may be used by others subject to  */
        /* the procedures and constraints described in Clause 23. */
} BACNET_REMAPPED_OBJECT_TYPE;

/** macro definations */
#define EVENT_STATE_IS_NORMAL       0x7F
#define EVENT_STATE_NOT_NORMAL      0x80
#define ACKTRANS_ALL_BIT_TRUE       0x07
#define SETBIT_7TH_ZERO             0xBF
#define SETBIT_7TH_ONE              0x40


/****************************************************************************
 **** Function Declarations.
 ****************************************************************************/

/** function to register supported properties for RPM-B service */
void Register_Object_Property( BACNET_OBJECT_TYPE eObjType, 
            BACNET_PROPERTY_ID eDevProp);

/** register all property support during stack init */
void Register_All_Supported_Properties(void);

/** function to check duplicate object name */
BACNET_RETURN_TYPE Check_Object_Name(uint32_t u32DeviceId, int8_t *pi8InObjectName,
                                     uint16_t *peObjectType, uint32_t *pu32ObjId );

/** Adds All Common properties for respective object */
BACNET_RETURN_TYPE UpdateCommonProperty(BACNET_OBJECT_TYPE eObjectType, 
                                void *pvObject, 
                                BACNET_PROPERTY_ID eDevProp,
                                int32_t i32ArrayIndex,
                                void *pvInData, 
                                PROP_ACCESS_TYPE ePermission,
                                uint32_t *pvPropertyPtr, 
                                void **pvPropAdd,
                                void *pvVirtualDev, 
                                bool bInitFunctionality,
                                bool bRemoteDevFlag,
                                bool bWriteNULL);

void * UpdateWriteDataOnPriority(ulong32_t ul32BaseAddr, 
                                 void *pvData,
                                 int32_t i32ArrayIndex, 
                                 BACNET_OBJECT_TYPE  eObjectType,
                                 bool bWriteNULL,
								 uint32_t *pu32CmdPriority);
/** function to convert input string value & write to property */
BACNET_RETURN_TYPE Convert_InStr_AddProperty(void *pvVirtualDev,
                                void *pvObject,
                                uint32_t u32ObjectID,
                                BACNET_OBJECT_TYPE eObjType,
                                BACNET_PROPERTY_ID eDevProp,
                                int32_t  i32ArrayIndex,
                                void *pvInData,
                                PROP_ACCESS_TYPE ePermission,
                                void **pvPropAdd,
                                bool bRemoteDev);

/* function to convert property data type to property application tag type */
BACNET_APPLICATION_TAG Convert_DataType_To_AppTagType(BACNET_DATA_TYPE eDataType);

/* function to convert property application tag type to property data type */
BACNET_DATA_TYPE Convert_AppTagType_To_DataType(BACNET_APPLICATION_TAG eTagType);

/** copy property value as per data type, used for PV, relinquish default, etc. */
BACNET_RETURN_TYPE UpdateObjectValue(BACNET_OBJECT_TYPE eObjectType, 
                                ulong32_t ul32Address, void *pvData);
                        
/** function to get the access type of any property of any object
    type as defined by stack */
PROP_ACCESS_TYPE GetDefndPropAccess(BACNET_OBJECT_TYPE eObjType,
            BACNET_PROPERTY_ID eDevProp, PROP_ACCESS_TYPE ePermission);

PROP_ACCESS_TYPE GetAsignPropAccess(BACNET_OBJECT_TYPE eObjType,
                                BACNET_PROPERTY_ID eDevProp,
                                void *pvObjectPrt,
                                PROP_ACCESS_TYPE eAccessType,
                                bool bReadOrWrite,
                                ulong32_t *pul32PropOffstAddr);

																

/** function to get the access type of any property of any object
    type as defined or set by user or application */
PROP_ACCESS_TYPE GetAsignPropAccess_offset(BACNET_OBJECT_TYPE eObjType,
                                BACNET_PROPERTY_ID eDevProp,
                                void *pvObjectPrt,
                                PROP_ACCESS_TYPE eAccessType,
                                bool bReadOrWrite,
                                ulong32_t *pul32PropOffstAddr);																

/** converts BACNET_OBJECT_TYPE to BACNET_REMAPPED_OBJECT_TYPE */
BACNET_REMAPPED_OBJECT_TYPE Remap_Objct_Id(BACNET_OBJECT_TYPE eObjType);

/** converts BACNET_REMAPPED_OBJECT_TYPE to BACNET_OBJECT_TYPE */
BACNET_OBJECT_TYPE Map_Objct_Type(BACNET_REMAPPED_OBJECT_TYPE eObjType);

/** gives offset address of PV or backup PV depending on value of out of service */
ulong32_t GetPresentValOffset(BACNET_OBJECT_TYPE eObjectType, 
                            uint8_t u8OutofService);

#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
/** give values of polarity */
BACNET_POLARITY GetPolarity(ulong32_t ul32BaseAddr, 
                            BACNET_OBJECT_TYPE eObjectType);

/** updates polarity value as needed */
BACNET_POLARITY Update_PV_Polarity(ulong32_t ul32BaseAddr, 
                            BACNET_OBJECT_TYPE eObjectType);
#endif

/** function returns true if out of service is true */
bool IsOutofSerivceFlagTrue(ulong32_t ul32BaseAddr, BACNET_OBJECT_TYPE eObjectType);

/** function returns true if object type belongs to physical IO entities */
bool IsPhysicalIOVObject(BACNET_OBJECT_TYPE eObjectType);

/** allocate & store property value as per its data type */
BACNET_RETURN_TYPE Store_Value_On_DataType(
    BACNET_DATA_TYPE eData_Type, void *pvSrcAdd, void **pvDstAdd);

/**
*
* DESCRIPTION                                                                          
* Function to update array of object id data type property.
*    
* @param pWpData          [in] write property data.
* @param pVirtualData     [in] void pointer for virtual data.
* @param pvData      	  [in] data to be written to property value.
* @param bInitFunctionality [in] indicates : call for init property or call from WP/WPM.
*
* @returns [out] void.
*
*/
void UpdateObjIDArrayProperty(
	BACNET_CONF_DATA *pWpData,
    void *pVirtualData,
	void *pvData, 
    bool bInitFunctionality
	);

/**
*
* DESCRIPTION
* Function to revers bitstring value as per data type.
*
* @param pvPropVal	[in] pointer to property value to be copied.
* @param eDataType	[in] data type of property.
*
* @return void		[out] no return value.
*
*/
void Reverse_BitString_DataType_Value(BACNET_DATA_TYPE eDataType, void *pvPropVal);

#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
/**
*
* DESCRIPTION
* Function get time delay value based on event state.
*   
* @param eObjectType	[in] Object Type 
* @param pvObject		[in] Object base address
* @param eEventStateTo	[in] event state.
* @param u32TimeDelay	[in] Time delay value pointer.
*
* @return [out] nothing.
*
*/
void Get_TimeDelay_value(
	BACNET_OBJECT_TYPE eObjectType, 
	void *pvObject, 
	BACNET_EVENT_STATE eEventStateTo,
	uint32_t *pu32TimeDelay);

#endif /* INTRINSIC_REPORTING &&  EN_B */
#ifdef BACDEL_PR14

/**
*
* DESCRIPTION
* Function to get skip property count of property List property.
*   
* @param pstPropertyList  [in] property list property pointer 
* @return [out] count of skip properties.
*
*/
uint8_t Get_PropertyList_SkipCount(
	special_property_list_t *pstPropertyList);

#endif /* PR14 */

/** Check if this is proprietary property */
bool IsProprietaryProp(BACNET_OBJECT_TYPE eObjType, BACNET_PROPERTY_ID eDevProp,
						PROP_ACCESS_TYPE *pePropType);


#endif /* OBJ_PROPERTY */
