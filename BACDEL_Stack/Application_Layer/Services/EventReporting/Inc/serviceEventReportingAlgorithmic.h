/******************************************************************************
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
*   SoftDEL Systems Ltd.                     		    india@softdel.com         
*   3rd Floor, Pentagon P4,								http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
*                                                                         
*   FILE
*	 serviceAlgorithmicReporting.h
*                                                                      
*   AUTHORS
*		Pratham N. Murkute
*                                                                         
*   DESCRIPTION
*		Algorithmic Event Notification Reporting.
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE			NAME					DESCRIPTION
*   30/oct/2012		Pratham N. Murkute		Created file & added api's.
*
******************************************************************************/

#ifndef ALGORITHMIC_H
#define ALGORITHMIC_H
#if (defined ALGORITHMIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_EE && defined BACDEL_OBJ_NC)

/* include files */
#include "objDevice.h"

/* macros */
#define MAX_ALGORITHMIC_RETRY_COUNT 2

/** struct to hold data for linklist of algorithmic linklist */
typedef struct AlgorithmicNodeData
{
	/* event enrollment object instance id */
	uint32_t			m_u32EEObjId;
	/* event enrollment object base address */
	void *				m_pvEEObjAddr;

	/* virtual device pointer */
	void *				m_pVirtualDev;
	/* reference object base address in case this is object 
		from same device */
	void *				m_pvRefObjAddr;
	/* feedback object base address in case event type is cmd failure & 
		object is from same device */
	void *				m_pvFbObjAddr;

	/* reference property id */
	BACNET_PROPERTY_ID	m_ePropId;
	/* Array Index & its flag */
    bool                m_bArrIndxFlag;
    uint32_t            m_u32ArrayIndex;
	/* reference object type */
	BACNET_OBJECT_TYPE	m_eObjectType;
	/* reference object instance id */
	uint32_t			m_u32ObjId;
	/* reference device instance id */
	uint32_t			m_u32DevId;
	
	/* current event state */
	BACNET_EVENT_STATE	m_eEventStateFrom;
	/* future event state */
	BACNET_EVENT_STATE	m_eEventStateTo;
    /* backup the to event state */
	BACNET_EVENT_STATE	m_eBackupToState;

	/* value of time delay property from event parameters */
	uint32_t			m_u32TimeDelay;

	/* state if the reference object is from this device or other device */
	/* if TRUE - this device else other device */
	bool				m_bThisDevice;

	/* state if the reference object is from this device or other device */
	/* if TRUE - this device else other device */
	bool				m_bStartTimer;

    /** variable for retry count, in case there is need to delete the node if 
        error occurs in acquiring data or invalid data or any api used within timer 
        returns error */
    uint8_t             m_u8RetryCount;

}NodeData_t;

/** struct to create a link list */
typedef struct AlgorithmicListNode
{
	/* struct that holds the actual data for the node */
	NodeData_t	m_stNodeData;
	/* self referiantial pointer to create link list */
	struct AlgorithmicListNode	*m_pstNext;
}AlgorithmicListNode_t;

/* linklist 1st last & mutex lock */
typedef struct AlgorithmicLinkList
{
	/* to store base address of link list i.e. 1st node */
	AlgorithmicListNode_t *pstFirstElem;	
	/* to store address of current i.e. last node */
	AlgorithmicListNode_t *pstLastElem;	
	/* To mutex lock the link list accessz */
	Mutex_H	hMtxLockAlgorithmic;
	/* global flag to enable / disable algorithmic reporting */
	bool bEnableAlgorithmicReporting;
}AlgorithmicLinkList_t;

/** holds the data for the link list */
typedef struct MsgQueListExt
{
	/* device instance id */
	uint32_t			m_u32DevId;
	/* object instance id */
	uint32_t			m_u32ObjectId;
	/* value of time delay property */
	int32_t				m_i32TimeDelay;
	int32_t				m_i32TimeDelayBackup;
	/* object base address */
	void				*m_pvObjBaseAddr;
	/* virtual device pointer */
	void				*m_pvVirtualDev;
	/* property due to which event state needs to be changed either PV or reliablity */
	BACNET_PROPERTY_ID	m_ePropId;
	/* object type */
	BACNET_OBJECT_TYPE	m_eObjectType;
	/* current event state */
	BACNET_EVENT_STATE	m_eEventStateFrom;
	/* future event state */
	BACNET_EVENT_STATE	m_eEventStateTo;
	/* backup the to event state */
	BACNET_EVENT_STATE	m_eBackupToState;
	/* current present value (normally / or any other reference property as per
	object type) - i.e. the value that is presently changed */
	PropertyValue_u		m_uPropValue;
}MsgQueListExt_t;

/** struct to create a link list */
typedef struct LinkListExt
{
	/* struct that holds the actual data for the node */
	MsgQueListExt_t		m_stListData;
	/* self referiantial pointer to create link list */
	struct LinkListExt	*	m_pstNext;
}LinkListExt_t;

/* global variables declaration */
typedef struct ExtAlgoEventStateLinkList
{
	/* to store base address of link list i.e. 1st node */
	LinkListExt_t *m_pstFirstNode;
	/* to store address of current i.e. last node */
	LinkListExt_t *m_pstLastNode;
	/* to store previous node while traversing */
	LinkListExt_t *m_pstPrevious;
	/* To mutex lock the link list accessz */
	Mutex_H	m_hExtAlgoMtxLock;
}ExtAlgoEventStateLinkList_t;

/* global structure for event_state linklist  - declaration only */
extern ExtAlgoEventStateLinkList_t g_stExternalAlgorithmic;

/* global structure for algorithmic reporting linklist  - declaration only */
extern AlgorithmicLinkList_t g_stAlgorithmicReporting;

/** create linklist to monitor property values for algorithmic alarms */
void Create_Algorithmic_Reporting_Linklist(void *pvListData);

/** validate if event type, event parameters & object property ref are valid
    with respect to data type, array index, etc. */
bool Validate_EE_Obj_Properties(void *pvObjAddr, void *pvVirtualDev, bool bCngEventState,
	void **pvInRefObj, void **pvInFeedBkObj, bool *pbIsExternal);

/** fills the data for node to be added in algorithmic reporting linklist */
void *Fill_Data_For_Algorithmic_Reporting(void *pvObject, void *pVirtualDevive, bool bCngEventState,
	void *pvRefObj, void *pvFeedBkObj);

/** timer for algorithmic reporting alarm & event notification service */
TIMER_CALLBACK AlgorithmicReportingTimer(void * lpParam, bool TimerOrWaitFired);

/**	api to clear all pending algorithmic notification belonging to any device,
    whenever that device is deleted. */
void Clear_Algorithmic_Notifications(uint32_t u32DevId);

/**
*
* DESCRIPTION
* This function is used to get present value of reference object
*
* @param  pvObjectAdd	[in]   Object base address
* @param  eObjType		[in]   Reference object type
* @param  ePropertId	[in]   Reference property id
* @param  pstPresentVal	[in]   Pointer to return present value
*
* @return bool	[out]  Success or error code.
*
*/
bool AlgorithmicAlarm_GetPropertyValue_FromRefObject(
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID ePropertId,
	void *pvObjectAdd, 
	AnyValue_t *pstPresentVal);

/**
*
* DESCRIPTION
* This function is used to add event entrollment object ID entry in 
* reference object structure which is used for internal algorithmic reporting
*
* @param  pvObjAdd			[in]   Object base address
* @param  pvVirtualDev		[in]   Virtual device base address
* @param  pstDevObjPropReff	[in]   Device object property reference property pointer 

* @return void		[out]  No return vakue.
*
*/
void AlgorithmicAlarm_AddEEObjectIdNode_InRefObject(
	void *pvObjAdd,
	void *pvVirtualDev,
	BACnetDevObjPropRef_t *pstDevObjPropReff);

/**
*
* DESCRIPTION
* This function is used to remove event entrollment object ID entry from 
* reference object structure.
*
* @param  pvObjAdd			[in]   Object base address
* @param  pvVirtualDev		[in]   Virtual device base address
* @param  pstDevObjPropReff	[in]   Device object property reference property pointer 

* @return void		[out]  No return vakue.
*
*/
void AlgorithmicAlarm_RemoveEEObjectIdNode_FromRefObject(
	void *pvObjAdd,
	void *pvVirtualDev,
	BACnetDevObjPropRef_t *pstDevObjPropReff);

/**
*
* DESCRIPTION
* This function is used to check internal algorithmic reporting is supported by 
* object and property 
*
* @param  pvMonitorVal	[in]   Property value 
* @param  pvVirtualDev	[in]   Virtual device pointer
* @param  eObjType		[in]   Object type
* @param  eObjProp		[in]   Object property
*
* @return void	[out]  No return value.
*
*/
void AlgorithmicAlarm_CheckForEventStateChange(
	void *pvMonitorVal,
	void *pvObjAddr,
	void *pvVirtualDev,
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID eObjProp);

/**
*
* DESCRIPTION
* This function is used to convert property value from 'void' datatype to 
* 'AnyValue_t' datatype
*
* @param  pvData		[in]   Property value
* @param  eObjType		[in]   Object type
* @param  eObjProp		[in]   Object propertye
* @param  pvVirtualDev	[in]   Virtual device pointer
*
* @return bool	[out]  TRUE: indicate value converted successfully
*					   FALSE: indicate value converted successfully
*
*/
bool AlgorithmicAlarm_Convert_MonitoredValue(
	void *pvData,
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID ePropType, 
	AnyValue_t *pstMonitoredValue);

/**
*
* DESCRIPTION
* This function is used to convert reference property value from 'void' datatype to 
* 'AnyValue_t' datatype
*
* @param  pvData		[in]   Property value
* @param  eObjType		[in]   Object type
* @param  eObjProp		[in]   Object propertye
* @param  pvVirtualDev	[in]   Virtual device pointer
*
* @return bool	[out]  TRUE: indicate value converted successfully
*					   FALSE: indicate error in value conversion
*
*/
bool AlgorithmicAlarm_Convert_ReferencePropertyValue(
	void *pvData,
	BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID ePropType, 
	AnyValue_t *pstMonitoredValue);

/**
*
* DESCRIPTION
* This function is used to update external feedback value when event parameter is updated 
* as command-failure event type
*
* @param  pvObject		[in]   Object base address
* @param  pvVirtualDev	[in]   Virtual device pointer
*
* @return void	[out]  No return value.
*
*/
void AlgorithmicAlarm_Update_ExternalFeedbackValue(
	void *pvObject,
	void *pvVirtualDev);

#endif /* (defined ALGORITHMIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_EE && defined BACDEL_OBJ_NC) */

/**
*
* DESCRIPTION
* This function check if event state needs changes in EE object and
* accordingly adds data in algorithmic reporting link list.
*
* @param  pvObject		[in]  EE object base address
* @param  pvVirtualDev	[in]  Virtual device data
* @param  bIsInternal	[in]  Specifies internal or external alarms
* @param  bWriteNULL	[in]  Flag indicate that call is received fron timer
*
* @return void	[out]  No return value
*
*/
void Ext_AlgorithmicAlarm_CheckForEventStateChange(
	void *pvObject,
	void *pvVirtualDev,
	bool bIsInternal,
	bool bWriteNULL);

/**
*
* DESCRIPTION
* Function checks if event state property needs to be updated or not depending
* upon the external property value.
* This function is called only after external property value is received from application
*
* @param  eObjectType		[in]  Object type
* @param  pvObjectAddr		[in]  EE object base address
* @param  pstMonitoredValue	[in]  Monitor property value 
* @param  pbEventChange		[in]  Flag indicate if event state needs to be updated or not
* @param  peEventStateTo	[in]  Pointer to hold the current value of event state
* @param  peEventStateFrom	[in]  Pointer to hold the new value of event state
* @param  pu32TimeDelay		[in]  Pointer to save time delay value
* @param  pvVirtualDev		[in]  Pointer of virtual device
* @param  bIsInternal		[in]  Specifies internal or external alarms
*
* @return BACNET_RETURN_TYPE	[out]  Success or suitable error code.
*
*/
BACNET_RETURN_TYPE Ext_AlgorithmicAlarm_UpdateEventState(
	BACNET_OBJECT_TYPE eObjectType,
	void *pvObjectAddr,
	AnyValue_t *pstMonitoredValue,
	bool *pbEventChange,
	BACNET_EVENT_STATE *peEventStateTo,
	BACNET_EVENT_STATE *peEventStateFrom,
	uint32_t *pu32TimeDelay, 
	void *pvVirtualDev,
	bool bIsInternal);

/**
*
* DESCRIPTION
* This function is used to convert property value from 'AnyValue_t' datatype to 
* 'PropertyValue_u' datatype.
* In external algorithmic list, property value needs to add of type  'PropertyValue_u'.
*
* @param  pstSrcValue	[in]   property value is read from here
* @param  puDestValue	[out]  converted value is stored here
* @param  pu32CopySize	[out]  size of data type copied
*
* @return void	[out]  No return value.
*
*/
void Ext_AlgorithmicAlarm_ConvertPresentValue(
	PropertyValue_u	*puDestValue,
	AnyValue_t *pstSrcValue,
	uint32_t *pu32CopySize);

/**
*
* DESCRIPTION
* This function checks if node is to be deleted from external linklist
*
* @param  pstDeleteNode	[in]  Pointer of node to be deleted
* @return void			[out] Void/nothing.
*
**/
static void Ext_AlgorithmicAlarm_CheckAndDeleteThisNode(LinkListExt_t *pstDeleteNode);

/**
*
* DESCRIPTION
* This function is used to provide monitored value for external alarms 
* functionality.
*
* @param  pstMonitoredValue			[in]  Monitored property value
* @param  pstMonitoredStatusFlags	[in]  Monitored object status flags
* @param  pvVirtualDev				[in]  Virtual device data
* @param  eObjectType				[in]  Event enrollment object type
* @param  u32ObjectId				[in]  Instance no of event enrollment object
* @param  bIsInternal				[in]  Specifies internal or external alarms
*
* @return void	[out]  No return value
*
*/
void Ext_AlgorithmicAlarm_SaveDataAndCheckForEventChange(
	AnyValue_t *pstMonitoredValue,
	BACnetBitStr_t *pstMonitoredStatusFlags,
	void *pvVirtualDev,
	BACNET_OBJECT_TYPE eObjectType,
	uint32_t u32ObjectId,
	bool bIsInternal);

/**
*
* DESCRIPTION
* This function is used to add new node in external algorithmic link list
*
* @param  pstData	[in]   Pointer of data need to add in link list
* @return void		[out]  No return vakue.
*
*/
static void Ext_AlgorithmicAlarm_AddNodeToList(MsgQueListExt_t *pstData);

/**
*
* DESCRIPTION
* This function creates the linklist for objects whose external property value is 
* received from application. it then monitors the property value till time_delay (property) expires.
* If it finds that event state needs to be changed after expiry of timer, then it
* will signal out to update the event state.
*
* @param  ePropId			 [in]  Property ID
* @param  eObjectType		 [in]  Object type
* @param  pvObjBaseAddr		 [in]  Object base address
* @param  peEventStateTo	 [in]  Pointer to hold the current value of event state
* @param  peEventStateFrom	 [in]  Pointer to hold the new value of event state
* @param  puPropValue		 [in]  Property value
* @param  u32TimeDelay		 [in]  Time delay value
* @param  u32TimeDelayNormal [in]  Time delay normal value
* @param  pvVirtualDev		 [in]  Virtual device data
*
* @return BACNET_RETURN_TYPE [out]  Success or suitable error code.
*
*/
BACNET_RETURN_TYPE Ext_AlgorithmicAlarm_CreateEventStateList(
	BACNET_PROPERTY_ID ePropId,
	BACNET_OBJECT_TYPE eObjectType,
	void *pvObjBaseAddr,
	BACNET_EVENT_STATE eEventStateTo,
	BACNET_EVENT_STATE eEventStateFrom,
	PropertyValue_u *puPropValue,
	uint32_t u32TimeDelay,
	uint32_t u32TimeDelayNormal,
	void *pvVirtualDev);

/**
*
* DESCRIPTION
* Function checks if property state and its values from event parameters 
* matches with external value received from application.
*
* @param  pstPropValue	[in]  External property value
* @param  pstListVal	[in]  Change of state event values
*
* @return bool	[out]  True indicate change of state values and external 
*					   value received from application are same.
*
*/
static bool Ext_AlgorithmicAlarm_CheckChangeOfStateValues( 
	AnyValue_t *pstPropValue, 
	ListOfBACnetPropertyStates_t *pstListVal);

/**
*
* DESCRIPTION
* This is timer for external algorithmic reporting service. It is called every 1 second.
*
* Scan entire external algorithmic reporting linklist.
* - decrement time_delay by 1 second for every node in linklist.
* - if time_delay becomes zero check actual present value & present value in node matches
*     - if it matches, update event state / reliablity (as applicable)
*     - else delete the node.
* - repeate same for every node in list.
*
*/
//TIMER_CALLBACK Ext_AlgorithmicAlarm_Timer(
void Ext_AlgorithmicAlarm_Timer(
	void * lpParam,
	bool TimerOrWaitFired);

#endif /* ALGORITHMIC */
