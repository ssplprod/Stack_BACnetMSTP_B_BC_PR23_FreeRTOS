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
*   SoftDEL Systems Ltd.                                india@softdel.com      
*   3rd Floor, Pentagon P4,                             http://www.softdel.com
*    Magarpatta City, Hadapsar
*    Pune - 411 028       
*
*    File Name - objDevice.h
*
*   RELEASE HISTORY                                                             
*   DATE         NAME                 DESCRIPTION
*   25/05/2011   Harshal Mangale      File Created
*   25/05/2011   Harshal Mangale      Implemented Virtual Device struct
*                                     and device object structure
*   20/06/2011   Prashant Badgujar    Added fields to devObject_t
*   30/01/2011   Prashant Badgujar    Updated comments section of API
*                                     & removed #if 0 old code.
*   06/07/2011   M.Venu               Inluded Multistate Object reference
*   07/07/2011   Prashant Badgujar    Modified function return types.
*   07/07/2011   Ashish Verma         Added Check_Device_Address_Range,
*                                     Who_Is_Request_Parser & 
*                                     Fill_I_AM_Structure functions.
*	15/07/2011	 Prashant Badgujar	  Changed Name m_pstObjectIDList
*									  to m_pstObjectIDList
*   15/07/2011   Ashish Verma         Removed functions 
*                                     Who_Is_Request_Parser,
*                                     Check_Device_Address_Range & 
*                                     Fill_I_AM_Structure.
*   18/07/2011   Prashant Badgujar    Changed Return type for 
*                                     Dv_Read_Prop 
*	20/07/2011	 Prashant Badgujar	  Modified Function Prototype to 
*                                     remove OS specific return type 
*                                     DvProcessThread
*   20/07/2011   Prashant badgujar    Code Review Comment Implementation
*	22/07/2011	 Prashant Badgujar	  Changing Dv_Write_Prop Prototype
*   26/07/2011   Ashish Verma         Modified devObject_t structure & 
*                                     supportedObj_t structure
*   28/07/2011   Ashish Verma         Modified virtualDevData_t
*	04/08/2011	 Prashant Badgujar	  Adding Create_Object prototype.
*   09/08/2011   Ashish Verma         Modified devObject_t structure.
*   18/08/2011   Prashant Badgujar    Removing m_stSupportedObjects & 
*                                     m_pstServiceSupported from device
*                                     structure.
*   19/09/2011   Ashish Verma         Modified the structure supportedObj_t. 
*   21/09/2011   Ashish Verma         Added u8DevCommStatus variable to
*                                     structure virtualDevData_t.
*	22/09/2011	 Prashant Badgujar	  Adding Default initializer for Required properties
*									  & removing device object variable from supportedObj.
*   23/09/2011   Ashish Verma         Added the handle for COV & DCC thread, 
*                                     device status, DCC lifetime & mutex
*                                     handle to virtualDevData_t structure.
*                                     Added fucntions Initiate_Timer_Param() &
*                                     TimerCallBackFun().
*   29/09/2011  Ashish Verma          Modified MAX_APDU_LENGTH_ACCEPTED from 1
*                                     to 1476. Added fucntion Initiate_Services
*                                     BACDEL_Get_Device_ID_Or_IP_Address &
*                                     Update_Device_Address_Binding()
*	30/09/2011	Prashant Badgujar	  Adding support of Remote Device.
******************************************************************************/

/** @file objDevice.h Defines functions for handling all BACnet objects belonging
 *                 to a BACnet device, as well as Device-specific properties. */

#ifndef DEVICE_H
#define DEVICE_H

#include "osalFreeRTOS.h"

#include "bacDELDef.h"
#include "pduServiceStructure.h"
#include "bacnetStackMgmt.h"
#include "serviceGenericHandler.h"

#ifdef BACDEL_OBJ_AI
#include "objAnalogInput.h"
#endif
#ifdef BACDEL_OBJ_AO
#include "objAnalogOutput.h"
#endif
#ifdef BACDEL_OBJ_AV
#include "objAnalogValue.h"
#endif
#ifdef BACDEL_OBJ_BO
#include "objBinaryOutput.h"
#endif
#ifdef BACDEL_OBJ_BI
#include "objBinaryInput.h"
#endif
#ifdef BACDEL_OBJ_BV
#include "objBinaryValue.h"
#endif
#ifdef BACDEL_OBJ_MSI
#include "objMultiStateInput.h"
#endif
#ifdef BACDEL_OBJ_MSO
#include "objMultiStateOutput.h"
#endif
#ifdef BACDEL_OBJ_MSV
#include "objMultiStateValue.h"
#endif
#ifdef BACDEL_OBJ_NC
#include "objNotificationClass.h"
#endif
#ifdef BACDEL_OBJ_EE
#include "objEventEnrollment.h"
#endif
#ifdef BACDEL_OBJ_CAL
#include "objCalendar.h"
#endif
#ifdef BACDEL_OBJ_SDL
#include "objSchedule.h"
#endif
#ifdef BACDEL_OBJ_NP
#include "objNetworkPort.h"
#endif

/* Device Object identifier which is special object identifier and 
should not be used to identify any device */
#define SPECIAL_DEVICE_OBJECT   4194303

/** Global variable to save character encoding for all Host devices */
extern BACNET_CHARACTER_STRING_ENCODING g_eCharEncoding;

/**
*                                                                         
*Name - devObject                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the device object properties.
*   
*/
typedef struct devObject
{
    /** ID used to identify the object */
    Pr_BACnetObjId_t				m_stObjectID; 

    /** Device Object Name */
    Pr_BACnetCharStr_t				m_stObjName;

    /** Type of Object. It should be DEVICE.*/
    Pr_BACnetObjType_t				m_stObjectType;

    /** Physical & Logical Stauts of device object.*/
    Pr_BACnetDevStatus_t            m_stSystemStatus;

    /** Manufacturer of BACner Device */
    Pr_BACnetCharStr_t              m_stVendorName;

    /** Vendor ID code assigned by ASHARE */
    Pr_BACnetUnsigned16_t           m_stVendorId;

    /** Model of BACner Device */
    Pr_BACnetCharStr_t              m_stModelName;

    /** Level of firmware installed in BACnet Device */
    Pr_BACnetCharStr_t              m_stFirmwareRev;

    /** Version of application sw installed in the machine */
    Pr_BACnetCharStr_t              m_stApplSoftwareVersion;

    /** Protocol version supported by BACnet Device */
    Pr_BACnetUnsigned32_t           m_stProtocolVersion;

    /** Minor revision level of the BACnet standard */
    Pr_BACnetUnsigned32_t           m_stProtocolRevision;

	/** A logical revision number for the device's database. 
        It is incremented when an object is created, an object 
        is deleted, an object's name is changed, an object's 
        Object_Identifier property is changed, or restored*/
    Pr_BACnetUnsigned32_t           m_stDatabaseRev;

    /** The amount of time in milliseconds between retransmissions
        of an APDU requiring acknowledgment for which no acknowledgment 
        has been received. */
    Pr_BACnetUnsigned32_t           m_stAPDUTimeout;

    /** Maximum number of times that APDU shall be transmitted. Default is 3 */
    Pr_BACnetUnsigned32_t           m_stNumOfAPDURetries;

	/** Maximum length of APDU supported by Device should be greater than or  
        equal to 50 */
    Pr_BACnetUnsigned16_t           m_stMaxAPDULenAccepted;

	/** Type of Segementation supported by the Device Object */
    Pr_BACnetSegmentation_t         m_stSegmentationSupport;

    /** Types of services supported by Device protocol implementation */
    Pr_BACnetServicesSupported_t    m_stServicesSupported;

    /** Objects type supported by this Device */
    Pr_BACnetObjectTypesSupported_t m_stObjectTypeSupported;

    /** ReadOnly list of Object Identifiers */
    Pr_ListOfObjId_t				m_stObjectIDList;

    /** actual device addresses that will be used when the remote device
        must be accessed via a BACnet service request*/
    Pr_ListOfBACnetAddrBinding_t    m_stDevAddBinding;

#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object. 
	The Object_Name, Object_Type, Object_Identifier, and Property_List properties 
	are not included in the list */
    Pr_BACnetPropertyList_t         m_stPropertyList;
#endif

    /** Shall be present if the device is a master node on an MS/TP network */
    Pr_BACnetUnsigned32_t           m_stMAXMaster;

    /** Shall be present if the device is a node on an MS/TP network*/
    Pr_BACnetUnsigned32_t           m_stMaxInfoFrames;

#ifdef SEGMENTATION_SUPPORTED
	/** Time in miliseconds between retransmission of an APDU segement 
        Default value is 2000 milisec */
    Pr_BACnetUnsigned32_t           m_stAPDUSegTimeout;

    /** Max Number of Segments that device can accept */
    Pr_BACnetUnsigned16_t           m_stMaxSegAccepted;
#endif

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
    /** Whenever a COV Subscription is created with the SubscribeCOV or 
        SubscribeCOVProperty service, a new entry is added to this*/
    Pr_ListOfBACnetCovSubs_t		m_stActiveCOVSubList;

    /** The COV variable which describes the COV service status */
    int32_t							i32ActiveCOV;
#endif

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B || \
	     (defined BACDEL_SER_AE_AA_A && defined BACDEL_PR23))
    /** Time of Day. If device dont have knowledge of time then shall be 
        omitted */
    Pr_BACnetTime_t                 m_stLocalTime;  

    /** Date. If device dont have knowledge of time then shall be omitted */
    Pr_BACnetDate_t                 m_stLocalDate; 
#endif

#ifdef BACDEL_SER_DM_UTC_B
    /** The number of minutes offset between local standard time and UTC*/
    Pr_BACnetSigned32_t             m_stUTCOffset;

    /** Daylight saving time effect */
    Pr_BACnetBool_t                 m_stDaylightSavingStatus;
#endif

#if (defined BACDEL_SER_DM_RD_B)
    //Pr_ListOfBACnetRecipient_t      m_stRestartNotificationReceipients;
#endif

    /** Physical Location of BACnet Device */
    Pr_BACnetCharStr_t              m_stLocation;

    /** BACnet Device Description */
    Pr_BACnetCharStr_t              m_stDescription;

#ifdef OPTIONAL_PROPERTY

#ifdef BACDEL_PR14
	/** Represent serial number in a vendor-specific model series. 
	The combination of Model_Name, Vendor_Identifier and Serial_Number 
	uniquely identifies a device. */
    Pr_BACnetCharStr_t              m_stSerialNumber;
#endif
#endif /* OPTIONAL_PROPERTY */
    /* This flag bit is added to inform application about callback registration */
    bool							bAppCallBack;

}devObject_t;

/**
*                                                                         
*Name - status_t                                         
*                                                                      
*DESCRIPTION                                                            
*   Supported Objects by Device. This structure contains start pointer of each 
*   object type supported by devuce.
*/
typedef struct supportedObj
{
    uint32_t     ui32dummy;
#ifdef BACDEL_OBJ_AI
    /** Analog Input Start Pointer in Device Object List */
    analoginput_t      *m_pstAnalogInput;
#endif
#ifdef BACDEL_OBJ_AO
    /** Analog Output Start Pointer in Device Object List */
	analogoutput_t     *m_pstAnalogOutput; 
#endif
#ifdef BACDEL_OBJ_AV
    /** Analog Value Start Pointer in Device Object List */
	analogvalue_t      *m_pstAnalogValue; 
#endif
#ifdef BACDEL_OBJ_BI
    /** Binary Input Start Pointer in Device Object List */
    BinaryInput_t       *m_pstBinaryInput;
#endif
#ifdef BACDEL_OBJ_BO
    /** Binary Output Start Pointer in Device Object List */
    BinaryOutput_t      *m_pstBinaryOutput;
#endif
#ifdef BACDEL_OBJ_BV
    /** Binary Value Start Pointer in Device Object List */
    BinaryValue_t       *m_pstBinaryValue;
#endif
#ifdef BACDEL_OBJ_MSI
    /** Multistate Input Start Pointer in Device Object List */
    MultistateInput_t   *m_pstMSInput;
#endif
#ifdef BACDEL_OBJ_MSO
    /** Multistate Output Start Pointer in Device Object List */
    MultistateOutput_t  *m_pstMSOutput;
#endif
#ifdef BACDEL_OBJ_MSV
    /** Multistate Value Start Pointer in Device Object List */
    MultistateValue_t   *m_pstMSValue; 
#endif
#ifdef BACDEL_OBJ_NC
    /** NOTIFICATION Object Start Pointer in Device Object List */
    Notificationclass_t	*m_pstNotificationClass;
#endif
#ifdef BACDEL_OBJ_CAL
    /** Calender Object Start Pointer in Device Object List */
    Calendar_t		    *m_pstCalendar;
#endif
#ifdef BACDEL_OBJ_SDL
    /** Schedule Object Start Pointer in Device Object List */
    Schedule_t		    *m_pstSchedule;
#endif


#ifdef BACDEL_OBJ_DEV
    /** Device Object Start Pointer in Device Object List */
    devObject_t         *m_pstDevObject;
#endif
#ifdef BACDEL_OBJ_NP
    /** Network Port Object Start Pointer in Device Object List */
    NetworkPort_t       *m_pstNetworkPort;
#endif


}supportedObj_t;

/**
*                                                                         
*Name - virtualDevData_t                                         
*                                                                      
*DESCRIPTION                                                            
*   This structure contains all required parameters for Adding new Device 
*   in the stack. Eg. vitual DV1, Virtual DV2.
*/
typedef struct
{          
    /** Device Object Structure within Device */
    devObject_t  m_stDevObject;    

    /** Object Supported by Device */
    supportedObj_t m_stSupportedObjects;
    
    /** Device Process Queue containing request for device. */
    processInfo_t* m_pstDeviceProcessQueue[MAX_PACKET];

    /** Index of processing data from m_pstDeviceProcessQueue*/
    int32_t m_i32QProcessIndex;

    /** Index of fill data in m_pstDeviceProcessQueue*/
    int32_t m_i32QFillIndex;

    /** Semaphore handle for Device thread */
    //Sem_H m_hDeviceSemaphore;
    
    /** Handle for Device thread */
    Thread_H m_hDeviceThread;

    /** Handle for Device Notification (COV + Event_Notification) thread */
    Thread_H m_hNotificationThread;

    /** Handle for Device COV Cancellation thread */
    Thread_H m_hCOVCancellationThread;

    /** The COV Cancellation Semaphore. */
    Sem_H m_hCOVCancelSemaphore;

    /** Handle for Device Management thread for DCC & Backup and Restore */
    Thread_H m_hDvManagementThread;

    /** Thread ID of the (COV + Event_Notification) thread */
    Thread_ID m_dwNotificationThreadID;

	/** Semaphore handle for Device callback notification thread */
    Sem_H m_hCallbackSemaphore;
    
    /** Handle for Device callback notification thread */
    Thread_H m_hCallbackNotifyThread;

    /** Thread exit variables */
    bool m_bCovCancelThreadExit;
    bool m_bDvMgmtThreadExit;
	bool m_bDvCallbackThreadExit;

	/* flag to enable socket communication based on BR state */
	bool m_bEnableScktComm;

    /** Stores the Source Address of the device */
    uint8_t m_u8DvSADR[BACNET_DADR_LEN];

    /** Stores the Source Network Number of the device */
    uint16_t m_u16SNET;

    /** Store the Device Communication Life Time Value */
    uint32_t m_u32DevCommLifeTime;

	/** Store the backup & failure timeout values */
    uint16_t m_u16BackupFailureTime;
    uint16_t m_u16BackupPreparationTime;
    uint16_t m_u16RestorePreparationTime;
    uint16_t m_u16RestoreCompletionTime;

	/** To store date_time difference between sys date_time & 
		time_sync date_time */
	int32_t	m_i32Sec_Diff;
	int32_t m_i32Days_Diff;

    /** To store the Time Change value for Trending service */
    Float_t m_fTimeChange;

	/** Store the Device Communication Status */
    BACNET_COMMUNICATION_STATE m_eDevCommStatus;

	/** Store the Reinit Device Status */
    BACNET_REINITIALIZED_STATE m_eReInitDvStatus;

    /** Store the type of Character Encoding for device */
    uint8_t m_u8CharEncoding;

    /** Password for Dcc & ReInitialize Dv services */
    BACnetCharStr_t m_stDvPassword;

	/** stores address of remote device for B&R functionality */
	BACnetAddress_t m_stRmtDevAddr;

}virtualDevData_t;


/** structure for callback notification queue data */
typedef struct CallBackNotifyData
{
	/** Stores the address from where the request is received */
    BACnetAddress_t             m_stRmtDevAddr;
	/** */
	BACnetTimeStamp_t			m_stTimeStamp;
	/** States the priority level of the message NORMAL/URGENT/CRITICAL/LIFE_SAFETY */
    BACNET_MESSAGE_PRIORITY     m_ePriority;
	/* service type */
	BACNET_SERVICES_SUPPORTED	m_eServiceType;
	/* service paramters */
	void						*m_pvServiceData;	
	/* next node */
	struct CallBackNotifyData	*pstNext;
}CallBackNotifyData_t;

/** structure for callback notification queue */
typedef struct CallbackNotifyQueue
{
	/* node count */
	uint32_t				m_u32QueCount;	
	/* Max count */
	uint32_t				m_u32MaxCount;
	/* first node */
	CallBackNotifyData_t	*m_pstFirstNode;
	/* Last node */
	CallBackNotifyData_t	*m_pstLastNode;
}CallbackNotifyQueue_t;

/* global structure for device callback notification queue */
extern CallbackNotifyQueue_t g_stDvCallbackNotifyQ;
/* mutex handle for callback notification queue */
extern Mutex_H  g_hCallbackMtxLock;


/**
*                                                                         
*Name - status_t                                         
*                                                                      
*DESCRIPTION                                                            
*   Device Look-Up table contianing Device Registration entries. For each device *   request this table gets serached to check if device is present in the system.
*/
typedef struct BACnet_Device_Struct
{
    /** DADR if Device. This field must be unique for each device.*/
    int32_t     m_i32DADR;

    /** Device Instance Id. This field must be unique for each device.*/
    uint32_t    m_u32DeviceInstace;

    /* Host Device Structure pointer contianing Device object information */
    virtualDevData_t *m_pstDeviceStruct;

    /* Pointer to Next BACnet Device */
    struct BACnet_Device_Struct   *pstNextDevice;
}BACnet_Device_Struct_t;

extern BACnet_Device_Struct_t gstHostDevice;
extern BACnet_Device_Struct_t *gpstRemoteDevice;
/* Global variable for device Address Binding */
extern Pr_ListOfBACnetAddrBinding_t g_stDevAddBinding;
//extern Mutex_H g_hDynDevAddrBindMtx;
extern osMutexId_t g_hDynDevAddrBindMtxHandle;

BACNET_ERROR_CODE Dv_Read_Prop(BACNET_CONF_DATA *pRpData, 
                                virtualDevData_t *pVirtualDev);                        

/**
*
* DESCRIPTION                                                                          
* Function to write device object property for specified device.    
*
* @param pWpData	 [in] write property data.
* @param pVirtualDev [in] virtual device data.
* @param pvData		 [in] property value to be written.
* @param ePermission [in] propperty access type.
*
* @return [out] true if success else false.
*
*/
bool Dv_Write_Prop(BACNET_CONF_DATA *pWpData,
    virtualDevData_t *pVirtualDev,
    void *pvData, 
	PROP_ACCESS_TYPE ePermission);

/** Device Process Thread */
void DvProcessThread_Task(void);

/** Function to execute the service request parser as per service choice. */
void Execute_Service_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pProcQIndex,
	bool *pbEncodeEnable,
	bool *pbServiceSupport,
	BACNET_SERVICES_SUPPORTED eServiceType,
	uint8_t u8ThreadNo);

/** Function to trigger transmit thread for sending service response */
void Dv_Send_Response(bool *pbServiceSupport,
	bool *pbEncodeEnable,
	processInfo_t *pProcQIndex,
	virtualDevData_t *pstVirtualDev,
	BACNET_SERVICES_SUPPORTED eServiceType);

void Init_Device_Properties
                (virtualDevData_t * pstDeviceStruct,
                uint32_t ui32Device_Id,
                bool bIsReInitCall);

void Device_Objects_Property_List
            (BACNET_OBJECT_TYPE eObjectType,
            special_property_list_t *pstPropertyList);

BACNET_RETURN_TYPE Create_Object(BACNET_OBJECT_TYPE eObjectType,
                                 virtualDevData_t *pVirtualDev,
                   uint32_t  ui32ObjId, const char *pui8ObjName); 

BACNET_RETURN_TYPE Remove_Object(BACNET_OBJECT_TYPE eObjectType,
                   virtualDevData_t *pVirtualDev,
                   uint32_t  ui32ObjId);

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_AE_EN_B)
/* Intitaites COV data processsing or Event Notification data processing. */
void DvNotificationThread_Task(void);
#endif

void DvManagementTimer(void);

/* free the malloced properties of all objects */
void Delete_Object_Properties(BACNET_OBJECT_TYPE eObjectType,
                  void *pvObjectAddr);

/*
 *   This function updates the object list property of device object
 */
void Update_Device_Object_List(virtualDevData_t *pVirtualDev,
    BACNET_OBJECT_TYPE eOjectType,
    uint32_t  ui32ObjId);

/*
 *   This function updates the Device Address Binding structure with self
 *   address.
 */
void Update_Device_Address_Binding(virtualDevData_t *pstDeviceStruct);

BACNET_RETURN_TYPE Clear_Device_Address_Binding(uint32_t u32DeviceID, BACnetAddress_t *pstDeviceAdd,
                                  virtualDevData_t *pstVirtualDev);

/** function to update the value of database revision property */
void Update_Database_Revision_Value(virtualDevData_t *pstVirtualDev);


#ifdef INITIATE_SERVICE_ENABLED
/*
 *   This function initiates the required services at the start of exe & when
 *   IP Address is changed.
 */
void Initiate_Services(int32_t i32DevId);
#endif

void* Sort_Object_List(BACNET_OBJECT_TYPE eObjectType,
                         uint32_t  ui32ObjId,
                         void   **pvObject,
                         void *pvNewObject);

void *Traverse_Object_List(BACNET_OBJECT_TYPE  eObjectType,
                                  uint32_t  ui32ObjId, virtualDevData_t *pVirtualDev,
                                  void **pPreElement);

/* This function is called to clear objects under the device 
by scanning device object list */
BACNET_RETURN_TYPE ClearObjectList(void *pstDeviceStruct);

/* This API Removes the device by releasing device resources */
void ClearDevice(void *pstDeviceStruct);

/** Api closes all threads, mutex, semaphores, etc for the device */
BACNET_RETURN_TYPE CloseDeviceResources(void *pstDeviceStruct);

///** Device Management thread api for DCC & Backup - Restore services. */
void DvManagementThread_Task(void);

///** Device callback notification thread api to send callback notifications
//to application after service request execution */
//Thread_API DvCallbackNotifyThread(LPvoid lpThreadParameter);

/** Function to add node in callback notification queue */
BACNET_ERROR_CODE CbNotifyQue_AddNode(
	BACnetAddress_t	*pstRmtDevAddr,
	BACnetTimeStamp_t *pstTimeStamp,
	BACNET_MESSAGE_PRIORITY	ePriority,
	BACNET_SERVICES_SUPPORTED eServiceType,
	void *pvServiceData,
	virtualDevData_t *pstVirtualDev,
	BACNET_ERROR_CLASS *peErrorClass);

/**
*
* DESCRIPTION
* This function is used to find address of node using BACnet address from 
* static or dynamic address binding list.
*
* @param pstDestBACnetAddr	[in] destination device BACnet address e.g IP address.
* @return					[out] pointer of found node.
*
*/
BACnetAddrBinding_t	*Find_Dev_Using_Address(BACnetAddress_t *pstDestBACnetAddr);

/**
*
* DESCRIPTION
* This function is used to find the device using BACnet address in dynamic
* device address binding list.
*
* @param pstDeviceAdd	[in]  destination device BACnet address e.g IP address.
* @return				[out] node pointer.
*
*/
BACnetAddrBinding_t *DB_Find_Dev_Using_Addr(BACnetAddress_t *pstDeviceAdd);



#endif /* DEVICE_H */
