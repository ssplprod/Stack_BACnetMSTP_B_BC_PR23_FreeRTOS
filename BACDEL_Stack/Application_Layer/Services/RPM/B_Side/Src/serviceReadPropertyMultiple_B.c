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
*    Magarpatta City, Hadapsar
*    Pune - 411 028                                
*                                                                         
*   FILE
*   serviceReadPropertyMultiple_B.c
*                                                                      
*   AUTHORS                                                                     
*   Harshal Mangale
*                                                                         
*   DESCRIPTION
*   The ReadPropertyMultiple service is used by a client BACnet-user to 
*	request the values of one or more specified properties of one or more 
*	BACnet Objects.
*   This file include function for RPM-B service. 
*                                                                         
*
**********************************************************************************/

#ifdef BACDEL_SER_DS_RPM_B

/* header files */
#include "serviceReadPropertyMultiple_B.h"
#include "miscMiscellaneous.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "pduAbort.h"
#include "bacnetDeviceMgmtInterface.h"
#include "bacnetInitiateServiceMgmt.h"
#include "propertyValueRead.h"
#include "propertyGenricHandler.h"
#include "propertyClearValues.h"


/* to include thread pool functionality */
#ifdef THREAD_POOL
#include "bacnetThreadPoolMgmt.h"
#endif


/**
*                                                                    
* DESCRIPTION
* This function is default handler when a read property multiple 
* request is received.
*    
* @param pstProcQInfo		 [in]	 Pointer to save decoded data.
* @param pu8ServiceRequest	 [in]    Data received for decoding.
* @param i32ServiceLen		 [in]    Length of service request.
*                                   
* @return BACNET_RETURN_TYPE [out]   Success or suitable error code.
*	
*/
BACNET_RETURN_TYPE RPM_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
    int32_t i32Len = 0;
    int32_t i32DecodeLen = 0;
    Bacnet_Multiple_Data_t *pstFirstRpmData = NULL;
    Bacnet_Multiple_Data_t *pstRpmData = NULL;
    Bacnet_Multiple_Data_t *pstTmpRpmData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_B_Decode_Handler: entry \r\n");
	#endif

    /* Check command is valid */
    if( (pstProcQInfo != NULL) && (pu8ServiceRequest != NULL) && (i32ServiceLen != 0 ))
    {
        /* decode apdu request */
	    for (;;) 
	    {
            /* Allocate buffer for read property multiple service data */
            /* Memory is assigned for each new Object ID present in Request */ 
            pstTmpRpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t),  __FILE__, __FUNCTION__, __LINE__);
            if( pstTmpRpmData == NULL )
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:RPM_B_Decode_Handler : Malloc for RPM-B param Failed\r\n");
				#endif
                
                 /* Free Malloc Memmory */
                Free_Service_Memory(pstFirstRpmData);

                /* Set Error for this command*/
                Service_Error_Handler( BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                    ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo, TRUE);

                return BACDEL_SUCCESS;
            }
            memset(pstTmpRpmData, 0, sizeof(Bacnet_Multiple_Data_t));

             /* First time assign pointer to "service_request_data" */
             /* for next ObjId create link list */
            if(pstFirstRpmData == NULL)
            {
                pstFirstRpmData = pstTmpRpmData;
            }
            else
            {
                pstRpmData->m_pstMDNext = pstTmpRpmData;
            }
            /* Pointer to memory to store decoded Parameters*/
            pstRpmData = pstTmpRpmData;

            /* "Fill_ObjId" Flag set to "1" indicate New Obj_Id*/
            pstRpmData->m_stServiceData.u8FillObjId = 1;


            /* Start by looking for an object ID */
            i32Len = Decode_MPS_object_id(&pu8ServiceRequest[i32DecodeLen],
                                i32ServiceLen - i32DecodeLen, pstRpmData);
            if (i32Len < 0)
            {
                /* bad encoding - skip to error/reject/abort handling */
                /* Set Error for this command*/
                Service_Error_Handler( i32Len, pstRpmData->m_stServiceData.eErrorClass,
                    pstRpmData->m_stServiceData.eErrorCode, pstProcQInfo, TRUE);
                
                /* Free Malloc Memmory*/
                Free_Service_Memory(pstFirstRpmData);
                
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RPM_B_Decode_Handler: invalid object id \r\n");
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
                    i32ServiceLen - i32DecodeLen, pstRpmData);
                if (i32Len < 0) 
                {
                    /* bad encoding - skip to error/reject/abort handling */
                     /* Set Error for this command */
                    Service_Error_Handler( i32Len, pstRpmData->m_stServiceData.eErrorClass,
                                            pstRpmData->m_stServiceData.eErrorCode, pstProcQInfo, TRUE);
                     /* Free Malloc Memmory */
                    Free_Service_Memory(pstFirstRpmData);

					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RPM_B_Decode_Handler: invalid property identifier \r\n");
					#endif

                    return BACDEL_SUCCESS;
                }
                i32DecodeLen += i32Len;

                /* handle the special properties */
                if ((pstRpmData->m_stServiceData.eObjectProperty == PROP_ALL) ||
                    (pstRpmData->m_stServiceData.eObjectProperty == PROP_REQUIRED) ||
                    (pstRpmData->m_stServiceData.eObjectProperty == PROP_OPTIONAL)) 
			    {
                    special_property_list_t stPropertyList;
                    uint32_t u32PropertyCnt = 0;
                    uint32_t u32Index = 0;
                    BACNET_PROPERTY_ID eSpecialObjectProperty;
					void *pvObjAddr = NULL;
                    
                /* Find Object in the device object list */
                pvObjAddr = Find_Object(pstRpmData->m_stServiceData.eObjectType,
                            pstRpmData->m_stServiceData.u32ObjectInstance,
                            pstProcQInfo->m_pvReqDevStruct);

				if(NULL != pvObjAddr)
                {
                    /*  No array index options for this special property.
                       Encode error for this object property response */
                    if (ARRAY_INDEX_PRESENT == pstRpmData->m_stServiceData.bArrIndxPresent)
                    {
                        pstRpmData->m_stServiceData.eErrorClass = ERROR_CLASS_PROPERTY;
                        pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
                        pstRpmData->m_stServiceData.bErrorStatus = true;
                    }
                    else 
                    {
                        eSpecialObjectProperty = pstRpmData->m_stServiceData.eObjectProperty;
                        
                        Device_Objects_Property_List(pstRpmData->m_stServiceData.eObjectType,
                            &stPropertyList);
                        
                        u32PropertyCnt =
                            RPM_Object_Property_Count(&stPropertyList, eSpecialObjectProperty);
                        
                        /* handle the error code - but use the special property */
                        if (u32PropertyCnt == 0) 
                        {
                            pstRpmData->m_stServiceData.eErrorClass = ERROR_CLASS_PROPERTY;
                            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_UNKNOWN_PROPERTY;
                            pstRpmData->m_stServiceData.bErrorStatus = true;
                        } 
                        else 
                        {
                             /* Store every supported property for given object */
                            for (u32Index = 0; u32Index < u32PropertyCnt; u32Index++) 
                            {
                                pstRpmData->m_stServiceData.eObjectProperty =
                                    RPM_Object_Property(&stPropertyList,
                                                eSpecialObjectProperty, u32Index);

                                pstRpmData->m_stServiceData.u32ArrayIndex = BACNET_ARRAY_ALL;
                                pstRpmData->m_stServiceData.bArrIndxPresent = ARRAY_INDEX_ABSENT;

                                if(u32Index < u32PropertyCnt - 1)
                                {
                                     /* malloc memory for next property */
                                     /* and assing Obj Id of previous command */
                                    pstTmpRpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t),  __FILE__, __FUNCTION__, __LINE__);
                                    if( pstTmpRpmData == NULL )
                                    {
										#ifdef DEBUG_PRINTF
                                        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: RPM_B_Decode_Handler: Malloc Failed for RPM_B param\r\n");
										#endif

                                         /* Free Malloc Memmory */
                                        Free_Service_Memory(pstFirstRpmData);
                                        
                                        /* Set Error for this command*/
                                        Service_Error_Handler( BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                                            ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo, TRUE);

                                        return BACDEL_SUCCESS;
                                    }

                                    memset(pstTmpRpmData, 0, sizeof(Bacnet_Multiple_Data_t));

                                     /* Get Object identifier from previous property parameters */
                                    pstTmpRpmData->m_stServiceData.eObjectType = pstRpmData->m_stServiceData.eObjectType;
                                    pstTmpRpmData->m_stServiceData.u32ObjectInstance = pstRpmData->m_stServiceData.u32ObjectInstance;
                                     /* Flag set to "0" indicate previous Obj_Id */
                                    pstTmpRpmData->m_stServiceData.u8FillObjId = 0;
                                    
                                    pstRpmData->m_pstMDNext = pstTmpRpmData;
                                    pstRpmData = pstTmpRpmData;
                                }
                            }
                        }
                    }
                }// End of Find Object
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
                    pstTmpRpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t),  __FILE__, __FUNCTION__, __LINE__);
                    if( pstTmpRpmData == NULL )
                    {
						#ifdef DEBUG_PRINTF
                        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: RPM_B_Decode_Handler: Malloc failed for RPM_B param\r\n");
						#endif

                         /* Free Malloc Memmory */
                        Free_Service_Memory(pstFirstRpmData);

                        /* Set Error for this command*/
                        Service_Error_Handler( BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                            ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo, TRUE);

                        return BACDEL_SUCCESS;
                    }

                    memset(pstTmpRpmData, 0, sizeof(Bacnet_Multiple_Data_t));

                     /* Get Object identifier from previous property parameters */
                    pstTmpRpmData->m_stServiceData.eObjectType = pstRpmData->m_stServiceData.eObjectType;
                    pstTmpRpmData->m_stServiceData.u32ObjectInstance = pstRpmData->m_stServiceData.u32ObjectInstance;
                     /* Flag set to "0" indicate previous Obj_Id */
                    pstTmpRpmData->m_stServiceData.u8FillObjId = 0;
                    
                    pstRpmData->m_pstMDNext = pstTmpRpmData;
                    pstRpmData = pstTmpRpmData;
                } 

            }/* End of Inner for loop */

            if (i32DecodeLen >= i32ServiceLen) 
            {
                /* Reached the end so finish up */
                pstRpmData->m_pstMDNext = NULL;

                /* For TESTING assign character string value to all properties */
                //TEST_RPM_B(pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData);
                
                 /* Assign pointer of Decoded RPM Data to service_request_data */
                pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstFirstRpmData;

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
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RPM_B_Decode_Handler: Obj_MgmtLayer returned error\r\n");
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
        
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RPM_B_Decode_Handler: parameter missing\r\n");
		#endif

         /* FIXME: Free Malloc Memmory */
        return BACDEL_SUCCESS;
    } 
	else
	{
		/* discard the request */
		eReturnType = BACDEL_ERROR;
	}
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_B_Decode_Handler: exit\r\n");
	#endif

    return eReturnType;
}







/**
*
* DESCRIPTION
* Function to encode the RPM-B acknowledgement. 
*
* @param pstProcQInfo	[in]	Pointer to service data
* @param pu8APDUResp	[in]    APDU buffer to save encoded data.
* @param i32APDULen		[in]    Length of encoded data in buffer.
*
* @return i32APDULen    [out]   Total length after encoding.
*
*/
int32_t RPM_B_Ack_Encoder(                    
    processInfo_t *pstProcQInfo,
    uint8_t **pu8APDUResp,
    int32_t i32APDULen)
{
    int32_t i32Len = 0;
    uint32_t u32CopyLen = 0;
    /* this array size is linkes with input_buffer_size parameter of BACApp_Encode_Data_Type */
    uint8_t u8TempBuf[MAX_APDU_BUFFER + 100] = { 0 };
    bool bCopyError = false;
    Bacnet_Multiple_Data_t *pstRpmData = NULL;
    //Bacnet_Multiple_Data_t *pstTmpRpmData = NULL;
    virtualDevData_t *pVirtualDev = NULL;
	uint32_t u32PropertyCount = 0;

#ifdef SEGMENTATION_SUPPORTED
	uint32_t u32Count = MAX_APDU_BUFFER;
#endif 

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_B_Ack_Encoder: entry \r\n");
	#endif

    pstRpmData = (Bacnet_Multiple_Data_t *)pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
    pVirtualDev = pstProcQInfo->m_pvReqDevStruct;

    if( pstRpmData == NULL || pVirtualDev == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RPM_B_Ack_Encoder: NULL Input Pointers\r\n");
		#endif

        return BACDEL_ERROR;
    }

    while( pstRpmData != NULL )
    {
		u32PropertyCount++;// increment counter 

         /* Fill first Object ID in response */
        if( pstRpmData->m_stServiceData.u8FillObjId )
        {
            /* Stick this object id into the reply - if it will fit */
            i32Len = Encode_MPS_ObjId_OTag(&u8TempBuf[0], pstRpmData);
#ifdef SEGMENTATION_SUPPORTED
			u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0], i32APDULen, 
				i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);		
#else
			u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0], i32APDULen, i32Len, MAX_APDU_LENGTH_ACCEPTED);
#endif 
            if (u32CopyLen == 0) 
            {
                bCopyError = true;
                break; // break from while loop
            } 
            i32APDULen += i32Len;
        }

		if((pstRpmData->m_stServiceData.eObjectProperty == PROP_ALL ||
            pstRpmData->m_stServiceData.eObjectProperty == PROP_REQUIRED ||
            pstRpmData->m_stServiceData.eObjectProperty == PROP_OPTIONAL) &&
            ERROR_CODE_UNKNOWN_PROPERTY == pstRpmData->m_stServiceData.eErrorCode)
		{
			;// donot encode anything
		}
		else
		{
         /* Fill Property identifier and Property APDUin response */
        i32Len = RPM_Ack_Encode_Apdu_Object_Property(&u8TempBuf[0], 
                        pstRpmData->m_stServiceData.eObjectProperty, 
                        pstRpmData->m_stServiceData.u32ArrayIndex,
                        pstRpmData->m_stServiceData.bArrIndxPresent);
#ifdef SEGMENTATION_SUPPORTED
		u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0], i32APDULen, 
			i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
#else
		u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0], i32APDULen, i32Len, MAX_APDU_LENGTH_ACCEPTED);
#endif 
        if (u32CopyLen == 0) 
        {
            bCopyError = true;
            break; // break from while loop
        }

        i32APDULen += i32Len;

         /* If Error in property value Reading create Error response for that propery */
        if(pstRpmData->m_stServiceData.bErrorStatus)
        {
            if(((pstRpmData->m_stServiceData.eObjectProperty == PROP_ALL) ||
                (pstRpmData->m_stServiceData.eObjectProperty == PROP_REQUIRED) ||
                (pstRpmData->m_stServiceData.eObjectProperty == PROP_OPTIONAL)) &&
                ERROR_CODE_UNKNOWN_PROPERTY == pstRpmData->m_stServiceData.eErrorCode)
            {
                /* Tag 5: propertyAccessError */
                i32APDULen += 
						Encode_Opening_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_4);

                i32APDULen += 
                    Encode_Closing_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_4);
            }
            else
            {
                i32Len =
                    RPM_Ack_Encode_Apdu_Object_Property_error(&u8TempBuf[0],
                                    pstRpmData->m_stServiceData.eErrorClass, 
                                    pstRpmData->m_stServiceData.eErrorCode);
#ifdef SEGMENTATION_SUPPORTED
					u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0], i32APDULen, 
						i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
#else
                    u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0], i32APDULen, i32Len, 
                            MAX_APDU_LENGTH_ACCEPTED);
#endif 
                if (u32CopyLen == 0) 
                {
                    bCopyError = true;
                    break; // break from while loop
                }
                i32APDULen += i32Len;
            }
        }
        else
        {
            /* enough room to fit the property value and tags */
            i32Len = RPM_Ack_Encode_Apdu_Object_Property_value( &u8TempBuf[0], pstRpmData,
                pVirtualDev->m_stDevObject.m_stSegmentationSupport.m_eSegmentationSupport,
                pVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val);
            /* check if error in encoding property value */
            if(pstRpmData->m_stServiceData.bErrorStatus)
            {
                bCopyError = true;
                break; // break from while loop
            }
#ifdef SEGMENTATION_SUPPORTED
		    u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0], i32APDULen, 
				i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
#else
            u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0], i32APDULen, i32Len, MAX_APDU_LENGTH_ACCEPTED);
#endif 
            if (u32CopyLen == 0) 
            {
                bCopyError = true;
                break; // break from while loop
            } 
            i32APDULen += i32Len;
        }
		}//all, required, optional
        
        /* Append Closing tag when property list for that ObjId is over or */
        /* it is last property of RPM-B command */
        if((pstRpmData->m_pstMDNext == NULL) || (pstRpmData->m_pstMDNext->m_stServiceData.u8FillObjId))
        {
            i32Len = RPM_Ack_Encode_Apdu_Object_End(&u8TempBuf[0]);
#ifdef SEGMENTATION_SUPPORTED
			u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0], 
				i32APDULen, i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
#else
			u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0],
                            i32APDULen, i32Len, MAX_APDU_LENGTH_ACCEPTED);
#endif 
            if (u32CopyLen == 0) 
            {
                bCopyError = true;
                break; // break from while loop
            } 
            else 
            {
                i32APDULen += u32CopyLen;
            }        
        }

        /* Free malloc memory for previous property */
        //pstTmpRpmData = pstRpmData;
        pstRpmData = pstRpmData->m_pstMDNext;
        //if( NULL != pstTmpRpmData)
        //OSAL_Free(pstTmpRpmData,  __FILE__, __FUNCTION__, __LINE__);

    }// End of while()

	/* check if error in endocing response */
    if(bCopyError)
    {
        /* If Buffer is full and no space to fill response */
        if(!pstRpmData->m_stServiceData.bErrorStatus)
		#ifdef SEGMENTATION_SUPPORTED
        pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_ABORT_BUFFER_OVERFLOW;
		#else
		pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
		#endif
        /* Set Error for this command */
        Service_Error_Handler( BACNET_STATUS_ABORT, pstRpmData->m_stServiceData.eErrorClass,
                                pstRpmData->m_stServiceData.eErrorCode, pstProcQInfo, TRUE);
		/* encode abort pdu */
        i32APDULen = Abort_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
        
        bCopyError = false;
    }

	/* Free malloc memory for duplicate property value */
	pstRpmData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
    while(NULL != pstRpmData )
	{
		/* free memory allocated to copy data */
		Clear_Duplicate_Prop_Values(pstRpmData->m_stServiceData.eData_Type,
			&pstRpmData->m_stServiceData.pvReadPropValue);        

		/* move to next node */
		pstRpmData = pstRpmData->m_pstMDNext;
	}

    /* Free Malloc Memmory */
    Free_Service_Memory(pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData);
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_B_Ack_Encoder: exit \r\n");
	#endif
    return i32APDULen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   encode the object Property and Array Index portion of the response
*
*@param pu8APDUResp     [out]   Response buffer
*@param eObjectProperty [in]    Property Identifier
*@param i32ArrayIndex   [in]    Array Index 
*
*@return    i32APDULen  Total encoded length of rsponse
*
*******************************************************************************/
int32_t RPM_Ack_Encode_Apdu_Object_Property(
    uint8_t * pu8APDUResp,
    BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex,
    bool bArrIndxPresent)
{
    int32_t i32APDULen = 0;   /* total length of the apdu, return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_Property: entry \r\n");
	#endif

    if (pu8APDUResp) 
    {
        /* Tag 2: propertyIdentifier */
        i32APDULen = Encode_Context_Enumerated(&pu8APDUResp[0], TAG_NO_2, 
            eObjectProperty);
        /* Tag 3: optional propertyArrayIndex */
        if (ARRAY_INDEX_PRESENT == bArrIndxPresent)
            i32APDULen +=
                Encode_Context_Unsigned(&pu8APDUResp[i32APDULen], TAG_NO_3, 
                u32ArrayIndex);
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_Property: exit \r\n");
	#endif

    return i32APDULen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   encode the property value
*
*@param pu8APDUResp [out]   Response buffer
*@param pstRpmData  [in]    Structure of RPM request parameters
*
*@return    i32APDULen  Total encoded length of rsponse
*
*******************************************************************************/
int32_t RPM_Ack_Encode_Apdu_Object_Property_value(
    uint8_t * pu8APDUResp,
    Bacnet_Multiple_Data_t * pstRpmData,
    uint32_t u32SegSupport, uint16_t u16MaxApduLen)
{
    int32_t i32APDULen = 0;   /* total length of the apdu, return value */
    //uint32_t u32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_Property_value: entry \r\n");
	#endif

    if (pu8APDUResp) 
    {
        /* Tag 4: propertyValue */
        i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_4);

        pstRpmData->m_stServiceData.i32ApplicationDataLen = 
            BACApp_Encode_Data_Type(&pu8APDUResp[i32APDULen],
                        MAX_APDU_BUFFER,
                        pstRpmData->m_stServiceData.pvReadPropValue,
                        pstRpmData->m_stServiceData.eData_Type,
                        pstRpmData->m_stServiceData.u32ArrayIndex,
                        i32APDULen, u32SegSupport,
                        pstRpmData->m_stServiceData.bArrIndxPresent,
                        u16MaxApduLen, NULL, NULL, 0);


        /* If data type is not supported*/
        if(DATA_TYPE_NOT_SUPPORTED == pstRpmData->m_stServiceData.i32ApplicationDataLen )
        {
            i32APDULen = 0;

            i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_5);
            i32APDULen +=
                Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], 
                ERROR_CLASS_PROPERTY);
            i32APDULen += Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], 
                ERROR_CODE_DATATYPE_NOT_SUPPORTED);
            i32APDULen += Encode_Closing_Tag(&pu8APDUResp[i32APDULen], TAG_NO_5);

        }
        else if(SEGMENTATION_NOT_SUPPORTED == 
			pstRpmData->m_stServiceData.i32ApplicationDataLen )
        {
            i32APDULen = 0;
            pstRpmData->m_stServiceData.bErrorStatus = true;
            pstRpmData->m_stServiceData.eErrorCode = 
                ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
        }
		else if(APDU_DATA_BUFFER_SIZE_EXCEEDED == 
			 pstRpmData->m_stServiceData.i32ApplicationDataLen )
        {
            i32APDULen = 0;
            pstRpmData->m_stServiceData.bErrorStatus = true;
            pstRpmData->m_stServiceData.eErrorCode = 
                ERROR_CODE_ABORT_BUFFER_OVERFLOW;
        }
        else
        {
            i32APDULen += pstRpmData->m_stServiceData.i32ApplicationDataLen;

            /* Encode the application closing tag */
            i32APDULen += Encode_Closing_Tag(&pu8APDUResp[i32APDULen], TAG_NO_4);
        }
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_Property_value: exit \r\n");
	#endif
	
    return i32APDULen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   encode the Error for object property
*
*@param pu8APDUResp [out]   Response buffer
*@param eErrorClass [in]    Error Class of Error for property
*@param eErrorCode  [in]    Error Code of Error for property
*
*@return    i32APDULen  Total encoded length of rsponse
*
*******************************************************************************/
int32_t RPM_Ack_Encode_Apdu_Object_Property_error(
    uint8_t * pu8APDUResp,
    BACNET_ERROR_CLASS eErrorClass,
    BACNET_ERROR_CODE eErrorCode)
{
    int32_t i32APDULen = 0;   /* total length of the apdu, return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_Property_error: entry \r\n");
	#endif

    if (pu8APDUResp) 
    {
        /* Tag 5: propertyAccessError */
        i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_5);
        i32APDULen +=
            Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], eErrorClass);
        i32APDULen += Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], eErrorCode)
            ;
        i32APDULen += Encode_Closing_Tag(&pu8APDUResp[i32APDULen], TAG_NO_5);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_Property_error: exit \r\n");
	#endif
	
    return i32APDULen;
}

/** 
*******************************************************************************
*
*DESCRIPTION:
*   encode the Closing Tag for Object Identifier
*
*@param pu8APDUResp [out]   Response buffer
*
*@return    i32APDULen  Total encoded length of rsponse
*
*******************************************************************************/
int32_t RPM_Ack_Encode_Apdu_Object_End(
    uint8_t * pu8APDUResp)
{
    int32_t i32APDULen = 0;   /* total length of the apdu, return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_End: entry \r\n");
	#endif
		
    if (pu8APDUResp) {
        i32APDULen = Encode_Closing_Tag(&pu8APDUResp[0], TAG_NO_1);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RPM_Ack_Encode_Apdu_Object_End: exit \r\n");
	#endif

    return i32APDULen;
}

/**
*
* DESCRIPTION                                                                          
* Function to free allocated memories in RPM service parser.
*    
* @param pstRpmResp		 [in]  pointer to RPM response structure.
* @param pstMultipleData [in]  pointer to RPM service structure. 
*                                   
* @return void.
*
*/
void RPM_B_Free_Service_Data(
	rpm_response_t *pstRpmResp,
	Bacnet_Multiple_Data_t *pstMultipleData)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	RPM_B_Free_Service_Data: Entry \r\n");     
	#endif

	/* check input pointers */
	if(NULL != pstRpmResp)
	{
		/* free memory allocated to rpm response struct */
		Free_Argument_Memory(pstRpmResp,
			SERVICE_SUPPORTED_READ_PROP_MULTIPLE);
	}

	/* check input pointers */
	if(NULL != pstMultipleData)
	{
		/* free decoded rpm memory */
		Free_Service_Memory(pstMultipleData);
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	RPM_B_Free_Service_Data: Exit \r\n");     
	#endif
}


/**
*
* DESCRIPTION                                                                          
* Function to validate and execute read multiple property service.
*    
* @param pVirtualDev	[in]  virtual device data.
* @param pstProcQInfo   [in]  rpm-b service data 
* @param u8ThreadNo     [in]  thread number
*                                   
* @return [out] true or false.
*
*/
bool RPM_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pstProcQInfo,	
	uint8_t u8ThreadNo)
{
	/* local variables */
	Bacnet_Multiple_Data_t *pstRpmData = NULL;
	Bacnet_Multiple_Data_t *pstBaseRpmData = NULL;
	#ifdef THREAD_POOL
	Bacnet_Multiple_Data_t *pstNextRpmData = NULL;	
	#endif
	rpm_response_t *pstRpmResponse = NULL;
	rpm_response_t *pstBaseRpmResp = NULL;
	BACnetAddress_t stRmtDevAddr = {0};	
	BACNET_CALLBACK_CONFIG_TYPE eCallbackConfig = CALLBACK_CONFIG_NOT_REQUIRED;
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	uint32_t u32ErrorCode = 0;
	BACNET_ERROR_CLASS eErrorClass = MAX_BACNET_ERROR_CLASS;	
	BACNET_PDU_TYPE ePDUType = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;    
	bool bIsCallbackRequired = FALSE;
	bool bFlag = FALSE;
	uint32_t u32CallbackId = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	RPM_B_Request_Parser: Entry \r\n");     
	#endif

	/* check input pointers */
	if(NULL == pstProcQInfo || NULL == pVirtualDev)
	{
		/* This should not occur ideally */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		RPM_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* default response pdu type */
	pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_COMPLEX_ACK;

	/* get service data pointer */
	pstRpmData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	if(NULL == pstRpmData)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		RPM_B_Request_Parser: Null service data pointer \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, 
			ERROR_CODE_OTHER);
		return TRUE;
	}

	/* backup base pointer */
	pstBaseRpmData = pstRpmData;

	#ifdef THREAD_POOL
	/* backup next pointer for the memory leak in application timeout */
	pstNextRpmData = pstBaseRpmData->m_pstMDNext;
	#endif

	/* loop to validate properties & check callback configuration */
	while(NULL != pstRpmData)
	{
		/* validate RPM request for property validations */
		Validate_ObjId_PropId_ArrayIndx(&pstRpmData->m_stServiceData, pVirtualDev);

		#if (CALLBACK_BEFORE_EXECUTION == CALLBACK_CONFIG_READ_PROPERTY_MULTIPLE)
		/* check if callback is required for each property in RPM */
		if(FALSE == bIsCallbackRequired)
		{
			/* get callback value */
			eCallbackConfig = GetPropertyRpCallback(pstRpmData->m_stServiceData.eObjectType,
				pstRpmData->m_stServiceData.eObjectProperty);

			/* if callback required for any 1 property, set the flag */
			if(CALLBACK_CONFIG_REQUIRED == eCallbackConfig)
			{
				bIsCallbackRequired = TRUE; 
			}
		}
		#endif

		/* move to next node */
		pstRpmData = pstRpmData->m_pstMDNext;
	}
	
	#if (CALLBACK_BEFORE_EXECUTION == CALLBACK_CONFIG_READ_PROPERTY_MULTIPLE)
	/* check if callback is registerd for RPM service */
	if(NULL == afpApplicationFunctionCb[APP_CB_READ_PROPERTY_MULTIPLE])
	{
		/* if callback is not registered for RPM service */
		#ifdef DEBUG_PRINT
		Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: \
		RPM_B_Request_Parser: Callback not registered \r\n");
		#endif			
	}
	else
	{
		/* copy RPM data list to callback response list, only if callback required for any 
		   or all properties in a list */
		if(TRUE == bIsCallbackRequired)
		{
			/* restore base pointer */
			pstRpmData = pstBaseRpmData;

			/* loop to copy request data foir sending it to application */
			while(NULL != pstRpmData && (NULL != pstRpmResponse || !bFlag))
			{				
				if(!bFlag)
				{
					/* allocate memory to base pointer */
					pstBaseRpmResp = (rpm_response_t *)
						OSAL_Malloc(sizeof(rpm_response_t),
						__FILE__, __FUNCTION__, __LINE__);

					/* save base pointer */					
					pstRpmResponse = pstBaseRpmResp;					
					bFlag = TRUE;
				}
				else
				{
					/* allocate memory */
					pstRpmResponse->m_pstRPMNextElem = (rpm_response_t *)
						OSAL_Malloc(sizeof(rpm_response_t),
						__FILE__, __FUNCTION__, __LINE__);
					/* move to new node */
					pstRpmResponse = pstRpmResponse->m_pstRPMNextElem;
				}

				/* check if memory was allocated */
				if(NULL != pstRpmResponse)
				{
					/* copy rpm data */
					pstRpmResponse->m_eObjectType = 
						pstRpmData->m_stServiceData.eObjectType;
					pstRpmResponse->m_u32ObjectInstance = 
						pstRpmData->m_stServiceData.u32ObjectInstance;
					pstRpmResponse->m_eObjectProperty = 
						pstRpmData->m_stServiceData.eObjectProperty;
					pstRpmResponse->m_u32ArrayIndex = 
						pstRpmData->m_stServiceData.u32ArrayIndex;
					pstRpmResponse->m_u8ArrayIndexPresent = 
						pstRpmData->m_stServiceData.bArrIndxPresent;

					/* fill error information if error occured in in RPM validation */
					if(TRUE == pstRpmData->m_stServiceData.bErrorStatus)
					{
						pstRpmResponse->m_eErrorClass = pstRpmData->m_stServiceData.eErrorClass;
						pstRpmResponse->m_eErrorCode = pstRpmData->m_stServiceData.eErrorCode;
					}
					else
					{
						/* save default values for error-class and error code */
						pstRpmResponse->m_eErrorClass = -1;
						pstRpmResponse->m_eErrorCode = -1;
					}
				}
				else
				{					 
					/* free memory allocated to rpm response struct */
					/* free decoded rpm memory */
					RPM_B_Free_Service_Data(pstBaseRpmResp, pstBaseRpmData);					
					pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

					/* set error parameters */
					Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
						BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
						ERROR_CODE_OUT_OF_MEMORY);
					return TRUE;
				}				 			

				/* move to next node */
				pstRpmData = pstRpmData->m_pstMDNext;
			}/* while */
			
			/* copy address */
			memcpy(&stRmtDevAddr, &pstProcQInfo->m_stProcessData.m_stRmDvAddr,
				sizeof(BACnetAddress_t));

			/* generate a new callback id */
			u32CallbackId = Generate_Callback_ID();

			#ifdef THREAD_POOL
			/* set thread callback id & thread state to await state */
			ThreadPool_SetCallbackId(u8ThreadNo, u32CallbackId);
			ThreadPool_SetState(u8ThreadNo, THREADPOOL_STATE_AWAIT);
			#endif

			/* call callback function */
			eReturnType = afpApplicationFunctionCb[APP_CB_READ_PROPERTY_MULTIPLE](
				pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId,
				u32CallbackId,
				SERVICE_SUPPORTED_READ_PROP_MULTIPLE,
				&eErrorClass,
				&u32ErrorCode,
				&ePDUType,
				NULL,
				pstBaseRpmResp,
				NULL,
				&stRmtDevAddr,
				NULL);

			#ifdef THREAD_POOL
			/* check timeout flag */
			if(ThreadPool_GetTimeOutFlagStatus(u8ThreadNo))
			{
				/* free memory allocated to rpm response struct */
				/* free decoded rpm memory */
				/* base pointer already be cleared in TX thread, so start clear 
				list from next of base pointer */
				RPM_B_Free_Service_Data(pstBaseRpmResp, pstNextRpmData);				
				return TRUE;
			}		
			/* set the thread state to busy state */
			ThreadPool_SetState(u8ThreadNo, THREADPOOL_STATE_BUSY);
			#endif

			#ifdef DISPLAY_PARAMETERS
			/* display values */
			Display_Before_Callback_Parameters(eReturnType, 
				SERVICE_SUPPORTED_READ_PROP_MULTIPLE, 
				eErrorClass, u32ErrorCode, ePDUType, NULL);
			#endif

			/* check callback returm value */
			if(BACDEL_SUCCESS != eReturnType)
			{				
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
				RPM_B_Request_Parser: callback returns error \r\n");
				#endif	

				/* free memory allocated to rpm response struct */
				/* free allocated service data */
				pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;				
				RPM_B_Free_Service_Data(pstBaseRpmResp, pstBaseRpmData);
				
				/* fill error parameters */
				Set_Error_Parameters_Of_Confirmed_Req(pstProcQInfo, 
					ePDUType, eErrorClass, u32ErrorCode);
				return TRUE;
			}

			/* restore base pointer */
			pstRpmData = pstBaseRpmData;
			pstRpmResponse = pstBaseRpmResp;


			/* loop to copy error response from application */
			while(NULL != pstRpmResponse && NULL != pstRpmData)
			{
				if(-1 != (int16_t)pstRpmResponse->m_eErrorClass &&
						-1 != (int16_t)pstRpmResponse->m_eErrorCode) //Earlier type casted to int32_t now converted to int16_t as per last possible enum.
				{
					pstRpmData->m_stServiceData.bErrorStatus = TRUE;
					pstRpmData->m_stServiceData.eErrorClass = 
						pstRpmResponse->m_eErrorClass;
					pstRpmData->m_stServiceData.eErrorCode =
						pstRpmResponse->m_eErrorCode;
				}

				/* move to next node */
				pstRpmData = pstRpmData->m_pstMDNext;
				pstRpmResponse = pstRpmResponse->m_pstRPMNextElem;
			}

			/* free the allocated memory for RPM response structure */
			RPM_B_Free_Service_Data(pstBaseRpmResp, NULL);
		}/* if(TRUE == bIsCallbackRequired) */
	}/* else */
	#endif /* CALLBACK_BEFORE_EXECUTION */

	/* restore base pointer */
	pstRpmData = pstBaseRpmData;

	/* loop to read property value, duplicate values, etc. */
	while(NULL != pstRpmData)
	{
		/* check error status and proceed */
		if(FALSE == pstRpmData->m_stServiceData.bErrorStatus)
		{
			/* get the property value for encoding */
			Read_All_Property_Value(&pstRpmData->m_stServiceData, pVirtualDev);
		}

		/* move to next node */
		pstRpmData = pstRpmData->m_pstMDNext;
	}	

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	RPM_B_Request_Parser: Exit \r\n");     
	#endif
	return TRUE;
}

#endif /* BACDEL_SER_DS_RPM_B */
