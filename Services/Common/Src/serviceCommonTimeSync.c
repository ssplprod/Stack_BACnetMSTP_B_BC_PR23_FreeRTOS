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
*   SoftDEL Systems Ltd.                            india@softdel.com      
*   3rd Floor, Pentagon P4,                         http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*   serviceCommonTimeSync.c
*                                                                      
*   AUTHORS                                                                     
*   Pratham M.
*                                                                         
*   DESCRIPTION                                                            
*   The B device interprets time synchronization messages from the A device &
*	shall update its local representation of time. 
*	This change shall be reflected in the Local_Time and Local_Date properties 
*	of the Device object.
*                                                                         
*    
*********************************************************************************/

/* includes header file */
#include "serviceCommonTimeSync.h"
#include "bacDELDef.h"
#include "osalFreeRTOS.h"
#include "pduEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "pduDateTime.h"

#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)
/**
*                                                                    
* DESCRIPTION                                                                          
* This function is default decode handler when a Time Synchronization or 
* UTC Time Synchronization request is received.
*    
* @param pu8ServiceRequest	[in]    Data received for decoding.
* @param i32APDULen			[in]    Length of service request.
* @param pstProcQInfo		[out]	Pointer to the instance of processQ & will 
*									contain the decoded parameters.
*                                   
* @return	-	BACNET_RETURN_TYPE.
*	
*/
BACNET_RETURN_TYPE TimeSync_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
	/* local variables */
	int32_t i32Len = 0;
	BACNET_RETURN_TYPE eReturnType = BACDEL_ERROR;
	#if ((defined BACDEL_SER_DM_TS_B && defined BACDEL_SER_DM_UTC_B) || (defined BACDEL_SER_DM_TS_B))
	time_sync_request_t *pstTimeSyncData = NULL;
	#else
	utc_time_sync_request_t *pstTimeSyncData = NULL;
	#endif
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	TimeSync_B_Decode_Handler: Entry \r\n");
	#endif
    
	/* validate null input pointers */ 
    if(NULL != pstProcQInfo && NULL != pu8ServiceRequest)
    {
		/* Must have minimum service length of 10 */
		if(i32ServiceLen < MINIMUM_SERVICE_DATA_LENGTH_TS) 
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			TimeSync_B_Decode_Handler : Truncated data received \r\n");
			#endif
			return eReturnType;
		}

		/* Allocate buffer for read property data */
		#if ((defined BACDEL_SER_DM_TS_B && defined BACDEL_SER_DM_UTC_B) || (defined BACDEL_SER_DM_TS_B))
		pstTimeSyncData = (time_sync_request_t *)OSAL_Malloc
			(sizeof(time_sync_request_t),  __FILE__, __FUNCTION__, __LINE__);
		#else
		pstTimeSyncData = (utc_time_sync_request_t *)OSAL_Malloc
			(sizeof(utc_time_sync_request_t),  __FILE__, __FUNCTION__, __LINE__);
		#endif
		/* validate null input pointers */ 
        if(NULL == pstTimeSyncData)
        {
			/* malloc failed */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
			TimeSync_B_Decode_Handler : Malloc failed \r\n");
			#endif
            return eReturnType;
        }

		/* START DECODING */

		/* decode date */
		i32Len += Decode_Application_Date(&pu8ServiceRequest[i32Len], 
			&pstTimeSyncData->m_stDate);

		/* decode time */
		i32Len += Decode_Application_Time(&pu8ServiceRequest[i32Len], 
			&pstTimeSyncData->m_stTime);

		/* END OF DECODING */

		/* check for received & decoded data length mismatch */
		if(i32Len != i32ServiceLen)
		{
			/* Free memory for TS service parameters */
			OSAL_Free(pstTimeSyncData,  __FILE__, __FUNCTION__, __LINE__);

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			TimeSync_B_Decode_Handler: Decode Error \r\n");
			#endif
			return eReturnType;
		}

		/* assign allocated memory to service request data */
		pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData 
			= pstTimeSyncData;

		/* Call Object Management Layer interface */
        eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
        if(eReturnType != BACDEL_CONTINUE)
        {
           /* Free memory for TS service parameters */
            if(NULL != pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData)
			{
			    OSAL_Free(pstTimeSyncData,  __FILE__, __FUNCTION__, __LINE__);
			}
			pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			TimeSync_B_Decode_Handler: Obj_MgmtLayer_Interface returns Error \r\n");
			#endif
            return eReturnType;
        }
	}//end of if.
	else
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		TimeSync_B_Decode_Handler: NULL Input Pointers \r\n");
		#endif
        return eReturnType;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	TimeSync_B_Decode_Handler: Exit \r\n");
	#endif
	return eReturnType;
}
#endif /* (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B) */
