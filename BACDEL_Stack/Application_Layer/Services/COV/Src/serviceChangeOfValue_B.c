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
*	 serviceChangeOfValue_B.c
*                                                                      
*   AUTHORS                                                                             
*	 Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	 Defines the COV_B functions for the BACnet application. 
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE        NAME		    DESCRIPTION
*   28/08/2011  Ashish Verma    File Created.
*   23/09/2011  Ashish Verma    Removed fucntion Initiate_Timer_Param. Modified
*                               Update_COVLifeTime() with
*                               DvCOV_B_Cancellation_Thread()
*   29/09/2011  Ashish Verma    Added BACDEL_Get_Device_ID_Or_IP_Address() in 
*                               Get_Notification_Parameter()
*   05/10/2011  Ashish Verma    Replaced Argument_List_t with
*								bacnetip_arguments_t.
*	11/11/2011	Ashish Verma	Modified Create_Message_Queue_Struct(),
*								Send_COV_Notification(),
*								COV_B_Subscribe_Handler(),
*								Register_COV_Request().
*   14/11/2011  Ashish Verma    Modified COVB_Notify_Encoder().
******************************************************************************/

/* To Include/Exclude the Change Of Value Service */
#ifdef BACDEL_SER_DS_COV_B

#include "serviceChangeOfValue_B.h"
#include "bacDELDef.h"
#include "propertyGenricHandler.h"
#include "pduEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "propertyValueRead.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"

extern QueueHandle_t Cov_EventQueue;
/**
*                                                                    
*DESCRIPTION                                                                          
*   Handler for COV Subscribe Service request. Decodes the COV Subscribe
*   service request and provides it to Device process thread for further
*   processing.
*    
*@param pu8ServiceRequest [in] The contents of service request.
*@param i32ServiceLen [in] Length of service request.
*@param pstProcQInfo [out] This will contain pointer to data of type
*                          ListOfBACnetCovSubs_t.
*@return BACDEL_SUCCESS/BACDEL_ERROR
*                                                                      
*/
BACNET_RETURN_TYPE COV_B_Subscribe_Handler(processInfo_t *pstProcQInfo,
                               uint8_t *pu8ServiceRequest,
                               int32_t i32ServiceLen)
{
     /* 
      * Follow the steps below:-
      * 1) allocate memory for COV structure.
      * 2) Parse the received packet & store it in the COV structure.
      * 3) Parse the Process Identifier.
      * 4) Parse the Monitored Object identifier.
      * 5) Parse the Isuue Confirmed Notifcation.
      * 6) Parse Lifetime parameter.
      * 7) Store the address/DeviceID present.
      * 8) Send it to the object Management layer for further processing.
      * 9) If the decoded packet is correct return continue otherwise return
      *    respective error.
      */ 
     ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;
     CovRecipientInfo_t *pstSubscriberInfo = NULL;
     uint8_t u8TagNumber = 0;
     int32_t i32Len = 0;
     uint32_t u32LenValue = 0;
     BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
     /* To check all Mandatory Prop Present*/
	 uint8_t ui8IsMandatoryPropPresent = 0; 
     /*COVP Cancellation, if Life-time and COV-Increment BOTH are abesent*/
	 uint8_t ui8IsoptionalProp = 0;	
     virtualDevData_t *pVirtualDev = NULL;
	 
	 /* function entry */
	 #ifdef DEBUG_PRINTF
	 Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COV_B_Subscribe_Handler: entry \r\n");
	 #endif

	 /* null check for input pointers */
	 if(NULL == pstProcQInfo)
	 {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: COV_B_Subscribe_Handler: null input pointers \r\n");
		#endif

		return BACDEL_ERROR;
	 }

	/* get virtual device data pointer */
     pVirtualDev = pstProcQInfo->m_pvReqDevStruct;     

     /* null check for input pointers */
	 if(NULL == pu8ServiceRequest || NULL == pVirtualDev)
	 {
	    #ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: COV_B_Subscribe_Handler: null input pointers \r\n");
		#endif

		return BACDEL_ERROR;
	 }

     /* Since the request should atleast conatin two parameters. Minimum
      * length of data that should be present is 7 */

     if(i32ServiceLen < MINIMUM_DATA_LENGTH)
     {
         /* Update the respective error PDU & set the status flag to 
          * PROC_DONE */
         Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
             BACNET_STATUS_REJECT, NULL_VALUE,
             ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER);

         /* To send error response need to return error */
		  #ifdef DEBUG_PRINTF
		  Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COV_B_Subscribe_Handler: missing parameter\r\n");
		  #endif

         return BACDEL_SUCCESS;
     }

     /* Allocate memory to COV structure to store the parsed request */
     pstCOVSubscribe = OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t),  __FILE__, __FUNCTION__, __LINE__);
     pstSubscriberInfo = OSAL_Malloc(sizeof(CovRecipientInfo_t),  __FILE__, __FUNCTION__, __LINE__);
     /* Check if malloc has successfully returned */
     if((NULL == pstCOVSubscribe) || (NULL == pstSubscriberInfo))
     {
		 #ifdef DEBUG_PRINTF
         Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:COV_B_Subscribe_Handler:pstCOVSubscribe"
             "Malloc Error\r\n");
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

        /* Update the respective error PDU & set the status flag to 
         * PROC_DONE */
         Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
             BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, ERROR_CODE_OTHER);
         /* To send error response need to return error */
         return BACDEL_SUCCESS;
     }
     /* Redirect the CovRecipientInfo_t data to ListOfBACnetCovSubs_t */
     pstCOVSubscribe->m_pstSubscriberInfo = pstSubscriberInfo;
     
     /* Redirect the service data to Process Queue data buffer */
	 pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = 
         pstCOVSubscribe;
        
     /* Extract the process identifier tag */
     i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
         &u8TagNumber, &u32LenValue);
    /* If tag is not of type process id return error */
	if(u8TagNumber != TAG_PROCESS_ID)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:COV_B_Subscribe_Handler:u8TagNumber ="
            "%d\t is not valid\r\n", u8TagNumber);
		#endif

        /* Update the respective error PDU & set the status flag to 
         * PROC_DONE */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_REJECT, NULL_VALUE, ERROR_CODE_REJECT_INVALID_TAG);
        /* To send error response need to return error */
        return BACDEL_SUCCESS;
    }
    /* Decode process Id of the subscriber */
    i32Len += Decode_Unsigned(&pu8ServiceRequest[i32Len], u32LenValue,
       &(pstCOVSubscribe->m_pstSubscriberInfo->m_u32ProcessId));
    ui8IsMandatoryPropPresent = ui8IsMandatoryPropPresent | 0x01;

    /* Extract Object Identifier tag */
    i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
        &u8TagNumber, &u32LenValue);
    /* If tag is not of type Object ID return error */
	if(u8TagNumber != TAG_OBJECT_ID)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:COV_B_Subscribe_Handler:u8TagNumber ="
            "%d\t is not valid\r\n", u8TagNumber);
		#endif

        /* Update the respective error PDU & set the status flag to 
         * PROC_DONE */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_REJECT, NULL_VALUE, ERROR_CODE_REJECT_INVALID_TAG);
        /* To send error response need to return error */
        return BACDEL_SUCCESS;
    }
    /* Decode the object Identifire */
    i32Len += Decode_Object_Id(&pu8ServiceRequest[i32Len],
		(uint32_t *)&pstCOVSubscribe->m_stObjId.m_eObjectType,
        &pstCOVSubscribe->m_stObjId.m_u32ObjId); 
    ui8IsMandatoryPropPresent = ui8IsMandatoryPropPresent | 0x06;    

    while((i32ServiceLen-i32Len) != NULL_VALUE)
    {   /* 60 5th octet */
        if(pu8ServiceRequest[i32Len] > 60)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:COV_B_Subscribe_Handler:u8TagNumber ="
            "%d\t is not valid\r\n", u8TagNumber);
			#endif

            /* Update the respective error PDU & set the status flag to 
             * PROC_DONE */
            Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_ERROR, ERROR_CLASS_SERVICES, ERROR_CODE_VALUE_OUT_OF_RANGE);
            /* To send error response need to return error */
            return BACDEL_SUCCESS;
        }
        i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
				&u8TagNumber, &u32LenValue);
        
        /*decode optional Property*/
        Decode_Optional_Property(u8TagNumber,pu8ServiceRequest,pstCOVSubscribe,
                                            &ui8IsoptionalProp, &i32Len ,&u32LenValue);

    }
    if(ALL_MANDATORY_PRO_PPRESENT != ui8IsMandatoryPropPresent || 
        ((BIT_CHECK(ui8IsoptionalProp,1)) && (!BIT_CHECK(ui8IsoptionalProp,0))))
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:COV_B_Subscribe_Handler:u8TagNumber ="
			"%d\t is not valid\r\n", u8TagNumber);
		#endif

		/* Update the respective error PDU & set the status flag to 
		 * PROC_DONE */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
			BACNET_STATUS_REJECT, NULL_VALUE, ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER);
		/* To send error response need to return error */
		return BACDEL_SUCCESS;
	}  
        
    /*check request is subcription or cancellation request*/
    if(COV_CANCELLATION != ui8IsoptionalProp)
    {
	    /* Indicate a subscription request not a cancellation request */
	     pstCOVSubscribe->m_pstSubscriberInfo->m_i8Subscribe = TRUE;
    }
     else 
    {
        /* Indicate a cancellation request not a subscription request */
        pstCOVSubscribe->m_pstSubscriberInfo->m_i8Subscribe = FALSE;
    }

    /* Extract the recipient address */
    pstCOVSubscribe->m_pstSubscriberInfo->m_stAddress =
        pstProcQInfo->m_stProcessData.m_stRmDvAddr;

    /* Call Object Management Layer interface */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
    if(BACDEL_CONTINUE != eReturnType)
    {
        /* Update the respective error PDU & set the status flag to 
         * PROC_DONE */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ABORT, NULL_VALUE,
			ERROR_CODE_ABORT_OUT_OF_RESOURCES);
			#ifdef DEBUG_PRINTF
			 Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COV_B_Subscribe_Handler: unknown device \r\n");
			#endif

        /* To send error response need to return error */
        return BACDEL_SUCCESS;
    } 
    return eReturnType;    
}


/**
*DESCRIPTION
*   Parse the request received & based on the status return Error packet
*   or a simple acknowledge.
*
*@param @param pstVirtualDev [in] Pointer to Virtual Device struct.
*
*@return TRUE/FALSE 
*
*/
BOOL COV_Subscribe_Request_Parser(virtualDevData_t *pstVirtualDev, processInfo_t * pstProcQInfo)
{
    ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;
    void *pvObjectIndex = NULL;
    BOOL bStatus = FALSE;
    BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	BACNET_ERROR_CLASS eErrorClass = MAX_BACNET_ERROR_CLASS;
	BACNET_COV_SUBS eCovSubType = COV_SUBS_DEFAULT;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COV_Subscribe_Request_Parser: entry \r\n");
	#endif
    
    /* Extract the COV request data */
    pstCOVSubscribe = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;

    if(!ValidateCovObject(pstCOVSubscribe,
                  pstVirtualDev,
                  pstProcQInfo))
    {
        return TRUE;
    }
    
    /* Based on Object type check if the requested Object exist in the device
       & extract its address */
	pvObjectIndex = Extract_Object_Address(pstCOVSubscribe->m_stObjId.m_eObjectType,
        pstCOVSubscribe->m_stObjId.m_u32ObjId, pstVirtualDev, pstProcQInfo);

    /* If NULL is returned then either object does not exist or the instance
       requested is not present or COV is not supported by the object */
    if(NULL == pvObjectIndex)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COV_Subscribe_Request_Parser: Object is not supported \r\n");
		#endif

        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_ERROR, ERROR_CLASS_OBJECT,
                ERROR_CODE_UNKNOWN_OBJECT);
        return TRUE;              
    }

	/* check process ID */
	/* clause 13.14.1.2 */
	if(BACNET_ZERO == pstCOVSubscribe->m_pstSubscriberInfo->m_u32ProcessId)
	{
		/* Value zero '0' is reserved for un-subscribed COV */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		COV_Subscribe_Request_Parser: Invalid process ID \r\n");
		#endif
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ERROR, ERROR_CLASS_SERVICES,
            ERROR_CODE_PARAMETER_OUT_OF_RANGE);
        return TRUE;              
	}

    /* Check if its a subscription request or a cancellation request */
    if(pstCOVSubscribe->m_pstSubscriberInfo->m_i8Subscribe)
    {
		/* get cov subscription type */
		eCovSubType = Get_CovSubs_Type(pstCOVSubscribe, pstVirtualDev);

		/* check cov subscription max list count */
		if(pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_u32Count >= 
			g_stStackMaxLimits.m_u32MaxActiveCovSubs && RESUBSCRIP_REQ != eCovSubType)
		{
			/* cannot add more suncsriptions */
			Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                ERROR_CODE_NO_SPACE_TO_ADD_LIST_ELEMENT);
			return TRUE;
		}

		/* register cov subscription */
        bStatus = Register_COV_Request(pvObjectIndex, pstVirtualDev, pstCOVSubscribe, &eCovSubType);
		if(bStatus)
		{
			if(ADD_NEW_RECIPIENT == eCovSubType || ADD_NEW_REQ == eCovSubType)
			{
				/* increment count for cov subscriptions */
				pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.m_u32Count++;
			}
		}
    }
    else
    {
		/* un-register cov subscription */
        bStatus = Cancel_COV_Subscription(pstVirtualDev, pvObjectIndex, 
			pstCOVSubscribe, &eErrorCode);
		/* As simple Ack is needed for every unsubscripted request the flags made true*/
		bStatus = true;
    }

    /* If during subscription/cancellation an error occurs */
    if(bStatus == FALSE)
    {
		/* update error class */
		if(eErrorCode == ERROR_CODE_UNKNOWN_SUBSCRIPTION)
			eErrorClass = ERROR_CLASS_SERVICES;
		else
			eErrorClass = ERROR_CLASS_PROPERTY;
		/* To send error response of object ID not supported */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
			BACNET_STATUS_ERROR, eErrorClass,
			eErrorCode);
    }
    else
    {
        /* Return a simple ack on successful subscription/cancellation */
        pstProcQInfo->m_stProcessData.m_ePDUType
            = PDU_TYPE_SIMPLE_ACK;
    }
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: COV_Subscribe_Request_Parser: exit \r\n");
	#endif

    return true;   
}

/* function to check if object supports cov increment property */
bool IsCOVIncrementSupported(BACNET_OBJECT_TYPE eObjectType)
{
	bool bReturnflag = false;

	/* switch as per object type */
    switch(eObjectType)
    {
        case OBJECT_ANALOG_INPUT :
        case OBJECT_ANALOG_OUTPUT :
        case OBJECT_ANALOG_VALUE :
            bReturnflag = true;
        break;

        default:
            bReturnflag = false;
    }

	return bReturnflag;
}

/**
*DESCRIPTION
*   Registers the COV request to ActiveCOVSubscription list.
*
*@param pvObjectIndex [in] Pointer to the ObjectID in Device structure.
*@param pstVirtualDev [in] Pointer to Virtual Device struct.
*
*@return TRUE/FALSE
*
*/
BOOL Register_COV_Request(void *pvObjectIndex,
	virtualDevData_t *pstVirtualDev,
	ListOfBACnetCovSubs_t *pstCOVSubscribe,
	BACNET_COV_SUBS *peCovSubType)
{
    /* local variables */
    void *pvCovFillPtr = NULL;
    uint32_t u32COVIndex = 0;
    CovPropElement_t *pstCOVPropElem = NULL;
    CovRecipientInfo_t *pstCOVRecipient = NULL;
    BOOL bStatus = FALSE;
    BACNET_PROPERTY_ID ePropertyId;
	BACNET_OBJECT_TYPE eObjectType;
	void *pvCOVIncrement = NULL;
    BOOL bRetVal = false;
    void *pvTemp = NULL;

    /* to post message to notification thread */
    Post_Thread_Msg_t   postThreadMsg = {0};
	
    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Register_COV_Request: entry \r\n");
	#endif

    /* check input pointers */
	if(NULL == pstCOVSubscribe || NULL == peCovSubType)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Register_COV_Request: \
									null service data pointer\r\n");
		#endif

		return FALSE;
	}

    /* Extract the address of CovPropElement_t structure from request */
    pstCOVPropElem = &pstCOVSubscribe->m_stCOVPropertyElem;
    /* Extract the Property ID */
    ePropertyId = pstCOVPropElem->m_ePropertyId;
	/* get object type */
	eObjectType = pstCOVSubscribe->m_stObjId.m_eObjectType;
    
    /* Get stored value of Present_Value and Status_Flag property */
    while(NULL != pstCOVPropElem )
    {
        /* Extract the pointer to the cov element of the respective
           property */
        pstCOVPropElem->m_pvStoredValue =
			Validate_And_Get_Property_COV_Addr(pstCOVSubscribe->m_stObjId.m_eObjectType,
                pvObjectIndex, pstCOVPropElem->m_ePropertyId);
        if(NULL == pstCOVPropElem->m_pvStoredValue)
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Register_COV_Request: "
				"pointer of cov element of property is NULL \r\n");
			#endif

            return FALSE;
        }
        if(NULL ==  pstCOVPropElem->m_pstNext)
        {
            break;
        }
        /* Point to the next Subscribed COV Index */
        pstCOVPropElem = (CovPropElement_t *)pstCOVPropElem->m_pstNext;
    }

    if(ePropertyId == PROP_PRESENT_VALUE)
    {
		if(IsCOVIncrementSupported(eObjectType))
		{
			/* Extract the COV Increment property address */
			pvCOVIncrement =  Get_COVIncrementValue(pvObjectIndex,
			pstCOVSubscribe->m_stObjId.m_eObjectType, pstVirtualDev);
			/* save cov increment value as per object type */				
			switch (eObjectType)
			{

				default:
				{
					/* real value */
					pstCOVSubscribe->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Real =
					((Pr_BACnetReal_t *)pvCOVIncrement)->m_fVal;
				}
				break;
			}	
		}
    }

    /* Extract the index of the active COV Subscription list where 
       the new subscription will be saved */ 
    pvCovFillPtr = Extract_COV_Subscription_Index(peCovSubType, &u32COVIndex, 
        pstCOVSubscribe, ePropertyId, pstVirtualDev);
    if(NULL == pvCovFillPtr)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Register_COV_Request: "
			"pointer to active COV Subscription list is NULL \r\n");
		#endif

        return FALSE;
    }

    /* Get the Subscription type & Index number where the request is to
       to be stored */
    pstCOVPropElem = &(pstCOVSubscribe->m_stCOVPropertyElem);
    
    /* Based on the request there can be New Subscription, Re-subscription
     * or adding new recipient to already subscribed request.
     */
    while(NULL != pstCOVPropElem)
    {
        /* Based on the value of u8COVListType processing will be done */
        switch(*peCovSubType)
        {
            //case 0:
            case ADD_NEW_REQ:
                /* Adding new request */
				/* copy new index no in cov subscription list */
                pstCOVSubscribe->m_i32Index = u32COVIndex;
				/* copy new index no into m_ActiveCov variable of subscribed property */
                memcpy(pstCOVPropElem->m_pvStoredValue,
                    &pstCOVSubscribe->m_i32Index, sizeof(int32_t));

				pstCOVPropElem->m_pvStoredValue = NULL;
				/* read base address of property structure */
                Read_All_Properties(
					pstCOVSubscribe->m_stObjId.m_eObjectType,
                    pstCOVPropElem->m_ePropertyId,
                    pstCOVPropElem->m_u32PropertyArrayIndex,
                    &(pstCOVPropElem->m_eData_Type),
                    &(pstCOVPropElem->m_pvStoredValue), pvObjectIndex,
					pstVirtualDev,
                    pstCOVSubscribe->m_stObjId.m_u32ObjId,
                    pstCOVPropElem->m_bArrIndxresent);

                pstCOVPropElem->m_pvCurrentValue =
                    pstCOVPropElem->m_pvStoredValue;
                
                /* Storing Value as per DataType so no pointer to actual property value */
                Store_Value_On_DataType(pstCOVPropElem->m_eData_Type,
                    pstCOVPropElem->m_pvStoredValue,
                    &(pstCOVPropElem->m_pvStoredValue));
                
                memcpy(pvCovFillPtr, pstCOVSubscribe, sizeof(ListOfBACnetCovSubs_t));
                bStatus = TRUE;
            break;

            case RESUBSCRIP_REQ:
                /* Resubscribing request */
                pstCOVRecipient = (CovRecipientInfo_t *)pvCovFillPtr;
                pstCOVPropElem->m_pvStoredValue = NULL;
				/* read base address of property structure */
                Read_All_Properties(
					pstCOVSubscribe->m_stObjId.m_eObjectType,
                    pstCOVPropElem->m_ePropertyId,
                    pstCOVPropElem->m_u32PropertyArrayIndex,
                    &(pstCOVPropElem->m_eData_Type),
                    &(pstCOVPropElem->m_pvStoredValue), pvObjectIndex,
					pstVirtualDev,
                    pstCOVSubscribe->m_stObjId.m_u32ObjId,
                    pstCOVPropElem->m_bArrIndxresent);
				/* update time remaining */
                pstCOVRecipient->m_u32TimeRemaining = 
					pstCOVSubscribe->m_pstSubscriberInfo->m_u32TimeRemaining;
				/* update type of notification */
                pstCOVRecipient->m_bIssueConfirmedNotification = 
					pstCOVSubscribe->m_pstSubscriberInfo->m_bIssueConfirmedNotification;
                /* Update COVInc */
				
                //pstCOVRecipient->m_fCOVIncrement = 
                    //pstCOVSubscribe->m_pstSubscriberInfo->m_fCOVIncrement;
					
				switch (eObjectType)
				{

					default:
					{
						/* real value */
						pstCOVRecipient->m_stPropertyValue.uValue.m_Real =
							pstCOVSubscribe->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Real;
					}
					break;
				}
					
                bStatus = TRUE;
            break;

            case ADD_NEW_RECIPIENT:
                /* Adding new recipient */
                pstCOVRecipient = (CovRecipientInfo_t *)pvCovFillPtr;
                pstCOVPropElem->m_pvStoredValue = NULL;
                Read_All_Properties(
					pstCOVSubscribe->m_stObjId.m_eObjectType,
                    pstCOVPropElem->m_ePropertyId,
                    pstCOVPropElem->m_u32PropertyArrayIndex,
                    &(pstCOVPropElem->m_eData_Type),
                    &(pstCOVPropElem->m_pvStoredValue), pvObjectIndex,
					pstVirtualDev,
                    pstCOVSubscribe->m_stObjId.m_u32ObjId,
                    pstCOVPropElem->m_bArrIndxresent);
				/* add new subscriber to cov list */
                memcpy(pstCOVRecipient, 
                    pstCOVSubscribe->m_pstSubscriberInfo, sizeof(CovRecipientInfo_t));
                bStatus = TRUE;
            break;

            default:
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Register_COV_Request: "
					"Register COV subscription function type not valid \r\n");
				#endif
                return FALSE;
            break;
        }
        if(NULL ==  pstCOVPropElem->m_pstNext)
        {
            break;
        }
        /* Point to the next Subscribed COV Index */
        pstCOVPropElem = (CovPropElement_t *)pstCOVPropElem->m_pstNext;
    }
    /* If the request is registered in ActiveCOVSubscription list inform 
       the COV thread that a notifications needs to be send */
    if(bStatus)
    { 
        void *pvMsgQdata;

        /* Number of paramaters to be passed to COV Thread are more than
         * message queue can support hence forming a structure 
         */
        pvMsgQdata = Create_Message_Queue_Struct(pstVirtualDev,
            pstVirtualDev->m_stDevObject.m_stActiveCOVSubList.
            m_pstCOVSubscribe->m_stCOVPropertyElem.m_pvStoredValue, TRUE,
            ePropertyId, pstCOVSubscribe->m_pstSubscriberInfo->m_stAddress,
            (int32_t)pstCOVSubscribe->m_pstSubscriberInfo->m_u32ProcessId);
        if(NULL != pvMsgQdata)
        {
			/* Post the message to the message queue */
			postThreadMsg.idThread = pstVirtualDev->m_dwNotificationThreadID;       
			postThreadMsg.MsgType = MSGQ_TYPE;
			postThreadMsg.wParam = u32COVIndex;
			postThreadMsg.lParam = (void*)pvMsgQdata;
			xQueueSend(Cov_EventQueue, &postThreadMsg, (TickType_t)100);
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Register_COV_Request: "
					"COV thread signalled \r\n");
				#endif

				//OSAL_Free(pvStoredValue,  __FILE__, __FUNCTION__, __LINE__);
				bRetVal = true; 
			}
		//	else
			{
				; // return error
			}
		}
		else
		{
			; // return error
		}
    }

    /* if this was resubscription request, then free the memories allocated to save 
        the subscription data */
    if(RESUBSCRIP_REQ == *peCovSubType || ADD_NEW_RECIPIENT == *peCovSubType)
    {
        /* free recipient / subscriber info */
        if(NULL != pstCOVSubscribe->m_pstSubscriberInfo)
            OSAL_Free(pstCOVSubscribe->m_pstSubscriberInfo, __FILE__,__FUNCTION__,__LINE__);
        pstCOVSubscribe->m_pstSubscriberInfo = NULL;
        /* free property element struct */
        pstCOVPropElem = pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext;
        pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext = NULL;
        while(NULL != pstCOVPropElem)
        {
            /* save next node */
            pvTemp = pstCOVPropElem;
            pstCOVPropElem = pstCOVPropElem->m_pstNext;
            /* free allocated memory */
            OSAL_Free(pvTemp, __FILE__,__FUNCTION__,__LINE__);
        }
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Register_COV_Request: exit \r\n");
	#endif

    return bRetVal;
}





bool ValidateCovObject(ListOfBACnetCovSubs_t *pstCOVSubscribe,
                  virtualDevData_t *pVirtualDev,
                  processInfo_t *pstProcQInfo)
{
    CovPropElement_t *pstPropertyElem = NULL;
    /* find the object, return error if not found */
	if(NULL != Find_Object(pstCOVSubscribe->m_stObjId.m_eObjectType,
                pstCOVSubscribe->m_stObjId.m_u32ObjId,
                pVirtualDev))
    {
        /* Being a COV request need to find out which specific properties are
           going to be monitored */
		switch(pstCOVSubscribe->m_stObjId.m_eObjectType)
        {
            case OBJECT_ANALOG_INPUT:
            case OBJECT_ANALOG_OUTPUT:
            case OBJECT_ANALOG_VALUE:
            case OBJECT_BINARY_INPUT:
            case OBJECT_BINARY_OUTPUT:
            case OBJECT_BINARY_VALUE:
            case OBJECT_MULTI_STATE_INPUT:
            case OBJECT_MULTI_STATE_OUTPUT:
            case OBJECT_MULTI_STATE_VALUE:

            //case OBJECT_SCHEDULE: /* bug fixed in build 1.0.7-0 */
                /* Specify the Property that is to be monitored */      
                pstCOVSubscribe->m_stCOVPropertyElem.m_ePropertyId =
                    PROP_PRESENT_VALUE;
                /* Update Data Array value */
                pstCOVSubscribe->m_stCOVPropertyElem.m_u32PropertyArrayIndex =
                    BACNET_ARRAY_ALL;
                /* Update the next cov element in the structure */
                pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext =
                    OSAL_Malloc(sizeof(CovPropElement_t),  __FILE__, __FUNCTION__, __LINE__);
                if(NULL == pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext)
                {
					#ifdef DEBUG_PRINTF
                     Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:COV_B_Subscribe_Handler:pstCOVPSubscribe"
                            "Malloc Error\r\n");
					#endif

                    /* Update the respective error PDU & set the status flag to PROC_DONE */
                    Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                         BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, ERROR_CODE_OTHER);
                     /* To send error response need to return error */
                     return false;
                }
                pstPropertyElem = pstCOVSubscribe->m_stCOVPropertyElem.m_pstNext;

                pstPropertyElem->m_ePropertyId = PROP_STATUS_FLAGS;
                pstPropertyElem->m_u32PropertyArrayIndex = BACNET_ARRAY_ALL;
                break;

            default:
                {
                    /* Update the respective error PDU & set the status flag to 
                     * PROC_DONE */
                    Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                        BACNET_STATUS_ERROR, ERROR_CLASS_OBJECT,
                        ERROR_CODE_OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED);

						#ifdef DEBUG_PRINTF
					     Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COV_B_Subscribe_Handler: service reques denied \r\n");
						#endif

                    /* To send error response need to return error */
                    return false;
                }
                break;
        }
        return true;
    }
    else
    {
        /* Update the respective error PDU & set the status flag to 
                     * PROC_DONE */
        Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
            BACNET_STATUS_ERROR, ERROR_CLASS_OBJECT,
            ERROR_CODE_UNKNOWN_OBJECT);

			#ifdef DEBUG_PRINTF
		     Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: COV_B_Subscribe_Handler: service reques denied \r\n");
			#endif

        /* To send error response need to return error */
        return false;
    }
}

#endif /* BACDEL_SER_DS_COV_B */
/*****************************************************************************/
