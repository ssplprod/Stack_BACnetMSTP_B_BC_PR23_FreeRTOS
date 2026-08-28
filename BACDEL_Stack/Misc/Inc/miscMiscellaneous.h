/********************************************************************************
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
*   FILE NAME
*	miscMiscellaneous.h
*
*   AUTHORS
*	Harshal Mangale, Venu M., Ashish Verma
*
*   DESCRIPTION                                                            
*   Miscellaneous functions used within the stack. These functions are mainly 
*	related to:
*	- StringCopy, StringCompare, etc.
*	- MemCopy, MemCompare, etc.
-	- BitReversing, etc.
*
********************************************************************************/

#ifndef MISC_H
#define MISC_H

/** header files */

#include <string.h>
/****************************************************************************
 **** Macros: 
 ****************************************************************************/





/****************************************************************************
 **** Bit Operations
 ****************************************************************************/

/* macro to set a flag */
#define SETBIT(bmem,flg)    ((bmem) |= (flg))
/* macro to reset a flag */
#define RESETBIT(bmem,flg)  ((bmem) &= ((~(flg))))
/* check if the bit is set */
#define CHKTRUE(bmem,flg)   (((bmem) & (flg)) != 0)
/* check if bit is reset */
#define CHKFALSE(bmem,flg)  (((bmem) & (flg)) == 0)

/* a = register, b = bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b))) 
#define BIT_FLIP(a,b) ((a) ^= (1<<(b))) // inverts bit
#define BIT_CHECK(a,b) ((a) & (1<<(b)))	// gives position of bit if it is 1
#define BIT_VALUE_CHECK(a,b) (((a)>>(b)) & (0x01)) // gives 1 if bit is 1 else 0

/* x = target variable, y = mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) ((x) & (y))

/* bit value */
#ifndef _BV
#define _BV(x) (1<<(x))
#endif

/****************************************************************************
 **** Byte Operations
 ****************************************************************************/

/* lower nibble */
#ifndef LO_NIB
#define LO_NIB(b) ((b) & 0xF)
#endif

/* higher nibble */
#ifndef HI_NIB
#define HI_NIB(b) ((b) >> 4)
#endif

/* lower byte */
#ifndef LO_BYTE
#define LO_BYTE(w) ((uint8_t)(w))
#endif

/* higher byte */
#ifndef HI_BYTE
#define HI_BYTE(w) ((uint8_t)((uint16_t)(w) >> 8))
#endif

/* lower word */
#ifndef LO_WORD
#define LO_WORD(x) ((uint16_t)(x))
#endif

/* higher word */
#ifndef HI_WORD
#define HI_WORD(x) ((uint16_t)((uint32_t)(x) >> 16))
#endif

/* create a word from bytes */
#ifndef MAKE_WORD
#define MAKE_WORD(lo,hi) \
    ((uint16_t)(((uint8_t)(lo))|(((uint16_t)((uint8_t)(hi)))<<8)))
#endif

/* create a long word from bytes */
#ifndef MAKE_LONG
#define MAKE_LONG(lo,hi) \
    ((uint32_t)(((uint16_t)(lo))|(((uint32_t)((uint16_t)(hi)))<<16)))
#endif

/****************************************************************************
 **** Endianness
 ****************************************************************************/

/* global variable to save the ENDIAN-NESS of hardware architecture */
extern uint8_t g_u8EndianNess;

/** 
*
* DESCRIPTION
* Function to determine ENDIANNESS of the hardware.
* This function will set a global variable for endianness.
*  
* @param  [in]  No parameter
* @return [out] No return value
* @update [out] EndianNess (global variable)
*
*/
void EndianNess_Configuration(void);

/****************************************************************************
 **** Function Declarations
 ****************************************************************************/

/** 
*
* DESCRIPTION
* Function to swap bytes for signed / unsigned short integer value.
*  
* @param  u16Value [in] Value to be swapped
* @return u16NewValue [out] Swapped value
*
*/
uint16_t Swap_Unsigned16_Bytes(uint16_t u16Value);

/**
*
* DESCRIPTION                                                                          
* Function to reverse bits of a byte.
*    
* @param u8InByte   [in]  Input byte value
* @return u8OutByte [out] Output byte value
*
*/
uint8_t Byte_Reverse_Bits(uint8_t u8InByte);

/**
*                                                                         
* DESCRIPTION                                                                          
* Function to copy string from source to destination
*    
* @param pu8Destination [in] Pointer to Destination array
* @param pu8Source      [in] Pointer to Source array        
* @param u32NumChar     [in] Number of characters to be copied
*
* @return VOID [out] No return value
*
*/
void Copy_String(uint8_t *pu8Destination, uint8_t *pu8Source, uint32_t u32NumChar);

/** 
*
* DESCRIPTION
* Copy bytes of data from source to destination (with offset) if there is 
* enough space.
* Returns 0 if there is not enough space, or the number of bytes copied.
*
* @param pvDest         [in]  Destination adderss
* @param pvSrc          [in]  Source address
* @param u32offset      [in]  Where in destination to copy the data 
* @param u32DataLen     [in]  Amount of data to copy
* @param u32MaxDestSize [in]  Total size of destination buffer
*
* @return [out] no of bytes copied.
*
*/
uint32_t Memcopy(void *pvDest, void *pvSrc, uint32_t u32offset, 
	uint32_t u32DataLen, uint32_t u32MaxDestSize);

/** 
*
* DESCRIPTION
* Copy bytes of data from source to destination (with offset) if there is 
* enough space.
* Destination buffer's size is increased if data to be copied is more.
* Returns 0 if there is not enough space, or the number of bytes copied.
*
* @param pvDest            [in]  Destination adderss
* @param pvSrc             [in]  Source address
* @param u32offset         [in]  Where in destination to copy the data 
* @param u32DataLen		   [in]  Amount of data to copy
* @param u32MaxDestSize    [in]  Total size of destination buffer
* @param u32IncreaseDestBy [in] If additional memory is required, then it 
*							    is increased by this size.
*
* @return [out] no of bytes copied.
*
*/
uint32_t Memcopy_Dynamic_Mem_Expansion(
	void **pvDest, void *pvSrc, uint32_t u32offset, 
	uint32_t u32DataLen, uint32_t *pu32MaxDestSize, 
    uint32_t u32IncreaseDestBy);

/** 
*
* DESCRIPTION
* Calculate the length of string.
* Returns string length, if length exceeds 'u32MaxDestSize' is returned.
*
* @param pvString		[in]  String pointer
* @param u32MaxDestSize [in]  Max allowed string length
*
* @return [out] length of string.
*
*/
uint32_t Strnlen(void *pvString, uint32_t u32MaxStrLen);

/** 
*
* DESCRIPTION
* Copy string from source to destination.
* Returns 0 if there is not enough space, or the number of bytes copied.
*
* @param pvDest         [in]  Destination adderss
* @param pvSrc          [in]  Source address
* @param u32MaxDestSize [in]  Total size of destination buffer
*
* @return [out] no of bytes copied.
*
*/
uint32_t Strcopy(void *pvDest, void *pvSrc, uint32_t u32MaxDestSize);

/** 
*
* DESCRIPTION
* Compare string of source and destination.
* - Returns 0 if both strings are same.
* - -ve value if first character that does not match has a lower value in str1.
* - +ve value if first character that does not match has a greater value in str1.
*
* @param pvStr1     [in]   Source string 1 address
* @param pvStr2     [in]   Source string 2 address
* @param u32DataLen [in]   Length of bytes to compare
*
* @return [out] 0, -ve or +ve values.
*
*/
int32_t Strncmp(void *pvStr1, void *pvStr2, uint32_t u32DataLen);

/** 
*
* DESCRIPTION
* Append number of source string bytes to destination string.
* Returns 0 if string is not appended else number of bytes appended.
*
* @param pvDest		[in]  Destination adderss
* @param pvSrc		[in]  Source address
* @param u32DataLen	[in]  Amount of data to append
*
* @return [out] length of string appended.
*
*/
uint32_t Strncat(void *pvDest, void *pvSrc, uint32_t u32DataLen);

/** 
*
* DESCRIPTION
* Verify is string 2 is subset of string 1.
* Returns a pointer to the first occurrence of str2 in str1, 
* or a null pointer if str2 is not part of str1.
*
* @param pvStr1	[in]  String 1 address
* @param pvStr2	[in]  string 2 address
*
* @return [out] pointer to first occurence of str2 in in str1
*
*/
uint8_t *StrStr(void *pvStr1, void *pvStr2);

#endif /* MISC_H */
