#ifndef DEFSTRUCT_H
#define DEFSTRUCT_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	uint16_t ui16Version;
	uint16_t ui16Size;
	uint32_t ui32CRC;
} NVHeader_t;


typedef enum {
	ebr_Auto,		// Automatic
	ebr_9600,
	ebr_19200,
	ebr_38400,
	ebr_57600,
	ebr_76800,
	ebr_115200,		// Further ones are not supported by BACnet MS/TP
	ebr_300,
	ebr_1200,
	ebr_2400,
	ebr_4800,
	ebr_Last
} eBaudRate_t;

typedef enum {
	ep_None,		// None
	ep_Odd,
	ep_Even,
} eParity_t;

typedef struct __attribute__ ((__packed__)) {
	eBaudRate_t Baudrate;
	eParity_t 	Parity;
	uint8_t		DataBits;
	uint8_t		StopBits;
	uint8_t		DummyVal;	// A struct > 4 bytes is more easy to manage in Tsk_Communication
} Cfg485_t;

//Cfg485_t setUart;

typedef struct __attribute__ ((__packed__)) {
	uint32_t	ObjID;
	uint8_t		ComID;					// aka MAC address (MSTP only)
	uint8_t		MaxMaster;				// MSTP only
	uint16_t	IPPort;					// IP only
} BACnetCfg_t;




typedef struct
{
	NVHeader_t 			g_Header;
	Cfg485_t			g_Com485Cfg;
	BACnetCfg_t			g_BACnetCfg;
} DB_t;

#endif
