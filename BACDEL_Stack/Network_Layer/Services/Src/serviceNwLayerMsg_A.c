/*************************************************************************************
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
*	serviceNwLayerMsg_A.c
*
*	AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION
*	This file contains function handlers for all A-side network layer 
*	protocol services.
*
*   RELEASE HISTORY
*   DATE            NAME				DESCRIPTION
*	18-Feb-2014		Pratham N. M.		File creation.
*	18-Feb-2014		Pratham N. M.		I-AM-Router-To-Nw decoder.
*
*
*************************************************************************************/

#ifdef NETWORK_LAYER_MESSAGE_A

/** OS specific includes */
#include "osalFreeRTOS.h"

/** include header files */
#include "serviceNwLayerMsg_A.h"
#include "pduEncodeDecode.h"


/** Global linklist to save I-Am-Router-To-Network data */
IamRouterToNwBindData_t *g_pstIamRouterToNw = NULL;


/**
*
* DESCRIPTION
* Function decodes the I-AM-Router-To-Nw responses.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*                                                                      
*/
BACNET_RETURN_TYPE I_AM_Router_To_Nw_Decode_Handler(
	processInfo_t *pstProcQNode, 
	uint8_t *pu8Npdu, 
	int32_t i32NpduLen)
{
	/* local variables */
    int32_t i32Len = 0;		  // Stores total length of decoded data
	uint16_t u16NwNo = 0;	  // Stores the network no
	IamRouterToNwBindData_t *pstIAMRouter = NULL; // structure to save the ip address of router
	IamRouterToNwBindData_t *pstIAMPrevious = NULL;
	int32_t i32BackupLen = 0; // backup length to exit do while loop
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR; // Function return value

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	I_AM_Router_To_Nw_Decode_Handler: entry \r\n");
	#endif

	/* check the function inputs */
    if (NULL == pstProcQNode || NULL == pu8Npdu || i32NpduLen <= 0)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		I_AM_Router_To_Nw_Decode_Handler: Inputs are invalid \r\n");
		#endif
        return eRetVal;
	}

	/* save backup length */
	i32BackupLen = i32NpduLen;

	do{

		/* get the global I-Am-Router-To-Nw linklist pointer */
		pstIAMRouter = g_pstIamRouterToNw;

		/* decode the network number */
		i32Len += Decode_Unsigned16(&pu8Npdu[i32Len], &u16NwNo);

		/* while */
		while(NULL != pstIAMRouter)
		{
			/* replace the old node data if, network no is same */
			if(u16NwNo == pstIAMRouter->m_u16NetworkNo)
			{
				/* replace the old ip address */
				memcpy(&pstIAMRouter->m_stAddress, 
					&pstProcQNode->m_stProcessData.m_stRmDvAddr,
					sizeof(BACnetAddress_t));
                /* save dnet */
                pstIAMRouter->m_stAddress.u16net = u16NwNo;
				/* break the loop */
				break;
			}
			/* maintain the previous node */
			pstIAMPrevious = pstIAMRouter;
			/* move to next node */
			pstIAMRouter = pstIAMRouter->pstNext;
		}

		/* If this network no was not foun in list, save it in new node */
		if(NULL == pstIAMRouter)
		{
			/* allocate memory */
			pstIAMRouter = (IamRouterToNwBindData_t *)OSAL_Malloc(sizeof(IamRouterToNwBindData_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL != pstIAMRouter)
			{
				/* save the network no & ip address of router */
				pstIAMRouter->m_u16NetworkNo = u16NwNo;
				memcpy(&pstIAMRouter->m_stAddress, 
					&pstProcQNode->m_stProcessData.m_stRmDvAddr,
					sizeof(BACnetAddress_t));
                /* save dnet */
                pstIAMRouter->m_stAddress.u16net = u16NwNo;
				pstIAMRouter->pstNext = NULL;
				/* attach this new node to global list */
				if(NULL == pstIAMPrevious)
					/* 1st node in list */
					g_pstIamRouterToNw = pstIAMRouter;
				else 
					pstIAMPrevious->pstNext = pstIAMRouter;
			}
			else
			{
				/* failed to allocate the memory */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
				I_AM_Router_To_Nw_Decode_Handler: Memory allocation failed \r\n");
				#endif
				return eRetVal;
			}
		}

		/* decrement backup length by 2, because network no in i am router to
			network should always be 2 bytes long. */
		i32BackupLen -= 2;

	}while(i32Len != i32NpduLen && i32BackupLen > 0);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	I_AM_Router_To_Nw_Decode_Handler: exit \r\n");
	#endif
    return eRetVal; 
}


#endif /* NETWORK_LAYER_MESSAGE_A */

/*************************** end of file ************************************/
