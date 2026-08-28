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
*   File Name - pduEncodeDecode.h
*
*   RELEASE HISTORY                                                                                                                           
*   DATE         NAME               DESCRIPTION
*   08/08/2011   Ashish Verma       Added Decode_Context_Boolean_Value()
*   23/09/2011   Ashish verma       Changed the Character string structure
*                                   from BACNET_CHARACTER_STRING to
*                                   Pr_BACnetCharStr_t.
******************************************************************************/

/** @file pduEncodeDecode.h Functions to encode/decode BACnet data types  */

#ifndef BACDCODE_H
#define BACDCODE_H

#include <stddef.h>
#include "bacDELDef.h"
#include "pduDateTime.h"
#include "pduDataType.h"
#include "pduServiceStructure.h"
#include "bacnetStackMgmt.h"
//#include "propertydef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BACNET_STATUS_ERROR (-1)
#define BACNET_STATUS_ABORT (-2)
#define BACNET_STATUS_REJECT (-3)

#define TAG_NO_0   0
#define TAG_NO_1   1
#define TAG_NO_2   2
#define TAG_NO_3   3
#define TAG_NO_4   4
#define TAG_NO_5   5
#define TAG_NO_6   6
#define TAG_NO_7   7
#define TAG_NO_8   8
#define TAG_NO_9   9
#define TAG_NO_10  10
#define TAG_NO_11  11
#define TAG_NO_12  12
#define TAG_NO_13  13
#define TAG_NO_14  14
#define TAG_NO_15  15
#define TAG_NO_16  16
#define TAG_NO_17  17
#define TAG_NO_18  18
#define TAG_NO_19  19
#define TAG_NO_20  20

// BACnet Data length
#define DATA_LEN_0 0
#define DATA_LEN_1 1
#define DATA_LEN_2 2
#define DATA_LEN_3 3
#define DATA_LEN_4 4
#define DATA_LEN_5 5

#define BACNET_ID_VALUE(bacnet_object_instance, bacnet_object_type) \
    ((((bacnet_object_type) & BACNET_MAX_OBJECT) << BACNET_INSTANCE_BITS) | \
    ((bacnet_object_instance) & BACNET_MAX_INSTANCE))

#define BACNET_INSTANCE(bacnet_object_id_num) ((bacnet_object_id_num)&BACNET_MAX_INSTANCE)

#define BACNET_TYPE(bacnet_object_id_num) \
    (((bacnet_object_id_num) >> BACNET_INSTANCE_BITS ) & BACNET_MAX_OBJECT)

/* from clause 20.2.1.2 Tag Number */
/* true if extended tag numbering is used */
#define IS_EXTENDED_TAG_NUMBER(x) ((x & 0xF0) == 0xF0)

/* from clause 20.2.1.3.1 Primitive Data */
/* true if the extended value is used */
#define IS_EXTENDED_VALUE(x) ((x & 0x07) == 5)

/* from clause 20.2.1.1 Class */
/* true if the tag is context specific */
#define IS_CONTEXT_SPECIFIC(x) ((x & BIT3) == BIT3)

/* from clause 20.2.1.3.2 Constructed Data */
/* true if the tag is an opening tag */
#define IS_OPENING_TAG(x) ((x & 0x07) == 6)

/* from clause 20.2.1.3.2 Constructed Data */
/* true if the tag is a closing tag */
#define IS_CLOSING_TAG(x) ((x & 0x07) == 7)

/* from clause 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */

/** 
*   Encodes the BACnet Tags.  
*   Following are found in clause 20.2.1 General Rules for Encoding BACnet Tags
*   @param apdu [out] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Application datatypes or Context Specific tag numbers.
*   @param context_specific [in] Class feild of BACnet tag.
*   @param len_value_type [in] Length/Value/Type feild of BACnet tag.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Tag(uint8_t *uAPDU, uint8_t uTag_Number, bool bContext_Specific,
            uint32_t uLen_Value_Type);

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */

/** 
*   Encodes the Opening Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [out] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Context Specific tag numbers.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Opening_Tag(uint8_t *uAPDU, uint8_t uTag_Number);

/** 
*   Encodes the Closing Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [out] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Context Specific tag numbers.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Closing_Tag(uint8_t *uAPDU, uint8_t uTag_Number);

/** 
*   Decodes the Tag Number of BACnet Tags.  
*   Following are found in clause 20.2.1.2, Tag Number.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [out] Pointer to Application datatypes or 
*                           Context Specific tag numbers.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number(uint8_t *uAPDU, uint8_t *uTag_Number);

/** 
*   Decodes the Tag Number of BACnet Tags, but will safely fail 
*   if packet has been truncated.  
*   Following are found in clause 20.2.1.2, Tag Number.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [out] Pointer to Application datatypes or 
*                           Context Specific tag numbers.
*   @param uAPDU_Len_Remaining [in] Length of APDU.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number_Safe(uint8_t *uAPDU, uint32_t uAPDU_Len_Remaining,
            uint8_t *uTag_Number);

/** 
*   Decodes the BACnet Tag Number and its Value.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [out] Pointer to the decoded value.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number_And_Value(uint8_t *uAPDU, uint8_t *uTag_Number,
            uint32_t *uValue);

/** 
*   Decodes the BACnet Tag Number and its Value.Same as function above, 
*   but will safely fail is packet has been truncate  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uAPDU_len_remaining [in]Length of remaining APDU segment.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [out] Pointer to the decoded value.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Tag_Number_And_Value_Safe(uint8_t *uAPDU, 
            uint32_t uAPDU_Len_Remaining, uint8_t *uTag_Number,
            uint32_t *uValue);

/* returns true if the tag is an opening tag and matches */

/** 
*   Decodes Opening tag. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Opening_Tag_Number(uint8_t *uAPDU, uint8_t uTag_Number);

/* returns true if the tag is a closing tag and matches */

/** 
*   Decodes Closing Tag. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Closing_Tag_Number(uint8_t *uAPDU, uint8_t uTag_Number);
/* returns true if the tag is context specific and matches */

/** 
*   Decodes Context Specific tag & Tag Namuber. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Context_Tag(uint8_t *uAPDU, uint8_t uTag_Number);

/** 
*   Decodes Context Specific tag & Tag Namuber. 
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param iTag_length [out] The number of bytes consumed.
*   @return Returns TRUE/FALSE. 
*/
bool Decode_Is_Context_Tag_With_Length(uint8_t *uAPDU, uint8_t uTag_Number,
            int32_t *iTag_Length);

/** 
*   Decodes the Opening Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @return TRUE/FALSE. 
*/
bool Decode_Is_Opening_Tag(uint8_t *uAPDU);

/** 
*   Decodes the Closing Tag.  
*   Following are found in clause 20.2.1.3.2, Constructed Data.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @return TRUE/FALSE. 
*/
bool Decode_Is_Closing_Tag(uint8_t *uAPDU);

bool Decode_Context_Tag(uint8_t *u8APDUReq, uint8_t u8TagNumber, 
                        uint32_t* u32LenValType);


/* from clause 20.2.2 Encoding of a Null Value */
/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as NULL. 
*   Following are found from clause 20.2.2 Encoding of a Null Value
*   and 20.2.1 General Rules for Encoding BACnet Tags
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param bBoolean_Value [in] Denotes Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Application_Null(uint8_t *uAPDU);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Null.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Null(uint8_t *uAPDU, uint8_t uTag_Number);

/* from clause 20.2.3 Encoding of a Boolean Value */
/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Boolean Datatype. 
*   Following are found from clause 20.2.3 Encoding of a Boolean Value
*   and 20.2.1 General Rules for Encoding BACnet Tags
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param bBoolean_Value [in] Denotes Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Application_Boolean(uint8_t *uAPDU, bool bBoolean_Value);

/**
*   Decodes Booean Value from BACnet frame.
*   @param uLen_Value [in] The Length/Value/Type feild in BACnet tag.
*   @return Returns TRUE/FALSE.
*/
bool Decode_Boolean(uint32_t uLen_Value);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific with Tag Number 
*   feild as Boolean Datatype.
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param bBoolean_Value [in] Denotes Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Encode_Context_Boolean(uint8_t *uAPDU, uint8_t uTag_Number, 
            bool bBoolean_Value);

/** 
*   Decodes Boolean from context specific data
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @return Returns TRUE/FALSE.
*/
bool Decode_Context_Boolean(uint8_t *uAPDU);

/*
 *  Decodes Boolean from context specific data
 */
uint32_t Decode_Context_Boolean_Value(uint8_t *uAPDU, bool *bBoolean_Value);

/**
*   Decodes Boolean from context specific data
*   @param apdu [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet tag.
*   @param bBoolean_Value [out] Pointer to Boolean data.
*   @return Returns the number of apdu bytes consumed. 
*/
int32_t Decode_Context_BOOLEAN(uint8_t *uAPDU, uint8_t uTag_Number, 
            bool *bBoolean_Value);

/* from clause 20.2.10 Encoding of a Bit String Value */
/* returns the number of apdu bytes consumed */
/** 
*   Decodes BitString Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param pstBitString [out] Pointer to decoded value in struct BACnetBitStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_BitString_Small(uint8_t *pu8APDU, uint32_t u32Len_Value,
                        BACnetBitStr_t *pstBitString);

/** 
*   Decodes BitString Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Number of bytes containing the Unsigned Integer.
*   @param pstBitString [out] Pointer to decoded value in struct BACnetBITStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_BITstring(uint8_t *pu8APDU, uint32_t u32Len_Value,
                BACnetBITStr_t *pstBitString);

/** 
*   Decodes Bitstrings of context specific data.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_BITSTRING(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBitStr_t *pstBitString);

/** 
*   Decodes Bitstrings of context specific data.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBITStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_BITstring(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBITStr_t *pstBitString);

/* returns the number of apdu bytes consumed */
/** 
*   Encodes Bitstrings in BACnet frame.It can be traced from clause 20.2.10 
*   Encoding of a Bit String Value in BACnet standard.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_BIT_STRING [in] Pointer to BACNET_BIT_STRING structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bitstring(uint8_t *uAPDU, BACNET_BIT_STRING *pBACNET_BIT_STRING);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as BitString.Following are found from clause 20.2.10 Encoding of 
*   a BitString.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBitString [in] Pointer to BACnetBITStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_BITstring(uint8_t *pu8APDU, BACnetBITStr_t *pstBitString);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as BitString.Following are found from clause 20.2.10 Encoding of 
*   a BitString.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pstBitString [in] Pointer to BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Bitstring_small(uint8_t *pu8APDU, BACnetBitStr_t *pstBitString);

/** 
*	encodes BACnetBitStr_t type context bitstrings.
*
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBitStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_BITSTRING(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBitStr_t *pstBitString);

/** 
*	encodes BACnetBITStr_t type context bitstrings.
*
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u8Tag_Number [in] Tag Number feild of BACnet Tag.
*   @param pstBitString [in] Pointer to BACnetBITStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_BITString(uint8_t *pu8APDU, uint8_t u8Tag_Number,
            BACnetBITStr_t *pstBitString);

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
            uint8_t iUnusedBits, uint8_t i8NumofBytes);

/* from clause 20.2.6 Encoding of a Real Number Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Real.Following are found from clause 20.2.6
*   Encoding of an Real Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param fValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Real(uint8_t *uAPDU, Float_t fValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Real.Following are found from clause 20.2.6
*   Encoding of an Real Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param fValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Real(uint8_t *uAPDU, uint8_t uTag_Number, Float_t fValue);

/* from clause 20.2.7 Encoding of a Double Precision Real Number Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Double.Following are found from clause 20.2.7
*   Encoding of an Double Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param dValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Double(uint8_t *uAPDU, Double_t dValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Double.Following are found from clause 20.2.7
*   Encoding of an Double Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param dValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Double(uint8_t *uAPDU, uint8_t uTag_Number, 
            Double_t dValue);

/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Decodes Object Identifier in the BACnet frame.Following are found 
*   from clause 20.2.14 Encoding of an Object Identifier Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uObject_Type [out] Pointer to the BACnet Object type.
*   @param uInstance [out] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Object_Id(uint8_t *pu8APDU, uint32_t *pu32Object_Type,
            uint32_t *u32Instance);

/** 
*   Decodes Object Identifier in the BACnet frame. Same as function above,
*   but will safely fail is packet has been truncated. Following are found
*   from clause 20.2.14 Encoding of an Object Identifier Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [in] Length of the remaining data.
*   @param uObject_Type [in] Pointer to the BACnet Object type.
*   @param uInstance [in] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Object_Id_Safe(uint8_t *uAPDU, uint32_t uLen_Value,
            uint32_t *uObject_Type, uint32_t *uInstance);

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Object Identifier.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uObject_Type [in] Pointer to the BACnet Object type.
*   @param uInstance [in] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Object_Id(uint8_t *uAPDU, uint8_t uTag_Number, 
            uint32_t *u32Object_Type, uint32_t *uInstance);

/** 
*   Encodes Object Identifier in the BACnet frame.Following are found 
*   from clause 20.2.14 Encoding of an Object Identifier Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uObject_Type [in] BACnet Object type.
*   @param uInstance [in] BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Object_Id(uint8_t *uAPDU, int32_t iObject_Type,
            uint32_t uInstance);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Object Identifier.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uObject_Type [in] Pointer to the BACnet Object type.
*   @param uInstance [in] Pointer to BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Object_Id(uint8_t *uAPDU, uint8_t uTag_Number,
            int32_t iObject_Type, uint32_t uInstance);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Object Identifier.Following are found from clause 20.2.14
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uObject_Type [in] BACnet Object type.
*   @param uInstance [in] BACnet Instance number.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Object_Id(uint8_t *uAPDU, int32_t iObject_Type,
            uint32_t uInstance);

/* from clause 20.2.8 Encoding of an Octet String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes Octet String to BACnet frame.Following are found from clause 20.2.8
*   Encoding of an Octet String Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Octet_String(uint8_t *uAPDU, 
            BACnetOctetStr_t *pBACNET_OCTET_STRING);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Octet String.Following are found from clause 20.2.8
*   Encoding of an Octet String Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Octet_String(uint8_t *uAPDU, 
            BACnetOctetStr_t *pBACNET_OCTET_STRING);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Octet String.Following are found from clause 20.2.8
*   Encoding of an Octet String Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Octet_String(uint8_t *uAPDU, uint8_t uTag_Number,
            BACnetOctetStr_t *pBACNET_OCTET_STRING);

/** 
*   Decodes Octet String in the BACnet frame.Following are found 
*   from clause 20.2.8 Encoding of an Octet String Value.
*   @param pu8APDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param u32Len_Value [out] Length of the Octet string.
*   @param pstOctetString [out] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_OCTET_String(uint8_t *pu8APDU, uint32_t u32Len_Value,
            BACnetOctetStr_t *pstOctetString);

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Octet String.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_OCTET_STRING [in] Pointer to BACnetOctetStr_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Octet_String(uint8_t *uAPDU, uint8_t uTag_Number,
            BACnetOctetStr_t *pBACNET_OCTET_STRING);

/* from clause 20.2.9 Encoding of a Character String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes Character String to BACnet frame.Following are found from 
*   clause 20.2.9 Encoding of an Character String Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Character_String(uint8_t *uAPDU,
                                       BACnetCharStr_t *pstCharString);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Character String.Following are found from clause 20.2.9
*   Encoding of an Character String Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Character_String(uint8_t *uAPDU,
            BACnetCharStr_t *pstCharString);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Character String.Following are found from clause 20.2.15
*   Encoding of an Character String Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_CHARACTER_STRING [in] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Character_String(uint8_t *uAPDU, uint8_t uTag_Number,
            BACnetCharStr_t *pstCharString);

/** 
*   Decodes Character String from the BACnet frame.Following are found 
*   from clause 20.2.9 Encoding of an Character String Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [out] Length of the Character string.
*   @param pBACNET_CHARACTER_STRING [out] Pointer to BACnetCharStr_t 
*           structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Character_String(uint8_t *uAPDU, uint32_t uLen_Value,
            BACnetCharStr_t *pstCharString);

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
            BACnetCharStr_t *pstCharString);

/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes Unsigned Integer to BACnet frame.Following are found from 
*   clause 20.2.4 Encoding of an Unsigned Integer Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be Encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Unsigned(uint8_t *uAPDU, uint32_t uValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.4
*   Encoding of an Unsigned Integer Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Unsigned(uint8_t *uAPDU, uint8_t uTag_Number, 
            uint32_t uValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.4
*   Encoding of an Unsigned Integer Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Unsigned(uint8_t *uAPDU, uint32_t uValue);

int32_t Encode_Application_StampUnsigned(uint8_t *uAPDU, uint32_t uValue);

/** 
*   Decodes Unsigned Integer from the BACnet frame.Following are found 
*   from clause 20.2.4 Encoding of an Unsigned Integer Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [out] Number of bytes containing the Unsigned Integer.
*   @param uValue [out] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Unsigned(uint8_t *uAPDU, uint32_t uLen_Value, 
            uint32_t * uValue);

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Unsigned Integer.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Unsigned(uint8_t *uAPDU, uint8_t uTag_Number,
            uint32_t * uValue);

/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes Signed Integer to BACnet frame.Following are found from 
*   clause 20.2.5 Encoding of an Signed Integer Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param iValue [in] Value to be Encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Signed(uint8_t *uAPDU, int32_t uValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Signed Integer.Following are found from clause 20.2.5
*   Encoding of an Signed Integer Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param iValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Signed(uint8_t *uAPDU, int32_t uValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Signed Integer.Following are found from clause 20.2.5
*   Encoding of an Signed Integer Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param iValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Signed(uint8_t *uAPDU, uint8_t uTag_Number, 
            int32_t uValue);

/** 
*   Decodes Signed Integer from the BACnet frame.Following are found 
*   from clause 20.2.5 Encoding of an Signed Integer Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [out] Number of bytes containing the Unsigned Integer.
*   @param iValue [out] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Signed(uint8_t *uAPDU, uint32_t uLen_Value, int32_t * iValue);

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Signed Integer.Following are found from clause 20.2.15
*   Encoding of a Tagged Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param iValue [in] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Signed(uint8_t *uAPDU, uint8_t uTag_Number, 
            int32_t * iValue);

/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Decodes Enumerated from the BACnet frame.Following are found 
*   from clause 20.2.11 Encoding of an Enumerated Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [out] Number of bytes containing the Unsigned Integer.
*   @param uValue [out] Pointer to decoded value.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Enumerated(uint8_t *uAPDU, uint32_t uLen_Value, 
            uint32_t *uValue);
int32_t DT_Encode_OptionalUnsigned(
	uint8_t *pu8Apdu,
	BACnetOptionalUnsigned_t *pstOptionalUnsigned);
/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Enumerated.Following are found from clause 20.2.15
*   Encoding of an Enumerated Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Enumerated(uint8_t *uAPDU, uint8_t uTag_Value,
            uint32_t * uValue);

/** 
*   Encodes Enumerated to BACnet frame.Following are found from 
*   clause 20.2.11 Encoding of an Enumerated Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be Encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Enumerated(uint8_t *uAPDU, uint32_t uValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Enumerated.Following are found from clause 20.2.11
*   Encoding of an Enumerated Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Enumerated(uint8_t *uAPDU, uint32_t uValue);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as BitString.Following are found from clause 20.2.10 Encoding of 
*   a BitString.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_BIT_STRING [in] Pointer to BACNET_BIT_STRING structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Byte(uint8_t *uAPDU, uint8_t *uValue, 
                        uint8_t iUnusedBits,uint8_t i8NumofBytes);


/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Enumerated.Following are found from clause 20.2.11
*   Encoding of an Enumerated Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param uValue [in] Value to be encoded.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Enumerated(uint8_t *uAPDU, uint8_t uTag_Number,
            uint32_t uValue);

/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes Time to BACnet frame.Following are found from 
*   clause 20.2.13 Encoding of an Time Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Time(uint8_t *uAPDU, BACnetTime_t *pBACNET_TIME);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Time(uint8_t *uAPDU, BACnetTime_t *pBACNET_TIME);

int32_t Encode_Application_StampTime(uint8_t *uAPDU, BACnetTime_t *pBACNET_TIME);


int32_t Encode_Application_StampDateTime(uint8_t *uAPDU,BACnetTime_t *pstTime,
                                                        BACnetDate_t *pstDate);

/** 
*   Decodes Time from the BACnet frame.Following are found 
*   from clause 20.2.13 Encoding of a Time Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_TIME [out] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Time(uint8_t *uAPDU, BACnetTime_t *pBACNET_TIME);

/** 
*   Decodes Time from the BACnet frame.Same as function above,
*   but will safely fail is packet has been truncated.Following are found 
*   from clause 20.2.13 Encoding of a Time Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [in] Length of the remaining data.
*   @param pBACNET_TIME [out] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Time_Safe(uint8_t *uAPDU, uint32_t uLen_Value,
            BACnetTime_t *pBACNET_TIME);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_TIME [in] Pointer Value to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Time(uint8_t *uAPDU, uint8_t uTag_Number, 
            BACnetTime_t *pBACNET_TIME);

/** 
*   Decodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Application_Time(uint8_t *uAPDU, BACnetTime_t *pBACNET_TIME);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Time.Following are found from clause 20.2.13
*   Encoding of an Time Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_TIME [in] Pointer to BACnetTime_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Bacnet_Time(uint8_t *uAPDU, uint8_t uTag_Number,
            BACnetTime_t *pBACNET_TIME);

/* BACnet Date */
/* year = years since 1900 */
/* month 1=Jan */
/* day = day of month */
/* wday 1=Monday...7=Sunday */

/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
/** 
*   Encodes Date to BACnet frame.Following are found from 
*   clause 20.2.12 Encoding of an Date Value.
*   @param uAPDU [out] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Bacnet_Date(uint8_t *uAPDU, BACnetDate_t *pBACNET_DATE);

/** 
*   Encodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Application_Date(uint8_t *uAPDU, BACnetDate_t *pBACNET_DATE);

/** 
*   Encodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Context_Date(uint8_t *uAPDU, uint8_t uTag_Number, 
            BACnetDate_t *pBACNET_DATE);

/** 
*   Decodes Date from the BACnet frame.Following are found 
*   from clause 20.2.12 Encoding of an Date Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_DATE [out] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Date(uint8_t *uAPDU, BACnetDate_t *pBACNET_DATE);

/** 
*   Decodes Date from the BACnet frame.Following are found 
*   from clause 20.2.12 Encoding of an Date Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uLen_Value [in] Length of the remaining data.
*   @param pBACNET_DATE [out] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Date_Safe(uint8_t *uAPDU, uint32_t uLen_Value,
            BACnetDate_t *pBACNET_DATE);

/** 
*   Decodes "CLASS" feild of BACnet tag as Application tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Application_Date(uint8_t *uAPDU, BACnetDate_t *pBACNET_DATE);

/** 
*   Decodes "CLASS" feild of BACnet tag as Context Specific tag with Tag Number 
*   feild as Date.Following are found from clause 20.2.12
*   Encoding of an Date Value.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param uTag_Number [in] Tag Number feild of BACnet Tag.
*   @param pBACNET_DATE [in] Pointer to BACnetDate_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Decode_Context_Date(uint8_t *uAPDU, uint8_t uTag_Number,
            BACnetDate_t *pBACNET_DATE);

/* from clause 20.1.2.4 max-segments-accepted */
/* and clause 20.1.2.5 max-APDU-length-accepted */
/* returns the encoded octet */

/** 
*   Encodes the max-segments-accepted & max-APDU-length-accepted octet.
*   It is described in section 20.1.2.4 & 20.1.2.5 of the BACnet standard.
*   @param iMax_Segs [in] Specifies the maximum number of segments that 
*                        the device will accept.
*   @param iMax_APDU [in] Specifies the maximum size of a single APDU 
*                        that the issuing device will accept.
*   @return Returns the encoded octet. 
*/
uint8_t Encode_Max_Segs_Max_Apdu(int32_t iMax_Segs, int32_t iMax_APDU);

/** 
*   Decodes the max-segments-accepted. It is described in section 20.1.2.4 
*   of the BACnet standard.
*   @param iOctet [in] Encoded octet containing max-segments-accepted & 
*                      max-APDU-length-accepted values.
*   @return Returns the value of max-segments-accepted. 
*/
int32_t Decode_Max_Segs(uint8_t uOctet);

/** 
*   Decodes the max-APDU-length-accepted.It is described in section 20.1.2.5 
*   of the BACnet standard. 
*   @param iOctet [in] Encoded octet containing max-segments-accepted & 
*                     max-APDU-length-accepted values.
*   @return Returns the value of max-APDU-length-accepted. 
*/
int32_t Decode_Max_Apdu(uint8_t uOctet);

/* returns the number of apdu bytes consumed */
/** 
*   Encodes simple acknowledge response.
*   @param uAPDU [in] Buffer which will hold the encoded Bacnet octets.
*   @param pBACnet_Device_Data [in] Pointer to bacnetRequestData_t structure.
*   @return Returns the number of apdu bytes consumed 
*/
int32_t Encode_Simple_Ack(processInfo_t *pBACnet_Device_Data, 
            uint8_t *uAPDU);
            
#ifdef BACDEL_PR23
/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
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
	Uint64_t u64Value);

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
	Uint64_t u64Value);

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
	Uint64_t u64Value);

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
	Uint64_t *pu64Value);

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
	uint8_t uT8ag_Number,
	Uint64_t *pu64Value);
#endif /* BACDEL_PR23 */

#ifdef __cplusplus

}
#endif /* __cplusplus */
#endif
