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
*	objNetworkPort.c
*
*   AUTHORS
*   Shruti Kulkarni, Pratham Murkute
*
*   DESCRIPTION
*	Functions, array related to network port object.
*
**********************************************************************************/

#ifdef BACDEL_OBJ_NP

/** include header files */
#include "objNetworkPort.h"
#include "miscMiscellaneous.h"
#include "propertyGenricHandler.h"
#include "propertyValueRead.h"
#include "propertyValueWrite.h"
#include "propertyClearValues.h"
#include "objDevice.h"
#include "DataB.h"
/* to include IP layer functions */
#include"datalinkMSTP.h"
/* to include bbmd layer functions */

/** external global variables */
extern bool g_bBBMDEnabled;
extern uint8_t g_au8ProfileName[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8ObjDescription[MAX_SUPPORTED_CHRSTR_LEN];
extern Pr_BACnetConfigData_t BACnetConfigData;

/** global variables */
uint32_t g_u32NetworkPortId = 0;
extern uint32_t enumToBaud(uint8_t Baudrate);
extern DB_t SMCfg;
/** required properties */
int32_t NetworkPort_Properties_Required[MAX_PROPERTY_LIST] = 
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
	PROP_STATUS_FLAGS,
	PROP_RELIABILITY,
	PROP_OUT_OF_SERVICE,
	PROP_NETWORK_TYPE,
	PROP_PROTOCOL_LEVEL,
	/* commented as per http://www.bacnet.org/Interpretations/IC135-2016-1.pdf */
	PROP_NETWORK_NUMBER,
	PROP_NETWORK_NUMBER_QUALITY,
	PROP_CHANGES_PENDING,
	PROP_APDU_LENGTH,
	PROP_MAC_ADDRESS,

	ENDOFPROPLIST
 };

/** optional properties */
int32_t NetworkPort_Properties_Optional[MAX_PROPERTY_LIST] =
{
	PROP_DESCRIPTION,
	PROP_MAX_INFO_FRAMES,
	PROP_MAX_MASTER,
	PROP_LINK_SPEED,
	ENDOFPROPLIST
};

/** proprietary properties */
int32_t NetworkPort_Properties_Proprietary[MAX_PROPERTY_LIST] =
{
    ENDOFPROPLIST
};

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
    int32_t **pProprietary)
{
    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
	"APDU: NetworkPort_Property_Lists: Entry \r\n");
	#endif

    /* save array pointers */
    if(pRequired)
	{
		/* required properties */
        *pRequired = NetworkPort_Properties_Required;
	}
    if(pOptional)
	{
		/* optional properties */
        *pOptional = NetworkPort_Properties_Optional;
	}
    if(pProprietary)
	{
		/* proprietary properties */
        *pProprietary = NetworkPort_Properties_Proprietary;
	}

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
	"APDU: NetworkPort_Property_Lists: Exit \r\n");
	#endif
    return;
}

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
	BACNET_NETWORK_TYPE eNetworkType)
{
	/* local variables */
	virtualDevData_t *pVirtualDev = NULL;
	void *pvNewObject = NULL;
	uint32_t u32Size = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
		"APDU: NetworkPort_Add_Object: Entry \r\n");
	#endif

	/* check for valid instance no */
	if(u32DeviceId >= BACNET_MAX_INSTANCE || u32ObjId >= BACNET_MAX_INSTANCE)
	{
		/* invalid ids*/
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_MAX_INSTANCE_ERROR, 
			"APDU: NetworkPort_Add_Object: invalid object instance no \r\n");
		#endif
		return BACDEL_MAX_INSTANCE_ERROR;
	}

	/* traverse the host device list */
	pVirtualDev = (virtualDevData_t *)Find_In_Host_Device_List(u32DeviceId);
	if(NULL == pVirtualDev)
	{
		/* device not found */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DEVICE_NOT_PRESENT, 
			"APDU: NetworkPort_Add_Object: device not found \r\n");
		#endif
		return BACDEL_DEVICE_NOT_PRESENT;
	}

	/* get the object type size */
	GET_OBJECT_SIZE(OBJECT_NETWORK_PORT, u32Size);
	if(0 == u32Size)
	{
		/* object type error */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, 
			"APDU: NetworkPort_Add_Object: object type is incorrect \r\n");
		#endif
		return BACDEL_OBJECT_TYPE_ERROR;
	}

	/* object count in a device cannot become more than max limit */
	if(pVirtualDev->m_stDevObject.m_stObjectIDList.m_u32ObjCount >= 
		g_stStackMaxLimits.m_u32MaxObject)
	{
		/* max object count exceeded */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_CANNOT_BE_CREATED, 
			"APDU: NetworkPort_Add_Object: max object count exceeded \r\n");
		#endif
		return BACDEL_OBJECT_CANNOT_BE_CREATED;
	}

	/* object name should not be same as any previous object */
	if(NULL == pstObjName)
	{
		/* invalid input pointer */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, 
			"APDU: NetworkPort_Add_Object: invalid input pointer \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}
	else if(BACDEL_DUPLICATE_OBJECT_NAME == Check_Object_Name(u32DeviceId,
		(int8_t *)pstObjName, NULL, NULL))
	{
		/* duplicate object name */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DUPLICATE_OBJECT_NAME, 
			"APDU: NetworkPort_Add_Object: duplicate object name \r\n");
		#endif
		return BACDEL_DUPLICATE_OBJECT_NAME;
	}

	/* check network type */ //v43
		if(eNetworkType != NETWORK_TYPE_MSTP)
		{
			/* invalid network type */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR,
				"APDU: NetworkPort_Add_Object: invalid network type \r\n");
			#endif
			return BACDEL_OUT_OF_RANGE_ERROR;
		}

	/* allocate memory for new object */
	pvNewObject = OSAL_Malloc(u32Size, __FILE__, __FUNCTION__, __LINE__);
	if(pvNewObject == NULL)
	{
		/* memory allocation failed */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, 
			"APDU: NetworkPort_Add_Object: memory allocation failed \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}

	/* initialize the properties with default value */
	NetworkPort_Initialize_Properties((NetworkPort_t *)pvNewObject, 
		u32ObjId, eNetworkType, pstObjName);

	/* attach the pointer in device */
	pVirtualDev->m_stSupportedObjects.m_pstNetworkPort = (NetworkPort_t *)pvNewObject;

	/* update object list property */
	Update_Device_Object_List(pVirtualDev, OBJECT_NETWORK_PORT, u32ObjId);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
		"APDU: NetworkPort_Add_Object: Exit \r\n");
	#endif
	return BACDEL_SUCCESS;
}

/**
*
* DESCRIPTION 
* Function to delete network port objects.
*
* @param pVirtualDev	[in] device data
* @return [out] success or suitable error code
*
*/
BACNET_RETURN_TYPE NetworkPort_Delete_Object(void *pVirtualDev)
{
	/* local variables */
	virtualDevData_t *pstVirtualDev = NULL;
	NetworkPort_t *pstNpObject = NULL;
	NetworkPort_t *pstNpNextObject = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
		"APDU: NetworkPort_Delete_Object: Entry \r\n");
	#endif

	/* get device data */
	pstVirtualDev = (virtualDevData_t *)pVirtualDev;

	/* get network port object data */
	pstNpObject = pstVirtualDev->m_stSupportedObjects.m_pstNetworkPort;
	pstVirtualDev->m_stSupportedObjects.m_pstNetworkPort = NULL;

	while(NULL != pstNpObject)
	{
		/* save next object */
		pstNpNextObject = pstNpObject->pstNext;

		/* clear bdt and fdt lists */
		Clear_BdtEntry_List(&pstNpObject->m_stBbmdBdt.m_pstBDTEntryList);
		Clear_BdtEntry_List(&pstNpObject->m_stBbmdBdtBackup.m_pstBDTEntryList);
		Clear_FdtEntry_List(&pstNpObject->m_stBbmdFdt.m_pstFDTEntryList);

		/* clear dns server list */
		Clear_OctetString_List(&pstNpObject->m_stIPDnsServer.m_pstOctetStrVal);

		/* release memory allocated to object */
		OSAL_Free((void *)pstNpObject, __FILE__, __FUNCTION__, __LINE__);

		/* move to next */
		pstNpObject = pstNpNextObject;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
		"APDU: NetworkPort_Delete_Object: Exit \r\n");
	#endif
	return BACDEL_SUCCESS;
}

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
	BACnetCharStr_t *pstObjName)
{
	/* local variables */		
	PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
	uint32_t u32Value = 0;
	uint16_t u16Value = 0;
	#ifdef BACDEL_PR14
	special_property_list_t stPropertyList = {0};
	uint8_t u8SkipPropCnt = 0;
	#endif
	uint8_t au8Description[] = "SoftDEL's BACnet/IP Network Port";

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
		"APDU: NetworkPort_Initialize_Properties: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pstNpObj || NULL == pstObjName)
	{
		/* invalid pointer */
		return;
	}

	/******* object id *******/
	pstNpObj->m_stObjectID.m_u32ObjId = u32ObjectId;
	pstNpObj->m_stObjectID.m_eObjectType = OBJECT_NETWORK_PORT;
	/* save the id in global variable */
	g_u32NetworkPortId = u32ObjectId;

	/******* object type *******/
	pstNpObj->m_stObjectType.m_eObjectType = OBJECT_NETWORK_PORT;

	/******* object name *******/
	memset(&(pstNpObj->m_stObjName.m_stCHARString.m_pu8CharStr[0]), 0,
		sizeof(pstNpObj->m_stObjName.m_stCHARString.m_pu8CharStr));
	/* get object name length & copy */
	u32Value = pstObjName->m_u32StrLen;
	pstNpObj->m_stObjName.m_stCHARString.m_u32StrLen = u32Value;
	memcpy(&pstNpObj->m_stObjName.m_stCHARString.m_pu8CharStr, &pstObjName->m_pu8CharStr[0], u32Value);	

	/******* status flag *******/
	pstNpObj->m_stStatusFlag.m_stBitStr.m_u8ByteCnt = 1;
	pstNpObj->m_stStatusFlag.m_stBitStr.m_u8UnusedBits = 4;
	pstNpObj->m_stStatusFlag.m_stBitStr.m_u8TransBits[0] = 0;

	/******* reliability *******/
	pstNpObj->m_stReliability.m_eReliabilty = RELIABILITY_NO_FAULT_DETECTED;

	/******* out of service *******/
	pstNpObj->m_stOutOfService.m_bVal = false;

	/******* network type *******/
	pstNpObj->m_stNetworkType.m_eNetworkType = eNetworkType;

	/******* protocol level *******/
	pstNpObj->m_stProtocolLevel.m_eProtocolLevel = PROTOCOL_LEVEL_BACNET_APPLICATION;

	/******* network number quality *******/
	pstNpObj->m_stNetworkNoQuality.m_eNetworkNoQuality = NETWORK_NUMBER_QUALITY_UNKNOWN;
	/* register property id in property list */
    Register_Object_Property(OBJECT_NETWORK_PORT, PROP_NETWORK_NUMBER_QUALITY);

	/******* changes pending *******/
	pstNpObj->m_stChangesPending.m_bVal = false;

	/******* apdu length *******/
	pstNpObj->m_stAPDULength.m_u32Val = MAX_APDU_LENGTH_ACCEPTED;
	/* register property id in property list */

    Register_Object_Property(OBJECT_NETWORK_PORT, PROP_APDU_LENGTH);

	/******* link speed *******/
	pstNpObj->m_stLinkSpeed.m_fVal = enumToBaud(SMCfg.g_Com485Cfg.Baudrate);//BACnetConfigData.m_u32Baudrate;//setUart.Baudrate;		//SK
	Register_Object_Property(OBJECT_NETWORK_PORT, PROP_LINK_SPEED);

	/******* link speeds *******/
	/* get the property access type for given object & property */
	pstNpObj->m_stLinkSpeeds.m_fVal[0] = MSTP_BAUD_RATE_9600;
	pstNpObj->m_stLinkSpeeds.m_fVal[1] = MSTP_BAUD_RATE_38400;
	pstNpObj->m_stLinkSpeeds.m_fVal[2] = MSTP_BAUD_RATE_115200;
	Register_Object_Property(OBJECT_NETWORK_PORT, PROP_LINK_SPEEDS);

	/************************ optional properties ***********************/
	/* Init Max Master and Max Info Frames  */

	pstNpObj->m_stMAXMaster.m_u32Val = 127;
	Register_Object_Property(OBJECT_NETWORK_PORT, PROP_MAX_MASTER);

	pstNpObj->m_stMaxInfoFrames.m_u32Val = 1;
	Register_Object_Property(OBJECT_NETWORK_PORT, PROP_MAX_INFO_FRAMES);
	/************************ optional properties ***********************/

	/******* description *******/
	memset(&(pstNpObj->m_stDescription.m_stCHARString.m_pu8CharStr[0]), 0,
		sizeof(pstNpObj->m_stDescription.m_stCHARString.m_pu8CharStr));
	/* get length & copy */
	Strcopy(&(pstNpObj->m_stDescription.m_stCHARString.m_pu8CharStr[0]),
		au8Description, sizeof(au8Description));
	pstNpObj->m_stDescription.m_stCHARString.m_u32StrLen =
		Strnlen(au8Description, sizeof(pstNpObj->m_stDescription.m_stCHARString.m_pu8CharStr));
    /* register property id in property list */
    Register_Object_Property(OBJECT_NETWORK_PORT, PROP_DESCRIPTION);

	/******* Profile Name *******/
	memset(&(pstNpObj->m_stProfileName.m_stCHARString.m_pu8CharStr[0]),
		00, sizeof(pstNpObj->m_stProfileName.m_stCHARString.m_pu8CharStr));
	/* get length & copy */
	Strcopy(&(pstNpObj->m_stProfileName.m_stCHARString.m_pu8CharStr[0]),
		g_au8ProfileName, sizeof(pstNpObj->m_stProfileName.m_stCHARString.m_pu8CharStr));
	pstNpObj->m_stProfileName.m_stCHARString.m_u32StrLen =
		Strnlen(g_au8ProfileName, sizeof(pstNpObj->m_stProfileName.m_stCHARString.m_pu8CharStr));

	/* register property id in property list */
    Register_Object_Property(OBJECT_NETWORK_PORT, PROP_PROFILE_NAME);

	#ifdef BACDEL_PR14
	/******* property list *******/
	/* initialize the propert-list property */
	Device_Objects_Property_List(OBJECT_NETWORK_PORT, &stPropertyList);

	#endif
	uint8_t u8MacId = MSTP_GetMacId(NULL);
	// copy to pstNpObj->m_stMacAddress
	pstNpObj->m_stMacAddress.m_u32OctetCount = 1;  // only 1 byte
	pstNpObj->m_stMacAddress.m_ai8OctetStr[0] = (int8_t)u8MacId;

	// Optional: also set into m_stOctetString if needed
	pstNpObj->m_stMacAddress.m_stOctetString.m_u32OctetCount = 1;
	pstNpObj->m_stMacAddress.m_stOctetString.m_ai8OctetStr[0] = (uint8_t)u8MacId;
	Register_Object_Property(OBJECT_NETWORK_PORT, PROP_MAC_ADDRESS);

	pstNpObj->m_stPropertyList.m_psti32RequiredProp = stPropertyList.Required.pList;
	pstNpObj->m_stPropertyList.m_psti32OptionalProp = stPropertyList.Optional.pList;
	pstNpObj->m_stPropertyList.m_psti32ProprietaryProp = stPropertyList.Proprietary.pList;

	/* get skip property count value */
	u8SkipPropCnt = Get_PropertyList_SkipCount(&stPropertyList);
	/* save total property count value */
	pstNpObj->m_stPropertyList.m_u32PropCount =
	(stPropertyList.Required.count + stPropertyList.Optional.count
	+ stPropertyList.Proprietary.count - u8SkipPropCnt);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
		"APDU: NetworkPort_Initialize_Properties: Exit \r\n");
	#endif
	return;
}

/**
*
* DISCREPTION
* Function to check for duplicate bdt entry in backup bbmd bdt and original bbmd bdt.
*
* @param pBackupBbmdBdtList		[in] pointer to backup bbmd bdt
* @param pBbmdBdtList			[in] pointer to original bbmd bdt
*
*/
void NetworkPort_Check_Duplicate_BDT_Entry(ListOfBACnetBDTEntry_t *pstBackupBbmdBdtList,
	ListOfBACnetBDTEntry_t *pstBbmdBdtList)
{
	/* local variables */
	ListOfBACnetBDTEntry_t *pstTemp = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
		"APDU: NetworkPort_Check_Duplicate_BDT_Entry: Entry \r\n");
	#endif

	/* save actual list pointer */
	pstTemp = pstBbmdBdtList;
}

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
* @param pvObjAddr			[in] objects base address
* @param bLMServiceReq		[in] True if call is received from LM-B service
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
	bool bLMServiceReq)
{
	/* local variables */
	uint32_t u32Data = 0;
	bool bRetValue = true;
	bool bData = false;
	void *pvProAddr = NULL;
	virtualDevData_t *pstVirtualDev = NULL;
	ListOfOctetStr_t *pstOctetStrList = NULL;
	NetworkPort_t *pstNpObj = NULL;
	BACnetBitStr_t stBitString = {0};

		/* function entry */
		#if(defined DEBUG_PRINTF && DL_3)
		Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
			"APDU: NetworkPort_Write_Property: Entry \r\n");
		#endif

		/* check input pointer */
		if(NULL == pVirtualDev || NULL == pvObjAddr /*|| NULL == pvData*/
			|| NULL == pvPropAdd)
		{
			/* invalid pointer */
			return FALSE;
		}

		/* list/array[n] type properties can have empty values */
		if(NULL == pvData && eObjectProperty != PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE
			&& eObjectProperty != PROP_BBMD_FOREIGN_DEVICE_TABLE
			&& eObjectProperty != PROP_IP_DNS_SERVER)
		{
			/* invalid pointer */
			return FALSE;
		}

		/* get the device & network port object base address pointer */
		pstVirtualDev = (virtualDevData_t *)pVirtualDev;
		pstNpObj = (NetworkPort_t *)pvObjAddr;

		/* reset value */
		*pvPropAdd = NULL;

		/* update respective property */
		switch(eObjectProperty)
		{
		case PROP_OBJECT_IDENTIFIER:
		{
			/* update object id */
			pstNpObj->m_stObjectID.m_u32ObjId = ((BACnetObjId_t *)pvData)->m_u32ObjInstance;
			pstNpObj->m_stObjectID.m_eObjectType = OBJECT_NETWORK_PORT;
			pvProAddr = &pstNpObj->m_stObjectID.m_u32ObjId;
		}
		break;

		case PROP_OBJECT_NAME:
		{
			/* update object name */
			memset(&pstNpObj->m_stObjName.m_stCHARString, 0x00,
				sizeof(BACnetCharStr_t));
			memcpy(&pstNpObj->m_stObjName.m_stCHARString, (BACnetCharStr_t *)pvData,
				sizeof(BACnetCharStr_t));
			pvProAddr = &pstNpObj->m_stObjName.m_stCHARString;
		}
		break;

		case PROP_OBJECT_TYPE:
		{
			/* update object type */
			memcpy(&pstNpObj->m_stObjectType.m_eObjectType,
				(BACNET_OBJECT_TYPE *)pvData, sizeof(BACNET_OBJECT_TYPE));
			pvProAddr = &pstNpObj->m_stObjectType.m_eObjectType;
		}
		break;

		case PROP_DESCRIPTION:
		{
			/* update description */
			memset(&pstNpObj->m_stDescription.m_stCHARString, 0x00,
				sizeof(BACnetCharStr_t));
			memcpy(&pstNpObj->m_stDescription.m_stCHARString, (BACnetCharStr_t *)pvData,
				sizeof(BACnetCharStr_t));
			pvProAddr = &pstNpObj->m_stDescription.m_stCHARString;
		}
		break;

		case PROP_PROFILE_NAME:
		{
			/* update profile name */
			memset(&pstNpObj->m_stProfileName.m_stCHARString, 0x00,
				sizeof(BACnetCharStr_t));
			memcpy(&pstNpObj->m_stProfileName.m_stCHARString, (BACnetCharStr_t *)pvData,
				sizeof(BACnetCharStr_t));
			pvProAddr = &pstNpObj->m_stProfileName.m_stCHARString;
		}
		break;

		case PROP_STATUS_FLAGS:
		{
			/* update status flags */
			memcpy(&pstNpObj->m_stStatusFlag.m_stBitStr, (BACnetBitStr_t *)pvData,
				sizeof(BACnetBitStr_t));
			pvProAddr = &pstNpObj->m_stStatusFlag.m_stBitStr;
		}
		break;

		case PROP_RELIABILITY:
		{
			/* update reliability */
			memcpy(&pstNpObj->m_stReliability.m_eReliabilty, (BACNET_RELIABILITY *)pvData,
				sizeof(BACNET_RELIABILITY));
			pvProAddr = &pstNpObj->m_stReliability.m_eReliabilty;

			if(!bRecursiveCall)
			{
				/* read the status-flags value and update as per reliability changes */
				memcpy(&stBitString, &pstNpObj->m_stStatusFlag.m_stBitStr,
					sizeof(BACnetBitStr_t));

				/* toggle the bit */
				if(RELIABILITY_NO_FAULT_DETECTED != *((BACNET_RELIABILITY *)pvData))
					stBitString.m_u8TransBits[0] |= 0x02; /* set bit */
				else
					stBitString.m_u8TransBits[0] &= 0xFD; /* reset bit */

				/* update status flag */
				pvData = &stBitString;
				bRetValue = NetworkPort_Write_Property(PROP_STATUS_FLAGS, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
			}
		}
		break;

		case PROP_OUT_OF_SERVICE:
		{
			/* update out of service */
			memcpy(&pstNpObj->m_stOutOfService.m_bVal, (bool *)pvData,
				sizeof(bool));
			pvProAddr = &pstNpObj->m_stOutOfService.m_bVal;

			/* check value and enable or disable bacnet communication */
			if(pstNpObj->m_stOutOfService.m_bVal && PROTOCOL_LEVEL_BACNET_APPLICATION ==
				pstNpObj->m_stProtocolLevel.m_eProtocolLevel)
			{
				/* disable bacnet communication */
				//BACDEL_Disable_Network_Communication();
			}
			else
			{
				/* enable bacnet communication */
				//BACDEL_Enable_Network_Communication();
			}

			if(!bRecursiveCall)
			{
				/* read the status-flags value and update as per out of service changes */
				memcpy(&stBitString, &pstNpObj->m_stStatusFlag.m_stBitStr,
					sizeof(BACnetBitStr_t));

				/* toggle the bit */
				if(*((bool *)pvData))
					/* set bit */
					stBitString.m_u8TransBits[0] |= (0x01<<STATUS_FLAG_OUT_OF_SERVICE);
				else
					/* reset bit */
					stBitString.m_u8TransBits[0] &= (0xFF ^(0x01<<STATUS_FLAG_OUT_OF_SERVICE));

				/* update status flag */
				pvData = &stBitString;
				bRetValue = NetworkPort_Write_Property(PROP_STATUS_FLAGS, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
			}
		}
		break;

		case PROP_NETWORK_TYPE:
		{
			/* update network type */
			memcpy(&(pstNpObj->m_stNetworkType.m_eNetworkType), (BACNET_NETWORK_TYPE *)pvData,
				sizeof(BACNET_NETWORK_TYPE));
			pvProAddr = &pstNpObj->m_stNetworkType.m_eNetworkType;

			if(!bRecursiveCall && NETWORK_TYPE_PTP == pstNpObj->m_stNetworkType.m_eNetworkType)
			{
				/* update network number quality */
				u32Data = NETWORK_NUMBER_QUALITY_CONFIGURED;
				pvData = &u32Data;
				bRetValue = NetworkPort_Write_Property(PROP_NETWORK_NUMBER_QUALITY, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
			}
		}
		break;

		case PROP_PROTOCOL_LEVEL:
		{
			/* update protocol level */
			memcpy(&pstNpObj->m_stProtocolLevel.m_eProtocolLevel, (BACNET_PROTOCOL_LEVEL *)pvData,
				sizeof(BACNET_PROTOCOL_LEVEL));
			pvProAddr = &pstNpObj->m_stProtocolLevel.m_eProtocolLevel;
		}
		break;

		case PROP_NETWORK_NUMBER:
		{
			/* update network number */
			memcpy(&pstNpObj->m_stNetworkNo.m_u16Val, (uint16_t *)pvData,
				sizeof(uint16_t));
			pvProAddr = &pstNpObj->m_stNetworkNo.m_u16Val;

			if(!bRecursiveCall)
			{
				/* update network number quality */
				if(pstNpObj->m_stNetworkNo.m_u16Val == 0)
					u32Data = NETWORK_NUMBER_QUALITY_UNKNOWN;
				else
					u32Data = NETWORK_NUMBER_QUALITY_CONFIGURED;
				pvData = &u32Data;
				bRetValue = NetworkPort_Write_Property(PROP_NETWORK_NUMBER_QUALITY, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);

				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btNetworkNo = TRUE;
			}
		}
		break;

		case PROP_NETWORK_NUMBER_QUALITY:
		{
			/* update network number quality */
			memcpy(&pstNpObj->m_stNetworkNoQuality.m_eNetworkNoQuality,
				(BACNET_NETWORK_NUMBER_QUALITY *)pvData,
				sizeof(BACNET_NETWORK_NUMBER_QUALITY));
			pvProAddr = &pstNpObj->m_stNetworkNoQuality.m_eNetworkNoQuality;
		}
		break;

		case PROP_CHANGES_PENDING:
		{
			/* update changes pending */
			memcpy(&pstNpObj->m_stChangesPending.m_bVal, (bool *)pvData,
				sizeof(bool));
			pvProAddr = &pstNpObj->m_stChangesPending.m_bVal;
		}
		break;

		case PROP_APDU_LENGTH:
		{
			/* update apdu length */
			memcpy(&pstNpObj->m_stAPDULength.m_u32Val, (uint32_t *)pvData,
				sizeof(uint32_t));
			pvProAddr = &pstNpObj->m_stAPDULength.m_u32Val;
		}
		break;

	case PROP_MAX_MASTER:		//SK
		{
			/* update apdu length */
			memcpy(&pstNpObj->m_stMAXMasterBackup.m_u32Val,(uint32_t *)pvData,sizeof(uint32_t));

			pvProAddr = &pstNpObj->m_stMAXMaster.m_u32Val;

			//required to be writable for MSTP
			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btMaxMaster = TRUE;
			}

			SMCfg.g_BACnetCfg.MaxMaster = pstNpObj->m_stMAXMasterBackup.m_u32Val;
			SaveFactoryDatatoNV();
		}
		break;

		case PROP_LINK_SPEED:
		{
			/* update link speed */
			memcpy(&pstNpObj->m_stLinkSpeed.m_fVal, (Float_t *)pvData,
				sizeof(Float_t));
			pvProAddr = &pstNpObj->m_stLinkSpeed.m_fVal;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btLinkSpeed = TRUE;
			}
		}
		break;

		case PROP_MAC_ADDRESS:
		{
			/* update mac address */
			memcpy(&pstNpObj->m_stMacAddress.m_stOctetString, (BACnetOctetStr_t *)pvData,
				sizeof(BACnetOctetStr_t));
			pvProAddr = &pstNpObj->m_stMacAddress.m_stOctetString;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btMacAddress = TRUE;
			}
		}
		break;

		case PROP_BACNET_IP_MODE:
		{
			/* update ip mode */
			memcpy(&pstNpObj->m_stIPMode.m_eIpMode, (BACNET_IP_MODE *)pvData,
				sizeof(BACNET_IP_MODE));
			pvProAddr = &pstNpObj->m_stIPMode.m_eIpMode;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btIPMode = TRUE;
			}
		}
		break;

		case PROP_IP_ADDRESS:
		{
			/* update ip address */
			memcpy(&pstNpObj->m_stIPAddress.m_stOctetString, (BACnetOctetStr_t *)pvData,
				sizeof(BACnetOctetStr_t));
			pvProAddr = &pstNpObj->m_stIPAddress.m_stOctetString;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btIPAddress = TRUE;
			}
		}
		break;

		case PROP_BACNET_IP_UDP_PORT:
		{
			/* update ip udp port */
			memcpy(&pstNpObj->m_stIPUdpPort.m_u16Val, (uint16_t *)pvData,
				sizeof(uint16_t));
			pvProAddr = &pstNpObj->m_stIPUdpPort.m_u16Val;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btIPUdpPort = TRUE;
			}
		}
		break;

		case PROP_IP_SUBNET_MASK:
		{
			/* update ip subnet mask */
			memcpy(&pstNpObj->m_stIPSubnetMask.m_stOctetString, (BACnetOctetStr_t *)pvData,
				sizeof(BACnetOctetStr_t));
			pvProAddr = &pstNpObj->m_stIPSubnetMask.m_stOctetString;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btIPSubnetMask = TRUE;
			}
		}
		break;

		case PROP_IP_DEFAULT_GATEWAY:
		{
			/* update ip default gateway */
			memcpy(&pstNpObj->m_stIPDefaultGateway.m_stOctetString, (BACnetOctetStr_t *)pvData,
				sizeof(BACnetOctetStr_t));
			pvProAddr = &pstNpObj->m_stIPDefaultGateway.m_stOctetString;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btIPDefaultGateway = TRUE;
			}
		}
		break;

		case PROP_IP_DNS_SERVER:
		{
			/* update dns server */
			/* clear list and attach pointer */
			Clear_OctetString_List(&pstNpObj->m_stIPDnsServer.m_pstOctetStrVal);
			pstNpObj->m_stIPDnsServer.m_u32Count = 0;
			pstNpObj->m_stIPDnsServer.m_pstOctetStrVal = (ListOfOctetStr_t *)pvData;
			pvProAddr = &pstNpObj->m_stIPDnsServer;

			/* get the BDT entry list data */
			pstOctetStrList	= pstNpObj->m_stIPDnsServer.m_pstOctetStrVal;
			while(NULL != pstOctetStrList)
			{
				/* increment count & move to next node */
				pstNpObj->m_stIPDnsServer.m_u32Count++;
				pstOctetStrList = pstOctetStrList->m_pstNext;
			}
		}
		break;

		#ifdef BBMD_ENABLED
		case PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE:
		{
			/* local variable */
			ListOfBACnetBDTEntry_t *pstListOfBdtEntry = NULL;

			/* update bbmd bdt */
			/* clear list and attach pointer */
			/* backup bdt is updated using WP-B, this is copied to actual
			bdt when changes are activated using RD-B or stack is restarted */
			if(false == bLMServiceReq)
			{
				/* call from WP-B module */
				Clear_BdtEntry_List(&pstNpObj->m_stBbmdBdtBackup.m_pstBDTEntryList);
				pstNpObj->m_stBbmdBdtBackup.m_pstBDTEntryList = (ListOfBACnetBDTEntry_t *)pvData;
			}
			else
			{
				/* call from LM-B module */
				/* get the property offset address */
				pstListOfBdtEntry = pstNpObj->m_stBbmdBdtBackup.m_pstBDTEntryList;
				if(NULL == pstListOfBdtEntry || pvData == pstListOfBdtEntry)
				{
					/* update pointer in property struct */
					pstListOfBdtEntry = (ListOfBACnetBDTEntry_t *)pvData;
					pstNpObj->m_stBbmdBdtBackup.m_pstBDTEntryList = pstListOfBdtEntry;
				}
				else
				{
					/* attach new node @ end of list */
					while(pstListOfBdtEntry->m_pstNext != NULL)
					{
						/* move to next node */
						pstListOfBdtEntry = pstListOfBdtEntry->m_pstNext;
					}
					/* add the new bdt entry in list */
					pstListOfBdtEntry->m_pstNext = (ListOfBACnetBDTEntry_t *)pvData;
				}
			}

			/* check for duplicate bdt entry in backup bbmd bdt and original bbmd bdt */
			NetworkPort_Check_Duplicate_BDT_Entry(
				pstNpObj->m_stBbmdBdtBackup.m_pstBDTEntryList,
				pstNpObj->m_stBbmdBdt.m_pstBDTEntryList);

			/* update the bbmd backup file */
			Update_BBMD_File();
			pvProAddr = &pstNpObj->m_stBbmdBdt;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btBbmdBdt = TRUE;
			}
			else
			{
				/* updated actual bdt list */
				NetworkPort_Activate_BDT_Changes(pstVirtualDev);
				/* set bit to false */
				pstNpObj->m_stNpChangesPending.m_btBbmdBdt = FALSE;
			}
		}
		break;

		case PROP_BBMD_FOREIGN_DEVICE_TABLE:
		{
			/* update bbmd bdt */
			/* clear list and attach pointer */
			Clear_FdtEntry_List(&pstNpObj->m_stBbmdFdt.m_pstFDTEntryList);
			pstNpObj->m_stBbmdFdt.m_pstFDTEntryList = (ListOfBACnetFDTEntry_t *)pvData;

			/* update the bbmd backup file */
			Update_BBMD_File();
			pvProAddr = &pstNpObj->m_stBbmdFdt;
		}
		break;

		case PROP_BBMD_ACCEPT_FD_REGISTRATIONS:
		{
			/* update bbmd accept fd registration */
			memcpy(&pstNpObj->m_stBbmdAcceptFD.m_bVal, (bool *)pvData,
				sizeof(bool));
			pvProAddr = &pstNpObj->m_stBbmdAcceptFD.m_bVal;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btBbmdAcceptFD = TRUE;
			}
		}
		break;
		#endif /* BBMD */

		case PROP_FD_BBMD_ADDRESS:
		{
			/* update fd bbmd address */
			memcpy(&pstNpObj->m_stFDBbmdAddr.m_stHostNPort,
				&((Pr_BACnetHostNPort_t *)pvData)->m_stHostNPort,
				sizeof(BACnetHostNPort_t));
			pvProAddr = &pstNpObj->m_stFDBbmdAddr.m_stHostNPort;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btFdBbmdAddress = TRUE;
			}
		}
		break;

		case PROP_FD_SUBSCRIPTION_LIFETIME:
		{
			/* update fd subscription lifetime */
			memcpy(&pstNpObj->m_stFDSubscriptionsLifetime.m_u16Val, (uint16_t *)pvData,
				sizeof(uint16_t));
			pvProAddr = &pstNpObj->m_stFDSubscriptionsLifetime.m_u16Val;

			if(!bRecursiveCall)
			{
				/* update changes pending */
				bData = true;
				pvData = &bData;
				bRetValue = NetworkPort_Write_Property(PROP_CHANGES_PENDING, u32ArrayIndex,
					bArrIndxPresent, TRUE, u8Priority, (void *)pstVirtualDev, pvData,
					READ_ONLY, (void *)pstNpObj, pvPropAdd, FALSE);
				/* set respective changes pending bit */
				pstNpObj->m_stNpChangesPending.m_btFDSubsLifetime = TRUE;
			}
		}
		break;

		default:
		{
			/* unknown property, return error */
			bRetValue = false;
		}
		}// switch ends

		/* function exit */
		#if(defined DEBUG_PRINTF && DL_3)
		Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
			"APDU: NetworkPort_Write_Property: Exit \r\n");
		#endif
		return bRetValue;
}

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
BACNET_RETURN_TYPE NetworkPort_Activate_BDT_Changes(void *pvVirtualDev)
{
	/* local variables */
	virtualDevData_t *pstVirtualDev = NULL;
	NetworkPort_t *pstNetPort = NULL;
	ListOfBACnetBDTEntry_t *pstBdtEntry = NULL;
	void *pvInData = NULL;
	void *pvTempData = NULL;
	BACNET_RETURN_TYPE eFunRetVal = BACDEL_SUCCESS;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
		"APDU: NetworkPort_Activate_BDT_Changes: Entry \r\n");
	#endif

		/* null check for input pointers */
		pstVirtualDev = (virtualDevData_t *)pvVirtualDev;
		if(NULL == pstVirtualDev)
		{
			/* invalid pointer */
			return BACDEL_DEVICE_NOT_PRESENT;
		}

		/* get the base address of network port object */
		pstNetPort =  pstVirtualDev->m_stSupportedObjects.m_pstNetworkPort;
		if(NULL == pstNetPort)
		{
			/* network port object is missing */
			return BACDEL_OBJECT_NOT_PRESENT;
		}

		/* get the backup BDT entry list data */
		pvInData = (void *)&pstNetPort->m_stBbmdBdtBackup;

		/* copy the backup value in actual value */
		eFunRetVal = Copy_PropVal_AsPer_DataType(pvInData, BACNET_DT_BDT_ENTRY_LIST, &pvTempData);
		if(BACDEL_SUCCESS == eFunRetVal)
		{
			/* clear previous actual bdt list and save new list */
			Clear_BdtEntry_List(&pstNetPort->m_stBbmdBdt.m_pstBDTEntryList);
			pstNetPort->m_stBbmdBdt.m_u32ArraySize = 0;
			pstNetPort->m_stBbmdBdt.m_pstBDTEntryList =
				((Pr_ListOfBACnetBDTEntry_t *)pvTempData)->m_pstBDTEntryList;
			OSAL_Free(pvTempData, __FILE__, __FUNCTION__, __LINE__);
		}

		/* get the BDT entry list data */
		pstBdtEntry	= pstNetPort->m_stBbmdBdt.m_pstBDTEntryList;
		while(NULL != pstBdtEntry)
		{
			/* set all entries as used */
			pstBdtEntry->m_stBDTEntry.m_bEntryUsed = TRUE;
			pstBdtEntry = pstBdtEntry->m_pstNext;

			/* increment count */
			pstNetPort->m_stBbmdBdt.m_u32ArraySize++;
		}

		/* function exit */
		#if(defined DEBUG_PRINTF && DL_3)
		Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
			"APDU: NetworkPort_Activate_BDT_Changes: Exit \r\n");
		#endif
		return eFunRetVal;
}

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
void Update_BBMD_File()
{
	uint8_t u8Index=0;
	NetworkPort_t *pstNpObj = NULL;
	ListOfBACnetBDTEntry_t *pstBdtList = NULL;
	ListOfBACnetFDTEntry_t *pstFdtList = NULL;
	virtualDevData_t *pstVirtualDev = NULL;
	BACnetConfigData.m_u8BDTEntryCount=1;

	/* get network port object */
	pstVirtualDev = gstHostDevice.m_pstDeviceStruct;
	pstNpObj = (NetworkPort_t *)Find_Object(OBJECT_NETWORK_PORT,
		g_u32NetworkPortId, pstVirtualDev);
	if(NULL == pstNpObj)
	{
		/* could not open file */
		#if(defined DEBUG_PRINTF && DL_1)
        Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_NOT_PRESENT,
			"BBMD: Update_BBMD_File: NP object not found \r\n");
		#endif
        return;
	}

	/* get bbmd bdt property */
	pstBdtList = pstNpObj->m_stBbmdBdtBackup.m_pstBDTEntryList;

	/* get bbmd bdt property */
	pstFdtList = pstNpObj->m_stBbmdFdt.m_pstFDTEntryList;


    /* write BDT entries */
    while(NULL != pstBdtList)
    {
    	if(u8Index == 0)
    	{
    		BACnetConfigData.m_u8BDTEntryCount=0;
    	}
        if(u8Index >= MAX_BDT_FDT_FD_ENTRIES)
        {
        	break;
        }
    	memcpy(&BACnetConfigData.m_stBDTEntry[u8Index],&pstBdtList->m_stBDTEntry,sizeof(BACnetBDTEntry_t));
		/* move to next node */
        pstBdtList = pstBdtList->m_pstNext;
        u8Index++;
        BACnetConfigData.m_u8BDTEntryCount++;
    }

    u8Index= 0;
    if(pstFdtList == NULL && BACnetConfigData.m_u8FDTEntryCount > 0 )
    {
    	memset(BACnetConfigData.m_stFDTEntry, 0x00,sizeof(BACnetFDTEntry_t)*MAX_BDT_FDT_FD_ENTRIES);
    	BACnetConfigData.m_u8FDTEntryCount=0;
    }
    while(NULL != pstFdtList)
    {
    	if(u8Index == 0)
    	{
    		BACnetConfigData.m_u8FDTEntryCount=0;
    		memset(BACnetConfigData.m_stFDTEntry, 0x00,sizeof(BACnetFDTEntry_t)*MAX_BDT_FDT_FD_ENTRIES);
    	}

        if(u8Index >= MAX_BDT_FDT_FD_ENTRIES)
        {
        	break;
        }
    	memcpy(&BACnetConfigData.m_stFDTEntry[u8Index],&pstFdtList->m_stFDTEntry,sizeof(BACnetFDTEntry_t));
		/* move to next node */
    	pstFdtList = pstFdtList->m_pstNext;
        u8Index++;
        BACnetConfigData.m_u8FDTEntryCount++;
    }



    SaveBACnetDatatoNV();

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
		"BBMD: Update_BBMD_File: Exit \r\n");
	#endif
	return;
}
#endif /* BBMD */
#endif /* NP object */

/******************************** end of source file *************************/
