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
*   SoftDEL Systems Ltd.						india@softdel.com      
*   3rd Floor, Pentagon P4,						http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - bacDELStackConfig.h
*
*   AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION                                                            
*   Defines all the data-types used in stack, macros, stack configuration 
*   settings, stack limits, generic property value structure, etc.
*
*
*********************************************************************************/


#ifndef SOFTDEL_BACNET_STACK_CONFIG_H
#define SOFTDEL_BACNET_STACK_CONFIG_H

/** Include files */
#include <stddef.h>
#include "bacDELDef.h"
#include "stdint.h"
#include "stdbool.h"


/****************************************************************************
 **** Standard Definations. 
 ****************************************************************************/

/* Used for defining boolean type declarations */
typedef unsigned char   bool_t;     /* 1 byte  0 to 255 */
typedef unsigned char   uint8_t;    /* 1 byte  0 to 255 */
typedef unsigned short  uint16_t;   /* 2 bytes 0 to 65535 */
typedef signed char     int8_t;     /* 1 byte -127 to 127 */
typedef signed short    int16_t;    /* 2 bytes -32767 to 32767 */
typedef long            ilong32_t;  /* singed long declarations */
typedef unsigned long   ulong32_t;  /* unsinged long declarations */
typedef float           Float_t;    /* float declarations */
typedef double          Double_t;   /* double declarations */
typedef int             enum_t;     /* enumerated declarations */
typedef long 		Int64_t;    /* 8 bytes –9223372036854775808 to 9223372036854775807 */	//<$$>
typedef unsigned long  Uint64_t; /* 8 bytes 0 to 18446744073709551615 */	//<$$>


#if !defined(__cplusplus)

#if !defined(__GNUC__)
/* _Bool builtin type is included in GCC */
/* ISO C Standard: 5.2.5 An object declared as 
   type _Bool is large enough to store 
   the values 0 and 1. */
/* We choose 8 bit to match C++ */
/* It must also promote to integer */
//typedef int8_t _Bool;
#endif

/* ISO C Standard: 7.16 Boolean type */
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#endif



/** Null definations for BACnet stack */
#define BACDEL_NULL				0
#define BN_DEVICE_MAX_MASTER 	127
#define BACNET_ARRAY_OF_FOUR	4
/**  This structure defines elements required for state machine to run */
typedef struct
{
   uint8_t		m_u8Binaries;			/** Total Binaries	*/
   uint8_t		m_u8NextStation;		/** Next station	*/
   uint8_t		m_u8PollStation;		/** Polling station	*/
   uint8_t		m_u8MaxMaster;			/** Maximum master	*/
   uint8_t		m_u8TokenCount;			/** Token count		*/
   uint8_t		m_u8ThisStation;		/** Node address	*/
   uint8_t		m_u8ReceivedDestination;/** Received destination address	*/
   uint8_t		m_u8ReceivedSource;		/** Received source					*/
   uint8_t		m_u8ReceivedFrameType;	/** Received frame type				*/
   uint8_t		m_u8SourceAddress;		/**	Source address					*/
   uint8_t		m_u8RetryCount;			/** Number of retry counts			*/
   uint8_t		m_u8EventCount;			/** Number of Event counts			*/
   uint16_t		m_u8FrameCount;			/** Number of frame counts			*/
   uint16_t		m_u8TnoTokens;			/** Transmit no. of tokens			*/
   uint16_t		m_u8TreplyTimeout;		/** Transmit reply time out			*/
   uint16_t		m_u8Tframeabort;		/** Transmit frame abort			*/
   uint16_t		m_u8Tsilence;			/** SoftTimer count					*/
   uint16_t		m_u8SubTsilence;		/** SoftTimer count					*/
   uint16_t		m_u8MaxInfoFrames;		/** Information frames				*/
   uint8_t		m_u8RS485DisableTime;	/**	RS485 Disable time				*/
   uint16_t 	m_u16TUsageTimeOut;
   uint16_t		m_u16NRetryToken;
   uint8_t		m_u8ApduReTryCount;
   uint8_t		m_u8TTurnaround;		/** Turnaround (x1ms)				*/
}stBACnetVariables_t;


/********************************************************************
 ******* Bit Masks.
 *********************************************************************/
#define BIT0            (0x01)
#define BIT1            (0x02)
#define BIT2            (0x04)
#define BIT3            (0x08)
#define BIT4            (0x10)
#define BIT5            (0x20)
#define BIT6            (0x40)
#define BIT7            (0x80)
#define BIT8          (0x0100)
#define BIT9          (0x0200)
#define BIT10         (0x0400)
#define BIT11         (0x0800)
#define BIT12         (0x1000)
#define BIT13         (0x2000)
#define BIT14         (0x4000)
#define BIT15         (0x8000)
#define BIT16       (0x010000UL)
#define BIT17       (0x020000UL)
#define BIT18       (0x040000UL)
#define BIT19       (0x080000UL)
#define BIT20       (0x100000UL)
#define BIT21       (0x200000UL)
#define BIT22       (0x400000UL)
#define BIT23       (0x800000UL)
#define BIT24     (0x01000000UL)
#define BIT25     (0x02000000UL)
#define BIT26     (0x04000000UL)
#define BIT27     (0x08000000UL)
#define BIT28     (0x10000000UL)
#define BIT29     (0x20000000UL)
#define BIT30     (0x40000000UL)
#define BIT31     (0x80000000UL)


/****************************************************************************
 **** Macro Definations. 
 ****************************************************************************/

/************ Initiate side macros ***********/
/** Max length for MSTP Tx (request/response data requiring token) queue */
#define MAX_MSTP_TX_QUE_LENGTH	16  //vks//5       //<@> 255
/** Max Queue Length */
#define MAX_PACKET	7
/** Initiate queue length */
#define MAX_INITIATE_INFOQ_LENGTH		19 //Recommened Value 20//vks //SYC PP

#define DEFAULT_INITIATE_INFOQ_LENGTH	MAX_INITIATE_INFOQ_LENGTH
/** BBMD Initiate queue length */
#define MAX_BBMD_INITIATE_Q_LENGTH		5
#define DEFAULT_BBMD_INITIATE_Q_LENGTH	MAX_BBMD_INITIATE_Q_LENGTH
/** VD Initiate queue length */
#define MAX_VD_INITIATE_Q_LENGTH		5// 1000 //<$$>
#define DEFAULT_VD_INITIATE_Q_LENGTH	MAX_VD_INITIATE_Q_LENGTH
/** A-side COV Subscripition request allowed */ 
#define MAX_COV_SUBSCRIPTION			5
#define DEFAULT_COV_SUBSCRIPTION		MAX_COV_SUBSCRIPTION
/** Invalid Invoke Id no. */
#define INVALID_INVOKE_ID			256
/** Invalid Token Id no. */
#define INVALID_TOKEN_ID			INT32_MAX

/** Initiate queue deletion count */
#define MAX_INITIATE_Q_DELETION_COUNT		(DEFAULT_INITIATE_INFOQ_LENGTH / 4)
#define DEFAULT_INITIATE_Q_DELETION_COUNT	MAX_INITIATE_Q_DELETION_COUNT
#define MIN_INITIATE_Q_DELETION_COUNT		100

/** A-Side Timeouts */
#define DEFAULT_APDU_TIMEOUT	10	// seconds
#define TIME_CLEAR_QUEUE		60	// seconds
#define CLEAR_INITIATED_REQ_TIMEOUT	10000	// milliseconds

/************ Max objects macro ************/
/** Max objects that can be created in single device */
#define MAX_OBJECTS				20  //500 <@>
#define DEFAULT_MAX_OBJECTS		MAX_OBJECTS

/************ Device related configurations **************/
/** Max device address binding that can be saved */
#define MAX_ADDRESS_BINDING_ENTRIES         5
#define DEFAULT_ADDRESS_BINDING_ENTRIES     MAX_ADDRESS_BINDING_ENTRIES

/************ Stack Queue length macro ************/
/** Queue length for Rx, DvProcess, Process & Tx threads */

/** Min Queue Size */
#define MIN_PACKET	0
/** Queue length for Dv callback notification thread */
#define MAX_CALLBACK_NOTIFY_Q_LENGTH  		5
#define DEFAULT_CALLBACK_NOTIFY_Q_LENGTH	MAX_CALLBACK_NOTIFY_Q_LENGTH

/************* Device related settings ************/
/** Min bytes for bit string type properties */
#define MIN_BITSTRING_BYTES 1
/** Max bytes for bit string type properties */
#define MAX_BITSTRING_BYTES 8
/** Max bits for bit string type properties */
#define MAX_BITSTRING_BITS (MAX_BITSTRING_BYTES * 8)
/** Max bytes required to save each bit into string */
#define MAX_BIT_SUPPORT (MAX_BITSTRING_BITS * 2)

/** Max bytes for octet string type properties */
#define MAX_OCTET_STRING_BYTES (16)
/** Max character string bytes for stack */
#define MAX_CHARACTER_STRING_BYTES 32                // 255 <$$>
/** Max character string bytes for MSV object */
#define MSV_MAX_CHARACTER_STRING_BYTES 255               
/** Max character string bytes for stack */
#define MAX_SUPPORTED_CHRSTR_LEN (MAX_CHARACTER_STRING_BYTES + 1)
/** Max character string bytes for device object */
#define MAX_SUPPORTED_CHRSTR_DEV_LEN  32 

/** Max APDU length */
//Common for IP and MSTP
#define MAX_APDU_LENGTH_ACCEPTED (480)
/** Max input string length accepted by stack from application */
#define MAX_APPLN_INPUT_STRLEN		1000
#define MAX_APPLN_PROP_VAL_STR		120

/** Max password length supported by device for Dcc & ReInit device services */
#define MAX_PASSWORD_CHAR_BYTES     20
#define MAX_SUPPORTED_PASSWORD_LEN (MAX_PASSWORD_CHAR_BYTES + 1)


/*************** Macros for Character Encoding ***************/
/** Code page for encoding character string in IBM_MS_DBCS format */
#define CODE_PAGE_DEFAULT   (0x0352) // 0x0352 = 850

/*************** Macros for Process Queue *******************/
/** macros for process queue timeouts */
#define PROCESS_Q_TIMEOUT_MAX		10000	// max value in milliseconds
#define PROCESS_Q_TIMEOUT_DEFAULT	PROCESS_Q_TIMEOUT_MAX	// default value in milliseconds

/*************** Macros for Segmentation ***************/
/** Max segments that can be transmitted */
#define MAX_SEGMENTS_TX			4
#define DEFAULT_SEGMENTS_TX 	MAX_SEGMENTS_TX
/** Max segments that can be received */
#define MAX_SEGMENTS_RX			4
#define DEFAULT_SEGMENTS_RX 	MAX_SEGMENTS_RX
/** Max Buffer segments to calculate max bytes for local buffer used to
	save encoded data in APDU encode handler. */
#define MAX_BUFFER_SEGMENTS		4     //<@@>
#define MAX_APDU_BUFFER	(MAX_APDU_LENGTH_ACCEPTED * MAX_BUFFER_SEGMENTS)
/** Proposed window size while sending 1st segment of a message */
#define PROPOSED_WINDOW_SIZE	1
/** Actual window size while sending 1st segment-ack of a message */
#define ACTUAL_WINDOW_SIZE		1
/** Max segment window size */
#define MAX_SEGMENT_WINDOW_SIZE 127

/*************** Macros for BBMD *******************/
/** Maximum entries in BDT / FDT / Register FD list */
#define MAX_BDT_FDT_FD_ENTRIES		5
#define DEFAULT_BDT_FDT_FD_ENTRIES  MAX_BDT_FDT_FD_ENTRIES 
/** macros for BBMD timeouts */
#define MAX_BBMD_INITQ_TIMEOUT		30	// max value in seconds
#define DEFAULT_BBMD_INITQ_TIMEOUT	MAX_BBMD_INITQ_TIMEOUT	// default value in seconds

/*************** Macros for B-side Thread Pool *******************/
/** macros for thread pool thread count */
#define THREADPOOL_COUNT		5
#define THREADPOOL_COUNT_MAX	10
/** macros for thread pool timeouts */
#define THREADPOOL_TIMEOUT_MAX		10000	// max value in milliseconds
#define THREADPOOL_TIMEOUT_DEFAULT	THREADPOOL_TIMEOUT_MAX	// default value in milliseconds


/****************************************************************************
 **** Definations - Macros, Enums & Typedef's. 
 ****************************************************************************/

/* Enum for Status Of Initiate service Queue */
typedef enum 
{
    STATE_IDLE,
    STATE_FILL_REQUEST,
    STATE_REQUEST_SEND_FAILED,
    STATE_AWAIT_RESPONSE,
    STATE_RESPONSE_RECEIVED,
    STATE_SEGMENTED_CONFIRMATION,
    STATE_FIXED,
	STATE_SEND_REQUEST = 253,
    STATE_TIMEOUT = 254,	 
	STATE_ERROR
}INITIATE_SERVICE_STATE;

/* Character string encoding type */
#define ASCII_ENCODING 0

/* Enum for Varible Initialization values 
( Magic Numbers ) */
typedef enum Init_Value
{
    BACNET_NEGATIVE_ONE = -1,
    BACNET_ZERO = 0,
    BACNET_ONE = 1,
    BACNET_TWO = 2,
}BACNET_INIT_VALUE;

/* Enum to check if property is array type */
typedef enum {
    ARRAY_TYPE_0 = 0,   /* Property is not BACNET_ARRAY */
    ARRAY_TYPE_1,       /* Property is BACNET_ARRAY */
    ARRAY_TYPE_2        /* Property has different data type as per Object */
}ARRAY_TYPE_SUPPORTED;

//Newly Added
typedef enum {
	BACNET_STACK_IP = 0,
	BACNET_STACK_MSTP,
	BACNET_STACK_NONE
} BACNET_STACK_SELECTION;

/** This stack implements this version of BACnet */
#define BACNET_PROTOCOL_VERSION	 1
#define BACNET_PROTOCOL_REVISION 19


/*************** BACnet Protocol related macros ***************/
/* Largest BACnet Instance Number */
/* do the MAX as define rather than enumeration for devices
   and compilers that want to use smaller datatype for enum */
#define MAX_BACNET_PROPERTY_ID 4194303
/* Also used as a device instance number wildcard address */
#define BACNET_MAX_INSTANCE	 (0x3FFFFF)
#define BACNET_INSTANCE_BITS 22
/* large BACnet Object Type */
#define BACNET_MAX_OBJECT	(0x3FF)
#define BACNET_MAX_PROPERTY	(0xFFFF)
/* Array index 0=size of array, n=array element n,  MAX=all array elements */
#define BACNET_ARRAY_ALL	(~(uint32_t)0)
/* Array index not present */
#define ARRAY_INDEX_ABSENT      0
/* Array index present */
#define ARRAY_INDEX_PRESENT     1


/*************** Macros for Endianness ***************/
#define BACDEL_LITTLE_ENDIAN		1
#define BACDEL_BIG_ENDIAN			0


/*************** Property related macros ***************/
/* Priority Array for commandable objects */
#define BACNET_NO_PRIORITY	0
#define BACNET_MIN_PRIORITY 1
#define BACNET_MAX_PRIORITY 16
#define USE_DEFAULT_DEVICE  0

/* Property Array size / List length related macro's */
/* max values */
#define MAX_STATE_TEXT_LEN					15			//255 // <@>
#define MAX_EXCEPTION_SCHEDULE_LEN			5
#define MAX_DATE_LIST_LEN					1
#define MAX_DESTINATION_LIST_LEN			2
#define MAX_TIME_VALUES_LIST_LEN			4
#define MAX_DEVOBJPROPREF_LIST_LEN			10
#define MAX_EVENT_PARAMETERS_LEN			1
#define MAX_ACTIVE_COV_LIST_LEN				10
#define MAX_AF_VALUES_LEN					1
#define MAX_WINDOW_SAMPLES					1
#define MIN_WINDOW_SAMPLES					1
#define MAX_LOG_DEV_ARRAY_SIZE				10
#define MAX_OBJECTID_LIST_LEN				1

/* Property Array size / List length related macro's */
/* default values */
#define DEFAULT_STATE_TEXT_LEN          	MAX_STATE_TEXT_LEN
#define DEFAULT_EXCEPTION_SCHEDULE_LEN  	MAX_EXCEPTION_SCHEDULE_LEN
#define DEFAULT_DATE_LIST_LEN           	MAX_DATE_LIST_LEN
#define DEFAULT_DESTINATION_LIST_LEN    	MAX_DESTINATION_LIST_LEN
#define DEFAULT_TIME_VALUES_LIST_LEN		MAX_TIME_VALUES_LIST_LEN
#define DEFAULT_DEVOBJPROPREF_LIST_LEN		MAX_DEVOBJPROPREF_LIST_LEN
#define DEFAULT_EVENT_PARAMETERS_LEN		MAX_EVENT_PARAMETERS_LEN
#define DEFAULT_ACTIVE_COV_LIST_LEN     	MAX_ACTIVE_COV_LIST_LEN
#define DEFAULT_AF_VALUES_LEN           	MAX_AF_VALUES_LEN
#define DEFAULT_MAX_WINDOW_SAMPLES			MAX_WINDOW_SAMPLES
#define DEFAULT_MIN_WINDOW_SAMPLES			MIN_WINDOW_SAMPLES
#define DEFAULT_LOG_DEV_ARRAY_SIZE			MAX_LOG_DEV_ARRAY_SIZE
#define DEFAULT_OBJECTID_LIST_LEN			MAX_OBJECTID_LIST_LEN

/** Minimum, Maximum and NAN floating values */
#define BACDEL_INF_P     0x7F800000	// positive Infinity
#define BACDEL_INF_N     0xFF800000	// Negative Infinity
#define BACDEL_NAN       0x7F800001	// Not a Number

/* Delay count for Program object */
#define MAX_LOAD_UNLOAD_DELAY		90	// Load-Unload state delay
#define MAX_PROGRAM_CHANGE_DELAY 	60  // Program change property delay


/*************** Services related macros ***************/
/** Max file size, i.e. max octet string bytes for file access services */
#define MAX_FILE_OCTET_STRING_BYTES 2  // <$$> 2048

/* private transfer service parameter max length */
#define MAX_PT_SER_DATA				2  // <$$> 2048


/*************** IP-Port related macros ***************/
/* Network related Bacnet definations */
#define MAX_PORT_NUMBER     65535	/* Max value of Port number used by stack */
#define MIN_PORT_NUMBER     0		/* Min value of Port number used by stack */
#define BACNET_HOST_DADR 10
#define BACNET_DADR_LEN 6
#define DADR_INDEX 2
#define BACNET_VIRTUAL_NW_NUMBER 0    /* For Virtual Routing */
#define BACNET_LOCAL_NW_NUMBER   0    /* For Router Device Object */
#define BACNET_LOCAL_BROADCAST_NETWORK_NO 0
#define BACNET_GLOBAL_BROADCAST_NETWORK_NO 0xFFFF
#define BACNET_BROADCAST_ADD_LENGTH 0
#define BACNET_PARAMETER_ABSENT -1
#define BACNET_BROADCAST_NPDU 11
#define BACNET_MSTP_BROADCAST_MAC_ADDRESS 255

/* In Broadcast packets DADR not present */
#define DADR_NOT_PRESENT 0

/* Any size MAC address should be allowed which is less than or
   equal to 7 bytes.  The IPv6 addresses are planned to be handled
   outside this area. */
/* FIXME: mac[] only needs to be as big as our local datalink MAC */
#define MAX_IP_LEN      4   // IPv4
#define MAX_PORT_LEN    2  
#define MAX_MAC_LEN    (MAX_IP_LEN + MAX_PORT_LEN)

#define MAX_MSTP_LEN	1


/** Structure to save device IP address, port no, DNET and DADR */
typedef struct BACnet_Device_Address 
{
    /* u8mac_len = 0 if global address */
    uint8_t u8mac_len;
	/* LEN = 0 denotes broadcast MAC ADR and ADR field is absent */
    /* LEN > 0 specifies length of ADR field */
    uint8_t u8dlen;        /* length of MAC address */
	/* DNET,DLEN,m_i8DADR or SNET,SLEN,SADR */
    /* the following are used if the device is behind a router */
    /* net = 0 indicates local */
    uint16_t u16net;       /* BACnet network number */
    /* note: MAC for IP addresses uses 4 bytes for addr, 2 bytes for port */
    /* use de/Encode_Unsigned32/16 for re/storing the IP address */
    uint8_t u8IpAddrs[MAX_MAC_LEN];
	/* hwaddr (MAC) address */
    uint8_t u8DvDadr[MAX_MAC_LEN];
}BACnetAddress_t;

/* PDU length calculations */
#define MAX_NPDU (1+1+2+1+MAX_MAC_LEN+2+1+MAX_MAC_LEN+1+1+2)
#define MAX_PDU (MAX_APDU_LENGTH_ACCEPTED + MAX_NPDU)

/* Specific defines for BACnet/IP over Ethernet */
#define MAX_HEADER (1 + 1 + 2)          /* Maximum Header allowed */
#define MAX_MPDU (MAX_HEADER+MAX_PDU+1)   /* Maximum PDU Support */
#define BVLL_TYPE_BACNET_IP (0x81)      /* BACnet Standard IP address */

/* Specific defines for BACnet/MSTP over serial bus */
#define MSTP_HEADER_SIZE		8  /* Maximum MSTP Header allowed */
#define MAX_MSTP_MPDU (MAX_PDU+MSTP_HEADER_SIZE+1) // 100 buffer bytes

/*****************************************************************************
 ******** Datatype To Configure Stack Parameters and Limits
 *****************************************************************************/

/** structure to save various Max limits */
typedef struct BACApp_MaxLimits
{
	/* max initiate Q length */
	uint32_t m_u32InitiateQueNodes;
	/* max BBMD initiate Q length */
	uint32_t m_u32BBMDInitiateQueNodes;
	/* max count for BDT, FDT & register FD entries */
	uint32_t m_u32MaxBdtFdtEntries;
	/* max objects that can be created in single device */
	uint32_t m_u32MaxObject;
	/* max count for dynamic address binding nodes */
	uint32_t m_u32MaxDynamicAddrBind;
	/* max count for COV subscriptions which can be sent by stack  */
	uint32_t m_u32MaxCovSubsTx;
	/* max count for properties or data-types */
	uint32_t m_u32MaxStateText;
	uint32_t m_u32MaxAFValuesList;
	uint32_t m_u32MaxExSchdList;
	uint32_t m_u32MaxTimeValueList;
	uint32_t m_u32MaxDateList;
	uint32_t m_u32MaxDestinationList;
	uint32_t m_u32MaxDevObjPropRefList;
	uint32_t m_u32MaxEventParaList;
	uint32_t m_u32MaxObjIdList;
	uint32_t m_u32MaxActiveCovSubs;	
	/* max count for static address binding nodes */
	uint32_t m_u32MaxStaticAddrBind;
	/* max count for network analyzability nodes */
	uint32_t m_u32MaxNwAnalyzability;
	/* max b-side callback notification Q length */
	uint32_t m_u32CbNotifyQueNodes;
}BACApp_MaxLimits_t;

/** structure to save stack config parameters */
typedef struct BACApp_StackConfig
{
	/* threadpool timeout in milliseconds */
	/* if timeout exceeds, received b-side request(s) is(are) aborted */
	uint32_t m_u32ThreadPoolTimeout;
	/* clear initiate queue timeout in seconds */
	/* on receiving response or in case of apdu timeout, node is deleted after 
	this timeout if callback is not registered */
	uint32_t m_u32ClearInitQTimeout;
	/* clear initiated request timeout in milliseconds */
	/* if initiate que node is in use, the node is not deleted until this timeout */
	uint32_t m_u32ClearInitReqTimeout;
	/* process queue timeout in milliseconds */
	/* if process queue is not empty, the process thread waits to check for
	empty node till this timeout */
	uint32_t m_u32ProcessQTimeout;
	/* clear initiate queue deletion count */
	/* clear initiate queue thread will be signaled to delete processed nodes
	from the queue, when no. of processed nodes is equal to this count */
	uint32_t m_u32ClearInitQDeletionCount;
	/* clear bbmd initiate queue timeout in seconds */
	/* in case of bbmd bvlc timeout, node is deleted after this timeout */
	uint32_t m_u32ClearBBMDInitQTimeout;
}BACApp_StackConfig_t;


/*****************************************************************************
 ******** Basic Datatype Structures.
 *****************************************************************************/

/**
*                                                                         
* Name - BACnetCharStr_t                                         
*                                                                      
* DESCRIPTION 
* This structure defines the property datatype 
* of type Character String.
*   
*/
typedef struct
{
    /** String lenght. */
    uint32_t    m_u32StrLen;
    /** Code page value */
    uint16_t	m_u16CodePage;
    /** Character Encoding Format */
    uint8_t     m_u8Encoding;
    /** Value for character string */
	int8_t      m_pu8CharStr[MAX_CHARACTER_STRING_BYTES];	
}BACnetCharStr_t;

/**
*                                                                         
* Name - BACnetOctetStr_t                                         
*                                                                      
* DESCRIPTION 
* This structure defines the property datatype 
* of type Octet String.
*   
*/
typedef struct
{
    /** String lenght. */
    uint32_t    m_u32OctetCount;
    /** Value for octet string */
    uint8_t		m_ai8OctetStr[MAX_OCTET_STRING_BYTES];
}BACnetOctetStr_t;

/**
*                          
* Name - BACnetBitStr_t
* 
* DESCRIPTION 
* This structure defines the property datatype 
* of type Bit String.
* Contains single byte bit string.
*   
*/
typedef struct
{	
    /* Number of unused bits in last byte */
    uint8_t		m_u8UnusedBits;
	/* Number of used byte in array */
    uint8_t		m_u8ByteCnt; 
	/* bit values */
    uint8_t		m_u8TransBits[MIN_BITSTRING_BYTES];
}BACnetBitStr_t;

/**
*                          
* Name - BACnetBitStr_t
* 
* DESCRIPTION 
* This structure defines the property datatype 
* of type Bit String.
* Contains multi byte bit string.
*   
*/
typedef struct
{	
    /* Number of unused bits in last byte */
    uint8_t		m_u8UnusedBits;
	/* Number of used byte in array */
    uint8_t		m_u8ByteCnt; 
	/* bit values */
    uint8_t		m_u8TransBits[MAX_BITSTRING_BYTES];
}BACnetBITStr_t;


/* structure of Bit-String datatype */
typedef struct BACnet_Bit_String 
{
    uint8_t bits_used;
    uint8_t value[MAX_BITSTRING_BYTES];
} BACNET_BIT_STRING;

/* structure of Character-String datatype */
typedef struct BACnet_Character_String 
{
    uint32_t length;
    uint16_t codepage;
    uint8_t encoding;
    /* limit - 6 octets is the most our tag and type could be */
    int8_t value[MAX_SUPPORTED_CHRSTR_LEN];
} BACNET_CHARACTER_STRING;

/* structure of Octet-String datatype */
typedef struct BACnet_Octet_String
 {
    uint32_t length;
    /* limit - 6 octets is the most our tag and type could be */
    uint8_t value[MAX_OCTET_STRING_BYTES];
} BACNET_OCTET_STRING;

/* date */
typedef struct BACnetDate
{
	/* Year value */
    uint16_t	m_u16Year;      /* AD */
	/* Month value */
    uint8_t		m_u8Month;      /* 1=Jan */
	/* Day value */
    uint8_t		m_u8Day;        /* 1..31 */
	/* Weekday value */
    uint8_t		m_u8Wday;       /* 1=Monday-7=Sunday */
} BACnetDate_t;

/* time */
typedef struct BACnetTime
{
	/* Hour value */
    uint8_t		m_u8Hour;
	/* Minutes value */
    uint8_t		m_u8Min;
	/* Second value */
    uint8_t		m_u8Sec;
	/* Hundredths value */
    uint8_t		m_u8Hundredths;
} BACnetTime_t;

/* note: with microprocessors having lots more code space than memory,
   it might be better to have a packed encoding with a library to
   easily access the data. */
typedef struct BACnetObjId 
{
	/* Object type */
    BACNET_OBJECT_TYPE	m_eObjType;
	/* Object instances number */
    uint32_t	m_u32ObjInstance;
} BACnetObjId_t;

/** Structure to access the data of Error Response */
typedef struct ERROR_RESPONSE
{
	/* Error class as per BACNET_ERROR_CLASS enums */
	BACNET_ERROR_CLASS   m_eErrorClass;
	/* Error code as per BACNET_ERROR_CODE enums */
	BACNET_ERROR_CODE    m_eErrorCode;
	/* First failed element number */
	int32_t     m_i32FirstFailedNo;
}error_response_t;

/** Structure to access the data of Reject Response */
typedef struct REJECT_RESPONSE
{	
	/* Reject reason as per BACNET_REJECT_REASON enums */
    BACNET_REJECT_REASON m_eRejectReason;
}reject_response_t;

/** Structure to access the data of Abort Response */
typedef struct ABORT_RESPONSE
{
	/* Abort reason as per BACNET_ABORT_REASON enums */
    BACNET_ABORT_REASON	 m_eAbortReason;
}abort_response_t;

/* BACnet Property Value Union */
struct Bacnet_Property_Value;
typedef struct Bacnet_Property_Value 
{
    int8_t  m_ContextSpecific;  /* true if context specific data */
	int8_t	m_StartTag;         /* only used for context specific data */
    int8_t	m_ContextTag;       /* only used for context specific data */
    int8_t	m_TagType;          /* application tag data type */
    union 
    {
        /* NULL - not needed as it is encoded in the tag alone */
        int8_t                      m_Boolean;
        uint32_t                    m_Unsigned_Int;
        int32_t                     m_Signed_Int;
        Float_t                     m_Real;
        Double_t                    m_Double;
        BACnetOctetStr_t			m_stOctet_String;
        BACnetCharStr_t				m_stCharacter_String;
        BACnetBITStr_t				m_stBit_String;
        uint32_t                    m_Enumerated;
        BACnetDate_t                m_stDate;
        BACnetTime_t                m_stTime;
        BACnetObjId_t				m_stObject_Id;
		void					   *m_pvValue;
    } uValue;
    /* simple linked list if needed */
    struct Bacnet_Property_Value *pstNextPropVal;
} BACNET_PROPERTY_VALUE;

/** Max size of Invoke ID queue */
#define INVOKE_ID_RANGE 255

/** structure to save device address binding data */
typedef struct BACnetAddrBinding
{
	/** Stores Device Identifier */
	uint32_t m_u32ObjId;		/** Object Instance*/
	BACNET_OBJECT_TYPE m_eObjectType;   /** Type of Object*/
	/** Stores MAX APDU Length */
	uint16_t m_u16MaxAPDULenAccepted;
    /** Vendor Id */
    uint16_t m_u16VendorId;
	/** Type of segmentation */
	BACNET_SEGMENTATION	m_eSegmentationSupport;		
	/** BACnet address */
	BACnetAddress_t m_stAddress;
	/** Next node in list */
	struct BACnetAddrBinding *pstNext;
}BACnetAddrBinding_t;

/****************************************************************************
 **** 
 ****************************************************************************/

/* BACAPP decodes WriteProperty service requests
   Choose the datatypes that your application supports */
#if !(defined(BACAPP_ALL) || \
    defined(BACAPP_NULL) || \
    defined(BACAPP_BOOLEAN) || \
    defined(BACAPP_UNSIGNED) || \
    defined(BACAPP_SIGNED) || \
    defined(BACAPP_REAL) || \
    defined(BACAPP_DOUBLE) || \
    defined(BACAPP_OCTET_STRING) || \
    defined(BACAPP_CHARACTER_STRING) || \
    defined(BACAPP_BIT_STRING) || \
    defined(BACAPP_ENUMERATED) || \
    defined(BACAPP_DATE) || \
    defined(BACAPP_TIME) || \
    defined(BACAPP_OBJECT_ID))
#define BACAPP_ALL
#endif

#if defined (BACAPP_ALL)
#define BACAPP_NULL
#define BACAPP_BOOLEAN
#define BACAPP_UNSIGNED
#define BACAPP_SIGNED
#define BACAPP_REAL
#define BACAPP_DOUBLE
#define BACAPP_OCTET_STRING
#define BACAPP_CHARACTER_STRING
#define BACAPP_BIT_STRING
#define BACAPP_ENUMERATED
#define BACAPP_DATE
#define BACAPP_TIME
#define BACAPP_OBJECT_ID
#endif


/****************************************************************************
 **** Stack Configurations
 ****************************************************************************/

/** Macro definations for callback configuration */
/** Donot change values of below 4 macros */
#define CALLBACK_NOT_REQUIRED		0
#define CALLBACK_REQUIRED			1
#define CALLBACK_BEFORE_EXECUTION	2
#define CALLBACK_AFTER_EXECUTION	3

/** Macro definations for customizations in stack */
#define STACK_CUSTOMIZE_I_AM_A							0
#define STACK_CUSTOMIZE_I_AM_B							0
#define STACK_CUSTOMIZE_I_HAVE_B						0
#define STACK_CUSTOMIZE_COV_NOTIFICATION_A				0
#define STACK_CUSTOMIZE_UTC_TIMESYNC_B                  0
#define STACK_CUSTOMIZE_SET_SEG_WINDOW_SIZE_API			0

/** Macro definations for configurations in stack */
#define	STACK_CONFIG_WHOIS_ROUTER_AT_STACK_INIT			0
#define	STACK_CONFIG_WHOIS_AT_STACK_INIT				1
#define	STACK_CONFIG_WHOIS_AT_ADD_DEVICE				0
#define	STACK_CONFIG_WHOIS_INTERNAL						0
#define	STACK_CONFIG_IAM_AT_STACK_INIT					0
#define	STACK_CONFIG_IAM_AT_ADD_DEVICE					0
#define	STACK_CONFIG_IAM_INTERNAL						0
#define	STACK_CONFIG_CLR_DYNAMIC_BIND_ON_WHOIS_GB		0
#define	STACK_CONFIG_CLR_DYNAMIC_BIND_ON_DEV_TIMEOUT	0
#define	STACK_CONFIG_IHAVE_BINDING						0
#define STACK_CONFIG_FORWARDED_NPDU_REPLY_IS_GLOBAL		1
#define STACK_CONFIG_SEND_SUBCOV_SUBS_ON_INIT			0
#define STACK_CONFIG_SEND_SUBCOV_RESUBS_ON_REINIT		0
#define STACK_CONFIG_SEND_SUBCOV_CANCELATION_ON_DEINIT	0

/** Macro definations for fallback mechanism support in stack */
#define STACK_FALLBACK_FOR_RPM							0
#define STACK_FALLBACK_FOR_WPM							0

/** Macro definations to receive callback for ... */
#define CALLBACK_FOR_APDU_RETRIES						CALLBACK_NOT_REQUIRED
#define CALLBACK_FOR_LOG_BUFFER							CALLBACK_NOT_REQUIRED
#define CALLBACK_FOR_EXTERNAL_TRENDING					CALLBACK_NOT_REQUIRED
#define CALLBACK_FOR_EXTERNAL_TRENDING					CALLBACK_NOT_REQUIRED
#define CALLBACK_FOR_INTERNAL_SCHEDULING				CALLBACK_NOT_REQUIRED
#define CALLBACK_FOR_EXTERNAL_SCHEDULING				CALLBACK_NOT_REQUIRED
#define	CALLBACK_FOR_FILE_OBJECT						CALLBACK_NOT_REQUIRED
#define	CALLBACK_FOR_WHO_IS_A							CALLBACK_REQUIRED
#define	CALLBACK_FOR_WHO_HAS_A							CALLBACK_NOT_REQUIRED

/** Stack Callback Configurations */
/** Macro definations for callback configuration for b-side as per service type */
/** Configure as per application requirement */
#define CALLBACK_CONFIG_READ_PROPERTY					CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_READ_PROPERTY_MULTIPLE			CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_READ_RANGE						CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_WRITE_PROPERTY					CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_WRITE_PROPERTY_MULTIPLE			CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_ADD_LIST_ELEMENT				CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_REMOVE_LIST_ELEMENT				CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_CREATE_OBJECT					CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_DELETE_OBJECT					CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_ATOMIC_READ_FILE				CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_ATOMIC_WRITE_FILE				CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_SUBSCRIBE_COV					CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_SUBSCRIBE_COVP					CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_COV_NOTIFICATION				CALLBACK_AFTER_EXECUTION
#define CALLBACK_CONFIG_EVENT_NOTIFICATION				CALLBACK_AFTER_EXECUTION
#define CALLBACK_CONFIG_ACKNOWLEDGE_ALARM				CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_GET_ALARM_SUMMARY				CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_GET_ENROLLMENT_SUMMARY			CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_GET_EVENT_INFORMATION			CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_DEVICE_COMMUNICATION_CONTROL	CALLBACK_NOT_REQUIRED //CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_REINITIALIZE_DEVICE				CALLBACK_NOT_REQUIRED //CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_PRIVATE_TRANSFER				CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_TEXT_MESSAGE					CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_TIME_SYNC						CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_UTC_TIME_SYNC					CALLBACK_BEFORE_EXECUTION
#define CALLBACK_CONFIG_WHO_IS							CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_WHO_HAS							CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_I_AM							CALLBACK_AFTER_EXECUTION
#define CALLBACK_CONFIG_I_HAVE							CALLBACK_AFTER_EXECUTION
#define CALLBACK_CONFIG_VT_OPEN							CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_VT_CLOSE						CALLBACK_NOT_REQUIRED
#define CALLBACK_CONFIG_VT_DATA							CALLBACK_NOT_REQUIRED


#endif /* SOFTDEL_BACNET_STACK_CONFIG_H */

