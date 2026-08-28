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
*   SoftDEL Systems Ltd.                                india@softdel.com      
*   3rd Floor, Pentagon P4,                             http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*   serviceReinitializeDevice_B.c
*                                                                      
*   AUTHORS                                                                     
*   Pratham N. Murkute
*                                                                         
*   DESCRIPTION                                                            
*	The B device performs reinitialization requests from the A device. 
*	The optional password field shall be supported.
*	Devices claiming conformance to DM-RD-B are only required to support 
*	the WARMSTART and COLDSTART service choices.
*                                                                         
*    
*********************************************************************************/

#ifdef BACDEL_SER_DM_RD_B

/* header fiels */
#include "serviceReinitializeDevice_B.h"
#include "bacDELDef.h"
#include "pduEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "miscMiscellaneous.h"
#include "bacDELDeviceConfig.h"
#include "Led.h"

/* to include network port object functionality */
#if (defined BACDEL_PR18 && defined BACDEL_OBJ_NP)
/* to include BBMD layer functions */
#endif


/* to include dcc functionality */
#ifdef BACDEL_SER_DM_DCC_B
#include "serviceDeviceCommControl.h"
#endif
/*include MSTP layer functions */

#include "datalinkMSTP.h"

extern DB_t SMCfg;


/**
*                                                                    
* DESCRIPTION                                                                          
* This function is default handler when a Reinitialize Device request is
* received.
*    
* @param pu8ServiceRequest	[in]    Data received for decoding.
* @param i32ServiceLen		[in]    Length of service request.
* @param pstProcQInfo		[out]	Pointer to the instance of processQ & will 
*									contain the decoded parameters.
*                                   
* @return	-	BACNET_RETURN_TYPE.
*	
**/
BACNET_RETURN_TYPE RD_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
	/* local variables */
	reinit_device_request_t *pstReinitDvData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	int32_t i32Len = 0;
	uint8_t u8TagNumber = 0;
	uint32_t u32LenValue = 0;
	uint32_t u32ReinitState = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RD_B_Decode_Handler: entry \r\n");
	#endif
	
	/* null check for input parameters */
    if(NULL == pstProcQInfo || NULL == pu8ServiceRequest)
    {
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RD_B_Decode_Handler: null input pointers \r\n");
		#endif
        return BACDEL_ERROR;
	}

    /* Allocate memory for RD data */
    pstReinitDvData = OSAL_Malloc( sizeof(reinit_device_request_t), 
						__FILE__, __FUNCTION__, __LINE__);
    if(NULL == pstReinitDvData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		RD_B_Decode_Handler: pstReinitDvData malloc failed \r\n");
		#endif

		/* Set Error for this command */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY);
        return BACDEL_SUCCESS;
    }

	/* fill below pointer with the address of pstReinitDvData structure */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstReinitDvData;

	/* Must have at least 12 tags, 13 if message text is also sent */
	if(i32ServiceLen < MINIMUM_DATA_FOR_REINIT_DEVICE_SERVICE)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TRUNCATED_DATA_FRAME, "APDU: \
		RD_B_Decode_Handler : Truncated data received \r\n");
		#endif

		/* Set Error for this command */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_MISSING_REQUIRED_PARAMETER);
		return BACDEL_SUCCESS;
	}

	/* START DECODING THE DATA */

	/* Extract Device Reinitialization State */
	i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
		&u8TagNumber, &u32LenValue);
	/* If tag is not of type Device Reinitialization State, return error */
	if(u8TagNumber != TAG_NO_0)
	{
		#if(defined DEBUG_PRINTF && DL_1)
        Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
		RD_B_Decode_Handler : reinit state tag is invalid \r\n");
		#endif

		/* Set Error to send invalid tag */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_REJECT, 
			BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
		return BACDEL_SUCCESS;
	}
	/* Decode event type of the notifier */
	i32Len += Decode_Enumerated(&pu8ServiceRequest[i32Len], u32LenValue,
		&u32ReinitState);
	pstReinitDvData->m_eDevReinitState = (BACNET_REINITIALIZED_STATE)u32ReinitState;

	if(i32Len != i32ServiceLen)
	{
		/* Extract Password */
		i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
			&u8TagNumber, &u32LenValue);
		/* If tag is not of type password, return error */
		if(u8TagNumber != TAG_NO_1)
		{
			#if(defined DEBUG_PRINTF && DL_1)
            Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			RD_B_Decode_Handler : password tag is invalid \r\n");
			#endif

			/* Set Error to send invalid tag */
			Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_REJECT, 
				BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
			return BACDEL_SUCCESS;
		}
        else if(u32LenValue > MAX_SUPPORTED_CHRSTR_LEN)
        {
			#if(defined DEBUG_PRINTF && DL_1)
            Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
			RD_B_Decode_Handler : max char string bytes limit reached. \r\n");
			#endif

		    /* Set Error for this command */
		    Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			    BACNET_STATUS_ERROR, ERROR_CLASS_SERVICES, ERROR_CODE_VALUE_OUT_OF_RANGE);
		    return BACDEL_SUCCESS;
        }
		/* Decode password */
		i32Len += Decode_Character_String(&pu8ServiceRequest[i32Len], u32LenValue,
			&pstReinitDvData->m_stPassword);
	}

	/* END OF DECODING */

	/* check for received & decoded data length mismatch */
	if(i32Len != i32ServiceLen)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DECODING_FAILED, "APDU: \
		RD_B_Decode_Handler: Decode Error \r\n");
		#endif

		/* Set Error to send inconsistent parameter */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_INCONSISTENT_PARAMETERS);
		return BACDEL_SUCCESS;
	}

	/* Call Object Management Layer interface */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
    if(BACDEL_CONTINUE != eReturnType)
    {
		/* Set Error to send device process queue was full */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ABORT, 
			BACNET_ZERO, ERROR_CODE_ABORT_OUT_OF_RESOURCES);

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		RD_B_Decode_Handler: Obj_MgmtLayer_Interface returns Other \r\n");
		#endif
        return BACDEL_SUCCESS;
    }

	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RD_B_Decode_Handler: exit \r\n");
	#endif
    return eReturnType;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to validate the Reinit Device Password.
*    
* @param pstPassword	[in] password received in request.
* @param pVirtualDev    [in] virtual device pointer.
*                                   
* @return - true if password matches else false.
*	
**/
static bool RD_B_Validate_Dv_Password(
	virtualDevData_t *pVirtualDev,
	BACnetCharStr_t *pstPassword)
{
	/* local variables */
	bool bReturnValue = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RD_B_Validate_Dv_Password: entry \r\n");
	#endif

	/* check for password length */
	if(0 == pVirtualDev->m_stDvPassword.m_u32StrLen)
	{
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
		RD_B_Validate_Dv_Password: password not required. \r\n");
		#endif
		return TRUE;
	}

	if(pstPassword->m_u32StrLen == pVirtualDev->m_stDvPassword.m_u32StrLen)
	{
		if(!Strncmp(pstPassword->m_pu8CharStr, pVirtualDev->m_stDvPassword.m_pu8CharStr,
			pVirtualDev->m_stDvPassword.m_u32StrLen))
		{
			/* Password matches, return true */
			bReturnValue = TRUE;
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RD_B_Validate_Dv_Password: exit \r\n");
	#endif
	return bReturnValue;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to validate the Reinit Device state received.
*    
* @param eReInitState [in]  reinitialize state.
* @param pVirtualDev  [in]  virtual device pointer.
* @param pstProcQInfo [in]  process Q data.
* @param peErrClass   [out] error class.
* @param peErrCode    [out] error code.
*                                   
* @return - true if password matches else false.
*	
**/
bool RD_B_Validate_Received_ReInit_State(
	BACNET_REINITIALIZED_STATE eReInitState, 
	virtualDevData_t *pVirtualDev,
	BACNET_ERROR_CLASS *peErrClass, 
	BACNET_ERROR_CODE *peErrCode,
	processInfo_t *pstProcQInfo)
{
	/* local variables */
	bool bReturnValue = FALSE;
	bool bRequestDenied = FALSE;
	bool bConfigInProgress = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RD_B_Validate_Received_ReInit_State: entry \r\n");
	#endif

	/* null check for input pointers */
	if(NULL == pstProcQInfo || NULL == pVirtualDev)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RD_B_Validate_Received_ReInit_State: null input pointers. \r\n");
		#endif

		*peErrClass = ERROR_CLASS_SERVICES;
		*peErrCode = ERROR_CODE_INTERNAL_ERROR;
		return FALSE;
	}

	/* stitch case as per reinit state */
	switch(eReInitState)
	{
	case BACNET_REINIT_COLDSTART:
	case BACNET_REINIT_WARMSTART:
	#ifdef BACDEL_PR18
	case BACNET_REINIT_ACTIVATE_CHANGES:
	#endif
	{
		#ifdef BACDEL_SER_DM_BR_B
		/* coldstart and warmstart */
		/* return true if backup state is idle / backup-failure or restore-failure */
		if(BACKUP_STATE_IDLE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState ||
		   BACKUP_STATE_BACKUP_FAILURE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState ||
		   BACKUP_STATE_RESTORE_FAILURE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bReturnValue = TRUE;
		}
		else
		{
			bConfigInProgress = TRUE;
		}
		#else
		/* coldstart and warmstart, return true always as BR not supported */
		bReturnValue = TRUE;
		#endif
	}
	break;

	#ifndef BACDEL_SER_DM_BR_B
	default:
	{
		/* other reinit device states needs to be claimed by backup & 
			restore service.
			refer clause 16.4 */

		/* send error response that service request denied */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_REQUEST_DENIED, "APDU: \
		RD_B_Validate_Received_ReInit_State: Request Denied. \r\n");
		#endif

		*peErrClass = ERROR_CLASS_SERVICES;
		*peErrCode = ERROR_CODE_OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED;
		//return FALSE;
	}
	break;
	#else

	case BACNET_REINIT_STARTBACKUP:
	{
		/* start backup, return true if backup state is idle */
		if(BACKUP_STATE_IDLE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bReturnValue = TRUE;
		}
		else
		{
			bConfigInProgress = TRUE;
		}
	}
	break;

	case BACNET_REINIT_ENDBACKUP:
	{
		/* end backup, return true if following */
		if(BACKUP_STATE_PREPARING_FOR_BACKUP == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState ||
		   BACKUP_STATE_PERFORMING_A_BACKUP == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState || 
		   BACKUP_STATE_BACKUP_FAILURE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bReturnValue = TRUE;
		}
		else if(BACKUP_STATE_IDLE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bRequestDenied = TRUE;
		}
		else
		{
			bConfigInProgress = TRUE;
		}
	}
	break;

	case BACNET_REINIT_STARTRESTORE:
	{
		/* start restore, return true if backup state is idle */
		if(BACKUP_STATE_IDLE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bReturnValue = TRUE;
		}
		else
		{
			bConfigInProgress = TRUE;
		}
	}
	break;

	case BACNET_REINIT_ENDRESTORE:
	case BACNET_REINIT_ABORTRESTORE:
	{
		/* end or abort restore, return true if following */
		if(BACKUP_STATE_PREPARING_FOR_RESTORE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState ||
		   BACKUP_STATE_PERFORMING_A_RESTORE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState || 
		   BACKUP_STATE_RESTORE_FAILURE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bReturnValue = TRUE;
		}
		else if(BACKUP_STATE_IDLE == pVirtualDev->m_stDevObject.
			m_stBackupRestoreState.m_eBackupState)
		{
			bRequestDenied = TRUE;
		}
		else
		{
			bConfigInProgress = TRUE;
		}
	}
	break;

	default:
		break;
	#endif /* BACDEL_SER_DM_BR_B */
	}//switch ends

	/* if request for backup-restore procedure, send error if communication is disabled */
	if(BACNET_REINIT_COLDSTART != eReInitState && 
		BACNET_REINIT_WARMSTART != eReInitState
		#ifdef BACDEL_PR18
		&& BACNET_REINIT_ACTIVATE_CHANGES != eReInitState
		#endif
		)
	{
		#ifdef BACDEL_SER_DM_DCC_B
		if(DCC_B_Get_Status(&pstProcQInfo->m_stProcessData.m_stIUTAddr) == COMMUNICATION_DISABLE)
		{
			/* To send error response that communication is disabled */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DCC_DISABLED, "APDU: \
			RD_B_Validate_Received_ReInit_State: Communication Disabled. \r\n");
			#endif

			*peErrClass = ERROR_CLASS_SERVICES;
			*peErrCode = ERROR_CODE_COMMUNICATION_DISABLED;
			return FALSE;
		}
		#endif
	}

	if(bConfigInProgress)
	{
		/* send error response that device Configuration in progress already. */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_CONFIGURATION_IN_PROGRESS, "APDU: \
		RD_B_Validate_Received_ReInit_State: Configuration in progress. \r\n");
		#endif

		*peErrClass = ERROR_CLASS_DEVICE;
		*peErrCode = ERROR_CODE_CONFIGURATION_IN_PROGRESS;
		bReturnValue = FALSE;
	}
	else if(bRequestDenied)
	{
		/* send error response that invalid request */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_REQUEST_DENIED, "APDU: \
		RD_B_Validate_Received_ReInit_State: Configuration in progress. \r\n");
		#endif

		*peErrClass = ERROR_CLASS_DEVICE;
		*peErrCode = ERROR_CODE_SERVICE_REQUEST_DENIED;
		bReturnValue = FALSE;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RD_B_Validate_Received_ReInit_State: exit \r\n");
	#endif
	return bReturnValue;
}

#if (defined BACDEL_PR18 && defined BACDEL_OBJ_NP)
/**
* 
* DESCRIPTION
* This function activate changes pending in network port object
* when reinitialize device with ACTIVATE_CHANGES is received.  
* 
* @param pVirtualDev [in] virtual device data. 
* @returns void.
* 
*/
void RD_B_Activate_Changes(virtualDevData_t *pVirtualDev)
{
	/* local variables */
	NetworkPort_t *pstNetPort = NULL;
	NpChangesPending_t *pstChngPending = NULL;
	void *pvPropAdd = NULL;
	bool bChangesPending = false;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, 
		"APDU: RD_B_Activate_Changes: Entry \r\n");
	#endif

	/* null check for input pointers */
    if(NULL == pVirtualDev)
    {
		/* invalid pointer */
		return;
	}

	/* get the base address of network port object */
	pstNetPort =  pVirtualDev->m_stSupportedObjects.m_pstNetworkPort;
	if(NULL == pstNetPort)
	{
		/* invalid pointer */
		return;
	}

	/* get the address of changes pending data */
	pstChngPending = &pstNetPort->m_stNpChangesPending;

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btNetworkNo)
	{
		/* set bit false */
		pstChngPending->m_btNetworkNo = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btLinkSpeed)
	{
		/* set bit false */
		pstChngPending->m_btLinkSpeed = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btMacAddress)
	{
		/* set bit false */
		pstChngPending->m_btMacAddress = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btIPMode)
	{
		/* set bit false */
		pstChngPending->m_btIPMode = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btIPAddress)
	{
		/* set bit false */
		pstChngPending->m_btIPAddress = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btIPUdpPort)
	{
		/* set bit false */
		pstChngPending->m_btIPUdpPort = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btIPSubnetMask)
	{
		/* set bit false */
		pstChngPending->m_btIPSubnetMask = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btIPDefaultGateway)
	{
		/* set bit false */
		pstChngPending->m_btIPDefaultGateway = FALSE;	
	}

	#ifdef BBMD_ENABLED
	/* check if bit is true */
	if(TRUE == pstChngPending->m_btBbmdBdt)
	{
		/* updated actual bdt list */
		NetworkPort_Activate_BDT_Changes(pVirtualDev);
		/* set bit false */
		pstChngPending->m_btBbmdBdt = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btBbmdAcceptFD)
	{
		/* set bit false */
		pstChngPending->m_btBbmdAcceptFD = FALSE;	
	}

	/* check if bit is true */
	if(TRUE == pstChngPending->m_btFdBbmdAddress || TRUE == pstChngPending->m_btFDSubsLifetime)
	{
		/* check if ip mode is normal or fd, then only send register fd */
		if(IP_MODE_NORMAL == pstNetPort->m_stIPMode.m_eIpMode ||
			IP_MODE_FOREIGN == pstNetPort->m_stIPMode.m_eIpMode)
		{
			/* check if host type is address */
			if(BACNET_DT_OCTETSTRING == pstNetPort->m_stFDBbmdAddr.m_stHostNPort.
				m_stHostAddress.m_eDataType)
			{
				uint8_t u8IpAddrs[4]= {0};
				/* send register fd request to specified bbmd device */
				// FIXME: this may fail if max registrations are already sent
				u8IpAddrs[3]=pstNetPort->m_stFDBbmdAddr.m_stHostNPort.
						m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[0];
				u8IpAddrs[2]=pstNetPort->m_stFDBbmdAddr.m_stHostNPort.
						m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[1];
				u8IpAddrs[1]=pstNetPort->m_stFDBbmdAddr.m_stHostNPort.
						m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[2];
				u8IpAddrs[0]=pstNetPort->m_stFDBbmdAddr.m_stHostNPort.
						m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[3];
			}
		}
		/* set bit false */
		pstChngPending->m_btFdBbmdAddress = FALSE;	
		pstChngPending->m_btFDSubsLifetime = FALSE;	
	}
	#endif /* bbmd */

	if(TRUE==pstChngPending->m_btMaxMaster)
	{
		/* updated actual bdt list */
		pstNetPort = pVirtualDev->m_stSupportedObjects.m_pstNetworkPort;
		pstNetPort ->m_stMAXMaster.m_u32Val = pstNetPort->m_stMAXMasterBackup.m_u32Val;
		pVirtualDev->m_stDevObject.m_stMAXMaster.m_u32Val = pstNetPort->m_stMAXMasterBackup.m_u32Val;
		g_stMstpVariables.MaxMaster = pstNetPort->m_stMAXMasterBackup.m_u32Val;

		/* set bit false */
		pstChngPending->m_btMaxMaster = FALSE;

	}

	/* update changes pending, set to false */
	NetworkPort_Write_Property(PROP_CHANGES_PENDING, -1, FALSE, FALSE, 0, 
		(void *)pVirtualDev, (void *)&bChangesPending, READ_ONLY, 
		(void *)pstNetPort, &pvPropAdd, FALSE);

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, 
		"APDU: RD_B_Activate_Changes: Exit \r\n");
	#endif
    return;
}
#endif /* PR18 && NP object */

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is to validate received reinitialize device request & do 
* necessary actions after successful validation.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pstProcQInfo	[in]	Pointer to the instance of processQ that 
*								contains the decoded parameters.
* @param u8ThreadNo     [in]    thread number.
*                                   
* @return	-	returns true or false.
*	
**/
bool RD_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pstProcQInfo,
	uint8_t u8ThreadNo)
{
	/* local variables */
	reinit_device_request_t *pstReinitDvData = NULL;
//	reinit_device_request_t *pstTempRDData = NULL;
	BACnetAddress_t stRmtDvAddr = {0};   
	bool bReturnVal = TRUE;
	uint32_t u32ErrCode = 0;
	BACNET_ERROR_CLASS eErrClass = MAX_BACNET_ERROR_CLASS;
    BACNET_REINITIALIZED_STATE eReInitState = BACNET_REINIT_IDLE;
	#ifdef SUPPORT_MULTIPLE_DEVICE
	Post_Thread_Msg_t stThreadMsg = {0};
	#endif

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RD_B_Request_Parser: entry \r\n");
	#endif

	/* null check for input parameters */
	if(NULL == pstProcQInfo || NULL == pVirtualDev)
	{
		/* This should not occur ideally */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RD_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* default pdu value for RD-B response */
	pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_SIMPLE_ACK;

	/* get RD-B service data */
	pstReinitDvData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	if(NULL == pstReinitDvData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RD_B_Request_Parser: Null service data pointer \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
				ERROR_CLASS_RESOURCES, ERROR_CODE_OTHER);
		return bReturnVal;
	}

	/* validate received reinit device state */
	if(!RD_B_Validate_Received_ReInit_State(pstReinitDvData->m_eDevReinitState, 
		pVirtualDev, &eErrClass, (BACNET_ERROR_CODE *)&u32ErrCode, pstProcQInfo))
	{
		/* To send error response that password was invalid */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
		RD_B_Request_Parser: ReInit state validation fails \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_ERROR, eErrClass, u32ErrCode);
		return bReturnVal;
	}

    /* validate password length, it should not be more than MAX_PASSWORD_CHAR_BYTES */
	if(pstReinitDvData->m_stPassword.m_u32StrLen > MAX_PASSWORD_CHAR_BYTES)
    {
        /* To send error response that character set not supported */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
		RD_B_Request_Parser: Password length exceeded. \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_PARAMETER_OUT_OF_RANGE);
		return bReturnVal;
    }

    /* validate character encoding of password */
	if(Validate_Charstring_Encoding(pstReinitDvData->m_stPassword.m_u8Encoding,
		pstReinitDvData->m_stPassword.m_u16CodePage))
    {
        /* To send error response that character set not supported */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_CHARACTER_SET_NOT_SUPPORTED, "APDU: \
		RD_B_Request_Parser: Character set not supported. \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SERVICES, ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED);
		return bReturnVal;
    }

	/* validate the received password */
	if(!RD_B_Validate_Dv_Password(pVirtualDev, &pstReinitDvData->m_stPassword))
	{
		/* To send error response that password was invalid */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_PASSWORD_ERROR, "APDU: \
		RD_B_Request_Parser: Password Mismatch. \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR, 
			ERROR_CLASS_SECURITY, ERROR_CODE_PASSWORD_FAILURE);
		return bReturnVal;
	}

    /* save reinit state received */
	eReInitState = pstReinitDvData->m_eDevReinitState;
	/* save address of remote device */
	memcpy(&stRmtDvAddr, &pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
		sizeof(BACnetAddress_t));

	/* make state of processQ location to done to send simple ack */
	pstProcQInfo->m_eState = PROC_DONE;

	//if(SMCfg.g_ui8ComType==ect_BACnet_TP/*BACnetConfigData.m_eBACStack == BACNET_STACK_MSTP*/)
	{
		/* set the flag to notify master tsm to stop reply postpone */
		b_ResponseSentOnNetwork = TRUE;
	}

	#ifndef SUPPORT_MULTIPLE_DEVICE  //KV
    /* Increment the semaphore of Tx thread to send simple ack */
    if (!OSAL_Release_Sem(g_hTxSemaphoreHandle, BACNET_ONE))
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_SEMAPHORE_ERROR, "APDU: \
		RD_B_Request_Parser: OSAL_Release_Sem Failed \r\n");
		#endif

        /* Call the clean up */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
			BACNET_STATUS_ERROR, ERROR_CLASS_DEVICE, ERROR_CODE_OPERATIONAL_PROBLEM);
		return bReturnVal;
    }
//    Osal_Sleep(100);

	#else
	/* Post the message to the message queue */
	stThreadMsg.idThread = gstHostDevice.m_pstDeviceStruct->m_dwTransmitThreadID;
	stThreadMsg.MsgType = MSGQ_TYPE;
	stThreadMsg.wParam = (WPARAM)0;
	stThreadMsg.lParam = (LPARAM)pstProcQInfo;
	if(!OSAL_Post_Message(&stThreadMsg))
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MESSAGE_QUEUE_ERROR,
		"APDU: RD_B_Request_Parser: message queue post error \r\n");
		#endif
		/* Call the clean up */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
			BACNET_STATUS_ERROR, ERROR_CLASS_DEVICE, ERROR_CODE_OPERATIONAL_PROBLEM);
		return bReturnVal;
	}
	#endif /* */

	/* make return value to false, to avoid trigerring Tx thread again */
	bReturnVal = FALSE;	

	/* perform task as per ReInt state */
	switch(eReInitState)
	{
	case BACNET_REINIT_COLDSTART:
    case BACNET_REINIT_WARMSTART:
	{
		/* set RD status */
		pVirtualDev->m_eReInitDvStatus = eReInitState;
		#if (CALLBACK_BEFORE_EXECUTION != CALLBACK_CONFIG_REINITIALIZE_DEVICE)
		/* Reinitialize the device. */
        ReInitializeDevice(pVirtualDev, (eReInitState == BACNET_REINIT_COLDSTART) ? 
			RESTART_REASON_COLDSTART : RESTART_REASON_WARMSTART);
		#else
		/* No action to be taken in stack */
		#endif
		#ifdef BACDEL_PR18
		if(BACNET_REINIT_WARMSTART == eReInitState)
		{
			/* activate changes that are pending */
			RD_B_Activate_Changes(pVirtualDev);
		}
		#endif
	}
	break;

	#ifdef BACDEL_PR18 
	case BACNET_REINIT_ACTIVATE_CHANGES:
	{
		/* set RD status */
		pVirtualDev->m_eReInitDvStatus = eReInitState;
		/* activate changes that are pending */
		RD_B_Activate_Changes(pVirtualDev);
	}
	break; 
	#endif /* */

	default:
	{
		/* invalid reinit state */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
		RD_B_Request_Parser: ReInit Device state received is invalid. \r\n");
		#endif
	}
	break;
	}//switch ends

	/* function exit */ 
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RD_B_Request_Parser: exit \r\n");
	#endif 
	return bReturnVal; 
} 

/**
*                                                                    
* DESCRIPTION                                                                          
* This function re initializes the device.
* the code in this function can be implemented as per
* client requirement.  
*    
* @param pVirtualDev [in] virtual device data. 
* @param eRestartReason [in] retsart reason.
*
* @returns void.
*	
**/
void ReInitializeDevice(virtualDevData_t *pVirtualDev, BACNET_RESTART_REASON eRestartReason)
{
    /* local variables */
	dcc_request_t stDCCReq = {0};
	BACnetAddress_t stRmtDevAddr = {0};
	uint8_t u8iLoop = 0;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
    ReInitializeDevice: entry \r\n");
	#endif

	/* null check for input pointers */
    if(NULL != pVirtualDev)
    {
        /* debug message .. */
		#if(defined DEBUG_PRINTF && DL_2)
        Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
        ReInitializeDevice: Re-Initializing the device %d .\r\n", 
        pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId);
		#endif

        /* reset timeouts of dcc & backup_restore */
        pVirtualDev->m_u32DevCommLifeTime = 0;
        pVirtualDev->m_u16BackupFailureTime = 0;
        pVirtualDev->m_u16BackupPreparationTime = 0;
        pVirtualDev->m_u16RestoreCompletionTime = 0;
        pVirtualDev->m_u16RestorePreparationTime = 0;

        /* reset device date & time to system default */
        /* reset date_time differences */
        pVirtualDev->m_i32Days_Diff = 0;
        pVirtualDev->m_i32Sec_Diff = 0;
        pVirtualDev->m_fTimeChange = 0;

        /* Set the default device password */
        pVirtualDev->m_stDvPassword.m_u8Encoding = g_eCharEncoding;
        pVirtualDev->m_stDvPassword.m_u32StrLen = BACnetConfigData.m_stPassword.m_u32StrLen;
        memcpy(pVirtualDev->m_stDvPassword.m_pu8CharStr, BACnetConfigData.m_stPassword.m_pu8CharStr,
        		BACnetConfigData.m_stPassword.m_u32StrLen);

		#ifdef BACDEL_SER_DM_DCC_B
        /* make dcc enable */
		stDCCReq.m_eDccState = COMMUNICATION_ENABLE;
        DCC_B_Update_State_And_Inform_Application(pVirtualDev, &stDCCReq, &stRmtDevAddr);
		#endif

        /* make sys status operational */
        #if (defined BACDEL_SER_DM_BR_B && defined BACDEL_OBJ_FILE)
        ;//do nothing
		#else
		{
			/* local variables */
			BACNET_CONF_DATA stWpData = {0};
			BACNET_DEVICE_STATUS eSysState = STATUS_OPERATIONAL;
			void *pvData = NULL;
			/* update property */
			stWpData.eObjectProperty = PROP_SYSTEM_STATUS;
			pvData = &eSysState;
			Dv_Write_Prop(&stWpData, pVirtualDev, pvData, READ_ONLY);
		}
		#endif

		#ifdef BACDEL_SER_DM_DDB_A
        /* send who is */
//        Send_WHO_IS(pVirtualDev, -1, -1, false, 0, DESTINATION_IS_GLOBAL_BROADCAST);
//        Osal_Sleep(100);
		#endif

		#ifdef BACDEL_SER_DM_DDB_B
        /* send i am */
//        Send_I_AM(pVirtualDev, false, DESTINATION_IS_GLOBAL_BROADCAST);
//        BSP_Set_ALL_LED_Off();
//        HAL_NVIC_SystemReset();
//        BSP_Set_ALL_LED_On();
//        Osal_Sleep(100);
		#endif

		for(u8iLoop = 0; u8iLoop < 3;)
		{
//			BSP_Set_L5_LED_G_on();
//			Osal_Sleep(1000);
//			BSP_Set_L5_LED_G_off();
//			Osal_Sleep(1000);
			u8iLoop++;
		}

    }

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
    ReInitializeDevice: exit \r\n");
	#endif
    return;
}

#endif /* BACDEL_SER_DM_RD_B */
