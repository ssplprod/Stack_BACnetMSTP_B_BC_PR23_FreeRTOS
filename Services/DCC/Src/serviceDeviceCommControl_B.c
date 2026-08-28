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
*   SoftDEL Systems Ltd.                     		india@softdel.com         
*   3rd Floor, Pentagon P4,							http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
*                                                                         
*   FILE
*	BACnet_DeviceCommunicationControl.c
*                                                                      
*   AUTHORS                                                                             
*	Shruti K.
*                                                                         
*   DESCRIPTION                                                            
*	Defines the fucntions used to implement the device communication control
*   service. 
*
*********************************************************************************/

#ifdef BACDEL_SER_DM_DCC_B

/* header files */
#include "serviceDeviceCommControl.h"
#include "pduEncodeDecode.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"
#include "bacnetDeviceMgmtInterface.h"

/* to include thread pool functionality */
#ifdef THREAD_POOL
#include "bacnetThreadPoolMgmt.h"
#endif

extern osMutexId_t m_hVirtualDevDataMtxLockHandle;

/**
*                                                                    
* DESCRIPTION                                                                          
* This API will decode the DCC service request and depending upon the
* result either generates a error code or SimpleAck.
*    
* @param pu8ServiceRequest   [in]    The contents of service request.
* @param i32ServiceLen       [in]    Length of service request.
* @param pstProcQInfo        [out]   This will contain pointer to data of type
*                                    bacnetRequestData_t.
* @return BACNET_RETURN_TYPE [out]   success or suitable error code.
*                                                                      
*/
BACNET_RETURN_TYPE DCC_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
	/* local variables */
    int32_t i32Len = 0;
    dcc_request_t *pstDCCData = NULL;
    int32_t i32BytesConsumed = 0;
    uint8_t u8TagNumber = 0;
    uint32_t u32LenValue = 0;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	DCC_B_Decode_Handler: entry \r\n");
	#endif
    
    if( pstProcQInfo != NULL && pu8ServiceRequest != NULL )
    {
        /* Allocate buffer for read property data */
        pstDCCData = (dcc_request_t *)OSAL_Malloc( sizeof(dcc_request_t),  
			__FILE__, __FUNCTION__, __LINE__);
        if(pstDCCData == NULL)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:DCC_B_Decode_Handler :"
            "malloc for DCC service parameter failed\r\n");
			#endif

            /* Set Error for this command*/
            Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, ERROR_CODE_OTHER);
            return BACDEL_SUCCESS;
        }
        pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstDCCData;

        /* Decode the Service Request */
        /* Check if frame is truncated */
        if (i32ServiceLen < DCC_MIN_PACKET_LEN)
	    {
			#ifdef DEBUG_PRINTF
		    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			DCC_B_Decode_Handler:i32ServiceLen = \
            %d\t no data present", i32ServiceLen);
			#endif

            /* Update the respective error PDU & set the status flag to 
             * PROC_DONE */
            Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_REJECT, DCC_NULL_VALUE,
                ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER);
            /* To send error response need to return error */
            return BACDEL_SUCCESS;
	    }
	    else
        {
            /* Check what values are present in the request */
            Decode_Tag_Number(&pu8ServiceRequest[i32Len], &u8TagNumber);
            /* Verify it against all the possible tags */
            if(u8TagNumber == DCC_TIME_PERIOD_TAG)
            {
                /* Extract the Time Value */
                i32BytesConsumed = Decode_Context_Unsigned(
                    &pu8ServiceRequest[i32Len], TAG_NO_0,
					&(pstDCCData->m_u32TimeDuration));
                /* If the TAG is improper return with error */
                if(DCC_TAG_ERROR == i32BytesConsumed)
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:DCC_B_Decode_Handler:"
                    "Time Value TAG is not valid\r\n");
					#endif

                    /* Update the respective error PDU & set the status flag to 
                     * PROC_DONE */
                    Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                        BACNET_STATUS_REJECT, DCC_NULL_VALUE,
                        ERROR_CODE_REJECT_INVALID_TAG);
                    /* To send error response need to return error */
                    return BACDEL_SUCCESS;
                }
                else
                {
                    i32Len += i32BytesConsumed;
                }
                /* Check what values are present in the request */
                Decode_Tag_Number(&pu8ServiceRequest[i32Len], &u8TagNumber);
            }
            else
            {
				pstDCCData->m_u32TimeDuration = 0;   
            }
            if(u8TagNumber == DCC_STATUS_TAG)
            {
                /* Extract Device Communication Status */
                i32BytesConsumed = Decode_Context_Enumerated(
                    &pu8ServiceRequest[i32Len], TAG_NO_1,
					(uint32_t *)&(pstDCCData->m_eDccState));
                /* If the TAG is improper return with error */
                if(DCC_TAG_ERROR == i32BytesConsumed)
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: DCC_B_Decode_Handler:"
                    "Device Communication Status TAG is not valid \r\n");
					#endif

                    /* Update the respective error PDU & set the status flag to 
                     * PROC_DONE */
                    Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                        BACNET_STATUS_REJECT, DCC_NULL_VALUE,
                        ERROR_CODE_REJECT_INVALID_TAG);
                    /* To send error response need to return error */
                    return BACDEL_SUCCESS;
                }
                else
                {
                    i32Len += i32BytesConsumed;
                }
            }
            if(i32ServiceLen != i32Len)
            {
                /* Extract Password */
			    i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
				    &u8TagNumber, &u32LenValue);
			    /* If tag is not of type password, return error */
			    if(u8TagNumber != DCC_PASSWORD_TAG)
			    {
					#ifdef DEBUG_PRINTF
				    Print_DebugMsg(DEBUG_LEVEL1,"Password tag u8TagNumber = \
					%d is not valid\r\n", u8TagNumber);
					#endif

				    /* Set Error to send invalid tag */
                    Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
						BACNET_STATUS_REJECT, DCC_NULL_VALUE,
						ERROR_CODE_REJECT_INVALID_TAG);
				    return BACDEL_SUCCESS;
			    }
                else if(u32LenValue > MAX_SUPPORTED_CHRSTR_LEN)
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
				    DCC_B_Decode_Handler : max char string bytes limit reached. \r\n");
					#endif

			        /* Set Error for this command */
			        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
				        BACNET_STATUS_ERROR, ERROR_CLASS_SERVICES, 
                        ERROR_CODE_VALUE_OUT_OF_RANGE);
			        return BACDEL_SUCCESS;
                }
			    /* Decode password */
			    i32BytesConsumed = Decode_Character_String(
                    &pu8ServiceRequest[i32Len], u32LenValue, &pstDCCData->m_stPassword);
                i32Len += i32BytesConsumed;
            }
            
			/* verify if all parameters are decoded i.e. service_length & 
				decoded_length are equal */
			if(i32ServiceLen != i32Len)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:DCC_B_Decode_Handler:"
                "Password is not present\r\n");
				#endif

                /* Update the respective error PDU & set the status flag to 
                 * PROC_DONE */
                Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                    BACNET_STATUS_ERROR, ERROR_CLASS_SERVICES,
                    ERROR_CODE_INCONSISTENT_PARAMETERS);
                /* To send error response need to return error */
                return BACDEL_SUCCESS;
            }

            /* Call Object Management Layer interface */
            eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
            if(BACDEL_CONTINUE != eReturnType)
            {
                /* Update the respective error PDU & set the status flag to 
                 * PROC_DONE */
                Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                    BACNET_STATUS_ABORT, DCC_NULL_VALUE,
                    ERROR_CODE_ABORT_OUT_OF_RESOURCES);

				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: DCC_B_Decode_Handler: \
				unknown device \r\n");
				#endif
                /* To send error response need to return error */
                return BACDEL_SUCCESS;
            }
        }
    }
    else
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:DCC_B_Decode_Handler :"
        "NULL Input Pointers\r\n");
		#endif
        return BACDEL_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	DCC_B_Decode_Handler: exit \r\n");
	#endif
    return eReturnType;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function will validate the received password against the
* stored password.
*    
* @param m_stPassword [in] Received Password.
* @return TRUE / FALSE
*                                                                      
*/
BOOL DCC_B_Validate_Password(
	virtualDevData_t *pstVirtualDev,
	BACnetCharStr_t *pstPassword)
{
	/* local variables */
    bool bRetVal = FALSE;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DCC_B_Validate_Password: Entry \r\n");
	#endif
	
	if(pstVirtualDev->m_stDvPassword.m_u32StrLen == 0)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: DCC_B_Validate_Password: \
		password not required. \r\n");
		#endif
        return TRUE;
	}

	if(pstPassword->m_u32StrLen == pstVirtualDev->m_stDvPassword.m_u32StrLen)
    {
		if(!Strncmp(pstPassword->m_pu8CharStr, pstVirtualDev->m_stDvPassword.m_pu8CharStr,
            pstPassword->m_u32StrLen))
        {
            bRetVal = TRUE;
        }     
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DCC_B_Validate_Password: Exit \r\n");
	#endif
    return bRetVal;
}

/**
*
* DESCRIPTION
* Parse the request received & based on the status return Error packet
* or a simple acknowledge.
*
* @param pstVirtualDev [in] Pointer to Virtual Device struct.
* @param pstProcQInfo  [in] process Q data 
* @param u8ThreadNo    [in] thread number
*
* @return TRUE/FALSE 
*
*/
BOOL DCC_B_Request_Parser(
	virtualDevData_t *pstVirtualDev, 
	processInfo_t * pstProcQInfo,
	uint8_t u8ThreadNo)
{
	/* local variables */
    dcc_request_t *pstDCCData = NULL;
	dcc_request_t stTempDCCReq = {0};
	BACnetAddress_t stRmtDvAddr = {0};

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DCC_B_Request_Parser: entry \r\n");
	#endif

	/* check input parameters */
	if(NULL == pstProcQInfo || NULL == pstVirtualDev)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: DCC_B_Request_Parser: \
		null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* Default vaue for DCC-B response*/
	pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_SIMPLE_ACK;
    
    /* Extract the request received */
    pstDCCData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
    /* If NULL pointer is returned */
    if(NULL == pstDCCData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:DCC_B_Request_Parser :"
        "NULL pstDCCData value\r\n");
		#endif

        /* Set Error for this command*/
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, ERROR_CODE_OTHER);
        /* To send error response need to return error */
        return TRUE;
    }

    /* validate password length, it should not be more than MAX_PASSWORD_CHAR_BYTES */
	if(pstDCCData->m_stPassword.m_u32StrLen > MAX_PASSWORD_CHAR_BYTES)
    {
        /* To send error response that character set not supported */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		DCC_B_Request_Parser: Password length exceeded. \r\n");
		#endif
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_PARAMETER_OUT_OF_RANGE);
		return TRUE;
    }

    /* validate character encoding for password */
	if(Validate_Charstring_Encoding(pstDCCData->m_stPassword.m_u8Encoding,
    		pstDCCData->m_stPassword.m_u16CodePage))
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:DCC_B_Request_Parser:"
        "Character encoding not supported. \r\n");
		#endif

        /* Update the respective error PDU & set the status flag to 
         * PROC_DONE */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ERROR, ERROR_CLASS_SERVICES,
            ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED);
        /* To send error response need to return error */
        return TRUE;
    }

    /* Validate the password for further processing */
    if(!DCC_B_Validate_Password(pstVirtualDev, &pstDCCData->m_stPassword))
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:DCC_B_Request_Parser:"
        "Password is not valid\r\n");
		#endif

        /* Update the respective error PDU & set the status flag to 
         * PROC_DONE */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ERROR, ERROR_CLASS_SECURITY,
            ERROR_CODE_PASSWORD_FAILURE);
        /* To send error response need to return error */
        return TRUE;
    }

	#ifdef BACDEL_PR23
	/* validate DccState, state should not be disabled value  */
	if(pstDCCData->m_eDccState == COMMUNICATION_DISABLE)
	{
		/* To send error response that disable state not supported */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_PASSWORD_ERROR, "APDU: \
		DCC_B_Request_Parser: state should not be disable . \r\n");
		#endif
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR,
			ERROR_CLASS_SERVICES, ERROR_CODE_SERVICE_REQUEST_DENIED);
		return TRUE;
	}
	#endif

	if(STATUS_OPERATIONAL != 
		pstVirtualDev->m_stDevObject.m_stSystemStatus.m_eDeviceStatus)
	{
		/* send error that device is busy */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:DCC_B_Request_Parser:"
        "device is busy in back & restore. \r\n");
		#endif

		/* Update the respective error PDU & set the status flag to 
		 * PROC_DONE */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
			BACNET_STATUS_ERROR, ERROR_CLASS_DEVICE, ERROR_CODE_DEVICE_BUSY);
		/* To send error response need to return error */
		return TRUE;
	}

	/* copy dcc data and remote dev addr to local variable */
	memcpy(&stTempDCCReq, pstDCCData, sizeof(dcc_request_t));
	memcpy(&stRmtDvAddr, &pstProcQInfo->m_stProcessData.m_stRmDvAddr,
		sizeof(BACnetAddress_t));

	/* free allocated servcie data */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
	OSAL_Free(pstDCCData, __FILE__,__FUNCTION__,__LINE__);
		

	/* Update the value of DCC in Device Structure */
	DCC_B_Update_State_And_Inform_Application(pstVirtualDev, &stTempDCCReq, 
				&stRmtDvAddr);

	/* save dcc time duration */
	pstVirtualDev->m_u32DevCommLifeTime = (stTempDCCReq.m_u32TimeDuration * 60);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DCC_B_Request_Parser: exit \r\n");
	#endif
    return TRUE;
}

/**
*
* DESCRIPTION
* Provides the current device communication status of the requested device.
*
* @param BACnetAddress_t				[in] Bacnet address.
* @return BACNET_COMMUNICATION_STATE    [out] Value indicates if the current state is
*											  ENABLE/DISABLE/DISABLE_INITIATION.
*
*/
BACNET_COMMUNICATION_STATE DCC_B_Get_Status(
	BACnetAddress_t *pstBACnetAddress)
{
	/* local variables */
	BACNET_COMMUNICATION_STATE eRetVal = COMMUNICATION_ENABLE;
	BACnet_Device_Struct_t *pstDevice = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:" \
	"DCC_B_Get_Status: entry \r\n");
	#endif

	#ifndef SUPPORT_MULTIPLE_DEVICE
    pstDevice = &gstHostDevice;
    /* Traverse the Host Device Link List */
    while(NULL != pstDevice)
    {
        if(0 == memcmp(&pstDevice->m_pstDeviceStruct->m_u8DvSADR[0], 
			&pstBACnetAddress->u8DvDadr[0], BACNET_DADR_LEN))
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
			DCC_B_Get_Status: host device found \r\n");
			#endif

            eRetVal = pstDevice->m_pstDeviceStruct->m_eDevCommStatus;
			break;
        }
        /* Move to next host device object */
        pstDevice = pstDevice->pstNextDevice;
    }
	#else
	/* return DCC status of pure IP device */
    eRetVal = gstHostDevice.m_pstDeviceStruct->m_eDevCommStatus;
	#endif

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DCC_B_Get_Status: exit \r\n");
	#endif
    return eRetVal;
}

/**
*
* Description
* Api to update the device communication state & then call the callback 
* function to notify application about changed state 
*
* @param pstInDCCReq	[in] dcc request data
* @param pstRmtDevAddr  [in] remote device address
* @param pstVirtualDev  [in] virtual device pointer
*
* @returns void
*
*/
void DCC_B_Update_State_And_Inform_Application(
	virtualDevData_t *pstVirtualDev, 
	dcc_request_t *pstInDCCReq,
	BACnetAddress_t *pstRmtDevAddr)
{
    /* local variables */
    BACNET_COMMUNICATION_STATE eCommState = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    DCC_B_Update_State_And_Inform_Application : entry \r\n");
	#endif

    /* get the DCC state */
	eCommState = pstInDCCReq->m_eDccState;

    /* input validation */
    if(NULL == pstVirtualDev || pstInDCCReq->m_eDccState >= MAX_BACNET_COMMUNICATION_STATE)
        return;

    /* lock device communication mutex */
    Osal_Wait_Mutex(m_hVirtualDevDataMtxLockHandle, INFINITE);

    /* check if same value */
    if(eCommState == pstVirtualDev->m_eDevCommStatus)
    {
    	;
    }

    /* update device DCC state */
    pstVirtualDev->m_eDevCommStatus = eCommState;

    /* Unlock Mutex */
    Osal_Release_Mutex(m_hVirtualDevDataMtxLockHandle);

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    DCC_B_Update_State_And_Inform_Application : exit \r\n");
	#endif
    return;
}

#endif /* BACDEL_SER_DM_DCC_B */
