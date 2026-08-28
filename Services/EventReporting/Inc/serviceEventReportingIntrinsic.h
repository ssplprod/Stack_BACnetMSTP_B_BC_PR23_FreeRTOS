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
******************************************************************************/
/****************************************************************************** 
*                                                                         
*   FILE
*	 serviceIntrinsicReporting.h
*                                                                      
*   AUTHORS                                                                             
*		Pratham N. Murkute
*                                                                         
*   DESCRIPTION                                                            
*		Intrinsic Event Notification Reporting.
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE        NAME		    		DESCRIPTION
*   03/03/2020  Pranav Phadatare		Add m_eReliabilty variable in MsgQueList_t
*   									to provide the support of Fault-Out of range
*   									Algorithm
*
******************************************************************************/
#ifndef INTRINSIC_H
#define INTRINSIC_H
#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)

#include "objDevice.h"

/* macro definations */
#define MATCH_FOUND_IN_ALARM_VALUES (1)
#define MATCH_FOUND_IN_FAULT_VALUES (-1)
#define NO_MATCH_FOUND (0)
#define MATCH_FOUND_IN_LIFE_SAFETY_ALARM_VALUES (2)

/** holds the data for the link list */
typedef struct MsgQueList
{
    /* device instance id */
	uint32_t			m_u32DevId;
	/* object instance id */
	uint32_t			m_u32ObjectId;
	/* value of time delay property */
	int32_t				m_i32TimeDelay;
	int32_t				m_i32TimeDelayBackup;
	/* object base address */
	void *				m_pvObjBaseAddr;
	/* virtual device pointer */
	void *				m_pvVirtualDev;
	/* property due to which event state needs to be changed either PV or reliablity */
	BACNET_PROPERTY_ID	m_ePropId;
	/* object type */
	BACNET_OBJECT_TYPE	m_eObjectType;
	/* current event state */
	BACNET_EVENT_STATE	m_eEventStateFrom;
	/* future event state */
	BACNET_EVENT_STATE	m_eEventStateTo;
	/*Reability*/
	BACNET_RELIABILITY  m_eReliabilty;
    /* backup the to event state */
	BACNET_EVENT_STATE	m_eBackupToState;
	/* current present value (normally / or any other reference property as per 
        object type) - i.e. the value that is presently changed */
	PropertyValue_u		m_uPropValue;
}MsgQueList_t;

/** struct to create a link list */
typedef struct LinkList
{
	/* struct that holds the actual data for the node */
	MsgQueList_t		m_stListData;
	/* self referiantial pointer to create link list */
	struct LinkList	*	m_pstNext;
}LinkList_t;

/* Global Variables Declaration */
typedef struct EventStateLinkList
{
	/* to store base address of link list i.e. 1st node */
	LinkList_t *pstFirstNode;	
	/* to store address of current i.e. last node */
	LinkList_t *pstLastNode;	
	/* to store previous node while traversing */
	LinkList_t *pstPrevious;	
	/* To mutex lock the link list accessz */
	Mutex_H	hMtxLock;
}IntrinsicLinkList_t;

/* global structure for event_state linklist  - declaration only */
extern IntrinsicLinkList_t g_stIntrinsicReporting;

/**	creates linklist to monitor every node in list for the period equal to time_delay */
BACNET_RETURN_TYPE Create_EventState_LinkList(BACNET_PROPERTY_ID	ePropId,
											  BACNET_OBJECT_TYPE	eObjectType,
											  void *				pvObjBaseAddr,
											  BACNET_EVENT_STATE	eEventStateTo,
											  BACNET_EVENT_STATE	eEventStateFrom,
											  BACNET_RELIABILITY    eReliabilty,
											  PropertyValue_u		*puPresentValue,
											  uint32_t				u32TimeDelay,
											  uint32_t				u32TimeDelayNormal,
											  void *				pvVirtualDev);

/** timer callback function that waits for time_delay amt of time & then checks if event
	state needs to be changed or not */
TIMER_CALLBACK IntrinsicReportingTimer(void*  lpParam, bool TimerOrWaitFired);

/**	function required for intrinsic reporting  
	updates event state when present value changes */
BACNET_RETURN_TYPE UpdateEventState(BACNET_OBJECT_TYPE eObjectType,
									void *pvObject,
									void *pvData,
									bool *pbEventChange,
									BACNET_EVENT_STATE	*peEventStateTo,
									BACNET_EVENT_STATE	*peEventStateFrom,
									BACNET_RELIABILITY *peReliabilty,
									PropertyValue_u *puPresentValue);

/** returns true if the monitored property for intrinsic alarms is PV */
bool Is_Reference_Prop_PV(BACNET_OBJECT_TYPE eObjectType);

/** save the reference property value that is monitored to generate
    intrinsic alarms. 
    it is mostly present value for most object type, except few */
void Get_Reference_Prop_Value(BACNET_OBJECT_TYPE eObjectType,
					   PropertyValue_u *puPropValue,
					   void *pvObject);

/** function checks if event notification is to be ganerated in case the
	reliability property changes */
BACNET_RETURN_TYPE UpdateEventState_On_Reliablity(BACNET_OBJECT_TYPE eObjectType,
									void *pvObject,
									void *pvData,
									bool *pbEventChange,
									BACNET_EVENT_STATE	*peEventStateTo,
									BACNET_EVENT_STATE	*peEventStateFrom,
									PropertyValue_u *puPresentValue,
									virtualDevData_t *pVirtualDev,
									uint16_t *pu16ExtFlag,
									uint32_t *pu32ExtTimeDelay);

/** function checks if event notification is to be ganerated in case the 
	values of reference properties change with respect to present value */
void Check_For_Intrinsic_Notification(BACNET_OBJECT_TYPE eObjectType, 
					void *pvObject, virtualDevData_t *pVirtualDev);

/**	api to clear all pending intrinsic notification belonging to any device,
    whenever that device is deleted. */
void Clear_Intrinsic_Notifications(uint32_t u32DevId);

#endif /* (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC) */
#endif /* INTRINSIC_H */
