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
*   3rd Floor, Pentagon P4,                              http://www.softdel.com
*    Magarpatta City, Hadapsar
*    Pune - 411 028       
*
*    File Name - device.c
*
*   RELEASE HISTORY                                                             
*        DATE                  NAME                    DESCRIPTION
*   25/05/2011      Harshal Mangale         File Created
*   26/05/2011      Harshal Mangale         Implemented Add device, Device
*                                           process thread.
*   26/05/2011      Harshal Mangale         Implemented function prototype 
*                                           for object read/write property value
*   26/05/2011      Prashant Badguajar      Added Add/Del Object APIs
*   14/06/2011      Prashant Badgujar       Coding Standard Changes
*   27/06/2011      Prashant Badgujar       Implemented Dv_Read_Prop & 
*                                           Dv_Write_Prop functions.
*   05-07-2011      M.Venu                  Added Add_Bi_Object,Add_Bo_Object
*                                           and Add_Bv_Object with their
*                                           Function Calls.
*   06-07-2011      M.Venu                  1. Added following functions
*                                           and their reference calls 
*                                           Add_MultistateInput_Object
*                                           Add_MultistateOutput_Object
*                                           Add_MultistateValue_Object
*                                           2. Read/Write Function calls
*                                           for Multistate Objects
*   06-07-2011      Harshal Mangale         Implemented device object property 
*                                           list
*                                           Implemented Device_Property_Lists
*                                           Implemented Find_Objects_PropertyList
*                                           Implemented property_list_count
*                                           Implemented 
*                                           Device_Objects_Property_List
*                                           
*   07/07/2011      Ashish Verma            Added Who_Is_Request_Parser
*                                           function call under
*                                           SERVICE_UNCONFIRMED_WHO_IS 
*                                           in DvProcessThread function
*                                           Added Fill_I_AM_Structure &
*                                           Check_Device_Address_Range 
*                                           functions.
*   08-07-11        Harshal Mangale         1.Changes in DvProcessThread,
*                                           function calling and event to Tx 
*                                           thread
*                                           2.Changes in Read_Object_Property,
*   14-07-11        M.Venu                  Added BACNET_STACK_EXPORT 
*   14/07/2011      Ashish Verma            Modifications based on review
*                                           comments.
*   15/07/2011      Ashish Verma            Removed functions 
*                                           Who_Is_Request_Parser,
*                                           Check_Device_Address_Range & 
*                                           Fill_I_AM_Structure.
*   15/07/2011      M.Venu                  Removed Device object common  
*                                           properties
*   18/07/2011      Prashant Badgujar       Chaged Return type for 
*                                           Dv_Read_Prop API
*   19/07/2011      Ashish Verma            Added Who_Has_Request_Parser()
*   19/07/2011      Harshal Mangale         Added Function call for Required & Optional
*                                           Property List for Binary Object
*   20/07/2011      Prashant Badgujar       Code Review Changes Implementation.
*   22/07/2011      Ashish Verma            Added fucntion
*                                           Exit_Routine_To_Error_Of_Unconfirmed_Request()
*   25/07/2011      Ashish Verma            Added DOB_A service in
*                                           DvProcessThread()
*   27/07/2011      M.Venu                  Commented Char String data types of 
*                                           device objec after changing its data type
*   26/07/2011      Ashish Verma            Modified BACDEL_Add_Device() to add Device
*                                           address binding functionalities.
*                                           Modified m_stObjectName to
*                                           m_stObjName of devObject_t
*   28/07/2011      Ashish Verma            Added Init_Device_Binding_Structure
*   01/08/2011      Harshal Mangale         Added Function call for Required & Optional
*                                           Property List for Analog and Multistate Obj
*	01/08/0211		Prashant Badgujar		Adding Support for Common Create Object API
*	03/08/2011		Prashant Badgujar		Adding changes in Dv_Read_Prop API
*											as per new Read functionality.
*   03/08/2011      M.Venu                  Replaced ValidateAccessAndIndex 
*                                           function call with ValidateNonArrayTypeProperty
*                                           in Dv_Read_Prop function
*	08/08/2011		Prashant Badgujar		Chaging BACDEL_Add_Object Prototype. Adding
*											support for object id & object name.
*   09/08/2011      Ashish Verma            Added the support of COV_B service.
*	09/08/2011		Prashant Badgujar		Adding SNET as function parameter to BACDEL_Add_Device API.
*	17/08/2011		Prashant Badgujar		Modified Dv_Write_Prop reflect changes inline with
*											common write API.
*   24/08/2011      Prashant Badgujar       Added all device supported properties in     
*                                           Dv_Write_Prop API
*	29/08/2011		Prashant Badgujar    	Updating BACDEL_Delete_Object API.
*   06/09/2011      Prashant Badgujar       Initializing Active COV subsciption 
*                                           pointer at device init time.
*	12/09/2011		Prashant Badgujar		Modifying Delete Device API to get freed device data.
*	13/09/2011		Prashant Badgujar		Fixing defect-1 of Release-2 & Config file property change.
*	13/09/2011		Prashant Badgujar		Making changes for object list data type.
*   15/09/2011      Prashant Badgujar       Modified Dv_Read_Property to support Slave 
*                                           Proxy Enable & Auto Slave Discovery 
*   19/09/2011      Prashant Badgujar       Adding Database Revision Property Support.
*   19/09/2011      Ashish Verma            Added the initialization of Device 
*                                           Object type in
*                                           Init_Device_Properties().
*   20/09/2011      Prashant Badgujar       Initializing the required device 
*                                           proeperties in Init_Device_Properties API
*   21/09/2011      Ashish Verma            Added functionality of Device
*                                           Communication Control
*   22/09/2011      Ashish Verma            Created Timer in the BACDEL_Add_Device().
*                                           Added fucntions TimerCallBackFunc,
*                                           Initiate_Timer_Param. Modified
*                                           DvprocesThread fucntion.
*   23/09/2011      Ashish Verma            Added functions TimerCallBackFun &
*                                           Initiate_Timer_Param. Modified the 
*                                           Design for COV Cancellation & DCC
*                                           service, instead of using seperate
*                                           timers we use seperate threads for
*                                           COV Cancellation & DCC. The threads 
*                                           are created in BACDEL_Add_Device() and are 
*                                           Suspended/Resumed by the
*                                           TimerCallBackFun fucntion.
*   28/09/2011      Ashish Verma            1) Modified the fucntionality of 
*                                              intializing the Device Address
*                                              Binding structure in
*                                              Init_Device_Properties().
*                                           2) Removed funtion
*                                              Init_Device_Binding_Structure 
*                                              form BACDEL_Add_Device().
*                                           3) Added fucntion Initiate_Services 
*                                              BACDEL_Get_Device_ID_Or_Address,
*                                              Update_Device_Address_Binding,
*                                              Send_I_AM & Send_WHO_IS.  
*	30/09/2011		Prashant Badgujar		Adding support of Remote Device.
*   05/10/2011      Ashish Verma            Replaced Argument_List_t with 
*                                           bacnetip_arguments_t.
******************************************************************************/
#include "bacDELDef.h"
#include "bacDELApi.h"
#include "bacDELDeviceConfig.h"
#include "objDevice.h"   
#include "propertyValueRead.h"
#include "propertyValueWrite.h"
#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"
#include "propertyClearValues.h"
#include "propertyRpmFallbackArrays.h"
#include "pduDateTime.h"
#include "bacnetAPDUHandler.h"

#include "../Inc/datalinkMSTP.h"
#include "DataB.h"
/* To inclide A-side stack support */
#ifdef INITIATE_SERVICE_ENABLED
#include "bacnetInitiateServiceMgmt.h"
#endif

/* To include RP-B Service */
#ifdef BACDEL_SER_DS_RP_B
#include "serviceReadProperty_B.h"
#endif

/* To include RPM-B Service */
#ifdef BACDEL_SER_DS_RPM_B
#include "serviceReadPropertyMultiple_B.h"
#endif

/* To include WP-B Service */
#ifdef BACDEL_SER_DS_WP_B
#include "serviceWriteProperty_B.h"
#endif

/* To include WPM-B Service */
#ifdef BACDEL_SER_DS_WPM_B
#include "serviceWritePropertyMultiple_B.h"
#endif

/* To include DDB-B Service */
#ifdef BACDEL_SER_DM_DDB_B
#include "serviceDynamicDeviceBinding_B.h"
#endif /* BACDEL_SER_DM_DDB_B */

/* To include DDB-A Service */
#ifdef BACDEL_SER_DM_DDB_A
#include "serviceDynamicDeviceBinding_A.h"
#endif /* BACDEL_SER_DM_DDB_A */

/* To include DOB-B Service */
#ifdef BACDEL_SER_DM_DOB_B
#include "serviceDynamicObjectBinding_B.h"
#endif /* BACDEL_SER_DM_DOB_B */

/* To include DCC service */
#ifdef BACDEL_SER_DM_DCC_B
#include "serviceDeviceCommControl.h"
#endif /* BACDEL_SER_DM_DCC_B */

/* To include COV service */
#ifdef BACDEL_SER_DS_COV_B
#include "serviceChangeOfValue_B.h"
#endif /* BACDEL_SER_DS_COV_B */


#ifdef BACDEL_SER_AE_GEI_B
#include "serviceGetEventInformation_B.h"
#endif /*BACDEL_SER_AE_GEI_B*/

/* To include EN-B service */
#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)
#include "serviceEventNotification_B.h"
#endif /* BACDEL_SER_AE_EN_B */


/* To include AA-B service */
#ifdef BACDEL_SER_AE_AA_B
#include "serviceAcknowledgeAlarm_B.h"
#endif /* BACDEL_SER_AE_AA_B */

/* To include TS-B service */
#ifdef BACDEL_SER_DM_TS_B
#include "serviceTimeSynchronization_B.h"
#endif /* BACDEL_SER_DM_TS_B */

/* To include UTC-TS-B service */
#ifdef BACDEL_SER_DM_UTC_B
#include "serviceUtcTimeSynchronization_B.h"
#endif /* BACDEL_SER_DM_UTC_B */

/* To include RD-B service */
#ifdef BACDEL_SER_DM_RD_B
#include "serviceReinitializeDevice_B.h"
#endif /* BACDEL_SER_DM_RD_B */


/* BACnet Host Device. First Host entry is static as there should be atleast 
one host device in BACnet Stack. Other host devices are getting added at the 
Next pointer to first static host device */
BACnet_Device_Struct_t gstHostDevice;

/* BACnet Remote Device. Remote Devices are getting added as link list.
 There is no static entry for host device as they are not mandatory for 
 BACnet stack */
BACnet_Device_Struct_t *gpstRemoteDevice;

/* Global variable for device Address Binding */
Pr_ListOfBACnetAddrBinding_t   g_stDevAddBinding = {0};
extern QueueHandle_t Cov_EventQueue;

/* global structure for device callback notification queue */
CallbackNotifyQueue_t   g_stDvCallbackNotifyQ = {0};
/* mutex handle for callback notification queue */
Mutex_H  g_hCallbackMtxLock = NULL;
extern DB_t SMCfg;


extern osSemaphoreId_t m_hDeviceSemaphoreHandle;
extern osSemaphoreId_t m_hDvMgmtSemaphoreHandle;
extern osSemaphoreId_t m_hCOVCancelSemaphoreHandle;

#ifdef SUPPORT_MULTIPLE_DEVICE
/* Global variable for Device Local Network No. */
extern uint16_t g_u16VirtualNWNo;
#endif

/** Global variable to save vendor specific data */
extern uint8_t g_au8VendorName[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8ModelName[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8FirmwareRev[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8ApplSoftwareVer[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8Location[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8DevDescription[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8ObjDescription[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8ProfileName[MAX_SUPPORTED_CHRSTR_LEN];
extern uint8_t g_au8DeviceName[MAX_SUPPORTED_CHRSTR_LEN];
extern uint16_t g_u16VendorID;
#ifdef BACDEL_PR14
extern uint8_t g_au8SerialNumber[MAX_SUPPORTED_CHRSTR_LEN];
#endif

/** This array used by the ReadPropertyMultiple handler, which should contain all *  required properties for device.
*/
const int32_t Device_Properties_Required[] =    
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_SYSTEM_STATUS,
    PROP_VENDOR_NAME,
    PROP_VENDOR_IDENTIFIER,
    PROP_MODEL_NAME,
    PROP_FIRMWARE_REVISION,
    PROP_APPLICATION_SOFTWARE_VERSION,
    PROP_PROTOCOL_VERSION,
    PROP_PROTOCOL_REVISION,
    PROP_PROTOCOL_SERVICES_SUPPORTED,
    PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED,
    PROP_OBJECT_LIST,
    PROP_MAX_APDU_LENGTH_ACCEPTED,
    PROP_SEGMENTATION_SUPPORTED,
    PROP_APDU_TIMEOUT,
    PROP_NUMBER_OF_APDU_RETRIES,
    PROP_DEVICE_ADDRESS_BINDING,
    PROP_DATABASE_REVISION,
    ENDOFPROPLIST    
};

/** This array used by the ReadPropertyMultiple handler, which should contain all *  Optional properties for device.
*/
const int32_t Device_Properties_Optional[] = 
{
	PROP_LOCATION,
	PROP_DESCRIPTION,
	PROP_MAX_SEGMENTS_ACCEPTED,
	PROP_LOCAL_DATE,
    PROP_LOCAL_TIME,
    PROP_UTC_OFFSET,
    PROP_DAYLIGHT_SAVINGS_STATUS,
	PROP_APDU_SEGMENT_TIMEOUT,
    PROP_ACTIVE_COV_SUBSCRIPTIONS,
    PROP_SERIAL_NUMBER,
	PROP_MAX_INFO_FRAMES,
	PROP_MAX_MASTER,
    ENDOFPROPLIST
};

/** This array used by the ReadPropertyMultiple handler, which should contain all *  Proprietary properties for device.
*/
const int32_t Device_Properties_Proprietary[] = {
    ENDOFPROPLIST
};


/**
*DISCREPTION
*   This function is used to assign Property list for device object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
static void Device_Property_Lists(int32_t **pi32Required,
                                  int32_t **pi32Optional,
                                  int32_t **pi32Proprietary)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Device_Property_Lists : Entry \r\n");
	#endif

    if (pi32Required)
    {
        /* Get the required Properties List */
        *pi32Required = (int32_t *)Device_Properties_Required;
    }
    if (pi32Optional)
    {
        /* Get the optional Properties List */
        *pi32Optional = (int32_t *)Device_Properties_Optional;
    }
    if (pi32Proprietary)
    {
        /* Get the proprietary Properties List */
        *pi32Proprietary = (int32_t *)Device_Properties_Proprietary;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Device_Property_Lists : Exit \r\n");
	#endif

    return;
}

/**
*DISCREPTION
*   This function is used to assign Property list for All objects 
*   supported in Device. It makes call to respective object property 
*   list handler function
*
*@param eObjectType [in] Object Type of which property list to retrieve
*@param pstPropertyList [Out] Pointer to property list of given object
*
*/
static void Find_Objects_PropertyList(BACNET_OBJECT_TYPE eObjectType, 
                    special_property_list_t *pstPropertyList)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Find_Objects_PropertyList : Entry \r\n");
	#endif

    /* Check Object type and get the Property list */
    switch(eObjectType)
    {
        case OBJECT_DEVICE:
        {
            /* Get Device all types of Properties */
            Device_Property_Lists(&pstPropertyList->Required.pList,
                                  &pstPropertyList->Optional.pList, 
                                  &pstPropertyList->Proprietary.pList);
        }
        break;

#ifdef BACDEL_OBJ_AI
        case OBJECT_ANALOG_INPUT:
        {
            /* Get Device all types of Properties */
            Analog_Input_Property_Lists(&pstPropertyList->Required.pList,
                                  &pstPropertyList->Optional.pList, 
                                  &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_AO
        case OBJECT_ANALOG_OUTPUT:
        {
            /* Get Device all types of Properties */
            Analog_Output_Property_Lists(&pstPropertyList->Required.pList,
                                  &pstPropertyList->Optional.pList, 
                                  &pstPropertyList->Proprietary.pList);
        }
        break;
#endif
#ifdef BACDEL_OBJ_AV
        case OBJECT_ANALOG_VALUE:
        {
            /* Get Device all types of Properties */
            Analog_Value_Property_Lists(&pstPropertyList->Required.pList,
                                  &pstPropertyList->Optional.pList, 
                                  &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_BI
        case OBJECT_BINARY_INPUT:
        {
            /* Get Binary Input all types of Properties */
            Binary_Input_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList, 
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_BO
		case OBJECT_BINARY_OUTPUT:
        {
            /* Get Binary Output all types of Properties */
            Binary_Output_Property_Lists(&pstPropertyList->Required.pList,
                                         &pstPropertyList->Optional.pList, 
                                         &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_BV
        case OBJECT_BINARY_VALUE:
        {
            /* Get Binary Value all types of Properties */
            Binary_Value_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList, 
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_MSI
        case OBJECT_MULTI_STATE_INPUT:
        {
            /* Get Binary Input all types of Properties */
            Multistate_Input_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList, 
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_MSO
        case OBJECT_MULTI_STATE_OUTPUT:
        {
            /* Get Binary Input all types of Properties */
            Multistate_Output_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList, 
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_MSV
        case OBJECT_MULTI_STATE_VALUE:
        {
            /* Get Binary Input all types of Properties */
            Multistate_Value_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList, 
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_NC
		case OBJECT_NOTIFICATION_CLASS:
        {
            /* Get Notification Class all types of Properties */
			Notification_Class_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList,
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif
#ifdef BACDEL_OBJ_CAL
		case OBJECT_CALENDAR:
        {
            /* Get Event Enrollment all types of Properties */
			Calendar_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList,
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

#ifdef BACDEL_OBJ_SDL
		case OBJECT_SCHEDULE:
        {
            /* Get Event Enrollment all types of Properties */
			Schedule_Property_Lists(&pstPropertyList->Required.pList,
                                        &pstPropertyList->Optional.pList,
                                        &pstPropertyList->Proprietary.pList);
        }
        break;
#endif

		#ifndef BACDEL_PR23  //Tempory commented // AC
//		case OBJECT_NETWORK_SECURITY:
//		{
//			/* Get Network Secuirity objects all types of Properties */
//			NetworkSecuirity_Property_Lists(&pstPropertyList->Required.pList,
//								&pstPropertyList->Optional.pList,
//								&pstPropertyList->Proprietary.pList);
//		}
//		break;
		#endif

#ifdef BACDEL_OBJ_NP
		case OBJECT_NETWORK_PORT:
		{
			/* Get NetworkPort objects all types of Properties */
            NetworkPort_Property_Lists(&pstPropertyList->Required.pList,
								&pstPropertyList->Optional.pList,
								&pstPropertyList->Proprietary.pList);
		}
		break;
#endif

        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Find_Objects_PropertyList :"
                "Object of type %d is not supported\r\n",eObjectType);
			#endif
        }
        break;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Find_Objects_PropertyList : Exit \r\n");
	#endif
}

/**
*DISCREPTION
*   This function counts number of properties supported in given 
*   property list
*
*@param pList [in]  Pointer to property list
*
*/
static uint32_t property_list_count(const int32_t *pList)
{
    uint32_t property_count = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:property_list_count : Entry \r\n");
	#endif
	
    if (pList)
    {
        while (*pList != ENDOFPROPLIST)
        {
            property_count++;
            pList++;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:property_list_count : Exit \r\n");
	#endif

    return property_count;
}

/** 
*DISCREPTION
*   For a given object type, returns the special property list.
*   This function is used for ReadPropertyMultiple calls which want
*   just Required, just Optional, or All properties.
*
*@param eObjectType [in] The desired BACNET_OBJECT_TYPE whose properties
*                        are to be listed.
*@param pstPropertyList [out] Reference to the structure which will, on 
*       return,list, separately, the Required, Optional, and Proprietary 
*       object properties with their counts.
*
*/
void Device_Objects_Property_List(
    BACNET_OBJECT_TYPE eObjectType,
    special_property_list_t *pstPropertyList)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Device_Objects_Property_List : Entry \r\n");
	#endif

    pstPropertyList->Required.pList = NULL;
    pstPropertyList->Optional.pList = NULL;
    pstPropertyList->Proprietary.pList = NULL;

    /* If we can find an entry for the required object type
     * and there is an Object_List_RPM fn ptr then call it
     * to populate the pointers to the individual list counters.
     */
     Find_Objects_PropertyList(eObjectType, pstPropertyList);
    
    /* Fetch the counts if available otherwise zero them */
    pstPropertyList->Required.count =
        pstPropertyList->Required.pList == NULL ? 0 : 
            property_list_count(pstPropertyList->Required.pList);

    pstPropertyList->Optional.count =
        pstPropertyList->Optional.pList == NULL ? 0 : 
            property_list_count(pstPropertyList->Optional.pList);

    pstPropertyList->Proprietary.count =
        pstPropertyList->Proprietary.pList == NULL ? 0 : 
            property_list_count(pstPropertyList->Proprietary.pList);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Device_Objects_Property_List : Exit \r\n");
	#endif

    return;
}


/* To Include/Exclude the Change Of Value & Event Notification 
	B Side Services. */
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_AE_EN_B)

/**                                                                         
*DESCRIPTION  
*
*   The API gets executed when a Notification has to be sent.
*
*@return None
*/

void DvNotificationThread_Task(void)
{
	/* local variables */
	Post_Thread_Msg_t stQueueMsg = {0};	

	while(1)
	{
		memset(&stQueueMsg,0,sizeof(Post_Thread_Msg_t));
		if(xQueueReceive(Cov_EventQueue, &stQueueMsg, portMAX_DELAY))
		{
			if(stQueueMsg.wParam)
			{
#ifdef BACDEL_SER_DS_COV_B


				/* this is cov notification */
				Send_COV_Notification(stQueueMsg.wParam, (msgquedata_t *)stQueueMsg.lParam);

#else
				/* do nothing */;
#endif
			}
			else
			{
#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)

				/* this is event notification */
				Send_Event_Notification(stQueueMsg.wParam,(void *)stQueueMsg.lParam);

#else
				/* do nothing */;
#endif
			}
			
		}
	}

	/* function exit */
#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DvNotificationThread : exit \r\n");
#endif
}

#endif /* defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_AE_EN_B */


/**                                                                         
*DESCRIPTION  
*
*   Thread API for DCC & Backup & Restore Timeout's.
*   This api is trigerred every 1 sec.
*   It decrements :
*   - Lifetime count of DCC if not for indefinite period.
*   - Backup failure timeout if in backup or restore state.
*
*@return None
*/

#if (defined BACDEL_SER_DM_DCC_B || defined BACDEL_SER_DM_BR_B)
void DvManagementTimer(void)
{
	virtualDevData_t *pstVirtualDev = NULL;
	dcc_request_t stDccReq = {0};
	BACnetAddress_t stRmtDevAddr = {0};
	pstVirtualDev = gstHostDevice.m_pstDeviceStruct;
	stDccReq.m_eDccState = COMMUNICATION_ENABLE;
	/* Thread Exit condition */
#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_AE_EN_B)	
	OSAL_Release_Sem(m_hCOVCancelSemaphoreHandle, BACNET_ONE);
#endif	
	if(TRUE == pstVirtualDev->m_bDvMgmtThreadExit)
	{
		/* reset the flag to default value */
		pstVirtualDev->m_bDvMgmtThreadExit = FALSE;
		return;
	}
#ifdef BACDEL_SER_DM_DCC_B
	if(pstVirtualDev->m_u32DevCommLifeTime != 0)
	{
		--(pstVirtualDev->m_u32DevCommLifeTime);
		if(pstVirtualDev->m_u32DevCommLifeTime == 0)
		{
			/* make dcc enable */
			stDccReq.m_eDccState = COMMUNICATION_ENABLE;
			DCC_B_Update_State_And_Inform_Application(pstVirtualDev,
			&stDccReq, &stRmtDevAddr);
		}
	}
#endif /* BACDEL_SER_DM_DCC_B */	
}
#endif


#if (defined BACDEL_SER_DM_DCC_B || defined BACDEL_SER_DM_BR_B)
void DvManagementThread_Task(void)
{
    /* local variables */
    virtualDevData_t *pstVirtualDev = NULL;
	dcc_request_t stDccReq = {0};
	BACnetAddress_t stRmtDevAddr = {0};


    /* thread entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DvManagementThread: entry \r\n");
	#endif
	
    /* Extract the Device structure from the formal parameter */
 	pstVirtualDev = gstHostDevice.m_pstDeviceStruct;
	stDccReq.m_eDccState = COMMUNICATION_ENABLE;

    for(;;)
    {
        /* Wait till the g_hDvMgmtSemaphore released */
       OSAL_Wait_Sem(m_hDvMgmtSemaphoreHandle, 1000);

#ifdef BACDEL_SER_DM_DCC_B
        if(pstVirtualDev->m_u32DevCommLifeTime != 0)
        {
            --(pstVirtualDev->m_u32DevCommLifeTime);
            if(pstVirtualDev->m_u32DevCommLifeTime == 0)
            {
                /* make dcc enable */
				stDccReq.m_eDccState = COMMUNICATION_ENABLE;
				DCC_B_Update_State_And_Inform_Application(pstVirtualDev, 
					&stDccReq, &stRmtDevAddr);
            }
        }
#endif /* BACDEL_SER_DM_DCC_B */

    }// for(;;) ends.

    /* thread exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DvManagementThread: exit \r\n");
	#endif

   
}
#endif /* (defined BACDEL_SER_DM_DCC_B || defined BACDEL_SER_DM_BR_B) */


/**                                                                         
*DESCRIPTION  
*
*   The API gets executed on device process thread which
*   checks for type of service to be invoked.
*
*@param lpThreadParameter Thread function Input parameter
*
*@return None
*/
void DvProcessThread_Task(void)
{
    /* Device Instace Pointer */
    BACnet_Device_Struct_t *pstHostDev = NULL;
    virtualDevData_t *pVirtualDev = NULL;
	BACNET_SERVICES_SUPPORTED eServiceChoice = MAX_BACNET_SERVICES_SUPPORTED;
    /* Flag to Start encoding of response */  
    bool bEncodeEnable = FALSE ;
    bool bServiceSupport = FALSE;
    processInfo_t * pProcQIndex = NULL;

    /* Initialize the device structure pointer */
	pstHostDev=&gstHostDevice;
    pVirtualDev = pstHostDev->m_pstDeviceStruct;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DvProcessThread : Entry \r\n");
	#endif
    
	OSAL_Wait_Sem(m_hDeviceSemaphoreHandle, INFINITE);    

    for(;;)
    {
        /* Wait on semaphore */
        OSAL_Wait_Sem(m_hDeviceSemaphoreHandle, INFINITE);    
        
        
        /* Get pointer to Received request to be processed */
        pProcQIndex = pVirtualDev->m_pstDeviceProcessQueue[pVirtualDev->m_i32QProcessIndex];

        /* The NULL pProcQIndex indicates that this semaphore is released by DeInit API */
        if(NULL == pProcQIndex)
        {
            break;
        }

		/* save VD pointer */
		pProcQIndex->m_pvReqDevStruct = (void *)pVirtualDev;

		/* get service type */
        eServiceChoice = pProcQIndex->m_stProcessData.m_eServiceSupported;

		/* execute the service request */
		Execute_Service_Request_Parser(pVirtualDev, pProcQIndex,
			&bEncodeEnable, &bServiceSupport, eServiceChoice, BACNET_ZERO);

		/* send response */
		Dv_Send_Response(&bServiceSupport, &bEncodeEnable, 
			pProcQIndex, pVirtualDev, eServiceChoice);

		/* reset flags */
		bEncodeEnable = FALSE;
		bServiceSupport = FALSE;

        /* Free the processed request from Queue */
        pVirtualDev->m_pstDeviceProcessQueue
        [pVirtualDev->m_i32QProcessIndex] = NULL;
        
        /* Move to Next request for device */
        pVirtualDev->m_i32QProcessIndex++;
        if( pVirtualDev->m_i32QProcessIndex == MAX_PACKET )
        {
            pVirtualDev->m_i32QProcessIndex = 0;
        }    
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:DvProcessThread : Exit \r\n");
	#endif

}

/** Api to initialize services supported by the device */
static void Init_Device_Services(virtualDevData_t *pstDeviceStruct)
{
    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Device_Services : Entry \r\n");
	#endif

    /* reset all service supported bits */
    memset(&pstDeviceStruct->m_stDevObject.m_stServicesSupported, 0, 
            sizeof(Pr_BACnetServicesSupported_t));

#ifdef BACDEL_SER_DM_DDB_A
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btIAm = 1;
#endif
#ifdef BACDEL_SER_DM_DDB_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btWhoIs  = 1;
#endif
#ifdef BACDEL_SER_DM_DOB_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btWhoHas  = 1;
#endif
#ifdef BACDEL_SER_DS_RP_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btReadProperty = 1;
#endif
#ifdef BACDEL_SER_DS_RPM_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btReadPropertyMultiple = 1;
#endif
#ifdef BACDEL_SER_DS_WP_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btWriteProperty = 1;
#endif
#ifdef BACDEL_SER_DS_WPM_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btWritePropertyMultiple = 1;
#endif
#ifdef BACDEL_SER_DM_TS_B
	pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
		m_btTimeSynchronization = 1;
#endif
#ifdef BACDEL_SER_DM_UTC_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
		m_btUtcTimeSynchronization = 1;
#endif

#ifdef SUPPORT_MULTIPLE_DEVICE
    /* 1st host device supports execution of above services only as 
	this is virtual router device */
	/* return if this is 1st device */
    if(gstHostDevice.m_u32DeviceInstace == pstDeviceStruct->m_stDevObject.
        m_stObjectID.m_u32ObjId)
        return;
#endif

#ifdef BACDEL_SER_DS_COV_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btSubscribeCov = 1;
#endif
#ifdef BACDEL_SER_DM_DCC_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
        m_btDeviceCommunicationControl = 1;
#endif

#if (defined BACDEL_SER_AE_AA_B && defined BACDEL_OBJ_NC)
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
		m_btAcknowledgeAlarm = 1;
#endif
#ifdef BACDEL_SER_DM_RD_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
		m_btReinitializeDevice = 1;
#endif
#ifdef BACDEL_SER_AE_GEI_B
    pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
		m_btGetEventInformation = 1;
#endif
#ifdef BACDEL_PR23
#ifdef BACDEL_SER_DM_DDA_A
	pstDeviceStruct->m_stDevObject.m_stServicesSupported.m_stServiceSupport.Bit.
		m_btWhoAmI = 1;
#endif
#endif // BACDEL_PR23

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Device_Services : Exit \r\n");
	#endif
}

/** Api to initialize objects supported by the device */
static void Init_Device_Object_Type(virtualDevData_t *pstDeviceStruct)
{
    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Device_Object_Type : Entry \r\n");
	#endif

    /* reset all object type supported bits */
    memset(&pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported, 0, 
            sizeof(Pr_BACnetObjectTypesSupported_t));

    /* Set the supported objects by the device */
#ifdef BACDEL_OBJ_DEV
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btDevice = 1;
#endif


#ifdef BACDEL_OBJ_AI
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btAnalogInput = 1;
#endif

#ifdef BACDEL_OBJ_AO
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btAnalogOutput = 1;
#endif

#ifdef BACDEL_OBJ_AV
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btAnalogValue = 1;
#endif

#ifdef BACDEL_OBJ_BI
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btBinaryInput = 1;
#endif

#ifdef BACDEL_OBJ_BO
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.   
        m_btBinaryOutput = 1;
#endif

#ifdef BACDEL_OBJ_BV
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btBinaryValue = 1;
#endif

#ifdef BACDEL_OBJ_MSV
    pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
        m_btMultiStateValue = 1;
#endif

#ifdef BACDEL_OBJ_MSI
	pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
			m_btMultiStateInput = 1;
#endif

#ifdef BACDEL_OBJ_MSO
	pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
			m_btMultiStateOutput = 1;
#endif

#ifdef BACDEL_OBJ_SDL
	pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
			m_btSchedule = 1;
#endif

#ifdef BACDEL_OBJ_CAL
	pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
		m_btCalendar = 1;
#endif

#ifdef BACDEL_OBJ_NC
	pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
		m_btNotificationClass = 1;
#endif

#ifdef BACDEL_OBJ_NP
	pstDeviceStruct->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.Bit.
		m_btNetworkPort = 1;
#endif

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Device_Object_Type : Exit \r\n");
	#endif
}

/**                                                                         
*DESCRIPTION                                                                *          
*   This API Initialize the device properties at time of device creation.
*
*@param pstDeviceStruct Device data containing device object 
*                   properties, supported object etc.
*@param bIsReInitCall [in] indicates if this is reinit device call or init call.
*
*@return None
*/
void Init_Device_Properties(virtualDevData_t * pstDeviceStruct,
                uint32_t ui32Device_Id, bool bIsReInitCall)
{
	/* local variables */
    //PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
#ifdef BACDEL_PR14
	special_property_list_t stPropertyList = {0};
    uint8_t u8SkipPropCnt = 0;
#endif
#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B || \
	     (defined BACDEL_SER_AE_AA_A && defined BACDEL_PR23))
	BACnetTime_t stLocalTime = {0};
	BACnetDate_t stLocalDate = {0};
#endif

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Device_Properties : Entry \r\n");
	#endif

	if(NULL == pstDeviceStruct)
		return;

    /******* Object Id *******/
    /* Get the property access type for given object & property. */

    pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId = ui32Device_Id;
	pstDeviceStruct->m_stDevObject.m_stObjectID.m_eObjectType = OBJECT_DEVICE;
  
    /******* Object type *******/
    /* Get the property access type for given object & property. */

    pstDeviceStruct->m_stDevObject.m_stObjectType.m_eObjectType = OBJECT_DEVICE;

    /******* Update Object Name *******/

    memset(&(pstDeviceStruct->m_stDevObject.m_stObjName.m_stCHARString.m_pu8CharStr[0]),00,
        sizeof(pstDeviceStruct->m_stDevObject.m_stObjName.m_stCHARString.m_pu8CharStr));
    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stObjName.m_stCHARString.m_pu8CharStr[0]), 
        g_au8DeviceName, sizeof(pstDeviceStruct->m_stDevObject.m_stObjName.m_stCHARString.m_pu8CharStr));
    
    pstDeviceStruct->m_stDevObject.m_stObjName.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8DeviceName, sizeof(pstDeviceStruct->m_stDevObject.m_stObjName.m_stCHARString.m_pu8CharStr));


    /******* Update Device Object List Property *******/
    /* Update the device object in device object list */

    if(!bIsReInitCall)
    pstDeviceStruct->m_stDevObject.m_stObjectIDList.m_pstArrayObjId = 
        (ListOfObjId_t *)OSAL_Malloc(sizeof(ListOfObjId_t),  __FILE__, 
                                    __FUNCTION__, __LINE__);
	if(NULL == pstDeviceStruct->m_stDevObject.m_stObjectIDList.m_pstArrayObjId)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:Init_Device_Properties :"
            "OSAL_Malloc failed\r\n");
		#endif
        return;
	}

    pstDeviceStruct->m_stDevObject.m_stObjectIDList.m_pstArrayObjId->m_stObjectId.
		m_eObjectType = OBJECT_DEVICE;
    pstDeviceStruct->m_stDevObject.m_stObjectIDList.m_pstArrayObjId->m_stObjectId.
        m_u32ObjId = ui32Device_Id;

    /* Update the device in device list count */
    if(!bIsReInitCall)
    {
    pstDeviceStruct->m_stDevObject.m_stObjectIDList.m_u32ObjCount = 1;
	pstDeviceStruct->m_stDevObject.m_stObjectIDList.m_pstArrayObjId->m_pstNext = NULL;
    }

    /******* Initialize Services supported for devices *******/
    Init_Device_Services(pstDeviceStruct);

    /******* Initialize Object Type supported for devices *******/
    Init_Device_Object_Type(pstDeviceStruct);


    /******* System Status *******/
    pstDeviceStruct->m_stDevObject.m_stSystemStatus.m_eDeviceStatus = SYSTEM_STATUS;

    /******* Vendor Id *******/
    pstDeviceStruct->m_stDevObject.m_stVendorId.m_u16Val = g_u16VendorID;

    /******* Update Vendor Name *******/
    memset(&(pstDeviceStruct->m_stDevObject.m_stVendorName.m_stCHARString.m_pu8CharStr[0]),00,
        sizeof(pstDeviceStruct->m_stDevObject.m_stVendorName.m_stCHARString.m_pu8CharStr));

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stVendorName.m_stCHARString.m_pu8CharStr[0]), 
        g_au8VendorName, sizeof(pstDeviceStruct->m_stDevObject.m_stVendorName.m_stCHARString.m_pu8CharStr));
    
    pstDeviceStruct->m_stDevObject.m_stVendorName.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8VendorName, sizeof(pstDeviceStruct->m_stDevObject.m_stVendorName.m_stCHARString.m_pu8CharStr));

    /******* Update Model Name *******/
    memset(&(pstDeviceStruct->m_stDevObject.m_stModelName.m_stCHARString.m_pu8CharStr[0]), 
        00, sizeof(pstDeviceStruct->m_stDevObject.m_stModelName.m_stCHARString.m_pu8CharStr)); 

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stModelName.m_stCHARString.m_pu8CharStr[0]), 
        g_au8ModelName, sizeof(pstDeviceStruct->m_stDevObject.m_stModelName.m_stCHARString.m_pu8CharStr)); 
    
    pstDeviceStruct->m_stDevObject.m_stModelName.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8ModelName, sizeof(pstDeviceStruct->m_stDevObject.m_stModelName.m_stCHARString.m_pu8CharStr));

    /******* Firmware Revision *******/
    memset(&(pstDeviceStruct->m_stDevObject.m_stFirmwareRev.m_stCHARString.m_pu8CharStr[0]), 
        00, sizeof(pstDeviceStruct->m_stDevObject.m_stFirmwareRev.m_stCHARString.m_pu8CharStr));

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stFirmwareRev.m_stCHARString.m_pu8CharStr[0]), 
        g_au8FirmwareRev, sizeof(pstDeviceStruct->m_stDevObject.m_stFirmwareRev.m_stCHARString.m_pu8CharStr));
    
    pstDeviceStruct->m_stDevObject.m_stFirmwareRev.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8FirmwareRev, sizeof(pstDeviceStruct->m_stDevObject.m_stFirmwareRev.m_stCHARString.m_pu8CharStr));

    /******* Application Software Version *******/
    memset(&(pstDeviceStruct->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString.m_pu8CharStr[0]), 
        00, 
        sizeof(pstDeviceStruct->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString.m_pu8CharStr));

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString.m_pu8CharStr[0]), 
        g_au8ApplSoftwareVer, 
        sizeof(pstDeviceStruct->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString.m_pu8CharStr)); 

    pstDeviceStruct->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8ApplSoftwareVer, 
        sizeof(pstDeviceStruct->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString.m_pu8CharStr));

    /******* Protocol Version *******/
    pstDeviceStruct->m_stDevObject.m_stProtocolVersion.m_u32Val = PROTOCOL_VERSION;

    /******* Protocol Revision *******/
    pstDeviceStruct->m_stDevObject.m_stProtocolRevision.m_u32Val = PROTOCOL_REVISION;

    /******* Database Revision *******/
    pstDeviceStruct->m_stDevObject.m_stDatabaseRev.m_u32Val = BACnetConfigData.m_u32DatabaseREV;

    /******* Number of APDU retries *******/
    pstDeviceStruct->m_stDevObject.m_stNumOfAPDURetries.m_u32Val 
        = NUMBER_OF_APDU_RETRIES;

    /******* APDU Timeout *******/
    pstDeviceStruct->m_stDevObject.m_stAPDUTimeout.m_u32Val = APDU_TIMEOUT;

    /******* MAX_APDU_LENGTH_ACCEPTED *******/
    pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val
        = MAX_APDU_LENGTH_ACCEPTED;

    /******* Segementation Supported *******/
    pstDeviceStruct->m_stDevObject.m_stSegmentationSupport.m_eSegmentationSupport = 
        SEG_SUPPORTED; 

#ifdef SEGMENTATION_SUPPORTED
	 /******* Max_Segements_Accepted *******/
    pstDeviceStruct->m_stDevObject.m_stMaxSegAccepted.m_u16Val = MAX_SEGMENTS_ACCEPTED;
    Register_Object_Property(OBJECT_DEVICE, PROP_MAX_SEGMENTS_ACCEPTED);

	/******* APDU_Segment_Timeout *******/
    pstDeviceStruct->m_stDevObject.m_stAPDUSegTimeout.m_u32Val = APDU_SEGMENT_TIMEOUT;
    Register_Object_Property(OBJECT_DEVICE, PROP_APDU_SEGMENT_TIMEOUT);
#endif /* SEGMENTATION_SUPPORTED */

    /******* Initialize the device address binding *******/

	memset(&pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding,
		0, sizeof(BACnetAddrBinding_t));
	
	memset(&g_stDevAddBinding, 0, sizeof(Pr_ListOfBACnetAddrBinding_t));
	/* update device address binding */
	Update_Device_Address_Binding(pstDeviceStruct);

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B || \
	     (defined BACDEL_SER_AE_AA_A && defined BACDEL_PR23))
	/* Get Device Date_Time */
	GetDevice_DateTime(&stLocalDate, &stLocalTime, pstDeviceStruct);

#ifdef OPTIONAL_PROPERTY
	/*********** Local Date ***********/
	
	pstDeviceStruct->m_stDevObject.m_stLocalDate.m_Date_val.m_u8Day = LOCAL_DATE_DAY;
	pstDeviceStruct->m_stDevObject.m_stLocalDate.m_Date_val.m_u8Month = LOCAL_DATE_MONTH;
	pstDeviceStruct->m_stDevObject.m_stLocalDate.m_Date_val.m_u8Wday = LOCAL_DATE_WEEKDAY;
	pstDeviceStruct->m_stDevObject.m_stLocalDate.m_Date_val.m_u16Year = LOCAL_DATE_YEAR;
	
    Register_Object_Property(OBJECT_DEVICE, PROP_LOCAL_DATE);

	/*********** Local Time ***********/
	pstDeviceStruct->m_stDevObject.m_stLocalTime.m_Time_val.m_u8Hour = LOCAL_TIME_HR;
	pstDeviceStruct->m_stDevObject.m_stLocalTime.m_Time_val.m_u8Hundredths = LOCAL_TIME_HUNSEC;
	pstDeviceStruct->m_stDevObject.m_stLocalTime.m_Time_val.m_u8Min = LOCAL_TIME_MIN;
	pstDeviceStruct->m_stDevObject.m_stLocalTime.m_Time_val.m_u8Sec = LOCAL_TIME_SEC;
    Register_Object_Property(OBJECT_DEVICE, PROP_LOCAL_TIME);
#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */

#ifdef BACDEL_SER_DM_UTC_B
	/*********** UTC Offset ***********/
	pstDeviceStruct->m_stDevObject.m_stUTCOffset.m_i32Val = BACnetConfigData.m_u32UTCOffset;
    Register_Object_Property(OBJECT_DEVICE, PROP_UTC_OFFSET);

	/********* Day Light Saving Status ********/
	pstDeviceStruct->m_stDevObject.m_stDaylightSavingStatus.m_bVal = BACnetConfigData.m_bDaylightSavingStatus;
    Register_Object_Property(OBJECT_DEVICE, PROP_DAYLIGHT_SAVINGS_STATUS);
#endif /* BACDEL_SER_DM_UTC_B */
#endif /* OPTIONAL_PROPERTY */

    /********* Optional Properties *****************/

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
    /******* Initialize Active COV Subscription list for devices *******/
    pstDeviceStruct->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe = NULL;

    /* If supporting COV Reporting Active_Cov_Subscription property is required */
    Register_Object_Property(OBJECT_DEVICE, PROP_ACTIVE_COV_SUBSCRIPTIONS);  // <@>
#endif


   /******* Update Location *******/
    /* Get the property access type for given object & property. */
    memset(&(pstDeviceStruct->m_stDevObject.m_stLocation.m_stCHARString.m_pu8CharStr[0]),
        00, sizeof(pstDeviceStruct->m_stDevObject.m_stLocation.m_stCHARString.m_pu8CharStr));

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stLocation.m_stCHARString.m_pu8CharStr[0]), 
        g_au8Location, sizeof(pstDeviceStruct->m_stDevObject.m_stLocation.m_stCHARString.m_pu8CharStr));
    
    pstDeviceStruct->m_stDevObject.m_stLocation.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8Location, sizeof(pstDeviceStruct->m_stDevObject.m_stLocation.m_stCHARString.m_pu8CharStr));
    Register_Object_Property(OBJECT_DEVICE, PROP_LOCATION);

   /******* Update Description *******/
    /* Get the property access type for given object & property. */
    memset(&(pstDeviceStruct->m_stDevObject.m_stDescription.m_stCHARString.m_pu8CharStr[0]),
        00, sizeof(pstDeviceStruct->m_stDevObject.m_stDescription.m_stCHARString.m_pu8CharStr));

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stDescription.m_stCHARString.m_pu8CharStr[0]),
        g_au8DevDescription, sizeof(pstDeviceStruct->m_stDevObject.m_stDescription.m_stCHARString.m_pu8CharStr));
    
    pstDeviceStruct->m_stDevObject.m_stDescription.m_stCHARString.m_u32StrLen =
        Strnlen(g_au8DevDescription, sizeof(pstDeviceStruct->m_stDevObject.m_stDescription.m_stCHARString.m_pu8CharStr));
    
    Register_Object_Property(OBJECT_DEVICE, PROP_DESCRIPTION);

	/********* MSTP Properties *****************/
	/******* Max masters *******/
		pstDeviceStruct->m_stDevObject.m_stMAXMaster.m_u32Val = SMCfg.g_BACnetCfg.MaxMaster;
		Register_Object_Property(OBJECT_DEVICE, PROP_MAX_MASTER);

		/******* Max info frames *******/
		pstDeviceStruct->m_stDevObject.m_stMaxInfoFrames.m_u32Val = BACnetConfigData.m_u32MaxInfoFrame;
		Register_Object_Property(OBJECT_DEVICE, PROP_MAX_INFO_FRAMES);

#ifdef OPTIONAL_PROPERTY

#ifdef BACDEL_PR14
	/******* Update Serial Number *******/
    memset(&(pstDeviceStruct->m_stDevObject.m_stSerialNumber.m_stCHARString.m_pu8CharStr[0]), 
        00, sizeof(pstDeviceStruct->m_stDevObject.m_stSerialNumber.m_stCHARString.m_pu8CharStr));

    Strcopy(&(pstDeviceStruct->m_stDevObject.m_stSerialNumber.m_stCHARString.m_pu8CharStr[0]), 
        g_au8SerialNumber, sizeof(pstDeviceStruct->m_stDevObject.m_stSerialNumber.m_stCHARString.m_pu8CharStr));

    pstDeviceStruct->m_stDevObject.m_stSerialNumber.m_stCHARString.m_u32StrLen = 
        Strnlen(g_au8SerialNumber, sizeof(pstDeviceStruct->m_stDevObject.m_stSerialNumber.m_stCHARString.m_pu8CharStr));

    Register_Object_Property(OBJECT_DEVICE, PROP_SERIAL_NUMBER);
#endif /* BACDEL_PR14 */
#endif /* OPTIONAL_PROPERTY */

#ifdef BACDEL_PR14
	/******* Property List *******/
	/* initialize property list for Input Object Type */
	Device_Objects_Property_List(OBJECT_DEVICE, &stPropertyList);

    pstDeviceStruct->m_stDevObject.m_stPropertyList.m_psti32RequiredProp = stPropertyList.Required.pList;
    pstDeviceStruct->m_stDevObject.m_stPropertyList.m_psti32OptionalProp = stPropertyList.Optional.pList;
	pstDeviceStruct->m_stDevObject.m_stPropertyList.m_psti32ProprietaryProp = stPropertyList.Proprietary.pList;

	/* get skip property count value */
	u8SkipPropCnt = Get_PropertyList_SkipCount(&stPropertyList);
	/* save total property count value */
	pstDeviceStruct->m_stDevObject.m_stPropertyList.m_u32PropCount = 
		(stPropertyList.Required.count + stPropertyList.Optional.count
		+ stPropertyList.Proprietary.count - u8SkipPropCnt);
#endif /* PR14 */


	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Device_Properties : Exit \r\n");
	#endif
}

/**                                                                         
*DESCRIPTION                                                                          
*   This API points the application data to corresponding 
*   object property value.
*
*@param pRpData[in / out] Pointer to read property data containing
*       Device id, and Device Property to read.
*       This pointer will act as output paramter with value for device
*       read property.
*@param m_stDevObject - Device object structure containing read property
*       information for object.
*@return BACNET_ERROR_CODE.
*/
BACNET_ERROR_CODE Dv_Read_Prop(BACNET_CONF_DATA *pRpData, 
            virtualDevData_t *pVirtualDev)  
{
    /* locals */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER; 

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B || \
	     (defined BACDEL_SER_AE_AA_A && defined BACDEL_PR23))	/* to save local time */
	BACnetTime_t stLocalTime = {0}; 
	/* to save local date */
	BACnetDate_t stLocalDate = {0}; 
#endif

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Dv_Read_Prop: Entry \r\n");
	#endif

    /* validate the array index & access type */
    eErrorCode = ValidateAccessAndIndex(pRpData->eObjectProperty, 
        pRpData->eObjectType, pRpData->u32ArrayIndex, 
        &pVirtualDev->m_stDevObject, NULL, pRpData->bArrIndxPresent);
    if(ERROR_CODE_OTHER != eErrorCode)
    {
        /* array index invalid or property not supported */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
        Read_Common_Properties : ValidateAccessAndIndex fails. \r\n");
		#endif
        /* return error code */
        return eErrorCode;
    }

    /* switch to property id */
    switch(pRpData->eObjectProperty)
    {
        /* For Object Identifier */
        case PROP_OBJECT_IDENTIFIER:
        {  
            /* Update Data type for Tag at encoding time */           
            pRpData->eData_Type = BACNET_DT_OBJECTID;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stObjectID);
        }
        break;

        /* Object Name */
        case PROP_OBJECT_NAME:
        {
            /* Fill in the RP data structure */
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stObjName);
        }
        break;

        case PROP_OBJECT_TYPE:
        {
            /* Fill in the RP data structure */
            pRpData->eData_Type = BACNET_DT_OBJTYPE;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stObjectType);
        }
        break;

#ifdef BACDEL_PR14
		case PROP_PROPERTY_LIST:
        {
            /* Fill in the RP data structure */
            pRpData->eData_Type = BACNET_DT_PROPERTY_LIST;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stPropertyList);
        }
        break;
#endif

        case PROP_SYSTEM_STATUS:
        {
            /* Fill in the RP data structure */
            pRpData->eData_Type = BACNET_DT_BACNETDEVSTAT;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stSystemStatus);
        }
        break;

        case PROP_VENDOR_NAME:
        {
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stVendorName);
        }
        break;

        case PROP_VENDOR_IDENTIFIER:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED16;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stVendorId);
        }
        break;

		case PROP_MODEL_NAME:
        {
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stModelName);
        }
        break;

        case PROP_FIRMWARE_REVISION:
        {
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stFirmwareRev);
        }
        break;

        case PROP_APPLICATION_SOFTWARE_VERSION:
        {
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stApplSoftwareVersion);
        }
        break;

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B || \
	     (defined BACDEL_SER_AE_AA_A && defined BACDEL_PR23))
        case PROP_LOCAL_TIME:
        {
			/* Update Device object local date & local time property every 
				time any of these property is read */
			GetDevice_DateTime(&stLocalDate, &stLocalTime, pVirtualDev);   
			Update_Dv_Local_DateTime_Properties(pVirtualDev, &stLocalTime, &stLocalDate, false);

            pRpData->eData_Type = BACNET_DT_TIME;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stLocalTime);
        }
        break;

        case PROP_LOCAL_DATE:
        {
			/* Update Device object local date & local time property every 
				time any of these property is read */
			GetDevice_DateTime(&stLocalDate, &stLocalTime, pVirtualDev);   
			Update_Dv_Local_DateTime_Properties(pVirtualDev, &stLocalTime, &stLocalDate, false);

            pRpData->eData_Type = BACNET_DT_DATE;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stLocalDate);
        }
        break;
#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */

#ifdef BACDEL_SER_DM_UTC_B
        case PROP_UTC_OFFSET:
        {
            pRpData->eData_Type = BACNET_DT_INTEGER;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stUTCOffset);
        }
        break;       

        case PROP_DAYLIGHT_SAVINGS_STATUS:
        {
            pRpData->eData_Type = BACNET_DT_BOOLEAN;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stDaylightSavingStatus);
        }
        break;
#endif /* BACDEL_SER_DM_UTC_B */

#ifdef OPTIONAL_PROPERTY

        case PROP_LOCATION:
        {
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stLocation);
        }
		break;

        /* Description */
        case PROP_DESCRIPTION:
        {
            /* Fill in the RP data structure */
            pRpData->eData_Type = BACNET_DT_CHARSTRING;
            pRpData->pvReadPropValue = 
               &(pVirtualDev->m_stDevObject.m_stDescription);
        }
        break;

#endif /* OPTIONAL_PROPERTY */

        case PROP_PROTOCOL_VERSION:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stProtocolVersion);
        }
        break;

		case PROP_PROTOCOL_REVISION:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stProtocolRevision);
        }
        break;

        case PROP_PROTOCOL_SERVICES_SUPPORTED:
        {
            pRpData->eData_Type = BACNET_DT_SERVICES_SUPPORTED;
            pRpData->pvReadPropValue = 
                &pVirtualDev->m_stDevObject.m_stServicesSupported;
        }
        break;

        case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
        {
            pRpData->eData_Type = BACNET_DT_OBJECT_TYPE_SUPPORTED;
            pRpData->pvReadPropValue = 
                &pVirtualDev->m_stDevObject.m_stObjectTypeSupported;
        }
        break;

        case PROP_OBJECT_LIST:
        {
			pRpData->eData_Type = BACNET_DT_OBJECTID_ARRAY;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stObjectIDList);
        }
        break;
        
        case PROP_MAX_APDU_LENGTH_ACCEPTED:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED16;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted);
        }
        break;

        case PROP_SEGMENTATION_SUPPORTED:
        {
            pRpData->eData_Type = BACNET_DT_BACNETSEG;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stSegmentationSupport);
        }
        break;

        case PROP_APDU_TIMEOUT :
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED32;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stAPDUTimeout);
        }
        break;

        case PROP_NUMBER_OF_APDU_RETRIES:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED32;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stNumOfAPDURetries);
        }
        break;

        case PROP_DEVICE_ADDRESS_BINDING:
        {
            pRpData->eData_Type = BACNET_DT_ADDRESSBINDING_LIST;
            pRpData->pvReadPropValue = &g_stDevAddBinding;
        }
        break;

        case PROP_DATABASE_REVISION:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED32;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stDatabaseRev);
        }
        break;

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
        case PROP_ACTIVE_COV_SUBSCRIPTIONS:
        {
            pRpData->eData_Type = BACNET_DT_COVSUB;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stActiveCOVSubList);
        }
        break;
#endif /* (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B) */

#ifdef SEGMENTATION_SUPPORTED
        case PROP_MAX_SEGMENTS_ACCEPTED:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED16;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stMaxSegAccepted);
        }
        break;

        case PROP_APDU_SEGMENT_TIMEOUT:
        {
            pRpData->eData_Type = BACNET_DT_UNSIGNED32;
            pRpData->pvReadPropValue = 
                &(pVirtualDev->m_stDevObject.m_stAPDUSegTimeout);
        }
   		break;
#endif /* SEGMENTATION_SUPPORTED */

        case PROP_MAX_INFO_FRAMES:
        {
			pRpData->eData_Type = BACNET_DT_UNSIGNED32;
			pRpData->pvReadPropValue =
				&(pVirtualDev->m_stDevObject.m_stMaxInfoFrames);
        }
		break;
        case PROP_MAX_MASTER:
        {
			pRpData->eData_Type = BACNET_DT_UNSIGNED32;
			pRpData->pvReadPropValue =
				&(pVirtualDev->m_stDevObject.m_stMAXMaster);
        }
		break;
       default :
       {
		   #ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Dv_Read_Prop :"
                "Property ID %d is Not Supported by Device\r\n", pRpData->eObjectProperty);
			#endif
            eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;
       }
       break;
    }   

    /* Copy into the Rp data structure*/
    if(eErrorCode != ERROR_CODE_OTHER)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:Dv_Read_Prop :"
            "Error Code - %d\r\n", eErrorCode);
		#endif
        pRpData->eErrorCode = eErrorCode;
        pRpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pRpData->bErrorStatus = true;
    
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Dv_Read_Prop: Exit \r\n");
	#endif

    return eErrorCode;
}

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
	PROP_ACCESS_TYPE ePermission)                  
{
	/* local variables */
    bool eStatus = TRUE;
	uint32_t u32TimeoutValue = 0;
	uint32_t u32PreviousVal = 0;
	void *pvProAddr = NULL;

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
	/* local variables to post cov notification */
    int32_t i32ActiveCOVValue = 0;
    int32_t *pvPropCovValue = NULL;
	void *pvMsgQdata = NULL;
	Post_Thread_Msg_t postThreadMsg;
#endif

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Dv_Write_Prop : Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pWpData || NULL == pVirtualDev || NULL == pvData)
		return FALSE;

    /* FIXME: iLen < application_data_len: more data? */
    switch (pWpData->eObjectProperty)
    {
        case PROP_OBJECT_IDENTIFIER:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId),
                (const uint32_t *)&((Pr_BACnetObjId_t *)pvData)->m_u32ObjId, 
                sizeof(uint32_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId);

        }
        break;

        case PROP_OBJECT_NAME:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stObjName.m_stCHARString),0x00,
                sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stObjName.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));

            pvProAddr = &(pVirtualDev->m_stDevObject.m_stObjName);
        }
		break;

        case PROP_OBJECT_TYPE:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stObjectType.m_eObjectType),
                    (const uint8_t *)pvData, sizeof(BACNET_OBJECT_TYPE));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stObjectType.m_eObjectType);
        }
        break;

        case PROP_SYSTEM_STATUS:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stSystemStatus.m_eDeviceStatus),
                    (const uint8_t *)pvData, sizeof(BACNET_DEVICE_STATUS));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stSystemStatus);
        }
        break;

        case PROP_VENDOR_NAME:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stVendorName.m_stCHARString),0x00,
                sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stVendorName.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stVendorName);
        }
        break;

        case PROP_VENDOR_IDENTIFIER:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stVendorId.m_u16Val),
                    (const uint32_t *)pvData, sizeof(uint16_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stVendorId);
        }
        break;

        case PROP_MODEL_NAME:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stModelName.m_stCHARString),
                    0x00, sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stModelName.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));

            pvProAddr = &(pVirtualDev->m_stDevObject.m_stModelName);
        }
        break;

        case PROP_FIRMWARE_REVISION:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stFirmwareRev.m_stCHARString),
                   0x00, sizeof(BACnetCharStr_t));
            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stFirmwareRev.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));
             pvProAddr = &(pVirtualDev->m_stDevObject.m_stFirmwareRev);
        }
        break;

        case PROP_APPLICATION_SOFTWARE_VERSION:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString),
                   0x00, sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stApplSoftwareVersion.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));
             pvProAddr = &(pVirtualDev->m_stDevObject.m_stApplSoftwareVersion);
        }
        break;

        case PROP_PROTOCOL_VERSION:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stProtocolVersion.m_u32Val),
                   (const uint32_t *)pvData, sizeof(uint32_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stProtocolVersion);
        }
        break;

        case PROP_PROTOCOL_REVISION:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stProtocolVersion.m_u32Val),
                   (const uint32_t *)pvData, sizeof(uint32_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stProtocolVersion);
        }
        break;

        case PROP_PROTOCOL_SERVICES_SUPPORTED:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stServicesSupported.m_stServiceSupport),
                   (void *)pvData, sizeof(BACnetServicesSupported_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stServicesSupported);
        }
        break;

        case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport),
                   (void *)pvData, sizeof(BACnetObjectTypesSupported_t));
             pvProAddr = &(pVirtualDev->m_stDevObject.m_stObjectTypeSupported);
        }
        break;

		case PROP_DEVICE_ADDRESS_BINDING:
        {
            /* TODO - THis case needs to be worked on */
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stDevAddBinding);
        }
        break;

		case PROP_DATABASE_REVISION:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stDatabaseRev.m_u32Val),
                    (uint32_t *)pvData, sizeof(uint32_t));
             pvProAddr = &(pVirtualDev->m_stDevObject.m_stDatabaseRev);

        }
        break;

        case PROP_OBJECT_LIST:
        {

        }
        break;

        case PROP_MAX_APDU_LENGTH_ACCEPTED:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val),
                   (const uint32_t *)pvData, sizeof(uint16_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted);
        }
        break;

        case PROP_APDU_TIMEOUT:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stAPDUTimeout.m_u32Val),
                   (uint32_t *)pvData, sizeof(uint32_t)); 
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stAPDUTimeout);
        }
        break;

        case PROP_NUMBER_OF_APDU_RETRIES:
        {
			u32PreviousVal = pVirtualDev->m_stDevObject.m_stNumOfAPDURetries.m_u32Val;
            memcpy(&(pVirtualDev->m_stDevObject.m_stNumOfAPDURetries.m_u32Val),
                   (uint32_t *)pvData, sizeof(uint32_t));  
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stNumOfAPDURetries);
			/* if retries is 0, no need to change timeouts */
			if(0 == pVirtualDev->m_stDevObject.m_stNumOfAPDURetries.m_u32Val)
				;// do nothing
			else if(pVirtualDev->m_stDevObject.m_stNumOfAPDURetries.m_u32Val &&
				!u32PreviousVal)
			{
				/* make apdu time out to non-zero */
				if(!pVirtualDev->m_stDevObject.m_stAPDUTimeout.m_u32Val)					
				{
					pWpData->eObjectProperty = PROP_APDU_TIMEOUT;
					u32TimeoutValue = APDU_TIMEOUT;
					pvData = &u32TimeoutValue;
					//Dv_Write_Prop(pWpData, pVirtualDev, pvData,pVirtualDev->m_stDevObject.m_stAPDUTimeout); //<@@>
				}
#ifdef SEGMENTATION_SUPPORTED
				/* make apdu segment time out to non-zero */
				if(!pVirtualDev->m_stDevObject.m_stAPDUSegTimeout.m_u32Val)
				{
					pWpData->eObjectProperty = PROP_APDU_SEGMENT_TIMEOUT;
					u32TimeoutValue = APDU_SEGMENT_TIMEOUT;
					pvData = &u32TimeoutValue;
					Dv_Write_Prop(pWpData, pVirtualDev, pvData, READ_WRITE);
				}
#endif
			}
        }
        break;

        case PROP_SEGMENTATION_SUPPORTED:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stSegmentationSupport.
                m_eSegmentationSupport), (const uint32_t *)pvData, sizeof
                (BACNET_SEGMENTATION));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stSegmentationSupport);
        }
        break;

#ifdef SEGMENTATION_SUPPORTED
        case PROP_APDU_SEGMENT_TIMEOUT:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stAPDUSegTimeout.m_u32Val),
                   (uint32_t *)pvData, sizeof(uint32_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stAPDUSegTimeout);
        }
        break;       
        case PROP_MAX_SEGMENTS_ACCEPTED:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stMaxSegAccepted.m_u16Val),
                   (uint32_t *)pvData, sizeof(uint16_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stMaxSegAccepted);
        }
        break;  
#endif /* SEGMENTATION_SUPPORTED */

        case PROP_MAX_MASTER:
		{
			memcpy(&(pVirtualDev->m_stDevObject.m_stMAXMaster.m_u32Val),
				   (uint32_t *)pvData, sizeof(uint32_t));
			pvProAddr = &(pVirtualDev->m_stDevObject.m_stMAXMaster);

			pVirtualDev->m_stSupportedObjects.m_pstNetworkPort->m_stMAXMaster.m_u32Val = pVirtualDev->m_stDevObject.m_stMAXMaster.m_u32Val;
			g_stMstpVariables.MaxMaster = pVirtualDev->m_stDevObject.m_stMAXMaster.m_u32Val;

			SMCfg.g_BACnetCfg.MaxMaster = pVirtualDev->m_stDevObject.m_stMAXMaster.m_u32Val;
			SaveFactoryDatatoNV();
		}
		break;

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
        case PROP_ACTIVE_COV_SUBSCRIPTIONS:
        {
			memcpy(&(pVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe),
                    (ListOfBACnetCovSubs_t *)pvData, sizeof(ListOfBACnetCovSubs_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stActiveCOVSubList);
        }
        break;
#endif /* (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B) */

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B || \
	     (defined BACDEL_SER_AE_AA_A && defined BACDEL_PR23))
        case PROP_LOCAL_TIME:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stLocalTime.m_Time_val),
                   (BACnetTime_t *)pvData, sizeof(BACnetTime_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stLocalTime);
        }
        break;
        case PROP_LOCAL_DATE:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stLocalDate.m_Date_val),
                   (BACnetDate_t *)pvData, sizeof(BACnetDate_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stLocalDate);
        }
        break;
#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */

#ifdef BACDEL_SER_DM_UTC_B
		case PROP_UTC_OFFSET:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stUTCOffset.m_i32Val),
                    (uint32_t *)pvData, sizeof(uint32_t));
             pvProAddr = &(pVirtualDev->m_stDevObject.m_stUTCOffset);
             BACnetConfigData.m_u32UTCOffset=pVirtualDev->m_stDevObject.m_stUTCOffset.m_i32Val;
             SaveBACnetDatatoNV();
        }
        break;
        case PROP_DAYLIGHT_SAVINGS_STATUS:
        {
            memcpy(&(pVirtualDev->m_stDevObject.m_stDaylightSavingStatus.m_bVal),
                   (uint32_t *)pvData, sizeof(bool));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stDaylightSavingStatus);
            BACnetConfigData.m_bDaylightSavingStatus=pVirtualDev->m_stDevObject.m_stDaylightSavingStatus.m_bVal;
            SaveBACnetDatatoNV();
        }
        break;
#endif /* BACDEL_SER_DM_UTC_B */
        

        case PROP_LOCATION:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stLocation.m_stCHARString),
                   0x00, sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stLocation.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stLocation);
        }
        break;
        case PROP_DESCRIPTION:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stDescription.m_stCHARString),
                   0x00, sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stDescription.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));
            pvProAddr = &(pVirtualDev->m_stDevObject.m_stDescription);
        }
        break;
#ifdef OPTIONAL_PROPERTY

#ifdef BACDEL_PR14
		case PROP_SERIAL_NUMBER:
        {
            memset(&(pVirtualDev->m_stDevObject.m_stSerialNumber.m_stCHARString),
                    0x00, sizeof(BACnetCharStr_t));

            /* Copy data into property offset address */
            memcpy(&(pVirtualDev->m_stDevObject.m_stSerialNumber.m_stCHARString),(BACnetCharStr_t *)pvData,
                            sizeof(BACnetCharStr_t));
             pvProAddr = &(pVirtualDev->m_stDevObject.m_stSerialNumber);
        }
        break; 
#endif

#endif /* OPTIONAL_PROPERTY */

		/* default case */
		default:
        {
            eStatus = FALSE;
            pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
            pWpData->eErrorCode = ERROR_CODE_WRITE_ACCESS_DENIED;         
        }
        break;
    }

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
    /* Retrive the COVIndex from the property */
    pvPropCovValue = Get_ActiveCOV_Value_Pointer(&(pVirtualDev->m_stDevObject), 
		OBJECT_DEVICE, pWpData->eObjectProperty);
    if(pvPropCovValue != NULL)
    {
        i32ActiveCOVValue = *pvPropCovValue;    //Get value of COV variable in Property
        if(i32ActiveCOVValue)
        {
			/* fill data for cov notification */
            BACnetAddress_t stAddress = {0};
			pvMsgQdata = Create_Message_Queue_Struct(pVirtualDev,
				pvProAddr, FALSE, pWpData->eObjectProperty,
				stAddress, DATA_NOT_AVALIABLE);
            if(NULL != pvMsgQdata)
            {
				/* post cov notification */
                postThreadMsg.idThread = pVirtualDev->m_dwNotificationThreadID;
				postThreadMsg.MsgType = MSGQ_TYPE;
				postThreadMsg.wParam = (unsigned int)i32ActiveCOVValue;
				postThreadMsg.lParam = (void*)pvMsgQdata;
				xQueueSend(Cov_EventQueue, &postThreadMsg, (TickType_t)100);
            }
        }
    }
#endif /* BACDEL_SER_DS_COV_B || BACDEL_SER_DS_COVP_B */

	/* check if property was written */
    if(!eStatus)
    {
		/* property is not writable */
        pWpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pWpData->eErrorCode = ERROR_CODE_WRITE_ACCESS_DENIED;         
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Dv_Write_Prop : Exit \r\n");
	#endif
    return eStatus;
}


/**
*
*DESCRIPTION
*   Clear the Host Device Address binding for Remote Device Entries. This API is
*called by Initiate_Service_Retry_Timer routine in case of Timer timeout.
*@param  i32DeviceID [in] Remote Device id whose entry from device address binding
*                         to be cleared.
**@param  pstVirtualDev [in] Host Device Pointer.
*
*@return BACNET_RETURN_TYPE BACDEL_SUCCESS : In case of Remote Device entry cleared
*from Address Binding array. BACDEL_DEVICE_NOT_PRESENT : In case when remote device*entry is not present.
*
*/
BACNET_RETURN_TYPE Clear_Device_Address_Binding(
	uint32_t u32DeviceID, 
	BACnetAddress_t *pstDeviceAdd,
    virtualDevData_t *pstVirtualDev)
{
	/* local variables */
	BACnetAddrBinding_t *pstAddBindData = NULL;
	BACnetAddrBinding_t *pstPrevious = NULL;
	BACNET_RETURN_TYPE eRetVal = BACDEL_DEVICE_NOT_PRESENT;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Device_Address_Binding: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == pstVirtualDev)
	{
        return BACDEL_ERROR;
	}

	/* Assign the base address of Address Binding data */
	/* acquire the mutex */
	Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);
	pstAddBindData = &g_stDevAddBinding.m_stAddBinding;

	/* make previous node as 1st node - default value */
	pstPrevious = pstAddBindData;

    /* Traverse the Device Address Binding Array */
    while(NULL != pstAddBindData)
    {
        /* Check for Remote Device Object id entry */
        if((pstAddBindData->m_u32ObjId == u32DeviceID) && 
            !memcmp(&pstAddBindData->m_stAddress, pstDeviceAdd, sizeof(BACnetAddress_t)) &&
		   (pstPrevious != pstAddBindData))
        {
			/* maintain the linklist */
			pstPrevious->pstNext = pstAddBindData->pstNext;
			pstAddBindData->pstNext = NULL;

            /* decrement total list count */
            if(g_stDevAddBinding.m_u32Count > 0)
			{
                g_stDevAddBinding.m_u32Count--;
			}

			/* return success */
            eRetVal = BACDEL_SUCCESS;
			break;
        }
		/* save previous node in linklist */
		pstPrevious = pstAddBindData;
		/* move to next node */
		pstAddBindData = pstAddBindData->pstNext;
    }

	/* release the mutex */
	Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);

	if(NULL != pstAddBindData)
	{
		/* free the memory allocated to node */
		OSAL_Free(pstAddBindData, __FILE__, __FUNCTION__, __LINE__);
	}

	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
	Clear_Device_Address_Binding: device not present \r\n");
	#endif
    return eRetVal;
}

/**
*
*DECRIPTION
*   This function updates the Device Address Binding structure with self
*   address.
*
*@param pstDeviceStruct [in] Pointer to Device data structure.
*
*@return void
*
*/
void Update_Device_Address_Binding(virtualDevData_t *pstDeviceStruct)
{
	/* local variables */
    PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;

   // struct sockaddr_in pstSocketAddress;

 
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Update_Device_Address_Binding : Entry \r\n");
	#endif

    /* Initialize the device address binding */
    ePermission = GetDefndPropAccess(OBJECT_DEVICE, PROP_DEVICE_ADDRESS_BINDING, 
        ePermission);
    if(ePermission == NOT_SUPPORTED || NULL == pstDeviceStruct)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
        Update_Device_Address_Binding : address binding not supported or null pointer. \r\n");
		#endif
        return;
    }

    /* clear the property structure */
    memset(&pstDeviceStruct->m_stDevObject.m_stDevAddBinding, 0, sizeof(Pr_ListOfBACnetAddrBinding_t));

    /* Initialize the Device Address Binding Structure with self IP Address */
    /* Update the self Address of the device */


#ifdef SUPPORT_MULTIPLE_DEVICE // Fixed against bug id 3165 - bugzilla
	if(gstHostDevice.m_u32DeviceInstace != pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId)
	{
    /* save DNET & DADR */
    pstDeviceStruct->m_stDevObject.
        m_stDevAddBinding.m_stAddBinding.m_stAddress.u16net = g_u16VirtualNWNo;
    memcpy(pstDeviceStruct->m_stDevObject.
        m_stDevAddBinding.m_stAddBinding.m_stAddress.u8DvDadr, 
        pstDeviceStruct->m_u8DvSADR, BACNET_DADR_LEN);
    pstDeviceStruct->m_stDevObject.
        m_stDevAddBinding.m_stAddBinding.m_stAddress.u8dlen = BACNET_DADR_LEN;
	}
#endif

    /* Update the Object Identifier */
	pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding.m_eObjectType = 
		pstDeviceStruct->m_stDevObject.m_stObjectID.m_eObjectType;
    pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding.m_u32ObjId =
		pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId;
    /* Update the Max APDU length Accepted */
    pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding.m_u16MaxAPDULenAccepted =
		pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val;
	/* Update the next node pointer */
	pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding.pstNext = NULL;
	/* Update the Segmentation Support */
	pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding.m_eSegmentationSupport = 
		pstDeviceStruct->m_stDevObject.m_stSegmentationSupport.m_eSegmentationSupport;
    /* Update the vendor id */
    pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding.
        m_u16VendorId = g_u16VendorID;

    /* if this is the 1st host device, 
       add its entry in global device address binding list */
    if(pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId == gstHostDevice.m_u32DeviceInstace)
    {
        /* copy data */
        memcpy(&g_stDevAddBinding.m_stAddBinding, 
            &pstDeviceStruct->m_stDevObject.m_stDevAddBinding.m_stAddBinding,
            sizeof(BACnetAddrBinding_t));
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Update_Device_Address_Binding : Exit \r\n");
	#endif

    return;
}

/**
*
* DECRIPTION
* Function to write database revision property.
*   - this api will generate covp notification if subscribed for the property.
*
*@param pstVirtualDev [in] virtual device instance.
*@return void
*
*/
void Update_Database_Revision_Value(virtualDevData_t *pstVirtualDev)
{
    /* local variables */
    BACNET_CONF_DATA stWpData = {0};
    uint32_t u32Value = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_Database_Revision_Value: entry \r\n");
	#endif

    if(NULL != pstVirtualDev)
    {
        /* Update the database revision */
        stWpData.eObjectType = OBJECT_DEVICE;
        stWpData.eObjectProperty = PROP_DATABASE_REVISION;

        /* increment the value by 1 */
        u32Value = pstVirtualDev->m_stDevObject.m_stDatabaseRev.m_u32Val;
        u32Value++;

        /* write the property */
        Dv_Write_Prop(&stWpData, pstVirtualDev, &u32Value, READ_ONLY);

        BACnetConfigData.m_u32DatabaseREV = pstVirtualDev->m_stDevObject.m_stDatabaseRev.m_u32Val;
        SaveBACnetDatatoNV();
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_Database_Revision_Value: exit \r\n");
	#endif

    return;
}

#ifdef INITIATE_SERVICE_ENABLED

/**
*
*DECRIPTION
*   This function initiates the required services at the start of exe & when
*   IP Address is changed.
*
*@param  i32DevId [in]  indicates the device id of which services are to be
*						initiated.
*						if -ve value then, initaite services of all devices.
*						if +ve value then, initiate services of respective device.
*@return void
*
*/
void Initiate_Services(int32_t i32DevId)
{
	/* local variables */
    BACnet_Device_Struct_t *pstHostDevice = NULL;
	int32_t i32DevRangeLowLimit = -1;
	int32_t i32DevRangeHighLimit = -1;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Initiate_Services : Entry \r\n");
	#endif

	/* get host device pointer */
	pstHostDevice = &gstHostDevice;

	/* initiate services of all devices */
	if(i32DevId < BACNET_ZERO)
	{
        /* Traverse the Host Device Link List */
		while(NULL != pstHostDevice)
		{
			/* update device address binding */
			Update_Device_Address_Binding(pstHostDevice->m_pstDeviceStruct);
			/* Move to Next Host Device in the link list */
			pstHostDevice = (BACnet_Device_Struct_t *)pstHostDevice->pstNextDevice;
		}

		#if (STACK_CONFIG_WHOIS_AT_STACK_INIT)
		/* send who is */
		Send_WHO_IS(gstHostDevice.m_pstDeviceStruct,
				i32DevRangeLowLimit, i32DevRangeHighLimit, false, 0,
				DESTINATION_IS_LOCAL_BROADCAST);
		#endif
		; // dummy statement
	}
	/* initiate services of respective device only */
	else
	{
		; // dummy statement
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Initiate_Services : Exit \r\n");
	#endif
}
#endif /* INITIATE_SERVICE_ENABLED */

/**
*
* DESCRIPTION
* Function to add node in callback notification queue.
*
* @param pstRmtDevAddr		[in] destination address.
* @param pstTimeStamp		[in] timestamp at which request received.
* @param ePriority			[in] priority for processing requests.
* @param eServiceType		[in] service type.
* @param pvServiceData		[in] service data.
* @param pstVirtualDev		[in] virtual device structure.
*
* @return BACDEL_ERROR - If function failes to add node in queue.
*		  BACDEL_SUCCESS - If function adds a node in queue.
*
*/
BACNET_ERROR_CODE CbNotifyQue_AddNode(
	BACnetAddress_t	*pstRmtDevAddr,
	BACnetTimeStamp_t *pstTimeStamp,
	BACNET_MESSAGE_PRIORITY	ePriority,
	BACNET_SERVICES_SUPPORTED eServiceType,
	void *pvServiceData,
	virtualDevData_t *pstVirtualDev,
	BACNET_ERROR_CLASS *peErrorClass)
{
	/* local variables */
	CallBackNotifyData_t *pstNewNode = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"CbNotifyQue_AddNode: Entry \r\n");
	#endif

	/* if input pointer NULL */
	if(NULL == pstRmtDevAddr || NULL == pvServiceData ||
	   NULL == pstVirtualDev || NULL == peErrorClass)
	{
		/* invalid pointers */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"CbNotifyQue_AddNode: null input pointer \r\n");
		#endif

		/* set error to return */
		if(NULL != peErrorClass)
			*peErrorClass = ERROR_CLASS_DEVICE;
		return ERROR_CODE_INTERNAL_ERROR;
	}

	/* allocate memory */
	pstNewNode = OSAL_Malloc(sizeof(CallBackNotifyData_t),
		__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstNewNode)
	{
		/* malloc failed */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"CbNotifyQue_AddNode: malloc error \r\n");
		#endif

		/* set error to return */
		*peErrorClass = ERROR_CLASS_RESOURCES;
		return ERROR_CODE_OUT_OF_MEMORY;
	}
	/* next of newely created node is always NULL */
	pstNewNode->pstNext = NULL;

	/* store data for that node */
	pstNewNode->m_ePriority = ePriority;
	pstNewNode->m_eServiceType = eServiceType;
	memcpy(&pstNewNode->m_stRmtDevAddr, pstRmtDevAddr, sizeof(BACnetAddress_t));
	pstNewNode->m_pvServiceData = pvServiceData;

	/* acquire the mutex to get callback notification queue */
	Osal_Wait_Mutex(g_hCallbackMtxLock, INFINITE);

	/* if queue count reaches max count */
	if(g_stDvCallbackNotifyQ.m_u32QueCount >= g_stStackMaxLimits.m_u32CbNotifyQueNodes)
	{
		/* release the mutex for callback notification queue */
		Osal_Release_Mutex(g_hCallbackMtxLock);

		/* queue full */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: "
		"CbNotifyQue_AddNode: queue full \r\n");
		#endif

		/* free the allocated memory */
		OSAL_Free(pstNewNode, __FILE__, __FUNCTION__, __LINE__);
		/* set error to return */
		*peErrorClass = ERROR_CLASS_RESOURCES;
		return ERROR_CODE_NO_SPACE_TO_ADD_LIST_ELEMENT;
	}

	/* add node to queue */
	if(g_stDvCallbackNotifyQ.m_pstFirstNode == NULL)
	{
		/* 1st node in list */
		g_stDvCallbackNotifyQ.m_pstFirstNode = pstNewNode;
	}
	else
	{
		/* add as last node of list */
		g_stDvCallbackNotifyQ.m_pstLastNode->pstNext = pstNewNode;
	}
	/* update the last node address */
	g_stDvCallbackNotifyQ.m_pstLastNode = pstNewNode;

	/* increment the current queue count */
	g_stDvCallbackNotifyQ.m_u32QueCount++;

	/* release the mutex for callback notification queue */
	Osal_Release_Mutex(g_hCallbackMtxLock);

	/* release semaphore */
	if (!OSAL_Release_Sem(pstVirtualDev->m_hCallbackSemaphore, BACNET_ONE))
    {
		/* sem release failed */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: "
		"CbNotifyQue_AddNode : sem release failed \r\n");
		#endif

		/* set error to return */
		*peErrorClass = ERROR_CLASS_DEVICE;
		return ERROR_CODE_OPERATIONAL_PROBLEM;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: "
	"CbNotifyQue_AddNode: Exit \r\n");
	#endif
	return MAX_BACNET_ERROR_CODE;
}


/**
*
* DESCRIPTION                                                                          
* Function to execute the service request parser as per service choice.
*    
* @param pVirtualDev      [in]  virtual device data
* @param pProcQIndex      [in]  service request data
* @param eServiceType	  [in]  service type
* @param u8ThreadNo		  [in]  thread pool thread number
* @param pbEncodeEnable   [out] flag to save service respone value
* @param pbServiceSupport [out] flag to save service support value
*
* @returns [out] void.
*
*/
void Execute_Service_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pProcQIndex,
	bool *pbEncodeEnable,
	bool *pbServiceSupport,
	BACNET_SERVICES_SUPPORTED eServiceType,
	uint8_t u8ThreadNo)
{
	/* local variables */
    BACNET_CONF_DATA *pWpData = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Execute_Service_Request_Parser: Entry \r\n");
	#endif

	/* null check for input pointers */
    if(NULL == pProcQIndex || NULL == pVirtualDev)
    {
        return;
    }

	/* set default flag values */
	*pbServiceSupport = FALSE;
	*pbEncodeEnable = FALSE;

	/* execute service request parser */
	switch(eServiceType)
    {
        #ifdef BACDEL_SER_DS_RP_B
        case SERVICE_SUPPORTED_READ_PROPERTY:
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btReadProperty)                
            {
				RP_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo); 
            }
            else
			{
                *pbServiceSupport = TRUE;
			}
                
            /* This is confirm service so response is must */
            *pbEncodeEnable = TRUE;
        }
        break; 
        #endif

        #ifdef BACDEL_SER_DS_WP_B
        case SERVICE_SUPPORTED_WRITE_PROPERTY:
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btWriteProperty)                
            {
				/* default value for WP-B response pdu type */
				pProcQIndex->m_stProcessData.m_ePDUType = PDU_TYPE_SIMPLE_ACK;
				/* get the write proeprty data from orocess Q */
                pWpData = pProcQIndex->m_stProcessData.m_stAPDU.
					m_pvServiceRequestData;
				/* write property */
				WP_B_Request_Parser(pVirtualDev, pWpData, 
					&pProcQIndex->m_stProcessData.m_stRmDvAddr, 
					NULL, u8ThreadNo);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
           *pbEncodeEnable = TRUE;
        }
        break;
        #endif

        #ifdef BACDEL_SER_DS_RPM_B
        case SERVICE_SUPPORTED_READ_PROP_MULTIPLE:
        {
			/* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btReadPropertyMultiple)
            {
				RPM_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
            *pbEncodeEnable = TRUE;
        }
        break;
        #endif

        #ifdef BACDEL_SER_DS_WPM_B
        case SERVICE_SUPPORTED_WRITE_PROP_MULTIPLE:
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btWritePropertyMultiple)
            {
				WPM_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
            *pbEncodeEnable = TRUE;
        }
        break;
        #endif

        #ifdef BACDEL_SER_DS_COV_B
        case SERVICE_SUPPORTED_SUBSCRIBE_COV :
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btSubscribeCov)
            {
                COV_Subscribe_Request_Parser(pVirtualDev, pProcQIndex);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
            *pbEncodeEnable = TRUE;
        }
        break;
        #endif /* BACDEL_SER_DS_COV_B */

        #ifdef BACDEL_SER_DM_DCC_B
        case SERVICE_SUPPORTED_DEVICE_COMMUNICATION_CONTROL :
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btDeviceCommunicationControl)
            {
				DCC_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
			*pbEncodeEnable = TRUE;
        }
        break;
        #endif /* BACDEL_SER_DS_COV_B */

        #ifdef BACDEL_SER_DM_DDB_B
        case SERVICE_SUPPORTED_WHO_IS :
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btWhoIs)
            {
                *pbEncodeEnable = Who_Is_B_Request_Parser(pVirtualDev, 
					pProcQIndex,u8ThreadNo);
            }
        }
        break;
        #endif /* BACDEL_SER_DM_DDB_B */

        #ifdef BACDEL_SER_DM_DDB_A
        case SERVICE_SUPPORTED_I_AM :
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btIAm)
            {
                I_AM_A_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
        }
        break;
        #endif /* BACDEL_SER_DM_DDB_A */

        #ifdef BACDEL_SER_DM_DOB_B
        case SERVICE_SUPPORTED_WHO_HAS :
		{
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btWhoHas)
            {
               *pbEncodeEnable = Who_Has_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);   
            }
		}
        break;
        #endif /* BACDEL_SER_DM_DOB_B */

		#if (defined BACDEL_SER_AE_AA_B && defined BACDEL_OBJ_NC)
		case SERVICE_SUPPORTED_ACKNOWLEDGE_ALARM:
		{
			/* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
				m_stServiceSupport.Bit.m_btAcknowledgeAlarm)                
            {
				*pbEncodeEnable = AckAlarm_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
            //*pbEncodeEnable = TRUE;
		}
		break;
		#endif /* BACDEL_SER_AE_AA_B */

		#ifdef BACDEL_SER_AE_GEI_B
        case SERVICE_SUPPORTED_GET_EVENT_INFORMATION :
        {
            /* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btGetEventInformation)
            {
                GEI_B_Request_Parser(pVirtualDev, pProcQIndex);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
            *pbEncodeEnable = TRUE;
        }
        break;
		#endif /* BACDEL_SER_AE_GEI_B */


		#ifdef BACDEL_SER_DM_TS_B
		case SERVICE_SUPPORTED_TIME_SYNCHRONIZATION:
		{
			/* this is unconfirmed service so no need to encode any reply */
			/* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btTimeSynchronization)
            {
                TimeSync_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
		}
		break;
		#endif /* BACDEL_SER_DM_TS_B */

		#ifdef BACDEL_SER_DM_UTC_B
		case SERVICE_SUPPORTED_UTC_TIME_SYNCHRONIZATION:
		{
			/* this is unconfirmed service so no need to encode any reply */
			/* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
                m_stServiceSupport.Bit.m_btUtcTimeSynchronization)
            {
                UtcTimeSync_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
		}
		break;
		#endif /* BACDEL_SER_DM_UTC_B */

		#ifdef BACDEL_SER_DM_RD_B
		case SERVICE_SUPPORTED_REINITIALIZE_DEVICE:
		{
			/* check service is supported or not */
            if(pVirtualDev->m_stDevObject.m_stServicesSupported.
				m_stServiceSupport.Bit.m_btReinitializeDevice)                
            {
                *pbEncodeEnable = RD_B_Request_Parser(pVirtualDev, pProcQIndex, u8ThreadNo);
            }
            else
			{
                *pbServiceSupport = TRUE;
			}

            /* This is confirm service so response is must */
            //*pbEncodeEnable = true;
		}
		break;
		#endif /* BACDEL_SER_DM_RD_B */
#ifdef BACDEL_PR23
		#ifdef BACDEL_SER_DM_DDA_A
		case SERVICE_SUPPORTED_WHO_AM_I:
		{
			/* check service is supported or not */
			if (pVirtualDev->m_stDevObject.m_stServicesSupported.
				m_stServiceSupport.Bit.m_btWhoAmI)
			{
				*pbEncodeEnable = Who_Am_I_A_Request_Parser(pVirtualDev,
					pProcQIndex, u8ThreadNo);
			}
		}
		break;
		#endif /* BACDEL_SER_DM_DDA_A */
#endif /* BACDEL_PR23 */

        default :
        {
            /* No chance to come here as service choice is checked in 
                APDU handler */
            *pbServiceSupport = TRUE;
        }
        break;           
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Execute_Service_Request_Parser: Exit \r\n");
	#endif
}


/**
*
* DESCRIPTION                                                                          
* Function to trigger transmit thread for sending service response.
*    
* @param pProcQIndex      [in] process Q data
* @param pstVirtualDev    [in] virtual device data
* @param eServiceType  	  [in] service type
* @param pbServiceSupport [in] flag to get service support
* @param pbEncodeEnable   [in] flag to check service response

* @returns [out] void.
*
*/
void Dv_Send_Response(bool *pbServiceSupport,
	bool *pbEncodeEnable,
	processInfo_t *pProcQIndex,
	virtualDevData_t *pstVirtualDev,
	BACNET_SERVICES_SUPPORTED eServiceType)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Dv_Send_Response: Entry \r\n");
	#endif

	/* service is not supported by this device */
    if(*pbServiceSupport)
    {
        /* service not supported, send UNRECOGNIZED_SERVICE error */
        pProcQIndex->m_stProcessData.m_ePDUType = PDU_TYPE_REJECT;
        pProcQIndex->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
            REJECT_REASON_UNRECOGNIZED_SERVICE;
    }

    /* check if we want give response to service request */
    if(*pbEncodeEnable)
    {
		#ifdef SUPPORT_MULTIPLE_DEVICE
        /* copy device address - DADR */
        Copy_Device_Address(pProcQIndex, pstVirtualDev);
		#endif

		/* change process Q state to done */
        pProcQIndex->m_eState = PROC_DONE;

        /* increment the TX semaphore */
        if (!OSAL_Release_Sem(g_hTxSemaphoreHandle, BACNET_ONE))
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
			Dv_Send_Response : semaphore release failed \r\n");
			#endif

            /* Can't return error from here as this API returns void */
            /* Call the clean up function */
            Exit_Routine_To_Error_Of_Unconfirmed_Request(pProcQIndex);
            /* clear the queue location */
            memset(pProcQIndex, 0, sizeof(processInfo_t));
        }
    }
    else
    {
		/* avoid making state to init for following services as they trigger
			the Tx thread directly from request parser */
		if(SERVICE_SUPPORTED_ACKNOWLEDGE_ALARM != eServiceType &&
			SERVICE_SUPPORTED_REINITIALIZE_DEVICE != eServiceType && 
			SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION != eServiceType)
		{
			/* Processing is complete so release Process Queue index 
			   for next Commad to fill */
			pProcQIndex->m_eState = PROC_INIT;

			/* check if memory needs to be freed */
			/* free service data if it is not cleared */
			Clear_Bside_ServiceData(pProcQIndex->m_stProcessData.m_eServiceSupported,
				&(pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData));

            /* check if this is dynamically allocated process_info structure */
            if(TRUE == pProcQIndex->m_bIsDynamic)
			{
                /* free the memory */
                OSAL_Free(pProcQIndex, __FILE__, __FUNCTION__, __LINE__);
			}
            else
			{
                /* clear the queue location */
                memset(pProcQIndex, 0, sizeof(processInfo_t));
			}
		}
    }

	/* reset flag to default values */
    *pbServiceSupport = FALSE;
    *pbEncodeEnable = FALSE;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Dv_Send_Response: Exit \r\n");
	#endif
}

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
BACnetAddrBinding_t *DB_Find_Dev_Using_Addr(BACnetAddress_t *pstDeviceAdd)
{
	/* local variables */	
	BACnetAddrBinding_t	*pstAddBindData = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	DB_Find_Dev_Using_Addr : entry \r\n");
	#endif

	/* check null pointer */
	if(NULL == pstDeviceAdd)
	{
		/* invalid inputs pointer */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAPI: \
		DB_Find_Dev_Using_Addr: Null input pointers \r\n");
		#endif
		return NULL;
	}

	/* acquire the mutex */
	Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);

	/* get pointer of 1st node */
	pstAddBindData = &g_stDevAddBinding.m_stAddBinding;

	/* Check if any entry is avaliable for the device */
    while(NULL != pstAddBindData)
    {
		/* compare address */
		if(!memcmp(&pstAddBindData->m_stAddress, pstDeviceAdd, 
			sizeof(BACnetAddress_t)))
        {
			/* match found */
            break;
        }		
        /* move to next node */
		pstAddBindData = pstAddBindData->pstNext;
    }

	/* release the mutex */
	Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	DB_Find_Dev_Using_Addr : exit \r\n");
	#endif
	/* return value */
	return pstAddBindData;
}

/**
*
* DESCRIPTION
* This function is used to find address of node using BACnet address from 
* static or dynamic address binding list.
*
* @param pstDestBACnetAddr	[in]  destination device BACnet address e.g IP address.
* @return					[out] node pointer.
*
*/
BACnetAddrBinding_t	*Find_Dev_Using_Address(BACnetAddress_t *pstDestBACnetAddr)
{
	/* local variable */
	BACnetAddrBinding_t *pstNodeAddress = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Find_Dev_Using_Address : entry \r\n");
	#endif

	/* check null pointer */
	if(NULL == pstDestBACnetAddr)
	{
		/* invalid inputs pointer */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Find_Dev_Using_Address: Null input pointers \r\n");
		#endif
		return NULL;
	}

	if(NULL == pstNodeAddress)
	{
		/* search node in dynamic address binding */
		pstNodeAddress = DB_Find_Dev_Using_Addr(pstDestBACnetAddr);
	}
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Find_Dev_Using_Address : exit \r\n");
	#endif
	/* return value */
	return pstNodeAddress;
}


/****************************** end of file ************************/
