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
*   SoftDEL Systems Ltd.                        india@softdel.com      
*   3rd Floor, Pentagon P4,                     http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - apiExportedApi.c
*
*   DESCRIPTION                                                            
*   Exported API's of SoftDEL's BACnet-IP Stack.
*
*   RELEASE HISTORY                                                             
*   DATE            NAME                    DESCRIPTION
*   02/01/2012      Prashant Badgujar       File Creation
*
******************************************************************************/

/* OS specific includes */
#include "osalFreeRTOS.h"

/* stack include files */
#include "bacDELApi.h"
#include "propertyValueRead.h"
#include "propertyValueWrite.h"
#include "propertyTagType.h"
#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"
#include "pduDataEncodeDecode.h"
#include "bacDELDeviceConfig.h"
#include "propertyValidations.h"
#include "propertyClearValues.h"
#include "bacnetNPDUHandler.h"
#include "serviceNwLayerMsg_B.h"
#include "serviceNwLayerMsg_A.h"
#include "serviceGenericHandler.h"
/* to include IP layer functions */

//#include "ComDef.h"
#include "DataB.h"

/* header file for segmentation */
#ifdef SEGMENTATION_SUPPORTED
#include "SegmentationGenericHandler.h"
#endif

/* include for A-side services */
#ifdef INITIATE_SERVICE_ENABLED
#include "bacnetInitiateServiceMgmt.h"
#endif

/* To include RP-B Service */
#ifdef BACDEL_SER_DS_RP_B
#include "serviceReadProperty_B.h"
#endif /* BACDEL_SER_DS_RP_B */

/* To include DDB-B Service */
#ifdef BACDEL_SER_DM_DDB_B
#include "serviceDynamicDeviceBinding_B.h"
#endif /* BACDEL_SER_DM_DDB_B */

/* To include DDB-A Service */
#ifdef BACDEL_SER_DM_DDB_A
//#include "serviceDynamicDeviceBinding_A.h"
#endif /* BACDEL_SER_DM_DDB_A */

/* To include DOB-B Service */
#ifdef BACDEL_SER_DM_DOB_B
#include "serviceDynamicObjectBinding_B.h"
#endif /* BACDEL_SER_DM_DOB_B */

/* To include COV service */
#ifdef BACDEL_SER_DS_COV_B
#include "serviceChangeOfValue_B.h"
#endif /* BACDEL_SER_DS_COV_B */


/* To include DCC-B service */
#ifdef BACDEL_SER_DM_DCC_B
#include "serviceDeviceCommControl.h"
#endif /* BACDEL_SER_DM_DCC_B */





/* To Include Reinitialize Device Service */
#ifdef BACDEL_SER_DM_RD_B
#include "serviceReinitializeDevice_B.h"
#endif /* BACDEL_SER_DM_RD_B */


extern void SaveFactoryDatatoNV();
extern uint32_t enumToBaud(uint8_t Baudrate);


/** Global variable for Device Local Network No. */
uint16_t g_u16LocalNWNo = BACNET_LOCAL_NW_NUMBER;
/** Global variable for Device Virtual Network No. */
uint16_t g_u16VirtualNWNo = BACNET_VIRTUAL_NW_NUMBER;

/** Global variable to save character encoding for all devices */
BACNET_CHARACTER_STRING_ENCODING g_eCharEncoding = CHARACTER_ANSI_X34;
uint16_t g_u16CodePage = CODE_PAGE_DEFAULT;

#ifdef BBMD_ENABLED
extern osMutexId_t m_hBBMD_InitMtxLockHandle;
#endif
extern void IS25xp_bytewrite(uint8_t *pui8buffer, uint32_t ui32StartAddress, uint16_t ui16TotalLength);
extern uint16_t IS25xp_read(uint32_t offset, uint16_t nbytes, uint8_t *buffer);
extern void UartConfig(void);
extern void MX_USART2_UART_Init(void);

extern DB_t SMCfg;

void EnableIntUartTim();
/** Global variable to save password for all devices */
BACnetCharStr_t g_stDevicePassword = 
{
	DEFAULT_PASSWORD_LEN,	// Password length
	CODE_PAGE_DEFAULT,		// Code page
	CHARACTER_ANSI_X34,		// Character set
	DEFAULT_PASSWORD		// Password
};

/** An array of application callback response functions */
App_AutoResp_Interface_t afpApplicationFunctionCb[MAX_APP_CALLBACK_FUN];

App_RTC_Interface_t afpApplRTCCallback[APP_CB_RTC_MAX];
///** Application internal callback response functions */
//App_Interface_t fpAppLayerCbFunction[2];
App_Interface_t fpAppLayerCbFunction;
/** flag to identify if stack is working as BBMD or not */
extern bool g_bBBMDEnabled;

/** flag to identify if stack is working as FD or not */
extern bool g_bRegisterdAsFD;

extern osMutexId_t m_hInitMtxLockHandle;


/** structure to set various max limits of stack */
BACApp_MaxLimits_t	g_stStackMaxLimits = 
{
	DEFAULT_INITIATE_INFOQ_LENGTH,		// Initiate Q
	DEFAULT_BBMD_INITIATE_Q_LENGTH,		// BBMD initiate Q
	DEFAULT_BDT_FDT_FD_ENTRIES,			// BDT, FDT & register FD entries
	DEFAULT_MAX_OBJECTS,				// Objects in 1device
	DEFAULT_ADDRESS_BINDING_ENTRIES,	// Dynamic address binding
	DEFAULT_COV_SUBSCRIPTION,			// Sub Cov requests A-side
	DEFAULT_STATE_TEXT_LEN,				// State text property
	DEFAULT_AF_VALUES_LEN,				// Alarm Fault values properties
	DEFAULT_EXCEPTION_SCHEDULE_LEN,		// Exception schedule
	DEFAULT_TIME_VALUES_LIST_LEN,		// Time-Values data-type
	DEFAULT_DATE_LIST_LEN,				// Date list property
	DEFAULT_DESTINATION_LIST_LEN,		// Recipient list property
	DEFAULT_DEVOBJPROPREF_LIST_LEN,		// Dev obj prop ref data-type
	DEFAULT_EVENT_PARAMETERS_LEN,		// Event parameters property
	DEFAULT_OBJECTID_LIST_LEN,			// Object ID list data-type
	DEFAULT_ACTIVE_COV_LIST_LEN,		// Active cov subs property
	BACNET_ZERO,					    // Static address binding
	BACNET_ZERO,						// Network analyzability 
	DEFAULT_CALLBACK_NOTIFY_Q_LENGTH	// Callback notify Q 
};

/** structure to set various configuration parameters of stack */
BACApp_StackConfig_t g_stStackConfigParams = 
{
	THREADPOOL_TIMEOUT_DEFAULT,			// Threadpool timeout
	TIME_CLEAR_QUEUE,					// Clear initiate Q timeout
	CLEAR_INITIATED_REQ_TIMEOUT,		// Clear initiated request timeout
	PROCESS_Q_TIMEOUT_DEFAULT,			// Process queue timeout
	DEFAULT_INITIATE_Q_DELETION_COUNT,	// Clear initiate Q deletion count
	DEFAULT_BBMD_INITQ_TIMEOUT			// BBMD clear initiate Q timeout
};

///* Gloabl Stack Init Flag to keep track of BACnet stack initialization */
extern bool g_bBACnetStackInitFlag;

///* Gloabl Flag used to stop receiving request from network */
extern bool g_bReception;

/* Global flag used to start & stop Socket Communication */
extern bool g_bSocketCommFlag;

///* Global flag used to clear complete device database in stack */
extern bool g_bDeleteDeviceDatabase;

/* Global flag used to clear initiate queue data */
extern bool g_bDeleteInitQueData;


/********** Vendor specific details for devices ************/
/** Global variable to save vendor specific data */
uint8_t g_au8VendorName[MAX_SUPPORTED_CHRSTR_LEN] = VENDOR_NAME;
uint8_t g_au8ModelName[MAX_SUPPORTED_CHRSTR_LEN] = MODEL_NAME;
uint8_t g_au8FirmwareRev[MAX_SUPPORTED_CHRSTR_LEN] = FIRMWARE_REV;
uint8_t g_au8ApplSoftwareVer[MAX_SUPPORTED_CHRSTR_LEN] = APP_SOFTWARE_VERSION;
uint8_t g_au8Location[MAX_SUPPORTED_CHRSTR_LEN] = LOCATION;
uint8_t g_au8DevDescription[MAX_SUPPORTED_CHRSTR_LEN] = DEV_DESCRIPTION;
uint8_t g_au8ObjDescription[MAX_SUPPORTED_CHRSTR_LEN] = OBJ_DESCRIPTION;
uint8_t g_au8ProfileName[MAX_SUPPORTED_CHRSTR_LEN] = PROFILE_NAME;
uint8_t g_au8DeviceName[MAX_SUPPORTED_CHRSTR_LEN] = DEVICE_NAME;
uint16_t g_u16VendorID = VENDOR_IDENTIFIER;
#ifdef BACDEL_PR14
uint8_t g_au8SerialNumber[MAX_SUPPORTED_CHRSTR_LEN] = SERIAL_NUMBER;
#endif


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
	BACDEL_Add_Device(uint32_t u32DADR, uint32_t u32Device_Id, uint16_t u16SNET)
{

    /* BACnet Stack first Host device (Static Host Device) pointer */
    BACnet_Device_Struct_t  *pstHostDev = NULL;
    BACnet_Device_Struct_t  *pstPrevHostDev = NULL;
    uint32_t u32QueueIndex = 0;        /** Device Queue index counter */
   
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
					BACDEL_Add_Device: Entry \r\n");
	#endif
	
	/* validate input parameters */
	if(u32Device_Id >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Device: \
		invalid device id \r\n");
		#endif
        return BACDEL_MAX_INSTANCE_ERROR;
	}

    /* Catch the Host Static Device */
    pstHostDev = &gstHostDevice;

#ifndef SUPPORT_MULTIPLE_DEVICE
	if(NULL != pstHostDev->m_pstDeviceStruct)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Device: \
		Only 1 device can be added \r\n");
		#endif
        return BACDEL_OBJECT_CANNOT_BE_CREATED;
    }
	else if(BACNET_ZERO != u16SNET)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Device: \
		invalid parameter \r\n");
		#endif
        return BACDEL_OBJECT_CANNOT_BE_CREATED;
	}
#endif
    
    /* In this case there is no host device, add first host device in 
       the system. No need to tranverse the link list. */
    if(NULL != pstHostDev->m_pstDeviceStruct)
    {
        /* In case of Vendors requiring multiple host, traverse the 
        Hose device object list */
        while(NULL != pstHostDev)
        {
            /* Check if the requested device id is valid. i.e. no other 
            device with same requested device id exists */
            if(pstHostDev->m_u32DeviceInstace == u32Device_Id)
            {
                /* duplicate device id */
				#ifdef DEBUG_PRINTF
	            Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAPI:BACDEL_Add_Device:\
				Host device is Already Present \r\n");
				#endif
	            return BACDEL_DUPLICATE_OBJECT;            
            }
            pstPrevHostDev = pstHostDev;
            /* Move to the next device in the Host device link list */
            pstHostDev = (BACnet_Device_Struct_t *)pstHostDev->pstNextDevice;
        }

        pstHostDev = pstPrevHostDev;

        /* Allocte Memory for New Host Device */
	    pstHostDev->pstNextDevice = 
            OSAL_Malloc(sizeof(BACnet_Device_Struct_t),
								 __FILE__, __FUNCTION__, __LINE__);
	    if(NULL == pstHostDev->pstNextDevice)
	    {
			#ifdef DEBUG_PRINTF
		    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Add_Device:"
			 "OSAL_Malloc failed for Host Device \r\n");
			#endif
		    return BACDEL_MALLOC_ERROR;
	    }

        /* Move to New Host Device */
        pstHostDev = (BACnet_Device_Struct_t *)pstHostDev->pstNextDevice;    
    }

	/* Allocate memory for Host device structure */
	pstHostDev->m_pstDeviceStruct = (virtualDevData_t*)OSAL_Malloc(sizeof
                    (virtualDevData_t),  __FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstHostDev->m_pstDeviceStruct)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Add_Device:\
		 OSAL_Malloc "
		"failed for m_pstDeviceStruct \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}
    /* Initialize the Host Device structure with zero */
    memset(pstHostDev->m_pstDeviceStruct, BACNET_ZERO, sizeof(virtualDevData_t));      

#ifdef BACDEL_SER_DM_DCC_B
    /* Disable Device Communication during add device process */
    pstHostDev->m_pstDeviceStruct->m_eDevCommStatus = COMMUNICATION_ENABLE;
#endif

    /* Now Initialize the Device ID and DADR (DADR not required in single host device) */
    pstHostDev->m_u32DeviceInstace = u32Device_Id;

    /* Mark the next device pointer of Host device as NULL */
    pstHostDev->pstNextDevice = NULL;

	/************************** Device Properties **********************/

	/* Initialize the device object properties */
	Init_Device_Properties(pstHostDev->m_pstDeviceStruct, u32Device_Id, false);

    /* Set the default device password */
	pstHostDev->m_pstDeviceStruct->m_stDvPassword.m_u8Encoding = g_eCharEncoding;
	pstHostDev->m_pstDeviceStruct->m_stDvPassword.m_u32StrLen = BACnetConfigData.m_stPassword.m_u32StrLen;
	//DEFAULT_PASSWORD_LEN;
	memcpy(pstHostDev->m_pstDeviceStruct->m_stDvPassword.m_pu8CharStr, 
			BACnetConfigData.m_stPassword.m_pu8CharStr,
			BACnetConfigData.m_stPassword.m_u32StrLen);

#ifdef BACDEL_OBJ_DEV
        /* Assign the pointer of Device Object structure to supportedObj_t structure. This
        is done to resolve the defect 27 reported in SoftDEL_BACnet_DefectReport.xls */
        pstHostDev->m_pstDeviceStruct->m_stSupportedObjects.m_pstDevObject = 
                &(pstHostDev->m_pstDeviceStruct->m_stDevObject);
#endif

        /* Initialize the Fill & Processing Index */
        pstHostDev->m_pstDeviceStruct->m_i32QProcessIndex = 0;
        pstHostDev->m_pstDeviceStruct->m_i32QFillIndex = 0;

        /* Set Default COVU frequency to 60sec */
		#ifdef BACDEL_SER_DS_COVU_B
        pstHostDev->m_pstDeviceStruct->m_stDevObject.m_u32DefaultCOVUFrequency = 60;
		#endif

        /* Initialize the Device Queue */
        for(u32QueueIndex = 0; u32QueueIndex < MAX_PACKET; u32QueueIndex++)
        {    
            pstHostDev->m_pstDeviceStruct->
								m_pstDeviceProcessQueue[u32QueueIndex] = NULL;
        }

    return BACDEL_SUCCESS;
}

 /**
 *
 * DESCRIPTION
 * API to set device object properties properties to default values.
 *
 * @param u32DevId [in] Device Id.
 * @param bClearObjectList [in] Clear object list if true
 *
 * @return BACNET_RETURN_TYPE [out] success or suitable error code.
 *
 */
 BACNET_RETURN_TYPE BACDEL_Init_Device_Properties(uint32_t u32DevId, bool bClearObjectList)
 {
     /* local variables */
     virtualDevData_t *pVirtualDev = NULL;

     /* function entry */
 	#if(defined DEBUG_PRINTF && DL_3)
     Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "API: \
     BACDEL_Init_Device_Properties: Entry \r\n");
 	#endif

     /* validate input parameters */
 	if(u32DevId >= BACNET_MAX_INSTANCE)
 	{
 		#if(defined DEBUG_PRINTF && DL_1)
 		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_MAX_INSTANCE_ERROR, "API: BACDEL_Init_Device_Properties: \
 		invalid device id \r\n");
 		#endif
 		return BACDEL_MAX_INSTANCE_ERROR;
 	}

     /* traverse the Host Device Link List */
     pVirtualDev = Find_In_Host_Device_List(u32DevId);
     if(NULL == pVirtualDev)
     {
 		/* host device not found */
 		#if(defined DEBUG_PRINTF && DL_1)
 		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DEVICE_NOT_PRESENT, "API: \
         BACDEL_Init_Device_Properties: No Device \r\n");
 		#endif
 		return BACDEL_DEVICE_NOT_PRESENT;
     }

     /* FIXME */
     /* TODO - check if object list needs to be freed ?? */

     /* clear device address binding property of device */
     Clear_AddressBinding_List(&pVirtualDev->m_stDevObject.
         m_stDevAddBinding.m_stAddBinding.pstNext, false);

 #ifdef OPTIONAL_PROPERTY1
 	/* clear Manual Slave address binding property of device */
     Clear_AddressBinding_List(&pVirtualDev->m_stDevObject.
         m_stManualSlaveAddressBinding.m_stAddBinding.pstNext, false);

 	/* clear slave address binding property of device */
     Clear_AddressBinding_List(&pVirtualDev->m_stDevObject.
         m_stSlaveAddressBinding.m_stAddBinding.pstNext, false);
 #endif

     /* if 1st device, clear the global device address binding list */
     if(u32DevId == gstHostDevice.m_u32DeviceInstace)
         Clear_Global_Device_Addr_Binding_List();

 #if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
 	 /* lock the mutex */
     Osal_Wait_Mutex(pVirtualDev->m_stDevObject.m_stActiveCOVSubList.
         m_hcovsublistMtxLock, INFINITE);

     /* free active cov subscription list */
     Clear_ActiveCovSubs_List(&pVirtualDev->m_stDevObject.
         m_stActiveCOVSubList.m_pstCOVSubscribe);

 	 /* lock the mutex */
 	Osal_Release_Mutex(pVirtualDev->m_stDevObject.m_stActiveCOVSubList.
         m_hcovsublistMtxLock);
 #endif

 #ifdef OPTIONAL_PROPERTY
     /* Clear Device Auto Slave Discovery property list */
//     Clear_Boolean_List(&pVirtualDev->m_stDevObject.
//         m_stAutoSlaveDiscovery.m_pstBoolList);

     /* clear slave proxy enable */
//     Clear_Boolean_List(&pVirtualDev->m_stDevObject.
//         m_stSlaveProxyEnable.m_pstBoolList);

     /* clear manual slave address binding */
//     Clear_AddressBinding_List(&pVirtualDev->m_stDevObject.
//         m_stManualSlaveAddressBinding.m_stAddBinding.pstNext, false);

     /* clear slave address binding */
//     Clear_AddressBinding_List(&pVirtualDev->m_stDevObject.
//         m_stSlaveAddressBinding.m_stAddBinding.pstNext, false);
 #endif

     /* call function to initialize the device object properties */
 	Init_Device_Properties(pVirtualDev, u32DevId, !bClearObjectList);

     /* function exit */
 	#if(defined DEBUG_PRINTF && DL_3)
     Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "API: \
     BACDEL_Init_Device_Properties: Exit \r\n");
 	#endif
     return BACDEL_SUCCESS;
 }
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
 BACNET_RETURN_TYPE BACDEL_Change_Device_Id(uint32_t u32OldDevId, uint32_t u32NewDevId, uint32_t u32NewDADR)
 {
     /* local variables */
     BACnet_Device_Struct_t *pstHostDev = NULL;
     virtualDevData_t *pVirtualDev = NULL;
     virtualDevData_t *pVirtualDevNew = NULL;
 #ifdef SUPPORT_MULTIPLE_DEVICE
     uint32_t u32Swapped = 0;
     BACnetAddress_t stDeviceAddr = {0};
 #endif
     /* to write device properties */
     BACNET_CONF_DATA stWpData = {0};
     Pr_BACnetObjId_t stObjectId = {0};


     /* function entry */
 	#if(defined DEBUG_PRINTF && DL_3)
     Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "API: BACDEL_Change_Device_Id: entry \r\n");
 	#endif

     /* validate input parameters */
 	if(u32NewDevId >= BACNET_MAX_INSTANCE || u32OldDevId >= BACNET_MAX_INSTANCE)
 	{
 		#if(defined DEBUG_PRINTF && DL_1)
 		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_MAX_INSTANCE_ERROR, "API: BACDEL_Change_Device_Id: \
 		invalid device id \r\n");
 		#endif
 		return BACDEL_MAX_INSTANCE_ERROR;
 	}

     /* traverse the Host Device Link List to find existing device */
     pstHostDev = &gstHostDevice;
 	while(NULL != pstHostDev)
 	{
 		if(pstHostDev->m_u32DeviceInstace == u32OldDevId)
         {
 			/* match found */
 			break;
         }
 		else
         {
 			/* move to next host device */
 			pstHostDev = pstHostDev->pstNextDevice;
         }
 	}

     /* check if device was found */
     if(NULL == pstHostDev)
     {
 		/* host device not found */
 		#if(defined DEBUG_PRINTF && DL_1)
 		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DEVICE_NOT_PRESENT, "API: BACDEL_Change_Device_Id: \
         No Device \r\n");
 		#endif
 		return BACDEL_DEVICE_NOT_PRESENT;
     }
     else
     {
         /* traverse the Host Device Link List to find new device id */
         pVirtualDevNew = Find_In_Host_Device_List(u32NewDevId);
         if(NULL != pVirtualDevNew)
         {
             /* host device not found */
 			#if(defined DEBUG_PRINTF && DL_1)
 		    Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DUPLICATE_OBJECT, "API: BACDEL_Change_Device_Id: \
             Device id not available. \r\n");
 			#endif
 		    return BACDEL_DUPLICATE_OBJECT;
         }
     }

     /* get the virtual device pointer */
     pVirtualDev = pstHostDev->m_pstDeviceStruct;

     /* re-initialize the device id */
     pstHostDev->m_u32DeviceInstace = u32NewDevId;

    {
         /* Set SNET and SADR to 0 for Router Device */
         pstHostDev->m_i32DADR = 0;
         memset(&pstHostDev->m_pstDeviceStruct->m_u8DvSADR[0], 0, MAX_MAC_LEN);
         pstHostDev->m_pstDeviceStruct->m_u16SNET = 0;    /* Pure IP Device */
     }

     /* update the object id property */
     stWpData.eObjectType = OBJECT_DEVICE;
     stWpData.eObjectProperty = PROP_OBJECT_IDENTIFIER;
 	stObjectId.m_eObjectType = OBJECT_DEVICE;
     stObjectId.m_u32ObjId = u32NewDevId;
     /* write the property value */
     Dv_Write_Prop(&stWpData, pVirtualDev, (void *)&stObjectId, READ_ONLY);

     /* update the entry in object list porperty of Virtual device */
     /* NOTE : 1st entry in object list is assumed to be od device itself
               as per the current stack design */
     pVirtualDev->m_stDevObject.m_stObjectIDList.
         m_pstArrayObjId->m_stObjectId.m_u32ObjId = u32NewDevId;

     //BACnetConfigData.m_u32DeviceID=u32NewDevId;  //kv
   //  SMCfg.g_BACnetCfg.ObjID = u32NewDevId;

    // SaveBACnetDatatoNV();

     /* update database revision property */
     Update_Database_Revision_Value(pVirtualDev);

     /* update device address binding */
 	Update_Device_Address_Binding(pVirtualDev);

 	/* send i am */
 	Send_I_AM(pVirtualDev, false, DESTINATION_IS_GLOBAL_BROADCAST);

     /* function exit */
 	#if(defined DEBUG_PRINTF && DL_3)
     Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "API: BACDEL_Change_Device_Id: exit \r\n");
 	#endif
     return BACDEL_SUCCESS;
 }

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
	uint32_t u32ObjId, const char *pu8ObjName)
{
	/* Virtual Device pointer */
	virtualDevData_t *pVirtualDev = NULL;
    /* Error/Success Status */
    BACNET_RETURN_TYPE  eAddObjectRet = BACDEL_SUCCESS;
	/* to save object size */
	ulong32_t ul32ObjSize = 0;
	/* to save object support */
	bool bApiRetVal = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Add_Object: Entry \r\n");
	#endif

    /* validate input parameters */
	if(u32DeviceId >= BACNET_MAX_INSTANCE || u32ObjId >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Object: \
		invalid object id \r\n");
		#endif
		return BACDEL_MAX_INSTANCE_ERROR;
	}

	/* Traverse the host device list */
	pVirtualDev = (virtualDevData_t *)Find_In_Host_Device_List(u32DeviceId);

    /* Traverse the Remote Device Object List, if no Host device with given device id
    found */
    if(NULL == pVirtualDev)
    {
		pVirtualDev = (virtualDevData_t *)Find_In_Remote_Device_List(u32DeviceId);
		/* Reached end of Remote Device Object List */
		if(NULL == pVirtualDev)
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Object: \
			Host with Device id %d is not present \r\n", u32DeviceId);
			#endif
			return BACDEL_DEVICE_NOT_PRESENT;
		}
    }

	/* get the object size */
    /* Note : used to check object support in stack */
    GET_OBJECT_SIZE(eObjType, ul32ObjSize);
	bApiRetVal = BACDEL_Check_Object_Support(u32DeviceId, eObjType);
    if(0 == ul32ObjSize || OBJECT_DEVICE == eObjType || !bApiRetVal
		#ifdef BACDEL_PR18
		|| OBJECT_NETWORK_PORT == eObjType
		#endif
    	)
    {
        /* object type not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Object: \
		object type is invalid \r\n");
		#endif
        return BACDEL_OBJECT_TYPE_ERROR;
    }

	/* Object count in a device cannot become more than Max limit (500) */
	if(pVirtualDev->m_stDevObject.m_stObjectIDList.m_u32ObjCount >= g_stStackMaxLimits.m_u32MaxObject)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Object: \
		object count exceeds the max limit \r\n");
		#endif
		return BACDEL_OBJECT_CANNOT_BE_CREATED;
	}

	/* object name should not be same as any previous object */
    if(BACDEL_DUPLICATE_OBJECT_NAME == Check_Object_Name(u32DeviceId,
        (int8_t *)pu8ObjName, NULL, NULL))
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Object: \
        Duplicate object name \r\n");
		#endif
        return BACDEL_DUPLICATE_OBJECT_NAME;
	}

    /* create the object */
	eAddObjectRet = Create_Object(eObjType,	pVirtualDev, u32ObjId, pu8ObjName);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Add_Object: Exit \r\n");
	#endif

    return eAddObjectRet;
}


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
	BACDEL_Delete_Object(uint32_t u32DeviceId, BACNET_OBJECT_TYPE eObjType, uint32_t u32ObjId)
{
    /* Device Object pointer to pass Remove_Obejct API */
    virtualDevData_t *pVirtualDev = NULL;
	/* Return type */
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	/* to check if this is configuration file object */
//	bool bIsConfigFile = FALSE;
	/* to save object size */
	ulong32_t ul32ObjSize = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Delete_Object: Entry \r\n");
	#endif

    /* validate input parameters */
	if(u32DeviceId >= BACNET_MAX_INSTANCE || u32ObjId >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Delete_Object: "
		"invalid object id \r\n");
		#endif
		return BACDEL_MAX_INSTANCE_ERROR;
	}

    /* Check if this is Host Device */
    pVirtualDev = (virtualDevData_t *)Find_In_Host_Device_List(u32DeviceId);
    /* If the Device is not Host Device then search Remote Device Object List */
    if(NULL == pVirtualDev)
    {
        pVirtualDev = (virtualDevData_t *)Find_In_Remote_Device_List(u32DeviceId);
		/* Check if the Device Object pointer is valid */
		if(NULL == pVirtualDev)
		{
			/* device not found */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Delete_Object: "
			"Host with Device id %d is not present \r\n", u32DeviceId);
			#endif
			return BACDEL_DEVICE_NOT_PRESENT;
		}
    }

	/* get the object size */
    /* Note : used to check object support in stack */
    GET_OBJECT_SIZE(eObjType, ul32ObjSize);
    if(0 == ul32ObjSize || OBJECT_DEVICE == eObjType
		#ifdef BACDEL_PR18
		|| OBJECT_NETWORK_PORT == eObjType
		#endif
    	)
    {
        /* object type not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Delete_Object: "
		"object type is invalid \r\n");
		#endif
        return BACDEL_OBJECT_TYPE_ERROR;
    }

	/* remove the object */
	eReturnVal = Remove_Object(eObjType, pVirtualDev, u32ObjId);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Delete_Object: Exit \r\n");
	#endif
    return eReturnVal;
}


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
    BACNET_APPLICATION_TAG *pvPropDataType)
{
	/* local variables */
    BACNET_RETURN_TYPE eFunRet = BACDEL_SUCCESS;
    PROP_ACCESS_TYPE ePermType = BACNET_DEFAULT;
    void *pvData = NULL;
    int32_t i32Data = 0;
	Pr_BACnetReal_t fAppData = {0};
    Float_t fData = 0.0;

    BACNET_APPLICATION_TAG  eAppTag = BACNET_APPLICATION_TAG_NULL;
    BACNET_REMAPPED_OBJECT_TYPE eRemapObjId = REMAPPED_MAX_BACNET_OBJECT_TYPE;

    /* Virtual Data pointer */
    virtualDevData_t *pVirtualDev = NULL;
    /* save the object base address */
    void *pvObjectPrt = NULL;
    /* flag to state if host device or remote device */
    bool bRemoteDev = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Add_Property: Entry \r\n");
	#endif

    /* validate input parameters */
	if(u32DevId >= BACNET_MAX_INSTANCE || u32ObjectID >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Property: "
		"invalid object id \r\n");
		#endif
		return BACDEL_MAX_INSTANCE_ERROR;
	}

	/* validate input pointers */
	if(NULL == pvPropAdd || NULL == pvPropDataType)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Add_Property: "
		"Null input pointers \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

    /* Check the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(u32DevId);

    /* If Device was not found in host list, find in remote device list */
    if(NULL == pVirtualDev)
    {
        pVirtualDev = Find_In_Remote_Device_List(u32DevId);
        if(NULL == pVirtualDev)
        {
            /* device not found, return error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Property: "
            "Device not found. \r\n");
			#endif
            return BACDEL_DEVICE_NOT_PRESENT;
        }
        else
            bRemoteDev = true;
    }

    /* Find the object */
    pvObjectPrt = Find_Object(eObjType, u32ObjectID, pVirtualDev);
    if(pvObjectPrt == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Property: "
        "Object not found. \r\n");
		#endif
        return BACDEL_OBJECT_NOT_PRESENT;
    }

    /* Get the property access type for given object & property. */
    ePermType = GetDefndPropAccess(eObjType, eDevProp, ePermission);
    ePermType = Validate_PropertySupport(eObjType, eDevProp, ePermType);
    if(ePermType == NOT_SUPPORTED)
    {
        eFunRet = BACDEL_PROPERTY_NOT_PRESENT;
    }
    else
    {
#ifdef BACDEL_SER_DS_RPM_B
        /* Register property Id in Property List */
        Register_Object_Property(eObjType, eDevProp);
#endif // #ifdef BACDEL_SER_DS_RPM_B

        /* Check Object Name is not already present in All Objects of Device */
        if(eDevProp == PROP_OBJECT_NAME)
        {
            if(BACDEL_DUPLICATE_OBJECT_NAME == Check_Object_Name(u32DevId, p_value,NULL,NULL))
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Add_Object : Duplicate object name\r\n");
				#endif
                return BACDEL_DUPLICATE_OBJECT_NAME;
			}
        }

        eRemapObjId = Remap_Objct_Id(eObjType);
        eAppTag = au8PropertyTagType[eDevProp][eRemapObjId];
        *pvPropDataType = eAppTag;
        switch(eAppTag)
        {
            case BACNET_APPLICATION_TAG_NULL:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_UNSIGNED_INT:
            case BACNET_APPLICATION_TAG_BOOLEAN:
            case BACNET_APPLICATION_TAG_SIGNED_INT:
            case BACNET_APPLICATION_TAG_ENUMERATED:
                i32Data = atoi(p_value);
                pvData = &i32Data;
                break;
            case BACNET_APPLICATION_TAG_REAL:
                 fData = (Float_t)atof(p_value);
                 pvData = &fData;
                break;
            case BACNET_APPLICATION_TAG_DOUBLE:
                fAppData.m_fVal = (Float_t)atof(p_value);
                pvData = &fAppData;
                break;
            case BACNET_APPLICATION_TAG_OCTET_STRING:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_BIT_STRING :
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_DATE:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_TIME:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_OBJECT_ID:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_DATETIME:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_LISTUNSIGN:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_LISTCHAR:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_PRIORTYARY:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_TIMESTAMP:
                pvData = p_value;
                break;
            case BACNET_APPLICATION_TAG_ACTIVECOV:
                pvData = p_value;
                break;
            default:
                pvData = p_value;
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAPI: BACDEL_Add_Property: Application "
				"tag is not supported \r\n");
				#endif
                break;
        }

        /* Update Void pointer for structure based data types used in
        AddCommonProperties functoin  */
        eFunRet = Convert_InStr_AddProperty(pVirtualDev, pvObjectPrt, u32ObjectID,
                eObjType, eDevProp, i8ArrayIndex, pvData, ePermType,
                pvPropAdd, bRemoteDev);

    }/* End of else block */

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Add_Property:Exit \r\n");
	#endif
    return eFunRet;
}

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
	BACNET_DATA_TYPE eDataType)
{
	/* local variables */
    BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
    virtualDevData_t *pVirtualDev = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Object_Property: Entry \r\n");
	#endif

    /* validate input parameters */
	if(u32DevId >= BACNET_MAX_INSTANCE || u32ObjectID >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Set_Object_Property: Invalid object id \r\n");
		#endif
		return ERROR_CODE_PARAMETER_OUT_OF_RANGE;
	}

    /* Check the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(u32DevId);
    if(NULL == pVirtualDev)
    {
        /* device not found, return error */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Set_Object_Property: Device not found \r\n");
		#endif
        return ERROR_CODE_UNKNOWN_DEVICE;
    }

	/* update property value */
    eErrorCode = Set_Property_Data(u32DevId, eObjType, u32ObjectID, pVirtualDev,
		eDevProp, pvWritePropVal, bArrayIndexPresent, u32ArrayIndex, u8Priority, eDataType);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Object_Property:Exit \r\n");
	#endif
	return eErrorCode;
}

/************************************************************************************************/
 /*****																					    *****/
 /*****  FUNCTION:	 Set_Baud_Rate											    			*****/
 /*****  DESCRIPTION: * API to set new supported Baud Rate									*****/
 /*****																					    *****/
 /***** 	@param BaudRateIndx			[in] Device in which object is to be added.				*****/
 /*****																						*****/
 /*****  @return            			[out]ERROR_CODE_SUCCESS on success.						*****/
 /*****																						*****/
 /*************************************************************************************************/
 BACNET_ERROR_CODE Set_Baud_Rate(eBaudRate_t BaudRateIndx)
 {
 	switch(BaudRateIndx)
 	{
 		case ebr_9600: //v43
 		{
 			/* re-initialize Baud Rate */
 			SMCfg.g_Com485Cfg.Baudrate = ebr_9600;
 		}
 		break;
 		case ebr_19200: //v43
 		{
 			/* re-initialize Baud Rate */
 			SMCfg.g_Com485Cfg.Baudrate = ebr_19200;
 		}
 		break;
 		case ebr_38400: //v43
 		{
 			/* re-initialize Baud Rate */
 			SMCfg.g_Com485Cfg.Baudrate = ebr_38400;
 		}
 		break;
 		default:
 			return ERROR_CODE_VALUE_OUT_OF_RANGE;

 	}
 	/*Update Baud rate init UartInit function & Reinit UART */
 	MX_USART2_UART_Init();	// Init UART for MSTP
 	EnableIntUartTim();      //Enable Interrupts for USART
 	UartConfig();
 	SaveFactoryDatatoNV();
 	return ERROR_CODE_SUCCESS;
 }
 /************************************************************************************************/
  /*****																					    *****/
  /*****  FUNCTION:	 Get_Baud_Rate											    			*****/
  /*****  DESCRIPTION: * API to Get  Baud Rate												*****/
  /*****																					    *****/
  /***** 	@param 						[in] Device in which object is to be added.				*****/
  /*****																						*****/
  /*****  @return   u32ReadBaudRate   [out]uint32 Variable on success.							*****/
  /*****																						*****/
  /*************************************************************************************************/
  uint32_t Get_BaudRate()
  {
  	uint32_t u32ReadBaudRate = 0;
  	//Read Baud rate from main structure and place to pointer location & return pointer
  	/*Read MACAddr From Flash Memory */

  	/*Updated in Local Variable*/
  	u32ReadBaudRate = enumToBaud(SMCfg.g_Com485Cfg.Baudrate);

  	/*return Local Variable*/
  	return u32ReadBaudRate;
  }

  /************************************************************************************************/
   /*****																					    *****/
   /*****  FUNCTION:	 SetMyMACAddress											    		*****/
   /*****  DESCRIPTION: API to Set MAC Address													*****/
   /*****																					    *****/
   /***** 	@param 	u8NewMACAddr		[in] Device in which object is to be added.				*****/
   /*****																						*****/
   /*****  @return eBACnetErrorCode_t  [out]ERROR_CODE_SUCCESS.								*****/
   /*****																						*****/
   /*************************************************************************************************/
   BACNET_ERROR_CODE SetMyMACAddress(uint8_t u8NewMACAddr)
   {
   	/*Validate Input Parameter*/
   	if(u8NewMACAddr > BN_DEVICE_MAX_MASTER)
   	{
   		/*return Value out of Range */
   		return ERROR_CODE_VALUE_OUT_OF_RANGE;
   	}

   	/*Update This Station with New MAC addr*/
   	gstBACnetVariables.m_u8ThisStation = u8NewMACAddr;

   	/* re-initialize the MAC address */
   	SMCfg.g_BACnetCfg.ComID = u8NewMACAddr;

   	/* write in Flash memory*/
    SaveBACnetDatatoNV();

   	/*Restart system */
   	NVIC_SystemReset();

   	/*Return Success*/
   	return ERROR_CODE_SUCCESS;
   }


    /************************************************************************************************/
    /*****																					    *****/
    /*****  FUNCTION:	 GetMyMACAddress											    		*****/
    /*****  DESCRIPTION: API to Get MAC Address													*****/
    /*****																					    *****/
    /***** 	@param 	None	 [in]																*****/
    /*****																						*****/
    /*****  @return uint8_t  [out] MAC addr u8ReadMACAddr.										*****/
    /*****																						*****/
    /*************************************************************************************************/
    uint8_t GetMyMACAddress()
    {
    	uint8_t u8ReadMACAddr = 0;
    	/*Read MACAddr From Flash Memory */

    	/*Updated in Local Variable*/
    	u8ReadMACAddr = SMCfg.g_BACnetCfg.ComID;

    	/*return Local Variable*/

    	return u8ReadMACAddr;
    }
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
	uint32_t *pu32ObjId)
{
	/* local variables */
	BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER;
	virtualDevData_t *pVirtualDev = NULL;
	void *PvTempPtr = NULL;
	BACNET_DATA_TYPE eDataType = BACNET_DT_MAX;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: "
	"BACDEL_Set_Object_Property_Multiple : Entry \r\n");
	#endif

	/* null check for input pointers */
	if(NULL == pstWpmData || NULL == pu8FirstFailElement ||
		NULL == peObjType || NULL == pePropId || NULL == pu32ObjId)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: "
		"BACDEL_Set_Object_Property_Multiple: Null input pointers \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

	/* set default value */
	*pu8FirstFailElement = 0;

	/* validate input parameters */
	if(u32DevId >= BACNET_MAX_INSTANCE || pstWpmData->m_u32ObjectInstance >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: "
		"BACDEL_Set_Object_Property_Multiple: Invalid object id \r\n");
		#endif
		return ERROR_CODE_PARAMETER_OUT_OF_RANGE;
	}

    /* Check the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(u32DevId);
    if(NULL == pVirtualDev)
    {
        /* device not found, return error */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: "
		"BACDEL_Set_Object_Property_Multiple: Device not found \r\n");
		#endif
        return ERROR_CODE_UNKNOWN_DEVICE;
    }

	/* copy input data and call set API */
	while(NULL != pstWpmData)
	{
		/* increment first failed element */
		(*pu8FirstFailElement)++;

		#ifdef NEW_RP_WP_INTERFACE
		/* save property data */
		PvTempPtr = pstWpmData->m_pvPropVal;
		eDataType = pstWpmData->m_eDataType;
		#endif

		/* set property values */
		eErrorCode = Set_Property_Data(u32DevId, pstWpmData->m_eObjectType,
			pstWpmData->m_u32ObjectInstance, pVirtualDev, pstWpmData->m_eObjectProperty,
			PvTempPtr, pstWpmData->m_u8ArrayIndexPresent,
			pstWpmData->m_u32ArrayIndex, (uint8_t)pstWpmData->m_u32Priority, eDataType);

		/* return error */
		if(MAX_BACNET_ERROR_CODE != eErrorCode)
		{
			/* update object type, object id and property id */
			*peObjType = pstWpmData->m_eObjectType;
			*pu32ObjId = pstWpmData->m_u32ObjectInstance;
			*pePropId = pstWpmData->m_eObjectProperty;
			 break;
		}

		/* move to next node */
		pstWpmData = pstWpmData->m_pstWPMNextElem;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: "
	"BACDEL_Set_Object_Property_Multiple :Exit \r\n");
	#endif
	return eErrorCode;
}

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
 BACNET_RETURN_TYPE BACDEL_Enable_Network_Communication(void)
{
    /* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

    /* fuction entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Enable_Network_Communication: Entry \r\n");
	#endif

	/* TODO - Need to resume Rx, Tx and Initiate threads if suspended */

	/* set the socket communication flag */
    g_bSocketCommFlag = TRUE;

	/* fuction exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Enable_Network_Communication: Exit \r\n");
	#endif
	return eRetVal;
}


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
 BACNET_RETURN_TYPE BACDEL_Disable_Network_Communication(void)
{
    /* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* fuction entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Disable_Network_Communication: Entry \r\n");
	#endif

	/* TODO - Need to suspend Rx, Tx and Initiate threads */

	/* reset the socket communication flag */
    g_bSocketCommFlag = FALSE;

	/* Clear initiate queue data if API is not called from De-init API */
	if(!g_bDeleteInitQueData)
	{
		/* Clear pending requets from Initiate queue */
		//Clear_Initiated_Request();
	}

	/* fuction exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Disable_Network_Communication: Exit \r\n");
	#endif
    return eRetVal;
}

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
    App_AutoResp_Interface_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_App_CallBack_Register: Entry \r\n");
	#endif

	if(NULL != fpFunction)
	{
		if(eAppChoice < MAX_APP_CALLBACK_FUN)
		{
			afpApplicationFunctionCb[eAppChoice] = fpFunction;
		}
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_App_CallBack_Register: Exit \r\n");
	#endif
}


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
	App_Callback_Interface_t fpFunctionPtr)
{
	/* local variables */
	initiator_response_t    stInitRet = {0};
    BACNET_RETURN_TYPE eResponseType = BACDEL_SUCCESS;
    uint32_t u32TmpGenID = 0;
    uint16_t u16TmpGenID = 0;
    uint32_t u32DefaultAPDUTimeout = 0;
    InitiateInfo_t *pstInitiateInfoQ = NULL;
    bacnetRequestData_t *pstReqParam = NULL;
	uint8_t u8Status = 0;
    BACnet_Device_Struct_t *pstHostDevice = NULL;
	bool bIsVdRequest = false;
	virtualDevData_t *pstVirtualData = NULL;
	BACnetAddrBinding_t *pstDeviceIDAdd = NULL;

	//printf("\n size = %d \n",sizeof(NotificationString_t));
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Generate_Aside_Request: Entry \r\n");
	#endif

    /* Validate the service request */
    eResponseType = Validate_Received_Request(pstServiceArgs);
    if(eResponseType != BACDEL_INITIATOR_SUCCESS)
    {
		/* Fill initiator error response */
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = eResponseType;
        stInitRet.m_i32TokenID = -1;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
		Request validation failed \r\n");
		#endif
        return stInitRet;
    }

    /* find the device with given "pu8SrcDADR", defualt use 1st device to initiate request */
    pstHostDevice = &gstHostDevice;

	/* get virtual device pointer */
	pstVirtualData = pstHostDevice->m_pstDeviceStruct;

	/* null check for device pointer */
	if(NULL == pstVirtualData)
	{
		/* Fill initiator error response */
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_ERROR;
        stInitRet.m_i32TokenID = -1;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
		Null virtual device pointer \r\n");
		#endif
        return stInitRet;
	}

	/* alocate the memory */
	pstInitiateInfoQ = (InitiateInfo_t *)OSAL_Malloc(sizeof(InitiateInfo_t),
		__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstInitiateInfoQ)
	{
		/* Deallocate the Queue location */
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_INITIATOR_NO_REQUEST_SPACE;
		stInitRet.m_i32TokenID = -1;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Generate_Aside_Request: No space in Initiate queue\r\n");
		#endif
		return stInitRet;
	}
	pstInitiateInfoQ->m_eServiceState = STATE_FILL_REQUEST;

    /* Get pointer to structure to fill request parameters */
    pstReqParam = &(pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData);

    /* Assign Virtual Device Generating Request */
    pstInitiateInfoQ->m_ReqProcInfo.m_pvReqDevStruct = pstVirtualData;
    /* Save the device Id */
    pstInitiateInfoQ->u32DeviceId = pstHostDevice->m_u32DeviceInstace;

    /* Set Destination address as received from application arguments,
       as in Encoding m_stRmDvAddr is used for Destination Device address */
    memcpy(&pstReqParam->m_stRmDvAddr, &pstServiceArgs->m_stDestBACnetAddr,
        sizeof(BACnetAddress_t));


    /* Lock the Process Queue for accquiring an avaliable queue location */
	#ifdef VIRTUAL_DEVICE_MANAGEMENT
	if(false == bIsVdRequest)
	{
	#endif
	/* check the socket communication status */
	if(FALSE == g_bSocketCommFlag)
	{
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_SOCKET_COMMUNICATION_DISABLED;
        stInitRet.m_i32TokenID = -1;
		/* free allocated initiate request queue node memory */
		OSAL_Free(pstInitiateInfoQ, __FILE__, __FUNCTION__, __LINE__);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Generate_Aside_Request: socket communication disabled \r\n");
		#endif
        return stInitRet;
	}
	/* check device communication status */
	else if(COMMUNICATION_DISABLE == pstVirtualData->m_eDevCommStatus ||
		(COMMUNICATION_DISABLE_INITIATION == pstVirtualData->m_eDevCommStatus &&
		SERVICE_SUPPORTED_I_AM != pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport))
	{
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_DCC_DISABLED;
        stInitRet.m_i32TokenID = -1;
		/* free allocated initiate request queue node memory */
		OSAL_Free(pstInitiateInfoQ, __FILE__, __FUNCTION__, __LINE__);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Generate_Aside_Request: DCC disabled \r\n");
		#endif
        return stInitRet;
	}

	/* search address binding list using BACnet address e.g. IP address */
	{
		/* search in static and dynamic address binding list */
		pstDeviceIDAdd = Find_Dev_Using_Address(&pstServiceArgs->m_stDestBACnetAddr);
	}

	/* Genrate Invoke ID */
	if(!Generate_Invoke_ID(&u16TmpGenID,
		pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport))
	{
		/* Fill initiator error response */
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_INITIATOR_NO_INVOKE_ID;
		stInitRet.m_i32TokenID = -1;
		/* free allocated initiate request queue node memory */
		OSAL_Free(pstInitiateInfoQ, __FILE__, __FUNCTION__, __LINE__);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
		Failed to generate Invoke ID \r\n");
		#endif
		return stInitRet;
	}
	/* save invoke id */
	pstReqParam->m_stAPDU.m_u8InvokeId = (uint8_t)u16TmpGenID;
	pstServiceArgs->m_stNPDUData.m_stAPDUData.m_i32InvokeID = (uint8_t)u16TmpGenID;

	/* acquire the mutex lock */
    if(Osal_Wait_Mutex(m_hInitMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
	{
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_INITIATOR_MUTEX_LOCK_FAILED;
        stInitRet.m_i32TokenID = -1;
		/* Free Invoke ID */
		Free_Invoke_ID((uint8_t)u16TmpGenID);
		/* free allocated initiate request queue node memory */
		OSAL_Free(pstInitiateInfoQ, __FILE__, __FUNCTION__, __LINE__);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Generate_Aside_Request: Process queue lock falied\r\n");
		#endif
        return stInitRet;
	}

    /* Check Maximum allowed Initiate request and Communication is enabled or not */
	if(g_stReqInitiateDataQue.m_i32InitiateQFillIndex <
		(int32_t)g_stStackMaxLimits.m_u32InitiateQueNodes)
    {
		/* Increment total number of Initiate queue elements */
        ++g_stReqInitiateDataQue.m_i32InitiateQFillIndex;

        /* If Initiate queue is empty, Add 1st node */
        if(g_stReqInitiateDataQue.m_pstInitiateInfoQ == NULL)
        {
			g_stReqInitiateDataQue.m_pstInitiateInfoQ = pstInitiateInfoQ;
		}
		else
		{
			/* If Initiate queue is not empty, Add last node */
			g_stReqInitiateDataQue.m_pstInitiateInfoQ_End_Address->pstNextAddress = pstInitiateInfoQ;
			pstInitiateInfoQ->pstNextAddress = NULL;
			//pstInitiateInfoQ->pstPreviousAddress = g_stReqInitiateDataQue.m_pstInitiateInfoQ_End_Address;
		}
    }
    else
    {
        /* Deallocate the Queue location */
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_INITIATOR_QUEUE_FULL;
        stInitRet.m_i32TokenID = -1;
		/* Free Invoke ID */
		Free_Invoke_ID((uint8_t)u16TmpGenID);
		/* free allocated initiate request queue node memory */
		OSAL_Free(pstInitiateInfoQ, __FILE__, __FUNCTION__, __LINE__);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Generate_Aside_Request: No space in Initiate queue\r\n");
		#endif
        Osal_Release_Mutex(m_hInitMtxLockHandle);
        return stInitRet;
    }

	g_stReqInitiateDataQue.m_pstInitiateInfoQ_End_Address = pstInitiateInfoQ;

	/* Unlock Mutex */
	Osal_Release_Mutex(m_hInitMtxLockHandle);

    /* Check callback function for this request is assigned or not */
    if(NULL != fpFunctionPtr)
    {
        /* Fill in the Application Callback address */
        pstInitiateInfoQ->pvCallBackFunc = fpFunctionPtr;
    }

    /* Set parametes used in Handling RPM fallback */
    pstInitiateInfoQ->m_bRPMError = false;
    pstInitiateInfoQ->m_i16ObjListCnt = -1;

    /* Genrate Token ID and Fill service paarameters */
    u32TmpGenID = Generate_Token_ID();
    if(INVALID_TOKEN_ID == u32TmpGenID)
    {
        /* No unused Token ID available */
		/* Fill initiator error response */
		Fill_Initiator_Response(BACDEL_INITIATOR_NO_TOKEN_ID, &stInitRet, &pstInitiateInfoQ);
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
						Can't assign Token ID\r\n");
		#endif
        return stInitRet;
    }
    pstInitiateInfoQ->u32TokenID = u32TmpGenID;
    pstServiceArgs->m_i32TokenID = pstInitiateInfoQ->u32TokenID;

    /* Initialise IPArgument sructure as 0 */
    memcpy(&(pstInitiateInfoQ->m_stIpArgs), pstServiceArgs, sizeof(bacnetip_arguments_t));
    memset(&(pstInitiateInfoQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice), 0, sizeof(service_choice_u));

    /* Fill BVLC function type */
	pstReqParam->m_eBVLCFunctionType = pstServiceArgs->m_eBvlcFunctionType;

    /* Save the destination type */
	pstReqParam->m_stNPDU.eDestinationType = pstServiceArgs->m_stNPDUData.m_eDestinationType;

    /* Calculate APDU Timeout value */
	u32DefaultAPDUTimeout = pstVirtualData->m_stDevObject.
		m_stAPDUTimeout.m_u32Val/CONVERT_TO_SECONDS;

#ifdef SEGMENTATION_SUPPORTED
	/* initialize m_stDstInfo structure with default values */
	pstReqParam->m_stDstInfo.m_eSegmentationSupport = SEGMENTATION_BOTH;
	pstReqParam->m_stDstInfo.bDstSegSupport = TRUE;
	pstReqParam->m_stDstInfo.m_u16MaxAPDULenAccepted = MAX_APDU_LENGTH_ACCEPTED;

	/* Fill recipient information */
	if(false == bIsVdRequest)
	{
		/* check null pointer */
		if(NULL != pstDeviceIDAdd)
		{
			/* device present in address binding list */
			pstReqParam->m_stDstInfo.m_u16MaxAPDULenAccepted =
				pstDeviceIDAdd->m_u16MaxAPDULenAccepted;
			pstReqParam->m_stDstInfo.m_eSegmentationSupport =
				pstDeviceIDAdd->m_eSegmentationSupport;
			/* if destination device support receiving segmented messages, than set the flag */
			if(pstDeviceIDAdd->m_eSegmentationSupport == SEGMENTATION_BOTH ||
				pstDeviceIDAdd->m_eSegmentationSupport == SEGMENTATION_RECEIVE)
				pstReqParam->m_stDstInfo.bDstSegSupport = TRUE;
			else pstReqParam->m_stDstInfo.bDstSegSupport = FALSE;
		}

	}// end of (false == bIsVdRequest)

	/* check device object properties & set the respective parameters for apdu_data */
	if( (pstVirtualData->m_stDevObject.
		m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_RECEIVE) ||
		(pstVirtualData->m_stDevObject.
		m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_BOTH) )
	{
		/* segmented response accepted */
		pstReqParam->m_stAPDU.m_bSegmentedResponseAccepted = TRUE;
	}
	else pstReqParam->m_stAPDU.m_bSegmentedResponseAccepted = FALSE;
	/* max segments accepted */
	pstReqParam->m_stAPDU.m_i32MaxSegs =
		pstVirtualData->m_stDevObject.m_stMaxSegAccepted.m_u16Val;
	/* max apdu_length_accepted */
	pstReqParam->m_stAPDU.m_i32MaxResp =
		pstVirtualData->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val;
#else

	pstReqParam->m_stAPDU.m_bSegmentedMessage = SEGMENTATION_NONE;
		pstReqParam->m_stAPDU.m_bSegmentedResponseAccepted = FALSE;

	/* MAX apdu length accepted by device */
	pstReqParam->m_stAPDU.m_i32MaxResp = MAX_APDU_LENGTH_ACCEPTED;
	/* Max segments accepted by device */
	pstReqParam->m_stAPDU.m_i32MaxSegs = 0;
#endif /* SEGMENTATION_SUPPORTED */

	/* Set APDU parameters */
    pstReqParam->m_i32DevTimeout = u32DefaultAPDUTimeout;
    pstReqParam->m_i32ClearIQTimer = g_stStackConfigParams.m_u32ClearInitQTimeout;
    pstReqParam->m_i32DevRetryCnt = 0;

	/* set service choice */
	pstReqParam->m_eServiceSupported = pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport;

    /* fill Parameters as per Service Choice */
    switch(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport)
    {
        #ifdef BACDEL_SER_DM_DDB_A
        case SERVICE_SUPPORTED_WHO_IS:
        {
            /* To extract the device instance range parameter */
            ddb_who_is_t *pstInstanceRange = NULL;

            /* Set service Choice */
            pstReqParam->m_stAPDU.m_u8ServiceChoice =
                SERVICE_UNCONFIRMED_WHO_IS;

            /* Set NPDU parameters */
            NPDU_Set_Data(&pstReqParam->m_stNPDU, FALSE,
                pstServiceArgs->m_ePriority, pstServiceArgs->m_stNPDUData.m_bIsNwLayerMsg,
				NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK);

			/* Set APDU type */
            pstReqParam->m_ePDUType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;

			/* Allocate buffer for WHO IS service data */
            pstInstanceRange = (ddb_who_is_t *)
                OSAL_Malloc(sizeof(ddb_who_is_t),  __FILE__, __FUNCTION__, __LINE__);
            /* Check if memeory is allocated */
            if(NULL == pstInstanceRange)
            {
				/* Fill initiator error response */
				Fill_Initiator_Response(BACDEL_INITIATOR_MALLOC_ERROR,
					&stInitRet, &pstInitiateInfoQ);
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
				pstInstanceRange malloc failed \r\n");
				#endif
				return stInitRet;
            }
            /* assign the service data pointer */
            pstReqParam->m_stAPDU.m_pvServiceRequestData = pstInstanceRange;

            /* Update the device instance range */
            pstInstanceRange->m_i32DevRangeLowLimit = pstServiceArgs->
                m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWHO_IS_Request.m_i32DevRangeLowLimit;
            pstInstanceRange->m_i32DevRangeHighLimit = pstServiceArgs->
                m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stWHO_IS_Request.m_i32DevRangeHighLimit;

			/* save the network no for who-is-router-to-network service */
			if(true == pstServiceArgs->m_stNPDUData.m_bIsNwLayerMsg)
			{
				/* save the network number */
				pstReqParam->m_stNPDU.u16RtrNetNumber =
					pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_u16RtrNetNumber;
			}

			/* Fill the destination parameter for request */
			u8Status = Fill_Destination_Parameter(pstReqParam, pstServiceArgs);

			/* check if error occurred */
			if(BACDEL_SUCCESS != u8Status)
			{
				/* Fill initiator error response */
				Fill_Initiator_Response(u8Status, &stInitRet, &pstInitiateInfoQ);
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
					BACDEL_Generate_Aside_Request: Who-Is Initiator error \r\n");
				#endif
				return stInitRet;
			}

			#if (STACK_CONFIG_CLR_DYNAMIC_BIND_ON_WHOIS_GB)
			if(pstServiceArgs->m_stNPDUData.m_eDestinationType ==
				    DESTINATION_IS_GLOBAL_BROADCAST)
            {
                /* clear previous global dev_address_binding list whenever
				global_broadcast is generated with no HL and LL */
                if(BACNET_PARAMETER_ABSENT == pstInstanceRange->m_i32DevRangeHighLimit &&
                    BACNET_PARAMETER_ABSENT == pstInstanceRange->m_i32DevRangeLowLimit)
                {
                    /* clear global device address binding list */
                    Clear_Global_Device_Addr_Binding_List();
                }
            }
			#endif
        }
        break;
        #endif /* BACDEL_SER_DM_DDB_A */

        #ifdef BACDEL_SER_DM_DDB_B
        case SERVICE_SUPPORTED_I_AM:
        {
            /* Pointer to store I-AM data */
            ddb_i_am_t *pstDDBDataBuf = NULL;

            /* Set service Choice */
            pstReqParam->m_stAPDU.m_u8ServiceChoice =
                SERVICE_UNCONFIRMED_I_AM;

			/* Set NPDU parameters */
            NPDU_Set_Data(&pstReqParam->m_stNPDU, FALSE,
               pstServiceArgs->m_ePriority, pstServiceArgs->m_stNPDUData.m_bIsNwLayerMsg,
				NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK);

			/* Set APDU type */
			pstReqParam->m_ePDUType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;

            /* Allocate buffer for WHO HAS service data */
            pstDDBDataBuf = (ddb_i_am_t *)
                OSAL_Malloc(sizeof(ddb_i_am_t),  __FILE__, __FUNCTION__, __LINE__);
            /* Check if memeory is allocated */
            if(NULL == pstDDBDataBuf)
            {
				/* Fill initiator error response */
				Fill_Initiator_Response(BACDEL_INITIATOR_MALLOC_ERROR,
					&stInitRet, &pstInitiateInfoQ);
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
				pstDDBDataBuf malloc failed \r\n");
				#endif
                return stInitRet;
            }

            /* Fill the service data */
            pstDDBDataBuf->m_u32DeviceInstance
                = pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_AM_Request.m_u32DeviceInstance;
            pstDDBDataBuf->m_eSegmentationSupported = pstServiceArgs->m_stNPDUData.
                m_stAPDUData.m_stServiceChoice.m_stI_AM_Request.
				m_eSegmentationSupported;
            pstDDBDataBuf->m_u32VendorID
                = pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_AM_Request.m_u32VendorID;
            pstDDBDataBuf->m_u16MaxAPDUlengthAccepted =
                pstServiceArgs->m_stNPDUData.m_stAPDUData.
                m_stServiceChoice.m_stI_AM_Request.
				m_u16MaxAPDUlengthAccepted;

            /* Assign service data pointer */
            pstReqParam->m_stAPDU.m_pvServiceRequestData = pstDDBDataBuf;

			/* Fill the destination parameter for request */
			u8Status = Fill_Destination_Parameter(pstReqParam, pstServiceArgs);

			/* check if error occurred */
			if(BACDEL_SUCCESS != u8Status)
			{
				/* Fill initiator error response */
				Fill_Initiator_Response(u8Status, &stInitRet, &pstInitiateInfoQ);
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
					BACDEL_Generate_Aside_Request: I-AM Initiator error \r\n");
				#endif
				return stInitRet;
			}
        }
        break;
        #endif /* BACDEL_SER_DM_DDB_B */
        #ifdef BACDEL_SER_DS_COV_B
        case SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION:
        case SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION:
            {
                if(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport
                    == SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION)
                {
                    /* Set service Choice */
                    pstReqParam->m_stAPDU.m_u8ServiceChoice =
                        SERVICE_CONFIRMED_COV_NOTIFICATION;

                    /* Set NPDU parameters */
                    NPDU_Set_Data(&pstReqParam->m_stNPDU, TRUE, //<@>
                        pstServiceArgs->m_ePriority, false, 0);

                    if(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
						m_stCOV_Notification.m_u8SendSimpleAck == FALSE)
                    {
						/* Set APDU type */
                        pstReqParam->m_ePDUType = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;

                        pstReqParam->m_stAPDU.m_pvServiceRequestData =
                            pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                                m_stCOV_Notification.pvCOVValue;
                    }
                    else
                    {
                        /* Send simple ack for COV notifiaction */
                        pstReqParam->m_ePDUType = PDU_TYPE_SIMPLE_ACK;

                        pstReqParam->m_stAPDU.m_u8InvokeId = (uint8_t)
                            pstServiceArgs->m_stNPDUData.m_stAPDUData.
                            m_i32InvokeID;

                        pstReqParam->m_stAPDU.m_pvServiceRequestData = NULL;
                    }
                }
                else if(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport ==
                    SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION)
                {

                    /* Set service Choice */
                    pstReqParam->m_stAPDU.m_u8ServiceChoice =
                        SERVICE_UNCONFIRMED_COV_NOTIFICATION;

					/* Set APDU type */
                    pstReqParam->m_ePDUType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;

                    /* Set NPDU parameters */
                    NPDU_Set_Data(&pstReqParam->m_stNPDU, FALSE, //<@>
                        pstServiceArgs->m_ePriority, false, 0);

                    pstReqParam->m_stAPDU.m_pvServiceRequestData =
                        pstServiceArgs->m_stNPDUData.m_stAPDUData.
                        m_stServiceChoice.m_stCOV_Notification.pvCOVValue;
                }
            }
        break;
        #endif /* BACDEL_SER_DS_COV_B */

    	#ifdef BACDEL_SER_AE_EN_B
        case SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION:
        case SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION:
        {
            if(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport
                == SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION)
            {
                /* Set service Choice */
                pstReqParam->m_stAPDU.m_u8ServiceChoice =
                    SERVICE_CONFIRMED_EVENT_NOTIFICATION;

                /* Set NPDU parameters */
//                NPDU_Set_Data(&pstReqParam->m_stNPDU, TRUE, <@>
//                    pstServiceArgs->m_ePriority, false, 0);

                if(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
					m_stEvent_Notification.m_u8SendSimpleAck == FALSE)
                {
                    pstReqParam->m_ePDUType = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;

                    /* set service data */
                    pstReqParam->m_stAPDU.m_pvServiceRequestData =
                        pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                            m_stEvent_Notification.m_pvEventValues;
                }
                else
                {
                    /* Send simple ack for event notification */
                    pstReqParam->m_ePDUType = PDU_TYPE_SIMPLE_ACK;

                    pstReqParam->m_stAPDU.m_u8InvokeId = (uint8_t)
                        pstServiceArgs->m_stNPDUData.m_stAPDUData.
                        m_i32InvokeID;

					pstReqParam->m_stNPDU.data_expecting_reply = FALSE;
                    //pstReqParam->m_stAPDU.m_pvServiceRequestData = NULL;
                }
            }
            else if(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport ==
                SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION)
            {
                /* Set service Choice */
                pstReqParam->m_stAPDU.m_u8ServiceChoice =
                    SERVICE_UNCONFIRMED_EVENT_NOTIFICATION;

                pstReqParam->m_ePDUType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;

                /* Set NPDU parameters */
//                NPDU_Set_Data(&pstReqParam->m_stNPDU, FALSE, <@>
//                    pstServiceArgs->m_ePriority, false, 0);

                pstReqParam->m_stAPDU.m_pvServiceRequestData =
                    pstServiceArgs->m_stNPDUData.m_stAPDUData.
                    m_stServiceChoice.m_stEvent_Notification.m_pvEventValues;
            }
        }
        break;
        #endif /* BACDEL_SER_AE_EN_B */

		#ifdef BACDEL_SER_DM_DOB_B
        case SERVICE_SUPPORTED_I_HAVE:
        {
            /* Pointer to store WHO-HAS data */
            dob_i_have_t *pstDOBDataBuf = NULL;

            /* Set service Choice */
            pstReqParam->m_stAPDU.m_u8ServiceChoice =
				SERVICE_UNCONFIRMED_I_HAVE;

			/* Set NPDU parameters */
            NPDU_Set_Data(&pstReqParam->m_stNPDU, FALSE,
                pstServiceArgs->m_ePriority, false, 0);

			/* Set APDU type */
            pstReqParam->m_ePDUType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;

            /* Allocate buffer for WHO HAS service data */
            pstDOBDataBuf = (dob_i_have_t *)
                OSAL_Malloc(sizeof(dob_i_have_t),  __FILE__, __FUNCTION__, __LINE__);
            /* Check if memeory is allocated */
            if(NULL == pstDOBDataBuf)
            {
				/* Fill initiator error response */
				Fill_Initiator_Response(BACDEL_INITIATOR_MALLOC_ERROR,
					&stInitRet, &pstInitiateInfoQ);
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
				pstDOBDataBuf malloc failed \r\n");
				#endif
                return stInitRet;
            }
            /* Assign the pointer */
            pstReqParam->m_stAPDU.m_pvServiceRequestData = pstDOBDataBuf;

			 /* Fill the service data */
			pstDOBDataBuf->m_u32DeviceInstance =
				pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_HAVE_Request.m_u32DeviceInstance;
			pstDOBDataBuf->m_eObjectType = pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_HAVE_Request.m_eObjectType;
			pstDOBDataBuf->m_u32ObjectInstance =
                pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_HAVE_Request.m_u32ObjectInstance;
			/* Copy request data */
			memcpy(&pstDOBDataBuf->m_stObjName,
                &pstServiceArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_HAVE_Request.m_stObjName,
                sizeof(BACnetCharStr_t));

			/* Fill the destination parameter for request */
			u8Status = Fill_Destination_Parameter(pstReqParam, pstServiceArgs);

			/* check if error occurred */
			if(BACDEL_SUCCESS != u8Status)
			{
				/* Fill initiator error response */
				Fill_Initiator_Response(u8Status, &stInitRet, &pstInitiateInfoQ);
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
					BACDEL_Generate_Aside_Request: I have Initiator error \r\n");
				#endif
				return stInitRet;
			}
        }
        break;
        #endif /* BACDEL_SER_DM_DOB_A */

#ifdef BACDEL_PR23
		#ifdef BACDEL_SER_DM_DDA_A
		case SERVICE_SUPPORTED_YOU_ARE:
		{
			/* Pointer to store YOU-ARE data */
			dda_you_are_t *pstYouAreData = NULL;

			/* Set service Choice */
			pstReqParam->m_stAPDU.m_u8ServiceChoice =
				SERVICE_UNCONFIRMED_YOU_ARE;

			/* Set NPDU parameters */
			NPDU_Set_Data(&pstReqParam->m_stNPDU, FALSE,
				pstServiceArgs->m_ePriority, false, 0);

			/* Set APDU type */
			pstReqParam->m_ePDUType = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;

			/* Allocate buffer for YOU ARE service data */
			pstYouAreData = (dda_you_are_t *)
				OSAL_Malloc(sizeof(dda_you_are_t), __FILE__, __FUNCTION__, __LINE__);
			/* Check if memeory is allocated */
			if(NULL == pstYouAreData)
			{
				/* Fill initiator error response */
				Fill_Initiator_Response(BACDEL_INITIATOR_MALLOC_ERROR,
					&stInitRet, &pstInitiateInfoQ);
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INITIATOR_MALLOC_ERROR, "API: BACDEL_Generate_Aside_Request: \
				pstYouAreData malloc failed \r\n");
				#endif
				return stInitRet;
			}
			/* Assign the pointer */
			pstReqParam->m_stAPDU.m_pvServiceRequestData = pstYouAreData;

			/* Fill the service data */
			memcpy(pstYouAreData, &pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stYOU_ARE_Request,
				sizeof(dda_you_are_t));

			/* Fill the destination parameter for request */
			u8Status = Fill_Destination_Parameter(pstReqParam, pstServiceArgs);

			/* check if error occurred */
			if(BACDEL_SUCCESS != u8Status)
			{
				/* Fill initiator error response */
				Fill_Initiator_Response(u8Status, &stInitRet, &pstInitiateInfoQ);
				#if(defined DEBUG_PRINTF && DL_1)
				Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INITIATOR_ERROR, "API: \
				BACDEL_Generate_Aside_Request: YOU-ARE Initiator error \r\n");

				#endif
				return stInitRet;
			}
		}
		break;
		#endif /* BACDEL_SER_DM_DDA_A */
#endif /* BACDEL_PR23 */
    	default:
        {
            /* Notify user service is not supported */
			/* Fill initiator error response */
//			Fill_Initiator_Response(BACDEL_INITIATOR_UNKNOWN_SERVICE_CHOICE, <@>
//				&stInitRet, &pstInitiateInfoQ);
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
						BACDEL_Generate_Aside_Request: Unknown service \r\n");
			#endif
            return stInitRet;
        }

    }// End of switch

	if(pstInitiateInfoQ->m_eServiceState == STATE_FILL_REQUEST)
	{
		/* Change status to send request */
		pstInitiateInfoQ->m_eServiceState = STATE_SEND_REQUEST;
        {
		    /* Release semaphore for InitiateService thread */
		    /* This function increases the count of the Receive
		     * semaphore object by a 1.
		     * ReleaseSemaphore(handle to semaphore, count, prevoius count) */
		    if(!OSAL_Release_Sem(g_hInitiateSemaphoreHandle, BACNET_ONE))
		    {
			    /* Can't send request */
				#ifdef DEBUG_PRINTF
			    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: BACDEL_Generate_Aside_Request: \
								    ReleaseSemaphore error: %ld", Osal_Get_Last_Error());
				#endif
			    /* Fill initiator error response */
			    Fill_Initiator_Response(BACDEL_INITIATOR_SEMAPHORE_FAILED,
				    &stInitRet, &pstInitiateInfoQ);
			    return stInitRet;
		    }
        }

		/* Request is sent succefully */
		stInitRet.m_eRetType = BACDEL_INITIATOR_SUCCESS;
		stInitRet.m_i32TokenID = pstServiceArgs->m_i32TokenID;
	}
	else
	{
		/* For ERROR function should return from above but still
			coming here because of some Synchronisation issue */
		stInitRet.m_eRetType = BACDEL_INITIATOR_ERROR;
		stInitRet.m_eErrorCode = BACDEL_OTHER;
        stInitRet.m_i32TokenID = -1;

	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Generate_Aside_Request: Exit \r\n");
	#endif
    return stInitRet;
}
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
	BACDEL_Set_Default_Parameters(bacnetip_arguments_t *pstBACnetArgs, int8_t i8ServiceSupport)
{
	/* local variables */
	uint16_t u16PortNum = DEFAULT_PORT_NUMBER;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Set_Default_Parameters: Entry \r\n");
	#endif

	/* validate input parameters */
	if(NULL == pstBACnetArgs)
	{
		/* value is out of range */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Set_Default_Parameters: Invalid input parameters \r\n");
		#endif
		return;
	}

	/* set all internal variables to 0 */
    memset(pstBACnetArgs, BACNET_ZERO, sizeof(bacnetip_arguments_t));

	/* switch to respective service */
    switch(i8ServiceSupport)
    {
	#ifdef BACDEL_SER_DM_DDB_A
    case BACDEL_REQ_WHO_IS:
        pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport
            = SERVICE_SUPPORTED_WHO_IS;
		pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stWHO_IS_Request.m_i32DevRangeHighLimit = BACNET_PARAMETER_ABSENT;
		pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stWHO_IS_Request.m_i32DevRangeLowLimit = BACNET_PARAMETER_ABSENT;
        break;
	#endif    /* DDB-A */
		#ifdef BACDEL_SER_DM_DDB_B
			case BACDEL_REQ_I_AM:
		pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport
			= SERVICE_SUPPORTED_I_AM;
		break;
	#endif /* DDB-B */

	#ifdef BACDEL_SER_DM_DOB_B
	case BACDEL_REQ_I_HAVE:
		pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport
			= SERVICE_SUPPORTED_I_HAVE;
		break;
	#endif /* DOB-B */
#ifdef BACDEL_PR23
#ifdef BACDEL_SER_DM_DDA_A
	case BACDEL_REQ_YOU_ARE:
		pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport
		= SERVICE_SUPPORTED_YOU_ARE;
		break;
#endif /* DDA-A */
#endif /* BACDEL_PR23 */
    default:
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Set_Default_Parameters: \
		Option Not valid \r\n");
		#endif
        break;
    }

	/* set default destination device ID */
	pstBACnetArgs->m_u32DeviceID = BACNET_MAX_INSTANCE;

    /* Set the MAC Length */
    pstBACnetArgs->m_stDestBACnetAddr.u8mac_len = MAX_MAC_LEN;
	/* Set the DNET to 0 */
	pstBACnetArgs->m_stDestBACnetAddr.u16net = 0;
    /* Set DADR to all empty */
    pstBACnetArgs->m_stDestBACnetAddr.u8dlen = 0;
	memset(pstBACnetArgs->m_stDestBACnetAddr.u8DvDadr, 0x00, MAX_MAC_LEN);

    /* Fill in the NPDU priority */
    pstBACnetArgs->m_ePriority = DEFAULT_NPDU_PRIORITY;

    /* set default token id */
    pstBACnetArgs->m_i32TokenID = -1;

    /* Set default BVLC type to unicast */
    pstBACnetArgs->m_eBvlcFunctionType = BVLC_ORIGINAL_UNICAST_NPDU;


	/* Fill if network layer message or not i.e contains apdu or not */
	pstBACnetArgs->m_stNPDUData.m_bIsNwLayerMsg = false;
	pstBACnetArgs->m_stNPDUData.m_eMsgType = NETWORK_MESSAGE_INVALID;
    pstBACnetArgs->m_stNPDUData.m_eDestinationType = DESTINATION_IS_UNICAST;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Set_Default_Parameters: Exit \r\n");
	#endif
}

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
 void BACDEL_Get_Memory_Usage(ulong32_t *pul32PeakUsage, ulong32_t *pul32CurrUsage)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Get_Memory_Usage: Entry \r\n");
	#endif

	if(NULL != pul32PeakUsage && NULL != pul32CurrUsage)
	{
#if 0 //PP
		memcpy(pul32PeakUsage, &stBACnetStackMemUsage.i32PeakMemoryUsage, sizeof
			(int32_t));
		memcpy(pul32CurrUsage, &stBACnetStackMemUsage.i32CurrMemoryUsage, sizeof
			(int32_t));
#endif
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Get_Memory_Usage: Exit \r\n");
	#endif
}


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
	uint32_t u32DestSize, bool bArrayIndexPresent)
{
#ifndef INITIATE_SERVICE_ENABLED
	return 0;
#else
    /* Read Property Data */
    BACNET_CONF_DATA *pRpData = NULL;
    /* Device Instace Pointer */
    virtualDevData_t *pVirtualDev = NULL;      
	/* Process Q data */
	processInfo_t stPropQData = {0};
    /* Data Buffer */
	uint8_t u8APDUResp[MAX_APDU_LENGTH_ACCEPTED] = {0};
    /* Decoded data lenght */
    int32_t     i32Len = 0;
    /* Application data first instance */
    BACNET_PROPERTY_VALUE appDataValue = {0};
	/* pointer */
	uint8_t *pu8APDUResp = NULL;
	/* to save string length */
	//int32_t i32StringLen = 0;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Get_Object_Property_String: Entry \r\n");
	#endif

    /* validate input parameters */
	if(u32DevId >= BACNET_MAX_INSTANCE || u32ObjId >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property_String: \
		invalid object id \r\n");
		#endif
		return ERROR_CODE_PARAMETER_OUT_OF_RANGE;
	}

	/* Check for null input pointers */
	if(NULL == pvReadData)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
		BACDEL_Get_Object_Property_String: Null input pointer. \r\n");
		#endif
		return ERROR_CODE_OTHER;
	}

    /* Check if this is Host Device */
    pVirtualDev = Find_In_Host_Device_List(u32DevId);

    /* If the Device is not Host Device then search Remote Device Object List */
    if(NULL == pVirtualDev)
    {
        pVirtualDev = Find_In_Remote_Device_List(u32DevId);
        /* Check if the Device Object pointer is valid */
        if(NULL == pVirtualDev)
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			BACDEL_Get_Object_Property_String: unknown object \r\n");
			#endif
            return ERROR_CODE_UNKNOWN_DEVICE;
        }
    }

    /* Allocate memory for Read Data */
    pRpData = (BACNET_CONF_DATA *)OSAL_Malloc(sizeof(BACNET_CONF_DATA),
		__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pRpData)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property_String: malloc failed \r\n");
		#endif
		return ERROR_CODE_OUT_OF_MEMORY;
	}

	/* Fill in the RP-B parameters in allocated Read data request */
    Generate_RPB_Data(u32ObjId, eObjectType, ePropId, u32ArrayIndex,
		pRpData, bArrayIndexPresent);
    /* Read the desired property */
    //Read_Object_Property(pVirtualDev, pRpData);
	stPropQData.m_stProcessData.m_stAPDU.m_pvServiceRequestData = pRpData;
	// TODO - fill any other required stPropQData parameters
	RP_B_Request_Parser(pVirtualDev, &stPropQData, THREADPOOL_COUNT);
    /* If there is no error the encode the read value */
    if(!pRpData->bErrorStatus)
    {
		/* encode property value */
        pRpData->i32ApplicationDataLen =
        BACApp_Encode_Data_Type(&u8APDUResp[0],
                                MAX_APDU_LENGTH_ACCEPTED,
                                pRpData->pvReadPropValue,
                                pRpData->eData_Type,
                                pRpData->u32ArrayIndex,
                                BACNET_ZERO,
                                pVirtualDev->m_stDevObject.
                                m_stSegmentationSupport.m_eSegmentationSupport,
                                bArrayIndexPresent,
                                pVirtualDev->m_stDevObject.
                                m_stMaxAPDULenAccepted.m_u16Val, NULL, NULL, 0);

		/* check for return value */
		if(pRpData->i32ApplicationDataLen < 0)
		{
			/* set error code */
			if(DATA_TYPE_NOT_SUPPORTED == pRpData->i32ApplicationDataLen)
			{
				eErrorCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
			}
			else if(APDU_DATA_BUFFER_SIZE_EXCEEDED == pRpData->i32ApplicationDataLen)
			{
				eErrorCode = ERROR_CODE_ABORT_BUFFER_OVERFLOW;
			}
			else if(SEGMENTATION_NOT_SUPPORTED == pRpData->i32ApplicationDataLen)
			{
				eErrorCode = ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
			}
			/* Free memory used for Read property request parameter */
			OSAL_Free(pRpData,  __FILE__, __FUNCTION__, __LINE__);
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			BACDEL_Get_Object_Property_String : encoding failed \r\n");
			#endif
			return eErrorCode;
		}
    }
    else
    {
		eErrorCode = pRpData->eErrorCode;
        /* Free memory used for Read property request parameter */
        OSAL_Free(pRpData,  __FILE__, __FUNCTION__, __LINE__);
	    pRpData = NULL;
        /* In case of Invalid array Index */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property_String : invalid array index \r\n");
		#endif
        return (eErrorCode);
    }

    /* Decode the some of the request */
    i32Len =
        BACApp_Decode_Application_Data(&u8APDUResp[0],
        (uint32_t)pRpData->i32ApplicationDataLen, &appDataValue,
        pRpData->eObjectProperty,
        pRpData->eObjectType);

    /* Converted value in BACNET_PROPERTY_VALUE structure
       Free memory used for Read property request parameter */
    OSAL_Free(pRpData,  __FILE__, __FUNCTION__, __LINE__);
	pRpData = NULL;

    if (i32Len > 0)
    {
		/* clear previous data from u8APDUResp */
		memset(u8APDUResp, 0, sizeof(u8APDUResp));

		/* convert the decoded value to string format */
		//Convert_AppTag_To_String((int8_t **)&pu8APDUResp, &appDataValue, eObjectType,
		//	ePropId, u32ArrayIndex, &i32StringLen, bArrayIndexPresent, false);

		/* Copy Property Value in String to Input Buffer */
		if(NULL != pu8APDUResp)
		{
			/* get the string length */
			u32DestSize = Strnlen(pu8APDUResp, u32DestSize);
			memcpy(pvReadData, pu8APDUResp, u32DestSize);
		}

		/* Free the pointer */
		OSAL_Free(pu8APDUResp, __FILE__, __FUNCTION__, __LINE__);
    }

    /* Clear decoded property value */
    Clear_Property_Value(appDataValue.pstNextPropVal);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Get_Object_Property_String: Exit \r\n");
	#endif
    return (MAX_BACNET_ERROR_CODE);
#endif
}


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
	BACNET_DATA_TYPE *peDataType)
{
	/* Local variables */
    virtualDevData_t *pVirtualDev = NULL;
	void *pvObjectPtr = NULL;
	void *pvPropVal = NULL;
	ulong32_t ul32ObjSize = 0;
	ulong32_t ul32BaseAddr = 0;
    ulong32_t ul32OffsetAddr = 0 ;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	BACNET_RETURN_TYPE eRetType = BACDEL_SUCCESS;
	Pr_BACnetUnsigned32_t stUnsignedVal = {0};

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Get_Object_Property: Entry \r\n");
	#endif

    /* validate input parameters */
	if(u32DevId >= BACNET_MAX_INSTANCE || u32ObjId >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Invalid object id \r\n");
		#endif
		return ERROR_CODE_PARAMETER_OUT_OF_RANGE;
	}

	/* get the object size */
    /* Note : used to check object support in stack */
    GET_OBJECT_SIZE(eObjType, ul32ObjSize);
    if(0 == ul32ObjSize)
    {
        /* object type not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Object type is invalid \r\n");
		#endif
		return ERROR_CODE_UNSUPPORTED_OBJECT_TYPE;
    }

	/* Check for null input pointers */
	if(NULL == pvReadPropVal || NULL == peDataType)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Null input pointer. \r\n");
		#endif
		return ERROR_CODE_INCONSISTENT_PARAMETERS;
	}

	/* set default values */
	*pvReadPropVal = NULL;
	*peDataType = BACNET_DT_EMPTY;

    /* Check if this is Host Device */
    pVirtualDev = Find_In_Host_Device_List(u32DevId);
	if(NULL == pVirtualDev)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Device not found. \r\n");
		#endif
		return ERROR_CODE_UNKNOWN_DEVICE;
	}

	/* Find Object in the device object list */
	pvObjectPtr = Find_Object(eObjType, u32ObjId, pVirtualDev);
	if(NULL == pvObjectPtr)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Object Not Found \r\n");
		#endif
		return ERROR_CODE_UNKNOWN_OBJECT;
    }

	/* Get the Structure Base Address */
    ul32BaseAddr = (ulong32_t )pvObjectPtr;

	/* check property support */
	if(NOT_SUPPORTED == GetDefndPropAccess(eObjType, eDevProp, BACNET_DEFAULT))
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Unknown property \r\n");
		#endif
		return ERROR_CODE_UNKNOWN_PROPERTY;
	}

	/* get property offset address */
	/* validate the array index & access type */
    eErrorCode = ValidateAccessAndIndex(eDevProp, eObjType, u32ArrayIndex,
        pvObjectPtr, &ul32OffsetAddr, bArrayIndexPresent);
	if(ERROR_CODE_OTHER != eErrorCode)
    {
        /* array index invalid or property not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: Access and index validation fails \r\n");
		#endif
        /* return error code */
        return eErrorCode;
    }

	/* get the property data type */
	*peDataType = GetPropertyDataType(eObjType, eDevProp, BACNET_ARRAY_ALL, FALSE);

	/* save property value address */
	pvPropVal = (void *)(ul32BaseAddr + ul32OffsetAddr);

	/* update offset address for address binding property */
	/* fix for bug id 5086 */
	if(OBJECT_DEVICE == eObjType && PROP_DEVICE_ADDRESS_BINDING == eDevProp)
	{
		/* get property offset address from global structure */
		pvPropVal = &g_stDevAddBinding;
	}

	/* for array index 0 return array size */
	if(0 == u32ArrayIndex && ARRAY_INDEX_PRESENT == bArrayIndexPresent)
	{
		/* get array size */
		eErrorCode = Get_Array_DataType_Count(peDataType, pvPropVal, &stUnsignedVal);
		if(MAX_BACNET_ERROR_CODE != eErrorCode)
		{
			/* array index invalid or property not supported */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			BACDEL_Get_Object_Property: property not an array type \r\n");
			#endif
			/* return error code */
			return eErrorCode;
		}

		/* update pointer */
		pvPropVal = &stUnsignedVal;
	}

	/* reset to default value */
	eErrorCode = MAX_BACNET_ERROR_CODE;

	/* copy value of property as per the data type */
	eRetType = Copy_PropVal_AsPer_DataType(pvPropVal, *peDataType,	pvReadPropVal);
	if(BACDEL_SUCCESS != eRetType)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Get_Object_Property: copy function fails \r\n");
		#endif
		/* return error code */
		return ERROR_CODE_OTHER;
	}

	/* reverse bit string value */
	Reverse_BitString_DataType_Value(*peDataType, *pvReadPropVal);

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Get_Object_Property: Exit \r\n");
	#endif
    return eErrorCode;
}


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
	void **pvData)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: "
	"BACDEL_Free_Object_Property: Entry \r\n");
	#endif

	/* free allocated memory */
	Clear_PropVal_AsPer_DataType(eDataType, pvData);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: "
	"BACDEL_Free_Object_Property: Exit \r\n");
	#endif
}

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
	BACnetAddress_t *pstDeviceAdd)
{
	/* local variables */
	IamRouterToNwBindData_t *pstIAMRouter = NULL;

//	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: "
	"BACDEL_Get_Router_Address_From_Network_No: Entry \r\n");
	#endif

//	/* check for null input pointer */
	if(NULL == pstDeviceAdd)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: "
		"BACDEL_Get_Router_Address_From_Network_No: Null input pointer \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

//	/* get the pointer of global i am router to network list */
	#ifdef NETWORK_LAYER_MESSAGE_A
	pstIAMRouter = g_pstIamRouterToNw;
	#else
	pstIAMRouter = NULL;
	#endif

    if(u16NetworkNo == 1)
	{
		// Set the router's MSTP MAC address on local network
		pstDeviceAdd->u16net = 0; // Local network
		pstDeviceAdd->u8mac_len = 1; // MSTP MAC length
		pstDeviceAdd->u8IpAddrs[0] = 0xFF; // Router's MAC address for broadcast
		return BACDEL_SUCCESS;
	}
	else if(u16NetworkNo == 2)
	{
		// Set the router's MSTP MAC address on local network for network 2
		pstDeviceAdd->u16net = 0; // Local network
		pstDeviceAdd->u8mac_len = 1; // MSTP MAC length
		pstDeviceAdd->u8IpAddrs[0] = 0x00; // Router's MAC address for broadcast
		return BACDEL_SUCCESS;
	}

//	/* traverse the list & find network no */
	while(NULL != pstIAMRouter)
	{
		if(u16NetworkNo == pstIAMRouter->m_u16NetworkNo)
		{
//			/* copy the ip address */
			memcpy(pstDeviceAdd, &pstIAMRouter->m_stAddress,
				sizeof(BACnetAddress_t));
//			/* network no found, return success */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: "
			"BACDEL_Get_Router_Address_From_Network_No: Ip address of router found. \r\n");
			#endif
			return BACDEL_SUCCESS;
		}
//		/* move to next node in list */
		pstIAMRouter = pstIAMRouter->pstNext;
	}

//	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: "
	"BACDEL_Get_Device_ID_Or_Address: Exit \r\n");
	#endif
	return BACDEL_ROUTER_IP_NOT_FOUND;
}


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
	BACDEL_Get_Dynamic_Device_Address_Binding(BACnetAddrBinding_t **pstAddrBindingList)
{
	/* local variables */
	BACnetAddrBinding_t *pstAddrBindData = NULL;
	BACnetAddrBinding_t *pstTemp = NULL;
	BACnetAddrBinding_t *pstBaseAddr = NULL;
	uint32_t u32Count = 0;

    /* fuction entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    BACDEL_Get_Dynamic_Device_Address_Binding: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstAddrBindingList)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
		BACDEL_Get_Dynamic_Device_Address_Binding: Null input pointer \r\n");
		#endif
		return;
	}

	/* acquire the mutex */
	Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);

	/* get pointer of 1st node */
    pstAddrBindData = &g_stDevAddBinding.m_stAddBinding;

	while(NULL != pstAddrBindData)
	{
		/* check if this is 1st call to allocate memory */
		if(BACNET_ZERO == u32Count)
		{
			/* allocate memory to 1st node */
			pstTemp = (BACnetAddrBinding_t *)OSAL_Malloc(sizeof(BACnetAddrBinding_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* save the base address */
			pstBaseAddr = pstTemp;
		}
		else
		{
			/* allocate memory to next node */
			pstTemp->pstNext = (BACnetAddrBinding_t *)OSAL_Malloc(sizeof(BACnetAddrBinding_t), 
				__FILE__, __FUNCTION__, __LINE__);
			/* move to allocated memory node */
			pstTemp = pstTemp->pstNext;
		}

		/* check the pointer */
		if(NULL == pstTemp)
		{
			/* memory allocation failed */
			Clear_AddressBinding_List(&pstBaseAddr, false);
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
			BACDEL_Get_Dynamic_Device_Address_Binding: Malloc failed \r\n");
			#endif
			break;
		}

		/* copy the data */
		memcpy(pstTemp, pstAddrBindData, sizeof(BACnetAddrBinding_t));
		pstTemp->pstNext = NULL;

		/* move to next node in list */
		pstAddrBindData = pstAddrBindData->pstNext;

		/* increment the count */
		u32Count++;
	}

	/* return the base address of list */
	*pstAddrBindingList = pstBaseAddr;

	/* release the mutex */
	Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);

	/* fuction exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    BACDEL_Get_Dynamic_Device_Address_Binding: Exit \r\n");
	#endif
}


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
	BACDEL_Free_Dynamic_Device_Address_Binding(BACnetAddrBinding_t **pstAddrBindingList)
{
	/* local variables */

    /* fuction entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    BACDEL_Free_Dynamic_Device_Address_Binding: Entry \r\n");
	#endif

	Clear_AddressBinding_List(pstAddrBindingList, false);

	/* fuction exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    BACDEL_Free_Dynamic_Device_Address_Binding: Exit \r\n");
	#endif
}

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
	BACnetAddress_t *pstDeviceAdd, bool bSearchIPAdd)
{
	/* local variables */
    //virtualDevData_t *pstVirtualDev = NULL;
	BACnetAddrBinding_t *pstAddBindData = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Get_Device_ID_Or_Address: Entry \r\n");
	#endif

    /* Verify search criteria. If we have to search IP Address or Device ID */
    if(bSearchIPAdd)
    {
		/* acquire the mutex */


		/* Get base address of device address binding data */
		pstAddBindData = &g_stDevAddBinding.m_stAddBinding;

        /* Check the Device Address Binding for same DADR.
         * If found Copy IP Address otherwise return BACDEL_OTHER */
		while(NULL != pstAddBindData)
		{
			if(pstAddBindData->m_u32ObjId == *pu32DeviceID)
			{
				/* copy address */
                memcpy(pstDeviceAdd,&pstAddBindData->m_stAddress, 
					sizeof(BACnetAddress_t));

				/* release the mutex */
				//Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);<@>

				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAPI: BACDEL_Get_Device_ID_Or_Address: "
					"Entry in Device address Binding found \r\n");
				#endif
                return BACDEL_SUCCESS;
            }
			/* move to next node */
			pstAddBindData = pstAddBindData->pstNext;
		}
    }
    else
    {
		/* acquire the mutex */
		//Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);<@>

		/* Get base address of device address binding data */
		pstAddBindData = &g_stDevAddBinding.m_stAddBinding;

        /* Check the Device Address Binding for same IP Address.
         * If found Copy DADR otherwise return BACDEL_OTHER */
		while(NULL != pstAddBindData)
		{
            if(!memcmp(&pstAddBindData->m_stAddress, pstDeviceAdd, 
				sizeof(BACnetAddress_t)))
            {
				/* copy id */
				*pu32DeviceID = pstAddBindData->m_u32ObjId;

				/* release the mutex */
				//Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);<@>

				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAPI: BACDEL_Get_Device_ID_Or_Address: "
					"Entry in Device address Binding found \r\n");
				#endif
                return BACDEL_SUCCESS;
            }
			/* move to next node */
			pstAddBindData = pstAddBindData->pstNext;
        }
    }

	/* release the mutex */
	//Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);<@>

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Get_Device_ID_Or_Address: Exit \r\n");
	#endif
    return BACDEL_DEVICE_NOT_PRESENT;
}

#ifdef BACDEL_SER_DM_RD_B  //KV

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
 void BACDEL_ReInitDevice(uint32_t u32DeviceId, BACNET_RESTART_REASON eRestartReason)
{
    /* virtual device */
    virtualDevData_t *pVirtualDev = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_ReInitDevice: Entry \r\n");
	#endif

	if(u32DeviceId >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_ReInitDevice: "
		"invalid device id \r\n");
		#endif
		return ;
	}

    /* Traverse the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(u32DeviceId);

    /* Reinitialize the device. */
    ReInitializeDevice(pVirtualDev, eRestartReason);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_ReInitDevice: Exit \r\n");
	#endif
    return;
}

#endif /* BACDEL_SER_DM_RD_B */

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
	bool bFlag)
{
	virtualDevData_t *pVirtualDev = NULL;

	/*BACnetTime_t stLocalTime = {0};
	BACnetDate_t stLocalDate = {0};*/

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Get_Device_Local_DateTime: Entry \r\n");
	#endif

	if(u32DevId >= BACNET_MAX_INSTANCE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: BACDEL_Set_Get_Device_Local_DateTime: \
		invalid device id \r\n");
		#endif
		return BACDEL_MAX_INSTANCE_ERROR;
	}

	if(NULL == pstDate || NULL == pstTime)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
		BACDEL_Set_Get_Device_Local_DateTime: Null Input Data Pointers. \r\n");
		#endif
		return BACDEL_INVALID_INPUT_PARAMETER;
	}

	/* Traverse the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(u32DevId);

    /* No Host device found, then check Remote Device Link List */
    if(NULL == pVirtualDev)
    {   
        pVirtualDev = Find_In_Remote_Device_List(u32DevId);
		if(NULL == pVirtualDev)
		{
			/* Reached End of Remote Device Link List */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			BACDEL_Set_Get_Device_Local_DateTime: No Device \r\n");
			#endif
			return BACDEL_DEVICE_NOT_PRESENT;     
		}
    }// end of remote device linklist.

	if(TRUE == bFlag && Validate_DateTime(pstDate, pstTime))
	{

		/* get date_time differences & save in virtual device */
		TimeSync_Get_DateTime_Diff(pstDate, pstTime,
			&pVirtualDev->m_i32Sec_Diff, &pVirtualDev->m_i32Days_Diff);

		/* update device local date time properties */		
		Update_Dv_Local_DateTime_Properties (pVirtualDev, pstTime, pstDate, false);
	}
	else if(FALSE == bFlag)
	{
		GetDevice_DateTime(pstDate, pstTime, pVirtualDev);
	}
	else; // do nothing

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Get_Device_Local_DateTime: Exit \r\n");
	#endif
	return BACDEL_SUCCESS;
}

#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */


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
 int32_t BACDEL_Get_Sizeof_Object(BACNET_OBJECT_TYPE eObjectType)
{
    /* local variables */
	int32_t i32Size = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Get_Sizeof_Object : Entry \r\n");
	#endif

	/* Get the object tpye size */
	GET_OBJECT_SIZE(eObjectType, i32Size);
        
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_Get_Sizeof_Object : Exit \r\n");
	#endif

	/* Return the Object size */
	return i32Size;
}

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
    PROP_ACCESS_TYPE eAccessType, bool bReadOrWrite)
{
    /* Virtual device structure pointer */
    virtualDevData_t *pVirtualDev = NULL;
	/* void pointer */
	void *pvObject = NULL;
    /* To save property access type */
    PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Get_Property_AccessType : entry \r\n");
	#endif

    /* Traverse the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(u32DevID);

    /* No Host device found, then check Remote Device Link List */
    if(NULL == pVirtualDev)
    {   
        pVirtualDev = Find_In_Remote_Device_List(u32DevID);
		if(NULL == pVirtualDev)
		{
			/* Reached End of Remote Device Link List */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			BACDEL_Set_Get_Property_AccessType: device with given id is not present. \r\n");
			#endif
			return NOT_SUPPORTED;
		}
    }// end of remote device linklist.

    pvObject = Find_Object(eObjectType, u32ObjId, pVirtualDev);
    if(NULL == pvObject)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Set_Get_Property_AccessType: object with given id & type is not present. \r\n");
		#endif
        return NOT_SUPPORTED;
    }

    /* Check if property is supported for given object type */
    ePermission = GetDefndPropAccess(eObjectType, ePropId, ePermission);
    ePermission = Validate_PropertySupport(eObjectType, ePropId, ePermission);
    if(NOT_SUPPORTED == ePermission)
        return ePermission;

    /* Write the property value */
    ePermission = GetAsignPropAccess(eObjectType, ePropId, pvObject, 
        eAccessType, bReadOrWrite, NULL);

    /* Check if property can be written even if it is read only */
    /* NOTE : this check is applicable only while reading access type */
    if((READ_ONLY == ePermission || COMMANDABLE_PROP == ePermission) && 
        false == bReadOrWrite)
    {
        if(Is_Property_Writable(ePropId, eObjectType, pvObject, pVirtualDev))
            ePermission = READ_WRITE;
        else;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Get_Property_AccessType : exit \r\n");
	#endif
    return ePermission;
}

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
 void BACDEL_Set_Device_Password(int32_t i32DevID, uint8_t *pu8Password)
{
    /* Virtual device structure pointer */
    virtualDevData_t *pVirtualDev = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Device_Password : entry \r\n");
	#endif

    /* check input values */
    if(NULL == pu8Password)
    {
        /* Invalid character encoding value */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
		BACDEL_Set_Device_Password: null input pointer. \r\n");
		#endif
		return;
    }

    /* Check if encoding is to be set @ device level */
    if(i32DevID >= 0)
    {
        /* Traverse the Host Device Link List to find the device. */
        pVirtualDev = Find_In_Host_Device_List(i32DevID);
        if(NULL == pVirtualDev)
        {   
		    /* Reached End of Host Device Link List */
			#ifdef DEBUG_PRINTF
		    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		    BACDEL_Set_Device_Password: device with given id is not present. \r\n");
			#endif
		    return;
        }

        /* clear previous password */
	    memset(&pVirtualDev->m_stDvPassword, 0, sizeof(BACnetCharStr_t));
	    /* password should be max 20 char */
		pVirtualDev->m_stDvPassword.m_u32StrLen = Strnlen(pu8Password, MAX_PASSWORD_CHAR_BYTES);
        /* copy password */
		Strcopy(pVirtualDev->m_stDvPassword.m_pu8CharStr, pu8Password, pVirtualDev->m_stDvPassword.m_u32StrLen);
    }
    else
    {
        /* clear previous password */
	    memset(&g_stDevicePassword, 0, sizeof(BACnetCharStr_t));
	    /* password should be max 20 char */
		g_stDevicePassword.m_u32StrLen = Strnlen(pu8Password, MAX_PASSWORD_CHAR_BYTES);
        /* copy password */
		Strcopy(g_stDevicePassword.m_pu8CharStr, pu8Password, g_stDevicePassword.m_u32StrLen);
    }

    /* Store in Flash*/
    BACnetConfigData.m_stPassword.m_u32StrLen = Strnlen(pu8Password, MAX_PASSWORD_CHAR_BYTES);
    Strcopy(BACnetConfigData.m_stPassword.m_pu8CharStr, pu8Password,
    		BACnetConfigData.m_stPassword.m_u32StrLen);

    SaveBACnetDatatoNV();
    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Device_Password : exit \r\n");
	#endif
    return;
}



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
    BACNET_OBJECT_TYPE eObjectType)
{
	/* local variables */
  virtualDevData_t *pVirtualDev = NULL; 
	bool bRetval = false;
	uint8_t u8Byte = 0;
	uint8_t u8Bit_no = 0;
	uint8_t u8ByteValue = 0;
	/* offset address for property values */
	ulong32_t ul32OffsetAddr = 0;
	
	/* function entry message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    BACDEL_Check_Object_Support: Invalid Object type \r\n");
	#endif

    /* validate inputs */
	if(eObjectType >= MAX_ASHRAE_OBJECT_TYPE)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
	    BACDEL_Check_Object_Support: Invalid Object  \r\n");
		#endif
		return bRetval;
	}

	if(i32DevId < 0)
	{
		/* Check object support in stack */
		/* get the object size */
		/* Note : used to check object support in stack */
		GET_OBJECT_SIZE(eObjectType, ul32OffsetAddr);
		if(0 == ul32OffsetAddr)
		{
			/* object type not supported */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
			BACDEL_Check_Object_Support: object type is invalid \r\n");
			#endif
			bRetval = false;
		}
		else 
			bRetval = true;
	}
	else
	{
		/* Check if this is Host Device */
		pVirtualDev = Find_In_Host_Device_List(i32DevId); 
		if(NULL == pVirtualDev)
    {
            /* device not found */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
			BACDEL_Check_Object_Support: unknown device \r\n");
			#endif
            bRetval = false;
    }
		else
		{
			u8Byte = (uint8_t)(eObjectType / 8);
			u8ByteValue = pVirtualDev->m_stDevObject.m_stObjectTypeSupported.
                m_stObjectSupport.Byte[u8Byte];
			u8Bit_no = eObjectType % 8;
			bRetval = BIT_VALUE_CHECK(u8ByteValue, u8Bit_no);
		}
	}

	/* function exit message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    BACDEL_Check_Object_Support: exit \r\n");
	#endif
	return bRetval;
}

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
	uint8_t u8PropsdWindowSize)
{
    /* local variables */
    BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Segmentation_WindowSize : entry \r\n");
	#endif

	/* check if stack is already running */
	if(TRUE == g_bBACnetStackInitFlag)
	{		
		/* stack already initialized */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAPI: \
		BACDEL_Set_Segmentation_WindowSize: Stack is already initialized. \r\n");
		#endif
		return BACDEL_STACK_ALREADY_INITIALIZED;
	}

	#if(!STACK_CUSTOMIZE_SET_SEG_WINDOW_SIZE_API)
	/* check min, max and max-rx-seg limits */ 
	if((u8ActWindowSize == 0 || u8ActWindowSize > (MAX_SEGMENTS_RX - 1)) ||
		u8ActWindowSize > MAX_SEGMENT_WINDOW_SIZE || u8PropsdWindowSize > MAX_SEGMENT_WINDOW_SIZE ||
		(u8PropsdWindowSize == 0 || u8PropsdWindowSize > (MAX_SEGMENTS_RX - 1)))
	#else
	/* check min and max limits only */
	if((u8ActWindowSize == 0 || u8ActWindowSize > MAX_SEGMENT_WINDOW_SIZE) || 
		(u8PropsdWindowSize > MAX_SEGMENT_WINDOW_SIZE || u8PropsdWindowSize == 0 ))
	#endif
	{
		/* Incorrect Value  - Value out of range */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAPI: \
		BACDEL_Set_Segmentation_WindowSize: Value out of range  \r\n");
		#endif
		return BACDEL_OUT_OF_RANGE_ERROR;
	}

	/* update the values */
	g_u8ActualWindowSize = u8ActWindowSize;
	g_u8ProposedWindowSize = u8PropsdWindowSize;
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
	BACDEL_Set_Segmentation_WindowSize : exit \r\n");
	#endif
    return eRetVal;
}

#endif /* SEGMENTATION_SUPPORTED */


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
    App_RTC_Interface_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_App_CallBack_Register: Entry \r\n");
	#endif
	
	if(NULL != fpFunction)
	{
		if(eAppChoice < APP_CB_RTC_MAX)
		{
			afpApplRTCCallback[eAppChoice] = fpFunction;
		}
	}
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: BACDEL_App_CallBack_Register: Exit \r\n");
	#endif
}
 /**
 *
 * DESCRIPTION
 * Select DataLink Layer
 *
 * @param eDataLinkChoice [IN]  IP or MSTP.
 * @return BACNET_RETURN_TYPE [out] BACDEL_SUCCESS/BACDEL_ERROR.
 *
 */
 BACNET_RETURN_TYPE BACDEL_Select_DataLink_Layer(BACNET_STACK_SELECTION eDataLinkChoice)
{
//	BACNET_RETURN_TYPE eRetval = BACDEL_SUCCESS;
////	uint8_t u8Restart_Flag = false;
//	if(eDataLinkChoice == BACNET_STACK_IP)
//	{
//		//if(SMCfg.g_ui8ComType != ect_BACnet_Phy/*BACnetConfigData.m_eBACStack != BACNET_STACK_IP*/)
//		{
////			u8Restart_Flag = true;
//		}
//		/* Application API */
//		SetComType((_Uui8) ect_BACnet_Phy );
//
//		SaveFactoryDatatoNV();
//	    //BACnetConfigData.m_eBACStack = BACNET_STACK_IP;
//		eRetval = BACDEL_SUCCESS;
//	}
//	else if(eDataLinkChoice == BACNET_STACK_MSTP)
//	{
//		if(SMCfg.g_ui8ComType != ect_BACnet_TP/*BACnetConfigData.m_eBACStack != BACNET_STACK_MSTP*/)
//		{
////			u8Restart_Flag = true;
//		}
//		/* Application API */
//		SetComType((_Uui8) ect_BACnet_TP );
//
//		SaveFactoryDatatoNV();
//		//BACnetConfigData.m_eBACStack = BACNET_STACK_IP;
//
//		eRetval = BACDEL_SUCCESS;
//	}
//	else
//	{
//		eRetval = BACDEL_ERROR;
//	}
//
//#if 0
//	//Restart
//	if(u8Restart_Flag == True)
//	{
//		HAL_NVIC_SystemReset();
//	}
//#endif
//	return eRetval;
}
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
	  uint32_t ui32StartAddress,uint16_t ui16TotalLength)
{
	  /**Check Buffer pointer & ui16TotalLength*/
	 if(pui8buffer == NULL || ui16TotalLength <= 0 )
	 {
		 return;
	 }
	 /**Write data in Flash memory*/
	 IS25xp_bytewrite( pui8buffer,  ui32StartAddress,  ui16TotalLength);
}
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
uint16_t BACDEL_Read_Flash_Config_Data(uint32_t offset,uint16_t nbytes,uint8_t *buffer)
{
	uint16_t ui16TotalLength=0;
	  /**Check Buffer pointer & ui16TotalLength*/
	 if(buffer == NULL || nbytes <= 0 )
	 {
		 return 0;
	 }
	 /**Write data in Flash memory*/
	 ui16TotalLength = IS25xp_read( offset,  nbytes, buffer);

	 return ui16TotalLength;
}

#ifndef BACDEL_PR18
/**
*
* DESCRIPTION
* API to set the BDT list, i.e list of BBMD's.
*
* @param pstBdtDataList [in/out] pointer to BDT list base node.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE BACDEL_Set_BDT_List(writeBdt_request_t *pstBdtDataList)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* fuction entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"API: BACDEL_Get_BDT_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstBdtDataList)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"API: BACDEL_Set_BDT_List: Null input pointer \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/* set bdt list */
	eRetVal = Set_BDT_List(pstBdtDataList);

	/* fuction exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"API: BACDEL_Set_BDT_List: Exit \r\n");
	#endif
	return eRetVal;
}


/**
*
* DESCRIPTION
* API to read the BDT list, i.e list of BBMD's.
*
* @param pstBdtDataList [in/out] pointer to save BDT list base node.
* @return [out] void/nothing.
*
* Note:
* Call clear BDT list api to free the list memory allocated in this api.
*
*/
void BACDEL_Get_BDT_List(readBdt_response_t *pstBdtDataList)
{
	/* local variables */

	/* fuction entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"API: BACDEL_Get_BDT_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstBdtDataList)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"API: BACDEL_Get_BDT_List: Null input pointer \r\n");
		#endif
		return;
	}

	/* get bdt list */
	Get_BDT_List(pstBdtDataList);

	/* fuction exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"API: BACDEL_Get_BDT_List: Exit \r\n");
	#endif
}


/**
*
* DESCRIPTION
* API to clear the BDT list initailly returned in the call to get
* BDT list api.
*
* @param pstBdtDataList [in] pointer containing BDT list base node.
* @return [out] void/nothing.
*
* Note:
* Pass the pointer that was returned in call to get BDT list api.
*
*/
void BACDEL_Free_BDT_List(readBdt_response_t *pstBdtDataList)
{
	/* local variables */
	readBdt_response_t *pstBdtData = NULL;
	void *pvTemp = NULL;

	/* fuction entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"API: BACDEL_Free_BDT_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL != pstBdtDataList)
	{
		/* get the base pointer of the list */
		pstBdtData = pstBdtDataList;
		pstBdtDataList = NULL;

		/* free the list nodes 1 by 1 */
		while(NULL != pstBdtData->m_pstBIpAddress)
		{
			pvTemp = pstBdtData->m_pstBIpAddress;
			pstBdtData->m_pstBIpAddress = pstBdtData->m_pstBIpAddress->pstNext;
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
	}

	/* fuction exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"API: BACDEL_Free_BDT_List: Exit \r\n");
	#endif
}


/**
*
* DESCRIPTION
* API to read the FDT list, i.e list of FD that have registered to BBMD .
*
* @param pstRegBdtData [in/out] pointer to save FDT list base node.
* @return [out] void/nothing.
*
* Note:
* Call clear BDT list api to free the list memory allocated in this api.
*
*/
void BACDEL_Get_FDT_List(readFdt_response_t *pstFdtDataList)
{
	/* local variables */

	/* fuction entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"API: BACDEL_Get_FDT_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstFdtDataList)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"API: BACDEL_Get_FDT_List: Null input pointer \r\n");
		#endif
		return;
	}

	/* get the fdt list */
	Get_FDT_List(pstFdtDataList);

	/* fuction exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"API: BACDEL_Get_Register_FD_List: Exit \r\n");
	#endif
}


/**
*
* DESCRIPTION
* API to clear the FDT list initailly returned in the call to get
* FDT list api.
*
* @param pstFdtDataList [in] pointer containing FDT list base node.
* @return [out] void/nothing.
*
* Note:
* Pass the pointer that was returned in call to get BDT list api.
*
*/
void BACDEL_Free_FDT_List(readFdt_response_t *pstFdtDataList)
{
	/* local variables */
	readFdt_response_t *pstFdtData = NULL;
	void *pvTemp = NULL;

	/* fuction entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"API: BACDEL_Free_FDT_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL != pstFdtDataList)
	{
		/* get the base pointer of the list */
		pstFdtData = pstFdtDataList;
		pstFdtDataList = NULL;

		/* free the list nodes 1 by 1 */
		while(NULL != pstFdtData->m_pstFdtData)
		{
			pvTemp = pstFdtData->m_pstFdtData;
			pstFdtData->m_pstFdtData = pstFdtData->m_pstFdtData->pstNext;
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
	}

	/* fuction exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"API: BACDEL_Free_FDT_List: Exit \r\n");
	#endif
}
#endif



/********************************** end of apiExportedApi.c file *************************/
