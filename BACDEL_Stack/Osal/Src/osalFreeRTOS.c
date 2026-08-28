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
*   SoftDEL Systems Ltd.                     india@softdel.com
*   3rd Floor, Pentagon P4,					 http://www.softdel.com
*	Magarpatta City, Hadapsar
*	Pune - 411 028
*

*
*   FILE
*	osalLinux.c
*
*   AUTHORS
*	Prashant Badgujar
*
*   DESCRIPTION
*	This file implements all linux system API's.
*
*   RELEASE HISTORY
*	DATE              	NAME					DESCRIPTION
*
*
******************************************************************************/

#include "osalFreeRTOS.h"
#include <string.h>

/*****************************************************************************/
// struct netbuf *buf;
// BACnet_Socket_Pkt_t g_stSocketPkt;

/*****************************************************************************/

void Osal_Sleep(int32_t i32time)
{
	osDelay(i32time);
}

/*****************************************************************************/
/**
*DESCRIPTION
*   The OSAL API Releases the Mutex acquired by process.
*
*@return 0 If the function succeeds, the return value is nonzero.
*/
int32_t Osal_Wait_Mutex(Mutex_H pMtxHandle, int32_t i32Time)
{
	/* function entry */
	return(osMutexAcquire(pMtxHandle,i32Time));

}


/*****************************************************************************/

/**
*DESCRIPTION
*   The OSAL API Releases the Mutex acquired by process.
*
*@return 0 If the function succeeds, the return value is nonzero.
*/
int32_t Osal_Release_Mutex(Mutex_H mutex_id)
{
	osStatus_t result = osOK;
	/* function entry */
	result=osMutexRelease(mutex_id);
	return result;

}
/*****************************************************************************/
/**
*DESCRIPTION
*   The OSAL API closes the Mutex acquired by process.
*
*@return 0 If the function succeeds, the return value is nonzero.
*/
int32_t Osal_Close_Mutex( Mutex_H pMtxHandle)
{
	osStatus_t result = osOK;
	result=osMutexDelete(pMtxHandle);
	return result;
	
}
/*****************************************************************************/
/**
*DESCRIPTION
*
*   The OSAL API releases Counting Semaphore value by i8Count
*
*@param psSemHandle [in] Semaphore Handle
*@param i8Count [in] Counter value to be decremented
*
*/
char OSAL_Release_Sem(Sem_H psSemHandle, uint8_t i8Count)
{
	osStatus_t result = osOK;
	result=osSemaphoreRelease(psSemHandle);
	if(result==osOK)
		result=1;
	else
		result=0;
	return result;
}
/*****************************************************************************/
/**
*DESCRIPTION
*
*   The OSAL API releases Counting Semaphore value by i8Count
*
*@param psSemHandle [in] Semaphore Handle
*@param i32Time [in] time value to block process
*
*/
int32_t OSAL_Wait_Sem(osSemaphoreId_t psSemHandle, uint32_t u32Time)
{
    if (osSemaphoreAcquire(psSemHandle, u32Time) == osOK)
        return 0;   // success
    else
        return -1;  // timeout or error
}
/*****************************************************************************/
/**
*DESCRIPTION
*
*   The OSAL API releases Counting Semaphore value by i8Count
*
*@param psSemHandle [in] Semaphore Handle
*@param i32Time [in] time value to block process
*
*/
bool OSAL_Close_Sem(Sem_H psSemHandle)
{
	osStatus_t result = osOK;
	result=osSemaphoreDelete(psSemHandle);
	if(result==osOK)
		result=1;
	else
		result=0;
	return result;
}
/*****************************************************************************/
/*
 **DESCRIPTION
 *  Suspends an active thread.
 *
 *@param hThread [in] Handle to the thread that needs to be suspended.
 *
 *@return DWORD Status Value.
 *
 */
DWORD OSAL_SuspendThread(Thread_H hThread)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackOsalLayer: OSAL_SuspendThread: Entry\r\n");
	#endif

	return 0;
}
/*****************************************************************************/
/**
*DESCRIPTION
*
*   The OSAL API terminate the thread
*
*@param Thread_H [in] Semaphore Handle
*
*/
bool Osal_Thread_Terminate(Thread_H pThreadTerminate)
{
	osStatus_t result = osOK;
	result=osThreadTerminate(pThreadTerminate);
	if(result==osOK)
		result=TRUE;
	else
		result=FALSE;
		
	return result;
}
/*****************************************************************************/

void *OSAL_Realloc(void *pvMemory, int32_t i32MemorySize, char *cFileName,
					const char *cFunctionName, int iLinenum)
{
		return NULL;
}
/*****************************************************************************/
/**
*DESCRIPTION
*   The OSAL API will allocate memory from the heap.
*
*@param i32MemorySize [in] Memory size to be allocated.
*@return Success: Pointer to the allocated memory.
*        Failed : NULL pointer.
*/
void *OSAL_Malloc(int32_t i32MemorySize, const char *cFileName, const char *cFunctionName,
                  int iLinenum)
{

    void *Allocated_Location;
	
	if(0== i32MemorySize)
	{
		return NULL;
	}

    /* Allocate memory of requested size */
	Allocated_Location = pvPortMalloc(i32MemorySize);
    if(Allocated_Location == NULL)
    {
        return NULL;
    }
    /* Initialize the allocated memory with 0 */
    memset(Allocated_Location,0,i32MemorySize);

    return Allocated_Location;
}

/*****************************************************************************/
/**
*DESCRIPTION
*   The OSAL API will deallocate assigned memory
*   pointed by the argument.
*
*@param pPointer [in] Pointer to the memory.
*/
void OSAL_Free(void *pvPointer, const char *cFileName, const char *cFunctionName,
               int iLinenum)
{

    /* Check for NULL pointer free */
    if(NULL == pvPointer)
        return;
    vPortFree(pvPointer);
	pvPointer = NULL;

}
/*****************************************************************************/

bool OSAL_Delete_Timer_Queue_Timer(osTimerId_t stQueueTimer)
{
	bool result = false;
	return result;
}
/*****************************************************************************/
void OSAL_Delete_Message_Queue(QueueHandle_t MsgQId)
{
	vQueueDelete(MsgQId);

}
/*****************************************************************************/

/*****************************************************************************/
