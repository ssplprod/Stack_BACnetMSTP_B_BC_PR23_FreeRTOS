/*
 * TestServiceAPI.c
 *
 *  Created on: 25-May-2020
 *      Author: Pranav.Phadatare
 */
#include "TestServiceAPI.h"
#include "bacDELDef.h"
#include "bacnetStackMgmt.h"
#include "objDevice.h"
#include "bacnetInitiateServiceMgmt.h"
#include "bacDELDeviceConfig.h"
#include "bacDELDef.h"
#include <string.h>
#include "defineStruct.h"
#include "DataB.h"
#include "ServiceAPICallBack.h"
#include <stdlib.h>
/** variable to save Token Id */
uint32_t g_u32TokenId = 0;
/** boolean flag for RP function */
bool g_bRpflag = false;

bool g_bRpmflag = false;

bool g_bWpflag = false;

uint32_t g_u32Current_Request_Id = 0;

extern DB_t SMCfg;
#ifdef BACDEL_SER_DS_RP_A
/**
*
* DESCRIPTION
* Function to get Input Parameter for RP-A service request.
*
* @return BACNET_RETURN_TYPE	[out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE RP_A_Request(void)
{
	uint32_t	u32SrsDevId = 0;
	uint32_t	u32DestDevId = 0;
	bool		bDestTypeFlag = TRUE;
	int32_t		i32TokenId = 1;

	uint32_t	u32ObjType = OBJECT_ANALOG_INPUT;
	uint32_t	u32ObjInst = 0;
    uint32_t	u32ObjPropID = PROP_PRESENT_VALUE;
    int32_t		i32ArrIndx = -1;
	rp_request_t		stRpReqData = {0};
	BACnetAddress_t		stDestinationAdd = {0};

	uint32_t	u32RequestId = 0;

	BACNET_RETURN_TYPE	eReturnType = BACDEL_SUCCESS;

	u32SrsDevId = SMCfg.g_BACnetCfg.ObjID;
	u32DestDevId = 32;

	stDestinationAdd.u8IpAddrs[0]=146;
	stDestinationAdd.u8IpAddrs[1]=1;
	stDestinationAdd.u8IpAddrs[2]=168;
	stDestinationAdd.u8IpAddrs[3]=192;

	stDestinationAdd.u8IpAddrs[4]=0xC0;
	stDestinationAdd.u8IpAddrs[5]=0xBA;
	stDestinationAdd.u8mac_len = 6;

	/* fill RP-A request parameter */
	stRpReqData.m_eObjectType = (BACNET_OBJECT_TYPE)u32ObjType;
	stRpReqData.m_u32ObjectInstance = u32ObjInst;
	stRpReqData.m_eObjectProperty = (BACNET_PROPERTY_ID)u32ObjPropID;
	stRpReqData.m_u32ArrayIndex = (uint32_t)i32ArrIndx;
	stRpReqData.m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;

	/* generate request-id */
	u32RequestId = Generate_Request_ID();

	eReturnType = BACDEL_Send_RP(
		u32SrsDevId,
		u32DestDevId,
		&stDestinationAdd,
		bDestTypeFlag,
		&stRpReqData,
		Callback_Function_RP,
		&i32TokenId,
		u32RequestId);

	if(BACDEL_SUCCESS == eReturnType)
	{
		g_bRpflag = true;
		g_u32TokenId = (uint32_t)i32TokenId;
	}

}
#endif
#ifdef BACDEL_SER_DS_RPM_A
/**
*
* DESCRIPTION
* Function to get Input Parameter for RPM-A service request.
*
* @return BACNET_RETURN_TYPE	[out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE RPM_A_Request(void)
{
	/* Local variables */

	uint32_t	u32SrsDevId = 0;
	uint32_t	u32DestDevId = 0;
	bool		bDestTypeFlag = TRUE;
	int32_t		i32TokenId = 1;
	uint32_t	u32ObjInst = 0;
    int32_t		i32ArrIndx = -1;
	rpm_request_t		stRpmReqData = {0};
	rpm_request_t		*pstRPMStruct = NULL;
	BACnetAddress_t		stDestinationAdd = {0};
	BACNET_RETURN_TYPE	eReturnType = BACDEL_SUCCESS;
  	uint32_t u32RequestId = 0;

	/* Save RPM Req. structure address in RPM Req. pointer */
	pstRPMStruct = &stRpmReqData;


		pstRPMStruct->m_eObjectType = OBJECT_ANALOG_INPUT;
		pstRPMStruct->m_u32ObjectInstance = u32ObjInst;
		pstRPMStruct->m_eObjectProperty = (BACNET_PROPERTY_ID)PROP_PRESENT_VALUE;
		pstRPMStruct->m_u32ArrayIndex = (uint32_t)i32ArrIndx;
		pstRPMStruct->m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;
		if(i32ArrIndx >= 0)
		{
			pstRPMStruct->m_u8ArrayIndexPresent = ARRAY_INDEX_PRESENT;
		}

        pstRPMStruct->m_pstRPMNextElem =
             (rpm_request_t *)malloc(sizeof(rpm_request_t));
         if(NULL == pstRPMStruct->m_pstRPMNextElem)
         {//				Free_RPM_A_Req_Data(stRpmReqData.m_pstRPMNextElem);
             return BACDEL_MALLOC_ERROR;// Return error
         }
		pstRPMStruct = pstRPMStruct->m_pstRPMNextElem;
		memset(pstRPMStruct, 0, sizeof(rpm_request_t));

		pstRPMStruct->m_eObjectType = OBJECT_ANALOG_INPUT;
		pstRPMStruct->m_u32ObjectInstance = u32ObjInst;
		pstRPMStruct->m_eObjectProperty = (BACNET_PROPERTY_ID)PROP_OBJECT_TYPE;
		pstRPMStruct->m_u32ArrayIndex = (uint32_t)i32ArrIndx;
		pstRPMStruct->m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;
		if(i32ArrIndx >= 0)
		{
			pstRPMStruct->m_u8ArrayIndexPresent = ARRAY_INDEX_PRESENT;
		}
		u32SrsDevId = SMCfg.g_BACnetCfg.ObjID;
		u32DestDevId = 32;

		stDestinationAdd.u8IpAddrs[0]=146;
		stDestinationAdd.u8IpAddrs[1]=1;
		stDestinationAdd.u8IpAddrs[2]=168;
		stDestinationAdd.u8IpAddrs[3]=192;

		stDestinationAdd.u8IpAddrs[4]=0xC0;
		stDestinationAdd.u8IpAddrs[5]=0xBA;

		stDestinationAdd.u8mac_len = 6;
	/* generate request-id */
	u32RequestId = Generate_Request_ID();

	/* call to API to send RPM-A service request */
	eReturnType = BACDEL_Send_RPM(
		u32SrsDevId,
		u32DestDevId,
		&stDestinationAdd,
		bDestTypeFlag,
		&stRpmReqData,
		Callback_Function_RPM,
		&i32TokenId,
		u32RequestId);

	if(BACDEL_SUCCESS == eReturnType)
	{
		g_bRpmflag = true;

		g_u32TokenId = (uint32_t)i32TokenId;
	}

//	/* free RPM-A request data */
//	Free_RPM_A_Req_Data(stRpmReqData.m_pstRPMNextElem);


    return eReturnType;
}
#endif /* BACDEL_SER_DS_RPM_A */

#ifdef BACDEL_SER_DS_WP_A
/**
*
* DESCRIPTION
* Function to get Input Parameter for WP-A service request.
*
* @return BACNET_RETURN_TYPE	[out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE WP_A_Request(void)
{
	/* Local variables */
	uint32_t	u32SrsDevId = 0;
	uint32_t	u32DestDevId = 0;
	bool		bDestTypeFlag = TRUE;
	int32_t		i32TokenId = 0;
	uint32_t	u32ObjInst = 0;
    int32_t		i32ArrIndx = -1;
	uint32_t	u32Priority = 0;
	wp_request_t		stWpReqData = {0};
	BACnetAddress_t		stDestinationAdd = {0};
	BACNET_RETURN_TYPE	eReturnType = BACDEL_SUCCESS;
	BACNET_DATA_TYPE	eDataType = BACNET_DT_MAX;
  	uint32_t u32RequestId = 0;
  	void *pvStructPtr = NULL;
	uint8_t		u8ValArr[100] = {0};
	uint32_t	u32Option = 0;

  		float test = 15.5;


		u32SrsDevId = SMCfg.g_BACnetCfg.ObjID;
		u32DestDevId = 32;

		stDestinationAdd.u8IpAddrs[0]=146;
		stDestinationAdd.u8IpAddrs[1]=1;
		stDestinationAdd.u8IpAddrs[2]=168;
		stDestinationAdd.u8IpAddrs[3]=192;

		stDestinationAdd.u8IpAddrs[4]=0xC0;
		stDestinationAdd.u8IpAddrs[5]=0xBA;

		stDestinationAdd.u8mac_len = 6;

		/* get data property type */
		eDataType = BACNET_DT_REAL;
#if 0
		*eDataType = BACDEL_Get_Property_DataType((BACNET_OBJECT_TYPE)u32ObjType,
			(BACNET_PROPERTY_ID)u32ObjPropID); //Obj type, Obj instance
#endif

		strcpy(u8ValArr,"15.5");

		pvStructPtr = Convert_StrVal_to_Strucure(eDataType, u8ValArr, i32ArrIndx,
			u32Option, &eReturnType);

		/* fill WP-A request parameter */
		#ifdef NEW_RP_WP_INTERFACE
		stWpReqData.m_pvPropVal= pvStructPtr;
		stWpReqData.m_eDataType = eDataType;
		#endif

		stWpReqData.m_eObjectType = OBJECT_ANALOG_OUTPUT;
		stWpReqData.m_u32ObjectInstance = u32ObjInst;
		stWpReqData.m_eObjectProperty = PROP_PRESENT_VALUE;
		stWpReqData.m_u32ArrayIndex = (uint32_t)i32ArrIndx;
		stWpReqData.m_u32Priority = u32Priority;
		stWpReqData.m_u8ArrayIndexPresent = ARRAY_INDEX_ABSENT;
		if(i32ArrIndx >= 0)
		{
			stWpReqData.m_u8ArrayIndexPresent = ARRAY_INDEX_PRESENT;
		}

		/* generate request-id */
		u32RequestId = Generate_Request_ID();

		/* call to API to send WP-A service request */
		eReturnType = BACDEL_Send_WP(
			u32SrsDevId,
			u32DestDevId,
			&stDestinationAdd,
			bDestTypeFlag,
			&stWpReqData,
			Callback_Function_WP,
			&i32TokenId,
			u32RequestId);

		if(BACDEL_SUCCESS == eReturnType)

		{
			g_bWpflag = true;
			g_u32TokenId = (uint32_t)i32TokenId;
		}

    return eReturnType;
}
#endif /* BACDEL_SER_DS_WP_A */


#ifdef BACDEL_SER_DM_DDB_A
/**
*
* DESCRIPTION
* Function to get Input Parameter for DDB-A service request.
*
* @return BACNET_RETURN_TYPE	[out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE WhoIs_A_Request(void)
{
		ddb_who_is_t stWhoIsData = {0};
		uint32_t u32RequestId =1;

		BACnetAddress_t stDestinationAdd = {0};
		uint32_t u32tokenID;
		stWhoIsData.m_i32DevRangeHighLimit = -1;
		stWhoIsData.m_i32DevRangeLowLimit = -1;

		stDestinationAdd.u8IpAddrs[0]=255;
		stDestinationAdd.u8IpAddrs[1]=0;
		stDestinationAdd.u8IpAddrs[2]=168;
		stDestinationAdd.u8IpAddrs[3]=192;

		stDestinationAdd.u8IpAddrs[4]=0xC0;
		stDestinationAdd.u8IpAddrs[5]=0xBA;
		stDestinationAdd.u8mac_len = 6;

		/* generate request-id */
		u32RequestId = Generate_Request_ID();

		BACDEL_Send_Who_Is(SMCfg.g_BACnetCfg.ObjID,0,&stDestinationAdd,false,DESTINATION_IS_GLOBAL_BROADCAST,false,0,&stWhoIsData,&u32tokenID,u32RequestId);
}
#endif

/**
*
* DESCRIPTION
* Function to get Input Parameter for I AM service request.
*
* @return BACNET_RETURN_TYPE	[out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE IAm_Request(void)
{


		BACnetAddress_t stDestinationAdd = {0};
		uint32_t u32tokenID;

		stDestinationAdd.u8IpAddrs[0]=255;
		stDestinationAdd.u8IpAddrs[1]=0;
		stDestinationAdd.u8IpAddrs[2]=168;
		stDestinationAdd.u8IpAddrs[3]=192;

		stDestinationAdd.u8IpAddrs[4]=0xC0;
		stDestinationAdd.u8IpAddrs[5]=0xBA;
		stDestinationAdd.u8mac_len = 6;

        BACDEL_Send_I_Am(SMCfg.g_BACnetCfg.ObjID,0,&stDestinationAdd,false,DESTINATION_IS_GLOBAL_BROADCAST,false,&u32tokenID);

}

/**
*
* DESCRIPTION
* This function generates request-id parameter value for A-Side request.
*
* @returns valid or invalid request id no.
*
*/
uint32_t Generate_Request_ID()
{
    /* local varaibles */
    uint32_t u32NewRequestId = 0;

	/* aquire mutex */
//    App_OSAL_Wait_Mutex(g_hIDReqIdMtxLock, INFINITE);

    /* get the current value of request id */
    u32NewRequestId = g_u32Current_Request_Id;

    /* increment value of request id */
    u32NewRequestId++;
    if(u32NewRequestId == 0)
        u32NewRequestId = 1;

    /* save the new request id value */
    g_u32Current_Request_Id = u32NewRequestId;

	/* release the mutex */
//	App_OSAL_Release_Mutex(g_hIDReqIdMtxLock);

    /* return invalid request id value */
    return(u32NewRequestId);
}

/* function to allocate memory */
void *Memory_Allocate(uint32_t u32Size, uint8_t u8InitValue)
{
    /* local variables */
    void *pvMemPtr = NULL;

    /* allocate the memory */
    pvMemPtr = (void *)malloc(u32Size);

    if(NULL != pvMemPtr)
    {
        /* initialize the memory with given value */
        memset(pvMemPtr, u8InitValue, u32Size);
    }

    /* return the pointer */
    return pvMemPtr;
}

/* function to free allocated memory */
void Memory_Free(void ** ppvMemPtr)
{
    /* local variables */
    void *pvMemPtr = NULL;

    /* check input pointer */
    if(NULL != ppvMemPtr && NULL != *ppvMemPtr)
    {
        /* get the pointer to be freed */
        pvMemPtr = *ppvMemPtr;
        /* reset original pointer */
        *ppvMemPtr = NULL;
        /* free the allocated memory */
        free(pvMemPtr);
    }
}
/**
*   Reverses the bits of a byte.
*   @param uIn_Byte [in] Byte to be reversed.
*   @return Returns the reversed byte
*/
uint8_t Reverse_Bits_In_Byte(uint8_t u8In_Byte)
{
	/* local variable */
    uint8_t u8Out_Byte = 0;

    if(u8In_Byte & BIT0)
    {
        u8Out_Byte |= BIT7;
    }
    if(u8In_Byte & BIT1)
    {
        u8Out_Byte |= BIT6;
    }
    if(u8In_Byte & BIT2)
    {
        u8Out_Byte |= BIT5;
    }
    if(u8In_Byte & BIT3)
    {
        u8Out_Byte |= BIT4;
    }
    if(u8In_Byte & BIT4)
    {
        u8Out_Byte |= BIT3;
    }
    if(u8In_Byte & BIT5)
    {
        u8Out_Byte |= BIT2;
    }
    if(u8In_Byte & BIT6)
    {
        u8Out_Byte |= BIT1;
    }
    if(u8In_Byte & BIT7)
    {
        u8Out_Byte |= BIT0;
    }

    return u8Out_Byte;
}
/* function to parse bit string values from CSV file */
void Parse_BitString_From_Csv(uint8_t *pInData, BACnetBITStr_t *pDataBitStr)
{
	uint8_t u8StrCnt=0, u8ValArr1[300]={0},u8ArrayCnt=0,i=0,j=0,k=0,u8TestArr[10]={0};
	uint8_t u8byte1=0, u8byte2=0;

	u8StrCnt = (uint32_t)strlen((const char *)pInData);
	if(u8StrCnt<200)
	{
		for(j=0;i<u8StrCnt;j++)
		{
			if(*pInData != '(' && *pInData != ')'&& *pInData != ','&& *pInData != '"')
			{
				u8ValArr1[i] = *pInData;
				i++;
			}
			pInData++;
			if(*pInData == ')')
				break;
		}
		if(i<=7)		//If string is less than 8 characters
		{
			u8ArrayCnt = 1;
			pDataBitStr->m_u8UnusedBits = (8-i);
			pDataBitStr->m_u8ByteCnt = u8ArrayCnt;
		}
		else
		{
			u8ArrayCnt = i/8;

			//u8UnusedBits = i%8;
			pDataBitStr->m_u8UnusedBits = (8-(i%8));
			if(0 != pDataBitStr->m_u8UnusedBits)
				u8ArrayCnt +=1;
			pDataBitStr->m_u8ByteCnt = u8ArrayCnt;
		}
		i=0;
		for(j=0;j<u8ArrayCnt;)
		{
			for(k=0;k<=7;k++)
			{
				if(u8ValArr1[i]== '1')
				{
				    //pDataBitStr->m_stBitString.m_u8TransBits[j] |= 1 << k;
					u8TestArr[j] |= 1 << k;
					pDataBitStr->m_u8TransBits[j]=u8TestArr[j];
					i++;

				}
				else
				{
					//pDataBitStr->m_stBitString.m_u8TransBits[j] |= 0 << k;
					u8TestArr[j] |= 0 << k;
					pDataBitStr->m_u8TransBits[j] = u8TestArr[j];
					i++;
				}

			}
			j++;
		}
	}

	/* reverse the bits of bytes */
	for(i =0; i< MIN_BITSTRING_BYTES; i++)
	{
		pDataBitStr->m_u8TransBits[i] = Reverse_Bits_In_Byte
			(pDataBitStr->m_u8TransBits[i]);
	}
}

/* Function to convert strings to structure of given data type and
*  return apprppriate data type structure pointer*/

void *Convert_StrVal_to_Strucure(BACNET_DATA_TYPE eDataType,
	void *pvInData,
	int32_t i32ArrIndx,
	uint32_t u32Option,
	BACNET_RETURN_TYPE *pstReturnType)
{
    /* Void pointer to be sent to UpdateCommonProperty function */
    void *pvData = NULL;
	uint32_t au32Array[10] ={0};
    /* Local loop counter and values */
    uint8_t *pInData = NULL;

    /* get the property data */
    if(NULL != pvInData)
        pInData = (uint8_t *)pvInData;

    switch(eDataType)
    {
		case BACNET_DT_NULL:
		case BACNET_DT_EMPTY:
		{
			pvData = NULL;
		}
		break;

		case BACNET_DT_REAL:
		{
			Pr_BACnetReal_t *pDataReal = NULL;
			pDataReal = (Pr_BACnetReal_t *) malloc(sizeof(Pr_BACnetReal_t));
			if(pDataReal == NULL)
			{
				return NULL;
			}
			memset(pDataReal, 0, sizeof(Pr_BACnetReal_t));
			pDataReal->m_fVal = (float)atof((const char*)pInData);
			pvData = (void *)pDataReal;
		}
		break;

		case BACNET_DT_BOOLEAN:
		{
			Pr_BACnetBool_t *pDataBool = NULL;
			pDataBool = (Pr_BACnetBool_t *) malloc(sizeof(Pr_BACnetBool_t));
			if(pDataBool == NULL)
			{
				return NULL;
			}
			memset(pDataBool, 0, sizeof(Pr_BACnetBool_t));
			if(pInData[0]=='1')
				pDataBool->m_bVal = TRUE;
			else
			pDataBool->m_bVal = FALSE;

			pvData = (void *)pDataBool;
		}
		break;

		case BACNET_DT_ENUM_NEW:
		case BACNET_DT_OBJTYPE:
		case BACNET_DT_BACNETSEG:
		case BACNET_DT_BACNETDEVSTAT:
		case BACNET_DT_BACKUPSTATE:
		{
			Pr_BACnetEnum_t *pDataEnum = NULL;
			pDataEnum = (Pr_BACnetEnum_t *) malloc(sizeof(Pr_BACnetEnum_t));
			if(pDataEnum == NULL)
			{
				return NULL;
			}
			memset(pDataEnum, 0, sizeof(Pr_BACnetEnum_t));
			pDataEnum->m_eVal = (uint32_t)strtoul((const char*)pInData,NULL,NULL);
			pvData = (void *)pDataEnum;

		}
		break;

		case BACNET_DT_ENUM:
		{
			Pr_BinaryEnumPV_t *pDataEnum = NULL;
			pDataEnum = (Pr_BinaryEnumPV_t *) malloc(sizeof(Pr_BinaryEnumPV_t));
			if(pDataEnum == NULL)
			{
				return NULL;
			}
			memset(pDataEnum, 0, sizeof(Pr_BinaryEnumPV_t));
			pDataEnum->m_eVal = (BACNET_BINARY_PV)strtoul((const char*)pInData,NULL,NULL);
			pvData = (void *)pDataEnum;
		}

		break;

		case BACNET_DT_UNSIGNED32:
		case BACNET_DT_UNSIGNED:
		{
			Pr_BACnetUnsigned32_t *pDataInt = NULL;
			pDataInt = (Pr_BACnetUnsigned32_t *) malloc(sizeof(Pr_BACnetUnsigned32_t));
			if(pDataInt == NULL)
			{
				return NULL;
			}
			memset(pDataInt, 0, sizeof(Pr_BACnetUnsigned32_t));
			pDataInt->m_u32Val = (uint32_t)strtoul((const char*)pInData,NULL,NULL);
			pvData = (void *)pDataInt;
		}
		break;

		case BACNET_DT_UNSIGNED16:
		{
			Pr_BACnetUnsigned16_t *pDataInt = NULL;
			pDataInt = (Pr_BACnetUnsigned16_t *) malloc(sizeof(Pr_BACnetUnsigned16_t));
			if(pDataInt == NULL)
			{
				return NULL;
			}
			memset(pDataInt, 0, sizeof(Pr_BACnetUnsigned16_t));
			pDataInt->m_u16Val = (uint16_t)strtoul((const char*)pInData,NULL,NULL);
			pvData = (void *)pDataInt;
		}
		break;

		case BACNET_DT_UNSIGNED8:
		{
			Pr_BACnetUnsigned8_t *pDataInt = NULL;
			pDataInt = (Pr_BACnetUnsigned8_t *) malloc(sizeof(Pr_BACnetUnsigned8_t));
			if(pDataInt == NULL)
			{
				return NULL;
			}
			memset(pDataInt, 0, sizeof(Pr_BACnetUnsigned8_t));
			pDataInt->m_u8Val = (uint8_t)strtoul((const char*)pInData,NULL,NULL);
			pvData = (void *)pDataInt;
		}
		break;

		case BACNET_DT_TIMESTAMP:
		{
			Pr_BACnetTimeStamp_t *pstPropTimeStamp = NULL;
			uint32_t u32Timestamptype = 0;
			uint32_t u32Hours = 0;
			uint32_t u32Mins = 0;
			uint32_t u32Secs = 0;
			uint32_t u32Hundreds = 0;
			uint32_t u32Year = 0;
			uint32_t u32Month = 0;
			uint32_t u32Date = 0;
			uint32_t u32WDay = 0;
			uint32_t u32SeqNo = 0;

			/* allocate memory */
			pstPropTimeStamp = (Pr_BACnetTimeStamp_t *)
				malloc(sizeof(Pr_BACnetTimeStamp_t));

			if(NULL == pstPropTimeStamp)
			{
				return NULL;
			}
			memset(pstPropTimeStamp, 0, sizeof(Pr_BACnetTimeStamp_t));

//			sscanf((const char*)pInData,"%u-%u(%u/%u/%u:%u)(%u:%u:%u.%u)",
//				&u32Timestamptype, &u32SeqNo, &u32Year, &u32Month, &u32Date, &u32WDay,
//				&u32Hours, &u32Mins, &u32Secs, &u32Hundreds);

			/* copy data */
			pstPropTimeStamp->m_stTimeStamp.m_eTimeStampType =
				(BACNET_TIMESTAMP_TYPE)u32Timestamptype;
			switch(u32Timestamptype)
			{
				case TIMESTAMP_TYPE_TIME:
				{
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stTime.m_u8Hour
						= (uint8_t)u32Hours;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stTime.m_u8Min
						= (uint8_t)u32Mins;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stTime.m_u8Sec
						= (uint8_t)u32Secs;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stTime.m_u8Hundredths
						= (uint8_t)u32Hundreds;
				}
				break;
				case TIMESTAMP_TYPE_SEQUENCE_NO:
				{
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_u32SeqNo = u32SeqNo;
				}
				break;
				case TIMESTAMP_TYPE_DATETIME:
				{
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.
						m_stDate.m_u16Year = (uint16_t)u32Year;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.
						m_stDate.m_u8Month 	= (uint8_t)u32Month;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.
						m_stDate.m_u8Day = (uint8_t)u32Date;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.
						m_stDate.m_u8Wday = (uint8_t)u32WDay;

					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime.
						m_u8Hour = (uint8_t)u32Hours;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime.
						m_u8Min = (uint8_t)u32Mins;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime.
						m_u8Sec = (uint8_t)u32Secs;
					pstPropTimeStamp->m_stTimeStamp.m_stTimeStamp.m_stDateTime.m_stTime.
						m_u8Hundredths = (uint8_t)u32Hundreds;
				}
				break;
				default:
					break;
			}

			pvData = (void *)pstPropTimeStamp;
		}
		break;

		case BACNET_DT_INTEGER:		//signed32
		{
			Pr_BACnetSigned32_t *pDataInt = NULL;
			pDataInt = (Pr_BACnetSigned32_t *) malloc(sizeof(Pr_BACnetSigned32_t));
			if(pDataInt == NULL)
			{
				return NULL;
			}
			memset(pDataInt, 0, sizeof(Pr_BACnetSigned32_t));
			pDataInt->m_i32Val = atoi((const char*)pInData);
			pvData = (void *)pDataInt;
		}
		break;

		case BACNET_DT_DOUBLE:
		{
			Pr_BACnetDouble_t *pDataDbl = NULL;
			pDataDbl = (Pr_BACnetDouble_t *) malloc(sizeof(Pr_BACnetDouble_t));
			if(pDataDbl == NULL)
			{
				return NULL;
			}
			memset(pDataDbl, 0, sizeof(Pr_BACnetDouble_t));
			pDataDbl->m_dVal = strtod((const char*)pInData, NULL);
			pvData = (void *)pDataDbl;
		}
		break;

		case BACNET_DT_CHARSTRING:
		{
			unsigned char i=0;
			Pr_BACnetCharStr_t *pDataChStr = NULL;
			pDataChStr = (Pr_BACnetCharStr_t *) malloc(sizeof(Pr_BACnetCharStr_t));
			if(pDataChStr == NULL)
			{
				return NULL;
			}
			memset(pDataChStr, 0, sizeof(Pr_BACnetCharStr_t));
			pDataChStr->m_stCHARString.m_u32StrLen = (uint32_t)strlen((const char*)pInData);
			for(i=0;i<pDataChStr->m_stCHARString.m_u32StrLen;i++)
			{
				pDataChStr->m_stCHARString.m_pu8CharStr[i] = *pInData;
				pInData++;
			}
			pvData = (void *)pDataChStr;
		}
		break;

		case BACNET_DT_TIME:
		{
			Pr_BACnetTime_t *pDataTimeDef = NULL;
			pDataTimeDef = (Pr_BACnetTime_t *) malloc(sizeof(Pr_BACnetTime_t));
			if(pDataTimeDef == NULL)
			{
				return NULL;
			}
			memset(pDataTimeDef, 0, sizeof(Pr_BACnetTime_t));

//			sscanf((const char*)pInData,"(%u:%u:%u.%u)", &au32Array[0],
//				&au32Array[1], &au32Array[2], &au32Array[3]);

			pDataTimeDef->m_Time_val.m_u8Hour = (uint8_t)au32Array[0];
			pDataTimeDef->m_Time_val.m_u8Min = (uint8_t)au32Array[1];
			pDataTimeDef->m_Time_val.m_u8Sec = (uint8_t)au32Array[2];
			pDataTimeDef->m_Time_val.m_u8Hundredths = (uint8_t)au32Array[3];
			pvData = (void *)pDataTimeDef;
		}
		break;

		case BACNET_DT_DATE:
		{
			Pr_BACnetDate_t *pDataDate = NULL;
			pDataDate = (Pr_BACnetDate_t *) malloc(sizeof(Pr_BACnetDate_t));
			if(pDataDate == NULL)
			{
				return NULL;
			}
			memset(pDataDate, 0, sizeof(Pr_BACnetDate_t));
//			sscanf((const char*)pInData,"(%u/%u/%u:%u)", &au32Array[0],
//				&au32Array[1], &au32Array[2], &au32Array[3]);

			pDataDate->m_Date_val.m_u16Year = (uint16_t)au32Array[0];
			pDataDate->m_Date_val.m_u8Month = (uint8_t)au32Array[1];
			pDataDate->m_Date_val.m_u8Day = (uint8_t)au32Array[2];
			pDataDate->m_Date_val.m_u8Wday = (uint8_t)au32Array[3];
			pvData = (void *)pDataDate;

		}
		break;

		case BACNET_DT_BITSTRING:
		case BACNET_DT_BITSTRING_NEW:
		{
			uint8_t u8StrCnt=0, u8ValArr1[80]={0},u8ArrayCnt=0,i=0,j=0,k=0,u8TestArr[10]={0};
			uint8_t u8byte1=0, u8byte2=0;
			Pr_BACnetBITStr_t *pDataBitStr = NULL;
			pDataBitStr = (Pr_BACnetBITStr_t *) malloc(sizeof(Pr_BACnetBITStr_t));
			if(pDataBitStr == NULL)
			{
				return NULL;
			}
			memset(pDataBitStr, 0, sizeof(Pr_BACnetBITStr_t));

			Parse_BitString_From_Csv(pInData, &pDataBitStr->m_stBitString);

			pvData = (void *)pDataBitStr;
		}
		break;

		case BACNET_DT_OCTETSTRING:
		{
			uint8_t i=0,j=0,k=0;
			uint8_t u8ValArr1[110]={0}, u8ValArr2[2]={0},u8StrLen=0;
			Pr_BACnetOctetStr_t *pDataOctStr = NULL;
			pDataOctStr = (Pr_BACnetOctetStr_t *) malloc(sizeof(Pr_BACnetOctetStr_t));
			if(pDataOctStr == NULL)
			{
				return NULL;
			}
			memset(pDataOctStr, 0, sizeof(Pr_BACnetOctetStr_t));
			u8StrLen = (uint32_t)strlen((const char*)pInData);
			for(i=0;i<u8StrLen;)
			{
				if(*pInData != '(' && *pInData != ')'&& *pInData != ':')
				{
					u8ValArr1[i] = *pInData;
					i++;
				}
				pInData++;
				if(*pInData == ')')
						break;
			}
			for(j=0;j<i;)
			{
				u8ValArr2[0]=u8ValArr1[j];
				u8ValArr2[1]=u8ValArr1[j+1];
				pDataOctStr->m_stOctetString.m_ai8OctetStr[k]=(uint8_t)strtoul((const char*)&u8ValArr2[0],NULL,16);
				k++;
				pDataOctStr->m_stOctetString.m_u32OctetCount = k;
				j+=2;
			}
			pvData = (void *)pDataOctStr;
		}
		break;

		case BACNET_DT_OBJECTID:
		{
			Pr_BACnetObjId_t *pObjId = NULL;
			pObjId = (Pr_BACnetObjId_t *) malloc(sizeof(Pr_BACnetObjId_t));
			if(pObjId == NULL)
			{
				return NULL;
			}
			memset(pObjId, 0, sizeof(Pr_BACnetObjId_t));
//			sscanf((const char*)pInData,"\"(%d,%d)\"",&pObjId->m_eObjectType,&pObjId->m_u32ObjId);
			pvData = (void *)pObjId;
		}
		break;
#if 0
		case BACNET_DT_SCALE:
		{
			Pr_BACnetScale_t *pBacnetScale = NULL;
			pBacnetScale = (Pr_BACnetScale_t *) malloc(sizeof(Pr_BACnetScale_t));
			if(pBacnetScale == NULL)
			{
				return NULL;
			}
			memset(pBacnetScale, 0, sizeof(Pr_BACnetScale_t));
			if(pInData[1]=='0')
			{
//				sscanf((const char*)pInData,"(%d-%f)",
//					&pBacnetScale->m_stScale.m_bIsintegerScale,
//					&pBacnetScale->m_stScale.Scale_u.m_fFloatScale);
			}
			else
			{
//				sscanf((const char*)pInData,"(%d-%d)",
//					&pBacnetScale->m_stScale.m_bIsintegerScale,
//					&pBacnetScale->m_stScale.Scale_u.m_i32IntegerScale);
			}

			pvData = (void *)pBacnetScale;
		}
		break;

		case BACNET_DT_PRESCALE:
		{
			Pr_BACnetPrescale_t *pBacnetPrescale = NULL;
			pBacnetPrescale = (Pr_BACnetPrescale_t *) malloc(sizeof(Pr_BACnetPrescale_t));
			if(pBacnetPrescale == NULL)
			{
				return NULL;
			}
			memset(pBacnetPrescale, 0, sizeof(Pr_BACnetPrescale_t));
			sscanf((const char*)pInData,"\"(%u,%u)\"",
				&pBacnetPrescale->m_stPrescale.m_u32Multiplier,
				&pBacnetPrescale->m_stPrescale.m_u32ModuloDivide);

			pvData = (void *)pBacnetPrescale;
		}
		break;
#endif
		case BACNET_DT_NOTIFICATIONPRIORITY:
		{
			/* local variables */
			Pr_BACnetNotifyPriority_t *pstNotifyPriority = NULL;

			/* allocate memory */
			pstNotifyPriority = (Pr_BACnetNotifyPriority_t *)
				malloc(sizeof(Pr_BACnetNotifyPriority_t));
			if(NULL == pstNotifyPriority)
			{
				return NULL;
			}
			memset(pstNotifyPriority, 0, sizeof(Pr_BACnetNotifyPriority_t));

//			/* get the value */
//			sscanf((const char*)pInData,"%u,%u,%u",
//				&pstNotifyPriority->m_u32Value[0],
//				&pstNotifyPriority->m_u32Value[1],
//				&pstNotifyPriority->m_u32Value[2]);

			/* return pointer value */
			pvData = (void *)pstNotifyPriority;
		}
		break;

#if 0
		case BACNET_DT_ACCRECORD:
		{
			/* local variable */
			uint32_t u32Year = 0;
			uint32_t u32Month = 0;
			uint32_t u32Day = 0;
			uint32_t u32Wday = 0;
			uint32_t u32Hour = 0;
			uint32_t u32Min = 0;
			uint32_t u32Second = 0;
			uint32_t u32Hundredths = 0;
			uint32_t u32PresentValue = 0;
			uint32_t u32AccumulatedValue = 0;
			uint32_t u32AccStatus = 0;
			Pr_BACnetAccRecord_t *pstBacnetAccRec = NULL;

			/* allocate memory */
			pstBacnetAccRec = (Pr_BACnetAccRecord_t *)
				malloc(sizeof(Pr_BACnetAccRecord_t));
			if(NULL == pstBacnetAccRec)
			{
				return NULL;
			}
			/* set to default value */
			memset(pstBacnetAccRec, 0, sizeof(Pr_BACnetAccRecord_t));

			sscanf((const char*)pInData,"(%u/%u/%u:%u)(%u:%u:%u.%u),%u,%u,%u",
				&u32Year, &u32Month, &u32Day, &u32Wday, &u32Hour, &u32Min,
				&u32Second, &u32Hundredths, &u32PresentValue, &u32AccumulatedValue,
				&u32AccStatus);

			/* fill the request parameter */
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stDate.m_u16Year =
				(uint16_t)u32Year;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stDate.m_u8Month =
				(uint8_t)u32Month;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stDate.m_u8Day =
				(uint8_t)u32Day;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stDate.m_u8Wday =
				(uint8_t)u32Wday;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stTime.m_u8Hour =
				(uint8_t)u32Hour;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stTime.m_u8Min =
				(uint8_t)u32Min;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stTime.m_u8Sec =
				(uint8_t)u32Second;
			pstBacnetAccRec->m_stRecord.m_stTimestamp.m_stTime.
				m_u8Hundredths = (uint8_t)u32Hundredths;
			pstBacnetAccRec->m_stRecord.m_u32PresentValue = u32PresentValue;
			pstBacnetAccRec->m_stRecord.m_u32AccumulatedValue = u32AccumulatedValue;
			pstBacnetAccRec->m_stRecord.m_eAccStatus = (BACNET_ACC_STATUS)u32AccStatus;

			/* return pointer value */
			pvData = (void *)pstBacnetAccRec;
		}
		break;
#endif
		case BACNET_DT_UNSIGNED_LIST:
		case BACNET_DT_UNSIGNED_ARRAY:
		{
			/* local variables */
			Pr_ListOfUnsigned_t *pstUnsignVal = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfUnsigned_t *pstIntList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstUnsignVal = (Pr_ListOfUnsigned_t *)malloc(sizeof(Pr_ListOfUnsigned_t));

			if(NULL == pstUnsignVal)
			{
				return NULL;
			}
			memset(pstUnsignVal, 0, sizeof(Pr_ListOfUnsigned_t));
			pstUnsignVal->m_pstUnsignVal = (ListOfUnsigned_t *)malloc(sizeof(ListOfUnsigned_t));
			pstIntList = pstUnsignVal->m_pstUnsignVal;
			if(NULL == pstIntList)
			{
				return NULL;
			}
			memset(pstIntList, 0, sizeof(ListOfUnsigned_t));


			/* get the 1st value */
			//pstUnsignVal->m_u32Value = atoi((const uint8_t* ) strtok (pInData,", "));
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,",");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
				sscanf((const char*)pu8Token, "(%d", &pstIntList->m_u32Value);
				//pstIntList->m_u32Value = atoi((const char * )pu8Token);


			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL,",");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;

				/* allocate memory */
				pstIntList->m_pstNext = (ListOfUnsigned_t *)malloc(sizeof(ListOfUnsigned_t));

				if(NULL == pstIntList->m_pstNext)
				{
					break;
				}

				/* move to the new node */
				pstIntList = pstIntList->m_pstNext;
				memset(pstIntList, 0, sizeof(ListOfUnsigned_t));

				/* save the value */
				pstIntList->m_u32Value = atoi((const char *)pu8Token);


			}
			pvData = (void *)pstUnsignVal;
		}
		break;

		case BACNET_DT_ENUM_ARRAY:
		case BACNET_DT_ENUM_LIST:
		{
			/* local variables */
			Pr_ListOfEnum_t *pstEnumList = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfEnum_t *pstEList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstEnumList = (Pr_ListOfEnum_t *)malloc(sizeof(Pr_ListOfEnum_t));

			if(NULL == pstEnumList)
			{
				return NULL;
			}
			memset(pstEnumList, 0, sizeof(Pr_ListOfEnum_t));

			pstEnumList->m_pstEnumList = (ListOfEnum_t *)malloc(sizeof(ListOfEnum_t));
			pstEList = pstEnumList->m_pstEnumList;

			if(NULL == pstEList)
			{
				return NULL;
			}
			memset(pstEList, 0, sizeof(ListOfEnum_t));


			/* get the 1st value */
			//pstUnsignVal->m_u32Value = atoi((const uint8_t* ) strtok (pInData,", "));
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,",");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
				pstEList->m_eVal = (uint32_t)strtoul((const char*)pu8Token,NULL,NULL);

			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL,",");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;

				/* allocate memory */
				pstEList->m_pstNext = (ListOfEnum_t *)malloc(sizeof(ListOfEnum_t));

				if(NULL == pstEList->m_pstNext)
				{
					break;
				}

				/* move to the new node */
				pstEList = pstEList->m_pstNext;
				memset(pstEList, 0, sizeof(ListOfEnum_t));

				/* save the value */
				pstEList->m_eVal = (uint32_t)strtoul((const char*)pu8Token,NULL,NULL);


			}
			pvData = (void *)pstEnumList;
		}
		break;

		case BACNET_DT_CHARSTRING_ARRAY:
		{
			/* local variables */
			Pr_ListOfCharStr_t *pstCharStringList = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfCharStr_t *pstStrList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstCharStringList = (Pr_ListOfCharStr_t *)malloc(sizeof(Pr_ListOfCharStr_t));

			if(NULL == pstCharStringList)
			{
				return NULL;
			}
			memset(pstCharStringList, 0, sizeof(Pr_ListOfCharStr_t));

			pstStrList = &pstCharStringList->m_stStringVal;

			/* get the 1st value */
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,",");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
			{
				pstStrList->m_stCharStr.m_u32StrLen = (uint32_t)strnlen((const char *)pu8Token, MAX_CHARACTER_STRING_BYTES);
				memcpy(&pstStrList->m_stCharStr.m_pu8CharStr, pu8Token, pstStrList->m_stCharStr.m_u32StrLen);
				pstCharStringList->m_u32Count++;
			}


			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL,",");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;

				/* allocate memory */
				pstStrList->m_pstNext = (ListOfCharStr_t *)malloc(sizeof(ListOfCharStr_t));

				if(NULL == pstStrList->m_pstNext)
				{
					break;
				}

				/* move to the new node */
				pstStrList = pstStrList->m_pstNext;
				memset(pstStrList, 0, sizeof(ListOfCharStr_t));

				/* save the value */
				pstStrList->m_stCharStr.m_u32StrLen = (uint32_t)strnlen((const char *)pu8Token, MAX_CHARACTER_STRING_BYTES);
				memcpy(&pstStrList->m_stCharStr.m_pu8CharStr, pu8Token, pstStrList->m_stCharStr.m_u32StrLen);
				pstCharStringList->m_u32Count++;
			}
			pvData = (void *)pstCharStringList;
		}
		break;

		case BACNET_DT_OPTCHARSTRING_ARRAY:
		{
			/* local variables */
			Pr_ListOfOptCharStr_t *pstOptStrArr = NULL;
			Pr_ListOfUnsigned_t *pstUnsignVal = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfOptCharStr_t *pstOptList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstOptStrArr = (Pr_ListOfOptCharStr_t *)malloc(sizeof(Pr_ListOfOptCharStr_t));

			if(NULL == pstOptStrArr)
			{
				return NULL;
			}
			memset(pstOptStrArr, 0, sizeof(Pr_ListOfOptCharStr_t));
			pstOptStrArr->m_pstOptCharStr = (ListOfOptCharStr_t *)malloc(sizeof(ListOfOptCharStr_t));
			pstOptList = pstOptStrArr->m_pstOptCharStr;
			if(NULL == pstOptList)
			{
				return NULL;
			}
			memset(pstOptList, 0, sizeof(ListOfOptCharStr_t));


			/* get the 1st value */
			//pstUnsignVal->m_u32Value = atoi((const uint8_t* ) strtok (pInData,", "));
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,",");
			if(strcmp((const char *)pu8Token, "null") == 0)
				{
					pstOptList->m_i8AppTag = BACNET_APPLICATION_TAG_NULL;
				}
				else
				{
					pstOptList->m_i8AppTag = BACNET_APPLICATION_TAG_CHARACTER_STRING;
					/* save the value */
					pstOptList->m_stCharStr.m_u32StrLen = (uint32_t)strnlen((const char *)pu8Token, MAX_CHARACTER_STRING_BYTES);
					memcpy(&pstOptList->m_stCharStr.m_pu8CharStr, pu8Token, pstOptList->m_stCharStr.m_u32StrLen);
				}

			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL,",");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;

				/* allocate memory */
				pstOptList->m_pstNext = (ListOfOptCharStr_t *)malloc(sizeof(ListOfOptCharStr_t));

				if(NULL == pstOptList->m_pstNext)
				{
					break;
				}

				/* move to the new node */
				pstOptList = pstOptList->m_pstNext;
				memset(pstOptList, 0, sizeof(ListOfOptCharStr_t));

				if(strcmp((const char *)pu8Token, "null") == 0)
				{
					pstOptList->m_i8AppTag = BACNET_APPLICATION_TAG_NULL;
				}
				else
				{
					pstOptList->m_i8AppTag = BACNET_APPLICATION_TAG_CHARACTER_STRING;
					/* save the value */
					pstOptList->m_stCharStr.m_u32StrLen = (uint32_t)strnlen((const char *)pu8Token, MAX_CHARACTER_STRING_BYTES);
					memcpy(&pstOptList->m_stCharStr.m_pu8CharStr, pu8Token, pstOptList->m_stCharStr.m_u32StrLen);
				}
			}

			pvData = (void *)pstOptStrArr;
		}
		break;

		case BACNET_DT_BITSTRING_ARRAY:
		{
			/* local variables */
			Pr_ListOfBitStr_t *pstBitStrList = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfBitStr_t * pstInBitStrList = NULL;
			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstBitStrList = (Pr_ListOfBitStr_t *)malloc(sizeof(Pr_ListOfBitStr_t));

			if(NULL == pstBitStrList)
			{
				return NULL;
			}
			memset(pstBitStrList, 0, sizeof(Pr_ListOfBitStr_t));

			/* allocate memory */
			pstBitStrList->m_pstBitString = (ListOfBitStr_t *)malloc(sizeof(ListOfBitStr_t));
			pstInBitStrList = pstBitStrList->m_pstBitString;
			/* null check */
			if(NULL == pstInBitStrList)
			{
				return NULL;
			}
			memset(pstInBitStrList, 0, sizeof(ListOfBitStr_t));

			/* get the 1st value */
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,"@");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
			{
				Parse_BitString_From_Csv(pu8Token, &pstInBitStrList->m_stBitString);
			}

			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL,"@");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;

				/* allocate memory */
				pstInBitStrList->m_pstNext = (ListOfBitStr_t *)malloc(sizeof(ListOfBitStr_t));

				if(NULL == pstInBitStrList->m_pstNext)
				{
					break;
				}

				/* move to the new node */
				pstInBitStrList = pstInBitStrList->m_pstNext;
				memset(pstInBitStrList, 0, sizeof(ListOfBitStr_t));

				/* save the value */
				Parse_BitString_From_Csv(pu8Token, &pstInBitStrList->m_stBitString);
			}
			pvData = (void *)pstBitStrList;
		}
		break;

		case BACNET_DT_DATETIME:
		{
			/* local variables */
			uint32_t u32Hours = 0;
			uint32_t u32Mins = 0;
			uint32_t u32Secs = 0;
			uint32_t u32Hundreds = 0;
			uint32_t u32Year = 0;
			uint32_t u32Month = 0;
			uint32_t u32Date = 0;
			uint32_t u32WDay = 0;
			Pr_BACnetDateTime_t *pstPropDateTime = NULL;

			/* allocate memory */
			pstPropDateTime = (Pr_BACnetDateTime_t *)
				malloc(sizeof(Pr_BACnetDateTime_t));

			if(NULL == pstPropDateTime)
			{
				return NULL;
			}
			memset(pstPropDateTime, 0, sizeof(Pr_BACnetDateTime_t));

//			sscanf((const char*)pInData,"(%u/%u/%u:%u)(%u:%u:%u.%u)",
//				&u32Year, &u32Month, &u32Date, &u32WDay,
//				&u32Hours, &u32Mins, &u32Secs, &u32Hundreds);

			pstPropDateTime->m_stDateTime.m_stDate.m_u16Year = (uint16_t)u32Year;
			pstPropDateTime->m_stDateTime.m_stDate.m_u8Month = (uint8_t)u32Month;
			pstPropDateTime->m_stDateTime.m_stDate.m_u8Day = (uint8_t)u32Date;
			pstPropDateTime->m_stDateTime.m_stDate.m_u8Wday = (uint8_t)u32WDay;

			pstPropDateTime->m_stDateTime.m_stTime.m_u8Hour = (uint8_t)u32Hours;
			pstPropDateTime->m_stDateTime.m_stTime.m_u8Min = (uint8_t)u32Mins;
			pstPropDateTime->m_stDateTime.m_stTime.m_u8Sec = (uint8_t)u32Secs;
			pstPropDateTime->m_stDateTime.m_stTime.m_u8Hundredths = (uint8_t)u32Hundreds;

			pvData = (void *)pstPropDateTime;
		}
		break;

#if (defined BACDEL_PR12 || defined BACDEL_PR14)
		case BACNET_DT_EVENT_MSG_TEXT:
		{
			/* local variables */
			Pr_BACnetEventMsgText_t *pstEventMsgTxt = NULL;
			uint8_t u8Count = 0;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstEventMsgTxt = (Pr_BACnetEventMsgText_t *)malloc(sizeof(Pr_BACnetEventMsgText_t));

			if(NULL == pstEventMsgTxt)
			{
				return NULL;
			}
			memset(pstEventMsgTxt, 0, sizeof(Pr_BACnetEventMsgText_t));

			/* get the 1st value */
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,",");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
			{
				/* save value */
				pstEventMsgTxt->m_stEventMsgText[0].m_u32StrLen =
					(uint32_t)strnlen((const char *)pu8Token, MAX_CHARACTER_STRING_BYTES);
				memcpy(&pstEventMsgTxt->m_stEventMsgText[0].m_pu8CharStr, pu8Token,
					pstEventMsgTxt->m_stEventMsgText[u8Count].m_u32StrLen);
			}

			/* get the value */
			for(u8Count = 1; u8Count < BACNET_ARRAY_OF_THREE; u8Count++)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL,",");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;
				/* save value */
				pstEventMsgTxt->m_stEventMsgText[u8Count].m_u32StrLen =
					(uint32_t)strnlen((const char *)pu8Token, MAX_CHARACTER_STRING_BYTES);
				memcpy(&pstEventMsgTxt->m_stEventMsgText[u8Count].m_pu8CharStr, pu8Token,
					pstEventMsgTxt->m_stEventMsgText[u8Count].m_u32StrLen);
			}

			pvData = (void *)pstEventMsgTxt;
		}
		break;
#endif /* (PR12 || PR14) */

#if 0 //1
		case BACNET_DT_CLIENTCOVINCREMENT:
		{
			/* get value */
			pvData = Get_ClientCOVInc_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_DATERANGE:
		{
			/* get value */
			pvData = Get_DateRange_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_DATELIST:
		{
			/* get value */
			pvData = Get_DateList_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_OBJPROPREF:
		{
			/* get value */
			pvData = Get_ObjPropRef_value(u32Option, pstReturnType, pvInData);
		}
		break;

		case BACNET_DT_SETPOINTREF:
		{
			/* get value */
			pvData = Get_SetPointRef_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_DEVOBJPROPREF:
		{
			/* get value */
			pvData = Get_DevObjPropRef_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_DESTINATION_LIST:
		{
			/* get value */
			pvData = Get_DestinationList_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_DEVOBJPROPREFF_LIST:
		{
			/* get value */
			pvData = Get_DevObjPropRefList_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_DAILYSCHEDULE_ARRAY:
		{
			/* get value */
			pvData = Get_DailyScheduleArray_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_SPECIALEVENT_ARRAY:
		{
			/* get value */
			pvData = Get_SpecialEventArray_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
		{
			/* get value */
			pvData = Get_SchedulePresentDefault_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_EVENTPARAMETERS:
		{
			/* get value */
			pvData = Get_EventParameter_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_SERVICES_SUPPORTED:
		{
			/* get value */
			pvData = Get_ServiceSpported_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_OBJECTID_ARRAY:
		{
			/* get value */
			pvData = Get_ObjIDArray_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_OBJECT_TYPE_SUPPORTED:
		{
			/* get value */
			pvData = Get_ObjectTypeSpported_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_LOGBUFFER_TRENDLOG:
		{
			/* get value */
			pvData = Get_LogBuffer_value(u32Option, pstReturnType);
		}
		break;

#ifdef BACDEL_PR14
		case BACNET_DT_EVENT_NOTIFY_SUBS_LIST:
		{
			/* get value */
			pvData = Get_EventNotifySubsList_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_PROCESS_ID_SELECTION:
		{
			/* get value */
			pvData = Get_ProcessIdSelection_value(u32Option, pstReturnType, pInData);
		}
		break;

		case BACNET_DT_PORT_PERMISSION:
		{
			/* local variables */
			uint32_t u32Enabled = 0;
			uint32_t u32PortId = 0;
			Pr_ListOfBACnetPortPermission_t *pstPortPerm = NULL;

			/* allocate memory */
			pstPortPerm = (Pr_ListOfBACnetPortPermission_t *)
				malloc(sizeof(Pr_ListOfBACnetPortPermission_t));
			/* check null pointer */
			if(NULL == pstPortPerm)
			{
				return NULL;
			}
			memset(pstPortPerm, 0, sizeof(Pr_ListOfBACnetPortPermission_t));

			/* allocate memory */
			pstPortPerm->m_pstPortPermissionList = (ListOfBACnetPortPermission_t *)
				malloc(sizeof(ListOfBACnetPortPermission_t));
			/* check null pointer */
			if(NULL == pstPortPerm)
			{
				return NULL;
			}
			memset(pstPortPerm->m_pstPortPermissionList, 0, sizeof(ListOfBACnetPortPermission_t));

			/* get the value */
			sscanf((const char*)pInData,"(%u,%u)",
				&u32Enabled, &u32PortId);

			/* save value */
			pstPortPerm->m_pstPortPermissionList->m_stPortPermission.m_bEnabled = (bool)u32Enabled;
			pstPortPerm->m_pstPortPermissionList->m_stPortPermission.m_u8PortId = (uint8_t)u32PortId;

			/* return pointer value */
			pvData = (void *)pstPortPerm;
		}
		break;

		case BACNET_DT_PORT_PERMISSION_ARRAY:
		{
			/* local variables */
			uint32_t u32Enabled = 0;
			uint32_t u32PortId = 0;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfBACnetPortPermission_t *pstPortPerm = NULL;
			ListOfBACnetPortPermission_t *pstPrevious = NULL;
			Pr_ListOfBACnetPortPermission_t *pstPortPermList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstPortPermList = (Pr_ListOfBACnetPortPermission_t *)
				malloc(sizeof(Pr_ListOfBACnetPortPermission_t));

			/* check null pointer */
			if(NULL == pstPortPermList)
			{
				return NULL;
			}
			memset(pstPortPermList, 0, sizeof(Pr_ListOfBACnetPortPermission_t));

			/* get the 1st value */
			pu8Token = (uint8_t *)strtok ((char *)ai8Data,")");
			if(NULL == pu8Token)
			{
				return NULL;
			}

			/* allocate memory */
			pstPortPermList->m_pstPortPermissionList = (ListOfBACnetPortPermission_t *)
				malloc(sizeof(ListOfBACnetPortPermission_t));

			pstPortPerm = pstPortPermList->m_pstPortPermissionList;
			/* check null pointer */
			if(NULL == pstPortPerm)
			{
				return NULL;
			}
			memset(pstPortPerm, 0, sizeof(ListOfBACnetPortPermission_t));

			sscanf((const char *)pu8Token, "(%u,%u)", &u32Enabled, &u32PortId);

			/* save value */
			pstPortPerm->m_stPortPermission.m_bEnabled = (bool)u32Enabled;
			pstPortPerm->m_stPortPermission.m_u8PortId = (uint8_t)u32PortId;

			/* save previous */
			pstPrevious = pstPortPerm;

			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok (NULL," )");

				/* if no more values, then break the loop */
				if(NULL == pu8Token)
					break;

				/* allocate memory */
				pstPortPerm = (ListOfBACnetPortPermission_t *)
					malloc(sizeof(ListOfBACnetPortPermission_t));

				if(NULL == pstPortPerm)
				{
					return NULL;
				}
				memset(pstPortPerm, 0, sizeof(ListOfBACnetPortPermission_t));

				/* update linklist & previous pointer */
				pstPrevious->m_pstNext = pstPortPerm;
				pstPrevious = pstPortPerm;

				sscanf((const char *)pu8Token, "(%u,%u)", &u32Enabled, &u32PortId);

				/* save value */
				pstPortPerm->m_stPortPermission.m_bEnabled = (bool)u32Enabled;
				pstPortPerm->m_stPortPermission.m_u8PortId = (uint8_t)u32PortId;
			}

			/* return pointer value */
			pvData = (void *)pstPortPermList;
		}
		break;

		case BACNET_DT_CHANNEL_VALUE:
		{
			/* get value */
			pvData = Get_ChannelValue_value(u32Option, pstReturnType, pInData);
		}
		break;

		case BACNET_DT_FAULTPARAMETERS:
		{
			/* get value */
			pvData = Get_FaultParameter_value(u32Option, pstReturnType);
		}
		break;
#endif /* PR14 */

		case BACNET_DT_TIMESTAMP_ARRAY:
		{
			/* get value */
			pvData = Get_Timestamp_Array_value(u32Option, pstReturnType);
		}
		break;
#endif
		case BACNET_DT_PRIORITY_ARRAY:
		{
			/* local variables */
			uint32_t u32Cnt = 0;
			Pr_BACnetPriorityArray_t *pstPriorityArray = NULL;

			Float_t fArray[MAX_PRIORITY_ARRAY_COUNT] = {(Float_t)11.1, (Float_t)2.2, (Float_t)3.3, (Float_t)4.4, (Float_t)5.5, (Float_t)6.6,
				(Float_t)7.4, (Float_t)8.8, (Float_t)9.9, (Float_t)10.10, (Float_t)11, (Float_t)12, (Float_t)13, (Float_t)14, (Float_t)15};
			uint32_t u32Array[MAX_PRIORITY_ARRAY_COUNT] = {11,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
			BACNET_BINARY_PV bArray[MAX_PRIORITY_ARRAY_COUNT] = {BINARY_INACTIVE,BINARY_ACTIVE,BINARY_ACTIVE,
				BINARY_ACTIVE,BINARY_INACTIVE,BINARY_ACTIVE,BINARY_INACTIVE,BINARY_ACTIVE,BINARY_INACTIVE,
				BINARY_INACTIVE,BINARY_INACTIVE,BINARY_ACTIVE,BINARY_ACTIVE,BINARY_INACTIVE,BINARY_ACTIVE,BINARY_INACTIVE};

			/* allocate memory */
			pstPriorityArray = (Pr_BACnetPriorityArray_t *)Memory_Allocate(sizeof(Pr_BACnetPriorityArray_t), 0);
			/* validate null pointer */
			if(NULL == pstPriorityArray)
			{
				return NULL;
			}

			/* set to default value */
			memset(pstPriorityArray, 0, sizeof(Pr_BACnetPriorityArray_t));
			if(1 == u32Option)
			{
				for(u32Cnt = 0; u32Cnt < MAX_PRIORITY_ARRAY_COUNT; u32Cnt++)
				{
					pstPriorityArray->m_bTagType[u32Cnt] = BACNET_APPLICATION_TAG_REAL;
					pstPriorityArray->m_uValue.m_fVal[u32Cnt] = fArray[u32Cnt];
				}
			}
			else if(2 == u32Option)
			{
				for(u32Cnt = 0; u32Cnt < MAX_PRIORITY_ARRAY_COUNT; u32Cnt++)
				{
					pstPriorityArray->m_bTagType[u32Cnt] = BACNET_APPLICATION_TAG_UNSIGNED_INT;
					pstPriorityArray->m_uValue.m_u32Val[u32Cnt] = u32Array[u32Cnt];
				}
			}
			else if(3 == u32Option)
			{
				for(u32Cnt = 0; u32Cnt < MAX_PRIORITY_ARRAY_COUNT; u32Cnt++)
				{
					pstPriorityArray->m_bTagType[u32Cnt] = BACNET_APPLICATION_TAG_ENUMERATED;
					pstPriorityArray->m_uValue.m_eVal[u32Cnt] = bArray[u32Cnt];
				}
			}
			else
			{
				pstPriorityArray = NULL;
			}
			/* set value */
			pvData = pstPriorityArray;
		}
		break;

		case BACNET_DT_RECIPIENT_LIST:
		{
			/* local variables */
			Pr_ListOfBACnetRecipient_t *pstRecipientList = NULL;
			ListOfBACnetRecipient_t *pstListOfRecipient = NULL;

			#ifdef PX12268
			if(BACNET_ZERO == u32Option)
			{
				/* get property value from json file */
				pstRecipientList = (Pr_ListOfBACnetRecipient_t *)
					Json_Get_RecipientList_value();
			}
			else
			#endif
			{
			/* allocate memeory */
			pstRecipientList = (Pr_ListOfBACnetRecipient_t *)
				Memory_Allocate(sizeof(Pr_ListOfBACnetRecipient_t), 0);
			/* validate null pointer */
			if(NULL == pstRecipientList)
			{
				return NULL;
			}

			/* set to default value */
			memset(pstRecipientList, 0x00, sizeof(Pr_ListOfBACnetRecipient_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstRecipientList);
			}
			if(2 == u32Option)
			{
				/* allocate memory for recipient list */
				pstRecipientList->m_pstListOfRecipient = (ListOfBACnetRecipient_t *)
					Memory_Allocate(sizeof(ListOfBACnetRecipient_t), 0);
				/* validate null pointer */
				if(NULL == pstRecipientList->m_pstListOfRecipient)
				{
					return NULL;
				}

				pstRecipientList->m_u32Count = 1;
				/* save recipient list pointer value */
				pstListOfRecipient = pstRecipientList->m_pstListOfRecipient;
				/* set to default value */
				memset(pstListOfRecipient, 0x00, sizeof(ListOfBACnetRecipient_t));
				pstListOfRecipient->m_stRecipient.m_eDestinationType = DESTINATION_IS_DEVICE_ID;
				pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stObjId.m_eObjectType = OBJECT_DEVICE;
				pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stObjId.m_u32ObjId = 8;
			}
			/* unicast */
			else if(3 == u32Option)
			{
				/* allocate memory for recipient list */
				pstRecipientList->m_pstListOfRecipient = (ListOfBACnetRecipient_t *)
					Memory_Allocate(sizeof(ListOfBACnetRecipient_t), 0);
				/* validate null pointer */
				if(NULL == pstRecipientList->m_pstListOfRecipient)
				{
					return NULL;
				}

				pstRecipientList->m_u32Count = 1;
				/* save recipient list pointer value */
				pstListOfRecipient = pstRecipientList->m_pstListOfRecipient;
				/* set to default value */
				memset(pstListOfRecipient, 0x00, sizeof(ListOfBACnetRecipient_t));
				pstListOfRecipient->m_stRecipient.m_eDestinationType = DESTINATION_IS_IP_ADDR;

				#ifdef BACDEL_IPv6_STACK
				if(g_bIPv4orIPv6_App)
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 123;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 3;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16IPv6UdpPort = 47808;
					TestApi_GetIPv6Address(&pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress);
				}
				else
				#endif
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 123;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 6;
				}
			}
			/* Remote broadcast */
			else if(4 == u32Option)
			{
				/* allocate memory for recipient list */
				pstRecipientList->m_pstListOfRecipient = (ListOfBACnetRecipient_t *)
					Memory_Allocate(sizeof(ListOfBACnetRecipient_t), 0);
				/* validate null pointer */
				if(NULL == pstRecipientList->m_pstListOfRecipient)
				{
					return NULL;
				}

				pstRecipientList->m_u32Count = 1;
				/* save recipient list pointer value */
				pstListOfRecipient = pstRecipientList->m_pstListOfRecipient;
				/* set to default value */
				memset(pstListOfRecipient, 0x00, sizeof(ListOfBACnetRecipient_t));
				pstListOfRecipient->m_stRecipient.m_eDestinationType = DESTINATION_IS_IP_ADDR;

				#ifdef BACDEL_IPv6_STACK
				if(g_bIPv4orIPv6_App)
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 167;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 3;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 1;
				}
				else
				#endif
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 1;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 2;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 6;
				}
			}
			/* local broadcast */
			else if(5 == u32Option)
			{
				/* allocate memory for recipient list */
				pstRecipientList->m_pstListOfRecipient = (ListOfBACnetRecipient_t *)
					Memory_Allocate(sizeof(ListOfBACnetRecipient_t), 0);
				/* validate null pointer */
				if(NULL == pstRecipientList->m_pstListOfRecipient)
				{
					return NULL;
				}

				pstRecipientList->m_u32Count = 1;
				/* save recipient list pointer value */
				pstListOfRecipient = pstRecipientList->m_pstListOfRecipient;
				/* set to default value */
				memset(pstListOfRecipient, 0x00, sizeof(ListOfBACnetRecipient_t));
				pstListOfRecipient->m_stRecipient.m_eDestinationType = DESTINATION_IS_IP_ADDR;
				#ifdef BACDEL_IPv6_STACK
				if(g_bIPv4orIPv6_App)
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16IPv6UdpPort = 47808;
					TestApi_GetIPv6Address(&pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress);
				}
				else
				#endif
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 0;
				}
			}
			/* global broadcast */
			else if(6 == u32Option)
			{
				/* allocate memory for recipient list */
				pstRecipientList->m_pstListOfRecipient = (ListOfBACnetRecipient_t *)
					Memory_Allocate(sizeof(ListOfBACnetRecipient_t), 0);
				/* validate null pointer */
				if(NULL == pstRecipientList->m_pstListOfRecipient)
				{
					return NULL;
				}

				pstRecipientList->m_u32Count = 1;
				/* save recipient list pointer value */
				pstListOfRecipient = pstRecipientList->m_pstListOfRecipient;
				/* set to default value */
				memset(pstListOfRecipient, 0x00, sizeof(ListOfBACnetRecipient_t));
				pstListOfRecipient->m_stRecipient.m_eDestinationType = DESTINATION_IS_IP_ADDR;
				#ifdef BACDEL_IPv6_STACK
				if(g_bIPv4orIPv6_App)
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 65535;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16IPv6UdpPort = 47808;
					TestApi_GetIPv6Address(&pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress);
				}
				else
				#endif
				{
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[0] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[1] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[2] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[3] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[4] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8IpAddrs[5] = 0;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u16net = 65535;
					pstListOfRecipient->m_stRecipient.BACnetRecipient_u.m_stAddress.u8mac_len = 0;
				}
			}
			}

			/* return pointer value */
			return ((void *)pstRecipientList);
		}
		break;

#if 0 //def BACDEL_PR18

		case BACNET_DT_OPTIONAL_REAL:
 		{
			/* local variables */
			Pr_BACnetOptionalReal_t *pstRealValue = NULL;

			/* allocate memory */
			pstRealValue = (Pr_BACnetOptionalReal_t *)
				Memory_Allocate(sizeof(Pr_BACnetOptionalReal_t), 0);

			/* check null pointer */
			if(NULL == pstRealValue)
 			{
				return NULL;
 			}

			/* get value */
			if(0 == u32Option)
 			{
				pstRealValue->m_stOptionalReal.m_eDataType =
					BACNET_DT_NULL;
 			}
			else if(1 == u32Option)
 			{
				/* free allocated memory */
				Memory_Free((void **)&pstRealValue);
 			}
			else if(2 == u32Option)
			{
				pstRealValue->m_stOptionalReal.m_eDataType =
					BACNET_DT_REAL;
				pstRealValue->m_stOptionalReal.m_fVal =
					(Float_t)atof((const char *)pvInData);
			}
			else
 			{
				/* free allocated memory */
				Memory_Free((void **)&pstRealValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
 			}

			/* return pointer value */
			pvData = (void *)pstRealValue;
 		}
		break;

		case BACNET_DT_OPTIONAL_BINARY_PV:
 		{
			/* local variables */
			Pr_BACnetOptionalBinaryPV_t *pstBinaryValue = NULL;

			/* allocate memory */
			pstBinaryValue = (Pr_BACnetOptionalBinaryPV_t *)
				Memory_Allocate(sizeof(Pr_BACnetOptionalBinaryPV_t), 0);

			/* check null pointer */
			if(NULL == pstBinaryValue)
 			{
				return NULL;
 			}

			/* get value */
			if(0 == u32Option)
 			{
				pstBinaryValue->m_stOptionalBinaryPV.m_eDataType =
					BACNET_DT_NULL;
 			}
			else if(1 == u32Option)
 			{
				/* free allocated memory */
				Memory_Free((void **)&pstBinaryValue);
 			}
			else if(2 == u32Option)
			{
				pstBinaryValue->m_stOptionalBinaryPV.m_eDataType =
					BACNET_DT_ENUM_NEW;
				pstBinaryValue->m_stOptionalBinaryPV.m_eVal =
					(BACNET_BINARY_PV)atoi((const char *)pvInData);
			}
			else
 			{
				/* free allocated memory */
				Memory_Free((void **)&pstBinaryValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstBinaryValue;
 		}
		break;

		case BACNET_DT_OPTIONAL_UNSIGNED:
 		{
			/* local variables */
			Pr_BACnetOptionalUnsigned_t *pstUnsignedValue = NULL;

			/* allocate memory */
			pstUnsignedValue = (Pr_BACnetOptionalUnsigned_t *)
				Memory_Allocate(sizeof(Pr_BACnetOptionalUnsigned_t), 0);

			/* check null pointer */
			if(NULL == pstUnsignedValue)
 			{
				return NULL;
			}
			memset(pstUnsignedValue, 0, sizeof(Pr_BACnetOptionalUnsigned_t));

			/* get value */
			if(0 == u32Option)
			{
				pstUnsignedValue->m_stOptionalUnsigned.m_eDataType =
					BACNET_DT_NULL;
			}
			else if(1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstUnsignedValue);
			}
			else if(2 == u32Option)
			{
				pstUnsignedValue->m_stOptionalUnsigned.m_eDataType =
					BACNET_DT_UNSIGNED;
				pstUnsignedValue->m_stOptionalUnsigned.m_u32Val =
					(uint32_t)atoi((const char *)pvInData);
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstUnsignedValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstUnsignedValue;
 		}
		break;

		case BACNET_DT_REAL_ARRAY:
 		{
			/* local variables */
			Pr_ListOfReal_t *pstRealVal = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			ListOfReal_t *pstRealList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			/* allocate memory */
			pstRealVal = (Pr_ListOfReal_t *)Memory_Allocate(sizeof(Pr_ListOfReal_t), 0);
			if(NULL == pstRealVal)
			{
				return NULL;
			}

			pstRealVal->m_pstRealVal = (ListOfReal_t *)Memory_Allocate(sizeof(ListOfReal_t), 0);
			pstRealList = pstRealVal->m_pstRealVal;
			if(NULL == pstRealList)
			{
				return NULL;
			}

			/* get the 1st value */
			//pstUnsignVal->m_u32Value = atoi((const uint8_t* ) strtok (pInData,", "));
			pu8Token = (uint8_t *)strtok((char *)ai8Data, ",");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
				pstRealList->m_fValue = (Float_t)atof((const char *)pu8Token);

			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok(NULL, ",");

				/* if no more values, then break the loop */
				if (NULL == pu8Token)
					break;

				/* allocate memory */
				pstRealList->m_pstNext = (ListOfReal_t *)Memory_Allocate(sizeof(ListOfReal_t), 0);
				if(NULL == pstRealList->m_pstNext)
				{
					break;
				}
				/* move to the new node */
				pstRealList = pstRealList->m_pstNext;

				/* save the value */
				pstRealList->m_fValue = (Float_t)atof((const char *)pu8Token);
			}

			/* return pointer value */
			pvData = (void *)pstRealVal;
 		}
		break;

		case BACNET_DT_OCTETSTRING_ARRAY:
 		{
#if 0
			/* local variables */
			uint8_t i = 0, j = 0, k = 0;
			uint8_t u8ValArr1[110] = { 0 }, u8ValArr2[2] = { 0 }, u8StrLen = 0;
			Pr_BACnetOctetStr_t *pDataOctStr = NULL;

			pDataOctStr = (Pr_BACnetOctetStr_t *)Memory_Allocate(sizeof(Pr_BACnetOctetStr_t), 0);
			if(pDataOctStr == NULL)
			{
				Print_Msg("\n malloc failed \n");
				return NULL;
			}

			u8StrLen = strlen((const char*)pInData);
			for(i = 0; i<u8StrLen;)
			{
				if(*pInData != '(' && *pInData != ')'&& *pInData != ':')
				{
					u8ValArr1[i] = *pInData;
					i++;
				}
				pInData++;
				if(*pInData == ')')
					break;
			}

			for(j = 0; j<i;)
			{
				u8ValArr2[0] = u8ValArr1[j];
				u8ValArr2[1] = u8ValArr1[j + 1];
				pDataOctStr->m_stOctetString.m_ai8OctetStr[k] = (uint8_t)strtoul((const char *)&u8ValArr2[0], NULL, 16);
				k++;
				pDataOctStr->m_stOctetString.m_u32OctetCount = k;
				j += 2;
			}

			/* return pointer value */
			pvData = (void *)pDataOctStr;
#else

			/* local variables */
			Pr_ListOfOctetStr_t *pstOctetStrVal = NULL;
			void *pvReturnVal = NULL;
			uint8_t *pu8Token = NULL;
			int8_t ai8Data[MAX_APPLN_INPUT_STRLEN] = {0};
			uint8_t i = 0, j = 0, k = 0;
			uint8_t u8ValArr1[110] = { 0 }, u8ValArr2[2] = { 0 }, u8StrLen = 0;
			ListOfOctetStr_t *pstOctetStrList = NULL;

			memcpy(ai8Data, pInData, strnlen((const char *)pInData, MAX_APPLN_INPUT_STRLEN));

			//u8StrLen = strlen((const char*)pInData);

			pstOctetStrVal = (Pr_ListOfOctetStr_t *)Memory_Allocate(sizeof(Pr_ListOfOctetStr_t), 0);
			if(pstOctetStrVal == NULL)
			{
				return NULL;
			}

			pstOctetStrVal->m_pstOctetStrVal = (ListOfOctetStr_t *)Memory_Allocate(sizeof(ListOfOctetStr_t), 0);
			pstOctetStrList = pstOctetStrVal->m_pstOctetStrVal;
			if(NULL == pstOctetStrList)
			{
				return NULL;
			}

			/* get the 1st value */
			pu8Token = (uint8_t *)strtok((char *)ai8Data, ",");
			if(NULL == pu8Token)
			{
				return NULL;
			}
			else
			{
				u8StrLen = strlen((const char*)pu8Token);
				for(i = 0; i<u8StrLen;)
				{
					if(*pInData != '(' && *pInData != ')'&& *pInData != ':' && *pInData != ',' && *pInData != '"')
					{
						u8ValArr1[i] = *pInData;
						i++;
					}
					pInData++;
					if(*pInData == ')')
						break;
				}
				for(j = 0; j<i;)
			  	{
					u8ValArr2[0] = u8ValArr1[j];
					u8ValArr2[1] = u8ValArr1[j + 1];
					pstOctetStrList->stOctetString.m_ai8OctetStr[k] = (uint8_t)strtoul((const char *)&u8ValArr2[0], NULL, 16);
					k++;
					pstOctetStrList->stOctetString.m_u32OctetCount = k;
					j += 2;
				}
			}

			/* get all other value to add to list */
			while(NULL != pu8Token)
			{
				/* get the string to extract value */
				pu8Token = (uint8_t *)strtok(NULL, ",");

				/* if no more values, then break the loop */
				if (NULL == pu8Token)
					break;

				/* allocate memory */
				pstOctetStrList->m_pstNext = (ListOfOctetStr_t *)Memory_Allocate(sizeof(ListOfOctetStr_t), 0);
				if(NULL == pstOctetStrList->m_pstNext)
				{
					break;
				}
				pstOctetStrList = pstOctetStrList->m_pstNext;

				u8StrLen = strlen((const char*)pu8Token);
				for(i = 0; i<u8StrLen;)
				{
					if(*pInData != '(' && *pInData != ')'&& *pInData != ':' && *pInData != ',')
					{
						u8ValArr1[i] = *pInData;
						i++;
					}
					pInData++;
					if(*pInData == ')')
						break;
				}

				/* set to zero */
				k = 0;

				for(j = 0; j<i;)
			  	{
					u8ValArr2[0] = u8ValArr1[j];
					u8ValArr2[1] = u8ValArr1[j + 1];
					pstOctetStrList->stOctetString.m_ai8OctetStr[k] = (uint8_t)strtoul((const char *)&u8ValArr2[0], NULL, 16);
					k++;
					pstOctetStrList->stOctetString.m_u32OctetCount = k;
					j += 2;
				}
			}
			/* return pointer value */
			pvData = (void *)pstOctetStrVal;
#endif
 		}
		break;

		case BACNET_DT_TIMESTAMP16_ARRAY:
 		{
			/* get value */
			pvData = Get_CmdTimeArray_value(u32Option, pstReturnType);
 		}
		break;

		case BACNET_DT_VALUE_SOURCE:
		{
			/* local variables */
			Pr_BACnetValueSource_t *pstValueSrc = NULL;

			/* allocate memory */
			pstValueSrc = (Pr_BACnetValueSource_t *)
				Memory_Allocate(sizeof(Pr_BACnetValueSource_t), 0);
			/* check null pointer */
			if(NULL == pstValueSrc)
			{
				return NULL;
			}

			/* get value */
			if(0 == u32Option)
 			{
				pstValueSrc->m_stValueSource.m_eDataType = BACNET_DT_NULL;
 			}
			if(1 == u32Option)
 			{
				/* free allocated memory */
				Memory_Free((void **)&pstValueSrc);
 			}
			else if(2 == u32Option)
			{
				pstValueSrc->m_stValueSource.m_eDataType = BACNET_DT_DEVOBJREFF;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_bDeviceIdPresent = false;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_eDeviceType = OBJECT_DEVICE;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_u32DeviceId = 25;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_eObjectType = OBJECT_ANALOG_OUTPUT;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_u32ObjId = 150000;
			}
			else if(3 == u32Option)
			{
				pstValueSrc->m_stValueSource.m_eDataType = BACNET_DT_DEVOBJREFF;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_bDeviceIdPresent = true;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_eDeviceType = OBJECT_DEVICE;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_u32DeviceId = 25;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_eObjectType = OBJECT_BINARY_OUTPUT;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stObject.m_u32ObjId = 150;
			}
			else if(4 == u32Option)
			{
				pstValueSrc->m_stValueSource.m_eDataType = BACNET_DT_ADDRESS;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u16net = 4;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8dlen = 0;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8IpAddrs[0] = 10;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8IpAddrs[1] = 20;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8IpAddrs[2] = 30;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8IpAddrs[3] = 40;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8IpAddrs[4] = 50;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8IpAddrs[5] = 60;
				pstValueSrc->m_stValueSource.u_ValueSource.m_stAddress.u8mac_len = 6;
			}
			else
 			{
				/* free allocated memory */
				Memory_Free((void **)&pstValueSrc);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
 			}

			/* return pointer value */
			pvData = (void *)pstValueSrc;
		}
		break;

		case BACNET_DT_VALUE_SOURCE_ARRAY:
		{
			/* get value */
			pvData = Get_ValueSourceArray_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_TIMER_STATE_CHANGE_VAL_ARRAY:
		{
			/* get value */
			pvData = Get_TimerStateChngArray_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_LANDING_CALL_STATUS:
		{
			/* local variables */
			Pr_BACnetLandingCallStatus_t *pstValue = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetLandingCallStatus_t *)
				Memory_Allocate(sizeof(Pr_BACnetLandingCallStatus_t), 0);
			/* check null pointer */
			if(NULL == pstValue)
			{
				return NULL;
			}

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stLandingCallStatus.m_u8FloorNo = 4;
				pstValue->m_stLandingCallStatus.m_u8CmdType = 1;
				pstValue->m_stLandingCallStatus.Command_u.m_eDirection = LIFT_CAR_DIRECTION_UP;
				pstValue->m_stLandingCallStatus.m_bFloorTextPresent = false;
			}
			else if(3 == u32Option)
			{
				pstValue->m_stLandingCallStatus.m_u8FloorNo = 255;
				pstValue->m_stLandingCallStatus.m_u8CmdType = 1;
				pstValue->m_stLandingCallStatus.Command_u.m_eDirection = LIFT_CAR_DIRECTION_DOWN;
				pstValue->m_stLandingCallStatus.m_bFloorTextPresent = true;

				memcpy(&pstValue->m_stLandingCallStatus.m_stFloorText.m_pu8CharStr, "abc123", 6);
				pstValue->m_stLandingCallStatus.m_stFloorText.m_u16CodePage = 850;
				pstValue->m_stLandingCallStatus.m_stFloorText.m_u32StrLen = 6;
				pstValue->m_stLandingCallStatus.m_stFloorText.m_u8Encoding = 0;
			}
			else if(4 == u32Option)
			{
				pstValue->m_stLandingCallStatus.m_u8FloorNo = 33;
				pstValue->m_stLandingCallStatus.m_u8CmdType = 2;
				pstValue->m_stLandingCallStatus.Command_u.m_u8Destination = 27;
				pstValue->m_stLandingCallStatus.m_bFloorTextPresent = true;

				memcpy(&pstValue->m_stLandingCallStatus.m_stFloorText.m_pu8CharStr, "abc", 3);
				pstValue->m_stLandingCallStatus.m_stFloorText.m_u16CodePage = 850;
				pstValue->m_stLandingCallStatus.m_stFloorText.m_u32StrLen = 3;
				pstValue->m_stLandingCallStatus.m_stFloorText.m_u8Encoding = 1;
			}
			else if(5 == u32Option)
			{
				pstValue->m_stLandingCallStatus.m_u8FloorNo = 127;
				pstValue->m_stLandingCallStatus.m_u8CmdType = 2;
				pstValue->m_stLandingCallStatus.Command_u.m_eDirection = LIFT_CAR_DIRECTION_UP_AND_DOWN;
				pstValue->m_stLandingCallStatus.m_bFloorTextPresent = false;
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstValue;
		}
		break;

		case BACNET_DT_LANDING_CALL_STATUS_LIST:
		{
			/* get value */
			pvData = Get_LandingCallStatus_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_ASSIGNED_LANDING_CALL:
		{
			/* local variables */
			Pr_BACnetAssignedLandingCalls_t *pstValue = NULL;
			ListOfLandingCalls_t *pstLandingCallList1 = NULL;
			ListOfLandingCalls_t *pstLandingCallList2 = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetAssignedLandingCalls_t *)
				malloc(sizeof(Pr_BACnetAssignedLandingCalls_t));

			/* check null pointer */
			if(NULL == pstValue)
			{
				return NULL;
			}
			memset(pstValue, 0, sizeof(Pr_BACnetAssignedLandingCalls_t));

			/* allocate memeory */
			pstValue->m_stAssLandCallsList.m_pstLandingCallsList = (ListOfLandingCalls_t *)
				malloc(sizeof(ListOfLandingCalls_t));
			pstLandingCallList1 = pstValue->m_stAssLandCallsList.m_pstLandingCallsList;
			if(NULL == pstLandingCallList1)
			{
				Memory_Free((void **)&pstValue);
				return NULL;
			}
			/* set to default value */
			memset(pstLandingCallList1, 0x00, sizeof(ListOfLandingCalls_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				Memory_Free((void **)&pstLandingCallList1);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stAssLandCallsList.m_u32ArraySize = 1;
				pstLandingCallList1->stLandingCalls.m_eCarDirection = LIFT_CAR_DIRECTION_STOPPED;
				pstLandingCallList1->stLandingCalls.m_u8FloorNo = 199;
			}
			else if(3 == u32Option)
			{
				pstValue->m_stAssLandCallsList.m_u32ArraySize = 2;
				pstLandingCallList1->stLandingCalls.m_eCarDirection = LIFT_CAR_DIRECTION_UP_AND_DOWN;
				pstLandingCallList1->stLandingCalls.m_u8FloorNo = 5;

				/* allocate memeory */
				pstLandingCallList1->m_pstNext = (ListOfLandingCalls_t *)
					malloc(sizeof(ListOfLandingCalls_t));
				pstLandingCallList2 = pstLandingCallList1->m_pstNext;
				if(NULL == pstLandingCallList2)
				{
					Memory_Free((void **)&pstValue);
					Memory_Free((void **)&pstLandingCallList1);
					return NULL;
				}
				/* set to default value */
				memset(pstLandingCallList2, 0x00, sizeof(ListOfLandingCalls_t));

				pstLandingCallList2->stLandingCalls.m_eCarDirection = LIFT_CAR_DIRECTION_NONE;
				pstLandingCallList2->stLandingCalls.m_u8FloorNo = 3;
			}
			else if(4 == u32Option)
			{
				pstValue->m_stAssLandCallsList.m_u32ArraySize = 0;
				pstValue->m_stAssLandCallsList.m_pstLandingCallsList = NULL;
				Memory_Free((void **)&pstLandingCallList1);
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstLandingCallList1);
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			return ((void *)pstValue);
		}
		break;

		case BACNET_DT_ASSIGNED_LANDING_CALL_ARRAY:
		{
			/* get value */
			pvData = Get_AssignedLandingCallArr_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_LIFT_CAR_CALL_LIST:
		{
			/* local variables */
			Pr_BACnetLiftCarCallList_t *pstValue = NULL;
			ListOfUnsigned_t *pstList1 = NULL;
			ListOfUnsigned_t *pstList2 = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetLiftCarCallList_t *)
				malloc(sizeof(Pr_BACnetLiftCarCallList_t));

			/* check null pointer */
			if(NULL == pstValue)
			{
				return NULL;
			}
			memset(pstValue, 0, sizeof(Pr_BACnetLiftCarCallList_t));

			/* allocate memeory */
			pstValue->m_stLiftCarCallsList.m_pstFloorNosList = (ListOfUnsigned_t *)
				malloc(sizeof(ListOfUnsigned_t));
			pstList1 = pstValue->m_stLiftCarCallsList.m_pstFloorNosList;
			if(NULL == pstList1)
			{
				Memory_Free((void **)&pstValue);
				return NULL;
			}
			/* set to default value */
			memset(pstList1, 0x00, sizeof(ListOfUnsigned_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				Memory_Free((void **)&pstList1);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stLiftCarCallsList.m_u32ArraySize = 1;
				pstList1->m_u32Value = 56;
			}
			else if(3 == u32Option)
			{
				pstValue->m_stLiftCarCallsList.m_u32ArraySize = 2;
				pstList1->m_u32Value = 250;

				/* allocate memeory */
				pstList1->m_pstNext = (ListOfUnsigned_t *)
					malloc(sizeof(ListOfUnsigned_t));
				pstList2 = pstList1->m_pstNext;
				if(NULL == pstList2)
				{
					Memory_Free((void **)&pstValue);
					Memory_Free((void **)&pstList1);
					return NULL;
				}
				/* set to default value */
				memset(pstList2, 0x00, sizeof(ListOfUnsigned_t));
				pstList2->m_u32Value = 4568;
			}
			else if(4 == u32Option)
			{
				pstValue->m_stLiftCarCallsList.m_u32ArraySize = 0;
				pstValue->m_stLiftCarCallsList.m_pstFloorNosList = NULL;
				Memory_Free((void **)&pstList1);
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstList1);
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			return ((void *)pstValue);
		}
		break;

		case BACNET_DT_LIFT_CAR_CALL_LIST_ARRAY:
		{
			/* get value */
			pvData = Get_CarCallListArr_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_LANDING_DOOR_STATUS:
		{
			/* local variables */
			Pr_BACnetLandingDoorStatus_t *pstValue = NULL;
			ListOfLandingDoors_t *pstList1 = NULL;
			ListOfLandingDoors_t *pstList2 = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetLandingDoorStatus_t *)
				malloc(sizeof(Pr_BACnetLandingDoorStatus_t));

			/* check null pointer */
			if(NULL == pstValue)
			{
				return NULL;
			}
			memset(pstValue, 0, sizeof(Pr_BACnetLandingDoorStatus_t));

			/* allocate memeory */
			pstValue->stLandingDoorsStatus.m_pstLandingDoorsList = (ListOfLandingDoors_t *)
				malloc(sizeof(ListOfLandingDoors_t));
			pstList1 = pstValue->stLandingDoorsStatus.m_pstLandingDoorsList;
			if(NULL == pstList1)
			{
				Memory_Free((void **)&pstValue);
				return NULL;
			}
			/* set to default value */
			memset(pstList1, 0x00, sizeof(ListOfLandingDoors_t));

			/* get value */
			if (0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				Memory_Free((void **)&pstList1);
			}
			else if(2 == u32Option)
			{
				pstValue->stLandingDoorsStatus.m_u32ArraySize = 1;
				pstList1->stLandingDoors.m_u8FloorNo = 111;
				pstList1->stLandingDoors.m_eDoorStatus = DOOR_STATUS_CLOSING;
			}
			else if(3 == u32Option)
			{
				pstValue->stLandingDoorsStatus.m_u32ArraySize = 2;
				pstList1->stLandingDoors.m_u8FloorNo = 66;
				pstList1->stLandingDoors.m_eDoorStatus = DOOR_STATUS_LIMITED_OPENED;

				/* allocate memeory */
				pstList1->m_pstNext = (ListOfLandingDoors_t *)
					malloc(sizeof(ListOfLandingDoors_t));
				pstList2 = pstList1->m_pstNext;
				if(NULL == pstList2)
				{
					Memory_Free((void **)&pstList1);
					Memory_Free((void **)&pstValue);
					return NULL;
				}
				/* set to default value */
				memset(pstList2, 0x00, sizeof(ListOfLandingDoors_t));
				pstList2->stLandingDoors.m_u8FloorNo = 123;
				pstList2->stLandingDoors.m_eDoorStatus = DOOR_STATUS_SAFETY_LOCKED;
			}
			else if(4 == u32Option)
			{
				pstValue->stLandingDoorsStatus.m_u32ArraySize = 0;
				pstValue->stLandingDoorsStatus.m_pstLandingDoorsList = NULL;
				Memory_Free((void **)&pstList1);
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstList1);
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			return ((void *)pstValue);
		}
		break;

		case BACNET_DT_LANDING_DOOR_STATUS_ARRAY:
		{
			/* get value */
			pvData = Get_LandingDoorStatusArr_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_HOSTNPORT:
		{
			/* local variables */
			Pr_BACnetHostNPort_t *pstValue = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetHostNPort_t *)
				malloc(sizeof(Pr_BACnetHostNPort_t));

			/* check null pointer */
			if(NULL == pstValue)
			{
				return NULL;
			}
			memset(pstValue, 0, sizeof(Pr_BACnetHostNPort_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stHostNPort.m_u16Port = 5;
				memcpy(&pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stName.m_pu8CharStr, "abcd123", 7);
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u16CodePage = 850;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u32StrLen = 7;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u8Encoding = 0;
				pstValue->m_stHostNPort.m_stHostAddress.m_eDataType = BACNET_DT_CHARSTRING;
			}
			else if(3 == u32Option)
			{
				pstValue->m_stHostNPort.m_u16Port = 8;
				pstValue->m_stHostNPort.m_stHostAddress.m_eDataType = BACNET_DT_OCTETSTRING;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[0] = 00;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[1] = 11;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[2] = 01;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[3] = 12;
				pstValue->m_stHostNPort.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_u32OctetCount = 4;
			}
			else if(4 == u32Option)
			{
				pstValue->m_stHostNPort.m_u16Port = 2;
				pstValue->m_stHostNPort.m_stHostAddress.m_eDataType = BACNET_DT_NULL;
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstValue;
		}
		break;

		case BACNET_DT_BDT_ENTRY:
		{
			/* local variables */
			Pr_BACnetBDTEntry_t *pstValue = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetBDTEntry_t *)
				malloc(sizeof(Pr_BACnetBDTEntry_t));

			/* check null pointer */
			if (NULL == pstValue)
			{
				return NULL;
			}
			memset(pstValue, 0, sizeof(Pr_BACnetBDTEntry_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_u16Port = 222;
				memcpy(&pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_pu8CharStr, "host", 4);
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u16CodePage = 850;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u32StrLen = 4;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u8Encoding = 0;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.m_eDataType = BACNET_DT_CHARSTRING;
				pstValue->m_stBDTEntry.m_bBrdcastMaskFlag = false;
			}
			else if(3 == u32Option)
			{
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_u16Port = 8888;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.m_eDataType = BACNET_DT_OCTETSTRING;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[0] = 00;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[1] = 11;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[2] = 01;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[3] = 12;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_u32OctetCount = 4;
				pstValue->m_stBDTEntry.m_bBrdcastMaskFlag = false;
			}
			else if(4 == u32Option)
			{
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_u16Port = 55555;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.m_eDataType = BACNET_DT_NULL;
				pstValue->m_stBDTEntry.m_bBrdcastMaskFlag = false;
			}
			else if(5 == u32Option)
			{
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_u16Port = 12;
				memcpy(&pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_pu8CharStr, "softdel", 7);
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u16CodePage = 850;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u32StrLen = 7;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stName.m_u8Encoding = 1;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.m_eDataType = BACNET_DT_CHARSTRING;
				pstValue->m_stBDTEntry.m_bBrdcastMaskFlag = true;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[0] = 255;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[1] = 255;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[2] = 0;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[3] = 0;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_u32OctetCount = 4;
			}
			else if(6 == u32Option)
			{
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_u16Port = 123;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.m_eDataType = BACNET_DT_OCTETSTRING;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[0] = 00;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[1] = 11;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[2] = 01;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_ai8OctetStr[3] = 12;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.BACnetHostAddr_u.m_stIPAddress.m_u32OctetCount = 4;
				pstValue->m_stBDTEntry.m_bBrdcastMaskFlag = true;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[0] = 255;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[1] = 0;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[2] = 0;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[3] = 0;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_u32OctetCount = 4;
			}
			else if(7 == u32Option)
			{
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_u16Port = 1234;
				pstValue->m_stBDTEntry.m_stBBMDAddress.m_stHostAddress.m_eDataType = BACNET_DT_NULL;
				pstValue->m_stBDTEntry.m_bBrdcastMaskFlag = true;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[0] = 255;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[1] = 255;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[2] = 255;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_ai8OctetStr[3] = 0;
				pstValue->m_stBDTEntry.m_stBrdcastMask.m_u32OctetCount = 3;
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
 			}

			/* return pointer value */
			pvData = (void *)pstValue;
 		}
 		break;

		case BACNET_DT_BDT_ENTRY_LIST:
		{
			/* get value */
			pvData = Get_BDTEntryList_value(u32Option, pstReturnType);
		}
		break;

		case BACNET_DT_FDT_ENTRY:
		{
			/* local variables */
			Pr_BACnetFDTEntry_t *pstValue = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetFDTEntry_t *)
				malloc(sizeof(Pr_BACnetFDTEntry_t));

			/* check null pointer */
			if(NULL == pstValue)
 			{
				return NULL;
 			}
			memset(pstValue, 0, sizeof(Pr_BACnetFDTEntry_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stFDTEntry.m_u16TimeToLive = 5000;
				pstValue->m_stFDTEntry.m_u16RemainingTimeToLive = 6000;
				pstValue->m_stFDTEntry.m_stIPAddress.m_ai8OctetStr[0] = 00;
				pstValue->m_stFDTEntry.m_stIPAddress.m_ai8OctetStr[1] = 11;
				pstValue->m_stFDTEntry.m_stIPAddress.m_ai8OctetStr[2] = 22;
				pstValue->m_stFDTEntry.m_stIPAddress.m_ai8OctetStr[3] = 33;
				pstValue->m_stFDTEntry.m_stIPAddress.m_ai8OctetStr[4] = 44;
				pstValue->m_stFDTEntry.m_stIPAddress.m_ai8OctetStr[4] = 55;
				pstValue->m_stFDTEntry.m_stIPAddress.m_u32OctetCount = 6;
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstValue;
 		}
		break;

		case BACNET_DT_FDT_ENTRY_LIST:
 		{
			/* get value */
			pvData = Get_FDTEntryList_value(u32Option, pstReturnType);
 		}
		break;

		case BACNET_DT_VMAC_ENTRY:
		{
			/* local variables */
			Pr_BACnetVMACEntry_t *pstValue = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetVMACEntry_t *)
				malloc(sizeof(Pr_BACnetVMACEntry_t));

			/* check null pointer */
			if(NULL == pstValue)
 			{
				return NULL;
 			}
			memset(pstValue, 0, sizeof(Pr_BACnetVMACEntry_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_ai8OctetStr[0] = 10;
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_ai8OctetStr[1] = 20;
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_ai8OctetStr[2] = 30;
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_ai8OctetStr[3] = 40;
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_ai8OctetStr[4] = 50;
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_ai8OctetStr[5] = 60;
				pstValue->m_stVMACEntry.m_stVirtualMacAddr.m_u32OctetCount = 6;

				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[0] = 10;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[1] = 20;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[2] = 30;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[3] = 40;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[4] = 50;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[5] = 60;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[6] = 70;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[7] = 80;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[8] = 90;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_ai8OctetStr[9] = 100;
				pstValue->m_stVMACEntry.m_stNativeMacAddr.m_u32OctetCount = 10;
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstValue;
 		}
		break;

		case BACNET_DT_VMAC_ENTRY_LIST:
 		{
			/* get value */
			pvData = Get_VMACEntryList_value(u32Option, pstReturnType);
 		}
		break;

		case BACNET_DT_ROUTER_ENTRY:
		{
			/* local variables */
			Pr_BACnetRouterEntry_t *pstValue = NULL;

			/* allocate memory */
			pstValue = (Pr_BACnetRouterEntry_t *)
				malloc(sizeof(Pr_BACnetRouterEntry_t));

			/* check null pointer */
			if(NULL == pstValue)
 			{
				return NULL;
 			}
			memset(pstValue, 0, sizeof(Pr_BACnetRouterEntry_t));

			/* get value */
			if(0 == u32Option || 1 == u32Option)
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
			}
			else if(2 == u32Option)
			{
				pstValue->m_stRouterEntry.m_eRouterStatus = ROUTER_STATUS_AVAILABLE;
				pstValue->m_stRouterEntry.m_bIsPerformanceIdx = true;
				pstValue->m_stRouterEntry.m_u8PerformanceIdx = 100;
				pstValue->m_stRouterEntry.m_u16NetworkNumber = 123;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[0] = 10;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[1] = 20;
				pstValue->m_stRouterEntry.m_stMacAddress.m_u32OctetCount = 2;
			}
			else if(3 == u32Option)
			{
				pstValue->m_stRouterEntry.m_eRouterStatus = ROUTER_STATUS_BUSY;
				pstValue->m_stRouterEntry.m_bIsPerformanceIdx = false;
				pstValue->m_stRouterEntry.m_u16NetworkNumber = 12345;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[0] = 10;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[1] = 20;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[2] = 10;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[3] = 20;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[4] = 10;
				pstValue->m_stRouterEntry.m_stMacAddress.m_ai8OctetStr[5] = 20;
				pstValue->m_stRouterEntry.m_stMacAddress.m_u32OctetCount = 6;
			}
			else
			{
				/* free allocated memory */
				Memory_Free((void **)&pstValue);
				*pstReturnType = BACDEL_ERROR;
				return NULL;
			}

			/* return pointer value */
			pvData = (void *)pstValue;
 		}
		break;

		case BACNET_DT_ROUTER_ENTRY_LIST:
 		{
			/* get value */
			pvData = Get_RouterEntryList_value(u32Option, pstReturnType);
 		}
		break;

		case BACNET_DT_COV_MULTIPLE_SUBS_LIST:
		{
			/* get value */
			pvData = Get_COVMultipleSubs_value(u32Option, pstReturnType);
		}
		break;

#endif /* PR18 */

		/* default case */
        default:
        {
            /* For remain properties send void pointer as it is */
			pvData = NULL;
        }
        break;
    }/* End of Switch Case */

	return pvData;
}
#if 0
/* Function to typecast data types & display values on console */
void DataType_Typecast(void* pvPropVal, BACNET_DATA_TYPE eDataType)
{
	uint32_t u32Val = 0;


	/* check input pointer */
	if(NULL == pvPropVal && BACNET_DT_NULL != eDataType &&
		BACNET_DT_EMPTY != eDataType)
	{
		return;
	}

	switch(eDataType)
	{
	case BACNET_DT_REAL:
		{
			Pr_BACnetReal_t *pReal;
			pReal = (Pr_BACnetReal_t*)pvPropVal;
		}
		break;

	case BACNET_DT_INTEGER:
		{
			Pr_BACnetSigned32_t *pInteger;
			pInteger = (Pr_BACnetSigned32_t*)pvPropVal;
		}
		break;

	case BACNET_DT_BITSTRING:
		{
			uint32_t u32i = 0;
			Pr_BACnetBitStr_t *pBitstr;
			pBitstr = (Pr_BACnetBitStr_t *)pvPropVal;
		}
		break;

	case BACNET_DT_ENUM_NEW:
		{
			Pr_BACnetEnum_t *pEumNew;
			pEumNew = (Pr_BACnetEnum_t *)pvPropVal;
		}
		break;

	case BACNET_DT_BOOLEAN:
		{
			Pr_BACnetBool_t *pBval;
			pBval = (Pr_BACnetBool_t *)pvPropVal;
		}
		break;

	case BACNET_DT_CHARSTRING:
		{
			Pr_BACnetCharStr_t *pCharstr;
			pCharstr = (Pr_BACnetCharStr_t *)pvPropVal;
		}
		break;

	case BACNET_DT_UNSIGNED:
		{
			Pr_BACnetUnsigned32_t *pUnsiged;
			pUnsiged = (Pr_BACnetUnsigned32_t *)pvPropVal;
		}
		break;

	case BACNET_DT_TIMESTAMP_ARRAY:
		{
			Pr_BACnetEventTimeStamp_t *pTimeStamp;
			uint32_t u32count = 0;
			pTimeStamp = (Pr_BACnetEventTimeStamp_t *)pvPropVal;
		}
		break;

	case BACNET_DT_UNSIGNED16:
		{
			Pr_BACnetUnsigned16_t *pUnsigned16;
			pUnsigned16 = (Pr_BACnetUnsigned16_t *)pvPropVal;
		}
		break;

	case BACNET_DT_ENUM:
		{
			Pr_BinaryEnumPV_t *pEnum;
			pEnum = (Pr_BinaryEnumPV_t *)pvPropVal;
		}
		break;

	case BACNET_DT_UNSIGNED32:
		{
			Pr_BACnetUnsigned32_t *pUnsiged;
			pUnsiged = (Pr_BACnetUnsigned32_t *)pvPropVal;
		}
		break;

	case BACNET_DT_DOUBLE:
		{
			Pr_BACnetDouble_t *pDouble;
			pDouble = (Pr_BACnetDouble_t *)pvPropVal;
		}
		break;

	case BACNET_DT_TIME:
		{
			Pr_BACnetTime_t *pTimedef;
			pTimedef = (Pr_BACnetTime_t *)pvPropVal;
		}
		break;

	case BACNET_DT_DATE:
		{
			Pr_BACnetDate_t *pDate;
			pDate = (Pr_BACnetDate_t *)pvPropVal;
		}
		break;

	case BACNET_DT_DATETIME:
		{
			Pr_BACnetDateTime_t *pDateTime;
			pDateTime = (Pr_BACnetDateTime_t *)pvPropVal;
		}
		break;

	case BACNET_DT_CHARSTRING_ARRAY:
		{
			Pr_ListOfCharStr_t *pStrArr;
			ListOfCharStr_t *pstString;
			pStrArr = (Pr_ListOfCharStr_t *)pvPropVal;
			pstString = &pStrArr->m_stStringVal;
		}
		break;

	case BACNET_DT_OCTETSTRING:
		{
			uint32_t u32i = 0;
			Pr_BACnetOctetStr_t *pOctStr;
			pOctStr = (Pr_BACnetOctetStr_t *)pvPropVal;
		}
		break;

	case BACNET_DT_UNSIGNED_LIST:
	case BACNET_DT_UNSIGNED_ARRAY:
		{
			Pr_ListOfUnsigned_t *pUnsignArr;
			ListOfUnsigned_t *pstIntlist;
			pUnsignArr = (Pr_ListOfUnsigned_t *)pvPropVal;
			pstIntlist = pUnsignArr->m_pstUnsignVal;
		}
		break;

	case BACNET_DT_OBJECTID:
		{
			Pr_BACnetObjId_t *pObjId;
			pObjId = (Pr_BACnetObjId_t *)pvPropVal;
		}
		break;

	case BACNET_DT_OBJTYPE:
		{
			Pr_BACnetObjType_t *pObjType;
			pObjType = (Pr_BACnetObjType_t *)pvPropVal;
		}
		break;

	case BACNET_DT_BACNETDEVSTAT:
		{
			Pr_BACnetDevStatus_t *pDevStat;
			pDevStat = (Pr_BACnetDevStatus_t *)pvPropVal;
		}
		break;

	case BACNET_DT_SERVICES_SUPPORTED:
		{
			uint32_t u32i = 0;
			Pr_BACnetServicesSupported_t *pSerSupp;
			pSerSupp = (Pr_BACnetServicesSupported_t *)pvPropVal;
		}
		break;

	case BACNET_DT_OBJECT_TYPE_SUPPORTED:
		{
			uint32_t u32i = 0;
			Pr_BACnetObjectTypesSupported_t *pObjSupp;
			pObjSupp = (Pr_BACnetObjectTypesSupported_t *)pvPropVal;
		}
		break;

	case BACNET_DT_OBJECTID_ARRAY:
		{
			Pr_ListOfObjId_t *pstListObjeID = NULL;
			ListOfObjId_t *pstObjId;
			pstListObjeID = (Pr_ListOfObjId_t *)pvPropVal;
			pstObjId = pstListObjeID->m_pstArrayObjId;
		}
		break;

	case BACNET_DT_BACNETSEG:
		{
			Pr_BACnetSegmentation_t *pSegSupp;
			pSegSupp = (Pr_BACnetSegmentation_t *)pvPropVal;
			u32Val = 0;
		}
		break;

	case BACNET_DT_BACNETVTCLASS:
		{
			Pr_BACnetVTClass_t *pVTClass;
			pVTClass = (Pr_BACnetVTClass_t *)pvPropVal;
		}
		break;

	case BACNET_DT_BACNETVTSESS:
		{
			Pr_ListOfBACnetVTSession_t *pVTSess;
			ListOfBACnetVTSession_t *pVTsessionList;
			uint32_t u32PortNo = 0;
			pVTSess = (Pr_ListOfBACnetVTSession_t *)pvPropVal;
			pVTsessionList = pVTSess->m_pstListOfVTSession;
		}
		break;

	case BACNET_DT_ADDRESSBINDING:
	case BACNET_DT_ADDRESSBINDING_LIST:
		{
			Pr_ListOfBACnetAddrBinding_t *pAddBind;
			BACnetAddrBinding_t *pBacnetAddrBind;
			uint32_t u32PortNo = 0;
			pAddBind = (Pr_ListOfBACnetAddrBinding_t *)pvPropVal;
			pBacnetAddrBind = &pAddBind->m_stAddBinding;
		}
		break;

	case BACNET_DT_TIMESTAMP:
		{
			Pr_BACnetTimeStamp_t *pTimStamp;
			pTimStamp = (Pr_BACnetTimeStamp_t *)pvPropVal;
		}
		break;

	case BACNET_DT_DEVOBJPROPREF:
		{
			Pr_BACnetDevObjPropRef_t *pPropRef;
			BACnetDevObjPropRef_t stDevObjPropRef;
			pPropRef = (Pr_BACnetDevObjPropRef_t *)pvPropVal;
			stDevObjPropRef = pPropRef->m_stDevObjPropReff;

		}
		break;

	case BACNET_DT_COVSUB:
		{
			Pr_ListOfBACnetCovSubs_t *pCovSub;
			ListOfBACnetCovSubs_t *pCovList;
			CovPropElement_t *pPropElement;
			CovRecipientInfo_t *pRecipInfo;
			uint32_t u32PortNo = 0;
			pCovSub = (Pr_ListOfBACnetCovSubs_t *)pvPropVal;
			pCovList = pCovSub->m_pstCOVSubscribe;

		}
		break;

	case BACNET_DT_PRIORITY_ARRAY:
		{
			Pr_BACnetPriorityArray_t *pBinPrio;
			BACNET_APPLICATION_TAG eAppTag;
			uint32_t u32count = 0;
			pBinPrio = (Pr_BACnetPriorityArray_t *)pvPropVal;

		}
		break;

	case BACNET_DT_BOOLEAN_ARRAY:
		{
			Pr_ListOfBoolen_t *pboolArr;
			ListOfBoolen_t *pBoolList;
			pboolArr = (Pr_ListOfBoolen_t *)pvPropVal;
			pBoolList = pboolArr->m_pstBoolList;
		}
		break;

	case BACNET_DT_DESTINATION_LIST:
		{
			Pr_ListOfBACnetDestination_t *pDest;
			ListOfBACnetDestination_t *pDestList;
			uint32_t u32i;
			uint32_t u32PortNo = 0;
			pDest = (Pr_ListOfBACnetDestination_t *)pvPropVal;
			pDestList = pDest->m_pstNCRecepient;

		}
		break;

	case BACNET_DT_EVENTPARAMETERS:
		{
			Pr_BACnetEventParameter_t *pEventPara;
			pEventPara = (Pr_BACnetEventParameter_t *)pvPropVal;
			u32Val = 0;
		}
		break;

	case BACNET_DT_NOTIFICATIONPRIORITY:
		{
			Pr_BACnetNotifyPriority_t *pNotiPrio;
			pNotiPrio = (Pr_BACnetNotifyPriority_t *)pvPropVal;
		}
		break;

	case BACNET_DT_RECIPIENT_LIST:
		{
			Pr_ListOfBACnetRecipient_t *pRecip;
			ListOfBACnetRecipient_t *pRecList;
			pRecip = (Pr_ListOfBACnetRecipient_t *)pvPropVal;
			pRecList = pRecip->m_pstListOfRecipient;
		}
		break;

	case BACNET_DT_DATELIST:
		{
			Pr_ListOfBACnetCalendarEntry_t *pDateList;
			ListOfBACnetCalendarEntry_t *pCalList;
			BACnetCalendarEntry_t stCalEntry;
			pDateList = (Pr_ListOfBACnetCalendarEntry_t *)pvPropVal;
			pCalList = pDateList->m_pstListOfCalendar;
		}
		break;

	case BACNET_DT_DATERANGE:
		{
			Pr_BACnetDateRange_t *pEffePeriod;
			BACnetDateRange_t stDateRange;
			pEffePeriod = (Pr_BACnetDateRange_t *)pvPropVal;
			stDateRange = pEffePeriod->m_stDateRange;

		}
		break;

	case BACNET_DT_DEVOBJPROPREFF_LIST:
		{
			Pr_ListOfBACnetDevObjPropRef_t *pObjPropRefList;
			ListOfBACnetDevObjPropRef_t *pPropRefList;
			pObjPropRefList = (Pr_ListOfBACnetDevObjPropRef_t *)pvPropVal;
			pPropRefList = pObjPropRefList->m_pstListOfBACnetDevObjPropReff;
		}
		break;

	case BACNET_DT_DAILYSCHEDULE:
		{
			Pr_BACnetDailySchedule_t *pWeekSchedule;
			BACnetTimeValue_t *pTimeVal;
			pWeekSchedule = (Pr_BACnetDailySchedule_t *)pvPropVal;
			pTimeVal = &pWeekSchedule->m_stTimeValue;
		}
		break;

	case BACNET_DT_DAILYSCHEDULE_ARRAY:
		{
			Pr_ListOfBACnetDailySchedule_t *pWeekSchedule;
			BACnetTimeValue_t *pTimeVal;
			uint32_t u32count = 0;
			pWeekSchedule = (Pr_ListOfBACnetDailySchedule_t *)pvPropVal;
		}
		break;

	case BACNET_DT_SPECIALEVENT_ARRAY:
	case BACNET_DT_SPECIALEVENT:
		{
			Pr_ListOfBACnetSpecialEvent_t *pExceSchedule;
			ListOfSpecialEvent_t *pSpecialEventList;
			BACnetCalendarEntry_t stCalEntry;
			BACnetTimeValue_t *pTimeVal;
			pExceSchedule = (Pr_ListOfBACnetSpecialEvent_t *)pvPropVal;
			pSpecialEventList = pExceSchedule->m_pstSplEvent;

			while(NULL != pSpecialEventList)
			{
				pTimeVal = &pSpecialEventList->m_stListSpecialEvent.m_stListOfTimeValues;
				if(STATUS_CAL_REFF == pSpecialEventList->m_stListSpecialEvent.m_eStatusCalendar)
				{
				}
				else
				{
					stCalEntry.m_eStatusCalendar = pSpecialEventList->m_stListSpecialEvent.m_eStatusCalendar;
					memcpy(&stCalEntry.m_stCalendar.m_stDate, &pSpecialEventList->m_stListSpecialEvent.m_stPeriod.m_stDate,
						sizeof(BACnetDate_t));
					memcpy(&stCalEntry.m_stCalendar.m_stDateRange, &pSpecialEventList->m_stListSpecialEvent.m_stPeriod.m_stDateRange,
						sizeof(BACnetDateRange_t));
					memcpy(&stCalEntry.m_stCalendar.m_stWeekNDay, &pSpecialEventList->m_stListSpecialEvent.m_stPeriod.m_stWeekNDay,
						sizeof(WeekNDay_t));
				}

				while(NULL != pTimeVal)
				{
					pTimeVal = pTimeVal->m_pstNext;
				}
				pSpecialEventList = pSpecialEventList->m_pstNext;
			}
			u32Val = 0;
		}
		break;

	case BACNET_DT_SCHEDULE_PRESENT_DEFAULT:
		{
#if 0
			AnyValue_t *pSchedulePresDef;
			BACNET_PROPERTY_VALUE *pstPropVal;
			pSchedulePresDef = (AnyValue_t *)pvPropVal;
			pstPropVal = &pSchedulePresDef->m_stValue;
			while(NULL != pstPropVal)
			{
				Print_BACnet_Prop_Value(pstPropVal);
				pstPropVal = pstPropVal->pstNextPropVal;
			}
			u32Val = 0;
#endif
		}
		break;

	case BACNET_DT_BACKUPSTATE:
		{
			Pr_BACnetBackupState_t *pBackup;
			pBackup = (Pr_BACnetBackupState_t *)pvPropVal;
		}
		break;

	case BACNET_DT_LOGBUFFER_TRENDLOG:
		{
			Pr_ListOfBACnetLogRecord_t *pTrendLogBuff;
			ListOfBACnetLogRecord_t *pstLogRecord;
			uint32_t u32i = 0;
			pTrendLogBuff = (Pr_ListOfBACnetLogRecord_t *)pvPropVal;
			pstLogRecord = pTrendLogBuff->m_pstLogRecord;
			u32Val = 0;
		}
		break;

	case BACNET_DT_CLIENTCOVINCREMENT:
		{
			Pr_BACnetClientCOV_t *pClientCovInc;
			pClientCovInc = (Pr_BACnetClientCOV_t *)pvPropVal;
		}
		break;

	case BACNET_DT_OBJPROPREF:
		{
			Pr_BACnetObjPropRef_t *pObjRefe;
			pObjRefe = (Pr_BACnetObjPropRef_t *)pvPropVal;
		}
		break;

	case BACNET_DT_SETPOINTREF:
		{
			Pr_BACnetSetpointRef_t *pSetptRef;
			pSetptRef = (Pr_BACnetSetpointRef_t *)pvPropVal;
		}
		break;

	case BACNET_DT_BITSTRING_NEW:
		{
			uint32_t u32i = 0;
			Pr_BACnetBITStr_t *pnewBitStr;
			pnewBitStr = (Pr_BACnetBITStr_t *)pvPropVal;
		}
		break;

	case BACNET_DT_PRESCALE:
		{
#if 0
			Pr_BACnetPrescale_t *pPrescale;
			pPrescale = (Pr_BACnetPrescale_t *)pvPropVal;
#endif
		}
		break;

	case BACNET_DT_SCALE:
		{
#if 0
			Pr_BACnetScale_t *pScale;
			pScale = (Pr_BACnetScale_t *)pvPropVal;
#endif
		}
		break;

	case BACNET_DT_ACCRECORD:
		{
#if 0
			Pr_BACnetAccRecord_t *pAccRec;
			BACnetAccRecord_t stAccrecord;
			pAccRec = (Pr_BACnetAccRecord_t *)pvPropVal;
			stAccrecord = pAccRec->m_stRecord;
#endif
		}
		break;

	case BACNET_DT_BITSTRING_ARRAY:
		{
			Pr_ListOfBitStr_t *pStrList;
			ListOfBitStr_t *pstBitStrList;
			uint32_t u32i = 0;
			pStrList = (Pr_ListOfBitStr_t *)pvPropVal;
			pstBitStrList = pStrList->m_pstBitString;

		}
		break;

	case BACNET_DT_ACTION_LIST_ARRAY:
	case BACNET_DT_ACTION_LIST:
		{
			Pr_ListOfBACnetActionList_t *pActList;
			ListOfBACnetActionList_t *pstActionList;
			ListOfBACnetActionCommand_t *pstActionCommand;
			pActList = (Pr_ListOfBACnetActionList_t *)pvPropVal;
			pstActionList = pActList->m_pstArrayActCmd;
		}
		break;

	case BACNET_DT_READACCESSSPECS_LIST:
		{
			Pr_ListOfReadAccessSpecs_t *pAccessSpeci;
			ListOfReadAccessSpecs_t *pstReadAccess;
			ListOfBACnetPropRef_t *pstPropRefList;
			pAccessSpeci = (Pr_ListOfReadAccessSpecs_t *)pvPropVal;
			pstReadAccess = pAccessSpeci->m_pstGpOfObj;

		}
		break;

	case BACNET_DT_READACCESSRESULT_LIST:
		{
			Pr_ListOfReadAccessResult_t *pAccessResult;
			ListOfReadAccessResult_t *pstreadAccess;
			listOfResults_t *pstResult;
			pAccessResult = (Pr_ListOfReadAccessResult_t *)pvPropVal;
			pstreadAccess = pAccessResult->m_pstGpObjResults;
		}
		break;

	case BACNET_DT_DEVOBJREFF:
		{
			Pr_BACnetDevObjRef_t *pDevObjRef;
			pDevObjRef = (Pr_BACnetDevObjRef_t *)pvPropVal;
		}
		break;

	case BACNET_DT_SHEDLEVEL:
		{
			Pr_BACnetShedLevel_t *pShedLev;
			BACnetShedLevel_t stShedLevel;
			pShedLev = (Pr_BACnetShedLevel_t *)pvPropVal;
			stShedLevel = pShedLev->m_stShedLevel;
		}
		break;

	case BACNET_DT_AUTHENTICATION_FACTOR:
		{
			Pr_BACnetAuFactor_t *pAuthFact;
			uint32_t u32i = 0;
			pAuthFact = (Pr_BACnetAuFactor_t *)pvPropVal;
		}
		break;

	case BACNET_DT_AU_FACTOR_FORMAT_ARRAY:
	case BACNET_DT_AU_FACTOR_FORMAT:
		{
			Pr_ListOfBACnetAuFactorFormat_t *pAuthFactForm;
			ListOfBACnetAuFactorFormat_t *pstAuFactorFormat;
			pAuthFactForm = (Pr_ListOfBACnetAuFactorFormat_t *)pvPropVal;
			pstAuFactorFormat = pAuthFactForm->m_pstAuFactFormatList;
		}
		break;

	case BACNET_DT_AUTHENTICATION_POLICY_ARRAY:
	case BACNET_DT_AUTHENTICATION_POLICY:
		{
			Pr_ListOfBACnetAuPolicy_t *pAuthPolicy;
			ListOfBACnetAuPolicy_t *pstAuList;
			Policy_t *pstPolicy;
			pAuthPolicy = (Pr_ListOfBACnetAuPolicy_t *)pvPropVal;
			pstAuList = pAuthPolicy->m_pstAuPolicy;

		}
		break;

	case BACNET_DT_ACCESS_RULE_ARRAY:
	case BACNET_DT_ACCESS_RULE:
		{
			Pr_ListOfBACnetAccessRule_t *pAcceRule;
			ListOfBACnetAccessRule_t *pstAccessRuleList;
			pAcceRule = (Pr_ListOfBACnetAccessRule_t *)pvPropVal;
			pstAccessRuleList = pAcceRule->m_pstAccessRuleArray;
		}
		break;

	case BACNET_DT_CRED_AU_FACTOR_ARRAY:
	case BACNET_DT_CRED_AU_FACTOR:
		{
			Pr_ListOfBACnetCredAuFactor_t *pCredAUFact;
			ListOfBACnetCredAuFactor_t *pstListAuFactor;
			uint32_t u32i = 0;
			pCredAUFact = (Pr_ListOfBACnetCredAuFactor_t *)pvPropVal;
			pstListAuFactor = pCredAUFact->m_pstCredAuFactArray;

		}
		break;

	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS_ARRAY:
	case BACNET_DT_ASSIGNED_ACCESS_RIGHTS:
		{
			Pr_ListOfBACnetAssignedAccessRights_t *pAssAccRights;
			ListOfBACnetAssignedAccessRights_t *pstAccessList;
			pAssAccRights = (Pr_ListOfBACnetAssignedAccessRights_t *)pvPropVal;
			pstAccessList = pAssAccRights->m_pstAsngdAccessArray;

		}
		break;

	case BACNET_DT_LOGBUFFER_TLM:
		{
			Pr_ListOfBACnetLogMultipleRecord_t *pLogMultTrend;
			ListOfBACnetLogMultipleRecord_t *pstLogMultipleList;
			BACnetLogData_t *pstLogdata;
			uint32_t u32i = 0;
			pLogMultTrend = (Pr_ListOfBACnetLogMultipleRecord_t *)pvPropVal;
			pstLogMultipleList = pLogMultTrend->m_pstLogMultipleRecord;


		}
		break;

	case BACNET_DT_LOGBUFFER_EL:
		{
			Pr_ListOfBACnetEventLogRecord_t *pLogEvent;
			ListOfBACnetEventLogRecord_t *pstEventLogList;
			uint32_t u32i = 0;
			pLogEvent = (Pr_ListOfBACnetEventLogRecord_t *)pvPropVal;
			pstEventLogList = pLogEvent->m_pstEventLogRecord;

		}
		break;

	case BACNET_DT_OPTCHARSTRING_ARRAY:
	case BACNET_DT_OPTIONAL_CHARSTRING:
		{
			Pr_ListOfOptCharStr_t *pOptCharStr;
			ListOfOptCharStr_t *pstOptCharList;
			pOptCharStr = (Pr_ListOfOptCharStr_t *)pvPropVal;
			pstOptCharList = pOptCharStr->m_pstOptCharStr;

		}
		break;

	case BACNET_DT_ENUM_LIST:
	case BACNET_DT_ENUM_ARRAY:
		{
			Pr_ListOfEnum_t *pEnumList;
			ListOfEnum_t *pstElist;
			pEnumList = (Pr_ListOfEnum_t *)pvPropVal;
			pstElist = pEnumList->m_pstEnumList;

		}
		break;

	case BACNET_DT_DEVOBJREFF_LIST:
	case BACNET_DT_DEVOBJREFF_ARRAY:
		{
			Pr_ListOfBACnetDevObjRef_t *pDevObjRefList;
			ListOfBACnetDevObjRef_t *pstdevObjList;
			pDevObjRefList = (Pr_ListOfBACnetDevObjRef_t *)pvPropVal;
			pstdevObjList = pDevObjRefList->m_pstListOfDevObjReff;

		}
		break;

	/*case BACNET_DT_DEVOBJREFF_ARRAY:
		{
			Pr_ListOfBACnetDevObjRef_t *pDevObjRefListArr;
			pDevObjRefListArr = (Pr_ListOfBACnetDevObjRef_t *)pvPropVal;
			u32Val = 0;
		}
		break;*/

	case BACNET_DT_DEVOBJPROPREFF_ARRAY:
		{
			Pr_ListOfBACnetDevObjPropRef_t *pDevObjPropRefArr;
			ListOfBACnetDevObjPropRef_t *pstDevObjList;
			pDevObjPropRefArr = (Pr_ListOfBACnetDevObjPropRef_t *)pvPropVal;
			pstDevObjList = pDevObjPropRefArr->m_pstListOfBACnetDevObjPropReff;

		}
		break;

	case BACNET_DT_UNSIGNED8:
		{
			Pr_BACnetUnsigned8_t *pUnsigned8 = NULL;
			pUnsigned8 = (Pr_BACnetUnsigned8_t *)pvPropVal;
			u32Val = pUnsigned8->m_u8Val;

		}
		break;

	case BACNET_DT_MAX:
		{

		}
		break;

	case BACNET_DT_EMPTY:
		{

		}
		break;

	case BACNET_DT_NULL:
		{

		}
		break;

	case BACNET_DT_NW_SECURITY_POLICY_ARRAY:
	case BACNET_DT_NW_SECURITY_POLICY:
		{
			Pr_ListOfBACnetNwSecurityPolicy_t *pNwSecPolicy;
			ListOfBACnetNwSecurityPolicy_t *pstSecurityPolicyArr;
			pNwSecPolicy = (Pr_ListOfBACnetNwSecurityPolicy_t *)pvPropVal;
			pstSecurityPolicyArr = pNwSecPolicy->m_pstSecurityPolicyArr;
		}
		break;

	case BACNET_DT_SECURITY_KEYSET:
		{
			Pr_BACnetSecurityKeySet_t *pSecKeySet;
			ListOfBACnetKeyId_t *pstKeyid;
			pSecKeySet = (Pr_BACnetSecurityKeySet_t *)pvPropVal;
			pstKeyid = &pSecKeySet->m_stSecurityKetSet.m_stKeyIds;


		}
		break;

	case BACNET_DT_SECURITY_KEYSET_ARRAY:
		{
			Pr_ListOfBACnetSecurityKeySet_t *pSecKeySet;
			uint32_t u32count = 0;
			ListOfBACnetKeyId_t *pstList;
			pSecKeySet = (Pr_ListOfBACnetSecurityKeySet_t *)pvPropVal;


		}
		break;

	case BACNET_DT_PROPERTYACCESSRESULT_ARRAY:
	case BACNET_DT_PROPERTYACCESSRESULT:
		{
			Pr_ListOfBACnetPropAccessRslt_t *pPropAccaeeResult;
			ListOfBACnetPropAccessRslt_t *pstPropAcceResult;
			pPropAccaeeResult = (Pr_ListOfBACnetPropAccessRslt_t *)pvPropVal;
			pstPropAcceResult = pPropAccaeeResult->m_pstPropAccRslt;

		}
		break;

	case BACNET_DT_EVENT_MSG_TEXT:
		{
			Pr_BACnetEventMsgText_t *pEventMsgText;
			pEventMsgText = (Pr_BACnetEventMsgText_t *)pvPropVal;
		}
		break;

#ifdef BACDEL_PR14
	case BACNET_DT_LIGHTING_COMMAND:
	{
		Pr_BACnetLightingCommand_t *pLightingCmnd;
		pLightingCmnd = (Pr_BACnetLightingCommand_t *)pvPropVal;
	}
	break;

	case BACNET_DT_EVENT_NOTIFY_SUBS_LIST:
	{
		Pr_ListOfBACnetEventNotifySubs_t *pEventNotiSubList;
		ListOfBACnetEventNotifySubs_t *pstEventNotiSubList;
		pEventNotiSubList = (Pr_ListOfBACnetEventNotifySubs_t *)pvPropVal;
		pstEventNotiSubList = pEventNotiSubList->m_pstEventNotiSubList;

	}
	break;

	case BACNET_DT_PORT_PERMISSION_ARRAY:
	case BACNET_DT_PORT_PERMISSION:
		{
			Pr_ListOfBACnetPortPermission_t *pPortPermission;
			ListOfBACnetPortPermission_t *pstPortPermission;
			pPortPermission = (Pr_ListOfBACnetPortPermission_t *)pvPropVal;
			pstPortPermission = pPortPermission->m_pstPortPermissionList;

		}
		break;

	case BACNET_DT_PROCESS_ID_SELECTION:
	{
		Pr_BACnetProcessIdSelection_t *pProcIdSelect;
		pProcIdSelect = (Pr_BACnetProcessIdSelection_t *)pvPropVal;

	}
	break;

	case BACNET_DT_CHANNEL_VALUE:
	{
		Pr_BACnetChannelValue_t *pChannelVal;
		int32_t i32SizeLen = 0;
		BACNET_PROPERTY_VALUE stPropVal = {0};
		Pr_BACnetLightingCommand_t stLightCmd = {NOT_SUPPORTED, 0};
		pChannelVal = (Pr_BACnetChannelValue_t *)pvPropVal;

		if(BACNET_DT_LIGHTING_COMMAND == pChannelVal->m_stChannelValue.m_eDataType)
		{
			memcpy(&stLightCmd.m_stLightingCmd,
				&pChannelVal->m_stChannelValue.uChannelVal.m_stLightCmdValue,
				sizeof(BACnetLightingCommand_t));
			DataType_Typecast(&stLightCmd, BACNET_DT_LIGHTING_COMMAND);
		}

	}
	break;
#endif /* BACDEL_PR14 */


#ifdef BACDEL_PR14
		case BACNET_DT_FAULTPARAMETERS:
		{
			Pr_BACnetFaultParameter_t *pstFaultPara;
			pstFaultPara = (Pr_BACnetFaultParameter_t *)pvPropVal;
			u32Val = 0;
		}
		break;

		case BACNET_DT_PROPERTY_LIST:
		{
			/* local variable */
			Pr_BACnetPropertyList_t *pstPropList = NULL;

			/* get property value */
			pstPropList = (Pr_BACnetPropertyList_t *)pvPropVal;

		}
		break;
#endif /* PR14 */

#ifdef BACDEL_PR18
#if 0
	case BACNET_DT_OPTIONAL_REAL:
	{
		Pr_BACnetOptionalReal_t *pstOptionalReal;
		pstOptionalReal = (Pr_BACnetOptionalReal_t *)pvPropVal;

		Print_Optional_Real(&pstOptionalReal->m_stOptionalReal);
	}
	break;

	case BACNET_DT_OPTIONAL_BINARY_PV:
	{
		Pr_BACnetOptionalBinaryPV_t *pstOptionalBinaryPV;
		pstOptionalBinaryPV = (Pr_BACnetOptionalBinaryPV_t *)pvPropVal;

		Print_Optional_BinaryPV(&pstOptionalBinaryPV->m_stOptionalBinaryPV);
	}
	break;
#endif
	case BACNET_DT_OPTIONAL_UNSIGNED:
	{
		Pr_BACnetOptionalUnsigned_t *pstOptionalUnsigned;
		pstOptionalUnsigned = (Pr_BACnetOptionalUnsigned_t *)pvPropVal;
	}
	break;
#if 0
	case BACNET_DT_VALUE_SOURCE:
	{
		Pr_BACnetValueSource_t *pValueSource;
		BACnetValueSource_t stValueSource;
		pValueSource = (Pr_BACnetValueSource_t *)pvPropVal;
		stValueSource = pValueSource->m_stValueSource;

	}
	break;

	case BACNET_DT_VALUE_SOURCE_ARRAY:
	{
		Pr_ListOfBACnetValueSource_t *pValueSource;
		pValueSource = (Pr_ListOfBACnetValueSource_t *)pvPropVal;


	}
	break;

	case BACNET_DT_TIMESTAMP16_ARRAY:
	{
		Pr_ListOfBACnetCmdTime_t *pCmdTime;
		pCmdTime = (Pr_ListOfBACnetCmdTime_t *)pvPropVal;

	}
	break;

	case BACNET_DT_TIMER_STATE_CHANGE_VAL_ARRAY:
	{
		Pr_ListOfBACnetTimerStateChngVal_t *pTimerStateChngValarr = NULL;

		pTimerStateChngValarr = (Pr_ListOfBACnetTimerStateChngVal_t *)pvPropVal;


	}
	break;

	case BACNET_DT_REAL_ARRAY:
	{
		Pr_ListOfReal_t *pRealArr;
		ListOfReal_t *pstRealValue;
		pRealArr = (Pr_ListOfReal_t *)pvPropVal;
		pstRealValue = pRealArr->m_pstRealVal;


	}
	break;
#endif
	case BACNET_DT_OCTETSTRING_ARRAY:
	{
		Pr_ListOfOctetStr_t *pOctetStrArr;
		ListOfOctetStr_t *pstOctetString;
		pOctetStrArr = (Pr_ListOfOctetStr_t *)pvPropVal;
		pstOctetString = pOctetStrArr->m_pstOctetStrVal;

	}
	break;

	case BACNET_DT_HOSTNPORT:
	{
		Pr_BACnetHostNPort_t *pHostNPort;
		BACnetHostNPort_t stHostNPort;
		pHostNPort = (Pr_BACnetHostNPort_t *)pvPropVal;
		stHostNPort = pHostNPort->m_stHostNPort;


	}
	break;

	case BACNET_DT_BDT_ENTRY_LIST:
	{
		uint32_t u32i = 0;
		Pr_ListOfBACnetBDTEntry_t *pBDTEntryArr;
		ListOfBACnetBDTEntry_t *pstBDTEntry;
		pBDTEntryArr = (Pr_ListOfBACnetBDTEntry_t *)pvPropVal;
		pstBDTEntry = pBDTEntryArr->m_pstBDTEntryList;


	}
	break;

	case BACNET_DT_FDT_ENTRY_LIST:
	{
		uint32_t u32i = 0;
		Pr_ListOfBACnetFDTEntry_t *pFDTEntryArr;
		ListOfBACnetFDTEntry_t *pstFDTEntry;
		pFDTEntryArr = (Pr_ListOfBACnetFDTEntry_t *)pvPropVal;
		pstFDTEntry = pFDTEntryArr->m_pstFDTEntryList;
	}
	break;

	case BACNET_DT_VMAC_ENTRY_LIST:
	{
		uint32_t u32i = 0;
		Pr_ListOfBACnetVMACEntry_t *pVMACEntryArr;
		ListOfBACnetVMACEntry_t *pstVMACEntry;
		pVMACEntryArr = (Pr_ListOfBACnetVMACEntry_t *)pvPropVal;
		pstVMACEntry = pVMACEntryArr->m_pstVMACEntryList;


	}
	break;

	case BACNET_DT_ROUTER_ENTRY_LIST:
	{
		uint32_t u32i = 0;
		Pr_ListOfBACnetRouterEntry_t *pRouterEntryArr;
		ListOfBACnetRouterEntry_t *pstRouterEntry;
		pRouterEntryArr = (Pr_ListOfBACnetRouterEntry_t *)pvPropVal;
		pstRouterEntry = pRouterEntryArr->m_pstRouterEntryList;


	}
	break;

#if 0
	case BACNET_DT_LANDING_CALL_STATUS:
	{
		Pr_BACnetLandingCallStatus_t *pLandingCallStatus;
		BACnetLandingCallStatus_t stLandingCallStatus;
		pLandingCallStatus = (Pr_BACnetLandingCallStatus_t *)pvPropVal;
		stLandingCallStatus = pLandingCallStatus->m_stLandingCallStatus;
		uint32_t u32i = 0;


	}
	break;

	case BACNET_DT_LANDING_CALL_STATUS_LIST:
	{
		Pr_ListOfBACnetLandingCallStatus_t *pLandingCallStatusArr;
		ListOfBACnetLandingCallStatus_t *pstLandingCallStatuslist;
		pLandingCallStatusArr = (Pr_ListOfBACnetLandingCallStatus_t *)pvPropVal;
		pstLandingCallStatuslist = pLandingCallStatusArr->m_pstLandingCallStatusList;


	}
	break;

	case BACNET_DT_LANDING_DOOR_STATUS_ARRAY:
	{
		Pr_ListOfBACnetLandingDoorStatus_t *pLandingDoorStatusList;
		ListOfBACnetLandingDoorStatus_t *pstLandingDoorStatusList;
		ListOfLandingDoors_t *pstListOfLandingDoors;
		pLandingDoorStatusList = (Pr_ListOfBACnetLandingDoorStatus_t *)pvPropVal;
		pstLandingDoorStatusList = pLandingDoorStatusList->m_pstLandingDoorStatusList;


	}
	break;

	case BACNET_DT_ASSIGNED_LANDING_CALL_ARRAY:
	{
		Pr_ListOfBACnetAssignedLandingCalls_t *pAssignedLandingCallsList;
		ListOfBACnetAssignedLandingCalls_t *pstAssignedLandingCallsList;
		ListOfLandingCalls_t *pstAssignedLandingCalls;
		pAssignedLandingCallsList = (Pr_ListOfBACnetAssignedLandingCalls_t *)pvPropVal;
		pstAssignedLandingCallsList = pAssignedLandingCallsList->m_pstAssLandCallsList;


	}
	break;

	case BACNET_DT_LIFT_CAR_CALL_LIST_ARRAY:
	{
		Pr_ListOfBACnetLiftCarCallList_t *pLiftCarCallList;
		ListOfBACnetLiftCarCallList_t *pstLiftCarCallList;
		ListOfUnsigned_t  *pstFloorNoList;
		pLiftCarCallList = (Pr_ListOfBACnetLiftCarCallList_t *)pvPropVal;
		pstLiftCarCallList = pLiftCarCallList->pstLiftCarCallListArr;

	}
	break;

	case BACNET_DT_COV_MULTIPLE_SUBS_LIST:
	{
		Pr_ListOfBACnetCovMultipleSubs_t *pCovMultipleSubsList;
		ListOfBACnetCovMultipleSubs_t *pstCovMultipleSubsList;
		ListOfCovSubscriptionSpecs_t  *pstCovSubscriptionSpecsList;
		ListOfCovReferences_t *pstCOVReferences;
		pCovMultipleSubsList = (Pr_ListOfBACnetCovMultipleSubs_t *)pvPropVal;
		pstCovMultipleSubsList = pCovMultipleSubsList->m_pstCOVMultipleSubsList;


	}
	break;
#endif
#endif 	/* PR18 */



	default:
		{
		}
		break;
	}
}
#endif
