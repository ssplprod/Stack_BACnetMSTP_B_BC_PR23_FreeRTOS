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
*   SoftDEL Systems Ltd.                        india@softdel.com      
*   3rd Floor, Pentagon P4,                     http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - bacDELApi.h
*
*   AUTHORS
*	Harshal Mangale
*
*   DESCRIPTION                                                            
*   Declaration of all exported API's and structure exported by the stack 
*   for various functionalities.
*   Includes A-side interface enums, callback function prototypes, etc.
*
*
*********************************************************************************/

#ifndef APIEXPORT_H
#define APIEXPORT_H

/** Include files */
#include "osalFreeRTOS.h"
#include "bacDELPropertyDef.h"

/****************************************************************************
 **** Stack API Enumerations
 ****************************************************************************/

/* These ENUM values are mapped to Function choice through Console */
typedef enum {
    BACDEL_CONFIG_STACK_INIT = 0,				/* Init stack */
    BACDEL_CONFIG_STACK_DEINIT = 1,				/* DeInit stack & exit */
    BACDEL_CONFIG_STACK_REINIT = 2,				/* Change IP-port, Virtual n/w no, local n/w no, etc */
    BACDEL_CONFIG_ADD_DEVICE = 3,				/* ADD new device */
    BACDEL_CONFIG_DELETE_DEVICE = 4,			/* Detele device */
    BACDEL_CONFIG_REPLICATE_DEVICE = 5,			/* Replicate device */
    BACDEL_CONFIG_ADD_OBJECT = 6,				/* Add object */
    BACDEL_CONFIG_DELETE_OBJECT = 7,			/* Delete object */
    BACDEL_CONFIG_DISCOVER_DEVICE = 8,			/* Discover devices in network */
    BACDEL_CONFIG_PROP_ACCESS_TYPE = 9,			/* Change property access type */
    BACDEL_CONFIG_BBMD_SUPPORT = 10,			/* BBMD support configuration */
    BACDEL_CONFIG_FD = 11,						/* Set device as Foreign device (FD) */
    BACDEL_CONFIG_COVU_FREQUENCY = 12,			/* Unsolicited COV frequency configuration */
    BACDEL_CONFIG_CHAR_ENCODING = 13,			/* Character encoding configuration */
	BACDEL_CONFIG_CREATE_DIRECTORY = 14,		/* Create directory or folder for BBMD, File object, etc */
    BACDEL_CONFIG_DEVICE_PASSWORD = 15,			/* Device password configuration */
    BACDEL_CONFIG_DEVICE_REINIT = 16,			/* ReInit device properties */
    BACDEL_CONFIG_DEVICE_SEGMENTATION = 17,		/* Device segmentation support configuration */
    BACDEL_CONFIG_DEVICE_APDU_TIMEOUT = 18,		/* Device apdu timeout configuration */
    BACDEL_CONFIG_DEVICE_APDU_RETRY = 19,		/* Device apdu retry configuration */
    BACDEL_CONFIG_DEVICE_SEGMENT_TIMEOUT = 20,	/* Device segment timeout configuration */
    BACDEL_CONFIG_DEVICE_SIMULATION = 21,		/* Device simulation configuration */
    BACDEL_CONFIG_DEVICE_DATETIME = 22,			/* Date and time configuration */
    BACDEL_CONFIG_CHANGE_DEV_ID = 23,			/* Change device id */
	BACDEL_CONFIG_CHANGE_IP_PORT = 24,			/* Change Ip Addr and/or port no */
	BACDEL_CONFIG_CHANGE_VIRTUAL_NW_NO = 25,	/* Change virtual network no */
	BACDEL_CONFIG_CHANGE_LOCAL_NW_NO = 26,		/* Change local network no */
	BACDEL_CONFIG_START_STACK = 27,				/* Stack stack socket communication */
	BACDEL_CONFIG_STOP_STACK = 28,				/* Stop stack socket communication */
	BACDEL_CONFIG_DEBUG_INFO = 29,				/* Debug information configuration */
	BACDEL_CONFIG_VENDOR_DATA = 30,				/* Vendor specific information */

	/* enums till value 40 are reserved for other config options */

    BACDEL_REQ_WHO_IS = 41,						/* Who is */
    BACDEL_REQ_WHO_HAS = 42,					/* Who has */
    BACDEL_REQ_RP  = 43,						/* Read property */
    BACDEL_REQ_WP = 44,							/* Write property */
    BACDEL_REQ_RPM  = 45,						/* Read property multiple */
    BACDEL_REQ_WPM  = 46,						/* Write property multiple */
    BACDEL_REQ_TIME_SYNC  = 47,					/* Time synchronization */
    BACDEL_REQ_UTC_TIME_SYNC = 48,				/* UTC Time synchronization */
    BACDEL_REQ_REINIT_DEVICE  = 49,				/* Reinitialize device */
    BACDEL_REQ_DCC  = 50,						/* Device communication control */
    BACDEL_REQ_COV_SUBSCRIBE  = 51,				/* Subscribe COV */
    BACDEL_REQ_COVU_SUBSCRIBE = 52,				/* Subscribe COV unsolicited */
    BACDEL_REQ_COVP_SUBSCRIBE  = 53,			/* Subscribe COV property */
    BACDEL_REQ_GET_EVENT_INFO = 54,				/* Get event information */
    BACDEL_REQ_GET_ALARM_SUMMARY = 55,			/* Get alarm summary */
    BACDEL_REQ_GET_ENROLLMENT_SUMMARY = 56,		/* Get enrollment summary */
    BACDEL_REQ_ACK_ALARM = 57,					/* Acknowledge alarm */
    BACDEL_REQ_PRIVATE_TRANSFER = 58,			/* Private transfer */
    BACDEL_REQ_TEXT_MESSAGE = 59,				/* text message */
    BACDEL_REQ_CREATE_OBJECT = 60,				/* Create object */
    BACDEL_REQ_DELETE_OBJECT = 61,				/* Delete object */
    BACDEL_REQ_READ_RANGE = 62,					/* Read range */
    BACDEL_REQ_UNUSED = 63,						/* unassigned */
	BACDEL_REQ_ADD_LIST_ELEMENT = 64,			/* Add list element */
	BACDEL_REQ_REMOVE_LIST_ELEMENT = 65,		/* Remove list element */
	BACDEL_REQ_LSO = 66,						/* Life safety operation */
	BACDEL_REQ_ATOMIC_FILE_READ = 67,			/* Atomic file read */
	BACDEL_REQ_ATOMIC_FILE_WRITE = 68,			/* Atomic file write */
	BACDEL_REQ_I_AM = 69,						/* I Am */
	BACDEL_REQ_I_HAVE = 70,						/* I Have */
	BACDEL_REQ_WRITE_GROUP = 100,				/* Write group */

	/* enums till value 70 are reserved for other BACnet A-side requests */
	/* enums from value 100 to 120 are reserved for other BACnet A-side requests */

    BACDEL_BBMD_REQ_READ_BDT = 71,				/* Read BDT */
    BACDEL_BBMD_REQ_READ_FDT = 72,				/* read FDT */
    BACDEL_BBMD_REQ_WRITE_BDT = 73,				/* Write BDT */
    BACDEL_BBMD_REQ_DELETE_FDT_ENTRY = 74,		/* Delete FDT */
    BACDEL_BBMD_REQ_REGISTER_FD = 75,			/* Register as FD */ 
    BACDEL_BBMD_READ_REG_FD_LIST = 76,			/* Read FD list from stack */

	/* enums till value 80 are reserved for other BBMD services and configurations */

    BACDEL_GET_MEMORY_USAGE  = 81,				/* Current stack memory usage */
    BACDEL_GET_OBJECT_SIZE = 82,				/* Object size in bytes */
	BACDEL_CHECK_OBJECT_SUPPORT = 83,			/* Check object support in stack & devices */
    BACDEL_READ_OBJ_PROP = 84,					/* Read properties of virtual devices & objects in stack */
    BACDEL_EVENT_NOTIFICATION_MSG = 85,			/* Event notification message configuration */
    BACDEL_ALGORITHMIC_SUPPORT = 86,			/* Algorithmic events enable or disable */
    BACDEL_TEST_MULTIPLE_RPA = 87,				/* Send multiple RP-A */
    BACDEL_TEST_MULTIPLE_RPMA = 88,				/* Send multiple RPM-A */
    BACDEL_TEST_MULTIPLE_COVA = 89,				/* Send multiple COV-A */

	BACDEL_RESERVED = 255,						/* Max reserved by stack */
	/* enums till value 255 are reserved by stack */
	/* Add new enums here */
	MAX_BACDEL_INITIATE_REQUEST					/* Max value for these enums */

}BACDEL_INITIATE_REQUEST;


/****************************************************************************
 **** BACnet Service Structures
 ****************************************************************************/
extern stBACnetVariables_t gstBACnetVariables;				/** Contain all status bits used for BACnet services*/
/** Structure to save Initiator response */
typedef struct INITIATOR_RESPONSE
{
    BACNET_RETURN_TYPE	m_eRetType;
	BACNET_RETURN_TYPE  m_eErrorCode;
    int32_t				m_i32TokenID;
}initiator_response_t;

/** Structure to access the data of Read Property Request */
typedef struct RP_REQUEST
{
    uint8_t     m_u8ArrayIndexPresent;
	BACNET_OBJECT_TYPE  m_eObjectType;
    uint32_t    m_u32ObjectInstance;
	BACNET_PROPERTY_ID  m_eObjectProperty;
    uint32_t    m_u32ArrayIndex;
}rp_request_t;

/** Structure to access the data of Read Property Response */
typedef struct RP_RESPONSE
{
    uint8_t     m_u8ArrayIndexPresent;
    BACNET_OBJECT_TYPE  m_eObjectType;
    uint32_t    m_u32ObjectInstance;
    BACNET_PROPERTY_ID  m_eObjectProperty;
    uint32_t    m_u32ArrayIndex;
	#ifdef NEW_RP_WP_INTERFACE
	BACNET_DATA_TYPE	  m_eDataType;
	void		*m_pvPropVal;
	#endif

}rp_response_t;

/** Structure to access the data of Write Property Request */
typedef struct WP_REQUEST
{
    uint8_t     m_u8ArrayIndexPresent;
    BACNET_OBJECT_TYPE    m_eObjectType;
    uint32_t    m_u32ObjectInstance;
    BACNET_PROPERTY_ID    m_eObjectProperty;
    uint32_t    m_u32ArrayIndex;
    uint32_t    m_u32Priority;

	#ifdef NEW_RP_WP_INTERFACE
	BACNET_DATA_TYPE	  m_eDataType;
	void		*m_pvPropVal;
	#endif
}wp_request_t;

/** Structure to access the data of Write Property Multiple Request */
typedef struct WPM_REQUEST
{
    uint8_t     m_u8ArrayIndexPresent;
    BACNET_OBJECT_TYPE    m_eObjectType;
    uint32_t    m_u32ObjectInstance;
    BACNET_PROPERTY_ID    m_eObjectProperty;
    uint32_t    m_u32ArrayIndex;
    uint32_t    m_u32Priority;

	#ifdef NEW_RP_WP_INTERFACE
	BACNET_DATA_TYPE	  m_eDataType;
	void		*m_pvPropVal;
	struct      WPM_REQUEST *m_pstWPMNextElem;
	#endif
}wpm_request_t;

/** Structure to access the data of Write Property Multiple Error Response */
typedef struct ERROR_WPM_RESPONSE
{
	BACNET_ERROR_CLASS    m_eErrorClass;
	BACNET_ERROR_CODE     m_eErrorCode;
    BACNET_OBJECT_TYPE	  m_eObjectType;
    uint32_t			  m_u32ObjectInstance;
    BACNET_PROPERTY_ID	  m_eObjectProperty;
	uint32_t			  m_u32ArrayIndex;
	uint8_t				  m_u8ArrayIndexPresent;
}error_wpm_response_t;

/** Structure to access the data of Read Property Multiple Request */
typedef struct RPM_REQUEST
{
    uint8_t     m_u8ArrayIndexPresent;
    BACNET_OBJECT_TYPE    m_eObjectType;
    uint32_t    m_u32ObjectInstance;
    BACNET_PROPERTY_ID    m_eObjectProperty;
    uint32_t    m_u32ArrayIndex;
    struct      RPM_REQUEST *m_pstRPMNextElem;
}rpm_request_t;

/** Structure to access the data of Read Property Multiple Response */
typedef struct RPM_RESPONSE
{
    uint8_t				  m_u8ArrayIndexPresent;
	BACNET_ERROR_CLASS    m_eErrorClass;
	BACNET_ERROR_CODE     m_eErrorCode;
    BACNET_OBJECT_TYPE	  m_eObjectType;
    uint32_t			  m_u32ObjectInstance;
    BACNET_PROPERTY_ID	  m_eObjectProperty;
    uint32_t			  m_u32ArrayIndex;
	#ifdef NEW_RP_WP_INTERFACE
	BACNET_DATA_TYPE	  m_eDataType;
	void		*m_pvPropVal;
	#endif
    struct      RPM_RESPONSE *m_pstRPMNextElem;
}rpm_response_t;

/** Structure to access the data of COV Notification Request */
typedef struct COV_NOTIFICATION_REQUEST
{
	uint8_t     m_u8SendSimpleAck;
	uint8_t     m_u8NotificationType;    
	BACNET_OBJECT_TYPE m_eMonitoredObjectType;
	uint32_t    m_u32MonitoredObjectInstance;
	uint32_t    m_u32DeviceInstance;
	uint32_t    m_u32ProcessIdentifier;
	uint32_t    m_u32Lifetime;
    void*       pvCOVValue;
    property_value_t m_stCOVValue;
	Float_t     m_fCOVIncrement;
	uint32_t    m_u32COVIncrement;
	Double_t    m_dCOVIncrement;
}cov_notification_t;

/** Structure to access the data of COV Subscribe Request */
typedef struct COV_SUBSCRIBE_REQUEST
{
	uint8_t     m_u8Subscribe;
	uint8_t     m_u8NotificationType;
	BACNET_OBJECT_TYPE m_eMonitoredObjectType;
	uint32_t    m_u32MonitoredObjectInstance;
	uint32_t    m_u32ProcessIdentifier;
    uint32_t    m_u32Lifetime;
}cov_subscribe_t;

/** Structure to access the data of I-HAVE service */
typedef struct DOB_I_HAVE
{
    uint32_t    m_u32DeviceInstance;
    BACNET_OBJECT_TYPE m_eObjectType;
    uint32_t    m_u32ObjectInstance;
	BACnetCharStr_t	m_stObjName;
    struct DOB_I_HAVE *pstNext;
}dob_i_have_t;

/** Structure to access the data of WHO-HAS service */
typedef struct DOB_WHO_HAS
{
	uint8_t     m_u8NotificationType;
	struct
	{
		int32_t     m_i32DevRangeLowLimit;
		int32_t     m_i32DevRangeHighLimit;
	}m_stDevRange;
	union
	{
		struct
		{
			BACNET_OBJECT_TYPE m_eObjectType;
			uint32_t    m_u32ObjectInstance;
		}m_stObjectId;
		BACnetCharStr_t	m_stObjName;
	}m_uObject;		
}dob_who_has_t;

/** Structure to access the data of I-AM service */
typedef struct DDB_I_AM
{
	BACNET_SEGMENTATION m_eSegmentationSupported;
	uint16_t    m_u16MaxAPDUlengthAccepted;
	uint32_t    m_u32DeviceInstance;
	uint32_t    m_u32VendorID;
    struct DDB_I_AM *pstNext; 
}ddb_i_am_t;

/** Structure to access the data of WHO-IS service */
typedef struct DDB_WHO_IS
{
    int32_t     m_i32DevRangeLowLimit;
    int32_t     m_i32DevRangeHighLimit;
}ddb_who_is_t;

/** Structure to access the data of DCC_A service request */
typedef struct DCC_REQUEST
{
	uint32_t					m_u32TimeDuration;
	BACNET_COMMUNICATION_STATE  m_eDccState;
	BACnetCharStr_t				m_stPassword;
}dcc_request_t;

#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_SER_AE_EN_B)
/** Structure to access the data of received Event Notification */
typedef struct EVENT_NOTIFICATION
{
	uint8_t     m_u8SendSimpleAck;
	uint8_t     m_u8IsConfirmedNotification;
	uint8_t		m_u8Priority;  
	uint8_t		m_u8AckRequired;  
	BACNET_OBJECT_TYPE  m_eInitiatingObjectType;
    uint32_t    m_u32InitiatingObjectId;
    uint32_t    m_u32InitiatingDeviceId;
    uint32_t    m_u32ProcessIdentifier;
	uint32_t	m_u32NotificationClass;
	BACNET_EVENT_TYPE	m_eEventType;
	BACNET_NOTIFY_TYPE	m_eNotifyType;
	BACNET_EVENT_STATE	m_eFromState;
	BACNET_EVENT_STATE	m_eToState;
	BACnetCharStr_t		m_CharString;
	BACnetTimeStamp_t		m_stTimeStamp;	
	void *					m_pvEventValues;
	#ifdef NEW_RP_WP_INTERFACE
	BACnetNotificationParameters_t	m_stNotifyParameters;
	#endif
}event_notification_t;
#endif /* EN_A || EN_B */

#if (defined BACDEL_SER_AE_AA_A || defined BACDEL_SER_AE_AA_B)
/** Structure to access the data of Acknowledge Alarm request */
typedef struct ACKNOWLEDGE_ALARM
{
	BACNET_OBJECT_TYPE  m_eInitiatingObjectType;
	uint32_t     m_u32InitiatingObjectId;
	uint32_t     m_u32ProcessIdentifier;
	BACNET_EVENT_STATE	m_eAcknowledgedState;
	BACnetCharStr_t		m_AcknowledgementSource;
	BACnetTimeStamp_t	m_stTimeStamp;
	BACnetTimeStamp_t	m_stAcknowledgementTime;
}ack_alarm_request_t;
#endif /* AA_A || AA_B */

#if (defined BACDEL_SER_DM_TS_A || defined BACDEL_SER_DM_TS_B)
/** Structure to access the data of Time Synchronization request */
typedef struct TIME_SYNC
{
	BACnetDate_t    m_stDate;
	BACnetTime_t	m_stTime;		
}time_sync_request_t;
#endif /* TS_A || TS_B */

#if (defined BACDEL_SER_DM_UTC_A || defined BACDEL_SER_DM_UTC_B)
/** structure to save utc offset and dayligh saving status */
typedef struct UTC_DATA
{
	int32_t			m_i32UtcOffset;
	bool			m_bDayLightStatus;
}utc_data_t;

/** Structure to access the data of Time Synchronization request */
typedef struct UTC_TIME_SYNC
{
	BACnetDate_t    m_stDate;
	BACnetTime_t	m_stTime;		
}utc_time_sync_request_t;
#endif /* UTC_A || UTC_B */

#if (defined BACDEL_SER_DM_RD_A || defined BACDEL_SER_DM_RD_B)
/** Structure to access the data of Time Synchronization request */
typedef struct REINIT_DEVICE_REQUEST
{
	BACNET_REINITIALIZED_STATE	m_eDevReinitState;
	BACnetCharStr_t				m_stPassword;
}reinit_device_request_t;
#endif /* RD_A || RD_B */

/** Structure to access the data of COVP Subscribe Request */
typedef struct COVP_SUBSCRIBE_REQUEST
{
	uint8_t     m_u8Subscribe;
	uint8_t     m_u8ArrayIndexPresent;
	uint8_t     m_u8NotificationType;
	BACNET_OBJECT_TYPE m_eMonitoredObjectType;
	BACNET_PROPERTY_ID m_eMonitoredObjectProperty;
	uint32_t    m_u32MonitoredObjectInstance;
	uint32_t    m_u32ProcessIdentifier;
	uint32_t    m_u32Lifetime;
	Float_t     m_fCOVIncrement;
	uint32_t	m_u32PropArryIndex;
	uint32_t    m_u32COVIncrement;
	Double_t    m_dCOVIncrement;
}covp_subscribe_t ;

#if (defined BACDEL_SER_AE_GEI_A || defined BACDEL_SER_AE_GEI_B)
/** Structure to access data of Event Summary */
typedef struct EVENT_SUMMARY
{
	BACNET_OBJECT_TYPE      m_eObjectType;
    uint32_t                m_u32ObjectInstance;
	BACNET_EVENT_STATE		m_eEventState;
	BACNET_NOTIFY_TYPE		m_eNotifyType;
	BACnetBitStr_t			m_stAckTransition;
	BACnetTimeStamp_t		m_stTimeStamp[BACNET_ARRAY_OF_THREE];
	BACnetBitStr_t			m_stEventEnable;
	uint32_t				m_EventPriority[BACNET_ARRAY_OF_THREE];
	struct EVENT_SUMMARY	*m_pstNextSummary;
}event_summary_t;

/** Structure to access data of Event Information response */
typedef struct EVENT_INFO_RESPONCE
{
	event_summary_t	*m_pstEventSummaryList;		
    bool			m_bMoreEvent;
}event_info_response_t;

/** Structure to access data of Event Information request */
typedef struct EVENT_INFO_REQUEST
{
	BACNET_OBJECT_TYPE m_eObjectType;
	uint32_t     m_u32ObjectInstance;
	bool	     m_bLastReciObj;
}event_info_req_t;
#endif /* GEI_A || GEI_B */

/** Service structures for BBDM services. */
/** structure to create linklist of B/IP Addresses. */
typedef struct BIp_Addr
{
    uint8_t          m_u8IpAddrs[MAX_IP_LEN];
    union{
        uint8_t      m_u8PortNo[MAX_PORT_LEN];
        uint16_t     m_u16PortNo;
    }m_uPortNo;
    uint8_t          m_u8BroadcastMask;
    struct BIp_Addr *pstNext;
}BIp_Address_list;

/** Structure to access the data of Read FDT response */
typedef struct Fdt_Data
{
    uint16_t            m_u16TimeToLive;
    uint16_t            m_u16TimeRemaining;
    uint8_t             m_u8IpAddrs[MAX_IP_LEN];
    union{
        uint8_t         m_u8PortNo[MAX_PORT_LEN];
        uint16_t        m_u16PortNo;
    }m_uPortNo;
    struct Fdt_Data    *pstNext;
}Fdt_Data_t;


/** Structure to access the data of Read BDT response */
typedef struct ReadBdt_Res
{
    BIp_Address_list   *m_pstBIpAddress;
    int32_t             m_i32StringLen;
	int8_t             *m_pi8PropValue;
}readBdt_response_t;

/** Structure to access the data of Read FDT response */
typedef struct ReadFdt_Res
{
    Fdt_Data_t         *m_pstFdtData; 
    int32_t             m_i32StringLen;
    int8_t             *m_pi8PropValue;
}readFdt_response_t;


/** Structure to access the data of Write BDT request */
typedef struct WriteBdt_Req
{
    BIp_Address_list    m_stBIpAddress;
}writeBdt_request_t;


/** Structure to access the data of Delete FDT request */
typedef struct DeleteFdt_Req
{
    BIp_Address_list    m_stBIpAddress;
}deleteFdt_request_t;

/** Structure to access the data of Register FD request */
typedef struct RegisterFd_Req
{
    uint16_t            m_u16TimeToLive;
}registerFD_request_t;
#ifdef BACDEL_PR23
/** Structure to access the data of WHO-AM-I service */
typedef struct DDA_WHO_AM_I
{
	uint16_t			m_u16VendorId;
	BACnetCharStr_t     m_stModelName;
	BACnetCharStr_t     m_stSerialNumber;
}dda_who_am_i_t;

/** Structure to access the data of YOU-ARE service */
typedef struct DDA_YOU_ARE
{
	uint16_t			m_u16VendorId;
	BACnetCharStr_t     m_stModelName;
	BACnetCharStr_t     m_stSerialNumber;
	/* Device Identifier flag */
	bool				m_bDeviceIdentifierPresent;
	BACNET_OBJECT_TYPE	m_eDeviceType;
	uint32_t			m_u32DeviceId;
	/* Device MAC Address flag */
	bool				m_bDeviceMACAddPresent;
	BACnetOctetStr_t	m_stDeviceMACAddrs;
}dda_you_are_t;
#endif /* BACDEL_PR23 */
/** Structure union to access the different service requests or 
	responses avaliable */
typedef union SERVICE_CHOICE
{
	/* member of union */
    error_response_t        m_stErrorResponse;  
	abort_response_t		m_stAbortResponse;
	reject_response_t		m_stRejectResponse;
    error_wpm_response_t    m_stWPM_ErrorResponse;
    wpm_request_t           m_stWPM_Request;
    rpm_response_t          m_stRPM_Response;
    rpm_request_t           m_stRPM_Request;
    rp_request_t            m_stRP_Request;
    rp_response_t           m_stRP_Response;
    wp_request_t            m_stWP_Request;
    dob_i_have_t            m_stI_HAVE_Request;
    dob_who_has_t           m_stWHO_HAS_Request;
    ddb_i_am_t              m_stI_AM_Request;
    ddb_who_is_t            m_stWHO_IS_Request;
	dcc_request_t			m_stDCC_Request;
    cov_notification_t      m_stCOV_Notification;
    cov_subscribe_t         m_stCOV_Subscribe;
#ifdef BACDEL_SER_AE_EN_B
	event_notification_t	m_stEvent_Notification;
#endif

    readBdt_response_t      m_stReadBdt_Response;
    readFdt_response_t      m_stReadFdt_Response;

    writeBdt_request_t      m_stWriteBdt_Request;
    deleteFdt_request_t     m_stDeleteFdt_Request;

    registerFD_request_t    m_stRegisterFD_Request;

	/** Stores the network number for network layer services */
	uint16_t				m_u16RtrNetNumber;
#ifdef BACDEL_PR23
#ifdef BACDEL_SER_DM_DDA_A
	dda_you_are_t			m_stYOU_ARE_Request;
#endif
#endif /* BACDEL_PR23 */
}service_choice_u;

/** Stores the BACnet data from APDU & beyond */
typedef struct APDU_PACKET
{
	/** Flag to indicate error status i.e. whether error is received from server or client */
	/** true if error is received from server */
	bool						m_bErrorFromServer;
    /** States the service which is requested or for which the response is stored */
    BACNET_SERVICES_SUPPORTED	m_eServiceSupport;
	/** PDU type of request */
	BACNET_PDU_TYPE				m_ePduType;
	/** invoke id for confirmed request */
    int32_t						m_i32InvokeID;
	/** service parameters of request */
    service_choice_u			m_stServiceChoice;
}apdu_packet_t;

/** Stores the BACnet data from NPDU & beyond */
typedef struct NPDU_PACKET
{
	/** Specifies if this is network layer message or contains apdu */
	bool						m_bIsNwLayerMsg;
    /** Specifies the transmit type BROADCAST/UNICAST */
	DESTINATION_TYPE			m_eDestinationType;
	/** Specifies network message type */
	BACNET_NETWORK_MESSAGE_TYPE m_eMsgType;
	/** Stores the BACnet data from APDU & beyond */
    apdu_packet_t       		m_stAPDUData;
}npdu_packet_t;

/** Structure to access the data of BACnet IP packet.
 *  This structure has been exposed to external applications 
 */
typedef struct BACNET_IP_ARGUMENTS
{
    /** Reference number for external application to retrieve response */
	int32_t                     m_i32TokenID;
    /** Reference number for application to generate A-Side request and retrieve response */
	uint32_t                    m_u32RequestID;
	/* Store device-id */
	uint32_t					m_u32DeviceID;
    /** Determines the packet type UNICAST/BROADCAST/DISTRIBUTED/FORWARD */
	BACNET_BVLC_FUNCTION        m_eBvlcFunctionType;
	/** BVLC result value for BBMD services */
	BACNET_BVLC_RESULT			m_eBvlcResult;
	/** States the priority level of the message NORMAL/URGENT/CRITICAL/LIFE_SAFETY */
    BACNET_MESSAGE_PRIORITY     m_ePriority;
	/** Stores the address from where the response is received */
    BACnetAddress_t             m_stDestBACnetAddr;
	/** Stores the BACnet data from NPDU & beyond */
    npdu_packet_t               m_stNPDUData;
}bacnetip_arguments_t;

/* BACnet structure of thread priorities of BACnet Stack. This structure should be 
    used by API that needs to change priority of BACnet Stack threads */
typedef struct BACnet_Thread_Priority
{
    BACNET_THREADS  eThread;
    void*           pvHandle;
    int8_t          i8Priority;    
    struct BACnet_Thread_Priority    *pstNext;
}BACnet_Thread_Priority_t;

/** Structure to access network number is request */
typedef struct NwNoIsReq
{
	/* specifies the network layer type */
	uint8_t 	m_u8NWType;
	/* network number */
	uint16_t 	m_u16RtrNetNumber;
}network_no_is_req_t;

/****************************************************************************
 **** Function Pointers
 ****************************************************************************/

/** Callback function pointer definition for HW interfaces */
typedef int8_t (* HardWare_Interface_t)
(
    uint32_t u32DevId,
    BACNET_OBJECT_TYPE eObjType,
    uint32_t u32ObjId,
    BACNET_PROPERTY_ID ePropId,
    void* pvGetVal
);
/** Application interface callback function pointer defination for A-side service callbacks 
 i.e. service request execution callbacks */
typedef BACNET_RETURN_TYPE (* App_Callback_Interface_t)
(
	bacnetip_arguments_t *pstServiceArgs, 
	uint8_t u8Reason
);

/** Application interface callback function pointer defination for Auto-Responses 
 i.e. service request execution callbacks */
typedef BACNET_RETURN_TYPE (* App_AutoResp_Interface_t)
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorResponse,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
);

/** Application interface callback function pointer defination for 
internal stack callbacks */
typedef void (* App_Interface_t)
(
	uint32_t u32DeviceId,
	uint32_t u32ObjectId,
	BACNET_OBJECT_TYPE eObjType,
	BACNET_CALLBACK_TYPE eCallbackType,
	BACNET_CALLBACK_REASON eCallbackReason,
	uint32_t u32DataValue,
	void *pvDataValue
);

/** Application intreface callback function pointer defination 
for transmit data over MSTP serial bus */
typedef BACNET_RETURN_TYPE (* App_MstpTx_Interface_t)
(
	uint8_t *pu8TxData,	
	uint16_t *pu16SilentTimer,
	uint16_t u16DataLen
);

typedef void (* App_RTC_Interface_t)
(
	BACnetDate_t *pstLocalDate, 
	BACnetTime_t *pstLocalTime
);
/****************************************************************************
 **** Exported API Prototypes. 
 ****************************************************************************/

/**
*
* DECRIPTION
* API to get the address of specific device or to get the 
* device ID of a specific device address.
*
* @param stDeviceID	   [in/out] Pointer to device id value.
* @param pstDeviceAdd  [in/out] Pointer to address structure.
* @param bSearchIPAdd  [in]		Search for address or device id.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Get_Device_ID_Or_Address(uint32_t *pu32DeviceID, 
	BACnetAddress_t *pstDeviceAdd, bool bSearchIPAdd);


/**
*
* DECRIPTION
* API to read the current device address binding list.
*
* @param pstAddrBindingList [in/out] Double pointer to save device
*									 address binding list base node.
* @return [out] void/nothing.
*
* Note: 
* Call clear device address binding list api to free the list memory allocated in this api.
*
*/
 void
	BACDEL_Get_Dynamic_Device_Address_Binding(BACnetAddrBinding_t **pstAddrBindingList);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to clear the device address binding list initailly returned in the call to get 
* device address binding list api.
*    
* @param pstAddrBindingList [in] Double pointer containing device address binding 
*								 list base node.
* @return [out] void/nothing.
*
* Note: 
* Pass the pointer that was returned in call to get device address binding list api.
*
*/
 void
	BACDEL_Free_Dynamic_Device_Address_Binding(BACnetAddrBinding_t **pstAddrBindingList);


/**
*                                                                    
* DESCRIPTION                                                                          
* This API is used to delete entry(s) from dynamic device address binding list. 
* If input device ID is negative, complete list will be deleted.
* 
* @param i32DeviceID	[in]	Device id of the requested device.
* @param pstDevAddress	[in]	Device address of the requested device.
*
* @return BACNET_RETURN_TYPE	[out]	success or suitable error code.
*                                                                      
*/
 BACNET_RETURN_TYPE
	BACDEL_Delete_Dynamic_Device_Address_Binding(
	int32_t i32DeviceID,
	BACnetAddress_t *pstDevAddress);


/**
*                                                                         
* DESCRIPTION                                                                          
* APi to get address of router from its network no.
*    
* @param u16NetworkNo [in] Network No whose address is required.
* @param pstDeviceAdd [out] Pointer to save router address.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Get_Router_Address_From_Network_No(
	uint16_t u16NetworkNo, 
	BACnetAddress_t *pstDeviceAdd);


/**
*
* DESCRIPTION
* API to be called by application in response to Write Property callback auto-response.
* Application should call this api on write success in hardware.
* As soon as thie function gets called, stack will update its database for the given object property.
*
* @param u16DevId [in] Device Id contaning object whose proeprty need to be write.
* @param u8ObjTyep [in] Type of object whose value is expected to be write.
* @param u16ObjId [in] The object Id for which property needs to be write.
* @param u16PropId [in] The property which needs to be write.
* @param pvGetVal [in] The Pointer containing value to be write.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
* @return BACDEL_HW_INTERFACE_SUCCESS on success.
*
*/
 BACNET_RETURN_TYPE BACDEL_HW_Update_Val(
	uint32_t u32DevId, BACNET_OBJECT_TYPE eObjType,
	uint32_t u32ObjId, BACNET_PROPERTY_ID ePropId,
	int8_t i8ArrayIndex, void *pvGetVal);


/**
*
* DESCRIPTION
* API to register callback functions for auto-responses from stack.
*
* @param eAppChoice [in] Type of Function to be called.
* @param fpFunction [in] The Callback function pointer of applications
*
* @return [out] void/nothing.
*
*/
 void BACDEL_App_CallBack_Register(
    BACAPP_CALLBACK_FUN_CHOICE eAppChoice,
    App_AutoResp_Interface_t fpFunction);

/**
*
* DESCRIPTION
* API to register callback functions for rtc from .
*
* @param eAppChoice [in] Type of Function to be called.
* @param fpFunction [in] The Callback function pointer of applications
*
* @return [out] void/nothing.
*
*/
 void BACDEL_App_RTC_CallBack_Register(
    BACNET_RTC_CALLBACK_CHOICE eAppChoice,
    App_RTC_Interface_t fpFunction);
	
 
/**
*
* DESCRIPTION
* API to Un-register callback functions for auto-responses from stack.
*
* @param eAppChoice [in] Type of Function to be called.
* @return [out] void/nothing.
*
*/
 void BACDEL_App_CallBack_UnRegister(
    BACAPP_CALLBACK_FUN_CHOICE eAppChoice);


 /**
 *
 * DESCRIPTION
 * API to register callback functions for auto-responses from stack.
 *
 * @param eAppChoice [in] Type of Function to be called.
 * @param fpFunction [in] The Callback function pointer of applications
 *
 * @return [out] void/nothing.
 *
 */
  void BACDEL_App_RTC_CallBack_Register(
     BACNET_RTC_CALLBACK_CHOICE eAppChoice,
     App_RTC_Interface_t fpFunction);


  BACNET_RETURN_TYPE BACDEL_Select_DataLink_Layer(
		  BACNET_STACK_SELECTION eDataLinkChoice);
  /**
  *
  * DESCRIPTION
  * write BACnet Configuration Data
  *
  * @param pui8buffer [in] data buffer pointer.
  * @param ui32StartAddress [in] starting address
  * @param ui16TotalLength [in] Data length
  *
  * @return [out] void/nothing.
  *
  */
  void BACDEL_Write_Flash_Config_Data(uint8_t *pui8buffer,
  	  uint32_t ui32StartAddress,uint16_t ui16TotalLength);

  /**
  *
  * DESCRIPTION
  * write BACnet Configuration Data
  *
  * @param offset [in] address.
  * @param nbytes [in] Number of bytes wants to read
  * @param buffer [in] To store read data
  *
  * @return ui16TotalLength[out] Number of bytes.
  *
  */
  uint16_t BACDEL_Read_Flash_Config_Data(uint32_t offset,uint16_t nbytes,
		  uint8_t *buffer);

#ifdef DEBUG_PRINTF
/**
*
* DESCRIPTION
* API to register debug / debug log callback functions of application.
*
* @param u8DebugLogLevel [in] Debug log level.
* @param fpFunction		 [in] Debug callback function.
* @param fpLogData		 [in] Debug log callback function.
*
* @return [out] void/nothing.
*
*/
 void BACDEL_App_Debug_Callback_Register(  //<@>
 uint8_t u8DebugLogLevel,
 Debug_Log_Messages_t fpFunction,
 Debug_Log_Data_t fpLogData);
#endif

/**
*
* DESCRIPTION
* API to un-register debug / debug log callback function.
*
* @param  [in] void/nothing.
* @return [out] void/nothing.
*
*/
 void BACDEL_App_Debug_Callback_UnRegister(void);


/**
*
* DESCRIPTION
* API to register internal callback functions from stack.
* @param fpFunction [in] The Callback function pointer of applications
*
* @return [out] void/nothing.
*
*/
 void BACDEL_AppLayer_CallBack_Register(
   App_Interface_t fpFunction);


/**
*
* DESCRIPTION
* API to Un-register internal callback functions from stack.
*
* @return [out] void/nothing.
*
*/
 void BACDEL_AppLayer_CallBack_UnRegister(void);

/**                                                                         
*
* DESCRIPTION                                                           
* API to add pure ip device or virtual devices in BACnet stack.
*
* @param u8DADR		  [in] New Device DADR (required only for VD's).
* @param u32Device_Id [in] Device Id assigned to new device.
* @param u16SNET	  [in] 2-octet source network number (0 for pure IP device)
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Add_Device(uint32_t u32DADR, uint32_t u32Device_Id, uint16_t u16SNET);


/**                                                                         
*
* DESCRIPTION                                                                          
* API to delete pure ip device or virtual devices present in stack.
*
* @param i32Device_Id [in] Device to be deleted.
*						   - if i32Device_Id == -1, then free all devices.
*						   - else free only i32Device_Id.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
* Note: 1st host device (pure ip device) can't be deleted with this api.
*
*/
 BACNET_RETURN_TYPE BACDEL_Delete_Device(int32_t i32Device_Id);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to add BACnet objects (other than device) in the created devices.
*    
* @param u32DeviceId	[in] Device in which object is to be added.
* @param eObjType		[in] The type of object to be added.
* @param u32ObjId		[in] The Object Identifier for new object.
* @param pu8ObjName		[in] Pointer to New Object Name.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Add_Object(uint32_t u32DeviceId, BACNET_OBJECT_TYPE eObjType, 
	uint32_t u32ObjId, const char *pu8ObjName);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to delete BACnet objects (other than device) in the created devices.
*    
* @param u32DeviceId	[in] Device in which object is to be deleted.
* @param eObjType		[in] The type of object to be deleted.
* @param u32ObjId		[in] Id of object to be deleted.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Delete_Object(uint32_t u32DeviceId, BACNET_OBJECT_TYPE eObjType, uint32_t u32ObjId);


/**
*                                                                         
* DESCRIPTION                                                              
* API will add/initialize the given property of given object with the 
* value passed to this api.
*    
* @param u32DevId	  [in] Device Id of which object property needs to be initialized.
* @param u32ObjectID  [in] Object Id (Not usefule for device property)
* @param eObjType	  [in] Type of Object of which property needs to be initialized.
* @param eDevProp	  [in] Property of object which is to be initialized.
* @param i8ArrayIndex [in] Array index for the property value.
* @param p_value	  [in] Initialization value.
* @param ePermission  [in] Access permission for property.
* @param pvPropAdd		[out] Returns property value address.
* @param pvPropDataType [out] Returns property tag type.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Add_Property(
	uint32_t u32DevId, uint32_t u32ObjectID,
	BACNET_OBJECT_TYPE eObjType,
    BACNET_PROPERTY_ID eDevProp,
    int8_t  i8ArrayIndex,
    void *p_value,
    PROP_ACCESS_TYPE ePermission,
    void **pvPropAdd,
    BACNET_APPLICATION_TAG *pvPropDataType);


/**
*
* DISCREPTION
* API to initialize the BACnet-IP stack on given network interface.
* It initializes-creates all the threads, mutex, handles, etc.
*
* @param  pu8IPAddrs		 [in] ip address of the interface to run the stack.
* @param u16UdpPortNum		 [in] udp port no to run the stack.
* @param u16LocalNetworkNo	 [in] local network no for stack.
* @param u16VirtualNetworkNo [in] virtual network no for stack.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Stack_Init(
	uint8_t *pu8IPAddrs, uint16_t u16UdpPortNum, 
	uint16_t u16LocalNetworkNo, 
	uint16_t u16VirtualNetworkNo);


/**
*
* DISCREPTION
* Api to change IP address and/or UDP port no at runtime.
* The follow up process after IP change will be 
*   1) Binding new IP address with the socket.
*   2) Sending an I-AM response.
*   3) Sending WHO-IS request.
*   4) Resending COV Subscribe Request to registered devices. 
*
* @param pu8IpAddr		[in] ip address of the interface to run the stack.
* @param u16UdpPortNum	[in] udp port no to run the stack.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Change_IpAddress_PortNo(
	uint16_t u16UdpPortNum, uint8_t *pu8IpAddr);



/**
*
* DISCREPTION
* API to de-initialize the BACnet stack.
* It delets all the threads, mutex, handles, etc and frees all dynamically 
* allocated memories.
*
* @param [in] void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Stack_Deinit(void);


/**
*
* DISCREPTION
* API to start or enable the stack's network communication.
* Datalink as IP : socket communication will be enabled.
* Datalink as MSTP : serial communication will be enabled.
*
* @param [in] void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Enable_Network_Communication(void);


/**
*
* DISCREPTION
* API to stop or disable the stack's network communication.
* Datalink as IP : socket communication will be disabled.
* Datalink as MSTP : serial communication will be disabled.
*
* @param [in] void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Disable_Network_Communication(void);


/**
*
* DISCREPTION
* API to converts input Error code to its string representation.
*
* @param eRetType	[in] Input Error code.
* @param pi8DstStr [out] String for error code.
*
* @return [out] void/nothing.
*
*/
 void
    BACDEL_Convert_ErrorCode_To_String(BACNET_RETURN_TYPE eRetType, 
	uint8_t *pu8DstBuffer, uint32_t u32DstBufferSize);


/**
*
* DISCREPTION
* API to delete all host/virtual devices and remote devices created in stack.    
*
* @param [in] void/nothing.
* @return [out] void/nothing.
*
*/
 void BACDEL_Clear_Device_Objects(void);


#ifdef INITIATE_SERVICE_ENABLED

/**
*
* DESCRIPTION
* API to initiate bacnet service request.
*
* @param pu8SrcDADR		[in] Device SADR that is initiating request. 
*							 Send NULL to use default ip device.
* @param pstServiceArgs [in] Service request parameters.
* @param FunctionPtr	[in] Callback function pointer.
*
* @return initiator_response_t [out] Contains token id and response codes.
* @return BACDEL_INITIATOR_SUCCESS on success.
*
*/
 initiator_response_t BACDEL_Generate_Aside_Request(
	uint8_t *pu8SrcDADR, 
	bacnetip_arguments_t *pstServiceArgs,
	App_Callback_Interface_t fpFunctionPtr);


/**
*
* DESCRIPTION
* API to initiate Unicast service request with Device ID.
*
* @param pu8SrcDADR		[in] Device SADR that is initiating request. 
*							 Send NULL to use default ip device.
* @param pstServiceArgs [in] Service request parameters.
* @u32Device_Id         [in] Destination Device ID.
* @param FunctionPtr	[in] Callback function pointer.
*
* @return initiator_response_t [out] Contains token id and response codes.
* @return BACDEL_INITIATOR_SUCCESS on success.
*
*/
 initiator_response_t BACDEL_Generate_Aside_Request_Using_DeviceID(
	uint8_t *pu8SrcDADR, 
	bacnetip_arguments_t *pstServiceArgs,
	App_Callback_Interface_t fpFunctionPtr,
	uint32_t u32Device_Id);


/**
*
* DESCRIPTION
* API to get the responses of initiated A-side bacnet service requests.
* The responses are available only if callback was not registered while
* initiating a confirmed service request.
* Responses can be obtained within predefined timeout (60 seconds is default).
*
* @param u32TokenId [in] Token id to search for response.
* @param fpFunctionPtr [out] Callback function pointer to send response to application.
*
* @return BACNET_A_SIDE_RESPONSE [out] Type of response code.
*
*/
 BACNET_A_SIDE_RESPONSE BACDEL_Get_A_Side_Val(
	uint32_t u32TokenId, 
	App_Callback_Interface_t fpFunctionPtr);

#endif /* INITIATE_SERVICE_ENABLED */


/**
*
* DESCRIPTION
* API to be called by Application to initialize bacnetip_arguments_t structure 
* before initiating any bacnet service request.
*
* @param pstBACnetArgs	  [in/out] Pointer to structure to fill default parameters.
* @param i8ServiceSupport [in] Type of service to be requested.
*
* @return void/nothing.
*
*/
 void
	BACDEL_Set_Default_Parameters(bacnetip_arguments_t *pstBACnetArgs, 
			int8_t i8ServiceSupport);


/**
*                                                                         
* DESCRIPTION                                                                          
* API allows application to relatively change priority of threads created
* within the stack.
*    
* @param u32DeviceId  [in] Id of device created in stack.
* @param eThread	  [in] Name/type of the thread.
* @param u8Priority   [in] Priority of thread.
* @param bSetPriority [in] Set priority if true else read current priority.
* @param pePriority   [in/out] Priority to be set or get.
* 
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
* @return void/nothing.
*
*/
 BACNET_RETURN_TYPE BACDEL_Set_Get_Thread_Priority(
	uint32_t u32DeviceId, BACNET_THREADS eThread, 
	BACNET_THREAD_PRIORITY *pePriority, uint8_t u8Priority,
	bool bSetPriority);

/**
*                                                                         
* DESCRIPTION                                                                          
* API will provide the heap memory consumption of the BACnet stack.
*    
* @param u32PeakUsage [out] Peak memory usage of BACnet Stack.
* @param u32CurrUsage [out] Current memory usage of BACnet Stack.
*
* @return void/nothing.
*
*/
 void BACDEL_Get_Memory_Usage(ulong32_t *pul32PeakUsage, ulong32_t *pul32CurrUsage);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to read property value of any object of any host or remote device 
* created within the stack.
* The property value is returned in the form of a string.
*
* @param bArrayIndexPresent [In] Indicates if array index in present or not.
* @param u32DevId		[In] Device to which object belongs.
* @param u32ObjId		[In] Object to which property belongs.
* @param eObjectType	[In] Type of object.
* @param ePropId		[In] Property that is to be read.
* @param u32ArrayIndex	[In] Array index.
* @param u32DestSize	[in] Size (in bytes) of "pvReadData".
* @param pvReadData		[Out] Buffer to save property value.
*
* @return BACNET_ERROR_CODE [out] success or suitable error code.
* @return ERROR_CODE_OTHER on success.
*
*/
 BACNET_ERROR_CODE BACDEL_Get_Object_Property_String(
	uint32_t u32DevId, BACNET_OBJECT_TYPE eObjectType, 
	uint32_t u32ObjId, BACNET_PROPERTY_ID ePropId,
	uint32_t u32ArrayIndex, void *pvReadData,
	uint32_t u32DestSize, bool bArrayIndexPresent);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to read property value of any object of any device created
* within the stack.
*
* @param u32DevId		[In] Device to which object belongs.
* @param u32ObjId		[In] Object to which property belongs.
* @param eObjType		[In] Type of object.
* @param eDevProp		[In] Property that is to be read.
* @param u32ArrayIndex	[In] Array index.
* @param bArrayIndexPresent [In] Indicates if array index in present or not.
* @param pvReadPropVal	[Out] Double pointer to save property value pointer.
* @param peDataType     [Out] Property data type.
*
* @return MAX_BACNET_ERROR_CODE on success.
*
*/
 BACNET_ERROR_CODE BACDEL_Get_Object_Property(
	uint32_t u32DevId, 
	uint32_t u32ObjId,
	BACNET_OBJECT_TYPE eObjType,
	BACNET_PROPERTY_ID eDevProp,
	uint32_t u32ArrayIndex,
	void **pvReadPropVal,
	bool bArrayIndexPresent,
	BACNET_DATA_TYPE *peDataType);

/**
*                                                                         
* DESCRIPTION                                                              
* API to update the property value of specified object.
* Property value of only one property can be updated using this API.
*    
* @param u32DevId		[in] Device Id.
* @param u32ObjectID	[in] Object Id.
* @param eObjType		[in] Type of Object.
* @param eDevProp		[in] Property ID.
* @param u32ArrayIndex	[in] Array index for the property value.
* @param pvWritePropVal	[in] Pointer to property value.
* @param u8Priority		[in] Priority.
* @param bArrayIndexPresent	[in] Specifies if array index is present or not.
* @param eDataType      [in] Property data type.
*
* @return MAX_BACNET_ERROR_CODE on success.
*
*/
 BACNET_ERROR_CODE BACDEL_Set_Object_Property(
	uint32_t u32DevId, uint32_t u32ObjectID,
	BACNET_OBJECT_TYPE eObjType,
    BACNET_PROPERTY_ID eDevProp,
	uint32_t u32ArrayIndex,
    void *pvWritePropVal,
	uint8_t u8Priority,
	bool bArrayIndexPresent,
	BACNET_DATA_TYPE eDataType);

/**
*                                                                         
* DESCRIPTION                                                              
* API to update the property value of specified object.
* Property value of multiple properties can be updated using this API.
* 
* @param u32DevId               [in]  device identifier.
* @param pstWpmData         	[in]  WPM data pointer.
* @param peObjType				[out] object type.
* @param pePropId				[out] property identifier.
* @param peObjType				[out] object type.
* @param pu8FirstFailElement    [out] first failed element number.
*
* @return MAX_BACNET_ERROR_CODE on success.
*
*/
 BACNET_ERROR_CODE BACDEL_Set_Object_Property_Multiple(
	uint32_t u32DevId,
	wpm_request_t *pstWpmData,
	uint8_t *pu8FirstFailElement,
	BACNET_OBJECT_TYPE *peObjType,
	BACNET_PROPERTY_ID *pePropId,
	uint32_t *pu32ObjId);

/**
*                                                                         
* DESCRIPTION                                                                          
* API to free allocated memory in BACDEL_Get_Object_Property() API.
*
* @param pvData		[In] Property value pointer.
* @param eDataType  [In] Property data type.
*
* @return void.
*
*/
void BACDEL_Free_Object_Property(
	BACNET_DATA_TYPE eDataType,
	void **pvData);


/**
*
* DISCREPTION
* API to initialize commandable property "Present Value".
*
* @param u32DevId	 [in] Device identifier to which object belongs.
* @param i32ObjectId [in] Object Identifier to which property belongs.
* @param eObjectType [in] Object type for given object identifier.
* @param bClearSimulationVal [in] Flag used in case of simulation.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Init_Commandable_Property(
	uint32_t u32DevId, int32_t i32ObjectId, 
	BACNET_OBJECT_TYPE eObjectType, 
	bool bClearSimulationVal);

/**
*                                                                         
* DESCRIPTION                                                                          
* API for enabling BBMD functionality.
* If register for FD was previously enabled, then it will be disabled.
*    
* @param [in] void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Enable_BBMD(void);


/**
*                                                                         
* DESCRIPTION                                                                          
* API is used for disabling BBMD functionality.
*    
* @param [in] void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Disable_BBMD(void);


/**
*
* DESCRIPTION
* API to initiate BBMD service requests.
*
* @param  pstServiceArgs  [in]  Contains service parameters to initiate request.
* @param  FunctionPtr     [in]  Function pointer for callback function.  
*
* @return initiator_response_t [out] Contains token id and response codes.
* @return BACDEL_INITIATOR_SUCCESS on success.
*
*/
initiator_response_t BACDEL_Generate_BBMD_Request(
	bacnetip_arguments_t *pstServiceArgs,
    App_Callback_Interface_t fpFunctionPtr);

/**
*                                                                         
* DESCRIPTION                                                                          
* API is used for enabling FD functionality.
* This api will send register FD request to the destination.
*    
* @param  pu8BBMDAddr  [in] pointer to destination ip address bytes.
* @param  u16BBMDPort  [in] destination port no.
* @param  u16BBMDTimeToLive  [in] lifetime for FD registration.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE
	BACDEL_Enable_FD(uint8_t *pu8BBMDAddr, uint16_t u16BBMDPort, uint16_t u16BBMDTimeToLive);

#ifdef BBMD_ENABLED


/**
*                                                                         
* DESCRIPTION                                                                          
* API is used for disabling FD functionality.
* This api will send delete FDT entry request to the destination in 
* register FD list.
*    
* @param [in] void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Disable_FD(void);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to read the register FD list, i.e list of BBMD's to whom register FD was sent.
*    
* @param pstRegFdData [in/out] Double pointer to save register FD list base node.
* @return [out] void/nothing.
*
* Note: 
* Call clear register FD list api to free the list memory allocated in this api.
*
*/
 void BACDEL_Get_Register_FD_List(Fdt_Data_t **pstRegFdData);


/**
*                                                                         
* DESCRIPTION                                                                          
* API to clear the register FD list initailly returned in the call to get 
* register FD list api.
*    
* @param pstRegFdData [in] Double pointer containing register FD list base node.
* @return [out] void/nothing.
*
* Note: 
* Pass the pointer that was returned in call to get register FD list api.
*
*/
 void BACDEL_Free_Register_FD_List(Fdt_Data_t **pstRegFdData);

#endif /* BBMD_ENABLED */


/**
*                                                                         
* DESCRIPTION                                                                          
* API to get device address from user.
* This api asks for user input from console.
*    
* @param pstServiceArgs [in/out] Structure pointer to save device address.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE
	BACDEL_Get_Device_Address(bacnetip_arguments_t *pstServiceArgs);

#ifdef OPTIONAL_PROPERTY

/**
*
* DECRIPTION
* API to get the IP address of specific device or to get the 
* device ID of a specific IP address.
*
* @param u32DevId  [in] Device to which object belongs.
* @param u32ObjId  [in] Object instance to which property belongs.
* @param eObjType  [in] Type of object.
* @param bSetFlag  [in] Set or get values flag (TRUE = set values).
* @param pfMinPv   [in/out] Min pv value pointer.
* @param pfMaxPv   [in/out]	Max pv value pointer.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Set_Get_Min_Max_PV(uint32_t u32DevId, uint32_t u32ObjId, 
	BACNET_OBJECT_TYPE eObjType, Float_t *pfMinPv, 
	Float_t *pfMaxPv, bool bSetFlag);

#endif /* OPTIONAL_PROPERTY */


/**
*
* DECRIPTION
* API to get the size (in bytes) required to create 1 instance of respective object type.
* This size does not include size of array or list type properties.
*
* @param eObjectType	[in]  type of bacnet object.
* @return int32_t		[out] size in no of bytes.
*
*/
int32_t BACDEL_Get_Sizeof_Object(BACNET_OBJECT_TYPE eObjectType);


#ifdef BACDEL_SER_AE_EN_B

/**
*
* DECRIPTION
* API to set event notification message text for all applicable object types.
* This message is sent in event notification sent by the respective object. 
*
* @param u32DevId  [in] Device to which object belongs.
* @param u32ObjId  [in] Object id to set the message.
* @param eObjType  [in] Type of object.
* @param pvString  [in] Message text.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Set_Event_Notification_Message(uint32_t u32DevId,
	BACNET_OBJECT_TYPE eObjType, uint32_t u32ObjId,
	void *pvString);

#endif /* BACDEL_SER_AE_EN_B */


#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)

/**
*
* DECRIPTION
* API to set or get device date and time.
*
* @param u32DevId  [in] Device id.
* @param pstTime   [in] Time to be set.
* @param pstDate   [in] Date to be set.
* @param bFlag     [in] Set if true else get the datetime.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Set_Get_Device_Local_DateTime(uint32_t u32DevId,
	BACnetTime_t *pstTime, BACnetDate_t *pstDate,
	bool bFlag);

#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */


#ifdef BACDEL_SER_DM_RD_B

/**
*
* DECRIPTION
* Api to reinit device. Reinit device mechanism is vendor specific.
* To use default stack implementation call this api from callback function.
*
* @param u32DeviceId  [in] Device id.
* @return [out] void/nothing.
*
*/
 void BACDEL_ReInitDevice(uint32_t u32DeviceId, BACNET_RESTART_REASON eRestartReason);

#endif /* BACDEL_SER_DM_RD_B */

/**
*
* DECRIPTION
* API to get the property application tag type as per the object type.
*
* @param eObjectType	  [in]  type of bacnet object.
* @param eObjectProperty  [in]  property id.
*
* @return BACNET_APPLICATION_TAG [out] type of application tag.
*
*/
BACNET_APPLICATION_TAG
	BACDEL_Get_Property_Tag(BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID eObjectProperty);

/**
*
* DECRIPTION
* API to get the property data-type as per the object type.
*
* @param eObjectType	   [in]  type of bacnet object.
* @param eObjectProperty   [in]  property id.
*
* @return BACNET_DATA_TYPE [out] type of data-type.
*
*/
BACNET_DATA_TYPE
	BACDEL_Get_Property_DataType(BACNET_OBJECT_TYPE eObjectType,
	BACNET_PROPERTY_ID eObjectProperty);


#ifdef SUPPORT_MULTIPLE_DEVICE

/**
*
* DESCRIPTION
* Api to set the local network no used by the stack.
* This is used by pure IP device for routing of packets within stack.
*
* @param u16NetworkNo [in] two octet local network no.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Set_Local_Network_No(uint16_t u16NetworkNo);


/**
*
* DESCRIPTION
* Api to set the virtual network no used by the stack.
* This is used by pure IP device for routing of packets to virtual devices within stack.
*
* @param u16NetworkNo [in] two octet virtual network no.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Set_Virtual_Network_No(uint16_t u16NetworkNo);

#endif /* SUPPORT_MULTIPLE_DEVICE */


/**
*
* DESCRIPTION
* Api to set or get the read-write access of object properties.
*
* @param u32DevID [in] Device Id.
* @param u32ObjId [in] Object Id.
* @param eObjType [in] Type of object.
* @param ePropId  [in] Property Id.
* @param eAccessType  [in] Property AccessType to be set.
* @param bReadOrWrite [in] Write access type if true else read & return access type.
*
* @return NOT_SUPPORTED on error & read or written value on success. 
*
*/
PROP_ACCESS_TYPE
	BACDEL_Set_Get_Property_AccessType(uint32_t u32DevID, uint32_t u32ObjId,
    BACNET_OBJECT_TYPE eObjectType, BACNET_PROPERTY_ID ePropId,
    PROP_ACCESS_TYPE eAccessType, bool bReadOrWrite);


/**
*
* DESCRIPTION
* Api to set character encoding to be used by devices in stack.
*
* @param i32DevID		[in] Device Id.
* @param eCharEncoding  [in] Type of character encoding.
* @param u16CodePage	[in] Code page for IBM_DBCS encoding.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code. 
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Set_Character_Encoding(int32_t i32DevID, 
	BACNET_CHARACTER_STRING_ENCODING eCharEncoding,
	uint16_t u16CodePage);


/**
*
* DESCRIPTION
* API to set Device password common for DCC & Reinit Dv services.
*
* @param u32DevID	 [in] Device Id.
* @param pu8Password [in] Password for device.
*
* @returns [out] void/nothing.
*
*/
void BACDEL_Set_Device_Password(int32_t i32DevID, uint8_t *pu8Password);


/**                                                                         
*
* DESCRIPTION                                                           
* API to set device object properties properties to default values.
*
* @param u32Device_Id [in] Device Id.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Init_Device_Properties(uint32_t u32DevId, bool bClearObjectList);



/**
*
* DESCRIPTION                                                           
* API to change the device id and / or DADR of any host device at runtime.
*
* @param u32OldDevId [in] current device id.
* @param u32NewDevId [in] new device id.
* @param u32NewDADR  [in] new device DADR value.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Change_Device_Id(uint32_t u32OldDevId, uint32_t u32NewDevId, uint32_t u32NewDADR);


/**                                                                         
*
* DESCRIPTION                                                           
* API to create directory/folder required to save files containing data for
* BBMD service and file object.
*
* @param  [in]	void/nothing.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Create_Directory(void);


/**
*
* DESCRIPTION
* API to Check wheteher Object type is supported or not.
* If " i32DevId " is -1, check the support in stack else check in 
* corresponding device.
*
* @param i32DevId       [in]  indicates device id.
* @param eObjectType    [in]  type of object.
*
* @return bool [out] true if supported else false.
*
*/
bool BACDEL_Check_Object_Support(
    int32_t i32DevId, 
    BACNET_OBJECT_TYPE eObjectType);


/**
*
* DESCRIPTION
* API to set device APDU timeout property value.
*
* @param u32DevID		[in] Device Id.
* @param u32ApduTimeout [in] APDU timeout value for given device.
* @param bAllDevice		[in] Change timeout of all virtual devices if true.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE
	BACDEL_Set_Device_Apdu_Timeout(uint32_t u32DevID, uint32_t u32ApduTimeout, bool bAllDevice);


/**
*
* DESCRIPTION
* API to set number of APDU retries property value of device object.
*
* @param u32DevID		[in] Device Id.
* @param u32ApduRetries [in] APDU retries value for given device.
* @param bAllDevice		[in] Change retries of all virtual devices if true.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE
	BACDEL_Set_Device_Apdu_Retries(uint32_t u32DevID, uint32_t u32ApduRetries, bool bAllDevice);


/**                                                                         
*
* DESCRIPTION                                                           
* API to Get Vendor Specific Parameters for BACnet stack.
*
* @param u16VendorId		  [in] Vendor ID.
* @param au8VendorName		  [in] Vendor Name.
* @param au8ModelName		  [in] Model Name.
* @param au8FirmwareRev		  [in] Firmware revision.
* @param au8ApplSoftwareVer	  [in] Application Software version.
* @param au8Location		  [in] Location.
* @param pu8DeviceDescription [in] Device Description.
* @param pu8ObjDescription	  [in] Object Description.
* @param pu8ProfileName		  [in] Profile Name.
*
* @return void [out] nothing.
*
*/
 void BACDEL_Set_Vendor_Specific_Data(
	uint16_t u16VendorId, uint8_t *pu8VendorName, 
	uint8_t *pu8ModelName, uint8_t *pu8FirmwareRev, 
	uint8_t *pu8ApplSoftwareVer, uint8_t *pu8Location, 
	uint8_t *pu8DeviceDescription, uint8_t *pu8ObjDescription,
	uint8_t *pu8ProfileName, uint8_t *pu8SerialNumber);


#ifdef SEGMENTATION_SUPPORTED

/*
*                                                                         
* DESCRIPTION                                                                          
* API is used to set Actual window size and 
* proposed window size for segmentation.
*    
* @param u8ActWindowSize      [in]   Actual window size
* @param u8PropsdWindowSize   [in]   Proposed window size
*
* @return BACNET_RETURN_TYPE  [out]  success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Set_Segmentation_WindowSize(
	uint8_t u8ActWindowSize, 
	uint8_t u8PropsdWindowSize);

#endif /* SEGMENTATION_SUPPORTED */


/**                                                                         
*
* DESCRIPTION                                                           
* API to set various max limits of bacnet stack parameters.
* If 'bSetSpecifiedLimit' is FALSE.
* - all limits are 1st validated
* - limits are set only if validation is successfull for all limits
* If 'bSetSpecifiedLimit' is TRUE.
* - only limit specified by 'u32LimitNo' is validated. 
* - 1st parameter in 'BACApp_MaxLimits_t' structure is limit no 1.
* - on successful validation, only specified limit is set.
*
* @param pstStackLimit		  [in]  pointer to structure of max limits
* @param peFirstFailedLimit   [in]  specifies 1st failed limit serial number
* @param eLimitType			  [in]  specifies limit no starting from 1.
* @param bSetSpecifiedLimit	  [in]  flag specifies which limit to set.
*
* @return BACNET_RETURN_TYPE  [out] success or suitable error code.
* 
*/
 BACNET_RETURN_TYPE BACDEL_Set_Max_Limits(
	BACApp_MaxLimits_t *pstStackLimit,
	BACNET_MAXLIMIT_PARAMETERS *peFirstFailedLimit,
	BACNET_MAXLIMIT_PARAMETERS eLimitType,
	bool bSetSpecifiedLimit);


/**                                                                         
*
* DESCRIPTION                                                           
* API to read values of various max limits parameters set in stack.
*
* @param pstStackLimit		  [in]  pointer to structure of max limits
* @return BACNET_RETURN_TYPE  [out] success or suitable error code.
* 
*/
 void BACDEL_Get_Max_Limits(
	BACApp_MaxLimits_t *pstStackLimit);


/** 
* 
* DESCRIPTION
* API to set various stack configuration parameters.
* If 'bSetSpecifiedLimit' is FALSE.
* - all parameters are 1st validated
* - parameters are set only if validation is successfull for all
* If 'bSetSpecifiedLimit' is TRUE.
* - only parameters specified by 'u32LimitNo' is validated. 
* - 1st parameter in 'BACApp_MaxLimits_t' structure is parameters no 1.
* - on successful validation, only specified parameters is set.
*
* @param pstStackConfig		  [in]  pointer to structure of config parameters
* @param peFirstFailedParam   [in]  specifies 1st failed parameters serial number
* @param eParameterType		  [in]  specifies parameters type
* @param bSetSpecifiedLimit	  [in]  flag specifies which limit to set
*
* @return BACNET_RETURN_TYPE  [out] success or suitable error code.
* 
*/
 BACNET_RETURN_TYPE BACDEL_Set_Stack_Configuration_Parameters(
	BACApp_StackConfig_t *pstStackConfig,
	BACNET_STACK_CONFIG_PARAMETERS *peFirstFailedParam,
	BACNET_STACK_CONFIG_PARAMETERS eParameterType,
	bool bSetSpecifiedLimit);


/** 
*
* DESCRIPTION
* API to read values of various stack configuration parameters set in stack.
*
* @param pstStackLimit		  [in]  pointer to structure of config parameters
* @return BACNET_RETURN_TYPE  [out] success or suitable error code.
* 
*/
 void BACDEL_Get_Stack_Configuration_Parameters(
	BACApp_StackConfig_t *pstStackConfig);


/**
*
* DECRIPTION
* API to set or get the current configuration of property RP callback 
* as per the object type.
*
* @param eObjectType	[in]  object type.
* @param ePropId		[in]  property id.
* @param bReadOrWrite   [in]  set if true else get value.
* @param peCbType   [in/out]  callback configuration value.
*
* @return [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Set_Get_Property_Rp_Callback(
	BACNET_OBJECT_TYPE eObjectType, 
	BACNET_PROPERTY_ID ePropId, 
	BACNET_CALLBACK_CONFIG_TYPE *peCbType, 
	bool bReadOrWrite);


/**
*
* DECRIPTION
* API to set or get the current configuration of property WP callback 
* as per the object type.
*
* @param eObjectType	[in]  object type.
* @param ePropId		[in]  property id.
* @param bReadOrWrite   [in]  set if true else get value.
* @param peCbType   [in/out]  callback configuration value.
*
* @return [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Set_Get_Property_Wp_Callback(
	BACNET_OBJECT_TYPE eObjectType, 
	BACNET_PROPERTY_ID ePropId, 
	BACNET_CALLBACK_CONFIG_TYPE *peCbType,
	bool bReadOrWrite);


/**
*
* DECRIPTION
* API to increment database revision property by one.
*
* @param u32DevId	[in]  device identifier.
* @return MAX_BACNET_ERROR_CODE on success.
*
*/
 BACNET_ERROR_CODE
	BACDEL_Update_DataBase_Revision(
	uint32_t u32DevId);




/****************************************************************************
 **** Service APIs
 ****************************************************************************/

#ifdef BACDEL_SER_DM_DDB_A
/**                                                                         
*
* DESCRIPTION                                                           
* API to send Who Is service request.
*
* @param u32SrcDevId			 [in]  Source Device ID. 
* @param u32DestDevId			 [in]  Destination Device ID. 
* @param pstDestinationAdd		 [in]  Destination Device IP address.
* @param bDestTypeFlag			 [in]  Flag to check request generated by
*									   Device ID or Device address. 
* @param eDestType				 [in]  Type of destination.
* @param bIsNwLayerMsg           [in]  Send either who_is or who_
*                                      is_router_to_network.
* @param u32NetworkNo            [in]  Network no for who_is_router_to_network.
* @param pstWhoIs                [in]  Structure pointer to access the data of
*                                      Who-Is service request
*
* @param pi32TokenId			 [out] Token ID will be returned
* @return BACNET_RETURN_TYPE     [out] success or suitable error code.
*
*/
 BACNET_RETURN_TYPE BACDEL_Send_Who_Is(
 	uint32_t u32SrcDevId,
 	uint32_t u32DestDevId,
 	BACnetAddress_t *pstDestinationAdd,
 	bool bDestTypeFlag,
 	DESTINATION_TYPE eDestType,
 	bool bIsNwLayerMsg,
 	uint16_t u16NetworkNo,
 	ddb_who_is_t *pstWhoIsData,
 	int32_t *pi32TokenId,
 	uint32_t u32RequestId);

#endif /* BACDEL_SER_DM_DDB_A */

#ifdef BACDEL_SER_DM_DDB_B
/**                                                                         
*
* DESCRIPTION                                                           
* API to send I-Am service request.
*
* @param u32SrcDevId			 [in]  Source Device ID. 
* @param u32DestDevId			 [in]  Destination Device ID. 
* @param pstDestinationAdd		 [in]  Destination Device IP address.
* @param bDestTypeFlag			 [in]  Flag to check request generated by
*									   Device ID or Device address. 
* @param eDestType				 [in]  Type of destination.
* @param bIsNwLayerMsg           [in]  Send either i_am or i_am_router_to_network.
* @param pstIAm        		     [in]  Structure pointer to access the data of 
*                                      I-Am service request
*
* @param pi32TokenId			 [out] Token ID will be returned
* @return BACNET_RETURN_TYPE     [out] success or suitable error code.
* 
*/
 BACNET_RETURN_TYPE BACDEL_Send_I_Am(
	uint32_t u32SrcDevId,						
	uint32_t u32DestDevId,						
	BACnetAddress_t *pstDestinationAdd,		
	bool bDestTypeFlag,							
	DESTINATION_TYPE eDestType,
	bool bIsNwLayerMsg,
	int32_t *pi32TokenId);

#endif /* BACDEL_SER_DM_DDB_B */

#ifdef BACDEL_SER_DM_DOB_B
/**                                                                         
*
* DESCRIPTION                                                           
* API to send I HAVE request.
*
* @param u32SrcDevId			 [in]  Source Device ID. 
* @param u32DestDevId			 [in]  Destination Device ID. 
* @param pstDestinationAdd		 [in]  Destination Device IP address.
* @param bDestTypeFlag			 [in]  Flag to check request generated by
*									   Device ID or Device address. 
* @param eDestType				 [in]  Type of destination.
* @param pstIHaveData			 [in]  Structure pointer to access the data of 
*                                      I HAVE service request
*
* @param pi32TokenId			 [out] Token ID will be returned
* @return BACNET_RETURN_TYPE     [out] success or suitable error code.
* 
*/
 BACNET_RETURN_TYPE BACDEL_Send_I_Have(
	uint32_t u32SrcDevId,						
	uint32_t u32DestDevId,						
	BACnetAddress_t *pstDestinationAdd,		
	bool bDestTypeFlag,							
	DESTINATION_TYPE eDestType,
	dob_i_have_t *pstIHaveData,
	int32_t *pi32TokenId);

#endif /* BACDEL_SER_DM_DOB_B */
#ifdef BACDEL_PR23
#ifdef BACDEL_SER_DM_DDA_A
/**
*
* DESCRIPTION
* API to send You are service request.
*
* @param u32SrcDevId			 [in]  Source Device ID.
* @param u32DestDevId			 [in]  Destination Device ID.
* @param pstDestinationAdd		 [in]  Destination Device IP address.
* @param bDestTypeFlag			 [in]  Flag to check request generated by
*									   Device ID or Device address.
* @param eDestType				 [in]  Type of destination.
* @param pstYouAreData           [in]  Structure pointer to access the data of
*                                      You-Are service request
* @param u32RequestId	         [in]  Request ID generated by application.
*
* @param pi32TokenId			 [out] Token ID will be returned
* @return BACNET_RETURN_TYPE     [out] success or suitable error code.
*
*/
BACNET_STACK_EXPORT BACNET_RETURN_TYPE BACDEL_Send_You_Are(
	uint32_t u32SrcDevId,
	uint32_t u32DestDevId,
	BACnetAddress_t *pstDestinationAdd,
	bool bDestTypeFlag,
	DESTINATION_TYPE eDestType,
	dda_you_are_t *pstYouAreData,
	int32_t *pi32TokenId,
	uint32_t u32RequestId);

#endif /* BACDEL_SER_DM_DDA_A */
#endif /* BACDEL_PR23 */
#endif /* APIEXPORT_H */
