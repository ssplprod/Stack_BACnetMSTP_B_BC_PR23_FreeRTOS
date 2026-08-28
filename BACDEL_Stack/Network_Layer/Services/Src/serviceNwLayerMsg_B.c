/**********************************************************************************************
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
*
*   FILE NAME
*	serviceNwLayerMsg_B.c
*
*	AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION
*	This file contains function handlers for all B-side network layer 
*	protocol services.
*
*   RELEASE HISTORY
*   DATE            NAME				DESCRIPTION
*	18-Feb-2014		Pratham N. M.		File creation.
*	18-Feb-2014		Pratham N. M.		Added WHO-IS-Router-to-Network request decoder.
*
*
***********************************************************************************************/

#ifdef NETWORK_LAYER_MESSAGE_B


/** include header files */
#include "serviceNwLayerMsg_B.h"
#include "serviceNwLayerMsg_A.h"
//#include "Debug.h"
#include "objDevice.h" 
#include "pduEncodeDecode.h"

/** Global variable for Device Virtual Network No. */
extern uint16_t g_u16VirtualNWNo;

/** Global variable for Device Local Network No. */
extern uint16_t g_u16LocalNWNo;

extern DB_t SMCfg;
/** Global linklist to save Router-Available-To-Network data */
IamRouterToNwBindData_t *g_pstRouterAvailNw = NULL;

/** global linklist to save Router-Busy-To-Network data */
IamRouterToNwBindData_t	*g_pstRouterBusyToNw;

/** 
*
* DESCRIPTION
* Function decodes the WHO-IS-Router-to-Network request.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*                                                                      
*/
BACNET_RETURN_TYPE WHO_IS_Router_To_Nw_Decode_Handler(
	processInfo_t *pstProcQNode, 
	uint8_t *pu8Npdu, 
	int32_t i32NpduLen)
{
    /* local variables */
    int32_t i32Len = 0;		// Stores the total length of decoded request
	uint16_t u16NwNo = 0;	// Stores the network no
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR; // Function return value
	bool bSendIAmRouter = FALSE;
	DESTINATION_TYPE eDestType = DESTINATION_IS_GLOBAL_BROADCAST;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"NPDU: WHO_IS_Router_To_Nw_Decode_Handler: entry \r\n");
	#endif

    /* check the function inputs */
    if(NULL == pstProcQNode || NULL == pu8Npdu || i32NpduLen < 0)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"NPDU: WHO_IS_Router_To_Nw_Decode_Handler: Inputs are invalid \r\n");
		#endif
        return eRetVal;
	}

	/* check if DNET is present or absent */
    if(0  == i32NpduLen)
	{
		/* check how the request was received */
		if(!pstProcQNode->m_stProcessData.m_stIUTAddr.u16net ||
			g_u16LocalNWNo == pstProcQNode->m_stProcessData.m_stIUTAddr.u16net)
		{
			/* DNET not received in request, send local broadcast */
			bSendIAmRouter = TRUE;
			eDestType = DESTINATION_IS_LOCAL_BROADCAST;
		}
		else
		{
			/* DNET not received in request, send global broadcast */
			bSendIAmRouter = TRUE;
			eDestType = DESTINATION_IS_GLOBAL_BROADCAST;
		}
	}
    else
    {
		/* DNET received in request, decode & validate it */
        /* decode the network number */
	    i32Len += Decode_Unsigned16(&pu8Npdu[i32Len], &u16NwNo);

		/* validate virtual network no */
        if(u16NwNo == g_u16VirtualNWNo)
		{
            /* check how the request was received */
			if(!pstProcQNode->m_stProcessData.m_stIUTAddr.u16net ||
				g_u16LocalNWNo == pstProcQNode->m_stProcessData.m_stIUTAddr.u16net)
			{
				/* DNET received in request, send local broadcast */
				bSendIAmRouter = TRUE;
				eDestType = DESTINATION_IS_LOCAL_BROADCAST;
			}
			else
			{
				/* DNET received in request, send global broadcast */
				bSendIAmRouter = TRUE;
				eDestType = DESTINATION_IS_GLOBAL_BROADCAST;
			}
		}
    }

	/* check the response flag */
	if(bSendIAmRouter)
	{
		/* broadcast I-Am-Router-to-Network */
		Send_I_AM(gstHostDevice.m_pstDeviceStruct, TRUE, eDestType);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"NPDU: WHO_IS_Router_To_Nw_Decode_Handler: exit \r\n");
	#endif
    return eRetVal;
}


/**
*
* DESCRIPTION
* Function decodes the What_Is_Network_no responses.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*
*/
BACNET_RETURN_TYPE What_Is_Nw_No_Decode_Handler(
	processInfo_t *pstProcQNode,
	uint8_t *pu8Npdu,
	int32_t i32NpduLen,
	BACNET_NETWORK_MESSAGE_TYPE eNWMsgType)
{
	/* local variables */
//    int32_t i32Len = 0;
//	uint16_t u16NwNo = 0;
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"NPDU: What_Is_Nw_No_Decode_Handler: entry \r\n");
	#endif

    /* check the function inputs */
    if(NULL == pstProcQNode || NULL == pu8Npdu || i32NpduLen < 0)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"NPDU: What_Is_Nw_No_Decode_Handler: Inputs are invalid \r\n");
		#endif
        return eRetVal;
	}

	/* send the network number only if address do not contain SNET/SADR or DNET/DADR information
	or local broadcast address */
	if(BVLC_ORIGINAL_BROADCAST_NPDU == pstProcQNode->m_stProcessData.m_eBVLCFunctionType &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stRmDvAddr.u8dlen &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stIUTAddr.u8dlen &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stRmDvAddr.u16net &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stIUTAddr.u16net)
	{
		/* As per clause 6.4.19 What-Is-Network-Number, If the What-Is-Network-Number message was broadcast,
		then a non-routing device may optionally wait for up to 10 seconds before sending the Network-Number-Is message.
		If during that time, a different device broadcasts the Network-Number-Is message, the non-routing device may choose to not
		send a Network-Number-Is message */

		// TODO: Add logic to wait for 10 second

		// As 10 second wait logic is optional, currently we are sending Network-Number-Is message without wait
//		Send_NW_NO(&pstProcQNode->m_stProcessData.m_stRmDvAddr, gstHostDevice.m_pstDeviceStruct, TRUE, DESTINATION_IS_GLOBAL_BROADCAST);
	}
	else if(BVLC_ORIGINAL_UNICAST_NPDU == pstProcQNode->m_stProcessData.m_eBVLCFunctionType &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stRmDvAddr.u8dlen &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stIUTAddr.u8dlen &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stRmDvAddr.u16net &&
		BACNET_ZERO == pstProcQNode->m_stProcessData.m_stIUTAddr.u16net)
	{
		// send network number
//		Send_NW_NO(&pstProcQNode->m_stProcessData.m_stRmDvAddr, gstHostDevice.m_pstDeviceStruct, TRUE, DESTINATION_IS_GLOBAL_BROADCAST);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"NPDU: What_Is_Nw_No_Decode_Handler: exit \r\n");
	#endif
    return eRetVal;
}

/**
*
* DESCRIPTION
* Function decodes the Router_Busy_To_Nw request.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*
*/
BACNET_RETURN_TYPE Router_Busy_To_Network_Decode_Handler(
	processInfo_t *pstProcQNode,
	uint8_t *pu8Npdu,
	int32_t i32NpduLen)
{
	/* local variables */
    int32_t i32Len = 0;
	uint16_t u16NwNo = 0;
	IamRouterToNwBindData_t *pstRouterBusyToNw = NULL;
	IamRouterToNwBindData_t *pstRouterBusyNwPrevious = NULL;
	int32_t i32BackupLen = 0;
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"NPDU: Router_Busy_To_Network_Decode_Handler: entry \r\n");
	#endif

	/* check the function inputs */
    if(NULL == pstProcQNode || NULL == pu8Npdu /*|| i32NpduLen <= 0*/)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"NPDU: Router_Busy_To_Network_Decode_Handler: Inputs are invalid \r\n");
		#endif
        return eRetVal;
	}

	/* save backup length */
	i32BackupLen = i32NpduLen;

	do{

		/* decode the network number */
		i32Len += Decode_Unsigned16(&pu8Npdu[i32Len], &u16NwNo);

		/* get the global Router-Busy_To-Nw linklist pointer */
		pstRouterBusyToNw = g_pstRouterBusyToNw;

		/* while */
		while(NULL != pstRouterBusyToNw)
		{
			/* replace the old node data if, network no is same */
			if(u16NwNo == pstRouterBusyToNw->m_u16NetworkNo)
			{
				/* break the loop */
				break;
			}

			/* maintain the previous node */
			pstRouterBusyNwPrevious = pstRouterBusyToNw;
			/* move to next node */
			pstRouterBusyToNw = pstRouterBusyToNw->pstNext;
		}

		/* if this network no was not found in list, save it in new node */
		if(NULL == pstRouterBusyToNw)
		{
			/* allocate memory */
			pstRouterBusyToNw = (IamRouterToNwBindData_t *)OSAL_Malloc(sizeof(IamRouterToNwBindData_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstRouterBusyToNw)
			{
				/* save the network no & ip address of router */
				pstRouterBusyToNw->m_u16NetworkNo = u16NwNo;
				memcpy(&pstRouterBusyToNw->m_stAddress,
					&pstProcQNode->m_stProcessData.m_stRmDvAddr,
					sizeof(BACnetAddress_t));
				/* set next element to NULL */
				pstRouterBusyToNw->pstNext = NULL;
				/* attach this new node to global list */
				if(NULL == pstRouterBusyNwPrevious)
					/* 1st node in list */
					g_pstRouterBusyToNw = pstRouterBusyToNw;
				else
					pstRouterBusyNwPrevious->pstNext = pstRouterBusyToNw;
			}
			else
			{
				/* failed to allocate the memory */
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR,
				"NPDU: Router_Busy_To_Network_Decode_Handler: Memory allocation failed \r\n");
				#endif
				return eRetVal;
			}
		}

		/* decrement backup length by 2, because network no in router busy to
			network should always be 2 bytes long. */
		i32BackupLen -= 2;

	}while(i32Len != i32NpduLen && i32BackupLen > 0);

	/* CB to application on timeout */
	if(NULL != fpAppLayerCbFunction)
	{
		/* RPT timeout */
		fpAppLayerCbFunction(
			gstHostDevice.m_u32DeviceInstace,
			gstHostDevice.m_u32DeviceInstace,
			OBJECT_DEVICE,
			CALLBACK_TYPE_NETWORK_LAYER_SERVICE,
			CALLBACK_REASON_ROUTER_BUSY_TO_NETWORK,
			0,
			g_pstRouterBusyToNw);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"NPDU: Router_Busy_To_Network_Decode_Handler: exit \r\n");
	#endif
    return eRetVal;
}

/**
*
* DESCRIPTION
* Function decodes the Router_Available_To_Nw responses.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*
*/
BACNET_RETURN_TYPE Router_Available_To_Network_Decode_Handler(
	processInfo_t *pstProcQNode,
	uint8_t *pu8Npdu,
	int32_t i32NpduLen)
{
	/* local variables */
    int32_t i32Len = 0;		  // Stores total length of decoded data
	uint16_t u16NwNo = 0;	  // Stores the network no
	IamRouterToNwBindData_t *pstRouterAvailNw = NULL; // structure to save the ip address of router
	IamRouterToNwBindData_t *pstRouterAvailNwPrevious = NULL;
	int32_t i32BackupLen = 0; // backup length to exit do while loop
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR; // Function return value

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"NPDU: Router_Available_To_Network_Decode_Handler: entry \r\n");
	#endif

	/* check the function inputs */
    if(NULL == pstProcQNode || NULL == pu8Npdu /*|| i32NpduLen <= 0*/)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0,BACDEL_INVALID_INPUT_PARAMETER,
		"NPDU: Router_Available_To_Network_Decode_Handler: Inputs are invalid \r\n");
		#endif
        return eRetVal;
	}

	/* save backup length */
	i32BackupLen = i32NpduLen;

	do{
		/* decode the network number */
		i32Len += Decode_Unsigned16(&pu8Npdu[i32Len], &u16NwNo);

		/* get the global Router-Busy_To-Nw linklist pointer */
		pstRouterAvailNw = g_pstRouterAvailNw;

		/* while */
		while(NULL != pstRouterAvailNw)
		{
			/* replace the old node data if, network no is same */
			if(u16NwNo == pstRouterAvailNw->m_u16NetworkNo)
			{
				/* break the loop */
				break;
			}
			/* maintain the previous node */
			pstRouterAvailNwPrevious = pstRouterAvailNw;
			/* move to next node */
			pstRouterAvailNw = pstRouterAvailNw->pstNext;
		}

		/* if this network no was not found in list, save it in new node */
		if(NULL == pstRouterAvailNw)
		{
			/* allocate memory */
			pstRouterAvailNw = (IamRouterToNwBindData_t *)OSAL_Malloc(sizeof(IamRouterToNwBindData_t),
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstRouterAvailNw)
			{
				/* save the network no & ip address of router */
				pstRouterAvailNw->m_u16NetworkNo = u16NwNo;
				memcpy(&pstRouterAvailNw->m_stAddress,
					&pstProcQNode->m_stProcessData.m_stRmDvAddr,
					sizeof(BACnetAddress_t));
                /* save dnet */
                pstRouterAvailNw->m_stAddress.u16net = u16NwNo;
				pstRouterAvailNw->pstNext = NULL;
				/* attach this new node to global list */
				if(NULL == pstRouterAvailNwPrevious)
					/* 1st node in list */
					g_pstRouterAvailNw = pstRouterAvailNw;
				else
					pstRouterAvailNwPrevious->pstNext = pstRouterAvailNw;
			}
			else
			{
				/* failed to allocate the memory */
				#if(defined DEBUG_PRINTF && DL_0)
				Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR,
				"NPDU: Router_Available_To_Network_Decode_Handler: Memory allocation failed \r\n");
				#endif
				return eRetVal;
			}
		}

		/* decrement backup length by 2, because network no in router busy to
			network should always be 2 bytes long. */
		i32BackupLen -= 2;

	}while(i32Len != i32NpduLen && i32BackupLen > 0);

	/* CB to application on timeout */
	if(NULL != fpAppLayerCbFunction)
	{
		/* RPT timeout */
		fpAppLayerCbFunction(
			gstHostDevice.m_u32DeviceInstace,
			gstHostDevice.m_u32DeviceInstace,
			OBJECT_DEVICE,
			CALLBACK_TYPE_NETWORK_LAYER_SERVICE,
			CALLBACK_REASON_ROUTER_AVAILABLE_TO_NETWORK,
			0,
			g_pstRouterAvailNw);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"NPDU: Router_Available_To_Network_Decode_Handler: exit \r\n");
	#endif
    return eRetVal;
}

/**
*
* DESCRIPTION
* Function decodes the Network_No_Is responses.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*
*/
BACNET_RETURN_TYPE Network_Number_Is_Decode_Handler(
	processInfo_t *pstProcQNode,
	uint8_t *pu8Npdu,
	int32_t i32NpduLen)
{
	/* local variables */
	int32_t i32Len = 0;
	uint32_t u32NwType = 0;
	network_no_is_req_t stNWNoIsReq = {0};
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	virtualDevData_t *pVirtualDev = NULL;
	NetworkPort_t *pstNpObj = NULL;
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"NPDU: Network_Number_Is_Decode_Handler: entry \r\n");
	#endif

    /* Check the Host Device Link List */
    pVirtualDev = Find_In_Host_Device_List(SMCfg.g_BACnetCfg.ObjID);

    pstNpObj = (NetworkPort_t *)pVirtualDev->m_stSupportedObjects.m_pstNetworkPort;	/* Find the object */

	/* check the function inputs */
    if(NULL == pstProcQNode || NULL == pu8Npdu || i32NpduLen <= 0)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER,
		"NPDU: Network_Number_Is_Decode_Handler: Inputs are invalid \r\n");
		#endif
        return BACDEL_ERROR;
	}

	/* decode the network number */
	i32Len += Decode_Unsigned16(&pu8Npdu[i32Len], &stNWNoIsReq.m_u16RtrNetNumber);

	/* get the network type configured or leaned */
	i32Len += Decode_Unsigned8(&pu8Npdu[i32Len], &u32NwType);
	/* save network type */
	stNWNoIsReq.m_u8NWType = (uint8_t)u32NwType;

	pstNpObj->m_stNetworkNo.m_u16Val = stNWNoIsReq.m_u16RtrNetNumber;
	if(stNWNoIsReq.m_u8NWType == 1)
		pstNpObj->m_stNetworkNoQuality.m_eNetworkNoQuality = NETWORK_NUMBER_QUALITY_CONFIGURED;
	else if(stNWNoIsReq.m_u8NWType == 0)
		pstNpObj->m_stNetworkNoQuality.m_eNetworkNoQuality = NETWORK_NUMBER_QUALITY_LEARNED;
	/* CB to application on timeout */
	if(NULL != fpAppLayerCbFunction)
	{
		/* RPT timeout */
		fpAppLayerCbFunction(
			gstHostDevice.m_u32DeviceInstace,
			gstHostDevice.m_u32DeviceInstace,
			OBJECT_DEVICE,
			CALLBACK_TYPE_NETWORK_LAYER_SERVICE,
			CALLBACK_REASON_NETWORK_NUMBER_IS,
			0,
			&stNWNoIsReq);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"NPDU: Network_Number_Is_Decode_Handler: exit \r\n");
	#endif
	return eReturnType;
}

/**
*
* DESCRIPTION
* Handles messages at the decode NPDU level of the BACnet stack.
* This API decodes the messages that are coming from BVLC layer
* handler and construct APDU packets for processing by APDU layes handler.
*
* @param pu8PduData			[in] Received PDU containing NPDU and APDU data
* @param u16PduLen			[in] No. of bytes recived
* @param pstNpduProcInfo	[in/out] Pointer to process queue node to save data
* @param pstNpduData		[in] Network layer data.
* @param u16ApduOffset		[in] offset
*
* @return BACNET_RETURN_TYPE [out]  Success or error
*
*/
BACNET_RETURN_TYPE NPDU_NwLayer_Msg_Decode_Handler(
	processInfo_t *pstNpduProcInfo,
    uint8_t *pu8PduData,
    uint16_t u16PduLen,
	Bacnet_Npdu_Data_t *pstNpduData,
	int16_t u16ApduOffset)
{
	/* local varibles*/
	BACNET_RETURN_TYPE eReturnValue = BACDEL_ERROR;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "NPDU: \
	NPDU_NwLayer_Msg_Decode_Handler: Entry \r\n");
	#endif

	/* validate input pointer */
	if(NULL == pstNpduProcInfo || NULL == pu8PduData || NULL == pstNpduData)
	{
		/* null input pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "NPDU: \
		NPDU_Decode_Pdu: Null input pointers \r\n");
		#endif
		return eReturnValue;
	}

	/* I-Am-Router-To-Network */
	if(NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK == pstNpduData->network_message_type)
	{
		/* decode I-Am-Router-To-Network */
		#ifdef NETWORK_LAYER_MESSAGE_A
		I_AM_Router_To_Nw_Decode_Handler(pstNpduProcInfo, &pu8PduData[u16ApduOffset],
			(u16PduLen - u16ApduOffset));
		#endif
		;//dummy statement
	}
	else if(NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK == pstNpduData->network_message_type)
	{
		/* decode Who-Is-Router-To-Network */
		WHO_IS_Router_To_Nw_Decode_Handler(pstNpduProcInfo, &pu8PduData[u16ApduOffset],
			(u16PduLen - u16ApduOffset));
	}
	else if(NETWORK_MESSAGE_ROUTER_BUSY_TO_NETWORK == pstNpduData->network_message_type)
	{
		/* decode ROUTER_BUSY_TO_NETWORK */
		Router_Busy_To_Network_Decode_Handler(pstNpduProcInfo, &pu8PduData[u16ApduOffset],
			(u16PduLen - u16ApduOffset));
	}
	else if(NETWORK_MESSAGE_ROUTER_AVAILABLE_TO_NETWORK == pstNpduData->network_message_type)
	{
		/* decode ROUTER_AVAILABLE_TO_NETWORK */
		Router_Available_To_Network_Decode_Handler(pstNpduProcInfo, &pu8PduData[u16ApduOffset],
			(u16PduLen - u16ApduOffset));
	}
	else if(NETWORK_MESSAGE_WHAT_IS_NETWORK_NUMBER == pstNpduData->network_message_type)
	{
		/* decode What-Is-Network-No */
		What_Is_Nw_No_Decode_Handler(pstNpduProcInfo, &pu8PduData[u16ApduOffset],
			(u16PduLen - u16ApduOffset), pstNpduData->network_message_type);
	}
	else if(NETWORK_MESSAGE_NETWORK_NUMBER_IS == pstNpduData->network_message_type)
	{	/* decode Network-No-Is*/
		Network_Number_Is_Decode_Handler(pstNpduProcInfo, &pu8PduData[u16ApduOffset],
			(u16PduLen - u16ApduOffset));
	}
	else if(pstNpduData->network_message_type > NETWORK_MESSAGE_NETWORK_NUMBER_IS)
	{
		/* unsupported network layer message */
		/* send Reject-Message-To-Network */
        pstNpduProcInfo->m_stProcessData.m_stNPDU.network_layer_message = true;
        pstNpduProcInfo->m_stProcessData.m_stNPDU.u8NwMsgRejectReason =
            NW_MSG_REJECT_REASON_UNKNOWN_MSG;
        pstNpduProcInfo->m_stProcessData.m_stNPDU.network_message_type =
            NETWORK_MESSAGE_REJECT_MESSAGE_TO_NETWORK;
        pstNpduProcInfo->m_stProcessData.m_stIUTAddr.u8dlen = 0;
        /* unsupported network layer message debug log */
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "NPDU: \
		NPDU_Decode_Handler: Discard unknown network layer message \r\n");
		#endif
		/* return success to send response */
        return BACDEL_SUCCESS;
	}

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "NPDU: \
	NPDU_NwLayer_Msg_Decode_Handler: Exit \r\n");
	#endif
	return eReturnValue;
}

#endif /* NETWORK_LAYER_MESSAGE_B */

/*************************** end of file ************************************/
