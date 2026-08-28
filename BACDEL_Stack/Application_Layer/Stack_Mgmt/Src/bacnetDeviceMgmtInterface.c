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
*	bacnetDeviceMgmtInterface.c
*                                                                      
*   AUTHORS                                                                     
*	Surendra Oturkar, Harshal Mangale, M.Venu, Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	Functions for passing request data to individual devices created within stack.
*	Pure IP device actsas the virtual router to send the request data to virtual 
*	devices.
*	In case a broadcast message is received, the data is passed to all devices 
*	within the stack.
*
**********************************************************************************/

/** header files */
#include "osalFreeRTOS.h"
#include "bacnetDeviceMgmtInterface.h"
#include "miscMiscellaneous.h"
#include "objDevice.h"

/* DCC-B service */
#ifdef BACDEL_SER_DM_DCC_B
#include "serviceDeviceCommControl.h"
#endif

/* external global variables */
extern BACnet_Device_Struct_t gstHostDevice;
extern uint16_t g_u16VirtualNWNo;
extern uint16_t g_u16LocalNWNo;
extern osSemaphoreId_t m_hDeviceSemaphoreHandle;


/**
*
* DESCRIPTION
* Function to fill request data and trigger respective device process thread.
* If device process queue has empty location, address of received process Q node
* is saved in device process queue.
* If VD are supported in stack & requestis broadcast, address of newly allocated
* memory for process Q node is saved in device process queue.
*
* @param bIsBroadcastReq [in] Specifies if unicast or broadcast request
* @param pvVirtualDev	 [in] Virtual device data
* @param pstReqProcInfo  [in] Pointer to Px Q node
*
* @return [out] BACNET_RETURN_TYPE enumeration
* - BACDEL_SUCCESS means send the response immediately
* - BACDEL_ERROR means discard the packet 
* - BACDEL_CONTINUE means data is sent to device process thread
* - BACDEL_OTHER means no empty space in queue
*
*/
BACNET_RETURN_TYPE Fill_Device_Queue(
	bool bIsBroadcastReq,
    void *pvVirtualDev, 
    processInfo_t *pstReqProcInfo)
{
	/* local variables */
    virtualDevData_t *pstVirtualDeviceData = NULL; 

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Fill_Device_Queue: entry \r\n");
	#endif

	/* get VD pointer */
    pstVirtualDeviceData = (virtualDevData_t *)pvVirtualDev;
	/* check input pointer */
    if(NULL == pstVirtualDeviceData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Fill_Device_Queue: Null input VD pointer \r\n");
		#endif
        return BACDEL_ERROR;
    }

    /* check if device process queue has space for this new request */
    if(NULL == pstVirtualDeviceData->m_pstDeviceProcessQueue
		[pstVirtualDeviceData->m_i32QFillIndex])
    {
        {
            /* save the pointer of process queue node address in device process queue */
            pstVirtualDeviceData->m_pstDeviceProcessQueue
                [pstVirtualDeviceData->m_i32QFillIndex] = pstReqProcInfo;

            /* update the state of process queue node */
            pstVirtualDeviceData->m_pstDeviceProcessQueue
                [pstVirtualDeviceData->m_i32QFillIndex]->m_eState = PROC_CONTINUE;
        }
    }
    else
    {
		/* no space available in device process queue */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: Fill_Device_Queue: \
		Device process queue has no space \r\n");
		#endif
        return BACDEL_OTHER;
    }

	/* trigger device process thread */
    /* release the device process thread's semaphore */
    if(!OSAL_Release_Sem(m_hDeviceSemaphoreHandle, BACNET_ONE))
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Fill_Device_Queue: \
        Device Px thread semaphore release error \r\n");
		#endif
        /* call the clean up function */
		// FIXME - check if all memories are getting freed 
        Exit_Routine_To_Error_Of_Unconfirmed_Request(pstReqProcInfo);
        return BACDEL_SEMAPHORE_ERROR;
    }
    
    /* increment device process queue's fill index to next position */
    pstVirtualDeviceData->m_i32QFillIndex++;
    if(pstVirtualDeviceData->m_i32QFillIndex == MAX_PACKET)
	{
		/* set to zero */
        pstVirtualDeviceData->m_i32QFillIndex = 0;
	}

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Fill_Device_Queue: exit \r\n");
	#endif
    return BACDEL_CONTINUE;
}

/**
*
* DESCRIPTION
* Process Thread uses this Interface. 
* Process thread will call this interface to pass received request data 
* to respective device thread.
*                                                                             
* @param pstReqProcInfo [in] Received request data.
* @return [out] BACDEL_SUCCESS on success else error
*
*/
BACNET_RETURN_TYPE Obj_MgmtLayer_Interface(processInfo_t *pstReqProcInfo)
{
	/* local variables */
    BACnet_Device_Struct_t *pstHostDevice = NULL;
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Obj_MgmtLayer_Interface: entry \r\n");
	#endif

    /* get pointer to host device link-list */
    pstHostDevice = &gstHostDevice;

    /* check if first host device has valid device object */
	/* check pointers */
    if(NULL == pstHostDevice->m_pstDeviceStruct || NULL == pstReqProcInfo)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Obj_MgmtLayer_Interface: Null pointers \r\n");
		#endif
        return BACDEL_DEVICE_NOT_PRESENT;
    }

	/* send request to pure-ip device */
	eRetVal = Fill_Device_Queue(FALSE, pstHostDevice->m_pstDeviceStruct, pstReqProcInfo);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Obj_MgmtLayer_Interface: exit \r\n");
	#endif
    return eRetVal;
}


/**
*
* DESCRIPTION
* This function is called by NPDU handler routine to check if
* the requested DADR is valid or not.
*                                                                             
* @param pstReqProcInfo [in] Px queue node containing DADR
* @param bDADRPresent   [in] Flag specifies if DADR is present or not
*
* @return 
* true - if destination DADR is presnet in virtual device table.
* false - if destination DADR is absent.
*
*/
bool Device_Look_Up(processInfo_t *pstReqProcInfo, bool bDADRPresent)
{
	/* local variables */
	bool bRetVal = FALSE;
    BACnet_Device_Struct_t *pstHostDevice = NULL;
    Bacnet_Npdu_Data_t *pstReqNPDU = NULL;
    BACnetAddress_t *pstReqIUTAddr = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Device_Look_Up: entry \r\n");
	#endif

    /* get pointer to host device link-list */
    pstHostDevice = &gstHostDevice;

	/* get pointer to npdu data & self device address */
    pstReqNPDU = &pstReqProcInfo->m_stProcessData.m_stNPDU;
    pstReqIUTAddr = &pstReqProcInfo->m_stProcessData.m_stIUTAddr;
    
    /* check if first host device has valid device object */
    if(NULL == pstHostDevice->m_pstDeviceStruct)
    {
		/* device not found */
        bRetVal = FALSE;
    }
    else
    {
        /* if request is for pure-ip device or local ip n/w broadcast */
        if(pstReqNPDU->m_bDestPresent == false ||
           ((pstReqIUTAddr->u16net == g_u16LocalNWNo || 
             pstReqIUTAddr->u16net == BACNET_LOCAL_BROADCAST_NETWORK_NO) && bDADRPresent == false))
        {
			/* save pure-ip device pointer */
            pstReqProcInfo->m_pvReqDevStruct = (void *)gstHostDevice.m_pstDeviceStruct;
            return TRUE;
        }
		#ifndef SUPPORT_MULTIPLE_DEVICE
		else if(pstReqIUTAddr->u16net == BACNET_GLOBAL_BROADCAST_NETWORK_NO)
		{
			/* if not supporting virtual devices, pass global broadcast to ip device */
			/* save pure-ip device pointer */
			pstReqProcInfo->m_pvReqDevStruct = (void *)gstHostDevice.m_pstDeviceStruct;
            return TRUE;
		}
		#endif

        /* if request is global broadcast or virtual n/w broadcast */
        if(pstReqIUTAddr->u16net == BACNET_GLOBAL_BROADCAST_NETWORK_NO ||
            (pstReqIUTAddr->u16net == g_u16VirtualNWNo && bDADRPresent == false))
        {
			/* save no pointer, as this is broadcast */
            pstReqProcInfo->m_pvReqDevStruct = NULL;
            return TRUE;
        }
        /* if unicast request for virtual device */
        else
        {
            /* traverse the virtual device link list to find VD */
            while(NULL != pstHostDevice)
            {
                /* if device has valid device object */
                if(NULL != pstHostDevice->m_pstDeviceStruct)
                {
					/* check is address matches */
                    if(0 == memcmp(&pstHostDevice->m_pstDeviceStruct->m_u8DvSADR[0], 
                        &pstReqProcInfo->m_stProcessData.m_stIUTAddr.u8DvDadr[0], BACNET_DADR_LEN)) 
                    {
						/* save virtual device pointer */
                        pstReqProcInfo->m_pvReqDevStruct = (void *)pstHostDevice->m_pstDeviceStruct;
                        return TRUE;
                    }
                }

                /* move to next host device */
                pstHostDevice = pstHostDevice->pstNextDevice;
            }
        }

    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Device_Look_Up: exit \r\n");
	#endif
    return bRetVal;
}

/************************** end of bacnetDeviceMgmtInterface.c file ***************************/
