
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
*		osalLinux.h
*
*   AUTHORS
*		Harshal Mangale
*
*   DESCRIPTION
*		.
*
*   RELEASE HISTORY
*		DATE         NAME					DESCRIPTION
*
******************************************************************************/
/*
 * osalFreeRTOS.h
 *
 *  Created on: Mar 30, 2016
 *      Author: ritesh.jain
 */

#ifndef OSALFREERTOS_H_
#define OSALFREERTOS_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "cmsis_os.h"

#include "timers.h"
#include "task.h"
#include "stm32h5xx_hal.h"

typedef SemaphoreHandle_t  Sem_H;
typedef SemaphoreHandle_t  Event_H;
typedef SemaphoreHandle_t  Mutex_H;
typedef TimerHandle_t  	  TIMER_H;
typedef TaskHandle_t  	  Thread_H;
typedef SemaphoreHandle_t  sem_t;


typedef bool BOOL; 

#define TRUE   1
#define FALSE  0
typedef unsigned long DWORD;

/* Used for defining boolean type declarations */
typedef unsigned char   bool_t;     /* 1 byte  0 to 255 */
typedef unsigned char   uint8_t;    /* 1 byte  0 to 255 */
typedef unsigned short  uint16_t;   /* 2 bytes 0 to 65535 */
typedef signed char     int8_t;     /* 1 byte -127 to 127 */
typedef signed short    int16_t;    /* 2 bytes -32767 to 32767 */
typedef long            ilong32_t;  /* singed long declarations */
typedef unsigned long   ulong32_t;  /* unsinged long declarations */
typedef float           Float_t;    /* float declarations */
typedef double          Double_t;   /* double declarations */
typedef int             enum_t;     /* enumerated declarations */
typedef long 		Int64_t;    /* 8 bytes �9223372036854775808 to 9223372036854775807 */	//<$$>
typedef unsigned long  Uint64_t; /* 8 bytes 0 to 18446744073709551615 */	//<$$>


#define WAIT_OBJECT_0 0
#define WAIT_OBJECT_1 1
#define WAIT_OBJECT_2 2
#define WAIT_TIMEOUT -1
#define INFINITE      0xFFFFFFFF  // Infinite timeout

/* message Type for Message Queue*/
#define MSGQ_TYPE 1 		// message type, must be > 0
#define EXIT_MSG_QUEUE 2

#define SENDER_PORT_NUM 47808

typedef int Thread_ID;

typedef void*  Thread_API;
typedef void*  TIMER_CALLBACK;


typedef struct MemoryUsage    //<@>
{
    /* This variable will keep track of memory allocated through OSAL_Malloc API
    */
    int32_t    i32CurrMemoryUsage;
   /* This variable will keep track of memory allocated through OSAL_Malloc API
    */
    int32_t    i32PeakMemoryUsage;
}MemoryUsage_t;

extern MemoryUsage_t   stBACnetStackMemUsage; 

typedef struct
{

	int32_t i32AddressFamily;
	int32_t i32Type;
	int32_t i32Protocol;
	struct netconn *conn;
}BACnet_Socket_Pkt_t;

typedef struct
{

  int32_t   i32SocketFd;
	struct netconn *conn;
  int32_t   i32Level;
  int32_t   i32Optname;
  void *    pvOptval;
}BACnet_Socket_Option_t;

typedef struct
{

  int32_t   i32SocketFd;
	struct netconn *conn;
  struct sockaddr* pstName;
  int32_t i32Namelen;
	uint16_t port;
}BACnet_Socket_Bind_Pkt_t;



typedef struct Socket_Pkt
{
    int s;
	  int32_t   i32SocketFd;
		struct netconn *conn;
    int8_t *buf;
    int32_t len;
    int32_t flags;
    struct sockaddr_in *from;
}Socket_Pkt_t;


typedef struct Post_Thread_Msg
{
	long idThread;			/* Message queue ID */
	unsigned int  MsgType;  /* message type, must be > 0 */
	unsigned int  wParam;
	void* lParam;
}Post_Thread_Msg_t;

/** Structure for IPv4 address */
//#define s_addr s_addr;
#define s_host s_addr;
#define s_net s_addr;
#define s_imp s_addr;
#define s_impno s_addr;
#define s_lh s_addr;

/*********************************** Prototypes ******************************/

DWORD Osal_Get_Last_Error(void);
void Osal_Sleep(int32_t i32time);

int32_t Osal_Release_Mutex(Mutex_H pMtxHandle);
int32_t Osal_Wait_Mutex(Mutex_H pMtxHandle, int32_t i32Time);
int32_t Osal_Close_Mutex( Mutex_H pMtxHandle);

char OSAL_Release_Sem(Sem_H psSemHandle, uint8_t i8Count);
int32_t OSAL_Wait_Sem(osSemaphoreId_t psSemHandle, uint32_t u32Time);
bool OSAL_Close_Sem(Sem_H psSemHandle);

bool Osal_Thread_Terminate(Thread_H pThreadTerminate);
DWORD OSAL_SuspendThread(Thread_H hThread);

void *OSAL_Realloc(void *pvMemory, int32_t i32MemorySize, char *cFileName,
				   const char *cFunctionName, int iLinenum);
	   
void *OSAL_Malloc(int32_t i32MemorySize,  const char *cFileName, const char *cFunctionName,
                  int iLinenum);
void OSAL_Free(void *pvPointer,  const char *cFileName, const char *cFunctionName,
               int iLinenum);


int32_t OSAL_Create_Socket(BACnet_Socket_Pkt_t    *pstSocketPkt);
int32_t OSAL_Set_Socket_Option(BACnet_Socket_Option_t   *pstSocketOptionParam);
int32_t OSAL_Get_Socket_Option(BACnet_Socket_Option_t   *pstSocketOptionParam);
int32_t OSAL_Socket_Bind(BACnet_Socket_Bind_Pkt_t *pstSocketBindPkt);
int32_t OSAL_Socket_Recv(Socket_Pkt_t *pstSocket_Recv);
int32_t OSAL_Socket_Send(Socket_Pkt_t *pstSocket_Send);

bool OSAL_Post_Message(Post_Thread_Msg_t *pstPostThreadMsg);

void OSAL_Delete_Message_Queue(QueueHandle_t MsgQId);

bool OSAL_Delete_Timer_Queue_Timer(osTimerId_t stQueueTimer);

#endif /* OSALFREERTOS_H_ */
