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
*	 serviceCommomChangeOfValue_B.h
*                                                                      
*   AUTHORS                                                                             
*	 
*                                                                         
*   DESCRIPTION                                                            
*	 Defines the COV_B & COVP_B common functions for the BACnet application. 
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE        NAME		    DESCRIPTION
*  
******************************************************************************/

#ifndef COV_B_COMMOM_H
#define COV_B_COMMOM_H
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)

#include "objDevice.h"
#include "bacnetInitiateServiceMgmt.h"
#include "bacnetStackMgmt.h"

#define TAG_PROCESS_ID			0
#define TAG_OBJECT_ID			1
#define TAG_NOTIFICATION_TYPE	2
#define TAG_TYPE_LIFETIME		3

#define MINIMUM_DATA_LENGTH		7
#define NULL_VALUE				0
#define NOTIFICATION_LENGTH		2
#define LIFETIME_LENGTH			2

#define COV_CANCELLATION		0
#define COVP_CANCELLATION		0

#define SHIFT_RIGHTBY_TWO		2
#define MASKBIST				0x03


#define ALL_MANDATORY_PRO_PPRESENT 7

/* enum for cov subscription type */
typedef enum{
	COV_SUBS_DEFAULT = 0,
	RESUBSCRIP_REQ = 1,
	ADD_NEW_RECIPIENT = 2,
	ADD_NEW_REQ = 3,
}BACNET_COV_SUBS;

/** To send data through Message Queue */
typedef struct msgquedata_t
{
    /** If this is the first Notification */
    BOOL m_bFstNotification;
    /** Pointer to property value */
    void *m_pvPropertyValue;
    /** Pointer to Device structure */
    void *m_pvDeviceStruct;
    /** Provides the data type */
    BACNET_PROPERTY_ID m_eDevProp;
    /** Provide the destination address of the request */
    BACnetAddress_t m_stDestAddress;
    /** Provide the Process ID for which the notification is to be sent */
    int32_t m_i32ProcessId;
    /** Pointer to itself to free it later on */
    void *m_pvSelfAddress;
	/* Provide current bit string data */
	BACnetBitStr_t m_stStatusFlags;
}msgquedata_t;


/*
 *  Extracts the address location & index number where new subscription is
 *  to be added.
 */
void *Extract_COV_Subscription_Index(BACNET_COV_SUBS *peCovSubType,
									 uint32_t *pu32CovIndex,
                                     ListOfBACnetCovSubs_t *pstCOVSubscribe, 
                                     BACNET_PROPERTY_ID ePropertyID,
                                     virtualDevData_t *pstVirtualDev);

/*
 *  Extracts the address location where new recipient is to be added.
 */
void *Traverse_COV_Recipient_List(virtualDevData_t *pstVirtualDev, 
                                  ListOfBACnetCovSubs_t *pstActiveCOVList,
								  BACNET_COV_SUBS *peCovSubType, 
                                  ListOfBACnetCovSubs_t *pstSubscribeCOV);

/* 
 *  Informs if the Object type supports COV.
 */
BOOL COV_Object_Support(void *pvObject, BACNET_OBJECT_TYPE eObjectType);

/* 
 *  Based on Object Type this API returns pointer to the address of the 
 *  Object Instance requested after traversing object linked list. 
 */
void *Extract_Object_Address(BACNET_OBJECT_TYPE  eObjectType,
                                  uint32_t ObjectID,
                                  virtualDevData_t *pstVirtualDev,
                                  processInfo_t * pstProcQInfo);

/* 
 *  Check if the object supports the property.If no return NULL otherwise
 *  Get the address of the COV property of specific object.
 */
void *Validate_And_Get_Property_COV_Addr(BACNET_OBJECT_TYPE eObjectType, void *pvObject,
                       BACNET_PROPERTY_ID ePropertyId);

/* 
 *  Provides the address of the specific instance number of an object.
 */
void *Get_Object_Instance_Pointer(void *pvObject, uint32_t ObjectID,
                                    BACNET_OBJECT_TYPE  eObjectType);

/* 
 *  Provides address of m_i32ActiveCOV element of a property.
 */
void *Get_ActiveCOV_Value_Pointer(void *pvObject, BACNET_OBJECT_TYPE eObjectType,
                          BACNET_PROPERTY_ID eDevProp);

/*
 *  Extracts the COV request data, forward for cancellation.
 */
BOOL Cancel_COV_Subscription(virtualDevData_t *pstVirtualDev,
                             void *pvObjectIndex, ListOfBACnetCovSubs_t *pstCOVSubReq,
                             BACNET_ERROR_CODE *eErrorCode);

/* 
 *  Extracts the index of ListOfBACnetCovSubs_t struct for cancellation of
 *  subscription.
 */
BOOL Extract_COV_Cancellation_Index(uint16_t u16ObjectType,
									 uint32_t u32ObjId, 
                                     BACNET_PROPERTY_ID ePropertyID,
                                     ListOfBACnetCovSubs_t *pstCOVSubsReq,
                                     Pr_ListOfBACnetCovSubs_t *pstCOVSubscribeList,
                                     void *pvObjectIndex,
                                     BACNET_ERROR_CODE *peErrorCode);

/*
 *  Extracts the index of CovRecipientInfo_t struct for cancellation of
 *  subscription.
 */
bool Extract_Recipient_Elem(ListOfBACnetCovSubs_t *pstActiveCOVList,
                            ListOfBACnetCovSubs_t *pstCOVSubscribe);

/*
 *  Validate Access Type and Array Index. Extracts the address of respective
 *  property value.
 */
void Read_Property_Value(void *pvObjectIndex, BACNET_OBJECT_TYPE eObjectType,
                         CovPropElement_t *pstCOVproperty,
						 void *pVirtualDev);

/*
 *  Extracts the value of COVIncrement property.
 */
void *Get_COVIncrementValue(void *pvObjectIndex,
                            BACNET_OBJECT_TYPE eObjectType,
							void *pVirtualDev);

/*
 *  Generates notification based on the value received.
 */
BACNET_RETURN_TYPE Send_COV_Notification(uint32_t u32COVIndex,
                              msgquedata_t *pstMsgQueData);

/*
 *  Creates the struct to be passed to message queue.
 */
void *Create_Message_Queue_Struct(virtualDevData_t *pstVirtualDev,
                                  void *pvCurrentData, BOOL bValue,
                                  BACNET_PROPERTY_ID eDevProp,
                                  BACnetAddress_t m_stDestAddress,
                                  int32_t i32ProcessID);

/* 
 *  Copies the value from the ActiveCOVSubscription list to be send.
 */
void *Get_Notification_Parameter(virtualDevData_t *pstVirtualDev,
                                ListOfBACnetCovSubs_t *pstActiveCOVList,
                                CovRecipientInfo_t *pstRecipientInfo,
								msgquedata_t *pstMsgQueData, bool bCopyData);

/*
 *  Encodes the COV Notification to be send.
 */
int32_t COVB_Notify_Encoder(processInfo_t *pstProcQInfo, uint8_t *pu8APDUResp,
                           int32_t i32APDULen);

/*
 *  This is callback thread initiated from hInitiateTimer.
 *  It is called every 1 second.
 */
void DvCOV_B_Cancellation_Thread_Task(void);


/*
 *  Encodes the COV Notification to be send.
 */
BOOL Delete_COV_Subscription(CovRecipientInfo_t *pstRecipientInfo,
                         CovRecipientInfo_t **pstRecipientAdd);

/*
 *	Decodes optional properties for cov & covp.
 */
void Decode_Optional_Property(uint8_t u8TagNumber,
							uint8_t *pu8ServiceRequest,
							ListOfBACnetCovSubs_t *pstCOVPSubscribe, 
                            uint8_t *pu8IsoptionalProp,
							int32_t *pi32Len,
							uint32_t *pu32LenValue );
void* GetListStoredValue(processInfo_t *pstProcQInfo);

/** function to update the cov increment value in existing 
    subscription when cov increment property value is changed */
void Update_CovIncrement_In_Subscription(virtualDevData_t *pVirtualDev, 
            BACNET_OBJECT_TYPE eObjType, void *pvObject,
            void *pvCovIncValue, uint32_t u32Len);

/**
* DESCRIPTION
* Returns the COV subscription type - (new subscription, new recepient, re-subscription)
*
* @param pstCovSubscribeReq	[in] subscription request
* @param pstVirtualDev		[in] Pointer to Virtual Device struct.
*
* @return COV subscription request type.
*
*/
BACNET_COV_SUBS Get_CovSubs_Type(
	ListOfBACnetCovSubs_t *pstCovSubscribeReq, 
	virtualDevData_t *pstVirtualDev);

#endif /* #if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B) */
#endif /* COV_B_COMMOM_H */
