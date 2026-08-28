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
*	 serviceEventReportingIntrinsic.c
*                                                                      
*   AUTHORS                                                                             
*		Pratham N. Murkute
*                                                                         
*   DESCRIPTION                                                            
*		Intrinsic Event Notification Reporting.
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	    DATE           NAME                DESCRIPTION
*	03/03/2020  Pranav Phadatare		Adding support to Fault Algorithm- Out of Range
*										Modified fucntion-IntrinsicReportingTimer()
*										UpdateEventState()
******************************************************************************/
#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)

/* include header files */
#include "serviceEventReportingIntrinsic.h"

#include "serviceEventNotification_B.h"

#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"

/* global structure to create singly linklist for event_state updation
	for intrinsic reporting */
IntrinsicLinkList_t g_stIntrinsicReporting = {0};


/** Function to allocate memory for LinkList_t struct to create link list */
static LinkList_t *Allocate_Memory_For_Node(void)
{
	LinkList_t *pstTempNode = NULL;

	/* Assign memory to the pointer */
    pstTempNode = OSAL_Malloc(sizeof(LinkList_t),  __FILE__, __FUNCTION__, __LINE__);
    if(NULL == pstTempNode)
    {
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: Allocate_Memory_To_Node: malloc failed \r\n");
		#endif

        return NULL;
    }
	/* return pointer of assigned memory */
	return pstTempNode;
}

/**	adds new node to linklist */
static void Add_New_Node_To_Link_List(MsgQueList_t *pstData)
{
    /* local variables */
	LinkList_t *pstTempNode = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Add_Node_To_Link_List: entry \r\n");
	#endif

    /* allocate memory */
    pstTempNode = Allocate_Memory_For_Node();
    if(NULL == pstTempNode)
    {
        return;
    }

	/* next of newely created node is always NULL */
	pstTempNode->m_pstNext = NULL;
	/* store data for that node */
	memcpy(&pstTempNode->m_stListData, pstData, sizeof(MsgQueList_t));

	if(g_stIntrinsicReporting.pstFirstNode == NULL)
	{
		/*update the root node address only when 1st node for the list is created */
		g_stIntrinsicReporting.pstFirstNode = pstTempNode;
	}
	else
	{
		/* create the list here */
		(*g_stIntrinsicReporting.pstLastNode).m_pstNext = pstTempNode;
	}
	/* update the current node address */
	g_stIntrinsicReporting.pstLastNode = pstTempNode;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Add_Node_To_Link_List: exit \r\n");
	#endif

	return;
}

/**	replaces old node from linklist with new data */
static void Replace_Node_From_Link_List(LinkList_t *pstLocalCurrentNode, MsgQueList_t *pstData)
{
	/* copy data from stData to pstLocalCurrentNode */
	memcpy(&pstLocalCurrentNode->m_stListData, pstData, sizeof(MsgQueList_t));
}

/**
*                                                                    
* DESCRIPTION                                                                          
*   This API creates the linklist for objects whose present value has changed and 
*	monitors the present value till time_delay (property) expires
*	If it finds that event state needs to be changed after expiry of timer, then it
*	will signal out to update the event state 
*
*@return				[out]	BACDEL_SUCCESS if successful.
*
**/
BACNET_RETURN_TYPE Create_EventState_LinkList(BACNET_PROPERTY_ID	ePropId,
											  BACNET_OBJECT_TYPE	eObjectType,
											  void *				pvObjBaseAddr,
											  BACNET_EVENT_STATE	eEventStateTo,
											  BACNET_EVENT_STATE	eEventStateFrom,
											  BACNET_RELIABILITY    eReliabilty,
											  PropertyValue_u		*puPresentValue,
											  uint32_t				u32TimeDelay,
											  uint32_t				u32TimeDelayNormal,
											  void *				pvVirtualDev)
{
	/* default return value */
	BACNET_RETURN_TYPE eFuncReturn = BACDEL_SUCCESS;
	MsgQueList_t stData = {0};
	LinkList_t *pstLocalCurrentNode = NULL;
	bool bNodeAlreadyExist = FALSE;
	ulong32_t ul32BaseAddr = 0;			/* for object base address */
	ulong32_t ul32OffsetAddr = 0;		/* for data offset address */
	uint32_t u32ObjectId;				/* for object id */

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Create_EventState_LinkList: entry \r\n");
	#endif

	if(NULL == pvObjBaseAddr || NULL == pvVirtualDev)
	{
		return BACDEL_ERROR;
	}

	/* get the base address */
	ul32BaseAddr = (ulong32_t)pvObjBaseAddr;
	/* get offset address of object if of given object type & given object base address */
	GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32OffsetAddr); 
	/* Copy data from property offset address to u32ObjectId */
	memcpy(&u32ObjectId, (uint32_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint32_t));

	/* Fill the data for the linklist node */
	stData.m_eEventStateTo		= eEventStateTo;
	stData.m_eEventStateFrom	= eEventStateFrom;
	stData.m_eReliabilty 		= eReliabilty;
	stData.m_eObjectType		= eObjectType;
	stData.m_u32ObjectId		= u32ObjectId;
	stData.m_pvObjBaseAddr		= pvObjBaseAddr;
	stData.m_ePropId			= ePropId;
	stData.m_pvVirtualDev		= pvVirtualDev;
    stData.m_u32DevId           = ((virtualDevData_t *)pvVirtualDev)->
                                    m_stDevObject.m_stObjectID.m_u32ObjId;

	/* updated time delay & check if double notification */
	if(IsDoubleNotification(eObjectType, eEventStateFrom, eEventStateTo))
	{
		#ifdef BACDEL_PR14
		stData.m_i32TimeDelay		= u32TimeDelayNormal;
		#else
		stData.m_i32TimeDelay		= u32TimeDelay;
		#endif
		stData.m_i32TimeDelayBackup = u32TimeDelay;
	}
	else
	{
		if(EVENT_STATE_NORMAL == eEventStateTo)
		{
			#ifdef BACDEL_PR14
			stData.m_i32TimeDelay		= u32TimeDelayNormal;
			stData.m_i32TimeDelayBackup = u32TimeDelayNormal;
			#else
			stData.m_i32TimeDelay		= u32TimeDelay;
			stData.m_i32TimeDelayBackup = u32TimeDelay;
			#endif
		}
		else if(EVENT_STATE_OFFNORMAL == eEventStateTo ||
			EVENT_STATE_HIGH_LIMIT == eEventStateTo ||
			EVENT_STATE_LOW_LIMIT == eEventStateTo)
		{
			stData.m_i32TimeDelay		= u32TimeDelay;
			stData.m_i32TimeDelayBackup = u32TimeDelay;
		}
	}

	/* copy the data */
	memcpy(&stData.m_uPropValue, puPresentValue, sizeof(PropertyValue_u));

	/* wait till the mutex is aquired */
	Osal_Wait_Mutex(g_stIntrinsicReporting.hMtxLock, INFINITE); 

	/* first node in linklist should be directly added to list */
	if(g_stIntrinsicReporting.pstFirstNode == NULL && 
		g_stIntrinsicReporting.pstLastNode == NULL)
	{
		Add_New_Node_To_Link_List(&stData);
	}
	/* if 1st node exists, then we need to check if the node with same object type & id already exists */
	else
	{
		/* local pointer to traverse linklist */
		pstLocalCurrentNode = g_stIntrinsicReporting.pstFirstNode;
		/* traverse the linklist */
		while(pstLocalCurrentNode != NULL)
		{
			if(pstLocalCurrentNode->m_stListData.m_pvObjBaseAddr == stData.m_pvObjBaseAddr &&
				pstLocalCurrentNode->m_stListData.m_eObjectType == stData.m_eObjectType &&
				pstLocalCurrentNode->m_stListData.m_u32ObjectId == stData.m_u32ObjectId /*&&
				pstLocalCurrentNode->m_stListData.m_eEventStateFrom == stData.m_eEventStateFrom*/)
			{
				bNodeAlreadyExist = TRUE;
				break;
			}
			/* maintain the previous node pointer */
			pstLocalCurrentNode = pstLocalCurrentNode->m_pstNext;
		}
		if(bNodeAlreadyExist == TRUE)
		{
			bNodeAlreadyExist = FALSE;
			/* replace the data of node with the new data */
			Replace_Node_From_Link_List(pstLocalCurrentNode, &stData);

			#if(defined DEBUG_PRINTF && DL_2)
			Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			Create_EventState_LinkList: node replaced in intrinsic linklist \r\n");
			#endif
		}
		else
		{
			/* since node with same obj address does not exist, add new node to link list */
			Add_New_Node_To_Link_List(&stData);
		}
	}

	/* release the mutex */
	Osal_Release_Mutex(g_stIntrinsicReporting.hMtxLock);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Create_EventState_LinkList: exit \r\n");
	#endif

	return 	eFuncReturn;
}

/**	deletes the first node of linklist and updates the firstnode pointer	*/
static void Delete_FirstNode(void)
{
	LinkList_t *pstDummyNode = NULL;
	pstDummyNode = g_stIntrinsicReporting.pstFirstNode->m_pstNext;
	OSAL_Free(g_stIntrinsicReporting.pstFirstNode,  __FILE__, __FUNCTION__, __LINE__);
	g_stIntrinsicReporting.pstFirstNode = pstDummyNode;
}

/**	deletes the last node of linklist and updates the lastnode pointer	*/
static void Delete_LastNode(void)
{
	g_stIntrinsicReporting.pstPrevious->m_pstNext = NULL;
	OSAL_Free(g_stIntrinsicReporting.pstLastNode,  __FILE__, __FUNCTION__, __LINE__);
	g_stIntrinsicReporting.pstLastNode = g_stIntrinsicReporting.pstPrevious;
}

/**	deletes any node in linklist that is anywhere between the first & last node	*/
static void Delete_MiddleNode(LinkList_t *pstLocalCurrentNode)
{
	g_stIntrinsicReporting.pstPrevious->m_pstNext = pstLocalCurrentNode->m_pstNext;
	OSAL_Free(pstLocalCurrentNode,  __FILE__, __FUNCTION__, __LINE__);
	if(g_stIntrinsicReporting.pstPrevious->m_pstNext == NULL)
		g_stIntrinsicReporting.pstLastNode = 
		g_stIntrinsicReporting.pstPrevious;
}

/**	if linklist has only one node, then delete it using this function	*/
static void Delete_The_Only_Node_In_LinkList(void)
{
	OSAL_Free(g_stIntrinsicReporting.pstFirstNode,  __FILE__, __FUNCTION__, __LINE__);
	g_stIntrinsicReporting.pstFirstNode = NULL;
	g_stIntrinsicReporting.pstLastNode = NULL;
	g_stIntrinsicReporting.pstPrevious = NULL;
}

/**	check if node is to be deleted from linklist */
static void Check_And_Delete_This_Node(LinkList_t *pstDeleteNode)
{
	if(g_stIntrinsicReporting.pstFirstNode == g_stIntrinsicReporting.pstLastNode 
		&& pstDeleteNode == g_stIntrinsicReporting.pstFirstNode)
	{
		Delete_The_Only_Node_In_LinkList();
	}
	else if(pstDeleteNode == g_stIntrinsicReporting.pstFirstNode)
	{
		Delete_FirstNode();
	}
	else if(pstDeleteNode == g_stIntrinsicReporting.pstLastNode)
	{
		Delete_LastNode();
	}
	else if(pstDeleteNode!= g_stIntrinsicReporting.pstFirstNode 
		&& pstDeleteNode != g_stIntrinsicReporting.pstLastNode)
	{
		Delete_MiddleNode(pstDeleteNode);
	}
	else;	// error condition
}

/**
*******************************************************************************
*DESCRIPTION
*   This is timer for Intrinsic reporting service. It is called every 1 second.
*
*	Scan entire intrinsic reporting linklist.
*   - decrement time_delay by 1 second for every node in linklist. 
*   - if time_delay becomes zero check actual PV & PV in node matches
*       - if it matches, update event state / reliablity (as applicable)
*       - else delete the node.
*   - repeate same for every node in list.
*
******************************************************************************/
TIMER_CALLBACK IntrinsicReportingTimer(void* lpParam, bool TimerOrWaitFired)
{
    /* local variables */
	LinkList_t	*pstLocalCurrentNode = NULL;	/* local pointer to traverse linklist */
	LinkList_t	*pstDeleteNode = NULL;          /* holds node to be deleted */
	ulong32_t	ul32BaseAddr = 0;				/* for object base address */
	ulong32_t	ul32OffsetAddr = 0;				/* for data offset address */
    
	/* for update commom property */
	void		*pvDummy = NULL;
	uint32_t	u32PropertyAdd = 0;
    
	/* flags for synchronization, etc */
	bool		bDeleteNode = FALSE;
	bool		bUpdateEventState = FALSE;
	bool		bUpdateReliability = FALSE;
	bool bDoubleNotification = FALSE;
	static bool bIntrinsicTimerFlag = false;
    
	virtualDevData_t *pVirtualDev = NULL;       /* virtual device */
	BACNET_RELIABILITY  eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
    int32_t i32Len = 0;                         /* hold Sizeof */
    PropertyValue_u uPropVal = {0};             /* variable to compare property value */

	/* avoid concurrent execution of multiple timer instances */
	if(true == bIntrinsicTimerFlag)
	{
		/* return from function */
		#ifdef __linux
		return NULL;
		#else
		return NULL;
		#endif
	}
	/* set the flag */
	bIntrinsicTimerFlag = true;

    /* timer entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: IntrinsicReportingTimer: entry \r\n");
	#endif

    /* return if list is empty */
	if(g_stIntrinsicReporting.pstFirstNode == NULL)
	{
		/* reset the flag */
		bIntrinsicTimerFlag = false;
#ifdef __linux
		return NULL;
#else
		return NULL;
#endif
	}
	else
	{
		/* Wait till the mutex handle is released */
		Osal_Wait_Mutex(g_stIntrinsicReporting.hMtxLock, INFINITE);  

        /* maintain the previous node pointer : assign to first node at start */
        g_stIntrinsicReporting.pstPrevious = g_stIntrinsicReporting.pstFirstNode;

		/* get the 1st node pointer */
		pstLocalCurrentNode = g_stIntrinsicReporting.pstFirstNode;

		while(pstLocalCurrentNode != NULL)
		{
			/* get host device address */
			if(NULL != pstLocalCurrentNode->m_stListData.m_pvVirtualDev)
				pVirtualDev = pstLocalCurrentNode->m_stListData.m_pvVirtualDev;
			else break;
			/* decrement time_delay by 1 : as this timer callback function is called every 1 sec */
			pstLocalCurrentNode->m_stListData.m_i32TimeDelay -= 1;
			if(pstLocalCurrentNode->m_stListData.m_i32TimeDelay <= 0) 
			{
				/* get object base address */
				ul32BaseAddr = (ulong32_t)(pstLocalCurrentNode->m_stListData.m_pvObjBaseAddr);
				//if(pstLocalCurrentNode->m_stListData.m_ePropId == PROP_PRESENT_VALUE)
				//{
					/* get offset address of present value */
					GET_REF_PROP_VAL_OFFSET(pstLocalCurrentNode->m_stListData.m_eObjectType, 
						ul32OffsetAddr, i32Len);

                    switch(i32Len)
                    {
                    case BACNET_ZERO:
                        /* delete this node */
				        bDeleteNode = TRUE;
				        pstDeleteNode = pstLocalCurrentNode;
                        break;

                    default:
                        /* copy the property value */
                        memcpy(&uPropVal, (void *)(ul32BaseAddr+ul32OffsetAddr), i32Len);
                        /* compare the values */
                        if(!memcmp(&uPropVal, &pstLocalCurrentNode->m_stListData.m_uPropValue, i32Len))
                        {
                            // update event state 
							bUpdateEventState = TRUE;
                            if(EVENT_STATE_FAULT ==
                                pstLocalCurrentNode->m_stListData.m_eEventStateTo)
							{
								/* update reliablity property */
								bUpdateReliability = TRUE;
								eReliabilty = pstLocalCurrentNode->m_stListData.m_eReliabilty;
								/* no need to write event state as it will be written 
								recursively when reliability is written */
								bUpdateEventState = FALSE;
							}
							else if(EVENT_STATE_FAULT == 
                                pstLocalCurrentNode->m_stListData.m_eEventStateFrom)
							{
								/* update reliablity property */
								bUpdateReliability = TRUE;
								eReliabilty = pstLocalCurrentNode->m_stListData.m_eReliabilty;
								/* no need to write event state as it will be written 
								recursively when reliability is written */
								bUpdateEventState = FALSE;
							}
							else;
                        }
                    }//switch ends


					if(bUpdateEventState)
					{
						/* reset bUpdateEventState */ 
						bUpdateEventState = FALSE;
						/* assign present value pointer */
						pvDummy = &pstLocalCurrentNode->m_stListData.m_uPropValue;
						/* check if double notification */
						if(IsDoubleNotification(pstLocalCurrentNode->m_stListData.m_eObjectType,
							pstLocalCurrentNode->m_stListData.m_eEventStateFrom,
							pstLocalCurrentNode->m_stListData.m_eEventStateTo))
						{
							/* backup to state */
							pstLocalCurrentNode->m_stListData.m_eBackupToState = 
                                pstLocalCurrentNode->m_stListData.m_eEventStateTo;
							/* change to state to normal */
							pstLocalCurrentNode->m_stListData.m_eEventStateTo = 
								EVENT_STATE_NORMAL;
							/* set bDoubleNotification */
							bDoubleNotification = TRUE;
						}
						/* call UpdateCommonProperty function to change event_state */
						UpdateCommonProperty(pstLocalCurrentNode->m_stListData.m_eObjectType, 
							(void *)ul32BaseAddr, 
							PROP_EVENT_STATE, 
							-1, 
							(void *)&pstLocalCurrentNode->m_stListData.m_eEventStateTo, 
							READ_ONLY,
							&u32PropertyAdd, &pvDummy, pVirtualDev,
							false, false, false);
						if(bDoubleNotification)
						{
							/* reset bDoubleNotification */
							bDoubleNotification = FALSE;
							/* restore time delay for 2nd notification */
							pstLocalCurrentNode->m_stListData.m_i32TimeDelay = 
								pstLocalCurrentNode->m_stListData.m_i32TimeDelayBackup;
							//pstLocalCurrentNode->m_stListData.m_i32TimeDelay = 1;
							/* update from & to state */
							pstLocalCurrentNode->m_stListData.m_eEventStateFrom = 
								EVENT_STATE_NORMAL;
							pstLocalCurrentNode->m_stListData.m_eEventStateTo = 
                                pstLocalCurrentNode->m_stListData.m_eBackupToState;
						}
						else
						{
							/* delete this node */
							bDeleteNode = TRUE;
							pstDeleteNode = pstLocalCurrentNode;
						}
					}

					else if(bUpdateReliability)
					{
						/* reset bUpdateReliability */
						bUpdateReliability = FALSE;
						/* assign value to pointer */
						pvDummy = &pstLocalCurrentNode->m_stListData.m_uPropValue;
						/* check if double notification is required */
						if(IsDoubleNotification(pstLocalCurrentNode->m_stListData.m_eObjectType,
							pstLocalCurrentNode->m_stListData.m_eEventStateFrom,
							pstLocalCurrentNode->m_stListData.m_eEventStateTo))
						{
							#ifdef FAULT_ALGORITHM
							/* backup to state */
							pstLocalCurrentNode->m_stListData.m_eBackupToState =
                                pstLocalCurrentNode->m_stListData.m_eEventStateTo;
							/* change to state to normal */
							pstLocalCurrentNode->m_stListData.m_eEventStateTo =
								EVENT_STATE_NORMAL;
							#endif /*FAULT_ALGORITHM*/
							/* set bDoubleNotification */
							bDoubleNotification = TRUE;
						}

						/* call UpdateCommonProperty function to change event_state */
						UpdateCommonProperty(pstLocalCurrentNode->m_stListData.m_eObjectType, 
							(void *)ul32BaseAddr, 
							PROP_RELIABILITY, 
							-1, 
							(void *)&eReliabilty, 
							READ_ONLY,
							&u32PropertyAdd, &pvDummy, pVirtualDev,
							false, false, false);
						/* NOTE - bWriteNull is sent true to indicate that this 
							is call from timer */
						if(bDoubleNotification)
						{
							/* reset bDoubleNotification */
							bDoubleNotification = FALSE;

							#ifdef FAULT_ALGORITHM
							/* restore time delay for 2nd notification */
							pstLocalCurrentNode->m_stListData.m_i32TimeDelay =
								pstLocalCurrentNode->m_stListData.m_i32TimeDelayBackup;
							//pstLocalCurrentNode->m_stListData.m_i32TimeDelay = 1;
							/* update from & to state */
							pstLocalCurrentNode->m_stListData.m_eEventStateFrom =
								EVENT_STATE_NORMAL;
							pstLocalCurrentNode->m_stListData.m_eEventStateTo =
                                pstLocalCurrentNode->m_stListData.m_eBackupToState;
							#endif /*FAULT_ALGORITHM*/
						}
						else
						{
							/* delete this node */
							bDeleteNode = TRUE;
							pstDeleteNode = pstLocalCurrentNode;
						}
					}
                    else
                    {
                        /* time_delay <= 0 & present value got changed, or object was deleted.
                            no need to monitor this node further, delete it. */
                        /* delete this node */
					    bDeleteNode = TRUE;
					    pstDeleteNode = pstLocalCurrentNode;
                    }
				//}// end of if 

			}//end of if time_delay <= 0
			
			if(bDeleteNode == FALSE)
			{
				/* maintain the previous node pointer */
				g_stIntrinsicReporting.pstPrevious = pstLocalCurrentNode;	
				/* move to next node in linklist */
				pstLocalCurrentNode = pstLocalCurrentNode->m_pstNext;
			}
			else 
			{
				/* if current node is deleted, then no need to update previous node pointer */
				bDeleteNode = FALSE;
				/* first move to next node in linklist */
				pstLocalCurrentNode = pstLocalCurrentNode->m_pstNext;
				/* now delete node */
				Check_And_Delete_This_Node(pstDeleteNode);
			}
		}//end of while

        /* maintain the previous node pointer */
	    g_stIntrinsicReporting.pstPrevious = NULL;

		/* release the mutex */
		Osal_Release_Mutex(g_stIntrinsicReporting.hMtxLock);
	}

	/* reset the flag */
	bIntrinsicTimerFlag = false;

    /* timer exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: IntrinsicReportingTimer: exit \r\n");
	#endif

#ifdef __linux
    return NULL;
#endif
    return NULL;
}

/**
*	Function to compare the alarm values & fault values list with present value
*	@param -	[in]	pvAvaluesList - pointer to list of alarm values
*	@param -	[in]	pvFvaluesList - pointer to list of fault values
*	@param -	[in]	puPresentValue - present value
*	@param -	[in]	eObjType      - object type.
*	@param -	[in]	pvObject      - object base pointer.
*   @param -    [in]    pbPrevMatchFound - true if match found in alarm value
*
*	@returns -	[out]	0 if no match.
*                       1 if match is found in alarm values list & 
*                       -1 if match is found in fault values list.
**/
static int8_t Check_AFvalues_List(void *pvAvaluesList, void *pvFvaluesList,
    PropertyValue_u *puPresentValue, BACNET_OBJECT_TYPE eObjType, void *pvObject,
	bool *pbPrevMatchFound)
{
    /* local variables */
    /* ToDo :  Make it union */
    ListOfUnsigned_t         *pstAFvalues = NULL;

	/* entry message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Check_AFvalues_List : Entry \r\n"); 
	#endif

	/* null check input pointer */
    if(NULL == pvObject || NULL == pbPrevMatchFound)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
        Check_AFvalues_List: null pointer \r\n");
		#endif
        return NO_MATCH_FOUND;
	}
	/* set default value */
	*pbPrevMatchFound = false;

    /* switch to type of object */
    switch(eObjType)
    {
    case OBJECT_MULTI_STATE_INPUT:
    case OBJECT_MULTI_STATE_VALUE:
	{
        /* scan through the alarm values list */
        pstAFvalues = pvAvaluesList;
        while(pstAFvalues != NULL)
        {
	        /* check if match found */
	        if(pstAFvalues->m_u32Value == puPresentValue->u32Val)
            {
		        return MATCH_FOUND_IN_ALARM_VALUES;
            }
	        pstAFvalues = pstAFvalues->m_pstNext;
        }
        /* scan through the fault values list */
        pstAFvalues = pvFvaluesList;
        while(pstAFvalues != NULL)
        {
	        /* check if match found */
            if(pstAFvalues->m_u32Value == puPresentValue->u32Val)
            {
		        return MATCH_FOUND_IN_FAULT_VALUES;
            }
	        pstAFvalues = pstAFvalues->m_pstNext;
        }
        break;
	}
	/* do nothing */
    default:
        break;
    }

	/* exit message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Check_AFvalues_List : Exit \r\n"); 
	#endif

	/* return false if no match is found */
	return NO_MATCH_FOUND;
}

/**
*	Function checks if event state property needs to be updated or not depending 
*	upon the present value.
*	This function is called only after all objects have been created && when present
*	value of any object changes.
*
*	@param		[in]		eObjectType - type of object
*	@param		[in]		pu32BaseAddr - base address of object structure
*	@param		[in]		pvData - present value
*	@param		[out]		pbEventChange - tells if event state needs to be updated or not
*	@param		[out]		eEventStateTo - hold the new value of event state
*	@param		[out]		eEventStateTo - hold the new value of event state
*
*	@return		BACDEL_SUCCESS / BACDEL_ERROR
**/
BACNET_RETURN_TYPE UpdateEventState(BACNET_OBJECT_TYPE eObjectType,
									void *pvObject,
									void *pvData,
									bool *pbEventChange,
									BACNET_EVENT_STATE	*peEventStateTo,
									BACNET_EVENT_STATE	*peEventStateFrom,
									BACNET_RELIABILITY  *peReliabilty,
									PropertyValue_u *puPresentValue)
{
	/* default return type */
	BACNET_RETURN_TYPE  eFuncRet = BACDEL_SUCCESS;
	/* variable to store property offset value */
	ulong32_t ul32OffsetAddr = 0;
	/* variable to store the base address of object */
	ulong32_t ul32BaseAddr	= 0;
	/* variable to store value of EVENT_STATE property */
	BACNET_EVENT_STATE	eEventState;
	/* to save reliablity */
	BACNET_RELIABILITY  eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
    /* hold sizeof */
    int32_t i32Len = 0;
	#ifdef BACDEL_PR14
	/* variable to store event detection enable value */
	bool bEventDetectionEnable = false;
	/* variable to store value of event algoritm inhibit property */
	bool bEventAlgoInhibit = false;
	/* variable to store value of reliability evaluation inhibit property */
	bool bReliaEvaluInhibitTrue = false;
	/* bErrorFlag = true indicate PV matches in alarm value or fault value */
	bool bErrorFlag = true;
	#endif /* PR14 */
	/* alarm value match flag */
	bool bPrevAlarmValMatch = false;

	/* print function entry debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: UpdateEventState : Entry \r\n"); 
	#endif

	/* check input pointers */
    if(NULL == pvObject || NULL == pvData || NULL == pbEventChange 
		|| NULL == peEventStateTo || NULL == peEventStateFrom ||
		NULL == puPresentValue)
	{
        return BACDEL_ERROR;
	}

	/* set to default value */
	*pbEventChange = false;

	/* get the base address of object */
    ul32BaseAddr = (ulong32_t)pvObject;

	#ifdef BACDEL_PR14
	/* read event detection enable prop value */
	GET_INTRINSIC_OFFSET(eObjectType, m_stEventDetectionEnable.m_bVal, ul32OffsetAddr); 
	memcpy(&bEventDetectionEnable, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(bool));
	/* check event detection enable is true or false */
	if(!bEventDetectionEnable)
	{
		/* intrinsic reporting is disabled in this object */
		*pbEventChange = FALSE;
		return eFuncRet;
	}
	#endif /* PR14 */

	/* get the offset address of reliablity property */
	GET_RELIABILITY_OFFSET(eObjectType, m_stReliability.m_eReliabilty, ul32OffsetAddr);
	/* copy the value */
	memcpy(&eReliabilty, (BACNET_RELIABILITY *)(ul32BaseAddr+ul32OffsetAddr),
		sizeof(BACNET_RELIABILITY));
	if(RELIABILITY_NO_FAULT_DETECTED != eReliabilty)
	{
		if(OBJECT_ANALOG_INPUT == eObjectType ||
			OBJECT_ANALOG_VALUE == eObjectType)
		{
			; /* do nothing */
		}
		else
		{
			#if(defined DEBUG_PRINTF && DL_2)
			Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			UpdateEventState: reliablity is not no fault detected. \r\n"); 
			#endif

			*pbEventChange = FALSE;
			return eFuncRet;
		}
	}

	#ifdef BACDEL_PR14
	#endif /* PR14 */

	/* get offset of event state & copy data into eEventStates from property offset address */
	GET_EVENTSTATE_OFFSET(eObjectType, m_stEventState.m_eEventState, ul32OffsetAddr);
	memcpy((void *)&eEventState, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(BACNET_EVENT_STATE));
	*peEventStateFrom = eEventState;

	/* switch to object type */
	switch(eObjectType)
	{
		#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV || defined BACDEL_OBJ_PC)
		case OBJECT_ANALOG_INPUT:
		case OBJECT_ANALOG_VALUE:
			{
				#ifdef FAULT_ALGORITHM
					/* local variable for Fault Algorithm*/
					Float_t faultHighLimit = 0.0, faultLowLimit = 0.0;
				#endif /*FAULT_ALGORITHM*/
				/* local variables */
				Float_t		fHighLimit = 0.0,	fLowLimit = 0.0,	fDeadband = 0.0,	fPresentValue = 0.0;
				#ifdef BACDEL_PR14
				uint8_t u8BitString = 0;
				#endif
				fPresentValue = *((Float_t *)pvData);
				puPresentValue->fVal = fPresentValue;

				#ifdef FAULT_ALGORITHM
				/* local variable for Fault Algorithm*/
				/* get offset of fault high limit & save value from that address to corresponding local variable */
				GET_FAULT_HL_LL_VALUE_OFFSET(eObjectType, m_stFaultHighLimit ,ul32OffsetAddr,i32Len);
				memcpy((void *)&faultHighLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* get offset of fault low limit & save value from that address to corresponding local variable */
				GET_FAULT_HL_LL_VALUE_OFFSET(eObjectType, m_stFaultLowLimit ,ul32OffsetAddr,i32Len);
				memcpy((void *)&faultLowLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				#endif/*FAULT_ALGORITHM*/

				/* get offset of high limit & save value from that address to corresponding local variable */
				GET_HL_LL_VALUE_OFFSET(eObjectType, m_stHighLimit ,ul32OffsetAddr,i32Len);
				memcpy((void *)&fHighLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* get offset of low limit & save value from that address to corresponding local variable */
				GET_HL_LL_VALUE_OFFSET(eObjectType, m_stLowLimit ,ul32OffsetAddr,i32Len);
				memcpy((void *)&fLowLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* get offset of deadband & save value from that address to corresponding local variable */
				GET_DEADBAND_COV_INCR_VAL_OFFSET(eObjectType,m_stDeadBand,ul32OffsetAddr,i32Len);
				memcpy((void *)&fDeadband, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* verify that high limit & low limit are not same, if they
						are same then dont compare */
				if(fLowLimit == fHighLimit
				#ifdef FAULT_ALGORITHM
				  ||faultLowLimit == faultHighLimit
				#endif /*FAULT_ALGORITHM*/
				  )
				{
					*pbEventChange = FALSE;
					break;
				}

				#ifdef BACDEL_PR14
				/* get limit_enable property offset & copy data into local variable */
				GET_LIMITENABLE_OFFSET(eObjectType, m_stLimitEnable.m_stBitStr.m_u8TransBits[0],
					ul32OffsetAddr);
				memcpy(&u8BitString, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint8_t));
				#endif
				/* check for Event Notification */
#ifdef FAULT_ALGORITHM
					if(fPresentValue < faultLowLimit && eReliabilty != RELIABILITY_UNDER_RANGE
						#ifdef BACDEL_PR14
						&& (LIMIT_ENABLE_LOW == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
						#endif
						)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_FAULT;
						*peReliabilty = RELIABILITY_UNDER_RANGE;
					}
					else if(fPresentValue < faultLowLimit && eReliabilty == RELIABILITY_UNDER_RANGE
						#ifdef BACDEL_PR14
						&& (LIMIT_ENABLE_LOW == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
						#endif
						)
					{
						*pbEventChange = FALSE;
						*peEventStateTo = EVENT_STATE_FAULT;
						*peReliabilty = RELIABILITY_UNDER_RANGE;
					}

					else if(fPresentValue > faultHighLimit && eReliabilty != RELIABILITY_OVER_RANGE
						#ifdef BACDEL_PR14
						&& (LIMIT_ENABLE_HIGH == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
						#endif
						)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_FAULT;
						*peReliabilty = RELIABILITY_OVER_RANGE;
					}

					else if(fPresentValue > faultHighLimit && eReliabilty == RELIABILITY_OVER_RANGE
						#ifdef BACDEL_PR14
						&& (LIMIT_ENABLE_HIGH == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
						#endif
						)
					{
						*pbEventChange = FALSE;
						*peEventStateTo = EVENT_STATE_FAULT;
						*peReliabilty = RELIABILITY_OVER_RANGE;
					}
#endif /*FAULT_ALGORITHM*/
					/* check for to_offnormal state - low limit */
					else if(fPresentValue < fLowLimit && eEventState != EVENT_STATE_LOW_LIMIT
						#ifdef BACDEL_PR14
						&& (LIMIT_ENABLE_LOW == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
						#endif
						)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_LOW_LIMIT;
						*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					}
					#ifdef BACDEL_PR14
					/* check for high to low, but low is disabeld */
					else if(fPresentValue < fLowLimit && eEventState == EVENT_STATE_HIGH_LIMIT)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_NORMAL;
						*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					}
					#endif
					/* check for to_offnormal state - high limit */
					else if(fPresentValue > fHighLimit && eEventState != EVENT_STATE_HIGH_LIMIT
						#ifdef BACDEL_PR14
						&& (LIMIT_ENABLE_HIGH == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
						#endif
						)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_HIGH_LIMIT;
						*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					}
					#ifdef BACDEL_PR14
					/* check for low to high, but high is disabeld */
					else if(fPresentValue > fHighLimit && eEventState == EVENT_STATE_LOW_LIMIT)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_NORMAL;
						*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					}
					#endif
					/* check for to_normal state */
					else if((fPresentValue > (fLowLimit+fDeadband) && (fPresentValue <= fHighLimit))
							&& (eEventState == EVENT_STATE_LOW_LIMIT || eEventState == EVENT_STATE_FAULT))
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_NORMAL;
						*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					}
					/* check for to_normal state */
					else if((fPresentValue < fHighLimit-fDeadband) && (fPresentValue >= fLowLimit)
							&& (eEventState == EVENT_STATE_HIGH_LIMIT || eEventState == EVENT_STATE_FAULT))
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_NORMAL;
						*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
					}
					else;
			}
			break;
		case OBJECT_ANALOG_OUTPUT:
        case OBJECT_PULSE_CONVERTER:
			{
				/* local variables */
				Float_t		fHighLimit = 0.0,	fLowLimit = 0.0,	fDeadband = 0.0,	fPresentValue = 0.0;
				#ifdef BACDEL_PR14
				uint8_t u8BitString = 0;
				#endif
				fPresentValue = *((Float_t *)pvData);
				puPresentValue->fVal = fPresentValue;

				/* get offset of high limit & save value from that address to corresponding local variable */
				GET_HL_LL_VALUE_OFFSET(eObjectType, m_stHighLimit ,ul32OffsetAddr,i32Len);
				memcpy((void *)&fHighLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* get offset of low limit & save value from that address to corresponding local variable */
				GET_HL_LL_VALUE_OFFSET(eObjectType, m_stLowLimit ,ul32OffsetAddr,i32Len);
				memcpy((void *)&fLowLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* get offset of deadband & save value from that address to corresponding local variable */
				GET_DEADBAND_COV_INCR_VAL_OFFSET(eObjectType,m_stDeadBand,ul32OffsetAddr,i32Len);
				memcpy((void *)&fDeadband, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

				/* verify that high limit & low limit are not same, if they
						are same then dont compare */
				if(fLowLimit == fHighLimit)
				{
					*pbEventChange = FALSE;
					break;
				}
				#ifdef BACDEL_PR14
				/* get limit_enable property offset & copy data into local variable */
				GET_LIMITENABLE_OFFSET(eObjectType, m_stLimitEnable.m_stBitStr.m_u8TransBits[0],
					ul32OffsetAddr);
				memcpy(&u8BitString, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint8_t));
				#endif

				/* check for to_offnormal state - low limit */
				if(fPresentValue < fLowLimit && eEventState != EVENT_STATE_LOW_LIMIT
					#ifdef BACDEL_PR14
					&& (LIMIT_ENABLE_LOW == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
					#endif
					)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_LOW_LIMIT;
					*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
				}
				#ifdef BACDEL_PR14
				/* check for high to low, but low is disabeld */
				else if(fPresentValue < fLowLimit && eEventState == EVENT_STATE_HIGH_LIMIT)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
					*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
				}
				#endif
				/* check for to_offnormal state - high limit */
				else if(fPresentValue > fHighLimit && eEventState != EVENT_STATE_HIGH_LIMIT
					#ifdef BACDEL_PR14
					&& (LIMIT_ENABLE_HIGH == u8BitString || LIMIT_ENABLE_BOTH == u8BitString)
					#endif
					)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_HIGH_LIMIT;
					*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
				}
				#ifdef BACDEL_PR14
				/* check for low to high, but high is disabeld */
				else if(fPresentValue > fHighLimit && eEventState == EVENT_STATE_LOW_LIMIT)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
					*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
				}
				#endif
				/* check for to_normal state */
				else if((fPresentValue > (fLowLimit+fDeadband) && (fPresentValue <= fHighLimit))
						&& (eEventState == EVENT_STATE_LOW_LIMIT || eEventState == EVENT_STATE_FAULT))
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
					*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
				}
				/* check for to_normal state */
				else if((fPresentValue < fHighLimit-fDeadband) && (fPresentValue >= fLowLimit)
						&& (eEventState == EVENT_STATE_HIGH_LIMIT || eEventState == EVENT_STATE_FAULT))
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
					*peReliabilty = RELIABILITY_NO_FAULT_DETECTED;
				}
				else;
			}
			break;
		#endif /* Analog || PC */

		#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BV)
		case OBJECT_BINARY_INPUT:
		case OBJECT_BINARY_VALUE:
			{
				/* local variables */
				BACNET_BINARY_PV	ePresentValue,	eAlarmValue;
				ePresentValue = *((BACNET_BINARY_PV *)pvData);
				puPresentValue->eVal = ePresentValue;

				/* get offset of alarm value & save value from that address to corresponding local variable */
				GET_ALARMVALUE_OFFSET(eObjectType,m_stAlarmValue.m_eVal , ul32OffsetAddr);
				memcpy((void *)&eAlarmValue, (BACNET_BINARY_PV *)(ul32BaseAddr+ul32OffsetAddr), 
					sizeof(BACNET_BINARY_PV));

				/* check for to_offnormal state */
				if(ePresentValue == eAlarmValue && eEventState != EVENT_STATE_OFFNORMAL)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
				/* check for to_normal state */
				else if(ePresentValue != eAlarmValue && eEventState != EVENT_STATE_NORMAL)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else;
			}
			break;
		#endif /* BI || BV */

		#ifdef BACDEL_OBJ_BO
		case OBJECT_BINARY_OUTPUT:
			{
				/* local variables */
				BACNET_BINARY_PV	ePresentValue,	eFeedbackValue;
				ePresentValue = *((BACNET_BINARY_PV *)pvData);
				puPresentValue->eVal = ePresentValue;

				/* get offset of feedback & save value from that address to corresponding local variable */
				GET_FEEDBACKVAL_OFFSET(eObjectType,m_stFeedbackValue , ul32OffsetAddr);
				memcpy((void *)&eFeedbackValue, (BACNET_BINARY_PV *)(ul32BaseAddr+ul32OffsetAddr), 
					sizeof(BACNET_BINARY_PV));

				/* check for to_normal state */
				if(ePresentValue == eFeedbackValue && eEventState != EVENT_STATE_NORMAL)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				/* check for to_offnormal state */
				else if(ePresentValue != eFeedbackValue && eEventState != EVENT_STATE_OFFNORMAL)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
				else;
			}
			break;
		#endif /* BO */

		#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSV)
		case OBJECT_MULTI_STATE_INPUT:
		case OBJECT_MULTI_STATE_VALUE:
			{
				/* local variables */
				int8_t i8Value;
				ListOfUnsigned_t *pstAvaluesIntList = NULL;
				ListOfUnsigned_t *pstFvaluesIntList = NULL;
				puPresentValue->u32Val = *((uint32_t *)pvData);

				#ifdef BACDEL_PR14
				/* check event algorithm inhibit property value for event algorithm */
				if(!bEventAlgoInhibit)
				{
					/* reset error flag */
					bErrorFlag = false;
					/* get offset of alarm values & assign the pointer of list to pstAvaluesIntList */
					GET_ALARM_VALUES_OFFSET(eObjectType, m_stAlarmValues, ul32OffsetAddr);
					pstAvaluesIntList = ((Pr_ListOfUnsigned_t *)(ul32BaseAddr+ul32OffsetAddr))->m_pstUnsignVal;
				}

				/* check event algorithm inhibit property value for fault algorithm */
				if(!bReliaEvaluInhibitTrue)
				{
					/* reset error flag */
					bErrorFlag = false;
					/* get offset of fault values & assign the pointer of list to pstFvaluesIntList */
					GET_FAULT_VALUES_OFFSET(eObjectType, m_stFaultValues, ul32OffsetAddr);
					pstFvaluesIntList = ((Pr_ListOfUnsigned_t *)(ul32BaseAddr+ul32OffsetAddr))->m_pstUnsignVal;
				}
				
				/* check error flag */
				if(bErrorFlag)
				{
					/* event or fault algorithm has been disabled */
					*pbEventChange = FALSE;
					return eFuncRet;
				}
				#else
				/* get offset of alarm values & assign the pointer of list to pstAvaluesIntList */
				GET_ALARM_VALUES_OFFSET(eObjectType,m_stAlarmValues, ul32OffsetAddr);
				pstAvaluesIntList = ((Pr_ListOfUnsigned_t *)(ul32BaseAddr+ul32OffsetAddr))->m_pstUnsignVal;

				/* get offset of fault values & assign the pointer of list to pstFvaluesIntList */
				GET_FAULT_VALUES_OFFSET(eObjectType,m_stFaultValues, ul32OffsetAddr);
                pstFvaluesIntList = ((Pr_ListOfUnsigned_t *)(ul32BaseAddr+ul32OffsetAddr))->m_pstUnsignVal;
				#endif /* PR14 */

				/* scan through the alarm & fault values list */
				i8Value = Check_AFvalues_List
					(pstAvaluesIntList, pstFvaluesIntList, puPresentValue, eObjectType, pvObject,
					 &bPrevAlarmValMatch);

				if(NO_MATCH_FOUND == i8Value && !bPrevAlarmValMatch)
				{
					/* no match found */
					if(eEventState != EVENT_STATE_NORMAL)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_NORMAL;
					}
				}
				else if(MATCH_FOUND_IN_ALARM_VALUES == i8Value)
				{
					/* match found in alarm values */
					/* refer clause 13.3.2 in 2012 manual */
					if(EVENT_STATE_NORMAL == eEventState || 
					  (EVENT_STATE_FAULT == eEventState &&  RELIABILITY_MULTI_STATE_FAULT == eReliabilty))
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_OFFNORMAL;
					}
				}
				else if(MATCH_FOUND_IN_FAULT_VALUES == i8Value)
				{
					/* match found in fault values */
					if(eEventState != EVENT_STATE_FAULT)
					{
						*pbEventChange = TRUE;
						*peEventStateTo = EVENT_STATE_FAULT;
					}
				}
			}
			break;
		#endif /* MSI || MSV */

		#ifdef BACDEL_OBJ_MSO
		case OBJECT_MULTI_STATE_OUTPUT:
			{
				/* local variables */
				uint32_t	u32PresentValue = 0,	u32FeedbackValue = 0;
				u32PresentValue = *((uint32_t *)pvData);
				puPresentValue->u32Val = u32PresentValue;

                /* get feedback value offset & save value */
				GET_FEEDBACKVAL_OFFSET(eObjectType,m_stFeedbackValue , ul32OffsetAddr);
				memcpy((void *)&u32FeedbackValue, (uint32_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint32_t));
				
				/* check for to_normal state */
				if(u32PresentValue == u32FeedbackValue && eEventState != EVENT_STATE_NORMAL)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				/* check for to_offnormal state */
				else if(u32PresentValue != u32FeedbackValue && eEventState != EVENT_STATE_OFFNORMAL)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
				else;
			}
			break;
		#endif /* MSO */

		default:
			/* default case debug message */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU:UpdateEventState : \
										Invalid Object Type \r\n");
			#endif

			eFuncRet = BACDEL_ERROR;
			break;
	}

	/* print function exit debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: UpdateEventState : Exit \r\n"); 
	#endif

	return eFuncRet;
}

/** returns true if the monitored / reference property for intrinsic 
    alarms is PV 
    returns false if prop is not PV or intrinsic alarms not supported */
bool Is_Reference_Prop_PV(BACNET_OBJECT_TYPE eObjectType)
{
	/* return value */
	bool bRetVal = FALSE;

    /* NOTE : 
        1.  present value for all primitive object types 
            eg. analog, binary, multistate, etc.
        2.  total record count value for trend log object.
        3.  controlled varaible value for loop object.
        4.  pulse rate value for accumulator object type.
        5.  return false for following objects:
                calendar, schedule
    */

    switch(eObjectType)
    {
        /* these objects support intrinsic alarms & monitored property is PV */
    case OBJECT_ANALOG_INPUT:
    case OBJECT_ANALOG_OUTPUT:
    case OBJECT_ANALOG_VALUE:
    case OBJECT_BINARY_INPUT:
    case OBJECT_BINARY_OUTPUT:
    case OBJECT_BINARY_VALUE:
    case OBJECT_MULTI_STATE_INPUT:
    case OBJECT_MULTI_STATE_VALUE:
    case OBJECT_MULTI_STATE_OUTPUT:
        /* reference property is PV */
        bRetVal = TRUE;
        break;

        /* by default return false */
    default:
        break;
    }

    /* return the value */
	return bRetVal;
}

/** save the reference property value that is monitored to generate
    intrinsic alarms. 
    it is mostly present value for most object type, except few */
void Get_Reference_Prop_Value(BACNET_OBJECT_TYPE eObjectType,
					   PropertyValue_u *puPropValue,
					   void *pvObject)
{
	/* base address of initiating object */
	ulong32_t ul32BaseAddr = 0;
	/* offset address for property values */
	ulong32_t ul32OffsetAddr = 0;
    /* hold Sizeof */
    int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: Get_Reference_Prop_Value: entry \r\n");
	#endif

    /* check input pointers */
    if(NULL == pvObject || NULL == puPropValue)
        return;

	/* Get the object base address */
    ul32BaseAddr = (ulong32_t )pvObject;

    /* NOTE : 
    1.  returns present value for of the object type eg. analog, binary, 
        multistate, etc.
    2.  returns total record count value for TL/TLM/EL objects as it doesn't 
        have any present value & as total record count value is sent in 
        notifications.
    3.  returns controlled varaible value for loop object as it is monitored
        for intrinsic reporting in loop object.
    4.  returns pulse rate value for accumulator object type.
    */

    /* get offset address of reference property value */
    GET_REF_PROP_VAL_OFFSET(eObjectType, ul32OffsetAddr, i32Len);

    /* copy the value */
    memcpy(puPropValue, (void*)(ul32BaseAddr+ul32OffsetAddr), i32Len);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: Get_Reference_Prop_Value: exit \r\n");
	#endif

	return;
}

/**
*	DESCRIPTION
*   Function decides which notification is to be generated when reliablity 
*   becomes to no fault detected.
*
*	@param		[in]		eObjectType - type of object.
*	@param		[in]		pvObject - base address of object structure.
*	@param		[in]		pvData - not used in api.
*	@param		[out]		pbEventChange - tells if event state needs to be updated or not.
*	@param		[out]		peEventStateTo -new event state.
*	@param		[out]		peEventStateFrom - previous event state.
*	@param		[out]		puPresentValue - returns present value.
*	@param		[in]		pVirtualDev - virtual device pointer.
*
*	@return		BACDEL_SUCCESS / BACDEL_ERROR
**/
BACNET_RETURN_TYPE UpdateEventState_On_Reliablity(BACNET_OBJECT_TYPE eObjectType,
									void *pvObject,
									void *pvData,
									bool *pbEventChange,
									BACNET_EVENT_STATE	*peEventStateTo,
									BACNET_EVENT_STATE	*peEventStateFrom,
									PropertyValue_u *puPresentValue,
									virtualDevData_t *pVirtualDev,
									uint16_t *pu16ExtFlag,
									uint32_t *pu32ExtTimeDelay)
{
	/* default return type */
	BACNET_RETURN_TYPE  eFuncRet = BACDEL_SUCCESS;
	/* variable to store property offset value */
	ulong32_t ul32OffsetAddr = 0;
	/* variable to store the base address of object */
	ulong32_t ul32BaseAddr	= 0;
	/* variable to store value of EVENT_STATE property */
	BACNET_EVENT_STATE	eEventState;
	/* to save reliablity */
	BACNET_RELIABILITY  eReliabilty = RELIABILITY_NO_FAULT_DETECTED;
    /* hold sizeof */
    int32_t i32Len = 0;
	/* alarm value match flag */
	bool bPrevAlarmValMatch = false;
	#ifdef BACDEL_PR14
	/* variable to store event detection enable value */
	bool bEventDetectionEnable = false;
	#if 0
	/* variable to store value of reliability evaluation inhibit property */
	bool bReliabilityEvaluInhibit = false;
	#endif
	/* variable to store value of event algorithm inhibit property */
	bool bEventAlgoInhibit = false;
	/* variable to store value of reliability evaluation inhibit property for backup */
	bool bRelEvalInhibitBackup = false;
	#endif /* PR14 */

	/* print function entry debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	UpdateEventState_On_Reliablity : Entry \r\n"); 
	#endif

    /* check input pointers */
    if(NULL == pvObject || NULL == puPresentValue || NULL == pVirtualDev)
        return BACDEL_ERROR;

	/* get the base address of object */
    ul32BaseAddr = (ulong32_t)pvObject;

	#ifdef BACDEL_PR14
	/* read event detection enable prop value */
	GET_INTRINSIC_OFFSET(eObjectType, m_stEventDetectionEnable.m_bVal, ul32OffsetAddr); 
	memcpy(&bEventDetectionEnable, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(bool));
	/* check event detection enable is true or false */
	if(!bEventDetectionEnable)
	{
		/* intrinsic reporting is disabled in this object */
		*pbEventChange = FALSE;
		return eFuncRet;
	}

	#endif /* PR14 */

	/* set default to false */
    *pbEventChange = FALSE;

	/* get offset of event state & copy data into eEventStates from property offset address */
	GET_EVENTSTATE_OFFSET(eObjectType, m_stEventState.m_eEventState, ul32OffsetAddr);
	memcpy((void *)&eEventState, (void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(BACNET_EVENT_STATE));
	*peEventStateFrom = eEventState;
	/* if event state is other than fault, do not execute this function */
	if(EVENT_STATE_FAULT != eEventState)
	{
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
		UpdateEventState_On_Reliablity: invalid event state. \r\n"); 
		#endif
		return BACDEL_ERROR;
	}

	/* get the present value */
	Get_Reference_Prop_Value(eObjectType, puPresentValue, pvObject);

	/* switch to object type */
	switch(eObjectType)
	{
		#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV)
		case OBJECT_ANALOG_INPUT:
		case OBJECT_ANALOG_OUTPUT:
		case OBJECT_ANALOG_VALUE:
		{
			/* local variables */
			Float_t	fHighLimit = 0.0, fLowLimit = 0.0, fDeadband = 0.0, fPresentValue = 0.0;
			#ifdef BACDEL_PR14
			uint8_t u8BitString = 0;
			#endif
			fPresentValue = puPresentValue->fVal;

			/* get offset of high limit & save value from that address to corresponding local variable */
			GET_HL_LL_VALUE_OFFSET(eObjectType, m_stHighLimit ,ul32OffsetAddr,i32Len);
			memcpy((void *)&fHighLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

			/* get offset of low limit & save value from that address to corresponding local variable */
			GET_HL_LL_VALUE_OFFSET(eObjectType, m_stLowLimit ,ul32OffsetAddr,i32Len);
			memcpy((void *)&fLowLimit, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

			/* get offset of deadband & save value from that address to corresponding local variable */
			GET_DEADBAND_COV_INCR_VAL_OFFSET(eObjectType,m_stDeadBand ,ul32OffsetAddr, i32Len);
			memcpy((void *)&fDeadband, (Float_t *)(ul32BaseAddr+ul32OffsetAddr), i32Len);

			/* verify that high limit & low limit are not same, if they
					are same then dont compare */
			if(fLowLimit == fHighLimit)
			{
				*pbEventChange = FALSE;
				 break;
			}

			#ifdef BACDEL_PR14
			/* get limit_enable property offset & copy data into local variable */
			GET_LIMITENABLE_OFFSET(eObjectType, m_stLimitEnable.m_stBitStr.m_u8TransBits[0], 
				ul32OffsetAddr);
			memcpy(&u8BitString, (uint8_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint8_t)); 
			#endif

			/* check for to_offnormal state - low limit */
			if(fPresentValue < fLowLimit /*&& eEventState != EVENT_STATE_LOW_LIMIT*/)
			{
				#ifdef BACDEL_PR14
				/* check if event algorithm is disabled or low limit flag is disabled */
				if(bEventAlgoInhibit || (LIMIT_ENABLE_NONE == u8BitString || LIMIT_ENABLE_HIGH == u8BitString))
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_LOW_LIMIT;
				}
			}	
			/* check for to_offnormal state - high limit */
			else if(fPresentValue > fHighLimit/* && eEventState != EVENT_STATE_HIGH_LIMIT*/)
			{
				#ifdef BACDEL_PR14
				/* check if event algorithm is disabled or high limit flag is disabled */
				if(bEventAlgoInhibit || (LIMIT_ENABLE_NONE == u8BitString || LIMIT_ENABLE_LOW == u8BitString))
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_HIGH_LIMIT;
				}
			}	
			/* check for to_normal state */
			else if((fPresentValue <= fHighLimit) && (fPresentValue > (fLowLimit+fDeadband)) 
					/*&& eEventState != EVENT_STATE_NORMAL*/)
			{
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
			/* check for to_normal state */
			else if((fPresentValue < fHighLimit-fDeadband) && (fPresentValue >= fLowLimit) 
					/*&& eEventState != EVENT_STATE_NORMAL*/)
			{
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
			else;
		}break;
		#endif /* Analog || PC */

		#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BV)
		case OBJECT_BINARY_INPUT:
		case OBJECT_BINARY_VALUE:
		{
			/* local variables */
			BACNET_BINARY_PV ePresentValue, eAlarmValue;
			ePresentValue = puPresentValue->eVal;

			/* get offset of alarm value & save value from that address to corresponding local variable */
			GET_ALARMVALUE_OFFSET(eObjectType,m_stAlarmValue.m_eVal , ul32OffsetAddr);
			memcpy((void *)&eAlarmValue, (BACNET_BINARY_PV *)(ul32BaseAddr+ul32OffsetAddr), 
				sizeof(BACNET_BINARY_PV));

			/* check for to_offnormal state */
			if(ePresentValue == eAlarmValue /*&& eEventState != EVENT_STATE_OFFNORMAL*/)
			{
				#ifdef BACDEL_PR14
				/* check if event algorithm is disabled */
				if(bEventAlgoInhibit)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
			}
			/* check for to_normal state */
			else if(ePresentValue != eAlarmValue /*&& eEventState != EVENT_STATE_NORMAL*/)
			{
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
			else;
		}break;
		#endif /* BI || BV */

		#ifdef BACDEL_OBJ_BO
		case OBJECT_BINARY_OUTPUT:
		{
			/* local variables */
			BACNET_BINARY_PV	ePresentValue,	eFeedbackValue;
			ePresentValue = puPresentValue->eVal;

			/* get offset of feedback & save value from that address to corresponding local variable */
			GET_FEEDBACKVAL_OFFSET(eObjectType,m_stFeedbackValue , ul32OffsetAddr);
			memcpy((void *)&eFeedbackValue, (BACNET_BINARY_PV *)(ul32BaseAddr+ul32OffsetAddr), 
				sizeof(BACNET_BINARY_PV));

			/* check for to_normal state */
			if(ePresentValue == eFeedbackValue /*&& eEventState != EVENT_STATE_NORMAL*/)
			{
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
			/* check for to_offnormal state */
			else if(ePresentValue != eFeedbackValue /*&& eEventState != EVENT_STATE_OFFNORMAL*/)
			{
				#ifdef BACDEL_PR14
				/* check if event algorithm is disabled */
				if(bEventAlgoInhibit)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
			}
			else;
		}break;
		#endif /* BO */

		#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSV)
		case OBJECT_MULTI_STATE_INPUT:
		case OBJECT_MULTI_STATE_VALUE:
		{
			/* local variables */
			int8_t i8Value = 0;
			ListOfUnsigned_t *pstAvaluesIntList = NULL;
			ListOfUnsigned_t *pstFvaluesIntList = NULL;
			void *pvDummy = NULL;
			uint32_t u32PropertyAdd = 0;

			/* get offset of alarm values & assign the pointer of list to pstAvaluesIntList */
			GET_ALARM_VALUES_OFFSET(eObjectType,m_stAlarmValues, ul32OffsetAddr);
			pstAvaluesIntList = ((Pr_ListOfUnsigned_t *)(ul32BaseAddr+ul32OffsetAddr))->m_pstUnsignVal;

			/* get offset of fault values & assign the pointer of list to pstFvaluesIntList */
			GET_FAULT_VALUES_OFFSET(eObjectType,m_stFaultValues, ul32OffsetAddr);
            pstFvaluesIntList = ((Pr_ListOfUnsigned_t *)(ul32BaseAddr+ul32OffsetAddr))->m_pstUnsignVal;

			/* scan through the alarm & fault values list */
			i8Value = Check_AFvalues_List
				(pstAvaluesIntList, pstFvaluesIntList, puPresentValue, eObjectType, pvObject,
				 &bPrevAlarmValMatch);

			if(NO_MATCH_FOUND == i8Value)
			{
				/* no match found */
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
			else if(MATCH_FOUND_IN_ALARM_VALUES == i8Value)
			{
				/* match found in alarm values */
				#ifdef BACDEL_PR14
				/* check if event algorithm is disabled */
				if(bEventAlgoInhibit)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
			}
			else if(MATCH_FOUND_IN_FAULT_VALUES == i8Value)
			{
				#ifdef BACDEL_PR14
				/* when reliability-evaluation-inhibit set true i.e. disabled,
				reliability-evaluation-inhibit-backup value is set to false, 
				so send ToNormal notification even if match is found in fault value */
				if(!bRelEvalInhibitBackup)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif /* PR14 */
				{
					/* match found in fault values */
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_FAULT;
					eReliabilty = RELIABILITY_MULTI_STATE_FAULT;
					/* special case, since match found in fault values,
						reliability again changes to multistate_fault */
					/* call UpdateCommonProperty function to change event_state */
					UpdateCommonProperty(eObjectType, pvObject, PROP_RELIABILITY,
						-1, (void *)&eReliabilty,
						READ_ONLY,
						&u32PropertyAdd, &pvDummy, pVirtualDev,
						false, false, false);
				}
			}
		}break;
		#endif /* MSI || MSV */

		#ifdef BACDEL_OBJ_MSO
		case OBJECT_MULTI_STATE_OUTPUT:
		{
			/* local variables */
			uint32_t	u32PresentValue = 0,	u32FeedbackValue = 0;
			u32PresentValue = puPresentValue->u32Val;

			GET_FEEDBACKVAL_OFFSET(eObjectType,m_stFeedbackValue , ul32OffsetAddr);
			memcpy((void *)&u32FeedbackValue, (uint32_t *)(ul32BaseAddr+ul32OffsetAddr), sizeof(uint32_t));
			
			/* check for to_normal state */
			if(u32PresentValue == u32FeedbackValue /*&& eEventState != EVENT_STATE_NORMAL*/)
			{
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
			/* check for to_offnormal state */
			else if(u32PresentValue != u32FeedbackValue /*&& eEventState != EVENT_STATE_OFFNORMAL*/)
			{
				#ifdef BACDEL_PR14
				/* check if event algorithm is disabled */
				if(bEventAlgoInhibit)
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_NORMAL;
				}
				else
				#endif
				{
					*pbEventChange = TRUE;
					*peEventStateTo = EVENT_STATE_OFFNORMAL;
				}
			}
			else;
		}break;
		#endif /* MSO */

		#if ((defined BACDEL_OBJ_SDL || defined BACDEL_OBJ_EE) && defined BACDEL_PR14) 
		case OBJECT_EVENT_ENROLLMENT:
		case OBJECT_SCHEDULE:
		{
			if(EVENT_STATE_FAULT == eEventState)
			{
				*pbEventChange = TRUE;
				*peEventStateTo = EVENT_STATE_NORMAL;
			}
		}
        break;
        #endif /* (SDL || EE) && PR14 */
		default:
			/* default case debug message */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
			UpdateEventState_On_Reliablity: Invalid Object Type \r\n");
			#endif

			eFuncRet = BACDEL_ERROR;
			break;
	}//end of switch

	/* print function exit debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	UpdateEventState_On_Reliablity : Exit \r\n"); 
	#endif

	return eFuncRet;
}

/***
* 
* DESCRIPTION
*   Api check if intrinsic notification is to be added in intrinsic list for 
*   monitoring whenever any reference property required for intrinsic alarms
*   changes.
*
**/
void Check_For_Intrinsic_Notification(BACNET_OBJECT_TYPE eObjectType, 
					void *pvObject, virtualDevData_t *pVirtualDev)
{
	/* local variables */
	BACNET_EVENT_STATE	eEventStateTo = 0;
	BACNET_EVENT_STATE	eEventStateFrom = 0;
	BACNET_RELIABILITY    eReliabilty= 0;
	bool				bEventChange = false;
	uint32_t			u32TimeDelay = 0;
	uint32_t			u32TimeDelayNormal = 0;
	PropertyValue_u		uPropertyValue = {0};
	PropertyValue_u		uPresentVal = {0};
	BACNET_RETURN_TYPE  eFuncRet = BACDEL_SUCCESS;
    BACNET_PROPERTY_ID ePropId = PROP_PRESENT_VALUE; /* default property for alarms */

	/* print function entry debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Check_For_Intrinsic_Notification : Entry \r\n"); 
	#endif

	if(NULL == pvObject || NULL == pVirtualDev)
		return;

	/* get the present value */
	Get_Reference_Prop_Value(eObjectType, &uPresentVal, pvObject);

	/* call function to check if event_state needs to be updated */
    eFuncRet = UpdateEventState(eObjectType, pvObject, &uPresentVal, 
							    &bEventChange, &eEventStateTo, &eEventStateFrom,
							    &eReliabilty,&uPropertyValue);

    if(TRUE == bEventChange && BACDEL_SUCCESS == eFuncRet)
    {
	    /* get time delay value */
	    Get_TimeDelay_value(eObjectType, pvObject, eEventStateTo, &u32TimeDelay);

        /* check the reference property */
        /* NOTE: intrinsic timer is ir-respective of this property for now */
	    /* add the data to event_state linklist for monitoring */
		Create_EventState_LinkList(ePropId, eObjectType, pvObject, eEventStateTo, 
            eEventStateFrom,eReliabilty, &uPropertyValue, u32TimeDelay, u32TimeDelayNormal,
			pVirtualDev);
    } 

	/* print function exit  debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Check_For_Intrinsic_Notification : exit \r\n"); 
	#endif
}

/**	api to clear all pending intrinsic notification belonging to any device,
    whenever that device is deleted. */
void Clear_Intrinsic_Notifications(uint32_t u32DevId)
{
    /* local variables */
	LinkList_t *pstLocalNode = NULL;
    LinkList_t *pstPrevNode = NULL;
    LinkList_t *pstNextNode = NULL;
    bool bNodeDeleted = FALSE;

    /* function entry debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
    Clear_Intrinsic_Notifications: entry \r\n");
	#endif

    /* Wait till the mutex handle is released */
    Osal_Wait_Mutex(g_stIntrinsicReporting.hMtxLock, INFINITE); 

    /* update the current node address */
	pstLocalNode = g_stIntrinsicReporting.pstFirstNode;

    while(NULL != pstLocalNode)
    {
        /* save next node */
	    pstNextNode = pstLocalNode->m_pstNext;
        /* check if node is to be deleted */
        if(u32DevId == pstLocalNode->m_stListData.m_u32DevId)
        {
            if(NULL == pstPrevNode && NULL == pstNextNode)
            {
	            /* this is the only node in linklist */
	            g_stIntrinsicReporting.pstFirstNode = NULL;
	            g_stIntrinsicReporting.pstLastNode = NULL;
            }
            else if(NULL == pstPrevNode)
            {
	            /* this is 1st node of list */
	            g_stIntrinsicReporting.pstFirstNode = pstNextNode;
            }
            else if(NULL == pstNextNode)
            {
	            /* this is last node of list */
	            pstPrevNode->m_pstNext = NULL;
	            g_stIntrinsicReporting.pstLastNode = pstPrevNode;
            }
            else
            {
	            /* middle node */
	            pstPrevNode->m_pstNext = pstNextNode;
            }
            /* free the node */
            OSAL_Free(pstLocalNode, __FILE__, __FUNCTION__, __LINE__);
            bNodeDeleted = TRUE;
        }
        if(bNodeDeleted)
            bNodeDeleted = FALSE;
        else
            /* update previous node */
	        pstPrevNode = pstLocalNode;
	    /* move to next node */
	    pstLocalNode = pstNextNode;
    }//while ends.

    /* release the mutex */
	Osal_Release_Mutex(g_stIntrinsicReporting.hMtxLock);

    /* function exit debug message */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
    Clear_Intrinsic_Notifications: exit \r\n");
	#endif

	return;
}

#endif /* (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC) */
