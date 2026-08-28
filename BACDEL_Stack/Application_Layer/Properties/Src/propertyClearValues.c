/*************************************************************************
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
*   SoftDEL Systems Ltd.                     india@softdel.com         
*   3rd Floor, Pentagon P4,                  http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028
*
*   File Name - propertyClearValues.c
*
*   RELEASE HISTORY
*
*	DATE				NAME					DESCRIPTION
*	18th March 2013		Pratham N. Murkute		Created file & moved api's.
*	18th March 2013		Pratham N. Murkute		Added new api's.
*
****************************************************************************/

/* header file includes */
#include "propertyClearValues.h"
#include "objDevice.h" 
#include "propertydef.h"
#include <string.h>


/***
*
* DESCRIPTION
*   This function frees memory allocated while decoding property value in 
*   Request (WP-B,WPM-B) or response for Initiated request(RP-A, RPM-A)
*
*   This function is used when 
*   1.  Request/Response is received with property value which is sequence of 
*       different data types
*   
* @param pstProcessQData   [in]  Pointer to start of property data link list
* @returns void.
*
*/
void Clear_Property_Value(BACNET_PROPERTY_VALUE * pstPropertyVal)
{
    /* local variables */
    void *pvTemp = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Property_Value : entry \r\n"); 
	#endif

    /* traverse list & free 1 by 1 */
    while(NULL != pstPropertyVal)
    {
        /* save next */
        pvTemp = pstPropertyVal->pstNextPropVal;
        /* free allocated memory */
        OSAL_Free(pstPropertyVal, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstPropertyVal = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Property_Value : exit \r\n"); 
	#endif
    return;
}

/**
*
* DESCRIPTION
*   This function frees memory allocated while decoding property value in 
*   Request (WP-B,WPM-B) or response for Context Property 
*
* @param eObjectProperty   [in]  property id.
* @param pvAppValue        [in]  base pointer of list.
* @param i32arrayIndex     [in]  array index.
* @returns void.
*
*/
void Clear_ContextProperty_Value(
			BACNET_DATA_TYPE ePropDataType, void *pvAppValue, 
            uint32_t u32arrayIndex, bool bArrIndxFlag)
{
    /* local variables */

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ContextProperty_Value : entry \r\n"); 
	#endif

    /* validate input pointer */
    if(NULL == pvAppValue)
        return;

    /* switch to property data-type */
    switch(ePropDataType)
    {
    case BACNET_DT_DEVOBJPROPREF:
		{
            /* get the pointer */
            Pr_BACnetDevObjPropRef_t *pstDevObjPropRef = NULL;
            pstDevObjPropRef = pvAppValue;  
            /* free the allocated memory */
            OSAL_Free(pstDevObjPropRef, __FILE__, __FUNCTION__, __LINE__);              
        }
        break;

	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
		{
			/* u32arrayIndex == 0 then it is array type free only the malloced unsigned value */
			if(0 == u32arrayIndex && ARRAY_INDEX_PRESENT == bArrIndxFlag)
			{
				/* free the allocated memory */
				OSAL_Free(pvAppValue, __FILE__, __FUNCTION__, __LINE__);
			}
			/* free the list type property */
			else
				Clear_DevObjPropReff_List((ListOfBACnetDevObjPropRef_t **)&pvAppValue);			
        }
        break;

    case BACNET_DT_DAILYSCHEDULE_ARRAY:
        {
            Pr_ListOfBACnetDailySchedule_t   *pstWeeklySchedule;
            BACnetTimeValue_t       *pstTimeValue ;
            int32_t i32count = 0;
            /* get the pointer */
            pstWeeklySchedule = pvAppValue;

			/*if(NULL == pstWeeklySchedule)
				break;*/

            if(ARRAY_INDEX_ABSENT == bArrIndxFlag)
            {
				for(i32count = 0; i32count < BACNET_ARRAY_OF_SEVEN; i32count++)
                {
                    pstTimeValue = pstWeeklySchedule->m_stTimeValue[i32count].m_pstNext;
                    Clear_TimeValues_List(&pstTimeValue);
					pstWeeklySchedule->m_stTimeValue[i32count].m_pstNext = NULL;
                }
            }
			/* clear allocated memory of main structure */
			OSAL_Free(pstWeeklySchedule, __FILE__,__FUNCTION__,__LINE__);

        }
        break;  

    case BACNET_DT_OBJPROPREF:
    case BACNET_DT_SETPOINTREF:
       {
            /* get the pointer */
            Pr_BACnetObjPropRef_t  *pstObjPropReff = NULL;
            pstObjPropReff = pvAppValue;
            /* free the allocated memory */
            OSAL_Free(pstObjPropReff, __FILE__, __FUNCTION__, __LINE__);
        }
		break;

    case BACNET_DT_EVENTPARAMETERS:
        {
            OSAL_Free(pvAppValue, __FILE__, __FUNCTION__, __LINE__);
        }
        break;
    /* default case - do nothing */
	default:
		break;
    }//switch ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ContextProperty_Value : exit \r\n"); 
	#endif
    return;
}

/**
*
* DESCRIPTION
* Api clears the global device address binding list.
*
* @param none.
* @returns void
*
*/
void Clear_Global_Device_Addr_Binding_List(void)
{
    /* local variables */
	BACnetAddrBinding_t *pstTemp = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Global_Device_Addr_Binding_List: entry \r\n");
	#endif

	/* acquire the mutex */
	Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);

    /* reset the list count */
    g_stDevAddBinding.m_u32Count = 1;
	pstTemp = g_stDevAddBinding.m_stAddBinding.pstNext;
	g_stDevAddBinding.m_stAddBinding.pstNext = NULL;

    /* clear the list */
    Clear_AddressBinding_List(&pstTemp, true);

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Global_Device_Addr_Binding_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of address binding type properties.
*
* @param ppAddrBindList [in] base pointer to free list.
* @param bGlobalList [in] if true remove references of address binding from init-que.
* @return - void;
*
*/
void Clear_AddressBinding_List(BACnetAddrBinding_t **ppAddrBindList, bool bGlobalList)
{
    /* local variables */
    void *pvTemp = NULL;
    BACnetAddrBinding_t *pstAddBindList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AddressBinding_List: entry \r\n");
	#endif

	/* check input pointers */
    if(NULL == ppAddrBindList)
    {
        return;
    }
	if(NULL == *ppAddrBindList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstAddBindList = *ppAddrBindList;
	*ppAddrBindList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstAddBindList)
    {
        /* save next */
        pvTemp = pstAddBindList->pstNext;
        /* free allocated memory */
        OSAL_Free(pstAddBindList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstAddBindList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AddressBinding_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of boolean type properties.
*
* @param ppBooleanList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Boolean_List(ListOfBoolen_t **ppBooleanList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBoolen_t *pstBoolList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Boolean_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppBooleanList)
    {
        return;
    }
	if(NULL == *ppBooleanList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstBoolList = *ppBooleanList;
	*ppBooleanList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstBoolList)
    {
        /* save next */
        pvTemp = pstBoolList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstBoolList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstBoolList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Boolean_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bit_string type properties.
*
* @param ppBitStrList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_BitString_List(ListOfBitStr_t **ppBitStrList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBitStr_t *pstBitList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_BitString_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppBitStrList)
    {
        return;
    }
	if(NULL == *ppBitStrList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstBitList = *ppBitStrList;
	*ppBitStrList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstBitList)
    {
        /* save next */
        pvTemp = pstBitList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstBitList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstBitList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_BitString_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for unsigned list type properties.
*
* @param ppUnsignIntList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_UnsignedInt_List(ListOfUnsigned_t **ppUnsignIntList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfUnsigned_t *pstIntList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_UnsignedInt_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppUnsignIntList)
    {
        return;
    }
	if(NULL == *ppUnsignIntList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstIntList = *ppUnsignIntList;
	*ppUnsignIntList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstIntList)
    {
        /* save next */
        pvTemp = pstIntList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstIntList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstIntList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_UnsignedInt_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of enum type properties.
*
* @param ppEnumerationList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Enumeration_List(ListOfEnum_t **ppEnumerationList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfEnum_t *pstEnumList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Enumeration_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppEnumerationList)
    {
        return;
    }
	if(NULL == *ppEnumerationList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstEnumList = *ppEnumerationList;
	*ppEnumerationList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstEnumList)
    {
        /* save next */
        pvTemp = pstEnumList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstEnumList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstEnumList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Enumeration_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of char_string type properties.
*
* @param ppCharStrList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_CharString_List(ListOfCharStr_t **ppCharStrList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfCharStr_t *pstCharList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_CharString_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppCharStrList)
    {
        return;
    }
	if(NULL == *ppCharStrList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstCharList = *ppCharStrList;
	*ppCharStrList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstCharList)
    {
        /* save next */
        pvTemp = pstCharList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstCharList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstCharList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_CharString_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet recipient type properties.
*
* @param ppRecpList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Recipient_List(ListOfBACnetRecipient_t **ppRecpList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetRecipient_t *pstRecipient = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Recipient_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppRecpList)
    {
        return;
    }
	if(NULL == *ppRecpList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstRecipient = *ppRecpList;
	*ppRecpList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstRecipient)
    {
        /* save next */
        pvTemp = pstRecipient->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstRecipient, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstRecipient = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Recipient_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet destination type properties.
*
* @param ppDestList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Destination_List(ListOfBACnetDestination_t **ppDestList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetDestination_t *pstDestination = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Destination_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppDestList)
    {
        return;
    }
	if(NULL == *ppDestList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstDestination = *ppDestList;
	*ppDestList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstDestination)
    {
        /* save next */
        pvTemp = pstDestination->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstDestination, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstDestination = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Destination_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of object id type properties.
*
* @param ppObjectIDList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ObjectID_List(ListOfObjId_t **ppObjectIDList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfObjId_t *pstObjIDList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ObjectID_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppObjectIDList)
    {
        return;
    }
	if(NULL == *ppObjectIDList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstObjIDList = *ppObjectIDList;
	*ppObjectIDList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstObjIDList)
    {
        /* save next */
        pvTemp = pstObjIDList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstObjIDList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstObjIDList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ObjectID_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet destination type properties.
*
* @param ppDevObjPropRefList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_DevObjPropReff_List(ListOfBACnetDevObjPropRef_t **ppDevObjPropRefList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetDevObjPropRef_t *pstDevObjPropReffList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_DevObjPropReff_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppDevObjPropRefList)
    {
        return;
    }
	if(NULL == *ppDevObjPropRefList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstDevObjPropReffList = *ppDevObjPropRefList;
	*ppDevObjPropRefList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstDevObjPropReffList)
    {
        /* save next */
        pvTemp = pstDevObjPropReffList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstDevObjPropReffList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstDevObjPropReffList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_DevObjPropReff_List: exit \r\n");
	#endif
    return;
}
/**
* 
* Function to clear the active cov subscription list.
*
* @param ppActCovSubList[in] base pointer to free list.
* @return - void;
*
*/
void Clear_ActiveCovSubs_List(ListOfBACnetCovSubs_t **ppActCovSubList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetCovSubs_t *pstCovList = NULL;
#if !(defined BACDEL_SER_DS_COV_A || defined BACDEL_SER_DS_COVP_A)
    CovRecipientInfo_t *pstSubscriber = NULL;
    CovPropElement_t *pstCovPropElem = NULL;
#endif

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ActiveCovSubs_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppActCovSubList)
	{
        return;
	}
	if(NULL == *ppActCovSubList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstCovList = *ppActCovSubList;
	*ppActCovSubList = NULL;

    while(NULL != pstCovList)
	{
		/* clear all subscribers */
		pstSubscriber = pstCovList->m_pstSubscriberInfo;
        pstCovList->m_pstSubscriberInfo = NULL;
		while(NULL != pstSubscriber)
		{
			pvTemp = pstSubscriber->m_pstNext;
			OSAL_Free(pstSubscriber, __FILE__, __FUNCTION__, __LINE__);
			pstSubscriber = pvTemp;
		}

		/* clear all subscribed properties & their values */
		/* free stored value pointer for 1st property */
		if(NULL != pstCovList->m_stCOVPropertyElem.m_pvStoredValue)
			OSAL_Free(pstCovList->m_stCOVPropertyElem.m_pvStoredValue, 
						__FILE__, __FUNCTION__, __LINE__);
        pstCovList->m_stCOVPropertyElem.m_pvStoredValue = NULL;

		/* check if more properties are present */
		pstCovPropElem = pstCovList->m_stCOVPropertyElem.m_pstNext;
        pstCovList->m_stCOVPropertyElem.m_pstNext = NULL;
		while(NULL != pstCovPropElem)
		{
            pvTemp = pstCovPropElem->m_pstNext;
			/* free stored value pointer for property */
			if(NULL != pstCovPropElem->m_pvStoredValue)
				OSAL_Free(pstCovPropElem->m_pvStoredValue, __FILE__, __FUNCTION__, __LINE__);
            pstCovPropElem->m_pvStoredValue = NULL;
			/* free property */
			OSAL_Free(pstCovPropElem, __FILE__, __FUNCTION__, __LINE__);
            pstCovPropElem = pvTemp;
		}

		/* now free the active cov list member */
		pvTemp = pstCovList->m_pstNext;
		OSAL_Free(pstCovList, __FILE__, __FUNCTION__, __LINE__);
		pstCovList = pvTemp;
	}

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ActiveCovSubs_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of extended parameters
* in the event prameters with event type EXTENDED.
*
* @param ppExtParamList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ExtendedParameters_List(Parameters_t **ppExtParamList)
{
    /* local variables */
    void *pvTemp = NULL;
    Parameters_t *pstValue = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ExtendedParameters_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppExtParamList)
	{
        return;
	}
	if(NULL == *ppExtParamList)
	{
        return;
	}
    /* get the base pointer of link list */
    pstValue = *ppExtParamList;
	*ppExtParamList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstValue)
    {
        /* save next */
        pvTemp = pstValue->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstValue, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstValue = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ExtendedParameters_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of property states
* in the event prameters with event type change of state.
*
* @param ppPropStatesList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_PropertyStates_List(ListOfBACnetPropertyStates_t **ppPropStatesList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetPropertyStates_t *pstValue = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_PropertyStates_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppPropStatesList)
	{
        return;
	}
    if(NULL == *ppPropStatesList)
	{
        return;
	}
    /* get the base pointer of link list */
    pstValue = *ppPropStatesList;
	*ppPropStatesList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstValue)
    {
        /* save next */
        pvTemp = pstValue->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstValue, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstValue = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_PropertyStates_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of BACnet VT Class.
*
* @param ppVTClassList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_VtClass_List(Pr_BACnetVTClass_t **ppVTClassList)
{
    /* local variables */
    void *pvTemp = NULL;
    Pr_BACnetVTClass_t *pstVTclassList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_VtClass_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppVTClassList)
	{
        return;
	}
	if(NULL == *ppVTClassList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstVTclassList = *ppVTClassList;
	*ppVTClassList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstVTclassList)
    {
        /* save next */
        pvTemp = pstVTclassList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstVTclassList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstVTclassList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_VtClass_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of BACnet VT sessions.
*
* @param ppVTSessionsList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_VtSessions_List(ListOfBACnetVTSession_t **ppVTSessionsList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetVTSession_t *pstVTsessionsList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_VtSessions_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppVTSessionsList)
	{
        return;
	}
	if(NULL == *ppVTSessionsList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstVTsessionsList = *ppVTSessionsList;
	*ppVTSessionsList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstVTsessionsList)
    {
        /* save next */
        pvTemp = pstVTsessionsList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstVTsessionsList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstVTsessionsList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_VtSessions_List: exit \r\n");
	#endif
    return;
}
/**
* 
* Function to clear property values for list of bacnet action command Type.
*
* @param ppActionList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ActionCmd_List(ListOfBACnetActionCommand_t **ppActionList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetActionCommand_t *pstActionList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ActionCmd_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppActionList)
	{
        return;
	}
	if(NULL == *ppActionList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstActionList = *ppActionList;
	*ppActionList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstActionList)
    {
        /* save next */
		pvTemp = pstActionList->m_pstNext;
		/* clear property data type */
		Clear_PropVal_AsPer_DataType(pstActionList->m_stPropPointer.m_eData_Type, 
			&pstActionList->m_stPropPointer.m_pvConstrProp);
        /* free allocated memory */
        OSAL_Free(pstActionList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstActionList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ActionCmd_List: exit \r\n");
	#endif
    return;
}


/**
* 
* Function to clear property values for list of bacnet action Type.
*
* @param ppActionList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Action_List(ListOfBACnetActionList_t **ppActionList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetActionList_t *pstActionList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Action_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppActionList)
	{
        return;
	}
	if(NULL == *ppActionList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstActionList = *ppActionList;
	*ppActionList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstActionList)
    {
        /* save next */
        pvTemp = pstActionList->m_pstNext;
		/* free action command list */
		Clear_ActionCmd_List(&pstActionList->m_pstActionCommand);
        /* free allocated memory */
        OSAL_Free(pstActionList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstActionList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Action_List: exit \r\n");
	#endif
    return;
}
/**
* 
* Function to clear property values for list of bacnet Read Access Result.
*
* @param ppReadAccResList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_GpResults_List(listOfResults_t **ppPropResultList)
{
    /* local variables */
    void *pvTemp = NULL;
    listOfResults_t *pstReadAccRes = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_GpResults_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppPropResultList)
	{
        return;
	}
	if(NULL == *ppPropResultList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstReadAccRes = *ppPropResultList;
	*ppPropResultList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstReadAccRes)
    {
        /* save next */
		pvTemp = pstReadAccRes->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstReadAccRes, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstReadAccRes = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_GpResults_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Read Access Result.
*
* @param ppReadAccResList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ReadAccessResult_List(ListOfReadAccessResult_t **ppReadAccResList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfReadAccessResult_t *pstReadAccRes = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ReadAccessResult_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppReadAccResList)
    {
        return;
    }
	if(NULL == *ppReadAccResList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstReadAccRes = *ppReadAccResList;
	*ppReadAccResList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstReadAccRes)
    {
        /* save next */
        pvTemp = pstReadAccRes->m_pstNext;
		/* free internal lists */
		Clear_GpResults_List(&pstReadAccRes->m_stListOfResults.m_pstNext);
        /* free allocated memory */
        OSAL_Free(pstReadAccRes, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstReadAccRes = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ReadAccessResult_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Read Access Specification.
*
* @param ppReadAcesSpecifList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_GpSpecs_List(ListOfBACnetPropRef_t **ppPropRefList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetPropRef_t *pstReadAcesSpecifList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_GpSpecs_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppPropRefList)
    {
        return;
    }
	if(NULL == *ppPropRefList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstReadAcesSpecifList = *ppPropRefList;
	*ppPropRefList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstReadAcesSpecifList)
    {
        /* save next */
		pvTemp = pstReadAcesSpecifList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstReadAcesSpecifList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstReadAcesSpecifList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_GpSpecs_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Read Access Specification.
*
* @param ppReadAcesSpecifList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ReadAccessSpecs_List(ListOfReadAccessSpecs_t **ppReadAcesSpecifList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfReadAccessSpecs_t *pstReadAcesSpecifList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ReadAccessSpecs_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppReadAcesSpecifList)
    {
        return;
    }
	if(NULL == *ppReadAcesSpecifList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstReadAcesSpecifList = *ppReadAcesSpecifList;
	*ppReadAcesSpecifList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstReadAcesSpecifList)
    {
        /* save next */
        pvTemp = pstReadAcesSpecifList->m_pstNext;
		/* free internal lists */
		Clear_GpSpecs_List(&pstReadAcesSpecifList->m_stlistOfPropRef.m_pstNext);
        /* free allocated memory */
        OSAL_Free(pstReadAcesSpecifList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstReadAcesSpecifList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_ReadAccessSpecs_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet device object reference.
*
* @param ppDevObjRefList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_DevObjRef_List(ListOfBACnetDevObjRef_t **ppDevObjRefList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetDevObjRef_t *pstDevObjRefList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_DevObjRef_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppDevObjRefList)
    {
        return;
    }
	if(NULL == *ppDevObjRefList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstDevObjRefList = *ppDevObjRefList;
	*ppDevObjRefList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstDevObjRefList)
    {
        /* save next */
        pvTemp = pstDevObjRefList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstDevObjRefList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstDevObjRefList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_DevObjRef_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Trend log record.
*
* @param ppLogRecordList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_TrendLogRecord_List(ListOfBACnetLogRecord_t **ppLogRecordList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetLogRecord_t *pstLogRecordList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_TrendLogRecord_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppLogRecordList)
    {
        return;
    }
	if(NULL == *ppLogRecordList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstLogRecordList = *ppLogRecordList;
	*ppLogRecordList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstLogRecordList)
    {
        /* save next */
        pvTemp = pstLogRecordList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstLogRecordList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstLogRecordList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_TrendLogRecord_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of event log record.
*
* @param ppEvntLogRecoList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_EventLogRecord_List(ListOfBACnetEventLogRecord_t **ppEvntLogRecoList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetEventLogRecord_t *pstEvntLogRecoList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_EventLogRecord_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppEvntLogRecoList)
    {
        return;
    }
	if(NULL == *ppEvntLogRecoList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstEvntLogRecoList = *ppEvntLogRecoList;
	*ppEvntLogRecoList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstEvntLogRecoList)
    {
        /* save next */
        pvTemp = pstEvntLogRecoList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstEvntLogRecoList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstEvntLogRecoList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_EventLogRecord_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Trend log Multiple record.
*
* @param ppTLogMultRecoList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_TrendLogMultipleRecord_List(ListOfBACnetLogMultipleRecord_t **ppTLogMultRecoList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetLogMultipleRecord_t *pstTLogMultRecoList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_TrendLogMultipleRecord_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppTLogMultRecoList)
    {
        return;
    }
	if(NULL == *ppTLogMultRecoList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstTLogMultRecoList = *ppTLogMultRecoList;
	*ppTLogMultRecoList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstTLogMultRecoList)
    {
        /* save next */
        pvTemp = pstTLogMultRecoList->m_pstNext;
		/* free list fo internal values i.e. log data */
		Clear_LogDataTLM_List(&pstTLogMultRecoList->m_stlogData.m_pstNext);
        /* free allocated memory */
        OSAL_Free(pstTLogMultRecoList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstTLogMultRecoList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_TrendLogMultipleRecord_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Authentication Factor format.
*
* @param ppAuthefactFormList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_AuFactorFormat_List(ListOfBACnetAuFactorFormat_t **ppAuthefactFormList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetAuFactorFormat_t *pstAuthefactFormList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AuFactorFormat_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppAuthefactFormList)
    {
        return;
    }
	if(NULL == *ppAuthefactFormList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstAuthefactFormList = *ppAuthefactFormList;
	*ppAuthefactFormList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstAuthefactFormList)
    {
        /* save next */
        pvTemp = pstAuthefactFormList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstAuthefactFormList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstAuthefactFormList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AuFactorFormat_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet Optional Character String.
*
* @param ppOptCharStrList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_OptionalCharString_List(ListOfOptCharStr_t **ppOptCharStrList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfOptCharStr_t *pstOptCharStrList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_OptionalCharString_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppOptCharStrList)
    {
        return;
    }
	if(NULL == *ppOptCharStrList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstOptCharStrList = *ppOptCharStrList;
	*ppOptCharStrList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstOptCharStrList)
    {
        /* save next */
        pvTemp = pstOptCharStrList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstOptCharStrList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstOptCharStrList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_OptionalCharString_List: exit \r\n");
	#endif
    return;
}

/**
* 
* DESCRIPTION
* Function to clear property values of type array of bacnet authentication policy list.
*
* @param ppPolicyList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Policy_List(Policy_t **ppPolicyList)
{
    /* local variables */
    void *pvTemp = NULL;
	Policy_t *pstPolicyList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Policy_List : entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == ppPolicyList)
        return;
	if(NULL == *ppPolicyList)
        return;

    /* get the base pointer of link list */
    pstPolicyList = *ppPolicyList;
	*ppPolicyList = NULL;

    /* free 1 by 1 */
    while(NULL != pstPolicyList)
    {
        /* save next */
        pvTemp = pstPolicyList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstPolicyList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next */
        pstPolicyList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Policy_List : exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values of type array of bacnet Authentication Policy.
*
* @param ppAuthePoliList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_AuPolicy_List(ListOfBACnetAuPolicy_t **ppAuthePoliList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetAuPolicy_t *pstAuthePoliList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AuPolicy_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppAuthePoliList)
    {
        return;
    }
	if(NULL == *ppAuthePoliList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstAuthePoliList = *ppAuthePoliList;
	*ppAuthePoliList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstAuthePoliList)
    {
        /* save next */
        pvTemp = pstAuthePoliList->m_pstNext;
		/* clear policy list */
		Clear_Policy_List(&pstAuthePoliList->m_stPolicy.m_pstNext);
		pstAuthePoliList->m_stPolicy.m_pstNext = NULL;
        /* free allocated memory */
        OSAL_Free(pstAuthePoliList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstAuthePoliList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AuPolicy_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values of type array of bacnet Access Rule.
*
* @param ppAccessRuleList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_AccessRule_List(ListOfBACnetAccessRule_t **ppAccessRuleList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetAccessRule_t *pstAccessRuleList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AccessRule_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppAccessRuleList)
    {
        return;
    }
	if(NULL == *ppAccessRuleList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstAccessRuleList = *ppAccessRuleList;
	*ppAccessRuleList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstAccessRuleList)
    {
        /* save next */
        pvTemp = pstAccessRuleList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstAccessRuleList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstAccessRuleList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AccessRule_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values of type array of 
*			bacnet Credential Authentication Factor.
*
* @param ppCredAutheFact [in] base pointer to free list.
* @return - void;
*
*/
void Clear_CredentialAuFactor_List(ListOfBACnetCredAuFactor_t **ppCredAutheFact)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetCredAuFactor_t *pstCredAutheFact = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_CredentialAuFactor_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppCredAutheFact)
    {
        return;
    }
	if(NULL == *ppCredAutheFact)
	{
        return;
	}

    /* get the base pointer of link list */
    pstCredAutheFact = *ppCredAutheFact;
	*ppCredAutheFact = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstCredAutheFact)
    {
        /* save next */
        pvTemp = pstCredAutheFact->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstCredAutheFact, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstCredAutheFact = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_CredentialAuFactor_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values of type array of 
*			bacnet Assigned Access Rights.
*
* @param ppAssAcceRit [in] base pointer to free list.
* @return - void;
*
*/
void Clear_AssignedAccessRights_List(ListOfBACnetAssignedAccessRights_t **ppAssAcceRit)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetAssignedAccessRights_t *pstAssAcceRit = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AssignedAccessRights_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppAssAcceRit)
    {
        return;
    }
	if(NULL == *ppAssAcceRit)
	{
        return;
	}

    /* get the base pointer of link list */
    pstAssAcceRit = *ppAssAcceRit;
	*ppAssAcceRit = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstAssAcceRit)
    {
        /* save next */
        pvTemp = pstAssAcceRit->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstAssAcceRit, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstAssAcceRit = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_AssignedAccessRights_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of Log Data type properties.
*
* @param ppLogDataList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_LogDataTLM_List(BACnetLogData_t **ppLogDataList)
{
    /* local variables */
    void *pvTemp = NULL;
    BACnetLogData_t *pstLogDataList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_LogDataTLM_List: entry \r\n");
	#endif

    if(NULL == ppLogDataList)
    {
        return;
    }
	if(NULL == *ppLogDataList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstLogDataList = *ppLogDataList;
	*ppLogDataList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstLogDataList)
    {
        /* save next */
        pvTemp = pstLogDataList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstLogDataList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstLogDataList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_LogDataTLM_List: exit \r\n");
	#endif
    return;
}

#ifdef BACDEL_PR12

/**
* 
* Function to clear property values for array Propert Access result data type..
*
* @param ppAccessResult [in] base pointer to free list.
* @return - void;
*
*/
void Clear_PropAccessResult_List(ListOfBACnetPropAccessRslt_t **ppAccessResult)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetPropAccessRslt_t *pstPropAccessResult = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_PropAccessResult_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppAccessResult)
    {
        return;
    }
	if(NULL == *ppAccessResult)
	{
        return;
	}

    /* get the base pointer of link list */
    pstPropAccessResult = *ppAccessResult;
	*ppAccessResult = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstPropAccessResult)
    {
        /* save next */
		pvTemp = pstPropAccessResult->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstPropAccessResult, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstPropAccessResult = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_PropAccessResult_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for array of Network security policy data type..
*
* @param ppNwSecPolArr [in] base pointer to free list.
* @return - void;
*
*/
void Clear_NwSecurityPolicy_List(ListOfBACnetNwSecurityPolicy_t **ppNwSecPolArr)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetNwSecurityPolicy_t *pstSecurityPolicy = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_NwSecurityPolicy_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppNwSecPolArr)
    {
        return;
    }
	if(NULL == *ppNwSecPolArr)
	{
        return;
	}

    /* get the base pointer of link list */
    pstSecurityPolicy = *ppNwSecPolArr;
	*ppNwSecPolArr = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstSecurityPolicy)
    {
        /* save next */
		pvTemp = pstSecurityPolicy->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstSecurityPolicy, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstSecurityPolicy = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_NwSecurityPolicy_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet KeyId type properties.
*
* @param ppKeyIdList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_KeyId_List(ListOfBACnetKeyId_t **ppKeyIdList)
{
    /* local variables */
    void *pvTemp = NULL;
    ListOfBACnetKeyId_t *pstKeyID = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_KeyId_List: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppKeyIdList)
    {
        return;
    }
	if(NULL == *ppKeyIdList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstKeyID = *ppKeyIdList;
	*ppKeyIdList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstKeyID)
    {
        /* save next */
        pvTemp = pstKeyID->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstKeyID, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstKeyID = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_KeyId_List: exit \r\n");
	#endif
    return;
}

/**
* 
* Function to clear property values for list of bacnet SecurityKeySet type properties.
*
* @param pstSecKeySetList [in] base pointer to free list.
* @return - void;
*
*/
bool Clear_SecurityKeySet_List(Pr_ListOfBACnetSecurityKeySet_t  *pstSecKeySetList)
{
	/* local variable */
    int8_t i8count = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_SecurityKeySet_List: entry \r\n");
	#endif

    for(i8count = 0; i8count < 2; i8count++ )
    {
        if(NULL != pstSecKeySetList)
        {
			/* clear key id list */
			Clear_KeyId_List(&pstSecKeySetList->m_stSecurityKetSet[i8count].m_stKeyIds.m_pstNext);
			memset(&pstSecKeySetList->m_stSecurityKetSet[i8count], 0, sizeof(BACnetSecurityKeySet_t));
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_SecurityKeySet_List: exit \r\n");
	#endif

    return true;
}

#endif /* BACDEL_PR12 */

/**
* 
* Function to Clear the memory as per data types.
* 
* @param peDataType [in] base pointer of data type.
* @param ppPropValue [in] base pointer to free Memory.
* @return - void;
*
*/
void Clear_PropVal_AsPer_DataType(BACNET_DATA_TYPE eDataType, void **ppPropValue)
{
	/* local variables */
    void *pvTemp = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_PropVal_AsPer_DataType: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == ppPropValue)
		return;
	if(NULL == *ppPropValue)
        return;

	/* get the base pointer of link list */
    pvTemp = *ppPropValue;
	*ppPropValue = NULL;

	/* Free Memory as per data type */
	switch(eDataType)
	{
		/* Data-type is Empty or NULL */
		case BACNET_DT_EMPTY:
		case BACNET_DT_NULL:
			break;

		case BACNET_DT_CHARSTRING_ARRAY:
		{
			Pr_ListOfCharStr_t *pStrArr = NULL;
			pStrArr = (Pr_ListOfCharStr_t *)pvTemp;
			Clear_CharString_List(&pStrArr->m_stStringVal.m_pstNext);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_UNSIGNED_ARRAY:
		case BACNET_DT_UNSIGNED_LIST:
		{
			Pr_ListOfUnsigned_t *pUnsignArr = NULL;
			pUnsignArr = (Pr_ListOfUnsigned_t *)pvTemp;	
			Clear_UnsignedInt_List(&pUnsignArr->m_pstUnsignVal);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_OBJECTID_ARRAY:
		{
			Pr_ListOfObjId_t *pstObjIdList = NULL;
			pstObjIdList = (Pr_ListOfObjId_t *)pvTemp;
			Clear_ObjectID_List(&pstObjIdList->m_pstArrayObjId);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_BACNETVTCLASS:
		{
			Pr_BACnetVTClass_t *pVTClass = NULL;
			pVTClass = (Pr_BACnetVTClass_t *)pvTemp;
			Clear_VtClass_List(&pVTClass);
		}
		break;

		case BACNET_DT_BACNETVTSESS:
		{
			Pr_ListOfBACnetVTSession_t *pVTSess = NULL;
			pVTSess = (Pr_ListOfBACnetVTSession_t *)pvTemp;
			Clear_VtSessions_List(&pVTSess->m_pstListOfVTSession);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_ADDRESSBINDING_LIST:
		{
			Pr_ListOfBACnetAddrBinding_t *pAddBind = NULL;
			pAddBind = (Pr_ListOfBACnetAddrBinding_t *)pvTemp;
			Clear_AddressBinding_List(&pAddBind->m_stAddBinding.pstNext, false);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_COVSUB:
		{
			Pr_ListOfBACnetCovSubs_t *pCovSub = NULL;
			pCovSub = (Pr_ListOfBACnetCovSubs_t *)pvTemp;
			Clear_ActiveCovSubs_List(&pCovSub->m_pstCOVSubscribe);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_BOOLEAN_ARRAY:
		{
			Pr_ListOfBoolen_t *pboolArr = NULL;
			pboolArr = (Pr_ListOfBoolen_t *)pvTemp;
			Clear_Boolean_List(&pboolArr->m_pstBoolList);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_DESTINATION_LIST:
		{
			Pr_ListOfBACnetDestination_t *pDest = NULL;
			pDest = (Pr_ListOfBACnetDestination_t *)pvTemp;
			Clear_Destination_List(&pDest->m_pstNCRecepient);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_EVENTPARAMETERS:
		{
			Pr_BACnetEventParameter_t *pEventPara = NULL;
			pEventPara = (Pr_BACnetEventParameter_t *)pvTemp;
			/* clear internal structures for change-of-state */
			if(EVENT_CHANGE_OF_STATE == pEventPara->m_stEventParam.m_eEventType)
			{
				/* clear list of property states */
				Clear_PropertyStates_List(&pEventPara->m_stEventParam.
					BACnetEventParameter_u.m_stCngState.m_pstListOfValues);
			}
			else if(EVENT_EXTENDED == pEventPara->m_stEventParam.m_eEventType)
			{
				/* clear list of extended parameters */
				Clear_ExtendedParameters_List(&pEventPara->m_stEventParam.
					BACnetEventParameter_u.m_stExtended.m_pstParameters);
			}
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_RECIPIENT_LIST:
		{
			Pr_ListOfBACnetRecipient_t *pRecip = NULL;
			pRecip = (Pr_ListOfBACnetRecipient_t *)pvTemp;
			Clear_Recipient_List(&pRecip->m_pstListOfRecipient);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_DATELIST:
		{
			Pr_ListOfBACnetCalendarEntry_t *pDateList = NULL;
			pDateList = (Pr_ListOfBACnetCalendarEntry_t *)pvTemp;
			Clear_CalendarEntry_List(&pDateList->m_pstListOfCalendar);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_DEVOBJPROPREFF_LIST:
		case BACNET_DT_DEVOBJPROPREFF_ARRAY:
		{
			Pr_ListOfBACnetDevObjPropRef_t *pObjPropRefList = NULL;
			pObjPropRefList = (Pr_ListOfBACnetDevObjPropRef_t *)pvTemp;
			Clear_DevObjPropReff_List(&pObjPropRefList->m_pstListOfBACnetDevObjPropReff);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_DAILYSCHEDULE:
		{
			Pr_BACnetDailySchedule_t *pWeekSchedule = NULL;
			pWeekSchedule = (Pr_BACnetDailySchedule_t *)pvTemp;
			Clear_TimeValues_List(&pWeekSchedule->m_stTimeValue.m_pstNext);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_DAILYSCHEDULE_ARRAY:
		{
			Pr_ListOfBACnetDailySchedule_t *pWeekSchedule = NULL;
			pWeekSchedule = (Pr_ListOfBACnetDailySchedule_t *)pvTemp;
			Clear_WeeklyShdl_List(pWeekSchedule);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_SPECIALEVENT_ARRAY:
		case BACNET_DT_SPECIALEVENT:
		{
			Pr_ListOfBACnetSpecialEvent_t *pExceSchedule = NULL;
			pExceSchedule = (Pr_ListOfBACnetSpecialEvent_t *)pvTemp;
			Clear_ExceptionShdl_List(&pExceSchedule->m_pstSplEvent);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_LOGBUFFER_TRENDLOG:
		{
			Pr_ListOfBACnetLogRecord_t *pTrendLogBuff = NULL;
			pTrendLogBuff = (Pr_ListOfBACnetLogRecord_t *)pvTemp;
			Clear_TrendLogRecord_List(&pTrendLogBuff->m_pstLogRecord);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_BITSTRING_ARRAY:
		{
			Pr_ListOfBitStr_t *pStrList = NULL;
			pStrList = (Pr_ListOfBitStr_t *)pvTemp;
			Clear_BitString_List(&pStrList->m_pstBitString);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_ACTION_LIST_ARRAY:
		{
			Pr_ListOfBACnetActionList_t *pActList = NULL;
			pActList = (Pr_ListOfBACnetActionList_t *)pvTemp;
			Clear_Action_List(&pActList->m_pstArrayActCmd);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_READACCESSSPECS_LIST:
		{
			Pr_ListOfReadAccessSpecs_t *pAccessSpeci = NULL;
			pAccessSpeci = (Pr_ListOfReadAccessSpecs_t *)pvTemp;
			Clear_ReadAccessSpecs_List(&pAccessSpeci->m_pstGpOfObj);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;
		
		case BACNET_DT_READACCESSRESULT_LIST:
		{
			Pr_ListOfReadAccessResult_t *pAccessResult = NULL;
			pAccessResult = (Pr_ListOfReadAccessResult_t *)pvTemp;
			Clear_ReadAccessResult_List(&pAccessResult->m_pstGpObjResults);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		#ifdef BACDEL_PR12
		case BACNET_DT_PROPERTYACCESSRESULT_ARRAY:
		case BACNET_DT_PROPERTYACCESSRESULT:
		{
			Pr_ListOfBACnetPropAccessRslt_t *pstPropAccResList  = NULL;
			pstPropAccResList = (Pr_ListOfBACnetPropAccessRslt_t *)pvTemp;
			Clear_PropAccessResult_List(&pstPropAccResList->m_pstPropAccRslt);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_NW_SECURITY_POLICY:
		case BACNET_DT_NW_SECURITY_POLICY_ARRAY:
		{
			/* Local Variables */	
			Pr_ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcyList  = NULL;
			pstNetSecPlcyList = (Pr_ListOfBACnetNwSecurityPolicy_t *)pvTemp;
			Clear_NwSecurityPolicy_List(&pstNetSecPlcyList->m_pstSecurityPolicyArr);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;
		#endif /* PR12 */

		case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:
		case BACNET_DT_AU_FACTOR_FORMAT:
		{
			Pr_ListOfBACnetAuFactorFormat_t *pAuthFactForm = NULL;
			pAuthFactForm = (Pr_ListOfBACnetAuFactorFormat_t *)pvTemp;
			Clear_AuFactorFormat_List(&pAuthFactForm->m_pstAuFactFormatList);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
		case BACNET_DT_AUTHENTICATION_POLICY:
		{
			Pr_ListOfBACnetAuPolicy_t *pAuthPolicy = NULL;
			pAuthPolicy = (Pr_ListOfBACnetAuPolicy_t *)pvTemp;
			Clear_AuPolicy_List(&pAuthPolicy->m_pstAuPolicy);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_ACCESS_RULE_ARRAY:
		case BACNET_DT_ACCESS_RULE:
		{
			Pr_ListOfBACnetAccessRule_t *pAcceRule = NULL;
			pAcceRule = (Pr_ListOfBACnetAccessRule_t *)pvTemp;
			Clear_AccessRule_List(&pAcceRule->m_pstAccessRuleArray);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_CRED_AU_FACTOR_ARRAY:
		case BACNET_DT_CRED_AU_FACTOR:
		{
			Pr_ListOfBACnetCredAuFactor_t *pCredAUFact = NULL;
			pCredAUFact = (Pr_ListOfBACnetCredAuFactor_t *)pvTemp;
			Clear_CredentialAuFactor_List(&pCredAUFact->m_pstCredAuFactArray);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
		case BACNET_DT_ASSIGNED_ACCESS_RIGHTS:
		{
			Pr_ListOfBACnetAssignedAccessRights_t *pAssAccRights = NULL;
			pAssAccRights = (Pr_ListOfBACnetAssignedAccessRights_t *)pvTemp;
			Clear_AssignedAccessRights_List(&pAssAccRights->m_pstAsngdAccessArray);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_LOGBUFFER_TLM:
		{
			Pr_ListOfBACnetLogMultipleRecord_t *pLogMultTrend = NULL;
			pLogMultTrend = (Pr_ListOfBACnetLogMultipleRecord_t *)pvTemp;
			Clear_TrendLogMultipleRecord_List(&pLogMultTrend->m_pstLogMultipleRecord);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_LOGBUFFER_EL:
		{
			Pr_ListOfBACnetEventLogRecord_t *pLogEvent = NULL;
			pLogEvent = (Pr_ListOfBACnetEventLogRecord_t *)pvTemp;
			Clear_EventLogRecord_List(&pLogEvent->m_pstEventLogRecord);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_OPTCHARSTRING_ARRAY:
		case BACNET_DT_OPTIONAL_CHARSTRING:
		{
			Pr_ListOfOptCharStr_t *pOptCharStr = NULL;
			pOptCharStr = (Pr_ListOfOptCharStr_t *)pvTemp;
			Clear_OptionalCharString_List(&pOptCharStr->m_pstOptCharStr);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_ENUM_LIST:
		{
			Pr_ListOfEnum_t *pEnumList = NULL;
			pEnumList = (Pr_ListOfEnum_t *)pvTemp;
			Clear_Enumeration_List(&pEnumList->m_pstEnumList);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_DEVOBJREFF_LIST:
		case BACNET_DT_DEVOBJREFF_ARRAY:
		{
			Pr_ListOfBACnetDevObjRef_t *pDevObjRefList;
			pDevObjRefList = (Pr_ListOfBACnetDevObjRef_t *)pvTemp;
			Clear_DevObjRef_List(&pDevObjRefList->m_pstListOfDevObjReff);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);	
		}
		break;

		case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
		{				
			AnyValue_t *pstAnyVal = NULL;
			BACNET_PROPERTY_VALUE *pstPropVal = NULL;
			pstAnyVal = (AnyValue_t *)pvTemp;
			pstPropVal = &pstAnyVal->m_stValue;
			Clear_Property_Value(pstPropVal->pstNextPropVal);
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
		}
		break;

		case BACNET_DT_SECURITY_KEYSET_ARRAY:
		{
			/* local variables */
			Pr_ListOfBACnetSecurityKeySet_t *pstSecurityKeySetArr = NULL;
			pstSecurityKeySetArr = (Pr_ListOfBACnetSecurityKeySet_t *)pvTemp;

			Clear_SecurityKeySet_List(pstSecurityKeySetArr);

			/* free allocated memory of Pr_ListOfBACnetSecurityKeySet_t pointer */
			OSAL_Free(pstSecurityKeySetArr, __FILE__, __FUNCTION__, __LINE__);
		}
		break;
		default:
			/* default case - Free the base pointer */
			OSAL_Free(pvTemp, __FILE__, __FUNCTION__, __LINE__);
			break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_PropVal_AsPer_DataType: exit \r\n");
	#endif	
}

/**
*
* DESCRIPTION 
* Function to clear property values in request or response of create 
* object-A service
*
* @param ppPropValList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_Prop_Val(property_value_t **ppPropValList)
{
    /* local variables */
    void *pvTemp = NULL;
    property_value_t *pstPropertyVal = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Prop_Val: entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == ppPropValList)
    {
        return;
    }
	if(NULL == *ppPropValList)
	{
        return;
	}

    /* get the base pointer of link list */
    pstPropertyVal = *ppPropValList;
	*ppPropValList = NULL;

    /* traverse list & free 1 by 1 */
    while(NULL != pstPropertyVal)
    {
        /* save next */
		pvTemp = pstPropertyVal->m_pstNextVal;
		/* clear memory allocated to internal pointer */
		#ifdef NEW_RP_WP_INTERFACE
		Clear_PropVal_AsPer_DataType(pstPropertyVal->m_eData_Type, 
			&pstPropertyVal->m_pvPropVal);
		#endif
        /* free allocated memory */
        OSAL_Free(pstPropertyVal, __FILE__, __FUNCTION__, __LINE__);
        /* move to next node */
        pstPropertyVal = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Prop_Val: exit \r\n");
	#endif
    return;
}
/** clears memory allocated during Rp-B, Rpm-B and Rr-b as per data types */
void Clear_Duplicate_Prop_Values(
	BACNET_DATA_TYPE eData_Type, 
	void **ppPropValue)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Duplicate_Prop_Values: entry \r\n");
	#endif

	/* switch as per data type */
	switch(eData_Type)
	{
	case BACNET_DT_COVSUB:
	case BACNET_DT_RECIPIENT_LIST:
	case BACNET_DT_ADDRESSBINDING_LIST:
	case BACNET_DT_OBJECTID_ARRAY: 
	case BACNET_DT_CHARSTRING_ARRAY: 
	case BACNET_DT_UNSIGNED_LIST:
	case BACNET_DT_DESTINATION_LIST: 
	case BACNET_DT_DATELIST:	
	case BACNET_DT_DEVOBJPROPREFF_LIST: 
	case BACNET_DT_SPECIALEVENT_ARRAY: 
	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	
	case BACNET_DT_LOGBUFFER_TRENDLOG:
	case BACNET_DT_LOGBUFFER_TLM:
	case BACNET_DT_LOGBUFFER_EL:
	case BACNET_DT_BOOLEAN_ARRAY:	
	case BACNET_DT_READACCESSSPECS_LIST:
	case BACNET_DT_READACCESSRESULT_LIST:
	case BACNET_DT_ENUM_LIST:
	case BACNET_DT_DEVOBJREFF_LIST:
	case BACNET_DT_DEVOBJREFF_ARRAY:
	case BACNET_DT_UNSIGNED_ARRAY:
	case BACNET_DT_PRIORITY_ARRAY:
	case BACNET_DT_BITSTRING_ARRAY:	
	case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:	
	case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
	case BACNET_DT_ACCESS_RULE_ARRAY:
	case BACNET_DT_CRED_AU_FACTOR_ARRAY:
	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
	case BACNET_DT_OPTCHARSTRING_ARRAY:	
	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
	{
		/* free allocated memory */
		Clear_PropVal_AsPer_DataType(eData_Type, ppPropValue);
	}
	break;

	default:
		/* do nothing */
		break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Duplicate_Prop_Values: exit \r\n");
	#endif
}

/**
*
* DESCRIPTION                                                                          
* Function to clear List type properties.
* This function clears internal lists.
*
* @param pvData		[in] pointer to property list.
* @param eDataType	[in] data type of property.
*
* @return nothing
*
*/
void Clear_Internal_ListType_Properties(
	void **pvPropVal,
	BACNET_DATA_TYPE eDataType)
{
	/* local variables */

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_Internal_ListType_Properties : Entry \r\n");
	#endif

	/* check input pointer */
    if(NULL == pvPropVal)
    {
        return;
    }
	if(NULL == *pvPropVal)
	{
        return;
	}

	/* free as per datatype */
	switch(eDataType)
	{
	case BACNET_DT_DATELIST:
	{
		/* clear calendar entry list */
		Clear_CalendarEntry_List((ListOfBACnetCalendarEntry_t **)pvPropVal);
	}
	break;

	case BACNET_DT_DESTINATION_LIST:
	{
		/* clear destination list */
		Clear_Destination_List((ListOfBACnetDestination_t **)pvPropVal);
	}
	break;

	case BACNET_DT_DEVOBJPROPREFF_LIST:
	{
		/* clear device object property reference list */
		Clear_DevObjPropReff_List((ListOfBACnetDevObjPropRef_t **)pvPropVal);
	}
	break;

	case BACNET_DT_UNSIGNED_LIST:
	{
		/* clear unsigned list */
		Clear_UnsignedInt_List((ListOfUnsigned_t **)pvPropVal);
	}
	break;

	case BACNET_DT_SPECIALEVENT_ARRAY:
	{
		/* clear special event list */
		Clear_ExceptionShdl_List((ListOfSpecialEvent_t **)pvPropVal);
	}
	break;

	case BACNET_DT_DAILYSCHEDULE_ARRAY:
	case BACNET_DT_EVENTPARAMETERS:
	{
		/* clear value as per data type */
		Clear_PropVal_AsPer_DataType(eDataType, pvPropVal);
	}
	break;

	default:
	{
		/* do nothing */
	}
	break;
	}// switch ends 

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_Internal_ListType_Properties : Exit \r\n");
	#endif
}


/**
*
* DESCRIPTION
* This function frees memory allocated while decoding notification parameters.
*
* @param pstNotifyParameters   [in]  pointer to notification data.
* @param eEventType  [in]  event type.
* @returns void.
*
*/
void Clear_Notification_Parameter(
	BACnetNotificationParameters_t *pstNotifyParameters, 
	BACNET_EVENT_TYPE eEventType)
{
    /* local variables */

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Notification_Parameter : entry \r\n"); 
	#endif

	/* check pointer */
    if(NULL == pstNotifyParameters)
    {
		return;
	}

	/* check event type */
	switch(eEventType)
	{
	case EVENT_EXTENDED:
	{
		/* free extended parameter list data */
		Clear_ExtendedParameters_List(&pstNotifyParameters->
			BACnetNotificationParameters_u.m_stExtended.m_pstParameters);
	}
	break;

	#ifdef BACDEL_PR14
	case EVENT_CHANGE_OF_RELIABILITY:
	{
		/* clear 1st property value */
		#ifdef NEW_RP_WP_INTERFACE
		Clear_PropVal_AsPer_DataType(pstNotifyParameters->
			BACnetNotificationParameters_u.m_stChangeOfReliability.
			m_stPropertyValue.m_eData_Type, 
			&pstNotifyParameters->
			BACnetNotificationParameters_u.m_stChangeOfReliability.
			m_stPropertyValue.m_pvPropVal);
		#endif
		/* free property value list data */
		Clear_Prop_Val(&pstNotifyParameters->
			BACnetNotificationParameters_u.m_stChangeOfReliability.
			m_stPropertyValue.m_pstNextVal);
	}
	break;
	#endif /* BACDEL_PR14 */

	default:
	break;
	}    

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Notification_Parameter : exit \r\n"); 
	#endif
    return;
}

/**
* 
* DESCRIPTION
* Function to clear property values of type list of bacnet time values.
*
* @param ppFreeTimeValue [in] base pointer to free list.
* @return - void;
*
*/
void Clear_TimeValues_List(BACnetTimeValue_t **ppFreeTimeValue)
{
	/* local variables */
	void *pvTemp = NULL;
    BACnetTimeValue_t   *pstTimeValue = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_TimeValues_List: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == ppFreeTimeValue)
        return;
	if(NULL == *ppFreeTimeValue)
        return;

	/* get the base pointer of link list */
    pstTimeValue = *ppFreeTimeValue;
	*ppFreeTimeValue = NULL;

    while(NULL != pstTimeValue)
    {
		/* save next */
        pvTemp = pstTimeValue->m_pstNext;
		/* free allocated memory */
        OSAL_Free(pstTimeValue,  __FILE__, __FUNCTION__, __LINE__);
		/* move to next node */
        pstTimeValue = pvTemp;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_TimeValues_List: exit \r\n");
	#endif
    return;
}

/**
* 
* DESCRIPTION
* Function to clear property values of type array of bacnet special events.
*
* @param ppSplEventList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_ExceptionShdl_List(ListOfSpecialEvent_t **ppSplEventList)
{
	/* local variable */
	void *pvTemp = NULL;
    ListOfSpecialEvent_t *pstSplEventList = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_ExceptionShdl_List: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == ppSplEventList)
        return;
	if(NULL == *ppSplEventList)
        return;

    /* get the base pointer of link list */
    pstSplEventList = *ppSplEventList;
	*ppSplEventList = NULL;

	/* free 1 by 1 */
    while(NULL != pstSplEventList)
    {
		/* save next */
        pvTemp = pstSplEventList->m_pstNext;

        /* free Time Values */
        pstSplEventList->m_stListSpecialEvent.m_stListOfTimeValues.bIsUsed = 0;
        Clear_TimeValues_List(&pstSplEventList->m_stListSpecialEvent.
			m_stListOfTimeValues.m_pstNext);
        pstSplEventList->m_stListSpecialEvent.m_stListOfTimeValues.m_pstNext = NULL;

		/* free memory & move to next */
        OSAL_Free(pstSplEventList,  __FILE__, __FUNCTION__, __LINE__);
        pstSplEventList = pvTemp;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_ExceptionShdl_List: exit \r\n");
	#endif
    return;
}

/**
* 
* DESCRIPTION
* Function to clear property values of type array of bacnet daily schedule.
*
* @param pstWeeklyList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_WeeklyShdl_List(Pr_ListOfBACnetDailySchedule_t *pstWeeklyList)
{
	/* local variable */
    int8_t i8Daycnt = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_WeeklyShdl_List: entry \r\n");
	#endif

	/* check pointer */
	if(NULL != pstWeeklyList)
    {
		/* daily schedule is array of 7 */
		for(i8Daycnt = 0; i8Daycnt < BACNET_ARRAY_OF_SEVEN; i8Daycnt++ )
		{
			/* free Time Values */
            Clear_TimeValues_List(&pstWeeklyList->m_stTimeValue[i8Daycnt].m_pstNext);
            memset(&pstWeeklyList->m_stTimeValue[i8Daycnt], 0, 
				sizeof(BACnetTimeValue_t));
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Clear_WeeklyShdl_List: exit \r\n");
	#endif
    return;
}

/**
* 
* DESCRIPTION
* Function to clear property values of type array of bacnet calendar entry.
*
* @param ppCalendarEntryList [in] base pointer to free list.
* @return - void;
*
*/
void Clear_CalendarEntry_List(ListOfBACnetCalendarEntry_t **ppCalendarEntryList)
{
    /* local variables */
    void *pvTemp = NULL;
	ListOfBACnetCalendarEntry_t *pstCalenEntryList = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_CalendarEntry_List : entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == ppCalendarEntryList)
        return;
	if(NULL == *ppCalendarEntryList)
        return;

    /* get the base pointer of link list */
    pstCalenEntryList = *ppCalendarEntryList;
	*ppCalendarEntryList = NULL;

    /* free 1 by 1 */
    while(NULL != pstCalenEntryList)
    {
        /* save next */
        pvTemp = pstCalenEntryList->m_pstNext;
        /* free allocated memory */
        OSAL_Free(pstCalenEntryList, __FILE__, __FUNCTION__, __LINE__);
        /* move to next */
        pstCalenEntryList = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_CalendarEntry_List : exit \r\n");
	#endif
    return;
}

#ifdef BACDEL_OBJ_NP
/**
*
* DESCRIPTION
* Function to clear memory allocated to list of BDT entry data type.
*
* @param ppBDTEntryList [in] base pointer to free list.
* @return [out] void
*
*/
void Clear_BdtEntry_List(ListOfBACnetBDTEntry_t **ppBdtEntryList)
{
	/* local variables */
	ListOfBACnetBDTEntry_t *pstTemp = NULL;
	ListOfBACnetBDTEntry_t *pstBDTEntryList = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: Clear_BdtEntry_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == ppBdtEntryList)
		return;
	if(NULL == *ppBdtEntryList)
		return;

	/* get the base pointer of link list */
	pstBDTEntryList = *ppBdtEntryList;
	*ppBdtEntryList = NULL;

	/* traverse list & free 1 by 1 */
	while(NULL != pstBDTEntryList)
	{
		/* save next */
		pstTemp = pstBDTEntryList->m_pstNext;
		/* free allocated memory */
		OSAL_Free(pstBDTEntryList, __FILE__, __FUNCTION__, __LINE__);
		/* move to next node */
		pstBDTEntryList = pstTemp;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: Clear_BdtEntry_List: Exit \r\n");
	#endif
	return;
}

/**
*
* DESCRIPTION
* Function to clear memory allocated to list of FDT entry data type.
*
* @param ppFDTEntryList [in] base pointer to free list.
* @return [out] void
*
*/
void Clear_FdtEntry_List(ListOfBACnetFDTEntry_t **ppFdtEntryList)
{
	/* local variables */
	ListOfBACnetFDTEntry_t *pstTemp = NULL;
	ListOfBACnetFDTEntry_t *pstFDTEntryList = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: Clear_FdtEntry_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == ppFdtEntryList)
		return;
	if(NULL == *ppFdtEntryList)
		return;

	/* get the base pointer of link list */
	pstFDTEntryList = *ppFdtEntryList;
	*ppFdtEntryList = NULL;

	/* traverse list & free 1 by 1 */
	while(NULL != pstFDTEntryList)
	{
		/* save next */
		pstTemp = pstFDTEntryList->m_pstNext;
		/* free allocated memory */
		OSAL_Free(pstFDTEntryList, __FILE__, __FUNCTION__, __LINE__);
		/* move to next node */
		pstFDTEntryList = pstTemp;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: Clear_FdtEntry_List: Exit \r\n");
	#endif
	return;
}
/**
*
* DESCRIPTION
* Function to clear memory allocated to list of octet string value data type.
*
* @param ppRealList [in] base pointer to free list.
* @return [out] void
*
*/
void Clear_OctetString_List(ListOfOctetStr_t **ppOctetStrList)
{
	/* local variables */
	ListOfOctetStr_t *pstTemp = NULL;
	ListOfOctetStr_t *pstOctetStrValList = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: Clear_OctetString_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == ppOctetStrList)
		return;
	if(NULL == *ppOctetStrList)
		return;

	/* get the base pointer of link list */
	pstOctetStrValList = *ppOctetStrList;
	*ppOctetStrList = NULL;

	/* traverse list & free 1 by 1 */
	while(NULL != pstOctetStrValList)
	{
		/* save next */
		pstTemp = pstOctetStrValList->m_pstNext;
		/* free allocated memory */
		OSAL_Free(pstOctetStrValList, __FILE__, __FUNCTION__, __LINE__);
		/* move to next node */
		pstOctetStrValList = pstTemp;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: Clear_OctetString_List: Exit \r\n");
	#endif
	return;
}

#endif
/*********************** end of propertyClearValues.c file *********************/

