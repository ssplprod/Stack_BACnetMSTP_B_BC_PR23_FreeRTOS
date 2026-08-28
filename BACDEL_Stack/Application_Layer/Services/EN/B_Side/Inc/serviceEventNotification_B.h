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
*   SoftDEL Systems Ltd.                                 india@softdel.com         
*   3rd Floor, Pentagon P4,                                http://www.softdel.com  
*    Magarpatta City, Hadapsar
*    Pune - 411 028                                
*                                                                         
******************************************************************************/
/**
*********************************************************************************
*
*   FILE
*      serviceEventNotification_B.h
*                                                                      
*   AUTHORS                                                                     
*       
*                                                                         
*   DESCRIPTION
*		Definations & declarations for serviceEventNotification_B.c
*       Initiate & Encode the confirmed & unconfirmed event notifications.  
*                                                                         
*   RELEASE HISTORY                                                                 
*        DATE           NAME                DESCRIPTION
*       
*********************************************************************************/

#ifndef SER_EN_B_H
#define SER_EN_B_H
#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)

#include "objDevice.h"

/** macro definations */
#define LIMIT_ENABLE_LOW 1
#define LIMIT_ENABLE_HIGH 2
#define LIMIT_ENABLE_BOTH 3
#define LIMIT_ENABLE_NONE 0

/** global variable for sequence no */
extern uint32_t g_u32SeqNo;

/** To send data through Message Queue */
typedef struct EventNotificationMsgQ
{
	/* object type */
	BACNET_OBJECT_TYPE	m_eObjectType;
	/* object base address */
	void *				m_pvObjBaseAddr;
	/* current event state */
	BACNET_EVENT_STATE	m_eEventStateFrom;
	/* future event state */
	BACNET_EVENT_STATE	m_eEventStateTo;
	/* current relibility */
	BACNET_RELIABILITY	m_eReliability;
	/* current present value - i.e. the value that is presently changed */
	PropertyValue_u		m_uPropValue;
	/* Pointer to itself to free it later on */
    void				*m_pvSelfAddress;
	/* virtual device address */
	void				*m_pvVirtualDev;
} EventNotificationMsgQ_t;

#ifdef BACDEL_SER_AE_AA_B

/** struct to save event notification for ack alarm service */
typedef struct SavedEventNotifications
{
	/* struct to hold data of notification */
	BacnetEnPropElem_t m_stENdata;
	/* next pointer to create linklist */
	struct SavedEventNotifications *m_pstNext;
} SavedEventNotifications_t;

/** linklist to save event notification for ack alarm service */
typedef struct SavedNotificationsLinklist
{
	/* 1st node of list */
	SavedEventNotifications_t *pstRootNode;
	/* last node of list */
	SavedEventNotifications_t *pstLastNode;
	/* mutex lock for list */
	Mutex_H	hMtxSavedNotification;
} SavedNotificationslist_t;

/* global struct for saved notifications list */
extern SavedNotificationslist_t g_stSavedEventNotifications;

#endif /* BACDEL_SER_AE_AA_B */

/** Function to check if two event notifications are to be generated */
bool IsDoubleNotification(BACNET_OBJECT_TYPE eObjectType,
		BACNET_EVENT_STATE eFromState, BACNET_EVENT_STATE eToState);

/** function that handles the event notifications, sends notifications to client from 
	recipient list */
BACNET_RETURN_TYPE Send_Event_Notification(uint32_t u32Value, void *pstMsgQueData);

/** event notification encoder for confirmed & unconfirmed event notification */
int32_t Event_Notification_Encoder(processInfo_t *pstProcQInfo, uint8_t *pu8APDUResp,
									int32_t i32APDULen);

/**	This API will decide if Event_Notification is to be generated or not.	*/	
bool Check_For_Event_Notification_Generation(void *pvObjBaseAddr, BACNET_OBJECT_TYPE eObjectType,
										BACNET_EVENT_STATE *eEventStateTo,
										BACNET_EVENT_STATE *eEventStateFrom);

/** Posts the struct to be passed to event notification thread.
	check if 2 notifications are to be generated or not **/
void Post_Event_Notification(BACNET_OBJECT_TYPE	eObjectType,
							void *				pvObjBaseAddr,
							BACNET_EVENT_STATE	eEventStateTo,
							BACNET_EVENT_STATE	eEventStateFrom,
							BACNET_RELIABILITY	eReliability,
							PropertyValue_u		*puPropValueData,
							void *				pvVirtualDev);

/** Sends the event notification to recipient based on the value received. */
void Send_Notification(bacnetip_arguments_t *pstIpArgs,
					   virtualDevData_t *pVirtualDev,
					   ListOfBACnetDestination_t *pstNCRecepient,
					   BacnetEnPropElem_t *pstENdata);

#endif /* BACDEL_SER_AE_EN_B */
#endif /* SER_EN_B_H */
