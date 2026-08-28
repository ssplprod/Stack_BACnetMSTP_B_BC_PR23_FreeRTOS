/**********************************************************************************
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
*   SoftDEL Systems Ltd.						india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*	pduServiceStructure.c
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale, M.Venu, Pratham Murkute
*                                                                         
*   DESCRIPTION                                                            
*	Defines data-types used by various queues, linked-lists, services, etc
*   in the stack.
*
**********************************************************************************/

#ifndef PDU_SRV_STRUCT_H
#define PDU_SRV_STRUCT_H

/** header file */
#include "bacDELDef.h"
#include "bacDELStackConfig.h"
#include "osalFreeRTOS.h"
/**
*
* DESCRIPTION
* Enumerations for process queue states.
*
*/
typedef enum
{
    /** in use either decoding or executing */
    PROC_CONTINUE = -1, 
    /** in use encoding response */
    PROC_DONE = 1,
    /** not in use */
    PROC_INIT = 0
}DDQ_STATE;

/**
*
* DESCRIPTION
* Structure to save data of services like RP, RPM, WP, WPM, etc.
* This structure is mainly used in B-side for executing service request.
*
*/
typedef struct BACnet_Confirm_Service_Data 
{
	/* object type */
    BACNET_OBJECT_TYPE  eObjectType;
	/* object instance */
    uint32_t            u32ObjectInstance;
	/* property id */
    BACNET_PROPERTY_ID  eObjectProperty;
	/* array index */
    uint32_t            u32ArrayIndex;
	/* data-type of property */
    BACNET_DATA_TYPE    eData_Type;
	/* pointer to property value as per data-type for RP */
	/* pointer to property value i.e. BACNET_PROPERTY_VALUE for WP */
    void                *pvReadPropValue;
	/* length iof buffer */
    int32_t             i32ApplicationDataLen;
	/* pointer to buffer containing encoded property value */
    uint8_t             *pu8PropValueBuffer;
	/* priority value for WP, use BACNET_NO_PRIORITY if no priority */
    uint8_t             u8Priority;
	/* used for RPM, WPM services */
    uint8_t             u8FillObjId;
	/* indicates if array index is present or absent */
    bool                bArrIndxPresent;
	/* indicates if this request is from application or BMS n/w */
	bool				bIsApplicationReq;
	/* execution status of service, success or error */
    bool                bErrorStatus;
	/* error class and error code in case of errro */
    BACNET_ERROR_CLASS  eErrorClass;
    BACNET_ERROR_CODE   eErrorCode;
	/* to store object base address */
	void				*pvObjectAddr;
	/* to store property offset address */
	ulong32_t			ulPropOffsetAddr;	
}BACNET_CONF_DATA;

/**
*
* DESCRIPTION
* Structure to create linked-list of BACNET_CONF_DATA structure.
* This structure is mainly used in B-side for executing service request.
*
*/
typedef struct BACnet_Multiple_Data
{
	/* service data */
    BACNET_CONF_DATA m_stServiceData;
	/* pointer to next node in list */
    struct BACnet_Multiple_Data *m_pstMDNext;
}Bacnet_Multiple_Data_t;

/**
*
* DESCRIPTION
* Network layer data.
*
*/
typedef struct bacnet_npdu_data
{
	/* n/w layer message priority */
    BACNET_MESSAGE_PRIORITY     priority;
    /* optional network layer message type */
    BACNET_NETWORK_MESSAGE_TYPE network_message_type;
	/* destination type */
	DESTINATION_TYPE			eDestinationType;
	/* protocol version */
    uint8_t     protocol_version;
	/* hop count */
    uint8_t     hopcount;
	/* reject reason for message */
    uint8_t     u8NwMsgRejectReason;
	/* indicates if data expects reply or not */
    bool        data_expecting_reply;
	/* indicates if n/w layer or app layer message */
	/* true if n/w layer msg */
    bool        network_layer_message;
	/* indicates that DNET, DADR, Hop Count, etc are present */
    bool        m_bDestPresent;
	/* optional vendor id if n/w message type is > 0x80 */
    uint16_t    vendor_id;
	/* to save network number */
	uint16_t	u16RtrNetNumber; 
}Bacnet_Npdu_Data_t;

/**
*
* DESCRIPTION
* Application layer data.
*
*/
typedef struct bacnet_apdu_data 
{
	/* max segments value that client can receive */
    int32_t		m_i32MaxSegs;
	/* max apdu length accepted by client */
    int32_t		m_i32MaxResp;
	/* indicates if message is segmented */
    bool		m_bSegmentedMessage;
	/* indicates more-follows value in case of segmented message */
    bool		m_bMoreFollows;
	/* indicates if segmented responses is accepted by client */
    bool		m_bSegmentedResponseAccepted;
	/* indicates if abort if from client or server */
	bool		m_bServer;
	/* indicates NAK if segmented message */
    bool		m_bNAK;
	/* invoke id of request */
    uint8_t		m_u8InvokeId;
	/* sequence number if segmented message */
    uint8_t		m_u8SequenceNumber;
	/* proposed window size if segmented message */
    uint8_t		m_u8ProposedWindowNumber;
	/* service type as per confirmed of un-confirmed pdu type */
    uint8_t		m_u8ServiceChoice;
	/* error class */
    uint32_t	m_u32ErrorClass;
	/* error code, abort-error code or reject-error code */
    uint32_t	m_u32ErrorCode;
	/* first failed element number */
	uint32_t	m_u32FirstFailedElementNo;
	/* pointer to service data */
    void		*m_pvServiceRequestData;
}bacnetAPDUData_t;


#ifdef SEGMENTATION_SUPPORTED
/**
*
* DESCRIPTION
* Application layer segmentation data.
*
*/
typedef struct bacnet_DstInfo
{
	/* segmentation support */
	BACNET_SEGMENTATION		m_eSegmentationSupport;
	/* max apdu length */
	uint16_t				m_u16MaxAPDULenAccepted;	
	/* indicates if destination accepts segmented message */
	/* true if destination accepts segmented message */
	bool					bDstSegSupport;
}bacnet_DstInfo_t;
#endif

/**
*
* DESCRIPTION
* Structure to save received message data.
* This structure contains data-link, NPDU and APDU data.
*
*/
typedef struct bacnetRequestData
{
    /* bvlc function type */
    BACNET_BVLC_FUNCTION	m_eBVLCFunctionType;
	/* pdu type */
    BACNET_PDU_TYPE			m_ePDUType;
	/* service type */
	BACNET_SERVICES_SUPPORTED m_eServiceSupported;

	/* mstp frame */
	MSTP_FRAME_TYPE		m_eFrameType;
	/* counter for reply postpone mechanism */
	uint16_t			m_u16ReplyPostponeCounter;
	/* indicates if token is required for MSTP data transmission */
	bool				m_bIsTokenRequired;

	/* device retry count or token id for vd request thread */
    int32_t				m_i32DevRetryCnt;
	/* device apdu timeout counter */
    int32_t				m_i32DevTimeout;
	/* clear initiate Q timeout counter */
    int32_t				m_i32ClearIQTimer;
	/* remote device address */
    BACnetAddress_t		m_stRmDvAddr;
	/* pure-ip or virtual device address */
    BACnetAddress_t		m_stIUTAddr;
	/* network layer data */
    Bacnet_Npdu_Data_t	m_stNPDU;
	/* application layer data */
    bacnetAPDUData_t	m_stAPDU;

	#ifdef SEGMENTATION_SUPPORTED
	/* application layer segmentation info */
	bacnet_DstInfo_t	m_stDstInfo;
	#endif
}bacnetRequestData_t;
 
/**
*
* DESCRIPTION
* Enumerations for property support/unsupport.
*
*/
typedef enum PropertyErr
{
    BACNET_PROP_SUCCESS = 0,

    /** Inidcates Property is Special to object */
    BACNET_PROP_SPECIAL,    
    
    /** Inidcates Property Not support for object */
    BACNET_PROP_NOT_SUPPORTED
}BACNET_PROPERTY_ERR;

#endif /* PDU_SRV_STRUCT_H */
