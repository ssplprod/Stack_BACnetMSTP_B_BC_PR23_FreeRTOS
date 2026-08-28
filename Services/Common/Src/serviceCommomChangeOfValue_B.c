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
*   SoftDEL Systems Ltd.                     		    india@softdel.com         
*   3rd Floor, Pentagon P4,								http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
******************************************************************************/
/****************************************************************************** 
*                                                                         
*   FILE
*	 serviceCommomChangeOfValue_B.c
*                                                                      
*   AUTHORS                                                                             
*	 
*                                                                         
*   DESCRIPTION                                                            
*	 Defines the COV_B & COVP_B common functions for the BACnet application. 
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE        NAME		    DESCRIPTION
*  
******************************************************************************/

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)

#include "serviceCommomChangeOfValue_B.h"
#include "bacDELDef.h"
#include "osalFreeRTOS.h"

#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "miscMiscellaneous.h"
#include "propertyGenricHandler.h"
#include "propertyValueRead.h"

extern Mutex_H m_hcovsublistMtxLockHandle;
extern osSemaphoreId_t m_hCOVCancelSemaphoreHandle;

/**
*                                                                    
*DESCRIPTION                                                                          
*   Creates the struct to be passed to message queue.
*    
*@param pvCurrentData [in] Pointer to struct to assigned as reference.
*@param pstVirtualDev [in] Pointer to struct virtualDevData_t.
*@param bValue [in] To check if first notification.
*@param eDevProp [in] The property for which notification is to be generated.
*@param m_stDestAddress [in] To which specific address the notification is to
*                            be sent. If the address is NULL then notification
*                            is to be sent to all.
*
*@return VOID pointer to the struct to be passed to message queue.
*                                                                      
*/
void *Create_Message_Queue_Struct(virtualDevData_t *pstVirtualDev,
                                  void *pvCurrentData, BOOL bValue,
                                  BACNET_PROPERTY_ID eDevProp,
                                  BACnetAddress_t m_stDestAddress,
                                  int32_t i32ProcessID)
{
    msgquedata_t *pstMsgQueData = NULL;
    
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_Message_Queue_Struct: entry \r\n");
	#endif

    /* Assign memory to the pointer */
    pstMsgQueData = OSAL_Malloc(sizeof(msgquedata_t),  __FILE__, __FUNCTION__, __LINE__);
    if(!pstMsgQueData)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Create_Message_Queue_Struct: malloc error \r\n");
		#endif

        return NULL;
    }
    /* Pointer of device structure from which data should be taken */
    pstMsgQueData->m_pvDeviceStruct = pstVirtualDev;
    /* Pointer to the structure of current data */
    pstMsgQueData->m_pvPropertyValue = pvCurrentData;
    /* To determine if its the first notification */
    pstMsgQueData->m_bFstNotification = bValue;
    /* To determine due to which property notification is to be generated */
    pstMsgQueData->m_eDevProp = eDevProp;
    /* To know for which device notification has to be sent */
    pstMsgQueData->m_stDestAddress = m_stDestAddress;
    /* To know for which Process ID the notification has to be sent */
    pstMsgQueData->m_i32ProcessId = i32ProcessID;
    /* Pointer to itself to free it later on */
    pstMsgQueData->m_pvSelfAddress = pstMsgQueData;
	/* Pointer of current status flags data */
	if(PROP_STATUS_FLAGS == eDevProp)
	{
		memcpy(&pstMsgQueData->m_stStatusFlags, &((Pr_BACnetBitStr_t *)pvCurrentData)->m_stBitStr,
		sizeof(BACnetBitStr_t));
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_Message_Queue_Struct: exit \r\n");
	#endif
	
    return (pstMsgQueData);
}


/**
*                                                                    
*DESCRIPTION                                                                          
*   Generates notification based on the value received. 
*    
*@param u32COVIndex [in] ActiveCOVSubscription list index for 
*                        whom notification has to be send.
*@param pstMsgQueData [in] Pointer to struct msgquedata_t.  
*
*@return VOID.
*                                                                      
*/
BACNET_RETURN_TYPE Send_COV_Notification(uint32_t u32COVIndex, msgquedata_t *pstMsgQueData)
{
    ListOfBACnetCovSubs_t *pstActiveCOVList = NULL;
    virtualDevData_t *pstVirtualDev = NULL;
    CovRecipientInfo_t *pstRecipientInfo = NULL;
    CovPropElement_t *pstCOVPropElem = NULL;
    void *pvNotifyParam = NULL;
    BOOL bNotify = TRUE;
	bool bCopyData = false;
    bacnetip_arguments_t stServiceArgs = {0};
    /*
     * 1) Based on the COV Index received extract the address pointer.
     * 2) Read the value of the property at the index.
     * 3) If the data type is real compare the value returned with stored 
     *    value & see if the remainder is greter than equal to COVIncrement.
     * 4) Based on the notification type call the Aside request function.
     */
	 
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Send_COV_Notification: entry \r\n");
	#endif

    if(pstMsgQueData == NULL)
        return BACDEL_ERROR;

    /* Extract the device structure */
    pstVirtualDev = pstMsgQueData->m_pvDeviceStruct;

	if(pstVirtualDev == NULL)
        return BACDEL_ERROR;

    /* Extract the Active COV Subscription structure */
    pstActiveCOVList = pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.
        m_pstCOVSubscribe;

    if(pstActiveCOVList == NULL)
        return BACDEL_ERROR;

    /* Traverse to the node for which notification has to be send */
    while(pstActiveCOVList->m_i32Index != (int32_t )u32COVIndex)
    {
        /* Point to the next Subscribed COV Index */
        pstActiveCOVList = (ListOfBACnetCovSubs_t *)pstActiveCOVList->m_pstNext;
		if(pstActiveCOVList == NULL)
        	return BACDEL_ERROR;
    }
    pstCOVPropElem = &pstActiveCOVList->m_stCOVPropertyElem;
    pstRecipientInfo = pstActiveCOVList->m_pstSubscriberInfo;
    
    /* verify property is present in subscription */
    while(pstCOVPropElem->m_ePropertyId != pstMsgQueData->m_eDevProp)
    {
        if(pstCOVPropElem->m_pstNext == NULL)
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Send_COV_Notification: bacnet error \r\n");
			#endif

            return BACDEL_ERROR;
        }
        pstCOVPropElem = pstCOVPropElem->m_pstNext;
    }
    
    /* Send Notifiacation to every Recepient address */
    do
    {
        /* If its not the first notification send notification directly */
        if(pstMsgQueData->m_bFstNotification == FALSE)
        {
            /* If not first notification check if the data type is real */
            bNotify = FALSE;
            switch(pstCOVPropElem->m_eData_Type)
            {
                case BACNET_DT_REAL:
                {
                    /* Compare the previously stored value & present value. If
                     * the difference is greater than or equal to COVIncrement 
                     * store the new value & send notification */
                    Pr_BACnetReal_t *rStoredValue;
                    Pr_BACnetReal_t *rCurrentValue;
                    Float_t fCmpValue;

                    rStoredValue = (Pr_BACnetReal_t *)pstCOVPropElem->m_pvStoredValue;
                    rCurrentValue = (Pr_BACnetReal_t *)pstCOVPropElem->m_pvCurrentValue;

                    /* Check COV Increment for Real Values */
                    fCmpValue = rStoredValue->m_fVal - rCurrentValue->m_fVal;

                    if(fCmpValue < 0)
                    {
                       if((rCurrentValue->m_fVal - rStoredValue->m_fVal) <
                            pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Real)
                        {
                            bNotify = FALSE; // Do not send notification
                        }
                        else if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            /* Update Stored Value for Last Recepient */
                            memcpy(rStoredValue, rCurrentValue, sizeof(Pr_BACnetReal_t));
                            bNotify = TRUE;
                        }
                        else
                        {
                            bNotify = TRUE;
                        }
                    }
                    else if(fCmpValue > 0)
                    {
                        if((rStoredValue->m_fVal - rCurrentValue->m_fVal) <
                            pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Real)	
                        {
                             bNotify = FALSE; // Do not send notification
                        }
                        else if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            /* Update Stored Value for Last Recepient */
                            memcpy(rStoredValue, rCurrentValue, sizeof(Pr_BACnetReal_t));
                            bNotify = TRUE;    
                        }
                        else
                        {
                            bNotify = TRUE;
                        }
                
                    }
                    else
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                }
                break;
				
				case BACNET_DT_INTEGER :
				{
					Pr_BACnetSigned32_t  *pstStoredValue;
					Pr_BACnetSigned32_t  *pstCurrentValue;
					int32_t i32CmpValue = 0;

					pstStoredValue = (Pr_BACnetSigned32_t *)pstCOVPropElem->m_pvStoredValue;
					pstCurrentValue = (Pr_BACnetSigned32_t *)pstCOVPropElem->m_pvCurrentValue;
					i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetSigned32_t));
			                
					                
					if(i32CmpValue < 0)
					{
						if((pstCurrentValue->m_i32Val - pstStoredValue->m_i32Val) <
						(pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Unsigned_Int))				
						{
							bNotify = FALSE; // Do not send notification
						}
						else if(NULL == pstRecipientInfo->m_pstNext)
						{
							/* Update Stored Value for Last Recepient */
							memcpy(pstStoredValue, pstCurrentValue, sizeof(Pr_BACnetSigned32_t));
							bNotify = TRUE;
						}
						else
						{
							bNotify = TRUE;
						}
					}
					else if(i32CmpValue > 0)
					{
						if((pstStoredValue->m_i32Val - pstCurrentValue->m_i32Val) <
						(pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Unsigned_Int))
						{
							bNotify = FALSE; // Do not send notification
						}
						else if(NULL == pstRecipientInfo->m_pstNext)
						{
							/* Update Stored Value for Last Recepient */
							memcpy(pstStoredValue, pstCurrentValue, sizeof(Pr_BACnetSigned32_t));
							bNotify = TRUE;
						}
						else
						{
							bNotify = TRUE;
						}
						                
					}
					else
					{
						bNotify = FALSE; // Do not send notification
					}
					                
				}
				break;
				
				case BACNET_DT_DOUBLE:
                {
                    /* Compare the previously stored value & present value. If
                     * the difference is greater than or equal to COVIncrement 
                     * store the new value & send notification */
                    Pr_BACnetDouble_t *rStoredValue;
                    Pr_BACnetDouble_t *rCurrentValue;
                    Double_t dCmpValue;

                    rStoredValue = (Pr_BACnetDouble_t *)pstCOVPropElem->m_pvStoredValue;
                    rCurrentValue = (Pr_BACnetDouble_t *)pstCOVPropElem->m_pvCurrentValue;

                    /* Check COV Increment for double Values */
                    dCmpValue = rStoredValue->m_dVal - rCurrentValue->m_dVal;

                    if(dCmpValue < 0)
                    {
                        if((rCurrentValue->m_dVal - rStoredValue->m_dVal) <
                            pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Double)

                        {
                            bNotify = FALSE; // Do not send notification
                        }
                        else if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            /* Update Stored Value for Last Recepient */
                            memcpy(rStoredValue, rCurrentValue, sizeof(Pr_BACnetDouble_t));
                            bNotify = TRUE;
                        }
                        else
                        {
                            bNotify = TRUE;
                        }
                    }
                    else if(dCmpValue > 0)
                    {
                        if((rStoredValue->m_dVal - rCurrentValue->m_dVal) <
                            pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Double)
                        {
                            bNotify = FALSE; // Do not send notification
                        }
                        else if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            /* Update Stored Value for Last Recepient */
                            memcpy(rStoredValue, rCurrentValue, sizeof(Pr_BACnetDouble_t));
                            bNotify = TRUE;    
                        }
                        else
                        {
                            bNotify = TRUE;
                        }
                
                    }
                    else
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                }
                break;
                /* If data type is not real copy the new value & send notification */
                case BACNET_DT_UNSIGNED :
                case BACNET_DT_UNSIGNED32:
                {                    
                    /* Compare the previously stored value & present value. If
                     * there is difference send notification */
                    Pr_BACnetUnsigned32_t *pstStoredValue = NULL;
                    Pr_BACnetUnsigned32_t *pstCurrentValue = NULL;

                    pstStoredValue = (Pr_BACnetUnsigned32_t *)pstCOVPropElem->m_pvStoredValue;
                    pstCurrentValue = (Pr_BACnetUnsigned32_t *)pstCOVPropElem->m_pvCurrentValue;


                    if(pstStoredValue->m_u32Val < pstCurrentValue->m_u32Val)
                    {
                      	if((pstCurrentValue->m_u32Val - pstStoredValue->m_u32Val) <
                            pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Unsigned_Int)
                        {
                             bNotify = FALSE; // Do not send notification
                        }
                        else if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            /* Update Stored Value for Last Recepient */
                            memcpy(pstStoredValue, pstCurrentValue, sizeof(Pr_BACnetUnsigned32_t));
                            bNotify = TRUE;
                        }
                        else
                        {
                            bNotify = TRUE;
                        }
                    }
                    else if(pstStoredValue->m_u32Val > pstCurrentValue->m_u32Val)
                    {
                        if((pstStoredValue->m_u32Val - pstCurrentValue->m_u32Val) <
                            pstActiveCOVList->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Unsigned_Int)	
                        {
                            bNotify = FALSE; // Do not send notification
                        }
                        else if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            /* Update Stored Value for Last Recepient */
                            memcpy(pstStoredValue, pstCurrentValue, sizeof(Pr_BACnetUnsigned32_t));
                            bNotify = TRUE;    
                        }
                        else
                        {
                            bNotify = TRUE;
                        }
                
                    }
                    else
                    {
                        bNotify = FALSE; // Do not send notification
                    }

                }
                break;
                case BACNET_DT_UNSIGNED16:
                {                    
                    /* Compare the previously stored value & present value. If
                     * there is difference send notification */
                    Pr_BACnetUnsigned16_t *pstStoredValue;
                    Pr_BACnetUnsigned16_t *pstCurrentValue;
                    int32_t i32CmpValue;

                    pstStoredValue = (Pr_BACnetUnsigned16_t *)pstCOVPropElem->m_pvStoredValue;
                    pstCurrentValue = (Pr_BACnetUnsigned16_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = (pstStoredValue->m_u16Val) - (pstCurrentValue->m_u16Val);
                    if(0 == i32CmpValue)
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                    else
                    {
                        if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            memcpy(pstStoredValue, pstCurrentValue, sizeof(Pr_BACnetUnsigned16_t)); 
                        }
                        bNotify = TRUE;
                    }
                }
                break;
                case BACNET_DT_ENUM : 
                case BACNET_DT_ENUM_NEW : 
                case BACNET_DT_BACKUPSTATE:
                case BACNET_DT_BACNETSEG:
                {
                    Pr_BinaryEnumPV_t *pstStoredValue;
                    Pr_BinaryEnumPV_t *pstCurrentValue;
                    int32_t i32CmpValue;

                    pstStoredValue = (Pr_BinaryEnumPV_t *)pstCOVPropElem->m_pvStoredValue;
                    pstCurrentValue = (Pr_BinaryEnumPV_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(&pstStoredValue->m_eVal,
                        &pstCurrentValue->m_eVal, sizeof(BACNET_BINARY_PV));
                    if(0 == i32CmpValue)
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                    else
                    {
                        if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            memcpy(pstStoredValue, pstCurrentValue,
                                sizeof(Pr_BinaryEnumPV_t));
                        }
                        bNotify = TRUE;
                    }
                }
                break;
                case BACNET_DT_BITSTRING:
                {
					/* local variable */
                    Pr_BACnetBitStr_t *pstStoredValue = NULL;
                    Pr_BACnetBitStr_t *pstCurrentValue = NULL;
					Pr_BACnetBitStr_t stCurrentValue = {0};
                    int32_t i32CmpValue = 0; 

					/* get stored value and current value */
                    pstStoredValue = (Pr_BACnetBitStr_t *)pstCOVPropElem->m_pvStoredValue;
                    //pstCurrentValue = (Pr_BACnetBitStr_t *)pstCOVPropElem->m_pvCurrentValue;
					/* get bitstring value */
					memcpy(&stCurrentValue.m_stBitStr, &pstMsgQueData->m_stStatusFlags,
						sizeof(BACnetBitStr_t));
					pstCurrentValue = &stCurrentValue;
                    i32CmpValue = memcmp(&pstStoredValue->m_stBitStr.m_u8TransBits,
                        &stCurrentValue.m_stBitStr.m_u8TransBits, MIN_BITSTRING_BYTES);
                    if(0 == i32CmpValue)
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                    else
                    {
                        if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            memcpy(pstStoredValue, pstCurrentValue,
                                sizeof(Pr_BACnetBitStr_t));

                            /* update Stored Present Value as When out of service changes T to F,
                                Present value is updated by Dummy Present Value */
                            if(pstActiveCOVList->m_stCOVPropertyElem.m_eData_Type == BACNET_DT_REAL)
                            {
                                memcpy(pstActiveCOVList->m_stCOVPropertyElem.m_pvStoredValue, 
                                    pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                                    sizeof(Pr_BACnetReal_t));
                            }
                            else if(pstActiveCOVList->m_stCOVPropertyElem.m_eData_Type == BACNET_DT_ENUM ||
                                pstActiveCOVList->m_stCOVPropertyElem.m_eData_Type == BACNET_DT_ENUM_NEW)
                            {
                                memcpy(pstActiveCOVList->m_stCOVPropertyElem.m_pvStoredValue, 
                                    pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                                    sizeof(Pr_BinaryEnumPV_t));
                            }
                            else if(pstActiveCOVList->m_stCOVPropertyElem.m_eData_Type == BACNET_DT_UNSIGNED)
                            {                            memcpy(pstActiveCOVList->m_stCOVPropertyElem.m_pvStoredValue, 
                                    pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                                    sizeof(Pr_BACnetUnsigned32_t));
                            }
                        }
						bCopyData = true;
                        bNotify = TRUE;
                    }
                }
                break;
                case BACNET_DT_BOOLEAN:
                {
                    Pr_BACnetBool_t *pstStoredValue;
                    Pr_BACnetBool_t *pstCurrentValue;
                    int32_t i32CmpValue; 

                    pstStoredValue = (Pr_BACnetBool_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetBool_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetBool_t));
                    if(0 == i32CmpValue)
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                    else
                    {
                        if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            memcpy(pstStoredValue, pstCurrentValue,
                                sizeof(Pr_BACnetBool_t));
                        }
                        bNotify = TRUE;
                    }
                }
                break;
                case BACNET_DT_CHARSTRING:
                {
                    Pr_BACnetCharStr_t *pstStoredValue;
                    Pr_BACnetCharStr_t *pstCurrentValue;
                    int32_t i32CmpValue; 

                    pstStoredValue = (Pr_BACnetCharStr_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetCharStr_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetCharStr_t));
                    if(0 == i32CmpValue)
                    {
                        bNotify = FALSE; // Do not send notification
                    }
                    else
                    {
                        if(NULL == pstRecipientInfo->m_pstNext)
                        {
                            memcpy(pstStoredValue, pstCurrentValue,
                                sizeof(Pr_BACnetCharStr_t));
                        }
                        bNotify = TRUE;
                    }
                }
                break;

                case BACNET_DT_DATETIME:
			    {
				    Pr_BACnetDateTime_t  *pstStoredValue;
				    Pr_BACnetDateTime_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetDateTime_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetDateTime_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetDateTime_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetDateTime_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;	
                case BACNET_DT_OBJECTID :
			    {
				    Pr_BACnetObjId_t  *pstStoredValue;
				    Pr_BACnetObjId_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetObjId_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetObjId_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetObjId_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetObjId_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_BACNETDEVSTAT :
			    {
				    Pr_BACnetDevStatus_t  *pstStoredValue;
				    Pr_BACnetDevStatus_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetDevStatus_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetDevStatus_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetDevStatus_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetDevStatus_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_DATE :
			    {
				    Pr_BACnetDate_t  *pstStoredValue;
				    Pr_BACnetDate_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetDate_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetDate_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetDate_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetDate_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_TIME :
			    {
				    Pr_BACnetTime_t  *pstStoredValue;
				    Pr_BACnetTime_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetTime_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetTime_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetTime_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetTime_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;

                case BACNET_DT_PRIORITY_ARRAY :
			    {
				    Pr_BACnetPriorityArray_t  *pstStoredValue;
				    Pr_BACnetPriorityArray_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetPriorityArray_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetPriorityArray_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetPriorityArray_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetPriorityArray_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_UNSIGNED_ARRAY :
				case BACNET_DT_UNSIGNED_LIST :
                case BACNET_DT_OBJECTID_ARRAY :
                case BACNET_DT_ADDRESSBINDING_LIST :
                case BACNET_DT_BOOLEAN_ARRAY :
                case BACNET_DT_RECIPIENT_LIST :
                case BACNET_DT_NOTIFICATIONPRIORITY :
			    {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        pstCOVPropElem->m_pvStoredValue = pstCOVPropElem->m_pvCurrentValue;                            
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_DATELIST :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfBACnetCalendarEntry_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_DESTINATION_LIST :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfBACnetDestination_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_DEVOBJPROPREFF_LIST :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfBACnetDevObjPropRef_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_DAILYSCHEDULE_ARRAY :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfBACnetDailySchedule_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_SPECIALEVENT_ARRAY :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfBACnetSpecialEvent_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_LOGBUFFER_TRENDLOG :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfBACnetLogRecord_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_EVENTPARAMETERS :
                {				
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_BACnetEventParameter_t));                  
                    }
                    bNotify = TRUE;
			    }
                break;
                case BACNET_DT_TIMESTAMP_ARRAY :
			    {
				    Pr_BACnetEventTimeStamp_t  *pstStoredValue;
				    Pr_BACnetEventTimeStamp_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetEventTimeStamp_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetEventTimeStamp_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetEventTimeStamp_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetEventTimeStamp_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_OBJTYPE :
			    {
				    Pr_BACnetObjType_t  *pstStoredValue;
				    Pr_BACnetObjType_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetObjType_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetObjType_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetObjType_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetObjType_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_SCHEDULE_PRESENT_DEFAULT :
			    {
				    AnyValue_t  *pstStoredValue;
				    AnyValue_t  *pstCurrentValue;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (AnyValue_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (AnyValue_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(AnyValue_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(AnyValue_t));
					    }
					    bNotify = TRUE;
				    }
			    }
                break;
                case BACNET_DT_DEVOBJPROPREF :
                {
                    Pr_BACnetDevObjPropRef_t   *pstStoredValue = NULL;                
				    Pr_BACnetDevObjPropRef_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetDevObjPropRef_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetDevObjPropRef_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetDevObjPropRef_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetDevObjPropRef_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_OBJPROPREF :
                {
                    Pr_BACnetObjPropRef_t   *pstStoredValue = NULL;                
				    Pr_BACnetObjPropRef_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetObjPropRef_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetObjPropRef_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetObjPropRef_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetObjPropRef_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_SETPOINTREF :
                {
                    Pr_BACnetSetpointRef_t   *pstStoredValue = NULL;                
				    Pr_BACnetSetpointRef_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetSetpointRef_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetSetpointRef_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetSetpointRef_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetSetpointRef_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_CLIENTCOVINCREMENT :
                {
                    Pr_BACnetClientCOV_t   *pstStoredValue = NULL;                
				    Pr_BACnetClientCOV_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetClientCOV_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetClientCOV_t *)pstCOVPropElem->m_pvCurrentValue;
				    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetClientCOV_t));

				    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetClientCOV_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_DATERANGE :
                {
                    Pr_BACnetDateRange_t   *pstStoredValue = NULL;                
				    Pr_BACnetDateRange_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetDateRange_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetDateRange_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetDateRange_t));
                    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetDateRange_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_SERVICES_SUPPORTED :
                {
                    Pr_BACnetServicesSupported_t   *pstStoredValue = NULL;                
				    Pr_BACnetServicesSupported_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetServicesSupported_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetServicesSupported_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetServicesSupported_t));
                    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetServicesSupported_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_OBJECT_TYPE_SUPPORTED :
                {
                    Pr_BACnetObjectTypesSupported_t   *pstStoredValue = NULL;                
				    Pr_BACnetObjectTypesSupported_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetObjectTypesSupported_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetObjectTypesSupported_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetObjectTypesSupported_t));
                    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetObjectTypesSupported_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_TIMESTAMP :
                {
                    /* FIXME - TODO - do as per type of time stamp */
                    Pr_BACnetDateTime_t   *pstStoredValue = NULL;                
				    Pr_BACnetDateTime_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetDateTime_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetDateTime_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetDateTime_t));
                    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetDateTime_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_OCTETSTRING :
                {
                    Pr_BACnetOctetStr_t   *pstStoredValue = NULL;                
				    Pr_BACnetOctetStr_t   *pstCurrentValue = NULL;
				    int32_t i32CmpValue = 0; 

				    pstStoredValue = (Pr_BACnetOctetStr_t *)pstCOVPropElem->m_pvStoredValue;
				    pstCurrentValue = (Pr_BACnetOctetStr_t *)pstCOVPropElem->m_pvCurrentValue;
                    i32CmpValue = memcmp(pstStoredValue,pstCurrentValue, sizeof(Pr_BACnetOctetStr_t));
                    if(0 == i32CmpValue)
				    {
					    bNotify = FALSE; // Do not send notification
				    }
				    else
				    {
					    if(NULL == pstRecipientInfo->m_pstNext)
					    {
						    memcpy(pstStoredValue, pstCurrentValue,
							    sizeof(Pr_BACnetOctetStr_t));
					    }
					    bNotify = TRUE;
				    }
                }
                break;
                case BACNET_DT_CHARSTRING_ARRAY :
                {
                    if(NULL == pstRecipientInfo->m_pstNext)
                    {
                        memcpy(pstCOVPropElem->m_pvStoredValue,pstCOVPropElem->m_pvCurrentValue,sizeof(Pr_ListOfCharStr_t));                  
                    }
                    bNotify = TRUE;

                }
                break;

                default :
                {
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Send_COV_Notification: Unknown data type \r\n");
					#endif

                    return BACDEL_ERROR;
                }
                break;
            }
        }
        else
        {
            while(memcmp(pstRecipientInfo->m_stAddress.u8IpAddrs, 
                pstMsgQueData->m_stDestAddress.u8IpAddrs,
                pstRecipientInfo->m_stAddress.u8mac_len))
            {
                if(pstRecipientInfo->m_pstNext == NULL)
                {
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Send_COV_Notification: End of pstRecipientInfo list \r\n");
					#endif

                    return BACDEL_ERROR;
                }
                pstRecipientInfo = pstRecipientInfo->m_pstNext;
            }

            while((int32_t)pstRecipientInfo->m_u32ProcessId != 
                pstMsgQueData->m_i32ProcessId)
            {
                if(pstRecipientInfo->m_pstNext == NULL)
                {
					#ifdef DEBUG_PRINTF
				    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: In Send_COV_Notification: End of pstRecipientInfo list \r\n");
					#endif

                    return BACDEL_ERROR;
                }
                pstRecipientInfo = pstRecipientInfo->m_pstNext;
            }
        }
        
        /* If notification is to be send */
        if(bNotify && pstRecipientInfo->m_i8Subscribe == TRUE)
        {
            /* Copy the data that has to be send */
        	pvNotifyParam = Get_Notification_Parameter(pstVirtualDev,
                pstActiveCOVList, pstRecipientInfo, pstMsgQueData, bCopyData);
            if(pvNotifyParam == NULL)
            {
				;
				#ifdef DEBUG_PRINTF
                 Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Send_COV_Notification:"
                    "Get_Notification_Parameter returned Error\r\n");
				#endif
            }

            /* Initialize all the required parameters */
            stServiceArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.
                m_stCOV_Notification.pvCOVValue = pvNotifyParam;

			/* set to default value */
			stServiceArgs.m_u32DeviceID = BACNET_MAX_INSTANCE;

            stServiceArgs.m_stDestBACnetAddr = pstRecipientInfo->m_stAddress;            

            /* Based on notification type send notification */
            if(pstRecipientInfo->m_bIssueConfirmedNotification)
            {
                stServiceArgs.m_stNPDUData.m_stAPDUData.m_eServiceSupport =
                    SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION;
            }
            else
            {
                stServiceArgs.m_stNPDUData.m_stAPDUData.m_eServiceSupport =
                    SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION;
            }               
                
            /* Update BVLC type for sending local broadcast COVU notification */
            if(stServiceArgs.m_stDestBACnetAddr.u8IpAddrs[3] == 0xFF)
                stServiceArgs.m_eBvlcFunctionType = BVLC_ORIGINAL_BROADCAST_NPDU;
			else
				stServiceArgs.m_eBvlcFunctionType = BVLC_ORIGINAL_UNICAST_NPDU;

            BACDEL_Generate_Aside_Request(pstVirtualDev->m_u8DvSADR, &stServiceArgs, NULL);
         }

        /* Point to the next Subscribed COV Index */
        pstRecipientInfo = pstRecipientInfo->m_pstNext;
    }while((NULL != pstRecipientInfo) && (pstMsgQueData->m_bFstNotification == FALSE));

    /* Free the original malloced memory */
    OSAL_Free(pstMsgQueData->m_pvSelfAddress,  __FILE__, __FUNCTION__, __LINE__);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Send_COV_Notification : exit \r\n");
	#endif

    return BACDEL_SUCCESS;
}


/**
*                                                                    
*DESCRIPTION                                                                          
*   Copies the value from the ActiveCOVSubscription list to be send. 
*    
*@param pstVirtualDev [in] Pointer to struct virtualDevData_t.
*@param pstActiveCOVList [in] Pointer to ActiveCOVSubscription list.  
*
*@return VOID pointer to the copied location.
*                                                                      
*/
void *Get_Notification_Parameter(virtualDevData_t *pstVirtualDev,
                                ListOfBACnetCovSubs_t *pstActiveCOVList,
                                CovRecipientInfo_t *pstRecipientInfo,
								msgquedata_t *pstMsgQueData,  bool bCopyData)
{
    /* There is a fair chance that while sending notification the
     * actual value may change hence make copy of the data to be 
     * send. */
    ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;
    CovRecipientInfo_t *pstSubscriberInfo = NULL;
    CovPropElement_t *pstCOVPropertyElem = NULL;
    CovPropElement_t *pstActiveCOVElem = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Get_Notification_Parameter: entry \r\n");
	#endif
    
    /* Allocate the memory for copying COV parameters */
    pstCOVSubscribe = OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t),  __FILE__, __FUNCTION__, __LINE__);
    pstSubscriberInfo = OSAL_Malloc(sizeof(CovRecipientInfo_t),  __FILE__, __FUNCTION__, __LINE__);
    if((pstCOVSubscribe == NULL) || (NULL == pstSubscriberInfo))
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
		#endif

		if(NULL != pstCOVSubscribe)
		{
			/* free malloced memory */
			OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
		}
		if(NULL != pstSubscriberInfo)
		{
			/* free malloced memory */
			OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
		}
        return NULL;
    }

    /* Copy the ActiveCOVSubscription list data to allocated memory */
    memcpy(pstSubscriberInfo, pstRecipientInfo, sizeof(CovRecipientInfo_t));

    /* Update the Device Object Identifier */
	pstSubscriberInfo->m_stObjId.m_eObjectType =
		pstVirtualDev->m_stDevObject.m_stObjectID.m_eObjectType;
    pstSubscriberInfo->m_stObjId.m_u32ObjId =
        pstVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId;
    /* fix for bug id - 3493 */
    pstSubscriberInfo->m_pstNext = NULL;
    
    /* Copy All the subscription data into new Structure */
    memcpy(pstCOVSubscribe, pstActiveCOVList, sizeof(ListOfBACnetCovSubs_t));

    /* Assign New Subscribe_Info Structure */
    pstCOVSubscribe->m_pstSubscriberInfo = pstSubscriberInfo;

    /* Based on the data types copy the stored value */
    switch(pstActiveCOVList->m_stCOVPropertyElem.m_eData_Type)
    {
        case BACNET_DT_REAL :
        {
            Pr_BACnetReal_t *prValue;
            prValue = OSAL_Malloc(sizeof(Pr_BACnetReal_t),  __FILE__, __FUNCTION__, __LINE__);
            if(prValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(prValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetReal_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = prValue;
        }
        break;
		
		case BACNET_DT_DOUBLE :
        {
            Pr_BACnetDouble_t *prValue;
            prValue = OSAL_Malloc(sizeof(Pr_BACnetDouble_t),  __FILE__, __FUNCTION__, __LINE__);
            if(prValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(prValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDouble_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = prValue;
        }
        break;
		
        case BACNET_DT_ENUM :
        case BACNET_DT_ENUM_NEW :
        case BACNET_DT_BACKUPSTATE :
        case BACNET_DT_BACNETSEG :
        {
            Pr_BinaryEnumPV_t *peValue;
            peValue = OSAL_Malloc(sizeof(Pr_BinaryEnumPV_t),  __FILE__, __FUNCTION__, __LINE__);
            if(peValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(peValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BinaryEnumPV_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = peValue;
        }
        break;
        case BACNET_DT_UNSIGNED : 
        case BACNET_DT_UNSIGNED32 :
               
        {
            Pr_BACnetUnsigned32_t *pusValue = NULL;
            pusValue = OSAL_Malloc(sizeof(Pr_BACnetUnsigned32_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pusValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pusValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetUnsigned32_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pusValue;
        }
        break;
        case BACNET_DT_UNSIGNED16 :
        {
            Pr_BACnetUnsigned16_t *pusValue;
            pusValue = OSAL_Malloc(sizeof(Pr_BACnetUnsigned16_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pusValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pusValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetUnsigned16_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pusValue;
        }
        break;
	    case BACNET_DT_BOOLEAN :
        { 
		    Pr_BACnetBool_t *pusValue;
            pusValue = OSAL_Malloc(sizeof(Pr_BACnetBool_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pusValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pusValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetBool_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pusValue;
        } 
        break;
        case BACNET_DT_CHARSTRING :
        { 
		    Pr_BACnetCharStr_t *pusValue = NULL;
            pusValue = OSAL_Malloc(sizeof(Pr_BACnetCharStr_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pusValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pusValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetCharStr_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pusValue;
        }
        break;
        case BACNET_DT_BITSTRING :
        { 
		    Pr_BACnetBitStr_t *pusValue = NULL;
            pusValue = OSAL_Malloc(sizeof(Pr_BACnetBitStr_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pusValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pusValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetBitStr_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pusValue;
        } 
        break;
	    case BACNET_DT_DATETIME :
        { 
		    Pr_BACnetDateTime_t *pstDateTimeValue = NULL;
            pstDateTimeValue = OSAL_Malloc(sizeof(Pr_BACnetDateTime_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstDateTimeValue == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstDateTimeValue, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDateTime_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstDateTimeValue;
        }
        break;
        case BACNET_DT_OBJECTID :
        { 
		    Pr_BACnetObjId_t *pstObjectID = NULL;
            pstObjectID = OSAL_Malloc(sizeof(Pr_BACnetObjId_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstObjectID == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstObjectID, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetObjId_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstObjectID;
        }
        break;
        case BACNET_DT_BACNETDEVSTAT :
        { 
		    Pr_BACnetDevStatus_t *pstDeviceStatus = NULL;
            pstDeviceStatus = OSAL_Malloc(sizeof(Pr_BACnetDevStatus_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstDeviceStatus == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstDeviceStatus, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDevStatus_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstDeviceStatus;
        }
        break;
        case BACNET_DT_DATE :
        { 
		    Pr_BACnetDate_t *pstDate = NULL;
            pstDate = OSAL_Malloc(sizeof(Pr_BACnetDate_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstDate == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstDate, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDate_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstDate;
        }
        break;
        case BACNET_DT_TIME :
        { 
		    Pr_BACnetTime_t *pstTime = NULL;
            pstTime = OSAL_Malloc(sizeof(Pr_BACnetTime_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstTime == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstTime, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetTime_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstTime;
        }
        break;
        case BACNET_DT_INTEGER :
        { 
		    Pr_BACnetSigned32_t *pstUTCArray = NULL;
            pstUTCArray = OSAL_Malloc(sizeof(Pr_BACnetSigned32_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstUTCArray == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstUTCArray, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetSigned32_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstUTCArray;
        }
        break;
        case BACNET_DT_PRIORITY_ARRAY :
        { 
		    Pr_BACnetPriorityArray_t *pstBinaryPriArray = NULL;
            pstBinaryPriArray = OSAL_Malloc(sizeof(Pr_BACnetPriorityArray_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstBinaryPriArray == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstBinaryPriArray, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetPriorityArray_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstBinaryPriArray;
        }
        break;
        case BACNET_DT_TIMESTAMP_ARRAY :
        {
		    Pr_BACnetEventTimeStamp_t *pstTimeStampArray = NULL;
            pstTimeStampArray = OSAL_Malloc(sizeof(Pr_BACnetEventTimeStamp_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstTimeStampArray == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstTimeStampArray, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetEventTimeStamp_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstTimeStampArray;
        } 
        break;
        case BACNET_DT_UNSIGNED_ARRAY :
		case BACNET_DT_UNSIGNED_LIST :
        { 
		    Pr_ListOfUnsigned_t    *pstSignedList = NULL;
            pstSignedList = OSAL_Malloc(sizeof(Pr_ListOfUnsigned_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstSignedList == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstSignedList, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfUnsigned_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstSignedList;
        }
        break;
        case BACNET_DT_OBJTYPE :
        { 
		    Pr_BACnetObjType_t    *pstObjectType = NULL;
            pstObjectType = OSAL_Malloc(sizeof(Pr_BACnetObjType_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstObjectType == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstObjectType, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetObjType_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstObjectType;
        }
        break;
        case BACNET_DT_SCHEDULE_PRESENT_DEFAULT :
	    {  
		    AnyValue_t   *pstPresentSchedul = NULL;
            pstPresentSchedul = OSAL_Malloc(sizeof(AnyValue_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstPresentSchedul == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstPresentSchedul, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(AnyValue_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstPresentSchedul;
        }
        break;
        case BACNET_DT_DEVOBJPROPREF :
	    {  
		    Pr_BACnetDevObjPropRef_t   *pstPresentDOPR = NULL;
            pstPresentDOPR = OSAL_Malloc(sizeof(Pr_BACnetDevObjPropRef_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstPresentDOPR == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstPresentDOPR, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDevObjPropRef_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstPresentDOPR;
        }
        break;
        case BACNET_DT_OBJPROPREF :
	    {  
		    Pr_BACnetObjPropRef_t 	*pstObjPropReff = NULL;
            pstObjPropReff = OSAL_Malloc(sizeof(Pr_BACnetObjPropRef_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstObjPropReff == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstObjPropReff, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetObjPropRef_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstObjPropReff;
        }
        break;
        case BACNET_DT_SETPOINTREF :
	    {  
		    Pr_BACnetSetpointRef_t   *pstSetPointReff = NULL;   
            pstSetPointReff = OSAL_Malloc(sizeof(Pr_BACnetSetpointRef_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstSetPointReff == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstSetPointReff, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetSetpointRef_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstSetPointReff;
        }
        break;
        case BACNET_DT_CLIENTCOVINCREMENT :
	    {  
		    Pr_BACnetClientCOV_t *pstClienCov = NULL;   
            pstClienCov = OSAL_Malloc(sizeof(Pr_BACnetClientCOV_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstClienCov == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstClienCov, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetClientCOV_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstClienCov;
        }
        break;
        case BACNET_DT_DATERANGE :
	    {  
		    Pr_BACnetDateRange_t       *pstEffectivePeriod = NULL;   
            pstEffectivePeriod = OSAL_Malloc(sizeof(Pr_BACnetDateRange_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstEffectivePeriod == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstEffectivePeriod, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDateRange_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstEffectivePeriod;
        }
        break;
        case BACNET_DT_SERVICES_SUPPORTED :
	    {  
		    Pr_BACnetServicesSupported_t *pstSerSupport = NULL;  
            pstSerSupport = OSAL_Malloc(sizeof(Pr_BACnetServicesSupported_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstSerSupport == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstSerSupport, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetServicesSupported_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstSerSupport;
        }
        break;
        case BACNET_DT_OBJECT_TYPE_SUPPORTED :
	    {  
		    Pr_BACnetObjectTypesSupported_t *pstObjSupport = NULL;  
            pstObjSupport = OSAL_Malloc(sizeof(Pr_BACnetObjectTypesSupported_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstObjSupport == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstObjSupport, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetObjectTypesSupported_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstObjSupport;
        }    
        break;
        case BACNET_DT_OBJECTID_ARRAY :
        {
            Pr_ListOfObjId_t *pvStartObjectList = NULL;  
            pvStartObjectList = OSAL_Malloc(sizeof(Pr_ListOfObjId_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pvStartObjectList == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pvStartObjectList, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfObjId_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pvStartObjectList;
        }
        break;
        case BACNET_DT_ADDRESSBINDING_LIST : 
        {
            Pr_ListOfBACnetAddrBinding_t *pstDevAddrBind = NULL;
            pstDevAddrBind = OSAL_Malloc(sizeof(Pr_ListOfBACnetAddrBinding_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstDevAddrBind == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstDevAddrBind, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetAddrBinding_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstDevAddrBind;
        }
        break;
        case BACNET_DT_BOOLEAN_ARRAY :
        {
            ListOfBoolen_t *pstBoolList = NULL;
            pstBoolList = OSAL_Malloc(sizeof(ListOfBoolen_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstBoolList == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstBoolList, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(ListOfBoolen_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstBoolList;
        }
        break;
        case BACNET_DT_DESTINATION_LIST :
        {
            Pr_ListOfBACnetDestination_t *pstRecipient = NULL;
            pstRecipient = OSAL_Malloc(sizeof(Pr_ListOfBACnetDestination_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstRecipient == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstRecipient, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetDestination_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstRecipient;
        }
        break;
        case BACNET_DT_EVENTPARAMETERS :
        {
            Pr_BACnetEventParameter_t *pstEventParaEncode = NULL;
            pstEventParaEncode = OSAL_Malloc(sizeof(Pr_BACnetEventParameter_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstEventParaEncode == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstEventParaEncode, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetEventParameter_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstEventParaEncode;
        }
        break;
        case BACNET_DT_RECIPIENT_LIST :
        {
            Pr_ListOfBACnetRecipient_t *pstRecipient = NULL;
            pstRecipient = OSAL_Malloc(sizeof(Pr_ListOfBACnetRecipient_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstRecipient == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstRecipient, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetRecipient_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstRecipient;
        }
        break;
        case BACNET_DT_DATELIST :
        {
            Pr_ListOfBACnetCalendarEntry_t   *pstDateList = NULL;
            pstDateList = OSAL_Malloc(sizeof(Pr_ListOfBACnetCalendarEntry_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstDateList == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstDateList, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetCalendarEntry_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstDateList;
        }
        break;
        case BACNET_DT_DEVOBJPROPREFF_LIST :
        {
            Pr_ListOfBACnetDevObjPropRef_t    *pstListOfObjectPropertyReference = NULL;
            pstListOfObjectPropertyReference = 
                OSAL_Malloc(sizeof(Pr_ListOfBACnetDevObjPropRef_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstListOfObjectPropertyReference == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstListOfObjectPropertyReference, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetDevObjPropRef_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstListOfObjectPropertyReference;
        }
        break;
        case BACNET_DT_DAILYSCHEDULE_ARRAY :
        {
            Pr_ListOfBACnetDailySchedule_t   *pstWeeklySchedule = NULL;
            pstWeeklySchedule = OSAL_Malloc(sizeof(Pr_ListOfBACnetDailySchedule_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstWeeklySchedule == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstWeeklySchedule, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetDailySchedule_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstWeeklySchedule;
        }
        break;
        case BACNET_DT_SPECIALEVENT_ARRAY :
        {
            Pr_ListOfBACnetSpecialEvent_t      *pstExceptionSchedule = NULL;
            pstExceptionSchedule = OSAL_Malloc(sizeof(Pr_ListOfBACnetSpecialEvent_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstExceptionSchedule == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstExceptionSchedule, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetSpecialEvent_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstExceptionSchedule;
        }
        break;
        case BACNET_DT_LOGBUFFER_TRENDLOG :
	    {	
            Pr_ListOfBACnetLogRecord_t     *pstLogBuffer = NULL;
            pstLogBuffer = OSAL_Malloc(sizeof(Pr_ListOfBACnetLogRecord_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstLogBuffer == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstLogBuffer, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfBACnetLogRecord_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstLogBuffer;
        }
        break;
        case BACNET_DT_NOTIFICATIONPRIORITY :
	    {		
            Pr_BACnetNotifyPriority_t     *pstNotiPriority = NULL;
            pstNotiPriority = OSAL_Malloc(sizeof(Pr_BACnetNotifyPriority_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstNotiPriority == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstNotiPriority, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetNotifyPriority_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstNotiPriority;
        }
        break;
        case BACNET_DT_TIMESTAMP :
	    {
            /* FIXME - TODO - do as per type of time stamp */
            Pr_BACnetDateTime_t     *pstTimeStamp = NULL;
            pstTimeStamp = OSAL_Malloc(sizeof(Pr_BACnetDateTime_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstTimeStamp == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstTimeStamp, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetDateTime_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstTimeStamp;
        }
        break;
        case BACNET_DT_OCTETSTRING :
	    {		
            Pr_BACnetOctetStr_t     *pstOctetString = NULL;
            pstOctetString = OSAL_Malloc(sizeof(Pr_BACnetOctetStr_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstOctetString == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstOctetString, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_BACnetOctetStr_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstOctetString;
        }
        break; 
        case BACNET_DT_CHARSTRING_ARRAY :
	    {		
            Pr_ListOfCharStr_t     *pstStringList = NULL;
            pstStringList = OSAL_Malloc(sizeof(Pr_ListOfCharStr_t),  __FILE__, __FUNCTION__, __LINE__);
            if(pstStringList == NULL)
            {
				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
				#endif

				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            memcpy(pstStringList, pstActiveCOVList->m_stCOVPropertyElem.m_pvCurrentValue,
                sizeof(Pr_ListOfCharStr_t));
            pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue = pstStringList;
        }
        break;
        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
			#endif

			/* free malloced memory */
			OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
			/* free malloced memory */
			OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
            return NULL;
        }
    }
    /* If more than one property is to be reported allocate memory for it only For COV */
   /* if(pstCOVSubscribe->m_stCOVPropertyElem.m_ePropertyId == PROP_PRESENT_VALUE || 
        pstCOVSubscribe->m_stCOVPropertyElem.m_ePropertyId == PROP_STATUS_FLAGS)
    {*/

        /* Copy the respective data */
        if(NULL != pstActiveCOVList->m_stCOVPropertyElem.m_pstNext)
        {
            pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext =
                OSAL_Malloc(sizeof(CovPropElement_t),  __FILE__, __FUNCTION__, __LINE__);
            if(NULL == pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext)
            {
				/* free malloced memory */
				OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
				/* free malloced memory */
				OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }

            memcpy(pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext,
                pstActiveCOVList->m_stCOVPropertyElem.m_pstNext,
                sizeof(CovPropElement_t));
            pstActiveCOVElem = pstActiveCOVList->m_stCOVPropertyElem.m_pstNext;
            pstCOVPropertyElem = pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext;
        
            if(pstActiveCOVElem->m_eData_Type == BACNET_DT_BITSTRING)
            {
				/* local variable */
                Pr_BACnetBitStr_t *pbsValue = NULL;
				Pr_BACnetBitStr_t stCurrentValue = {0};
                pbsValue = OSAL_Malloc(sizeof(Pr_BACnetBitStr_t),  __FILE__, __FUNCTION__, __LINE__);
                if(pbsValue == NULL)
                {
					#ifdef DEBUG_PRINTF
		            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: In Get_Notification_Parameter: malloc error \r\n");
					#endif

					/* free malloced memory */
					OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
					/* free malloced memory */
					OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                    return NULL;
                }

				/* copy bit status flag data from message queue */
				if(bCopyData)
				{
					memcpy(&stCurrentValue.m_stBitStr, &pstMsgQueData->m_stStatusFlags, sizeof(BACnetBitStr_t));
					memcpy(pbsValue, &stCurrentValue, sizeof(Pr_BACnetBitStr_t));
				}
				else
				{
					memcpy(pbsValue, pstActiveCOVElem->m_pvCurrentValue, sizeof(Pr_BACnetBitStr_t));
				}
                pstCOVPropertyElem->m_pvStoredValue = pbsValue;
            }
        }

        pstActiveCOVElem = pstActiveCOVList->m_stCOVPropertyElem.m_pstNext;
        /* for loop object has four property in notifications */
        if( pstActiveCOVElem  != NULL  && NULL != pstActiveCOVElem->m_pstNext)
        {
            while(NULL != pstActiveCOVElem->m_pstNext)
            {
                pstCOVPropertyElem->m_pstNext =
                OSAL_Malloc(sizeof(CovPropElement_t),  __FILE__, __FUNCTION__, __LINE__);
                if(NULL ==  pstCOVPropertyElem->m_pstNext)
                {
					/* free malloced memory */
					OSAL_Free(pstCOVSubscribe, __FILE__, __FUNCTION__, __LINE__);
					/* free malloced memory */
					OSAL_Free(pstSubscriberInfo, __FILE__, __FUNCTION__, __LINE__);
                    return NULL;
                }
                memcpy( pstCOVPropertyElem->m_pstNext,
                pstActiveCOVElem->m_pstNext,
                sizeof(CovPropElement_t));
                pstActiveCOVElem = pstActiveCOVElem->m_pstNext;
                pstCOVPropertyElem = pstCOVPropertyElem->m_pstNext;
        
                if(pstActiveCOVElem->m_eData_Type == BACNET_DT_REAL)
                {
                    Pr_BACnetReal_t *prValue;
                    prValue = OSAL_Malloc(sizeof(Pr_BACnetReal_t),  __FILE__, __FUNCTION__, __LINE__);

                    memcpy(prValue, pstActiveCOVElem->m_pvCurrentValue,
                        sizeof(Pr_BACnetReal_t));
                    pstCOVPropertyElem->m_pvStoredValue = prValue;
                }                
                if(pstActiveCOVElem->m_eData_Type == BACNET_DT_DATETIME)
                {
                    Pr_BACnetDateTime_t *prValue;
                    prValue = OSAL_Malloc(sizeof(Pr_BACnetDateTime_t),  __FILE__, __FUNCTION__, __LINE__);

                    memcpy(prValue, pstActiveCOVElem->m_pvCurrentValue,
                        sizeof(Pr_BACnetDateTime_t));
                    pstCOVPropertyElem->m_pvStoredValue = prValue;
                }   
            }
            pstCOVPropertyElem->m_pstNext = NULL;
        } 

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Get_Notification_Parameter : exit \r\n");
	#endif

    /* Return Pointer to the allocated memory */
    return pstCOVSubscribe;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Encodes the COV Notification to be send. 
*    
*@param pstProcQIndex [in]  Pointer to processInfo_t structure.
*@param pu8APDUResp   [out] Pointer to Transmit buffer.
*@param i32APDULen    [out] Transmit buffer fill Index
*
*@return Number of Bytes consumed. 
*                                                                      
*/
int32_t COVB_Notify_Encoder(processInfo_t *pstProcQInfo, uint8_t *pu8APDUResp,
                           int32_t i32APDULen)
{
    ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;
    CovPropElement_t *pstCOVPropElem = NULL;
    virtualDevData_t *pVirtualDev = NULL;
    //void *pvPropStoredVal = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COVB_Notify_Encoder : entry \r\n");
	#endif

    pVirtualDev = pstProcQInfo->m_pvReqDevStruct;
    pstCOVSubscribe =
        pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
    if(pstCOVSubscribe == NULL || pVirtualDev == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COVB_Notify_Encoder : pstCOVSubscribe ="
            "NULL Input Pointers\r\n");
		#endif

        return 0;
    }
    pstCOVPropElem = &(pstCOVSubscribe->m_stCOVPropertyElem);
    if(pstCOVPropElem == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COVB_Notify_Encoder : pstCOVPropElem ="
            "NULL Input Pointers\r\n");
		#endif

        return 0;
    }
    
    /* Encode Process ID */
    i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen],TAG_NO_0 , 
        pstCOVSubscribe->m_pstSubscriberInfo->m_u32ProcessId);
    /* Encode Device ID */
    i32APDULen += Encode_Context_Object_Id(&pu8APDUResp[i32APDULen], TAG_NO_1,
		(int32_t)pstCOVSubscribe->m_pstSubscriberInfo->m_stObjId.m_eObjectType, 
        pstCOVSubscribe->m_pstSubscriberInfo->m_stObjId.m_u32ObjId);
    /* Encode Object ID */
    i32APDULen += Encode_Context_Object_Id(&pu8APDUResp[i32APDULen], TAG_NO_2,
		(int32_t)pstCOVSubscribe->m_stObjId.m_eObjectType, 
        pstCOVSubscribe->m_stObjId.m_u32ObjId);
    /* Encode Life Time */
    i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen], TAG_NO_3, 
        pstCOVSubscribe->m_pstSubscriberInfo->m_u32TimeRemaining);
    /* Encode Opening Tag */
    i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_4);

    /* Encode Present Value based on Data Type */
    while(NULL != pstCOVPropElem)
    {
        i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen], TAG_NO_0,
                pstCOVPropElem->m_ePropertyId); 
		i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_2);        

        if(pstCOVPropElem->m_eData_Type == BACNET_DT_DESTINATION_LIST ||
            pstCOVPropElem->m_eData_Type == BACNET_DT_EVENTPARAMETERS ||
            pstCOVPropElem->m_eData_Type == BACNET_DT_DEVOBJPROPREFF_LIST ||
            pstCOVPropElem->m_eData_Type == BACNET_DT_DAILYSCHEDULE_ARRAY || 
            pstCOVPropElem->m_eData_Type == BACNET_DT_SPECIALEVENT_ARRAY ||
            pstCOVPropElem->m_eData_Type == BACNET_DT_LOGBUFFER_TRENDLOG ||
            pstCOVPropElem->m_eData_Type == BACNET_DT_DATELIST ||
			pstCOVPropElem->m_eData_Type == BACNET_DT_LOGBUFFER_TLM ||
            pstCOVPropElem->m_eData_Type == BACNET_DT_READACCESSSPECS_LIST)       
		{  
		    void *pvPropStoredVal = NULL;
			pvPropStoredVal = GetListStoredValue(pstProcQInfo);
            if(NULL == pvPropStoredVal)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COVB_Notify_Encoder : pstCOVSubscribe ="
                    "NULL Input Pointers\r\n");
				#endif

                return 0;
            }           
            i32APDULen += BACApp_Encode_Data_Type(&pu8APDUResp[i32APDULen], 
                MAX_APDU_LENGTH_ACCEPTED, 
                pvPropStoredVal, pstCOVPropElem->m_eData_Type, pstCOVPropElem->m_u32PropertyArrayIndex,
                i32APDULen, pVirtualDev->m_stDevObject.m_stSegmentationSupport.m_eSegmentationSupport,
                pstCOVPropElem->m_bArrIndxresent,
                pVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val, NULL, NULL, 0);
		}
        else
        {
            i32APDULen += BACApp_Encode_Data_Type(&pu8APDUResp[i32APDULen], 
            MAX_APDU_LENGTH_ACCEPTED, 
            pstCOVPropElem->m_pvStoredValue, pstCOVPropElem->m_eData_Type, pstCOVPropElem->m_u32PropertyArrayIndex,
            i32APDULen, pVirtualDev->m_stDevObject.m_stSegmentationSupport.m_eSegmentationSupport,
            pstCOVPropElem->m_bArrIndxresent,
            pVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val, NULL, NULL, 0);		
        }
        i32APDULen += Encode_Closing_Tag(&pu8APDUResp[i32APDULen], TAG_NO_2);

        pstCOVPropElem = pstCOVPropElem->m_pstNext;
    }
    /* Encode Closing tag */
    i32APDULen += Encode_Closing_Tag(&pu8APDUResp[i32APDULen], TAG_NO_4);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COVB_Notify_Encoder : exit \r\n");
	#endif

    return i32APDULen;
}


/**                                                                         
*DESCRIPTION  
*
*   Api to clear active cov subscription list (of device object) 
*   elements after their lifetime expires.
*   The API gets executed when a Timer ISR Occurs.
*
*@return None
*/
void DvCOV_B_Cancellation_Thread_Task(void)
{
    virtualDevData_t *pstVirtualDev = NULL;
    ListOfBACnetCovSubs_t *pstActiveCOVList = NULL;
    CovRecipientInfo_t *pstRecipientInfo = NULL;
    ListOfBACnetCovSubs_t **pstActiveCOVAdd = NULL;
    CovRecipientInfo_t **pstRecipientAdd = NULL;
    ListOfBACnetCovSubs_t *pstNxtCOVElem = NULL;
    CovPropElement_t *pstCOVPropElem = NULL;
    CovPropElement_t *pstTempCOVPropElem = NULL;
    void *pvObjectIndex = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DvCOV_B_Cancellation_Thread: entry \r\n");
	#endif
	
    //pstVirtualDev = lpThreadParameter;
	pstVirtualDev = gstHostDevice.m_pstDeviceStruct;

    for(;;)
    {
        BOOL bStatus = FALSE;        
           
        /* Wait till the g_hCOVCancelSemaphore released by COV Timer */
        OSAL_Wait_Sem(m_hCOVCancelSemaphoreHandle, INFINITE);

        pstActiveCOVAdd = &(pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.
            m_pstCOVSubscribe);
        pstActiveCOVList =
            pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe;
        
        /* Enter only if a subscription exists */
        if(pstActiveCOVList != NULL)
        {
            /* Continue until there is data in subscription list */
            while(NULL != pstActiveCOVList)
            {
                pstRecipientAdd = &(pstActiveCOVList->m_pstSubscriberInfo);
                pstRecipientInfo = pstActiveCOVList->m_pstSubscriberInfo;
                
                /* Continue until there is data in recipient list */
                while(NULL != pstRecipientInfo)
                {
                    /* find the object */
                    pvObjectIndex = Find_Object(
										pstActiveCOVList->m_stObjId.m_eObjectType,
                                        pstActiveCOVList->m_stObjId.m_u32ObjId, pstVirtualDev);
                    if(NULL == pvObjectIndex)
                    {
                        /* if not found cancle its subscription */
                        if(pstActiveCOVList->m_pstSubscriberInfo->m_stAddress.u8IpAddrs[3] == 0xFF)
                        {
                            /* cancle  COV-U subscription */
							#if (defined BACDEL_SER_DS_COVU_B)
							Cancel_COVU_Subscription(pstVirtualDev,
								pstActiveCOVList->m_stObjId.m_eObjectType,
                                pstActiveCOVList->m_stObjId.m_u32ObjId);
							#else
							;
							#endif
                        }
                        else
                        {
                            /* cancle  COV / COVP subscription */
                           pstRecipientInfo->m_i8Subscribe = FALSE;
                        }
                    }

                    /* If the Lifetime is non zero decrement the value by 1 */
                    if((pstRecipientInfo->m_u32TimeRemaining > 1) &&
                        (pstRecipientInfo->m_i8Subscribe == TRUE))
                    {
                        pstRecipientInfo->m_u32TimeRemaining =
                            (pstRecipientInfo->m_u32TimeRemaining - 1);
                    }
                    /* If the lifetime is 1 or cancel request is received */
                    else if((pstRecipientInfo->m_u32TimeRemaining == 1) ||
                        (pstRecipientInfo->m_i8Subscribe == FALSE))
                    {
                        /* Lock Mutex */
                        if(Osal_Wait_Mutex(m_hcovsublistMtxLockHandle, INFINITE)
                            != WAIT_OBJECT_0)
        			    {
							;							
							#ifdef DEBUG_PRINTF
		        		    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
								DvCOV_B_Cancellation_Thread: Mutex Falied\r\n");
							#endif				
			            }
                        /* Delete the recipient info from recipient list */
                        bStatus = Delete_COV_Subscription(pstRecipientInfo,
                            pstRecipientAdd);
                        /* Update the pstRecipientInfo parameter */
                        if(bStatus)
                        {
                            pstRecipientInfo = *pstRecipientAdd;
							/* decrement count after cov deletion */
							if(pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_u32Count > 0)
							{
								/* decrement by one */
								pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_u32Count--;
							}
                        }

                        /* Unlock Mutex */
                        Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

                        // Prashant -Commenting out OS specific call
		                //Osal_Release_Mutex(pstSubscriptionAdd->m_hMtxLock);
                    }

                    /* If there is only one recipient in the list */
                    if(pstRecipientInfo == NULL)
                    {
                        break;
                    }
                    else if(pstRecipientInfo->m_pstNext == NULL)
                    {
                        break;
                    }
                    else
                    {
                        pstRecipientAdd = &(pstRecipientInfo->m_pstNext);
                        pstRecipientInfo = pstRecipientInfo->m_pstNext;
                    }
                }
                /* If recipient is Deleted */ 
                if(bStatus)
                {
                    /* There was only one recipient in the list delete the entry
                     * from ActiveCOVSubscription list */
                    if(NULL == pstActiveCOVList->m_pstSubscriberInfo)
                    {
                        int32_t i32NULL = 0;
                        if(Osal_Wait_Mutex(m_hcovsublistMtxLockHandle, INFINITE)
                            != WAIT_OBJECT_0)
        			    {
							;
							#ifdef DEBUG_PRINTF
		        		    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
								DvCOV_B_Cancellation_Thread: Mutex Falied\r\n");
							#endif
			            }
                        pvObjectIndex = Find_Object(
							pstActiveCOVList->m_stObjId.m_eObjectType,
                                        pstActiveCOVList->m_stObjId.m_u32ObjId, pstVirtualDev);
                        pstCOVPropElem = &(pstActiveCOVList->m_stCOVPropertyElem);
                        while(NULL != pstCOVPropElem)
                        {
                            /* Free the stored Memory Location */
                            OSAL_Free(pstCOVPropElem->m_pvStoredValue, __FILE__, 
                                __FUNCTION__, __LINE__);
                            /* Extract the Pointer to the cov element of the 
                            respective property */
                            pstCOVPropElem->m_pvStoredValue =
                                Validate_And_Get_Property_COV_Addr(pstActiveCOVList->m_stObjId.
								m_eObjectType,
                                pvObjectIndex, pstCOVPropElem->m_ePropertyId);
                            if(NULL == pstCOVPropElem->m_pvStoredValue)
                            {
                                break;//return FALSE;
                            }
                            if(NULL != pvObjectIndex)
                            {
                                memcpy(pstCOVPropElem->m_pvStoredValue, &i32NULL,
                                    sizeof(int32_t));
                            }
							if(pstActiveCOVList->m_stObjId.m_eObjectType != OBJECT_DEVICE &&
								pstActiveCOVList->m_stObjId.m_eObjectType != OBJECT_CALENDAR &&
								pstActiveCOVList->m_stObjId.m_eObjectType != OBJECT_FILE &&
								pstActiveCOVList->m_stObjId.m_eObjectType != OBJECT_NOTIFICATION_CLASS &&
								pstActiveCOVList->m_stObjId.m_eObjectType != OBJECT_EVENT_ENROLLMENT &&
								pstActiveCOVList->m_stObjId.m_eObjectType != OBJECT_LOOP)
                            {
                                if(NULL ==  pstCOVPropElem->m_pstNext)
                                {
                                    /* Free the stored Memory Location */
                                    OSAL_Free(pstCOVPropElem, __FILE__, __FUNCTION__, __LINE__);
                                    break;
                                }
                            }
                            /* Loop object has 4 prop in notification */
							if(pstActiveCOVList->m_stObjId.m_eObjectType == OBJECT_LOOP)
                            {   /* 1st element is static */    
                                if(pstCOVPropElem->m_ePropertyId != PROP_PRESENT_VALUE &&
                                    (pstCOVPropElem->m_ePropertyId == PROP_SETPOINT ||
                                    pstCOVPropElem->m_ePropertyId == PROP_CONTROLLED_VARIABLE_VALUE ||
                                    pstCOVPropElem->m_ePropertyId == PROP_STATUS_FLAGS))
                                {
                                    pstTempCOVPropElem = pstCOVPropElem->m_pstNext;
                                    OSAL_Free(pstCOVPropElem, __FILE__, __FUNCTION__, __LINE__);
                                    pstCOVPropElem = pstTempCOVPropElem;
                                }
                                else/* 1st element is static */
                                    pstCOVPropElem = pstCOVPropElem->m_pstNext;
                            }
                            else /* all object other than Loop (obj with status flag prop) */
                            {
                                /* Point to the next Subscribed COV Index */
                                pstCOVPropElem = (CovPropElement_t *)
                                    pstCOVPropElem->m_pstNext;
                            }
                        }
                        /* Update pstActiveCOVList */
                        pstNxtCOVElem = pstActiveCOVList->m_pstNext;
                        OSAL_Free(*pstActiveCOVAdd,  __FILE__, __FUNCTION__, __LINE__);
                        *pstActiveCOVAdd = pstNxtCOVElem;
                        pstActiveCOVList = *pstActiveCOVAdd;
                        /* Unlock Mutex */
		                Osal_Release_Mutex(m_hcovsublistMtxLockHandle);
                    }
                }
                /* If there is no next element exit */
                if(NULL == pstActiveCOVList)
                {
                    break;
                }
                else if(pstActiveCOVList->m_pstNext == NULL)
                {
                    break;
                }
                else
                {
                    pstActiveCOVAdd = &(pstActiveCOVList->m_pstNext);
                    pstActiveCOVList = pstActiveCOVList->m_pstNext;
                }
            }
        }
   }// for(;;) ends

   /* function exit */
   #ifdef DEBUG_PRINTF
   Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: DvCOV_B_Cancellation_Thread : exit \r\n");
   #endif
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Encodes the COV Notification to be send. 
*    
*@param pstRecipientInfo [in]  Pointer to CovRecipientInfo_t struct which
*                              is going to be deleted.
*@param pstRecipientAdd  [out] Pointer to address of m_pstNext element of 
*                              CovRecipientInfo_t.
*
*@return BOOL. 
*                                                                      
*/
BOOL Delete_COV_Subscription(CovRecipientInfo_t *pstRecipientInfo,
                         CovRecipientInfo_t **pstRecipientAdd)
{
    CovRecipientInfo_t *pstNxtCOVElem = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Delete_COV_Subscription: entry \r\n");
	#endif
    
    /* Extract the address of the next recipient structure */
    pstNxtCOVElem = pstRecipientInfo->m_pstNext;
    /* Free the current the present recipient structure */
    OSAL_Free(*pstRecipientAdd,  __FILE__, __FUNCTION__, __LINE__);
    /* ReAssign the CovRecipientInfo_t m_pstNext element with 
     * the next recipient structure */
    *pstRecipientAdd = pstNxtCOVElem;
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Delete_COV_Subscription: exit \r\n");
	#endif
	
    return TRUE;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Informs if the Object type supports COV.
*    
*@param pvObject [in] Pointer to the Object.
*@param eObjectType [in] Object Type.
*
*@return TRUE/FALSE
*                                                                      
*/
BOOL COV_Object_Support(void *pvObject, BACNET_OBJECT_TYPE eObjectType)
{
    ulong32_t ul32BaseAddr = 0;
    ulong32_t ul32OffsetAddr = 0 ;
    int32_t i32ObjCovSupport = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COV_Object_Support: entry \r\n");
	#endif

    /* To access memory location type cast void pointer */
    ul32BaseAddr = (ulong32_t )pvObject;
    /* If m_i32ObjCOVSupport is -1 i.e. COV is not supported
     * If m_i32ObjCOVSupport is 0 i.e. COV is supported
     */
    GET_OBJCOMMON_OFFSET(eObjectType, m_i32ObjCOVSupport, ul32OffsetAddr);
    memcpy((void *)&i32ObjCovSupport, (void *)(ul32BaseAddr+ul32OffsetAddr),
                                             sizeof(int32_t));
    if(BACNET_ZERO == i32ObjCovSupport)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COV_Object_Support: exit \r\n");
		#endif

        return true;   
    }

	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COV_Object_Support: COV not supported \r\n");
	#endif

    return false;
}

/**
*                                                                    
*DESCRIPTION                                                                          
*   Provides the address of the specific instance number of an object.
*    
*@param pvObject [in/out] Pointer to the Object.
*@param ObjectID [in] Instance number.
*@param eObjectType [in] Object Type.
*
*@return TRUE/FALSE
*                                                                      
*/
void *Get_Object_Instance_Pointer(void *pvObject, uint32_t ObjectID,
                                    BACNET_OBJECT_TYPE  eObjectType)
{
    ulong32_t ul32BaseAddr = 0;
    ulong32_t ul32OffsetAddr = 0 ;
    void * pvNextOffset = 0;
	void ** pvTemp = NULL;
	uint32_t u32ObjId = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Get_Object_Instance_Pointer: entry \r\n");
	#endif
    
    /* To access memory location type cast void pointer */
    ul32BaseAddr = (ulong32_t)pvObject;
    
    /* Traverse the object list */
    while(BACNET_ZERO != ul32BaseAddr)
    {        
        /* Extract the Object Instance Number from the respective objects */
		GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32OffsetAddr);
        memcpy((void *)&u32ObjId, (void *)(ul32BaseAddr+ul32OffsetAddr),
                                             sizeof(uint32_t));
        /* If Instance number matches, return the address */
        if(u32ObjId == ObjectID)
        {
            pvObject = (void *)ul32BaseAddr;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Get_Object_Instance_Pointer: \
									Object instance found \r\n");
			#endif

            return pvObject;
        }

        /* If Instance number does not matches move to Next object Pointer */
		GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32OffsetAddr);

        /* Copy next instance address stored in "->pstNext" */
		pvTemp = (void *)(ul32BaseAddr+ul32OffsetAddr);
		pvNextOffset = *pvTemp;

        /* If the next address feild contain NULL return NULL */
        if(NULL == pvNextOffset)
        {
            pvObject = NULL;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Get_Object_Instance_Pointer: \
									Object instance not found \r\n");
			#endif

            return pvObject;
        }
        /* Reassign the address to traverse the object list */
        ul32BaseAddr = (ulong32_t)pvNextOffset;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Get_Object_Instance_Pointer: exit \r\n");
	#endif

    return NULL;
}

/**
*DESCRIPTION
*   Based on Object Type this API returns pointer to the address of the 
*   Object Instance requested after traversing object linked list.
*
*@param eObjectType [in] Object Type
*@param pstVirtualDev [in] Pointer to Virtual Device struct
*@param ObjectID [in] Object Instance Number
*
*@return VOID pointer of specific object type & object instance / NULL 
*
*/
void *Extract_Object_Address(BACNET_OBJECT_TYPE  eObjectType,
                                  uint32_t ObjectID,
                                  virtualDevData_t *pstVirtualDev,
                                  processInfo_t * pstProcQInfo)
{
    /* 
     * Follow the Steps below:- 
     * 1) Based on Object type retrive the base address pointer. 
     * 2) If the retrived pointer value is NULL update the error response 
     *    packet & EXIT otherwise continue.
     * 3) Extract the pointer to the specific instance.
     * 4) Check if the specific instance of the object is supported.
     * 5) If supported return 3 otherwise return NULL.
     */
    
    void *pvObject = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Extract_Object_Address: entry \r\n");
	#endif
    
    /** Find The Object Instance Existance  */
    pvObject = Find_Object(eObjectType, ObjectID, pstVirtualDev);

    /* If instance of the object is not created or Object is not present */
    if(pvObject == NULL)
    {
        /* To send error response of object not supported */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ERROR, ERROR_CLASS_OBJECT,
            ERROR_CODE_UNKNOWN_OBJECT);

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_Object_Address: \
									Unsupported object type \r\n");
		#endif

        /* To send error response return NULL value */
        return NULL;
    }
    else
    {
        /* Check if the Object supports COV subscription */
        if(COV_Object_Support(pvObject, eObjectType))
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_Object_Address: \
								COV supported by object\r\n");
			#endif

			return pvObject;
        }
        else
        {
             /* To send error response if COV not supported for Object */
            Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_ERROR, ERROR_CLASS_OBJECT, ERROR_CODE_OTHER);

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_Object_Address: \
								COV not supported by object type \r\n");
			#endif

            return NULL;
        }
    }
	/* below statement would never execute as return is called 1st in all cases */
// /* function exit */
}


/**
*DESCRIPTION
*   Check if the object supports the property.If no return NULL otherwise
*   Get the address of the COV property of specific object.
*
*@param eObjectType [in] Object Type
*@param pvObject [in/out] Pointer to Object list
*@param ePropertyId [in] Property to be found.
*
*@return VOID pointer of specific object type & object instance / NULL 
*
*/
void *Validate_And_Get_Property_COV_Addr(BACNET_OBJECT_TYPE eObjectType, void *pvObject,
                       BACNET_PROPERTY_ID ePropertyId)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
						Validate_And_Get_Property_COV_Addr: entry \r\n");
	#endif
    /* 
     * 1) Check if the property is supported.
     * 2) Check if the property supports COV subscription.
     * 3) Yes, return the pointer to the COV subscription otherwise 
     *    return NULL.
     */
    if(NOT_SUPPORTED == GetDefndPropAccess(eObjectType, ePropertyId,
        BACNET_DEFAULT))
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
						Validate_And_Get_Property_COV_Addr: property not supported \r\n");
		#endif

        return NULL;
    }
    else
    {        
        pvObject = Get_ActiveCOV_Value_Pointer(pvObject, eObjectType, ePropertyId);   
        if(NULL == pvObject)
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
						Validate_And_Get_Property_COV_Addr: Null property pointer \r\n");
			#endif

            return NULL;
        }

		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
						Validate_And_Get_Property_COV_Addr: exit \r\n");
		#endif

        return pvObject;
    }
}

/**
*DESCRIPTION
*   Provides address of m_i32ActiveCOV element of a property.
*
*@param pvObject [in/out] Pointer to the ObjectID in Device structure.
*@param eObjectType [in] Object Number.
*@param eDevProp [in] Property ID.
*
*@return Pointer to the m_i32ActiveCOV element of requested Property of
*        requested object ID in Device structure.
*
*/
void *Get_ActiveCOV_Value_Pointer(void *pvObject, BACNET_OBJECT_TYPE eObjectType,
                          BACNET_PROPERTY_ID eDevProp)
{
    ulong32_t ul32BaseAddr = 0;
    ulong32_t ul32OffsetAddr = 0;
    int32_t *pu32COVvalue = 0;
    
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
			Get_ActiveCOV_Value_Pointer: entry \r\n");
	#endif
	
    /* Get the Structure BaseAddress */
    ul32BaseAddr = (ulong32_t )pvObject;
      
    if(eObjectType != OBJECT_DEVICE)
    {
        /* Based on Property Type */
        switch(eDevProp)
        {
			case PROP_STATUS_FLAGS:
			{
				/* Get property offset */
				GET_STATUSFLAG_OFFSET(eObjectType, m_stStatusFlag.m_i32ActiveCOV,
				ul32OffsetAddr);
			}
			break;
			case PROP_PRESENT_VALUE:
			{
				/* Get Offset for  member of Present Value Property */
				GET_PV_ELEMENT_OFFSET(eObjectType, m_stPresentValue.m_i32ActiveCOV,
				ul32OffsetAddr);
			}
			break;
#ifdef BACDEL_PR18
			case PROP_CURRENT_COMMAND_PRIORITY:
			{
				/* Get property offset */
				GET_RELINQUISH_OFFSET(eObjectType,
					m_stCurrentCmdPriority.m_i32ActiveCOV, ul32OffsetAddr);
			}
			break;
#endif
			/* default case */
            default:
            {   
                /* No property found */
                ul32OffsetAddr = 0;
            }
            break;
        }
    }

    if(ul32OffsetAddr)
    {
        pu32COVvalue = (int32_t *)(ul32BaseAddr+ul32OffsetAddr);
        pvObject = (void *)pu32COVvalue;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
			Get_ActiveCOV_Value_Pointer: COV supported by property\r\n");
		#endif

        return (pvObject);
    }
    /* If element is not present in property */
    else
    {
        pvObject = NULL;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Get_ActiveCOV_Value_Pointer: Property is not supported \r\n");
		#endif

        return (pvObject);
    }
}


/**
*DESCRIPTION
*   Extracts the address location & index number where new subscription is
*   to be added.
*
*@param pu32COVListType [out] Notifies if a new subscription/resubscription
*                             /new recipient is to be added.
*@param stObjId [in] Instance number & Object Id requested.
*@param ePropertyID [in] Property ID requested.
*@param pstVirtualDev [in/out] Pointer to Virtual Device struct.
*
*@return Pointer to the ActiveCOVSubscription list / NULL.
*
*/
void *Extract_COV_Subscription_Index(BACNET_COV_SUBS *peCovSubType,
									 uint32_t *pu32CovIndex,
                                     ListOfBACnetCovSubs_t *pstCOVSubscribe, 
                                     BACNET_PROPERTY_ID ePropertyID,
                                     virtualDevData_t *pstVirtualDev)
{
    /*  
     * 1) Check if the requested Property with ObjectID is registered.
     * 2) If registered provide the pointer to the address
     * 3) If not registered provide pointer to a new location.
     */
    ListOfBACnetCovSubs_t *pstActiveCOVList = NULL;
	CovRecipientInfo_t *pstRecipientInfo = NULL;
    uint16_t    u16ObjectType;
    uint32_t    u32ObjId;
    uint32_t u32PropertyArrayIndex = BACNET_ARRAY_ALL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Extract_COV_Subscription_Index: Entry \r\n");
	#endif

	/* lock the mutex */
    Osal_Wait_Mutex(m_hcovsublistMtxLockHandle, INFINITE);

    /* Extract the Active COV subscription base pointer */
    pstActiveCOVList = 
        pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe;
	
    /* Get Monitored Object ID */
	u16ObjectType = pstCOVSubscribe->m_stObjId.m_eObjectType;
	u32ObjId = pstCOVSubscribe->m_stObjId.m_u32ObjId;
    u32PropertyArrayIndex = pstCOVSubscribe->m_stCOVPropertyElem.m_u32PropertyArrayIndex;

    /* If list does not has any entry, Add 1st entry in List */
    if(pstActiveCOVList == NULL)
    {
		//*pu32COVListType = 4;
        *peCovSubType = ADD_NEW_REQ;
		*pu32CovIndex = BACNET_ONE;
        pstActiveCOVList = 
            OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t),  __FILE__, __FUNCTION__, __LINE__);
        if(NULL == pstActiveCOVList)
        {
			/* release the mutex */
			Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_COV_Subscription_Index: Null pstActiveCOVList value\r\n");
			#endif

            return NULL;
        }

        pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe = pstActiveCOVList;

		/* release the mutex */
		//Osal_Release_Mutex(pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_hcovsublistMtxLock);
		
		Osal_Release_Mutex(m_hcovsublistMtxLockHandle);
		
		/* function exit */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Extract_COV_Subscription_Index: exit \r\n");
		#endif

        return ((void*) pstActiveCOVList);
    }

    /* check ObjID, ObjType n ProcessID, if matched check Recipient address with sender
        else Add new Link for New ObjID */
    while(pstActiveCOVList != NULL)
    {
        /* Check if the subscription exist */
        if((pstActiveCOVList->m_stCOVPropertyElem.m_ePropertyId == ePropertyID) &&
           (pstActiveCOVList->m_stObjId.m_u32ObjId == u32ObjId) &&
		   (pstActiveCOVList->m_stObjId.m_eObjectType == u16ObjectType) &&
           (pstActiveCOVList->m_stCOVPropertyElem.m_u32PropertyArrayIndex == u32PropertyArrayIndex))
        {
            /* Check for subscriberinfo match. If no match is found assign
             * a new location in subscriberinfo */
            //*pu32COVListType += ((pstActiveCOVList->m_i32Index) << 2);
			*pu32CovIndex = pstActiveCOVList->m_i32Index;
            
            pstRecipientInfo = 
                Traverse_COV_Recipient_List(pstVirtualDev,pstActiveCOVList, 
                        peCovSubType, pstCOVSubscribe);

            if(NULL == pstRecipientInfo)
            {
				/* release the mutex */
				//Osal_Release_Mutex(pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_hcovsublistMtxLock);
				
				Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_COV_Subscription_Index: subscription not found \r\n");
				#endif

                return NULL;
            }
			/* release the mutex */
			Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

			#ifdef DEBUG_PRINTF
	        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_COV_Subscription_Index: Subscription found \r\n");
			#endif

            return ((void *)pstRecipientInfo);
        }
        else if(NULL == pstActiveCOVList->m_pstNext)
        {
            /* No matching subscription exist assign a new location in 
             * ListOfBACnetCovSubs_t */
            *peCovSubType = ADD_NEW_REQ;
            //*pu32COVListType += (((pstActiveCOVList->m_i32Index)+1) << 2);
			*pu32CovIndex = pstActiveCOVList->m_i32Index + 1;

            pstActiveCOVList->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t),  __FILE__, __FUNCTION__, __LINE__);
            if(NULL == pstActiveCOVList->m_pstNext)
            {
				/* release the mutex */
				Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

				#ifdef DEBUG_PRINTF
		        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_COV_Subscription_Index: subscription not found\r\n");
				#endif

                return NULL;
            }

			/* release the mutex */
			Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

			#ifdef DEBUG_PRINTF
	        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_COV_Subscription_Index: subscription found \r\n");
			#endif

            return ((void *)pstActiveCOVList->m_pstNext);
        }
        /* Point to the next Subscribed COV Index */
        pstActiveCOVList = pstActiveCOVList->m_pstNext;
    } 

	/* release the mutex */
	Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

    #ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Extract_COV_Subscription_Index: null pstActiveCOVList value \r\n");
	#endif

    return NULL;
}


/**
*DESCRIPTION
*   Extracts the address location where new recipient is to be added.
*
*@param pu32COVListType [out] Notifies if a new subscription/resubscription
*                             /new recipient is to be added.
*@param pstActiveCOVList [in/out] Pointer to ActiveCOVSubscription struct.
*@param pstVirtualDev [in/out] Pointer to Virtual Device struct.
*
*@return Pointer to the ActiveCOVSubscription list / NULL.
*
*/
void *Traverse_COV_Recipient_List(virtualDevData_t *pstVirtualDev, 
                                  ListOfBACnetCovSubs_t *pstActiveCOVList,
								  BACNET_COV_SUBS *peCovSubType, ListOfBACnetCovSubs_t *pstSubscribeCOV)
{
    CovRecipientInfo_t *pstRecipientInfo = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Traverse_COV_Recipient_List: entry \r\n");
	#endif
    
    pstRecipientInfo = pstActiveCOVList->m_pstSubscriberInfo;
    if(NULL == pstRecipientInfo)
    {
        return NULL;
    }
    /* Check if the requested subscription already exists */
    do
    {
        if((!memcmp(&pstSubscribeCOV->m_pstSubscriberInfo->m_stAddress,
            &pstRecipientInfo->m_stAddress, sizeof(BACnetAddress_t))) &&
            (pstSubscribeCOV->m_pstSubscriberInfo->m_u32ProcessId == 
            pstRecipientInfo->m_u32ProcessId))
        {
            *peCovSubType = RESUBSCRIP_REQ;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Traverse_COV_Recipient_List: subscription already exists \r\n");
			#endif

			/* clear the time remaining , if COV resubscribe request */
			pstRecipientInfo->m_u32TimeRemaining = 0;

            return ((void *)pstRecipientInfo);
        }
        if(NULL ==  pstRecipientInfo->m_pstNext)
        {
            break;
        }
        /* Point to the next Subscribed COV Index */
        pstRecipientInfo = pstRecipientInfo->m_pstNext;
    }while(NULL != pstRecipientInfo);

    /* If no match is found assign a new location */
    if(NULL == pstRecipientInfo->m_pstNext)
    {
        *peCovSubType = ADD_NEW_RECIPIENT;
        pstRecipientInfo->m_pstNext =
            OSAL_Malloc(sizeof(CovRecipientInfo_t),  __FILE__, __FUNCTION__, __LINE__);

        if(NULL == pstRecipientInfo->m_pstNext)
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Traverse_COV_Recipient_List: malloc error \r\n");
			#endif

            return NULL;
        }

		/* set the flag, if new recipient request */
		pstRecipientInfo->m_pstNext->m_i8Subscribe = TRUE;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Traverse_COV_Recipient_List: Added node in COV ricipient list\r\n");
		#endif

        return ((void *)pstRecipientInfo->m_pstNext);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Traverse_COV_Recipient_List: exit \r\n");
	#endif

    return NULL;
}


/**
*DESCRIPTION
*   Validate Access Type and Array Index. Extracts the address of respective
*   property value. 
*
*@param pvObjectIndex [in] Pointer to Object Structure in Device.
*@param eObjectType [in] Object type received.
*@param pstCOVproperty [in/out] Pointer to CovPropElement_t struct.
*
*@return VOID.
*
*/
void Read_Property_Value(void *pvObjectIndex, BACNET_OBJECT_TYPE eObjectType,
                         CovPropElement_t *pstCOVproperty,
						 void *pVirtualDev)
{
	BACNET_PROPERTY_ID ePropertyId;
	int32_t i32ArrayIndex = 0;
	BACNET_DATA_TYPE  *peData_Type = NULL;
	BACNET_ERROR_CODE eErrorCode = ERROR_CODE_OTHER;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Read_Property_Value: entry \r\n");
	#endif
    
    /* Update local variables */
    ePropertyId = pstCOVproperty->m_ePropertyId;
    i32ArrayIndex = pstCOVproperty->m_u32PropertyArrayIndex;
    peData_Type = &pstCOVproperty->m_eData_Type;

	eErrorCode =  Read_All_Properties(eObjectType,
										ePropertyId ,
                                        i32ArrayIndex,
                                        peData_Type ,
                                        &(pstCOVproperty->m_pvStoredValue), 
                                        pvObjectIndex,
										pVirtualDev,
                                        0/* this parameter is not used yet, can pass any value */,
                                        pstCOVproperty->m_bArrIndxresent);

    /* Copy into the Rp data structure*/
    if((eErrorCode == ERROR_CODE_UNKNOWN_PROPERTY) ||
       (eErrorCode == ERROR_CODE_INVALID_ARRAY_INDEX))
    {
		;
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Read_Property_Value:\
            No Property Found for object Id %d \r\n", eObjectType);
		#endif

        /*pstRpData->eErrorCode = eErrorCode;
        pstRpData->eErrorClass = ERROR_CLASS_PROPERTY;
        pstRpData->bErrorStatus = true;*/
    
    }

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Read_Property_Value: exit \r\n");
	#endif
}

/**
*DESCRIPTION
*    Extracts the COV request data, forward for cancellation.
*
*@param pstVirtualDev [in] Pointer to Virtual Device struct.
*@param pvObjectIndex  [in] Pointer to request ObjectID in Device struct.
*
*@return TRUE/FALSE 
*
*/
BOOL Cancel_COV_Subscription(virtualDevData_t *pstVirtualDev,
                             void *pvObjectIndex, 
                             ListOfBACnetCovSubs_t *pstCOVSubReq,
                             BACNET_ERROR_CODE *peErrorCode)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Cancel_COV_Subscription: entry \r\n");
	
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Cancel_COV_Subscription: exit \r\n");
	#endif

	return (Extract_COV_Cancellation_Index(pstCOVSubReq->m_stObjId.m_eObjectType,
		pstCOVSubReq->m_stObjId.m_u32ObjId,
		pstCOVSubReq->m_stCOVPropertyElem.m_ePropertyId, 
        pstCOVSubReq,
        &(pstVirtualDev->m_stDevObject.m_stActiveCOVSubList),
        pvObjectIndex,
        peErrorCode));
}

/**
*DESCRIPTION
*   Extracts the index of ListOfBACnetCovSubs_t struct for cancellation of
*   subscription.
*
*@param u32ObjId [in] Object whose subscription is to be cancelled.
*@param pvObjectIndex [in] Object base address.
*@param ePropertyID [in] Property whose subscription is to be cancelled.
*@param pstCOVSubsReq [in] Received Request.
*@param pstCOVSubscribeList [in] Pointer to the ActiveCOVSubscription list.
*
*@return TRUE/FALSE 
*
*/
BOOL Extract_COV_Cancellation_Index(uint16_t u16ObjectType,
	uint32_t u32ObjId, 
	BACNET_PROPERTY_ID ePropertyID,
	ListOfBACnetCovSubs_t *pstCOVSubsReq,
	Pr_ListOfBACnetCovSubs_t *pstCOVSubscribeList,
	void *pvObjectIndex,
	BACNET_ERROR_CODE *peErrorCode)
{
	/* local variables */
	ListOfBACnetCovSubs_t *pstActiveCOVList = NULL;
	CovPropElement_t *pstCOVPropElem = NULL;
	CovPropElement_t *pstPropElemNxt = NULL;
	bool bSubscriptionFound = FALSE;
	 
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Extract_COV_Cancellation_Index: entry \r\n");
	#endif

	/* wait for mutex */
	Osal_Wait_Mutex(m_hcovsublistMtxLockHandle, INFINITE);
	/* Extract the Active COV subscription base pointer */    
	pstActiveCOVList = pstCOVSubscribeList->m_pstCOVSubscribe;

	/* Check until a empty location is encountered */
	while(pstActiveCOVList != NULL)
	{     
        /* Check if the subscription exist */
        if((pstActiveCOVList->m_stCOVPropertyElem.m_ePropertyId == ePropertyID)
        && (pstActiveCOVList->m_stObjId.m_u32ObjId == u32ObjId)
		&& (pstActiveCOVList->m_stObjId.m_eObjectType == u16ObjectType))
        {
            /* Check if there are more than one recipient */
            if(Extract_Recipient_Elem(pstActiveCOVList, pstCOVSubsReq))
                bSubscriptionFound = true;

            if(NULL == pstActiveCOVList->m_pstSubscriberInfo)
            {
                /* If the number of recipient is one, delete the entry from 
                 * ListOfBACnetCovSubs_t structure otherwie do not. */
                pstCOVPropElem = &(pstActiveCOVList->m_stCOVPropertyElem);

                while(NULL != pstCOVPropElem)
                {
                    /* Extract the pointer to the cov element of the respective
                       property */
                    pstCOVPropElem->m_pvStoredValue = 
                            Validate_And_Get_Property_COV_Addr(
									pstCOVSubsReq->m_stObjId.m_eObjectType,
                                    pvObjectIndex, pstCOVPropElem->m_ePropertyId);

                    if(NULL != pstCOVPropElem->m_pvStoredValue)
                    {
						/* Free the pointer */
						OSAL_Free(pstCOVPropElem->m_pvStoredValue, __FILE__, __FUNCTION__, __LINE__);
						pstCOVPropElem->m_pvStoredValue = NULL;
					}                    

                    /* Point to the next Subscribed COV Index */
                    pstCOVPropElem = pstCOVPropElem->m_pstNext;
                }
            }

			/* break the loop if subscription found */
			if(bSubscriptionFound)
				break;
        }

        /* Point to the next Subscribed COV Index */
        pstActiveCOVList = pstActiveCOVList->m_pstNext;
    }

	/* release mutex */
	Osal_Release_Mutex(m_hcovsublistMtxLockHandle);
	/* free COV-cancellation request data even if expired or unknown subscription */
	pstCOVPropElem = pstCOVSubsReq->m_stCOVPropertyElem.m_pstNext;
	pstCOVSubsReq->m_stCOVPropertyElem.m_pstNext = NULL;

	/* free property data */
    while(pstCOVPropElem != NULL)
    {
        pstPropElemNxt = pstCOVPropElem->m_pstNext;
        OSAL_Free(pstCOVPropElem, __FILE__, __FUNCTION__, __LINE__);
        pstCOVPropElem = pstPropElemNxt;
    }

	/* free subscribe data */
    OSAL_Free(pstCOVSubsReq->m_pstSubscriberInfo,
        __FILE__, __FUNCTION__, __LINE__);
	pstCOVSubsReq->m_pstSubscriberInfo = NULL;

	/* return simple ack even if expired or unknown subscription */
	/* test case reference : 135-1-2009 => 9.10.1.5 */

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Extract_COV_Cancellation_Index: exit \r\n");
	#endif
	return bSubscriptionFound;
}

/**
*DESCRIPTION
*   Extracts the index of CovRecipientInfo_t struct for cancellation of
*   subscription.
*
*@param pstActiveCOVList [in] Pointer to ActiveCOVSubscription list.
*@param pstCOVSubscribe [in] Pointer to request received.
*
*@return TRUE/FALSE 
*
*/
bool Extract_Recipient_Elem(ListOfBACnetCovSubs_t *pstActiveCOVList,
                            ListOfBACnetCovSubs_t *pstCOVSubsReq)
{
    CovRecipientInfo_t *pstRecipientInfo = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Extract_Recipient_Elem: entry \r\n");
	#endif

    /* Extract the base address of recipient list */
    pstRecipientInfo = pstActiveCOVList->m_pstSubscriberInfo;

    while(NULL != pstRecipientInfo)
    {
        /* Check if the same recipient exists */
        if((pstRecipientInfo->m_u32ProcessId ==
            pstCOVSubsReq->m_pstSubscriberInfo->m_u32ProcessId)
            &&
            (!memcmp(&pstRecipientInfo->m_stAddress,
            &pstCOVSubsReq->m_pstSubscriberInfo->m_stAddress,
            sizeof(BACnetAddress_t))))
        {
            /* Update the Subscription flag to cancel */
            pstRecipientInfo->m_i8Subscribe = FALSE;
            return true;            
        }

        /* Point to the next Subscribed COV Index */
        pstRecipientInfo = pstRecipientInfo->m_pstNext;
    }
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Extract_Recipient_Elem: exit \r\n");
	#endif

    return false;
}

/**
*DESCRIPTION
*   Extracts the value of COVIncrement property.
*
*@param pvObjectIndex [in] Pointer to Object list.
*@param eObjectType [in] Object type to be referred.
*
*@return  void pointer of the value.
*
*/
void *Get_COVIncrementValue(void *pvObjectIndex, BACNET_OBJECT_TYPE eObjectType,
							void *pVirtualDev)
{
	CovPropElement_t stCOVproperty = {0};
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Get_COVIncrementValue: entry \r\n");
	#endif

    stCOVproperty.m_ePropertyId = PROP_COV_INCREMENT;
    stCOVproperty.m_u32PropertyArrayIndex = BACNET_ARRAY_ALL;
    
    Read_Property_Value(pvObjectIndex, eObjectType, &stCOVproperty, pVirtualDev);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Get_COVIncrementValue: exit \r\n");
	#endif
	
    return (stCOVproperty.m_pvStoredValue);
}

/*
 *	Decodes optional properties for cov & covp.
 */
void Decode_Optional_Property(uint8_t u8TagNumber,uint8_t *pu8ServiceRequest,
							ListOfBACnetCovSubs_t *pstCOVPSubscribe,
                            uint8_t *pu8IsoptionalProp, int32_t *pi32Len,
							uint32_t *pu32LenValue )
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Optional_Property: entry \r\n");
	#endif

	switch(u8TagNumber)
	{
		case TAG_NO_2:
		{
			/* Decode the Notification type */
			(*pi32Len) = (*pi32Len) + Decode_Context_Boolean_Value(&pu8ServiceRequest[*pi32Len],
			   &pstCOVPSubscribe->m_pstSubscriberInfo->m_bIssueConfirmedNotification);
			*pu8IsoptionalProp = *pu8IsoptionalProp | 0x01;
		}
		break;
		case TAG_NO_3:
		{
			/* Decode the Lifetime */
			*pi32Len = *pi32Len + Decode_Unsigned(&pu8ServiceRequest[*pi32Len], *pu32LenValue,
				&pstCOVPSubscribe->m_pstSubscriberInfo->m_u32TimeRemaining);
				*pu8IsoptionalProp = *pu8IsoptionalProp | 0x02;
		}
		break;
		break;
		case TAG_NO_5:
		{
			/* Decode Cov Increment */
			*pi32Len = *pi32Len + Decode_Real(&pu8ServiceRequest[*pi32Len],
				&(pstCOVPSubscribe->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Real));
            	
            /* set the flag */
            pstCOVPSubscribe->m_pstSubscriberInfo->m_bCovIncFrmClient = true;
		}
		break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Optional_Property: exit \r\n");
	#endif
}

/** */
void* GetListStoredValue(processInfo_t *pstProcQInfo)
{
    ListOfBACnetCovSubs_t      *pstCOVSubscribe = NULL;
    virtualDevData_t    *pVirtualDev = NULL;
    ulong32_t            ul32BaseAddr  = 0;
	void *FObject = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetListStoredValue : entry \r\n");
	#endif

    pVirtualDev = pstProcQInfo->m_pvReqDevStruct;
    pstCOVSubscribe =
        pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
		
	FObject = Find_Object(pstCOVSubscribe->m_stObjId.m_eObjectType,
							pstCOVSubscribe->m_stObjId.m_u32ObjId,
							pVirtualDev);
	ul32BaseAddr =  (ulong32_t)FObject;
	
    if(0 == ul32BaseAddr)
        return NULL;
    switch(pstCOVSubscribe->m_stCOVPropertyElem.m_ePropertyId)
    {
#ifdef BACDEL_OBJ_CAL
        case PROP_DATE_LIST:
        {
            return(&((Calendar_t*)ul32BaseAddr)->m_stDateList);
        }
        break;
#endif /* BACDEL_OBJ_CAL */
#ifdef BACDEL_OBJ_NC
        case PROP_RECIPIENT_LIST:
        {
            return(&((Notificationclass_t*)ul32BaseAddr)->m_stRecipientList);
        }
        break;
#endif /* BACDEL_OBJ_NC */
#ifdef BACDEL_OBJ_SDL
        case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
        {
            return(&((Schedule_t*)ul32BaseAddr)->m_stListOfObjectPropertyReference);
        }
        break;
        case PROP_WEEKLY_SCHEDULE:
        {
            return(&((Schedule_t*)ul32BaseAddr)->m_stWeeklySchedule);
        }
        break;
        case PROP_EXCEPTION_SCHEDULE:
        {
            return(&((Schedule_t*)ul32BaseAddr)->m_stExceptionSchedule);
        }
        break;
#endif /* BACDEL_OBJ_SDL */

        default:
        	break;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetListStoredValue : exit\r\n");
	#endif

    return NULL;
}


/** function to update the cov increment value in existing 
    subscription when cov increment property value is changed */
void Update_CovIncrement_In_Subscription(virtualDevData_t *pVirtualDev, 
            BACNET_OBJECT_TYPE eObjType, void *pvObject,
            void *pvCovIncValue, uint32_t u32Len)
{
    /* local variables */
    ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;
    CovRecipientInfo_t *pstSubscriberInfo = NULL;
    uint32_t u32ObjId = 0;
    ulong32_t ul32BaseAddr = 0;
    ulong32_t ul32OffsetAddr = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_CovIncrement_In_Subscription: entry \r\n");
	#endif

    /* check input pointer */
    if(NULL == pVirtualDev || NULL == pvCovIncValue || NULL == pvObject)
    {
        /* function entry */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
        Update_CovIncrement_In_Subscription: null input pointer. \r\n");
		#endif

        return;
    }

    /* convert base address */
    ul32BaseAddr = (ulong32_t)pvObject;

    /* get object id */
    GET_OBJCOMMON_OFFSET(eObjType, m_stObjectID.m_u32ObjId, ul32OffsetAddr);
    memcpy(&u32ObjId, (uint32_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint32_t));

    /* lock the mutex */
    Osal_Wait_Mutex(m_hcovsublistMtxLockHandle, INFINITE);
    /* get the pointer for 1st cov subscription */
    pstCOVSubscribe = pVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe;

    /* traverse the list & update value */
    while(NULL != pstCOVSubscribe)
    {
        /* check if object id & type matches */
		if(eObjType == pstCOVSubscribe->m_stObjId.m_eObjectType &&
            u32ObjId == pstCOVSubscribe->m_stObjId.m_u32ObjId)
        {
            /* get pointer of 1st subscriber */
            pstSubscriberInfo = pstCOVSubscribe->m_pstSubscriberInfo;

            /* traverse the subscriber list & update cov increment value */
            while(NULL != pstSubscriberInfo)
            {
                /* update the cv increment value only if not specified by cov-client */
                if(!pstSubscriberInfo->m_bCovIncFrmClient)
                {
                    /* copy value */
                    memcpy(&pstSubscriberInfo->m_stPropertyValue.uValue, pvCovIncValue, u32Len);

                }

                /* move to next subscriber */
                pstSubscriberInfo = pstSubscriberInfo->m_pstNext;         
            }
        }
        /* move to next cov subscription */
        pstCOVSubscribe = pstCOVSubscribe->m_pstNext;
    }

    /* unlock mutex */
    Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_CovIncrement_In_Subscription: exit \r\n");
	#endif

    return;
}

/**
* DESCRIPTION
* Returns the COV subscription type - (new subscription, new recepient, re-subscription)
*
* @param pstCOVSubscribe	[in] subscription request
* @param ePropertyID		[in] Property ID requested.
* @param pstVirtualDev		[in] Pointer to Virtual Device struct.
*
* @return COV subscription request type.
*
*/
BACNET_COV_SUBS Get_CovSubs_Type(
	ListOfBACnetCovSubs_t *pstCovSubscribeReq, 
	virtualDevData_t *pstVirtualDev)
{
    /* local variables */
    ListOfBACnetCovSubs_t *pstActiveCOVList = NULL;
	CovRecipientInfo_t *pstRecipientInfo = NULL;
    uint16_t u16ObjectType = 0;
    uint32_t u32ObjId = 0;
    uint32_t u32PropertyArrayIndex = BACNET_ARRAY_ALL;
	BACNET_PROPERTY_ID ePropertyID = MAX_PROP_SUPPORTED;
	BACNET_COV_SUBS eCovSubType = COV_SUBS_DEFAULT;
	bool bArrayIndxFlag = FALSE;
	bool bInnerLoopFlag = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Get_CovSubs_Type: Entry \r\n");
	#endif

	/* acquire the mutex */
	Osal_Wait_Mutex(m_hcovsublistMtxLockHandle,INFINITE);

    /* Extract the Active COV subscription base pointer */
    pstActiveCOVList = pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_pstCOVSubscribe;
	
    /* Get Monitored Object ID */
	u16ObjectType = pstCovSubscribeReq->m_stObjId.m_eObjectType;
	u32ObjId = pstCovSubscribeReq->m_stObjId.m_u32ObjId;
	ePropertyID = pstCovSubscribeReq->m_stCOVPropertyElem.m_ePropertyId;
    u32PropertyArrayIndex = pstCovSubscribeReq->m_stCOVPropertyElem.m_u32PropertyArrayIndex;
	bArrayIndxFlag = pstCovSubscribeReq->m_stCOVPropertyElem.m_bArrIndxresent;

    /* check if COV list has any value */
    if(pstActiveCOVList == NULL)
    {
		/* empty list, this is 1st new subscription request */
        eCovSubType =  ADD_NEW_REQ;
    }   
	else
	{
		/* compare request data with each node in cov list property */
		while(pstActiveCOVList != NULL)
		{
			/* check if the subscription exist */
			if((pstActiveCOVList->m_stCOVPropertyElem.m_ePropertyId == ePropertyID) &&
			   (pstActiveCOVList->m_stObjId.m_u32ObjId == u32ObjId) &&
			   (pstActiveCOVList->m_stObjId.m_eObjectType == u16ObjectType) &&
			   (pstActiveCOVList->m_stCOVPropertyElem.m_u32PropertyArrayIndex == u32PropertyArrayIndex) &&
			   (pstActiveCOVList->m_stCOVPropertyElem.m_bArrIndxresent == bArrayIndxFlag))
			{
				/* check if the requested subscription already exists from same subscriber */
				pstRecipientInfo = pstActiveCOVList->m_pstSubscriberInfo;
				if(NULL == pstRecipientInfo)
				{
					/* this is new recipient */
					eCovSubType = ADD_NEW_RECIPIENT;
					break;
				}
				else
				{
					do // scan all recipients
					{
						if((!memcmp(&pstCovSubscribeReq->m_pstSubscriberInfo->m_stAddress,
							&pstRecipientInfo->m_stAddress, sizeof(BACnetAddress_t))) 
							&&
							(pstCovSubscribeReq->m_pstSubscriberInfo->m_u32ProcessId == 
							pstRecipientInfo->m_u32ProcessId))
						{
							/* this is re-subscription request */
							eCovSubType = RESUBSCRIP_REQ;
							bInnerLoopFlag = TRUE;
							break;
						}
						if(NULL == pstRecipientInfo->m_pstNext)
						{
							/* this is new recipient */
							eCovSubType = ADD_NEW_RECIPIENT;
							bInnerLoopFlag = TRUE;
							break;
						}

						/* move to next recipient */
						pstRecipientInfo = pstRecipientInfo->m_pstNext;
					}while(NULL != pstRecipientInfo);

					/* break if match found in inner loop */
					if(bInnerLoopFlag)
						break;
				}				
			}
			else if(NULL == pstActiveCOVList->m_pstNext)
			{
				/* no matching subscription, this is new subcsription */           
				eCovSubType = ADD_NEW_REQ;           
			}

			/* move to next cov list node */
			pstActiveCOVList = pstActiveCOVList->m_pstNext;
		}
	}

	/* release the mutex */
	Osal_Release_Mutex(m_hcovsublistMtxLockHandle);

   /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Get_CovSubs_Type: Exit \r\n");
	#endif
    return eCovSubType;
}


#endif /* #if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B) */
/*****************************************************************************/
