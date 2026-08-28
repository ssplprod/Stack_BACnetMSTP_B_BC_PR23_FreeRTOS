/**********************************************************************************
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
*   SoftDEL Systems Ltd.						india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*	datalinkMSTPStateMachines.c
*                                                                      
*   AUTHORS                                                                     
*	Anagha Chitale
*                                                                         
*   DESCRIPTION                                                            
*	Functions for MSTP layer of stack.
*	Functions related to receive, master and slave state machines.
*
**********************************************************************************/

/** header file includes */
#include "../Inc/datalinkMSTPStateMachines.h"
#include "../Inc/datalinkMSTPHandler.h"
#include "../Inc/datalinkMSTP.h"
//#include "Debug.h"
#include "../../../Misc/Inc/miscMiscellaneous.h"
#include "../../../Api/Inc/bacDELApi.h"
#include "../../../Application_Layer/Stack_Mgmt/Inc/bacnetInitiateServiceMgmt.h"
#include "../../../Application_Layer/Objects/Common/Inc/bacDELDeviceConfig.h"
#include <string.h>
int8_t g_i8SendDelayTime = 0;
static void InitializeSMElements(void);
static void Process_Received_Data(void);
static void CreateShortTXPacket(uint8_t u8FrmType, uint8_t u8DestAdd);

static uint8_t u8MasterState;				/* For Changing BACnet state      */
uint8_t g_u8ReplyPostPoned;					/* Check for replay postpone      */
static uint8_t g_ucReceiveError;			/* Receive Error flag             */
static uint8_t g_ucDataAvailable;			/* Data available flag */

/** The Receive Message Queue, filled by receive thread. */
extern msgQue_t g_stRxMsgQue;
extern DB_t SMCfg;
//struct pollfd g_pfd;<@>

/* Global Receive Buffer of 200 bytes size */
DefBnRxTxQue BnRxQue;
/* Global Transmit Buffer of 200 bytes size */
DefBnRxTxQue BnTxQue;

/* Global variables  */
uint8_t g_u8ReceivedDestination;
uint8_t g_u8ReceivedSource;
uint8_t g_u8ReceivedFrameType;

volatile uint8_t b_RcvFlag;
bool_t b_RcvSMValidFrm;

uint8_t g_u8ChkFrameValidity;

/** Process Queue */
extern processDataQue_t g_stProcDataQ;
bool_t bRespRecvd;

#ifdef DEBUG_LOGFILES
uint16_t g_iTheTime;
FILE *tempTSMFp;
FILE *tempRxTSMFp;
FILE *tempRxFP;
char sTxKey[100] = "";
char sRxKey[100] = "";
FILE *ReqRespLog;
#endif

//06/01/2016
Def_BN_CMDPending BN_PendingCmd;			/** Global Pending Command Buffer of 200 bytes size	*/
MstpFillRcvQue_t g_astMstpFillRcvQueue;
bool_t g_bStartProcessing;
uint16_t u16ReplyDelayCounter;
bool_t g_bTransmitEnble;
bool_t g_bReplyDelayTimeoutFlag;
bool_t g_bTokenAvailable;
bool_t g_bDataAvlblToSend;




/**
*
* DESCRIPTION
* Function for BACDEL_MSTP_Receive_Frame_TSM of MSTP layer in stack.
*
* @param void	[in]	No input parameter
* @return void	[out]	No output parameter
*
*/
void BACDEL_MSTP_Receive_Frame_TSM(void) 
{
	uint16_t workingdata;
	uint16_t cnt;
	uint16_t crcvalue;
	uint16_t tail;
	static uint8_t bCheckRunning = 0;
	if (0 == bCheckRunning)
	{
		bCheckRunning = 1;
	}
	else
	{
		#ifdef DEBUG_LOGFILES
		if (NULL != tempRxTSMFp)
		{
			fprintf(tempRxTSMFp, "\nBACDEL_MSTP_Receive_Frame_TSM():Already running: GT = %u", g_iTheTime);
		}
		#endif
		return;
	}
	if(g_bTransmitEnble == 1)
	{
		#ifdef DEBUG_LOGFILES
		if (NULL != tempRxTSMFp)
		{
			fprintf(tempRxTSMFp, "\nBACDEL_MSTP_Receive_Frame_TSM():Transmit = 1. Exit: GT = %u", g_iTheTime);
		}
		#endif
		bCheckRunning = 0;
		return;
	}
	#ifdef DEBUG_LOGFILES
	if (NULL != tempRxTSMFp)
	{
		fprintf(tempRxTSMFp, "\nGT = %u sltm = %u Head = %d Tail = %d ExtHead = %d state = %u bin = %u TxKey=%s RxKey=%s", g_iTheTime, 
			g_stMstpVariables.Tsilence, BnRxQue.i16Head, BnRxQue.i16Tail, BnRxQue.MessageExtractHead, BnRxQue.State, g_stMstpVariables.Binaries, sTxKey, sRxKey);
	}
	#endif
	//check for data in the queue,
	while (BnRxQue.MessageExtractHead != BnRxQue.i16Head)
	{
		//Get the pointed to data char
		//Save this in case tail gets reset      
		tail = BnRxQue.MessageExtractHead;
		workingdata = BnRxQue.Data[BnRxQue.MessageExtractHead];
		g_ucReceiveError = MSTP_FALSE;
		g_ucDataAvailable = MSTP_TRUE;
		
		#ifdef DEBUG_LOGFILES
		if (NULL != tempRxTSMFp)
		{
			fprintf(tempRxTSMFp, " data=%02X, rcvError = %u, dataAvl = %u", workingdata, g_ucReceiveError, g_ucDataAvailable);
		}
		#endif

		BnRxQue.MessageExtractHead++;

		//Check if at the top of queue
		if(BnRxQue.MessageExtractHead > (BN_QUESIZE - 1)) 
		{
			BnRxQue.MessageExtractHead = 0;
		}
		
		/* Based on BACnet Receive Queue status */
		switch (BnRxQue.State)
		{
		case BN_EMSM_IDLE:
			// If it's An Error
			if (MSTP_TRUE == g_ucReceiveError)
			{
				g_ucReceiveError = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 0;
				BnRxQue.DataLength = 0;
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// At An Octet
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BNC_PREAMBLE1 != workingdata))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 0;
				BnRxQue.DataLength = 0;
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// Preamble 1
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BNC_PREAMBLE1 == workingdata))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 0;
				BnRxQue.DataLength = 0;
				RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.i16Tail = tail;  //move tail up one
				BnRxQue.State = BN_EMSM_PREAMBLE;
				break;
			}
			BnRxQue.State = BN_EMSM_IDLE;
			break;

			/* Preamble State */
		case BN_EMSM_PREAMBLE:
			// Time out
			if (g_stMstpVariables.Tsilence > g_stMstpVariables.Tframeabort)
			{
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// Error
			else if (MSTP_TRUE == g_ucReceiveError)
			{
				g_ucReceiveError = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// Repeated Preamble 1
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BNC_PREAMBLE1 == workingdata))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.i16Tail = tail;  //move tail up one
				BnRxQue.State = BN_EMSM_PREAMBLE;
				break;
			}
			// NotPreamble
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BNC_PREAMBLE1 != workingdata)
					&& (BNC_PREAMBLE2 != workingdata))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.i16Tail = tail;  //move tail up one
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// Preamble 2
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BNC_PREAMBLE2 == workingdata))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 0;
				BnRxQue.State = BN_EMSM_HEADER;
				break;
			}
			BnRxQue.State = BN_EMSM_IDLE;
			break;

			/* Header State */
		case BN_EMSM_HEADER:
			// Timeout
			if (g_stMstpVariables.Tsilence > g_stMstpVariables.Tframeabort)
			{
				BnRxQue.State = BN_EMSM_IDLE;
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				#ifdef DEBUG_LOGFILES
				printf("\n HEADER: received invalid frame : %d", g_stMstpVariables.Tsilence);
				#endif
				break;
			}
			// Error
			else if (MSTP_TRUE == g_ucReceiveError)
			{
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				#ifdef DEBUG_LOGFILES
				printf("\n HEADER: received invalid frame : error");
				#endif
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// FrameType
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (0 == BnRxQue.MessageCnt))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 1;
				g_stMstpVariables.ReceivedFrameType = (uint8_t) workingdata;
				BnRxQue.State = BN_EMSM_HEADER;
				break;
			}
			// Destination
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (1 == BnRxQue.MessageCnt)) {
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 2;
				g_stMstpVariables.ReceivedDestination = (uint8_t) workingdata;
				BnRxQue.State = BN_EMSM_HEADER;
				break;
			}
			// Source
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (2 == BnRxQue.MessageCnt)) {
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 3;
				g_stMstpVariables.ReceivedSource = (uint8_t) workingdata;
				BnRxQue.State = BN_EMSM_HEADER;
				break;
			}
			// Length 1
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (3 == BnRxQue.MessageCnt))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 4;
				BnRxQue.DataLength = workingdata;   //msb first
				BnRxQue.DataLength = BnRxQue.DataLength * 256;
				BnRxQue.State = BN_EMSM_HEADER;
				break;
			}
			// Length 2
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (4 == BnRxQue.MessageCnt))
			{
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_stMstpVariables.EventCount++;
				BnRxQue.MessageCnt = 5;
				BnRxQue.DataLength = BnRxQue.DataLength + workingdata;
				BnRxQue.State = BN_EMSM_HEADERCRC;
				break;
			}
			else
			{
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead; //move tail up one
				BnRxQue.State = BN_EMSM_IDLE;
				BnRxQue.ErrCnt++;
			}
			break;

			/* Header CRC State */
		case BN_EMSM_HEADERCRC:
			crcvalue = CalHeaderCRC(BACNET_CRC_FUNC_REC, &BnRxQue.Data[0]);
			#ifdef DEBUG_LOGFILES
			sprintf(sRxKey, "FT%02X_S%02X_D%02X_%u", g_stMstpVariables.ReceivedFrameType, g_stMstpVariables.ReceivedSource, g_stMstpVariables.ReceivedDestination, g_iTheTime);
			#endif
			// BadCRC
			if (0 != crcvalue)
			{
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead; //move tail up one
				BnRxQue.State = BN_EMSM_IDLE;
				BnRxQue.ErrCnt++;
				#ifdef DEBUG_LOGFILES
				printf("\n HEADER CRC: Invalid frame: %d", crcvalue);
				#endif
				break;
			}
			//NotForUs
			else if ((0 == crcvalue)
					&& ((g_stMstpVariables.ThisStation != g_stMstpVariables.ReceivedDestination)
					&& (BACNET_BROADCAST != g_stMstpVariables.ReceivedDestination))
					&& (0 == BnRxQue.DataLength))
			{
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			//DataNotForUs
			else if ((0 == crcvalue)
					&& ((g_stMstpVariables.ThisStation != g_stMstpVariables.ReceivedDestination)
					&& (BACNET_BROADCAST != g_stMstpVariables.ReceivedDestination))
					&& (0 != BnRxQue.DataLength))
			{
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				BnRxQue.MessageCnt = 0;
				BnRxQue.State = BN_EMSM_SKIPDATA;
				break;
			}
			// Frame Too Long
			else if ((0 == crcvalue)
					&& ((g_stMstpVariables.ThisStation == g_stMstpVariables.ReceivedDestination)
					|| (BACNET_BROADCAST == g_stMstpVariables.ReceivedDestination))
					&& (BnRxQue.DataLength > MAX_MSTP_MPDU))
			{
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				BnRxQue.State = BN_EMSM_SKIPDATA;
				BnRxQue.ErrCode = BN_RxTx_ERRCODE_2MUCH_DATA;
				BnRxQue.ErrCnt++;
				break;
			}
			// NoData
			else if ((0 == crcvalue)
					&& ((g_stMstpVariables.ThisStation == g_stMstpVariables.ReceivedDestination)
					|| (0xff == g_stMstpVariables.ReceivedDestination))
					&& (0 == BnRxQue.DataLength))
			{
				SETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
				b_RcvSMValidFrm = 1;
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				BnRxQue.State = BN_EMSM_IDLE;
				break;
			}
			// Data
			else if ((0 == crcvalue)
					&& ((g_stMstpVariables.ThisStation == g_stMstpVariables.ReceivedDestination)
					|| (BACNET_BROADCAST == g_stMstpVariables.ReceivedDestination))
					&& (0 != BnRxQue.DataLength)
					&& (BnRxQue.DataLength <= MAX_MSTP_MPDU))
			{
				BnRxQue.MessageCnt = 0;
				#ifdef DEBUG_LOGFILES
				fprintf(tempRxFP, "\nData_CRC1_%u_%u: ", g_iTheTime,BnRxQue.DataLength);
				#endif
				BnRxQue.State = BN_EMSM_DATA;
				break;
			}
			BnRxQue.State = BN_EMSM_IDLE;
			break;

			/* Data State */
		case BN_EMSM_DATA:
			// Timeout
			if (g_stMstpVariables.Tsilence > g_stMstpVariables.Tframeabort)
			{
				BnRxQue.State = BN_EMSM_IDLE;
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				#ifdef DEBUG_LOGFILES
				printf("\n DATA: invalid frame : %d", g_stMstpVariables.Tsilence);
				#endif
				break;
			}
			// Error
			else if (MSTP_TRUE == g_ucReceiveError)
			{
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				g_ucReceiveError = MSTP_FALSE;
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				BnRxQue.State = BN_EMSM_IDLE;
				#ifdef DEBUG_LOGFILES
				printf("\n DATA: invalid frame : error");
				#endif
				break;
			}
			// DataOctet
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BnRxQue.MessageCnt < BnRxQue.DataLength)) {
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
			
				#ifdef DEBUG_LOGFILES
				fprintf(tempRxFP, " %02X(%u   )(%u   )  %02X\n ", workingdata, BnRxQue.MessageCnt, BnRxQue.MessageExtractHead,BnRxQue.Data[BnRxQue.MessageExtractHead]);
				#endif

				BnRxQue.MessageCnt++;
				BnRxQue.State = BN_EMSM_DATA;
				break;
			}
			// CRC1
			else if ((MSTP_FALSE == g_ucReceiveError)
					&& (MSTP_TRUE == g_ucDataAvailable)
					&& (BnRxQue.MessageCnt == BnRxQue.DataLength)) {
				g_ucDataAvailable = MSTP_FALSE;
				g_stMstpVariables.Tsilence = 0;
				g_stMstpVariables.SubTsilence = 0;
				BnRxQue.MessageCnt++;
				BnRxQue.State = BN_EMSM_DATACRC;
				#ifdef DEBUG_LOGFILES
				if (NULL != tempRxTSMFp)
				{
					fprintf(tempRxTSMFp, " CRC1, MsgCnt = %u, DataLen = %u", BnRxQue.MessageCnt, BnRxQue.DataLength);
				}
				#endif
				break;
			}
			else
			{
				BnRxQue.State = BN_EMSM_IDLE;
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
				break;
			}

			/* Data CRC State */
		case BN_EMSM_DATACRC:
			g_ucDataAvailable = MSTP_FALSE;
			g_stMstpVariables.Tsilence = 0;
			g_stMstpVariables.SubTsilence = 0;
			crcvalue = CalDataCRC(BACNET_CRC_FUNC_REC, &BnRxQue.Data[0], 0);
			#ifdef DEBUG_LOGFILES
			if (NULL != tempRxTSMFp)
			{
				fprintf(tempRxTSMFp, " CRC Calc, CRC = %u", crcvalue);
			}
			#endif
			if (0 != crcvalue)
			{
				SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead; //move tail up one
				BnRxQue.State = BN_EMSM_IDLE;
				BnRxQue.ErrCnt++;
				#ifdef DEBUG_LOGFILES
				printf("\n DATA CRC: Invalid frame: %d", crcvalue);
				#endif
				break;
			}
			else if (0 == crcvalue)
			{
				tail = BnRxQue.i16Tail;
				if (BN_PendingCmd.i8Head < BN_NUM_OF_PENDING_CMD_MSG - 1)
					BN_PendingCmd.i8Head++;
				else
					BN_PendingCmd.i8Head = 0;

				BnRxQue.MessageCnt = BnRxQue.DataLength + 8; 
				memset(BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Data, 0x00,
						MAX_MSTP_MPDU);
				//move complete message with Preambles to pending command buffer
				for (cnt = 0; cnt <= BnRxQue.MessageCnt; cnt++) {
					BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Data[cnt] =
							BnRxQue.Data[tail];
					if (tail < BN_QUESIZE - 1)
						tail++;
					else
						tail = 0;
				}

				BN_PendingCmd.Msg[BN_PendingCmd.i8Head].NumofChar =
						BnRxQue.MessageCnt;
				BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Status =
				BN_PENDING_STATUS_CMDFILLED;

				SETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
				b_RcvSMValidFrm = 1;
				BnRxQue.i16Tail = BnRxQue.MessageExtractHead; //move tail up one
				BnRxQue.State = BN_EMSM_IDLE;
				#ifdef DEBUG_LOGFILES
				if (NULL != tempRxTSMFp)
				{
					fprintf(tempRxTSMFp, " CRC Calc Done. Bin = %u, state = %u", g_stMstpVariables.Binaries, BnRxQue.State);
				}
				#endif
				break;
			}
			BnRxQue.State = BN_EMSM_IDLE;
			break;

			/* Skip Data State */
			case BN_EMSM_SKIPDATA:
				// Timeout
				if (g_stMstpVariables.Tsilence > g_stMstpVariables.Tframeabort)
				{
					BnRxQue.State = BN_EMSM_IDLE;
					SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
					BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
					#ifdef DEBUG_LOGFILES
					printf("\n SKIP DATA: invalid frame: %d", g_stMstpVariables.Tsilence);
					#endif
					break;
				}
				// Error
				else if (MSTP_TRUE == g_ucReceiveError)
				{
					g_ucReceiveError = MSTP_FALSE;
					g_stMstpVariables.Tsilence = 0;
					g_stMstpVariables.SubTsilence = 0;
					SETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
					BnRxQue.i16Tail = BnRxQue.MessageExtractHead;
					BnRxQue.State = BN_EMSM_IDLE;
					#ifdef DEBUG_LOGFILES
					printf("\n SKIP DATA: invalid frame: error");
					#endif
					break;
				}
				// DataOctet
				else if ((MSTP_FALSE == g_ucReceiveError)
						&& (MSTP_TRUE == g_ucDataAvailable)
						&& (BnRxQue.MessageCnt < (BnRxQue.DataLength +1) ))
				{
					g_ucDataAvailable = MSTP_FALSE;
					g_stMstpVariables.Tsilence = 0;
					g_stMstpVariables.SubTsilence = 0;
					BnRxQue.MessageCnt++;
					BnRxQue.State = BN_EMSM_SKIPDATA;
					break;
				}
				// Done
				else if ((MSTP_FALSE == g_ucReceiveError)
						&& (MSTP_TRUE == g_ucDataAvailable)
						&& (BnRxQue.MessageCnt == (BnRxQue.DataLength + 1)))
				{
					g_ucDataAvailable = MSTP_FALSE;
					g_stMstpVariables.Tsilence = 0;
					g_stMstpVariables.SubTsilence = 0;
					BnRxQue.State = BN_EMSM_IDLE;
					break;
				}
				break;

		default:
			break;
		} //End of switch

		if (CHKTRUE(g_stMstpVariables.Binaries, BN_REC_VALID_FRM) && b_RcvSMValidFrm == 1)
		{
			MSTP_FillValidData();
			b_RcvSMValidFrm = 0;
			b_RcvFlag = 1;
			break;
		}
	} //End of while
	bCheckRunning = 0;
	return;
}

void MSTP_FillValidData(void)
{
	MstpFillRcvQue_t		*pstTxQueTofill = NULL;
	MstpFillRcvQueNode_t	*pstTxQNode     = NULL;
	uint32_t u32FillIndex = 0;

	/* save pointer to fill queue */
	pstTxQueTofill = &g_astMstpFillRcvQueue;
	u32FillIndex = pstTxQueTofill->m_u32FillIndex;
	pstTxQNode = &pstTxQueTofill->m_stDataNode[u32FillIndex];

	/* reset previous data if any */
	memset(pstTxQNode, 0x00, sizeof(MstpFillRcvQueNode_t));
	pstTxQNode->m_stRcvData.m_u8ReceivedDestination = g_stMstpVariables.ReceivedDestination;
	pstTxQNode->m_stRcvData.m_u8ReceivedSource = g_stMstpVariables.ReceivedSource;
	/* save frame type */
	pstTxQNode->m_stRcvData.m_u8ReceivedFrameType = g_stMstpVariables.ReceivedFrameType;
	pstTxQNode->m_stRcvData.m_u8FrameValidity = g_stMstpVariables.Binaries;

	/* increment fill index, to point to next index */
	++pstTxQueTofill->m_u32FillIndex;

    if(3 >= pstTxQueTofill->m_u32FillIndex)
	{
		/* set to zero */
		pstTxQueTofill->m_u32FillIndex = 0;
	}
}

void MSTP_ExtractValidData(void)
{
	uint32_t u32ProcIndex = 0;	
	MstpFillRcvQue_t     *pstTxQueToProcess = NULL;
	MstpFillRcvQueNode_t *pstTxQNode = NULL;

	if(b_RcvFlag == 1)
	{
		/* copy data to local variables */
		pstTxQueToProcess = &g_astMstpFillRcvQueue;
		u32ProcIndex = pstTxQueToProcess->m_u32ProcIndex;
		pstTxQNode = &pstTxQueToProcess->m_stDataNode[u32ProcIndex];

		g_u8ReceivedDestination = pstTxQNode->m_stRcvData.m_u8ReceivedDestination;
		g_u8ReceivedSource = pstTxQNode->m_stRcvData.m_u8ReceivedSource;
		g_u8ReceivedFrameType = pstTxQNode->m_stRcvData.m_u8ReceivedFrameType;
		g_u8ChkFrameValidity = pstTxQNode->m_stRcvData.m_u8FrameValidity;

		/* increment process index */
		++pstTxQueToProcess->m_u32ProcIndex;

		if(3 >= pstTxQueToProcess->m_u32ProcIndex)
		{
			/* set to zero */
			pstTxQueToProcess->m_u32ProcIndex = 0;
		}

		b_RcvFlag = 0;
	}
	else
	{
		;//do nothing
	}
}

/**
*
* DESCRIPTION                                                                          
* Function to send pending Use_token data (initiated requests or postponed responses).
* This function will check the pendinng requests/postponed responses which are waiting
* for token and sends it on network.
* 
* Function updates the master state of Master TSM and Number of frame counts.
* Function changes initiate queue service state.
*
* @param pu8MasterSate		 [in] master state
*
* @return [out] boolean flag (if FALSE, no data sent on network, else data sent) 
*
*/
//bool ProcessUseTokenData(void)
//{
//	/* local variables */
//	bool bDataAvlblToSend = FALSE;
//	uint32_t u32ProcIndex = 0;
//	MstpTxDataQue_t  *pstTxQueToProcess = NULL;
//	MstpTxDataQueNode_t *pstTxQNode = NULL;
//	InitiateInfo_t *pstInitiateInfoQ = NULL;
//
//	/* check if data is present in response queue */
//	/* copy data to local variables */
//	pstTxQueToProcess = &g_astMstpTxQueue[MSTP_TX_QUE_RESPONSE];
//	u32ProcIndex = pstTxQueToProcess->m_u32ProcIndex;
//	pstTxQNode = &pstTxQueToProcess->m_stDataNode[u32ProcIndex];
//
//	/* if data is available to send */
//	if(TRUE == pstTxQNode->m_bState)
//	{
//		/* data is present */
//		bDataAvlblToSend = TRUE;
//	}
//	/* check if data is present in request queue */
//	else
//	{
//		/* copy data to local variables */
//		pstTxQueToProcess = &g_astMstpTxQueue[MSTP_TX_QUE_REQUEST];
//		u32ProcIndex = pstTxQueToProcess->m_u32ProcIndex;
//		pstTxQNode = &pstTxQueToProcess->m_stDataNode[u32ProcIndex];
//
//		/* if data is available to send */
//		if(TRUE == pstTxQNode->m_bState)
//		{
//			/* data is present */
//			bDataAvlblToSend = TRUE;
//		}
//		else
//		{
//			/* data not present */
//			bDataAvlblToSend = FALSE;
//		}
//	}
//
//	/* if data is available to send in any queue */
//	if(TRUE == bDataAvlblToSend)
//	{
//		/* send data on serial bus */
//		MSTP_SendDataOnSerialBus(&pstTxQNode->m_au8buffer[0],
//			&g_stMstpVariables.Tsilence, pstTxQNode->m_u16DataLength);
//
//		/* get pointer to initaite queue node if any */
//		pstInitiateInfoQ = (InitiateInfo_t *)pstTxQNode->m_pvInitQueueRef;
//
//		/* non-null value indicates that this is a request pdu */
//		/* confirmed request (data expecting reply) */
//		if(MSTP_FRAME_TYPE_BACNET_DER == pstTxQNode->m_eFrameType )	//test request, der && destn 1= 255, propriatery type
//		{
//			/* make initiate state to await-response */
//			__NOP();
//			pstInitiateInfoQ->m_eServiceState = STATE_AWAIT_RESPONSE;
//			/* make master state to wait for reply */
//			u8MasterState = BACNET_STATE_WAIT_FOR_REPLY;
//
//		}
//		/* unconfirmed request (data not expecting reply) */
//		else if(MSTP_FRAME_TYPE_BACNET_DNER == pstTxQNode->m_eFrameType)
//		{
//			/* clear initiated request */
//			Clear_InitiateQ_State(&pstInitiateInfoQ);
//			/* set frame count */
//			//g_stMstpVariables.FrameCount = g_stMstpVariables.MaxInfoFrames;
//			/* make master state done with token */
//			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;
//		}
//
//		/* change tx queue node state to free */
//		pstTxQNode->m_bState = FALSE;
//		pstTxQNode->m_u16DataLength = 0;
//
//		/* decrement the queue packet count, so location gets available */
//		--pstTxQueToProcess->m_u32PacketCount;
//
//		/* increment process index */
//		++pstTxQueToProcess->m_u32ProcIndex;
//		if(MAX_MSTP_TX_QUE_LENGTH <= pstTxQueToProcess->m_u32ProcIndex)
//		{
//			/* set to zero */
//			pstTxQueToProcess->m_u32ProcIndex = 0;
//		}
//	}
//
//	/* function return value */
//	return bDataAvlblToSend;
//}
//


bool ProcessUseTokenData(void)
{
	/* local variables */
	bool bDataAvlblToSend = FALSE;
	uint32_t u32ProcIndex = 0;	
	MstpTxDataQue_t  *pstTxQueToProcess = NULL;
	MstpTxDataQueNode_t *pstTxQNode = NULL;
	InitiateInfo_t *pstInitiateInfoQ = NULL;

	/* check if data is present in response queue */
	/* copy data to local variables */
	pstTxQueToProcess = &g_astMstpTxQueue[MSTP_TX_QUE_RESPONSE];
	u32ProcIndex = pstTxQueToProcess->m_u32ProcIndex;
	pstTxQNode = &pstTxQueToProcess->m_stDataNode[u32ProcIndex];

	/* if data is available to send */
	if(TRUE == pstTxQNode->m_bState)
	{
		/* data is present */
		bDataAvlblToSend = TRUE;
	}
	/* check if data is present in request queue */
	else
	{
		/* copy data to local variables */
		pstTxQueToProcess = &g_astMstpTxQueue[MSTP_TX_QUE_REQUEST];
		u32ProcIndex = pstTxQueToProcess->m_u32ProcIndex;
		pstTxQNode = &pstTxQueToProcess->m_stDataNode[u32ProcIndex];

		/* if data is available to send */
		if(TRUE == pstTxQNode->m_bState)
		{
			/* data is present */
			bDataAvlblToSend = TRUE;
		}
		else
		{
			/* data not present */
			bDataAvlblToSend = FALSE;
		}
	}

	/* if data is available to send in any queue */
	if(TRUE == bDataAvlblToSend)
	{
		/* send data on serial bus */
		MSTP_SendDataOnSerialBus(&pstTxQNode->m_au8buffer[0],
			&g_stMstpVariables.Tsilence, pstTxQNode->m_u16DataLength);

		/* get pointer to initiate queue node if any */
		pstInitiateInfoQ = (InitiateInfo_t *)pstTxQNode->m_pvInitQueueRef;

		/* handle based on frame type */
		if(MSTP_FRAME_TYPE_BACNET_DER == pstTxQNode->m_eFrameType)
		{
			/* confirmed request - expecting a response */
			if(pstInitiateInfoQ != NULL)
			{
				__NOP();
				pstInitiateInfoQ->m_eServiceState = STATE_AWAIT_RESPONSE;
			}
			/* set master state regardless */
			u8MasterState = BACNET_STATE_WAIT_FOR_REPLY;
		}
		else if(MSTP_FRAME_TYPE_BACNET_DNER == pstTxQNode->m_eFrameType)
		{
			/* unconfirmed request - no reply expected */
			if(pstInitiateInfoQ != NULL)
			{
				Clear_InitiateQ_State(&pstInitiateInfoQ);
			}
			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;
		}

		/* change tx queue node state to free */				
		pstTxQNode->m_bState = FALSE;
		pstTxQNode->m_u16DataLength = 0;

		/* decrement the queue packet count */
		--pstTxQueToProcess->m_u32PacketCount;

		/* increment process index */
		++pstTxQueToProcess->m_u32ProcIndex;
		if(MAX_MSTP_TX_QUE_LENGTH <= pstTxQueToProcess->m_u32ProcIndex)
		{
			pstTxQueToProcess->m_u32ProcIndex = 0;
		}
	}

	/* function return value */
	return bDataAvlblToSend;
}
/**
*
* DESCRIPTION
* Function for BACDEL_MSTP_Master_Node_Tsm of MSTP layer in stack.
*
* @param void	[in]	No input parameter
* @return void	[out]	No output parameter
*
*/
void BACDEL_MSTP_Master_Node_Tsm(void) 
{
	WordSplit_u datacrc; /* Union for tracing EndianNess */
	uint16_t u16DataLength;
	uint16_t NextPollStation;

	#ifdef DEBUG_LOGFILES
	if (NULL != tempTSMFp)
	{
		fprintf(tempTSMFp, "\nGT=%u Mst=%2u RX=%2u SlTm=%3u Bin=%02X NS=%02X PS=%02X F_Cnt=%2u T_Cnt=%2u R_CNT=%u TxKey=%s RxKey=%s", 
			g_iTheTime, u8MasterState, BnRxQue.State, g_stMstpVariables.Tsilence, g_stMstpVariables.Binaries, 
			g_stMstpVariables.NextStation, g_stMstpVariables.PollStation, g_stMstpVariables.FrameCount, g_stMstpVariables.TokenCount,
			g_stMstpVariables.RetryCount, sTxKey, sRxKey);
		//if(CHKTRUE(g_stMstpVariables.Binaries, BN_REC_VALID_FRM))
		if(CHKTRUE(g_u8ChkFrameValidity, BN_REC_VALID_FRM))
		{
			fprintf(tempTSMFp, "\tsrc=%02X, dest=%02X, frame=%02X, length=%d", g_stMstpVariables.ReceivedSource, g_stMstpVariables.ReceivedDestination, 
				g_stMstpVariables.ReceivedFrameType, BnRxQue.DataLength);
		}
		if (g_stMstpVariables.Tsilence >= (uint16_t)(SM_T_NO_TOKEN))
		{
			fprintf(tempTSMFp, "\n Token Lost Scene: NS = %u, PS = %u, Binaries = %02X", g_stMstpVariables.NextStation, g_stMstpVariables.PollStation, g_stMstpVariables.Binaries);
		}
	}
	#endif

	/* Check for the Status */
	switch (u8MasterState)
	{
	/** Initialization State */
	case BACNET_STATE_INITIAL:
		InitializeSMElements();
		break;

	/** Lost Token/ No Token State */
	case BACNET_STATE_NO_TOKEN:
	{
		/** Find New Successor*/
		if ((g_stMstpVariables.Tsilence < (uint32_t)(SM_T_NO_TOKEN + (SM_T_SLOT * g_stMstpVariables.ThisStation))
				&& g_stMstpVariables.EventCount > SM_MIN_OCTETS))
		{
			u8MasterState = BACNET_STATE_IDLE;
			break;
		}
		/** Generate Token*/
		else if ((g_stMstpVariables.Tsilence >= (uint16_t)(SM_T_NO_TOKEN + (SM_T_SLOT * g_stMstpVariables.ThisStation))
				&& ((g_stMstpVariables.Tsilence < (uint16_t)(SM_T_NO_TOKEN + (SM_T_SLOT * (g_stMstpVariables.ThisStation + 1)))))))
		{
			/** Reset Poll Station and Next station */
			g_stMstpVariables.PollStation = (g_stMstpVariables.ThisStation + 1) % (g_stMstpVariables.MaxMaster + 1);
			g_stMstpVariables.NextStation = g_stMstpVariables.ThisStation;
			g_stMstpVariables.TokenCount = 0;
			g_stMstpVariables.RetryCount = 0;
			/** Send PFM Request next poll station */
			CreateShortTXPacket(BACNET_FRM_PFM, g_stMstpVariables.PollStation);
			u8MasterState = BACNET_STATE_POLL_FOR_MASTER; /** Go to PFM state*/
		}
	}
		break;

	/* BACnet Idle State */
	case BACNET_STATE_IDLE:
	{
		/** Find lost token */
		if (g_stMstpVariables.Tsilence >= SM_T_NO_TOKEN)
		{
			u8MasterState = BACNET_STATE_NO_TOKEN; /** Go to No Token State*/
			g_stMstpVariables.EventCount = 0;
		}
		/** Received Invalid frame */
		else if(CHKTRUE(g_u8ChkFrameValidity, BN_REC_INVALID_FRM))
		{
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_INVALID_FRM);
			u8MasterState = BACNET_STATE_IDLE;
		}
		/** Received valid frame */
		else if (CHKTRUE(g_u8ChkFrameValidity, BN_REC_VALID_FRM))
		{
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_VALID_FRM);
			/* Received Unwanted Frame*/
				if(g_u8ReceivedDestination != BACNET_BROADCAST
					&& g_u8ReceivedDestination != g_stMstpVariables.ThisStation)
			{
				u8MasterState = BACNET_STATE_IDLE; /** Go to IDLE */
			}
			else if (g_u8ReceivedDestination == BACNET_BROADCAST)
			{
				switch (g_u8ReceivedFrameType)
				{
				/* Broadcast Data Needing Reply*/
				case BACNET_FRM_DATA_EXP_REPLY:
				/* Received unwanted frame */
				case BACNET_FRM_TEST_REQUEST:
				case BACNET_FRM_TOKEN:
				{
					  u8MasterState = BACNET_STATE_IDLE; /** Go to IDLE */
				}
				break;

				/* Received Data No Reply*/
				case BACNET_FRM_TEST_REPONSE:			
				case BACNET_FRM_DATA_NOTEXP_REPLY:
				{
					Process_Received_Data(); /** Process the request */
					u8MasterState = BACNET_STATE_IDLE; /** Go to IDLE */
				}
				break;

				default:
					break;
				}
			}
			/** If destination is This station */
			else if (g_u8ReceivedDestination == g_stMstpVariables.ThisStation)				
			{
				/** What is frame type */
				switch (g_u8ReceivedFrameType)
				{
				/** Received Token */
				case BACNET_FRM_TOKEN:
				{
					g_stMstpVariables.FrameCount = 0;
					g_bTokenAvailable = TRUE;
					RESETBIT(g_stMstpVariables.Binaries, BN_SOLEMASTER);
					u8MasterState = BACNET_STATE_USE_TOKEN; /** Go to USe Token */
				}
				break;
				/** Received  Poll For Master (PFM) */
				case BACNET_FRM_PFM:
				{
					CreateShortTXPacket(BACNET_FRM_REPLY_TO_PFM,
							g_u8ReceivedSource);
					u8MasterState = BACNET_STATE_IDLE; /** Go to IDLE */
				}
				break;
				/* Received Data Needing Reply*/
				/** Case  Data Expecting Reply */
				case BACNET_FRM_DATA_EXP_REPLY:
				case BACNET_FRM_TEST_REQUEST:
				{
					/** Start Reply time delay*/
					g_bReplyDelayTimeoutFlag = TRUE;
					g_bStartProcessing = TRUE;
					u16ReplyDelayCounter = FALSE;
					u8MasterState = BACNET_STATE_ANSWER_DATA_REQUEST; /** Go to ANSWER DATA REQUEST*/
				}
				break;
				/* Received Data No Reply*/
				/** Case  Data Not Expecting Reply */
				case BACNET_FRM_DATA_NOTEXP_REPLY: //Need to Handle
				{
					#ifdef DEBUG_LOGFILES
					printf("\nProcess-Data-1");
					//fprintf(tempRxFP, "\nAck_ %u_ %u: ", BnRxQue.DataLength, bIsTxData);
					//fprintf(tempRxFP, "Ack_%02X(%u   ) ", pu8Data[u16index], u16index);
					fprintf(tempRxFP, "\nAck_%u_DataSize_%u :\n ", g_iTheTime, BN_PendingCmd.Msg[BN_PendingCmd.i8Tail].NumofChar);
					for(u16cnt=0; u16cnt < BN_PendingCmd.Msg[BN_PendingCmd.i8Tail].NumofChar; u16cnt++)
					{
						fprintf(tempRxFP, " %02X", BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Data[u16cnt]);
					}
					#endif
					Process_Received_Data(); /** Process the request */
					u8MasterState = BACNET_STATE_IDLE; /** Go to IDLE */

				}
				break;
					/** Case default - Unwanted Frame */
				default:
				{
					u8MasterState = BACNET_STATE_IDLE; /**Go to IDLE */
				}
					break;
				} // End of switch case
			} // End of else if this station
			else
				u8MasterState = BACNET_STATE_IDLE; //Unwanted frame
		} // End of valid frame if condition
	}
	break;

	/** Use Token State */
	case BACNET_STATE_USE_TOKEN:
	{
		g_bDataAvlblToSend = FALSE;
		if(g_bTokenAvailable == TRUE && (g_stMstpVariables.Tsilence <= SM_USAGE_TIMEOUT)
			&& g_stMstpVariables.FrameCount < g_stMstpVariables.MaxInfoFrames)
		{
			g_bDataAvlblToSend = ProcessUseTokenData();
			if(TRUE == g_bDataAvlblToSend)
			{
				/* increment frame count */
				g_stMstpVariables.FrameCount++;
				/** clear flag */
				b_ResponseSentOnNetwork = FALSE;
			}
			else
			{
				g_stMstpVariables.FrameCount = g_stMstpVariables.MaxInfoFrames;
				g_bTokenAvailable = FALSE;
				u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;	/** Go to Done With Token */
			}	
		}
		else
		{
			g_stMstpVariables.FrameCount = g_stMstpVariables.MaxInfoFrames; //TODO replace MAcro with variable
			g_bTokenAvailable = FALSE;
			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;	/** Go to Done With Token */
		}
	}
	break;

	/** Pass Token state */
	case BACNET_STATE_PASS_TOKEN:
	{
		/** Saw Token User */
		if ((g_stMstpVariables.Tsilence < SM_USAGE_TIMEOUT)
				&& (g_stMstpVariables.EventCount > SM_MIN_OCTETS))
		{
			u8MasterState = BACNET_STATE_IDLE; /** Go to IDLE*/
		}
		/** If Token usage timed out */
		else if (g_stMstpVariables.Tsilence >= SM_USAGE_TIMEOUT)
		{
			/** Retry Send Token */
			if ((g_stMstpVariables.RetryCount < SM_RETRY_TOKEN))
			{
				/** Increment the Retry Count */
				g_stMstpVariables.RetryCount++;
				if(g_stMstpVariables.ThisStation != g_stMstpVariables.NextStation)
					CreateShortTXPacket(BACNET_FRM_TOKEN, g_stMstpVariables.NextStation); /** Send Token again*/
				break;
			}
			/** Find next new master node */
			else if ((g_stMstpVariables.RetryCount >= SM_RETRY_TOKEN))
			{
				/** Update Poll Station */
				g_stMstpVariables.PollStation = (g_stMstpVariables.NextStation + 1)	% (g_stMstpVariables.MaxMaster + 1);

				// If the poll station is equal to this station then increment the poll station.
				if (g_stMstpVariables.PollStation	== g_stMstpVariables.ThisStation)
				{
					g_stMstpVariables.PollStation = (g_stMstpVariables.PollStation + 1) % (g_stMstpVariables.MaxMaster + 1);
				}
				/** Send poll for master in order to find new master node*/
				CreateShortTXPacket(BACNET_FRM_PFM,	g_stMstpVariables.PollStation);
				g_stMstpVariables.NextStation = g_stMstpVariables.ThisStation;
				g_stMstpVariables.TokenCount = 0;
				u8MasterState = BACNET_STATE_POLL_FOR_MASTER; /** Go to PFM State */
				g_stMstpVariables.RetryCount = 0;
			}
		}      //End of Token usage timed out
	}      // End of case
	break;

	/** BACnet Done with the Token State */
	case BACNET_STATE_DONE_WITH_TOKEN:
	{
		/** Send Another Frame */
		if(g_stMstpVariables.FrameCount < g_stMstpVariables.MaxInfoFrames)
		{
			u8MasterState = BACNET_STATE_USE_TOKEN;
			break; //exit from switch case
		}

		if(g_stMstpVariables.FrameCount >= g_stMstpVariables.MaxInfoFrames)
		{
		NextPollStation = (g_stMstpVariables.ThisStation + 1)	% (g_stMstpVariables.MaxMaster + 1);

		// SendToken Case
		if (((CHKFALSE(g_stMstpVariables.Binaries, BN_SOLEMASTER)))
				&& ((g_stMstpVariables.TokenCount < SM_NUM_POLLED - 1)
				|| (g_stMstpVariables.NextStation == NextPollStation) 
				/*|| (1 == bRespRecvd)*/))
		{

			g_stMstpVariables.TokenCount = g_stMstpVariables.TokenCount + 1;
			if(g_stMstpVariables.ThisStation != g_stMstpVariables.NextStation)
				CreateShortTXPacket(BACNET_FRM_TOKEN, g_stMstpVariables.NextStation);
			g_stMstpVariables.RetryCount = 0;
			g_stMstpVariables.EventCount = 0;
			bRespRecvd = 0;
			u8MasterState = BACNET_STATE_PASS_TOKEN;
		}
		else if (g_stMstpVariables.TokenCount < SM_NUM_POLLED - 1)
		{
			/** SoleMaster */
			if (CHKTRUE(g_stMstpVariables.Binaries, BN_SOLEMASTER))
			{
				g_stMstpVariables.FrameCount = 0;
				//g_stMstpVariables.TokenCount++;
				g_stMstpVariables.TokenCount = SM_NUM_POLLED;
				u8MasterState = BACNET_STATE_USE_TOKEN;
			}
			else /** Not SoleMaster*/
			{
				/** NextStationUnknown */
				if (g_stMstpVariables.NextStation	== g_stMstpVariables.ThisStation)
				{
					g_stMstpVariables.PollStation = (uint8_t) NextPollStation;
					CreateShortTXPacket(BACNET_FRM_PFM,	g_stMstpVariables.PollStation);
					g_stMstpVariables.RetryCount = 0;
					u8MasterState = BACNET_STATE_POLL_FOR_MASTER;
				}
				else
				{
					/** If next station known, pass the token */
					g_stMstpVariables.TokenCount = g_stMstpVariables.TokenCount + 1;
					CreateShortTXPacket(BACNET_FRM_TOKEN, g_stMstpVariables.NextStation);
					g_stMstpVariables.RetryCount = 0;
					g_stMstpVariables.EventCount = 0;
					u8MasterState = BACNET_STATE_PASS_TOKEN;
				}
			} //End of else - not sole master
		} //End of if(g_stMstpVariables.TokenCount <  SM_NUM_POLLED - 1)
		else
		{
			NextPollStation = (g_stMstpVariables.PollStation + 1)	% (g_stMstpVariables.MaxMaster + 1);
			/** Send Maintenance PFM */
			if (NextPollStation != g_stMstpVariables.NextStation)
			{
				g_stMstpVariables.PollStation = (uint8_t) NextPollStation;
				CreateShortTXPacket(BACNET_FRM_PFM,	g_stMstpVariables.PollStation);
				g_stMstpVariables.RetryCount = 0;
				u8MasterState = BACNET_STATE_POLL_FOR_MASTER;
			}
			else
			{	
				/** If sole master */
				if (CHKTRUE(g_stMstpVariables.Binaries, BN_SOLEMASTER))
				{
					/*** Sole Master Restart Maintenance PFM */
					g_stMstpVariables.PollStation = (g_stMstpVariables.NextStation + 1)	% (g_stMstpVariables.MaxMaster + 1);
					CreateShortTXPacket(BACNET_FRM_PFM,	g_stMstpVariables.PollStation);
					g_stMstpVariables.NextStation = g_stMstpVariables.ThisStation;
					g_stMstpVariables.RetryCount = 0;
					g_stMstpVariables.EventCount = 0;
					g_stMstpVariables.TokenCount = 1;								
					u8MasterState = BACNET_STATE_POLL_FOR_MASTER;
				}
				else if (g_stMstpVariables.NextStation != g_stMstpVariables.ThisStation)
				{ /** If not sole master - Reset Maintenance PFM */
					g_stMstpVariables.RetryCount = 0;
					g_stMstpVariables.EventCount = 0;
					g_stMstpVariables.TokenCount = 1;							
					CreateShortTXPacket(BACNET_FRM_TOKEN, g_stMstpVariables.NextStation);
					g_stMstpVariables.PollStation = g_stMstpVariables.ThisStation;
					u8MasterState = BACNET_STATE_PASS_TOKEN;
				}
			}
		}
		}
	} //End of switch case
	break;
		/** BACnet Poll For Master  (PFM ) State */
	case BACNET_STATE_POLL_FOR_MASTER:
	{
		//if (CHKTRUE(g_stMstpVariables.Binaries, BN_REC_VALID_FRM))
		if (CHKTRUE(g_u8ChkFrameValidity, BN_REC_VALID_FRM))
		{
			/** Received Reply to PFM */
			if ((g_u8ReceivedDestination == g_stMstpVariables.ThisStation)
					&& (g_u8ReceivedFrameType == BACNET_FRM_REPLY_TO_PFM))
			{
				RESETBIT(g_stMstpVariables.Binaries, BN_SOLEMASTER);
				g_stMstpVariables.NextStation = g_u8ReceivedSource; //sets next address to source address
				CreateShortTXPacket(BACNET_FRM_TOKEN, g_stMstpVariables.NextStation);
				g_stMstpVariables.PollStation = g_stMstpVariables.ThisStation;
				g_stMstpVariables.TokenCount = 0;
				g_stMstpVariables.RetryCount = 0;
				u8MasterState = BACNET_STATE_PASS_TOKEN;
			}
			else
			{
				/** Received Unexpected Frame */
				u8MasterState = BACNET_STATE_IDLE;
			}
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_VALID_FRM);
		}
		/** If we receive invalid frame */
		else if ((g_stMstpVariables.Tsilence >= SM_USAGE_TIMEOUT)
				|| CHKTRUE(g_u8ChkFrameValidity, BN_REC_INVALID_FRM))
		{
			/** If Sole Master */
			if (CHKTRUE(g_stMstpVariables.Binaries, BN_SOLEMASTER))
			{
				g_stMstpVariables.FrameCount = 0;
				u8MasterState = BACNET_STATE_USE_TOKEN;
				RESETBIT(g_stMstpVariables.Binaries, BN_XMIT_READY);
			}
			else
			{ /** Next station known */
				if (g_stMstpVariables.NextStation	!= g_stMstpVariables.ThisStation)
				{
					CreateShortTXPacket(BACNET_FRM_TOKEN, g_stMstpVariables.NextStation);
					g_stMstpVariables.EventCount = 0;
					g_stMstpVariables.RetryCount = 0;
					u8MasterState = BACNET_STATE_PASS_TOKEN;
				}
				else
				{
					/** Next station unknown */
					NextPollStation = (g_stMstpVariables.PollStation + 1) % (g_stMstpVariables.MaxMaster + 1);
					/** Send Next PFM */
					if (NextPollStation != g_stMstpVariables.ThisStation)
					{
						g_stMstpVariables.PollStation = (uint8_t) NextPollStation;
						g_stMstpVariables.RetryCount = 0;
						CreateShortTXPacket(BACNET_FRM_PFM, g_stMstpVariables.PollStation);
					}
					else /** Declare Sole Master */
					{
						SETBIT(g_stMstpVariables.Binaries, BN_SOLEMASTER);
						g_stMstpVariables.FrameCount = 0;
						u8MasterState = BACNET_STATE_USE_TOKEN;
					}
				}
			}      // End of if not sole master

			if (CHKTRUE(g_u8ChkFrameValidity, BN_REC_INVALID_FRM))
			{
				RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
				RESETBIT(g_u8ChkFrameValidity, BN_REC_INVALID_FRM);
			}
		}      //End of received invalid frame
	}      //End of switch case
	break;

		/* BACnet Wait for replay state */
	case BACNET_STATE_WAIT_FOR_REPLY:
	{
		//(Reply Time out)
		if (g_stMstpVariables.Tsilence >= SM_REPLY_TIMEOUT)
		{
			g_stMstpVariables.FrameCount = (uint8_t) g_stMstpVariables.MaxInfoFrames;
			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;
			break;
		}
		//(Invalid Frame)
		else if ((g_stMstpVariables.Tsilence < SM_REPLY_TIMEOUT)
				&& (CHKTRUE(g_u8ChkFrameValidity, BN_REC_INVALID_FRM)))
		{
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_INVALID_FRM);
			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;
			break;
		}
		//(Received Reply
		else if ((g_stMstpVariables.Tsilence < SM_REPLY_TIMEOUT)
				&& (CHKTRUE(g_u8ChkFrameValidity, BN_REC_VALID_FRM))
				&& (g_u8ReceivedDestination == g_stMstpVariables.ThisStation)
				&& ((g_u8ReceivedFrameType == BACNET_FRM_TEST_REPONSE)
				|| (g_u8ReceivedFrameType == BACNET_FRM_DATA_NOTEXP_REPLY)))
		{
			Process_Received_Data();
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_VALID_FRM);
			bRespRecvd = 1;
			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;
			break;
		}
		//Received Postpone
		else if ((g_stMstpVariables.Tsilence < SM_REPLY_TIMEOUT)
				&& (CHKTRUE(g_u8ChkFrameValidity, BN_REC_VALID_FRM)
				&& (g_u8ReceivedDestination == g_stMstpVariables.ThisStation)
				&& (g_u8ReceivedFrameType == BACNET_FRM_REPLY_POSTPONED)))
		{
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_VALID_FRM);
			bRespRecvd = 1;
			u8MasterState = BACNET_STATE_DONE_WITH_TOKEN;
			break;
		}
		//Received Unexpected Frame
		else if ((g_stMstpVariables.Tsilence < SM_REPLY_TIMEOUT)
				&& (CHKTRUE(g_u8ChkFrameValidity, BN_REC_VALID_FRM))
				&& ((g_u8ReceivedDestination != g_stMstpVariables.ThisStation)
				|| (g_u8ReceivedFrameType != BACNET_FRM_TEST_REPONSE)
				|| (g_u8ReceivedFrameType != BACNET_FRM_DATA_NOTEXP_REPLY)))
		{
			RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
			RESETBIT(g_u8ChkFrameValidity, BN_REC_VALID_FRM);
			u8MasterState = BACNET_STATE_IDLE;
			break;
		}
	}
	break;

	/** BACnet Answer for Data Request state */
	case BACNET_STATE_ANSWER_DATA_REQUEST:
	{
		RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);	
		/** start processing the request */
		if(u16ReplyDelayCounter <= SM_REPLY_DELAY)
		{	
			if(TRUE == g_bStartProcessing)
			{
				Process_Received_Data();
			}
			g_bStartProcessing = FALSE;

			/** check response sent on network */
		if((TRUE  == b_ResponseSentOnNetwork))//(-1 != g_i32ProcessQIndex)  &&
			{
				/** change state to idle */
				u8MasterState = BACNET_STATE_IDLE;	
				/** set index value to default value */
				g_i32ProcessQIndex = -1;
				/** clear flag */
				b_ResponseSentOnNetwork = FALSE;
			}
		}
		else
		{		
			if((-1 != g_i32ProcessQIndex)  && (FALSE  == b_ResponseSentOnNetwork))
			{
				g_stProcDataQ.m_stProcessQueue[g_i32ProcessQIndex].
					m_stProcessData.m_bIsTokenRequired = TRUE;
				/** set index value to default value */
				g_i32ProcessQIndex = -1;
			}
			/** send reply postponed */
			CreateShortTXPacket(BACNET_FRM_REPLY_POSTPONED,
					g_u8ReceivedSource);

			/** clear flags */
			g_bStartProcessing = FALSE;
			u16ReplyDelayCounter = FALSE;
			g_bReplyDelayTimeoutFlag = FALSE;

			/** change state to idle */
			u8MasterState = BACNET_STATE_IDLE;
		}

	}
	break;

	default:
		break;
	} //End of u8MasterState switch statement

	/* Prepare Transmit buffer with Preambles and CRC */
	if (CHKTRUE(g_stMstpVariables.Binaries,BN_XMIT_READY))
	{
		RESETBIT(g_stMstpVariables.Binaries, BN_XMIT_READY);
		BnTxQue.Data[BN_PREAMBLE1] = 0x55;           //preamble 1
		BnTxQue.Data[BN_PREAMBLE2] = 0xFF;           //preamble 2
		BnTxQue.Data[BN_SOURCE_ADD] = g_stMstpVariables.ThisStation; //Source Address (this node)

		u16DataLength = BnTxQue.DataLength - 8;

		if (u16DataLength > 255)
		{
			MoveUnsigned16toString(&BnTxQue.Data[BN_LENGTHM], u16DataLength);
		} 
		else 
		{
			BnTxQue.Data[BN_LENGTHM] = 0;						//MSB of length
			BnTxQue.Data[BN_LENGTHL] = (uint8_t) u16DataLength; //LSB of length
		}

		//Calculate header checksum and fill in checksum buffer placement "res_buf[7]"
		BnTxQue.Data[BN_HEADER_CRC] = CalHeaderCRC(BACNET_CRC_FUNC_XMIT, &BnTxQue.Data[0]);

		//Check for data frame, data frames have a second checksum
		if (BnTxQue.DataLength != BACNET_SHORT_FRAME) {
			//Calculate data frame checksum
			datacrc.u16Word = CalDataCRC(BACNET_CRC_FUNC_XMIT, &BnTxQue.Data[0], BnTxQue.DataLength);
			BnTxQue.Data[BnTxQue.DataLength] = datacrc.stBytes.u8HighByte; //msb of length
			BnTxQue.Data[BnTxQue.DataLength + 1] = datacrc.stBytes.u8LowByte; //lsb of length
			BnTxQue.DataLength = BnTxQue.DataLength + 2;
		}

		g_stMstpVariables.SubTsilence = 0;
		g_stMstpVariables.Tsilence = 0;
		BnTxQue.Status = 1;

		//callback
		if (BnTxQue.Data[BN_FRAME_TYPE] == BACNET_FRM_TOKEN)
		{
			MSTP_SendDataOnSerialBus(&BnTxQue.Data[0], &g_stMstpVariables.Tsilence, BnTxQue.DataLength);
		}
		else
		{
			MSTP_SendDataOnSerialBus(&BnTxQue.Data[0], &g_stMstpVariables.Tsilence, BnTxQue.DataLength);
		}
	}
}


/**
*
* DESCRIPTION
* This function executes in a 1ms Timeout ISR and updates soft timers
*		required to run BACnet MSTP state machine
*
* @param void	[in]	No input parameter
* @return void	[out]	No output parameter
*
*/
void BACnet_StateMachine_Timers(void)
{
	/* increment value of silence counter */
	g_stMstpVariables.Tsilence++;

	/* increment reply delay counter */
	if(TRUE == g_bReplyDelayTimeoutFlag)
	{
		u16ReplyDelayCounter++;
	}
}



/* End of MoveLongtoString */
/**
 @brief MoveUnsigned16toString
 *  		Copies Unsigned 16 byte type value into a given array
 @param destination		destination  value
 @param value			destination  value
 */
void MoveUnsigned16toString(uint8_t* destination, uint16_t value) {
	WordSplit_u wvalue;

	wvalue.u16Word = value;
	destination[0] = wvalue.stBytes.u8LowByte;
	destination[1] = wvalue.stBytes.u8HighByte;
}
/* End of MoveUnsigned16toString */


/**
*
* DESCRIPTION
*	Make transmit buffer ready with give frame type and destination address
*	Also it set transmit bit ready to send out immediately
*
* @param u8FrmType		[in]	Frame Type
* @param u8DestAdd		[in]	Destination MAC address
*
* @return void			[out]	No output parameter
*
*/

static void CreateShortTXPacket(uint8_t u8FrmType, uint8_t u8DestAdd)
{
#ifdef DEBUG_LOGFILES
	sprintf(sTxKey, "FT%02X_D%02X_%u", u8FrmType, u8DestAdd, g_iTheTime);
#endif
	RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
	BnTxQue.Data[BN_FRAME_TYPE] = u8FrmType; /** Set frame type*/
	BnTxQue.Data[BN_DESTINATION_ADD] = u8DestAdd; /** Set destination address*/
	BnTxQue.DataLength = BACNET_SHORT_FRAME;
	SETBIT(g_stMstpVariables.Binaries, BN_XMIT_READY);

	//Reset counters
	switch (u8FrmType) {
	case BACNET_FRM_PFM:
		g_stMstpVariables.RetryCount = 0;
		break;
	case BACNET_FRM_TOKEN:
		g_stMstpVariables.EventCount = 0;
		break;
	default:
		break;
	}
}


/**
 @brief Process_Received_Data
 *	Decodes the request and fill the response buffer according to
 *	the requested BACnet service choice
 *
 @param 	u8FrmType		Frame Type
 @param	u8DestAdd		Destination MAC address
 @return void
 */
static void Process_Received_Data(void) {
	/** Move to next filled pending queue location */
	g_stMstpVariables.Tsilence = 0;
	if (BN_PendingCmd.i8Tail < BN_NUM_OF_PENDING_CMD_MSG - 1)
		BN_PendingCmd.i8Tail++;
	else
		BN_PendingCmd.i8Tail = 0;

	#ifdef DEBUG_LOGFILES
	if (NULL != ReqRespLog)
	{
		fprintf(ReqRespLog, "\nProcReqT_%u_DataSize_%u : ", g_iTheTime, BN_PendingCmd.Msg[BN_PendingCmd.i8Tail].NumofChar);
		for(u16cnt=0; u16cnt < BN_PendingCmd.Msg[BN_PendingCmd.i8Tail].NumofChar; u16cnt++)
		{
			fprintf(ReqRespLog, " %02X", BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Data[u16cnt]);
		}
	}
	#endif

	Fill_Data_In_ReceiveQ(&BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Data[0],
			BN_PendingCmd.Msg[BN_PendingCmd.i8Tail].NumofChar);

	/* clear the queue location */
    memset(&BN_PendingCmd.Msg[BN_PendingCmd.i8Head].Data[0], 0, 
		BN_PendingCmd.Msg[BN_PendingCmd.i8Tail].NumofChar);
                        

}

/**
*
* DESCRIPTION
*	Initialize the state machine elements
*
* @param void	[in]	No input parameter
* @return void	[out]	No output parameter
*
*/
static void InitializeSMElements(void) 
{
	g_stMstpVariables.Tsilence = 0;
	g_stMstpVariables.SubTsilence = 0;
	g_stMstpVariables.NextStation = g_stMstpVariables.ThisStation;
	g_stMstpVariables.PollStation = (g_stMstpVariables.ThisStation)	% (g_stMstpVariables.MaxMaster + 1);
	g_stMstpVariables.TokenCount = SM_NUM_POLLED;
	RESETBIT(g_stMstpVariables.Binaries, BN_SOLEMASTER);
	RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
	RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
	u8MasterState = BACNET_STATE_IDLE;
}

/**
*
* DESCRIPTION
*    This module initialize variables related to the MSTP state machine
*	 Also, Initializes response buffer and Command buffer.
*
* @param pu8FileName	[in]	Serial communication port or file
* @return				[out]	BACDEL_SUCCESS on success else error code 
*
*/
BACNET_RETURN_TYPE BACDEL_MSTP_InitializeStack(uint8_t *pu8FileName) 
{
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;
	unsigned char cnt;
	g_u8ReplyPostPoned = 0;

	/* Initialization dependent on Baud Rate */
	g_stMstpVariables.Tframeabort = SM_FRAME_ABORT;

	/* Initialization of BACnet Variables */
	g_stMstpVariables.ThisStation = MSTP_GetMacId(NULL);
	g_stMstpVariables.NextStation = g_stMstpVariables.ThisStation;
	g_stMstpVariables.PollStation = g_stMstpVariables.ThisStation;
	g_stMstpVariables.SourceAddress = 8;
	g_stMstpVariables.RetryCount = 0;
	g_stMstpVariables.MaxMaster = SMCfg.g_BACnetCfg.MaxMaster;
	g_stMstpVariables.TokenCount = SM_NUM_POLLED;
	g_stMstpVariables.MaxInfoFrames = BACnetConfigData.m_u32MaxInfoFrame;
	g_stMstpVariables.TnoTokens = SM_T_NO_TOKEN;
	g_stMstpVariables.TreplyTimeout = 0;
	g_stMstpVariables.FrameCount = 0;
	g_stMstpVariables.EventCount = 0;

	/* Initializes Command buffer */
	for (cnt = 0; cnt < BN_NUM_OF_PENDING_CMD_MSG; cnt++) {
		//command
		BN_PendingCmd.Msg[cnt].NumofChar = 0;
		BN_PendingCmd.Msg[cnt].Status = BN_PENDING_STATUS_EMPTY;
		BN_PendingCmd.Msg[cnt].Reply = BN_PENDING_REPLY_NO;
	}

	BN_PendingCmd.i8Head = 0;
	BN_PendingCmd.i8Tail = BN_PendingCmd.i8Head;

	g_stMstpVariables.Tsilence = 0;//SM_USAGE_TIMEOUT;
	RESETBIT(g_stMstpVariables.Binaries, BN_REC_VALID_FRM);
	RESETBIT(g_stMstpVariables.Binaries, BN_REC_INVALID_FRM);
	g_stMstpVariables.TokenCount = SM_NUM_POLLED;
	g_stMstpVariables.RS485_DisableTime = 0xff;
	BnRxQue.MessageCnt = 0;
	BnTxQue.MessageCnt = 0;
	BnTxQue.DataLength = 0;
	BnTxQue.MessageExtractHead = 0;

	BnRxQue.State = BN_EMSM_IDLE;
	g_ucReceiveError = MSTP_TRUE;
	g_ucDataAvailable = MSTP_FALSE;

	BnTxQue.i16Head = 0;
	BnTxQue.i16Tail = 0;
	BnTxQue.Status = 0;
	BnRxQue.RxsCRA_OLD = 0;
	BnRxQue.MessageExtractHead = 0;
	BnRxQue.i16Head = 0;
	BnRxQue.i16Tail = 0;
	BnRxQue.Status = 0;

	memset(BnTxQue.Data, 0x00, BN_QUESIZE);
	memset(BnRxQue.Data, 0x00, BN_QUESIZE);
	g_stMstpVariables.Tsilence = 0;
	g_stMstpVariables.SubTsilence = 0;
	g_bTransmitEnble = 0;
	SETBIT(g_stMstpVariables.Binaries, BN_SOLEMASTER);
	u8MasterState = BACNET_STATE_INITIAL;

	g_u8ReceivedDestination = g_stMstpVariables.ReceivedDestination;
	g_u8ReceivedSource = g_stMstpVariables.ReceivedSource;
	g_u8ReceivedFrameType = g_stMstpVariables.ReceivedFrameType;

	#ifdef DEBUG_LOGFILES
	tempTSMFp = fopen("testTsmLog.txt", "w");
	if(NULL == tempTSMFp)
	{
		printf("\n Error in opening TSM file ");
	}
	tempRxTSMFp = fopen("testRxTsmLog.txt", "w");
	if(NULL == tempRxTSMFp)
	{
		printf("\n Error in opening Rx TSM file ");
	}
	tempRxFP = fopen("testRxLog.txt", "w");
	if(NULL == tempRxFP)
	{
		printf("\n Error in opening Rx log file ");
	}	
	ReqRespLog = fopen("testReqLog.txt", "w");
	if(NULL == ReqRespLog)
	{
		printf("\n Error in opening Req log file ");
	}
	#endif

	/* initialize the serial port for communication */
	//eRetVal = OSAL_Serial_Init(MSTP_GetBaudRate(), pu8FileName);

	/* return value */
	return eRetVal;
}
/* End of BACDEL_MSTP_InitializeStack */

/**
*
* DESCRIPTION
* Function to copy data in receive queue.
*
* @param pu8RxBuf		[in]	pointer to received buffer
* @param u16DataLength	[in]	length of received data
*
* @return voida	[out]	No output parameter
*
*/
void Fill_Data_In_ReceiveQ(uint8_t *pu8RxBuf, uint16_t u16DataLength)
{
	/* Rx Queue buffer pointer */
	msgBuffer_t* pstTmpMsg = NULL;
	/** Process Thread Counter Semaphore. */
	
	g_stMstpVariables.Tsilence = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "Mstp state machine: Entry \r\n");
	#endif

	/* Get pointer to a index of Receive Queue */
	pstTmpMsg = &g_stRxMsgQue.m_astRxQueue[g_stRxMsgQue.m_u32QFillIndex];


    /* Fill Rx queue if m_eState is '0'
     * m_eState is used to avoid buffer overwrite
     */
    if ((u16DataLength > MIN_PACKET) && (! pstTmpMsg->m_eState))
    {
		#ifdef DEBUG_PRINTF
        /* Debug Print */
	    Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: Receive_Thread: \
		Received Packet \r\n");
		#endif

        /* Fill in the Rx data in RX Queue */
	    memcpy(pstTmpMsg->m_u8Buffer, pu8RxBuf, u16DataLength);
		pstTmpMsg->m_u32PktLen = u16DataLength;
	    pstTmpMsg->m_eState = TRUE;


	    /* This function increases the count of the Process Thread semaphore
         * object by a 1.
         * ReleaseSemaphore(handle to semaphore, count) */
        if(!OSAL_Release_Sem(g_hPxSemaphoreHandle, BACNET_ONE))
        {
			/* semaphore release error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0, "Mstp state machine:: \
			Receive_Thread: ReleaseSemaphore error: %ld \r\n",
            Osal_Get_Last_Error());
			#endif
            pstTmpMsg->m_eState = FALSE;
        }
        else
        {
		    /* Increment QFill index of Receive queue */
		    g_stRxMsgQue.m_u32QFillIndex++ ;
		    if(MAX_PACKET <= g_stRxMsgQue.m_u32QFillIndex)
			    g_stRxMsgQue.m_u32QFillIndex = MIN_PACKET;

            /* Get pointer to a index of Receive Queue */
		    pstTmpMsg = &g_stRxMsgQue.m_astRxQueue[g_stRxMsgQue.m_u32QFillIndex];
        }
    }
    else if(pstTmpMsg->m_eState)
    {
        /* Receive queue is not empty */
		#ifdef DEBUG_PRINTF
	    Print_DebugMsg(DEBUG_LEVEL0, "Mstp state machine:: \
		Receive queue is not empty to store packet \r\n");
		#else
		; // dummy statement
		#endif
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "Mstp state machine: : Exit \r\n");
	#endif
}

/************************** end of datalinkMSTPHandler.c file ***************************/
