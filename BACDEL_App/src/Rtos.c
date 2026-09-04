/**
 ******************************************************************************
 * File Name          : rtos.c
 * Date               :
 * Description        : Initialisation of RTOS related objects (tasks, semaphore, event,...)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rtos.h"

/* Includes ------------------------------------------------------------------*/
#include "Rtos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "objDevice.h"
#include "bacnetStackMgmt.h"
#include "bacDELApi.h"
#include "bacDELDeviceConfig.h"
#include "Tsk_Bacnet.h"
#include "objDevice.h"
#include "serviceCommomChangeOfValue_B.h"
#include "serviceEventNotification_B.h"

#ifdef SEGMENTATION_SUPPORTED
#include "bacnetInitiateServiceMgmt.h"
#include "SegmentationGenericHandler.h"
#endif

#ifdef BACDEL_OBJ_SDL
#include "serviceScheduling_B.h"
#endif

#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
#include "serviceEventReportingIntrinsic.h"
#endif
/* USER CODE END Includes */

#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
extern IntrinsicLinkList_t g_stIntrinsicReporting;
#endif

/* =================== Mutex Handles and Attributes =================== */

osMutexId_t m_hRxSegMtxLockHandle;
const osMutexAttr_t attr_m_hRxSegMtxLock = { .name = "RxSegMtx" };

osMutexId_t m_hCmplxAckMtxLockHandle;
const osMutexAttr_t attr_m_hCmplxAckMtxLock = { .name = "CmplxAckMtx" };

osMutexId_t m_hProcessDataQMtxLockHandle;
const osMutexAttr_t attr_m_hProcessDataQMtx = { .name = "ProcessDataQMtx" };

osMutexId_t m_hVirtualDevDataMtxLockHandle;
const osMutexAttr_t attr_m_hVirtualDevDataMtx = { .name = "VirtualDevMtx" };

osMutexId_t m_hInitMtxLockHandle;
const osMutexAttr_t attr_m_hInitMtx = { .name = "InitMtx" };

osMutexId_t g_hIDMtxLockHandle;
const osMutexAttr_t attr_g_hIDMtx = { .name = "IDMtx" };

osMutexId_t g_hDynDevAddrBindMtxHandle;
const osMutexAttr_t attr_g_hDynDevAddrBindMtx = { .name = "DynDevAddrBindMtx" };


osMutexId_t m_hcovsublistMtxLockHandle;
const osMutexAttr_t attr_m_hcovsublistMtx = { .name = "CovSublistMtx" };

osMutexId_t g_hsMstpTxMutexLockHandle;
const osMutexAttr_t attr_g_hsMstpTxMutexLock = { .name = "g_hsMstpTxMutexLock" };

const osMutexAttr_t attr_hMtxLock = { .name = "hMtxLock" };
const osMutexAttr_t attr_hMtxSavedNotification = { .name = "hMtxSavedNotification" };


/* =================== Thread Handles and Attributes =================== */
osThreadId_t Task_Transmit_THandle;
const osThreadAttr_t attr_Task_Transmit = {
		.name = "Task_transmit", .priority = osPriorityNormal, .stack_size = 2048*4
};

osThreadId_t Task_Process_ThHandle;
const osThreadAttr_t attr_TaskProcess = {
		.name = "IPProcessThread", .priority = osPriorityNormal, .stack_size = 3072*4
};

osThreadId_t Task_DvProcessTHandle;
const osThreadAttr_t attr_DvProcess = {
		.name = "IPDeviceThread", .priority = osPriorityNormal, .stack_size = 1024*4
};

osThreadId_t m_hCOVCancellationThreadHandle;
const osThreadAttr_t attr_COVCancel = {
		.name = "m_hCOVCancel", .priority = osPriorityNormal, .stack_size = 512*4
};

osThreadId_t AcknowledgementThreadHandle;
const osThreadAttr_t attr_Acknowledgement = {
		.name = "Acknowledgement", .priority = osPriorityNormal, .stack_size = 512*4
};

osThreadId_t Task_DvManagemeHandle;
const osThreadAttr_t attr_DvManagement = {
		.name = "Task_DvManageme", .priority = osPriorityNormal, .stack_size = 2048*4
};

osThreadId_t SegmentationThreadHandle;
const osThreadAttr_t attr_Segmentation = {
		.name = "m_Segmentation", .priority = osPriorityNormal, .stack_size = 2048*4
};

osThreadId_t Task_Initiate_THandle;
const osThreadAttr_t attr_Initiate = {
		.name = "Initiate_Task", .priority = osPriorityNormal, .stack_size = 2048*4
};

osThreadId_t dwThreadIDHandle;
const osThreadAttr_t attr_dwThreadID = {
		.name = "dwThreadID", .priority = osPriorityHigh, .stack_size = 2048*4
};

osSemaphoreId_t DvNotificationThreadHandle;
const osThreadAttr_t attr_DvNotification = {
		.name = "DvNotification", .priority = osPriorityAboveNormal, .stack_size = 2048*4
};

/*===================== Data sharing Test Handles====================*/

#ifdef BACDEL_SER_DS_RP_A
osThreadId_t Task_RP_A_Test_THandle;
const osThreadAttr_t attr_RP_A_Test = {
		.name = "RP_A_Test_Task", .priority = osPriorityLow, .stack_size = 1024*4
};
#endif

#ifdef BACDEL_SER_DS_WP_A
osThreadId_t Task_WP_A_Test_THandle;
const osThreadAttr_t attr_WP_A_Test = {
		.name = "WP_A_Test_Task", .priority = osPriorityLow, .stack_size = 1024*4
};
#endif

#ifdef BACDEL_SER_DS_RPM_A
osThreadId_t Task_RPM_A_Test_THandle;
const osThreadAttr_t attr_RPM_A_Test = {
		.name = "RPM_A_Test_Task", .priority = osPriorityLow, .stack_size = 1024*4
};
#endif
/*===================== Data sharing Test Handles====================*/


/*Event queue declarations*/
QueueHandle_t Cov_EventQueue;

/*semaphore  declarations*/
osSemaphoreId_t g_hPxSemaphoreHandle;
osSemaphoreId_t g_hTxSemaphoreHandle;
osSemaphoreId_t m_hDeviceSemaphoreHandle;
osSemaphoreId_t m_hDvMgmtSemaphoreHandle;
osSemaphoreId_t g_hInitiateSemaphoreHandle;
osSemaphoreId_t m_hCOVCancelSemaphoreHandle;
osSemaphoreId_t g_hAckSemaphoreHandle;
osSemaphoreId_t g_hLockSemaphoreHandle;
osSemaphoreId_t m_hSchdlTimerSemaphoreHandle;
osSemaphoreId_t g_hsMstpRxSemaphoreHandle;

#ifdef SEGMENTATION_SUPPORTED
osSemaphoreId_t g_hSegmentationSemaphoreHandle;
#endif

/*Timer declarations*/
osTimerId_t g_ClearInitiateQTimerHandle;
osTimerId_t g_ClearBBMD_FDT_TimerHandle;
osTimerId_t g_FDRegister_TimerHandle;
osTimerId_t g_DvManagementTimerHandle;
osTimerId_t g_Initiate_Service_Retry_TimerHandle;
osTimerId_t g_hMstpCountersTimerHandle;
osTimerId_t g_hMstpTsmTimerHandle;

#ifdef SEGMENTATION_SUPPORTED
osTimerId_t g_SegmentationQTimerHandle;
#endif

#ifdef BBMD_ENABLED
osTimerId_t g_hBBMDTimerHandle;
#endif

#ifdef BACDEL_OBJ_SDL
osTimerId_t g_ScheduleTimerHandle;
#endif

#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
osTimerId_t g_IntrinsicReportingQTimerHandle;
#endif

/* Variables -----------------------------------------------------------------*/
osThreadId_t SupervisorHandle;
osThreadId_t MesureHandle;
osThreadId_t COMHandle;
osThreadId_t Tsk_SliderHandle;
osThreadId_t CommunicationHandle;
osThreadId_t ModbusHandle;

// Semaphore
xSemaphoreHandle xComTXBufMutex = NULL;
xSemaphoreHandle xUsbTXBufMutex = NULL;
xSemaphoreHandle xWaitAnswerFromSLD = NULL;

/********************************************************************************/

void InitFreeRtos(void)
{
	/* Mutex Creation */
	   /* definition and creation of m_hProcessDataQMtxLock */
	m_hProcessDataQMtxLockHandle = osMutexNew(&attr_m_hProcessDataQMtx);
	   /* definition and creation of m_hVirtualDevDataMtxLock */
	m_hVirtualDevDataMtxLockHandle = osMutexNew(&attr_m_hVirtualDevDataMtx);
	   /* definition and creation of m_hInitMtxLock */
	m_hInitMtxLockHandle = osMutexNew(&attr_m_hInitMtx);
	   /* definition and creation of g_hIDMtxLock */
	g_hIDMtxLockHandle = osMutexNew(&attr_g_hIDMtx);
	   /* definition and creation of g_hDynDevAddrBindMtx */
	g_hDynDevAddrBindMtxHandle = osMutexNew(&attr_g_hDynDevAddrBindMtx);
	  
	   /* definition and creation of m_hcovsublistMtxLock */
	m_hcovsublistMtxLockHandle = osMutexNew(&attr_m_hcovsublistMtx);

	   /* definition and creation of m_hRxSegMtxLock & m_hCmplxAckMtxLock */
#ifdef SEGMENTATION_SUPPORTED
	m_hRxSegMtxLockHandle = osMutexNew(&attr_m_hRxSegMtxLock);

	m_hCmplxAckMtxLockHandle = osMutexNew(&attr_m_hCmplxAckMtxLock);
#endif

	   /* definition and creation of g_hsMstpTxMutexLock */
	    g_hsMstpTxMutexLockHandle = osMutexNew(&attr_g_hsMstpTxMutexLock);

	   /* definition and creation of g_stIntrinsicReporting.hMtxLock */
	   #if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)

	   g_stIntrinsicReporting.hMtxLock = osMutexNew(&attr_hMtxLock);
	   #endif

	   /* definition and creation of g_stSavedEventNotifications.hMtxSavedNotification */
	   #ifdef BACDEL_SER_AE_AA_B
	   g_stSavedEventNotifications.hMtxSavedNotification =  osMutexNew(&attr_hMtxSavedNotification);
	   #endif

	/* =================== Semaphore Creation =================== */
	   /* definition and creation of g_hPxSemaphore */
	g_hPxSemaphoreHandle = osSemaphoreNew(1, 1, NULL);
	   /* definition and creation of g_hTxSemaphore */
	g_hTxSemaphoreHandle = osSemaphoreNew(5, 5, NULL);
	   /* definition and creation of m_hDeviceSemaphore */
	m_hDeviceSemaphoreHandle = osSemaphoreNew(1, 1, NULL);
	   /* definition and creation of m_hDvMgmtSemaphore */
	m_hDvMgmtSemaphoreHandle = osSemaphoreNew(1, 1, NULL);
	   /* definition and creation of g_hInitiateSemaphore */
	g_hInitiateSemaphoreHandle = osSemaphoreNew(8, 8, NULL);
	   /* definition and creation of m_hCOVCancelSemaphore */
	m_hCOVCancelSemaphoreHandle = osSemaphoreNew(1, 1, NULL);



	   /* definition and creation of g_hSegmentationSemaphore */
#ifdef SEGMENTATION_SUPPORTED
	g_hSegmentationSemaphoreHandle = osSemaphoreNew(1, 1, NULL);
#endif

 /* definition and creation of g_hAcknowledgementSemaphore */
	g_hAckSemaphoreHandle = osSemaphoreNew(3, 3, NULL);
	
	   /* definition and creation of g_hLockSemaphoreHandle */
	g_hLockSemaphoreHandle = osSemaphoreNew(1, 1, NULL);

 /* definition and creation of m_hSchdlTimerSemaphore */	
#ifdef BACDEL_OBJ_SDL
	m_hSchdlTimerSemaphoreHandle = osSemaphoreNew(1, 1, NULL);
#endif
	   /* definition and creation of m_MstpRxSemaphore */

	   g_hsMstpRxSemaphoreHandle =osSemaphoreNew(1, 1, NULL);
	/* =================== Queue Creation =================== */
	Cov_EventQueue = xQueueCreate((UBaseType_t) 25, (UBaseType_t) sizeof(Post_Thread_Msg_t));

	/* =================== Timer Creation =================== */
	g_ClearInitiateQTimerHandle = osTimerNew(ClearInitiateQTimer_Timer, osTimerPeriodic, NULL, NULL);

	g_Initiate_Service_Retry_TimerHandle = osTimerNew(Initiate_Service_Retry_Timer, osTimerPeriodic, NULL, NULL);


	g_DvManagementTimerHandle = osTimerNew(DvManagementTimer, osTimerPeriodic, NULL, NULL);


#ifdef BACDEL_OBJ_SDL
	g_ScheduleTimerHandle = osTimerNew(SCHEDULE_TIMER, osTimerPeriodic, NULL, NULL);
#endif

#ifdef SEGMENTATION_SUPPORTED
	g_SegmentationQTimerHandle = osTimerNew(SegmentationQTimer_Timer, osTimerPeriodic, NULL, NULL);
#endif

	   #if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
	   g_IntrinsicReportingQTimerHandle = osTimerNew(IntrinsicReportingTimer, osTimerPeriodic, NULL,NULL);
	   #endif

	/* Thread Creation */
	  dwThreadIDHandle = osThreadNew(MSTP_Recieve_Thread, NULL,&attr_dwThreadID);

	  Task_Transmit_THandle = osThreadNew(Transmit_Thread_Task, NULL,&attr_Task_Transmit);
	   /* definition and creation of IPProcessThread */
	Task_Process_ThHandle = osThreadNew(Process_Thread_Task, NULL, &attr_TaskProcess);
	   /* definition and creation of IPDeviceThread */
	Task_DvProcessTHandle = osThreadNew(DvProcessThread_Task, NULL, &attr_DvProcess);
	   /* definition and creation of Task_Initiate_T */
	Task_Initiate_THandle = osThreadNew(Initiate_Thread_Task, NULL, &attr_Initiate);
	   /* definition and creation of Task_DvManageme */
	Task_DvManagemeHandle = osThreadNew(DvManagementThread, NULL, &attr_DvManagement);


	/*---------------------- Enable Threads for A-side Test ----------------------*/

#ifdef BACDEL_SER_DS_TEST
	/* definition and creation of Task_RP_A_Test - started after app_bacnet_init()
	    * (called earlier in main.c, before InitFreeRtos()) has registered the RP-A
	    * confirmed-ack handler and brought up the initiate queue/thread above */
#ifdef BACDEL_SER_DS_RP_A
	Task_RP_A_Test_THandle = osThreadNew(RP_A_Test_Thread, NULL, &attr_RP_A_Test);
#endif
#ifdef BACDEL_SER_DS_WP_A
	Task_WP_A_Test_THandle = osThreadNew(WP_A_Test_Thread, NULL, &attr_WP_A_Test);
#endif
#ifdef BACDEL_SER_DS_RPM_A
	Task_RPM_A_Test_THandle = osThreadNew(RPM_A_Test_Thread, NULL, &attr_RPM_A_Test);
#endif

#endif   /*  BACDEL_SER_DS_TEST  */
	/*----------------------------------------------------------------------------------------------*/

#if (defined BACDEL_SER_DS_COV_B || defined BACDEL_SER_AE_EN_B)
		 /* definition and creation of DvNotification */
	DvNotificationThreadHandle = osThreadNew(DvNotificationThread_Task, NULL, &attr_DvNotification);
		 /* definition and creation of DvNotification */
	m_hCOVCancellationThreadHandle = osThreadNew(DvCOV_B_Cancellation_Thread_Task, NULL, &attr_COVCancel);
		 /* definition and creation of DvNotification */
	AcknowledgementThreadHandle = osThreadNew(Acknowledgement_Thread_Task, NULL, &attr_Acknowledgement);
#endif

	   /* definition and creation of Segmentation */
#ifdef SEGMENTATION_SUPPORTED
	SegmentationThreadHandle = osThreadNew(Segmentation_Thread_Task, NULL, &attr_Segmentation);
#endif


	osTimerStart(g_ClearInitiateQTimerHandle,1);
	osTimerStart(g_Initiate_Service_Retry_TimerHandle,1000);
	osTimerStart(g_DvManagementTimerHandle,1000);

	/* Start 1000 msec g_ScheduleTimerHandle Timer  */
#ifdef BACDEL_OBJ_SDL
	osTimerStart(g_ScheduleTimerHandle,1000);
#endif

	/* Start 1000 msec g_IntrinsicReportingQTimerHandle Timer  */
#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
	osTimerStart(g_IntrinsicReportingQTimerHandle,1000);
#endif

	/* Start 1000 msec g_SegmentationQTimerHandle Timer  */
#ifdef SEGMENTATION_SUPPORTED
	osTimerStart(g_SegmentationQTimerHandle,1000);
#endif

	/* Start 1000 msec g_hBBMDTimerHandle Timer  */
#ifdef BBMD_ENABLED
	osTimerStart(g_hBBMDTimerHandle,1000);
#endif

}

//void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
//{
//	// Do something
//	while(1);
//}

