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
*	miscMiscellaneous.c
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

/** header files */
#include <stddef.h>
#include <string.h>
#include "osalFreeRTOS.h"
#include "miscMiscellaneous.h"
#include "bacDELStackConfig.h"


/* global variable to save hardware endian-ness */
//uint8_t g_u8EndianNess = BACDEL_LITTLE_ENDIAN;    //TODO


/** 
*
* DESCRIPTION
* Function to swap bytes for signed / unsigned short integer value.
*  
* @param  u16Value [in] Value to be swapped
* @return u16NewValue [out] Swapped value
*
*/
uint16_t Swap_Unsigned16_Bytes(uint16_t u16Value)
{
	/* local variables */
    uint16_t u16NewValue = 0;

	/* swap bytes */
    u16NewValue = (u16Value & 0xFF00);
    u16NewValue = u16NewValue >> 8;
    u16Value = u16Value << 8;
    u16NewValue = u16NewValue | u16Value;
	
	/* return result */
    return u16NewValue;
}

uint32_t Swap_Unsigned32_Bytes(uint32_t u32Value)
{
	/* local variables */
	uint32_t u32NewValue = 0;

	u32NewValue = ((u32Value & 0x000000FF) << 24) |
      ((u32Value & 0x0000FF00) <<  8) |
      ((u32Value & 0x00FF0000) >>  8) |
      ((u32Value & 0xFF000000) >> 24);

  return u32NewValue;
}

/**
*
* DESCRIPTION                                                                          
* Function to reverse bits of a byte.
*    
* @param u8InByte   [in]  Input byte value
* @return u8OutByte [out] Output byte value
*
*/
uint8_t Byte_Reverse_Bits(uint8_t u8InByte)
{
	/* local variables */
    uint8_t u8OutByte = 0;

    if (u8InByte & BIT0) 
    {
        u8OutByte |= BIT7;
    }
    if (u8InByte & BIT1) 
    {
        u8OutByte |= BIT6;
    }
    if (u8InByte & BIT2) 
    {
        u8OutByte |= BIT5;
    }
    if (u8InByte & BIT3) 
    {
        u8OutByte |= BIT4;
    }
    if (u8InByte & BIT4) 
    {
        u8OutByte |= BIT3;
    }
    if (u8InByte & BIT5) 
    {
        u8OutByte |= BIT2;
    }
    if (u8InByte & BIT6) 
    {
        u8OutByte |= BIT1;
    }
    if (u8InByte & BIT7) 
    {
        u8OutByte |= BIT0;
    }
	
    return u8OutByte;
}

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
void Copy_String(uint8_t *pu8Destination, uint8_t *pu8Source, uint32_t u32NumChar)
{
	/* local variables */
    uint32_t u32Count = 0;

	/* function entry */
	/* copy the data */
    for(u32Count = 0; u32Count < u32NumChar; u32Count++)
    {
        *(pu8Destination + u32Count) = *pu8Source;
        pu8Source++;
    }

	/* terminate with null character */
    *(pu8Destination + u32Count) = 0x00;

	/* function exit */

}

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
	uint32_t u32DataLen, uint32_t u32MaxDestSize)
{
	/* local variables */
    uint32_t u32CopyLen = 0;
    int8_t *pi8TmpDst = NULL;
    int8_t *pi8TmpSrc = NULL;

	/* function entry */


	/* get the pointers */
    pi8TmpDst = pvDest;
    pi8TmpSrc = pvSrc;

	/* check destination buffer size */
    if (u32DataLen <= (u32MaxDestSize - u32offset)) 
    {
		/* copy data */
        for (u32CopyLen = 0; u32CopyLen < u32DataLen; u32CopyLen++) 
        {
			/* copy from specified offset address */
            pi8TmpDst[u32offset + u32CopyLen] = pi8TmpSrc[u32CopyLen];
        }
    }

	/* function exit */

    return u32CopyLen;
}

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
    uint32_t u32IncreaseDestBy)
{
	/* local variables */
    uint32_t u32CopyLen = 0;
    int8_t *pi8TmpDst = NULL;
    int8_t *pi8TmpSrc = NULL;
	uint8_t *pi8Memory = NULL;

	/* function entry */

	/* calculate the buffer increase size */
	if(u32IncreaseDestBy < u32DataLen)
	{
		/* if data length is more, increase buffer as per data length */
        u32IncreaseDestBy = u32DataLen;
	}

	/* get the pointers */
    pi8TmpDst = *pvDest;
    pi8TmpSrc = pvSrc;

	/* check for null input pointers */
	if(NULL == pi8TmpDst || NULL == pi8TmpSrc)
	{
		/* null input pointers */

		return 0;
	}	

	/* copy data if destinaton can accomodate data of length u32DataLen */
    if (u32DataLen <= (*pu32MaxDestSize - u32offset)) 
    {
		/* copy data */
        for (u32CopyLen = 0; u32CopyLen < u32DataLen; u32CopyLen++) 
		{
            pi8TmpDst[u32offset + u32CopyLen] = pi8TmpSrc[u32CopyLen];
		}
    }
	else
	{
		/* increase max destination size by u32increasedestby upto max limit */
		*pu32MaxDestSize = *pu32MaxDestSize + u32IncreaseDestBy;
		/* check if this is not max destination size */
		if(*pu32MaxDestSize > 1476 /*MAX_APDU_BUFFER*/)  //TODO
		{
			return 0;
		}

		/* re-allocate new memory */
		pi8Memory = (uint8_t *)OSAL_Realloc(pi8TmpDst, *pu32MaxDestSize,
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pi8Memory)
		{
			/* re-alloc failed */

			return 0;
		}

		/* init the reallocated memory */
		memset(&pi8Memory[*pu32MaxDestSize - u32IncreaseDestBy], 0, u32IncreaseDestBy);
		/* update the destination address with new memory address */
		*pvDest = pi8Memory;
		pi8TmpDst = (int8_t *)pi8Memory;

		/* copy remaining data */
		for (u32CopyLen = 0; u32CopyLen < u32DataLen; u32CopyLen++) 
		{
            pi8TmpDst[u32offset + u32CopyLen] = pi8TmpSrc[u32CopyLen];
		}
	}

    /* function exit */

    return u32CopyLen;
}

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
uint32_t Strcopy(void *pvDest, void *pvSrc, uint32_t u32MaxDestSize)
{
	/* local variables */
    uint32_t u32DataLen = 0;
    int8_t *pi8TmpDst = NULL;
    int8_t *pi8TmpSrc = NULL;

	/* function entry */

	/* get the pointers */
    pi8TmpDst = pvDest;
    pi8TmpSrc = pvSrc;

	/* check input pointers */
    if((pvDest != NULL) && (pvSrc!= NULL))
    {
		/* get length of source string */
		u32DataLen = Strnlen(pi8TmpSrc, u32MaxDestSize);

		/* If data to copy is greater then buffer size then truncate 
		   the string to be copied */
        if(u32DataLen > u32MaxDestSize)
		{
			/* select lesser length */
            u32DataLen = u32MaxDestSize;
		}

		/* copy string */
        strncpy((char *)pi8TmpDst, (const char *)pi8TmpSrc, 
			(size_t)u32DataLen);
    }
	else if(pvDest != NULL)
	{
		/* save null character */
		pi8TmpDst[0] = 0;
	}

	/* function exit */

    return u32DataLen;
}

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
int32_t Strncmp(void *pvStr1, void *pvStr2, uint32_t u32DataLen)
{
	/* local variables */
    int8_t *pi8TmpDst = NULL;
    int8_t *pi8TmpSrc = NULL;
	int32_t i32RetVal = 0;

	/* function entry */
	/* get the pointers */
    pi8TmpDst = pvStr1;
    pi8TmpSrc = pvStr2;

	/* check input pointers */
    if((pvStr1 != NULL) && (pvStr2 != NULL))
    {
		/* compare strings */
        i32RetVal = strncmp((const char *)pi8TmpDst, (const char *)pi8TmpSrc, 
			(size_t)u32DataLen);
    }

	/* function exit */

    return i32RetVal;
}

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
uint32_t Strnlen(void *pvString, uint32_t u32MaxStrLen)
{
	/* local variables */
    int8_t *pi8TmpSrc = NULL;
    uint32_t u32DataLen = 0;

	/* function entry */

	/* get string pointer */
	pi8TmpSrc = pvString;

	/* check input pointer */
    if(pvString != NULL)
    {
		/* get string length */
			u32DataLen = (uint32_t)strlen((const char *)pi8TmpSrc);
    }

	/* function exit */
    return u32DataLen;
}
/**************************** end of miscMiscellaneous.c file *******************************/
