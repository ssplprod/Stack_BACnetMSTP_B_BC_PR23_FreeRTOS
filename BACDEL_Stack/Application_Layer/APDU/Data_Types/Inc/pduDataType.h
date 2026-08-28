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
/************************************************************************* 
*                                                                         
* FILE
*   pduDataType.h 
*
*  AUTHOR
*
*  DESCRIPTION
*   Support header fle for bacreal.c. This include all defines and
*   Declarations useful for real/double conversions.
*
*  RELEASE HISTORY
*   DATE                NAME                      DESCRIPTION
*
*   15-09-2011                                     Initial Draft
*
*************************************************************************/

/* To Include the following file */
#ifndef BACDATATYPE_H
#define BACDATATYPE_H

#include "bacDELDef.h"
#include <stdint.h>
#include "bacDELStackConfig.h"
#include "propertydef.h"

/*****************************************************************************/

void BitString_Init(BACNET_BIT_STRING * );
void BitString_Set_Bit( BACNET_BIT_STRING * , uint8_t , bool );
bool BitString_Bit(BACNET_BIT_STRING * , uint8_t );
uint8_t BitString_Bits_Used( BACNET_BIT_STRING * );
  
/* Returns the number of bytes that a bit string is using */
uint8_t BitString_Bytes_Used(BACNET_BIT_STRING * );
uint8_t BitString_Bits_Capacity(BACNET_BIT_STRING * );
    
/* Used for encoding and decoding from the APDU */
uint8_t BitString_Octet(BACNET_BIT_STRING * ,uint8_t );
bool BitString_Set_Octet(BACNET_BIT_STRING * , uint8_t , uint8_t );
bool BitString_Set_Bits_Used(BACNET_BIT_STRING * , uint8_t , uint8_t );
bool BitString_Copy(BACNET_BIT_STRING * , BACNET_BIT_STRING * );
bool BitString_COPY(BACnetBITStr_t *pstDstBitStr, BACNET_BIT_STRING *pstSrcBitStr);
bool BitString_Same(BACNET_BIT_STRING * , BACNET_BIT_STRING * );


/* returns false if the string exceeds capacity
   initialize by using length=0 */
bool CharacterString_Init(BACnetCharStr_t * , uint8_t , const int8_t *,uint32_t );
/* Used for ANSI C-Strings */
bool CharacterString_Init_ANSI(BACnetCharStr_t * , const int8_t *);
bool CharacterString_ByteCopy(int8_t *, uint32_t , BACnetCharStr_t * );
/* copies from Pr_BACnetCharStr_t to BACNET_CHARACTER_STRING struct */
bool CharacterString_Copy(int8_t *pstSrcString, 
    BACnetCharStr_t *pstDestString, uint32_t u32DestMaxLen);
/* Returns the value */
int8_t *CharacterString_Value(BACnetCharStr_t * );
/* Returns the length */
uint32_t CharacterString_Length(BACnetCharStr_t *pstCharString);
uint8_t CharacterString_Encoding(BACnetCharStr_t *);



/* Returns false if the string exceeds capacity initialize by using length=0 */
bool OctetString_Init(BACnetOctetStr_t * , uint8_t * , uint32_t );
bool OctetString_Copy(BACnetOctetStr_t * ,BACnetOctetStr_t * );
    
/* Returns false if the string exceeds capacity */
bool OctetString_Append(BACnetOctetStr_t * , uint8_t * , uint32_t );
                            
/* This function sets a new length without changing the value. If length 
 * exceeds capacity, no modification happens and function returns false. */
bool OctetString_Truncate(BACnetOctetStr_t * ,uint32_t );
    
/* Returns the value */
uint8_t *OctetString_Value( BACnetOctetStr_t * );
    
/* Returns the length.*/
uint32_t OctetString_Length(BACnetOctetStr_t * );
    
uint32_t OctetString_Capacity(BACnetOctetStr_t * );
    
/* Returns true if the same length and contents */
bool OctetString_Value_Same(BACnetOctetStr_t * , BACnetOctetStr_t * );
/* End of fucntion supporting string data type */
/*****************************************************************************/

/*****************************************************************************/
/* Include the function prototype of Data Type Real */
int32_t Decode_Real_Safe(uint8_t * , uint32_t , Float_t *);

int32_t Decode_Real(uint8_t * , Float_t *);

int32_t Decode_Context_Real(uint8_t *,uint8_t ,Float_t *);
    
int32_t Encode_Bacnet_Real(Float_t , uint8_t * );
int32_t Decode_Double(uint8_t * ,  Double_t *);
int32_t Decode_Context_Double(uint8_t * ,  uint8_t ,
                          Double_t *);
int32_t Decode_Double_Safe(uint8_t * , uint32_t ,
                          Double_t *);

int32_t Encode_Bacnet_Double(Double_t , uint8_t * );

/* End of fucntion supporting real data type */
/*****************************************************************************/

/*****************************************************************************/
/* Include the function prototype of Data Type Int */
/** 
*   Encode datatype unsigned16.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type unsigned16.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned16(uint8_t *uAPDU, uint16_t uValue);

/** 
*   Decode datatype unsigned16.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type unsigned16.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned16(uint8_t *uAPDU, uint16_t *uValue);

/** 
*   Encode datatype unsigned24.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned24(uint8_t *uAPDU, uint32_t uValue);

/** 
*   Decode datatype unsigned24.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned24(uint8_t *uAPDU, uint32_t *uValue);

/** 
*   Encode datatype unsigned32.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned32(uint8_t *uAPDU, uint32_t uValue);

/** 
*   Decode datatype unsigned32.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned32(uint8_t *uAPDU, uint32_t *uValue);

/* signed value encoding and decoding */

/** 
*   Encode datatype signed8.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type signed8.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Signed8(uint8_t *uAPDU, int8_t iValue);

/** 
*   Decode datatype signed8.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type signed32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed8(uint8_t *uAPDU, int32_t *iValue);

/** 
*   Encode datatype unsigned8.
*   @param pu8APDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param u8Value [in] Value of type unsigned8.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Unsigned8(uint8_t *pu8APDU, uint8_t u8Value);

/** 
*   Decode datatype unsigned8.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*    @param pu32Value [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Unsigned8(uint8_t *pu8APDU, uint32_t *pu32Value);

/** 
*   Encode datatype signed16.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type signed16.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Signed16(uint8_t *uAPDU, int16_t iValue);

/** 
*   Decode datatype signed16.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type signed32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed16(uint8_t *uAPDU, int32_t *iValue);

/** 
*   Encode datatype signed24.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_signed24(uint8_t *uAPDU, int32_t iValue);

/** 
*   Encode datatype signed24.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed24(uint8_t *uAPDU, int32_t *iValue);

/** 
*   Encode datatype unsigned32.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [in] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Encode_Signed32(uint8_t *uAPDU, int32_t iValue);

/** 
*   Encode datatype unsigned32.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*	@param uValue [out] Value of type unsigned32.
*   @return Returns number of Bytes consumed. 
*/
int32_t Decode_Signed32(uint8_t *uAPDU, int32_t *iValue);

/** */
bool Bitstring_PropParsing(int8_t *pu8PropertyVal,
                        BACNET_PROPERTY_VALUE * value);

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
	Uint64_t u64Value);

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
	Uint64_t u64Value);

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
	Uint64_t u64Value);

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
	Uint64_t u64Value);

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
	Uint64_t *pu64Value);

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
	Uint64_t *pu64Value);

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
	Uint64_t *pu64Value);

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
	Uint64_t *pu64Value);

#endif /* BACDEL_PR23 */
/* End of fucntion supporting int data type */
/*****************************************************************************/
#endif /* BACDATATYPE_H */

