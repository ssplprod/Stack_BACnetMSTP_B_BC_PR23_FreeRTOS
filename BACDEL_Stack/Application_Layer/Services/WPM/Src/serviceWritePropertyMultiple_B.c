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
*   SoftDEL Systems Ltd.                             india@softdel.com         
*   3rd Floor, Pentagon P4,                          http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name
*	serviceWritePropertyMultiple_B.c
*
*   AUTHORS
*   Harshal Mangale, M. Venu
*       
*   DESCRIPTION
*   The WritePropertyMultiple service is used by a client BACnet-user 
*   to modify the value of one or more specified properties of a 
*	BACnet object.
*	This files includes functions for Write Property Multiple service.
*    
******************************************************************************/                                           

#ifdef BACDEL_SER_DS_WPM_B

/* header files */
#include "osalFreeRTOS.h"
#include "miscMiscellaneous.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "propertyValueWrite.h"
#include "serviceWritePropertyMultiple_B.h"
/**
*
* DESCREPTION
* Function to decode WPM-B request received from network.
*
* @param pu8ServiceRequest  [in]  Data to be decoded.
* @param i32ServiceLen      [in]  Received data length.
* @param pstProcQInfo      [out]  Pointer to save WPM-B data
*
* @return [out] success or error
*
*/
BACNET_RETURN_TYPE WPM_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
	/* local variables */
    int32_t i32Len = 0;
    int32_t i32DecodeLen = 0;
    uint32_t u32LenValueType = 0;
    uint32_t u32UnsignedValue = 0;
    Bacnet_Multiple_Data_t *pstFirstWpmData = NULL;
    Bacnet_Multiple_Data_t *pstWpmData = NULL;
    Bacnet_Multiple_Data_t *pstTmpWpmData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WPM_B_Decode_Handler: entry \r\n");
	#endif

    /* Check command is valid */
    if( pstProcQInfo != NULL && pu8ServiceRequest != NULL && 
						i32ServiceLen != 0)
    {
        /* decode apdu request */
	    for (;;) 
	    {
            /* Allocate buffer for read property multiple service data */
            /* Memory is assigned for each new Object ID present in Request */ 
            pstTmpWpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t),  __FILE__, __FUNCTION__, __LINE__);
            if( pstTmpWpmData == NULL )
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
				WPM_B_Decode_Handler: Malloc failed for WPM_B param \r\n");
				#endif
                
                /* Free Malloc Memmory */
                Free_Service_Memory(pstFirstWpmData);

                /* Set Error for this command*/
                Service_Error_Handler( BACNET_STATUS_ERROR, 
									ERROR_CLASS_RESOURCES,
                                    ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo,
									TRUE);

                return BACDEL_SUCCESS;
            }
            memset(pstTmpWpmData, 0, sizeof(Bacnet_Multiple_Data_t));

             /* First time assign pointer to "service_request_data" */
             /* for next ObjId create link list */
            if(pstFirstWpmData == NULL)
            {
                pstFirstWpmData = pstTmpWpmData;
            }
            else
            {
                pstWpmData->m_pstMDNext = pstTmpWpmData;
            }
            /* Pointer to memory to store decoded Parameters*/
            pstWpmData = pstTmpWpmData;

            /* "Fill_ObjId" Flag set to "1" indicate New Obj_Id*/
            pstWpmData->m_stServiceData.u8FillObjId = 1;


            /* Start by looking for an object ID */
            i32Len = Decode_MPS_object_id(&pu8ServiceRequest[i32DecodeLen],
                                i32ServiceLen - i32DecodeLen, pstWpmData);
            if (i32Len < 0)
            {
                /* bad encoding - skip to error/reject/abort handling */
                /* Set Error for this command*/
                Service_Error_Handler( i32Len, pstWpmData->m_stServiceData.eErrorClass,
                    pstWpmData->m_stServiceData.eErrorCode, pstProcQInfo, TRUE);
                
                /* Free Malloc Memmory*/
                Free_Service_Memory(pstFirstWpmData);

				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
				WPM_B_Decode_Handler: invalid object id \r\n");
				#endif

                return BACDEL_SUCCESS;
            }
            i32DecodeLen += i32Len;

            /* do each property of this object of the RPM request */
            for (;;) 
		    {
                /* Fetch a property identifier and Array Index */
            	i32Len =
                	Decode_MPS_property_ArrIdx(&pu8ServiceRequest[i32DecodeLen],
                    i32ServiceLen - i32DecodeLen, pstWpmData);
                if (i32Len < 0)
                {
                    /* bad encoding - skip to error/reject/abort handling */
                     /* Set Error for this command */
                    Service_Error_Handler( i32Len, pstWpmData->m_stServiceData.eErrorClass,
                        pstWpmData->m_stServiceData.eErrorCode, pstProcQInfo, TRUE);
                     /* Free Malloc Memmory */
                    Free_Service_Memory(pstFirstWpmData);

					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
					WPM_B_Decode_Handler: invalid property identifier \r\n");
					#endif

                    return BACDEL_SUCCESS;
                }
                i32DecodeLen += i32Len;



                /* Tag_2: opening context tag for Property Value */
                if(Decode_Is_Opening_Tag_Number(&pu8ServiceRequest[i32DecodeLen], TAG_NO_2))
                {
                    /* determine the length of the data block */
                    pstWpmData->m_stServiceData.i32ApplicationDataLen =
                        BACApp_Verify_Data_Len(&pu8ServiceRequest[i32DecodeLen], 
						i32ServiceLen - i32DecodeLen,
                        pstWpmData->m_stServiceData.eObjectProperty);
                    if( BACNET_STATUS_ERROR == pstWpmData->m_stServiceData.i32ApplicationDataLen)
                    {
                        /* bad encoding -skip to error/reject/abort handling */
                         /* Set Error for this command */
                        Service_Error_Handler( BACNET_STATUS_REJECT, 0,
                            ERROR_CODE_REJECT_INVALID_TAG, pstProcQInfo, TRUE);
                         /* Free Malloc Memmory */
                        Free_Service_Memory(pstFirstWpmData);

						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
						WPM_B_Decode_Handler: invalid tag \r\n");
						#endif

                        return BACDEL_SUCCESS;
                    }
                    /* a tag number of 3 is not extended so only
					one octet increment*/
                    i32DecodeLen++;
                    
                    pstWpmData->m_stServiceData.pu8PropValueBuffer = 
                        (uint8_t*) OSAL_Malloc( (pstWpmData->m_stServiceData.i32ApplicationDataLen+1),
                         __FILE__, __FUNCTION__, __LINE__);

                    if(NULL == pstWpmData->m_stServiceData.pu8PropValueBuffer)
                    {
                        Service_Error_Handler( BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                            ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo, TRUE);
                         /* Free Malloc Memmory */
                        Free_Service_Memory(pstFirstWpmData);

						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
						WPM_B_Decode_Handler: invalid tag \r\n");
						#endif

                        return BACDEL_SUCCESS;
                    }

                    memcpy(pstWpmData->m_stServiceData.pu8PropValueBuffer, 
                        &pu8ServiceRequest[i32DecodeLen], 
						pstWpmData->m_stServiceData.i32ApplicationDataLen);

                    /* add on the data length */
                    i32DecodeLen += pstWpmData->m_stServiceData.i32ApplicationDataLen;
                    
                    if (i32DecodeLen && Decode_Is_Closing_Tag_Number(&pu8ServiceRequest[i32DecodeLen], TAG_NO_2)) 
                    {
                        i32DecodeLen++;
                    }
                    else
                    {
                        /* bad encoding - skip to error/reject/abort handling */
                         /* Set Error for this command */
                        Service_Error_Handler( BACNET_STATUS_REJECT, 0,
                            ERROR_CODE_REJECT_INVALID_TAG, pstProcQInfo, TRUE);
                         /* Free Malloc Memmory */
                        Free_Service_Memory(pstFirstWpmData);

						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
						WPM_B_Decode_Handler: invalid tag \r\n");
						#endif

                        return BACDEL_SUCCESS;
                    }
                }
                else
                {
                    /* bad encoding - skip to error/reject/abort handling */
                     /* Set Error for this command */
                    Service_Error_Handler( BACNET_STATUS_REJECT, 0,
                        ERROR_CODE_REJECT_INVALID_TAG, pstProcQInfo, TRUE);
                     /* Free Malloc Memmory */
                    Free_Service_Memory(pstFirstWpmData);

					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
					WPM_B_Decode_Handler: invalid tag \r\n");
					#endif

                    return BACDEL_SUCCESS;
                }


                /* Tag 3: Optional Priority */
                pstWpmData->m_stServiceData.u8Priority = BACNET_MAX_PRIORITY;

                if(Decode_Context_Tag(&pu8ServiceRequest[i32DecodeLen], TAG_NO_3, &u32LenValueType))
                {
                    i32DecodeLen++ ;
                    i32DecodeLen += Decode_Unsigned(&pu8ServiceRequest[i32DecodeLen], u32LenValueType,
                                                &u32UnsignedValue);
                    if ((u32UnsignedValue >= BACNET_MIN_PRIORITY) &&
                        (u32UnsignedValue <= BACNET_MAX_PRIORITY)) 
                    {
                        pstWpmData->m_stServiceData.u8Priority = 
							(uint8_t) u32UnsignedValue;
                    }
                    else
                    {
                         /* Set Error for this command */
                        Service_Error_Handler( BACNET_STATUS_REJECT, 0,
                            ERROR_CODE_REJECT_PARAMETER_OUT_OF_RANGE,
							pstProcQInfo, TRUE);
                         /* Free Malloc Memmory */
                        Free_Service_Memory(pstFirstWpmData);

						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
						WPM_B_Decode_Handler: parameter out of range \r\n");
						#endif

                        return BACDEL_SUCCESS;
                    }
                }

                /* Check End of property list is reached or not */
                if (Decode_Is_Closing_Tag_Number(&pu8ServiceRequest[i32DecodeLen], TAG_NO_1)) 
                {
                    /* Reached end of property list so cap the result list */
                    i32DecodeLen++;
                    break;  /* finished with this property list */
                } 
                else 
                {
                     /* Continue decoding Property identifier from command */
                     /* malloc new RPMData memory and store decoded param in it */
                    pstTmpWpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t),  __FILE__, __FUNCTION__, __LINE__);
                    if( pstTmpWpmData == NULL )
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:RP_B_Request_Handler : Malloc for WPM-B param failed\r\n");
						#endif

                         /* Free Malloc Memmory */
                        Free_Service_Memory(pstFirstWpmData);

                        /* Set Error for this command*/
                        Service_Error_Handler( BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                            ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo, TRUE);

                        return BACDEL_SUCCESS;
                    }

                    memset(pstTmpWpmData, 0, sizeof(Bacnet_Multiple_Data_t));

                    /*Get Object identifier from previous property parameters*/
                    pstTmpWpmData->m_stServiceData.eObjectType = 
						pstWpmData->m_stServiceData.eObjectType;
                    pstTmpWpmData->m_stServiceData.u32ObjectInstance = 
						pstWpmData->m_stServiceData.u32ObjectInstance;
                     /* Flag set to "0" indicate previous Obj_Id */
                    pstTmpWpmData->m_stServiceData.u8FillObjId = 0;
                    
                    pstWpmData->m_pstMDNext = pstTmpWpmData;
                    pstWpmData = pstTmpWpmData;

                } 

            }/* End of Inner for loop */

            if (i32DecodeLen >= i32ServiceLen) 
            {
                /* Reached the end so finish up */

                /* Assign pointer of Decoded RPM Data to service_request_data*/
                pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = 
					pstFirstWpmData;

                /* Call Object Management Layer interface */
                eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
                if( eReturnType != BACDEL_CONTINUE )
                {
                    /* Free Malloc Memmory */
                    Free_Service_Memory(pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData);
                    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
					/* Set Error for this command*/
                    Service_Error_Handler( BACNET_STATUS_ABORT, 0,
                        ERROR_CODE_ABORT_OUT_OF_RESOURCES, pstProcQInfo, TRUE);

					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
					WPM_B_Decode_Handler: Obj_MgmtLayer returned error \r\n");
					#endif

                    return BACDEL_SUCCESS;
                }
                break;
            }
        }/* End of Outer for Loop */
    } 
    else if(NULL != pstProcQInfo)
    {
        /* Required parameters are missing */
        pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_REJECT;

        pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
                        REJECT_REASON_MISSING_REQUIRED_PARAMETER;
        
        /* FIXME: Free Malloc Memmory */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WPM_B_Decode_Handler: parameters missing \r\n");
		#endif

        return BACDEL_SUCCESS;
    } 
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WPM_B_Decode_Handler: exit \r\n");
	#endif
    return eReturnType;
}

/**
*
* DESCRIPTION
* Function to encode and send WPM error ack.
*    
* @param pu8APDUResp   [in]  Buffer to save encoded data
* @param i32APDULen	   [in]  Encoded data length.
* @param pstProcQInfo  [in]  WPM service data
*
* @return [out] total encoded data length
*                                                                      
*/
int32_t WPM_B_Ack_Encoder(                    
	processInfo_t *pstProcQInfo,
	uint8_t *pu8APDUResp,
	int32_t i32APDULen)
{
	/* local variables */
    Bacnet_Multiple_Data_t *pstWpmData = NULL;
	Bacnet_Multiple_Data_t stWpmData = {0};
    int32_t i32EncodeLen = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WPM_B_Ack_Encoder: entry \r\n");
	#endif

	/* get current encoded data length */
    i32EncodeLen = i32APDULen;

	/* get pointer to WPM data */
    pstWpmData = (Bacnet_Multiple_Data_t *)pstProcQInfo->m_stProcessData.
		m_stAPDU.m_pvServiceRequestData;

	/* check pointer */
    if(NULL == pstWpmData )
    {
		/* encode error response, assign local pointer */
		pstWpmData = &stWpmData;
		/* save error class and error code */
		stWpmData.m_stServiceData.bErrorStatus = TRUE;
		stWpmData.m_stServiceData.eErrorClass = pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass;
		stWpmData.m_stServiceData.eErrorCode = pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode;
    }

    /* check for error while writing any property value */
    while(NULL != pstWpmData)
    {
        /* check if property failed while writing */
        if(pstWpmData->m_stServiceData.bErrorStatus)
        {
			/* encode error pdu */
            pu8APDUResp[0] = PDU_TYPE_ERROR;
            pu8APDUResp[1] = pstProcQInfo->m_stProcessData.
				m_stAPDU.m_u8InvokeId;
            pu8APDUResp[2] = pstProcQInfo->m_stProcessData.
				m_stAPDU.m_u8ServiceChoice;
            i32EncodeLen = 3;
            
            /* Encode Opening Tag for Error */ 
            i32EncodeLen += Encode_Opening_Tag(
				&pu8APDUResp[i32EncodeLen], TAG_NO_0);

            /* Encode Error Class */ 
            i32EncodeLen += Encode_Application_Enumerated(
				&pu8APDUResp[i32EncodeLen],
                pstWpmData->m_stServiceData.eErrorClass);

            /* Encode Error Code */ 
            i32EncodeLen += Encode_Application_Enumerated(
				&pu8APDUResp[i32EncodeLen], 
                pstWpmData->m_stServiceData.eErrorCode);

            /* Encode the application closing tag */
            i32EncodeLen += Encode_Closing_Tag(
				&pu8APDUResp[i32EncodeLen], TAG_NO_0);


            /* Encode Opening Tag for First Failed Write Attempt */ 
            i32EncodeLen += Encode_Opening_Tag(
				&pu8APDUResp[i32EncodeLen], TAG_NO_1);

            /* Encode Obj_ID, PropID, PropIndex, AppOpening _Tag */
            i32EncodeLen += Encode_ObjId_Prop_ArrIdx(
				&pu8APDUResp[i32EncodeLen], 
                &pstWpmData->m_stServiceData);

            /* Encode the application closing tag */
            i32EncodeLen += Encode_Closing_Tag(
				&pu8APDUResp[i32EncodeLen], TAG_NO_1);
            break;
        }

		/* move to next node */
        pstWpmData = pstWpmData->m_pstMDNext;
    }

     /* Free memory allocated for WPM-B service */
    if(NULL != pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData)
	{
		/* */
        Free_Service_Memory(pstProcQInfo->m_stProcessData.
			m_stAPDU.m_pvServiceRequestData);
	}
	/* set pointer to NULL */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
    
	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WPM_B_Ack_Encoder: exit \r\n");
	#endif
    return i32EncodeLen;
}

/**
*
* DESCRIPTION
* Function to validate and execute WPM-B service.
*
* @param pstVirtualDev [in] Virtual device data.
* @param pstProcQInfo  [in] WPM-B service data. 
* @param u8ThreadNo    [in] Thread number
*
* @return TRUE or FALSE 
*
*/
bool WPM_B_Request_Parser(
	virtualDevData_t *pstVirtualDev, 
	processInfo_t *pstProcQIndex,
	uint8_t u8ThreadNo)
{
	/* local variables */
	Bacnet_Multiple_Data_t *pWpmData = NULL;
	#ifdef BACDEL_PR23
	uint32_t u8Count = 0;
	#endif /* BACDEL_PR23 */
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	WPM_B_Request_Parser: Entry \r\n");
	#endif

	/* null check for input pointers */
    if(NULL == pstProcQIndex || NULL == pstVirtualDev)
    {
		/* null input pointers */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		WPM_B_Request_Parser: Null input pointers \r\n");
		#endif
        return FALSE;
    }

    /* default value for WPM-B response pdu type */
    pstProcQIndex->m_stProcessData.m_ePDUType = PDU_TYPE_SIMPLE_ACK;

	/* get pointer to WPM-B data */
	pWpmData = pstProcQIndex->m_stProcessData.m_stAPDU.
        m_pvServiceRequestData;

	/* loop for all properties */
	while(NULL != pWpmData)
	{		
		/* call WP-B function to execute */
		WP_B_Request_Parser(pstVirtualDev, &pWpmData->m_stServiceData,
			&pstProcQIndex->m_stProcessData.m_stRmDvAddr, 
			NULL,u8ThreadNo); 

		#ifdef BACDEL_PR23
		/* according to addendum 135-2016bl,in PR20,for 'First-Failed-Write-Attempt' parameter value is specified */
		/*Increment the count value */
		u8Count++;
		/* validate wpm error response */
		if(BACNET_ONE != u8Count)
		{
			/* validate count value */
			if(true == pWpmData->m_stServiceData.bErrorStatus)
			{
				pWpmData->m_stServiceData.u32ObjectInstance = BACNET_MAX_INSTANCE;
			}
		}
		#endif /* BACDEL_PR23 */

		/* if Wp-B fails for any one property, do not update 
		further properties */
		if(pWpmData->m_stServiceData.bErrorStatus)
		{
			/* break the loop */
			break;
		}

		/* move to next node */
		pWpmData = pWpmData->m_pstMDNext;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	WPM_B_Request_Parser: Exit \r\n");
	#endif
	return TRUE;
}

#endif /* BACDEL_SER_DS_WPM_B */
