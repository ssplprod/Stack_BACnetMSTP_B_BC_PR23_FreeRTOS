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
*   File Name - bacDELPropertyDef.h
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

#ifndef BAC_PROPERTY_DEF_H
#define BAC_PROPERTY_DEF_H

/* header file includes */
#include "bacDELStackConfig.h"
#include "bacDELDef.h"
#include "DefineStruct.h"
#include "bacDELPropertyDef.h"

/** general macro */
#define PROPERTY_STRUCT 1

/** definations for RPM service */
#define MAX_PROPERTY_LIST 25
#define ENDOFPROPLIST -1

/** RPM property list with count */
struct property_list_t {
    int32_t *pList;
    uint32_t count;
};

/** RPM special property list */
typedef struct special_property_list {
    struct property_list_t Required;
    struct property_list_t Optional;
    struct property_list_t Proprietary;
}special_property_list_t;

/** For Array type properties */
#define BACNET_ARRAY_OF_TWO	    2
#define BACNET_ARRAY_OF_THREE	3
#define BACNET_ARRAY_OF_SEVEN	7
#define BACNET_ARRAY_OF_SIXTEEN	16

/** Maximum Priorities that can be assign for a single object */
#define MAX_PRIORITY_ARRAY_COUNT    16      /* Maximum Priority Array count */
#define PRIORITY_1 1

/** Service supported & object supported */
//SK
#ifdef BACDEL_PR18
#define MAX_OBJECT_TYPE_BYTE 8
#define OBJECT_SUPPORTED_UNUSED_BITS 4
#define MAX_SERVICE_SUPPORT_BYTE 6
#define SERVICE_SUPPORTED_UNUSED_BITS 4
#elif defined BACDEL_PR14
#define MAX_OBJECT_TYPE_BYTE 8
#define OBJECT_SUPPORTED_UNUSED_BITS 1
#define MAX_SERVICE_SUPPORT_BYTE 6
#define SERVICE_SUPPORTED_UNUSED_BITS 7
#else
#define MAX_OBJECT_TYPE_BYTE 7
#define OBJECT_SUPPORTED_UNUSED_BITS 5
#define MAX_SERVICE_SUPPORT_BYTE 5
#define SERVICE_SUPPORTED_UNUSED_BITS 0
#endif

/** Used for Choice type selection for EventTimeStamp property.
If any thing not define DateTime will be taken as default  */
#ifndef STAMP_AS_TIME
#ifndef STAMP_AS_SEQUENCE
#ifndef STAMP_AS_DATETIME
#define STAMP_AS_DATETIME
#endif
#endif
#endif

/****************************************************************************
 **** Basic Datatypes.
 ****************************************************************************/

#ifdef PROPERTY_STRUCT

/**                                                                         
*Name - Pr_BACnetBool_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bool. 
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */ 
    int32_t           m_i32ActiveCOV;
#endif
	/** Bool Value */
    bool              m_bVal;
}Pr_BACnetBool_t;

/**
*                                                                         
*Name - Pr_BACnetSigned32_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type signed integer. 
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */ 
    int32_t          m_i32ActiveCOV;
#endif
	/* rely on OS, if there is one */
    int32_t          m_i32Val;  
}Pr_BACnetSigned32_t;

/**
*                                                                         
*Name - Pr_BACnetUnsigned8_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Unsigned 8.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t          m_i32ActiveCOV;
#endif
	/** Value for varible of this type */
    uint8_t          m_u8Val;     
}Pr_BACnetUnsigned8_t;

/**
*
*Name - BACnetOptionalUnsigned_t //KV
*
*DESCRIPTION
*   This is structure of bacnet optional unsigned.
*
*/
typedef struct BACnetOptionalUnsigned
{
	/** variable to save data type - unsigned or null */
	BACNET_DATA_TYPE m_eDataType;
    /** value if data is unsigned */
	uint32_t   m_u32Val;
}BACnetOptionalUnsigned_t;

/**
*
*Name - Pr_BACnetOptionalUnsigned_t //KV
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet optional Unsigned.
*
*/
typedef struct Pr_BACnetOptionalUnsigned
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/* optional unsigned value */
	BACnetOptionalUnsigned_t m_stOptionalUnsigned;
}Pr_BACnetOptionalUnsigned_t;
/**
*                                                                         
*Name - Pr_BACnetUnsigned16_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Unsigned 16.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
    /** COV Status Specifier */
	int32_t         m_i32ActiveCOV;
#endif
	/** Value for data */
    uint16_t        m_u16Val;    
}Pr_BACnetUnsigned16_t;

/**
*                                                                         
*Name - Pr_BACnetUnsigned32_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type unsigned integer
*   
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
	/** data value */
    uint32_t            m_u32Val;    
}Pr_BACnetUnsigned32_t;

/**
*                                                                         
*Name - Pr_BACnetEnum_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type enum.
*   
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
	/** enum data value */
    enum_t              m_eVal;    
}Pr_BACnetEnum_t;

/**
*                                                                         
*Name - Pr_BACnetReal_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type real / float.
*   
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t          m_i32ActiveCOV;
#endif
	/* rely on OS, if there is one */ 
    Float_t          m_fVal;     
}Pr_BACnetReal_t;


/**
*
*Name - Pr_BACnetArrReal_t  //SK
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type real / float.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t          m_i32ActiveCOV;
#endif
	/* rely on OS, if there is one */
    Float_t          m_fVal[BACNET_ARRAY_OF_THREE];
}Pr_BACnetArrReal_t;


/**
*                                                                         
*Name - Pr_BACnetDouble_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type double.
*   
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t          m_i32ActiveCOV;
#endif
	/* rely on OS, if there is one */ 
    Double_t          m_dVal;     
}Pr_BACnetDouble_t;

/**
*                                                                         
*Name - Pr_BACnetCharStr_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Character String.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
	/** struct to save string, its length & encoding */
    BACnetCharStr_t     m_stCHARString;
}Pr_BACnetCharStr_t;

/**
*                                                                         
*Name - Pr_BACnetOptCharStr_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnet Optional Character String.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
    /** tag type either null or char_string */
    bool                m_bTagValue;
	/** struct to save string, its length & encoding */
    BACnetCharStr_t     m_stCHARString;
}Pr_BACnetOptCharStr_t;

/**
*                                                                         
*Name - Pr_BACnetOctetStr_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Octet String.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
    /** String lenght. It should be 1 character */
    uint32_t            m_u32OctetCount;
    /** Value for octet string */
    int8_t  m_ai8OctetStr[MAX_OCTET_STRING_BYTES];
    BACnetOctetStr_t      m_stOctetString;
}Pr_BACnetOctetStr_t;

/**
*                          
*Name - Pr_BACnetBitStr_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Bit String.
*   - contains single byte.
*   
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/* Bit string value */
	BACnetBitStr_t  m_stBitStr;
}Pr_BACnetBitStr_t;

/**
*                          
*Name - Pr_BACnetBITStr_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BITSTRING.
*   - contains multiple bytes.
*   
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/* bitstring value */
    BACnetBITStr_t     m_stBitString;
}Pr_BACnetBITStr_t;

/**
*                                                                         
*Name - Pr_BACnetObjId_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnetObjectIdentifier.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t          m_i32ActiveCOV;
#endif
	/** Type of Object*/
    BACNET_OBJECT_TYPE m_eObjectType;
	/** Object Instance*/     
    uint32_t         m_u32ObjId;       
}Pr_BACnetObjId_t;

/**
*                                                                         
*Name - Pr_BACnetTime_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type time
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
	/* rely on OS, if there is one */
    BACnetTime_t        m_Time_val;  
}Pr_BACnetTime_t;

/**
*                                                                         
*Name - Pr_BACnetDate_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type date. 
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
	/* rely on OS, if there is one */
    BACnetDate_t        m_Date_val;  
}Pr_BACnetDate_t;

#endif /* PROPERTY_STRUCT */
/**
*
* Name - ListofEEObjPropRef
*
* DESCRIPTION
* Linked list of event entrollment object prop ref list.
*
*/
typedef struct ListofEEObjPropRef
{
	/* reference property identiifer */
	BACNET_PROPERTY_ID	m_ePropertyId;
	/* event entrollment object identifire */
	uint32_t			m_u32EEObjId;
	/* next element */
	struct ListofEEObjPropRef *m_pstNext;
}ListofEEObjPropRef_t;
/**
*
* Name - Pr_ListofEEObjPropRef
*
* DESCRIPTION
* This structure defines the datatype for
* event entrollment object property reference list.
*
*/
typedef struct Pr_ListofEEObjPropRef
{
	/* flag bit is added to get the status of internal
	algorithmic reporting in a object */
	bool					m_bIsAlgoReporting;
	/* event entrollment object id list */
	ListofEEObjPropRef_t	*m_pstEEObjectList;
	/* last node in list */
	ListofEEObjPropRef_t	*m_pstLastNode;
}Pr_ListofEEObjPropRef_t;
/****************************************************************************
 **** Constructed Datatypes.
 ****************************************************************************/

#ifdef PROPERTY_STRUCT

/**
*                          
*Name - AnyValue_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type ABSTRACT-SYNTAX.
*   -- any primitive datatype; complex types cannot be decoded
*   
*/
typedef struct AnyValue
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/* Present value union */
    BACNET_PROPERTY_VALUE   m_stValue;
}AnyValue_t;

/**
*
*Name - BACnetDateTime_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type DataTime
*   - without access type & active cov.
*   
*/
typedef struct
{
    /** Time structure */
    BACnetTime_t            m_stTime;
    /** Date structure */
    BACnetDate_t            m_stDate;
}BACnetDateTime_t;


/**
*
*Name - Pr_BACnetDateTime_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type DataTime
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
    /** Time structure */
    BACnetTime_t            m_stTime;
    /** Date structure */
    BACnetDate_t            m_stDate;
    /** Date time value */
    BACnetDateTime_t        m_stDateTime;
}Pr_BACnetDateTime_t;

/**
*
*Name - timeStamp_u
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type TimeStamp union.
*   
*/
typedef union
{
    /** Time Structure */
    BACnetTime_t        m_stTime;
    /** Date and Time structure */
	BACnetDateTime_t    m_stDateTime;
    /** Sequence Number */
    uint32_t            m_u32SeqNo;
}timeStamp_u;

/**
*
*Name - BACnetTimeStamp_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type TimeStamp.
*   
*/
typedef struct 
{
    /** indicates type of time-stamp */
	BACNET_TIMESTAMP_TYPE	m_eTimeStampType;
    /** Time Stamp Structure */
    timeStamp_u				m_stTimeStamp;
}BACnetTimeStamp_t;

/**
*
*Name - Pr_BACnetTimeStamp_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type TimeStamp.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
    /** Time Stamp Structure */
    BACnetTimeStamp_t  m_stTimeStamp;
}Pr_BACnetTimeStamp_t;

/**
*
*Name - Pr_BACnetPriorityArray_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type priority array.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
    /** Priority Value type (Enum, Bool, Real..)
    Based on the data type App. Tag will be added */
    bool_t  m_bTagType[MAX_PRIORITY_ARRAY_COUNT];
    
	union{
		/* Priority array boolean value */
		bool				m_bVal[MAX_PRIORITY_ARRAY_COUNT];

		/* Priority array Binary value */
		BACNET_BINARY_PV    m_eVal[MAX_PRIORITY_ARRAY_COUNT];
	    
		/* Priority array Real value */
		Float_t             m_fVal[MAX_PRIORITY_ARRAY_COUNT];
	    
		/* Priority array unsigned32 value */
		uint32_t            m_u32Val[MAX_PRIORITY_ARRAY_COUNT];

		/* Priority array Integer value */
        int32_t             m_i32IntVal[MAX_PRIORITY_ARRAY_COUNT];

	}m_uValue; // union ends.

}Pr_BACnetPriorityArray_t;

/**
*
*Name - ListOfUnsigned_t
* 
*DESCRIPTION 
*   Linked list of Unsigned / signed data.
*   
*/
typedef struct ListOfUnsigned
{
    uint32_t m_u32Value;
    struct ListOfUnsigned *m_pstNext;
}ListOfUnsigned_t;

/**
*
*Name - Pr_ListOfUnsigned_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type list unsigned.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t 			m_i32ActiveCOV;
#endif
    uint32_t          	m_u32Count;
	/** data value */
    ListOfUnsigned_t 	*m_pstUnsignVal;    
}Pr_ListOfUnsigned_t;

/**
*
*Name - ListOfCharStr_t
* 
*DESCRIPTION 
*   Linked list of character string data.
*   
*/
typedef struct ListOfCharStr
{
	BACnetCharStr_t m_stCharStr;
    struct ListOfCharStr *m_pstNext;
}ListOfCharStr_t;

/**
*                          
*Name - Pr_ListOfCharStr_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type list character string.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif 
    /** Number of States */
    uint32_t          m_u32Count;
    /** Value for character string */
    ListOfCharStr_t   m_stStringVal;
}Pr_ListOfCharStr_t;

/**
*
*Name - ListOfOptCharStr_t
* 
*DESCRIPTION 
*   Linked list of optional character string data.
*   either null or char_string.
*   
*/
typedef struct ListOfOptCharStr
{
	BACnetCharStr_t m_stCharStr;
    int8_t      m_i8AppTag;
    struct ListOfOptCharStr *m_pstNext;
}ListOfOptCharStr_t;

/**
*                                                                         
*Name - Pr_ListOfOptCharStr_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnet Optional Character String.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
    /** no of nodes in array */
    uint32_t            m_u32Count;
	/** struct to save string, its length & encoding */
    ListOfOptCharStr_t  *m_pstOptCharStr;
}Pr_ListOfOptCharStr_t;

/**
*
*Name - ListOfBitStr_t
* 
*DESCRIPTION 
*   Linked list of bit string data.
*   
*/
typedef struct ListOfBitStr
{
    BACnetBITStr_t     m_stBitString;
    struct ListOfBitStr *m_pstNext;
}ListOfBitStr_t;

/**
*
*Name - ListOfOctetStr_t
*
*DESCRIPTION
*   Linked list of octet string data.
*
*/
typedef struct ListOfOctetStr
{
	BACnetOctetStr_t stOctetString;
	struct ListOfOctetStr *m_pstNext;
}ListOfOctetStr_t;

/**
*
*Name - Pr_ListOfOctetStr_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type list octet string.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
	int32_t 			m_i32ActiveCOV;
#endif
	uint32_t          	m_u32Count;
	/** data value */
	ListOfOctetStr_t 	*m_pstOctetStrVal;
}Pr_ListOfOctetStr_t;
/****************************************************************************
 **** Network Port object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_NP || defined PROPERTY_STRUCT)

/**
*
*Name - Pr_BACnetNetworkType_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type enum for network type.
*
*/
typedef struct Pr_BACnetNetworkType
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t              m_i32ActiveCOV;
#endif
	/** enum Value */
	BACNET_NETWORK_TYPE     m_eNetworkType;
}Pr_BACnetNetworkType_t;

/**
*                          
*Name - Pr_BACnetProtocolLevel_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type enum for protocol level.
*
*/
typedef struct Pr_BACnetProtocolLevel
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t              m_i32ActiveCOV;
#endif
	/** enum Value */
	BACNET_PROTOCOL_LEVEL     m_eProtocolLevel;
}Pr_BACnetProtocolLevel_t;

/**
*
*Name - Pr_BACnetNetworkNoQuality_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type enum for network number quality.
*
*/
typedef struct Pr_BACnetNetworkNoQuality
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t              m_i32ActiveCOV;
#endif
	/** enum Value */
	BACNET_NETWORK_NUMBER_QUALITY     m_eNetworkNoQuality;
}Pr_BACnetNetworkNoQuality_t;

/**
*
*Name - Pr_BACnetNetworkPortCmd_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type enum for network port command.
*
*/
typedef struct Pr_BACnetNetworkPortCmd
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t              m_i32ActiveCOV;
#endif
	/** enum Value */
	BACNET_NETWORK_PORT_COMMAND     m_ePortCmd;
}Pr_BACnetNetworkPortCmd_t;

/**
*
*Name - Pr_BACnetIPMode_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type enum for ip mode.
*
*/
typedef struct Pr_BACnetIPMode
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t              m_i32ActiveCOV;
#endif
	/** enum Value */
	BACNET_IP_MODE     m_eIpMode;
}Pr_BACnetIPMode_t;

/**
*
*Name - BACnetHostAddress_t
*
*DESCRIPTION
*   BACnet host address
*
*/
typedef struct BACnetHostAddress
{
	/* type 0 = null, 1 = octet string, 2 = character string */
	BACNET_DATA_TYPE m_eDataType;
	union
	{
		/* octet string address */
		BACnetOctetStr_t	m_stIPAddress;
		/* name */
		BACnetCharStr_t		m_stName;
	}BACnetHostAddr_u;
}BACnetHostAddress_t;

/**
*
*Name - Pr_BACnetHostAddress_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet host address.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** element */
    BACnetHostAddress_t  m_stHostAddress;
}Pr_BACnetHostAddress_t;

/**
*
*Name - BACnetHostNPort_t
*
*DESCRIPTION
*   BACnet host N port
*
*/
typedef struct BACnetHostNPort
{
	/* host address */
	BACnetHostAddress_t		m_stHostAddress;
	/* port number */
	uint16_t				m_u16Port;
}BACnetHostNPort_t;

/**
*
*Name - Pr_BACnetHostNPort_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet host N port.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** element */
    BACnetHostNPort_t  m_stHostNPort;
}Pr_BACnetHostNPort_t;

/**
*
*Name - BACnetBDTEntry_t
*
*DESCRIPTION
*   BACnet BDT entry
*
*/
typedef struct BACnetBDTEntry
{
	/* bbmd address */
	BACnetHostNPort_t		m_stBBMDAddress;
	/* broadcast mask */
	BACnetOctetStr_t		m_stBrdcastMask;
	/* flag for optional broadcast mask */
	bool					m_bBrdcastMaskFlag;
	/* flag indicates if this entry is used or not
	i.e. changes are pending or not */
	bool					m_bEntryUsed;
}BACnetBDTEntry_t;

/**
*
*Name - Pr_BACnetBDTEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet bdt entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** value */
    BACnetBDTEntry_t  m_stBDTEntry;
}Pr_BACnetBDTEntry_t;

/**
*
*Name - ListOfBACnetBDTEntry_t
*
*DESCRIPTION
*   BACnet BDT entry list
*
*/
typedef struct ListOfBACnetBDTEntry
{
	BACnetBDTEntry_t  m_stBDTEntry;
	struct ListOfBACnetBDTEntry *m_pstNext;
}ListOfBACnetBDTEntry_t;

/**
*
*Name - Pr_ListOfBACnetBDTEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type list of bacnet bdt entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
	/** array size */
	uint32_t    m_u32ArraySize;
	/** value */
    ListOfBACnetBDTEntry_t  *m_pstBDTEntryList;
}Pr_ListOfBACnetBDTEntry_t;

/**
*
*Name - BACnetFDTEntry_t
*
*DESCRIPTION
*   BACnet FDT entry
*
*/
typedef struct BACnetFDTEntry
{
//	/* ip address */
	BACnetOctetStr_t		m_stIPAddress;

	/* port number, used internally */
	uint16_t				m_u16PortNo;
	/* time to live */
	uint16_t				m_u16TimeToLive;
	/* remaining time to live */
	uint16_t				m_u16RemainingTimeToLive;
	/* flag indicates if this entry is used or not
	i.e. changes are pending or not */
	bool					m_bEntryUsed;
}BACnetFDTEntry_t;

/**
*
*Name - Pr_BACnetFDTEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet fdt entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
	/** value */
    BACnetFDTEntry_t  m_stFDTEntry;
}Pr_BACnetFDTEntry_t;

/**
*
*Name - ListOfBACnetFDTEntry_t
*
*DESCRIPTION
*   BACnet FDT entry
*
*/
typedef struct ListOfBACnetFDTEntry
{
	BACnetFDTEntry_t  m_stFDTEntry;
	struct ListOfBACnetFDTEntry *m_pstNext;
}ListOfBACnetFDTEntry_t;

/**
*
*Name - Pr_ListOfBACnetFDTEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type list of bacnet fdt entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
	/** array size */
	uint32_t    m_u32ArraySize;
	/** value */
    ListOfBACnetFDTEntry_t  *m_pstFDTEntryList;
}Pr_ListOfBACnetFDTEntry_t;

/**
*
*Name - VMACEntry_t
*
*DESCRIPTION
*   BACnet VMAC entry
*
*/
typedef struct BACnetVMACEntry
{
	/* virtual mac address */
	BACnetOctetStr_t		m_stVirtualMacAddr;
	/* native mac address */
	BACnetOctetStr_t		m_stNativeMacAddr;
}BACnetVMACEntry_t;

/**
*
*Name - Pr_BACnetVMACEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet virtual mac entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** value */
    BACnetVMACEntry_t  m_stVMACEntry;
}Pr_BACnetVMACEntry_t;

/**
*
*Name - ListOfBACnetVMACEntry_t
*
*DESCRIPTION
*   BACnet VMAC entry list
*
*/
typedef struct ListOfBACnetVMACEntry
{
	BACnetVMACEntry_t  m_stVmacEntry;
	struct ListOfBACnetVMACEntry *m_pstNext;
}ListOfBACnetVMACEntry_t;

/**
*
*Name - Pr_ListOfBACnetVMACEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type list of bacnet vmac entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
	/** array size */
	uint32_t    m_u32ArraySize;
	/** value */
    ListOfBACnetVMACEntry_t  *m_pstVMACEntryList;
}Pr_ListOfBACnetVMACEntry_t;

/**
*
*Name - BACnetRouterEntry_t
*
*DESCRIPTION
*   BACnet Router entry
*
*/
typedef struct BACnetRouterEntry
{
	/* mac address */
	BACnetOctetStr_t		m_stMacAddress;
	/* router status */
	BACNET_ROUTER_STATUS	m_eRouterStatus;
	/* network number */
	uint16_t				m_u16NetworkNumber;
	/* performance index */
	bool					m_bIsPerformanceIdx;
	uint8_t					m_u8PerformanceIdx;
}BACnetRouterEntry_t;

/**
*
*Name - Pr_BACnetRouterEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type bacnet router entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
	/* value */
    BACnetRouterEntry_t  m_stRouterEntry;
}Pr_BACnetRouterEntry_t;

/**
*
*Name - ListOfBACnetRouterEntry_t
*
*DESCRIPTION
*   BACnet router entry list
*
*/
typedef struct ListOfBACnetRouterEntry
{
	BACnetRouterEntry_t  m_stRouterEntry;
	struct ListOfBACnetRouterEntry *m_pstNext;
}ListOfBACnetRouterEntry_t;

/**
*
*Name - Pr_ListOfBACnetRouterEntry_t
*
*DESCRIPTION
*   This structure defines the property datatype
*   of type list of bacnet router entry.
*
*/
typedef struct
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
	/** array size */
	uint32_t    m_u32ArraySize;
	/** value */
	ListOfBACnetRouterEntry_t  *m_pstRouterEntryList;
}Pr_ListOfBACnetRouterEntry_t;

#endif /* BACDEL_OBJ_NP *//**
*
*Name - Pr_ListOfBitStr_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   for bit string value object - bitstring array
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t     m_i32ActiveCOV;
#endif
    /** no of bit string count */
    uint32_t    m_u32Count;
    /**/
    ListOfBitStr_t    *m_pstBitString;
}Pr_ListOfBitStr_t;

/**
*
*Name - bacnetArrayIndex_t
* 
*DESCRIPTION 
*   This structure defines the property Array index, 
*   & union for all primitive data types.
*   
*/
typedef struct
{
    /** Array index */
    int32_t     m_i32ArrayIndex;
    /** union */
    union
    {
        /** Unsigned value */
        uint32_t            m_UnsignVal;

        /** Signed value */
        int32_t             m_SignVal;

        /** Real value */
        Float_t             m_floatVal;

        /** Enum value */
        BACNET_BINARY_PV    m_eVal;

        /** Double value */
        Double_t            m_doubleVal;

        /** Char string value */
        int8_t              m_au8CharString[MAX_CHARACTER_STRING_BYTES];
        
        /** Bit string value */
        BACnetBITStr_t      m_stBitStringVal;

        /** Octet string value */
        uint8_t             m_au8OctetString[MAX_OCTET_STRING_BYTES];

        /** ADD data type if required */

    }m_uValue;
}bacnetArrayIndex_t;

/**
*
*Name - ListOfBoolen_t
* 
*DESCRIPTION 
*   Linked list of boolean data.
*   
*/
typedef struct ListOfBoolen
{
	/** Bool Value */
    bool                    m_bVal;
	/** Next element */        
    struct ListOfBoolen    *m_pstNext;  
}ListOfBoolen_t;

/**
*                          
*Name - Pr_ListOfBoolen_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type boolean array.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** Boolean List count */
    uint32_t    m_u32BoolCount;
    /** next value */
    ListOfBoolen_t   *m_pstBoolList;
}Pr_ListOfBoolen_t;

/**
*                          
*Name - Pr_BinaryEnumPV_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type enum & enum array with polarity.
*   
*/
typedef struct Pr_BinaryEnumPV
{
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/** COV Status Specifier */ 
    int32_t              m_i32ActiveCOV;
#endif
	/** Bool Value */
    BACNET_BINARY_PV     m_eVal;
	/** Next element */        
    struct Pr_BinaryEnumPV *m_pstNext; 
	/* polarity for Present value in Binary objects */ 
    BACNET_POLARITY      m_ePVPolarity;   
}Pr_BinaryEnumPV_t;

/**
*
*Name - ListOfEnum_t
* 
*DESCRIPTION 
*   Linked list of enumerated data.
*   
*/
typedef struct ListOfEnum
{
	/** Enumerated Value */
    enum_t              m_eVal;
	/** Next element */        
    struct ListOfEnum   *m_pstNext;  
}ListOfEnum_t;

/**
*                          
*Name - Pr_ListOfEnum_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type enum array/list.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** Enum List count */
    uint32_t    m_u32Count;
    /** next value */
    ListOfEnum_t   *m_pstEnumList;
}Pr_ListOfEnum_t;

/**
*                                                                         
*Name - Pr_BACnetPropertyList                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type property list.
*   
*/
typedef struct Pr_BACnetPropertyList
{
	/* Property List count */
	uint32_t	m_u32PropCount;
	/** Required property pointer to array */
    int32_t     *m_psti32RequiredProp;
	/** Optional property pointer to array */
    int32_t     *m_psti32OptionalProp;
	/** Proprietary property pointer to array */
    int32_t     *m_psti32ProprietaryProp;
}Pr_BACnetPropertyList_t;

#endif /* PROPERTY_STRUCT */


/****************************************************************************
 **** Property value union and ANY data type
 ****************************************************************************/

/**	this union is defined to pass the present (or any other) value 
	using pass by value mechanism. */
/** can also be used to pass value of any other property with below
	included data types */
typedef union 
{
    bool                bVal;
	uint32_t			u32Val;
    int32_t             i32Val;
	BACNET_BINARY_PV	eVal;
	Float_t				fVal;
    Double_t            dVal;
    BACnetBITStr_t      stBitStringVal;
    BACnetCharStr_t     stCharStringVal;
    BACnetOctetStr_t    stOctetStringVal;
    BACnetTime_t        TimeVal;
    BACnetDate_t        DateVal;
    BACnetDateTime_t    DateTimeVal;
	BACnetObjId_t		stObjectId;
    /** Add data types here, if required */
}PropertyValue_u;

/**
*
* Name - BACnetAbstractData_t 
* DESCRIPTION 
* Structure to save ANY data type property values.
*
*/
typedef struct 
{
	/* pointer to save property value */
    void                *m_pvConstrProp;
	/* property data type */
    BACNET_DATA_TYPE    m_eData_Type;
}BACnetAbstractData_t;

/**
*
* Name - property_value_t 
* DESCRIPTION 
* Structure to save property values.
* Datatype as per standard is BACnetPropertyValue.
*
*/
typedef struct PROPERTY_VALUES
{
	BACNET_PROPERTY_ID   m_eObjectProperty;
    int32_t     m_i32PropertyArrayIndex;
    bool        m_bIsArrayIndxPresent;
	BACNET_DATA_TYPE     m_eData_Type;	
	uint8_t		m_u8Priority;
    struct PROPERTY_VALUES *m_pstNextVal;
	//#ifdef NEW_RP_WP_INTERFACE
	void		*m_pvPropVal;
	//#endif
}property_value_t;


/****************************************************************************
 **** Properties with Enum datatype values.
 ****************************************************************************/

#ifdef PROPERTY_STRUCT

/**
*                          
*Name - Pr_BACnetObjType_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet object type.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/** Bacnet Object type */
    BACNET_OBJECT_TYPE  m_eObjectType;   
}Pr_BACnetObjType_t;

/**
*                          
*Name - Pr_BACnetDevStatus_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet device status.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */ 
    int32_t m_i32ActiveCOV;
#endif
	/** Status of device*/
    BACNET_DEVICE_STATUS    m_eDeviceStatus; 
}Pr_BACnetDevStatus_t;

/**
*                          
*Name - Pr_BACnetBackupState_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type backup & restore state.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */ 
    int32_t m_i32ActiveCOV;
#endif
	/** Backup & Restore state of device */
    BACNET_BACKUP_STATE    m_eBackupState; 
}Pr_BACnetBackupState_t;

/**
*                          
*Name - Pr_BACnetEnggUnits_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type engineering units.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/** Measurement Unit */
    BACNET_ENGINEERING_UNITS    m_eEnggUnits;
}Pr_BACnetEnggUnits_t;

/**
*                                                                         
*Name - Pr_BACnetSegmentation_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnet Segementation. 
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B         
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/** Type of segmentation */
    BACNET_SEGMENTATION    m_eSegmentationSupport;  
}Pr_BACnetSegmentation_t;

/**
*                          
*Name - Pr_BACnetEventState_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Event state.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/* Event state value */
    BACNET_EVENT_STATE  m_eEventState;
}Pr_BACnetEventState_t;

/**
*                          
*Name - Pr_BACnetReliability_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Reliability.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B  
	/** COV Status Specifier */ 
    int32_t m_i32ActiveCOV;
#endif
	/* Relaiblity value */
    BACNET_RELIABILITY  m_eReliabilty;
}Pr_BACnetReliability_t;

/**
*                          
*Name - Pr_BACnetBinaryPV_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type binary PV.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/* Event state value */
    BACNET_BINARY_PV  m_eBinaryPv;
}Pr_BACnetBinaryPV_t;

/**
*                          
*Name - Pr_BACnetPolarity_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Polarity.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
    /** polarity value */
    BACNET_POLARITY     m_ePolarity;
}Pr_BACnetPolarity_t;

/**
*                          
*Name - Pr_BACnetNotifyType_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Notify Type
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B   
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /* type of notification */
    BACNET_NOTIFY_TYPE m_eNotifyType;
}Pr_BACnetNotifyType_t;

/**
*                          
*Name - Pr_BACnetEventType_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type event type.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /** type of event */
    BACNET_EVENT_TYPE m_eEventType;
}Pr_BACnetEventType_t;

/**
*                          
*Name - Pr_BACnetFileAccessMethod_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type file access method.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/** File Access Method */
    BACNET_FILE_ACCESS_METHOD    m_eAccessMethod;
}Pr_BACnetFileAccessMethod_t;

/**
*                          
*Name - Pr_BACnetLoggingType_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet logging type.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /* type of logging */
    BACNET_LOGGING_TYPE m_eLoggingType;
}Pr_BACnetLoggingType_t;

/**
*                          
*Name - Pr_BACnetAction_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet action.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of action */
	BACNET_ACTION   m_eAction;
}Pr_BACnetAction_t;

/**
*                                                                         
*Name - Pr_BACnetLifeSafetyMode_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet life safety mode.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
    /** COV Status Specifier */
    int32_t             m_i32ActiveCOV;
#endif
    /** type of life safety mode */
	BACNET_LIFE_SAFETY_MODE  m_eSafetyMode;
}Pr_BACnetLifeSafetyMode_t;

/**
*
*Name - Pr_BACnetLifeSafetyState_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet life safety state.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B  
	/** COV Status Specifier */ 
    int32_t             m_i32ActiveCOV;
#endif 
    /** type of life safety state */
	BACNET_LIFE_SAFETY_STATE   m_eSafetyState;
}Pr_BACnetLifeSafetyState_t;

/**
*                                                                         
*Name - Pr_BACnetLifeSafetyOperation_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet life safety operation.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of life safety operation */
	BACNET_LIFE_SAFETY_OPERATION   m_eSafetyOperation;
}Pr_BACnetLifeSafetyOperation_t;

/**
*                                                                         
*Name - Pr_BACnetSilencedState_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet silenced state.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */         
    int32_t              m_i32ActiveCOV;
#endif
    /** type of silenced state */
	BACNET_SILENCED_STATE  m_eSilencedState;
}Pr_BACnetSilencedState_t;

/**                                                                         
*                          
*Name - Pr_BACnetMaintenance_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet maintenance.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of maintenance */
	BACNET_MAINTENANCE   m_eMaintenance;
}Pr_BACnetMaintenance_t;

/**
*                          
*Name - Pr_BACnetProgramState_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet program state.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of program state */
	BACNET_PROGRAM_STATE   m_eProgramState;
}Pr_BACnetProgramState_t;

/**
*                          
*Name - Pr_BACnetProgramRequest_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet program request.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of program request */
	BACNET_PROGRAM_REQUEST  m_eProgramRequest;
}Pr_BACnetProgramRequest_t;

/**
*                          
*Name - Pr_BACnetProgramError_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet program error.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of program error */
	BACNET_PROGRAM_ERROR   m_eProgramError;
}Pr_BACnetProgramError_t;

/**                                                                         
*                          
*Name - Pr_BACnetDoorValue_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet door value.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */         
    int32_t m_i32ActiveCOV;
#endif
    /** type of door value */
	BACNET_DOOR_VALUE  m_eDoorValue;
}Pr_BACnetDoorValue_t;

/**                                                                         
*                          
*Name - Pr_BACnetDoorStatus_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet door status.
*   
*/
typedef struct
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** type of door status */
	BACNET_DOOR_STATUS   m_eDoorStatus;
}Pr_BACnetDoorStatus_t;

/**                                                                         
*                          
*Name - Pr_BACnetDoorSecuredStatus_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet door secured status.
*   
*/
typedef struct
{
    /** type of door secured status */
	BACNET_DOOR_SECURED_STATE   m_eDoorSecuredState;
}Pr_BACnetDoorSecuredStatus_t;

/**                                                                         
*                          
*Name - Pr_BACnetLockStatus_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet lock status.
*   
*/
typedef struct
{
    /** type of lock status */
	BACNET_LOCK_STATUS   m_eLockStatus;
}Pr_BACnetLockStatus_t;

/**                                                                         
*                          
*Name - Pr_BACnetDoorAlarmState_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet door alarm state.
*   
*/
typedef struct
{
    /** type of door alarm state */
	BACNET_DOOR_ALARM_STATE  m_eDoorAlarmState;
}Pr_BACnetDoorAlarmState_t;

/**
*                          
*Name - Pr_BACnetAccessEvent_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet access event.
*   
*/
typedef struct
{
    /** type of access event */
	BACNET_ACCESS_EVENT  m_eAccessEvent;
}Pr_BACnetAccessEvent_t;

/**
*                          
*Name - Pr_BACnetAccessZoneOccState_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet access zone occupancy state.
*   
*/
typedef struct
{
    /** type of access zone occupancy state */
	BACNET_ACCESS_ZONE_OCCUPANCY_STATE  m_eOccupancyState;
}Pr_BACnetAccessZoneOccState_t;

/**
*                          
*Name - Pr_BACnetAccessPassbackMode_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet access passback mode.
*   
*/
typedef struct
{
    /** type of access passback mode */
	BACNET_ACCESS_PASSBACK_MODE   m_ePassbackMode;
}Pr_BACnetAccessPassbackMode_t;

/**
*                                                                         
*Name - Pr_BACnetAccessUserType_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet access user type.
*   
*/
typedef struct
{
    /** type of access user */
	BACNET_ACCESS_USER_TYPE   m_eUserType;
}Pr_BACnetAccessUserType_t;

/**
*                                                                         
*Name - Pr_BACnetAccessCredDisable_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet access credential disable.
*   
*/
typedef struct
{
    /** type of access credential disable value */
	BACNET_ACCESS_CREDENTIAL_DISABLE  m_eCredentialDisable;
}Pr_BACnetAccessCredDisable_t;

/**                                                                         
*                          
*Name - Pr_BACnetNodeType_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet node type.
*   
*/
typedef struct
{
    /** type of node */
	BACNET_NODE_TYPE   m_eNodeType;
}Pr_BACnetNodeType_t;

/**
*                          
*Name - Pr_BACnetSecurityLevel_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet security level.
*   
*/
typedef struct
{
    /** type of security level */
	BACNET_SECURITY_LEVEL   m_eSecurityevel;
}Pr_BACnetSecurityLevel_t;

/**
*                                                                         
*Name - Pr_BACnetShedState_t
*                                                                      
*DESCRIPTION 
*   structure for BACnet Shed State
*   
*/

typedef struct 
{
    /** type of shed level */
    BACNET_SHED_LEVEL     m_eShedLevelType;    
}Pr_BACnetShedState_t;

/**
*
*Name - Pr_BACnetFaultType_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Fault Type.
*   
*/
typedef struct
{
	/* Fault type value */
    BACNET_FAULT_TYPE  m_eFaultType;
}Pr_BACnetFaultType_t;

#endif /* PROPERTY_STRUCT */


/****************************************************************************
 **** Property Datatypes.
 ****************************************************************************/

#ifdef PROPERTY_STRUCT

/**
*                                                                         
*Name - Pr_BACnetEventTimeStamp_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Event Time Stamp array.
*   
*/
typedef struct
{
	/** indicates type of time-stamp */
	BACNET_TIMESTAMP_TYPE	m_eTimeStampType[BACNET_ARRAY_OF_THREE];
    /** time stamp union - and array of three */
    timeStamp_u         m_uEvntStamp[BACNET_ARRAY_OF_THREE];
}Pr_BACnetEventTimeStamp_t;

/***
*
*Name - Pr_BACnetEventMsgText_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet event message text.
*
*/

typedef struct 
{
    /** array of 3 character strings */
    BACnetCharStr_t    m_stEventMsgText[BACNET_ARRAY_OF_THREE];
}Pr_BACnetEventMsgText_t;


/**
*                                                                         
*Name - ListOfObjId_t                                         
*                                                                      
*DESCRIPTION 
*   Linked list of Object Id. 
*   
*/
typedef struct ListOfObjId
{
	/** flag that indicates if object is dynamically created or not */
	bool	m_bObjDynamicallyCreated;
	/** Object Instance */
	struct{
		/** Type of Object*/
		BACNET_OBJECT_TYPE  m_eObjectType;
		/** Object Instance*/     
		uint32_t         m_u32ObjId;   
	}m_stObjectId;     
	/** Pointer to List */
    struct ListOfObjId   *m_pstNext; 
}ListOfObjId_t;

/**
*                                                                         
*Name - Pr_ListOfObjId_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of list / array of BACnet Object Id. 
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /* Object List count */
	uint32_t	m_u32ObjCount;
    /* Array of object Ids */
    ListOfObjId_t    *m_pstArrayObjId; 
}Pr_ListOfObjId_t;

/**
*Name - BACnetServicesSupported_t                                         
* 
*DESCRIPTION 
*   structure defines the types of services supported. 
*   
*/
typedef union
{
    uint8_t Byte[MAX_SERVICE_SUPPORT_BYTE];                /*   */
    struct
    {
       uint32_t m_btAcknowledgeAlarm             : 1;/*    Bit  0    */
       uint32_t m_btConfirmedCovNotification     : 1;
       uint32_t m_btConfirmedEventNotification   : 1;
       uint32_t m_btGetAlarmSummary              : 1;// Bit 4
       uint32_t m_btGetEnrollmentSummary         : 1;
       uint32_t m_btSubscribeCov                 : 1;
       uint32_t m_btAtomicReadFile               : 1;
       uint32_t m_btAtomicWriteFile              : 1;// Bit 7

       uint32_t m_btAddListElement               : 1;// Bit 8
       uint32_t m_btRemoveListElement            : 1;
       uint32_t m_btCreateObject                 : 1;
       uint32_t m_btDeleteObject                 : 1;
       uint32_t m_btReadProperty                 : 1;// Bit 12
       uint32_t m_btReadPropertyConditional      : 1;
       uint32_t m_btReadPropertyMultiple         : 1;
       uint32_t m_btWriteProperty                : 1;// Bit 15

       uint32_t m_btWritePropertyMultiple        : 1;
       uint32_t m_btDeviceCommunicationControl   : 1;
       uint32_t m_btConfirmedPrivateTransfer     : 1;
       uint32_t m_btConfirmedTextMessage         : 1;
       uint32_t m_btReinitializeDevice           : 1;
       uint32_t m_btVtOpen                       : 1;
       uint32_t m_btVtClose                      : 1;
       uint32_t m_btVtData                       : 1;

       uint32_t m_btAuthenticate                 : 1;
       uint32_t m_btRequestKey                   : 1;
       uint32_t m_btIAm                          : 1;
       uint32_t m_btIHave                        : 1;
       uint32_t m_btUnconfirmedCovNotification   : 1;
       uint32_t m_btUnconfirmedEventNotification : 1;
       uint32_t m_btUnconfirmedPrivateTransfer   : 1;
       uint32_t m_btUnconfirmedTextMessage       : 1;

       uint32_t m_btTimeSynchronization          : 1;
       uint32_t m_btWhoHas                       : 1;
       uint32_t m_btWhoIs                        : 1;
       uint32_t m_btReadRange                    : 1;
       uint32_t m_btUtcTimeSynchronization       : 1;
       uint32_t m_btLifeSafetyOperation          : 1;
       uint32_t m_btSubscribeCovProperty         : 1;
       uint32_t m_btGetEventInformation          : 1;

#if (defined BACDEL_PR14 || defined BACDEL_PR18)  //SK
	   uint32_t m_btWriteGroup							: 1;
	   uint32_t m_btSubscribeCovMultiple				: 1;
	   uint32_t m_btConfirmedCovNotificationMultiple	: 1;
	   uint32_t m_btUnconfirmedCovNotificationMultiple	: 1;
#ifdef BACDEL_PR23
	   uint32_t m_btSpare4						 : 1;
       uint32_t m_btWhoAmI								: 1; // Bit 47
       uint32_t m_btYouAre								: 1; // Bit 48
#endif /* BACDEL_PR23 */	   
       uint32_t m_btSpare5						 : 1;
	   uint32_t m_btSpare6						 : 1;
	   uint32_t m_btSpare7						 : 1;
	   #endif
    }Bit;
}BACnetServicesSupported_t;

/**
*Name - Pr_BACnetServicesSupported_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type object services supported. 
*   
*/
typedef struct 
{
    BACnetServicesSupported_t   m_stServiceSupport;
}Pr_BACnetServicesSupported_t;

/**
*Name - BACnetObjectTypesSupported_t                                         
*                                                                      
*DESCRIPTION 
*   structure defines the types of object supported. 
*   
*/
typedef union
{
    uint32_t u32Value;
    uint8_t Byte[MAX_OBJECT_TYPE_BYTE];
    struct
    {
        uint32_t m_btAnalogInput        : 1;/*    Bit  0    */
        uint32_t m_btAnalogOutput       : 1;
        uint32_t m_btAnalogValue        : 1;
        uint32_t m_btBinaryInput        : 1;
        uint32_t m_btBinaryOutput       : 1;
        uint32_t m_btBinaryValue        : 1;
        uint32_t m_btCalendar           : 1;
        uint32_t m_btCommand            : 1;// Bit 7

        uint32_t m_btDevice             : 1;// Bit 8
        uint32_t m_btEventEnrollment    : 1;
        uint32_t m_btFile               : 1;
        uint32_t m_btGroup              : 1;
        uint32_t m_btLoop               : 1;
        uint32_t m_btMultiStateInput    : 1;
        uint32_t m_btMultiStateOutput   : 1;
        uint32_t m_btNotificationClass  : 1;

        uint32_t m_btProgram            : 1;// Bit 16
        uint32_t m_btSchedule           : 1;
        uint32_t m_btAveraging          : 1;
        uint32_t m_btMultiStateValue    : 1;
        uint32_t m_btTrendLog           : 1;
        uint32_t m_btLifeSafetyPoint    : 1;
        uint32_t m_btLifeSafetyZone     : 1;
        uint32_t m_btAccumulator        : 1;
       
        uint32_t m_btPulseConverter     : 1;// Bit 24
        uint32_t m_btEventLog           : 1;
        uint32_t m_btGlobalGroup        : 1;
        uint32_t m_btTrendLogMultiple   : 1;
        uint32_t m_btLoadControl        : 1;
        uint32_t m_btStructuredView     : 1;
        uint32_t m_btAccessDoor         : 1;
        uint32_t m_btUnassigned31       : 1;// Unused in BACnet Std.

        uint32_t m_btAccessCredential   : 1;// Bit 32
        uint32_t m_btAccessPoint        : 1;
        uint32_t m_btAccessRights       : 1;
        uint32_t m_btAccessUser         : 1;
        uint32_t m_btAccessZone             : 1;
        uint32_t m_btCredentialDataInput    : 1;
        uint32_t m_btNetworkSecurity        : 1;
        uint32_t m_btBitstringValue         : 1;

        uint32_t m_btCharacterstringValue   : 1;// Bit 40
        uint32_t m_btDatePatternValue       : 1;
        uint32_t m_btDateValue              : 1;
        uint32_t m_btDateTimePatternValue   : 1;
        uint32_t m_btDateTimeValue          : 1;
        uint32_t m_btIntegerValue           : 1;
        uint32_t m_btLargeAnalogValue       : 1;
        uint32_t m_btOctetstringValue       : 1;

        uint32_t m_btPositiveIntegerValue   : 1;// Bit 48
        uint32_t m_btTimePatternValue       : 1;
        uint32_t m_btTimeValue              : 1;
		#ifdef BACDEL_PR14
        uint32_t m_btNotificationForwarder  : 1;
        uint32_t m_btAlertEnrollment        : 1;
        uint32_t m_btChannel                : 1;
        uint32_t m_btLightingOutput         : 1;
        uint32_t m_btSpare1                 : 1;
		#else
		uint32_t m_btSpare1                 : 1;
        uint32_t m_btSpare2                 : 1;
        uint32_t m_btSpare3                 : 1;
        uint32_t m_btSpare4                 : 1;
        uint32_t m_btSpare5                 : 1;
		#endif /* */

		#ifdef BACDEL_OBJ_NP
		uint32_t m_btNetworkPort			: 1;
		uint32_t m_btElevatorGroup			: 1;
		uint32_t m_btEscalator              : 1;
		uint32_t m_btLift					: 1;
		uint32_t m_btSpare5                 : 1;
		uint32_t m_btSpare6                 : 1;
		uint32_t m_btSpare7                 : 1;
		uint32_t m_btSpare8                 : 1;
		#endif /* */
    }Bit;
}BACnetObjectTypesSupported_t;

/**                                                                         
*Name - Pr_BACnetObjectTypesSupported_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type object types supported. 
*   
*/
typedef struct
{
    /* type of objects supported */
    BACnetObjectTypesSupported_t   m_stObjectSupport;
}Pr_BACnetObjectTypesSupported_t;

/**                                                                         
*Name - Pr_BACnetAddrBinding_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnet Address Bind . 
*   
*/
typedef struct Pr_BACnetAddrBinding
{
	/** Address binding data for device */
	BACnetAddrBinding_t   m_stAddBinding;
}Pr_BACnetAddrBinding_t;

/**                                                                         
*Name - Pr_ListOfBACnetAddrBinding_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnet Address Bind list. 
*   
*/
typedef struct Pr_ListOfBACnetAddrBinding
{
	/** count for no of nodes in list */ 
    uint32_t    m_u32Count;
	/** Address binding data for device */
	BACnetAddrBinding_t   m_stAddBinding;
}Pr_ListOfBACnetAddrBinding_t;

/**                                                                         
*Name - CovRecipientInfo_t                                         
*                                                                      
*DESCRIPTION 
*   recipient info for COV subscription. 
*/
typedef struct CovRecipientInfo
{
    /** If Cancellation/Subscription request */
    int8_t m_i8Subscribe;
    /** The time remaining for the subscription to get cancelled */
    uint32_t m_u32TimeRemaining;
    /** If Property type to be monitored is Real, unsigned, integer COV notification is send if the value changes by COVIncrement */
	
	BACNET_PROPERTY_VALUE m_stPropertyValue;
	
    //Float_t m_fCOVIncrement;
    /** flag indicates if cov increment was received in subscription or copied
        from cov increment property 
        true == from subscription else false */
    bool m_bCovIncFrmClient;
    /** Notification to be send will be of type confirmed or unconfirmed */
    bool m_bIssueConfirmedNotification;
    /** The process Id of device to which notification is to be send */
    uint32_t m_u32ProcessId;
    /** Device Id */
	struct
	{
		/** Type of Object*/
		BACNET_OBJECT_TYPE m_eObjectType;
		/** Object Instance*/     
		uint32_t         m_u32ObjId;
	}m_stObjId;
    /** Address of the registered device */
    BACnetAddress_t 	 m_stAddress;
    /** Next List element */
    struct CovRecipientInfo *m_pstNext;
}CovRecipientInfo_t;

/**                                                                         
*Name - CovPropElement_t                                         
*                                                                      
*DESCRIPTION 
*   property info & value for COV subscription. 
*/
typedef struct CovPropElement
{
    /** Data Type of property */
    BACNET_DATA_TYPE   m_eData_Type;
    /** Monitored Property */
    BACNET_PROPERTY_ID m_ePropertyId;
    /** indicates if array index is present or absent */
    bool    m_bArrIndxresent;
    /** Array index parameter if present */
    uint32_t m_u32PropertyArrayIndex;
	/** priority value */
	uint8_t	m_u8Priority;
    /** Value that has to be compared */
    void *m_pvStoredValue;
    /** Value that has to be compared */
    void *m_pvCurrentValue;
    /** Next Element in list */
    struct CovPropElement *m_pstNext;
}CovPropElement_t;

/**
*                          
*Name - ListOfBACnetCovSubs_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet COV subscription.
*   
*/
typedef struct ListOfBACnetCovSubs
{
    /** Index number */
    int32_t m_i32Index;
    /** Object Identifier of monitored property */
    struct
	{
		/** Type of Object*/
		BACNET_OBJECT_TYPE	m_eObjectType;
		/** Object Instance*/     
		uint32_t            m_u32ObjId;
	}m_stObjId;
    /** Property that needs to be subscribed */
    CovPropElement_t	    m_stCOVPropertyElem;
    /** Stores relevent data of the subscriber */
    CovRecipientInfo_t  *m_pstSubscriberInfo;
    /** Next Element in list */
    struct ListOfBACnetCovSubs *m_pstNext;
}ListOfBACnetCovSubs_t;

/**
*                          
*Name - Pr_ListOfBACnetCovSubs_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type list of bacnet COV subscription.
*   
*/
typedef struct 
{
    /** For Mutex Lock */
    void * m_hcovsublistMtxLock;
	/* list count */
	uint32_t m_u32Count;
    /** COV subscription structure */
    ListOfBACnetCovSubs_t *m_pstCOVSubscribe;
}Pr_ListOfBACnetCovSubs_t;

/**
*                          
*Name - covUnsolicitlist_t
* 
*DESCRIPTION 
*   list of unsolicited COV subscription.
*   
*/
typedef struct covunsolicitlist
{
    uint32_t m_u32TimeRemaining;
    /** pointer to COV subscription list */
    ListOfBACnetCovSubs_t *m_pstCOVSubscribe;
    /* Next monitored object */
    struct covunsolicitlist * m_pstNextObject;
}covUnsolicitlist_t;

/**
*                                                                         
*Name - Pr_BACnetVTClass_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   BACnet VT Class. 
*   
*/
typedef struct Pr_BACnetVTClass
{
	/** Type of VT class */
    BACNET_VT_CLASS          m_eVTClassSupport; 
	/** Poiter to next list element */
    struct Pr_BACnetVTClass   *m_pstNext; 
}Pr_BACnetVTClass_t;

/**
*                                                                         
*Name - BACnetVTSession_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   BACnet VT sessions. 
*   
*/
typedef struct 
{
	/** Local Session Id */
    uint8_t               m_u8LocalVTSessionId;  
	/** Remote Session Id */
    uint8_t               m_u8RemoteVTSessionId;
	/** Remote Address */
    BACnetAddress_t       m_stRemoteVTAddress; 
}BACnetVTSession_t;

/**
*                                                                         
*Name - ListOfBACnetVTSession_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   BACnet VT sessions list. 
*   
*/
typedef struct ListOfBACnetVTSession
{
	BACnetVTSession_t   m_stVTSession;
	struct ListOfBACnetVTSession *m_pstNext;
}ListOfBACnetVTSession_t;

/**
*                                                                         
*Name - Pr_ListOfBACnetVTSession_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   BACnet VT sessions. 
*   
*/
typedef struct 
{
	/* list count */
	uint32_t m_u32Count;
	/* list of vt sessions */
	ListOfBACnetVTSession_t *m_pstListOfVTSession;
}Pr_ListOfBACnetVTSession_t;

/**                                                                         
*Name - Pr_BACnetSessKey_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type session key. 
*   
*/
typedef struct Pr_BACnetSessKey
{
	/** Cryptographic Session Key */
    uint8_t             m_sessionKey[8];
	/** BACnet Address of the peer with which secure communications is requested*/
    BACnetAddress_t     m_PeerAddress;  
	/** Next list element pointer */
    struct Pr_BACnetSessKey  *m_pstNext;  
}Pr_BACnetSessKey_t;

/** 
*                            
*Name - BACnetRecipient_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet recipient.
*   
*/
typedef struct 
{
	DESTINATION_TYPE m_eDestinationType;
	union
	{
		/** Device Id */
		struct{
			/** Type of Object*/
			BACNET_OBJECT_TYPE  m_eObjectType; 
			/** Object Instance*/
			uint32_t  m_u32ObjId; 
		}m_stObjId;
		/** Address of the registered device */
		BACnetAddress_t m_stAddress;
	}BACnetRecipient_u;
}BACnetRecipient_t;

/**
*                          
*Name - ListOfBACnetRecipient_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type list of bacnet recipient.
*   
*/
typedef struct ListOfBACnetRecipient
{
	 /** The address of device to which notification is to be send */
	BACnetRecipient_t   m_stRecipient;
	/** Next list element pointer */
	struct ListOfBACnetRecipient *m_pstNext;
}ListOfBACnetRecipient_t;

/**
*                          
*Name - Pr_ListOfBACnetRecipient_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet recipient.
*   
*/
typedef struct 
{
	/* count for no of list elements */
	uint32_t    m_u32Count;
	/* list of recipients */
	ListOfBACnetRecipient_t *m_pstListOfRecipient;
}Pr_ListOfBACnetRecipient_t;

/**
*                          
*Name - BACnetRecipientProcess_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet recipient.
*   - stores the process ID & recipient.
*   
*/
typedef struct 
{
    /** The address of device to which notification is to be send */
    BACnetRecipient_t   m_stRecepient;
    /** The process Id of device to which notification is to be send */
    uint32_t m_u32ProcessId;
}BACnetRecipientProcess_t;

/**
*                          
*Name - BACnetDevObjRef_t
* 
*DESCRIPTION 
*   struct for bacnet device object refferences.
*   
*/
typedef struct 
{
	/** Device Type flag */    
    bool            m_bDeviceIdPresent;
	/** Type of Object*/
    BACNET_OBJECT_TYPE m_eObjectType;
	/** Device Type */
    BACNET_OBJECT_TYPE m_eDeviceType;
	/** Object Instance*/     
    uint32_t        m_u32ObjId;
	/** Device Instance*/
    uint32_t		m_u32DeviceId;
}BACnetDevObjRef_t;


/**
*                                                                         
*Name - Pr_BACnetDevObjRef_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet device object reference. 
*   
*/
typedef struct 
{
	/** Device Object references data */
	BACnetDevObjRef_t m_stDeviceObjReff;
}Pr_BACnetDevObjRef_t;

/**
*
*Name - ListOfBACnetDevObjRef_t
* 
*DESCRIPTION 
*   Linked list of device object reference.
*   
*/
typedef struct ListOfBACnetDevObjRef
{
	/** list of device object reference */
    BACnetDevObjRef_t      m_stDevObjRef; 
	/** Next list element pointer */
    struct ListOfBACnetDevObjRef *m_pstNext;
}ListOfBACnetDevObjRef_t;

/**
*                                                                         
*Name - Pr_ListOfBACnetDevObjRef_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array/list of bacnet device object reference.
*   
*/
typedef struct 
{
    /** array size */
    uint32_t    m_u32ArraySize;
    /** list of values */
    ListOfBACnetDevObjRef_t    *m_pstListOfDevObjReff;
}Pr_ListOfBACnetDevObjRef_t;

/**
*                                                                         
*Name - Pr_BACnetObjPropRef_t                                         
*                                                                      
*DESCRIPTION 
*   This is structure of bacnet object property reference. 
*   
*/
typedef struct 
{
	/** Object Instance*/     
    uint32_t        m_u32ObjId;	    
	/** Type of Object*/
    BACNET_OBJECT_TYPE m_eObjectType;	
    /** Array Index & its flag */
    bool            m_bArrIndxPresent;
    uint32_t		m_u32ArrayIndex;
	/** Property Identifier*/
    BACNET_PROPERTY_ID m_ePropertyIdentifier;
}BACnetObjPropRef_t;

/**
*                                                                         
*Name - Pr_BACnetObjPropRef_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet object property reference. 
*   
*/
typedef struct 
{
	/** Object Property Reference data */
	BACnetObjPropRef_t m_stObjPropRef;
}Pr_BACnetObjPropRef_t;

/**
*
*Name - ListOfBACnetObjPropRef_t
* 
*DESCRIPTION 
*   Linked list of object property reference.
*   
*/
typedef struct ListOfBACnetObjPropRef
{
	/** Object Property Reference data */
    BACnetObjPropRef_t          m_stObjPropRef; 
	/** Next list element pointer */
    struct ListOfBACnetObjPropRef    *m_pstNext;
}ListOfBACnetObjPropRef_t;

/**
*                                                                         
*Name - Pr_ListOfBACnetObjPropRef_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array/list of bacnet object property reference.
*   
*/
typedef struct 
{
    /** array size */
    uint32_t    m_u32ArraySize;
    /** list of values */
    ListOfBACnetObjPropRef_t    *m_pstListOfObjPropReff;
}Pr_ListOfBACnetObjPropRef_t;

/**
*                          
*Name - BACnetDevObjPropRef_t
* 
*DESCRIPTION 
*   struct for bacnet device object property ref.
*   
*/
typedef struct 
{
	/** Type of Object*/
    BACNET_OBJECT_TYPE m_eObjectType;
    BACNET_OBJECT_TYPE m_eDeviceType;
	/** Object Instance*/     
    uint32_t        m_u32ObjId;
	/** Device Instance*/
    uint32_t		m_u32DeviceInstace;
	/** Device Type & its flag */    
    bool            m_bDeviceIdPresent;
    /** Array Index & its flag */
    bool            m_bArrIndxPresent;
    uint32_t        m_u32ArrayIndex;
	/** Property Identifier*/
    BACNET_PROPERTY_ID m_ePropertyIdentifier; 
}BACnetDevObjPropRef_t;

/**
*                                                                         
*Name - Pr_BACnetDevObjPropRef_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet device object property reference.
*   
*/
typedef struct 
{
	/** Device Object Property references data */
	BACnetDevObjPropRef_t m_stDevObjPropReff;
}Pr_BACnetDevObjPropRef_t;

/**
*
*Name - ListOfBACnetDevObjPropRef_t
* 
*DESCRIPTION 
*   Linked list of device object property reference.
*   
*/
typedef struct ListOfBACnetDevObjPropRef
{
	/** Device Object Property references data */
    BACnetDevObjPropRef_t   m_stDevObjPropRef; 
	/** Next list element pointer */
    struct ListOfBACnetDevObjPropRef    *m_pstNext;
}ListOfBACnetDevObjPropRef_t;

/**
*                                                                         
*Name - Pr_ListOfBACnetDevObjPropRef_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet device object property reference.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /* hold array size only for 
    sBACnetARRAY[N] of BACnetDeviceObjectPropertyReference */
    uint32_t    m_u32ArraySize;
    /** list of values */
    ListOfBACnetDevObjPropRef_t    *m_pstListOfBACnetDevObjPropReff;
}Pr_ListOfBACnetDevObjPropRef_t;

/**
*                                                                         
*Name - BACnetDevObjPropVal_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet device object property value.
*   
*/
typedef struct 
{
    /** device id */
	uint32_t    m_u32DeviceInstace;
    /** object id */
    uint32_t    m_u32ObjId;
    /** device type */
	BACNET_OBJECT_TYPE m_eDeviceType;
    /** object type */
	BACNET_OBJECT_TYPE m_eObjectType;
    /** array index */
	uint32_t	m_u32ArrayIndex;
    /** property id */
	BACNET_PROPERTY_ID m_ePropertyId;
    /** Any value */
	BACNET_PROPERTY_VALUE m_uValue;
}BACnetDevObjPropVal_t;

/**
*                                                                         
*Name - BACnetPropertyStates_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet property states.
*   
*/
typedef struct
{
    /* tag for encoding & decoding, also denotes BACNET_PROPERTY_STATES value */
    BACNET_PROPERTY_STATES m_ePropState;
    /* tag to save application tag of prop data type */
    BACNET_APPLICATION_TAG m_eAppTag;
    union
    {
	    /*boolean-value*/
	    bool	 					m_BooleanValue;
	    /*binary-value*/
	    BACNET_BINARY_PV			m_BinaryValue;
	    /*event-type*/
	    BACNET_EVENT_TYPE			m_EventType;
	    /*polarity*/
	    BACNET_POLARITY				m_Polarity;
	    /*program-change*/
	    BACNET_PROGRAM_REQUEST		m_ProgramChange; 
	    /*program-state*/
	    BACNET_PROGRAM_STATE		m_ProgramState; 
	    /*reason-for-halt*/
	    BACNET_PROGRAM_ERROR		m_ReasonForHalt;
	    /*reliability*/
	    BACNET_RELIABILITY			m_Reliability;
	    /*state*/
	    BACNET_EVENT_STATE			m_Eventstate;
	    /*system-status*/
	    BACNET_DEVICE_STATUS		m_SystemStatus;
	    /*units*/
	    BACNET_ENGINEERING_UNITS	m_Units;
	    /*unsigned-value*/
	    uint32_t					m_UnsignedValue;
	    /*life-safety-mode*/
	    BACNET_LIFE_SAFETY_MODE		m_LifeSafetyMode;
	    /*life-safety-state*/
	    BACNET_LIFE_SAFETY_STATE	m_LifeSafetyState;
    }BACnetPropertyStates_U;
}BACnetPropertyStates_t;

/**
*                                                                         
*Name - ListOfBACnetPropertyStates_t                                         
*                                                                      
*DESCRIPTION 
*   Linked list of bacnet property states.
*   
*/
typedef struct ListOfBACnetPropertyStates
{
	/** BACnetPropertyStates */
	BACnetPropertyStates_t		m_stPropStateVal;
    /** Next list element pointer */
    struct ListOfBACnetPropertyStates *m_pstNext;
}ListOfBACnetPropertyStates_t;

/**
*                                                                         
*Name - bacnetActiveElapse_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Elapsed Active Time.
*   
*/
typedef struct
{
    /** For storing Active state elapse info */
    Pr_BACnetUnsigned32_t   stActiveElapseTime;
    /** For storing time of Active state start */
    Pr_BACnetDateTime_t     stActiveStateTime;    
}bacnetActiveElapse_t;

/**
*                                                                         
*Name - ListOfBACnetLifeSafetyStates_t                                         
*                                                                      
*DESCRIPTION 
*   Linked list of bacnet life safety states.
*   
*/
typedef struct ListOfBACnetLifeSafetyStates
{
    /** life safety state */
	BACNET_LIFE_SAFETY_STATE            m_eLifeSafetyState;
    /** pointer to create link list */
	struct ListOfBACnetLifeSafetyStates  *m_pstNext;
}ListOfBACnetLifeSafetyStates_t;

/**
*                          
*Name - BACnetAuFactor_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet Authentication Factor.
*   
*/
typedef struct 
{
    /** type of authentication format */
    BACNET_AUTHENTICATION_FACTOR_TYPE     m_eFormatType;
    /** format class */     
    uint32_t				m_u32FormatClass;
    /** value of type - octetstring */
    BACnetOctetStr_t		m_stOctetStrValue;
}BACnetAuFactor_t;

#endif /* PROPERTY_STRUCT */


/****************************************************************************
 **** Notification Class object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_NC || defined PROPERTY_STRUCT)

/**
*                                                                         
*Name - Pr_BACnetNotifyPriority_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet priority.
*   - unsigned array of 3.
*   
*/
typedef struct
{
	/** data value - array of three */
   uint32_t m_u32Value[BACNET_ARRAY_OF_THREE];    
}Pr_BACnetNotifyPriority_t;

/**
*                                                                         
*Name - ListOfBACnetDestination_t                                         
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet destination.
*   - linked list of destinations.
*
*/
typedef struct ListOfBACnetDestination
{
	/** Notification to be send will be of type confirmed or unconfirmed */
    bool					m_bIssueConfirmedNotification;
	/** The process Id of device to which notification is to be send */
	uint32_t				m_stProcessId;
	/** valid from time after which the notification can be sent to particular destination */
	BACnetTime_t			m_stFromTime;
	/** valid to time upto which the notification can be sent to particular destination */
	BACnetTime_t			m_stToTime;
	/**	valid day of week on which the notification is to be sent */
	BACnetBitStr_t		    m_stDaysOfWeek;
    /**	event transitions bits	*/
	BACnetBitStr_t		    m_stTransitions;
	/** address of the recipient for the notification */
	BACnetRecipient_t		m_stRecipient;
	/** pointer to create a link list for no of destinations */
	struct ListOfBACnetDestination	*m_pstNext;
}ListOfBACnetDestination_t;

/**
*                                                                         
*Name - Pr_ListOfBACnetDestination_t                                         
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type list of bacnet destination.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/** no of array/list elements */
    uint32_t m_u32Count;
	/** Recipient List */
	ListOfBACnetDestination_t *m_pstNCRecepient;
}Pr_ListOfBACnetDestination_t;

#endif /* BACDEL_OBJ_NC */


/****************************************************************************
 **** Event Parameters.
 ****************************************************************************/

#if (defined BACDEL_OBJ_EE || defined PROPERTY_STRUCT)

/* change-of-bitstring */
typedef struct
{
	/** time-delay */
	uint32_t            m_u32Timedelay; 
	/** bitmask */
	Pr_BACnetBITStr_t   m_stBitmask;
    /** count for no of list elements */
    uint32_t            m_u32Count;
	/** list-of-bitstring-values */
    ListOfBitStr_t      *m_pstBitStrList;
}ChangeOfBitstring_t;

/* change-of-state */
typedef struct
{
	/** time-delay */
	uint32_t				        m_u32Timedelay;
	/** BACnetPropertyStates */
	ListOfBACnetPropertyStates_t    *m_pstListOfValues;
}ChangeOfState_t;

/* change-of-value union */
typedef union
{	/** bitmask */
	Pr_BACnetBITStr_t   m_stBitmask;
	/** referenced-property-increment */
	Float_t             m_fRefPropIncr; 
}covcriteria_U; 

/* change-of-value */
typedef struct
{
	/** time-delay */
	uint32_t		m_u32Timedelay;
	/** change-of-value union */
    /** type of value for union */
    int8_t          m_i8CriteriaType;
	covcriteria_U   m_stCovCriteria;
}ChangeOfValue_t;

/* command-failure */
typedef struct
{
	/** to save application tag */
	uint8_t		            m_u8Apptag;
	/** time-delay */
	int32_t                 m_u32Timedelay;
	/** feedback-property-reference */
	BACnetDevObjPropRef_t	m_stFeedbackPropertyReference;
	/** to save feedback property value for sending it in notification */
	PropertyValue_u		    m_uFeedbackPropValue;
}CommandFailure_t;

/* floating-limit */
typedef struct
{
    /** time-delay */
    uint32_t                m_u32Timedelay;
    /** setpoint-reference */
    BACnetDevObjPropRef_t   m_stSetpointReference;
    /** low-diff-limit */
    Float_t                 m_fLow_Diff_Limit;
    /** high-diff-limit */
    Float_t                 m_fHigh_Diff_Limit;
    /** deadband */
    Float_t                 m_fDeadband;  
}FloatingLimit_t;

/* out-of-range */
typedef struct
{
    /** time-delay */
    uint32_t           m_u32Timedelay;
    /** low-limit */
    Float_t            m_fLow_Limit;
    /** high-limit */
    Float_t            m_fHigh_Limit;
    /** deadband */
    Float_t            m_fDeadband;  
}OutOfRange_t;

/* change-of-life-safety */
typedef struct
{
	/** time-delay */
	uint32_t          m_u32Timedelay;
	/** list-of-life-safety-alarm-values */
	ListOfEnum_t      *m_pstListOfLifeSafetyAlarmValues;
	/** list-of-alarm-values */
	ListOfEnum_t      *m_pstListOfAlarmValues;
	/** mode-property-reference */
	BACnetDevObjPropRef_t   m_stModePropReff;
}ChangeOfLifeSafety_t;

/* extended-parameters-union */
typedef union 
{
	/** boolean value */
    bool                    bVal;
	/** unsigned value */
	uint32_t			    u32Val;
	/** integer value */
    int32_t                 i32Val;
	/** enum value */
	BACNET_BINARY_PV	    eVal;
	/** real value */
	Float_t				    fVal;
	/** double value */
    Double_t                dVal;
	/** bitsring value */
    BACnetBITStr_t          stBitStringVal;
	/** octetstring value */
    BACnetOctetStr_t        stOctetStringVal;
	/** device object property reference */
    BACnetDevObjPropRef_t   stDevObjPropReff;
	/** Object id */
    Pr_BACnetObjId_t		stObjId;
	/** character string */
	BACnetCharStr_t			stCharStr;
	/** date */
    BACnetDate_t			stDate;
	/** time */
	BACnetTime_t			stTime;
}Parameters_u;

/* extended-parameters-structure */
typedef struct ExtdParameters
{
    /** type of parameter */
    uint8_t                 m_u8ParameterType;
    /** parameter value */
    Parameters_u            m_stParameters;
    /** to create linklist */
    struct ExtdParameters  *m_pstNext;
}Parameters_t;

/* extended */
typedef struct
{
	/** vendorId */
	uint32_t        m_u32VendorId;
	/** extendedEventType */
	uint32_t        m_u32ExtendedEventType;
	/** sequence of parameters */
	Parameters_t   *m_pstParameters;
}Extended_t;

/* buffer-ready */
typedef struct
{
	/** notification-threshold */
	uint32_t     m_u32NotificationThreshold;
	/** previous-notification-count */
	uint32_t     m_u32PreviousNotificationCount;
}BufferReady_t;

/* unsigned-range */
typedef struct 
{
	/** time-delay */
	uint32_t		m_u32Timedelay;
	/** low-limit */
	uint32_t		m_u32LowLimit; 
	/** high-limit */
	uint32_t		m_u32HighLimit; 
}UnsignedRange_t;

/* access-event */
typedef struct
{
    /** no of elements in list */
    uint32_t                m_u32Count;
    /** list-of-access-events */
    ListOfEnum_t           *m_pstAccessEventList;
    /** access-event-time-reference */
    BACnetDevObjPropRef_t   m_stAccessEventTimeReff;
}AccessEvent_t;

/* double-out-of-range */
typedef struct
{
    /** time-delay */
    uint32_t            m_u32Timedelay;
    /** low-limit */
    Double_t            m_dLow_Limit;
    /** high-limit */
    Double_t            m_dHigh_Limit;
    /** deadband */
    Double_t            m_dDeadband;  
}DoubleOutOfRange_t;

/* signed-out-of-range */
typedef struct
{
    /** time-delay */
    uint32_t           m_u32Timedelay;
    /* low-limit */
    int32_t            m_i32Low_Limit;
    /* high-limit */
    int32_t            m_i32High_Limit;
    /* deadband */
    uint32_t           m_u32Deadband;
}SignedOutOfRange_t;

/* unsigned-out-of-range */
typedef struct
{
    /* time-delay */
    uint32_t           m_u32Timedelay;
    /* low-limit */
    uint32_t           m_u32Low_Limit;
    /* high-limit */
    uint32_t           m_u32High_Limit;
    /* deadband */
    uint32_t           m_u32Deadband;
}UnSignedOutOfRange_t;

/* change-of-characterstring */
typedef struct
{
    /* time-delay */
    uint32_t          m_u32Timedelay;
    /* no of list elements */
    uint32_t          m_u32Count;
    /* list-of-alarm-values */
    ListOfCharStr_t  *m_pstListOfAlarmValues;
}ChangeOfCharString_t;

/* change-of-status-flags */
typedef struct
{
    /* time-delay */
    uint32_t                m_u32Timedelay;
    /* selected-flags */
    BACnetBitStr_t			m_stStatusFlag;
}ChangeOfStatusFlag_t;


/**
*
*Name - BACnetEventParameter_t
* 
*DESCRIPTION 
*   This structure defines the bacnet event parameters datatype.
*
*/
typedef struct 
{
    /** type of event */
	BACNET_EVENT_TYPE m_eEventType;
    /** parameters */
	union 
	{
		/* change-of-bitstring */
		ChangeOfBitstring_t		m_stCngBitstring;
		/* change-of-state */
		ChangeOfState_t			m_stCngState;
		/* change-of-value */
		ChangeOfValue_t			m_stCngValue;
		/* command-failure */
		CommandFailure_t		m_stCmdFail;
		/* floating-limit */
		FloatingLimit_t			m_stFlotLimit;
		/* out-of-range */
		OutOfRange_t			m_stOutRange;
		/* change-of-life-safety */
		ChangeOfLifeSafety_t    m_stCngLifeSafety;
		/* extended */
		Extended_t				m_stExtended;
		/* buffer-ready */
		BufferReady_t			m_stBuffReady;
		/* unsigned-range */
		UnsignedRange_t			m_stUnsiRange;
        /* access-event */
        AccessEvent_t           m_stAccessEvent;
        /* double-out-of-range */
        DoubleOutOfRange_t      m_stDoubleOutofRange;
        /* signed-out-of-range */
        SignedOutOfRange_t      m_stSignedOutofRange;
        /* unsigned-out-of-range */
        UnSignedOutOfRange_t    m_stUnSignedOutofRange;
        /* change-of-characterstring */
        ChangeOfCharString_t    m_stCngCharString;
        /* change-of-status-flags */
        ChangeOfStatusFlag_t    m_stCngStatusFlag;
	}BACnetEventParameter_u;
}BACnetEventParameter_t;

/**
*
*Name - Pr_BACnetEventParameter_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet event parameters.
*
*/
typedef struct 
{
	/* BACnet event parameter data */
	BACnetEventParameter_t   m_stEventParam;
}Pr_BACnetEventParameter_t;

/**
*
*Name - FaultStatusFlags_t
* 
*DESCRIPTION 
*   structure for fault status flag type.
*
*/
typedef struct
{
    /* status-flags-reference */
    BACnetDevObjPropRef_t   m_stStatusflagsReff;
}FaultStatusFlags_t;


/**
*
*Name - FaultState_t
* 
*DESCRIPTION 
*   structure for fault state type.
*
*/
typedef struct
{
	/* no of list elements */
    uint32_t          m_u32Count;
	/* BACnetPropertyStates */
	ListOfBACnetPropertyStates_t    *m_pstListOfFaultValues;
}FaultState_t;

/**
*
*Name - FaultLifeSafety_t
* 
*DESCRIPTION 
*   structure for Life safety fault type.
*
*/
typedef struct
{
	/* no of list elements */
    uint32_t          m_u32Count;
	/* list-of-fault-values */
	ListOfEnum_t      *m_pstListOfFaultValues;
	/* mode-property-reference */
	BACnetDevObjPropRef_t   m_stModePropReff;
}FaultLifeSafety_t;

/**
*
*Name - FaultExtended_t
* 
*DESCRIPTION 
*   structure for Extended fault type.
*
*/
typedef struct
{
	/* vendorId */
	uint16_t        m_u16VendorId;
	/* extended fault type */
	uint32_t        m_u32ExtendedFaultType;
	/* no of list elements */
    uint32_t          m_u32Count;
	/* sequence of parameters */
	Parameters_t   *m_pstParameters;
}FaultExtended_t;

/**
*
*Name - FaultCharString_t
* 
*DESCRIPTION 
*   structure for fault character string list.
*
*/
typedef struct BacnetFaultCharString
{
    /** no of list elements */
    uint32_t          m_u32Count;
    /** list-of-fault-values */
    ListOfCharStr_t  *m_pstListOfFaultValues;
}FaultCharString_t;

/**
*
*Name - FaultType_u
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type FaultType union.
*   
*/
typedef union
{
    /** Fault Character string Structure */
    FaultCharString_t	m_stFaultCharString;
	/** Fault Extended Structure */
    FaultExtended_t		m_stFaultExtended;
    /** Fault Life Safety Structure */
    FaultLifeSafety_t	m_stFaultLifeSafety;
	/** Fault State Structure */
    FaultState_t		m_stFaultState;
	/** Fault Status Flags Structure */
    FaultStatusFlags_t	m_stFaultStatusFlags;
}FaultType_u;

/**
*
*Name - BACnetFaultParameter_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type FaultParameter.
*   
*/
typedef struct 
{
    /** indicates type of Fault */
	BACNET_FAULT_TYPE	m_eFaultType;
    /** Fault type Structure */
    FaultType_u			m_stFaultType;
}BACnetFaultParameter_t;

/**
*
*Name - Pr_BACnetFaultParameter_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type Fault parameter.
*   
*/
typedef struct
{
    /** Fault Parameter Structure */
    BACnetFaultParameter_t  m_stFaultParameter;
}Pr_BACnetFaultParameter_t;

#endif /* BACDEL_OBJ_EE */


/****************************************************************************
 **** Notification Parameters.
 ****************************************************************************/

#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_SER_AE_EN_B || defined PROPERTY_STRUCT)

/* bacnet notification parameters - change of bitstring */
typedef struct
{
	BACnetBITStr_t		        m_stRefBitString; // referenced bit string i.e PV
	BACnetBitStr_t				m_stStatusFlag; // status flag
}ChangeOfBitstringNP_t;

/* bacnet notification parameters - change of state */
typedef struct
{
	BACnetPropertyStates_t	    m_uNewState; // new states of PV
	BACnetBitStr_t				m_stStatusFlag; // status flag
}ChangeOfStateNP_t;

/* bacnet notification parameters - change of value */
typedef struct
{
    int8_t                  m_i8ValueType;
	union{
		Float_t				m_fChangedValue; // changed value
		BACnetBITStr_t	    m_stChangedBits; // changed bits
	}uNewValue;
	BACnetBitStr_t		    m_stStatusFlag; // status flag
}ChangeOfValueNP_t;

/* bacnet notification parameters - command failure */
typedef struct
{
	/* to save application tag */
	uint8_t					m_u8Apptag;
	PropertyValue_u			m_stCommandValue; // command value is PV
	PropertyValue_u			m_stFeedbackValue;
	BACnetBitStr_t			m_stStatusFlag; // status flag
}CommandFailureNP_t;

/* bacnet notification parameters - floating limit */
typedef struct
{
	 Float_t	            m_fReferenceValue; // controlled variable value
	 Float_t	            m_fSetPointValue; // set point value		
	 Float_t	            m_fErrorLimit; // error limit		
	 BACnetBitStr_t			m_stStatusFlag; // status flag
}FloatingLimitNP_t;

/* bacnet notification parameters - out of range */
typedef struct
{
	 Float_t	            m_fExceedingValue; // exceeding value is PV
	 Float_t	            m_fExceededLimit; // exceeded limit is high or low limit
	 Float_t	            m_fDeadband; // deadband
	 BACnetBitStr_t			m_stStatusFlag; // status flag
}OutOfRangeNP_t;

/* bacnet notification parameters - complex event type */
typedef struct
{
	// this structure is not complete, refer manual
	//bacnet property value
	uint16_t                i16Val;
}ComplexEventTypeNP_t;

/* bacnet notification parameters - change of life safety */
typedef struct
{
	BACNET_LIFE_SAFETY_MODE		  m_eNewMode; // new mode
	BACNET_LIFE_SAFETY_OPERATION  m_eOperationExpected; // operations expected
	BACNET_LIFE_SAFETY_STATE	  m_eNewState; // life safety states of PV
	BACnetBitStr_t				  m_stStatusFlag; // status flag
}ChangeOfLifeSafetyNP_t;

/* bacnet notification parameters - extended */
typedef struct
{
	uint16_t                m_u16VendorId; // vendor id
	uint32_t                m_u32ExtendedEventType; // extended event type
	/* sequence of parameters */
	Parameters_t           *m_pstParameters;
}ExtendedNP_t;

/* bacnet notification parameters -  buffer ready */
typedef struct
{
	uint32_t	            m_u32PreviousNotification; // last notifiy record 
	uint32_t	            m_u32CurrentNotification; // total record count
	BACnetDevObjPropRef_t   m_stBufferProperty; // buffer property
}BufferReadyNP_t;

/* bacnet notification parameters - unsigned range */
typedef struct 
{
	uint32_t	            m_u32ExceedingValue; // pulse rate
	uint32_t	            m_u32ExceededLimit; // low or high limit
	BACnetBitStr_t			m_stStatusFlag; // status flag
}UnsignedRangeNP_t;

/* bacnet notification parameters - access event */
typedef struct 
{
	BACNET_ACCESS_EVENT     m_eAccessEvent; // access event value
    uint32_t                m_u32EventTag; // access event tag value
    BACnetTimeStamp_t       m_stEventTime; // access event time
    BACnetDevObjRef_t		m_AccessCredential; // access credential value
	BACnetBitStr_t			m_stStatusFlag; // status flag
    bool                    m_bAuFactorPresent;
    BACnetAuFactor_t		m_stAuFactor;
}AccessEventNP_t;

/* bacnet notification parameters - double out of range */
typedef struct
{
	Double_t	            m_dExceedingValue; // exceeding value is PV
	Double_t	            m_dExceededLimit; // exceeded limit is high or low limit
	Double_t	            m_dDeadband; // deadband
	BACnetBitStr_t			m_stStatusFlag; // status flag
}DoubleOutOfRangeNP_t;

/* bacnet notification parameters - signed out of range */
typedef struct
{
	int32_t	                m_i32ExceedingValue; // exceeding value is PV
	int32_t	                m_i32ExceededLimit; // exceeded limit is high or low limit
	uint32_t	            m_u32Deadband; // deadband
	BACnetBitStr_t			m_stStatusFlag; // status flag
}SignedOutOfRangeNP_t;

/* bacnet notification parameters - unsigned out of range */
typedef struct
{
    uint32_t	            m_u32ExceedingValue; // exceeding value is PV
    uint32_t	            m_u32ExceededLimit; // exceeded limit is high or low limit
    uint32_t	            m_u32Deadband; // deadband
    BACnetBitStr_t			m_stStatusFlag; // status flag
}UnSignedOutOfRangeNP_t;

/* bacnet notification parameters - change of character string */
typedef struct
{
    BACnetCharStr_t            m_stChangedValue; // PV value
    BACnetCharStr_t            m_stAlarmValue; // corresponding alarm value
    BACnetBitStr_t			   m_stStatusFlag; // status flag
}ChangeOfCharStringNP_t;

/* bacnet notification parameters - change of status flags */
typedef struct
{
    uint8_t                 m_u8AppTag;
    PropertyValue_u         m_uPresentValue; // PV of reference property
    BACnetBitStr_t			m_stStatusFlag; // status flag
}ChangeOfStatusFlagsNP_t;

/* bacnet notification parameters - change of reliability */
typedef struct
{
    BACNET_RELIABILITY		m_eReliabilty; // reliability
    BACnetBitStr_t			m_stStatusFlag; // status flag
	property_value_t        m_stPropertyValue; // prperty value
}ChangeOfReliabilityNP_t;

/**
*
*Name - BACnetNotificationParameters_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet notification parameters.
*
*/
typedef struct NotificationParameters
{
    /** event type */
    BACNET_EVENT_TYPE       m_eEventType;
	/** flag to specify if event values were decoded successfully */
	/** value is true if event values decoding fails */
	bool					m_bDecodeFailure;
    /** notification parameters */
	union{
		ChangeOfBitstringNP_t		m_stChangeBitstring;
		ChangeOfStateNP_t			m_stChangeState;
		ChangeOfValueNP_t			m_stChangeValue;
		CommandFailureNP_t			m_stCmdFailure;
		FloatingLimitNP_t			m_stFlotLimit;
		OutOfRangeNP_t				m_stOutOfRange;
		ComplexEventTypeNP_t		m_stCmplxEventType;
		ChangeOfLifeSafetyNP_t		m_stChangeLifeSafety;
		ExtendedNP_t				m_stExtended;
		BufferReadyNP_t				m_stBufferReady;
		UnsignedRangeNP_t			m_stUnsignedRange;
        AccessEventNP_t             m_stAccessEvent;
        DoubleOutOfRangeNP_t        m_stDoubleOutOfRange;
        SignedOutOfRangeNP_t        m_stSignedOutOfRange;
        UnSignedOutOfRangeNP_t      m_stUnSgndOutOfRange;
        ChangeOfCharStringNP_t      m_stChangeCharString;
        ChangeOfStatusFlagsNP_t     m_stChangeStatusFlags;
		ChangeOfReliabilityNP_t		m_stChangeOfReliability;
	}BACnetNotificationParameters_u;
}BACnetNotificationParameters_t;

/** structure to save the message string for event notification */
typedef struct
{
    /** String lenght */
    uint32_t    m_u32StrLen;
    /** For Encoding Format */
    uint8_t     m_Encoding;
    /** Value for character string */
    int8_t      m_pu8CharStr[MAX_CHARACTER_STRING_BYTES];
}NotificationString_t;

#endif /* BACDEL_SER_AE_EN_A || BACDEL_SER_AE_EN_B */


/****************************************************************************
 **** Confirmed / Unconfirmed Event Notification service parameters.
 ****************************************************************************/

/* To include service structures for Event Notification */
#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_SER_AE_EN_B || defined PROPERTY_STRUCT)

/** structure to hold the event notification data */
typedef struct 
{
	/* The process Id of device to which notification is to be send */
    uint32_t		m_u32ProcessId;
    /** Initiating Device Id */
    //Pr_BACnetObjId_t	m_stDeviceId;
    uint32_t        m_u32DeviceId;		/* Object Instance */
	BACNET_OBJECT_TYPE  m_eDeviceType;	/* Type of Object */
	/* Event Object Id */
    //Pr_BACnetObjId_t m_stObjectId;
	BACNET_OBJECT_TYPE  m_eObjectType;	/* Type of Object */
    uint32_t        m_u32ObjId;		/* Object Instance */
	/* Time Stamp */
	BACnetTimeStamp_t	m_stTimeStamp;
	/* Notification Class */
	uint32_t		m_u32NotifyClass;
	/* Priority - use BACNET_NO_PRIORITY if no priority */
	uint8_t			m_u8Priority;   
	/* Event Type */
	BACNET_EVENT_TYPE	m_eEventType;
	/* Notify Type */
	BACNET_NOTIFY_TYPE	m_eNotifyType;
	/* From State */
	BACNET_EVENT_STATE	m_eFromState;
	/* To State */
	BACNET_EVENT_STATE	m_eToState;
	/* Message Text (optional) */
	BACnetCharStr_t		m_CharString;
	/* Event Parameters */
	BACnetNotificationParameters_t	m_stEventValues;
	/* Ack Required */
	bool	m_bAckReq;
	/* Status of service success or Error */
	bool	bErrorStatus;   
}BacnetEnPropElem_t;

#endif /* BACDEL_SER_AE_EN_A || BACDEL_SER_AE_EN_B || PROPERTY_STRUCT */


/****************************************************************************
 **** File object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_FILE || defined PROPERTY_STRUCT)

/**
*                                                                         
*Name - file_data                                         
*                                                                      
*DESCRIPTION 
*   Structure to hold file stream data or list of ercords.
*   
*/
typedef struct file_data
{
	/* indicates the data size read from the file.
		- no of bytes read in case of stream access
		- no of bytes for the particular record in case of record access */
	uint32_t	m_u32NoOfBytes;
	/* buffer to save file data */
	uint8_t				m_u8FileData[MAX_FILE_OCTET_STRING_BYTES];
	/* Self reference pointer to create linklist */
	struct file_data	*m_pstNext;
}LIST_OF_FILE_DATA;

#endif /* BACDEL_OBJ_FILE */


/****************************************************************************
 **** Calendar & Schedule object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_SDL || defined BACDEL_OBJ_CAL || defined PROPERTY_STRUCT)

/**
*                          
*Name - BACnetDateRange_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type date range.
*   
*/
typedef struct
{
    /** start date*/
    BACnetDate_t      m_stStartDate;
    /** end date */
    BACnetDate_t      m_stEndDate; 
}BACnetDateRange_t;

/**
*                          
*Name - Pr_BACnetDateRange_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet date range.
*   
*/
typedef struct
{   
    /** date range value */
    BACnetDateRange_t   m_stDateRange;
}Pr_BACnetDateRange_t;

/**
*                          
*Name - WeekNDay_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet Week N Day.
*   
*/
typedef struct
{   /** Month */
    BACNET_MONTH m_eMonth;
    /** Week */
    BACNET_WEEK_OF_MONTH m_eWeekOfMonth;
    /** Days */
    BACNET_WEEKDAY  m_eWeekNDay;
}WeekNDay_t;

/**
*                          
*Name - Calendar_u
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet calendar entry union.
*   
*/
typedef union
{   /*Date*/
    BACnetDate_t m_stDate;
    /*Date Range*/
    BACnetDateRange_t  m_stDateRange;
    /*Week and Day*/
    WeekNDay_t m_stWeekNDay;
}Calendar_u;

/** 
* 
*Name - BACnetCalendarEntry_t
* 
*DESCRIPTION 
*   calendar entry data type.
*   
*/
typedef struct 
{
    /** type of calendar entry value */
    CALENDAR_ENTRY_STATUS   m_eStatusCalendar;
    /** calendar entry value */
    Calendar_u              m_stCalendar;    
}BACnetCalendarEntry_t;

/**
*
*Name - ListOfBACnetCalendarEntry_t
* 
*DESCRIPTION 
*   Linked list of calendar entry.
*   
*/
typedef struct ListOfBACnetCalendarEntry
{
    /** type of calendar entry value */
    CALENDAR_ENTRY_STATUS   m_eStatusCalendar;
    /** calendar entry value */
    Calendar_u              m_stCalendar;
    /** pointer for next Calendar entry */
    struct ListOfBACnetCalendarEntry *m_pstNext;
}ListOfBACnetCalendarEntry_t;

/**
*                          
*Name - Pr_ListOfBACnetCalendarEntry_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet calendar entry list/array.
*   
*/
typedef struct
{   
	/** count for no of nodes in list */ 
    uint32_t m_u32Count;
    /** List of Calendar */
    ListOfBACnetCalendarEntry_t *m_pstListOfCalendar;
}Pr_ListOfBACnetCalendarEntry_t;

/**
*                          
*Name - BACnetTimeValue_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet time value.
*   
*/
typedef struct BACnetTimeValue
{
    bool bIsUsed;
	/* Time for weekly schedule */
   	BACnetTime_t m_stTime;
	/* Value for weekly schedule */
   	BACNET_PROPERTY_VALUE 	m_stPropValue;
	/* for next element */
	struct BACnetTimeValue	*m_pstNext;
} BACnetTimeValue_t;

/**
*                          
*Name - Pr_BACnetDailySchedule_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet daily schedule.
*   
*/
typedef struct 
{
	/* Time Value */
    BACnetTimeValue_t m_stTimeValue;
}Pr_BACnetDailySchedule_t;

/**
*                          
*Name - Pr_ListOfBACnetDailySchedule_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet daily schedule.
*   
*/
typedef struct 
{
	/* Time Value */
    BACnetTimeValue_t m_stTimeValue[BACNET_ARRAY_OF_SEVEN];
}Pr_ListOfBACnetDailySchedule_t;


/**
*                          
*Name - BACnetSpecialEvent_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet special event.
*   
*/
typedef struct 
{
    union
    {    /** Date */
        BACnetDate_t m_stDate;
        /** Date Range */
        BACnetDateRange_t  m_stDateRange;
        /** Week and Day*/
        WeekNDay_t m_stWeekNDay; 
        /** Calendar Reference */
        struct{
            /** Type of Object*/
            BACNET_OBJECT_TYPE  m_eObjectType;
	        /** Object Instance*/     
            uint32_t        m_u32ObjId;
        }m_stCalReff;
        //CalendarObjId_t m_stCalReff;
    }m_stPeriod;

    /** type of special event value */
    CALENDAR_ENTRY_STATUS   m_eStatusCalendar;
    
    /** list Of Time Values */
    BACnetTimeValue_t       m_stListOfTimeValues;
    
    /** Event Priority (1..16) */
    uint32_t                m_u32EventPriority;
}BACnetSpecialEvent_t;

/**
*
*Name - ListOfSpecialEvent_t
* 
*DESCRIPTION 
*   Linked list of special events.
*   
*/
typedef struct ListOfSpecialEvent
{
    BACnetSpecialEvent_t    m_stListSpecialEvent;
    struct ListOfSpecialEvent  *m_pstNext;
}ListOfSpecialEvent_t;

/**
*                          
*Name - Pr_ListOfBACnetSpecialEvent_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type list/array of bacnet special events.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /** count for no of items in list */
    uint32_t m_u32Count;
    /** list / array member */
    ListOfSpecialEvent_t    *m_pstSplEvent;
}Pr_ListOfBACnetSpecialEvent_t;

#endif /* BACDEL_OBJ_SDL || BACDEL_OBJ_CAL */


/****************************************************************************
 **** Trend Log & Trend Log Multiple object properties.
 ****************************************************************************/

#if (defined PROPERTY_STRUCT)

/**
*                          
*Name - BacnetLogStatus_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type log status.
*   
*/
typedef struct 
{
    uint8_t m_u8UnusedBits;
    uint8_t m_u8ByteCnt;
    /** 
       First three MSB Bits are used as log status bits 
       Bit 7 -> log-disabled (0),
       Bit 6 -> buffer-purged (1),
       Bit 5 -> log-interrupted (2)
    */
    uint8_t   m_u8TransBits[MIN_BITSTRING_BYTES];
} BacnetLogStatus_t;

/**
*                          
*Name - logDatum_t 
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type log datum.
*
*NOTE - log datum is same as BACnetLogData.
*   
*/
typedef struct logDatum
{
    /** type of datum value */
    uint8_t m_TagType;
    /** value */
    union 
    {
        BacnetLogStatus_t       m_stlog_status;
        bool                    m_Boolean;       
        Float_t                 m_Real;
        uint32_t                m_Enum;
        uint32_t                m_Unsigned_Int;
        int32_t                 m_Signed_Int;
        Double_t                m_Double;
        uint8_t					m_Byte[MAX_BITSTRING_BYTES];
        BACnetBITStr_t          m_stBit_String;
        error_response_t        m_stError;
        Float_t                 m_fTimechange;
        BACNET_PROPERTY_VALUE   m_stAnyValue;
    }bacnetlogDatum_U;
}logDatum_t;

/**
*                          
*Name - BACnetLogRecord_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet log record.
*   
*/
typedef struct 
{
    /** time stamp */
    BACnetDateTime_t        m_stTimeStamp;
    /** log value */
    logDatum_t              m_stlogDatum;
    /** status flag */
    BACnetBitStr_t			m_stStatusFlag;
}BACnetLogRecord_t;

/**
*
*Name - ListOfBACnetLogRecord_t
* 
*DESCRIPTION 
*   Linked list of bacnet log records.
*   
*/
typedef struct ListOfBACnetLogRecord
{
    /** single log record value */
    BACnetLogRecord_t				m_stListofLogRecord;
    /** sequence no of log record */
    uint32_t						m_u32SequenceNo;
    /** for creating linklist */
    struct ListOfBACnetLogRecord	*m_pstNext; 
}ListOfBACnetLogRecord_t;

/**
*                          
*Name - Pr_ListOfBACnetLogRecord_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet log record array/list.
*   
*/
typedef struct 
{
	/** count for no of nodes in list */ 
    uint32_t 			m_u32Count;
    /** Variable to save previous value, for cov monitoring */
    PropertyValue_u     m_stPreviousValue;
    /** List of Log Records */
    ListOfBACnetLogRecord_t     *m_pstLogRecord;    
}Pr_ListOfBACnetLogRecord_t;

/**
*                          
*Name - BACnetClientCOV_t
*                                                                      
*DESCRIPTION 
*   This is structure of bacnet client cov increment.
*   
*/
typedef struct 
{
    /** value if tag is real */
    Float_t   m_fVal;
    /** variable to save data type - real or null */
	BACNET_APPLICATION_TAG m_eAppTagtype;
}BACnetClientCOV_t;

/**
*                          
*Name - Pr_BACnetClientCOV_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet client cov increment.
*   
*/
typedef struct 
{
	BACnetClientCOV_t m_stClientCOV;
}Pr_BACnetClientCOV_t;

/**
*                          
*Name - BACnetLogData_t 
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet log data.
*   
*/
typedef struct BACnetLogData
{
    /** type of datum value */
    uint8_t m_TagType;
    /** value */
    union 
    {
        BacnetLogStatus_t       m_stlog_status;
        bool                    m_Boolean;       
        Float_t                 m_Real;
        uint32_t                m_Enum;
        uint32_t                m_Unsigned_Int;
        int32_t                 m_Signed_Int;
        Double_t                m_Double;
        uint8_t					m_Byte[MAX_BITSTRING_BYTES];
        BACnetBITStr_t          m_stBit_String;
        error_response_t        m_stError;
        Float_t                 m_fTimechange;
        BACNET_PROPERTY_VALUE   m_stAnyValue;
    }logMultipleData_u;
    /** to create linklist */
    struct BACnetLogData  *m_pstNext;
}BACnetLogData_t;

/**
*                                                                         
*Name - ListOfBACnetLogMultipleRecord_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet log multiple record.
*   Linked list of bacnet log multiple records.
*   
*/
typedef struct ListOfBACnetLogMultipleRecord
{
    /** single log record value */
    /** time stamp */
    BACnetDateTime_t        m_stTimeStamp;
    /** list of log data values */
    BACnetLogData_t         m_stlogData;
    /** sequence no of log record */
    uint32_t				m_u32SequenceNo;
    /** for creating linklist */
    struct ListOfBACnetLogMultipleRecord	*m_pstNext;
}ListOfBACnetLogMultipleRecord_t;

/**
*                          
*Name - Pr_ListOfBACnetLogMultipleRecord_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet log multiple record list.
*   
*/
typedef struct 
{
	/** count for no of nodes in list */ 
    uint32_t    m_u32Count;
    /** Variable to save previous value, for cov monitoring */
    //PropertyValue_u     m_stPreviousValue;
    /** List of Log Records */
    ListOfBACnetLogMultipleRecord_t     *m_pstLogMultipleRecord;    
}Pr_ListOfBACnetLogMultipleRecord_t;

#endif /* (defined BACDEL_OBJ_TL || defined BACDEL_OBJ_TLM) */


/****************************************************************************
 **** Event Log object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_EL || defined PROPERTY_STRUCT)

/**
*                          
*Name - EventlogDatam_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type event log data.
*   
*/
typedef struct 
{
    /** type of datum value */
    uint8_t m_TagType;
    /** value */
    union 
    {
        BacnetLogStatus_t       m_stlog_status;
        Float_t                 m_fTimechange;
        BacnetEnPropElem_t      m_stNotification;
    }eventlogData_u;
}EventlogDatam_t;

/**
*                                                                         
*Name - ListOfBACnetEventLogRecord_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet event log record.
*   Linked list of bacnet event log records.
*   
*/
typedef struct ListOfBACnetEventLogRecord
{
    /** single log record value */
    /** time stamp */
    BACnetDateTime_t         m_stTimeStamp;
    /** event log data value */
    EventlogDatam_t          m_stlogData;
    /** sequence no of log record */
    uint32_t                 m_u32SequenceNo;
    /** for creating linklist */
    struct ListOfBACnetEventLogRecord	*m_pstNext;
}ListOfBACnetEventLogRecord_t;

/**
*                          
*Name - Pr_ListOfBACnetEventLogRecord_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet event log record list.
*   
*/
typedef struct 
{
	/** count for no of nodes in list */ 
    uint32_t m_u32Count;
    /** List of Event Log Records */
    ListOfBACnetEventLogRecord_t     *m_pstEventLogRecord;    
}Pr_ListOfBACnetEventLogRecord_t;

#endif /* BACDEL_OBJ_EL */


/****************************************************************************
 **** Loop object properties.
 ****************************************************************************/

#if ( defined PROPERTY_STRUCT)

/**
*
*Name - BACnetSetpointRef_t
* 
*DESCRIPTION 
*   This is structure of bacnet set point reference.
*   
*/
typedef struct 
{   
	/* BACnet Object property reference data */
	BACnetObjPropRef_t m_stObjPropRef;
    /** indicates set pt reff empty or not */
    bool            m_bDataFlag;
}BACnetSetpointRef_t;

/**
*
*Name - Pr_BACnetSetpointRef_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet set point reference.
*   
*/
typedef struct
{
	/* set point ref data */
	BACnetSetpointRef_t m_stSptRef;
}Pr_BACnetSetpointRef_t;

#endif /* BACDEL_OBJ_LOOP */


/****************************************************************************
 **** Command object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_CMD || defined PROPERTY_STRUCT)

/**
*                                                                         
*Name - ListOfBACnetActionCommand_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet action command.
*
*/
typedef struct ListOfBACnetActionCommand
{ 
    /** Type of Object*/
    BACNET_OBJECT_TYPE m_eDeviceType;
	/** Type of Object*/
    BACNET_OBJECT_TYPE m_eObjectType;
	/** Object Instance*/     
    uint32_t        m_u32DevId;
	/** Object Instance*/     
    uint32_t        m_u32ObjId;
	/** Property Identifier*/
    BACNET_PROPERTY_ID m_ePropertyIdentifier;
    /** Array Index & its flag */
    uint32_t        m_u32ArrayIndex;
    bool            m_bArrIndxFlag;
    /** if true terminate the execution of the 
    action list prematurely */
    bool            m_bQuitOnFailure;
    /** Current action write Successful */
    bool            m_bWriteSuccess;
    /** If the property being written is a commandable property, 
    then a priority value shall be supplied */
    uint32_t        m_u32Priority;
    /** To identify whether priority present */
    bool            m_bPriorityFlag;
    /** represent a delay in seconds */
    uint32_t        m_u32PostDelay;
    /** To identify whether PostDelay present */
    bool            m_bPostDelayFlag;
    /** Property Value to command */
	BACnetAbstractData_t        m_stPropPointer;
    /** List Of Action command */
    struct ListOfBACnetActionCommand    *m_pstNext;
}ListOfBACnetActionCommand_t;

/**
*
*Name - ListOfBACnetActionList_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet action command list.
*   
*/
typedef struct ListOfBACnetActionList
{
    /** list of action command */
    ListOfBACnetActionCommand_t    *m_pstActionCommand;
    bool							m_bTerminateList;
    struct ListOfBACnetActionList  *m_pstNext;
}ListOfBACnetActionList_t;

/**
*                                                                         
*Name - ActionElement_t
*
*DESCRIPTION 
*   member element for array of bacnet action list.
*
*/
typedef struct 
{
    int32_t						m_i32NoListElement;
    ListOfBACnetActionList_t   *m_pstArrayActCmd;
}ActionElement_t;

/**
*                                                                         
*Name - Pr_ListOfBACnetActionList_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet action list.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
	/** array or list count */
	uint32_t		m_u32Count;
    /** array of action list */
    ListOfBACnetActionList_t   *m_pstArrayActCmd;
}Pr_ListOfBACnetActionList_t;

#endif /* BACDEL_OBJ_CMD */


/****************************************************************************
 **** Group object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_GP || defined PROPERTY_STRUCT)

/**
*
*Name - ListOfBACnetPropRef_t
* 
*DESCRIPTION 
*   list of property reference.
*
*/
typedef struct ListOfBACnetPropRef
{
    /** Property Identifier*/
    BACNET_PROPERTY_ID  m_ePropertyIdentifier;
    /** Property_Array_Index */
    uint32_t            m_u32ArrayIndex;
    /** check if array index present  */
    bool                m_bIsArrayIndxPresent;
    /** to create list */
    struct ListOfBACnetPropRef    *m_pstNext;
}ListOfBACnetPropRef_t;

/**
*
*Name - ListOfReadAccessSpecs_t
* 
*DESCRIPTION 
*   structure for read access specification list.
*
*/
typedef struct ListOfReadAccessSpecs
{
    /** Type of Object */
    BACNET_OBJECT_TYPE  m_eObjectType;	
	/** Object Instance */     
    uint32_t            m_u32ObjId;
    /** List Of Prop */
    ListOfBACnetPropRef_t    m_stlistOfPropRef;
    /** Next Object */
    struct ListOfReadAccessSpecs   *m_pstNext;
}ListOfReadAccessSpecs_t;

/**
*
*Name - Pr_ListOfReadAccessSpecs_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet Read Access Specification.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
    /** count */
    uint32_t        m_u32Count;
    /** Group of obj n Prop */
	ListOfReadAccessSpecs_t       *m_pstGpOfObj;
}Pr_ListOfReadAccessSpecs_t;

/**
*
*Name - listOfResults_t
* 
*DESCRIPTION 
*   structure for list of read access results.
*
*/
typedef struct listOfResults
{
    /** Property Identifier*/
    BACNET_PROPERTY_ID  m_ePropertyIdentifier;
    /** Property_Array_Index */
    uint32_t            m_u32ArrayIndex;
    /** indicates if array index is present or not */
    bool                m_bArrIndxFlag; 
    /** Indicate memeber in union is in use */
    /** tag == 4 is property value
        tag == 5 is property access error */
    uint32_t            m_u32UnionMember;
    union 
    {
        /** Property Value  -- m_u32UniMember 1*/
        //BACNET_PROPERTY_VALUE m_stPropertyValue;
        /** Error -- m_u32UniMember 2 */
        error_response_t            m_stError;
        /* Pointer to hold property add -- m_u32UniMember 3 */
       BACnetAbstractData_t        m_stPropPointer;
    }m_stReadResult;
    /** to create list */
    struct listOfResults *m_pstNext;
}listOfResults_t;

/**
*
*Name - ListOfReadAccessResult_t
* 
*DESCRIPTION 
*   .
*
*/
typedef struct  ListOfReadAccessResult
{
    /** Type of Object */
    BACNET_OBJECT_TYPE  m_eObjectType;	
	/** Object Instance */     
    uint32_t            m_u32ObjId;
    /** list of results */
    listOfResults_t     m_stListOfResults;
    /** to create list */
    struct ListOfReadAccessResult *m_pstNext;
}ListOfReadAccessResult_t;

/**
*
*Name - Pr_ListOfReadAccessResult_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet Read Access Result.
*
*/
typedef struct  
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t     m_i32ActiveCOV;
#endif
    /** count */
    uint32_t   m_u32Count;
    /** list of values */
    ListOfReadAccessResult_t  *m_pstGpObjResults;	
}Pr_ListOfReadAccessResult_t;

#endif /* BACDEL_OBJ_GP */


/****************************************************************************
 **** Global Group object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_GGP || defined PROPERTY_STRUCT)

/**
*
*Name - ListOfBACnetPropAccessRslt_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of Property Access Result.
*
*/
typedef struct  ListOfBACnetPropAccessRslt
{
    /** Type of Object */
    BACNET_OBJECT_TYPE  m_eObjectType;
    /** Type of Device */
    BACNET_OBJECT_TYPE  m_eDevType;
	/** Object Instance */     
    uint32_t            m_u32ObjId;
	/** Device Instance */     
    uint32_t            m_u32DevId;
	/** Priority */
    uint32_t            m_u32Priority;
    /** */
    BACNET_PROPERTY_ID  m_ePropertyIdentifier;
    /** Property_Array_Index */
    uint32_t            m_u32ArrayIndex;
    /** check if array index present  */
    bool                m_bIsArrayIndxPresent;
    /** Indicate memeber in union is in use */
    /** tag == 4 is property value 
        tag == 5 is property access error */
    uint32_t            m_u32UniMember;
    union 
    {
        /** Property Value  -- m_u32UniMember 1*/
        BACnetAbstractData_t m_stPropertyValue;
        /** Error -- m_u32UniMember 2 */
        error_response_t            m_stError;       
    }m_stReadResult;
    /** Globle grp support only primitive data type */
    bool                m_bDataTypeNotSupproted;
    /** to create list */
    struct ListOfBACnetPropAccessRslt *m_pstNext;
}ListOfBACnetPropAccessRslt_t;

/**
*
*Name - Pr_ListOfBACnetPropAccessRslt_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet Property Access Result.
*
*/
typedef struct  
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** array size */
    uint32_t    m_u32ArraySize;
    /** array elements */
    ListOfBACnetPropAccessRslt_t  *m_pstPropAccRslt;	
}Pr_ListOfBACnetPropAccessRslt_t;

#endif /* BACDEL_OBJ_GGP */


/****************************************************************************
 **** Load Control object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_LC || defined PROPERTY_STRUCT)

/**
*
*Name - BACnetShedLevel_t
* 
*DESCRIPTION 
*   union for shed level
*
*/
typedef struct 
{
    /** type of shed level */
    BACNET_SHED_LEVEL     m_eShedLevelType;
    union{
        /** percent value */     
        uint32_t    m_u32Percent;
	    /** level value */     
        uint32_t    m_u32Level;
	    /** amount value */
        Float_t     m_fAmount;
    }shedlevel_u;
}BACnetShedLevel_t;

/**
*
*Name - Pr_BACnetShedLevel_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet shed level.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t     m_i32ActiveCOV;
#endif
	/** value of shed level */
    BACnetShedLevel_t    m_stShedLevel;
}Pr_BACnetShedLevel_t;

#endif /* BACDEL_OBJ_LC */


/****************************************************************************
 **** Credential Data Input object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_CDI || defined PROPERTY_STRUCT)

/**
*                          
*Name - Pr_BACnetAuFactor_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet Authentication Factor.
*   
*/
typedef struct 
{   
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t m_i32ActiveCOV;
#endif
    /** Authentication Factor value */
    BACnetAuFactor_t  m_stAuFactor;
}Pr_BACnetAuFactor_t;


/**
*                          
*Name - ListOfBACnetAuFactorFormat_t
* 
*DESCRIPTION 
*   array of Authentication Factor format.
*   
*/
typedef struct ListOfBACnetAuFactorFormat
{
    /** type of authentication format */
    BACNET_AUTHENTICATION_FACTOR_TYPE     m_eFormatType;
    /** vendor id */     
    uint16_t        m_u16VendorId;
    /** vendor format */
    uint16_t        m_u16VendorFormat;
	/** flags for vendor id and vendor format */
	bool			m_bVIdFlag;
	bool			m_bVFormatFlag;
    /** to create array/list */
    struct ListOfBACnetAuFactorFormat   *m_pstNext;
}ListOfBACnetAuFactorFormat_t;

/**
*                          
*Name - Pr_ListOfBACnetAuFactorFormat_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet Authentication Factor format.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t     m_i32ActiveCOV;
#endif
    /** array size count */
    uint32_t   m_u32ArraySize;
    /** elements of array */
    ListOfBACnetAuFactorFormat_t  *m_pstAuFactFormatList;
}Pr_ListOfBACnetAuFactorFormat_t;

#endif /* BACDEL_OBJ_CDI */


/****************************************************************************
 **** Network Security object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_NS || defined PROPERTY_STRUCT)

/**
*
*Name - ListOfBACnetNwSecurityPolicy_t
* 
*DESCRIPTION 
*   array of Network Security Policy.
*   
*/
typedef struct ListOfBACnetNwSecurityPolicy
{
    /** port id */
    uint8_t   m_u8PortId;
    /** security policy */     
    BACNET_SECURITY_POLICY   m_eSecurityPolicy;
    /** to create list */
    struct ListOfBACnetNwSecurityPolicy  *m_pstNext;
}ListOfBACnetNwSecurityPolicy_t;

/**
*                          
*Name - Pr_ListOfBACnetNwSecurityPolicy_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet network security policy.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B  
	/** COV Status Specifier */ 
    int32_t m_i32ActiveCOV;
#endif
    /** array size count */
    uint32_t   m_u32ArraySize;
    /** elements of array */
    ListOfBACnetNwSecurityPolicy_t  *m_pstSecurityPolicyArr;
}Pr_ListOfBACnetNwSecurityPolicy_t;


/**
*                          
*Name - ListOfBACnetKeyId_t
* 
*DESCRIPTION 
*   struct for bacnet key identifier.
*   
*/
typedef struct ListOfBACnetKeyId
{
    /** algorithm value */
    uint8_t   m_u8Algorithm;
    /** key id value */
    uint8_t   m_u8KeyId;
	/** to create list */
    struct ListOfBACnetKeyId  *m_pstNext;
}ListOfBACnetKeyId_t;

/**
*                          
*Name - Pr_ListOfBACnetKeyId_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet key identifier.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /** value of key identifier */
    ListOfBACnetKeyId_t    m_stKeyId;
}Pr_ListOfBACnetKeyId_t;

/**
*
*Name - BACnetSecurityKeySet_t
* 
*DESCRIPTION 
*   structure for Security KeySet.
*   
*/
typedef struct 
{
    /** port id */
    uint8_t					m_u8KeyRevision;
    /** activation time */
    BACnetDateTime_t        m_stActivationTime;
    /** expiration time */
    BACnetDateTime_t        m_stExpirationTime;
    /** key identifier */
    ListOfBACnetKeyId_t     m_stKeyIds;
    /** to create list */
}BACnetSecurityKeySet_t;

/**
*                          
*Name - Pr_ListOfBACnetSecurityKeySet_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet security keyset.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /** array size count */
    uint32_t   m_u32ArraySize;
    /** elements of array */
    BACnetSecurityKeySet_t  m_stSecurityKetSet[BACNET_ARRAY_OF_TWO];
}Pr_ListOfBACnetSecurityKeySet_t;

/**
*                          
*Name - Pr_BACnetSecurityKeySet_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet security keyset.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B
    /** COV Status Specifier */
    int32_t m_i32ActiveCOV;
#endif
    /** elements of array */
    BACnetSecurityKeySet_t  m_stSecurityKetSet;
}Pr_BACnetSecurityKeySet_t;


#endif /* BACDEL_OBJ_NS */


/****************************************************************************
 **** Access Point object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_AP || defined PROPERTY_STRUCT)

/**
*                          
*Name - Policy_t
* 
*DESCRIPTION 
*   struct for authentication policy.
*   
*/
typedef struct Policy
{
    /** device object reference */
    BACnetDevObjRef_t  m_stCredentialDataInput;
    /** index value */
    uint32_t   m_u32Index;
	/** to create array/list */
    struct Policy  *m_pstNext;
}Policy_t;

/**
*                          
*Name - ListOfBACnetAuPolicy_t
* 
*DESCRIPTION 
*   array of Authentication Policy.
*   
*/
typedef struct ListOfBACnetAuPolicy
{
    /** timeout value */
    uint32_t   m_u32Timeout;
    /** order enforced */
    bool     m_bOrderEnforced;
    /** policy */
    Policy_t   m_stPolicy;
    /** to create array/list */
    struct ListOfBACnetAuPolicy  *m_pstNext;
}ListOfBACnetAuPolicy_t;

/**
*                          
*Name - Pr_ListOfBACnetAuPolicy_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet Authentication Policy.
*   
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t     m_i32ActiveCOV;
#endif
    /** array size */
    uint32_t   m_u32ArraySize;
    /** array/list elements */
    ListOfBACnetAuPolicy_t  *m_pstAuPolicy;
}Pr_ListOfBACnetAuPolicy_t;

#endif /* BACDEL_OBJ_AP */


/****************************************************************************
 **** Access Rights object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_AR || defined PROPERTY_STRUCT)

/**
*
*Name - ListOfBACnetAccessRule_t
* 
*DESCRIPTION 
*   struct for array/list of access rule.
*
*/
typedef struct ListOfBACnetAccessRule
{
    /** time range specifier */
    BACNET_TIME_RANGE_SPECIFIER   m_eTimeRangeType;
    /** location specifier */
    BACNET_LOCATION_SPECIFIER     m_eLocationType;
    /** time range - optional */
    BACnetDevObjPropRef_t		  m_stTimeRange;
    /** location - optional */
    BACnetDevObjRef_t			  m_stLocation;
    /** enable value */
    bool						  m_bEnable;
    /** to create list/array */
    struct ListOfBACnetAccessRule *m_pstNext;
}ListOfBACnetAccessRule_t;

/**
*
*Name - Pr_ListOfBACnetAccessRule_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet Access Rule.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** array size */
    uint32_t   m_u32ArraySize;
    /** array/list elements */
    ListOfBACnetAccessRule_t  *m_pstAccessRuleArray;
}Pr_ListOfBACnetAccessRule_t;

#endif /* BACDEL_OBJ_AR */


/****************************************************************************
 **** Access Credential object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_AC || defined PROPERTY_STRUCT)

/**
*
*Name - ListOfBACnetCredAuFactor_t
* 
*DESCRIPTION 
*   struct for array/list of Credential Authentication Factor.
*
*/
typedef struct ListOfBACnetCredAuFactor
{
    /** disable value */
    BACNET_ACCESS_AUTHENTICATION_FACTOR_DISABLE   m_eDisable;
    /** authentication factor */
    BACnetAuFactor_t  m_stAuFactor;
    /** to create list/array */
    struct ListOfBACnetCredAuFactor   *m_pstNext;
}ListOfBACnetCredAuFactor_t;

/**
*
*Name - Pr_ListOfBACnetCredAuFactor_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet Credential Authentication Factor.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
    /** array size */
    uint32_t   m_u32ArraySize;
    /** array/list elements */
    ListOfBACnetCredAuFactor_t  *m_pstCredAuFactArray;
}Pr_ListOfBACnetCredAuFactor_t;


/**
*
*Name - ListOfBACnetAssignedAccessRights_t
* 
*DESCRIPTION 
*   struct for array/list of Assigned Access Rights.
*
*/
typedef struct ListOfBACnetAssignedAccessRights
{
    /** assigned access rights value */
    BACnetDevObjRef_t      m_stAssAccessRights;
    /** enable value */
    bool    m_bEnable;
    /** to create list/array */
    struct ListOfBACnetAssignedAccessRights   *m_pstNext;
}ListOfBACnetAssignedAccessRights_t;

/**
*
*Name - Pr_ListOfBACnetAssignedAccessRights_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type array of bacnet Assigned Access Rights.
*
*/
typedef struct 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t     m_i32ActiveCOV;
#endif
    /** array size */
    uint32_t   m_u32ArraySize;
    /** array/list elements */
    ListOfBACnetAssignedAccessRights_t  *m_pstAsngdAccessArray;
}Pr_ListOfBACnetAssignedAccessRights_t;

#endif /* BACDEL_OBJ_AC */


/****************************************************************************
 **** Channel object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_CHNL || defined PROPERTY_STRUCT)

/**
*
*Name - BACnetLightingCommand_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type lighting commands.
*
*/
typedef struct BACnetLightingCommand 
{
	/* fade time */
	uint32_t m_u32FadeTime;
	/* target level */
	Float_t m_fTargetLevel;
	/* ramp rate */
	Float_t m_fRampRate;
	/* step increment */
	Float_t m_fStepIncrement;
	/* priority */
	uint8_t m_u8Priority;
	/* flags for optional commands */
	bool m_bFadeTimeFlag;
	bool m_bPriorityFlag;
	bool m_bTargetLevelFlag;
	bool m_bRampRateFlag;
	bool m_bStepIncrementFlag;
	/* lighting operations */
	BACNET_LIGHTING_OPERATION	m_eOperation;
}BACnetLightingCommand_t;

/**
*
*Name - Pr_BACnetLightingCommand_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type lighting commands.
*
*/
typedef struct Pr_BACnetLightingCommand 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** cov status specifier */  
    int32_t     m_i32ActiveCOV;
#endif
	/* lighting command value */
	BACnetLightingCommand_t m_stLightingCmd;
}Pr_BACnetLightingCommand_t;

/**
*
*Name - BACnetChannelValue_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type channel value.
*
*/
typedef struct BACnetChannelValue
{
	/* data type */
	BACNET_DATA_TYPE		m_eDataType;
	/* application tag type */
	BACNET_APPLICATION_TAG	m_eAppTag;
	/* channel value */
	union{
		/* primitive data type value */
		BACNET_PROPERTY_VALUE   m_stPrimitiveValue;
		/* lighting command value */
		BACnetLightingCommand_t m_stLightCmdValue;
	}uChannelVal;
}BACnetChannelValue_t;

/**
*
*Name - Pr_BACnetChannelValue_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type channel value.
*
*/
typedef struct Pr_BACnetChannelValue 
{
#ifdef BACDEL_SER_DS_COV_B 
	/** cov status specifier */  
    int32_t     m_i32ActiveCOV;
#endif
	/* channel value */
	BACnetChannelValue_t m_stChannelValue;
}Pr_BACnetChannelValue_t;

/**
*
*Name - BACnetGroupChannelValue_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type group channel value.
*
*/

typedef struct BACnetGroupChannelValue 
{
	/** channel number */
	uint16_t m_u16Channel;
	/** overriding priority */
	uint8_t m_u8OverridingPriority;
	/** channel value */
	BACnetChannelValue_t m_stValue;
	/** group channel value */
	struct BACnetGroupChannelValue *m_pstNext;
}BACnetGroupChannelValue_t;

#endif /* BACDEL_OBJ_CHNL */


/****************************************************************************
 **** Notification Forwarder object properties.
 ****************************************************************************/

#if (defined BACDEL_OBJ_NF || defined PROPERTY_STRUCT)

/**
*
*Name - BACnetEventNotifySubs_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of Event Notification Subscription.
*   
*/
typedef struct BACnetEventNotifySubs
{
	/** Address of device to which notification is to be send */
    BACnetRecipient_t	m_stRecepient;
	/** Process identifier */
	uint32_t			m_u32ProcessIdentifier;
	/** Time remaining */
	uint32_t			m_u32TimeRemaining;
	/** True if confirmed notifications are to be sent and
	false if unconfirmed notifications are to be sent */
	bool				m_bIssueConfirmedNotify;
}BACnetEventNotifySubs_t;

/**
*                                                                         
*Name - Pr_BACnetEventNotifySubs_t                                         
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type BACnet Event Notification Subscription.
*   
*/
typedef struct Pr_BACnetEventNotifySubs
{
#ifdef BACDEL_SER_DS_COV_B
	/** COV Status Specifier */   
    int32_t				m_i32ActiveCOV;
#endif
    /** Event notification subscription Structure */
    BACnetEventNotifySubs_t  m_stEventNotifySubs;
}Pr_BACnetEventNotifySubs_t;

/**
*
*Name - ListOfBACnetEventNotifySubs_t
* 
*DESCRIPTION 
*   Linked list of Event Notification Subscription data.
*   
*/
typedef struct ListOfBACnetEventNotifySubs
{
	/** Event notification subscription value */
    BACnetEventNotifySubs_t     m_stEventNotifySubs;
	/** Next element */        
    struct ListOfBACnetEventNotifySubs   *m_pstNext;  
}ListOfBACnetEventNotifySubs_t;

/**
*                          
*Name - Pr_ListOfBACnetEventNotifySubs_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type event notification subscription list.
*   
*/
typedef struct Pr_ListOfBACnetEventNotifySubs
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** Event Notification Subscription List count */
    uint32_t    m_u32Count;
    /** pointer to list of values */
    ListOfBACnetEventNotifySubs_t   *m_pstEventNotiSubList;
}Pr_ListOfBACnetEventNotifySubs_t;

/**
*
*Name - BACnetProcessIdSelection_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of Process Id Selection.
*   
*/
typedef struct BACnetProcessIdSelection
{
	/** Process identifier */
	uint32_t				m_u32ProcessIdentifier;
	/** Variable to save data type - null or unsigned */
	BACNET_APPLICATION_TAG	m_eAppTagtype;
}BACnetProcessIdSelection_t;

/**
*                                                                         
*Name - Pr_BACnetProcessIdSelection_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet process id selection.
*   
*/
typedef struct Pr_BACnetProcessIdSelection
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
	/* Process Id Selection data */
	BACnetProcessIdSelection_t m_stProcessIdSelection;
}Pr_BACnetProcessIdSelection_t;

/**
*
*Name - BACnetPortPermission_t
*
*DESCRIPTION 
*   This structure defines the property datatype 
*   of Port Permission.
*   
*/
typedef struct BACnetPortPermission
{
	/** port id */
	uint8_t	m_u8PortId;
	/** enabled */
	bool	m_bEnabled;
}BACnetPortPermission_t;

/**
*                                                                         
*Name - Pr_BACnetPortPermission_t
*                                                                      
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type bacnet port permission..
*   
*/
typedef struct Pr_BACnetPortPermission
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */  
    int32_t m_i32ActiveCOV;
#endif
	/* Port Permission data */
	BACnetPortPermission_t m_stPortPermission;
}Pr_BACnetPortPermission_t;

/**
*
*Name - ListOfBACnetPortPermission_t
* 
*DESCRIPTION 
*   Linked list of Port Permission data.
*   
*/
typedef struct ListOfBACnetPortPermission
{
	/** Port Permission value */
    BACnetPortPermission_t         m_stPortPermission;
	/** Next element */        
    struct ListOfBACnetPortPermission   *m_pstNext;  
}ListOfBACnetPortPermission_t;


/**
*                          
*Name - Pr_ListOfBACnetPortPermission_t
* 
*DESCRIPTION 
*   This structure defines the property datatype 
*   of type port permission list.
*   
*/
typedef struct Pr_ListOfBACnetPortPermission
{
#ifdef BACDEL_SER_DS_COV_B 
	/** COV Status Specifier */
    int32_t     m_i32ActiveCOV;
#endif
    /** Port Permission List count */
    uint32_t    m_u32Count;
    /** list of port permision values */
    ListOfBACnetPortPermission_t   *m_pstPortPermissionList;
}Pr_ListOfBACnetPortPermission_t;

#endif /* BACDEL_OBJ_NF */


/** structure to save BACnet Config Flash data */
typedef struct
{
	NVHeader_t 			g_Header;

	/** Database Revision */
	uint32_t m_u32DatabaseREV;

	/** UTC Offset */
	int32_t m_u32UTCOffset;

	/** Day Light Saving Status */
	bool m_bDaylightSavingStatus;

	/** address of the recipient for the notification */
	BACnetRecipient_t		m_stRecipient[2];

	/** FDT Entry */
	BACnetFDTEntry_t  m_stFDTEntry[MAX_BDT_FDT_FD_ENTRIES];

	/** BDT Entry */
	BACnetBDTEntry_t  m_stBDTEntry[MAX_BDT_FDT_FD_ENTRIES];

	/** MAX Info Frame */
	uint32_t m_u32MaxInfoFrame;



	/** Recipient Entry Count */
	uint8_t m_u8RecipientCount;

	/** FDT Entry Count */
	uint8_t m_u8FDTEntryCount;

	/** BDT Entry Count */
	uint8_t m_u8BDTEntryCount;

	/** Date Rane */
	BACnetDateRange_t     m_stDateRange;

	/** Stack Selection */
	BACNET_STACK_SELECTION m_eBACStack;
	BACnetCharStr_t m_stPassword;

}Pr_BACnetConfigData_t;

extern Pr_BACnetConfigData_t BACnetConfigData;


//End Newly Added
#endif /* BAC_PROPERTY_DEF_H */
