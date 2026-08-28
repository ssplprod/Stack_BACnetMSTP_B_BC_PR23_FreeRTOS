/**********************************************************************************
*
*                   Copyright (c) by SoftDEL Systems Pvt. Ltd.
*
*   This software is copyrighted by and is the sole property of SoftDEL
*   Systems Pvt. Ltd. All rights, title, ownership, or other interests in the
*   software remain the property of  SoftDEL Systems Pvt. Ltd. This software
*   may only be used in accordance with the corresponding license
*   agreement. Any unauthorized use, duplication, transmission,
*   distribution, or disclosure of this software is expressly forbidden.
*
*   This Copyright notice may not be removed or modified without prior
*   written consent of SoftDEL Systems Pvt. Ltd.
*
*   SoftDEL Systems Pvt. Ltd. reserves the right to modify this software
*   without notice.
*
*   SoftDEL Systems Pvt. Ltd.						india@softdel.com
*   3rd Floor, Pentagon P4,						http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028
*
*   FILE
*	objNetworkPort.h
*
*   AUTHORS
*   Shruti Kulkarni, Pratham Murkute
*
*   DESCRIPTION
*	Functions, array related to network port object.
*
**********************************************************************************/

#ifndef NP_H
#define NP_H
#ifdef BACDEL_OBJ_NP

/** header includes */
#include "bacDELDef.h"
#include "propertydef.h"

/** global variables */
extern uint32_t g_u32NetworkPortId;

/** 
* 
* NAME: NpChangesPending_t
* 
* DESCRIPTION 
* Structure defines the changes pending properties.
*   
*/
typedef struct NpChangesPending
{
	uint32_t m_btNetworkNo			:1;	// bit 0
	uint32_t m_btLinkSpeed			:1;
	uint32_t m_btMacAddress			:1;
	uint32_t m_btIPMode				:1;
	uint32_t m_btIPAddress			:1;
	uint32_t m_btIPUdpPort			:1;
	uint32_t m_btIPSubnetMask		:1;
	uint32_t m_btIPDefaultGateway	:1;	// bit 7

	uint32_t m_btBbmdBdt			:1;	// bit 8
	uint32_t m_btBbmdAcceptFD    	:1;
	uint32_t m_btFdBbmdAddress		:1;
	uint32_t m_btFDSubsLifetime		:1;
	uint32_t m_btMaxMaster			:1;
	uint32_t m_btUnused5			:1;
	uint32_t m_btUnused6			:1;
	uint32_t m_btUnused7			:1;	// bit 15

}NpChangesPending_t;

/**
*
* NAME: Network Port
* 
* DESCRIPTION 
* BACnet network port object structure with all properties.
*   
*/
typedef struct NetworkPort
{
    /** object identifier */
    Pr_BACnetObjId_t				m_stObjectID;
    
    /** object name */
    Pr_BACnetCharStr_t				m_stObjName;

    /** the value of this property shall be networkport */
    Pr_BACnetObjType_t				m_stObjectType;
	
	/** represents four boolean flags that indicate the general "health" of 
	an network port */
	Pr_BACnetBitStr_t				m_stStatusFlag;

	/** an indication whether (true) or not (false) the physical input that the 
	object represents is not in service */
	Pr_BACnetBool_t					m_stOutOfService;

	/** represents the type of network this network port object is representing */
	Pr_BACnetNetworkType_t			m_stNetworkType;

	/** indicates whether the object represents a physical network interface, a non-bacnet 
	protocol, the bacnet use of the protocol, or a non-bacnet use of the protocol */
	Pr_BACnetProtocolLevel_t		m_stProtocolLevel;

	/** represents the BACnet network number associated with this network */
	Pr_BACnetUnsigned16_t			m_stNetworkNo;

	/** represents the current quality of the network_number property */
	Pr_BACnetNetworkNoQuality_t		m_stNetworkNoQuality;

	/** indicates whether the configuration settings in the network port object map 
	to the current configuration settings */
	Pr_BACnetBool_t					m_stChangesPending;

	/** maximum number of octets that may be contained in a single indivisible application
	protocol data unit sent or received on this port */
	Pr_BACnetUnsigned32_t			m_stAPDULength;

	/** represents the network communication rate as the number of bits per second. a value of 0
	indicates an unknown communication rate */
	Pr_BACnetReal_t					m_stLinkSpeed;

	//	/** represents the network communication rate as the number of bits per second. a value of 0
	Pr_BACnetArrReal_t				m_stLinkSpeeds;

	/* changes pending properties */
	NpChangesPending_t				m_stNpChangesPending;

	#ifdef BACDEL_SER_DS_COV_B
    /** cov variable which describes the cov service status */
    int32_t							m_i32ObjCOVSupport;
	#endif

	#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object. 
	the object_name, object_type, object_identifier, and property_list 
	properties are not included in the list */
    Pr_BACnetPropertyList_t    		m_stPropertyList;
	#endif

	#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B)
	/** provides an indication of whether the present_value or the operation of
	the physical input in question is "reliable" as far as the bacnet
	device or operator can determine and, if not, why? **/
	Pr_BACnetReliability_t			m_stReliability;
	#endif

	//#ifdef OPTIONAL_PROPERTY
	/** name of an object profile to which this object conforms */
    Pr_BACnetCharStr_t				m_stProfileName;

    /** description about the object is a string of printable characters 
	whose content is not restricted */
    Pr_BACnetCharStr_t				m_stDescription;

	/** contains the bacnet mac address used on this network */
	Pr_BACnetOctetStr_t				m_stMacAddress;

	/** indicates the bacnet/ip mode of this network port */
	Pr_BACnetIPMode_t				m_stIPMode;

	/** indicates the ip address of this network port */
	Pr_BACnetOctetStr_t				m_stIPAddress;

	/** indicates the udp port number of this network port */
	Pr_BACnetUnsigned16_t			m_stIPUdpPort;

	/** indicates the subnet mask for this network */
	Pr_BACnetOctetStr_t				m_stIPSubnetMask;

	/** indicates the ip address of the default gateway for this network */
	Pr_BACnetOctetStr_t				m_stIPDefaultGateway;

	/** indicates the dns server used by this network port for internet host
	name resolution */
	Pr_ListOfOctetStr_t				m_stIPDnsServer;

	/** bbmd broadcast distribution table */
	Pr_ListOfBACnetBDTEntry_t		m_stBbmdBdt;

	/** backup of bbmd broadcast distribution table */
	Pr_ListOfBACnetBDTEntry_t		m_stBbmdBdtBackup;

	/** bbmd accept fd registrations */
	Pr_BACnetBool_t					m_stBbmdAcceptFD;

	/** bbmd foreign device table */
	Pr_ListOfBACnetFDTEntry_t		m_stBbmdFdt;

	/** fd bbmd address */
	Pr_BACnetHostNPort_t			m_stFDBbmdAddr;

	/** fd subscriptions lifetime */
	Pr_BACnetUnsigned16_t			m_stFDSubscriptionsLifetime;
	//#endif /* optional */ 

	/** Shall be present if the device is a master node on an MS/TP network */
	Pr_BACnetUnsigned32_t           m_stMAXMaster;

	/** backup of MaxMaster master node on an MS/TP network */
	Pr_BACnetUnsigned32_t           m_stMAXMasterBackup;		//SK


	/** Shall be present if the device is a node on an MS/TP network*/
	Pr_BACnetUnsigned32_t           m_stMaxInfoFrames;

	/* this flag bit is added to for managing callback registration */
    bool					bAppCallBack;

    /** for dynamic object add/delete */
	struct  NetworkPort		*pstNext;

}NetworkPort_t;

/**
*
* DISCREPTION
* Function is used to assign property list for network port object.
* this list is used in rpm service to determine all supported properties.
*
* @param pRequired	  [out] pointer to required property list
* @param pOptional	  [out] pointer to optional property list
* @param pProprietary [out] pointer to proprietary property list
*
*/
void NetworkPort_Property_Lists(
    int32_t **pRequired,
    int32_t **pOptional,
    int32_t **pProprietary);

/**
*
* DESCRIPTION 
* Function to add network port object.
*
* @param u32DeviceId	[in] device id
* @param u32ObjId		[in] object id for new object
* @param pstObjName		[in] object name for new objects
* @param eNetworkType	[in] network type of this object
*
* @return [out] success or suitable error code
*
*/
BACNET_RETURN_TYPE NetworkPort_Add_Object(
	uint32_t u32DeviceId,
	uint32_t u32ObjId,
	BACnetCharStr_t *pstObjName,
	BACNET_NETWORK_TYPE eNetworkType);

/**
*
* DESCRIPTION 
* Function to delete network port objects.
*
* @param pVirtualDev	[in] device data
* @return [out] success or suitable error code
*
*/
BACNET_RETURN_TYPE NetworkPort_Delete_Object(void *pVirtualDev);

/**
*
* DESCRIPTION 
* This API Initialize the network port object properties.
*
* @param pstNpObj		[in] network port object base address
* @param u32ObjectId	[in] object id for new object
* @param eNetworkType	[in] network type of this object
* @param pstObjName		[in] object name for new objects
*
* @return [out] void
*
*/
void NetworkPort_Initialize_Properties(
	NetworkPort_t *pstNpObj,
	uint32_t u32ObjectId, 
	BACNET_NETWORK_TYPE eNetworkType,
	BACnetCharStr_t *pstObjName);

/**
*
* DESCRIPTION                                                                          
* Function to write property of network port object.    
*
* @param eObjectProperty	[in] object property.
* @param u32ArrayIndex		[in] array index.
* @param bArrIndxPresent	[in] array index flag.
* @param bRecursiveCall		[in] recursion flag.
* @param u8Priority			[in] writing priority.
* @param pVirtualDev		[in] void pointer for virtual device data.
* @param pvData				[in] property value to be written.
* @param ePermission		[in] propperty access type.
* @param pstBaseAddr		[in] objects base address
* @param bLMServiceReq		[in] true if call is received from LM-B service
*
* @return [out] true if success else false.
*
*/
bool NetworkPort_Write_Property(
	BACNET_PROPERTY_ID eObjectProperty,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent,
	bool bRecursiveCall,
	uint8_t u8Priority,
	void *pVirtualDev,
	void *pvData,
	PROP_ACCESS_TYPE ePermission,
	void *pvObjAddr,
	void **pvPropAdd,
	bool bLMServiceReq);

#ifdef BBMD_ENABLED
/** 
* 
* DESCRIPTION
* This function activate changes relatd to BDT in network port object
* when reinitialize device with ACTIVATE_CHANGES is received or when application 
* calls set object property API.
* 
* @param pVirtualDev [in] virtual device data. 
* @returns success or suitable error code.
* 
*/
BACNET_RETURN_TYPE NetworkPort_Activate_BDT_Changes(void *pvVirtualDev);

/**
*
* DESCRIPTION
* Function to backup the data currently present in the 
* BBMD BDT and BBMD FDT properties.
* The data is stored in a .bin file on file system.
*   
* @param  [in]  No input parameters
* @return [out] No output parameters
*
*/
void Update_BBMD_File();

/**
*
* DESCRIPTION
* Function to read the data currently present in the 
* BBMD file and create BDT and FDT linked list.
* The data is read from a .bin file previously saved on file system.
*   
* @param  [in]  No input parameters
* @return [out] No output parameters
*
*/
void Read_BBMD_File(void);
#endif /* BBMD */
#endif /* NP object */
#endif /* NP_H */

/******************************** end of header file *************************/
