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
*   SoftDEL Systems Ltd.                                 india@softdel.com
*   3rd Floor, Pentagon P4,                              http://www.softdel.com
*    Magarpatta City, Hadapsar
*    Pune - 411 028       
*
*    File Name - objectGenricHandler.c
*
*   RELEASE HISTORY                                                             
*        DATE                  NAME                    DESCRIPTION
*   28/07/2011          Prashant Badgujar             File Created
*   08/08/2011          Prashant Badgujar             Shifting macro definations to 
*                                                     propertyGenricHandler.h file
*	29/08/2011			Prashant Badgujar			  Adding Remove_Object API.
*	13/09/2011			Prashant Badgujar			  Updating Update_Device_Object_List for 
*													  object list changes.
*	19/09/2011			Prashant Badgujar			  Adding Database revision support.
*	20/09/2011			Prashant Badgujar			  Updating Object Type field with desired
*													  object type.
*	22/09/2011			Prashant Badgujar			  Adding Init_Required_Properties,
*													  Init_Priority_Array, 
*                                                     Init_Relinquish_Default,
*													  Init_Present_Value, 
*                                                     Init_Object_All_Properties
*   27/09/2011          Prashant Badgujar             Modified Init_Required_Properties
*                                                     API. Used Convert_InStr_AddProperty 
*                                                     API to update object properties 
*                                                     value 
*   10/04/2023          Heramb Joshi                  Code changes : New API added
*                                                       Init_Object_All_Properties
*                                                       InitObjectProperty
*
******************************************************************************/

//#include "osalDef.h" 
#include "osalFreeRTOS.h"
#include "bacDELDeviceConfig.h"
#include "propertyGenricHandler.h"
#include "objDevice.h"
#include "propertyValueRead.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include "propertyValidations.h"
#include "propertyClearValues.h"

/**
*                                                                         
* DESCRIPTION                                                                          
* API to increment database revision of device object as per clause 12.11.35
*    
* @param pVirtualDev [in] Virtual Device.
* @param eOjectType [in] Type of object.
* @param ui32ObjId [in] Object Id.
*
* @returns void.
*
**/
static void Update_Database_Revision(virtualDevData_t *pVirtualDev,
                        BACNET_OBJECT_TYPE eOjectType, 
                        uint32_t u32ObjId)
{
    /* local varaibles */
    bool bUpdateDR = TRUE;

    /* function entry debug message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_Database_Revision : Entry \r\n");
	#endif

    if(NULL == pVirtualDev)
        return;

    /* switch to device backup state */
	#if (defined BACDEL_SER_DM_BR_B && defined BACDEL_OBJ_FILE)
    switch(pVirtualDev->m_stDevObject.m_stBackupRestoreState.m_eBackupState)
    {
    case BACKUP_STATE_PERFORMING_A_BACKUP:
    case BACKUP_STATE_PERFORMING_A_RESTORE:
    case BACKUP_STATE_PREPARING_FOR_BACKUP:
    case BACKUP_STATE_PREPARING_FOR_RESTORE:
        /* check if configuration file */
        bUpdateDR = Is_Configuration_File(pVirtualDev, u32ObjId, eOjectType);
        /* if yes, then do not update database revision */
        bUpdateDR = !bUpdateDR;
        break;

        /* default - update database revision */
    default:
        break;
    }//switch ends.
    #endif /* BACDEL_SER_DM_BR_B */

    if(bUpdateDR)
    {
        /* update database revision property value by 1 */
        Update_Database_Revision_Value(pVirtualDev);

        /* debug message */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
        Update_Database_Revision : database revision updated. \r\n");
		#endif
    }

    /* function exit debug message */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_Database_Revision : Exit \r\n");
	#endif
}

/**
*                                                                         
*DESCRIPTION                                                                          
*        This API adds object of type particular type to update the device object list.
*    
*@param i8DevId [in] The index in Virtual Device Table
*@param pvObjData [in] The Object property fields/data.
*@param eOjectType [in] Object type to be updated in device object list
*
*@return None
*
*/
void Update_Device_Object_List(virtualDevData_t *pVirtualDev,
                               BACNET_OBJECT_TYPE eOjectType,
                               uint32_t  ui32ObjId)
{
    /** Temp. Object pointer*/
    void*   pvNewObject = NULL;
    /** To traverse object list */
    ListOfObjId_t *pstObjId = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Update_Device_Object_List : Entry \r\n");
	#endif

    /* Make a new entry for this object in device object list */
    pvNewObject = OSAL_Malloc(sizeof(ListOfObjId_t),  __FILE__, __FUNCTION__, __LINE__); 
    /* Check allocated heap memory */
    if(NULL == pvNewObject || NULL == pVirtualDev)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:Update_Device_Object_List : \
									NUll pointers - malloc failed \r\n");
		#endif
		if(NULL != pvNewObject)
		{
			/* free the pointer */
			OSAL_Free(pvNewObject, __FILE__,__FUNCTION__,__LINE__);
		}
        return ;        
    }
	/* initialize new memory */
	//memset(pvNewObject, 0, sizeof(ListOfObjId_t));

    /* Keep track of start of Device Object List, irrespective of number of memebers.
    */
    pstObjId = pVirtualDev->m_stDevObject.m_stObjectIDList.
        m_pstArrayObjId;
    if(pstObjId != NULL)
    {            
        /* Update the Device Object List */
        while(pstObjId->m_pstNext != NULL)
        {
            pstObjId =pstObjId->m_pstNext;
        }
    }

	if(pstObjId != NULL)
    {
		/* Put the new memeber of objectlist at end */
		pstObjId->m_pstNext = (ListOfObjId_t *)pvNewObject;    

		/* Move to newely added element */
		pstObjId = pstObjId->m_pstNext;

		 /* Increment the object list counter */ 
		pVirtualDev->m_stDevObject.m_stObjectIDList.m_u32ObjCount++;

		/* Mark pstNext element as NULL */
		pstObjId->m_pstNext = NULL;

		pstObjId->m_stObjectId.m_u32ObjId = ui32ObjId, 
		pstObjId->m_stObjectId.m_eObjectType = eOjectType;
	}
	else
	{
		/* free the allocated memory */
		OSAL_Free(pvNewObject, __FILE__,__FUNCTION__,__LINE__);
	}

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Update_Device_Object_List : Exit \r\n");
	#endif
}

/***
*                                                                    
*DESCRIPTION                                                                          
*   This function removes the entry of deleted object from device object list.
*    
*@param pVirtualDev	[in]    virtual device data.
*@param eOjectType	[in]	type of object.
*@param	ui32ObjId	[in]	object id.
*                                   
*@return none
*	
***/
static void Modify_Device_ObjectList(BACNET_OBJECT_TYPE eOjectType,
				uint32_t ui32ObjId, virtualDevData_t *pVirtualDev)
{
    /* local varaibles */
	ListOfObjId_t *pstArrObjId = NULL;
	void *pvPrev = NULL;
	void **pvStart = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Modify_Device_ObjectList: entry \r\n");
	#endif

	/* get address of 1st object in object list */
	pstArrObjId = pVirtualDev->m_stDevObject.m_stObjectIDList.m_pstArrayObjId;
	pvStart = (void **)&pVirtualDev->m_stDevObject.m_stObjectIDList.m_pstArrayObjId;
	while(NULL != pstArrObjId)
	{
		if(eOjectType == pstArrObjId->m_stObjectId.m_eObjectType
			&&
			ui32ObjId == pstArrObjId->m_stObjectId.m_u32ObjId)
		{
			/* match found, remove this object from object list */
			if(NULL == pvPrev)
			{
				/* 1st member of list */
				/* update base pointer of list */
				*pvStart = pstArrObjId->m_pstNext;
			}
			else
			{
				/* maintain the linklist */
				((ListOfObjId_t *)pvPrev)->m_pstNext = pstArrObjId->m_pstNext;
			}
			/* free the pointer */
			OSAL_Free(pstArrObjId, __FILE__,__FUNCTION__,__LINE__);
			pstArrObjId = NULL;
			break;
		}
		else
		{
			/* maintain previous pointer */
			pvPrev = pstArrObjId;
			/* move to next object in list */
			pstArrObjId = pstArrObjId->m_pstNext;
		}
	}

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Modify_Device_ObjectList: exit \r\n");
	#endif
}

/**
*DESCRIPTION
*   This API is called be Create_Object API to insert new object in sorted Object List
*
*@param eObjectType [in] Object Type
*@param u32InObjID [in] Object Instance ID
*@param pvObject [in/Out] Pointer to start of object list of given object type.
*@param pvNewObject [in] New object to add in the object list
*
*@return void * [out] Pointer to newly added object
*
*/
void* Sort_Object_List(BACNET_OBJECT_TYPE eObjectType,
                         uint32_t  u32ObjId,
                         void   **pvObject,
                         void *pvNewObject)
{
    ulong32_t ul32BaseAddr = 0;     /* For Base struct Addr */
    ulong32_t ul32OffsetAddr = 0 ;  /* For Data offset Addr */
    ulong32_t   ul32ObjectOffset = 0;
    void   *pvPrevNode = NULL;      /* Pointer to Previous Link list node */
    void   **pvNextNode = NULL;      /* Pointer to Next Link list node */
    void    *pvPrevEle = NULL;      /* Previous Node object value */
    void    *pvNextEle = NULL;      /* Next Node object value */
    void    *pvTmpEle = NULL;       /* Temporary Pointer to hold the address */
    uint32_t  u32PrevObjId = 0;     /* Last object id */
    uint32_t  u32NextObjId = 0;     /* Next Object id */
    void    **pvNext = NULL;        /* Pointer of next element of node */
    void    **pvNewNext = NULL;     /* Pointer of next element of node */
    bool    bEndList = FALSE;       /* Flag variable to mark end of link list */
    void    *pvDuplicateRet = (void *)0x0001;    /* Return for Duplicate object */

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Sort_Object_List : Entry \r\n");
	#endif

    /* Keep track of Previous Link List Node. */
    pvPrevNode = *pvObject;
    ul32BaseAddr = (ulong32_t)pvPrevNode;

    /* Now get the Next Link List Node */
	GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32ObjectOffset);
    pvNextNode = (void *)(ul32BaseAddr+ul32ObjectOffset);
    
    /* This is the case when there is only single object in the list */
    if(NULL == *pvNextNode)
    {
        /* Compare the object identifier of new element with the previous 
        element */
    	ul32ObjectOffset = (ulong32_t)pvPrevNode;
        /* Get offset of object id for validation */
        GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32ObjectOffset);
        pvTmpEle = (void *)(ul32BaseAddr+ul32ObjectOffset);
        memcpy(&u32PrevObjId, pvTmpEle, sizeof(uint32_t));

        if( u32PrevObjId == u32ObjId)
        {
            /* As this object is duplicate object, return pointer to object 
            structure. Some application of this API needs current object
            pointer */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Sort_Object_List :"
                " Duplicate Object\r\n");
			#endif
            return pvDuplicateRet;
        }
        else /* Sort the element if both object ids are not in order */
        if(u32PrevObjId > u32ObjId)
        {
            ul32BaseAddr = (ulong32_t)pvNewObject;
            /* Now get the next pointer of this Node.*/
			GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32ObjectOffset);
            pvNextNode = (void *)(ul32BaseAddr+ul32ObjectOffset);

            /* Insert the new node after swapping the two link list Nodes */
            *pvNextNode = pvPrevNode;

            /* Update the device object list with this New Node pointer */
            *pvObject = pvNewObject;
        }
        else    
        {
            /* Simple add this new element in ths list */
            *pvNextNode = pvNewObject;
            /* Return the pointer of newly added link list Node */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Sort_Object_List :"
                " New Object Added to Link List\r\n");
			#endif
            return *pvNextNode;
        }
    }
    else
    {
        /* Traverse the link list till end of list */
        while(NULL != *pvNextNode)
        {
            /* Take Previous Node Object Id */
            ul32BaseAddr = (ulong32_t)pvPrevNode;
            GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32ObjectOffset);
            pvPrevEle = (void *)(ul32BaseAddr+ul32ObjectOffset);
            memcpy(&u32PrevObjId, pvPrevEle, sizeof(uint32_t));
            
            /* Take Next Node Object id */
            ul32BaseAddr = (ulong32_t)*pvNextNode;
            GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32ObjectOffset);
            pvNextEle = (void *)(ul32BaseAddr+ul32ObjectOffset);
            memcpy(&u32NextObjId, pvNextEle, sizeof(uint32_t));

            if( (u32PrevObjId == u32ObjId) || (u32NextObjId == u32ObjId) )
            {
                /* As this object is duplicate object, return pointer to 
                object structure. Some application of this API needs current 
                object pointer */
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Sort_Object_List :"
                    " Duplicate Object Found\r\n");
				#endif

                return pvDuplicateRet;
            }
            /* If the Prevoius Node has greate object id than the new node */
            else if(u32PrevObjId > u32ObjId)
            {
                ul32BaseAddr = (ulong32_t)pvNewObject;
                /* Now get the next node pointer */
				GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32ObjectOffset);
                pvNextNode = (void *)(ul32BaseAddr+ul32ObjectOffset);
                /* Inset this New node before Previous Node */
                *pvNextNode = pvPrevNode;

                /* Update the device object list with this new first element 
                pointer */
                *pvObject = pvNewObject;

				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Sort_Object_List :"
                    " Object Placed at start of link list\r\n");
				#endif

                /* Return the pointer of newly added node */
                return *pvObject;
            }
            /* Compare the New Node object id with the Previous & Next Node */
            if( (u32PrevObjId < u32ObjId) && (u32NextObjId > u32ObjId))
            {
                /* Take the Previous Node Next Pointer */
                ul32BaseAddr = (ulong32_t)pvPrevNode;                       
				GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32ObjectOffset);
				pvNext = (void *)(ul32BaseAddr+ul32ObjectOffset);
                /* Previous Node next pointer will always point to Next node entry,
                so update Next node pointer with this */
                *pvNextNode = *pvNext;

                /* Take Next Pointer of New Node */
                ul32BaseAddr = (ulong32_t)pvNewObject;
                pvNewNext = (void *)(ul32BaseAddr+ul32ObjectOffset);

                /* Assign the new element Next Pointer to the Next Node */
                *pvNewNext = *pvNextNode; 
                
                /* Assign New element at the next pointer of Previous Node */
                *pvNext = pvNewObject;

				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Sort_Object_List :"
                    " Object Placed in link list\r\n");
				#endif

                /* There is no need to traverse the link list further, so break the 
                while loop */
                return pvNewObject;
            }
            else
            {
                /* Move to Next node in the list */
                pvPrevNode = *pvNextNode;
                ul32BaseAddr = (ulong32_t)*pvNextNode;
                /* Get offset of Next element */
				GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32ObjectOffset);
				memcpy((void *)&ul32OffsetAddr, (int8_t *)(ul32BaseAddr+
						ul32ObjectOffset), sizeof(uint32_t));
                /* Check end of Link List */
                if(BACNET_ZERO == ul32OffsetAddr)
                {
                    bEndList = TRUE;
                    /* Store the last element, so that end node can be added next to 
                    this node */
                    pvPrevNode = (void *)ul32BaseAddr;
                    /* End of Object List */
                    break;
                }
                else
                {
                    pvNextNode = (void *)(ul32BaseAddr+ul32ObjectOffset);
                }
            }
        }

        if(TRUE == bEndList)
        {
            /* Else if Next element pointer is null then add this element 
            at the end of list, so add the element at end node */    
            ul32BaseAddr = (ulong32_t)pvPrevNode;
            /* Get Next Pointer offset */
			GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32ObjectOffset);
            pvNext = (void *)(ul32BaseAddr+ul32ObjectOffset);
            /* Place the element in the end of list */
            *pvNext = pvNewObject;
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Sort_Object_List :"
                "Placing Object at end of list\r\n");
			#endif
            return pvNewObject;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Sort_Object_List : Exit \r\n");
	#endif

    /* Retrun List Base object pointer */
    return *pvObject;  
}

/**
*DESCRIPTION
*   Based on Object Type and Object Instance ID, this API 
*   Returns object pointer after traversing object linked list
*
*@param eObjectType [in] Object Type
*@param u32InObjID [in] Object Instance ID
*@param pVirtualDev [in] Pointer to Virtual Device struct
*
*@return void * [out] Object pointer of specific object Id, if not found
*                     returns NULL. Returns 0x0001 in case of duplicate object.
*
*/
void *Traverse_Object_List(BACNET_OBJECT_TYPE  eObjectType,
                                  uint32_t  ui32ObjId, virtualDevData_t *pVirtualDev,
                                  void **pPreElement)
{
    void *pvObject = NULL;
    ulong32_t ul32BaseAddr = 0;     /* For Base struct Addr */
    ulong32_t ul32OffsetAddr = 0 ;  /* For Data offset Addr */
    ulong32_t ul32NextOffset = 0;       /* For Object ID */
    ulong32_t ul32ObjectOffset = 0;    /* For getting object id offest */
    void        *pvTmpEle = NULL; 

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Traverse_Object_List : Entry \r\n");
	#endif

    /* TODO - NOTE: can replace switch case by GET_OBJBASE_OFFSET */
    /* Based on Object Type */
    switch(eObjectType)
    {
#ifdef BACDEL_OBJ_AI
        case OBJECT_ANALOG_INPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstAnalogInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_AO
        case OBJECT_ANALOG_OUTPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstAnalogOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_AV
        case OBJECT_ANALOG_VALUE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstAnalogValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BI
        case OBJECT_BINARY_INPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstBinaryInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BO
        case OBJECT_BINARY_OUTPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstBinaryOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BV
        case OBJECT_BINARY_VALUE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstBinaryValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSI
        case OBJECT_MULTI_STATE_INPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstMSInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSO
        case OBJECT_MULTI_STATE_OUTPUT:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstMSOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSV
        case OBJECT_MULTI_STATE_VALUE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstMSValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_NC
        case OBJECT_NOTIFICATION_CLASS:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstNotificationClass;
        }
        break;
#endif
#ifdef BACDEL_OBJ_CAL
        case OBJECT_CALENDAR:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstCalendar;
        }
        break;
#endif
#ifdef BACDEL_OBJ_SDL
        case OBJECT_SCHEDULE:
        {
            pvObject = (void *)pVirtualDev->m_stSupportedObjects.m_pstSchedule;
        }
        break;
#endif

        default:
        {
            /* Return the Located object Pointer */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Traverse_Object_List :"
                "Object Type Not Found\r\n");
			#endif
            pvObject = NULL;
        }
    }/* End of Switch Case */
    
    if(pvObject == NULL)
    {
        /* Return the Located object Pointer */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Traverse_Object_List :"
            "Object Not Found\r\n");
		#endif
        return pvObject;
    }
    /* Traverse the Object list */
    while(pvObject != NULL)
    {
        ul32BaseAddr = (ulong32_t )pvObject;
        /* This situation comes in case of traversal for particular object is required
        Otherwise user wants to traverse object list till end of list only */
		/* Get offset of object id for validation */
		GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32ObjectOffset);
		//GET_OBJECTID_OFFSET(eObjectType, u32ObjectOffset);
		/* Get the object identifier */
		pvTmpEle = (void *)(ul32BaseAddr+ul32ObjectOffset);

		/* Check if this object is present in list */
		if( memcmp(pvTmpEle, (void *)&ui32ObjId, sizeof(uint32_t)) == 0)
		{
			/* Return the Located object Pointer */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer:Traverse_Object_List :"
				"Object Found\r\n");
			#endif
			return pvObject;
		 }
		else
		{
			/* Keep track of previous element */
			*pPreElement = pvObject;
		}

        /* Move to Next object Pointer */
		GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32OffsetAddr);  
        memcpy((void *)&ul32NextOffset, (void *)(ul32BaseAddr+ul32OffsetAddr),
                                             sizeof(void *));        
        /* update pointer */
		pvObject = (void *)ul32NextOffset;
        /* If the next address fied contain NULL */
        if(ul32NextOffset == 0)
        {
            break;
        }
    }   

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Traverse_Object_List : Exit \r\n");
	#endif
 
    /* Retrun List Base object pointer */
    return pvObject;
}

/**
*
* DESCRIPTION
* This function is used to add / initialize the required property to given object.
*
* @param pVirtualDev	[in] virtual device pointer
* @param u32ObjectID	[in] object identifier
* @param eObjectType	[in] object type
* @param eObjvProp		[in] object property
* @param eObjvProp		[in] array index value
* @param pvData			[in] input data pointer
* @param pvObject		[in] object base address pointer
*
* @return void			[out] no return value.
*
*/
void Init_Required_Properties(void *pVirtualDev, uint32_t u32ObjectID,
                                     BACNET_OBJECT_TYPE eObjectType, 
                                     BACNET_PROPERTY_ID eObjvProp,
                                     int8_t i8ArrayIndex,
                                     void *pvData,
                                     void *pvObject)
{
    /* local variables */
    PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
    BACNET_RETURN_TYPE  eApiRetVal = BACDEL_ERROR;    
    void    *pvDummy = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Required_Properties : Entry \r\n");
	#endif

    /* Get the property access type for given object & property. */
    ePermission = GetDefndPropAccess(eObjectType, eObjvProp, ePermission);
    ePermission = Validate_PropertySupport(eObjectType, eObjvProp, ePermission);
    if(ePermission == NOT_SUPPORTED)
    {
        ePermission = BACDEL_PROPERTY_NOT_PRESENT;
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Required_Properties :"
            "BACDEL_PROPERTY_NOT_PRESENT\r\n");
		#endif
        return;
    }

#ifdef BACDEL_SER_DS_RPM_B
    /* Register property Id in Property List */
    Register_Object_Property(eObjectType, eObjvProp);
#endif

    /* convert & update the property value */
    eApiRetVal = Convert_InStr_AddProperty(pVirtualDev, pvObject, u32ObjectID, 
            eObjectType, eObjvProp, i8ArrayIndex, pvData, 
            ePermission, &pvDummy, false);

    if(eApiRetVal != BACDEL_SUCCESS)
    {
		;
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Required_Properties :"
            "Convert_InStr_AddProperty Failed\r\n");
		#endif
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Required_Properties : Exit \r\n");
	#endif

    return;
}


/** api to update the priority array property for objects with commandable
    present value property */
static void Init_Priority_Array(void *pvVirtualDev, BACNET_OBJECT_TYPE eObjectType,
                               void *pvObject,
                               int8_t   i8ArrayIndex)
{
    /* local varaibles */
    PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
    Pr_BACnetPriorityArray_t stPriorityArray = {0};
    BACNET_RETURN_TYPE eApiRetVal = BACDEL_ERROR;
    void *pvDummy = NULL;
    void *pvData = NULL;
    uint32_t u32PropPtr = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Priority_Array : Entry \r\n");
	#endif

    /* Priority Array has to be initialize if Present Value property of 
    given object type is Commandable */
    /* Get the property access type for given object & property. */
    ePermission = GetDefndPropAccess(eObjectType, PROP_PRESENT_VALUE, BACNET_DEFAULT);
    if(ePermission == NOT_SUPPORTED)
    {
		;
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Priority_Array :"
            "BACDEL_PROPERTY_NOT_PRESENT\r\n");
		#endif
    }

    /* If PV is commandable then only initialization priority array */
    if(ePermission == COMMANDABLE_PROP)
    {
#ifdef BACDEL_SER_DS_RPM_B
        /* Register property Id in Property List */
        Register_Object_Property(eObjectType, PROP_PRIORITY_ARRAY);
#endif

        /* get access type for priority array */
        ePermission = GetDefndPropAccess(eObjectType, PROP_PRIORITY_ARRAY, BACNET_DEFAULT);
		
        /* get property data */
        pvData = &stPriorityArray;

        /* update the property value */
        eApiRetVal = UpdateCommonProperty(eObjectType, pvObject, PROP_PRIORITY_ARRAY, 
            i8ArrayIndex, pvData, ePermission, &u32PropPtr, &pvDummy, pvVirtualDev,
            true, false, false);
        if(eApiRetVal == BACDEL_ERROR)
        {
			;
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Priority_Array :"
                "UpdateCommonProperty Failed\r\n");
			#endif
        }
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Priority_Array : Exit \r\n");
	#endif
}

/** api to update the relinquish default property for objects with commandable
    present value property */
static void Init_Relinquish_Default(void *pvVirtualDev, BACNET_OBJECT_TYPE eObjectType,
                               void *pvObject, int8_t i8ArrayIndex)
{
    /* local varaibles */
    PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
    BACNET_RETURN_TYPE eApiRetVal = BACDEL_ERROR;
    void *pvDummy = NULL;
    uint32_t u32PropPtr = 0;
    PropertyValue_u uPropVal = {0};

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Relinquish_Default : Entry \r\n");
	#endif

    /* Relinquish default has to be initialize if Present Value property of 
    given object type is Commandable */
    /* Get the property access type for given object & property. */
    ePermission = GetDefndPropAccess(eObjectType, PROP_PRESENT_VALUE, BACNET_DEFAULT);
    if(ePermission == NOT_SUPPORTED)
    {
		;
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Relinquish_Default :"
            "BACDEL_PROPERTY_NOT_PRESENT\r\n");
		#endif
    }

    /* In case AO,AV,BO,BV,MSO & MSV, if these properties are commandable then only do 
    initialization for it */
    if(ePermission == COMMANDABLE_PROP)
    {
#ifdef BACDEL_SER_DS_RPM_B
        /* Register property Id in Property List */
        Register_Object_Property(eObjectType, PROP_RELINQUISH_DEFAULT);
#endif

        /* get access type for relinquish default */
        ePermission = GetDefndPropAccess(eObjectType, PROP_RELINQUISH_DEFAULT, BACNET_DEFAULT);

        switch(eObjectType)
        {
            case OBJECT_ANALOG_INPUT:
            case OBJECT_ANALOG_OUTPUT:
            case OBJECT_ANALOG_VALUE:
            {
                uPropVal.fVal = (Float_t)atof(ANALOG_PRESENT_VALUE);
            }
            break;
            case OBJECT_BINARY_INPUT:
            case OBJECT_BINARY_OUTPUT:
            case OBJECT_BINARY_VALUE:
            {
                uPropVal.eVal = atoi(BINARY_PRESENT_VALUE);
            }
            break;
            case OBJECT_MULTI_STATE_INPUT:
            case OBJECT_MULTI_STATE_OUTPUT:
            case OBJECT_MULTI_STATE_VALUE:
            {
                uPropVal.u32Val = atoi(MULTISTATE_PRESENT_VALUE);
            }
            break;

            default:
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Present_Value :"
                    "Object Type Not Supported\r\n");
				#endif
                return;
            }
        }

        /* update the property value */
        eApiRetVal = UpdateCommonProperty(eObjectType, pvObject, PROP_RELINQUISH_DEFAULT, 
            i8ArrayIndex, &uPropVal, ePermission, &u32PropPtr, &pvDummy, pvVirtualDev,
            true, false, false);
        if(eApiRetVal == BACDEL_ERROR) //if(eApiRetVal != BACDEL_SUCCESS)
        {
			;
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Relinquish_Default :"
                "UpdateCommonProperty Failed \r\n");
			#endif
        }
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Relinquish_Default : Exit \r\n");
	#endif
}

/** api to update/initialize the present value property */
static void Init_Present_Value(void *pvVirtualDev, BACNET_OBJECT_TYPE eObjectType, void *pvObject)
{
    /* local variables */
    PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
    ulong32_t ul32Propoffset = 0;
    ulong32_t ul32BaseAddr = 0;
    PropertyValue_u uPresentVal = {0};
    void *pvTemp = NULL;
	void *pvBackupPv = NULL;
    int32_t i32Len = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Present_Value : Entry \r\n");
	#endif

    /* Get the property access type for given object & property. */
    ePermission = GetDefndPropAccess(eObjectType, PROP_PRESENT_VALUE, ePermission);
    if(ePermission == NOT_SUPPORTED)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Present_Value :"
            "BACDEL_PROPERTY_NOT_PRESENT\r\n");
		#endif
        return;
    }

	ul32BaseAddr = (ulong32_t)pvObject;

    /* get the present value offset */
    GET_PRESENTVAL_OFFSET(eObjectType, m_stPresentValue, ul32Propoffset, i32Len);
    pvTemp = (void *)(ul32BaseAddr + ul32Propoffset);

    /* get the backup present value offset */
    GET_PRESENTVAL_OFFSET(eObjectType, m_stBackupPresentValue, ul32Propoffset, i32Len);
    pvBackupPv = (void *)(ul32BaseAddr + ul32Propoffset);

    /* Initialize the required properties for Object */
    switch(eObjectType)
    {
        /* real present value */
        case OBJECT_ANALOG_INPUT:
        case OBJECT_ANALOG_OUTPUT:
        case OBJECT_ANALOG_VALUE:
        {
            uPresentVal.fVal = (Float_t)atof(ANALOG_PRESENT_VALUE);
        }
        break;

        /* enumerated present value */
        case OBJECT_BINARY_INPUT:
        case OBJECT_BINARY_OUTPUT:
        case OBJECT_BINARY_VALUE:
        {
            uPresentVal.eVal = atoi(BINARY_PRESENT_VALUE);
        }
        break;

        /* unsigned present value */
        case OBJECT_MULTI_STATE_INPUT:
        case OBJECT_MULTI_STATE_OUTPUT:
        case OBJECT_MULTI_STATE_VALUE:
        {
            uPresentVal.u32Val = atoi(MULTISTATE_PRESENT_VALUE);
        }
        break;

        /* boolean present value */
#ifdef BACDEL_OBJ_CAL
        case OBJECT_CALENDAR:
        {
            uPresentVal.bVal = (bool)(atoi(CALENDAR_PRESENT_VALUE));
        }
        break;
#endif
        /* default - return */
        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Init_Present_Value :"
                "Object Type Not Supported\r\n");
			#endif
            return;
        }
    }

    /* copy the present value */
    memcpy(pvTemp, (void*)&uPresentVal, i32Len);
	memcpy(pvBackupPv, (void*)&uPresentVal, i32Len);

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Init_Present_Value : Exit \r\n");
	#endif
}
#ifdef BACDEL_PR14
/** function to update/initialize the property List property */
void Init_Property_List(BACNET_OBJECT_TYPE eObjectType, void *pvObject)
{
    /* local variables */
    ulong32_t ul32BaseAddr = 0;     /* For object base addr */
	ulong32_t ul32OffsetAddr = 0 ;  /* For data offset addr */
	special_property_list_t stPropertyList = {0};
	//PROP_ACCESS_TYPE ePermission = BACNET_DEFAULT;
	Pr_BACnetPropertyList_t *pstPropList = NULL;
	uint8_t u8SkipPropCnt = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Init_Property_List: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pvObject)
	{
		/* invalid inputs pointer */
		return;
	}

	/* save the base address of given object. */
	ul32BaseAddr = (ulong32_t)pvObject;

	/* get offset of property */
    GET_OBJCOMMON_OFFSET(eObjectType, m_stPropertyList, ul32OffsetAddr);

	/* initialize property list for Input Object Type */
	Device_Objects_Property_List(eObjectType, &stPropertyList);

	/* save the property address */
	pstPropList = (Pr_BACnetPropertyList_t *)(ul32BaseAddr+ul32OffsetAddr);
	pstPropList->m_psti32RequiredProp = stPropertyList.Required.pList;
	pstPropList->m_psti32OptionalProp = stPropertyList.Optional.pList;
	pstPropList->m_psti32ProprietaryProp = stPropertyList.Proprietary.pList;

	/* get skip property count value */
	u8SkipPropCnt = Get_PropertyList_SkipCount(&stPropertyList);
	/* save total property count value */
	pstPropList->m_u32PropCount = 
		(stPropertyList.Required.count + stPropertyList.Optional.count
		+ stPropertyList.Proprietary.count - u8SkipPropCnt);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Init_Property_List: Exit \r\n");
	#endif
}
#endif /* BACDEL_PR14 */
/** Initialize the properties with default values */
static void Init_Object_Property (BACNET_PROPERTY_ID ePropId,
                          virtualDevData_t *pVirtualDev,
                          int8_t i8ArrayIndex,
                          uint32_t  ui32ObjId,
                          BACNET_OBJECT_TYPE eObjectType,
                          void *pvObject)
{
	/* local variables */
    int32_t     i32Data = 0;
    void        *pvData = NULL;
    Float_t     fData   = 0.0;
	Double_t    dData   = 0;
	uint32_t     u32Data = 0;
	uint16_t    u16Data =0;
	Pr_BACnetDateTime_t    stDateTime = {0};

	/* external global variables */
	extern uint8_t g_au8ObjDescription[MAX_SUPPORTED_CHRSTR_LEN];
	extern uint8_t g_au8ProfileName[MAX_SUPPORTED_CHRSTR_LEN];

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Init_Object_Property: entry \r\n");
	#endif

    switch(ePropId)
    {
         /* Priority Array */
        case PROP_PRIORITY_ARRAY:
        {
            Init_Priority_Array(pVirtualDev, eObjectType, pvObject, i8ArrayIndex);
            return;
        }
        break;

        /* Relinquish Default */
        case PROP_RELINQUISH_DEFAULT:
        {
            Init_Relinquish_Default(pVirtualDev, eObjectType, pvObject, i8ArrayIndex);
            return;
        }
        break;

	    /* Present Value */
        case PROP_PRESENT_VALUE:
        {
            Init_Present_Value(pVirtualDev, eObjectType, pvObject);
            return;
        }
        break;

#ifdef BACDEL_PR18
		case PROP_CURRENT_COMMAND_PRIORITY:
		{
			/* check if PV is commandable */
			if(COMMANDABLE_PROP == GetDefndPropAccess(eObjectType, PROP_PRESENT_VALUE, BACNET_DEFAULT))
			{
				/* save the value */
				i32Data = atoi(CURRENT_COMMAND_PRIORITY);
				pvData = &i32Data;
			}
			else
			{
				/* PV is not commandable, no need to initialize this property */
				return;
			}
		}
		break;
#endif

#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_PR14)
		/* Event_Detection_Enable */
        case PROP_EVENT_DETECTION_ENABLE:
	    {
	    	switch(eObjectType)
	    	{
				case OBJECT_ANALOG_INPUT:
				case OBJECT_ANALOG_OUTPUT:
				case OBJECT_ANALOG_VALUE:
				{
				    i32Data = atoi("1");
				    pvData = &i32Data;
					break;
				}
				default:
				{
				    i32Data = atoi(EVENT_DETECTION_ENABLE);
				    pvData = &i32Data;
				}
	    	}

		    /*Init_Required_Properties(pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId,
			    ui32ObjId, eObjectType, PROP_EVENT_DETECTION_ENABLE, i8ArrayIndex, pvData);*/
	    }
        break;

		/* event_algorithm_inhibit */
		case PROP_EVENT_ALGORITHM_INHIBIT:
	    {
		    i32Data = atoi(EVENT_ALGO_INHIBIT);
		    pvData = &i32Data;
		    /*Init_Required_Properties(pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId,
			    ui32ObjId, eObjectType, PROP_EVENT_ALGORITHM_INHIBIT, i8ArrayIndex, pvData);*/
	    }
        break;

#ifdef OPTIONAL_PROPERTY
		/* Time_Delay_Normal */
        case PROP_TIME_DELAY_NORMAL:
	    {
            i32Data = atoi(TIME_DELAY_NORMAL);
		    pvData = &i32Data;
		   /* Init_Required_Properties(pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId,
			    ui32ObjId, eObjectType, PROP_TIME_DELAY_NORMAL, i8ArrayIndex, pvData);*/
        }
        break;
#endif

		case PROP_EVENT_MESSAGE_TEXTS:
        {
			uint8_t au8Arr[] = " ";
            pvData = &au8Arr;
            Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType,
            		PROP_EVENT_MESSAGE_TEXTS, i8ArrayIndex, pvData, pvObject);
            return;
	    }
        break;

		case PROP_EVENT_MESSAGE_TEXTS_CONFIG:
        {
			uint8_t au8Arr[] = " ";
            pvData = &au8Arr;
            Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType,
            		PROP_EVENT_MESSAGE_TEXTS_CONFIG, i8ArrayIndex, pvData, pvObject);
            return;
	    }
        break;

#endif
	    /* Status Flag */
        case PROP_STATUS_FLAGS:	
	    {
		    i32Data = atoi(STATUS_FLAG);
		    pvData = &i32Data;
	   }
        break;

        /* Event State */
        case PROP_EVENT_STATE:
	    {
		    i32Data = EVENT_STATE;
		    pvData = &i32Data;   
	  }
        break;

        /* Out of Service */
        case PROP_OUT_OF_SERVICE:
	    {
		    i32Data = OUT_OF_SERVICE;
		    pvData = &i32Data;
	    }
        break;

        /* Unit */
        case PROP_UNITS:
        {
        	i32Data = UNITS_NO_UNITS;
            pvData = &i32Data;
        }
        break;

#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO)
	    /* Polarity */
        case PROP_POLARITY:
        {
            i32Data = POLARITY;
            pvData = &i32Data;
        }
        break;
#endif /* BI - BO */

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
	    /* Number_Of_States */
        case PROP_NUMBER_OF_STATES:
        {
            i32Data = atoi(NUMBER_OF_STATE);
			pvData = &i32Data;
		}
        break;
#endif

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_DS_COVP_B)
        /* COV Increment */
        case PROP_COV_INCREMENT:
        {
            switch(eObjectType)
            {
            	case OBJECT_ANALOG_INPUT:
					fData = (Float_t)atof(AI_COV_INCREMENT);
					pvData = &fData;
				break;

				case OBJECT_ANALOG_OUTPUT:
					fData = (Float_t)atof(AO_COV_INCREMENT);
					pvData = &fData;
				break;
				
				case OBJECT_ANALOG_VALUE:
					fData = (Float_t)atof(AV_COV_INCREMENT);
					pvData = &fData;
				break;

                default:
                    fData = (Float_t)atof(AI_COV_INCREMENT);
                    pvData = &fData;
                break;
            }
        }
        break;
#endif

#ifdef OPTIONAL_PROPERTY
#if (defined BACDEL_OBJ_BI || defined BACDEL_OBJ_BO || defined BACDEL_OBJ_BV)
        /* ACTIVE_TEXT */
        case PROP_ACTIVE_TEXT:
        {
            pvData = (void *)ACTIVE_TEXT;
        }
        break;

        /* INACTIVE_TEXT */
        case PROP_INACTIVE_TEXT:
        {
            pvData = (void *)INACTIVE_TEXT;
        }
        break;

        /* Change of State Time */
        case PROP_CHANGE_OF_STATE_TIME:
        {
		    memset((void *)&stDateTime, 0x00,sizeof(Pr_BACnetDateTime_t));
            pvData = &stDateTime.m_stDateTime;
        }
        break;

        /* Change_Of_State_Count */
        case PROP_CHANGE_OF_STATE_COUNT:
        {
            u16Data = atoi(CHANGE_OF_STATE_COUNT);
            pvData = &u16Data;
        }
        break;

        /* Time_Of_State_Count_Reset */
        case PROP_TIME_OF_STATE_COUNT_RESET:
        {
            memset((void *)&stDateTime, 0x00,sizeof(Pr_BACnetDateTime_t));
            pvData = &stDateTime.m_stDateTime;
        }
        break;

        /* Elapsed_Active_Time */
        case PROP_ELAPSED_ACTIVE_TIME:
        {
            u32Data = atoi(ELAPSED_ACTIVE_TIME);
            pvData = &u32Data;
        }
        break;

        /* Time_Of_Active_Time_Reset */
        case PROP_TIME_OF_ACTIVE_TIME_RESET:
        {
            memset((void *)&stDateTime, 0x00,sizeof(Pr_BACnetDateTime_t));
            pvData = &stDateTime.m_stDateTime;
        }
        break;
#endif /* BI - BO - BV */

#if (defined BACDEL_OBJ_MSI || defined BACDEL_OBJ_MSO || defined BACDEL_OBJ_MSV)
	    /* State_Text */
        case PROP_STATE_TEXT:
        {
			uint8_t au8Arr[200];
			memset(&au8Arr,0,sizeof(au8Arr));
			strcpy((char*)&au8Arr,"S1,S2,S3,S4,S5");


            /* NOTE : update no of states as per no of states default value */
            pvData = &au8Arr;
			
		   Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType,
            		PROP_STATE_TEXT, i8ArrayIndex, pvData, pvObject);
            return;
	    }
        
#endif /* MSI - MSO - MSV */

        case PROP_DESCRIPTION:
        {
            /* Object Description - optional */
	        pvData = g_au8ObjDescription;
		}
        break;

	    /* Profile Name - optional */
        case PROP_PROFILE_NAME:
        {
	        pvData = g_au8ProfileName;
	    }
        break;

	    /* Device Type - optional */
        case PROP_DEVICE_TYPE:
        {
		    pvData = "Type of Device";			
		}
        break;

#if (defined BACDEL_OBJ_AI || defined BACDEL_OBJ_AO || defined BACDEL_OBJ_AV )
	    /* Min Pres-Value */
        case PROP_MIN_PRES_VALUE:
        {
			switch(eObjectType)
			{
				case OBJECT_ANALOG_INPUT:
				case OBJECT_ANALOG_OUTPUT:
				case OBJECT_ANALOG_VALUE:
					i32Data = (int32_t)atof(MIN_PRESENT_VALUE);
            		pvData = &i32Data;
				break;
				default:
					fData = (Float_t)atof(MIN_PRESENT_VALUE);
            		pvData = &fData;
				break;

			}
		}
        break;
#endif /* AI - AO */

	    /* Resolution */
        case PROP_RESOLUTION:
        {
			switch(eObjectType)
			{

				default:
					fData = (Float_t)atof(RESOLUTION);
            		pvData = &fData;
				break;
			}
	   }
        break;

	    /* Min On Time */
        case PROP_MINIMUM_ON_TIME:
        {
		    i32Data = atoi(MIN_ON_TIME);
            pvData = &i32Data;
        }
        break;

	    /* Min Off Time */
        case PROP_MINIMUM_OFF_TIME:
        {
		    i32Data = atoi(MIN_OFF_TIME);
            pvData = &i32Data;
        }
        break;

#endif /* OPTIONAL_PROPERTY */

#if (defined OPTIONAL_PROPERTY)
	    /* Max Pres-Value */
        case PROP_MAX_PRES_VALUE:
        {
        	switch(eObjectType)
        	{
        		case OBJECT_ANALOG_INPUT:
        		case OBJECT_ANALOG_OUTPUT:
        		case OBJECT_ANALOG_VALUE:
        		{
                    fData = (Float_t)atof(MAX_PRESENT_VALUE);
                    pvData = &fData;
                    break;
        		}

        		default:
        		{
                    fData = (Float_t)atof(MAX_PRESENT_VALUE);
                    pvData = &fData;
        		}
        	}
 		}
        break;
#endif /* (defined OPTIONAL_PROPERTY) */

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ESUM_A) || (defined BACDEL_SER_AE_ESUM_B))
	    /* Notification Class - Copy id of NC obj directly */
        case PROP_NOTIFICATION_CLASS:
        {
		    i32Data = (int32_t)ui32ObjId;
            pvData = &i32Data;
		}
        break;
#endif

#ifdef BACDEL_OBJ_NC		    
	    /* Priority */    
        case PROP_PRIORITY:
        {
            uint8_t	u8Priority[] = "2,3,1";
	        pvData = &u8Priority;
            Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_PRIORITY, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;

	    /* Recipient_List */
        case PROP_RECIPIENT_LIST:
        {
            int8_t i8temp[]= {"(((Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday), 00:00, 23:59,(8, 200), 100, False, (TRUE, TRUE, TRUE)))"};
	        pvData = &i8temp[0];
	        Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType,
                PROP_RECIPIENT_LIST, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;

	    /* Ack_Required */
        case PROP_ACK_REQUIRED:
        {
	        uint8_t u8bitstring[] = "0,0,0";
            pvData = &u8bitstring[0];
	        Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_ACK_REQUIRED, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;
#endif /* BACDEL_OBJ_NC */


	    /* Event_Enable */
        case PROP_EVENT_ENABLE:
        {
            uint8_t u8bitstring[] = "0,0,0";
	        pvData = &u8bitstring[0];
	        Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_EVENT_ENABLE, i8ArrayIndex, pvData, pvObject);
            return;
        }

#ifdef BACDEL_OBJ_CAL
	    /* Date List Property */
       case PROP_DATE_LIST:
        {
		    int8_t strcal[] = {"(01-1-2012)"};
		    pvData = &strcal[0];
		    Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_DATE_LIST, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;
#endif /* BACDEL_OBJ_CAL */

#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B || defined BACDEL_OBJ_SDL)
        /* Reliablity - optional, required for intrinsic reporting or scheduling object or Life Safety Point Object 
			or Life Safety Zone object */
        case PROP_RELIABILITY:
        {
            i32Data = RELIABILITY;
            pvData = &i32Data;   
	   }
        break;
#endif

#ifdef BACDEL_OBJ_SDL        

	    /* Effective Period */
        case PROP_EFFECTIVE_PERIOD:
        {
            int8_t i8strEffPeriod[] = {"((01-01-2012)-(01-12-2013))"};
	        pvData = &i8strEffPeriod;
	    }
        break;

        /* List of PROP_SCHEDULE_DEFAULT */
        case PROP_SCHEDULE_DEFAULT:
        {
            //int8_t i8StrScheduleDefault[] = {"0,0"};
	        pvData = NULL;
	    }
        break;

	    /* List of Object Property References */
        case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
        {
            //int8_t i8strObjReffProp[] = {"((8,12),(2,1), 45)"};
	        pvData = NULL;
	    }
        break;

	    /* Weekly Schedule */
        case PROP_WEEKLY_SCHEDULE:
        {
		    int8_t i8StrWeeklySchedule[] =  {" ((1:00,1)),\
						       ((2:00,1)),\
						       ((3:00,0)),\
						       ((4:00,1)),\
						       ((5:00,1)),\
						       ((6:00,1)),\
						       ((7:00,1))"};

	        pvData = &i8StrWeeklySchedule[0] ;//NULL;
	    }
        break;

        /* Exception Schedule */
        case PROP_EXCEPTION_SCHEDULE:
        {
            //int8_t i8StrExcepSchedule[] = {"(((23-9-2012)),((0:00,0)),10)"};
	        pvData = NULL;//&i8StrExcepSchedule[0];
	    }
        break;
#endif /* BACDEL_OBJ_SDL */

#if ( defined BACDEL_OBJ_SDL)
        case PROP_PRIORITY_FOR_WRITING:
        {
            /* Priority for Writing */
            i32Data = atoi(PRIORITY_FOR_WRITING);
            pvData = &i32Data;
        }
        break;
#endif

#if (defined OPTIONAL_PROPERTY && (defined BACDEL_OBJ_AI ))
        /* Update Interval */
        case PROP_UPDATE_INTERVAL:
        {

            i32Data = atoi(OBJ_UPDATE_INTERVAL);
            pvData = &i32Data;
        }
        break;
#endif

#if (defined BACDEL_SER_AE_EN_B)
	    /* alarm values - list of unsigned */
        case PROP_ALARM_VALUES:
        {
			/* assign value ase per object type */
			switch(eObjectType)
			{
			default:
				pvData = ALARM_VALUES;
				break;
			}
        }
        break;
#endif

#ifdef BACDEL_SER_AE_EN_B
	    /* time delay */
        case PROP_TIME_DELAY:
        {
            i32Data = atoi(TIME_DELAY);
		    pvData = &i32Data;
		}
        break;

	    /* high limit */
        case PROP_HIGH_LIMIT:
        {
            switch(eObjectType)
            {
            	case OBJECT_ANALOG_INPUT:
            	case OBJECT_ANALOG_OUTPUT:
            	case OBJECT_ANALOG_VALUE:
            	{
                    fData = (Float_t)atof(HIGH_LIMIT);
                    pvData = &fData;
            		break;
            	}
                default:
                {
                    fData = (Float_t)atof(HIGH_LIMIT);
                    pvData = &fData;
                }
                break;
            }
        }
        break;

	    /* lowlimit */
        case PROP_LOW_LIMIT:
        {
            switch(eObjectType)
            {
				case OBJECT_ANALOG_INPUT:
				case OBJECT_ANALOG_OUTPUT:
				case OBJECT_ANALOG_VALUE:
				{
					fData = (Float_t)atof(LOW_LIMIT);
					pvData = &fData;
					break;
				}
                default:
                {
                    fData = (Float_t)atof(LOW_LIMIT);
                    pvData = &fData;
                }
                break;
            }
        }
        break;

	    /* deadband */
        case PROP_DEADBAND:
        {
            switch(eObjectType)
            {

                default:
                    fData = (Float_t)atof(DEADBAND);
                    pvData = &fData;
                break;
            }
         }
        break;

        /* limit enable */
        case PROP_LIMIT_ENABLE:
         {
            uint8_t u8BitstringLEArr[]		= "0,0";
		    pvData = &u8BitstringLEArr[0];
            Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_LIMIT_ENABLE, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;

        /* alarm value - binary pv */
        case PROP_ALARM_VALUE:
	    {
		    i32Data = atoi(ALARM_VALUE);
            pvData = &i32Data;
		}
        break;

        /* feedback value - binary pv */
        case PROP_FEEDBACK_VALUE:
	    {
            if(eObjectType == OBJECT_MULTI_STATE_OUTPUT)
                i32Data = atoi(FEEDBACK_VALUE_UNSIGNED);
            else
		        i32Data = atoi(FEEDBACK_VALUE_BINARY_PV);
            pvData = &i32Data;
	    }
        break;


#endif /* BACDEL_SER_AE_EN_B */

 /* high limit */ //PR-19
               case PROP_FAULT_HIGH_LIMIT:
               {
                   switch(eObjectType)
                   {
                   	case OBJECT_ANALOG_INPUT:
                    case OBJECT_ANALOG_VALUE:
                   	{
                           fData = (Float_t)atof(FAULT_HIGH_LIMIT);
                           pvData = &fData;
                   		break;
                   	}
				   default:
				   {
					   fData = (Float_t)atof(FAULT_HIGH_LIMIT);
					   pvData = &fData;
				   }
				   break;
                   }
               }
               break;

       	    /* lowlimit */
               case PROP_FAULT_LOW_LIMIT:
               {
                   switch(eObjectType)
                   {
       				case OBJECT_ANALOG_INPUT:
       				case OBJECT_ANALOG_VALUE:
       				{
       					fData = (Float_t)atof(FAULT_LOW_LIMIT);
       					pvData = &fData;
       					break;
       				}
				   default:
				   {
					   fData = (Float_t)atof(FAULT_LOW_LIMIT);
					   pvData = &fData;
				   }
				   break;
                   }
               }
               break;

	    /* acked transitions */
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_A) || (defined BACDEL_OBJ_EE))
        case PROP_ACKED_TRANSITIONS:
        {
            uint8_t u8bitstring2[]			= "1,1,1"; /* for acked transition */
            pvData = &u8bitstring2[0];
            Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_ACKED_TRANSITIONS, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;
#endif

	    /* notify type */
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ASUM_B) || (defined BACDEL_OBJ_EE))
        case PROP_NOTIFY_TYPE:
        {
	        i32Data = atoi(NOTIFY_TYPE);
            pvData = &i32Data;
        }
        break;
#endif

    	/* event time stamp */
#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_OBJ_EE))
        case PROP_EVENT_TIME_STAMPS:
        {
            /* event time stamp structure */
	        Pr_BACnetEventTimeStamp_t	stEventTimeStamp; //= {0xFF};
	        memset(&stEventTimeStamp,0xFF,sizeof(stEventTimeStamp));
            pvData = (Pr_BACnetEventTimeStamp_t *)&stEventTimeStamp;
            Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
                PROP_EVENT_TIME_STAMPS, i8ArrayIndex, pvData, pvObject);
            return;
        }
        break;
#endif

		default :
        {
            return;
        }
    }

    /* Update Property */
    Init_Required_Properties(pVirtualDev, ui32ObjId, eObjectType, 
        ePropId, i8ArrayIndex, pvData, pvObject);

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Init_Object_Property: exit \r\n");
	#endif
}

/**
*                                                                         
*DESCRIPTION                                                                          
*        This API initializes the required propery for provided object
*    
*@param pVirtualDev Pointer to Device containing desired object.
*@param ui32Objid The Object Identifier for new object
*@param pucObjName  Pointer to New Object Name
*
*@return none
*/
static void Init_Object_All_Properties(BACNET_OBJECT_TYPE eObjectType,
                                 virtualDevData_t *pVirtualDev,
                                 int8_t i8ArrayIndex,
                                 uint32_t  ui32ObjId, uint8_t *pui8ObjName,
                                 void *pvObject)   
{
    /* local variables */
    uint32_t i32count    = 0 ;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Init_Object_All_Properties: Entry \r\n");
	#endif
    
    switch(eObjectType)
    {
        /* Find the Object whose properties need to be initialized */        
#ifdef BACDEL_OBJ_AI
        case OBJECT_ANALOG_INPUT :
            {
            	int32_t ObjAIProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE, PROP_OUT_OF_SERVICE, PROP_UNITS

            	#ifdef OPTIONAL_PROPERTY
				,PROP_DESCRIPTION,PROP_DEVICE_TYPE,PROP_RELIABILITY,PROP_UPDATE_INTERVAL,PROP_MIN_PRES_VALUE, PROP_MAX_PRES_VALUE,
				PROP_RESOLUTION, PROP_COV_INCREMENT,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,PROP_HIGH_LIMIT,PROP_LOW_LIMIT,PROP_DEADBAND,PROP_LIMIT_ENABLE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,
				PROP_NOTIFY_TYPE,PROP_TIME_DELAY_NORMAL,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,PROP_PROPERTY_LIST,PROP_FAULT_HIGH_LIMIT,PROP_FAULT_LOW_LIMIT,
            	#endif
				};

                for(i32count = 0; i32count < sizeof(ObjAIProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjAIProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_AO
        case OBJECT_ANALOG_OUTPUT :
            {
                int32_t ObjAOProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS,PROP_EVENT_STATE,
									PROP_OUT_OF_SERVICE,PROP_UNITS,PROP_PRIORITY_ARRAY,PROP_RELINQUISH_DEFAULT
#ifdef OPTIONAL_PROPERTY
									,PROP_DESCRIPTION,PROP_DEVICE_TYPE, PROP_RELIABILITY, PROP_MIN_PRES_VALUE , PROP_MAX_PRES_VALUE,
									PROP_RESOLUTION, PROP_COV_INCREMENT,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,PROP_HIGH_LIMIT,
									PROP_LOW_LIMIT,PROP_DEADBAND,PROP_LIMIT_ENABLE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,
									PROP_NOTIFY_TYPE,PROP_TIME_DELAY_NORMAL,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,
									PROP_PROPERTY_LIST

#ifdef BACDEL_PR18
				, PROP_CURRENT_COMMAND_PRIORITY
				#endif
#endif
									  };
                for(i32count = 0; i32count < sizeof(ObjAOProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjAOProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_AV
        case OBJECT_ANALOG_VALUE :
            {
				int32_t ObjAVProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE, PROP_OUT_OF_SERVICE, PROP_UNITS
#ifdef OPTIONAL_PROPERTY
										,PROP_DESCRIPTION,PROP_RELIABILITY,PROP_PRIORITY_ARRAY, 
										PROP_RELINQUISH_DEFAULT, PROP_COV_INCREMENT,PROP_MIN_PRES_VALUE, PROP_MAX_PRES_VALUE,
										PROP_RESOLUTION,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,PROP_HIGH_LIMIT,
										PROP_LOW_LIMIT,PROP_DEADBAND,PROP_LIMIT_ENABLE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,
										PROP_NOTIFY_TYPE,PROP_TIME_DELAY_NORMAL,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,
										PROP_PROPERTY_LIST,PROP_FAULT_HIGH_LIMIT,PROP_FAULT_LOW_LIMIT
#ifdef BACDEL_PR18
				, PROP_CURRENT_COMMAND_PRIORITY
				#endif
				#endif
									};
									
                for(i32count = 0; i32count < sizeof(ObjAVProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjAVProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_BI						
        case OBJECT_BINARY_INPUT:
            {
                int32_t ObjBIProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE, PROP_OUT_OF_SERVICE,
                		PROP_POLARITY
#ifdef OPTIONAL_PROPERTY
					,PROP_DESCRIPTION, PROP_DEVICE_TYPE, PROP_RELIABILITY, PROP_INACTIVE_TEXT, PROP_ACTIVE_TEXT,
					PROP_CHANGE_OF_STATE_TIME, PROP_CHANGE_OF_STATE_COUNT,PROP_TIME_OF_STATE_COUNT_RESET,
					PROP_ELAPSED_ACTIVE_TIME, PROP_TIME_OF_ACTIVE_TIME_RESET,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,
					PROP_ALARM_VALUE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,
					PROP_NOTIFY_TYPE,PROP_TIME_DELAY_NORMAL,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,
					PROP_PROPERTY_LIST
#endif
					};

                for(i32count = 0; i32count < sizeof(ObjBIProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjBIProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_BO
        case OBJECT_BINARY_OUTPUT :
            {
				int32_t ObjBOProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE, PROP_OUT_OF_SERVICE,
									   PROP_POLARITY,PROP_PRIORITY_ARRAY, PROP_RELINQUISH_DEFAULT
#ifdef OPTIONAL_PROPERTY
					,PROP_DESCRIPTION, PROP_DEVICE_TYPE, PROP_RELIABILITY, PROP_INACTIVE_TEXT, PROP_ACTIVE_TEXT,
					PROP_CHANGE_OF_STATE_TIME,PROP_CHANGE_OF_STATE_COUNT, PROP_TIME_OF_STATE_COUNT_RESET,
					PROP_ELAPSED_ACTIVE_TIME,PROP_TIME_OF_ACTIVE_TIME_RESET,PROP_MINIMUM_OFF_TIME,
					PROP_MINIMUM_ON_TIME, PROP_FEEDBACK_VALUE,PROP_PRIORITY_ARRAY,PROP_TIME_DELAY,
					PROP_NOTIFICATION_CLASS,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,PROP_NOTIFY_TYPE,
					PROP_TIME_DELAY_NORMAL,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,
					PROP_PROPERTY_LIST,PROP_CURRENT_COMMAND_PRIORITY
#ifdef BACDEL_PR18
				, PROP_CURRENT_COMMAND_PRIORITY
				#endif
				#endif
					}; 
								
                for(i32count = 0; i32count < sizeof(ObjBOProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjBOProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_BV
        case OBJECT_BINARY_VALUE :
            {
				int32_t ObjBVProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE,  PROP_OUT_OF_SERVICE 
#ifdef OPTIONAL_PROPERTY
					, PROP_DESCRIPTION, PROP_RELIABILITY, PROP_INACTIVE_TEXT, 
					PROP_ACTIVE_TEXT, PROP_CHANGE_OF_STATE_TIME,PROP_CHANGE_OF_STATE_COUNT, PROP_TIME_OF_STATE_COUNT_RESET,
					PROP_ELAPSED_ACTIVE_TIME, PROP_TIME_OF_ACTIVE_TIME_RESET, PROP_MINIMUM_OFF_TIME, PROP_MINIMUM_ON_TIME
					,PROP_PRIORITY_ARRAY ,PROP_RELINQUISH_DEFAULT,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,
					PROP_ALARM_VALUE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,PROP_NOTIFY_TYPE,PROP_TIME_DELAY_NORMAL,
					PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,PROP_PROPERTY_LIST
#endif
					};
                for(i32count = 0; i32count < sizeof(ObjBVProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjBVProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            } 
            break;
#endif

#ifdef BACDEL_OBJ_CAL
        case OBJECT_CALENDAR :
            {
                int32_t ObjCALProp[] = {PROP_DESCRIPTION, PROP_PRESENT_VALUE, PROP_DATE_LIST,PROP_PROPERTY_LIST};

                for(i32count = 0; i32count < sizeof(ObjCALProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjCALProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_MSI
        case OBJECT_MULTI_STATE_INPUT :
            {
				int32_t ObjMSIProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE,
                PROP_OUT_OF_SERVICE, PROP_NUMBER_OF_STATES, PROP_RELIABILITY,PROP_STATE_TEXT,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,
				PROP_ALARM_VALUES,PROP_TIME_DELAY_NORMAL,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,PROP_NOTIFY_TYPE,
				PROP_EVENT_TIME_STAMPS,PROP_FAULT_VALUES};

                for(i32count = 0; i32count < sizeof(ObjMSIProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjMSIProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_MSO
        case OBJECT_MULTI_STATE_OUTPUT :
            {
				int32_t ObjMSOProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE,
                PROP_OUT_OF_SERVICE, PROP_NUMBER_OF_STATES, PROP_PRIORITY_ARRAY, PROP_RELINQUISH_DEFAULT,
                PROP_RELIABILITY,PROP_STATE_TEXT,PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,PROP_FEEDBACK_VALUE,
                PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,PROP_NOTIFY_TYPE,PROP_TIME_DELAY_NORMAL,
                PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_TIME_STAMPS,PROP_CURRENT_COMMAND_PRIORITY
#ifdef BACDEL_PR18
				, PROP_CURRENT_COMMAND_PRIORITY
				#endif
				};


                for(i32count = 0; i32count < sizeof(ObjMSOProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjMSOProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_NC
        case OBJECT_NOTIFICATION_CLASS :
            {
                int32_t ObjNCProp[] = {PROP_DESCRIPTION, PROP_NOTIFICATION_CLASS, PROP_PRIORITY, PROP_ACK_REQUIRED, PROP_RECIPIENT_LIST,
                                       PROP_STATUS_FLAGS,PROP_EVENT_STATE,PROP_RELIABILITY,PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_ENABLE,
                                       PROP_ACKED_TRANSITIONS,PROP_NOTIFY_TYPE,PROP_EVENT_TIME_STAMPS};

                for(i32count = 0; i32count < sizeof(ObjNCProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjNCProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_SDL
        case OBJECT_SCHEDULE :
            {
                int32_t ObjSDLProp[] = {PROP_PRESENT_VALUE, PROP_DESCRIPTION, PROP_EFFECTIVE_PERIOD, PROP_WEEKLY_SCHEDULE, 
										PROP_SCHEDULE_DEFAULT, PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES, PROP_PRIORITY_FOR_WRITING,
										PROP_STATUS_FLAGS, PROP_RELIABILITY, PROP_OUT_OF_SERVICE,PROP_EXCEPTION_SCHEDULE};



                for(i32count = 0; i32count < sizeof(ObjSDLProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjSDLProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

#ifdef BACDEL_OBJ_MSV
        case OBJECT_MULTI_STATE_VALUE :
            {
				int32_t ObjMSVProp[] = {PROP_PRESENT_VALUE, PROP_STATUS_FLAGS, PROP_EVENT_STATE,PROP_OUT_OF_SERVICE, PROP_NUMBER_OF_STATES
#ifdef OPTIONAL_PROPERTY
										,PROP_DESCRIPTION,PROP_RELIABILITY,PROP_STATE_TEXT,
										PROP_TIME_DELAY,PROP_NOTIFICATION_CLASS,PROP_ALARM_VALUES,PROP_TIME_DELAY_NORMAL,
										PROP_EVENT_DETECTION_ENABLE,PROP_EVENT_ENABLE,PROP_ACKED_TRANSITIONS,PROP_NOTIFY_TYPE,
										PROP_EVENT_TIME_STAMPS,PROP_FAULT_VALUES,

										#endif
										};
                for(i32count = 0; i32count < sizeof(ObjMSVProp)/sizeof(int); i32count++)
                {
                    Init_Object_Property(ObjMSVProp[i32count], pVirtualDev,
                                        i8ArrayIndex, ui32ObjId, eObjectType, pvObject);
                }
            }
            break;
#endif

            default:
            break;

    }// switch ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Init_Object_All_Properties: Exit \r\n");
	#endif
}

/**
*DESCRIPTION
*   Generic Object create API. This API creates object of given object type.   
*
*@param eObjectType [in] Object Type
*@param pVirtualDev [in] Pointer to Virtual Device struct
*@param ui32ObjId   [in] Object Identifier for new object.
*@param pui8ObjName [in] Pointer to Object name.
*
*@return BACNET_RETURN_TYPE : success or error. 
*
*/
BACNET_RETURN_TYPE Create_Object(BACNET_OBJECT_TYPE eObjectType,
                   virtualDevData_t *pVirtualDev,
                   uint32_t  ui32ObjId, const char *pui8ObjName)   
{
	/* For size of object */
    ulong32_t ul32Size = 0 ;  
    /** Pointer to keep track of start of link list*/
    void   **pObjectStartIndex = NULL;

    void   *pObjectIndex = NULL;
    void   *pvNewObject = NULL;      /** Temp. Object pointer*/
    ulong32_t   ul32offset = 0;   /* Variable for offset */
	ulong32_t ul32baseaddr = 0;

    /* Duplicate object ID return address value */
    void    *pvDuplicateAdd = (void *)0x0001;
    uint32_t u32StrLen = 0;
    void    **pvObject = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Create_Object : Entry \r\n");
	#endif

    /* Get the object tpye size */
    GET_OBJECT_SIZE(eObjectType, ul32Size);

    /* Allocate New Object.*/
    pvNewObject = OSAL_Malloc(ul32Size,  __FILE__, __FUNCTION__, __LINE__);        
    /* Check allocated heap memory */
    if(pvNewObject == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:Create_Object :"
            "BACDEL_MALLOC_ERROR\r\n");
		#endif
        return BACDEL_MALLOC_ERROR;        
    }

	/* copy the pointer */
	ul32baseaddr = (ulong32_t)pvNewObject;

    /* Based on Object Type */
    switch(eObjectType)
    {
#ifdef BACDEL_OBJ_AI
        case OBJECT_ANALOG_INPUT:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstAnalogInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_AO
        case OBJECT_ANALOG_OUTPUT:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstAnalogOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_AV
        case OBJECT_ANALOG_VALUE:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstAnalogValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BI
        case OBJECT_BINARY_INPUT:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstBinaryInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BO
        case OBJECT_BINARY_OUTPUT:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstBinaryOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_BV
        case OBJECT_BINARY_VALUE:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstBinaryValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSI
        case OBJECT_MULTI_STATE_INPUT:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstMSInput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSO
        case OBJECT_MULTI_STATE_OUTPUT:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstMSOutput;
        }
        break;
#endif
#ifdef BACDEL_OBJ_MSV
        case OBJECT_MULTI_STATE_VALUE:
        {
            pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstMSValue;
        }
        break;
#endif
#ifdef BACDEL_OBJ_NC
        case OBJECT_NOTIFICATION_CLASS:
        {
			pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstNotificationClass; 
        }
		break;  
#endif
#ifdef BACDEL_OBJ_CAL
        case OBJECT_CALENDAR:
        {
			pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstCalendar; 
        }
		break;  
#endif
#ifdef BACDEL_OBJ_SDL
        case OBJECT_SCHEDULE:
        {
			pvObject = (void *)&pVirtualDev->m_stSupportedObjects.m_pstSchedule; 
        }
		break;  
#endif
		
        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Create_Object :"
                "Object Type Not Supported\r\n");
			#endif
            pvObject = NULL;
        }
    }/* End of Switch Case */

    if((NULL != pvObject) && (NULL == *pvObject))
    {
         pObjectStartIndex = pvObject;
        /* Assign new object to start of object list */
        *pObjectStartIndex = pvNewObject;

        /* As this is the first instance supproted, update the device object type 
         * supproted bit .
         */
		/* COMMENT: no need to updated this property as we have hardcoded value for 
		supported object types. 
		also we do not allow to add the objects that we don't support */
        //pVirtualDev->m_stDevObject.m_stObjectTypeSupported.m_stObjectSupport.u32Value 
        //    |= (1 << eObjectType);
     }
    else if(NULL != pvObject)
    {
       /* Sort the Object list. This API will return the new object pointer for 
       addition of required properties further */
        pObjectIndex = Sort_Object_List(eObjectType, ui32ObjId, pvObject, 
            pvNewObject);    

        pObjectStartIndex = &pObjectIndex;
        if(pvDuplicateAdd == pObjectIndex)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Create_Object :"
                "Duplicate Object\r\n");
			#endif
            OSAL_Free(pvNewObject,  __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_DUPLICATE_OBJECT;        
        }
    }
    else
    {
        /* If Object not supported then retrun ERROR */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:Create_Object :"
            "Object not Supported\r\n");
		#endif
		/* free the allocated memory */
		OSAL_Free(pvNewObject, __FILE__, __FUNCTION__, __LINE__);
		pvNewObject = NULL;
        return BACDEL_OBJECT_TYPE_ERROR;
    }

    /* Initialize the Object ID */
    GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_u32ObjId, ul32offset)
    memcpy((void *)(ul32baseaddr + ul32offset), &ui32ObjId, sizeof(uint32_t));


    /* Fill in the object Type in Object ID property */
    GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectID.m_eObjectType, ul32offset)
	memcpy((void *)(ul32baseaddr + ul32offset), &eObjectType, sizeof(uint16_t));
    

    /* Initialize object Name */
    GET_OBJCOMMON_OFFSET(eObjectType,   m_stObjName.m_stCHARString.m_pu8CharStr, ul32offset)
    u32StrLen = Strcopy((void *)(ul32baseaddr + ul32offset), (void *)pui8ObjName, MAX_CHARACTER_STRING_BYTES);  

    /* Initialize object Name String Length */
    GET_OBJCOMMON_OFFSET(eObjectType, m_stObjName.m_stCHARString.m_u32StrLen, ul32offset)
    memcpy((void *)(ul32baseaddr + ul32offset), (uint8_t *)&u32StrLen, sizeof(uint32_t));

	/* NOTE: enable this if object type is used as separate property in objects */
    /* Update the Object Type Property & Access Permission */
    GET_OBJCOMMON_OFFSET(eObjectType, m_stObjectType.m_eObjectType, ul32offset)
    memcpy((void *)(ul32baseaddr + ul32offset), &eObjectType, sizeof(BACNET_OBJECT_TYPE));

        /* Update the Device Object List */
        Update_Device_Object_List(pVirtualDev, eObjectType, ui32ObjId);

    /* Now Initialize the required property for created Object */
    Init_Object_All_Properties(eObjectType, pVirtualDev, 0, ui32ObjId,(const char *) pui8ObjName, pvNewObject);

#ifdef BACDEL_PR14
	/* position changed: fix for bug id: 5029 */
	/* initialize property list property for created Object */
	Init_Property_List(eObjectType, pvNewObject);
#endif

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Create_Object : Exit \r\n");
	#endif

    return BACDEL_SUCCESS;
}

/**
*DESCRIPTION
*   Generic Object delete API. This API deletes object of given object type.   
*
*@param eObjectType [in] Object Type
*@param pVirtualDev [in] Pointer to Virtual Device struct
*@param ui32ObjId - [in] Object Identifier.
*
*@return BACNET_RETURN_TYPE : success or error. 
*
*/
BACNET_RETURN_TYPE Remove_Object(BACNET_OBJECT_TYPE eObjectType,
                   virtualDevData_t *pVirtualDev,
                   uint32_t  ui32ObjId)
{
    /* For Data offset Addr */
    ulong32_t ul32OffsetAddr = 0 ; 
    /** Keep track of last element in the list */
    ulong32_t    ul32PreElement = 0;
    /** Keep track of last element in the list */
    void    *pvPreElement = NULL; 
	/** Keep track of Next element in the list */
    void    *pvNextElement = NULL;  
    /* Pointer to Element to be deleted from list */
    void    *pObjectIndex = NULL;   
    void    **ppvBasePointer = NULL;
    void    *pvNext = NULL; 
    /* For Base struct Addr */
    ulong32_t   ul32BaseAddr = 0;     
    ulong32_t   ul32NextOffset = 0;  
    void        **ppvNext = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Remove_Object : Entry \r\n");
	#endif

    ppvBasePointer = (void *)&(pVirtualDev->m_stSupportedObjects);

    /* Check if object list is present. If object list is present then search the 
    desired object */

    /* find the object */
    pObjectIndex = Traverse_Object_List(eObjectType, ui32ObjId, pVirtualDev, 
        (void *)&ul32PreElement);

    /* If object not found or its next element is null */
    if(pObjectIndex == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:Remove_Object :"
            "Traverse_Object_List Failed\r\n");
		#endif
        return BACDEL_OBJECT_NOT_PRESENT;        
    }    
    else
    {
		/* Free malloced properties of objects */ 
		Delete_Object_Properties(eObjectType, pObjectIndex);

        /* Take Previous element pointer for further processing */
        pvPreElement = (void *)ul32PreElement;

        /* Now take next element of object to be deleted in list */
        ul32BaseAddr = (ulong32_t )pObjectIndex;
		GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32OffsetAddr);  
        memcpy((void *)&ul32NextOffset, (void *)(ul32BaseAddr+ul32OffsetAddr),
        sizeof(ulong32_t));   
        /* Store the address of next element in the object list */
        pvNextElement  = (void *)ul32NextOffset;

        /* Get the Previous element next pointer & point it to Next Element */
        if((void *)pvPreElement == NULL)    
            /* This check is needed in case of first object to be deleted from list */
        {
            /* Set Next element as current element */
            ul32BaseAddr = (ulong32_t)ppvBasePointer;
            ppvBasePointer= (void *)(ul32BaseAddr+ul32OffsetAddr);
 			//commented to update 1st pointer in supported objects.
                *ppvBasePointer = pvNextElement;
        }
        else
        {
            /* Now take next element of object to be deleted in list */
            ul32BaseAddr = (ulong32_t )pvPreElement;
            /* Get the offset of Next Pointer */
			GET_OBJCOMMON_OFFSET(eObjectType, pstNext, ul32OffsetAddr);
            ppvNext = (void *)(ul32BaseAddr + ul32OffsetAddr);
            /* Connect the rest of link list to previous element of deleted object */  
            *ppvNext = pvNextElement;

            /* Now mark the Next pointer of Object to be removed as NULL */
            pvNext = pObjectIndex;
            pvNext = (void *)((ulong32_t)pvNext + ul32OffsetAddr);
            memset(pvNext, 0, sizeof(void *));
        }

        /* Free memeory for object */
        OSAL_Free(pObjectIndex,  __FILE__, __FUNCTION__, __LINE__);        
        pObjectIndex = NULL;

        /* Update the database revision */
        Update_Database_Revision(pVirtualDev, eObjectType, ui32ObjId);

		/* Update object count */
		pVirtualDev->m_stDevObject.m_stObjectIDList.m_u32ObjCount--;

		/* Update Object list - remove deleted object */
		Modify_Device_ObjectList(eObjectType, ui32ObjId, pVirtualDev);
    } 

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Remove_Object : Exit \r\n");
	#endif

	return BACDEL_SUCCESS;
}

/***
*DESCRIPTION
*   Free the dynamically added object properties.
*
*@param eObjectType [in] Object Type.
*@param pVirtualDev [in] Pointer to Virtual Device struct.
*
*@return None 
*
*/

void Delete_Object_Properties(BACNET_OBJECT_TYPE eObjectType, void *pvObjectAddr)
{
    /* local variables */

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Delete_Object_Properties : Entry \r\n");
	#endif

    /* check input pointer */
	if(NULL == pvObjectAddr)
	{
        /* null input pointer */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
						Delete_Object_Properties : null input pointer \r\n");
		#endif
		return;
	}
	
	/* check Object Whose Prop is to free*/
	switch(eObjectType)
	{
#ifdef BACDEL_OBJ_NC
		case OBJECT_NOTIFICATION_CLASS:
		{
            /* get address of NC object */
			Notificationclass_t	*pstNotificationClass = NULL;
			pstNotificationClass = pvObjectAddr;

            /* clear the list of values */
            Clear_Destination_List(&pstNotificationClass->m_stRecipientList.m_pstNCRecepient);
			break;
		}
#endif /* BACDEL_OBJ_NC */
		
#ifdef BACDEL_OBJ_SDL
		case OBJECT_SCHEDULE:
		{
            /* get address of schedule object */
			Schedule_t *pstSchedule = NULL;
			pstSchedule = pvObjectAddr;
            /* clear list of object prop ref property */
            Clear_DevObjPropReff_List(&pstSchedule->m_stListOfObjectPropertyReference.
                m_pstListOfBACnetDevObjPropReff);
            /* clear weekly schedule property */
            Clear_WeeklyShdl_List(&pstSchedule->m_stWeeklySchedule);
            /* clear exception schedule property */
            Clear_ExceptionShdl_List(&pstSchedule->m_stExceptionSchedule.m_pstSplEvent);
			break;
		}
#endif /* BACDEL_OBJ_SDL */

#ifdef BACDEL_OBJ_MSV

#endif /* BACDEL_OBJ_MSV */

#ifdef BACDEL_OBJ_MSI

#endif /* BACDEL_OBJ_MSI */

#ifdef BACDEL_OBJ_MSO

#endif /* BACDEL_OBJ_MS0 */

#ifdef BACDEL_OBJ_CAL
        case OBJECT_CALENDAR:
        {
            /* get address of calendar object */
            Calendar_t *ObjCalendar = NULL;
            ObjCalendar = pvObjectAddr;
            /* clear calendar entry list values */
            Clear_CalendarEntry_List(&ObjCalendar->m_stDateList.m_pstListOfCalendar);
        }
        break;
#endif /* BACDEL_OBJ_CAL */



        /* default case - do nothing */
		default:
			break;
	}// switch ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Delete_Object_Properties : Exit \r\n");
	#endif

    return;
}
