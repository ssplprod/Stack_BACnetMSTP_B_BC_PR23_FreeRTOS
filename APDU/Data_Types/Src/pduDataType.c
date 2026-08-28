/*************************************************************************
*
*            Copyright (c) by SoftDEL Systems Pvt. Ltd.               
*
*  This software is copyrighted by and is the sole property of 
*  SoftDEL Systems Pvt. Ltd.
*  All rights, title, ownership, or other interests         
*  in the software remain the property of  SoftDEL Systems Pvt. Ltd.  This
*  software may only be used in accordance with the corresponding        
*  license agreement.  Any unauthorized use, duplication, transmission,  
*  distribution, or disclosure of this software is expressly forbidden.  
*
*  This Copyright notice may not be removed or modified without prior    
*  Written consent of SoftDEL Systems Pvt. Ltd.
*
*  SoftDEL Systems Pvt. Ltd. reserves the right to modify this software
*  Without notice.
*
*  SoftDEL Systems Pvt. Ltd.                      info@softdel.com         
*  3rd Floor, Pentagon P4,                        http://www.softdel.com   
*  Magarpatta City, Hadapsar                                                
*  Pune - 411 028
*
*************************************************************************/
/************************************************************************ 
*                                                                         
* FILE
*   bacreal.c 
*
*  AUTHOR
*
*  DESCRIPTION
*   This source file contain encode and decode API's for real and double
*   tyep conversions.
*
*  RELEASE HISTORY
*   DATE                  NAME                      DESCRIPTION
*
*   14-09-2011                                     Initial Draft
*
*************************************************************************/
/*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#include <string.h>
#include "bacDELDef.h"
#include "pduEncodeDecode.h"
#include "pduDataType.h"
#include "miscMiscellaneous.h"

//#include "Debug.h"
/*
*******************************************************************************
*                            DEFINES
*******************************************************************************
*/
#define FLOAT_BYTES     4   /* As Float takes 4 bytes of memory */
#define DOUBLE_BYTES    8   /* As Double takes 8 bytes of memory*/

/** Global variable to save character encoding for all Host devices */
extern BACNET_CHARACTER_STRING_ENCODING g_eCharEncoding;

/*****************************************************************************/
/* To Include the Real Data Type related fucntions */

/**
*
*   Convert Input character array data into real type variable.
*
*    @param pu8Apdu [in] Input APDU packet
*    @param pfRealValue [in] Converted value to be updated into it.
*
*    @return FLOAT_BYTES [out] Returns the number of apdu bytes consumed
*
*/
int32_t Decode_Real(uint8_t * pu8Apdu, Float_t *pfRealValue)
{
    /* Local Union for converting character array into real type */
    union
    {
        uint8_t m_cArrByte[FLOAT_BYTES]; /* Assume that Flot will take 4 bytes*/
        Float_t m_fRealValue;
    } uRealData;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Real:Entry\r\n"); 
	#endif
            
    /* NOTE: assumes the compiler stores Float_t as IEEE-754 Float_t
             Copy Input Data into the Data array of union */
#if BACDEL_BIG_ENDIAN
    uRealData.m_cArrByte[0] = pu8Apdu[0];
    uRealData.m_cArrByte[1] = pu8Apdu[1];
    uRealData.m_cArrByte[2] = pu8Apdu[2];
    uRealData.m_cArrByte[3] = pu8Apdu[3];
#else
    uRealData.m_cArrByte[0] = pu8Apdu[3];
    uRealData.m_cArrByte[1] = pu8Apdu[2];
    uRealData.m_cArrByte[2] = pu8Apdu[1];
    uRealData.m_cArrByte[3] = pu8Apdu[0];
#endif

    /* Update input variable with the converted Float_t value */
    *pfRealValue = uRealData.m_fRealValue;

   /* Retrun 4 as number  bytes of apdu data packet used for Float_t conversion */
   /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Real:Exit\r\n"); 
	#endif
    return FLOAT_BYTES;
}

/**
*    This is the safe side conversion, based on the number of bytes to use
*    Convert Input character array data into the real type variable.
*
*    @param pu8Apdu [in] Input APDU packet
*    @param pfRealValue [in] Converted value to be updated into it
*    @param u32LenValue [in] Number of bytes to be used   
*    @return u32LenValue [out] Returns the number of bytes used
*
*/
int32_t Decode_Real_Safe(uint8_t * pu8Apdu,uint32_t u32LenValue,Float_t *pfRealValue)
{
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Real_Safe:Entry\r\n"); 
	#endif

    /* If Number of bytes to be taken for conversion is not equal to 4  */
    if (u32LenValue != FLOAT_BYTES)
    {
        /* Return zero as Float_t never take more that 4 bytes */
        *pfRealValue = 0.0f;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Decode_Real_Safe:Float_t Overflow\r\n");
		#endif
        return (int32_t) u32LenValue;
    }
    else
    {
        /* Convert to real and return the number of bytes used */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Real_Safe:Exit\r\n");
		#endif
        return Decode_Real(pu8Apdu, pfRealValue);
    }
}


/**
*    It decodes to real value based on APDU tag number
*
*
*    @param pu8Apdu [in] Input APDU packet
*    @param pfRealValue [in] Converted value to be updated into it
*    @param u8TagNumber [in] Tag number
*    @return iLength [out] number of bytes used for conversion
*/

int32_t Decode_Context_Real(uint8_t * pu8Apdu,uint8_t u8TagNumber,Float_t *pfRealValue)
{
    uint32_t u32LenValue;
    int32_t i32Length = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Context_Real:Entry\r\n");
	#endif

    if (Decode_Is_Context_Tag(&pu8Apdu[i32Length], u8TagNumber))
    {
        i32Length +=
            Decode_Tag_Number_And_Value(&pu8Apdu[i32Length], &u8TagNumber,
                                        &u32LenValue);
        i32Length += Decode_Real(&pu8Apdu[i32Length], pfRealValue);
    }
    else 
    {
        i32Length = -1;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Context_Real:Exit\r\n");
	#endif
    return i32Length;
}

/**
*    It Encodes from real value to APDU bytes
*
*    @param pu8Apdu [in] APDU to be filled with the real value
*    @param fRealValue [in] Input Real vlaue
*    @return FLOAT_BYTES [out] Returns the number of apdu bytes update 
                                with real value
*/
int32_t Encode_Bacnet_Real(Float_t fRealValue, uint8_t * pu8Apdu)
{
    /* Local Union for converting real value into character array */
    union
    {
        uint8_t m_cArrByte[4];        /*  Assume that Flot will take 4 bytes*/
        Float_t m_fRealValue;
    } uRealData;

    /* NOTE: assumes the compiler stores Float_t as IEEE-754 Float_t
                Copy Input real  into the Data array of union */
    uRealData.m_fRealValue = fRealValue;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Bacnet_Real:Entry\r\n");
	#endif

#if BACDEL_BIG_ENDIAN
    pu8Apdu[0] = uRealData.m_cArrByte[0];
    pu8Apdu[1] = uRealData.m_cArrByte[1];
    pu8Apdu[2] = uRealData.m_cArrByte[2];
    pu8Apdu[3] = uRealData.m_cArrByte[3];
#else
    pu8Apdu[0] = uRealData.m_cArrByte[3];
    pu8Apdu[1] = uRealData.m_cArrByte[2];
    pu8Apdu[2] = uRealData.m_cArrByte[1];
    pu8Apdu[3] = uRealData.m_cArrByte[0];
#endif

    /* Retrun 4 as number  bytes of apdu data packet used for updating with the
            converted Real value */
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Bacnet_Real:Exit\r\n");
	#endif
    return FLOAT_BYTES;
}

/**
*      It Encodes APDU bytes to double value
*
*
*    @param pu8Apdu [in] APDU to be filled with the double value
*    @param pdDoubleValue [in] Input double vlaue
*    @return DOUBLE_BYTES [out] Return the number of apdu bytes used for
*                              double conversion
*/
int32_t Decode_Double(uint8_t * pu8Apdu,Double_t *pdDoubleValue)
{
    /* Local Union for converting character array into double type */
    union {
        uint8_t m_cArrByte[DOUBLE_BYTES];
        Double_t m_dDoubleValue;
    } uDoubleData;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Double:Entry\r\n");
	#endif

    /* NOTE: assumes the compiler stores Float_t as IEEE-754 Float_t 
                            Copy Input Data into the Data array of the union */
#if BACDEL_BIG_ENDIAN
    uDoubleData.m_cArrByte[0] = pu8Apdu[0];
    uDoubleData.m_cArrByte[1] = pu8Apdu[1];
    uDoubleData.m_cArrByte[2] = pu8Apdu[2];
    uDoubleData.m_cArrByte[3] = pu8Apdu[3];
    uDoubleData.m_cArrByte[4] = pu8Apdu[4];
    uDoubleData.m_cArrByte[5] = pu8Apdu[5];
    uDoubleData.m_cArrByte[6] = pu8Apdu[6];
    uDoubleData.m_cArrByte[7] = pu8Apdu[7];
#else
    uDoubleData.m_cArrByte[0] = pu8Apdu[7];
    uDoubleData.m_cArrByte[1] = pu8Apdu[6];
    uDoubleData.m_cArrByte[2] = pu8Apdu[5];
    uDoubleData.m_cArrByte[3] = pu8Apdu[4];
    uDoubleData.m_cArrByte[4] = pu8Apdu[3];
    uDoubleData.m_cArrByte[5] = pu8Apdu[2];
    uDoubleData.m_cArrByte[6] = pu8Apdu[1];
    uDoubleData.m_cArrByte[7] = pu8Apdu[0];
#endif

    /* Update input variable with the converted Float_t value */
    *pdDoubleValue = uDoubleData.m_dDoubleValue;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Double:Exit\r\n");
	#endif
   /* Retrun 4 as number  bytes of apdu data packet used for Float_t conversion */
    return DOUBLE_BYTES;
}

/**
*    This is the safe side conversion based on the number of bytes to be used.
*    Convert Input character array data into the double type variable.
*
*    @param pu8Apdu [in] Input APDU packet
*    @param pdDoubleValue [in] Converted value to be updated into it
*    @param u32LenValue [in] Number of bytes to be used   
*
*    @return u32LenValue [out] Returns the number of bytes used
*/

int32_t Decode_Double_Safe(uint8_t *pu8Apdu,uint32_t u32LenValue,Double_t *pdDoubleValue)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Double_Safe:Entry\r\n");
	#endif

    /* If Number of bytes to be taken for conversion is not equal to 8 */
    if (u32LenValue != DOUBLE_BYTES)
    {
        /* Return zero as Float_t never take more that 8 bytes */
        *pdDoubleValue = 0.0;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:Decode_Double_Safe:Float_t Overflow\r\n");
		#endif
        return (int32_t) u32LenValue;
    }
    else
    {
        /* Convert to double and return the number of bytes used */
		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Double_Safe:Exit\r\n");
		#endif
        return Decode_Double(pu8Apdu, pdDoubleValue);
    }
}

/**
*    It Encodes from double value to APDU bytes
*
*
*    @param pu8Apdu [in] APDU to be filled with the double value
*    @param dDoubleValue [in] Input double vlaue
*    @return DOUBLE_BYTES [out] Returns the number of apdu bytes update 
                                with double value
*/
int32_t Encode_Bacnet_Double(Double_t dDoubleValue, uint8_t * pu8Apdu)
{
    /* Local Union for converting character array into double type */
    union
    {
        uint8_t m_cArrByte[DOUBLE_BYTES];
        Double_t m_dDoubleValue;
    } uDoubleData;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Bacnet_Double:Entry\r\n");
	#endif

    /* NOTE: assumes the compiler stores Float_t as IEEE-754 Float_t */
    uDoubleData.m_dDoubleValue = dDoubleValue;
#if BACDEL_BIG_ENDIAN
    pu8Apdu[0] = uDoubleData.m_cArrByte[0];
    pu8Apdu[1] = uDoubleData.m_cArrByte[1];
    pu8Apdu[2] = uDoubleData.m_cArrByte[2];
    pu8Apdu[3] = uDoubleData.m_cArrByte[3];
    pu8Apdu[4] = uDoubleData.m_cArrByte[4];
    pu8Apdu[5] = uDoubleData.m_cArrByte[5];
    pu8Apdu[6] = uDoubleData.m_cArrByte[6];
    pu8Apdu[7] = uDoubleData.m_cArrByte[7];
#else
    pu8Apdu[0] = uDoubleData.m_cArrByte[7];
    pu8Apdu[1] = uDoubleData.m_cArrByte[6];
    pu8Apdu[2] = uDoubleData.m_cArrByte[5];
    pu8Apdu[3] = uDoubleData.m_cArrByte[4];
    pu8Apdu[4] = uDoubleData.m_cArrByte[3];
    pu8Apdu[5] = uDoubleData.m_cArrByte[2];
    pu8Apdu[6] = uDoubleData.m_cArrByte[1];
    pu8Apdu[7] = uDoubleData.m_cArrByte[0];
#endif

   /* Retrun 8 as number  bytes of apdu data packet used for updating with the
            converted Double value */
	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Bacnet_Double:Exit\r\n");
	#endif
    return DOUBLE_BYTES;
}

/**
*   It Decodes from double value to APDU bytes based on tag number
* 
*
*   @param pu8Apdu [in] APDU to be decoded to double value
*   @param pdDoubleValue [in] Varibale in which converted value to be copeid
*   @param u8TagNumber [in] Tag number
*   @return FLOAT_BYTES [out] Returns the number of apdu bytes 
*                   updated with the  real value
*
*/
int32_t Decode_Context_Double(uint8_t *pu8Apdu,uint8_t u8TagNumber,
                          Double_t *pdDoubleValue)
{
    uint32_t u32LenValue;
    int32_t i32Length = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Context_Double:Entry\r\n");
	#endif

    /* Validate is the input tage number from the current APDU content */
    if (Decode_Is_Context_Tag(&pu8Apdu[i32Length], u8TagNumber))
    {
        /* Decode Tage number and Value form the APDU */
        i32Length +=
           Decode_Tag_Number_And_Value(&pu8Apdu[i32Length],&u8TagNumber,&u32LenValue);
        i32Length += Decode_Double(&pu8Apdu[i32Length], pdDoubleValue);
    }
    else
    {
        i32Length = -1;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Context_Double:Exit\r\n");
	#endif
    return i32Length;
}
/* end of bacreal.c */
/*****************************************************************************/

/*****************************************************************************/
/* To Include the Int Data Type related fucntions */

/** 
*   Encode datatype unsigned16.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*    @param u16Value [in] Value of type unsigned16.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned16(uint8_t *pu8APDU, uint16_t u16Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Unsigned16:Entry\r\n");
	#endif

    pu8APDU[0] = (uint8_t) ((u16Value & 0xff00) >> 8);
    pu8APDU[1] = (uint8_t) (u16Value & 0x00ff);

    return 2;
}

/** 
*   Decode datatype unsigned16.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pu16Value [out] Value of type unsigned16.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned16(uint8_t *pu8APDU, uint16_t *pu16Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Unsigned16:Entry\r\n");
	#endif

    if (pu16Value) 
    {
        *pu16Value = (uint16_t) ((((uint16_t) pu8APDU[0]) << 8) & 0xff00);
        *pu16Value |= ((uint16_t) (((uint16_t) pu8APDU[1]) & 0x00ff));
    }

    return 2;
}

/** 
*   Encode datatype unsigned24.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param u32Value [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned24(uint8_t *pu8APDU, uint32_t u32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Unsigned24:Entry\r\n");
	#endif

    pu8APDU[0] = (uint8_t) ((u32Value & 0xff0000) >> 16);
    pu8APDU[1] = (uint8_t) ((u32Value & 0x00ff00) >> 8);
    pu8APDU[2] = (uint8_t) (u32Value & 0x0000ff);

    return 3;
}

/** 
*   Decode datatype unsigned24.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pu32Value [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned24(uint8_t *pu8APDU, uint32_t *pu32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Unsigned24:Entry\r\n");
	#endif

    if (pu32Value) 
    {
        *pu32Value = ((uint32_t) ((((uint32_t) pu8APDU[0]) << 16) & 0x00ff0000));
        *pu32Value |= (uint32_t) ((((uint32_t) pu8APDU[1]) << 8) & 0x0000ff00);
        *pu32Value |= ((uint32_t) (((uint32_t) pu8APDU[2]) & 0x000000ff));
    }

    return 3;
}

/** 
*   Encode datatype unsigned32.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*    @param u32Value [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned32(uint8_t *pu8APDU, uint32_t u32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Unsigned32:Entry\r\n");
	#endif

    pu8APDU[0] = (uint8_t) ((u32Value & 0xff000000) >> 24);
    pu8APDU[1] = (uint8_t) ((u32Value & 0x00ff0000) >> 16);
    pu8APDU[2] = (uint8_t) ((u32Value & 0x0000ff00) >> 8);
    pu8APDU[3] = (uint8_t) (u32Value & 0x000000ff);

    return 4;
}

/** 
*   Decode datatype unsigned32.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pu32Value [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned32(uint8_t *pu8APDU, uint32_t *pu32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Unsigned32:Entry\r\n");
	#endif

    if (pu32Value) 
    {
        *pu32Value = ((uint32_t) ((((uint32_t) pu8APDU[0]) << 24) & 0xff000000));
        *pu32Value |= ((uint32_t) ((((uint32_t) pu8APDU[1]) << 16) & 0x00ff0000));
        *pu32Value |= ((uint32_t) ((((uint32_t) pu8APDU[2]) << 8) & 0x0000ff00));
        *pu32Value |= ((uint32_t) (((uint32_t) pu8APDU[3]) & 0x000000ff));
    }

    return 4;
}

/** 
*   Encode datatype signed8.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param i8Value [in] Value of type signed8.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Signed8(uint8_t *pu8APDU, int8_t i8Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Signed8:Entry\r\n");
	#endif

    pu8APDU[0] = (uint8_t) i8Value;

    return 1;
}

/** 
*   Decode datatype unsigned8.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pu32Value [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned8(uint8_t *pu8APDU, uint32_t *pu32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Unsigned8:Entry\r\n");
	#endif

    if (pu32Value) 
    {
        //*pu32Value |= ((uint32_t) (((uint32_t) pu8APDU[0]) & 0x000000ff));
		*pu32Value = pu8APDU[0];
    }

    return 1;
}

/** 
*   Encode datatype unsigned8.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param u8Value [in] Value of type unsigned8.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned8(uint8_t *pu8APDU, uint8_t u8Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Unsigned8:Entry\r\n");
	#endif

    pu8APDU[0] = u8Value;

    return 1;
}

/** 
*   Decode datatype signed8.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pi32Value [out] Value of type signed32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed8(uint8_t *pu8APDU, int32_t *pi32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Signed8:Entry\r\n");
	#endif

    if (pi32Value) 
    {
        /* negative - bit 7 is set */
        if (pu8APDU[0] & 0x80)
            *pi32Value = 0xFFFFFF00;
        else
            *pi32Value = 0;
        *pi32Value |= ((int32_t) (((int32_t) pu8APDU[0]) & 0x000000ff));
    }

    return 1;
}

/** 
*   Encode datatype signed16.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*    @param u16Value [in] Value of type signed16.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Signed16(uint8_t *pu8APDU, int16_t u16Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Signed16:Entry\r\n");
	#endif

    pu8APDU[0] = (uint8_t) ((u16Value & 0xff00) >> 8);
    pu8APDU[1] = (uint8_t) (u16Value & 0x00ff);

    return 2;
}

/** 
*   Decode datatype signed16.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pi32Value [out] Value of type signed32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed16(uint8_t *pu8APDU, int32_t *pi32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Signed16:Entry\r\n");
	#endif

    if (pi32Value) 
    {
        /* negative - bit 7 is set */
        if (pu8APDU[0] & 0x80)
            *pi32Value = 0xFFFF0000;
        else
            *pi32Value = 0;
        *pi32Value |= ((int32_t) ((((int32_t) pu8APDU[0]) << 8) & 0x0000ff00));
        *pi32Value |= ((int32_t) (((int32_t) pu8APDU[1]) & 0x000000ff));
    }

    return 2;
}

/** 
*   Encode datatype signed24.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*    @param i32Value [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_signed24(uint8_t *pu8APDU, int32_t i32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_signed24:Entry.\r\n");
	#endif

    pu8APDU[0] = (uint8_t) ((i32Value & 0xff0000) >> 16);
    pu8APDU[1] = (uint8_t) ((i32Value & 0x00ff00) >> 8);
    pu8APDU[2] = (uint8_t) (i32Value & 0x0000ff);

    return 3;
}

/** 
*   Encode datatype signed24.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pi32Value [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed24(uint8_t *pu8APDU, int32_t *pi32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Signed24:Entry.\r\n");
	#endif

    if (pi32Value) {
        /* negative - bit 7 is set */
        if (pu8APDU[0] & 0x80)
            *pi32Value = 0xFF000000;
        else
            *pi32Value = 0;
        *pi32Value |= ((int32_t) ((((int32_t) pu8APDU[0]) << 16) & 0x00ff0000));
        *pi32Value |= ((int32_t) ((((int32_t) pu8APDU[1]) << 8) & 0x0000ff00));
        *pi32Value |= ((int32_t) (((int32_t) pu8APDU[2]) & 0x000000ff));
    }

    return 3;
}

/** 
*   Encode datatype unsigned32.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*    @param i32Value [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Signed32(uint8_t *pu8APDU, int32_t i32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Signed32:Entry.\r\n");
	#endif

    pu8APDU[0] = (uint8_t) ((i32Value & 0xff000000) >> 24);
    pu8APDU[1] = (uint8_t) ((i32Value & 0x00ff0000) >> 16);
    pu8APDU[2] = (uint8_t) ((i32Value & 0x0000ff00) >> 8);
    pu8APDU[3] = (uint8_t) (i32Value & 0x000000ff);

    return 4;
}

/** 
*   Encode datatype unsigned32.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pi32Value [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed32(uint8_t *pu8APDU, int32_t *pi32Value)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Decode_Signed32:Entry\r\n");
	#endif

    if (pi32Value) {
        *pi32Value = ((int32_t) ((((int32_t) pu8APDU[0]) << 24) & 0xff000000));
        *pi32Value |= ((int32_t) ((((int32_t) pu8APDU[1]) << 16) & 0x00ff0000));
        *pi32Value |= ((int32_t) ((((int32_t) pu8APDU[2]) << 8) & 0x0000ff00));
        *pi32Value |= ((int32_t) (((int32_t) pu8APDU[3]) & 0x000000ff));
    }

    return 4;
}
/* end of decoding_encoding.c */
/*****************************************************************************/

/*****************************************************************************/
/* To Include the String Data Type related fucntions */
/**
*
*DESCRIPTION
*   Bit string initialization 
*
*@param pstBitString [in] Bit String Structure pointer
*@return void
*
*/
void BitString_Init(BACNET_BIT_STRING *pstBitString)
{
	/* local variable */
    uint32_t u32Count;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Init:Entry..."
        "To Include the String Data Type related fucntions\r\n");
	#endif

    /* Reset bits used status flag */
    pstBitString->bits_used = 0;
    
    /* Initialize bit string array with all zero's */
    for (u32Count = 0; u32Count < MAX_BITSTRING_BYTES; u32Count++)
    {
        pstBitString->value[u32Count] = 0;
    }
}

/**
*
*DESCRIPTION
*   Set given value with in a given bit string based on bit position
*
*@param pstBitString [in] Bit String Structure pointer
*@param u8BitNumber [in] Bit position to set
*@param bValue [in] binary value to be set
*@return void
*
*/
void BitString_Set_Bit(BACNET_BIT_STRING *pstBitString, uint8_t u8BitNumber,
                        bool bValue)
{
    /* Get the byte number from the input bit position */
    uint8_t u8ByteNumber = u8BitNumber / 8;   
    uint8_t u8BitMask = 1;       /* Bit mask */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Set_Bit:Entry...Set given value with in a given bit string based on bit position\r\n");
	#endif

    if (u8ByteNumber < MAX_BITSTRING_BYTES) 
    {
        /* Is given bit number alreade been used for other  */
        if (pstBitString->bits_used < (u8BitNumber + 1))
        {
            /* If Not, update bits used variable with the current bit number */
            pstBitString->bits_used = u8BitNumber + 1;
        }
        
        /* Use mask for fetching require bit from the given string */
        u8BitMask = u8BitMask << (u8BitNumber - (u8ByteNumber * 8));
        if (bValue)
        {
            /* If Set to 1 , do OR operation */
            pstBitString->value[u8ByteNumber] |= u8BitMask;
        }
        else
        {
            /* If Set to 0 , do AND with the complement */
            pstBitString->value[u8ByteNumber] &= (~(u8BitMask));
        }
    }/* End of  if (u8ByteNumber < MAX_BITSTRING_BYTES) */
}

/**
*
*DESCRIPTION
*   Retrun the given Bit Status from the input bit string buffer
*
*@param pstBitString [in] Bit String Structure pointer
*@param u8BitNumber [in] Bit position to set
*@return bvalue [out] Bit status
*
*/
bool BitString_Bit(BACNET_BIT_STRING *pstBitString,uint8_t u8BitNumber)
{
    bool bValue = false;    /* Default return is FALSE */
    /* Get the byte number from the input bit number */
    uint8_t u8ByteNumber = u8BitNumber / 8;   
    uint8_t u8BitMask = 1;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Bit:Entry...Retrun the given Bit Status from the input bit string buffer\r\n");
	#endif


    /* Is bit number is with in the Maximum bits range */
    if (u8BitNumber < (MAX_BITSTRING_BYTES * 8))
    {
        /* Get the bit mask value and return the status of the bit */
        u8BitMask = u8BitMask << (u8BitNumber - (u8ByteNumber * 8));
        if (pstBitString->value[u8ByteNumber] & u8BitMask)
        {
            bValue = true;
        }
    }
    return bValue;
}

/**
*
*DESCRIPTION
*   Retruns the last bit used from the bit input bit string
*
*@param pstBitString [in] Bit String Structure pointer
*@return bit_string->bits_used  [out] Last bit used
*
*/
uint8_t BitString_Bits_Used(BACNET_BIT_STRING *pstBitString)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Bits_Used:Entry...Retruns the last bit used from the bit input bit string\r\n");
	#endif

    return pstBitString->bits_used;
}

/**
*
*DESCRIPTION
*   Returns the number of bytes that a bit string is using 
*
*@param pstBitString [in] Bit String Structure pointer
*@return u8ReturnValue  [out] Last bit used
*
*/
uint8_t BitString_Bytes_Used(BACNET_BIT_STRING *pstBitString)
{
    uint8_t u8ReturnValue = 0;    /* Return Default value */
    uint8_t u8UsedBytes = 0;
    uint8_t u8LastBit = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Bytes_Used:Entry...Returns the number of bytes that a bit string is using\r\n");
	#endif

    /* Check is their any bits used or not ?  If not return Zero */
    if (pstBitString->bits_used)
    {
        u8LastBit = pstBitString->bits_used - 1;
        u8UsedBytes = u8LastBit / 8;

        /* add one for the first byte */
        u8UsedBytes++;
        u8ReturnValue = u8UsedBytes;
    }
    return u8ReturnValue;
}

/**
*
*DESCRIPTION
*   Returns the give Octate status form the input bit string 
*
*@param pstBitString [in] Bit String Structure pointer
*@return u8ReturnOctate [out] Last bit used
*
*/
uint8_t BitString_Octet(BACNET_BIT_STRING *pstBitString, uint8_t u8OctetIndex)
{
    uint8_t u8ReturnOctate = 0;  /* Default Return as Zero */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Octet:Entry...Returns the give Octate status form the input bit string\r\n");
	#endif

    /* If input octate is non Zero */
    //if (u8OctetIndex) 
    //{
        /* If its less than maximum bits allowed then only returne the value*/
        if (u8OctetIndex < MAX_BITSTRING_BYTES)
        {
            u8ReturnOctate = pstBitString->value[u8OctetIndex];
        }
    //}
    return u8ReturnOctate;
}

/**
*
*DESCRIPTION
*   Set the in the given bit string buffer with the given octate value 
*
*@param pstBitString [in] Bit String Structure pointer
*@param u8BitIndex [in] Bit position
*@param u8OctetValue [in] Octate value to be set
*@return bReturnStatus [out] Return the status of operation of octate set
*
*/
bool BitString_Set_Octet(BACNET_BIT_STRING *pstBitString,uint8_t u8BitIndex,
                         uint8_t u8OctetValue)
{
    bool bReturnStatus = false; /* Defalut Retrun as false */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Set_Octet:Entry...Set the in the given bit string buffer with the given octate value\r\n");
	#endif

    if (pstBitString)
    {
        /* If its less than maximum bits allowed then set the ocate value*/
        if (u8BitIndex < MAX_BITSTRING_BYTES)
        {
            pstBitString->value[u8BitIndex] = u8OctetValue;
            bReturnStatus = true;
        }
    }
    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Update the bits used status in the Bit string buffer 
*
*@param pstBitString [in] Bit String Structure pointer
*@param u8BytesUsed [in] Number of used bits
*@param u8UnUsedBits[in] Number of un used bits
*@return bReturnStatus [out] Return the status of operation
*
*/
bool BitString_Set_Bits_Used(BACNET_BIT_STRING *pstBitString, 
                             uint8_t u8BytesUsed, uint8_t u8UnUsedBits)
{
    bool bReturnStatus = false; /* Defalut Retrun as false */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Set_Bits_Used:Entry...Update the bits used status in the Bit string buffer\r\n");
	#endif

    if (pstBitString)
    {
        /* FIXME: check that bytes_used is at least one? */
        pstBitString->bits_used = u8BytesUsed * 8;
        pstBitString->bits_used -= u8UnUsedBits;
        bReturnStatus = true;
    }

    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Return the capacity of Bit String buffer
*
*@param pstBitString [in] Bit String Structure pointer
*@return u8ReturnStatus [out] Returns the maximum number of bits allowed
*
*/
uint8_t BitString_Bits_Capacity(BACNET_BIT_STRING *pstBitString)
{
    uint8_t u8ReturnStatus = 0; /* Defalut Retrun as zero  */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Bits_Capacity:Entry... Return the capacity of Bit String buffer\r\n");
	#endif

    if (pstBitString) 
    {
        u8ReturnStatus =  (MAX_BITSTRING_BYTES * 8);
    }
    return u8ReturnStatus;
}

/**
*
*DESCRIPTION
*   Copy Source Bit string into Destination bit string
*
*@param pstDstBitStr [in] Destination Bit String Structure pointer
*@param pstSrcBitStr [in] Source Bit String Structure pointer
*@return bReturnStatus [out] Return the status of operation
*
*/
bool BitString_Copy(BACNET_BIT_STRING *pstDstBitStr, 
                    BACNET_BIT_STRING *pstSrcBitStr)
{
    uint8_t u8Count;
    bool bReturnStatus = false; /* Defalut Retrun as false */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Copy:Entry...Copy Source Bit string into Destination bit string\r\n");
	#endif

    if (pstDstBitStr && pstSrcBitStr)
    {
        /* Copy Destination bits used from Source bit string structure */
        pstDstBitStr->bits_used = pstSrcBitStr->bits_used;
        
        /* Copy bytes form Source to Destination structure */
        for (u8Count = 0; u8Count < MAX_BITSTRING_BYTES; u8Count++) 
        {
            pstDstBitStr->value[u8Count] = pstSrcBitStr->value[u8Count];
        }
        bReturnStatus = true;
    }

    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Copy Source Bit string into Destination bit string
*
*@param pstDstBitStr [in] Destination Bit String Structure pointer
*@param pstSrcBitStr [in] Source Bit String Structure pointer
*@return bReturnStatus [out] Return the status of operation
*
*/
bool BitString_COPY(BACnetBITStr_t *pstDstBitStr, BACNET_BIT_STRING *pstSrcBitStr)
{
    /* locals */
    uint8_t u8Count = 0;
    uint8_t u8UsedBytes = 0;
    bool bReturnStatus = false; /* Defalut Retrun as false */

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    BitString_COPY: entry .\r\n");
	#endif

    if(pstDstBitStr && pstSrcBitStr)
    {
        /* copy bytes */
        for(u8Count =0; u8Count < MAX_BITSTRING_BYTES; u8Count++)
            pstDstBitStr->m_u8TransBits[u8Count] = pstSrcBitStr->value[u8Count];
        /* get the used number of bytes */
        u8UsedBytes = pstSrcBitStr->bits_used / 8;
        if(0 != (pstSrcBitStr->bits_used % 8))
            u8UsedBytes++;
        /* set byte count */
        pstDstBitStr->m_u8ByteCnt = u8UsedBytes;
        /* set unused bits */
        pstDstBitStr->m_u8UnusedBits = ((u8UsedBytes *8)- pstSrcBitStr->bits_used);
        /* update return value */
        bReturnStatus = true;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    BitString_COPY: exit .\r\n");
	#endif

    return bReturnStatus;
}
/**
*
*DESCRIPTION
*  Validates two strings as are they equal or not
*
*@param pstDstBitStr1 [in] First Bit String Structure pointer
*@param pstDstBitStr2 [in] Second Bit String Structure pointer
*@return bReturnStatus [out] Return the status True if they equal else False
*
*/
bool BitString_Same(BACNET_BIT_STRING *pstDstBitStr1,
                    BACNET_BIT_STRING *pstDstBitStr2)
{
    uint32_t u32Count = 0;          /* loop counter */
    uint32_t u32BytesUsed = 0;
    uint8_t u8CompareMask = 0;
    bool bReturnStatus = false; /* Defalut Retrun as false */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:BitString_Same:Entry...Validates two strings as are they equal or not\r\n");
	#endif

    if (pstDstBitStr1 && pstDstBitStr2)
    {
        /* Check the both bits used status  and its validate its value (
                            (Is less than maximum allowed) */
        if ((pstDstBitStr1->bits_used == pstDstBitStr2->bits_used) &&
            (pstDstBitStr1->bits_used / 8 <= MAX_BITSTRING_BYTES))
        {
            /* Get the number of bytes used  (its taken from first , because 
                                     both having the same number of bits used)*/
            u32BytesUsed = (uint32_t) (pstDstBitStr1->bits_used / 8);
            u8CompareMask = 0xFF >> (8 - (pstDstBitStr1->bits_used % 8));

            /* Validate each value of both bit string buffers */
            for (u32Count = 0; u32Count < u32BytesUsed; u32Count++)
            {
                if(pstDstBitStr1->value[u32Count] !=
                    pstDstBitStr2->value[u32Count])
                {
                    bReturnStatus = false;
                }
            }
            if ((pstDstBitStr1->value[u32BytesUsed] & u8CompareMask) !=
                (pstDstBitStr2->value[u32BytesUsed] & u8CompareMask))
            {
                bReturnStatus =  false;
            } 
            else
            {
                bReturnStatus =  true;
            }
        }
    }
    return bReturnStatus;
}

/**
*
*DESCRIPTION
*  Initializes character string with the input character pointer
*
*@param pstCharString [in] Pointer to Character string
*@param pi8InCharValue[in] Pointer to character string to use for init
*@param u32Length[in] Length of buffer to be filled
*@return bReturnStatus [out] returns false if the string exceeds capacity  
*                            initialize by using pi8InCharValue=NULL 
*
*/
bool CharacterString_Init(BACnetCharStr_t *pstCharString, uint8_t u8Encoding,
                          const int8_t *pi8InCharValue, uint32_t u32Length)
{
    bool bReturnStatus = false;        /* return value */
    uint32_t u32Count = 0;   /* counter */
    uint16_t u16CodePage = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:CharacterString_Init:Entry \r\n");
	#endif

    if (pstCharString)
    {
        pstCharString->m_u32StrLen = 0;
        pstCharString->m_u8Encoding = u8Encoding;

		/* decode code page if IBM_MS_DBCS type encoding */
        if(CHARACTER_IBM_MS_DBCS == u8Encoding)
        {
            /* code page is always 2bytes */
            Decode_Unsigned16((uint8_t *)pi8InCharValue, &u16CodePage);
            u32Length = u32Length - 2; // subtract from length
		    if(pi8InCharValue)
        	    pi8InCharValue += 2; // increment pointer by 2
        }

        /* save the code page */
        pstCharString->m_u16CodePage = u16CodePage;

        /* save a byte at the end for NULL (Assumes printable characters )*/
        if(u32Length <= MAX_CHARACTER_STRING_BYTES)
        {

            /* If non Zero, initialize with the input data */
            if (pi8InCharValue)
            {
				for (u32Count = 0; u32Count < MAX_CHARACTER_STRING_BYTES; u32Count++)
                {
                    if (u32Count < u32Length)
                    {
                        pstCharString->m_pu8CharStr[pstCharString->m_u32StrLen]
                                = pi8InCharValue[u32Count];
                                pstCharString->m_u32StrLen++;
                    } 
                    else
                    {
                        pstCharString->m_pu8CharStr[u32Count] = 0;
                    }
                }
            }
            else /* Else initialize with all zeros */
            {
                for (u32Count = 0; u32Count < MAX_CHARACTER_STRING_BYTES;
                    u32Count++)
                {
                    pstCharString->m_pu8CharStr[u32Count] = 0;
                }
            }
            bReturnStatus = true;
        }/* End of if(u32Length <= MAX_SUPPORTED_CHRSTR_LEN) */
        else
            pstCharString->m_u32StrLen = u32Length;
    }
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:CharacterString_Init:Exit\r\n");
	#endif

    return bReturnStatus;
}

/**
*
*DESCRIPTION
*  Initializes character string As per Ansi encoding formate
*
*@param pstCharString [in] Pointer to Character string
*@param pi8InCharValue[in] Pointer to character string, use to init
*@return bReturnStatus [out] Return the status of operation
*
*/
bool CharacterString_Init_ANSI(BACnetCharStr_t *pstCharString,
                                const int8_t *pi8InCharValue)
{
	/* fiunction entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:CharacterString_Init_ANSI:Entry...Initializes character string As per Ansi encoding formate\r\n");
	#endif

    return CharacterString_Init(pstCharString, CHARACTER_ANSI_X34, 
        pi8InCharValue,pi8InCharValue ? Strnlen((void*)pi8InCharValue, MAX_CHARACTER_STRING_BYTES) : 0);
}

/**
*
*DESCRIPTION
*  Copies ANSI standard Source character string into Destination
*
*@param pi8DestCharString [in] Pointer to Destination Character string
*@param pstSrcCharString [in] Pointer to Source Character string
*@param u32DestMaxLen [in] Destination maximum length
*@return bReturnStatus [out] Return the status of operation
*
*/
bool CharacterString_ByteCopy(int8_t *pi8DestCharString, uint32_t u32DestMaxLen,
                               BACnetCharStr_t *pstSrcCharString)
{
    uint32_t u32Count;   /* counter */
    bool bReturnStatus = false;        /* return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:CharacterString_ANSI_Copy:Entry..."
        "Copies ANSI standard Source character string into Destination\r\n");
	#endif

    /* If Source string support ANSI standard and the 
        Destination length is less than the source */
    if (pi8DestCharString && pstSrcCharString)
    {
        if(pstSrcCharString->m_u32StrLen <= u32DestMaxLen)
        {
            for (u32Count = 0; u32Count < pstSrcCharString->m_u32StrLen; u32Count++)
            {
                pi8DestCharString[u32Count] = pstSrcCharString->m_pu8CharStr[u32Count];
            }
            bReturnStatus = true;
        }
    }
    return bReturnStatus;
}

/**
*
*DESCRIPTION
*  Copies ANSI standard Source character string into Destination
*
*@param pstDestString [in] Pointer to Destination Character string
*@param pstSourceString [in] Pointer to Source Character string
*@param u32DestMaxLen [in] Destination maximum length
*@return bReturnStatus [out] Return the status of operation
*
*/
bool CharacterString_Copy(int8_t *pstSrcString, 
    BACnetCharStr_t *pstDestString, uint32_t u32DestMaxLen)
{
    /* locals */
    uint32_t u32Count = 0;
    uint32_t u32StrngLen = 0;
    bool bReturnStatus = false;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: CharacterString_Copy: entry \r\n");
	#endif

    /* null check input pointers */
    if (pstSrcString && pstDestString)
    {
        /* get the string length */
        u32StrngLen = Strnlen(pstSrcString, MAX_CHARACTER_STRING_BYTES);

        /* copy only if the length of string can be accomodated in destination */
        if(u32StrngLen <= u32DestMaxLen)
        {
            /* set encoding */
            pstDestString->m_u8Encoding = g_eCharEncoding;
            /* set length */
            pstDestString->m_u32StrLen = u32StrngLen;

            /* copy bytes */
            for (u32Count = 0; u32Count < u32StrngLen; u32Count++)
                pstDestString->m_pu8CharStr[u32Count] = pstSrcString[u32Count];

            /* return true */
            bReturnStatus = true;
        }
    }// if() ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: CharacterString_Copy: exit \r\n");
	#endif

    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Returns the data from given character string
*   
*@param pstCharString [in] Pointer to standard Character string
*@return pi8Value [out] Returns data of current index of character string,
*                         If no Data available returns NULL.
*
*/
int8_t *CharacterString_Value(BACnetCharStr_t *pstCharString)
{
    int8_t *pi8Value = NULL;   /* Set default as NULL */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:*CharacterString_Value:Entry...Returns the data from given character string\r\n");
	#endif

    /* if not NULL return data else return NULL */
    if (pstCharString)
    {
        pi8Value = pstCharString->m_pu8CharStr;
    }

    return pi8Value;
}


/**
*
*DESCRIPTION
*   Returns the length of the given character string
*   
*@param pstCharString [in] Pointer to standard Character string
*@return u32Length [out] Returns the length
*
*/
uint32_t CharacterString_Length(BACnetCharStr_t *pstCharString)
{
    uint32_t u32Length = 0;  /* Set zero as default */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:CharacterString_Length:Entry...Returns the length of the given character string\r\n");
	#endif

    /* If not NULL return the length else return zero */
    if (pstCharString) 
    {
        /* FIXME: validate length is within bounds? */
        u32Length = pstCharString->m_u32StrLen;
    }
    return u32Length;
}

/**
*
*DESCRIPTION
*   Returns the encoding formate of given charcter string
*   
*@param pstCharString [in] Pointer to standard Character string
*@return u8Encoding [out] Returns the encoding format.
*
*/
uint8_t CharacterString_Encoding(BACnetCharStr_t *pstCharString)
{
    uint8_t u8Encoding = 0; /* Set zero as default */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:CharacterString_Encoding:Entry...Returns the encoding formate of given string\r\n");
	#endif

    if (pstCharString)
    {
        u8Encoding = pstCharString->m_u8Encoding;
    }
    return u8Encoding;
}

/**
*
*DESCRIPTION
*   Initialize the Octate string with the give data
*   
*@param pstOctateString [in] Pointer to Octate string
*@param pu8Data [in] Pointer to charcter string to be filled
*@param u32Length [in] Length of data to be filled
*@return bReturnStatus [out] Returns false if the string exceeds capacity
*                            initialize by using value=NULL
*
*/
bool OctetString_Init(BACnetOctetStr_t *pstOctateString,uint8_t *pu8Data,
                      uint32_t u32Length)
{
    bool bReturnStatus = false;         /* Default Return value as false */
    uint32_t u32Count;                      /* Counter */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Init:Entry...Initialize the Octate string with the give data\r\n");
	#endif

    if (pstOctateString)
    {
       /* Initialize the Octate length and validate input no.of bytes*/
		pstOctateString->m_u32OctetCount = 0;
        if (u32Length <= MAX_OCTET_STRING_BYTES)
        {
            if (pu8Data) 
            {
                /* Initialize Octate string with the input data */
                for (u32Count = 0; u32Count < u32Length; u32Count++)
                {
					pstOctateString->m_ai8OctetStr[pstOctateString->m_u32OctetCount]=
                                                    pu8Data[u32Count];
					pstOctateString->m_u32OctetCount++;
                }
            }
            else
            {
                /* Initialize with all zero's */
                for (u32Count = 0;u32Count < MAX_OCTET_STRING_BYTES; u32Count++)
                {
					pstOctateString->m_ai8OctetStr[u32Count] = 0;
                }
            }
            bReturnStatus = true;
        }
        else
			pstOctateString->m_u32OctetCount = u32Length;
    }
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Init:Exit\r\n");
	#endif

    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Copy Destination octate string with the source
*   
*@param pstDestOctateStr [in] Pointer to Destination Octate string
*@param pstSrcOctateStr [in] Pointer to Destination Octate string
*@return bReturnStatus [out] Returns false if the string exceeds capacity
*                            initialize by using value=NULL
*
*/
bool OctetString_Copy(BACnetOctetStr_t * pstDestOctateStr,
                      BACnetOctetStr_t * pstSrcOctateStr)
{
    bool bReturnStatus = false;         /* Default Return value as false */
//    bReturnStatus =  OctetString_Init(pstDestOctateStr, 
//                            OctetString_Value(pstSrcOctateStr),
//                            OctetString_Length(pstSrcOctateStr));
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Copy:Entry...Copy Destination octate string with the source\r\n");
	#endif

    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Append given octate string with the input data
*   
*@param pstOctateString [in] Pointer to Destination Octate string
*@param pu8Data [in] Pointer to Data to be filled
*@param u32Length [in] Length of data
*@return bReturnStatus [out] Returns false if the string exceeds capacity
*
*/
bool OctetString_Append(BACnetOctetStr_t *pstOctateString, uint8_t *pu8Data,
                        uint32_t u32Length)
{
    uint32_t u32Count;   /* Counter */
    bool bReturnStatus = false;        /* Return Default value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Append:Entry.. Append given octate string with the input data\r\n");
	#endif

    if (pstOctateString) 
    {
        /* Validate input octate string length for append */
		if ((u32Length + pstOctateString->m_u32OctetCount) <= MAX_OCTET_STRING_BYTES)
        {
            for (u32Count = 0; u32Count < u32Length; u32Count++)
            {
				pstOctateString->m_ai8OctetStr[pstOctateString->m_u32OctetCount]=pu8Data[u32Count];
				pstOctateString->m_u32OctetCount++;
            }
            bReturnStatus = true;
        }
    }
    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   This function sets a new length without changing the value.
*   If length exceeds capacity, no modification happens and
*   function returns false
*   
*@param pstOctateString [in] Pointer to Destination Octate string
*@param u32Length [in] New length to truncate
*@return bReturnStatus [out] Returns false if the string exceeds capacity
*
*/
bool OctetString_Truncate(BACnetOctetStr_t *pstOctateString,
                          uint32_t u32Length)
{
    bool bReturnStatus = false;        /* Return Default value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Truncate:Entry   sets a new length\r\n");
	#endif

    if (pstOctateString)
    {
        /* Check the input length is with in the capcity range */
        if (u32Length <= MAX_OCTET_STRING_BYTES) 
        {
			pstOctateString->m_u32OctetCount = u32Length;
            bReturnStatus = true;
        }
    }
    return bReturnStatus;
}

/**
*
*DESCRIPTION
*   Returns the value of given octate string
*
*@param pstOctateString [in] Pointer to Destination Octate string
*@return pu8Value [out] Returns a pointer to the value
*
*/
uint8_t *OctetString_Value(BACnetOctetStr_t *pstOctateString)
{
    uint8_t *pu8Value = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:*OctetString_Value:Entry and Returns the value of given octate string\r\n");
	#endif

    if (pstOctateString)
    {
		pu8Value = pstOctateString->m_ai8OctetStr;
    }

    return pu8Value;
}

/**
*
*DESCRIPTION
*   Returns the value of given octate string
*
*@param pstOctateString [in] Pointer to Destination Octate string
*@return u32Length [out] Returns the length.
*
*/
uint32_t OctetString_Length(BACnetOctetStr_t *pstOctateString)
{
    uint32_t u32Length = 0;     /* Return Default value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Length:Entry and Returns the value of given octate string\r\n");
	#endif

    if (pstOctateString)
    {
        /* FIXME: validate length is within bounds? */
		u32Length = pstOctateString->m_u32OctetCount;
    }
    return u32Length;
}

/**
*
*DESCRIPTION
*   Returns the maximum capacity
*
*@param pstOctateString [in] Pointer to Destination Octate string
*@return u32Length [out] Returns the maximum capacity
*
*/
uint32_t OctetString_Capacity(BACnetOctetStr_t *pstOctateString)
{
    uint32_t u32Length = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Capacity:Entry  Returns the maximum capacity \r\n");
	#endif

    if (pstOctateString)
    {
        /* FIXME: validate length is within bounds? */
        u32Length = MAX_OCTET_STRING_BYTES;
    }

    return u32Length;
}

/**
*
*DESCRIPTION
*   Compare two octate strings and return TRUE if they are same.
*
*@param pstDstOctateString [in] Pointer to Destination Octate string
*@param pstSrcOctateString [in] Pointer to Destination Octate string
*@return bReturnStatus [out] Returns true if the same length and contents
*
*/
bool OctetString_Value_Same(BACnetOctetStr_t *pstDstOctateString,
                            BACnetOctetStr_t *pstSrcOctateString)
{
    uint32_t u32Count = 0;       /* loop counter */
    bool bReturnStatus = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:OctetString_Value_Same:Entry  Compare two octate, TRUE if they are same\r\n");
	#endif

    if (pstDstOctateString && pstSrcOctateString) 
    {
        /* Validate length and capacity */
		if ((pstDstOctateString->m_u32OctetCount == pstSrcOctateString->m_u32OctetCount) &&
			(pstDstOctateString->m_u32OctetCount <= MAX_OCTET_STRING_BYTES)) 
        {
            /* Check content byte by byte */
			for (u32Count = 0; u32Count < pstDstOctateString->m_u32OctetCount; u32Count++) 
            {
				if (pstDstOctateString->m_ai8OctetStr[u32Count] != 
					pstSrcOctateString->m_ai8OctetStr[u32Count])
                {
                    bReturnStatus = false;
                }
            }
            bReturnStatus = true;
        }
    }
    return bReturnStatus;
}



bool Bitstring_PropParsing(int8_t *pu8PropertyVal,
                        BACNET_PROPERTY_VALUE * value)
{
    uint8_t *pu8InData = NULL;
    int8_t i8bit = 0;
    uint8_t u8count = 0;
	int8_t i8Usedbits = 0;
    uint8_t u8charStr[MAX_BIT_SUPPORT] = {0};

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Bitstring_PropParsing:Entry \r\n");
	#endif

    memset(&value->uValue.m_stBit_String, 0, sizeof(BACnetBITStr_t));
    pu8InData = (uint8_t *)pu8PropertyVal;
    /* copy into local string */
    u8count = (uint8_t)strlen((const char *)pu8InData);
    if(u8count > MAX_BIT_SUPPORT)
        return false;
    memcpy(&u8charStr[0],pu8InData,u8count);
    pu8InData = &u8charStr[0];
    u8count = 0;    
    pu8InData = (uint8_t *)strtok((char *)pu8InData,"{ ,");
    while(NULL != pu8InData)
    {
        if(i8bit > 8)
        {
            i8bit = 0;
            u8count++;
        }
        if('1' == *pu8InData)
        {
			BIT_SET(value->uValue.m_stBit_String.m_u8TransBits[u8count],i8bit);
        }
        pu8InData = (uint8_t *)strtok(NULL," ,}");
        i8bit++;
        i8Usedbits++;
    }
	value->uValue.m_stBit_String.m_u8ByteCnt = (i8Usedbits/8)+1;
	value->uValue.m_stBit_String.m_u8UnusedBits = 8 - (i8Usedbits%8);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Bitstring_PropParsing:Exit \r\n");
	#endif

    return true;
}

#ifdef BACDEL_PR23
/**
*
* DESCRIPTION
*
* This function is used to encode unsigned40 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the encoded octets.
* @param u64Value 	[in] Value of type unsigned40.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Encode_Unsigned40(
	uint8_t *pu8APDU,
	Uint64_t u64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Unsigned40: Entry\r\n");
	#endif

	pu8APDU[0] = (uint8_t)((u64Value & 0xff00000000) >> 32);
	pu8APDU[1] = (uint8_t)((u64Value & 0x00ff000000) >> 24);
	pu8APDU[2] = (uint8_t)((u64Value & 0x0000ff0000) >> 16);
	pu8APDU[3] = (uint8_t)((u64Value & 0x000000ff00) >> 8);
	pu8APDU[4] = (uint8_t)(u64Value & 0x00000000ff);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Unsigned40: Exit\r\n");
	#endif

	return 5;
}

/**
*
* DESCRIPTION
*
* This function is used to encode unsigned48 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the encoded octets.
* @param u64Value 	[in] Value of type unsigned48.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Encode_Unsigned48(
	uint8_t *pu8APDU,
	Uint64_t u64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Unsigned48: Entry\r\n");
	#endif

	pu8APDU[0] = (uint8_t)((u64Value & 0xff0000000000) >> 40);
	pu8APDU[1] = (uint8_t)((u64Value & 0x00ff00000000) >> 32);
	pu8APDU[2] = (uint8_t)((u64Value & 0x0000ff000000) >> 24);
	pu8APDU[3] = (uint8_t)((u64Value & 0x000000ff0000) >> 16);
	pu8APDU[4] = (uint8_t)((u64Value & 0x00000000ff00) >> 8);
	pu8APDU[5] = (uint8_t)(u64Value & 0x0000000000ff);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Unsigned48: Exit\r\n");
	#endif

	return 6;
}

/**
*
* DESCRIPTION
*
* This function is used to encode unsigned56 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the encoded octets.
* @param u64Value 	[in] Value of type unsigned56.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Encode_Unsigned56(
	uint8_t *pu8APDU,
	Uint64_t u64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Unsigned56: Entry\r\n");
	#endif

	pu8APDU[0] = (uint8_t)((u64Value & 0xff000000000000) >> 48);
	pu8APDU[1] = (uint8_t)((u64Value & 0x00ff0000000000) >> 40);
	pu8APDU[2] = (uint8_t)((u64Value & 0x0000ff00000000) >> 32);
	pu8APDU[3] = (uint8_t)((u64Value & 0x000000ff000000) >> 24);
	pu8APDU[4] = (uint8_t)((u64Value & 0x00000000ff0000) >> 16);
	pu8APDU[5] = (uint8_t)((u64Value & 0x0000000000ff00) >> 8);
	pu8APDU[6] = (uint8_t)(u64Value & 0x000000000000ff);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Unsigned56: Exit\r\n");
	#endif

	return 7;
}

/**
*
* DESCRIPTION
*
* This function is used to encode unsigned64 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the encoded octets.
* @param u64Value 	[in] Value of type unsigned64.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Encode_Unsigned64(
	uint8_t *pu8APDU,
	Uint64_t u64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Unsigned64: Entry\r\n");
	#endif

	pu8APDU[0] = (uint8_t)((u64Value & 0xff00000000000000) >> 56);
	pu8APDU[1] = (uint8_t)((u64Value & 0x00ff000000000000) >> 48);
	pu8APDU[2] = (uint8_t)((u64Value & 0x0000ff0000000000) >> 40);
	pu8APDU[3] = (uint8_t)((u64Value & 0x000000ff00000000) >> 32);
	pu8APDU[4] = (uint8_t)((u64Value & 0x00000000ff000000) >> 24);
	pu8APDU[5] = (uint8_t)((u64Value & 0x0000000000ff0000) >> 16);
	pu8APDU[6] = (uint8_t)((u64Value & 0x000000000000ff00) >> 8);
	pu8APDU[7] = (uint8_t)(u64Value & 0x00000000000000ff);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Unsigned64: Exit\r\n");
	#endif

	return 8;
}

/**
*
* DESCRIPTION
*
* This function is used to decode unsigned40 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the decoded octets.
* @param pu64Value 	[in] Value of type unsigned40.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Decode_Unsigned40(
	uint8_t *pu8APDU,
	Uint64_t *pu64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Decode_Unsigned40: Entry\r\n");
	#endif

	/* validate pointer */
	if(NULL != pu64Value)
	{
		*pu64Value = ((Uint64_t)((((Uint64_t)pu8APDU[0]) << 32) & 0xff00000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[1]) << 24) & 0x00ff000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[2]) << 16) & 0x0000ff0000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[3]) << 8) & 0x000000ff00));
		*pu64Value |= ((Uint64_t)(((Uint64_t)pu8APDU[4]) & 0x00000000ff));
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Decode_Unsigned40: Exit\r\n");
	#endif

	return 5;
}

/**
*
* DESCRIPTION
*
* This function is used to decode unsigned48 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the decoded octets.
* @param pu64Value 	[in] Value of type unsigned48.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Decode_Unsigned48(
	uint8_t *pu8APDU,
	Uint64_t *pu64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Decode_Unsigned48: Entry\r\n");
	#endif

	/* validate pointer */
	if(NULL != pu64Value)
	{
		*pu64Value = ((Uint64_t)((((Uint64_t)pu8APDU[0]) << 40) & 0xff0000000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[1]) << 32) & 0x00ff00000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[2]) << 24) & 0x0000ff000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[3]) << 16) & 0x000000ff0000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[4]) << 8) & 0x00000000ff00));
		*pu64Value |= ((Uint64_t)(((Uint64_t)pu8APDU[5]) & 0x0000000000ff));
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Decode_Unsigned48: Exit\r\n");
	#endif

	return 6;
}

/**
*
* DESCRIPTION
*
* This function is used to decode unsigned56 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the decoded octets.
* @param pu64Value 	[in] Value of type unsigned56.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Decode_Unsigned56(
	uint8_t *pu8APDU,
	Uint64_t *pu64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Decode_Unsigned56: Entry\r\n");
	#endif

	/* validate pointer */
	if(NULL != pu64Value)
	{
		*pu64Value = ((Uint64_t)((((Uint64_t)pu8APDU[0]) << 48) & 0xff000000000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[1]) << 40) & 0x00ff0000000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[2]) << 32) & 0x0000ff00000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[3]) << 24) & 0x000000ff000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[4]) << 16) & 0x00000000ff0000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[5]) << 8) & 0x0000000000ff00));
		*pu64Value |= ((Uint64_t)(((Uint64_t)pu8APDU[6]) & 0x000000000000ff));
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Decode_Unsigned56: Exit\r\n");
	#endif

	return 7;
}

/**
*
* DESCRIPTION
*
* This function is used to decode unsigned64 datatype.
*
* @param pu8APDU	[in] Buffer which will hold the decoded octets.
* @param pu64Value 	[in] Value of type unsigned64.
*
* @return int32_t 	[out] Returns number of Bytes consumed.
*/
int32_t Decode_Unsigned64(
	uint8_t *pu8APDU,
	Uint64_t *pu64Value)
{
	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Decode_Unsigned64: Entry\r\n");
	#endif

	if(NULL != pu64Value)
	{
		*pu64Value = ((Uint64_t)((((Uint64_t)pu8APDU[0]) << 56) & 0xff00000000000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[1]) << 48) & 0x00ff000000000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[2]) << 40) & 0x0000ff0000000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[3]) << 32) & 0x000000ff00000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[4]) << 24) & 0x00000000ff000000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[5]) << 16) & 0x0000000000ff0000));
		*pu64Value |= ((Uint64_t)((((Uint64_t)pu8APDU[6]) << 8) & 0x000000000000ff00));
		*pu64Value |= ((Uint64_t)(((Uint64_t)pu8APDU[7]) & 0x00000000000000ff));
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Decode_Unsigned64: Exit\r\n");
	#endif

	return 8;
}
#endif /* BACDEL_PR23 */

/* End of bacstr.c file */
/*****************************************************************************/

