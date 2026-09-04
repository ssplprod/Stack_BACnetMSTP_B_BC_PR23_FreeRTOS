/*
 * Tsk_Bacnet.c
 */

#include "stdlib.h"

#include "Tsk_Bacnet.h"
//#include <ApplicationGPIO.h>
#include "bacnetStackMgmt.h"
#include "bacDELApi.h"
#include "objDevice.h"
#include "bacnetInitiateServiceMgmt.h"
#include "bacDELDeviceConfig.h"
#include "bacDELDef.h"
#include "datalinkMSTPStateMachines.h"

#include "datalinkMSTP.h"
#include "stm32h563xx.h"
#include <string.h>
#include "DefineStruct.h"
#include "DataB.h"

#include "main.h"
//-------------------------------------------------------------------
//	Global variables
//

struct netconn *conn = NULL;
//struct netbuf *buf;
//struct ip_addr *addr;
extern struct netif gnetif;
extern uint32_t enumToBaud(uint8_t Baudrate);
extern BACnet_Socket_Pkt_t g_stSocketPkt;
//extern ip4_addr_t ipaddr;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart2;
extern uint32_t gi32TimeDurationSec;
extern uint8_t gu8CommStatus;
extern bool gbDisableInitiation;
bool gbCommControl = FALSE;	/** Communication Control flag*/
extern osSemaphoreId_t g_hsMstpRxSemaphoreHandle;
extern BACnetMstpVariables_t g_stMstpVariables;
extern osMutexId_t g_hsMstpTxMutexLockHandle;
extern DB_t SMCfg; //Added By SoftDEL
extern struct netif gnetif;
//-------------------------------------------------------------------
//	Local variables
//
extern volatile uint8_t u8BACnetRxByte;
devObject_t   gstDeviceObject;
bool gbDeviceStatus = TRUE;			               /** Provides the DCC service check*/
extern uint32_t gui32StartupDelay;

void MSTP_Recieve_Thread(void const * argument);
void Transmit_Thread(void const * argument);
void Initiate_Retry_Timer(void const * argument);
void Test_API(void);

typedef struct DT {
	/* Year value */
	uint16_t	m_u16Year;      /* AD */
	/* Month value */
	uint8_t		m_u8Month;      /* 1=Jan */
	/* Day value */
	uint8_t		m_u8Day;        /* 1..31 */
	/* Weekday value */
	uint8_t		m_u8Wday;       /* 1=Monday-7=Sunday */
	/* Hour value */
	uint8_t		m_u8Hour;
	/* Minutes value */
	uint8_t		m_u8Min;
	/* Second value */
	uint8_t		m_u8Sec;
	/* Hundredths value */
	uint8_t		m_u8Hundredths;

}DT_t;
DT_t dt={0,1,1,1,0,0,0,0};

Pr_BACnetConfigData_t BACnetConfigData;
Pr_BACnetConfigData_t *GetpBACnetfg()
{
	return &BACnetConfigData;
}

/**                                                                         
*
* DESCRIPTION
* Function to receive auto response for RP service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_RP
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* return value */
	return eRetVal;
}


/**                                                                         
*
* DESCRIPTION
* Function to receive auto response for WP service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_WP
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* return value */
	return eRetVal;
}
/**
*
* DESCRIPTION
* Function to receive auto response for RPM service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_RPM
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceResp)
	{
		return BACDEL_ERROR;
	}

	/* Write application code here*/


	/* return value */
	return eRetVal;
}
#ifdef BACDEL_SER_DM_RD_B
/**
*
* DESCRIPTION
* Function to receive auto response for Reinitialize device service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_RD
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;


	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* Write application code here*/

	/* return value */
	return eRetVal;
}

#endif
/**
*
* DESCRIPTION
* Function to receive auto response for DCC service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_DCC
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* Write application code here*/


	/* return value */
	return eRetVal;
}

/**                                                                         
*
* DESCRIPTION
* Function to receive auto response for TimeSync. service request.
*
*/
void RTC_Read_Callback(BACnetDate_t *pstLocalDate, BACnetTime_t *pstLocalTime)
{

	pstLocalDate->m_u8Day				= dt.m_u8Day;			/* store date */
	pstLocalDate->m_u8Wday				= dt.m_u8Wday;			/* store week day */
	pstLocalDate->m_u8Month				= dt.m_u8Month;			/* store month */
	pstLocalDate->m_u16Year		 		= dt.m_u16Year;			/* store year */

	pstLocalTime->m_u8Hour		 		= dt.m_u8Hour;			/* store hours */
	pstLocalTime->m_u8Min		 		= dt.m_u8Min;			/* store minutes */
	pstLocalTime->m_u8Sec		 		= dt.m_u8Sec;			/* store seconds */
	pstLocalTime->m_u8Hundredths		= dt.m_u8Hundredths;	/* system does not return milliseconds so bacnet default is X'FF */
}


BACNET_RETURN_TYPE init_sld_bacnet_objects()
{
	BACNET_RETURN_TYPE ret = BACDEL_SUCCESS;

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_ANALOG_INPUT,0,(const char *)"My Analog i/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_ANALOG_OUTPUT,0,(const char *)"My Analog o/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_ANALOG_VALUE,0,(const char *)"My Analog val 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_BINARY_INPUT,0,(const char *)"My Binary i/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_BINARY_OUTPUT,0,(const char  *)"My Binary o/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_BINARY_VALUE,0,(const char *)"My Binary val 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_MULTI_STATE_INPUT,0,(const char *)"My MSI 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_MULTI_STATE_OUTPUT,0,(const char *)"My MSO 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_MULTI_STATE_VALUE,0,(const char *)"My MSV 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_CALENDAR,0,(const char *)"My CAL 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_NOTIFICATION_CLASS,0,(const char *)"My NC 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_SCHEDULE,0,(const char *)"My SCH 0");


	/******************** Network Port Object for MSTP *******************/
	BACnetCharStr_t stObjName = {0};
	memcpy(&stObjName.m_pu8CharStr[0], "BACnet/MSTP Network Port", 25);
	stObjName.m_u32StrLen = 25;
	NetworkPort_Add_Object(SMCfg.g_BACnetCfg.ObjID,0, &stObjName, NETWORK_TYPE_MSTP);

	return ret;
}
void app_bacnet_init()
{
	BACNET_RETURN_TYPE ret = BACDEL_SUCCESS;

	uint8_t  uwBacMacAddr = SMCfg.g_BACnetCfg.ComID;
	uint32_t uwBacBaudRate = ebr_76800;//enumToBaud(SMCfg.g_Com485Cfg.Baudrate);
	BACDEL_MSTP_Stack_Init(uwBacMacAddr,uwBacBaudRate,0 );

	Init_Service_Handlers();

	BACDEL_App_RTC_CallBack_Register(APP_CB_RTC_READ,RTC_Read_Callback);

	ret &= BACDEL_Add_Device(0,SMCfg.g_BACnetCfg.ObjID,0);

	ret &= init_sld_bacnet_objects();

	BACDEL_App_CallBack_Register(APP_CB_READ_PROPERTY,AutoResponse_RP);
	BACDEL_App_CallBack_Register(APP_CB_WRITE_PROPERTY,AutoResponse_WP);
	BACDEL_App_CallBack_Register(APP_CB_TIME_SYNC,AutoResponse_TimeSync);
	BACDEL_App_CallBack_Register(APP_CB_UTC_TIME_SYNC,AutoResponse_UTC_TimeSync);

	BACDEL_App_CallBack_Register(APP_CB_READ_PROPERTY_MULTIPLE,AutoResponse_RPM);
	BACDEL_App_CallBack_Register(APP_CB_REINITIALIZE_DEVICE,AutoResponse_RD);
	BACDEL_App_CallBack_Register(APP_CB_DEVICE_COMMUNICATION_CONTROL,AutoResponse_DCC);
}

/*================================== Data sharing services Test =====================================================*/

#if (defined BACDEL_SER_DS_RP_A) || (defined BACDEL_SER_DS_RPM_A)
/**
 * DESCRIPTION
 * Very small, best-effort printer for a decoded RP-A/RPM-A property value.
 * Only handles the common simple/primitive tag types (matching the request's
 * "keep it simple" scope) - richer per-type formatting (dates, object lists,
 * etc.) can be added later by extending this switch, see BACApp_Decode_Data_Type()
 * in pduDataEncodeDecode.c for the full set of BACNET_DATA_TYPE variants and
 * their corresponding Pr_BACnetXxx_t decode structures.
 */
static void A_Side_Test_Print_Value(BACNET_DATA_TYPE eDataType, void *pvPropVal)
{
	if(NULL == pvPropVal)
	{
		Debug_LogFmt("A-Side Test: value = <none/empty> (data type=%d)\r\n", (int)eDataType);
		return;
	}

	switch(eDataType)
	{
	case BACNET_DT_BOOLEAN:
		Debug_LogFmt("A-Side Test: value (bool) = %u\r\n",
			(unsigned)((Pr_BACnetBool_t *)pvPropVal)->m_bVal);
		break;

	case BACNET_DT_UNSIGNED:
	case BACNET_DT_UNSIGNED32:
	case BACNET_DT_UNSIGNED16:
		Debug_LogFmt("A-Side Test: value (unsigned) = %lu\r\n",
			(unsigned long)((Pr_BACnetUnsigned32_t *)pvPropVal)->m_u32Val);
		break;

	case BACNET_DT_INTEGER:
		Debug_LogFmt("A-Side Test: value (signed) = %ld\r\n",
			(long)((Pr_BACnetSigned32_t *)pvPropVal)->m_i32Val);
		break;

	case BACNET_DT_ENUM:
		Debug_LogFmt("A-Side Test: value (enumerated) = %lu\r\n",
			(unsigned long)((Pr_BinaryEnumPV_t *)pvPropVal)->m_eVal);
		break;

	case BACNET_DT_REAL:
	{
		/* manual fixed-point style print - avoids depending on the
		 * newlib-nano float printf formatter being linked in */
		float fVal = ((Pr_BACnetReal_t *)pvPropVal)->m_fVal;
		int32_t i32Whole = (int32_t)fVal;
		int32_t i32Frac = (int32_t)((fVal - (float)i32Whole) * 1000.0f);
		if(i32Frac < 0)
		{
			i32Frac = -i32Frac;
		}
		Debug_LogFmt("A-Side Test: value (real) = %ld.%03ld\r\n", (long)i32Whole, (long)i32Frac);
	}
	break;

	case BACNET_DT_CHARSTRING:
	{
		/* Pr_BACnetCharStr_t -> BACnetCharStr_t m_stCHARString - a fixed-size
		 * embedded buffer (m_pu8CharStr[MAX_CHARACTER_STRING_BYTES]) with an
		 * explicit length (m_u32StrLen), NOT NUL-terminated - print exactly
		 * m_u32StrLen bytes via %.*s, clamped to the buffer size for safety. */
		BACnetCharStr_t *pstStr = &((Pr_BACnetCharStr_t *)pvPropVal)->m_stCHARString;
		uint32_t u32Len = pstStr->m_u32StrLen;
		if(u32Len > MAX_CHARACTER_STRING_BYTES)
		{
			u32Len = MAX_CHARACTER_STRING_BYTES;
		}
		Debug_LogFmt("A-Side Test: value (string) = \"%.*s\"\r\n",
			(int)u32Len, (const char *)pstStr->m_pu8CharStr);
	}
	break;

	case BACNET_DT_OBJECTID:
		Debug_LogFmt("A-Side Test: value (object id) = type=%lu instance=%lu\r\n",
			(unsigned long)((Pr_BACnetObjId_t *)pvPropVal)->m_eObjectType,
			(unsigned long)((Pr_BACnetObjId_t *)pvPropVal)->m_u32ObjId);
		break;

	default:
		/* string / date / time / composite types - not decoded here, see
		 * comment above. */
		Debug_LogFmt("A-Side Test: value not printed (data type=%d, add a case in "
			"A_Side_Test_Print_Value() if needed)\r\n", (int)eDataType);
		break;
	}
}
#endif /* BACDEL_SER_DS_RP_A || BACDEL_SER_DS_RPM_A */

#ifdef BACDEL_SER_DS_RP_A
/*
 * ---------------------------------------------------------------------------
 * RP-A (ReadProperty client) test harness.
 *
 * This is a minimal application-level exerciser for the newly ported RP-A
 * service: it periodically sends a ReadProperty request to a hard-coded
 * MS/TP MAC address and logs whatever comes back (value, error, or timeout).
 *
 * CHANGE THESE BEFORE USING ON A REAL BUS:
 *   - RP_A_TEST_DEST_MAC   : must be the MS/TP MAC of an actual device on the
 *                            bus (this device's own MAC is SMCfg.g_BACnetCfg.ComID,
 *                            which DataB.c sets to 5 - pick something else).
 *   - RP_A_TEST_OBJ_TYPE/INSTANCE/PROPERTY : arbitrary standard object/property,
 *                            defaulted here to Device-0 / Object_Name.
 * ---------------------------------------------------------------------------
 */
#define RP_A_TEST_DEST_MAC			9u			/* placeholder - some other device's MSTP MAC, distinct from our own (5) */
#define RP_A_TEST_OBJ_TYPE			OBJECT_ANALOG_OUTPUT     //OBJECT_DEVICE
#define RP_A_TEST_OBJ_INSTANCE		0u
#define RP_A_TEST_PROP_ID			PROP_PRESENT_VALUE   	//PROP_OBJECT_NAME

/* value printer used by this callback has moved above the
 * #ifdef BACDEL_SER_DS_RP_A guard (still named A_Side_Test_Print_Value) so
 * the RPM-A test callback below can reuse it too. */

/**
 * DESCRIPTION
 * Response callback for the test RP-A request - matches the App_Callback_Interface_t
 * signature expected by BACDEL_Send_RP(). The initiate framework
 * (bacnetInitiateServiceMgmt.c/bacnetInitiateServiceCommon.c) calls this directly
 * when the complex-ack/error/reject/abort/timeout for our request is processed -
 * there is no separate APP_CB_* registration for A-side confirmed-service
 * responses (that mechanism is only used for B-side/server callbacks); the
 * per-request function pointer passed to BACDEL_Send_RP() *is* the hook.
 */
BACNET_RETURN_TYPE RP_A_Test_Response_Callback(bacnetip_arguments_t *pstServiceArgs, uint8_t u8Reason)
{
	rp_response_t *pstRpResp = NULL;

	if(NULL == pstServiceArgs)
	{
		Debug_LogFmt("RP-A Test: response callback got NULL service args\r\n");
		return BACDEL_ERROR;
	}

	if(STATE_TIMEOUT == (INITIATE_SERVICE_STATE)u8Reason)
	{
		Debug_LogFmt("RP-A Test: request to MAC=%u timed out - no response\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0]);
		return BACDEL_SUCCESS;
	}

	if(PDU_TYPE_COMPLEX_ACK != pstServiceArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
	{
		/* Error/Abort/Reject - Fill_BacnetIP_Arguments() already decoded this into
		 * m_stServiceChoice.m_stErrorResponse/m_stAbortResponse/m_stRejectResponse */
		Debug_LogFmt("RP-A Test: request to MAC=%u failed, pdu type=%d (see "
			"m_stErrorResponse/m_stAbortResponse/m_stRejectResponse for details)\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0],
			(int)pstServiceArgs->m_stNPDUData.m_stAPDUData.m_ePduType);
		return BACDEL_SUCCESS;
	}

	pstRpResp = &(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stRP_Response);

	Debug_LogFmt("RP-A Test: response from MAC=%u  ObjType=%u ObjInst=%lu PropId=%u\r\n",
		(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0],
		(unsigned)pstRpResp->m_eObjectType,
		(unsigned long)pstRpResp->m_u32ObjectInstance,
		(unsigned)pstRpResp->m_eObjectProperty);

	A_Side_Test_Print_Value(pstRpResp->m_eDataType, pstRpResp->m_pvPropVal);

	/* free the decoded value allocated by BACApp_Decode_Data_Type() (via
	 * Decode_A_Side_Data() in Fill_BacnetIP_Arguments()) - mirrors how
	 * Free_Argument_Memory() frees the equivalent RPM-A linked-list entries */
	Clear_PropVal_AsPer_DataType(pstRpResp->m_eDataType, &pstRpResp->m_pvPropVal);

	return BACDEL_SUCCESS;
}

/**
 * DESCRIPTION
 * Builds and sends one RP-A test request to RP_A_TEST_DEST_MAC for
 * RP_A_TEST_OBJ_TYPE/INSTANCE/PROPERTY. Called periodically from
 * RP_A_Test_Thread().
 */
void RP_A_Test_Request(void)
{
	rp_request_t stRpReqData = {0};
	BACnetAddress_t stDestAddr = {0};
	int32_t i32TokenId = -1;
	static uint32_t s_u32RequestId = 0;
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;

	/* destination address - raw MS/TP MAC, no device-ID binding required.
	 * This exact struct (via Copy_Destination_Parameter's full-struct copy)
	 * ends up in pstReqParam->m_stRmDvAddr, which Acknowledgement_Thread
	 * later memcmp()'s whole-struct against the incoming response's decoded
	 * source address to match it back to this request. MSTP_Decode_Handler
	 * (datalinkMSTPHandler.c, RX side) only ever sets u8IpAddrs[0] and
	 * u8mac_len=MSTP_ADDRESS_LEN on that decoded address - it never touches
	 * u8dlen, u16net, or u8DvDadr, which therefore come out as 0. So this
	 * struct must be filled the exact same way, or the memcmp never matches
	 * and every response gets silently dropped as "no matching request". */
	stDestAddr.u8mac_len = MSTP_ADDRESS_LEN;
	stDestAddr.u8dlen = 0;
	stDestAddr.u16net = 0;						/* local MS/TP segment, no routing */
	stDestAddr.u8IpAddrs[0] = RP_A_TEST_DEST_MAC;

	stRpReqData.m_eObjectType = RP_A_TEST_OBJ_TYPE;
	stRpReqData.m_u32ObjectInstance = RP_A_TEST_OBJ_INSTANCE;
	stRpReqData.m_eObjectProperty = RP_A_TEST_PROP_ID;
	stRpReqData.m_u32ArrayIndex = 0;
	stRpReqData.m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;

	eReturnType = BACDEL_Send_RP(
		SMCfg.g_BACnetCfg.ObjID,			/* our own device instance (source) */
		0,									/* unused - bDestTypeFlag is FALSE */
		&stDestAddr,
		FALSE,								/* address by MAC, not by device ID */
		&stRpReqData,
		RP_A_Test_Response_Callback,
		&i32TokenId,
		s_u32RequestId++);

	if(BACDEL_SUCCESS != eReturnType)
	{
		Debug_LogFmt("RP-A Test: BACDEL_Send_RP() to MAC=%u failed, err=%d\r\n",
			(unsigned)RP_A_TEST_DEST_MAC, (int)eReturnType);
	}
	else
	{
		Debug_LogFmt("RP-A Test: request sent to MAC=%u, token=%ld\r\n",
			(unsigned)RP_A_TEST_DEST_MAC, (long)i32TokenId);
	}
}

/* RP_A_Test_Thread function - periodic caller for RP_A_Test_Request(),
 * started from Rtos.c's InitFreeRtos() (mirrors DvManagementThread's
 * for(;;){...; osDelay();} pattern) */
void RP_A_Test_Thread(void const * argument)
{
	for(;;)
	{
		RP_A_Test_Request();
		osDelay(5000);
	}
}
#endif /* BACDEL_SER_DS_RP_A */

#ifdef BACDEL_SER_DS_WP_A
/*
 * ---------------------------------------------------------------------------
 * WP-A (WriteProperty client) test harness.
 *
 * Periodically writes a REAL value to WP_A_TEST_OBJ_TYPE/INSTANCE/PROPERTY
 * on WP_A_TEST_DEST_MAC and logs the simple-ack/error/timeout outcome (a
 * successful WriteProperty response carries no data - see the comment at
 * the top of serviceWriteProperty_A.c).
 *
 * CHANGE THESE BEFORE USING ON A REAL BUS - see the RP-A test harness above
 * for the same caveats (destination MAC, object/property must exist and,
 * for PROP_PRESENT_VALUE, be commandable/writable on the target device).
 * ---------------------------------------------------------------------------
 */
#define WP_A_TEST_DEST_MAC			9u			/* placeholder - some other device's MSTP MAC, distinct from our own (5) */
#define WP_A_TEST_OBJ_TYPE			OBJECT_ANALOG_VALUE
#define WP_A_TEST_OBJ_INSTANCE		1u
#define WP_A_TEST_PROP_ID			PROP_PRESENT_VALUE
#define WP_A_TEST_VALUE				72.5f

/**
 * DESCRIPTION
 * Response callback for the test WP-A request - matches the
 * App_Callback_Interface_t signature expected by BACDEL_Send_WP(). Unlike
 * RP-A, a successful response is a Simple-Ack (no service data to print).
 */
BACNET_RETURN_TYPE WP_A_Test_Response_Callback(bacnetip_arguments_t *pstServiceArgs, uint8_t u8Reason)
{
	if(NULL == pstServiceArgs)
	{
		Debug_LogFmt("WP-A Test: response callback got NULL service args\r\n");
		return BACDEL_ERROR;
	}

	if(STATE_TIMEOUT == (INITIATE_SERVICE_STATE)u8Reason)
	{
		Debug_LogFmt("WP-A Test: request to MAC=%u timed out - no response\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0]);
		return BACDEL_SUCCESS;
	}

	if(PDU_TYPE_SIMPLE_ACK == pstServiceArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
	{
		Debug_LogFmt("WP-A Test: write to MAC=%u succeeded (simple-ack)\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0]);
	}
	else
	{
		/* Error/Abort/Reject - Fill_BacnetIP_Arguments() already decoded this into
		 * m_stServiceChoice.m_stErrorResponse/m_stAbortResponse/m_stRejectResponse */
		Debug_LogFmt("WP-A Test: write to MAC=%u failed, pdu type=%d (see "
			"m_stErrorResponse/m_stAbortResponse/m_stRejectResponse for details)\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0],
			(int)pstServiceArgs->m_stNPDUData.m_stAPDUData.m_ePduType);
	}

	return BACDEL_SUCCESS;
}

/**
 * DESCRIPTION
 * Builds and sends one WP-A test request to WP_A_TEST_DEST_MAC for
 * WP_A_TEST_OBJ_TYPE/INSTANCE/PROPERTY. Called periodically from
 * WP_A_Test_Thread().
 */
void WP_A_Test_Request(void)
{
	wp_request_t stWpReqData = {0};
	/* static: WP_A_Request_Encoder() dereferences m_pvPropVal later, from
	 * the Initiate thread, well after this function has returned - a
	 * stack-local value here would be a dangling pointer by then. */
	static Pr_BACnetReal_t s_stWpTestValue = {0};
	BACnetAddress_t stDestAddr = {0};
	int32_t i32TokenId = -1;
	static uint32_t s_u32RequestId = 0;
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;

	/* destination address - raw MS/TP MAC, see RP_A_Test_Request() for why
	 * every field here must be filled exactly this way */
	stDestAddr.u8mac_len = MSTP_ADDRESS_LEN;
	stDestAddr.u8dlen = 0;
	stDestAddr.u16net = 0;
	stDestAddr.u8IpAddrs[0] = WP_A_TEST_DEST_MAC;

	s_stWpTestValue.m_fVal = WP_A_TEST_VALUE;

	stWpReqData.m_eObjectType = WP_A_TEST_OBJ_TYPE;
	stWpReqData.m_u32ObjectInstance = WP_A_TEST_OBJ_INSTANCE;
	stWpReqData.m_eObjectProperty = WP_A_TEST_PROP_ID;
	stWpReqData.m_u32ArrayIndex = 0;
	stWpReqData.m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;
	stWpReqData.m_u32Priority = BACNET_NO_PRIORITY;
	#ifdef NEW_RP_WP_INTERFACE
	stWpReqData.m_eDataType = BACNET_DT_REAL;
	stWpReqData.m_pvPropVal = &s_stWpTestValue;
	#endif

	eReturnType = BACDEL_Send_WP(
		SMCfg.g_BACnetCfg.ObjID,			/* our own device instance (source) */
		0,									/* unused - bDestTypeFlag is FALSE */
		&stDestAddr,
		FALSE,								/* address by MAC, not by device ID */
		&stWpReqData,
		WP_A_Test_Response_Callback,
		&i32TokenId,
		s_u32RequestId++);

	if(BACDEL_SUCCESS != eReturnType)
	{
		Debug_LogFmt("WP-A Test: BACDEL_Send_WP() to MAC=%u failed, err=%d\r\n",
			(unsigned)WP_A_TEST_DEST_MAC, (int)eReturnType);
	}
	else
	{
		Debug_LogFmt("WP-A Test: request sent to MAC=%u, token=%ld\r\n",
			(unsigned)WP_A_TEST_DEST_MAC, (long)i32TokenId);
	}
}

/* WP_A_Test_Thread function - periodic caller for WP_A_Test_Request(),
 * started from Rtos.c's InitFreeRtos() (mirrors RP_A_Test_Thread's pattern) */
void WP_A_Test_Thread(void const * argument)
{
	for(;;)
	{
		WP_A_Test_Request();
		osDelay(5000);
	}
}
#endif /* BACDEL_SER_DS_WP_A */

#ifdef BACDEL_SER_DS_RPM_A
/*
 * ---------------------------------------------------------------------------
 * RPM-A (ReadPropertyMultiple client) test harness.
 *
 * Periodically requests two properties (RPM_A_TEST_PROP_ID_1/2) of
 * RPM_A_TEST_OBJ_TYPE/INSTANCE from RPM_A_TEST_DEST_MAC in a single request
 * and logs each decoded value/error in the response.
 *
 * CHANGE THESE BEFORE USING ON A REAL BUS - see the RP-A test harness above
 * for the same caveats.
 * ---------------------------------------------------------------------------
 */
#define RPM_A_TEST_DEST_MAC			9u			/* placeholder - some other device's MSTP MAC, distinct from our own (5) */
#define RPM_A_TEST_OBJ_TYPE			OBJECT_DEVICE
#define RPM_A_TEST_OBJ_INSTANCE		0u
#define RPM_A_TEST_PROP_ID_1			PROP_OBJECT_NAME
#define RPM_A_TEST_PROP_ID_2			PROP_MODEL_NAME

/**
 * DESCRIPTION
 * Response callback for the test RPM-A request - matches the
 * App_Callback_Interface_t signature expected by BACDEL_Send_RPM(). Walks
 * the decoded rpm_response_t list (head plus m_pstRPMNextElem chain) built
 * by Fill_BacnetIP_Arguments(), then releases it: the head node is a
 * value-copy embedded directly in pstServiceArgs (nothing to free), while
 * nodes 2..N are individually malloc'd and released via
 * Free_Argument_Memory() - the same "second element onward" convention
 * RPM_B_Free_Service_Data() already documents for the B-side.
 */
BACNET_RETURN_TYPE RPM_A_Test_Response_Callback(bacnetip_arguments_t *pstServiceArgs, uint8_t u8Reason)
{
	rpm_response_t *pstRpmResp = NULL;

	if(NULL == pstServiceArgs)
	{
		Debug_LogFmt("RPM-A Test: response callback got NULL service args\r\n");
		return BACDEL_ERROR;
	}

	if(STATE_TIMEOUT == (INITIATE_SERVICE_STATE)u8Reason)
	{
		Debug_LogFmt("RPM-A Test: request to MAC=%u timed out - no response\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0]);
		return BACDEL_SUCCESS;
	}

	if(PDU_TYPE_COMPLEX_ACK != pstServiceArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
	{
		/* Error/Abort/Reject - Fill_BacnetIP_Arguments() already decoded this into
		 * m_stServiceChoice.m_stErrorResponse/m_stAbortResponse/m_stRejectResponse */
		Debug_LogFmt("RPM-A Test: request to MAC=%u failed, pdu type=%d (see "
			"m_stErrorResponse/m_stAbortResponse/m_stRejectResponse for details)\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0],
			(int)pstServiceArgs->m_stNPDUData.m_stAPDUData.m_ePduType);
		return BACDEL_SUCCESS;
	}

	pstRpmResp = &(pstServiceArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stRPM_Response);

	while(NULL != pstRpmResp)
	{
		Debug_LogFmt("RPM-A Test: response from MAC=%u  ObjType=%u ObjInst=%lu PropId=%u\r\n",
			(unsigned)pstServiceArgs->m_stDestBACnetAddr.u8IpAddrs[0],
			(unsigned)pstRpmResp->m_eObjectType,
			(unsigned long)pstRpmResp->m_u32ObjectInstance,
			(unsigned)pstRpmResp->m_eObjectProperty);

		/* -1/-1 is the "no error" sentinel (see Fill_BacnetIP_Arguments()'s
		 * SERVICE_SUPPORTED_READ_PROP_MULTIPLE case) */
		if(-1 == (int16_t)pstRpmResp->m_eErrorClass && -1 == (int16_t)pstRpmResp->m_eErrorCode)
		{
			A_Side_Test_Print_Value(pstRpmResp->m_eDataType, pstRpmResp->m_pvPropVal);
			/* free the decoded value allocated by BACApp_Decode_Data_Type() -
			 * the node itself (2nd..Nth) is freed below via Free_Argument_Memory() */
			Clear_PropVal_AsPer_DataType(pstRpmResp->m_eDataType, &pstRpmResp->m_pvPropVal);
		}
		else
		{
			Debug_LogFmt("RPM-A Test: property error, class=%u code=%lu\r\n",
				(unsigned)pstRpmResp->m_eErrorClass, (unsigned long)pstRpmResp->m_eErrorCode);
		}

		pstRpmResp = pstRpmResp->m_pstRPMNextElem;
	}

	/* release nodes 2..N (the head is embedded in pstServiceArgs, not malloc'd) */
	Free_Argument_Memory(pstServiceArgs->m_stNPDUData.m_stAPDUData.
		m_stServiceChoice.m_stRPM_Response.m_pstRPMNextElem,
		SERVICE_SUPPORTED_READ_PROP_MULTIPLE);

	return BACDEL_SUCCESS;
}

/**
 * DESCRIPTION
 * Builds and sends one RPM-A test request to RPM_A_TEST_DEST_MAC for two
 * properties of RPM_A_TEST_OBJ_TYPE/INSTANCE. Called periodically from
 * RPM_A_Test_Thread().
 */
void RPM_A_Test_Request(void)
{
	/* static: BACDEL_Generate_Aside_Request() copies these field values into
	 * freshly malloc'd internal nodes synchronously, within this call, so a
	 * stack-local list would in fact be safe here too - kept static anyway
	 * for consistency with WP_A_Test_Request()'s must-be-static value. */
	static rpm_request_t s_stRpmReqData[2] = {0};
	BACnetAddress_t stDestAddr = {0};
	int32_t i32TokenId = -1;
	static uint32_t s_u32RequestId = 0;
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;

	/* destination address - raw MS/TP MAC, see RP_A_Test_Request() for why
	 * every field here must be filled exactly this way */
	stDestAddr.u8mac_len = MSTP_ADDRESS_LEN;
	stDestAddr.u8dlen = 0;
	stDestAddr.u16net = 0;
	stDestAddr.u8IpAddrs[0] = RPM_A_TEST_DEST_MAC;

	/* first property */
	s_stRpmReqData[0].m_eObjectType = RPM_A_TEST_OBJ_TYPE;
	s_stRpmReqData[0].m_u32ObjectInstance = RPM_A_TEST_OBJ_INSTANCE;
	s_stRpmReqData[0].m_eObjectProperty = RPM_A_TEST_PROP_ID_1;
	s_stRpmReqData[0].m_u32ArrayIndex = 0;
	s_stRpmReqData[0].m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;
	s_stRpmReqData[0].m_pstRPMNextElem = &s_stRpmReqData[1];

	/* second property - same object, so both share one Object-ID header
	 * on the wire (see RPM_A_Request_Encoder()'s u8FillObjId grouping) */
	s_stRpmReqData[1].m_eObjectType = RPM_A_TEST_OBJ_TYPE;
	s_stRpmReqData[1].m_u32ObjectInstance = RPM_A_TEST_OBJ_INSTANCE;
	s_stRpmReqData[1].m_eObjectProperty = RPM_A_TEST_PROP_ID_2;
	s_stRpmReqData[1].m_u32ArrayIndex = 0;
	s_stRpmReqData[1].m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;
	s_stRpmReqData[1].m_pstRPMNextElem = NULL;

	eReturnType = BACDEL_Send_RPM(
		SMCfg.g_BACnetCfg.ObjID,			/* our own device instance (source) */
		0,									/* unused - bDestTypeFlag is FALSE */
		&stDestAddr,
		FALSE,								/* address by MAC, not by device ID */
		&s_stRpmReqData[0],
		RPM_A_Test_Response_Callback,
		&i32TokenId,
		s_u32RequestId++);

	if(BACDEL_SUCCESS != eReturnType)
	{
		Debug_LogFmt("RPM-A Test: BACDEL_Send_RPM() to MAC=%u failed, err=%d\r\n",
			(unsigned)RPM_A_TEST_DEST_MAC, (int)eReturnType);
	}
	else
	{
		Debug_LogFmt("RPM-A Test: request sent to MAC=%u, token=%ld\r\n",
			(unsigned)RPM_A_TEST_DEST_MAC, (long)i32TokenId);
	}
}

/* RPM_A_Test_Thread function - periodic caller for RPM_A_Test_Request(),
 * started from Rtos.c's InitFreeRtos() (mirrors RP_A_Test_Thread's pattern) */
void RPM_A_Test_Thread(void const * argument)
{
	for(;;)
	{
		RPM_A_Test_Request();
		osDelay(5000);
	}
}
#endif /* BACDEL_SER_DS_RPM_A */

/*================================== Data sharing services Test =====================================================*/


/* USER CODE BEGIN Application */

/* DvManagementThread function */
void DvManagementThread(void const * argument)
{
  /* USER CODE BEGIN DvManagementThread */

	// static uint8_t u8RPTestDone = 0;
  /* Infinite loop */
  for(;;)
  {
     DvManagementThread_Task();
  }
  /* USER CODE END DvManagementThread */
}

/* Initiate_Thread function */
void Bacnet_Initiate_Thread(void const * argument)
{
  /* USER CODE BEGIN Initiate_Thread */

  /* Infinite loop */
  for(;;)
  {
    Initiate_Thread_Task();
  }
  /* USER CODE END Initiate_Thread */
}

/* ClearInitiateQTimer_Timer function */
void ClearInitiateQTimer_Timer(void const * argument)
{
  /* USER CODE BEGIN  */
	ClearInitiateQTimer();
  /* USER CODE END */
}

/* MSTP_Counters_Timer function */
void MSTP_Counters_Timer(void const * argument)
{
	if(BACnetConfigData.m_eBACStack == BACNET_STACK_MSTP)
	{
		/* USER CODE BEGIN MSTP_Counters_Timer */
		MSTP_Counters_Timer_Task();
		/* USER CODE END MSTP_Counters_Timer */
	}
}

/* MSTP_StateMachine_Timer function */
void MSTP_StateMachine_Timer(void const * argument)
{
	if(BACnetConfigData.m_eBACStack == BACNET_STACK_MSTP)
	{
		/* USER CODE BEGIN MSTP_StateMachine_Timer */
		MSTP_StateMachine_Timer_Task();
		/* USER CODE END MSTP_StateMachine_Timer */
	}
}

// Add MSTP ifdef
void EnableIntUartTim()
{
	/* Set Interrupt Group Priority */
	  // Enable USART Interrupt
	  HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);

	  HAL_NVIC_EnableIRQ(USART2_IRQn);

	  // Enable Timer Interrupt
	  /*Configure the TIM2 IRQ priority */
	  HAL_NVIC_SetPriority(TIM2_IRQn, 5 ,0);
	  /* Enable the TIM2 global Interrupt */
	  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

//UartRxEnable
void UartRxEnable()
{
	BSP_Set_485_EN_On();
	BSP_Set_485_RTS_Off();
}

//UartTxEnable
void UartTxEnable()
{
	BSP_Set_485_EN_Off();
	BSP_Set_485_RTS_On();
}

/**
 * @fn 	    uint8_t SendByteBacnet(uint8_t byte)
 * @brief   Send 1 byte to BACnet bus
 * @param   None
 * @retval  1 if OK
 */
uint8_t SendPacketBacnet(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size)
{
	if(huart->gState == HAL_UART_STATE_READY)
	{
		/* set the flag to notify master tsm to stop reply postpone */
		b_ResponseSentOnNetwork = TRUE;
		/** Disable UART Receive Interrupt */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
		/** Enable Transmission Of RS485*/
		UartTxEnable();

		HAL_UART_Transmit_IT(&huart2,data,size);

		/**Enable UART Transmission Interrupt*/
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);

		/**Enable UART Transmission Complete Interrupt*/
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
		return 1; //nw
	}
	else //nw
	{
		return 0;
	}
}

void BACRS485_Interrupt_Rx(void)
{
	BnRxQue.Data[BnRxQue.i16Head] = u8BACnetRxByte;

	/** Update header and soft timers */
	BnRxQue.i16Head++;
	/** Check buffer overflow */
	if(BnRxQue.i16Head > (BN_QUESIZE-1))
	{
		/* set to zero */
		BnRxQue.i16Head = 0;
	}
	g_stMstpVariables.Tsilence = 0;
	g_stMstpVariables.SubTsilence = 0;

	/* trigger the mstp receive thread */
	if(!OSAL_Release_Sem(g_hsMstpRxSemaphoreHandle, BACNET_ONE))
	{
	   return;
	}
}

void UartConfig(void)
{
	  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	  BSP_Set_485_EN_On();//On
	  BSP_Set_485_RTS_Off();
}

/* MSTP_Recieve_Thread function */
void MSTP_Recieve_Thread(void const * argument)
{
  /* USER CODE BEGIN MSTP_Recieve_Thread */
  /* Infinite loop */
	for(;;)
	{
	   MSTP_Recieve_Thread_Task();
	}
  /* USER CODE END MSTP_Recieve_Thread */
}

void Test_API(void)
{
	// RP_A_Request();
	return;
}
