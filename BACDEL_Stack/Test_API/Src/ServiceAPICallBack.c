/*
 * ServiceAPICallBack.c
 *
 *  Created on: 25-May-2020
 *      Author: Pranav.Phadatare
 */
#include "bacDELDef.h"
#include "bacDELApi.h"
#include "TestServiceAPI.h"

#ifdef BACDEL_SER_DS_RP_A

BACNET_RETURN_TYPE Callback_Function_RP(bacnetip_arguments_t *pstIpArgs, uint8_t u8Reason)
{
	if(NULL == pstIpArgs)
		return BACDEL_ERROR;

	if(STATE_RESPONSE_RECEIVED != u8Reason)
	{
		if(u8Reason == STATE_SEND_REQUEST)
		{
		}
		else if(u8Reason == STATE_TIMEOUT)
		{
			/* print abort reason response */
			if(PDU_TYPE_ABORT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
			{
			}
			/* print reject reason response */
			else if(PDU_TYPE_REJECT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
			{
			}
			/* print error response */
			else
			{
			}
			if(g_u32TokenId == pstIpArgs->m_i32TokenID)
				g_bRpflag = false;
		}
		else
		{

		}
		return BACDEL_OTHER;
	}

	/* print PDU type */
	if(PDU_TYPE_COMPLEX_ACK == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
	{
		#if 0 //def NEW_RP_WP_INTERFACE
		DataType_Typecast(
			pstIpArgs->m_stNPDUData.m_stAPDUData.
			m_stServiceChoice.m_stRP_Response.m_pvPropVal,
			pstIpArgs->m_stNPDUData.m_stAPDUData.
			m_stServiceChoice.m_stRP_Response.m_eDataType);
		#endif
	}
	else
	{
		/* print abort reason response */
		if(PDU_TYPE_ABORT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
		{
		}
		/* print reject reason response */
		else if(PDU_TYPE_REJECT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
		{
		}
		/* print error response */
		else
		{
		}
	}

	if(g_u32TokenId == pstIpArgs->m_i32TokenID)
		g_bRpflag = false;

	return BACDEL_SUCCESS;
}
#endif

#ifdef BACDEL_SER_DS_RPM_A

/* function to receive RPM-A callbacks */
BACNET_RETURN_TYPE Callback_Function_RPM(bacnetip_arguments_t *pstIpArgs, uint8_t u8Reason)
{
	rpm_response_t *pstRpmResp = NULL;

	if(NULL == pstIpArgs)
		return BACDEL_ERROR;

	if(STATE_RESPONSE_RECEIVED != u8Reason)
	{
		if(u8Reason == STATE_SEND_REQUEST)
		{
		}
		else if(u8Reason == STATE_TIMEOUT)
		{
			/* print abort reason response */
			if(PDU_TYPE_ABORT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
			{
			}
			/* print reject reason response */
			else if(PDU_TYPE_REJECT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
			{
			}
			/* print error response */
			else
			{
			}
			if(g_u32TokenId == pstIpArgs->m_i32TokenID)
				g_bRpmflag = false;
		}
		else
		{
		}
		return BACDEL_OTHER;
	}

	if(PDU_TYPE_COMPLEX_ACK == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
	{
		/* */
		pstRpmResp = &pstIpArgs->m_stNPDUData.m_stAPDUData.
			m_stServiceChoice.m_stRPM_Response;

		while(NULL != pstRpmResp)
		{
			if(-1 == (int32_t)pstRpmResp->m_eErrorClass && -1 == (int32_t)pstRpmResp->m_eErrorCode)
			{
				#if 0 //def NEW_RP_WP_INTERFACE
				DataType_Typecast(pstRpmResp->m_pvPropVal, pstRpmResp->m_eDataType);
				#endif
			}
			else
			{
			}
			pstRpmResp = pstRpmResp->m_pstRPMNextElem;
		}
	}
	else
	{

		/* print abort reason response */
		if(PDU_TYPE_ABORT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
		{
		}
		/* print reject reason response */
		else if(PDU_TYPE_REJECT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
		{
		}
		/* print error response */
		else
		{
		}
	}

	if(g_u32TokenId == pstIpArgs->m_i32TokenID)
		g_bRpmflag = false;

	return BACDEL_SUCCESS;
}
#endif

#ifdef BACDEL_SER_DS_WP_A

/* function to receive WP-A callbacks */
BACNET_RETURN_TYPE Callback_Function_WP(bacnetip_arguments_t *pstIpArgs, uint8_t u8Reason)
{


	if(NULL == pstIpArgs)
		return BACDEL_ERROR;

	if(STATE_RESPONSE_RECEIVED != u8Reason)
	{
		if(u8Reason == STATE_SEND_REQUEST)
		{
		}
		else if(u8Reason == STATE_TIMEOUT)
		{
			/* print abort reason response */
			if(PDU_TYPE_ABORT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
			{
			}
			/* print reject reason response */
			else if(PDU_TYPE_REJECT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
			{
			}
			/* print error response */
			else
			{
			}
			if(g_u32TokenId == pstIpArgs->m_i32TokenID)
				g_bWpflag = false;
		}
		else
		{
		}
		return BACDEL_OTHER;
	}


	if(PDU_TYPE_SIMPLE_ACK == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
	{

	}
	else
	{
		/* print abort reason response */
		if(PDU_TYPE_ABORT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
		{
		}
		/* print reject reason response */
		else if(PDU_TYPE_REJECT == pstIpArgs->m_stNPDUData.m_stAPDUData.m_ePduType)
		{
		}
		/* print error response */
		else
		{
		}
	}

	if(g_u32TokenId == pstIpArgs->m_i32TokenID)
		g_bWpflag = false;

	return BACDEL_SUCCESS;
}
#endif
