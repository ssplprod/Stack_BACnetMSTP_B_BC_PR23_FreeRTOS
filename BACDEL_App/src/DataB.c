/**
 * \file 	DataB.C
 * \brief	global data access functions
 * \author
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "crc.h"

//#include "util.h"
#include "Rtos.h"
//#include "NVStorage.h"
#include "DataB.h"

//#include <ApplicationGPIO.h>


////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
////////////////////////////////////////////////////////////////////////////////////////////////

DB_t SMCfg;

DB_t *GetpSMCfg()
{
	return &SMCfg;
}


void InitBACnetStruct()
{
	memset(&SMCfg.g_BACnetCfg, 0, sizeof(SMCfg.g_BACnetCfg));

	SMCfg.g_BACnetCfg.ObjID = 0x55;  // ObjID : A-side = Ox55 (85) , B-side = 0x20 (32)

	// MSTP-only
	SMCfg.g_BACnetCfg.ComID = 5;    // MAC ID:  A-side = 5, B-side = 9
	SMCfg.g_BACnetCfg.MaxMaster = 127;

}


////////////////////////////////////////////////////////////////////////////////////////////////
// Database functions
////////////////////////////////////////////////////////////////////////////////////////////////
#define DATAB_VERSION	1

void SaveFactoryDatatoNV()
{
	uint8_t *pData = (uint8_t *)GetpSMCfg();
//
	NVHeader_t *pDataHeader = (NVHeader_t *)pData;
	pDataHeader->ui32CRC = 0;
	pDataHeader->ui16Version = DATAB_VERSION;
//
//	// Compute CRC
	pDataHeader->ui32CRC = HAL_CRC_Calculate(&hcrc, (uint32_t *) pData, pDataHeader->ui16Size / 4);				// CRC is computed based on uint32_t
//
//	SaveDataWithCRC(pData, sfFactoryCfg_1, TRUE);
}

void ResetToFactoryDefault()
{
	SMCfg.g_Header.ui16Size = sizeof(SMCfg);
	SMCfg.g_Header.ui16Version = DATAB_VERSION;
	InitBACnetStruct();

//	SaveFactoryDatatoNV();
}

void DataB_Init()
{
	memset (&SMCfg, 0, sizeof(SMCfg));

//	uint8_t ui8Ret = ReadDataWithCRC((uint8_t *)&SMCfg, sfFactoryCfg_1, TRUE);

//	if (FALSE == ui8Ret || SMCfg.g_Header.ui16Version != DATAB_VERSION )
	{
		ResetToFactoryDefault();
	}
}

//--------------------------------------------------------------------
// COMS
//
// UART parameters: Convert IMI enums to STM32 constants
uint32_t enumToBaud(eBaudRate_t Baudrate)
{
	switch (Baudrate)
	{
		case ebr_300:
			return 300;
		break;

		case ebr_1200:
			return 1200;
		break;

		case ebr_2400:
			return 2400;
		break;

		case ebr_4800:
			return 4800;
		break;

		case ebr_19200:
			return 19200;
		break;

		case ebr_38400:
			return 38400;
		break;

		case ebr_57600:
			return 57600;
		break;

		case ebr_76800:
			return 76800;
		break;

		case ebr_115200:
			return 115200;
		break;

		case ebr_9600:
		case ebr_Auto:
		default:
			return 9600;
		break;
	}
}

uint32_t enumToParity(eParity_t Parity)
{
	if (ep_Even == Parity)
		return UART_PARITY_EVEN;
	else if (ep_Odd == Parity)
		return UART_PARITY_ODD;
	else
		return UART_PARITY_NONE;
}

uint32_t enumToDataBits(uint8_t	DataBits)
{
	if (9 == DataBits)
		return UART_WORDLENGTH_9B;
	else
		return UART_WORDLENGTH_8B;
}

uint32_t enumToStopBits(uint8_t	StopBits)
{
	if (2 == StopBits)
		return UART_STOPBITS_2;
	else
		return UART_STOPBITS_1;
}



