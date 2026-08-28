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
*   SoftDEL Systems Ltd.                                 india@softdel.com
*   3rd Floor, Pentagon P4,                              http://www.softdel.com
*    Magarpatta City, Hadapsar
*    Pune - 411 028       
*
*    File Name - bacdcode.c
*
*   RELEASE HISTORY                                                             
*    DATE          NAME                 DESCRIPTION
*   15/06/2011    Ashish Verma         The file is modified as per SoftDel 
*                                      coding standard.
*   12/07/2011    M.Venu               Added Encode_Application_Byte
*   23/07/2011    Harshal Mangale      Addded Decode_Context_Tag function
*   27/07/2011    M.Venu               Encode_Application_Byte function changed
*                                      for Bit String data type
*   03/08/2011    M.Venu               Made Byte_Reverse_Bits function as global
*   05/08/2011    M.Venu               Added Encode_Application_StampTime,
*                                      Encode_Application_StampDateTime ,
*                                      Encode_Application_StampUnsigned functions
*   08/08/2011    Ashish Verma         Added Decode_Context_Boolean_Value().
*   19/09/2011    M.Venu               Removed warnings (L4)
*   22/09/2011    M.Venu               Defect Fix of R2 -18
*   23/09/2011    Ashish Verma         Modified BACNET_CHARACTER_STRING
*                                      structure to Pr_BACnetCharStr_t in 
*                                      BACApp_Encode_Data_Type().
******************************************************************************/
#include <string.h>
#include "bacDELDef.h"
#include "pduEncodeDecode.h"
#include "pduDataType.h"
#include "miscMiscellaneous.h"

/** Global variable to save character encoding for all Host devices */
extern BACNET_CHARACTER_STRING_ENCODING g_eCharEncoding;
extern uint16_t g_u16CodePage;

#define CODE_PAGE_MSB   ((g_u16CodePage >> 8) & 0xFF)
#define CODE_PAGE_LSB   ((g_u16CodePage & 0x00FF) & 0xFF)

/** 
*   Encodes the max-segments-accepted & max-APDU-length-accepted octet.
*   It is described in section 20.1.2.4 & 20.1.2.5 of the BACnet standard.
*   @param i32Max_Segs [in] Specifies the maximum number of segments that 
*                        the device will accept.
*   @param i32Max_APDU [in] Specifies the maximum size of a single APDU 
*                        that the issuing device will accept.
*   @return Returns the encoded octet. 
*/
uint8_t Encode_Max_Segs_Max_Apdu(int32_t i32Max_Segs, int32_t i32Max_APDU)
{
    uint8_t u8Octet = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Max_Segs_Max_Apdu : entry \r\n");
	#endif

    if(i32Max_Segs < 2)           /* Unspecified number of segments accepted. */
        u8Octet = 0;
    else if (i32Max_Segs < 4)      /* 2 segments accepted. */
        u8Octet = 0x10;
    else if (i32Max_Segs < 8)      /* 4 segments accepted. */
        u8Octet = 0x20;
    else if (i32Max_Segs < 16)     /* 8 segments accepted. */
        u8Octet = 0x30;
    else if (i32Max_Segs < 32)     /* 16 segments accepted. */
        u8Octet = 0x40;
    else if (i32Max_Segs < 64)     /* 32 segments accepted. */
        u8Octet = 0x50;
    else if (i32Max_Segs == 64)    /* 64 segments accepted. */
        u8Octet = 0x60;
    else
        u8Octet = 0x70;           /* Greater than 64 segments accepted. */

    if(i32Max_APDU <= 50)         /* i32Max_APDU must be 50 octets minimum */
        u8Octet |= 0x00;
    else if (i32Max_APDU <= 128)
        u8Octet |= 0x01;
    else if (i32Max_APDU <= 206)   /* Fits in a LonTalk frame */
        u8Octet |= 0x02;
    else if (i32Max_APDU <= 480)   /* Fits in an ARCNET or MS/TP frame */
        u8Octet |= 0x03;
    else if (i32Max_APDU <= 1024)  
        u8Octet |= 0x04;
    else if (i32Max_APDU <= 1476)  /* Fits in an ISO 8802-3 frame */
        u8Octet |= 0x05;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Max_Segs_Max_Apdu : exit \r\n");
	#endif
    return u8Octet;
}

/** 
*   Decodes the max-segments-accepted. It is described in section 20.1.2.4 
*   of the BACnet standard.
*   @param iOctet [in] Encoded octet containing max-segments-accepted & 
*                      max-APDU-length-accepted values.
*   @return Returns the value of max-segments-accepted. 
*/
int32_t Decode_Max_Segs(uint8_t u8Octet)
{
    int32_t i32Max_Segs = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Max_Segs: entry \r\n");
	#endif

    switch(u8Octet & 0xF0) 
    {
        case 0:                 
            i32Max_Segs = 0;       /* Unspecified number of segments accepted. */
            break;
        case 0x10:
            i32Max_Segs = 2;       /* 2 segments accepted. */
            break;
        case 0x20:
            i32Max_Segs = 4;       /* 4 segments accepted. */
            break;
        case 0x30:
            i32Max_Segs = 8;       /* 8 segments accepted. */
            break;
        case 0x40:
            i32Max_Segs = 16;      /* 16 segments accepted. */
            break;
        case 0x50:
            i32Max_Segs = 32;      /* 32 segments accepted. */
            break;
        case 0x60:
            i32Max_Segs = 64;      /* 64 segments accepted. */
            break;
        case 0x70:
            i32Max_Segs = 65;      /* Greater than 64 segments accepted. */
            break;
        default:
            break;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Max_Segs: exit \r\n");
	#endif
    return i32Max_Segs;
}

/** 
*   Decodes the max-APDU-length-accepted.It is described in section 20.1.2.5 
*   of the BACnet standard. 
*   @param iOctet [in] Encoded octet containing max-segments-accepted & 
*                     max-APDU-length-accepted values.
*   @return Returns the value of max-APDU-length-accepted. 
*/
int32_t Decode_Max_Apdu(uint8_t u8Octet)
{
    int32_t i32Max_APDU = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Max_Apdu: entry \r\n");
	#endif

    switch(u8Octet & 0x0F)
    {
        case 0:
            i32Max_APDU = 50;      /* max_apdu must be 50 octets minimum */
            break;
        case 1:
            i32Max_APDU = 128;     
            break;
        case 2:
            i32Max_APDU = 206;     /* Fits in a LonTalk frame */
            break;
        case 3:
            i32Max_APDU = 480;     /* Fits in an ARCNET or MS/TP frame */
            break;
        case 4:
            i32Max_APDU = 1024;
            break;
        case 5:
            i32Max_APDU = 1476;    /* Fits in an ISO 8802-3 frame */
            break;
        default:
            /* MAX_APDU_LENGTH Supported by Device */
            i32Max_APDU = MAX_APDU_LENGTH_ACCEPTED;
            break;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Max_Apdu: exit \r\n");
	#endif
    return i32Max_APDU;
}


/** 
*   Encodes the BACnet Tags.  
*   Following are found in clause 20.2.1 General Rules for Encoding BACnet Tags
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Application datatypes or Context Specific tag numbers.
*   @param context_specific [in] Class feild of BACnet tag.
*   @param len_value_type [in] Length/Value/Type feild of BACnet tag.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Tag(uint8_t *pu8APDU, uint8_t u8Tag_Number, bool bContext_Specific,
        uint32_t u32Len_Value_Type)
{
    int32_t i32Len = 1;                       /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Tag: entry \r\n");
	#endif

    pu8APDU[0] = 0;                           /* Update the "CLASS" bit as Application tag 
                                               in the pu8APDU. */           
    if(bContext_Specific)    
    {
        pu8APDU[0] = BIT3;                    /* Update the "CLASS" bit as Context Specfic 
                                               tag in the pu8APDU. */
    }

    if(u8Tag_Number <= 14)                   
    {
        pu8APDU[0] |= (u8Tag_Number << 4);     /* The value of Tag Number bit feild. */
    } 
    else                                    
    {                                       
        pu8APDU[0] |= 0xF0;                   /* Additional tag byte after this byte for */
        pu8APDU[1] = u8Tag_Number;             /* extended tag byte */
        i32Len++;
    }

    /* NOTE: additional i32Len byte(s) after extended tag byte */
    /* If Value is upto 4, update Length/Value/Type feild accordingly. */
    if (u32Len_Value_Type <= 4) 
    {                                      
        pu8APDU[0] |= u32Len_Value_Type;        
    }                                          
                                              
    else 
    { 
        pu8APDU[0] |= 5;               
        /* If Value is upto 253, update Length/Value/Type feild to 5, and add 
         * an octet which contains the Data Length.
         */
        if (u32Len_Value_Type <= 253) 
        {                                             
            pu8APDU[i32Len++] = (uint8_t) u32Len_Value_Type;  
        }
        /* If Value is > 253, update Length/Value/Type feild to 5, and add two
         * octets which contains the data length represented as a 16-bit binary 
         * integer with the most significant octet first.
         */
        else if (u32Len_Value_Type <= 65535)
        {                           
            pu8APDU[i32Len++] = 254;     
            i32Len += Encode_Unsigned16(&pu8APDU[i32Len], (uint16_t) u32Len_Value_Type);
        }
        /* If Value is > 65535, update Length/Value/Type feild to 5, and add 4
         * octets which contains the data length represented as a 32-bit binary 
         * integer with the most significant octet first.
         */
        else 
        {
            pu8APDU[i32Len++] = 255;
            i32Len += Encode_Unsigned32(&pu8APDU[i32Len], u32Len_Value_Type);
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Tag: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes the Opening Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [out] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Context Specific tag numbers.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Opening_Tag(uint8_t *pu8APDU, uint8_t u8Tag_Number)
{
    int32_t i32Len = 1;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Opening_Tag: entry \r\n");
	#endif

    /* set class field to context specific */
    pu8APDU[0] = BIT3;
    /* additional tag byte after this byte for extended tag byte */
    if (u8Tag_Number <= 14) 
    {
        pu8APDU[0] |= (u8Tag_Number << 4);
    } 
    else 
    {
        pu8APDU[0] |= 0xF0;
        pu8APDU[1] = u8Tag_Number;     /* extended tag byte */
        i32Len++;
    }
    pu8APDU[0] |= 6;      /* Set type field to opening tag */

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Opening_Tag: exit \r\n");
	#endif
	
    return i32Len;
}

/** 
*   Encodes the Closing Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [out] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Context Specific tag numbers.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Closing_Tag(uint8_t *pu8APDU, uint8_t u8Tag_Number)
{
    int32_t i32Len = 1;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Closing_Tag: entry \r\n");
	#endif

    /* set class field to context specific */
    pu8APDU[0] = BIT3;
    /* additional tag byte after this byte for extended tag byte */
    if (u8Tag_Number <= 14) 
    {
        pu8APDU[0] |= (u8Tag_Number << 4);
    } 
    else 
    {
        pu8APDU[0] |= 0xF0;
        pu8APDU[1] = u8Tag_Number;     /* extended tag byte */
        i32Len++;
    }
    pu8APDU[0] |= 7;  /* Set type field to closing tag */

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Closing_Tag: exit \r\n");
	#endif
	
    return i32Len;
}


/** 
*   Decodes the Tag Number of BACnet Tags.  
*   Following are found in clause 20.2.1.2, Tag Number.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [out] Pointer to Application datatypes or 
*                           Context Specific tag numbers.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number(uint8_t *pu8APDU, uint8_t *pu8Tag_Number)
{
    int32_t i32Len = 1;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number: entry \r\n");
	#endif

    /* decode the tag number first */
    if (IS_EXTENDED_TAG_NUMBER(pu8APDU[0])) 
    {
        /* extended tag */
        if (pu8Tag_Number) 
        {
            *pu8Tag_Number = pu8APDU[1];
        }
        i32Len++;
    } 
    else 
    {
        if (pu8Tag_Number) 
        {
            *pu8Tag_Number = (uint8_t) (pu8APDU[0] >> 4);
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes the Tag Number of BACnet Tags, but will safely fail 
*   if packet has been truncated.  
*   Following are found in clause 20.2.1.2, Tag Number.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [out] Pointer to Application datatypes or 
*                           Context Specific tag numbers.
*   @param u32APDU_Len_Remaining [in] Length of APDU.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number_Safe(uint8_t *pu8APDU, uint32_t u32APDU_Len_Remaining, 
        uint8_t *pu8Tag_Number)
{
    int32_t i32Len = 0;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number_Safe: entry \r\n");
	#endif

    /* decode the tag number first */
    if (u32APDU_Len_Remaining >= 1) 
    {
        if (IS_EXTENDED_TAG_NUMBER(pu8APDU[0]) && u32APDU_Len_Remaining >= 2) 
        {
            /* extended tag */
            if (pu8Tag_Number) 
            {
                *pu8Tag_Number = pu8APDU[1];
            }
            i32Len = 2;
        } 
        else 
        {
            if (pu8Tag_Number) 
            {
                *pu8Tag_Number = (uint8_t) (pu8APDU[0] >> 4);
            }
            i32Len = 1;
        }
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number_Safe: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes the Opening Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @return TRUE/FALSE. 
*/
bool Decode_Is_Opening_Tag(uint8_t *pu8APDU)
{
	/* function entry - exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Opening_Tag: entry - exit \r\n");
	#endif
    return (bool) ((pu8APDU[0] & 0x07) == 6);
}

/** 
*   Decodes the Closing Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @return TRUE/FALSE. 
*/
bool Decode_Is_Closing_Tag(uint8_t *pu8APDU)
{
	/* function entry - exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Closing_Tag: entry - exit \r\n");
	#endif
    return (bool) ((pu8APDU[0] & 0x07) == 7);
}

/** 
*   Decodes the BACnet Tag Number and its Value.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pu32Value [out] Pointer to the decoded value.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number_And_Value(uint8_t *pu8APDU, uint8_t *pu8Tag_Number,
        uint32_t *pu32Value)
{
    int32_t i32Len = 1;
    uint16_t uValue16 = 0;
    uint32_t uValue32 = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number_And_Value: entry \r\n");
	#endif

    i32Len = Decode_Tag_Number(&pu8APDU[0], pu8Tag_Number);
    if (IS_EXTENDED_VALUE(pu8APDU[0])) 
    {
        /* tagged as uint32_t */
        if (pu8APDU[i32Len] == 255) 
        {
            i32Len++;
            i32Len += Decode_Unsigned32(&pu8APDU[i32Len], &uValue32);
            if (pu32Value) 
            {
                *pu32Value = uValue32;
            }
        }
        /* tagged as uint16_t */
        else if (pu8APDU[i32Len] == 254) 
        {
            i32Len++;
            i32Len += Decode_Unsigned16(&pu8APDU[i32Len], &uValue16);
            if (pu32Value) 
            {
                *pu32Value = uValue16;
            }
        }
        /* no tag - must be uint8_t */
        else 
        {
            if (pu32Value) 
            {
                *pu32Value = pu8APDU[i32Len];
            }
            i32Len++;
        }
    } 
    else if (IS_OPENING_TAG(pu8APDU[0]) && pu32Value) 
    {
        *pu32Value = 0;
    } 
    else if (IS_CLOSING_TAG(pu8APDU[0]) && pu32Value) 
    {
        /* closing tag */
        *pu32Value = 0;
    } 
    else if (pu32Value) 
    {
        /* small value */
        *pu32Value = pu8APDU[0] & 0x07;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number_And_Value: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes the BACnet Tag Number and its Value.Same as function above, 
*   but will safely fail is packet has been truncate  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32APDU_len_remaining [in]Length of remaining APDU segment.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pu32Value [out] Pointer to the decoded value.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number_And_Value_Safe(uint8_t *pu8APDU, 
        uint32_t u32APDU_len_remaining, uint8_t *pu8Tag_Number, uint32_t *pu32Value)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number_And_Value: entry \r\n");
	#endif

    i32Len = Decode_Tag_Number_Safe(&pu8APDU[0], u32APDU_len_remaining, pu8Tag_Number);

    if (i32Len > 0) 
    {
        u32APDU_len_remaining -= i32Len;
        if (IS_EXTENDED_VALUE(pu8APDU[0])) 
        {
            /* tagged as uint32_t */
            if (pu8APDU[i32Len] == 255 && u32APDU_len_remaining >= 5) 
            {
                uint32_t uValue32;
                i32Len++;
                i32Len += Decode_Unsigned32(&pu8APDU[i32Len], &uValue32);
                if (pu32Value) 
                {
                    *pu32Value = uValue32;
                }
            }
            /* tagged as uint16_t */
            else if (pu8APDU[i32Len] == 254 && u32APDU_len_remaining >= 3) 
            {
                uint16_t uValue16;
                i32Len++;
                i32Len += Decode_Unsigned16(&pu8APDU[i32Len], &uValue16);
                if (pu32Value) {
                    *pu32Value = uValue16;
                }
            }
            /* no tag - must be uint8_t */
            else if (pu8APDU[i32Len] < 254 && u32APDU_len_remaining >= 1) 
            {
                if (pu32Value) 
                {
                    *pu32Value = pu8APDU[i32Len];
                }
                i32Len++;
            } 
            else 
            {
                /* packet is truncated */
                i32Len = 0;
            }
        } 
        else if (IS_OPENING_TAG(pu8APDU[0]) && pu32Value) 
        {
            *pu32Value = 0;
        } 
        else if (IS_CLOSING_TAG(pu8APDU[0]) && pu32Value) 
        {
            /* closing tag */
            *pu32Value = 0;
        } 
        else if (pu32Value) 
        {
            /* small value */
            *pu32Value = pu8APDU[0] & 0x07;
        }
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Tag_Number_And_Value: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Context Specific tag & Tag Namuber. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Context_Tag(uint8_t *pu8APDU, uint8_t u8Tag_Number)
{
    uint8_t u8My_Tag_Number = 0;
    int8_t i8Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Context_Tag: entry \r\n");
	#endif

    i8Len = (int8_t)Decode_Tag_Number(pu8APDU, &u8My_Tag_Number);
    if(IS_CONTEXT_SPECIFIC(*pu8APDU) &&
        (u8My_Tag_Number == u8Tag_Number))
    {
		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Context_Tag: exit \r\n");
		#endif
        return i8Len;
    }
	
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Decode_Is_Context_Tag: Tag is not Context specific \r\n");
	#endif
    return (0);
}


/** 
*   Decodes Context Specific tag & Tag Namuber. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pi32Tag_length [out] The number of bytes consumed.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Context_Tag_With_Length(uint8_t *pu8APDU, uint8_t u8Tag_Number,
        int32_t *pi32Tag_length)
{
    uint8_t u8My_Tag_Number = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Context_Tag_With_Length: entry \r\n");
	#endif

    *pi32Tag_length = Decode_Tag_Number(pu8APDU, &u8My_Tag_Number);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Context_Tag_With_Length: exit \r\n");
	#endif

    return (bool) (IS_CONTEXT_SPECIFIC(*pu8APDU) &&
        (u8My_Tag_Number == u8Tag_Number));
}

/** 
*   Decodes Opening tag. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Opening_Tag_Number(uint8_t *pu8APDU, uint8_t u8Tag_Number)
{
    uint8_t u8My_Tag_Number = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Opening_Tag_Number: entry \r\n");
	#endif

    Decode_Tag_Number(pu8APDU, &u8My_Tag_Number);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Opening_Tag_Number: exit \r\n");
	#endif
    return (bool) (IS_CONTEXT_SPECIFIC(pu8APDU[0]) && 
                   IS_OPENING_TAG(pu8APDU[0]) && 
                   (u8My_Tag_Number == u8Tag_Number));
}

/** 
*   Decodes Closing Tag. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Closing_Tag_Number(uint8_t *pu8APDU, uint8_t u8Tag_Number)
{
    uint8_t u8My_Tag_Number = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Closing_Tag_Number: entry \r\n");
	#endif

    Decode_Tag_Number(pu8APDU, &u8My_Tag_Number);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Is_Closing_Tag_Number: exit \r\n");
	#endif
	
    return (bool) (IS_CONTEXT_SPECIFIC(pu8APDU[0]) && 
                   IS_CLOSING_TAG(pu8APDU[0]) && 
                   (u8My_Tag_Number == u8Tag_Number));
}


bool Decode_Context_Tag(uint8_t *pu8APDUReq, uint8_t u8TagNumber, uint32_t* 
                        pu32LenValType)
{
    uint8_t u8TmpTagNumber = 0;
	
	/*function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Tag: entry \r\n");
	#endif
    
    Decode_Tag_Number_And_Value( pu8APDUReq, &u8TmpTagNumber, pu32LenValType);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Tag: exit \r\n");
	#endif
    
    return (bool) ( IS_CONTEXT_SPECIFIC(*pu8APDUReq) &&
                    (u8TmpTagNumber == u8TagNumber) );
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Boolean Datatype. 
*   Following are found from clause 20.2.3 Encoding of a Boolean Value
*   and 20.2.1 General Rules for Encoding BACnet Tags
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param bBoolean_Value [in] Denotes Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Application_Boolean(uint8_t *pu8APDU, bool bBoolean_Value)
{
    int32_t i32Len = 0;
    uint32_t u32Len_Value = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Boolean: entry \r\n");
	#endif

    if (bBoolean_Value) 
    {
        u32Len_Value = 1;
    }
    i32Len = Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_BOOLEAN, false, u32Len_Value);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Boolean: exit \r\n");
	#endif

    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific with Tag Number 
*   feild as Boolean Datatype.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param bBoolean_Value [in] Denotes Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Context_Boolean(uint8_t *pu8APDU, uint8_t u8Tag_Number,
                               bool bBoolean_Value)
{
    int32_t i32Len = 0;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Boolean: entry \r\n");
	#endif

    i32Len = Encode_Tag(&pu8APDU[0], (uint8_t) u8Tag_Number, true, 1);
    pu8APDU[i32Len] = (bool) (bBoolean_Value ? 1 : 0);
    i32Len++;
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Boolean: exit \r\n");
	#endif

    return i32Len;
}

/** 
*   Decodes Boolean from context specific data
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @return Returns TRUE/FALSE.
*/
bool Decode_Context_Boolean(uint8_t *pu8APDU)
{
    bool bBoolean_Value = false;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Boolean: entry \r\n");
	#endif

    if (pu8APDU[0]) 
    {
        bBoolean_Value = true;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Boolean: exit \r\n");
	#endif

    return bBoolean_Value;
}

/** 
*   Decodes Boolean from context specific data
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param bBoolean_Value [out] Pointer to the out
*   @return Returns TRUE/FALSE.
*/
uint32_t Decode_Context_Boolean_Value(uint8_t *pu8APDU, bool *bBoolean_Value)
{
    /* Since length of bool is 1 */
    uint32_t i32Len = 1;
    *bBoolean_Value = false;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Boolean_Value entry \r\n");
	#endif
    
    if (pu8APDU[0]) 
    {
        *bBoolean_Value = true;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Boolean_Value exit \r\n");
	#endif

    return i32Len;
}

/**
*   Decodes Boolean from context specific data
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet tag.
*   @param bBoolean_Value [out] Pointer to Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Context_BOOLEAN(uint8_t *pu8APDU, uint8_t u8Tag_Number,
                            bool *bBoolean_Value)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_BOOLEAN: entry \r\n");
	#endif
	
    if (Decode_Is_Context_Tag_With_Length(&pu8APDU[i32Len], u8Tag_Number, &i32Len)) 
    {
        if (pu8APDU[i32Len]) 
        {
            *bBoolean_Value = true;
        } 
        else 
        {
            *bBoolean_Value = false;
        }
        i32Len++;
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_BOOLEAN: exit \r\n");
	#endif
    return i32Len;
}

/* from clause 20.2.3 Encoding of a Boolean Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */

/**
*   Decodes Booean Value from BACnet frame.
*   @param u32Len_Value [in] The Length/Value/Type feild in BACnet tag.
*   @return Returns TRUE/FALSE.
*/
bool Decode_Boolean(uint32_t u32Len_Value)
{
    bool bBoolean_Value = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Boolean: entry \r\n");
	#endif

    if (u32Len_Value) 
    {
        bBoolean_Value = true;
    }

	/* function exit*/
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Boolean: exit \r\n");
	#endif

    return bBoolean_Value;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as NULL. 
*   Following are found from clause 20.2.2 Encoding of a Null Value
*   and 20.2.1 General Rules for Encoding BACnet Tags
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param bBoolean_Value [in] Denotes Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Application_Null(uint8_t *pu8APDU)
{
	/* function entry - exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Null: entry - exit \r\n");
	#endif
    return Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_NULL, false, 0);
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Null.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Null(uint8_t *pu8APDU, uint8_t u8Tag_Number)
{
	/* function entry - exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Null: entry - exit \r\n");
	#endif
    return Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 0);
}

/** 
*   Decodes Bitstrings of context specific data.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_BITSTRING(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBitStr_t *pstBitString)
{
    uint32_t u32Len_Value;
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_BITSTRING: entry \r\n");
	#endif

    if (NULL != pstBitString && Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number))
    {
        /* get tag no & length */
        i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len], 
            &u8Tag_Number, &u32Len_Value);
        /* save the values */
        pstBitString->m_u8ByteCnt = (uint8_t)(u32Len_Value -1);
        pstBitString->m_u8UnusedBits = pu8APDU[i32Len++];
        Memcopy(pstBitString->m_u8TransBits, &pu8APDU[i32Len], 0, (u32Len_Value-1), MIN_BITSTRING_BYTES);
		i32Len += ((u32Len_Value-1) > MIN_BITSTRING_BYTES ? MIN_BITSTRING_BYTES : (u32Len_Value-1));
        //memcpy(pstBitString->m_u8TransBits, (pu8APDU), (u32Len_Value-1));
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_BITSTRING: exit \r\n");
	#endif
    return i32Len; 
} 

/** 
*   Decodes Bitstrings of context specific data.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBITStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_BITstring(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBITStr_t *pstBitString)
{
    uint32_t u32Len_Value;
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_BITstring: entry \r\n");
	#endif

    if (NULL != pstBitString && Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number))
    {
        /* get tag no & length */
        i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len], 
            &u8Tag_Number, &u32Len_Value);
        /* save the values */
        pstBitString->m_u8ByteCnt = (uint8_t)(u32Len_Value -1);
        pstBitString->m_u8UnusedBits = pu8APDU[i32Len++];
        Memcopy(pstBitString->m_u8TransBits, (pu8APDU), 0, (u32Len_Value-1), MAX_BITSTRING_BYTES);
		i32Len += ((u32Len_Value-1) > MAX_BITSTRING_BYTES ? MAX_BITSTRING_BYTES : (u32Len_Value-1));
        //memcpy(pstBitString->m_u8TransBits, (pu8APDU), (u32Len_Value-1));
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_BITstring: exit \r\n");
	#endif
    return i32Len; 
}

/** 
*   Encodes Bitstrings in BACnet frame.It can be traced from clause 20.2.10 
*   Encoding of a Bit String Value in BACnet standard.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_BIT_STRING [in] Pointer to BACNET_BIT_STRING structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bitstring(uint8_t *pu8APDU, BACNET_BIT_STRING *pstBACNET_BIT_STRING)
{
    int32_t i32Len = 0;
    uint8_t u8Remaining_Used_Bits = 0;
    uint8_t u8Used_Bytes = 0;
    uint8_t u8I = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bitstring: entry \r\n");
	#endif

    /* if the bit string is empty, then the first octet shall be zero */
    if (BitString_Bits_Used(pstBACNET_BIT_STRING) == 0) 
    {
        pu8APDU[i32Len++] = 0;
    } 
    else 
    {
        u8Used_Bytes = BitString_Bytes_Used(pstBACNET_BIT_STRING);
        u8Remaining_Used_Bits = (uint8_t)(BitString_Bits_Used(pstBACNET_BIT_STRING)
                              - ((u8Used_Bytes - 1) * 8));
        /* number of unused bits in the subsequent final octet */
        pu8APDU[i32Len++] = (uint8_t)(8 - u8Remaining_Used_Bits);
        for (u8I = 0; u8I < u8Used_Bytes; u8I++) 
        {
            pu8APDU[i32Len++] = 
                Byte_Reverse_Bits(BitString_Octet(pstBACNET_BIT_STRING, u8I));
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bitstring: exit \r\n");
	#endif
    return i32Len;
}


/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as BitString.Following are found from clause 20.2.10 Encoding of 
*   a BitString.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBitString [in] Pointer to BACnetBITStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_BITstring(uint8_t *pu8APDU, BACnetBITStr_t *pstBitString)
{
    int32_t i32Len = 0;
    uint32_t u32Bit_String_Encoded_Length = 1;     /* 1 for the bits remaining octet */
    BACNET_BIT_STRING stBACNET_BIT_STRING = {0};
    uint8_t u8Cnt = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_BITstring: entry \r\n");
	#endif

    /* set used bits */
    BitString_Set_Bits_Used(&stBACNET_BIT_STRING, pstBitString->m_u8ByteCnt,
        pstBitString->m_u8UnusedBits);

    /* copy bytes */
    for(; u8Cnt < MAX_BITSTRING_BYTES ; u8Cnt++)
        stBACNET_BIT_STRING.value[u8Cnt] = pstBitString->m_u8TransBits[u8Cnt];

    /* bit string may use more than 1 octet for the tag, so find out how many */
    u32Bit_String_Encoded_Length += BitString_Bytes_Used(&stBACNET_BIT_STRING);
    i32Len =
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_BIT_STRING, false,
        u32Bit_String_Encoded_Length);
    i32Len += Encode_Bitstring(&pu8APDU[i32Len], &stBACNET_BIT_STRING);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_BITstring: exit \r\n");
	#endif
    return i32Len;
}
/**
*
* DESCRIPTION
* Function to encode bacnet optional unsigned data type.
*
* @param pu8APDU				[out]	pointer to save encoded data.
* @param pstOptionalUnsigned	[in]	pointer of data to be encoded.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_OptionalUnsigned(
	uint8_t *pu8Apdu,
	BACnetOptionalUnsigned_t *pstOptionalUnsigned)
{
	/* local variables */
	int32_t i32Len = 0;
//	uint8_t u8Index = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_OptionalUnsigned: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstOptionalUnsigned)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	switch(pstOptionalUnsigned->m_eDataType)
	{
	/* encode null value */
	case BACNET_DT_NULL:
	{
		pu8Apdu[i32Len] = 0;
        i32Len++;
	}
	break;

	/* encode unsigned value */
	case BACNET_DT_UNSIGNED:
	case BACNET_DT_UNSIGNED8:
	case BACNET_DT_UNSIGNED16:
	case BACNET_DT_UNSIGNED32:
	{
		i32Len = Encode_Application_Unsigned(&pu8Apdu[i32Len],
			pstOptionalUnsigned->m_u32Val);
	}
	break;

	/* default case */
	default:
		break;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_OptionalUnsigned: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}
/**  
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as BitString.Following are found from clause 20.2.10 Encoding of 
*   a BitString.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBitString [in] Pointer to BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Bitstring_small(uint8_t *pu8APDU, BACnetBitStr_t *pstBitString)
{
    int32_t i32Len = 0;
    uint32_t u32Bit_String_Encoded_Length = 1;     /* 1 for the bits remaining octet */
    BACNET_BIT_STRING stBACNET_BIT_STRING = {0};
    uint8_t u8Cnt = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Bitstring_small: entry \r\n");
	#endif

    /* set used bits */
    BitString_Set_Bits_Used(&stBACNET_BIT_STRING, pstBitString->m_u8ByteCnt,
        pstBitString->m_u8UnusedBits);

    /* copy bytes */
    for(; u8Cnt < MIN_BITSTRING_BYTES ; u8Cnt++)
        stBACNET_BIT_STRING.value[u8Cnt] = pstBitString->m_u8TransBits[u8Cnt];

    /* bit string may use more than 1 octet for the tag, so find out how many */
    u32Bit_String_Encoded_Length += BitString_Bytes_Used(&stBACNET_BIT_STRING);
    i32Len =
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_BIT_STRING, false,
        u32Bit_String_Encoded_Length);
    i32Len += Encode_Bitstring(&pu8APDU[i32Len], &stBACNET_BIT_STRING);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Bitstring_small: exit \r\n");
	#endif
    return i32Len;
}

/** 
*	encodes Pr_BACnetBitStr_t type context bitstrings.
*
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_BITSTRING(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBitStr_t *pstBitString)
{
    int32_t i32Len = 0;
    uint32_t u32Bit_String_Encoded_Length = 1;     /* 1 for the bits remaining octet */
	BACNET_BIT_STRING stBACNET_BIT_STRING = {0};
	int8_t u8ByteCnt = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_BITSTRING: entry \r\n");
	#endif

	/* convert Pr_BACnetBitStr_t to BACNET_BIT_STRING */
	BitString_Set_Bits_Used(&stBACNET_BIT_STRING, 
						pstBitString->m_u8ByteCnt, 
						pstBitString->m_u8UnusedBits);

    /* Copy Property Value(bytes) in structure for Encoding  */
    for(u8ByteCnt = 0; u8ByteCnt < MIN_BITSTRING_BYTES; u8ByteCnt++)
        stBACNET_BIT_STRING.value[u8ByteCnt] = 
				pstBitString->m_u8TransBits[u8ByteCnt];

    /* bit string may use more than 1 octet for the tag, so find out how many */
    u32Bit_String_Encoded_Length += BitString_Bytes_Used(&stBACNET_BIT_STRING);
    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, u32Bit_String_Encoded_Length);
    i32Len += Encode_Bitstring(&pu8APDU[i32Len], &stBACNET_BIT_STRING);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_BITSTRING: exit \r\n");
	#endif
    return i32Len;
}

/** 
*	encodes BACnetBITStr_t type context bitstrings.
*
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to Pr_BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_BITString(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBITStr_t *pstBitString)
{
    int32_t i32Len = 0;
    uint32_t u32Bit_String_Encoded_Length = 1;     /* 1 for the bits remaining octet */
	BACNET_BIT_STRING stBACNET_BIT_STRING = {0};
	int8_t u8ByteCnt = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_BITString: entry \r\n");
	#endif
	/* convert Pr_BACnetBitStr_t to BACNET_BIT_STRING */
	BitString_Set_Bits_Used(&stBACNET_BIT_STRING, 
						pstBitString->m_u8ByteCnt, 
						pstBitString->m_u8UnusedBits);

    /* Copy Property Value(bytes) in structure for Encoding  */
    for(u8ByteCnt = 0; u8ByteCnt < MAX_BITSTRING_BYTES; u8ByteCnt++)
        stBACNET_BIT_STRING.value[u8ByteCnt] = 
				pstBitString->m_u8TransBits[u8ByteCnt];

    /* bit string may use more than 1 octet for the tag, so find out how many */
    u32Bit_String_Encoded_Length += BitString_Bytes_Used(&stBACNET_BIT_STRING);
    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, u32Bit_String_Encoded_Length);
    i32Len += Encode_Bitstring(&pu8APDU[i32Len], &stBACNET_BIT_STRING);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_BITString: exit \r\n");
	#endif
    return i32Len;
}

/** 
*	encodes objects supported & service supported type context bitstrings.
*
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uValue  [in] Bytes to be encoded.
*   @param u8Tag_Number [in] Context Tag Number to be encoded.
*   @param iUnusedBits  [in] Unused bits in last byte.
*   @param i8NumofBytes [in] Total no of bytes to encode.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Byte(uint8_t *pu8APDU, uint8_t *uValue, uint8_t u8Tag_Number,
            uint8_t iUnusedBits, uint8_t i8NumofBytes)
{
	/* local variables */
    int32_t i32Len = 0; /* return value */
    int8_t i8Count = 0;
    uint8_t u8Byte = 0;
    uint32_t u32RevData = 0;

    uint8_t u8BytCnt = 0;
    uint8_t u8TmpBuf[MAX_APDU_LENGTH_ACCEPTED] = {0};
	//	  uint8_t u8TmpBuf[500] = {0};
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Byte: entry \r\n");
	#endif

    /* Encode Unused bit number */
    u8BytCnt = (uint8_t)Encode_Bacnet_Enumerated(&u8TmpBuf[u8BytCnt], iUnusedBits);

    /* Encode Bytes with reversed bits */
    for( i8Count =0; i8Count < i8NumofBytes; i8Count++)
    {
        u8Byte = *(uValue+i8Count);
        u32RevData = Byte_Reverse_Bits(u8Byte);
        u8BytCnt += (uint8_t)Encode_Bacnet_Enumerated(&u8TmpBuf[u8BytCnt], u32RevData);
    }

    /* Encode Application Tag */
    i32Len += Encode_Tag(&pu8APDU[0], u8Tag_Number, true, u8BytCnt);

    /* Copy Unused bits and Bit String */
    for(i8Count =0; i8Count < u8BytCnt; i8Count++)
        pu8APDU[i32Len++] = u8TmpBuf[i8Count];

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Byte: exit\r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Object Identifier in the BACnet frame.Following are found 
*   from clause 20.2.14 Encoding of an Object Identifier Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u16Object_Type [out] Pointer to the BACnet Object type.
*   @param u32Instance [out] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Object_Id(uint8_t *pu8APDU, uint32_t *pu32Object_Type,
            uint32_t *u32Instance)
{
    uint32_t u32Value = 0;
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Object_Id: entry \r\n");
	#endif

    i32Len = Decode_Unsigned32(pu8APDU, &u32Value);
    *pu32Object_Type =
        (((u32Value >> BACNET_INSTANCE_BITS) & BACNET_MAX_OBJECT));
    *u32Instance = (u32Value & BACNET_MAX_INSTANCE);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Object_Id: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Object Identifier in the BACnet frame. Same as function above,
*   but will safely fail is packet has been truncated. Following are found
*   from clause 20.2.14 Encoding of an Object Identifier Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [in] Length of the remaining data.
*   @param uObject_Type [in] Pointer to the BACnet Object type.
*   @param u32Instance [in] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Object_Id_Safe(uint8_t *pu8APDU, uint32_t u32Len_Value, 
            uint32_t *uObject_Type, uint32_t *u32Instance)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Object_Id_Safe: entry \r\n");
	#endif
    if (u32Len_Value != 4) 
	{
		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Object_Id_Safe: exit \r\n");
		#endif
        return 0;
    } 
	else 
	{
		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Object_Id_Safe: exit \r\n");
		#endif
        return Decode_Object_Id(pu8APDU, uObject_Type, u32Instance);
    }
}

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Object Identifier.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param uObject_Type [in] Pointer to the BACnet Object type.
*   @param u32Instance [in] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Object_Id(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            uint32_t *u32Object_Type, uint32_t *u32Instance)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Object_Id: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag_With_Length(&pu8APDU[i32Len], u8Tag_Number, &i32Len)) {
        i32Len += Decode_Object_Id(&pu8APDU[i32Len], u32Object_Type, u32Instance);
    } else {
        i32Len = BACNET_STATUS_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Object_Id: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes Object Identifier in the BACnet frame.Following are found 
*   from clause 20.2.14 Encoding of an Object Identifier Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uObject_Type [in] BACnet Object type.
*   @param u32Instance [in] BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Object_Id(uint8_t *pu8APDU, int32_t iObject_Type,
            uint32_t u32Instance)
{
    uint32_t u32Value = 0;
    uint32_t uType = 0;
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Object_Id: entry \r\n");
	#endif

    uType = (uint32_t) iObject_Type;
    u32Value =
        ((uType & BACNET_MAX_OBJECT) << BACNET_INSTANCE_BITS) | (u32Instance &
        BACNET_MAX_INSTANCE);
    i32Len = Encode_Unsigned32(pu8APDU, u32Value);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Object_Id: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Object Identifier.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param uObject_Type [in] Pointer to the BACnet Object type.
*   @param u32Instance [in] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Object_Id(uint8_t *pu8APDU, uint8_t u8Tag_Number, 
            int32_t iObject_Type, uint32_t u32Instance)
{
    int32_t i32Len = 0;
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Object_Id: entry \r\n");
	#endif

    /* length of object id is 4 octets, as per 20.2.14 */

    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 4);
    i32Len += Encode_Bacnet_Object_Id(&pu8APDU[i32Len], iObject_Type, u32Instance);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Object_Id: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Object Identifier.Following are found from clause 20.2.14
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uObject_Type [in] BACnet Object type.
*   @param u32Instance [in] BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Object_Id(uint8_t *pu8APDU, int32_t iObject_Type,
            uint32_t u32Instance)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Object_Id: entry \r\n");
	#endif

    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Object_Id(&pu8APDU[1], iObject_Type, u32Instance);
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_OBJECT_ID, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Object_Id: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes Octet String to BACnet frame.Following are found from clause 20.2.8
*   Encoding of an Octet String Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Octet_String(uint8_t *pu8APDU, 
            BACnetOctetStr_t *pBACNET_OCTET_STRING)
{
    int32_t i32Len = 0;        /* return value */
    uint8_t *uValue = NULL;
    int32_t uI = 0;  /* loop counter */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Octet_String: entry \r\n");
	#endif

    if (pBACNET_OCTET_STRING) {
        /* FIXME: might need to pass in the length of the APDU
           to bounds check since it might not be the only data chunk */
        i32Len = (int32_t) OctetString_Length(pBACNET_OCTET_STRING);
        uValue = OctetString_Value(pBACNET_OCTET_STRING);
        for(uI = 0; uI < i32Len; uI++) {
            pu8APDU[uI] = uValue[uI];
        }
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Octet_String: exit \r\n");
	#endif

    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Octet String.Following are found from clause 20.2.8
*   Encoding of an Octet String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Octet_String(uint8_t *pu8APDU, 
            BACnetOctetStr_t *pBACNET_OCTET_STRING)
{
    int32_t iAPDU_Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Octet_String: entry \r\n");
	#endif

    if (pBACNET_OCTET_STRING) 
    {
        iAPDU_Len =
            Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_OCTET_STRING, false,
            OctetString_Length(pBACNET_OCTET_STRING));
        /* FIXME: probably need to pass in the length of the APDU
           to bounds check since it might not be the only data chunk */
        if ((iAPDU_Len + OctetString_Length(pBACNET_OCTET_STRING)) < MAX_APDU_LENGTH_ACCEPTED) 
        {
            iAPDU_Len += Encode_Octet_String(&pu8APDU[iAPDU_Len], pBACNET_OCTET_STRING);
        } 
        else 
        {
            iAPDU_Len = 0;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Octet_String: exit \r\n");
	#endif
    return iAPDU_Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Octet String.Following are found from clause 20.2.8
*   Encoding of an Octet String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Octet_String(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetOctetStr_t *pBACNET_OCTET_STRING)
{
    int32_t iAPDU_Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Octet_String: entry \r\n");
	#endif

    if (pu8APDU && pBACNET_OCTET_STRING) 
    {
        iAPDU_Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 
                            OctetString_Length(pBACNET_OCTET_STRING));
        if ((iAPDU_Len + OctetString_Length(pBACNET_OCTET_STRING)) < MAX_APDU_LENGTH_ACCEPTED) 
        {
            iAPDU_Len += Encode_Octet_String(&pu8APDU[iAPDU_Len], pBACNET_OCTET_STRING);
        } 
        else 
        {
            iAPDU_Len = 0;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Octet_String: exit \r\n");
	#endif
    return iAPDU_Len;
}

/** 
*   Decodes Octet String in the BACnet frame.Following are found 
*   from clause 20.2.8 Encoding of an Octet String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Length of the Octet string.
*   @param pstOctetString [out] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_OCTET_String(uint8_t *pu8APDU, uint32_t u32Len_Value,
            BACnetOctetStr_t *pstOctetString)
{
    /* local variables */
    int32_t i32Len = 0;        /* return value */
    bool bStatus = false;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Decode_OCTET_String: entry \r\n");
	#endif

    /* check input pointer */
    if(NULL != pstOctetString)
    {
        /* decode octet string */
        bStatus = OctetString_Init(pstOctetString, &pu8APDU[0], u32Len_Value);
        if(bStatus) 
		{
            i32Len = (int32_t) u32Len_Value;
		}
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Decode_OCTET_String: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Octet String.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Octet_String(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetOctetStr_t *pBACNET_OCTET_STRING)
{
    int32_t i32Len = 0;        /* return value */
    bool bStatus = false;
    uint32_t u32Len_Value = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Octet_String: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number)) 
    {
        i32Len +=
            Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8Tag_Number, &u32Len_Value);

        bStatus = OctetString_Init(pBACNET_OCTET_STRING, &pu8APDU[i32Len], u32Len_Value);

        if (bStatus) 
        {
            i32Len += u32Len_Value;
        }
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Octet_String: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes Character String to BACnet frame.Following are found from 
*   clause 20.2.9 Encoding of an Character String Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Character_String(uint8_t *pu8APDU,
                                       BACnetCharStr_t *pstCharString)
{
    /* local varaibles */
    int32_t i32Len = 0; 
    int32_t iI = 0;
    int8_t *pcString = NULL;
    int32_t i32StringLen = 0;

    /* function entry debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Character_String: entry \r\n");
	#endif

    i32StringLen = (int32_t)CharacterString_Length(pstCharString);
   
    /* encode type of character encoding */
    pu8APDU[i32Len++] = (uint8_t)g_eCharEncoding;

    /* encode code page if IBM_MS_DBCS type encoding */
    if(CHARACTER_IBM_MS_DBCS == g_eCharEncoding)
    {
        pu8APDU[i32Len++] = (uint8_t)CODE_PAGE_MSB;
        pu8APDU[i32Len++] = (uint8_t)CODE_PAGE_LSB;
    }

    /* encode string value */
    pcString = CharacterString_Value(pstCharString);
    for (iI = 0; iI < i32StringLen; iI++)
    {
        pu8APDU[i32Len++] = (uint8_t)pcString[iI];
    }

    /* function exit debug message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Character_String: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Character String.Following are found from clause 20.2.9
*   Encoding of an Character String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Character_String(uint8_t *pu8APDU,
            BACnetCharStr_t *pstCharString)
{
    int32_t i32Len = 0;
    int32_t iString_Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Character_String: entry \r\n");
	#endif

    /* calculate character string length */
    iString_Len =
        (int32_t) CharacterString_Length(pstCharString) + 1 /* for encoding */ ;
    /* add 2bytes in string length (for code page) if IBM_MS_DBCS type encoding */
    if(CHARACTER_IBM_MS_DBCS == g_eCharEncoding)
        iString_Len += 2;
    /* encode tag */
    i32Len =
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_CHARACTER_STRING, false,
        (uint32_t) iString_Len);
    /* encode string */
    if ((i32Len + iString_Len) < MAX_APDU_LENGTH_ACCEPTED) 
    {
        i32Len += Encode_Bacnet_Character_String(&pu8APDU[i32Len], pstCharString);
    } 
    else 
    {
        i32Len = 0;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Character_String: exit \r\n");
	#endif
    return i32Len;
}


/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Character String.Following are found from clause 20.2.15
*   Encoding of an Character String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Character_String(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetCharStr_t *pstCharString)
{
    int32_t i32Len = 0;
    int32_t iString_Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Character_String: entry \r\n");
	#endif

    /* calculate character string length */
    iString_Len =
        (int32_t) CharacterString_Length(pstCharString) + 1 /* for encoding */ ;
    /* add 2bytes in string length (for code page) if IBM_MS_DBCS type encoding */
    if(CHARACTER_IBM_MS_DBCS == g_eCharEncoding)
        iString_Len += 2;
    /* encode tag value */
    i32Len += Encode_Tag(&pu8APDU[0], u8Tag_Number, true, (uint32_t) iString_Len);
    /* encode string */
    if ((i32Len + iString_Len) < MAX_APDU_LENGTH_ACCEPTED) 
    {
        i32Len += Encode_Bacnet_Character_String(&pu8APDU[i32Len], pstCharString);
    } 
    else 
    {
        i32Len = 0;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Character_String: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Character String from the BACnet frame.Following are found 
*   from clause 20.2.9 Encoding of an Character String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Length of the Character string.
*   @param pBACNET_CHARACTER_STRING [out] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Character_String(uint8_t *pu8APDU, uint32_t u32Len_Value,
            BACnetCharStr_t *pstCharString)
{
    int32_t i32Len = 0;        /* return value */
    bool bStatus = false;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Character_String: entry \r\n");
	#endif

    bStatus =
        CharacterString_Init(pstCharString, pu8APDU[0], (int8_t *) &pu8APDU[1],
        u32Len_Value - 1);
    if (bStatus) 
    {
        i32Len = (int32_t) u32Len_Value;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Character_String: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Character String.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Character_String(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetCharStr_t *pstCharString)
{
    int32_t i32Len = 0;        /* return value */
    bool bStatus = false;
    uint32_t u32Len_Value = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Character_String: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number)) 
    {
        i32Len +=
            Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8Tag_Number, &u32Len_Value);

        bStatus =
            CharacterString_Init(pstCharString, pu8APDU[i32Len],
            (int8_t *) &pu8APDU[i32Len + 1], u32Len_Value - 1);
        if (bStatus) 
        {
            i32Len += u32Len_Value;
        }
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Character_String: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Unsigned Integer from the BACnet frame.Following are found 
*   from clause 20.2.4 Encoding of an Unsigned Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param uValue [out] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Unsigned(uint8_t *pu8APDU, uint32_t u32Len_Value, uint32_t *uValue)
{
    uint16_t uValue16 = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Unsigned: entry \r\n");
	#endif

    if (uValue) 
    {
        switch (u32Len_Value) 
        {
            case 1:
                *uValue = pu8APDU[0];
                break;
            case 2:
                Decode_Unsigned16(&pu8APDU[0], &uValue16);
                *uValue = uValue16;
                break;
            case 3:
                Decode_Unsigned24(&pu8APDU[0], uValue);
                break;
            case 4:
                Decode_Unsigned32(&pu8APDU[0], uValue);
                break;
            default:
                *uValue = 0;
                break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Unsigned: exit \r\n");
	#endif
    return (int32_t) u32Len_Value;
}


/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Unsigned(uint8_t *pu8APDU, uint8_t u8Tag_Number, 
            uint32_t *uValue)
{
    uint32_t u32Len_Value = 0;
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Unsigned: entry \r\n");
	#endif
    if (Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number)) 
    {
        i32Len +=
            Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8Tag_Number, &u32Len_Value);
        i32Len += Decode_Unsigned(&pu8APDU[i32Len], u32Len_Value, uValue);
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Unsigned: exit \r\n");
	#endif
    return i32Len;
}


/** 
*   Encodes Unsigned Integer to BACnet frame.Following are found from 
*   clause 20.2.4 Encoding of an Unsigned Integer Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be Encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Unsigned(uint8_t *pu8APDU, uint32_t uValue)
{
    int32_t i32Len = 0;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Unsigned: entry \r\n");
	#endif

    if (uValue < 0x100) 
    {
        pu8APDU[0] = (uint8_t) uValue;
        i32Len = 1;
    } 
    else if (uValue < 0x10000) 
    {
        i32Len = Encode_Unsigned16(&pu8APDU[0], (uint16_t) uValue);
    } 
    else if (uValue < 0x1000000) 
    {
        i32Len = Encode_Unsigned24(&pu8APDU[0], uValue);
    } 
    else 
    {
        i32Len = Encode_Unsigned32(&pu8APDU[0], uValue);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Unsigned: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.4
*   Encoding of an Unsigned Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Unsigned(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            uint32_t uValue)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Unsigned: entry \r\n");
	#endif

    /* length of uint32_t is variable, as per 20.2.4 */
    if (uValue < 0x100) 
    {
        i32Len = 1;
    } 
    else if (uValue < 0x10000) 
    {
        i32Len = 2;
    } 
    else if (uValue < 0x1000000) 
    {
        i32Len = 3;
    } 
    else 
    {
        i32Len = 4;
    }

    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, (uint32_t) i32Len);
    i32Len += Encode_Bacnet_Unsigned(&pu8APDU[i32Len], uValue);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Unsigned: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.4
*   Encoding of an Unsigned Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Unsigned(uint8_t *pu8APDU, uint32_t uValue)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Unsigned: entry \r\n");
	#endif

    i32Len = Encode_Bacnet_Unsigned(&pu8APDU[1], uValue);
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_UNSIGNED_INT, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Unsigned: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.4
*   Encoding of an Unsigned Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_StampUnsigned(uint8_t *pu8APDU, uint32_t uValue)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_StampUnsigned: entry \r\n");
	#endif

    i32Len = Encode_Bacnet_Unsigned(&pu8APDU[1], uValue); 

    /* Tag must have context class specific */ 
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_BOOLEAN, true,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_StampUnsigned: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Enumerated from the BACnet frame.Following are found 
*   from clause 20.2.11 Encoding of an Enumerated Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param uValue [out] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Enumerated(uint8_t *pu8APDU, uint32_t u32Len_Value, uint32_t *uValue)
{
    uint32_t uValue32 = 0;
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Enumerated: entry \r\n");
	#endif
    i32Len = Decode_Unsigned(pu8APDU, u32Len_Value, &uValue32);
    if (uValue) {
        *uValue = uValue32;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Enumerated: exit \r\n");
	#endif
    return i32Len;
}


/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Enumerated.Following are found from clause 20.2.15
*   Encoding of an Enumerated Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Enumerated(uint8_t *pu8APDU, uint8_t uTag_Value,
            uint32_t *uValue)
{
    int32_t i32Len = 0;
    uint8_t u8Tag_Number = 0;
    uint32_t u32Len_Value = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Enumerated: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag(&pu8APDU[i32Len], uTag_Value)) {
        i32Len +=
            Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8Tag_Number, &u32Len_Value);
        i32Len += Decode_Enumerated(&pu8APDU[i32Len], u32Len_Value,uValue);
    } else {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Enumerated: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes Enumerated to BACnet frame.Following are found from 
*   clause 20.2.11 Encoding of an Enumerated Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be Encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Enumerated(uint8_t *pu8APDU, uint32_t uValue)
{
	/* function entry - exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Enumerated: entry - exit \r\n");
	#endif
    return Encode_Bacnet_Unsigned(pu8APDU, uValue);
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Enumerated.Following are found from clause 20.2.11
*   Encoding of an Enumerated Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Enumerated(uint8_t *pu8APDU, uint32_t uValue)
{
    int32_t i32Len = 0;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Enumerated: entry \r\n");
	#endif

    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Enumerated(&pu8APDU[1], uValue);
    i32Len +=
                Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_ENUMERATED, 
                false, (uint32_t) i32Len);

	/* function exit*/
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Enumerated: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as BitString.Following are found from clause 20.2.10 Encoding of 
*   a BitString.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_BIT_STRING [in] Pointer to BACNET_BIT_STRING structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Byte(uint8_t *pu8APDU, uint8_t *uValue, uint8_t iUnusedBits,
                                uint8_t i8NumofBytes)
{
    int32_t i32Len = 0;        /* return value */
    int8_t i8Count = 0;
    uint8_t u8Byte = 0;
    uint32_t u32RevData = 0;

    uint8_t u8BytCnt = 0;
    uint8_t u8TmpBuf[MAX_APDU_LENGTH_ACCEPTED] = {0};
	// uint8_t u8TmpBuf[500] = {0};
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Byte: entry \r\n");
	#endif

    /* Encode Unused bit number */
    u8BytCnt = (uint8_t)Encode_Bacnet_Enumerated(&u8TmpBuf[u8BytCnt], iUnusedBits);

    /* Encode Bytes with reversed bits */
    for( i8Count =0; i8Count < i8NumofBytes; i8Count++)
    {
        u8Byte = *(uValue+i8Count);
        u32RevData = Byte_Reverse_Bits(u8Byte);
        u8BytCnt += (uint8_t)Encode_Bacnet_Enumerated(&u8TmpBuf[u8BytCnt], u32RevData);
    }

    /* Encode Application Tag */
    i32Len += Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_BIT_STRING, 
                        false, u8BytCnt);

    /* Copy Unused bits and Bit String */
    for(i8Count =0; i8Count < u8BytCnt; i8Count++)
        pu8APDU[i32Len++] = u8TmpBuf[i8Count];

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Byte: exit\r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Enumerated.Following are found from clause 20.2.11
*   Encoding of an Enumerated Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Enumerated(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            uint32_t uValue)
{
    int32_t i32Len = 0;        /* return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Enumerated: entry \r\n");
	#endif
    /* length of enumerated is variable, as per 20.2.11 */
    if (uValue < 0x100) 
    {
        i32Len = 1;
    } 
    else if (uValue < 0x10000) 
    {
        i32Len = 2;
    } 
    else if (uValue < 0x1000000) 
    {
        i32Len = 3;
    } 
    else 
    {
        i32Len = 4;
    }

    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, (uint32_t) i32Len);
    i32Len += Encode_Bacnet_Enumerated(&pu8APDU[i32Len], uValue);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Enumerated: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Signed Integer from the BACnet frame.Following are found 
*   from clause 20.2.5 Encoding of an Signed Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param iValue [out] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Signed(uint8_t *pu8APDU, uint32_t u32Len_Value, int32_t *iValue)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Signed: entry \r\n");
	#endif
    if (iValue) 
    {
        switch (u32Len_Value) 
        {
            case 1:
                Decode_Signed8(&pu8APDU[0], iValue);
                break;
            case 2:
                Decode_Signed16(&pu8APDU[0], iValue);
                break;
            case 3:
                Decode_Signed24(&pu8APDU[0], iValue);
                break;
            case 4:
                Decode_Signed32(&pu8APDU[0], iValue);
                break;
            default:
                *iValue = 0;
                break;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Signed: exit \r\n");
	#endif
    return (int32_t) u32Len_Value;
}


/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Signed Integer.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param iValue [in] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Signed(uint8_t *pu8APDU, uint8_t u8Tag_Number, int32_t *iValue)
{
    uint32_t u32Len_Value = 0;
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Signed: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number)) 
    {
        i32Len +=
            Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8Tag_Number, &u32Len_Value);
        i32Len += Decode_Signed(&pu8APDU[i32Len], u32Len_Value, iValue);
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Signed: exit\r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes Signed Integer to BACnet frame.Following are found from 
*   clause 20.2.5 Encoding of an Signed Integer Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param iValue [in] Value to be Encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Signed(uint8_t *pu8APDU, int32_t iValue)
{
    int32_t i32Len = 0;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Signed: entry \r\n");
	#endif

    /* don't encode the leading X'FF' or X'00' of the two's compliment.
       That is, the first octet of any multi-octet encoded value shall
       not be X'00' if the most significant bit (bit 7) of the second
       octet is 0, and the first octet shall not be X'FF' if the most
       significant bit of the second octet is 1. */
    if ((iValue >= -128) && (iValue < 128)) 
    {
        i32Len = Encode_Signed8(&pu8APDU[0], (int8_t) iValue);
    } 
    else if ((iValue >= -32768) && (iValue < 32768)) 
    {
        i32Len = Encode_Signed16(&pu8APDU[0], (int16_t) iValue);
    } 
    else if ((iValue > -8388608) && (iValue < 8388608)) 
    {
        i32Len = Encode_signed24(&pu8APDU[0], iValue);
    } 
    else 
    {
        i32Len = Encode_Signed32(&pu8APDU[0], iValue);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Signed: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Signed Integer.Following are found from clause 20.2.5
*   Encoding of an Signed Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param i32Value [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Signed(uint8_t *pu8APDU, int32_t i32Value)
{
    int32_t i32Len = 0;        /* return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Signed: entry \r\n");
	#endif
    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Signed(&pu8APDU[1], i32Value);
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_SIGNED_INT, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Signed: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Signed Integer.Following are found from clause 20.2.5
*   Encoding of an Signed Integer Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param i32Value [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Signed(uint8_t *pu8APDU, uint8_t u8Tag_Number, int32_t i32Value)
{
    int32_t i32Len = 0;        /* return value */
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Signed: entry \r\n");
	#endif

    /* length of signed int32_t is variable, as per 20.2.11 */
    if ((i32Value >= -128) && (i32Value < 128)) {
        i32Len = 1;
    } else if ((i32Value >= -32768) && (i32Value < 32768)) {
        i32Len = 2;
    } else if ((i32Value > -8388608) && (i32Value < 8388608)) {
        i32Len = 3;
    } else {
        i32Len = 4;
    }

    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, (uint32_t) i32Len);
    i32Len += Encode_Bacnet_Signed(&pu8APDU[i32Len], i32Value);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Signed: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Real.Following are found from clause 20.2.6
*   Encoding of an Real Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param fValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Real(uint8_t *pu8APDU, Float_t fValue)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Real: entry \r\n");
	#endif

    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Real(fValue, &pu8APDU[1]);
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_REAL, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Real: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Real.Following are found from clause 20.2.6
*   Encoding of an Real Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param fValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Real(uint8_t *pu8APDU, uint8_t u8Tag_Number, Float_t fValue)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Real: entry \r\n");
	#endif

    /* length of double is 4 octets, as per 20.2.6 */
    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 4);
    i32Len += Encode_Bacnet_Real(fValue, &pu8APDU[i32Len]);
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Real: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Double.Following are found from clause 20.2.7
*   Encoding of an Double Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param dValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Double(uint8_t *pu8APDU, Double_t dValue)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Double: entry \r\n");
	#endif

    /* assumes that the tag only consumes 2 octet */
    i32Len = Encode_Bacnet_Double(dValue, &pu8APDU[2]);

    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_DOUBLE, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Double: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Double.Following are found from clause 20.2.7
*   Encoding of an Double Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param dValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Double(uint8_t *pu8APDU, uint8_t u8Tag_Number, Double_t dValue)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Double: entry \r\n");
	#endif

    /* length of double is 8 octets, as per 20.2.7 */
    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 8);
    i32Len += Encode_Bacnet_Double(dValue, &pu8APDU[i32Len]);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Double: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes Time to BACnet frame.Following are found from 
*   clause 20.2.13 Encoding of an Time Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Time(uint8_t *pu8APDU, BACnetTime_t *pstBACNET_TIME)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Time: entry \r\n");
	#endif

    pu8APDU[0] = pstBACNET_TIME->m_u8Hour;
    pu8APDU[1] = pstBACNET_TIME->m_u8Min;
    pu8APDU[2] = pstBACNET_TIME->m_u8Sec;
    pu8APDU[3] = pstBACNET_TIME->m_u8Hundredths;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Time: exit \r\n");
	#endif
    return 4;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Time(uint8_t *pu8APDU, BACnetTime_t *pstBACNET_TIME)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Time: entry \r\n");
	#endif

    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Time(&pu8APDU[1], pstBACNET_TIME);
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_TIME, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_Time: exit\r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_StampTime(uint8_t *pu8APDU, BACnetTime_t *pstBACNET_TIME)
{
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_StampTime: entry \r\n");
	#endif

    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Time(&pu8APDU[1], pstBACNET_TIME);
    /* 0x0c is defined as 3rd bit for context specific and 
       length of time will be always 4 bytes */
    pu8APDU[0] = 0x0C;
    i32Len++;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_StampTime: exit \r\n");
	#endif
    return i32Len;
}


/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstTime [in] Pointer to BACnetTime_t structure.
*   @param pstDate [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_StampDateTime(uint8_t *pu8APDU,BACnetTime_t *pstTime,
                                                        BACnetDate_t *pstDate)
{
    int32_t i32Len, i32TimeLen = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_StampDateTime: entry \r\n");
	#endif

    /* Context Opening Tag : 0x2E is defined as 3rd bit for context specific,
       2-> indicates choice type is datetime and 
       length of time will be always 5 bytes */
    pu8APDU[0] = 0x2E;
    i32Len = Encode_Bacnet_Date(&pu8APDU[2], pstDate);
    i32Len +=
        Encode_Tag(&pu8APDU[1], BACNET_APPLICATION_TAG_DATE, false,
        (uint32_t) i32Len);

    i32TimeLen += Encode_Bacnet_Time(&pu8APDU[i32Len+2], pstTime); 
    i32Len +=
        Encode_Tag(&pu8APDU[i32Len+1], BACNET_APPLICATION_TAG_TIME, false,
        (uint32_t) i32TimeLen);
        
    i32Len += i32TimeLen;
    
    /* Closing Context tag */
    i32Len++;   
    pu8APDU[i32Len] = 0x2F;
    i32Len++;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Application_StampDateTime: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBACNET_TIME [in] Pointer Value to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Time(uint8_t *pu8APDU, uint8_t u8Tag_Number, 
            BACnetTime_t *pstBACNET_TIME)
{
    int32_t i32Len = 0;        /* return value */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Time: entry \r\n");
	#endif

    /* length of time is 4 octets, as per 20.2.13 */
    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 4);
    i32Len += Encode_Bacnet_Time(&pu8APDU[i32Len], pstBACNET_TIME);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Time: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Time from the BACnet frame.Following are found 
*   from clause 20.2.13 Encoding of a Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_TIME [out] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Time(uint8_t *pu8APDU, BACnetTime_t *pstBACNET_TIME)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time: entry \r\n");
	#endif

    pstBACNET_TIME->m_u8Hour = pu8APDU[0];
    pstBACNET_TIME->m_u8Min = pu8APDU[1];
    pstBACNET_TIME->m_u8Sec = pu8APDU[2];
    pstBACNET_TIME->m_u8Hundredths = pu8APDU[3];

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time: exit \r\n");
	#endif
    return 4;
}

/** 
*   Decodes Time from the BACnet frame.Same as function above,
*   but will safely fail is packet has been truncated.Following are found 
*   from clause 20.2.13 Encoding of a Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [in] Length of the remaining data.
*   @param pstBACNET_TIME [out] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Time_Safe(uint8_t *pu8APDU, uint32_t u32Len_Value, 
            BACnetTime_t *pstBACNET_TIME)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time_Safe: entry \r\n");
	#endif

    if (u32Len_Value != 4) 
    {
        pstBACNET_TIME->m_u8Hour = 0;
        pstBACNET_TIME->m_u8Hundredths = 0;
        pstBACNET_TIME->m_u8Min = 0;
        pstBACNET_TIME->m_u8Sec = 0;

		/*function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time_Safe: exit \r\n");
		#endif
        return (int32_t) u32Len_Value;
    } 
    else 
    {
		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Time_Safe: exit \r\n");
		#endif
        return Decode_Time(pu8APDU, pstBACNET_TIME);
    }
}


/** 
*   Decodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Application_Time(uint8_t *pu8APDU, BACnetTime_t *pstBACNET_TIME)
{
    int32_t i32Len = 0;
    uint8_t u8Tag_Number = 0;
    Decode_Tag_Number(&pu8APDU[i32Len], &u8Tag_Number);
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Application_Time: entry \r\n");
	#endif

    if (u8Tag_Number == BACNET_APPLICATION_TAG_TIME) 
    {
        i32Len++;
        i32Len += Decode_Time(&pu8APDU[i32Len], pstBACNET_TIME);
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Application_Time: exit \r\n");
	#endif
    return i32Len;
}


/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Bacnet_Time(uint8_t *pu8APDU, uint8_t u8Tag_Number, 
            BACnetTime_t *pstBACNET_TIME)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Bacnet_Time: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag_With_Length(&pu8APDU[i32Len], u8Tag_Number, &i32Len)) {
        i32Len += Decode_Time(&pu8APDU[i32Len], pstBACNET_TIME);
    } else {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Bacnet_Time: exit \r\n");
	#endif
    return i32Len;
}


/* BACnet Date */
/* year = years since 1900 */
/* month 1=Jan */
/* day = day of month */
/* wday 1=Monday...7=Sunday */

/** 
*   Encodes Date to BACnet frame.Following are found from 
*   clause 20.2.12 Encoding of an Date Value.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Date(uint8_t *pu8APDU, BACnetDate_t *pstBACNET_DATE)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Date: entry \r\n");
	#endif

	/* NOTE :	to encode year, as per bacnet standard it should take only 1 byte.
				this byte should have the value CURRENT YEAR - 1900.
				eg: say current year is 2012
				hence value = 2012-1900 = 112
				now value 112 is to be encoded i.e X'70 in hex */
	if(pstBACNET_DATE->m_u16Year >= 1900)
	{
		pu8APDU[0] = (uint8_t) (pstBACNET_DATE->m_u16Year - 1900);
		/* in case of unspecified value of year i.e. 0xFFFF send 0xFF */
		if(pstBACNET_DATE->m_u16Year == 0xFFFF)
			pu8APDU[0] = (uint8_t) 0xFF;
	}
    if(pstBACNET_DATE->m_u16Year == 0xFF)
        pu8APDU[0] = (uint8_t) 0xFF;
    pu8APDU[1] = pstBACNET_DATE->m_u8Month;
    pu8APDU[2] = pstBACNET_DATE->m_u8Day;
    pu8APDU[3] = pstBACNET_DATE->m_u8Wday;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Bacnet_Date: exit \r\n");
	#endif
    return 4;
}


/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Date(uint8_t *pu8APDU, BACnetDate_t *pstBACNET_DATE)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Application_Date: entry \r\n");
	#endif

    /* assumes that the tag only consumes 1 octet */
    i32Len = Encode_Bacnet_Date(&pu8APDU[1], pstBACNET_DATE);
    i32Len +=
        Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_DATE, false,
        (uint32_t) i32Len);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Encode_Application_Date: exit \r\n");
	#endif
    return i32Len;

}

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Date(uint8_t *pu8APDU, uint8_t u8Tag_Number, 
            BACnetDate_t *pstBACNET_DATE)
{
    int32_t i32Len = 0;        /* return value */

	/*function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Date: entry \r\n");
	#endif

    /* length of date is 4 octets, as per 20.2.12 */
    i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, 4);
    i32Len += Encode_Bacnet_Date(&pu8APDU[i32Len], pstBACNET_DATE);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Context_Date: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes Date from the BACnet frame.Following are found 
*   from clause 20.2.12 Encoding of an Date Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_DATE [out] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Date(uint8_t *pu8APDU, BACnetDate_t *pstBACNET_DATE)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Date: entry \r\n");
	#endif

    pstBACNET_DATE->m_u16Year = (uint16_t) (pu8APDU[0] + 1900);
    if(pu8APDU[0] == 0xFF)
        pstBACNET_DATE->m_u16Year = pu8APDU[0];
    pstBACNET_DATE->m_u8Month = pu8APDU[1];
    pstBACNET_DATE->m_u8Day = pu8APDU[2];
    pstBACNET_DATE->m_u8Wday = pu8APDU[3];

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Date: exit \r\n");
	#endif
    return 4;
}

/** 
*   Decodes Date from the BACnet frame.Following are found 
*   from clause 20.2.12 Encoding of an Date Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [in] Length of the remaining data.
*   @param pstBACNET_DATE [out] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Date_Safe(uint8_t *pu8APDU, uint32_t u32Len_Value, 
            BACnetDate_t *pstBACNET_DATE)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Date_Safe: entry \r\n");
	#endif
    if (u32Len_Value != 4) 
    {
        pstBACNET_DATE->m_u8Day = 0;
        pstBACNET_DATE->m_u8Month = 0;
        pstBACNET_DATE->m_u8Wday = 0;
        pstBACNET_DATE->m_u16Year = 0;

		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Date_Safe: exit \r\n");
		#endif
        return (int32_t) u32Len_Value;
    } 
    else 
    {
		/* function exit */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Date_Safe: exit \r\n");
		#endif
        return Decode_Date(pu8APDU, pstBACNET_DATE);
    }
}


/** 
*   Decodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Application_Date(uint8_t *pu8APDU, BACnetDate_t *pstBACNET_DATE)
{
    int32_t i32Len = 0;
    uint8_t u8Tag_Number = 0;
    Decode_Tag_Number(&pu8APDU[i32Len], &u8Tag_Number);
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Application_Date: entry \r\n");
	#endif

    if (u8Tag_Number == BACNET_APPLICATION_TAG_DATE) {
        i32Len++;
        i32Len += Decode_Date(&pu8APDU[i32Len], pstBACNET_DATE);
    } else {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Application_Date: exit \r\n");
	#endif
    return i32Len;
}

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Date(uint8_t *pu8APDU, uint8_t u8Tag_Number, 
            BACnetDate_t *pstBACNET_DATE)
{
    int32_t i32Len = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Date: entry \r\n");
	#endif

    if (Decode_Is_Context_Tag_With_Length(&pu8APDU[i32Len], u8Tag_Number, &i32Len)) 
    {
        i32Len += Decode_Date(&pu8APDU[i32Len], pstBACNET_DATE);
    } 
    else 
    {
        i32Len = BACNET_STATUS_ERROR;
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_Context_Date: exit \r\n");
	#endif
    return i32Len;
}



/** 
*   Encodes simple acknowledge response.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBACnet_Device_Data [in] Pointer to bacnetRequestData_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Simple_Ack(processInfo_t *pstBACnet_Device_Data, uint8_t *pu8APDU)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Simple_Ack: entry \r\n");
	#endif

    pu8APDU[0] = (int8_t)pstBACnet_Device_Data->m_stProcessData.m_ePDUType;
    pu8APDU[1] = pstBACnet_Device_Data->m_stProcessData.m_stAPDU.m_u8InvokeId;
    pu8APDU[2] = pstBACnet_Device_Data->m_stProcessData.m_stAPDU.m_u8ServiceChoice;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Simple_Ack: exit \r\n");
	#endif
    return 3;
}

/** 
*   Decodes BitString Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param pstBitString [out] Pointer to decoded value in struct BACnetBitStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_BitString_Small(uint8_t *pu8APDU, uint32_t u32Len_Value,
                        BACnetBitStr_t *pstBitString)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_BitString_Small: entry \r\n");
	#endif
	
    pstBitString->m_u8ByteCnt = (uint8_t)u32Len_Value -1;

    pstBitString->m_u8UnusedBits = *pu8APDU;

    Memcopy(pstBitString->m_u8TransBits, (pu8APDU+1), 0, (u32Len_Value-1), MIN_BITSTRING_BYTES);
    //memcpy(pstBitString->m_u8TransBits, (pu8APDU+1), (u32Len_Value-1));

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_BitString_Small: exit \r\n");
	#endif
	
    return u32Len_Value;
}

/** 
*   Decodes BitString Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param pstBitString [out] Pointer to decoded value in struct BACnetBITStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_BITstring(uint8_t *pu8APDU, uint32_t u32Len_Value,
                BACnetBITStr_t *pstBitString)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_BITstring: entry \r\n");
	#endif
	
    pstBitString->m_u8ByteCnt = (uint8_t)u32Len_Value -1;

    pstBitString->m_u8UnusedBits = *pu8APDU;

    Memcopy(pstBitString->m_u8TransBits, (pu8APDU+1), 0, (u32Len_Value-1), MAX_BITSTRING_BYTES);
    //memcpy(pstBitString->m_u8TransBits, (pu8APDU+1), (u32Len_Value-1));

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Decode_BITstring: exit \r\n");
	#endif
	
    return u32Len_Value;
}

#ifdef BACDEL_PR23
/**
*
* DESCRIPTION
* This function is used to decode Unsigned Integer from the BACnet frame.
* Following are found from clause 20.2.4 Encoding of an Unsigned Integer Value.
*
* @param pu8APDU      [in] Buffer which will hold the encoded octets.
* @param u32Len_Value [in] Number of bytes containing the Unsigned Integer.
* @param pu64Value    [in] Pointer to decoded value.
*
* @return int32_t     [out] Returns the number of apdu bytes consumed
*/
int32_t Decode_Unsigned_64(
	uint8_t *pu8APDU,
	uint32_t u32Len_Value,
	Uint64_t *pu64Value)
{
	uint16_t uValue16 = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Decode_Unsigned_64: entry \r\n");
	#endif

	if(NULL != pu64Value)
	{
		switch(u32Len_Value)
		{
		case 1:
			*pu64Value = pu8APDU[0];
			break;
		case 2:
			Decode_Unsigned16(&pu8APDU[0], &uValue16);
			*pu64Value = uValue16;
			break;
		case 3:
			Decode_Unsigned24(&pu8APDU[0], (uint32_t *)pu64Value);
			break;
		case 4:
			Decode_Unsigned32(&pu8APDU[0], (uint32_t *)pu64Value);
			break;
		case 5:
			Decode_Unsigned40(&pu8APDU[0], pu64Value);
			break;
		case 6:
			Decode_Unsigned48(&pu8APDU[0], pu64Value);
			break;
		case 7:
			Decode_Unsigned56(&pu8APDU[0], pu64Value);
			break;
		case 8:
			Decode_Unsigned64(&pu8APDU[0], pu64Value);
			break;
		default:
			*pu64Value = 0;
			break;
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Decode_Unsigned_64: exit \r\n");
	#endif
	return (int32_t)u32Len_Value;
}


/**
*
* DESCRIPTION
* This function is used to decode "CLASS" feild of BACnet tag as
* Context Specific tag with Tag Number feild as Unsigned Integer.
* Following are found from clause 20.2.15 Encoding of a Tagged Value.
*
* @param pu8APDU      [in] Buffer which will hold the encoded octets.
* @param uT8ag_Number [in] Tag Number feild of BACnet Tag.
* @param pu64Value    [in] Pointer to decoded value.
*
* @return int32_t     [out] Returns the number of apdu bytes consumed
*/
int32_t Decode_Context_Unsigned_64(
	uint8_t *pu8APDU,
	uint8_t u8Tag_Number,
	Uint64_t *pu64Value)
{
	uint32_t u32Len_Value = 0;
	int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Decode_Context_Unsigned_64: entry \r\n");
	#endif
	if(Decode_Is_Context_Tag(&pu8APDU[i32Len], u8Tag_Number))
	{
		i32Len +=
			Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8Tag_Number, &u32Len_Value);
		i32Len += Decode_Unsigned_64(&pu8APDU[i32Len], u32Len_Value, pu64Value);
	}
	else
	{
		i32Len = BACNET_STATUS_ERROR;
	}
	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Decode_Context_Unsigned_64: exit \r\n");
	#endif
	return i32Len;
}

/**
*
* DESCRIPTION
* This function is used to encode Unsigned Integer to BACnet frame.
* Following are found from clause 20.2.4 Encoding of an Unsigned Integer Value.
*
* @param pu8APDU	[in] Buffer which will hold the encoded octets.
* @param u64Value	[in] Value to be Encoded.
*
* @return int32_t	[out] Returns the number of apdu bytes consumed
*/
int32_t Encode_Bacnet_Unsigned64(
	uint8_t *pu8APDU,
	Uint64_t u64Value)
{
	int32_t i32Len = 0;        /* return value */

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Bacnet_Unsigned64: entry \r\n");
	#endif

	if(u64Value < 0x100)
	{
		pu8APDU[0] = (uint8_t)u64Value;
		i32Len = 1;
	}
	else if(u64Value < 0x10000)
	{
		i32Len = Encode_Unsigned16(&pu8APDU[0], (uint16_t)u64Value);
	}
	else if(u64Value < 0x1000000)
	{
		i32Len = Encode_Unsigned24(&pu8APDU[0], (uint32_t)u64Value);
	}
	else if(u64Value < 0x100000000)
	{
		i32Len = Encode_Unsigned32(&pu8APDU[0], (uint32_t)u64Value);
	}
	else if(u64Value < 0x10000000000)
	{
		i32Len = Encode_Unsigned40(&pu8APDU[0], u64Value);
	}
	else if(u64Value < 0x1000000000000)
	{
		i32Len = Encode_Unsigned48(&pu8APDU[0], u64Value);
	}
	else if(u64Value < 0x100000000000000)
	{
		i32Len = Encode_Unsigned56(&pu8APDU[0], u64Value);
	}
	else
	{
		i32Len = Encode_Unsigned64(&pu8APDU[0], u64Value);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Bacnet_Unsigned64: exit \r\n");
	#endif
	return i32Len;
}

/**
*
* DESCRIPTION
* This function is used to encode "CLASS" feild of BACnet tag as
* Context Specific tag with Tag Number feild as Unsigned Integer.
* Following are found from clause 20.2.4 Encoding of an Unsigned Integer Value.
*
* @param pu8APDU      [in] Buffer which will hold the encoded octets.
* @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
* @param u64Value     [in] Value to be encoded.
*
* @return int32_t     [out] Returns the number of apdu bytes consumed
*/
int32_t Encode_Context_Unsigned64(
	uint8_t *pu8APDU,
	uint8_t u8Tag_Number,
	Uint64_t u64Value)
{
	int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Context_Unsigned64: entry \r\n");
	#endif

	if(u64Value < 0x100)
	{
		i32Len = 1;
	}
	else if(u64Value < 0x10000)
	{
		i32Len = 2;
	}
	else if(u64Value < 0x1000000)
	{
		i32Len = 3;
	}
	else if(u64Value < 0x100000000)
	{
		i32Len = 4;
	}
	else if(u64Value < 0x10000000000)
	{
		i32Len = 5;
	}
	else if(u64Value < 0x1000000000000)
	{
		i32Len = 6;
	}
	else if(u64Value < 0x100000000000000)
	{
		i32Len = 7;
	}
	else
	{
		i32Len = 8;
	}

	i32Len = Encode_Tag(&pu8APDU[0], u8Tag_Number, true, (uint32_t)i32Len);
	i32Len += Encode_Bacnet_Unsigned64(&pu8APDU[i32Len], u64Value);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Context_Unsigned64: exit \r\n");
	#endif
	return i32Len;
}


/**
*
* DESCRIPTION
* This function is used to encode "CLASS" feild of BACnet tag as
* Application tag with Tag Number feild as Unsigned Integer.
* Following are found from clause 20.2.4 Encoding of an Unsigned Integer Value.
*
* @param pu8APDU  [in] Buffer which will hold the encoded octets.
* @param u64Value [in] Value to be encoded.
*
* @return int32_t [out]Returns the number of apdu bytes consumed
*/
int32_t Encode_Application_Unsigned64(
	uint8_t *pu8APDU,
	Uint64_t u64Value)
{
	int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
	"APDU: Encode_Application_Unsigned64: entry \r\n");
	#endif

	i32Len = Encode_Bacnet_Unsigned64(&pu8APDU[2], u64Value);
	i32Len +=
		Encode_Tag(&pu8APDU[0], BACNET_APPLICATION_TAG_UNSIGNED_INT, false,
		(uint32_t)i32Len);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
	"APDU: Encode_Application_Unsigned64: exit \r\n");
	#endif
	return i32Len;
}
#endif /* BACDEL_PR23 */
