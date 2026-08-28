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
*   SoftDEL Systems Ltd.                     india@softdel.com
*   3rd Floor, Pentagon P4,                  http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - pduEncodeDecodeDT.c
*
*   AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION                                                            
*   This file includes encoding and decoding of all property data-types 
*	supported by stack.
*
*********************************************************************************/

/** header files */
#include "pduEncodeDecodeDT.h"
#include "pduEncodeDecode.h"
#include "pduDataType.h"
#include "pduDataEncodeDecode.h"
#include "miscMiscellaneous.h"
#include "propertyClearValues.h"
#include "serviceGenericHandler.h"

/** to include event notification encoder */
#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)
#include "serviceEventNotification_B.h"
#endif

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode date-time data type.
*    
* @param pu8APDU [in]	data to be decoded.
* @param pstDate [out]	pointer to save date value.
* @param pstTime [out]	pointer to save time value.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DateTime(
	uint8_t *pu8Apdu, 
	BACnetDate_t *pstDate, 
	BACnetTime_t *pstTime)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstDate || NULL == pstTime)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_DATE != u8TagNo || u32LenValueType != 4)
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* decode date */
	i32DecodeLen = Decode_Date(&pu8Apdu[i32Len], pstDate);
	i32Len += i32DecodeLen;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_TIME != u8TagNo || u32LenValueType != 4)
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* decode time */
	i32DecodeLen = Decode_Time(&pu8Apdu[i32Len], pstTime);
	i32Len += i32DecodeLen;

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode time-stamp data type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param puTimeStamp	 [out]	pointer to save decoded data.
* @param peTimeStampType [out]	pointer to save time stamp type.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_TimeStamp(
	uint8_t *pu8Apdu,
	timeStamp_u *puTimeStamp, 
	BACNET_TIMESTAMP_TYPE *peTimeStampType)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puTimeStamp || NULL == peTimeStampType)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check type of time-stamp value */
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[0], TAG_NO_2))
	{
		/* time-stamp is date-time */
		*peTimeStampType = TIMESTAMP_TYPE_DATETIME;
		/* increment by decoded data length */
		/* decode date-time value */
		i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len], 
			&puTimeStamp->m_stDateTime.m_stDate,
			&puTimeStamp->m_stDateTime.m_stTime);
		/* check decode length */
		if(i32DecodeLen < 0)
		{
			/* return the error code */
			i32Len = i32DecodeLen;
			return i32Len;
		}
		else
		{
			/* increment by decoded data length */
			i32Len += i32DecodeLen;
		}
		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
	}
	else
	{
		/* check context tag value */
		if(Decode_Is_Context_Tag(&pu8Apdu[0], TAG_NO_0) == TRUE)
		{
			/* time-stamp is time */
			*peTimeStampType = TIMESTAMP_TYPE_TIME;
			/* decode the application tag */
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode time */
			i32DecodeLen = Decode_Time(&pu8Apdu[i32Len], &puTimeStamp->m_stTime);
			i32Len += i32DecodeLen;
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[0], TAG_NO_1) == TRUE)
		{
			/* time-stamp is sequence no. */
			*peTimeStampType = TIMESTAMP_TYPE_SEQUENCE_NO;
			/* decode the application tag */
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode sequence number */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, &puTimeStamp->m_u32SeqNo);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* error case */
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}
	}

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array-of-object-id data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstObjIdList [out]	pointer to save obj-id list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_ObjectID_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfObjId_t **pstObjIdList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32ObjId = 0;
	uint32_t u32ObjType = 0;
	ListOfObjId_t *pstBaseNode = NULL;
	ListOfObjId_t *pstNewNode = NULL;
	ListOfObjId_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstObjIdList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstObjIdList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_OBJECT_ID != u8TagNo || u32LenValueType != 4)
		{
			/* free previously allocated memories & return error */
			Clear_ObjectID_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], &u32ObjType, &u32ObjId);
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfObjId_t *)OSAL_Malloc(sizeof(ListOfObjId_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_ObjectID_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		pstNewNode->m_stObjectId.m_eObjectType = (BACNET_OBJECT_TYPE)u32ObjType;
		pstNewNode->m_stObjectId.m_u32ObjId = u32ObjId;
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstObjIdList = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array-of-bool data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstBoolList  [out]	pointer to save bool values list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Boolean_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBoolen_t **pstBoolList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfBoolen_t *pstBaseNode = NULL;
	ListOfBoolen_t *pstNewNode = NULL;
	ListOfBoolen_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstBoolList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstBoolList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag & boolean value */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BOOLEAN != u8TagNo || u32LenValueType > 1)
		{
			/* free previously allocated memories & return error */
			Clear_Boolean_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* allocate memory & save value */
		pstNewNode = (ListOfBoolen_t *)OSAL_Malloc(sizeof(ListOfBoolen_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_Boolean_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		pstNewNode->m_bVal = (bool)u32LenValueType;
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstBoolList = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode address data type.
*    
* @param pu8APDU	[in]	data to be decoded.
* @param pstAddress	[out]	pointer to save address.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Address(
	uint8_t *pu8Apdu, 
	BACnetAddress_t *pstAddress)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACnetOctetStr_t stOctetStr = {0};
	uint32_t u32NwNo = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAddress)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo || u32LenValueType < 1 || u32LenValueType > 2)
	{
		return DT_ERR_INVALID_TAG;
	}

	/* decode network number */
	i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, &u32NwNo);
	i32Len += i32DecodeLen;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_OCTET_STRING != u8TagNo || u32LenValueType > MAX_MAC_LEN)
	{
		return DT_ERR_INVALID_TAG;
	}

	/* decode mac address */
	i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len], u32LenValueType, &stOctetStr);
	i32Len += i32DecodeLen;
	/* copy */
	memcpy(&pstAddress->u8IpAddrs[0], &stOctetStr.m_ai8OctetStr[0], MAX_MAC_LEN);
	pstAddress->u8mac_len = (uint8_t)(stOctetStr.m_u32OctetCount);
	pstAddress->u16net = (uint16_t)u32NwNo;

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Address-Binding data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstAddrBindList [out]	pointer to save address-binding list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AddressBinding(
	uint8_t *pu8Apdu, 
	BACnetAddrBinding_t *pstAddrBind)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32ObjId = 0;
	uint32_t u32ObjType = 0;
	BACnetAddrBinding_t *pstNewNode = NULL;
	BACnetAddress_t stAddress = {0};

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAddrBind)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	/* 1st node */
	pstNewNode = pstAddrBind;
	pstAddrBind->pstNext = NULL;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_OBJECT_ID != u8TagNo || u32LenValueType != 4)
	{
		/* free previously allocated memories & return error */
		Clear_AddressBinding_List(&pstAddrBind->pstNext, false);
		return DT_ERR_INVALID_TAG;
	}

	/* decode object id */
	i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], &u32ObjType, &u32ObjId);
	i32Len += i32DecodeLen;

	/* decode address */
	i32DecodeLen = DT_Decode_Address(&pu8Apdu[i32Len], &stAddress);
	if(i32DecodeLen < 0)
	{
		/* free previously allocated memories & return error */
		Clear_AddressBinding_List(&pstAddrBind->pstNext, false);
		return i32DecodeLen;
	}
	i32Len += i32DecodeLen;

	/* save decoded data */
	pstNewNode->m_eObjectType = (BACNET_OBJECT_TYPE)u32ObjType;
	pstNewNode->m_u32ObjId = u32ObjId;
	memcpy(&pstNewNode->m_stAddress, &stAddress, sizeof(BACnetAddress_t));
	pstNewNode->pstNext = NULL;

	/* return total decoded data & length */
	return i32Len;
}



/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array-of-object-id data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstAddrBindList [out]	pointer to save address-binding list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AddressBinding_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	BACnetAddrBinding_t *pstAddrBindList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32ObjId = 0;
	uint32_t u32ObjType = 0;
	BACnetAddrBinding_t *pstNewNode = NULL;
	BACnetAddress_t stAddress = {0};
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAddrBindList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	/* set default values */
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* 1st node */
	pstNewNode = pstAddrBindList;
	pstAddrBindList->pstNext = NULL;

	/* decode list of values */
	while(NULL != pstNewNode)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_OBJECT_ID != u8TagNo || u32LenValueType != 4)
		{
			/* free previously allocated memories & return error */
			Clear_AddressBinding_List(&pstAddrBindList->pstNext, false);
			return DT_ERR_INVALID_TAG;
		}

		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], &u32ObjType, &u32ObjId);
		i32Len += i32DecodeLen;

		/* decode address */
		i32DecodeLen = DT_Decode_Address(&pu8Apdu[i32Len], &stAddress);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_AddressBinding_List(&pstAddrBindList->pstNext, false);
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		pstNewNode->m_eObjectType = (BACNET_OBJECT_TYPE)u32ObjType;
		pstNewNode->m_u32ObjId = u32ObjId;
		memcpy(&pstNewNode->m_stAddress, &stAddress, sizeof(BACnetAddress_t));
		pstNewNode->pstNext = NULL;

		/* increment count */
		u32Count++;

		/* allocate memory is more data is available to decode */
		if((u32ApduLen - i32Len) > 0)
		{
			/* allocate memory & save value */
			pstNewNode->pstNext = (BACnetAddrBinding_t *)OSAL_Malloc(sizeof(BACnetAddrBinding_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstNewNode->pstNext)
			{
				/* free previously allocated memories & return error */
				Clear_AddressBinding_List(&pstAddrBindList->pstNext, false);
				return DT_ERR_MALLOC_FAILED;
			}
			/* move to new node */
			pstNewNode = pstNewNode->pstNext;
			pstNewNode->pstNext = NULL;
		}
		else
		{
			/* break the loop */
			pstNewNode = NULL;
		}
	}

	/* return total decoded data & length */
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of VT-Class data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstVtClassList [out]	pointer to save vt-class list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_VtClass_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	Pr_BACnetVTClass_t **pstVtClassList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	Pr_BACnetVTClass_t *pstBaseNode = NULL;
	Pr_BACnetVTClass_t *pstNewNode = NULL;
	Pr_BACnetVTClass_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstVtClassList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstVtClassList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_ENUMERATED != u8TagNo || u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* free previously allocated memories & return error */
			Clear_VtClass_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* decode vt class */
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, &u32Value);
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (Pr_BACnetVTClass_t *)OSAL_Malloc(sizeof(Pr_BACnetVTClass_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_VtClass_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		pstNewNode->m_eVTClassSupport = (BACNET_VT_CLASS)u32Value;
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstVtClassList = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of VT-session data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstVtSessionList [out]	pointer to save vt-session list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_VtSession_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetVTSession_t **pstVtSessionList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	ListOfBACnetVTSession_t *pstBaseNode = NULL;
	ListOfBACnetVTSession_t *pstNewNode = NULL;
	ListOfBACnetVTSession_t *pstPreviousNode = NULL;
	BACnetVTSession_t stVtData = {0};
	bool bFlag = FALSE;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstVtSessionList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstVtSessionList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo || u32LenValueType != 1)
		{
			/* free previously allocated memories & return error */
			Clear_VtSessions_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* decode local vt session id */
		i32DecodeLen = Decode_Unsigned8(&pu8Apdu[i32Len], &u32Value);
		i32Len += i32DecodeLen;
		stVtData.m_u8LocalVTSessionId = (uint8_t)u32Value;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo || u32LenValueType != 1)
		{
			/* free previously allocated memories & return error */
			Clear_VtSessions_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* decode remote vt session id */
		i32DecodeLen = Decode_Unsigned8(&pu8Apdu[i32Len], &u32Value);
		i32Len += i32DecodeLen;
		stVtData.m_u8RemoteVTSessionId = (uint8_t)u32Value;

		/* decode address */
		i32DecodeLen = DT_Decode_Address(&pu8Apdu[i32Len], &stVtData.m_stRemoteVTAddress);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_VtSessions_List(&pstBaseNode);
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetVTSession_t *)OSAL_Malloc(sizeof(ListOfBACnetVTSession_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_VtSessions_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		memcpy(&pstNewNode->m_stVTSession, &stVtData, sizeof(BACnetVTSession_t));
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstVtSessionList = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode recipient data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstRecipient	[out]	pointer to save recipient data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Recipient(
	uint8_t *pu8Apdu, 
	BACnetRecipient_t *pstRecipient)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstRecipient)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstRecipient, 0x00, sizeof(BACnetRecipient_t));

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstRecipient->BACnetRecipient_u.m_stObjId.m_eObjectType,
			&pstRecipient->BACnetRecipient_u.m_stObjId.m_u32ObjId);
		i32Len += i32DecodeLen;
		/* set recipient type */
		pstRecipient->m_eDestinationType = DESTINATION_IS_DEVICE_ID;
	}
	else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{
		i32Len += i32DecodeLen;
		/* decode address */
		i32DecodeLen = DT_Decode_Address(&pu8Apdu[i32Len], &pstRecipient->
			BACnetRecipient_u.m_stAddress);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;
		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++;
		}
		else
		{
			return DT_ERR_INVALID_TAG;
		}
		/* set recipient type */
		pstRecipient->m_eDestinationType = DESTINATION_IS_IP_ADDR;
	}
	else
	{
		/* set recipient type */
		pstRecipient->m_eDestinationType = DESTINATION_IS_UNRECOGNIZED;
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data & length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of list of recipients data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstRecipientList [out]	pointer to save recipient list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Recipient_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetRecipient_t **pstRecipientList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetRecipient_t *pstBaseNode = NULL;
	ListOfBACnetRecipient_t *pstNewNode = NULL;
	ListOfBACnetRecipient_t *pstPreviousNode = NULL;
	BACnetRecipient_t stRecipient = {0};
	bool bFlag = FALSE;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstRecipientList || NULL == pu32Count 
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstRecipientList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* */
		i32DecodeLen = DT_Decode_Recipient(&pu8Apdu[i32Len], &stRecipient);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_Recipient_List(&pstBaseNode);
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetRecipient_t *)OSAL_Malloc(sizeof(ListOfBACnetRecipient_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_Recipient_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		memcpy(&pstNewNode->m_stRecipient, &stRecipient, sizeof(BACnetRecipient_t));
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstRecipientList = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode recipient process data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstRecipient	[out]	pointer to save recipient data.
* @param pu32ProcessId	[out]	pointer to process id.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_RecipientProcess(
	uint8_t *pu8Apdu, 
	BACnetRecipient_t *pstRecipient,
	uint32_t *pu32ProcessId)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstRecipient || NULL == pu32ProcessId)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* check opening tag */
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
	{
		i32Len++; // tag decoded
		/* decode the recipient */
		i32DecodeLen = DT_Decode_Recipient(&pu8Apdu[i32Len], pstRecipient);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // recipient decoded

		/* check opening tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len++; // tag decoded
		}
		else
		{
			/* invalid closing tag */
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}

		/* decode process id */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, pu32ProcessId);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data & length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode object property reference data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstObjPropRef	[out]	pointer to save recipient data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_ObjPropReff(
	uint8_t *pu8Apdu, 
	BACnetObjPropRef_t *pstObjPropRef)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstObjPropRef)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstObjPropRef->m_eObjectType, &pstObjPropRef->m_u32ObjId);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode property id */
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t*)&pstObjPropRef->m_ePropertyIdentifier);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* check if array index is present or not */
	pstObjPropRef->m_bArrIndxPresent = ARRAY_INDEX_ABSENT;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode array index */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstObjPropRef->m_u32ArrayIndex);
		i32Len += i32DecodeLen;
		/* set array index flag */
		pstObjPropRef->m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
	}

	/* return total decoded data & length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode cov-subscription data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstObjPropRef	[out]	pointer to save cov subscription data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_CovSubscription(
	uint8_t *pu8Apdu, 
	ListOfBACnetCovSubs_t *pstCovSubscription)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	BACnetObjPropRef_t stObjPropRef = {0};
	BACnetRecipient_t stRecipient = {0};
	CovRecipientInfo_t *pstTempInfo = NULL;
	uint32_t u32ProcessId = 0;
	uint32_t u32TimeRemaining = 0;
	Float_t fCovIncr = 0.0;
	bool bBoolValue = FALSE;
	bool bCovFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstCovSubscription)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	if(NULL == pstCovSubscription->m_pstSubscriberInfo)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* save pointer locally */
	pstTempInfo = pstCovSubscription->m_pstSubscriberInfo;
	/* set default values */
	memset(pstCovSubscription, 0x00, sizeof(ListOfBACnetCovSubs_t));
	/* restore pointer value */
	pstCovSubscription->m_pstSubscriberInfo = pstTempInfo;

	/* decode recipient */
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
	{
		i32Len++; // tag decoded
		i32DecodeLen = DT_Decode_RecipientProcess(&pu8Apdu[i32Len], 
			&stRecipient, &u32ProcessId);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // recipient process decoded 
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len++; // tag decoded
		}
		else
		{
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode monitored property reference */
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{
		i32Len++; // tag decoded
		i32DecodeLen = DT_Decode_ObjPropReff(&pu8Apdu[i32Len], &stObjPropRef);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // data decoded 
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++; // tag decoded
		}
		else
		{
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode issue confirmed notification flag */
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 1)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode boolean */
		bBoolValue = Decode_Boolean(pu8Apdu[i32Len]);
		i32Len++; // value decoded
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode time remaining */
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode boolean */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, &u32TimeRemaining);
		i32Len += i32DecodeLen; // value decoded
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode cov increment - optional parameter */
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_4) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode boolean */
		i32DecodeLen = Decode_Real(&pu8Apdu[i32Len], &fCovIncr);
		i32Len += i32DecodeLen; // value decoded
		bCovFlag = TRUE;
	}

	/* copy decoded values */
	pstCovSubscription->m_stObjId.m_eObjectType = stObjPropRef.m_eObjectType;
	pstCovSubscription->m_stObjId.m_u32ObjId = stObjPropRef.m_u32ObjId;
	pstCovSubscription->m_stCOVPropertyElem.m_ePropertyId = stObjPropRef.m_ePropertyIdentifier;
	pstCovSubscription->m_stCOVPropertyElem.m_u32PropertyArrayIndex = stObjPropRef.m_u32ArrayIndex;
	pstCovSubscription->m_stCOVPropertyElem.m_bArrIndxresent = stObjPropRef.m_bArrIndxPresent;
	pstCovSubscription->m_pstSubscriberInfo->m_u32ProcessId = u32ProcessId;
	pstCovSubscription->m_pstSubscriberInfo->m_bIssueConfirmedNotification = bBoolValue;
	pstCovSubscription->m_pstSubscriberInfo->m_u32TimeRemaining = u32TimeRemaining;
	//pstCovSubscription->m_pstSubscriberInfo->m_fCOVIncrement = fCovIncr;
	pstCovSubscription->m_pstSubscriberInfo->m_stPropertyValue.uValue.m_Real = fCovIncr;
	if(bCovFlag)
		pstCovSubscription->m_stCOVPropertyElem.m_eData_Type = BACNET_DT_REAL;
	else
		pstCovSubscription->m_stCOVPropertyElem.m_eData_Type = BACNET_DT_NULL;
	/* copy recipient value */
	/* if destination is address copy address */
	if(DESTINATION_IS_IP_ADDR == stRecipient.m_eDestinationType)
	{
		memcpy(&pstCovSubscription->m_pstSubscriberInfo->m_stAddress, 
			&stRecipient.BACnetRecipient_u.m_stAddress, sizeof(BACnetAddress_t));
	}
	else
	{
		pstCovSubscription->m_pstSubscriberInfo->m_stObjId.m_eObjectType = 
			stRecipient.BACnetRecipient_u.m_stObjId.m_eObjectType;
		pstCovSubscription->m_pstSubscriberInfo->m_stObjId.m_u32ObjId = 
			stRecipient.BACnetRecipient_u.m_stObjId.m_u32ObjId;
	}

	/* return total decoded data & length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of cov-subscription data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param u32ApduLen		[in]	length of data to be decoded.
* @param pstCovSubscription	[out]	pointer to save cov subscription data.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_CovSubscription_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetCovSubs_t **pstCovSubscription, 
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetCovSubs_t *pstBaseNode = NULL;
	ListOfBACnetCovSubs_t *pstNewNode = NULL;
	ListOfBACnetCovSubs_t *pstPreviousNode = NULL;
	ListOfBACnetCovSubs_t stCovSub = {0};
	CovRecipientInfo_t stSubsInfo = {0};
	CovRecipientInfo_t *pstSubscriberInfo = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstCovSubscription || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstCovSubscription = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* decode cov subscription value */
		stCovSub.m_pstSubscriberInfo = &stSubsInfo;
		i32DecodeLen = DT_Decode_CovSubscription(&pu8Apdu[i32Len], &stCovSub);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_ActiveCovSubs_List(&pstBaseNode);	
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetCovSubs_t *)OSAL_Malloc(sizeof(ListOfBACnetCovSubs_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_ActiveCovSubs_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		/* allocate memory for subscriber info */
		pstSubscriberInfo = (CovRecipientInfo_t *)OSAL_Malloc(sizeof(CovRecipientInfo_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstSubscriberInfo)
		{
			/* free previously allocated memories & return error */
			Clear_ActiveCovSubs_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		memcpy(pstNewNode, &stCovSub, sizeof(ListOfBACnetCovSubs_t));
		memcpy(pstSubscriberInfo, &stSubsInfo, sizeof(CovRecipientInfo_t));
		pstNewNode->m_pstNext = NULL;
		pstNewNode->m_pstSubscriberInfo = pstSubscriberInfo;
		pstSubscriberInfo->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstCovSubscription = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of time-stamp data type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [in]	length of data to be decoded.
* @param pstTimeStamp	 [out]	pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_TimeStamp_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetEventTimeStamp_t *pstTimeStamp)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8IndexCount = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstTimeStamp)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0 && u8IndexCount < BACNET_ARRAY_OF_THREE)
	{
		/* decode time stamp value */
		i32DecodeLen = DT_Decode_TimeStamp(&pu8Apdu[i32Len], 
			&pstTimeStamp->m_uEvntStamp[u8IndexCount], 
			&pstTimeStamp->m_eTimeStampType[u8IndexCount]);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* increment the indec count */
		u8IndexCount++;
	}

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode priority array value as per tag type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param pbTagType		 [out]	to save the data type.
* @param pstPropValue	 [out]	pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_PriorityValue(
	uint8_t *pu8Apdu,
	bool_t *pbTagType,
	PropertyValue_u *pstPropValue)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACNET_PROPERTY_VALUE stPropVal = {0};

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstPropValue || NULL == pbTagType)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decod the tag type */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_DATE != u8TagNo || u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode date */
		i32DecodeLen = Decode_Date(&pu8Apdu[i32Len], &pstPropValue->DateTimeVal.m_stDate);
		i32Len += i32DecodeLen; // value decoded

		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_TIME != u8TagNo || u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode time */
		i32DecodeLen = Decode_Time(&pu8Apdu[i32Len], &pstPropValue->DateTimeVal.m_stTime);
		i32Len += i32DecodeLen; // value decoded

		/* check closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len++; // tag decoded
		}
		else
		{
			return DT_ERR_INVALID_TAG;
		}

		/* set the data type */
		*pbTagType = BACNET_APPLICATION_TAG_DATETIME;
	}

	else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		return DT_ERR_DATA_TYPE_NOT_SUPPORTED;
	}

	else
	{
		/* check if application tag value less than 12 */
		if(u8TagNo <= BACNET_APPLICATION_TAG_OBJECT_ID)
		{
			if(u8TagNo != BACNET_APPLICATION_TAG_NULL)
				i32Len += i32DecodeLen;
			/* decode value */
			stPropVal.m_TagType = u8TagNo;
			i32DecodeLen = BACApp_Decode_Data(&pu8Apdu[i32Len], u8TagNo, 
				u32LenValueType, &stPropVal);
			if(i32DecodeLen < 0)
			{
				return DT_ERR_INVALID_DATA_TYPE;
			}
			if(u8TagNo == BACNET_APPLICATION_TAG_NULL)
				i32Len++;
			else
			i32Len += i32DecodeLen;

			/* get the smallest size structure & use as copy length */
			i32DecodeLen = sizeof(PropertyValue_u) <= sizeof(stPropVal.uValue) ? 
				sizeof(PropertyValue_u) : sizeof(stPropVal.uValue);
			/* copy property value */
			memcpy(pstPropValue, &stPropVal.uValue, i32DecodeLen);
			/* set the application tag type */
			*pbTagType = u8TagNo;
		}
	}

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode priority array data type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [in]	length of data to be decoded.
* @param pstPriorityArray	 [out]	pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Priority_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetPriorityArray_t *pstPriorityArray)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8IndexCount = 0;
	PropertyValue_u stPropValue = {0};

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstPriorityArray)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0 && u8IndexCount < MAX_PRIORITY_ARRAY_COUNT)
	{
		/* decode value */
		i32DecodeLen = DT_Decode_PriorityValue(&pu8Apdu[i32Len],
			&pstPriorityArray->m_bTagType[u8IndexCount], &stPropValue);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // value decoded

		/* copy as per tag type */
		switch(pstPriorityArray->m_bTagType[u8IndexCount])
		{
		case BACNET_APPLICATION_TAG_SIGNED_INT:
		case BACNET_APPLICATION_TAG_UNSIGNED_INT:
		case BACNET_APPLICATION_TAG_ENUMERATED:
			/* copy unsigned integer */
			pstPriorityArray->m_uValue.m_u32Val[u8IndexCount] = stPropValue.u32Val;
			break;

		case BACNET_APPLICATION_TAG_REAL:
			/* copy real */
			pstPriorityArray->m_uValue.m_fVal[u8IndexCount] = stPropValue.fVal;
			break;

		case BACNET_APPLICATION_TAG_DOUBLE:
			/* copy double */
			break;

		case BACNET_APPLICATION_TAG_BOOLEAN:
			/* copy boolean */
			pstPriorityArray->m_uValue.m_bVal[u8IndexCount] = stPropValue.bVal;
			break;

		case BACNET_APPLICATION_TAG_DATE:
			/* copy date */
			break;

		case BACNET_APPLICATION_TAG_TIME:
			/* copy time */
			break;

		case BACNET_APPLICATION_TAG_CHARACTER_STRING:
			/* copy character string */
			break;

		case BACNET_APPLICATION_TAG_BIT_STRING:
			/* copy bit string */
			break;

		case BACNET_APPLICATION_TAG_OCTET_STRING:
			/* copy octet string */
			break;

		case BACNET_APPLICATION_TAG_DATETIME:
			/* copy date time */
			break;

		case BACNET_APPLICATION_TAG_NULL:
		default:
			/* copy nothing */
			break;
		}// switch ends

		/* increment the index count */
		u8IndexCount++;
	}

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list-of-unsigned data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstUnitList  [out]	pointer to save unsigned values list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Unsigned_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfUnsigned_t **pstUnitList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfUnsigned_t *pstBaseNode = NULL;
	ListOfUnsigned_t *pstNewNode = NULL;
	ListOfUnsigned_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;
	uint32_t u32Value = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstUnitList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstUnitList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */		
		(*pu32FirstFailedElement)++;

		/* decode the application tag & unsigned value */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo || u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* free previously allocated memories & return error */
			Clear_UnsignedInt_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* decode unsigned value */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, &u32Value);
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfUnsigned_t *)OSAL_Malloc(sizeof(ListOfUnsigned_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_UnsignedInt_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		pstNewNode->m_u32Value = u32Value;
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstUnitList = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode BACnetClientCOV data type.
*    
* @param pu8APDU	  [in]	data to be decoded. 
* @param pstClientCOV [out]	pointer to save BACnetClientCOV value.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_ClientCOVIncrement(
	uint8_t *pu8Apdu,
	BACnetClientCOV_t *pstClientCOV)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstClientCOV)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_REAL == u8TagNo)
	{
		/* set the tag type as real */
		pstClientCOV->m_eAppTagtype = BACNET_APPLICATION_TAG_REAL;
		if(u32LenValueType != 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}		
		/* decode real-increment */
		i32DecodeLen = Decode_Real(&pu8Apdu[i32Len],&pstClientCOV->m_fVal);
		i32Len += i32DecodeLen;	
	}
	else if(BACNET_APPLICATION_TAG_NULL == u8TagNo)
	{
		/* set the tag type as NULL */
		pstClientCOV->m_eAppTagtype = BACNET_APPLICATION_TAG_NULL;
		pstClientCOV->m_fVal = 0;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}
	
	/* return total decoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode BAcnetDateRange data type.
*    
* @param pu8APDU	  [in]	data to be decoded. 
* @param pstDateRange [out]	pointer to save BAcnetDateRange value.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DateRange(
	uint8_t *pu8Apdu,
	BACnetDateRange_t *pstDateRange)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
  
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstDateRange)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
		&u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_DATE != u8TagNo || u32LenValueType != 4)
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}
	/* decode start date */
	i32DecodeLen = Decode_Date(&pu8Apdu[i32Len], 
		&pstDateRange->m_stStartDate);
	i32Len += i32DecodeLen;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
		&u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_DATE != u8TagNo || u32LenValueType != 4)
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}
	/* decode end date */
	i32DecodeLen = Decode_Date(&pu8Apdu[i32Len], 
		&pstDateRange->m_stEndDate);
	i32Len += i32DecodeLen;

	/* return total decoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Bacnet Calender Entry.
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstCalenderEntry	[out]	pointer to save single calender Entry.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_BACnetCalenderEntry(
	uint8_t *pu8Apdu, 
	BACnetCalendarEntry_t *pstCalenderEntry)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;		
	BACnetOctetStr_t stOctetString = {0};	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstCalenderEntry)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
		&u32LenValueType);
	/* decode date */
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;

		/* check the length of date to be 4 bytes only */
		if(u32LenValueType != 4)
		{
			/* invalid tag, return error */
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode date */
		i32DecodeLen = Decode_Date(&pu8Apdu[i32Len],&pstCalenderEntry->m_stCalendar.m_stDate);
		i32Len += i32DecodeLen;

		/* save decoded status of decoded data */
		pstCalenderEntry->m_eStatusCalendar = STATUS_DATE;		
	}
	/* decode daterange */
	else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{	
		i32Len += i32DecodeLen;
		/* decode date range */
		i32DecodeLen = DT_Decode_DateRange(&pu8Apdu[i32Len],
			&pstCalenderEntry->m_stCalendar.m_stDateRange);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */							
			i32Len = i32DecodeLen;
			return i32Len;
		}			
		i32Len += i32DecodeLen;			
		
		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++;
		}
		else
		{
			/* invalid data received */
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* save status of decoded decoded data */
		pstCalenderEntry->m_eStatusCalendar = STATUS_DATE_RANGE;		
	}
	/* decode weekNDay */
	else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2))
	{
		i32Len += i32DecodeLen;
		
		i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len], u32LenValueType, 
			&stOctetString);
		if(i32DecodeLen < 0)
		{
			/* invalid data received */							
			i32Len = i32DecodeLen;
			return i32Len;
		}	
		i32Len += i32DecodeLen;

		/* save status of decoded decoded data */
		pstCalenderEntry->m_eStatusCalendar = STATUS_WEEK_N_DAY;
		pstCalenderEntry->m_stCalendar.m_stWeekNDay.m_eMonth =
			stOctetString.m_ai8OctetStr[0];
		pstCalenderEntry->m_stCalendar.m_stWeekNDay.m_eWeekOfMonth =
			stOctetString.m_ai8OctetStr[1];
		pstCalenderEntry->m_stCalendar.m_stWeekNDay.m_eWeekNDay =
			stOctetString.m_ai8OctetStr[2];		
	}
	else
	{		
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of BACnetCalendarEntry data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstCalenderList [out]	pointer to save address-binding list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_CalenderEntry_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetCalendarEntry_t **pstCalenderList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetCalendarEntry_t *pstNewNode = NULL;
	ListOfBACnetCalendarEntry_t *pstBaseNode = NULL;	
	ListOfBACnetCalendarEntry_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;
	BACnetCalendarEntry_t stCalenderEntry = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstCalenderList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstCalenderList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;
	
	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* decode single node od Bacnet Calender Entry. */
		i32DecodeLen = DT_Decode_BACnetCalenderEntry(&pu8Apdu[i32Len],&stCalenderEntry);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_CalendarEntry_List(&pstBaseNode);					
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetCalendarEntry_t *)OSAL_Malloc(sizeof(ListOfBACnetCalendarEntry_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_CalendarEntry_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		/* save decoded status of decoded data */
		pstNewNode->m_eStatusCalendar = stCalenderEntry.m_eStatusCalendar;
		if(STATUS_DATE == pstNewNode->m_eStatusCalendar)
		{	
			memcpy(&pstNewNode->m_stCalendar.m_stDate, &stCalenderEntry.m_stCalendar.m_stDate, 
				sizeof(BACnetDate_t));						
		}
		else if(STATUS_DATE_RANGE == pstNewNode->m_eStatusCalendar)
		{
			memcpy(&pstNewNode->m_stCalendar.m_stDateRange, 
				&stCalenderEntry.m_stCalendar.m_stDateRange,sizeof(BACnetDateRange_t));							
		}
		else if(STATUS_WEEK_N_DAY == pstNewNode->m_eStatusCalendar)
		{
			memcpy(&pstNewNode->m_stCalendar.m_stWeekNDay, 
				&stCalenderEntry.m_stCalendar.m_stWeekNDay,sizeof(WeekNDay_t));						
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_CalendarEntry_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}/* while */

	/* return total decoded data & length */
	*pstCalenderList = pstBaseNode;
	*pu32Count = u32Count;

	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode device object property reference data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstObjPropRef	[out]	pointer to save recipient data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DevObjPropReff(
	uint8_t *pu8Apdu, 
	BACnetDevObjPropRef_t *pstDevObjPropRef)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstDevObjPropRef)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstDevObjPropRef, 0x00, sizeof(BACnetDevObjPropRef_t));

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstDevObjPropRef->m_eObjectType, &pstDevObjPropRef->m_u32ObjId);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode property id */
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t*)&pstDevObjPropRef->m_ePropertyIdentifier);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* check if array index is present or not */
	pstDevObjPropRef->m_bArrIndxPresent = ARRAY_INDEX_ABSENT;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE
	&& !Decode_Is_Closing_Tag(&pu8Apdu[i32Len]))
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode array index */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstDevObjPropRef->m_u32ArrayIndex);
		i32Len += i32DecodeLen;
		/* set array index flag */
		pstDevObjPropRef->m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
	}

	/* check if device id is present or not */
	pstDevObjPropRef->m_bDeviceIdPresent = false;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstDevObjPropRef->m_eDeviceType, &pstDevObjPropRef->m_u32DeviceInstace);
		i32Len += i32DecodeLen;
		/* set device id flag */
		pstDevObjPropRef->m_bDeviceIdPresent = true;
	
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode set point reference data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstSetPtReff	[out]	pointer to save set point refernce data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_SetPtReff(
	uint8_t *pu8Apdu, 
	BACnetSetpointRef_t *pstSetPtReff)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstSetPtReff)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
	{
		i32Len += 1;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);		
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode object id */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&pstSetPtReff->m_stObjPropRef.m_eObjectType, 
				&pstSetPtReff->m_stObjPropRef.m_u32ObjId);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode property id */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
					(uint32_t*)&pstSetPtReff->m_stObjPropRef.m_ePropertyIdentifier);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* check if array index is present or not */
		pstSetPtReff->m_stObjPropRef.m_bArrIndxPresent = ARRAY_INDEX_ABSENT;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode array index */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				&pstSetPtReff->m_stObjPropRef.m_u32ArrayIndex);
			i32Len += i32DecodeLen;
			/* set array index flag */
			pstSetPtReff->m_stObjPropRef.m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
		}

		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}

		/* set the set point reference data flag */
		pstSetPtReff->m_bDataFlag = TRUE;
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of BACnetDestination data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstRecepientList [out]	pointer to save recipient list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Destination_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetDestination_t **pstRecepientList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBACnetDestination_t *pstNewNode = NULL;
	ListOfBACnetDestination_t *pstBaseNode = NULL;	
	ListOfBACnetDestination_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;
	BACnetRecipient_t stRecipient = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstRecepientList || NULL == pu32Count 
		||NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	
	/* set default value */
	*pstRecepientList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)	
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetDestination_t *)OSAL_Malloc(sizeof(ListOfBACnetDestination_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode validDays */
		i32DecodeLen = Decode_BitString_Small(&pu8Apdu[i32Len], u32LenValueType, 
			&pstNewNode->m_stDaysOfWeek);
		i32Len += i32DecodeLen;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_TIME != u8TagNo || u32LenValueType != 4)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode fromTime */
		i32DecodeLen = Decode_Time(&pu8Apdu[i32Len],&pstNewNode->m_stFromTime);
		i32Len += i32DecodeLen;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_TIME != u8TagNo || u32LenValueType != 4)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode toTime */
		i32DecodeLen = Decode_Time(&pu8Apdu[i32Len],&pstNewNode->m_stToTime);
		i32Len += i32DecodeLen;

		/* decode recipient */
		i32DecodeLen = DT_Decode_Recipient(&pu8Apdu[i32Len], &stRecipient);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		memcpy(&pstNewNode->m_stRecipient,&stRecipient, 
				sizeof(BACnetRecipient_t));

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo || u32LenValueType < 1 
			|| u32LenValueType > 4)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode processIdentifier */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
			&pstNewNode->m_stProcessId);
		i32Len += i32DecodeLen;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BOOLEAN != u8TagNo || 
			(1 != u32LenValueType && 0 != u32LenValueType))			
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode issueConfirmedNotifications flag */
		pstNewNode->m_bIssueConfirmedNotification = Decode_Boolean(u32LenValueType);		

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		i32Len += i32DecodeLen;

		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Destination_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode transitions */
		i32DecodeLen = Decode_BitString_Small(&pu8Apdu[i32Len], u32LenValueType, 
			&pstNewNode->m_stTransitions);
		i32Len += i32DecodeLen;

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}/* while */

	/* return total decoded data & length */
	*pstRecepientList = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list/array of BACnetDeviceObjectPropertyReference data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstDevObjPropReffList [out]	pointer to save recipient list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DevObjPropRef_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetDevObjPropRef_t **pstDevObjPropReffList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetDevObjPropRef_t *pstNewNode = NULL;
	ListOfBACnetDevObjPropRef_t *pstBaseNode = NULL;	
	ListOfBACnetDevObjPropRef_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	BACnetDevObjPropRef_t stDevObjPropRef = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstDevObjPropReffList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstDevObjPropReffList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetDevObjPropRef_t *)
			OSAL_Malloc(sizeof(ListOfBACnetDevObjPropRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_DevObjPropReff_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		i32DecodeLen = DT_Decode_DevObjPropReff(&pu8Apdu[i32Len], &stDevObjPropRef);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_DevObjPropReff_List(&pstBaseNode);				
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		memcpy(&pstNewNode->m_stDevObjPropRef,&stDevObjPropRef, 
				sizeof(BACnetDevObjPropRef_t));

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstDevObjPropReffList = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode BACnet Time-Value data type.
*    
* @param pu8APDU [in]	data to be decoded.
* @param pstTimeValue [out]	pointer to save time-value .
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_TimeValue(
	uint8_t *pu8Apdu, 
	BACnetTimeValue_t *pstTimeValue)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstTimeValue)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}	

	/* set default values */
	memset(pstTimeValue, 0x00, sizeof(BACnetTimeValue_t));

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	/* check the application tag type */
	if(BACNET_APPLICATION_TAG_TIME != u8TagNo || u32LenValueType != 4)
	{
		i32Len = DT_ERR_INVALID_DATA_TYPE;
		return i32Len;
	}
	/* decode time */
	i32DecodeLen = Decode_Time(&pu8Apdu[i32Len], &pstTimeValue->m_stTime);
	i32Len += i32DecodeLen;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	
	switch(u8TagNo)
	{
	    case BACNET_APPLICATION_TAG_REAL:
		{
			i32Len += i32DecodeLen;
			 /* check the length of real to be 4 bytes only */
			if(u32LenValueType != 4)
			{
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode real value */
			i32DecodeLen = Decode_Real(&pu8Apdu[i32Len], 
				&pstTimeValue->m_stPropValue.uValue.m_Real);			
		}
		break;

		case BACNET_APPLICATION_TAG_BOOLEAN:
		{
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode boolean value */
			pstTimeValue->m_stPropValue.uValue.m_Boolean
				= Decode_Boolean(u32LenValueType);					 
		}
		break;
		case BACNET_APPLICATION_TAG_UNSIGNED_INT:
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode unsigned value */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
				&pstTimeValue->m_stPropValue.uValue.m_Unsigned_Int);			
		}
		break;
		case BACNET_APPLICATION_TAG_SIGNED_INT:
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode signed value */
			i32DecodeLen = Decode_Signed(&pu8Apdu[i32Len],u32LenValueType,
				&pstTimeValue->m_stPropValue.uValue.m_Signed_Int);
			
		}
		break;
		case BACNET_APPLICATION_TAG_ENUMERATED:
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode enum value */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
					&pstTimeValue->m_stPropValue.uValue.m_Enumerated);
		}
		break;		
		case BACNET_APPLICATION_TAG_NULL:
		{
		}
		break;
		default:
		{
			/* invalid data received */
			i32Len = DT_ERR_DATA_TYPE_NOT_SUPPORTED;
			return i32Len;
		}
		break;
	}/* switch */

	/* data value is decoded, increment length */
	i32Len += i32DecodeLen;
	/* save tag type */
	pstTimeValue->m_stPropValue.m_TagType = u8TagNo;

	/* return total decoded data length */
	return i32Len;
}

int32_t DT_Decode_TimeValue_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	BACnetTimeValue_t *pstTimeValueList)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	BACnetTimeValue_t *pstNewNode = NULL;
	BACnetTimeValue_t *pstBaseNode = NULL;	
	BACnetTimeValue_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	BACnetTimeValue_t stTimeValue = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstTimeValueList)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}	

	/* set default values */
	memset(pstTimeValueList, 0x00, sizeof(BACnetTimeValue_t));
	pstBaseNode = pstTimeValueList;

	/* decode list of values */
	while(((int32_t)(u32ApduLen - ((uint32_t)i32Len))) > 0)
	{
		i32DecodeLen = DT_Decode_TimeValue(&pu8Apdu[i32Len], &stTimeValue);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_TimeValues_List(&pstBaseNode->m_pstNext);
			pstBaseNode->bIsUsed = FALSE;
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		if(FALSE == bFlag)
		{
			/* only copy data, no need to allocate memory for 1st node */
			pstNewNode = pstTimeValueList;
		}
		else
		{
			/* allocate memory & save value */
			pstNewNode = (BACnetTimeValue_t *)
				OSAL_Malloc(sizeof(BACnetTimeValue_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstNewNode)
			{
				/* free previously allocated memories & return error */
				Clear_TimeValues_List(&pstBaseNode->m_pstNext);
				pstBaseNode->bIsUsed = FALSE;
				return DT_ERR_MALLOC_FAILED;
			}
		}

		stTimeValue.bIsUsed = true;
		/* save decoded data */
		memcpy(pstNewNode,&stTimeValue,	sizeof(BACnetTimeValue_t));

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	return i32Len;
}

#ifndef PROFILE_BOD_ONLY
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetDailySchedule data type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [in]	length of data to be decoded.
* @param pstTimeStamp	 [out]	pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DailySchedule_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_ListOfBACnetDailySchedule_t *pstDailySchedule, 
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint8_t u8IndexCount = 0;
	BACnetTimeValue_t stBACnetTimeValue = {0};
	uint32_t u32TimeValueLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstDailySchedule)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0 && u8IndexCount < BACNET_ARRAY_OF_SEVEN)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			//i32Len += i32DecodeLen;			
			u32TimeValueLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], u32ApduLen - i32Len,
                    ePropID);
			
			i32Len += i32DecodeLen;
			/* decode time value */			
			i32DecodeLen = DT_Decode_TimeValue_List(&pu8Apdu[i32Len],u32TimeValueLen, 
					&stBACnetTimeValue);

			if(i32DecodeLen < 0)
			{
				return i32DecodeLen;
			}
			i32Len += i32DecodeLen;
			
			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				//TODO
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}		
		}
		else
		{
			/* clear allocated memories & return error */
			//TODO
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}				

		 memcpy(&(pstDailySchedule->m_stTimeValue[u8IndexCount]), 
					&stBACnetTimeValue, sizeof(BACnetTimeValue_t));
		

		/* increment the indec count */
		u8IndexCount++;
	}

	u8IndexCount = 0;
	
	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode single node of special event.
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstSpecialEvent	[out]	pointer to save special event.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_BACnetSpecialEvent(
	uint8_t *pu8Apdu, 
	BACnetSpecialEvent_t *pstSpecialEvent,
	uint32_t u32ApduLen,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACnetCalendarEntry_t stCalenderEntry = {0};
	BACnetTimeValue_t stBACnetTimeValue = {0};
	uint32_t u32TimeValueLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstSpecialEvent)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstSpecialEvent, 0x00, sizeof(BACnetSpecialEvent_t));

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
	{	
		i32Len += i32DecodeLen;
		/* decode calendarEntry */		
		/* decode single node od Bacnet Calender Entry. */
		i32DecodeLen = DT_Decode_BACnetCalenderEntry(&pu8Apdu[i32Len],&stCalenderEntry);
		if(i32DecodeLen < 0)
		{					
			return i32DecodeLen;			
		}
		i32Len += i32DecodeLen;

		/* save decoded status of decoded data */
		pstSpecialEvent->m_eStatusCalendar = stCalenderEntry.m_eStatusCalendar;

		if(STATUS_DATE == pstSpecialEvent->m_eStatusCalendar)
		{				
			memcpy(&pstSpecialEvent->m_stPeriod.m_stDate,
				&stCalenderEntry.m_stCalendar.m_stDate,sizeof(BACnetDate_t));			
		}
		else if(STATUS_DATE_RANGE == pstSpecialEvent->m_eStatusCalendar)
		{
			memcpy(&pstSpecialEvent->m_stPeriod.m_stDateRange,
				&stCalenderEntry.m_stCalendar.m_stDateRange,sizeof(BACnetDateRange_t));				
		}
		else if(STATUS_WEEK_N_DAY == pstSpecialEvent->m_eStatusCalendar)
		{
			memcpy(&pstSpecialEvent->m_stPeriod.m_stWeekNDay,
				&stCalenderEntry.m_stCalendar.m_stWeekNDay,sizeof(WeekNDay_t));				
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
	
		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len++;
		}
		else
		{					
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}		
	}
	else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* decode calendarReference */
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstSpecialEvent->m_stPeriod.m_stCalReff.m_eObjectType, 
			&pstSpecialEvent->m_stPeriod.m_stCalReff.m_u32ObjId);	
		
		i32Len += i32DecodeLen;

		/* save decoded status of decoded data */
		pstSpecialEvent->m_eStatusCalendar = STATUS_CAL_REFF;	
	}
	else
	{
		/* invalid data */
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2))
	{
		u32TimeValueLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], u32ApduLen - i32Len,
                    ePropID);			
		i32Len += i32DecodeLen;

		/* decode time value list */
		i32DecodeLen = DT_Decode_TimeValue_List(&pu8Apdu[i32Len],u32TimeValueLen, 
				&stBACnetTimeValue);				
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/*  save decoded data */
		memcpy(&pstSpecialEvent->m_stListOfTimeValues,
			&stBACnetTimeValue,sizeof(BACnetTimeValue_t));
		
		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2))
		{
			i32Len++;
		}
		else
		{
			/* clear allocated memories & return error */
			//TODO
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
	}
	else
	{
		/* invalid data */
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* decode eventPriority */
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode array index */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstSpecialEvent->m_u32EventPriority);

		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data */
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* return total decoded data length */
	return i32Len;
}
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetSpecialEvent data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstListOfSpecialEvent [out]	pointer to save special event list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_SpecialEvent_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfSpecialEvent_t **pstListOfSpecialEvent,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfSpecialEvent_t *pstNewNode = NULL;
	ListOfSpecialEvent_t *pstBaseNode = NULL;	
	ListOfSpecialEvent_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	BACnetSpecialEvent_t stBACnetSpecialEvent= {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstListOfSpecialEvent || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstListOfSpecialEvent = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* allocate memory & save value */
		pstNewNode = (ListOfSpecialEvent_t *)
			OSAL_Malloc(sizeof(ListOfSpecialEvent_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_ExceptionShdl_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		i32DecodeLen = DT_Decode_BACnetSpecialEvent(&pu8Apdu[i32Len], &stBACnetSpecialEvent,
			u32ApduLen, ePropID);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_ExceptionShdl_List(&pstBaseNode);				
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		memcpy(&pstNewNode->m_stListSpecialEvent,&stBACnetSpecialEvent, 
				sizeof(BACnetSpecialEvent_t));

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstListOfSpecialEvent = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}
#endif /* !PROFILE_BOD_ONLY */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode single node of bacnet log record. (for TL object).
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstLogRecord	[out]	pointer to save log record.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_LogRecord(
	uint8_t *pu8Apdu, 
	BACnetLogRecord_t *pstLogRecord
	)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACnetBITStr_t stBITString = {0};
	uint8_t u8Bitcount = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstLogRecord)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
	{	
		/* decode timestamp field */
		i32Len += i32DecodeLen;		
		/* decode BACnetDateTime */
		i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len], 
			&pstLogRecord->m_stTimeStamp.m_stDate,
			&pstLogRecord->m_stTimeStamp.m_stTime);
		/* check decode length */
		if(i32DecodeLen < 0)
		{
			/* return the error code */
			i32Len = i32DecodeLen;
			return i32Len;
		}
		else
		{
			/* increment by decoded data length */
			i32Len += i32DecodeLen;
		}
		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}	
	}
	else
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{
		/* decode logDatum field */
		i32Len += i32DecodeLen;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
				&stBITString);

			if(i32DecodeLen < 0)
			{
				i32Len = i32DecodeLen;
				return i32Len;			
			}
			i32Len += i32DecodeLen;
			/* save decoded bit string data in log status */
			pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_stlog_status.m_u8ByteCnt =
				stBITString.m_u8ByteCnt;
			pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_stlog_status.m_u8UnusedBits =
				stBITString.m_u8UnusedBits;
			for(u8Bitcount=0; u8Bitcount < MIN_BITSTRING_BYTES; ++u8Bitcount)
			{
				pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_stlog_status.
					m_u8TransBits[u8Bitcount] = 
					stBITString.m_u8TransBits[u8Bitcount];			
			}
			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_LOGBUFFER_LOGSTATUS;
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType)
			{				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode boolean value */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
			pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_Boolean
				= Decode_Boolean(u32LenValueType);
			++i32Len;

			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_BOOLEAN;
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the length of real to be 4 bytes only */
			if(u32LenValueType != 4)
			{
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode real value */
			i32DecodeLen = Decode_Real(&pu8Apdu[i32Len], 
				&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_Real);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_REAL;
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode enum value */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
					&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_Enum);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_ENUM;			
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_4) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode unsigned value */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
				&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_Unsigned_Int);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_UNSIGNED;		
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_5) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode unsigned value */
			i32DecodeLen = Decode_Signed(&pu8Apdu[i32Len],u32LenValueType,
				&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_Signed_Int);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_INTEGER;		
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_6) == TRUE)
		{
			i32Len += i32DecodeLen;
			i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
				&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_stBit_String);

			if(i32DecodeLen < 0)
			{
				i32Len = i32DecodeLen;
				return i32Len;			
			}
			i32Len += i32DecodeLen;
			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_BITSTRING;
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_7) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_NULL;

		}
		else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_8))
		{
			i32Len += i32DecodeLen;

			/* decode error value */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
					(uint32_t*)&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_stError.
				m_eErrorClass);
			i32Len += i32DecodeLen;

			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], 
				&u8TagNo, &u32LenValueType);
			i32Len += i32DecodeLen;
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
					(uint32_t*)&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_stError.
				m_eErrorCode);
			i32Len += i32DecodeLen;

			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_8))
			{
				i32Len++;
			}
			else
			{
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_LOGBUFFER_ERROR;
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_9) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the length of real to be 4 bytes only */
			if(u32LenValueType != 4)
			{
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode real value */
			i32DecodeLen = Decode_Real(&pu8Apdu[i32Len], 
				&pstLogRecord->m_stlogDatum.bacnetlogDatum_U.m_fTimechange);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstLogRecord->m_stlogDatum.m_TagType = BACNET_DT_LOGBUFFER_TIMECHANGE;		
		}
		else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_10) == TRUE)
		{
			i32Len = DT_ERR_DATA_TYPE_NOT_SUPPORTED;
			return i32Len;		
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}

		/* decode closing tag of logDatum choice */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}	
	}
	else
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
			&stBITString);

		if(i32DecodeLen < 0)
		{
			i32Len = i32DecodeLen;
			return i32Len;			
		}
		i32Len += i32DecodeLen;
		/* save decoded bit string data in log status */
		pstLogRecord->m_stStatusFlag.m_u8ByteCnt =
			stBITString.m_u8ByteCnt;
		pstLogRecord->m_stStatusFlag.m_u8UnusedBits =
			stBITString.m_u8UnusedBits;
		for(u8Bitcount=0; u8Bitcount < MIN_BITSTRING_BYTES; ++u8Bitcount)
		{
			pstLogRecord->m_stStatusFlag.m_u8TransBits[u8Bitcount] = 
				stBITString.m_u8TransBits[u8Bitcount];			
		}		
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of BACnetLogRecord data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstLogRecordList [out]	pointer to save log record list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_LogRecord_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetLogRecord_t **pstLogRecordList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetLogRecord_t *pstNewNode = NULL;
	ListOfBACnetLogRecord_t *pstBaseNode = NULL;	
	ListOfBACnetLogRecord_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	BACnetLogRecord_t stLogRecord = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstLogRecordList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstLogRecordList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetLogRecord_t *)
			OSAL_Malloc(sizeof(ListOfBACnetLogRecord_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_TrendLogRecord_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		i32DecodeLen = DT_Decode_LogRecord(&pu8Apdu[i32Len], &stLogRecord);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* free previously allocated memories & return error */
			Clear_TrendLogRecord_List(&pstBaseNode);			
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		memcpy(&pstNewNode->m_stListofLogRecord,&stLogRecord, 
				sizeof(BACnetLogRecord_t));

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstLogRecordList = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode event paramters.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstEventParameter	[out]	pointer to save event parameter.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/

int32_t DT_Decode_EventParameters(
	uint8_t *apdu, 
	BACnetEventParameter_t *pstEventParameter,
	uint32_t u32Max_Apdu_Len
	)
{
	/* local variables */
	int32_t i32Len = 0;
	int32_t i32DecodeLen = 0;
	uint8_t u8Tag = 0;
	uint8_t u8Tag_Number = 0;
	uint32_t u32Tag_Len = 0;
	void *pvBaseNode = NULL;

	/* check input pointers */
	if(NULL == apdu || NULL == pstEventParameter)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the event type value */
    if(Decode_Is_Opening_Tag(&apdu[i32Len]))
    {
        i32Len += Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag,
                     &u32Tag_Len);
    }
    else
    {
       i32DecodeLen = DT_ERR_INVALID_DATA_TYPE;
	   return i32DecodeLen;
    }
    /* save event type */
    pstEventParameter->m_eEventType = u8Tag;

    /* decode parameters as per the event type */
    switch(u8Tag)
    {
        case EVENT_CHANGE_OF_STATE:
        {
            ListOfBACnetPropertyStates_t  *pstListOfValues = NULL;

            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngState.m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            pstEventParameter->BACnetEventParameter_u.m_stCngState.m_pstListOfValues = 
                OSAL_Malloc(sizeof(ListOfBACnetPropertyStates_t),__FILE__,__FUNCTION__,__LINE__);
            pstListOfValues = pstEventParameter->BACnetEventParameter_u.m_stCngState.m_pstListOfValues;
            pvBaseNode = pstListOfValues;
            if(NULL == pstListOfValues)
            {
                 i32Len = DT_ERR_MALLOC_FAILED;
				 return i32Len;
            }
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                /* decode List value */
                Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
                pstListOfValues->m_stPropStateVal.m_ePropState = u8Tag_Number;
                if(u8Tag_Number == TAG_NO_0)
                {
                    i32Len += Decode_Context_BOOLEAN(&apdu[i32Len], u8Tag_Number,
                        &pstListOfValues->m_stPropStateVal.BACnetPropertyStates_U.m_BooleanValue);
                }
                else
                {
                    i32Len += Decode_Context_Enumerated(&apdu[i32Len], u8Tag_Number, 
                        &pstListOfValues->m_stPropStateVal.BACnetPropertyStates_U.m_UnsignedValue);
                }
                /* allocate memory for new element */
                if(!Decode_Is_Closing_Tag(&apdu[i32Len]))
                {
                    pstListOfValues->m_pstNext = OSAL_Malloc(sizeof(ListOfBACnetPropertyStates_t),
					    __FILE__,__FUNCTION__,__LINE__);
                    if(NULL == pstListOfValues->m_pstNext)
                    {
                        /* free previous list members */
                        pstListOfValues = pvBaseNode;
                        while(NULL != pstListOfValues)
                        {
                            pvBaseNode = pstListOfValues;
                            pstListOfValues = pstListOfValues->m_pstNext;
                            OSAL_Free(pvBaseNode, __FILE__, __FUNCTION__, __LINE__);
                        }
                        pstEventParameter->BACnetEventParameter_u.m_stCngState.
                            m_pstListOfValues = NULL;
						i32Len = DT_ERR_MALLOC_FAILED;
						return i32Len;
                    }
                    pstListOfValues = pstListOfValues->m_pstNext;
                    pstListOfValues->m_pstNext = NULL;
                }
                else
                    break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
				/* free previous list members */
                pstListOfValues = pvBaseNode;
                while(NULL != pstListOfValues)
                {
					pvBaseNode = pstListOfValues;
                    pstListOfValues = pstListOfValues->m_pstNext;
                    OSAL_Free(pvBaseNode, __FILE__, __FUNCTION__, __LINE__);
                 }
                 pstEventParameter->BACnetEventParameter_u.m_stCngState.
                 m_pstListOfValues = NULL;

				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
				/* free previous list members */
                pstListOfValues = pvBaseNode;
                while(NULL != pstListOfValues)
                {
					pvBaseNode = pstListOfValues;
                    pstListOfValues = pstListOfValues->m_pstNext;
                    OSAL_Free(pvBaseNode, __FILE__, __FUNCTION__, __LINE__);
                 }
                 pstEventParameter->BACnetEventParameter_u.m_stCngState.
                 m_pstListOfValues = NULL;

                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_COMMAND_FAILURE :
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    (uint32_t *)(&pstEventParameter->BACnetEventParameter_u.
                    m_stCmdFail.m_u32Timedelay));
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode object id */
                i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_0,
                    (uint32_t *)&(pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
					m_stFeedbackPropertyReference.m_eObjectType),
                    &(pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
                    m_stFeedbackPropertyReference.m_u32ObjId));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* decode property id */
                i32Len += Decode_Context_Enumerated(&apdu[i32Len],TAG_NO_1,
                    (uint32_t *)(&(pstEventParameter->BACnetEventParameter_u.
                    m_stCmdFail.m_stFeedbackPropertyReference.m_ePropertyIdentifier)));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_2))
            {
                /* decode array index */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_2, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
                    m_stFeedbackPropertyReference.m_u32ArrayIndex);
                pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
                    m_stFeedbackPropertyReference.m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
            }
			else 
			{
				pstEventParameter->
                    BACnetEventParameter_u.m_stCmdFail.
					m_stFeedbackPropertyReference.m_u32ArrayIndex = BACNET_ARRAY_ALL;
                pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
                    m_stFeedbackPropertyReference.
                    m_bArrIndxPresent = ARRAY_INDEX_ABSENT;
			}
            if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_3))
            { 
				/* set device ID flag */
				pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
				    m_stFeedbackPropertyReference.m_bDeviceIdPresent = true;

			    /* decode device id */
                i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_3,
                    (uint32_t *)&(pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
					m_stFeedbackPropertyReference.m_eDeviceType),
                    &(pstEventParameter->BACnetEventParameter_u.m_stCmdFail.
				    m_stFeedbackPropertyReference.m_u32DeviceInstace));
			}
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_3))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_OUT_OF_RANGE :
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stOutRange.m_u32Timedelay);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* Decode Low limit */
                i32Len += Decode_Context_Real(&apdu[i32Len],TAG_NO_1,&(pstEventParameter->
                    BACnetEventParameter_u.m_stOutRange.m_fLow_Limit));
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_2)
            {
                /* decode High Limit */
                i32Len += Decode_Context_Real(&apdu[i32Len],TAG_NO_2,&(pstEventParameter->
                    BACnetEventParameter_u.m_stOutRange.m_fHigh_Limit));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_3)
            {
                /* decode deadband */
                i32Len += Decode_Context_Real(&apdu[i32Len],TAG_NO_3,&(pstEventParameter->
                    BACnetEventParameter_u.m_stOutRange.m_fDeadband));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_5))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break; 

        case EVENT_FLOATING_LIMIT:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    (uint32_t *)(&pstEventParameter->
                    BACnetEventParameter_u.m_stFlotLimit.m_u32Timedelay));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(TAG_NO_1 == u8Tag_Number)
            {                        
                if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                    i32Len++;
                else
                {
                   i32Len = DT_ERR_INVALID_DATA_TYPE;
				   return i32Len;
                }
                Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
                if(u8Tag_Number == TAG_NO_0)
                {
                    /* decode object id */
                    i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_0,
                        (uint32_t *)&(pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
						m_stSetpointReference.m_eObjectType),
                        &(pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
                        m_stSetpointReference.m_u32ObjId));
                }
                else
                {
                   i32Len = DT_ERR_INVALID_DATA_TYPE;
				   return i32Len;
                }
                Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
                if(u8Tag_Number == TAG_NO_1)
                {
                    /* decode property id */
                    i32Len += Decode_Context_Enumerated(&apdu[i32Len],TAG_NO_1,
                        (uint32_t *)(&(pstEventParameter->BACnetEventParameter_u.
                        m_stFlotLimit.m_stSetpointReference.m_ePropertyIdentifier)));
                }
                else
                {
                    i32Len = DT_ERR_INVALID_DATA_TYPE;
					return i32Len;
                }
                if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_2))
                {
                    /* decode array index */
                    i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_2,
                        &pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
                        m_stSetpointReference.m_u32ArrayIndex);
                    pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
                        m_stSetpointReference.m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
                }
			    else 
			    {
				    pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
                        m_stSetpointReference.m_u32ArrayIndex = BACNET_ARRAY_ALL;
                    pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
                        m_stSetpointReference.m_bArrIndxPresent = ARRAY_INDEX_ABSENT;
			    }
                if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_3))
                {  
			        /* decode device id */
                    i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_3,
                        (uint32_t *)&(pstEventParameter->BACnetEventParameter_u.
						m_stFlotLimit.m_stSetpointReference.m_eDeviceType),
                        &(pstEventParameter->BACnetEventParameter_u.m_stFlotLimit.
                        m_stSetpointReference.m_u32DeviceInstace));
			    }
                i32Len += Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1);
            }                   
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }

            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(TAG_NO_2 == u8Tag_Number)
            {
                 /* decode Low Diff limit */
                i32Len +=  Decode_Context_Real(&apdu[i32Len],TAG_NO_2,(&pstEventParameter->
                    BACnetEventParameter_u.m_stFlotLimit.m_fLow_Diff_Limit));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(TAG_NO_3 == u8Tag_Number)
            {
                /* decode High Diff limit */
                i32Len +=  Decode_Context_Real(&apdu[i32Len],TAG_NO_3,(&pstEventParameter->
                    BACnetEventParameter_u.m_stFlotLimit.m_fHigh_Diff_Limit));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(TAG_NO_4 == u8Tag_Number)
            {
                 /* decode deadband */
                i32Len +=  Decode_Context_Real(&apdu[i32Len],TAG_NO_4,(&pstEventParameter->
                    BACnetEventParameter_u.m_stFlotLimit.m_fDeadband));
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_4))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_BUFFER_READY:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode notification threshold */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stBuffReady.
                    m_u32NotificationThreshold);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* decode previous notification count */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_1, 
                    &pstEventParameter->BACnetEventParameter_u.m_stBuffReady.
                    m_u32PreviousNotificationCount);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_10))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_DOUBLE_OUT_OF_RANGE:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stDoubleOutofRange.
                    m_u32Timedelay);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* Decode Low limit */
                i32Len += Decode_Context_Double(&apdu[i32Len], TAG_NO_1,
                    &pstEventParameter->BACnetEventParameter_u.m_stDoubleOutofRange.
                    m_dLow_Limit);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_2)
            {
                /* decode High Limit */
                i32Len += Decode_Context_Double(&apdu[i32Len], TAG_NO_2,
                    &pstEventParameter->BACnetEventParameter_u.m_stDoubleOutofRange.
                    m_dHigh_Limit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_3)
            {
                /* decode deadband */
                i32Len += Decode_Context_Double(&apdu[i32Len], TAG_NO_3,
                    &pstEventParameter->BACnetEventParameter_u.m_stDoubleOutofRange.
                    m_dDeadband);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_14))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_SIGNED_OUT_OF_RANGE:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stSignedOutofRange.
                    m_u32Timedelay);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* Decode Low limit */
                i32Len += Decode_Context_Signed(&apdu[i32Len], TAG_NO_1,
                    &pstEventParameter->BACnetEventParameter_u.m_stSignedOutofRange.
                    m_i32Low_Limit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_2)
            {
                /* decode High Limit */
                i32Len += Decode_Context_Signed(&apdu[i32Len], TAG_NO_2,
                    &pstEventParameter->BACnetEventParameter_u.m_stSignedOutofRange.
                    m_i32High_Limit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_3)
            {
                /* decode deadband */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_3,
                    &pstEventParameter->BACnetEventParameter_u.m_stSignedOutofRange.
                    m_u32Deadband);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_15))
                i32Len += 2; // this is extended closing tag
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_UNSIGNED_OUT_OF_RANGE:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnSignedOutofRange.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* Decode Low limit */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_1,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnSignedOutofRange.
                    m_u32Low_Limit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_2)
            {
                /* decode High Limit */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_2,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnSignedOutofRange.
                    m_u32High_Limit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_3)
            {
                /* decode deadband */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_3,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnSignedOutofRange.
                    m_u32Deadband);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_16))
                i32Len += 2; // this is extended closing tag
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_UNSIGNED_RANGE:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnsiRange.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* Decode Low limit */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_1,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnsiRange.
                    m_u32LowLimit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_2)
            {
                /* decode High Limit */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_2,
                    &pstEventParameter->BACnetEventParameter_u.m_stUnsiRange.
                    m_u32HighLimit);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_11))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_CHANGE_OF_STATUS_FLAGS:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0,
                    &pstEventParameter->BACnetEventParameter_u.m_stCngStatusFlag.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* Decode status flags */
                i32Len += Decode_Context_BITSTRING(&apdu[i32Len], TAG_NO_1,
                    &pstEventParameter->BACnetEventParameter_u.m_stCngStatusFlag.
                    m_stStatusFlag);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_18))
                i32Len += 2; // this is extended closing tag
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_CHANGE_OF_CHARACTERSTRING:
        {
            ListOfCharStr_t  *pstListValue = NULL;
			uint32_t u32MaxStrLen = 0;

            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngCharString.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
               i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            /* allocate memory for 1st node */
            pstListValue = OSAL_Malloc(sizeof(ListOfCharStr_t),
                __FILE__,__FUNCTION__,__LINE__);
            pvBaseNode = pstListValue;
            if(NULL == pstListValue)
            {
				i32Len = DT_ERR_MALLOC_FAILED;
				return i32Len;
            }
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                /* decode character string & copy */
                i32Len += Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, 
                    &u32Tag_Len);
                if(BACNET_APPLICATION_TAG_CHARACTER_STRING == u8Tag_Number)
                {
                    i32Len += Decode_Character_String(&apdu[i32Len], 
                        u32Tag_Len, &pstListValue->m_stCharStr);
                }
                else
                {
                    /* free previous list members */
                    Clear_CharString_List((ListOfCharStr_t **)&pvBaseNode);
                   i32Len = DT_ERR_INVALID_DATA_TYPE;
					return i32Len;
                }
                /* check string length */
				u32MaxStrLen = ((pstListValue->m_stCharStr.m_u8Encoding == CHARACTER_IBM_MS_DBCS) ?
					(MAX_SUPPORTED_CHRSTR_LEN +2) : MAX_SUPPORTED_CHRSTR_LEN);
                if(pstListValue->m_stCharStr.m_u32StrLen > u32MaxStrLen)
                {
                    /* free previous list members */
                    Clear_CharString_List((ListOfCharStr_t **)&pvBaseNode);
                   i32Len = DT_ERR_VALUE_OUT_OF_RANGE;
				   return i32Len;
                }
                /* check character encoding */
				else if(Validate_Charstring_Encoding(pstListValue->m_stCharStr.m_u8Encoding, 
					pstListValue->m_stCharStr.m_u16CodePage))
                {
                    /* free previous list members */
                    Clear_CharString_List((ListOfCharStr_t **)&pvBaseNode);
					i32Len = DT_ERR_VALUE_OUT_OF_RANGE;
					return i32Len;
                }
                /* allocate memory for new element */
                if(!Decode_Is_Closing_Tag(&apdu[i32Len]))
                {
                    pstListValue->m_pstNext = OSAL_Malloc(sizeof(ListOfCharStr_t),
					    __FILE__,__FUNCTION__,__LINE__);
                    if(NULL == pstListValue->m_pstNext)
                    {
                        /* free previous list members */
                        Clear_CharString_List((ListOfCharStr_t **)&pvBaseNode);
						i32Len = DT_ERR_MALLOC_FAILED;
						return i32Len;                       
                    }
                    /* move to new node */
                    pstListValue = pstListValue->m_pstNext;
                    pstListValue->m_pstNext = NULL;
                }
                else
                    break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
			  	/* free previous list members */
              	Clear_CharString_List((ListOfCharStr_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_17))
                i32Len += 2; // this is extended closing tag
            else
            {
			  	/* free previous list members */
              	Clear_CharString_List((ListOfCharStr_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            /* attact the pointer of list of values */
            pstEventParameter->BACnetEventParameter_u.m_stCngCharString.
                m_pstListOfAlarmValues = pvBaseNode;
        }
        break;

        case EVENT_CHANGE_OF_BITSTRING:
        {
            ListOfBitStr_t  *pstListValue = NULL;

            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngBitstring.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* decode bit mask */
                i32Len += Decode_Context_BITstring(&apdu[i32Len], TAG_NO_1, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngBitstring.
                    m_stBitmask.m_stBitString);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            /* decode list of values */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
               i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            /* allocate memory for 1st node */
            pstListValue = OSAL_Malloc(sizeof(ListOfBitStr_t),
                __FILE__,__FUNCTION__,__LINE__);
            pvBaseNode = pstListValue;
            if(NULL == pstListValue)
            {
				i32Len = DT_ERR_MALLOC_FAILED;
				return i32Len;
            }
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                /* decode bitstring value */
                i32Len += Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, 
                    &u32Tag_Len);
                if(BACNET_APPLICATION_TAG_BIT_STRING == u8Tag_Number)
                {
                    i32Len += Decode_BITstring(&apdu[i32Len], 
                        u32Tag_Len, &pstListValue->m_stBitString);
                }
                else
                {
                    /* free previous list members */
                    Clear_BitString_List((ListOfBitStr_t **)&pvBaseNode);
                    i32Len = DT_ERR_INVALID_DATA_TYPE;
				    return i32Len;
                }
                /* allocate memory for new element */
                if(!Decode_Is_Closing_Tag(&apdu[i32Len]))
                {
                    pstListValue->m_pstNext = OSAL_Malloc(sizeof(ListOfBitStr_t),
					    __FILE__,__FUNCTION__,__LINE__);
                    if(NULL == pstListValue->m_pstNext)
                    {
                        /* free previous list members */
                        Clear_BitString_List((ListOfBitStr_t **)&pvBaseNode);
                        i32Len = DT_ERR_MALLOC_FAILED;
						return i32Len;
                    }
                    /* move to new node */
                    pstListValue = pstListValue->m_pstNext;
                    pstListValue->m_pstNext = NULL;
                }
                else
                    break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
				i32Len = DT_ERR_INVALID_DATA_TYPE;
			   /* free previous list members */
                Clear_BitString_List((ListOfBitStr_t **)&pvBaseNode);
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_0))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
			    /* free previous list members */
                Clear_BitString_List((ListOfBitStr_t **)&pvBaseNode);
				return i32Len;
            }
            /* attact the pointer of list of values */
            pstEventParameter->BACnetEventParameter_u.m_stCngBitstring.
                m_pstBitStrList = pvBaseNode;
        }
        break;

        case EVENT_CHANGE_OF_VALUE:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngValue.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
                i32Len = DT_ERR_INVALID_TAG;
				return i32Len;               
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(TAG_NO_0 == u8Tag_Number)
            {
                /* decode bitmask */
                i32Len += Decode_Context_BITstring(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngValue.
                    m_stCovCriteria.m_stBitmask.m_stBitString);
                pstEventParameter->BACnetEventParameter_u.m_stCngValue.
                    m_i8CriteriaType = TAG_NO_0;
            }
            else if(TAG_NO_1 == u8Tag_Number)
            {
                /* decode reference property increment */
                i32Len += Decode_Context_Real(&apdu[i32Len], TAG_NO_1,
                    &pstEventParameter->BACnetEventParameter_u.m_stCngValue.
                    m_stCovCriteria.m_fRefPropIncr);
                pstEventParameter->BACnetEventParameter_u.m_stCngValue.
                    m_i8CriteriaType = TAG_NO_1;
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        case EVENT_ACCESS_EVENT:
        {
            ListOfEnum_t *pstAccessEvent = NULL;

            /* decode list of access event */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_0))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
                i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            /* allocate memory for 1st node */
            pstAccessEvent = OSAL_Malloc(sizeof(ListOfEnum_t),__FILE__,__FUNCTION__,__LINE__);
            pvBaseNode = pstAccessEvent;
            if(NULL == pstAccessEvent)
            {
               i32Len = DT_ERR_MALLOC_FAILED;
				return i32Len;
            }
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                /* decode tag */
                i32Len += Decode_Tag_Number_And_Value(&apdu[i32Len], 
                    &u8Tag_Number, &u32Tag_Len);
                if(BACNET_APPLICATION_TAG_ENUMERATED == u8Tag_Number)
                {
                    /* decode access event */
                    i32Len += Decode_Enumerated(&apdu[i32Len], u32Tag_Len,
                    		(uint32_t*)&pstAccessEvent->m_eVal);
                }
                else
                {
                    /* free previous list members */
                    Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                   i32Len = DT_ERR_INVALID_DATA_TYPE;
					return i32Len;
                }
                /* allocate memory for new element */
                if(!Decode_Is_Closing_Tag(&apdu[i32Len]))
                {
                    pstAccessEvent->m_pstNext = OSAL_Malloc(sizeof(ListOfEnum_t),
					    __FILE__,__FUNCTION__,__LINE__);
                    if(NULL == pstAccessEvent->m_pstNext)
                    {
                        /* free previous list members */
                        Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                        i32Len = DT_ERR_MALLOC_FAILED;
						return i32Len;
                    }
                    /* move to new node */
                    pstAccessEvent = pstAccessEvent->m_pstNext;
                    pstAccessEvent->m_pstNext = NULL;
                }
                else
                    break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_0))
                i32Len++;
            else
            {
				/* free previous list members */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }

            /* decode access event time reference */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
			   	/* free previous list members */
               	Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode object id */
                i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_0,
                    (uint32_t *)&(pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
					m_stAccessEventTimeReff.m_eObjectType),
                    &(pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
                    m_stAccessEventTimeReff.m_u32ObjId));
            }
            else
            {
			   	/* free previous list members */
               	Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* decode property id */
                i32Len += Decode_Context_Enumerated(&apdu[i32Len],TAG_NO_1, 
                    (uint32_t *)(&(pstEventParameter->BACnetEventParameter_u.
                    m_stAccessEvent.m_stAccessEventTimeReff.m_ePropertyIdentifier)));
            }
            else
            {
			   	/* free previous list members */
               	Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_2))
            {
                /* decode array index */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_2, 
                    &pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
                    m_stAccessEventTimeReff.m_u32ArrayIndex);
                pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
                    m_stAccessEventTimeReff.m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
            }
			else 
			{
				pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
					m_stAccessEventTimeReff.m_u32ArrayIndex = BACNET_ARRAY_ALL;
                pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
                    m_stAccessEventTimeReff.m_bArrIndxPresent = ARRAY_INDEX_ABSENT;
			}
            if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_3))
            { 
			    /* decode device id */
                i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_3,
                    (uint32_t *)&(pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
					m_stAccessEventTimeReff.m_eDeviceType),
                    &(pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
				    m_stAccessEventTimeReff.m_u32DeviceInstace));
			}
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
			   	/* free previous list members */
               	Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_13))
                i32Len++;
            else
            {
			   	/* free previous list members */
               	Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
               	i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            /* attact the list of values pointer */
            pstEventParameter->BACnetEventParameter_u.m_stAccessEvent.
                m_pstAccessEventList = pvBaseNode;
        }
        break;

        case EVENT_CHANGE_OF_LIFE_SAFETY:
        {
            ListOfEnum_t *pstLifeSafetyState = NULL;
            void *pvTemp = NULL;

            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode time delay */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                    m_u32Timedelay);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }

            /* decode list of life safety alarm values */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            /* allocate memory for 1st node */
            pstLifeSafetyState = OSAL_Malloc(sizeof(ListOfEnum_t),
                __FILE__,__FUNCTION__,__LINE__);
            pvBaseNode = pstLifeSafetyState;
            if(NULL == pstLifeSafetyState)
            {
                i32Len = DT_ERR_MALLOC_FAILED;
				return i32Len;
            }
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                /* decode tag */
                i32Len += Decode_Tag_Number_And_Value(&apdu[i32Len], 
                    &u8Tag_Number, &u32Tag_Len);
                if(BACNET_APPLICATION_TAG_ENUMERATED == u8Tag_Number)
                {
                    /* decode life safety alarm value */
                    i32Len += Decode_Enumerated(&apdu[i32Len], u32Tag_Len,
                    		(uint32_t*)&pstLifeSafetyState->m_eVal);
                }
                else
                {
                    /* free previous list members */
                    Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                   	i32Len = DT_ERR_INVALID_DATA_TYPE;
					return i32Len;
                }
                /* allocate memory for new element */
                if(!Decode_Is_Closing_Tag(&apdu[i32Len]))
                {
                    pstLifeSafetyState->m_pstNext = OSAL_Malloc(sizeof(ListOfEnum_t),
					    __FILE__,__FUNCTION__,__LINE__);
                    if(NULL == pstLifeSafetyState->m_pstNext)
                    {
                        /* free previous list members */
                        Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                       i32Len = DT_ERR_MALLOC_FAILED;
						return i32Len;
                    }
                    /* move to new node */
                    pstLifeSafetyState = pstLifeSafetyState->m_pstNext;
                    pstLifeSafetyState->m_pstNext = NULL;
                }
                else
                    break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_1))
                i32Len++;
            else
            {
			    /* free previous list members */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }

            /* decode list of alarm values */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
				/* free previous list members */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
				/* free previous list members */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            /* allocate memory for 1st node */
            pstLifeSafetyState = OSAL_Malloc(sizeof(ListOfEnum_t),
                __FILE__,__FUNCTION__,__LINE__);
            pvTemp = pstLifeSafetyState;
            if(NULL == pstLifeSafetyState)
            {
				/* free previous list members */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                i32Len = DT_ERR_MALLOC_FAILED;
				return i32Len;
            }
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                /* decode tag */
                i32Len += Decode_Tag_Number_And_Value(&apdu[i32Len], 
                    &u8Tag_Number, &u32Tag_Len);
                if(BACNET_APPLICATION_TAG_ENUMERATED == u8Tag_Number)
                {
                    /* decode alarm value */
                    i32Len += Decode_Enumerated(&apdu[i32Len], u32Tag_Len,
                    		(uint32_t*)&pstLifeSafetyState->m_eVal);
                }
                else
                {
                    /* free previous list members */
                    Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                    Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
                    i32Len = DT_ERR_INVALID_DATA_TYPE;
					return i32Len;
                }
                /* allocate memory for new element */
                if(!Decode_Is_Closing_Tag(&apdu[i32Len]))
                {
                    pstLifeSafetyState->m_pstNext = OSAL_Malloc(sizeof(ListOfEnum_t),
					    __FILE__,__FUNCTION__,__LINE__);
                    if(NULL == pstLifeSafetyState->m_pstNext)
                    {
                        /* free previous list members */
                        Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                        Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
                        i32Len = DT_ERR_MALLOC_FAILED;
						return i32Len;
                    }
                    /* move to new node */
                    pstLifeSafetyState = pstLifeSafetyState->m_pstNext;
                    pstLifeSafetyState->m_pstNext = NULL;
                }
                else
                    break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
                /* clear allocated memories */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }

            /* decode mode property reference */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_3))
                i32Len++;
            else
            {
                /* clear allocated memories */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode object id */
                i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_0,
                    (uint32_t *)&pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
					m_stModePropReff.m_eObjectType,
                    &pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                    m_stModePropReff.m_u32ObjId);
            }
            else
            {
                /* clear allocated memories */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_1)
            {
                /* decode property id */
                i32Len += Decode_Context_Enumerated(&apdu[i32Len],TAG_NO_1, 
                    (uint32_t *)(&pstEventParameter->BACnetEventParameter_u.
                    m_stCngLifeSafety.m_stModePropReff.m_ePropertyIdentifier));
            }
            else
            {
                /* clear allocated memories */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_2))
            {
                /* decode array index */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_2, 
                    &pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                    m_stModePropReff.m_u32ArrayIndex);
                pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                    m_stModePropReff.m_bArrIndxPresent = ARRAY_INDEX_PRESENT;
            }
			else 
			{
				pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
					m_stModePropReff.m_u32ArrayIndex = BACNET_ARRAY_ALL;
                pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                    m_stModePropReff.m_bArrIndxPresent = ARRAY_INDEX_ABSENT;
			}
            if(Decode_Is_Context_Tag(&apdu[i32Len],TAG_NO_3) && 
				!Decode_Is_Closing_Tag_Number(&apdu[i32Len], TAG_NO_3))
            { 
			    /* decode device id */
                i32Len += Decode_Context_Object_Id(&apdu[i32Len],TAG_NO_3,
                    (uint32_t *)&pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
					m_stModePropReff.m_eDeviceType,
                    &pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
				    m_stModePropReff.m_u32DeviceInstace);
			}
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_3))
                i32Len++;
            else
            {
                /* clear allocated memories */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_8))
                i32Len++;
            else
            {
                /* clear allocated memories */
                Clear_Enumeration_List((ListOfEnum_t **)&pvBaseNode);
                Clear_Enumeration_List((ListOfEnum_t **)&pvTemp);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            /* attact the list of values pointer */
            pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                m_pstListOfLifeSafetyAlarmValues = pvBaseNode;
            pstEventParameter->BACnetEventParameter_u.m_stCngLifeSafety.
                m_pstListOfAlarmValues = pvTemp;
        }
        break;

        case EVENT_EXTENDED:
        {
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode vendor id */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stExtended.
					m_u32VendorId);
            }
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            Decode_Tag_Number_And_Value(&apdu[i32Len], &u8Tag_Number, &u32Tag_Len);
            if(u8Tag_Number == TAG_NO_0)
            {
                /* decode extended event type */
                i32Len += Decode_Context_Unsigned(&apdu[i32Len], TAG_NO_0, 
                    &pstEventParameter->BACnetEventParameter_u.m_stExtended.
					m_u32ExtendedEventType);
            }
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }

            /* decode list of parameters */
            if(Decode_Is_Opening_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
               i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag(&apdu[i32Len]))
            {
                i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
            }
            // TODO - implement while loop
            while(i32Len < (int32_t)u32Max_Apdu_Len)
            {
                // hardcoded - need to change
                i32Len = u32Max_Apdu_Len - 2;
                break;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_2))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
            if(Decode_Is_Closing_Tag_Number(&apdu[i32Len],TAG_NO_9))
                i32Len++;
            else
            {
                i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
            }
        }
        break;

        /* default case - check for error */
        default:
        {
            if(Decode_Is_Closing_Tag_Number(&apdu[u32Max_Apdu_Len-i32Len], u8Tag))               
				i32Len = DT_ERR_DATA_TYPE_NOT_SUPPORTED;
            else
                 i32Len = DT_ERR_INVALID_DATA_TYPE;
            return i32Len;
        }
        break;
    }

	return i32Len;	
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of Bitstring data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstBacnet_BitStringList [out]	pointer to save bit string array.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_BitString_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBitStr_t **pstBacnet_BitStringList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBitStr_t *pstNewNode = NULL;
	ListOfBitStr_t *pstBaseNode = NULL;	
	ListOfBitStr_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;		
	bool bFlag = FALSE;
	BACnetBITStr_t stBITstring = {0};

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstBacnet_BitStringList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstBacnet_BitStringList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* allocate memory & save value */
		pstNewNode = (ListOfBitStr_t *)
			OSAL_Malloc(sizeof(ListOfBitStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_BitString_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		/* check tag value */
		if(BACNET_APPLICATION_TAG_BIT_STRING != u8TagNo)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_BitString_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}
		i32Len += i32DecodeLen;

		/* decode single node of bitstring */
		i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
				&stBITstring);

		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_BitString_List(&pstBaseNode);				
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		memcpy(&pstNewNode->m_stBitString,&stBITstring, 
				sizeof(BACnetBITStr_t));

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstBacnet_BitStringList = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}
/**
*
* DESCRIPTION
* Function to decode optional unsigned data type.
*
* @param pu8APDU			 [in]	data to be decoded.
* @param pstOptionalUnsigned [out]	pointer to save value.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
*/
int32_t DT_Decode_OptionalUnsigned(
	uint8_t *pu8Apdu,
	BACnetOptionalUnsigned_t *pstOptionalUnsigned)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Decode_OptionalUnsigned: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstOptionalUnsigned)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;

	if(BACNET_APPLICATION_TAG_NULL == u8TagNo)
	{
		/* set the tag type as NULL */
		pstOptionalUnsigned->m_eDataType = BACNET_DT_NULL;
	}
	/* check the application tag type */
	else if(BACNET_APPLICATION_TAG_UNSIGNED_INT == u8TagNo)
	{
		/* set the tag type as unsigned int*/
		pstOptionalUnsigned->m_eDataType = BACNET_DT_UNSIGNED;

		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* invalid data received */
			i32Len = DT_ERR_VALUE_OUT_OF_RANGE;
			return i32Len;
		}
		/* decode optional unsigned int */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				&pstOptionalUnsigned->m_u32Val);

		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Decode_OptionalUnsigned: Exit \r\n");
	#endif
	/* return total decoded data length */
	return i32Len;
}
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of Optional characterstring data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstOptCharStrList [out]	pointer to save optional character string ;ist.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_OpCharString_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfOptCharStr_t **pstOptCharStrList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfOptCharStr_t *pstNewNode = NULL;
	ListOfOptCharStr_t *pstBaseNode = NULL;	
	ListOfOptCharStr_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	bool bFlag = FALSE;
	uint32_t u32MaxStrLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstOptCharStrList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstOptCharStrList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* allocate memory & save value */
		pstNewNode = (ListOfOptCharStr_t *)
			OSAL_Malloc(sizeof(ListOfOptCharStr_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_OptionalCharString_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);		

		/* decode single node of optional character string */
		if(u8TagNo ==  BACNET_APPLICATION_TAG_NULL)
		{
			pstNewNode->m_i8AppTag = BACNET_APPLICATION_TAG_NULL;		
		}
		else if(u8TagNo ==  BACNET_APPLICATION_TAG_CHARACTER_STRING)
		{
			i32Len += i32DecodeLen;			
			i32DecodeLen = Decode_Character_String(&pu8Apdu[i32Len], u32LenValueType,
				&pstNewNode->m_stCharStr);			
			pstNewNode->m_i8AppTag = BACNET_APPLICATION_TAG_CHARACTER_STRING;		

			/* get max supported string length */
			u32MaxStrLen = ((pstNewNode->m_stCharStr.m_u8Encoding == CHARACTER_IBM_MS_DBCS) ?
				(MAX_SUPPORTED_CHRSTR_LEN +2) : MAX_SUPPORTED_CHRSTR_LEN);
		}		

		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* free previously allocated memories & return error */
			Clear_OptionalCharString_List(&pstBaseNode);			
			i32Len = i32DecodeLen;
			return i32Len;
		}
		else if(u32LenValueType > u32MaxStrLen)
		{
			/* free previously allocated memories & return error */
			Clear_OptionalCharString_List(&pstBaseNode);			
			return DT_ERR_VALUE_OUT_OF_RANGE;
		}
		i32Len += i32DecodeLen;

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstOptCharStrList = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode any data type.
*    
* @param pu8APDU	  [in]	data to be decoded.* 
* @param pstScale     [out]	pointer to save bacnet any value.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AnyValue(
	uint8_t *pu8Apdu, 							
	BACNET_PROPERTY_VALUE *pstAnyValue)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAnyValue)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
		&u32LenValueType);	
	if(u8TagNo != BACNET_APPLICATION_TAG_NULL)
		i32Len += i32DecodeLen;		

	/* decode application tagged data */
	i32DecodeLen = BACApp_Decode_Data(&pu8Apdu[i32Len], u8TagNo,
		u32LenValueType, pstAnyValue);
	if(i32DecodeLen < 0)
	{
		/* invalid data received */
		i32Len = i32DecodeLen;
		return i32Len;
	}

	/* increment decode length if NULL tag */
	if(u8TagNo == BACNET_APPLICATION_TAG_NULL)
		++i32Len;
	else
		i32Len += i32DecodeLen;

	/* save the tag */
	pstAnyValue->m_TagType = u8TagNo;

	/* return total decoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode device object reference data type.
*    
* @param pu8APDU		[in]	data to be decoded.
* @param pstDevObjRef	[out]	pointer to save recipient data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DevObjReff(
	uint8_t *pu8Apdu, 
	BACnetDevObjRef_t *pstDevObjRef)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstDevObjRef)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstDevObjRef, 0x00, sizeof(BACnetDevObjRef_t));

	/* check if device id is present or not */
	pstDevObjRef->m_bDeviceIdPresent = false;

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode device id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstDevObjRef->m_eDeviceType, &pstDevObjRef->m_u32DeviceId);
		i32Len += i32DecodeLen;
		/* set device id flag */
		pstDevObjRef->m_bDeviceIdPresent = true;
	}	

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode object id */
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstDevObjRef->m_eObjectType, &pstDevObjRef->m_u32ObjId);
		i32Len += i32DecodeLen;		
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list/array of BACnetDeviceObjectReference data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstOptCharStrList [out]	pointer to save optional character string ;ist.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_DevObjRef_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetDevObjRef_t **pstListOfDevObjReff,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetDevObjRef_t *pstNewNode = NULL;
	ListOfBACnetDevObjRef_t *pstBaseNode = NULL;	
	ListOfBACnetDevObjRef_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;
	BACnetDevObjRef_t stDevObjReff = {0};
	bool bFlag = FALSE;
	//uint8_t u8CharCount = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstListOfDevObjReff || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstListOfDevObjReff = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetDevObjRef_t *)
			OSAL_Malloc(sizeof(ListOfBACnetDevObjRef_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_DevObjRef_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}

		/* decode single node of BACnetDeviceObjectReference */
		i32DecodeLen = DT_Decode_DevObjReff(&pu8Apdu[i32Len], &stDevObjReff);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_DevObjRef_List(&pstBaseNode);				
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* save decoded data */
		memcpy(&pstNewNode->m_stDevObjRef,&stDevObjReff, 
				sizeof(BACnetDevObjRef_t));

		/* increment count */
		u32Count++;

		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstListOfDevObjReff = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode bacnet shed level data type.
*    
* @param pu8APDU	  [in]	data to be decoded.* 
* @param pstBacnetShedLevel    [out]	pointer to save bacnet Scale value.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_ShedLevel(
	uint8_t *pu8Apdu, 							
	Pr_BACnetShedLevel_t *pstBacnetShedLevel)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstBacnetShedLevel)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	i32Len += i32DecodeLen;
	
	/* check context tag value */
	if(Decode_Is_Context_Tag(&pu8Apdu[0], TAG_NO_0) == TRUE)
	{
		/* check the length of int is between 1 to 4 bytes only */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode percent */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstBacnetShedLevel->m_stShedLevel.shedlevel_u.m_u32Percent);
		i32Len += i32DecodeLen;

		pstBacnetShedLevel->m_stShedLevel.m_eShedLevelType = SHED_LEVEL_PERCENT;
	}
	else if(Decode_Is_Context_Tag(&pu8Apdu[0], TAG_NO_1) == TRUE)
	{
		/* check the length of int is between 1 to 4 bytes only */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode level */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstBacnetShedLevel->m_stShedLevel.shedlevel_u.m_u32Level);
		i32Len += i32DecodeLen;

		pstBacnetShedLevel->m_stShedLevel.m_eShedLevelType = SHED_LEVEL_LEVEL;
	}
	else if(Decode_Is_Context_Tag(&pu8Apdu[0], TAG_NO_2) == TRUE)
	{
		/* check the length of real to be 4 bytes only */
		if(u32LenValueType != 4)
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode amount */
		i32DecodeLen = Decode_Real(&pu8Apdu[i32Len], 
			&pstBacnetShedLevel->m_stShedLevel.shedlevel_u.m_fAmount);
		i32Len += i32DecodeLen;

		pstBacnetShedLevel->m_stShedLevel.m_eShedLevelType = SHED_LEVEL_AMOUNT;
	}
	else
	{
		/* error case */
		i32Len = DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode single node of bacnet log data (for TLM object).
*    
* @param pu8APDU		[in]	data to be decoded.
* @param BACnetLogData_t		[out]	pointer to save log data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_LogData(
	uint8_t *pu8Apdu, 
	BACnetLogData_t *pstLogData,
	uint32_t u32APDULen,
	BACNET_PROPERTY_ID ePropID
	)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACnetBITStr_t stBITString = {0};
	uint8_t u8Bitcount = 0;
	BACnetLogData_t *pstNewNode = NULL;
	uint32_t u32LogDataLen = 0;


	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstLogData)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstLogData, 0x00, sizeof(BACnetLogData_t));

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
			&stBITString);

		if(i32DecodeLen < 0)
		{
			i32Len = i32DecodeLen;
			return i32Len;			
		}
		i32Len += i32DecodeLen;
		/* save decoded bit string data in log status */
		pstLogData->logMultipleData_u.m_stlog_status.m_u8ByteCnt = stBITString.m_u8ByteCnt;
		pstLogData->logMultipleData_u.m_stlog_status.m_u8UnusedBits = stBITString.m_u8UnusedBits;
		for(u8Bitcount=0; u8Bitcount < MIN_BITSTRING_BYTES; ++u8Bitcount)
		{
			pstLogData->logMultipleData_u.m_stlog_status.m_u8TransBits[u8Bitcount] = 
				stBITString.m_u8TransBits[u8Bitcount];			
		}
		/* save tag type */
		pstLogData->m_TagType = BACNET_DT_LOGBUFFER_LOGSTATUS;
	}
	else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{
		/* verify the length */
		u32LogDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], u32APDULen - i32Len, ePropID);

		i32Len += i32DecodeLen;
		/* 1st node */
		pstNewNode = pstLogData;
		pstLogData->m_pstNext = NULL;
		
		/* decode list of values */
		while(NULL != pstNewNode)
		{
			/* decode the application tag */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
				&u32LenValueType);
			if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
			{
				i32Len += i32DecodeLen;
				if(1 != u32LenValueType && 0 != u32LenValueType)
				{
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					i32Len = DT_ERR_INVALID_TAG;
					return i32Len;
				}
				/* decode boolean value */
				/* decode the application tag */
				i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], 
					&u8TagNo, &u32LenValueType);
				pstNewNode->logMultipleData_u.m_Boolean	= Decode_Boolean(u32LenValueType);
				++i32Len;

				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_BOOLEAN;
			}
			else if((Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1)) == TRUE &&
					(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))== FALSE)
			{
				i32Len += i32DecodeLen;
				/* check the length of real to be 4 bytes only */
				if(u32LenValueType != 4)
				{
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					i32Len = DT_ERR_INVALID_TAG;
					return i32Len;
				}
				/* decode real value */
				i32DecodeLen = Decode_Real(&pu8Apdu[i32Len],&pstNewNode->logMultipleData_u.m_Real);
				i32Len += i32DecodeLen;

				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_REAL;
			}
			else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
			{
				i32Len += i32DecodeLen;
				/* check the application tag type */
				if(u32LenValueType < 1 || u32LenValueType > 4)
				{
					/* invalid data received */
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					i32Len = DT_ERR_INVALID_TAG;
					return i32Len;
				}
				/* decode enum value */
				i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
						&pstNewNode->logMultipleData_u.m_Enum);
				i32Len += i32DecodeLen;

				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_ENUM;			
			}
			else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
			{
				i32Len += i32DecodeLen;
				/* check the application tag type */
				if(u32LenValueType < 1 || u32LenValueType > 4)
				{
					/* invalid data received */				
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					i32Len = DT_ERR_INVALID_TAG;
					return i32Len;
				}
				/* decode unsigned value */
				i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
					&pstNewNode->logMultipleData_u.m_Unsigned_Int);
				i32Len += i32DecodeLen;

				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_UNSIGNED;		
			}
			else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_4) == TRUE)
			{
				i32Len += i32DecodeLen;
				/* check the application tag type */
				if(u32LenValueType < 1 || u32LenValueType > 4)
				{
					/* invalid data received */
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);									
					i32Len = DT_ERR_INVALID_TAG;
					return i32Len;
				}
				/* decode unsigned value */
				i32DecodeLen = Decode_Signed(&pu8Apdu[i32Len],u32LenValueType,
					&pstNewNode->logMultipleData_u.m_Signed_Int);
				i32Len += i32DecodeLen;

				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_INTEGER;		
			}
			else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_5) == TRUE)
			{
				i32Len += i32DecodeLen;
				i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
					&pstNewNode->logMultipleData_u.m_stBit_String);

				if(i32DecodeLen < 0)
				{
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					i32Len = i32DecodeLen;
					return i32Len;			
				}
				i32Len += i32DecodeLen;
				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_BITSTRING;
			}
			else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_6) == TRUE)
			{
				i32Len += i32DecodeLen;
				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_NULL;

			}
			else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_7))
			{
				i32Len += i32DecodeLen;

				/* decode error value */
				i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
						(uint32_t*)&pstNewNode->logMultipleData_u.m_stError.m_eErrorClass);
				i32Len += i32DecodeLen;

				i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
						(uint32_t*)&pstNewNode->logMultipleData_u.m_stError.m_eErrorCode);
				i32Len += i32DecodeLen;

				if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_7))
				{
					i32Len++;
				}
				else
				{
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					i32Len = DT_ERR_INVALID_TAG;
					return i32Len;
				}
				/* save tag type */
				pstNewNode->m_TagType = BACNET_DT_LOGBUFFER_ERROR;
			}
			else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_8) == TRUE)
			{
				/* free previously allocated memories & return error */
				Clear_LogDataTLM_List(&pstLogData->m_pstNext);
				i32Len = DT_ERR_DATA_TYPE_NOT_SUPPORTED;
				return i32Len;		
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_LogDataTLM_List(&pstLogData->m_pstNext);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;			
			}

			/* allocate memory is more data is available to decode */
			/* -1 subtracted for closing tag */
			if((u32LogDataLen - (i32Len - 1)) > 0)
			{
				/* allocate memory & save value */
				pstNewNode->m_pstNext = (BACnetLogData_t *)OSAL_Malloc(sizeof(BACnetLogData_t), 
					__FILE__,__FUNCTION__,__LINE__);
				if(NULL == pstNewNode->m_pstNext)
				{
					/* free previously allocated memories & return error */
					Clear_LogDataTLM_List(&pstLogData->m_pstNext);
					return DT_ERR_MALLOC_FAILED;
				}
				/* move to new node */
				pstNewNode = pstNewNode->m_pstNext;
				pstNewNode->m_pstNext = NULL;
			}
			else
			{
				/* break the loop */
				pstNewNode = NULL;
			}
		}/* while */

		/* decode closing tag of logDatum choice */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_LogDataTLM_List(&pstLogData->m_pstNext);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}	
	}
	else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the length of real to be 4 bytes only */
		if(u32LenValueType != 4)
		{
			/* free previously allocated memories & return error */
			Clear_LogDataTLM_List(&pstLogData->m_pstNext);
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode real value */
		i32DecodeLen = Decode_Real(&pu8Apdu[i32Len],
			&pstLogData->logMultipleData_u.m_fTimechange);
		i32Len += i32DecodeLen;

		/* save tag type */
		pstLogData->m_TagType = BACNET_DT_LOGBUFFER_TIMECHANGE;		
	}
	else
	{
		/* free previously allocated memories & return error */
		Clear_LogDataTLM_List(&pstLogData->m_pstNext);
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of BACnetLogMultipleRecord data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstLogMultipleRecord [out]	pointer to save log record list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_LogMultipleRecord_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetLogMultipleRecord_t **pstLogMultipleRecord,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBACnetLogMultipleRecord_t *pstNewNode = NULL;
	ListOfBACnetLogMultipleRecord_t *pstBaseNode = NULL;	
	ListOfBACnetLogMultipleRecord_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	BACnetLogData_t stLogData = {0};
	BACnetDateTime_t stDATETIME = {0};
	bool bFlag = FALSE;
	uint32_t u32LogDataLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstLogMultipleRecord || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstLogMultipleRecord = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len += i32DecodeLen;
			i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len],
				&stDATETIME.m_stDate, &stDATETIME.m_stTime);
			/* check decode length */
			if(i32DecodeLen < 0)
			{
				/* clear allocated memories & return error */
				Clear_TrendLogMultipleRecord_List(&pstBaseNode);
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;		

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_TrendLogMultipleRecord_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_TrendLogMultipleRecord_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{			
			/* verify the length */
			u32LogDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], (u32ApduLen - i32Len), ePropID);

			i32Len += i32DecodeLen;
			i32DecodeLen = DT_Decode_LogData(&pu8Apdu[i32Len], &stLogData,
					u32LogDataLen, ePropID);
			if(i32DecodeLen < 0)
			{			
				/* invalid data received */
				/* clear allocated memories & return error */
				Clear_TrendLogMultipleRecord_List(&pstBaseNode);				
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_TrendLogMultipleRecord_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}		
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_TrendLogMultipleRecord_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}		

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetLogMultipleRecord_t *)
			OSAL_Malloc(sizeof(ListOfBACnetLogMultipleRecord_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* clear allocated memories & return error */
			Clear_TrendLogMultipleRecord_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(&pstNewNode->m_stTimeStamp,&stDATETIME, sizeof(BACnetDateTime_t));
		memcpy(&pstNewNode->m_stlogData,&stLogData,	sizeof(BACnetLogData_t));

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstLogMultipleRecord = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode datatype - enum-list.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstEnumList  [out]	pointer to save list of enum values.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Enum_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfEnum_t **pstEnumList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfEnum_t *pstNewNode = NULL;
	ListOfEnum_t *pstBaseNode = NULL;	
	ListOfEnum_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;
	uint32_t u32Value = 0;
	bool bFlag = FALSE;	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstEnumList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstEnumList = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		i32Len += i32DecodeLen;

		if(BACNET_APPLICATION_TAG_ENUMERATED != u8TagNo || 
			u32LenValueType < 1 || u32LenValueType > 4)
		{
			/* free previously allocated memories & return error */
			Clear_Enumeration_List(&pstBaseNode);
			return DT_ERR_INVALID_TAG;
		}

		/* decode enum value */
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, &u32Value);
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfEnum_t *)
			OSAL_Malloc(sizeof(ListOfEnum_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_Enumeration_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		pstNewNode->m_eVal = u32Value;	

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstEnumList = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode single node of bacnet log record. (for TL object).
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstLogRecord	[out]	pointer to save log record.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_EventLogData(
	uint8_t *pu8Apdu, 
	EventlogDatam_t *pstEventLog,
	uint32_t u32APDULen,
	BACNET_PROPERTY_ID ePropID
	)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACnetBITStr_t stBITString = {0};
	uint8_t u8Bitcount = 0;
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstEventLog)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
		
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	/* Log status */
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		i32DecodeLen = Decode_BITstring(&pu8Apdu[i32Len], u32LenValueType,
			&stBITString);

		if(i32DecodeLen < 0)
		{
			i32Len = i32DecodeLen;
			return i32Len;			
		}
		i32Len += i32DecodeLen;

		/* save decoded bit string data in log status */
		pstEventLog->eventlogData_u.m_stlog_status.m_u8ByteCnt =
			stBITString.m_u8ByteCnt;
		pstEventLog->eventlogData_u.m_stlog_status.m_u8UnusedBits =
			stBITString.m_u8UnusedBits;
		for(u8Bitcount=0; u8Bitcount < MIN_BITSTRING_BYTES; ++u8Bitcount)
		{
			pstEventLog->eventlogData_u.m_stlog_status.m_u8TransBits[u8Bitcount] = 
				stBITString.m_u8TransBits[u8Bitcount];			
		}
		/* save tag type */
		pstEventLog->m_TagType = BACNET_DT_LOGBUFFER_LOGSTATUS;
	}
	/* confirmed event notification-request */
	else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{
		i32Len += i32DecodeLen;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);		
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */					
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode processIdentifier */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
				&pstEventLog->eventlogData_u.m_stNotification.m_u32ProcessId);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_UNSIGNED;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode initiatingDeviceIdentifier */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&pstEventLog->eventlogData_u.m_stNotification.m_eDeviceType, 
				&pstEventLog->eventlogData_u.m_stNotification.m_u32DeviceId);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_OBJECTID;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode eventObjectIdentifier */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&pstEventLog->eventlogData_u.m_stNotification.m_eObjectType, 
				&pstEventLog->eventlogData_u.m_stNotification.m_u32ObjId);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_OBJECTID;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_3))
		{			
			timeStamp_u u_timeStamp = {0};
			BACNET_TIMESTAMP_TYPE eTimeStamp = MAX_TIMESTAMP_TYPE;

			i32Len += i32DecodeLen;

			/* decode timeStamp */
			i32DecodeLen = DT_Decode_TimeStamp(&pu8Apdu[i32Len], 
				&u_timeStamp, &eTimeStamp);
			if(i32DecodeLen < 0)
			{
				return i32DecodeLen;
			}
			i32Len += i32DecodeLen;

			/* save decoded data */
			if(TIMESTAMP_TYPE_TIME == eTimeStamp)
			{				
				memcpy(&pstEventLog->eventlogData_u.m_stNotification.m_stTimeStamp.m_stTimeStamp.m_stTime,
					&u_timeStamp.m_stTime,sizeof(BACnetTime_t));							
			}
			else if(TIMESTAMP_TYPE_DATETIME == eTimeStamp)
			{
				memcpy(&pstEventLog->eventlogData_u.m_stNotification.m_stTimeStamp.m_stTimeStamp.m_stDateTime,
					&u_timeStamp.m_stDateTime,sizeof(BACnetDateTime_t));	
			}
			else if(TIMESTAMP_TYPE_SEQUENCE_NO == eTimeStamp)
			{
				pstEventLog->eventlogData_u.m_stNotification.m_stTimeStamp.m_stTimeStamp.m_u32SeqNo 
					= u_timeStamp.m_u32SeqNo;
			}
			pstEventLog->eventlogData_u.m_stNotification.m_stTimeStamp.m_eTimeStampType 
					= TIMESTAMP_TYPE_TIME;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_3))
			{
				i32Len++;
			}
			else
			{			
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_TIMESTAMP;
		}
		else
		{			
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);		
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_4) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* invalid data received */					
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode notificationClass */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
				&pstEventLog->eventlogData_u.m_stNotification.m_u32NotifyClass);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_UNSIGNED;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);		
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_5) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 1 ) /* for unsigned 8 datatype */
			{
				/* invalid data received */					
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode notificationClass */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],u32LenValueType,
				(uint32_t *)&pstEventLog->eventlogData_u.m_stNotification.m_u8Priority);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_UNSIGNED8;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_6) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{				
				return DT_ERR_INVALID_TAG;
			}
			/* decode eventType */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t*)&pstEventLog->eventlogData_u.m_stNotification.m_eEventType);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_ENUM;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_7) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* decode messageText */
			i32DecodeLen = Decode_Character_String(&pu8Apdu[i32Len],u32LenValueType,
				&pstEventLog->eventlogData_u.m_stNotification.m_CharString);
			if(i32DecodeLen < 0)
			{
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;	
			}
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_CHARSTRING;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_8) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{				
				return DT_ERR_INVALID_TAG;
			}
			/* decode NotifyType */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&pstEventLog->eventlogData_u.m_stNotification.m_eNotifyType);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_ENUM;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_9) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{				
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode ackRequired */
			/* decode the application tag */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
				&u8TagNo, &u32LenValueType);
			pstEventLog->eventlogData_u.m_stNotification.m_bAckReq
				= Decode_Boolean(u32LenValueType);
			++i32Len;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_BOOLEAN;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_10) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{				
				return DT_ERR_INVALID_TAG;
			}
			/* decode fromState */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&pstEventLog->eventlogData_u.m_stNotification.m_eFromState);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_ENUM;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_11) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{				
				return DT_ERR_INVALID_TAG;
			}
			/* decode toState */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&pstEventLog->eventlogData_u.m_stNotification.m_eToState);
			i32Len += i32DecodeLen;

			/* save tag type */
			pstEventLog->m_TagType = BACNET_DT_ENUM;
		}
		else
		{
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;		
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_12))
		{

			#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_OBJ_EL || defined OLD_RP_WP_INTERFACE)
			i32DecodeLen = Decode_Notification_Parameters(&stBacnetEnPropElem,
				&pu8Apdu[i32Len], (u32EventNotifyLen - (i32Len-1)));
			#endif

			if(i32DecodeLen < 0)
			{
				i32Len = i32DecodeLen;
				return i32Len;			
			}
			i32Len += i32DecodeLen;
		}

		/* decode closing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++;
		}
		else
		{			
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
	}	
	else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the length of real to be 4 bytes only */
		if(u32LenValueType != 4)
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}
		/* decode real value */
		i32DecodeLen = Decode_Real(&pu8Apdu[i32Len],&pstEventLog->eventlogData_u.m_fTimechange);
		i32Len += i32DecodeLen;

		/* save tag type */
		pstEventLog->m_TagType = BACNET_DT_LOGBUFFER_TIMECHANGE;		
	}
	else
	{
		i32Len = DT_ERR_INVALID_TAG;
		return i32Len;
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of Event Log Record data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstEventLogRecord [out]	pointer to save event log record list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_EventLogRecord(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetEventLogRecord_t **pstEventLogRecord,
	uint32_t *pu32Count,
	uint32_t u32APDULen,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBACnetEventLogRecord_t *pstNewNode = NULL;
	ListOfBACnetEventLogRecord_t *pstBaseNode = NULL;	
	ListOfBACnetEventLogRecord_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	EventlogDatam_t stEventLogData = {0};
	BACnetDateTime_t stDATETIME = {0};
	bool bFlag = FALSE;
	uint32_t u32EventNotifyLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstEventLogRecord || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstEventLogRecord = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len += i32DecodeLen;
			i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len],
				&stDATETIME.m_stDate, &stDATETIME.m_stTime);
			/* check decode length */
			if(i32DecodeLen < 0)
			{
				/* clear allocated memories & return error */
				Clear_EventLogRecord_List(&pstBaseNode);
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;		

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_EventLogRecord_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_EventLogRecord_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			/* verify the length */
			u32EventNotifyLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], 
				u32APDULen - i32Len, ePropID);

			i32Len += i32DecodeLen;

			i32DecodeLen = DT_Decode_EventLogData(&pu8Apdu[i32Len],&stEventLogData,
				u32EventNotifyLen, ePropID);
			if(i32DecodeLen < 0)
			{			
				/* invalid data received */
				/* clear allocated memories & return error */
				Clear_EventLogRecord_List(&pstBaseNode);			
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_EventLogRecord_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}		
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_EventLogRecord_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}		

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetEventLogRecord_t *)
			OSAL_Malloc(sizeof(ListOfBACnetEventLogRecord_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* clear allocated memories & return error */
			Clear_EventLogRecord_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(&pstNewNode->m_stTimeStamp,&stDATETIME, sizeof(BACnetDateTime_t));
		memcpy(&pstNewNode->m_stlogData,&stEventLogData,sizeof(EventlogDatam_t));

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstEventLogRecord = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode characterString array data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstOptCharStrList [out]	pointer to save optional character string ;ist.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_CharString_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfCharStr_t *pstCharStrList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfCharStr_t *pstNewNode = NULL;
	ListOfCharStr_t *pstBaseNode = NULL;	
	ListOfCharStr_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	BACnetCharStr_t stCharString = {0};
	bool bFlag = FALSE;
	uint32_t u32MaxStrLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstCharStrList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pu32Count = 0;
	pstBaseNode = pstCharStrList; // 1st node has static memory 

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		/* check tag value */
		if(BACNET_APPLICATION_TAG_CHARACTER_STRING != u8TagNo)
		{
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_CharString_List(&pstBaseNode->m_pstNext);
			return DT_ERR_INVALID_TAG;
		}
		i32Len += i32DecodeLen;

		/* decode single node of character string */
		i32DecodeLen = Decode_Character_String(&pu8Apdu[i32Len], u32LenValueType,
			&stCharString);

		/* get max supported string length */
		u32MaxStrLen = ((stCharString.m_u8Encoding == CHARACTER_IBM_MS_DBCS) ?
			(MAX_SUPPORTED_CHRSTR_LEN +2) : MAX_SUPPORTED_CHRSTR_LEN);

		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_CharString_List(&pstBaseNode->m_pstNext);				
			i32Len = i32DecodeLen;
			return i32Len;
		}
		else if(u32LenValueType > u32MaxStrLen)
		{
			/* clear allocated memories & return error */
			Clear_CharString_List(&pstBaseNode->m_pstNext);
			return DT_ERR_VALUE_OUT_OF_RANGE;
		}
		i32Len += i32DecodeLen;

		if(FALSE == bFlag)
		{
			/* only copy data, no need to allocate memory for 1st node */
			pstNewNode = pstCharStrList;
		}
		else
		{
			/* allocate memory & save value */
			pstNewNode = (ListOfCharStr_t *)
				OSAL_Malloc(sizeof(ListOfCharStr_t),__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstNewNode)
			{
				/* free previously allocated memories & return error */
				Clear_CharString_List(&pstBaseNode->m_pstNext);
				return DT_ERR_MALLOC_FAILED;
			}
		}	

		/* save decoded data */
		pstNewNode->m_stCharStr.m_u32StrLen = stCharString.m_u32StrLen;
		pstNewNode->m_stCharStr.m_u16CodePage = stCharString.m_u16CodePage;
		pstNewNode->m_stCharStr.m_u8Encoding = stCharString.m_u8Encoding;
		memcpy(&pstNewNode->m_stCharStr.m_pu8CharStr[0],&stCharString.m_pu8CharStr[0], 
			stCharString.m_u32StrLen);		

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode List of property references.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstlistOfPropReff   [out]	pointer to save list of property references.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_PropertyRef_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetPropRef_t *pstlistOfPropReff,
	uint32_t u32ApduDataLen)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBACnetPropRef_t *pstNewNode = NULL;
	ListOfBACnetPropRef_t *pstBaseNode = NULL;	
	ListOfBACnetPropRef_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32PropID= 0, u32PropIndex = 0;
	bool bArrayIndexPresent = ARRAY_INDEX_ABSENT;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstlistOfPropReff)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstlistOfPropReff, 0x00, sizeof(ListOfBACnetPropRef_t));
	
	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories */
				Clear_GpSpecs_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode property id */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType,	&u32PropID);
			i32Len += i32DecodeLen;
		}
		else
		{
			Clear_GpSpecs_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* clear flag for array index before each node decode */
		bArrayIndexPresent = ARRAY_INDEX_ABSENT;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE &&
			!Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				Clear_GpSpecs_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode property Array Index */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,&u32PropIndex);
			i32Len += i32DecodeLen;
			/* set flag for array index */
			bArrayIndexPresent = ARRAY_INDEX_PRESENT;
		}

		if(FALSE == bFlag)
		{
			/* only copy data, no need to allocate memory for 1st node */
			pstNewNode = pstlistOfPropReff;
		}
		else
		{
			/* allocate memory & save value */
			pstNewNode = (ListOfBACnetPropRef_t *)
				OSAL_Malloc(sizeof(ListOfBACnetPropRef_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstNewNode)
			{
				/* free previously allocated memories & return error */
				Clear_GpSpecs_List(&pstBaseNode);
				return DT_ERR_MALLOC_FAILED;
			}
		}		

		/* save decoded data */
		pstNewNode->m_ePropertyIdentifier = u32PropID;		
		pstNewNode->m_bIsArrayIndxPresent = bArrayIndexPresent;
		if(ARRAY_INDEX_PRESENT == bArrayIndexPresent)
			pstNewNode->m_u32ArrayIndex = u32PropIndex;	

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode List of ReadAccessSpecification data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstGpOfObj   [out]	pointer to save list of ReadAccessSpecification.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_ReadAccessSpecs_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfReadAccessSpecs_t **pstGpOfObj,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfReadAccessSpecs_t *pstNewNode = NULL;
	ListOfReadAccessSpecs_t *pstBaseNode = NULL;	
	ListOfReadAccessSpecs_t *pstPreviousNode = NULL;
	ListOfReadAccessSpecs_t stReadAceesSpeci = {0};
	uint32_t u32ApduDataLen = 0 ;
	uint32_t u32Count = 0;	
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstGpOfObj || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstGpOfObj = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{		
		/* decode single node of ReadAccessSpecification */
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				/* clear allocated memories & return error */
				Clear_ReadAccessSpecs_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode device id */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&stReadAceesSpeci.m_eObjectType, &stReadAceesSpeci.m_u32ObjId);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_ReadAccessSpecs_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			/* verify the length */
			u32ApduDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len],
				u32ApduLen - i32Len, ePropID);

			i32Len += i32DecodeLen;
			/* decode the data */
			i32DecodeLen = DT_Decode_PropertyRef_List(&pu8Apdu[i32Len],u32ApduDataLen, 
				&stReadAceesSpeci.m_stlistOfPropRef, u32ApduDataLen);
			if(i32DecodeLen < 0)
			{			
				/* invalid data received */
				/* clear allocated memories & return error */
				Clear_ReadAccessSpecs_List(&pstBaseNode);			
				i32Len = i32DecodeLen;
				return i32Len;
			}			
			i32Len += i32DecodeLen;							

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_ReadAccessSpecs_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_DATA_TYPE;
				return i32Len;
			}
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_ReadAccessSpecs_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* allocate memory & save value */
		pstNewNode = (ListOfReadAccessSpecs_t *)
			OSAL_Malloc(sizeof(ListOfReadAccessSpecs_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* clear allocated memories & return error */
			Clear_ReadAccessSpecs_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(pstNewNode, &stReadAceesSpeci, sizeof(ListOfReadAccessSpecs_t));				

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstGpOfObj = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode listOfResults field .
*    
* @param pu8APDU		[in]  data to be decoded.
* @param u32ApduLen		[on]  length to be decoded.
* @param pstResults     [out] pointer to save listOfResults data.
* @param u16ObjectType	[in]  object type decoded in objectIdentifier.
* @param u32ObjId    	[in]  object id decoded in objectIdentifier.
* @param ePropID		[in]  parameter to be passed to a function
							  which verifies the length between opening and closing tags.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Results_list(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	listOfResults_t *pstResults,
	uint16_t u16ObjectType,
	uint32_t u32ObjId,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	listOfResults_t *pstNewNode = NULL;
	listOfResults_t *pstBaseNode = NULL;	
	listOfResults_t *pstPreviousNode = NULL;
	listOfResults_t stListResult= {0};
	bool bFlag = FALSE;
	uint32_t u32ApduDataLen = 0;
	uint32_t u32FirstFailedElement = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstResults)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstResults, 0x00, sizeof(listOfResults_t));

	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{		
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories */
				Clear_GpResults_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode property id */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType,	
				(uint32_t *)&stListResult.m_ePropertyIdentifier);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* clear allocated memories */
			Clear_GpResults_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}
		/* clear flag for array index before each node decode */
		stListResult.m_bArrIndxFlag = ARRAY_INDEX_ABSENT;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories */
				Clear_GpResults_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode property Array Index */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				&stListResult.m_u32ArrayIndex);
			i32Len += i32DecodeLen;
			/* set flag for array index */
			stListResult.m_bArrIndxFlag = ARRAY_INDEX_PRESENT;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_5))
		{
			i32Len += i32DecodeLen;
			/* decode error value */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,
				&u32LenValueType);
			i32Len += i32DecodeLen;
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&stListResult.m_stReadResult.m_stError.m_eErrorClass);				
			i32Len += i32DecodeLen;

			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],&u8TagNo,
				&u32LenValueType);
			i32Len += i32DecodeLen;
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&stListResult.m_stReadResult.m_stError.m_eErrorCode);				
			i32Len += i32DecodeLen;

			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_5))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories */
				Clear_GpResults_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* set union member */
			stListResult.m_u32UnionMember = TAG_NO_5;
		}
		else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_4))
		{
			/* verify the length between opening and closing tags */
			u32ApduDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len],
				u32ApduLen - i32Len, ePropID);
			i32Len += i32DecodeLen;

			i32DecodeLen = BACApp_Decode_Data_Type(&pu8Apdu[i32Len],u32ApduDataLen,
				stListResult.m_ePropertyIdentifier, u16ObjectType,
				&stListResult.m_stReadResult.m_stPropPointer.m_eData_Type,
				&stListResult.m_stReadResult.m_stPropPointer.m_pvConstrProp,
				stListResult.m_u32ArrayIndex, stListResult.m_bArrIndxFlag, 
				&u32FirstFailedElement);
			if(i32DecodeLen < 0)
			{			
				/* invalid data received */
				/* clear allocated memories & return error */
				Clear_GpResults_List(&pstBaseNode);			
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;
			/* decode closing taga for propety value */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_4))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories */
				Clear_GpResults_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* set union member */
			stListResult.m_u32UnionMember = TAG_NO_4;
		}
		else
		{
			/* clear allocated memories */
			Clear_GpResults_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}

		if(FALSE == bFlag)
		{
			/* only copy data, no need to allocate memory for 1st node */
			pstNewNode = pstResults;
		}
		else
		{
			/* allocate memory & save value */
			pstNewNode = (listOfResults_t *)
				OSAL_Malloc(sizeof(listOfResults_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstNewNode)
			{
				/* free previously allocated memories & return error */
				Clear_GpResults_List(&pstBaseNode);
				return DT_ERR_MALLOC_FAILED;
			}
		}		

		/* save decoded data */
		memcpy(pstNewNode, &stListResult, sizeof(listOfResults_t));

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;

	}/* while */

	/* return total decoded data & length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode List of ReadAccessResult data type.
*    
* @param pu8APDU		   [in]	data to be decoded.
* @param u32ApduLen		   [out]received data length.
* @param pstGpObjResults   [out]pointer to save list of ReadAccessResult.
* @param pu32Count		   [out]to save no of elements in array.
* @param ePropID		   [in]parameter to be passed to a function
								which verifies the length between opening and closing tags.                                  
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_ReadAccessResult_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfReadAccessResult_t **pstGpObjResults,
	uint32_t *pu32Count,	
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfReadAccessResult_t *pstNewNode = NULL;
	ListOfReadAccessResult_t *pstBaseNode = NULL;	
	ListOfReadAccessResult_t *pstPreviousNode = NULL;
	ListOfReadAccessResult_t stReadAccessResult= {0};
	uint32_t u32Count = 0;	
	bool bFlag = FALSE;
	uint32_t u32ApduDataLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstGpObjResults || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstGpObjResults = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{		
		/* decode single node of ReadAccessResult */		
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				Clear_ReadAccessResult_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode object id */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&stReadAccessResult.m_eObjectType, &stReadAccessResult.m_u32ObjId);
			i32Len += i32DecodeLen;
		}
		else
		{
			Clear_ReadAccessResult_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			/* verify the length */
			u32ApduDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len],
				u32ApduLen - i32Len, ePropID);

			i32Len += i32DecodeLen;
			/* decode list of results */
			i32DecodeLen = DT_Decode_Results_list(&pu8Apdu[i32Len],	u32ApduDataLen,
				&stReadAccessResult.m_stListOfResults,stReadAccessResult.m_eObjectType,
				stReadAccessResult.m_u32ObjId,ePropID);
			if(i32DecodeLen < 0)
			{			
				/* invalid data received */
				/* clear allocated memories & return error */
				Clear_ReadAccessResult_List(&pstBaseNode);				
				i32Len = i32DecodeLen;
				return i32Len;
			}			
			i32Len += i32DecodeLen;
			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_ReadAccessResult_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}		
		}

		/* allocate memory & save value */
		pstNewNode = (ListOfReadAccessResult_t *)
			OSAL_Malloc(sizeof(ListOfReadAccessResult_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_ReadAccessResult_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(&pstNewNode->m_stListOfResults, &stReadAccessResult.m_stListOfResults, 
			sizeof(listOfResults_t));
		pstNewNode->m_eObjectType = stReadAccessResult.m_eObjectType;
		pstNewNode->m_u32ObjId = stReadAccessResult.m_u32ObjId;							

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstGpObjResults = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode NotificationPriority (BACnetARRAY[3] of Unsigned)
*    
* @param pu8APDU		   [in]	data to be decoded.
* @param u32ApduLen		   [in]	length of data to be decoded.
* @param pstNotifyPriority [out]pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_NotificationPriority(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetNotifyPriority_t *pstNotifyPriority)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint8_t u8IndexCount = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstNotifyPriority)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0 && u8IndexCount < BACNET_ARRAY_OF_THREE)
	{
		/* decode application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNo ||
			u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode array index */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstNotifyPriority->m_u32Value[u8IndexCount]);		
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* increment the index count */
		u8IndexCount++;
	}

	/* return total decoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode List of BACnetActionCommand.
*    
* @param pu8APDU		   [in]	data to be decoded.
* @param u32ApduLen		   [out]received data length.
* @param pstAction_List    [out]pointer to save list of BACnetActionList.
* @param pu32Count		   [out]to save no of elements in array.
* @param ePropID		   [in]parameter to be passed to a function
								which verifies the length between opening and closing tags.                                  
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Action_Command(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetActionCommand_t **pstAction_Command,
	BACNET_PROPERTY_ID ePropID
	)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBACnetActionCommand_t *pstNewNode = NULL;
	ListOfBACnetActionCommand_t *pstBaseNode = NULL;	
	ListOfBACnetActionCommand_t *pstPreviousNode = NULL;
	ListOfBACnetActionCommand_t  stAction_Command = {0};
	bool bFlag = FALSE;
	uint32_t u32ApduDataLen = 0;
	uint32_t u32FirstFailedElement = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAction_Command)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstAction_Command = NULL;	

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode single node of ReadAccessResult */		
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo,
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode device id */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&stAction_Command.m_eDeviceType,&stAction_Command.m_u32DevId);
			i32Len += i32DecodeLen;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType != 4)
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode object id */
			i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
				(uint32_t *)&stAction_Command.m_eObjectType,&stAction_Command.m_u32ObjId);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_ActionCmd_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo,
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode property id */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType,	
				(uint32_t *)&stAction_Command.m_ePropertyIdentifier);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_ActionCmd_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* clear flag for array index before each node decode */
		stAction_Command.m_bArrIndxFlag = ARRAY_INDEX_ABSENT;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode property Array Index */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				&stAction_Command.m_u32ArrayIndex);
			i32Len += i32DecodeLen;
			/* set flag for array index */
			stAction_Command.m_bArrIndxFlag = ARRAY_INDEX_PRESENT;
		}
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_4))
		{			
			/* verify the length between opening and closing tags */
			u32ApduDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len],
				u32ApduLen - i32Len, ePropID);
			i32Len += i32DecodeLen;

			i32DecodeLen = BACApp_Decode_Data_Type(&pu8Apdu[i32Len],u32ApduDataLen,
				stAction_Command.m_ePropertyIdentifier, stAction_Command.m_eObjectType,
				&stAction_Command.m_stPropPointer.m_eData_Type,
				&stAction_Command.m_stPropPointer.m_pvConstrProp,
				stAction_Command.m_u32ArrayIndex, stAction_Command.m_bArrIndxFlag, 
				&u32FirstFailedElement);
			if(i32DecodeLen < 0)
			{			
				/* invalid data received */
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);				
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;
			/* decode closing taga for propety value */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_4))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}			
		}
		else
		{
			/* invalid data type */
			/* clear allocated memories & return error */
			Clear_ActionCmd_List(&pstBaseNode);
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* clear flag for array index before each node decode */
		stAction_Command.m_bPriorityFlag = false;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_5) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode priority */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,	
				&stAction_Command.m_u32Priority);
			i32Len += i32DecodeLen;
			stAction_Command.m_bPriorityFlag = true;
		}

		/* clear flag for array index before each node decode */
		stAction_Command.m_bPostDelayFlag = false;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_6) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode post delay */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,	
				&stAction_Command.m_u32PostDelay);
			i32Len += i32DecodeLen;
			stAction_Command.m_bPostDelayFlag = true;
		}
		
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_7) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{				
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode boolean value */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
			stAction_Command.m_bQuitOnFailure = Decode_Boolean(u32LenValueType);
			++i32Len;
		}
		else
		{
			/* invalid data type */
			/* clear allocated memories & return error */
			Clear_ActionCmd_List(&pstBaseNode);
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_8) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{				
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
			/* decode boolean value */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
			stAction_Command.m_bWriteSuccess = Decode_Boolean(u32LenValueType);
			++i32Len;
		}
		else
		{
			/* invalid data type */
			/* clear allocated memories & return error */
			Clear_ActionCmd_List(&pstBaseNode);
			return DT_ERR_INVALID_DATA_TYPE;
		}		

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetActionCommand_t *)
			OSAL_Malloc(sizeof(ListOfBACnetActionCommand_t),__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* clear allocated memories & return error */
			Clear_ActionCmd_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(pstNewNode, &stAction_Command, sizeof(ListOfBACnetActionCommand_t));	
		/* set default values */
		memset(&stAction_Command, 0x00, sizeof(ListOfBACnetActionCommand_t));

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstAction_Command = pstBaseNode;	
	
	return i32Len;
}

#ifndef PROFILE_BOD_ONLY
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode List of BACnetActionList data type.
*    
* @param pu8APDU		   [in]	data to be decoded.
* @param u32ApduLen		   [out]received data length.
* @param pstAction_List    [out]pointer to save list of BACnetActionList.
* @param pu32Count		   [out]to save no of elements in array.
* @param ePropID		   [in]parameter to be passed to a function
								which verifies the length between opening and closing tags.                                  
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Action_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetActionList_t **pstAction_List,
	uint32_t *pu32Count,	
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	ListOfBACnetActionList_t *pstNewNode = NULL;
	ListOfBACnetActionList_t *pstBaseNode = NULL;	
	ListOfBACnetActionList_t *pstPreviousNode = NULL;
	ListOfBACnetActionCommand_t  *pstAction_Command= {0};
	uint32_t u32Count = 0;	
	bool bFlag = FALSE;
	uint32_t u32ApduDataLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAction_List || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstAction_List = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			/* verify the length */
			u32ApduDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len],
				u32ApduLen - i32Len, ePropID);

			i32Len += i32DecodeLen;
			/* decode single node of BACnetActionList (list of BACnetActionCommand)*/		
			i32DecodeLen = DT_Decode_Action_Command(&pu8Apdu[i32Len], 
				u32ApduDataLen, &pstAction_Command, ePropID);
			if(i32DecodeLen < 0)
			{
				/* invalid data received */
				/* free previously allocated memories & return error */
				Clear_Action_List(&pstBaseNode);
				i32Len = i32DecodeLen;
				return i32Len;
			}
			else
			{
				i32Len += i32DecodeLen;
				/* allocate memory & save value */
				pstNewNode = (ListOfBACnetActionList_t *)
					OSAL_Malloc(sizeof(ListOfBACnetActionList_t),
					__FILE__,__FUNCTION__,__LINE__);
				if(NULL == pstNewNode)
				{
					/* free previously allocated memories & return error */
					Clear_ActionCmd_List(&pstAction_Command);
					Clear_Action_List(&pstBaseNode);
					return DT_ERR_MALLOC_FAILED;
				}
				else
				{
					pstNewNode->m_pstActionCommand = pstAction_Command;
				}
			}
			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* clear allocated memories & return error */
				Clear_ActionCmd_List(&pstAction_Command);
				Clear_Action_List(&pstBaseNode);
				OSAL_Free(pstNewNode, __FILE__, __FUNCTION__, __LINE__);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}
		}
		else
		{
			/* clear allocated memories & return error */
			Clear_Action_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pstAction_List = pstBaseNode;
	*pu32Count = u32Count;
	
	return i32Len;
}
#endif /* !PROFILE_BOD_ONLY */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode BACnetAuthenticationFactor datatype.
*    
* @param pu8APDU		   [in]	data to be decoded.
* @param u32ApduLen		   [in]	length of data to be decoded.
* @param pstAuthFactor     [out]pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AuthenticationFactor(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	BACnetAuFactor_t *pstAuthFactor)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	BACnetOctetStr_t      stOctetStrValue = {0};

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAuthFactor)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	
	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode format type */
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
			(uint32_t *)&pstAuthFactor->m_eFormatType);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* check the application tag type */
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode format class */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
			&pstAuthFactor->m_u32FormatClass);
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* decode value */
		i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len], u32LenValueType, 
			&stOctetStrValue);

		memcpy(&pstAuthFactor->m_stOctetStrValue,&stOctetStrValue,sizeof(BACnetOctetStr_t));
		
		if(i32DecodeLen < 0)
		{
			/* invalid data received */							
			i32Len = i32DecodeLen;
			return i32Len;
		}	
		i32Len += i32DecodeLen;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data & length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetAuthenticationFactorFormat datatype.
*    
* @param pu8APDU				  [in]	data to be decoded.
* @param u32ApduLen				  [out]	received data length.
* @param pstAuthFactFormat_Array  [out]	pointer to save authentication factor format array.
* @param pu32Count				  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AuthFactorFormat_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAuFactorFormat_t **pstAuthFactFormat_Array,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfBACnetAuFactorFormat_t *pstBaseNode = NULL;
	ListOfBACnetAuFactorFormat_t *pstNewNode = NULL;
	ListOfBACnetAuFactorFormat_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;
	ListOfBACnetAuFactorFormat_t stAuFactorFormat = {0};
	uint32_t u32DecodeVal = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAuthFactFormat_Array || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstAuthFactFormat_Array = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_AuFactorFormat_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode format type */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&stAuFactorFormat.m_eFormatType);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AuFactorFormat_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* clear the flag for vendor id at each node before decoding */
		stAuFactorFormat.m_bVIdFlag = false;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_AuFactorFormat_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode vendor id */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				(uint32_t *)&u32DecodeVal);			
			i32Len += i32DecodeLen;

			stAuFactorFormat.m_u16VendorId = (uint16_t)u32DecodeVal;

			/* set the flag for vendor id*/
			stAuFactorFormat.m_bVIdFlag = true;
		}

		/* clear the flag for vendor format at each node before decoding */
		stAuFactorFormat.m_bVFormatFlag = false;
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_AuFactorFormat_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode vendor format */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				(uint32_t *)&u32DecodeVal);			
			i32Len += i32DecodeLen;

			stAuFactorFormat.m_u16VendorFormat = (uint16_t)u32DecodeVal;

			/* set the flag for vendor id*/
			stAuFactorFormat.m_bVFormatFlag = true;
		}		

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetAuFactorFormat_t *)OSAL_Malloc(sizeof(ListOfBACnetAuFactorFormat_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_AuFactorFormat_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		pstNewNode->m_eFormatType = stAuFactorFormat.m_eFormatType;
		pstNewNode->m_u16VendorId = stAuFactorFormat.m_u16VendorId;
		pstNewNode->m_bVIdFlag = stAuFactorFormat.m_bVIdFlag;
		pstNewNode->m_u16VendorFormat = stAuFactorFormat.m_u16VendorFormat;
		pstNewNode->m_bVFormatFlag = stAuFactorFormat.m_bVFormatFlag;
		
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstAuthFactFormat_Array = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetCredentialAuthenticationFactor datatype.
*    
* @param pu8APDU				  [in]	data to be decoded.
* @param u32ApduLen				  [out]	received data length.
* @param pstCredentialAuthFactor  [out]	pointer to save crede.Auth.factor list.
* @param pu32Count				  [out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_CredentialAuthFactor_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetCredAuFactor_t **pstCredentialAuthFactor,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfBACnetCredAuFactor_t *pstBaseNode = NULL;
	ListOfBACnetCredAuFactor_t *pstNewNode = NULL;
	ListOfBACnetCredAuFactor_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;
	ListOfBACnetCredAuFactor_t stCredAuthFactor = {0};	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstCredentialAuthFactor || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstCredentialAuthFactor = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_CredentialAuFactor_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode disable */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&stCredAuthFactor.m_eDisable);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_CredentialAuFactor_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len += i32DecodeLen;
			i32DecodeLen = DT_Decode_AuthenticationFactor(&pu8Apdu[i32Len],
				u32ApduLen,&stCredAuthFactor.m_stAuFactor);
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_CredentialAuFactor_List(&pstBaseNode);
				/* return the error code */
				i32Len = i32DecodeLen;
				return i32Len;
			}
			
			/* increment by decoded data length */
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_CredentialAuFactor_List(&pstBaseNode);
				/* invalid tag */
				return DT_ERR_INVALID_TAG;
			}		
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_CredentialAuFactor_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}	
				

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetCredAuFactor_t *)OSAL_Malloc(sizeof(ListOfBACnetCredAuFactor_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_CredentialAuFactor_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		pstNewNode->m_eDisable = stCredAuthFactor.m_eDisable;
		memcpy(&pstNewNode->m_stAuFactor, &stCredAuthFactor.m_stAuFactor, 
			sizeof(BACnetAuFactor_t));
		
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstCredentialAuthFactor = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode policy list.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstPolicy    [out]	pointer to save list of enum values.
* @param pu32Count	  [out]	to save no of elements in array.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_Policy_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	Policy_t *pstPolicy)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;	
	Policy_t *pstNewNode = NULL;
	Policy_t *pstBaseNode = NULL;	
	Policy_t *pstPreviousNode = NULL;
	Policy_t stPolicy = {0};	
	bool bFlag = FALSE;	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstPolicy)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstPolicy, 0x00, sizeof(Policy_t));

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len += i32DecodeLen;
			/* decode single node of device-object-reference */		
			i32DecodeLen = DT_Decode_DevObjReff(&pu8Apdu[i32Len],
				&stPolicy.m_stCredentialDataInput);			
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_Policy_List(&pstBaseNode->m_pstNext);
				/* invalid data received */
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_Policy_List(&pstBaseNode->m_pstNext);
				/* invalid tag */
				return DT_ERR_INVALID_TAG;
			}
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_Policy_List(&pstBaseNode->m_pstNext);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_Policy_List(&pstBaseNode->m_pstNext);
				return DT_ERR_INVALID_TAG;
			}
			/* decode index */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				&stPolicy.m_u32Index);			
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_Policy_List(&pstBaseNode->m_pstNext);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		if(FALSE == bFlag)
		{
			/* only copy data, no need to allocate memory for 1st node */
			pstNewNode = pstPolicy;
		}
		else
		{
			/* allocate memory & save value */
			pstNewNode = (Policy_t *)
				OSAL_Malloc(sizeof(Policy_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstNewNode)
			{
				/* free previously allocated memories & return error */
				Clear_Policy_List(&pstBaseNode->m_pstNext);
				return DT_ERR_MALLOC_FAILED;
			}
		}
		
		/* save decoded data */
		pstNewNode->m_u32Index = stPolicy.m_u32Index;
		memcpy(&pstNewNode->m_stCredentialDataInput, &stPolicy.m_stCredentialDataInput, 
			sizeof(BACnetDevObjRef_t));		

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of BACnetAuthenticationPolicy datatype.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [out]	received data length.
* @param pstAuthPolicy   [out]	pointer to save Authentication policy list.
* @param pu32Count		 [out]	to save no of elements in list.
* @param ePropID		 [in]	property id to be passed to a function finding data length
								between opening and closing tags.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AuthenticationPolicy_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAuPolicy_t **pstAuthPolicy,
	uint32_t *pu32Count,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfBACnetAuPolicy_t *pstBaseNode = NULL;
	ListOfBACnetAuPolicy_t *pstNewNode = NULL;
	ListOfBACnetAuPolicy_t *pstPreviousNode = NULL;
	bool bFlag = FALSE;
	uint32_t u32Count = 0;
	ListOfBACnetAuPolicy_t stAuthenticationPolicy = {0};	
	uint32_t u32ApduDataLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAuthPolicy || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstAuthPolicy = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			/* verify the length */
			u32ApduDataLen = BACApp_Verify_Data_Len(&pu8Apdu[i32Len],
				u32ApduLen - i32Len, ePropID);

			i32Len += i32DecodeLen;
			/* decode list of policy */		
			i32DecodeLen = DT_Decode_Policy_List(&pu8Apdu[i32Len], 
				u32ApduDataLen, &stAuthenticationPolicy.m_stPolicy);
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_AuPolicy_List(&pstBaseNode);			
				/* invalid data received */
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_AuPolicy_List(&pstBaseNode);
				/* invalid tag */
				return DT_ERR_INVALID_TAG;
			}
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AuPolicy_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{				
				i32Len = DT_ERR_INVALID_TAG;
				Clear_AuPolicy_List(&pstBaseNode);
				return i32Len;
			}
			/* decode order enforced */
			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
			stAuthenticationPolicy.m_bOrderEnforced = Decode_Boolean(u32LenValueType);
			++i32Len;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AuPolicy_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_AuPolicy_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode timeout */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType,
				&stAuthenticationPolicy.m_u32Timeout);			
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AuPolicy_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetAuPolicy_t *)OSAL_Malloc(sizeof(ListOfBACnetAuPolicy_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_AuPolicy_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		pstNewNode->m_bOrderEnforced = stAuthenticationPolicy.m_bOrderEnforced;
		pstNewNode->m_u32Timeout = stAuthenticationPolicy.m_u32Timeout;		
		memcpy(&pstNewNode->m_stPolicy,&stAuthenticationPolicy.m_stPolicy, 
			sizeof(Policy_t));
		
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstAuthPolicy = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetAssignedAccessRights datatype.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [out]	received data length.
* @param pstAccesssRights[out]	pointer to save Assigned access rights.
* @param pu32Count		 [out]	to save no of elements in list.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AssAccessRights_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAssignedAccessRights_t **pstAccesssRights,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfBACnetAssignedAccessRights_t *pstBaseNode = NULL;
	ListOfBACnetAssignedAccessRights_t *pstNewNode = NULL;
	ListOfBACnetAssignedAccessRights_t *pstPreviousNode = NULL;	
	bool bFlag = FALSE;
	uint32_t u32Count = 0;
	ListOfBACnetAssignedAccessRights_t stAssignedAccessRights = {0};	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAccesssRights || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstAccesssRights = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len += i32DecodeLen;
			/* decode device-object-reference */		
			i32DecodeLen = DT_Decode_DevObjReff(&pu8Apdu[i32Len],
				&stAssignedAccessRights.m_stAssAccessRights);			
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_AssignedAccessRights_List(&pstBaseNode);
				/* invalid data received */
				i32Len = i32DecodeLen;
				return i32Len;;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_AssignedAccessRights_List(&pstBaseNode);
				/* invalid tag */
				return DT_ERR_INVALID_TAG;
			}
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AssignedAccessRights_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{
				/* free previously allocated memories & return error */
				Clear_AssignedAccessRights_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}

			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
			/* decode enable */
			stAssignedAccessRights.m_bEnable = Decode_Boolean(u32LenValueType);
			++i32Len;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AssignedAccessRights_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}		

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetAssignedAccessRights_t *)OSAL_Malloc(sizeof(ListOfBACnetAssignedAccessRights_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_AssignedAccessRights_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		pstNewNode->m_bEnable = stAssignedAccessRights.m_bEnable;				
		memcpy(&pstNewNode->m_stAssAccessRights,&stAssignedAccessRights.m_stAssAccessRights, 
			sizeof(BACnetDevObjRef_t));
		
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstAccesssRights = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetAccessRule datatype.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [out]	received data length.
* @param pstAccessRule   [out]	pointer to save array of access rule.
* @param pu32Count		 [out]	to save no of elements in list.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_AccessRule_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetAccessRule_t **pstAccessRule,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	ListOfBACnetAccessRule_t *pstBaseNode = NULL;
	ListOfBACnetAccessRule_t *pstNewNode = NULL;
	ListOfBACnetAccessRule_t *pstPreviousNode = NULL;	
	bool bFlag = FALSE;
	uint32_t u32Count = 0;
	ListOfBACnetAccessRule_t stAccessRules = {0};	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAccessRule || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstAccessRule = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* clear allocated memories & return error */
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode timeRangeSpecifier */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&stAccessRules.m_eTimeRangeType);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AccessRule_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len += i32DecodeLen;
			/* decode device-object-property-reference (timeRange)*/		
			i32DecodeLen = DT_Decode_DevObjPropReff(&pu8Apdu[i32Len],
				&stAccessRules.m_stTimeRange);			
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				/* invalid data received */
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				/* invalid tag */
				return DT_ERR_INVALID_TAG;
			}
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
			/* decode locationSpecifier */
			i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
				(uint32_t *)&stAccessRules.m_eLocationType);
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AccessRule_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_3))
		{
			i32Len += i32DecodeLen;
			/* decode device-object-reference (location)*/		
			i32DecodeLen = DT_Decode_DevObjReff(&pu8Apdu[i32Len],
				&stAccessRules.m_stLocation);			
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				/* invalid data received */
				i32Len = i32DecodeLen;
				return i32Len;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_3))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				/* invalid tag */
				return DT_ERR_INVALID_TAG;
			}
		}		

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_4) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(1 != u32LenValueType && 0 != u32LenValueType)
			{
				/* free previously allocated memories & return error */
				Clear_AccessRule_List(&pstBaseNode);
				i32Len = DT_ERR_INVALID_TAG;
				return i32Len;
			}

			i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, 
			&u32LenValueType);
			/* decode enable */
			stAccessRules.m_bEnable = Decode_Boolean(u32LenValueType);
			++i32Len;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_AccessRule_List(&pstBaseNode);
			/* invalid data type */
			return DT_ERR_INVALID_DATA_TYPE;
		}		

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetAccessRule_t *)OSAL_Malloc(sizeof(ListOfBACnetAccessRule_t), 
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_AccessRule_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		pstNewNode->m_eTimeRangeType = stAccessRules.m_eTimeRangeType;
		memcpy(&pstNewNode->m_stTimeRange,&stAccessRules.m_stTimeRange, 
			sizeof(BACnetDevObjPropRef_t));
		pstNewNode->m_eLocationType = stAccessRules.m_eLocationType;						
		memcpy(&pstNewNode->m_stLocation,&stAccessRules.m_stLocation, 
			sizeof(BACnetDevObjRef_t));
		pstNewNode->m_bEnable = stAccessRules.m_bEnable;
		
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstAccessRule = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Property value datatype.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [out]	received data length.
* @param pstPropVal		 [out]	pointer to save property value.
* @param eObjType		 [out]	object type.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_PropertyValue(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	property_value_t *pstPropVal,
	BACNET_OBJECT_TYPE eObjType)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	uint32_t u32FirstFailedElement = 0;
	void *pvPropVal = NULL;
	
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstPropVal)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstPropVal, 0x00, sizeof(property_value_t));

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Property ID */	
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
			(uint32_t *)&pstPropVal->m_eObjectProperty);
		i32Len += i32DecodeLen;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* save default value for array index flag */
	pstPropVal->m_bIsArrayIndxPresent = ARRAY_INDEX_ABSENT;

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Property Array Index */	
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			(uint32_t *)&pstPropVal->m_i32PropertyArrayIndex);
		i32Len += i32DecodeLen;
		pstPropVal->m_bIsArrayIndxPresent = ARRAY_INDEX_PRESENT;
	}

	/* check opening tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		/* get property value data length */
		u32LenValueType = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], 
			(u32ApduLen - i32Len), pstPropVal->m_eObjectProperty);
		i32Len += i32DecodeLen;

		/* decode property value */
		i32DecodeLen = BACApp_Decode_Data_Type(&pu8Apdu[i32Len], u32LenValueType, 
			pstPropVal->m_eObjectProperty,
			eObjType, 
			&pstPropVal->m_eData_Type,
			&pvPropVal,
			pstPropVal->m_i32PropertyArrayIndex,
			pstPropVal->m_bIsArrayIndxPresent, &u32FirstFailedElement);

		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			// TODO - check if memory needs to be freed 
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;
		/* save pointer */
		#ifdef NEW_RP_WP_INTERFACE
		pstPropVal->m_pvPropVal = pvPropVal;
		#endif

		/* decode closing tag of logDatum choice */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}	
	}

	/* check if priority is present */
	if(i32Len < (int32_t)u32ApduLen)
	{
		/* decode the tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
		{
			i32Len += i32DecodeLen;
			if(u32LenValueType < 1 || u32LenValueType > 4)
			{
				return DT_ERR_INVALID_TAG;
			}
			/* decode Priority */	
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, &u32Value);
			pstPropVal->m_u8Priority = (uint8_t)u32Value;
			i32Len += i32DecodeLen;
		}
	}

	/* return total decoded data length */
	return i32Len;
}
/**
*
* DESCRIPTION
* Function to decode host address data type.
*
* @param pu8APDU		[in]	data to be decoded.
* @param pstHostAddress	[out]	pointer to save decoded data.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
*/
int32_t DT_Decode_HostAddress(
	uint8_t *pu8Apdu,
	BACnetHostAddress_t *pstHostAddress)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32MaxStrLen = 0;
	uint8_t u8TagNo = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Decode_HostAddress: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstHostAddress)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstHostAddress, 0x00, sizeof(BACnetHostAddress_t));

	/* decode the application tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
		&u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{
		i32Len += i32DecodeLen;
		/* set host type as none */
		pstHostAddress->m_eDataType = BACNET_DT_NULL_CONTEXT;
	}
	else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1))
	{
		i32Len += i32DecodeLen;
		/* decode ip address */
		i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len],
			u32LenValueType, &pstHostAddress->BACnetHostAddr_u.m_stIPAddress);

		/* check length */
		if(pstHostAddress->BACnetHostAddr_u.m_stIPAddress.
			m_u32OctetCount > MAX_OCTET_STRING_BYTES)
		{
			/* max length exceeded */
			return DT_ERR_VALUE_OUT_OF_RANGE;
		}
		i32Len += i32DecodeLen;

		/* set host type as ip address */
		pstHostAddress->m_eDataType = BACNET_DT_OCTETSTRING;
	}
	else if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2))
	{
		i32Len += i32DecodeLen;
		/* decode name */
		i32DecodeLen = Decode_Character_String(&pu8Apdu[i32Len],
			u32LenValueType, &pstHostAddress->BACnetHostAddr_u.m_stName);

		/* check length */
		u32MaxStrLen = ((pstHostAddress->BACnetHostAddr_u.m_stName.m_u8Encoding == CHARACTER_IBM_MS_DBCS) ?
			(MAX_SUPPORTED_CHRSTR_LEN +2) : MAX_SUPPORTED_CHRSTR_LEN);
		if(u32LenValueType > u32MaxStrLen)
		{
			/* max length exceeded */
			return DT_ERR_VALUE_OUT_OF_RANGE;
		}
		i32Len += i32DecodeLen;

		/* set host type as name */
		pstHostAddress->m_eDataType = BACNET_DT_CHARSTRING;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Decode_HostAddress: Exit \r\n");
	#endif
	/* return total decoded data length */
	return i32Len;
}
/**
*
* DESCRIPTION
* Function to decode host n port data type.
*
* @param pu8APDU		[in]	data to be decoded.
* @param pstHostNPort	[out]	pointer to save decoded data.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
*/
int32_t DT_Decode_HostNPort(
	uint8_t *pu8Apdu,
	BACnetHostNPort_t *pstHostNPort)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	uint8_t u8TagNo = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Decode_HostNPort: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstHostNPort)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* check opening tag */
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
	{
		i32Len++;
		/* decode the host address */
		i32DecodeLen = DT_Decode_HostAddress(&pu8Apdu[i32Len],
			&pstHostNPort->m_stHostAddress);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* check opening tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len++;
		}
		else
		{
			/* invalid closing tag */
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
		&u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1))
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType < 1 || u32LenValueType > 2)
		{
			/* incorrect range */
			return DT_ERR_VALUE_OUT_OF_RANGE;
		}
		/* decode port */
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],
			u32LenValueType, &u32Value);
		pstHostNPort->m_u16Port = (uint16_t)u32Value;
		i32Len += i32DecodeLen;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Decode_HostNPort: Exit \r\n");
	#endif
	/* return total decoded data length */
	return i32Len;
}
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of property value datatype.
*    
* @param pu8APDU			[in]	data to be decoded.
* @param u32ApduLen			[out]	received data length.
* @param pstPropValueList	[out]	pointer to save list of property value.
* @param eObjType			[out]	object type.
* @param pu32Count			[out]	to save no of elements in list.
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
/* Function to decode Property value list */
int32_t DT_Decode_PropValue_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	property_value_t *pstPropValueList,
	BACNET_OBJECT_TYPE eObjType,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	property_value_t *pstNewNode = NULL;
	property_value_t *pstBaseNode = NULL;	
	property_value_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	property_value_t stPropertyValue = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstPropValueList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}	

	/* set default values */
	memset(pstPropValueList, 0x00, sizeof(property_value_t));
	pstBaseNode = pstPropValueList;
	*pu32Count = 0;

	/* decode list of values */
	while(((int32_t)((u32ApduLen) - ((uint32_t)i32Len))) > 0)
	{
		/* decode property value */
		i32DecodeLen = DT_Decode_PropertyValue(&pu8Apdu[i32Len], 
			u32ApduLen, &stPropertyValue, eObjType);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_Prop_Val(&pstBaseNode->m_pstNextVal);
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		if(FALSE == bFlag)
		{
			/* only copy data, no need to allocate memory for 1st node */
			pstNewNode = pstPropValueList;
		}
		else
		{
			/* allocate memory & save value */
			pstNewNode = (property_value_t *)
				OSAL_Malloc(sizeof(property_value_t),__FILE__,__FUNCTION__,__LINE__);

			if(NULL == pstNewNode)
			{
				/* free previously allocated memories & return error */
				Clear_Prop_Val(&pstBaseNode->m_pstNextVal);
				return DT_ERR_MALLOC_FAILED;
			}
		}

		/* save decoded data */
		memcpy(pstNewNode, &stPropertyValue, sizeof(property_value_t));
		pstNewNode->m_pstNextVal = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNextVal = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}
	
	/* return total decoded data & length */
	*pu32Count = u32Count;
	return i32Len;
}
/**
*
* DESCRIPTION
* Function to decode list of bdt entry data type.
*
* @param pu8APDU				[in]	data to be decoded.
* @param u32ApduLen				[in]	received data length.
* @param pstBdtEntryList		[out]	pointer to save decoded data.
* @param pu32Count				[out]	to save no of elements in list.
* @param pu32FirstFailedElement	[out]	entry at which decoding failed.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
*/
int32_t DT_Decode_BdtEntry_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	ListOfBACnetBDTEntry_t **pstBdtEntryList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	bool bFlag = FALSE;
	uint32_t u32LenValueType = 0;
	uint32_t u32Count = 0;
	ListOfBACnetBDTEntry_t *pstBaseNode = NULL;
	ListOfBACnetBDTEntry_t *pstNewNode = NULL;
	ListOfBACnetBDTEntry_t *pstPreviousNode = NULL;
	BACnetBDTEntry_t stBDTData = {0};

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Decode_BdtEntry_List: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstBdtEntryList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstBdtEntryList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
			&u8TagNo, &u32LenValueType);

		if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len += i32DecodeLen;
			/* decode HostNPort */
			i32DecodeLen = DT_Decode_HostNPort(&pu8Apdu[i32Len],
				&stBDTData.m_stBBMDAddress);
			if(i32DecodeLen < 0)
			{
				/* free previously allocated memories & return error */
				Clear_BdtEntry_List(&pstBaseNode);
				return i32DecodeLen;
			}
			i32Len += i32DecodeLen;

			/* decode closing tag */
			if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_0))
			{
				i32Len++;
			}
			else
			{
				/* free previously allocated memories & return error */
				Clear_BdtEntry_List(&pstBaseNode);
				return DT_ERR_INVALID_TAG;
			}
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_BdtEntry_List(&pstBaseNode);
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* set default value for flag */
		stBDTData.m_bBrdcastMaskFlag = false;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
			&u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len += i32DecodeLen;
			/* decode array index */
			i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len],
				u32LenValueType, &stBDTData.m_stBrdcastMask);

			/* check range */
			if(stBDTData.m_stBrdcastMask.m_u32OctetCount > MAX_OCTET_STRING_BYTES)
			{
				/* free previously allocated memories & return error */
				Clear_BdtEntry_List(&pstBaseNode);
				return DT_ERR_VALUE_OUT_OF_RANGE;
			}
			i32Len += i32DecodeLen;

			/* set array index flag */
			stBDTData.m_bBrdcastMaskFlag = true;
		}
		else
		{
			/* set defaukt mask as 32 bit */
			stBDTData.m_stBrdcastMask.m_u32OctetCount = MAX_IP_LEN;
			stBDTData.m_stBrdcastMask.m_ai8OctetStr[0] = UINT8_MAX;
			stBDTData.m_stBrdcastMask.m_ai8OctetStr[1] = UINT8_MAX;
			stBDTData.m_stBrdcastMask.m_ai8OctetStr[2] = UINT8_MAX;
			stBDTData.m_stBrdcastMask.m_ai8OctetStr[3] = UINT8_MAX;
		}

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetBDTEntry_t *)OSAL_Malloc(
			sizeof(ListOfBACnetBDTEntry_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_BdtEntry_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */
		memcpy(&pstNewNode->m_stBDTEntry, &stBDTData, sizeof(BACnetBDTEntry_t));
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data */
	*pstBdtEntryList = pstBaseNode;
	*pu32Count = u32Count;

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Decode_BdtEntry_List: Exit \r\n");
	#endif
	/* return total decoded data length */
	return i32Len;
}

/**
*
* DESCRIPTION
* Function to decode list of fdt entry data type.
*
* @param pu8APDU				[in]	data to be decoded.
* @param u32ApduLen				[in]	received data length.
* @param pstFdtEntryList		[out]	pointer to save decoded data.
* @param pu32Count				[out]	to save no of elements in list.
* @param pu32FirstFailedElement	[out]	entry at which decoding failed.
*
* @return [out] returns no of bytes decoded on success else -ve error value.
*
*/
int32_t DT_Decode_FdtEntry_List(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen,
	ListOfBACnetFDTEntry_t **pstFdtEntryList,
	uint32_t *pu32Count,
	uint32_t *pu32FirstFailedElement)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	bool bFlag = FALSE;
	uint32_t u32LenValueType = 0;
	uint32_t u32Count = 0;
	uint32_t u32Value = 0;
	ListOfBACnetFDTEntry_t *pstBaseNode = NULL;
	ListOfBACnetFDTEntry_t *pstNewNode = NULL;
	ListOfBACnetFDTEntry_t *pstPreviousNode = NULL;
	BACnetFDTEntry_t stFDTData = {0};

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Decode_FdtEntry_List: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstFdtEntryList || NULL == pu32Count
		|| NULL == pu32FirstFailedElement)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstFdtEntryList = NULL;
	*pu32Count = 0;
	*pu32FirstFailedElement = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* increment first failed element count */
		(*pu32FirstFailedElement)++;

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
			&u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0))
		{
			i32Len += i32DecodeLen;
			/* decode ip address */
			i32DecodeLen = Decode_OCTET_String(&pu8Apdu[i32Len],
				u32LenValueType, &stFDTData.m_stIPAddress);

			/* check range */
			if(stFDTData.m_stIPAddress.m_u32OctetCount > MAX_OCTET_STRING_BYTES)
			{
				/* free previously allocated memories & return error */
				Clear_FdtEntry_List(&pstBaseNode);
				return DT_ERR_VALUE_OUT_OF_RANGE;
			}
			i32Len += i32DecodeLen;

			/* convert and save port no */
			memcpy(&stFDTData.m_u16PortNo, &stFDTData.m_stIPAddress.
				m_ai8OctetStr[4], MAX_PORT_LEN);
			stFDTData.m_u16PortNo = Swap_Unsigned16_Bytes(stFDTData.m_u16PortNo);
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_FdtEntry_List(&pstBaseNode);
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
			&u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 2)
			{
				/* free previously allocated memories & return error */
				Clear_FdtEntry_List(&pstBaseNode);
				return DT_ERR_VALUE_OUT_OF_RANGE;
			}
			/* decode time to live */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],
				u32LenValueType, &u32Value);
			stFDTData.m_u16TimeToLive = (uint16_t) u32Value;
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_FdtEntry_List(&pstBaseNode);
			i32Len = DT_ERR_INVALID_DATA_TYPE;
			return i32Len;
		}

		/* decode the application tag */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len],
			&u8TagNo, &u32LenValueType);
		if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2))
		{
			i32Len += i32DecodeLen;
			/* check the application tag type */
			if(u32LenValueType < 1 || u32LenValueType > 2)
			{
				/* free previously allocated memories & return error */
				Clear_FdtEntry_List(&pstBaseNode);
				return DT_ERR_VALUE_OUT_OF_RANGE;
			}
			/* decode remaining time to live */
			i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len],
				u32LenValueType, &u32Value);
			stFDTData.m_u16RemainingTimeToLive = (uint16_t) u32Value;
			i32Len += i32DecodeLen;
		}
		else
		{
			/* free previously allocated memories & return error */
			Clear_FdtEntry_List(&pstBaseNode);
			return DT_ERR_INVALID_DATA_TYPE;
		}

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetFDTEntry_t *)OSAL_Malloc(
			sizeof(ListOfBACnetFDTEntry_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_FdtEntry_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}
		/* save decoded data */

		memcpy(&pstNewNode->m_stFDTEntry, &stFDTData, sizeof(BACnetFDTEntry_t));
		pstNewNode->m_pstNext = NULL;

		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data */
	*pstFdtEntryList = pstBaseNode;
	*pu32Count = u32Count;

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Decode_FdtEntry_List: Exit \r\n");
	#endif
	/* return total decoded data length */
	return i32Len;
}
#ifdef BACDEL_PR12
#ifndef PROFILE_BOD_ONLY

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Network security policy data type.
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstSecurityPolicy	[out]	pointer to save Security policy.
*                                   
* @return					[out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_NwSecurityPolicy(
	uint8_t *pu8Apdu, 
	ListOfBACnetNwSecurityPolicy_t *pstSecurityPolicy)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstSecurityPolicy)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstSecurityPolicy, 0x00, sizeof(ListOfBACnetNwSecurityPolicy_t));

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{	
		i32Len += i32DecodeLen;
		if(u32LenValueType != 1)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode port ID */		
		i32DecodeLen = Decode_Unsigned8(&pu8Apdu[i32Len], &u32Value);
		i32Len += i32DecodeLen;
		/* save decoded status of decoded data */
		pstSecurityPolicy->m_u8PortId = (uint8_t)u32Value;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode security-level */	
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
			(uint32_t *)&pstSecurityPolicy->m_eSecurityPolicy);
		i32Len += i32DecodeLen;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}
	
	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Array of Network security policy data type.
*    
* @param pu8APDU				[in]	data to be decoded.
* @param u32ApduLen				[out]	received data length.
* @param pstSecurityPolicyArr	[out]	pointer to save Security Policy Array.
* @param pu32Count				[out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/

int32_t DT_Decode_NwSecurityPolicy_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetNwSecurityPolicy_t **pstSecurityPolicyArr,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetNwSecurityPolicy_t *pstNewNode = NULL;
	ListOfBACnetNwSecurityPolicy_t *pstBaseNode = NULL;	
	ListOfBACnetNwSecurityPolicy_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	ListOfBACnetNwSecurityPolicy_t stSecPolicy = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstSecurityPolicyArr || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstSecurityPolicyArr = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode n/w security policy */
		i32DecodeLen = DT_Decode_NwSecurityPolicy(&pu8Apdu[i32Len], &stSecPolicy);
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_NwSecurityPolicy_List(&pstBaseNode);				
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetNwSecurityPolicy_t *)OSAL_Malloc(sizeof(ListOfBACnetNwSecurityPolicy_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_NwSecurityPolicy_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(pstNewNode, &stSecPolicy, sizeof(ListOfBACnetNwSecurityPolicy_t));
		pstNewNode->m_pstNext = NULL;
	
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstSecurityPolicyArr = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;	
}
#endif /* !PROFILE_BOD_ONLY */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Key Identifier data type.
*   
* @param pu8APDU	[in]	data to be decoded.
* @param pstKeyID	[out]	pointer to save KeyId.
*                                   
* @return			[out]	returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_KeyID(
	uint8_t *pu8Apdu, 
	ListOfBACnetKeyId_t *pstKeyID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstKeyID)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstKeyID, 0x00, sizeof(ListOfBACnetKeyId_t));

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{	
		i32Len += i32DecodeLen;
		if(u32LenValueType != 1)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Algorithm */		
		i32DecodeLen = Decode_Unsigned8(&pu8Apdu[i32Len], &u32Value);
		i32Len += i32DecodeLen;
		/* save decoded data */
		pstKeyID->m_u8Algorithm = (uint8_t)u32Value;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType != 1)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Key-Id */	
		i32DecodeLen = Decode_Unsigned8(&pu8Apdu[i32Len], &u32Value);
		i32Len += i32DecodeLen;
		/* save decoded data */
		pstKeyID->m_u8KeyId = (uint8_t)u32Value;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}
	
	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode list of Key identifier data type.
*    
* @param pu8APDU	  [in]	data to be decoded.
* @param u32ApduLen	  [out]	received data length.
* @param pstKeyIdList [out]	pointer to save KeyId list.
* @param pu32Count	  [out]	to save no of elements in list.
*                                   
* @return			  [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_KeyID_List(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetKeyId_t *pstKeyIdList,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetKeyId_t *pstNewNode = NULL;
	uint32_t u32Count = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstKeyIdList || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	*pu32Count = 0;
	/* 1st node */
	pstNewNode = pstKeyIdList;
	pstNewNode->m_pstNext = NULL;

	/* decode list of values */
	while(NULL != pstNewNode)
	{
		i32DecodeLen = DT_Decode_KeyID(&pu8Apdu[i32Len], pstNewNode);
		if(i32DecodeLen < 0)
		{
			/* free previously allocated memories & return error */
			Clear_KeyId_List(&pstKeyIdList->m_pstNext);
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* increment count */
		u32Count++;

		/* allocate memory is more data is available to decode */
		if((u32ApduLen - i32Len) > 0)
		{
			/* allocate memory & save value */
			pstNewNode->m_pstNext = (ListOfBACnetKeyId_t *)OSAL_Malloc(sizeof(ListOfBACnetKeyId_t), 
				__FILE__,__FUNCTION__,__LINE__);
			if(NULL == pstNewNode->m_pstNext)
			{
				/* free previously allocated memories & return error */
				Clear_KeyId_List(&pstKeyIdList->m_pstNext);
				return DT_ERR_MALLOC_FAILED;
			}
			/* move to new node */
			pstNewNode = pstNewNode->m_pstNext;
			pstNewNode->m_pstNext = NULL;
		}
		else
		{
			/* break the loop */
			pstNewNode = NULL;
		}
	}

	/* return total decoded data & length */
	*pu32Count = u32Count;
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode SecurityKeySet data type.
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstSecurityKeySet	[out]	pointer to save Security Key set.
* @param @param u32ApduLen	[out]	received data length.
*                                   
* @return					[out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_SecurityKeySet(
	uint8_t *pu8Apdu, 
	BACnetSecurityKeySet_t *pstSecurityKeySet, 
	uint32_t u32MaxApduLen,
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Count = 0;
	uint32_t u32Value = 0;
	
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstSecurityKeySet)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstSecurityKeySet, 0x00, sizeof(BACnetSecurityKeySet_t));

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{	
		i32Len += i32DecodeLen;
		if(u32LenValueType != 1)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Key Revision */		
		i32DecodeLen = Decode_Unsigned8(&pu8Apdu[i32Len], &u32Value);
		i32Len += i32DecodeLen;
		/* save decoded data */
		pstSecurityKeySet->m_u8KeyRevision = (uint8_t)u32Value;
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* check opening tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
	{
		i32Len += i32DecodeLen;
		/* Decode Activation time */	
		i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len], 
			&pstSecurityKeySet->m_stActivationTime.m_stDate, 
			&pstSecurityKeySet->m_stActivationTime.m_stTime);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // Activation time decoded

		/* check clasing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_1))
		{
			i32Len++; // tag decoded
		}
		else
		{
			/* invalid closing tag */
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* check opening tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2))
	{
		i32Len += i32DecodeLen;
		/* Decode expiration time */	
		i32DecodeLen = DT_Decode_DateTime(&pu8Apdu[i32Len], 
			&pstSecurityKeySet->m_stExpirationTime.m_stDate, 
			&pstSecurityKeySet->m_stExpirationTime.m_stTime);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // expiration time decoded

		/* check clasing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_2))
		{
			i32Len++; // tag decoded
		}
		else
		{
			/* invalid closing tag */
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* check opening tag */
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_3))
	{
		/* get length of data */
		u32LenValueType = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], 
			(u32MaxApduLen - i32Len), ePropID);
		i32Len++; // tag decoded
		/* Decode Key ID list */	
		i32DecodeLen = DT_Decode_KeyID_List(&pu8Apdu[i32Len], u32LenValueType, 
			&pstSecurityKeySet->m_stKeyIds, &u32Count);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen; // key id list decoded

		/* check clasing tag */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_3))
		{
			i32Len++; // tag decoded
		}
		else
		{
			/* invalid closing tag */
			return DT_ERR_INVALID_TAG;
		}
	}
	else
	{
		/* invalid data type */
		return DT_ERR_INVALID_DATA_TYPE;
	}
		
	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of BACnetSecurityKeySet data type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [in]	length of data to be decoded.
* @param pstTimeStamp	 [out]	pointer to save decoded data.
*                                   
* @return				 [out]	returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_SecurityKeySet_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_ListOfBACnetSecurityKeySet_t *pstKeySetArray, 
	BACNET_PROPERTY_ID ePropID)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8IndexCount = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstKeySetArray)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0 && u8IndexCount < BACNET_ARRAY_OF_TWO)
	{
		/* decode Security Key set */			
		i32DecodeLen = DT_Decode_SecurityKeySet(&pu8Apdu[i32Len], 
			&pstKeySetArray->m_stSecurityKetSet[u8IndexCount], 
			(u32ApduLen - i32Len), ePropID);

		if(i32DecodeLen < 0)
		{
			/* clear memories */
			Clear_SecurityKeySet_List(pstKeySetArray);
			return i32DecodeLen;
		}
		/* add to decode length */
		i32Len += i32DecodeLen;
		
		/* increment the indec count */
		u8IndexCount++;
	}

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Property Access Result data type.
*    
* @param pu8APDU			[in]	data to be decoded.
* @param pstAccessResult	[out]	pointer to save Property Access result.
*                                   
* @return					[out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_PropertyAccessResult(
	uint8_t *pu8Apdu, 
	ListOfBACnetPropAccessRslt_t *pstAccessResult,
	uint32_t u32ApduLen)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint32_t u32Value = 0;
	uint32_t u32FirstFailedElement = 0;
	
	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAccessResult)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default values */
	memset(pstAccessResult, 0x00, sizeof(ListOfBACnetPropAccessRslt_t));

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_0) == TRUE)
	{	
		i32Len += i32DecodeLen;
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Object ID */		
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstAccessResult->m_eObjectType, 
			&pstAccessResult->m_u32ObjId);
		i32Len += i32DecodeLen;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_1) == TRUE)
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Property ID */	
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, 
			(uint32_t *)&pstAccessResult->m_ePropertyIdentifier);
		i32Len += i32DecodeLen;
	}
	else
	{
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* save default value for array index flag */
	pstAccessResult->m_bIsArrayIndxPresent = ARRAY_INDEX_ABSENT;

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_2) == TRUE)
	{
		i32Len += i32DecodeLen;
		if(u32LenValueType < 1 || u32LenValueType > 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Property Array Index */	
		i32DecodeLen = Decode_Unsigned(&pu8Apdu[i32Len], u32LenValueType, 
			&pstAccessResult->m_u32ArrayIndex);
		i32Len += i32DecodeLen;
		pstAccessResult->m_bIsArrayIndxPresent = ARRAY_INDEX_PRESENT;
	}

	/* decode the tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Context_Tag(&pu8Apdu[i32Len], TAG_NO_3) == TRUE)
	{	
		i32Len += i32DecodeLen;
		if(u32LenValueType != 4)
		{
			return DT_ERR_INVALID_TAG;
		}
		/* decode Device ID */		
		i32DecodeLen = Decode_Object_Id(&pu8Apdu[i32Len], 
			(uint32_t *)&pstAccessResult->m_eDevType, 
			&pstAccessResult->m_u32DevId);
		i32Len += i32DecodeLen;
	}

	/* check opening tag */
	i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
	if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_4) == TRUE)
	{
		/* save the flag */
		pstAccessResult->m_u32UniMember = TAG_NO_4;

		/* get property value data length */
		u32LenValueType = BACApp_Verify_Data_Len(&pu8Apdu[i32Len], 
			(u32ApduLen - i32Len), pstAccessResult->m_ePropertyIdentifier);
		i32Len += i32DecodeLen;

		/* decode property value */
		i32DecodeLen = BACApp_Decode_Data_Type(&pu8Apdu[i32Len], u32LenValueType, 
			pstAccessResult->m_ePropertyIdentifier,
			pstAccessResult->m_eObjectType, 
			&pstAccessResult->m_stReadResult.m_stPropertyValue.m_eData_Type,
			&pstAccessResult->m_stReadResult.m_stPropertyValue.m_pvConstrProp,
			pstAccessResult->m_u32ArrayIndex,
			pstAccessResult->m_bIsArrayIndxPresent, &u32FirstFailedElement);

		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			//TODO				
			i32Len = i32DecodeLen;
			return i32Len;
		}
		i32Len += i32DecodeLen;

		/* decode closing tag of logDatum choice */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_4))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}	
	}

	/* decode the tag */
	else if(Decode_Is_Opening_Tag_Number(&pu8Apdu[i32Len], TAG_NO_5) == TRUE)
	{
		/* save the flag */
		pstAccessResult->m_u32UniMember = TAG_NO_5;
		i32Len += i32DecodeLen;

		/* decode error class */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, &u32Value);
		i32Len += i32DecodeLen;
		pstAccessResult->m_stReadResult.m_stError.m_eErrorClass = u32Value;

		/* decode error code */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		i32Len += i32DecodeLen;
		i32DecodeLen = Decode_Enumerated(&pu8Apdu[i32Len], u32LenValueType, &u32Value);
		i32Len += i32DecodeLen;
		pstAccessResult->m_stReadResult.m_stError.m_eErrorCode = u32Value;
		/* decode closing tag of logDatum choice */
		if(Decode_Is_Closing_Tag_Number(&pu8Apdu[i32Len], TAG_NO_5))
		{
			i32Len++;
		}
		else
		{
			i32Len = DT_ERR_INVALID_TAG;
			return i32Len;
		}	
	}
	else
	{
		/* data type not supported */
		return DT_ERR_INVALID_DATA_TYPE;
	}

	/* return total decoded data length */
	return i32Len;
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode Array of Network security policy data type.
*    
* @param pu8APDU				[in]	data to be decoded.
* @param u32ApduLen				[out]	received data length.
* @param pstSecurityPolicyArr	[out]	pointer to save Security Policy Array.
* @param pu32Count				[out]	to save no of elements in list.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/

int32_t DT_Decode_PropertyAccessResult_Array(
	uint8_t *pu8Apdu, 
	uint32_t u32ApduLen, 
	ListOfBACnetPropAccessRslt_t **pstPropAccessResult,
	uint32_t *pu32Count)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	ListOfBACnetPropAccessRslt_t *pstNewNode = NULL;
	ListOfBACnetPropAccessRslt_t *pstBaseNode = NULL;	
	ListOfBACnetPropAccessRslt_t *pstPreviousNode = NULL;
	uint32_t u32Count = 0;	
	ListOfBACnetPropAccessRslt_t stPropAccessResult = {0};
	bool bFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstPropAccessResult || NULL == pu32Count)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* set default value */
	*pstPropAccessResult = NULL;
	*pu32Count = 0;

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0)
	{
		/* decode n/w security policy */
		i32DecodeLen = DT_Decode_PropertyAccessResult(&pu8Apdu[i32Len], 
			&stPropAccessResult, (u32ApduLen - i32Len));
		if(i32DecodeLen < 0)
		{			
			/* invalid data received */
			/* clear allocated memories & return error */
			Clear_PropAccessResult_List(&pstBaseNode);				
			return i32DecodeLen;
		}
		i32Len += i32DecodeLen;

		/* allocate memory & save value */
		pstNewNode = (ListOfBACnetPropAccessRslt_t *)OSAL_Malloc(sizeof(ListOfBACnetPropAccessRslt_t),
			__FILE__,__FUNCTION__,__LINE__);
		if(NULL == pstNewNode)
		{
			/* free previously allocated memories & return error */
			Clear_PropAccessResult_List(&pstBaseNode);
			return DT_ERR_MALLOC_FAILED;
		}

		/* save decoded data */
		memcpy(pstNewNode, &stPropAccessResult, sizeof(ListOfBACnetPropAccessRslt_t));
		pstNewNode->m_pstNext = NULL;
	
		/* increment count */
		u32Count++;

		/* maintain list */
		if(!bFlag)
		{
			/* 1st node */
			pstBaseNode = pstNewNode;
			bFlag = TRUE;
		}
		else
		{
			/* add new node @ the end of list */
			pstPreviousNode->m_pstNext = pstNewNode;
		}
		/* update previous pointer */
		pstPreviousNode = pstNewNode;
		pstNewNode = NULL;
	}

	/* return total decoded data & length */
	*pstPropAccessResult = pstBaseNode;
	*pu32Count = u32Count;
	return i32Len;	
}


/***
*                                                                    
* DESCRIPTION                                                                          
* Function to decode array of Event message text data type.
*    
* @param pu8APDU		 [in]	data to be decoded.
* @param u32ApduLen		 [in]	length of data to be decoded.
* @param pstEveMsgText	 [out]	pointer to save decoded data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Decode_EveMsgTxt_Array(
	uint8_t *pu8Apdu,
	uint32_t u32ApduLen, 
	Pr_BACnetEventMsgText_t *pstEveMsgText)
{
	/* local variables */
	int32_t i32DecodeLen = 0;
	int32_t i32Len = 0;
	uint8_t u8TagNo = 0;
	uint32_t u32LenValueType = 0;
	uint8_t u8IndexCount = 0;
	uint32_t u32MaxStrLen = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstEveMsgText)
	{
		i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* decode list of values */
	while((u32ApduLen - ((uint32_t)i32Len)) > 0 && u8IndexCount < BACNET_ARRAY_OF_THREE)
	{
		/* decode Event msg text value */
		i32DecodeLen = Decode_Tag_Number_And_Value(&pu8Apdu[i32Len], &u8TagNo, &u32LenValueType);
		if(BACNET_APPLICATION_TAG_CHARACTER_STRING != u8TagNo)
		{			
			/* invalid data received */
			return DT_ERR_INVALID_TAG;
		}
		i32Len += i32DecodeLen;
		i32DecodeLen = Decode_Character_String(&pu8Apdu[i32Len], 
			u32LenValueType,
			&pstEveMsgText->m_stEventMsgText[u8IndexCount]);

		/* get max supported string length */
		u32MaxStrLen = ((pstEveMsgText->m_stEventMsgText[u8IndexCount].m_u8Encoding == CHARACTER_IBM_MS_DBCS) ?
			(MAX_SUPPORTED_CHRSTR_LEN +2) : MAX_SUPPORTED_CHRSTR_LEN);
		if(i32DecodeLen < 0)
		{
			return i32DecodeLen;
		}
		else if(u32LenValueType > u32MaxStrLen)
		{
			return DT_ERR_VALUE_OUT_OF_RANGE;
		} 
		i32Len += i32DecodeLen;

		/* increment the index count */
		u8IndexCount++;
	}

	/* return total decoded data length */
	return i32Len;
}

#endif /* BACDEL_PR12 */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode time-stamp data type.
*    
* @param pu8APDU		 [out]	pointer to save Encoded data.
* @param puTimeStamp	 [in]	pointer of data to be Encoded.
* @param eTimeStampType  [in]	Timestamp Type to be encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_TimeStamp(
	uint8_t *pu8Apdu,
	timeStamp_u *puTimeStamp, 
	BACNET_TIMESTAMP_TYPE eTimeStampType)
{
	/* local variables */
	int32_t i32Len = 0;				/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puTimeStamp)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	switch(eTimeStampType)
	{
	case TIMESTAMP_TYPE_TIME:
		/* For Time Value */
        i32Len += Encode_Application_StampTime
			(&pu8Apdu[i32Len], &puTimeStamp->m_stTime);
		break;

	case TIMESTAMP_TYPE_SEQUENCE_NO:
		/* For Sequence no */
        i32Len += Encode_Application_StampUnsigned
			(&pu8Apdu[i32Len], puTimeStamp->m_u32SeqNo);
		break; 

	case TIMESTAMP_TYPE_DATETIME:
		/* For Date Time Value */
        i32Len += Encode_Application_StampDateTime
			(&pu8Apdu[i32Len],
			&puTimeStamp->m_stDateTime.m_stTime, 
			&puTimeStamp->m_stDateTime.m_stDate);
		break;

	default:
		/* invalid time-stamp type */
		break;
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode array of time-stamp data type.
*    
* @param pu8APDU		 [out]	pointer to save Encoded data.
* @param puTimeStampArr	 [in]	pointer to Array of data to be Encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_TimeStamp_Array(
	uint8_t *pu8Apdu,
	Pr_BACnetEventTimeStamp_t *puTimeStampArr)
{
	/* local variables */
	int32_t i32Len = 0;				/* Total Encoded Length */
	uint8_t u8Index = 0;			/* Used for for loop index */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puTimeStampArr)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* For all 3 Event Time stamps  */
	for(u8Index = 0; u8Index < BACNET_ARRAY_OF_THREE ; u8Index++)
    {
	switch(puTimeStampArr->m_eTimeStampType[u8Index])
	{
	case TIMESTAMP_TYPE_TIME:
		/* For Time Value */
		i32Len += Encode_Application_StampTime
		  (&pu8Apdu[i32Len],
		  &puTimeStampArr->m_uEvntStamp[u8Index].m_stTime);     
	break;

	case TIMESTAMP_TYPE_SEQUENCE_NO:
		/* For Sequences no. */
		i32Len += Encode_Application_StampUnsigned
		  (&pu8Apdu[i32Len],
		  puTimeStampArr->m_uEvntStamp[u8Index].m_u32SeqNo);
	break; 

	case TIMESTAMP_TYPE_DATETIME:
		/* For Date Time Value */
		i32Len += Encode_Application_StampDateTime
			(&pu8Apdu[i32Len],
			 &puTimeStampArr->m_uEvntStamp[u8Index].m_stDateTime.m_stTime,
			 &puTimeStampArr->m_uEvntStamp[u8Index].m_stDateTime.m_stDate);
	break;

	default:
		/* invalid time-stamp type */
		break;
	}
	}/* End of For Loop*/

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode List of Unsigned data type.
*    
* @param pu8APDU		 [in]	data to be Encoded.
* @param puUnsignedList	 [out]	pointer to save Encoded data.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_Unsigned_List(
	uint8_t *pu8Apdu,
	Pr_ListOfUnsigned_t *puUnsigList,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize)
{
	/* local variables */
	int32_t i32PrevLen = 0;
	int32_t i32Len = 0;
	ListOfUnsigned_t *pstUnsignedList = NULL;	
	bool bReadRangeFlag = FALSE;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puUnsigList)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstUnsignedList = puUnsigList->m_pstUnsignVal;

	/* if encode function is called from ReadRange-B service 
	   make node count of nodes to 0 and set the flag */
	if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
	{
		*pu32NodeCnt = 0;
		bReadRangeFlag = TRUE;
	}

	while((pstUnsignedList != NULL) && ( i32Len < u32MaxInBufSize))
    {             
		/* save previous encoded data length */
		i32PrevLen = i32Len;
		i32Len += Encode_Application_Unsigned(&pu8Apdu[i32Len],
				pstUnsignedList->m_u32Value);
		/* move to next Unsigned value */
        pstUnsignedList = pstUnsignedList->m_pstNext; 	

		/* if encode function called from ReadRange-B service, save the length of
		   each node in array */
		if(bReadRangeFlag)
		{
			if(*pu32NodeCnt < i32NodeLenArrSize)
			{
				pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32Len - i32PrevLen);
				(*pu32NodeCnt)++;
			}
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode device object reference.
*    
* @param pu8APDU		[out]	Buffer which will hold the encoded Bacnet octets.
* @param puDevObjReff   [in]	base pointer of data to be encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_DevObjReff(uint8_t *pu8Apdu, BACnetDevObjRef_t *puDevObjReff)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puDevObjReff)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* Encode Device Identifier */
	 if(TRUE == puDevObjReff->m_bDeviceIdPresent)
	 {
		i32Len += Encode_Context_Object_Id(&pu8Apdu[i32Len], TAG_NO_0, 
				(int32_t)puDevObjReff->m_eDeviceType, 
				puDevObjReff->m_u32DeviceId);
	  }

	  /* Encode Object Identifier */
	  i32Len += Encode_Context_Object_Id(&pu8Apdu[i32Len], TAG_NO_1,
			(int32_t)puDevObjReff->m_eObjectType, 
			puDevObjReff->m_u32ObjId);

	  /* return total Encoded data length */
	  return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode device object Property reference.
*    
* @param pu8APDU		    [out]	Buffer which will hold the encoded Bacnet octets.
* @param puDevObjPropReff   [in]	base pointer of data to be encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_DevObjPropReff(
	uint8_t *pu8Apdu,
	BACnetDevObjPropRef_t *puDevObjPropReff)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puDevObjPropReff)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* object id */
    i32Len += Encode_Context_Object_Id(&pu8Apdu[i32Len], TAG_NO_0,
		(int32_t)puDevObjPropReff->m_eObjectType,
		puDevObjPropReff->m_u32ObjId);

	/* Encode Property Identifier */            
	i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len], TAG_NO_1,
		puDevObjPropReff->m_ePropertyIdentifier);

	/* array index */
    if(ARRAY_INDEX_PRESENT == puDevObjPropReff->m_bArrIndxPresent)
    {
		i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_2,
			puDevObjPropReff->m_u32ArrayIndex);
    }

	/* device id & type */
	if(true == puDevObjPropReff->m_bDeviceIdPresent)
	{
		i32Len += Encode_Context_Object_Id(&pu8Apdu[i32Len], TAG_NO_3,
			(int32_t)OBJECT_DEVICE, puDevObjPropReff->m_u32DeviceInstace);
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode bacnet Authentication Factor format.
*    
* @param pu8APDU	     [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAuthFactForm  [in]	base pointer of data to be encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AuthFactForm(uint8_t *pu8Apdu,
							ListOfBACnetAuFactorFormat_t *puAuthFactForm)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAuthFactForm)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	
	/* Encode Format Type */
	i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_0,
		(puAuthFactForm->m_eFormatType));

	/* Encode Vendor Id */
	if(TRUE == puAuthFactForm->m_bVIdFlag)
	{
		i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_1,
			(puAuthFactForm->m_u16VendorId));
	}

	/* Encode Vendor Format */
	if(TRUE == puAuthFactForm->m_bVFormatFlag)
	{
		i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_2,
			(puAuthFactForm->m_u16VendorFormat));
	}
	
	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode List of device object reference.
*    
* @param pu8APDU		  [out]	Buffer which will hold the encoded Bacnet octets.
* @param puDevObjReffList [in]	base pointer to List of data to be encoded.
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_DevObjReff_List(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetDevObjRef_t *puDevObjReffList,
	int32_t u32MaxInBufSize)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	ListOfBACnetDevObjRef_t *pstDevObjReffList = NULL;	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puDevObjReffList)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstDevObjReffList = puDevObjReffList->m_pstListOfDevObjReff;

	/* encode all elements in list & check max length is not exceeded */
    while((NULL != pstDevObjReffList) && (i32Len < u32MaxInBufSize))
    {
		i32Len += DT_Encode_DevObjReff(&pu8Apdu[i32Len], &pstDevObjReffList->m_stDevObjRef);
		/* move to next Array Index value */
		pstDevObjReffList = pstDevObjReffList->m_pstNext;
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Array of device object reference.
*    
* @param pu8APDU		   [out]  Buffer which will hold the encoded Bacnet octets.
* @param puDevObjReffArr   [in]	  base pointer to Array of data to be encoded.
* @param u32MaxInBufSize   [in]   Maximum Buffer Size Value.
* @param u32ArrayIndex	   [in]   Array Index Value.
* @param bArrIndxPresent   [in]   Array Index notification.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_DevObjReff_Array(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetDevObjRef_t *puDevObjReffArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetDevObjRef_t *pstDevObjReffArr = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puDevObjReffArr)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstDevObjReffArr = puDevObjReffArr->m_pstListOfDevObjReff;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstDevObjReffArr) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_DevObjReff(&pu8Apdu[i32Len], 
				&pstDevObjReffArr->m_stDevObjRef);
			/* move to next Array Index value */
			pstDevObjReffArr = pstDevObjReffArr->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&pu8Apdu[i32Len], puDevObjReffArr->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstDevObjReffArr)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstDevObjReffArr = pstDevObjReffArr->m_pstNext;
        }
        if(NULL != pstDevObjReffArr)
        {
			i32Len += DT_Encode_DevObjReff(&pu8Apdu[i32Len], 
				&pstDevObjReffArr->m_stDevObjRef);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Shed Level Data Type.
*    
* @param pu8APDU	  [out]	Buffer which will hold the encoded Bacnet octets.
* @param puShedLevel  [in]	base pointer of data to be encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_ShedLevel(uint8_t *pu8Apdu, Pr_BACnetShedLevel_t *puShedLevel)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puShedLevel)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* Encode percent value */
	if(SHED_LEVEL_PERCENT == puShedLevel->m_stShedLevel.m_eShedLevelType)
	{
		i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_0,
			puShedLevel->m_stShedLevel.shedlevel_u.m_u32Percent);
	}
	/* Encode level value */
    else if(SHED_LEVEL_LEVEL == puShedLevel->m_stShedLevel.m_eShedLevelType) 
	{
		i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_1,
			puShedLevel->m_stShedLevel.shedlevel_u.m_u32Level);
	}
	/* Encode amount value */
	else if(SHED_LEVEL_AMOUNT == puShedLevel->m_stShedLevel.m_eShedLevelType)
	{
		i32Len += Encode_Context_Real(&pu8Apdu[i32Len], TAG_NO_2,
			puShedLevel->m_stShedLevel.shedlevel_u.m_fAmount);
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode List of Trend Log Record.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puLogRecord	 [in]	base pointer to List of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_TrendLogRec_List(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetLogRecord_t *puLogRecord,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize)
{
	/* local variables */
	int32_t i32Len = 0;
	int32_t i32PrevLen = 0;
	bool bReadRangeFlag = FALSE;

	ListOfBACnetLogRecord_t *pstLogRecord = NULL;	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puLogRecord)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstLogRecord = puLogRecord->m_pstLogRecord;

	/* if encode function is called from ReadRange-B service 
	   make node count of nodes to 0 and set the flag */
	if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
	{
		*pu32NodeCnt = 0;
		bReadRangeFlag = TRUE;
	}

	/* encode all elements in list & check max length is not exceeded */
	while(NULL != pstLogRecord && i32Len < u32MaxInBufSize)
    {
		/* save previous encoded data length */
		i32PrevLen = i32Len;
		/* encode opening tag for time stamp of log */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);
		/* encode Date value */
        i32Len += Encode_Application_Date(&pu8Apdu[i32Len], 
			&(pstLogRecord->m_stListofLogRecord.m_stTimeStamp.m_stDate));
		/* encode Time value */
        i32Len += Encode_Application_Time(&pu8Apdu[i32Len], 
			&(pstLogRecord->m_stListofLogRecord.m_stTimeStamp.m_stTime));
		/* encode closing tag for time stamp of log */
        i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);

		/* encode opening tag for log record value */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1);
		/* encode log value */
        switch(pstLogRecord->m_stListofLogRecord.m_stlogDatum.m_TagType)
        {
			case BACNET_DT_BOOLEAN:
			{
			/* encode context boolean */
			i32Len += Encode_Context_Boolean(&pu8Apdu[i32Len],TAG_NO_1,
				(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					bacnetlogDatum_U.m_Boolean));
			}
			break;

			case BACNET_DT_REAL:
			{
			/* encode context real */
			i32Len += Encode_Context_Real(&pu8Apdu[i32Len],TAG_NO_2,
				(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					bacnetlogDatum_U.m_Real));
			}
			break;
			 
			case BACNET_DT_DOUBLE:
			{
			/* encode context double */
			   i32Len += Encode_Context_Double(&pu8Apdu[i32Len],TAG_NO_2,
				(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
				bacnetlogDatum_U.m_Double));
			}
			break; 
			
			case BACNET_DT_ENUM:
			case BACNET_DT_ENUM_NEW:
			{
			/* encode context enum */
			i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_3,
				(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					bacnetlogDatum_U.m_Enum));
			}
			break;

			 case BACNET_DT_UNSIGNED:
			 case BACNET_DT_UNSIGNED16:
             case BACNET_DT_UNSIGNED32:
             {
				/* encode context unsigned */
				i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_4,
					(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					 bacnetlogDatum_U.m_Unsigned_Int));
              }
              break;

              case BACNET_DT_INTEGER:
              {
				 /* encode context signed */
                 i32Len += Encode_Context_Signed(&pu8Apdu[i32Len],TAG_NO_5,
					 (pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					  bacnetlogDatum_U.m_Signed_Int));
              }
              break;
                        
              case BACNET_DT_BITSTRING:
			  case BACNET_DT_BITSTRING_NEW:
              {
			  	 /* encode context bitstring */
			  	 i32Len += Encode_Context_BITString(&pu8Apdu[i32Len],TAG_NO_6,
			  		 &(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					   bacnetlogDatum_U.m_stBit_String));
              }
              break;
			
			  case BACNET_DT_SERVICES_SUPPORTED:
		      {
				 /* encode context service supported by device */
				 i32Len += Encode_Context_Byte(&pu8Apdu[i32Len],
					&(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					bacnetlogDatum_U.m_Byte[0]), TAG_NO_6, 
					SERVICE_SUPPORTED_UNUSED_BITS, MAX_SERVICE_SUPPORT_BYTE);
			  }
			  break;

			  case BACNET_DT_OBJECT_TYPE_SUPPORTED:
			  {
				/* encode context object supported by device */
				i32Len += Encode_Context_Byte(&pu8Apdu[i32Len],
					&(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					bacnetlogDatum_U.m_Byte[0]), TAG_NO_6, 
					OBJECT_SUPPORTED_UNUSED_BITS, MAX_OBJECT_TYPE_BYTE);
			  }
			  break;

              case BACNET_DT_NULL:
		      {
				 /* encode context null */
                 i32Len += Encode_Context_Null(&pu8Apdu[i32Len],TAG_NO_7);
			  }
              break;

              case BACNET_DT_LOGBUFFER_LOGSTATUS:
              {
				  /* Local Variables */
				  BACnetBitStr_t stBitString = {0};
				  uint32_t u32Count = 0;

			  	  /* copy bitstring to local struct */
                  for(u32Count = 0; u32Count < MIN_BITSTRING_BYTES; u32Count++)
			  	  {
			   	 	 stBitString.m_u8TransBits[u32Count] = 
                         pstLogRecord->m_stListofLogRecord.m_stlogDatum.
			  	 		 bacnetlogDatum_U.m_stlog_status.m_u8TransBits[u32Count];
				  }

				  /* save byte count & unused bits */
				  stBitString.m_u8ByteCnt = pstLogRecord->m_stListofLogRecord.
					  m_stlogDatum.bacnetlogDatum_U.m_stlog_status.m_u8ByteCnt;
				  stBitString.m_u8UnusedBits = pstLogRecord->m_stListofLogRecord.
					  m_stlogDatum.bacnetlogDatum_U.m_stlog_status.m_u8UnusedBits;

			   	  /* encode value */
			  	  i32Len += Encode_Context_BITSTRING(&pu8Apdu[i32Len], 
			  	 	 TAG_NO_0, &stBitString);
			  }
              break;
              
			  case BACNET_DT_LOGBUFFER_ERROR:
              {
				  /* encode opening tag for log buffer error */
                  i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_8);
				  /* encode error class */
                  i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len],
					 pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					 bacnetlogDatum_U.m_stError.m_eErrorClass);
				  /* encode error code */
                  i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len],
                     pstLogRecord->m_stListofLogRecord.m_stlogDatum.
					 bacnetlogDatum_U.m_stError.m_eErrorCode);
				  /* encode Closing tag for log buffer error */
                  i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_8);
               }
               break; 

			   case BACNET_DT_LOGBUFFER_TIMECHANGE:
			   {
					/* encode time_change as context real with tag no 9 */
					i32Len += Encode_Context_Real(&pu8Apdu[i32Len],TAG_NO_9,
						(pstLogRecord->m_stListofLogRecord.m_stlogDatum.
						bacnetlogDatum_U.m_fTimechange));
			   }
               break;

			   default:
				break;
        }//switch ends
		/* encode closing tag for log record value */
        i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1); 

		/* encode status falg value if present */
        if(pstLogRecord->m_stListofLogRecord.m_stStatusFlag.m_u8ByteCnt)
        {
			i32Len += Encode_Context_BITSTRING(&pu8Apdu[i32Len],TAG_NO_2,
				&(pstLogRecord->m_stListofLogRecord.m_stStatusFlag));
         }

		/* Move to next List value */
         pstLogRecord = pstLogRecord->m_pstNext;

		/* if encode function called from ReadRange-B service, save the length of
		   each node in array */
		if(bReadRangeFlag)
		{
			if(*pu32NodeCnt < i32NodeLenArrSize)
			{
				pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32Len - i32PrevLen);
				(*pu32NodeCnt)++;
			}
		}
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode List of Event Log Record.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puEventLogRec	 [in]	base pointer to List of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_EventLogRec_List(uint8_t *pu8Apdu, 
								 Pr_ListOfBACnetEventLogRecord_t *puEventLogRec,
								 int32_t u32MaxInBufSize)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	ListOfBACnetEventLogRecord_t *pstELogRecord = NULL;	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puEventLogRec)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstELogRecord = puEventLogRec->m_pstEventLogRecord;

	while(NULL != pstELogRecord && i32Len < u32MaxInBufSize)
    {
		/* encode opening tag for time stamp of log */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);
		/* encode Date value */
        i32Len += Encode_Application_Date(&pu8Apdu[i32Len], 
			&(pstELogRecord->m_stTimeStamp.m_stDate));
		/* encode Time value */
        i32Len += Encode_Application_Time(&pu8Apdu[i32Len], 
			&(pstELogRecord->m_stTimeStamp.m_stTime));
		/* encode closing tag for time stamp of log */
        i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);
		
		/* encode opening tag for log record value */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1);

		if(BACNET_DT_LOGBUFFER_LOGSTATUS == pstELogRecord->m_stlogData.m_TagType)
		{
			/* Local variables */
			BACnetBitStr_t stLogStatus = {0};
			uint8_t u8ByteCnt = 0;

			/* Copy Bacnet log status in structure for Encoding  */
			stLogStatus.m_u8ByteCnt = pstELogRecord->m_stlogData
				.eventlogData_u.m_stlog_status.m_u8ByteCnt;

			stLogStatus.m_u8UnusedBits = pstELogRecord->m_stlogData
				.eventlogData_u.m_stlog_status.m_u8UnusedBits;

			for(u8ByteCnt=0; u8ByteCnt < MIN_BITSTRING_BYTES; u8ByteCnt++)
            {
              stLogStatus.m_u8TransBits[u8ByteCnt] = pstELogRecord->m_stlogData
						.eventlogData_u.m_stlog_status.m_u8TransBits[u8ByteCnt];
            }

			/* encode context Bit String */
			i32Len += Encode_Context_BITSTRING(&pu8Apdu[i32Len],TAG_NO_0,
				&stLogStatus);
		}
		else if(BACNET_DT_LOGBUFFER_TIMECHANGE == pstELogRecord->m_stlogData
																	 .m_TagType)
		{
			/* encode context real */
			i32Len += Encode_Context_Real(&pu8Apdu[i32Len],TAG_NO_2,
				(pstELogRecord->m_stlogData.eventlogData_u.m_fTimechange));
		}
		else 
		{
			/* Local Variables */
			processInfo_t stProcQInfo = {0};

			/* Get Values */
			stProcQInfo.m_stProcessData.m_stAPDU.m_pvServiceRequestData = 
				(processInfo_t *)&pstELogRecord->m_stlogData.eventlogData_u.m_stNotification;

			/* encode opening tag for Confirmed Event Notification */
			i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1);

			/* encode Confirmed Event Notification-Request */
			#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)
			i32Len += Event_Notification_Encoder(&stProcQInfo, &pu8Apdu[i32Len], i32Len);
			#endif

			/* encode closing tag for Confirmed Event Notification */
			i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1);
			
		}
		/* encode closing tag for log record value */
        i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1);

		/* move to next value */
        pstELogRecord = pstELogRecord->m_pstNext;
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode array of Event Message Texts data type.
*    
* @param pu8APDU		 [out]	pointer to save Encoded data.
* @param puEveMsgTxtArr	 [in]	pointer to Array of data to be Encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_EveMsgTxt_Array(
	uint8_t *pu8Apdu,
	Pr_BACnetEventMsgText_t *puEveMsgTxtArr)
{
	/* local variables */
	int32_t i32Len = 0;							 /* Total Encoded Length */
	uint8_t u8OutIndex = 0;						 /* Used for for loop index */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puEveMsgTxtArr)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* For all 3 Event Msg Texts  */
	for(u8OutIndex = 0; u8OutIndex < BACNET_ARRAY_OF_THREE ; u8OutIndex++)
    {		
		i32Len += Encode_Application_Character_String(&pu8Apdu[i32Len], 
			&puEveMsgTxtArr->m_stEventMsgText[u8OutIndex]); 
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Event Message Texts data type.
*    
* @param pu8APDU		 [out]	pointer to save Encoded data.
* @param puEveMsgTxt	 [in]	pointer to Array of data to be Encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_EveMsgTxt(
	uint8_t *pu8Apdu,
	Pr_BACnetEventMsgText_t *puEveMsgTxt)
{
	/* local variables */
	int32_t i32Len = 0;							 /* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puEveMsgTxt)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	return i32Len;
}

#ifndef PROFILE_BOD_ONLY
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Bacnet Log Data.
*    
* @param pu8APDU	     [out]	pointer to save Encoded data.
* @param puLogData	     [in]	pointer to Array of data to be Encoded.
* @param u32MaxInBufSize [in]	 Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_LogData(
	uint8_t *pu8Apdu,
	ListOfBACnetLogMultipleRecord_t *puLogData,
	int32_t u32MaxInBufSize)
{
	/* local variables */
	int32_t i32Len = 0;							 /* Total Encoded Length */
	BACnetLogData_t *pstLogData = NULL;
	
	/* get Value */
	pstLogData = &puLogData->m_stlogData;
	
	if(BACNET_DT_LOGBUFFER_LOGSTATUS == pstLogData->m_TagType)
	{
		/* Local variables */
		BACnetBitStr_t stLogStatus = {0};
		uint8_t u8ByteCnt = 0;

		/* Copy Bacnet log Data in structure for Encoding  */
		stLogStatus.m_u8ByteCnt = pstLogData->logMultipleData_u.m_stlog_status
			.m_u8ByteCnt;

		stLogStatus.m_u8UnusedBits = pstLogData->logMultipleData_u
			.m_stlog_status.m_u8UnusedBits;
			

		for(u8ByteCnt=0; u8ByteCnt < MIN_BITSTRING_BYTES; u8ByteCnt++)
        {
			stLogStatus.m_u8TransBits[u8ByteCnt] = pstLogData->
				logMultipleData_u.m_stlog_status.m_u8TransBits[u8ByteCnt];
        }

		/* encode BACnet log Data */
		i32Len += Encode_Context_BITSTRING(&pu8Apdu[i32Len],TAG_NO_0,
			&stLogStatus);
	}
	else if(BACNET_DT_LOGBUFFER_TIMECHANGE == pstLogData->m_TagType)
	{
		/* encode context real */
		i32Len += Encode_Context_Real(&pu8Apdu[i32Len],TAG_NO_2,
			(pstLogData->logMultipleData_u.m_fTimechange));
	}
	else 
	{
		/* encode opening tag for BACnet Log Data */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1); 
		/* encode all elements in list & check max length is not exceeded */
		while(NULL != pstLogData && i32Len < u32MaxInBufSize)
		{
			/* encode log value */
			switch(pstLogData->m_TagType)
		    {
			case BACNET_DT_BOOLEAN:
			{
				/* encode context boolean */
				i32Len += Encode_Context_Boolean(&pu8Apdu[i32Len],TAG_NO_0,
					(pstLogData->logMultipleData_u.m_Boolean));
			}
			break;

			case BACNET_DT_REAL:
			{
		 		/* encode context real */
		 		i32Len += Encode_Context_Real(&pu8Apdu[i32Len],TAG_NO_1,
					(pstLogData->logMultipleData_u.m_Real));
			}
			break;

			case BACNET_DT_DOUBLE:
			{
				/* encode context double */
				i32Len += Encode_Context_Double(&pu8Apdu[i32Len],TAG_NO_1,
				(pstLogData->logMultipleData_u.m_Double));
			}
			break;

			case BACNET_DT_ENUM:
			case BACNET_DT_ENUM_NEW:
			{
		 		/* encode context enum */
		 		i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_2,
					(pstLogData->logMultipleData_u.m_Enum));
			}
			break;

			case BACNET_DT_UNSIGNED:
			case BACNET_DT_UNSIGNED16:
			case BACNET_DT_UNSIGNED32:
			{
				/* encode context unsigned */
				i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_3,
					(pstLogData->logMultipleData_u.m_Unsigned_Int));
			}
			break;

			case BACNET_DT_INTEGER:
			{
				/* encode context signed */
				i32Len += Encode_Context_Signed(&pu8Apdu[i32Len],TAG_NO_4,
					(pstLogData->logMultipleData_u.m_Signed_Int));
			}
			break;

			case BACNET_DT_BITSTRING:
			case BACNET_DT_BITSTRING_NEW:
			{
		 		/* encode context bitstring */
		 		i32Len += Encode_Context_BITString(&pu8Apdu[i32Len],TAG_NO_5,
					&(pstLogData->logMultipleData_u.m_stBit_String));
			}
			break;
		
			case BACNET_DT_NULL:
			{
				/* encode context null */
				i32Len += Encode_Context_Null(&pu8Apdu[i32Len],TAG_NO_6);
			}
			break;

			case BACNET_DT_LOGBUFFER_ERROR:
			{
				/* encode opening tag for log buffer error */
				i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_7);
				/* encode error class */
				i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len],
					pstLogData->logMultipleData_u.m_stError.m_eErrorClass);
				/* encode error code */
				i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len],
					pstLogData->logMultipleData_u.m_stError.m_eErrorCode);
				/* encode Closing tag for log buffer error */
				i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_7);
			}
			break; 

			default:
			break;
		}//switch ends

		/* Move to Next Link List Value */
		pstLogData = pstLogData->m_pstNext;	
	}
	/* encode closing tag for log record value */
	i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1);
  }

  /* return total Encoded data length */
  return i32Len;
}


/***
*
* DESCRIPTION
* Function to Encode List of Trend Log Multiple Record.
*
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puTreLogMultRec [in]	base pointer to List of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
**/
int32_t DT_Encode_TrendLogMultRec_List(
	uint8_t *pu8Apdu, 
	Pr_ListOfBACnetLogMultipleRecord_t *puTreLogMultRec,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize
	)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	int32_t i32PrevLen = 0;
	bool bReadRangeFlag = FALSE;
	ListOfBACnetLogMultipleRecord_t *pstTreLogMultRec = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puTreLogMultRec)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstTreLogMultRec = puTreLogMultRec->m_pstLogMultipleRecord;

	/* if encode function is called from ReadRange-B service 
	   make node count of nodes to 0 and set the flag */
	if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
	{
		*pu32NodeCnt = 0;
		bReadRangeFlag = TRUE;
	}
	
	/* encode all elements in list & check max length is not exceeded */
	while(NULL != pstTreLogMultRec && i32Len < u32MaxInBufSize)
	{
		/* encode opening tag for time stamp of TLM */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);
		/* encode Date value */
        i32Len += Encode_Application_Date(&pu8Apdu[i32Len], 
			&(pstTreLogMultRec->m_stTimeStamp.m_stDate));
		/* encode Time value */
        i32Len += Encode_Application_Time(&pu8Apdu[i32Len], 
			&(pstTreLogMultRec->m_stTimeStamp.m_stTime));
		/* encode closing tag for time stamp of TLM */
        i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);
	
		/* encode opening tag for BACnet Log Data */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1); 
		/* encode Log Data value */
		i32Len += DT_Encode_LogData(&pu8Apdu[i32Len], pstTreLogMultRec, u32MaxInBufSize);
		/* encode Closing tag for BACnet Log Data */
		i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1); 

		/* move to next value */
		pstTreLogMultRec = pstTreLogMultRec->m_pstNext;
		
		/* if encode function called from ReadRange-B service, save the length of
		   each node in array */
		if(bReadRangeFlag)
		{
			if(*pu32NodeCnt < i32NodeLenArrSize)
			{
				pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32Len - i32PrevLen);
				(*pu32NodeCnt)++;
			}
		}
	}

	/* return total Encoded data length */
	return i32Len;
}
#endif /* !PROFILE_BOD_ONLY */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode bacnet Authentication Factor.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAuthFactor    [in]	base pointer of data to be encoded.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AuthFactor(uint8_t *pu8Apdu,
							 BACnetAuFactor_t *puAuthFactor)
{
	/* local variables */
	int32_t i32Len = 0;							  /* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAuthFactor)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* Encode Format Type */
	i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_0,
		(puAuthFactor->m_eFormatType));

	/* Encode Format Class */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_1,
		(puAuthFactor->m_u32FormatClass));

	/* Encode Value */
	i32Len += Encode_Context_Octet_String(&pu8Apdu[i32Len],TAG_NO_2,
		&(puAuthFactor->m_stOctetStrValue));

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode bacnet Authentication Policy.
*    
* @param pu8APDU	     [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAuthPolicy    [in]	base pointer of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AuthPolicy(uint8_t *pu8Apdu,
							 ListOfBACnetAuPolicy_t *puAuthPolicy,
							 int32_t u32MaxInBufSize)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	Policy_t *pstPolicyList = NULL;				/* Polict List Pointer */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAuthPolicy)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}
	
	/* get the value */
	pstPolicyList = &(puAuthPolicy->m_stPolicy);

	/* encode opening tag for Policy */
	i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);
	/* encode all elements in list & check max length is not exceeded */
	while(NULL != pstPolicyList && i32Len < u32MaxInBufSize)
	{
		/* encode opening tag for Credential Data Input */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);
		/* Encode Device Identifier */
		if(OBJECT_DEVICE == pstPolicyList->m_stCredentialDataInput.m_eDeviceType)
		{
			i32Len += Encode_Context_Object_Id(&pu8Apdu[i32Len], TAG_NO_0, 
				(int32_t)pstPolicyList->m_stCredentialDataInput.m_eDeviceType, 
				pstPolicyList->m_stCredentialDataInput.m_u32DeviceId);
		}
		/* Encode Object Identifier */
		i32Len += Encode_Context_Object_Id(&pu8Apdu[i32Len], TAG_NO_1,
			(int32_t)pstPolicyList->m_stCredentialDataInput.m_eObjectType, 
			pstPolicyList->m_stCredentialDataInput.m_u32ObjId);	
		/* encode Closing tag for Credential Data Input */
		i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);

		/* Encode Index */
		i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_1,
			(pstPolicyList->m_u32Index));
    
		/* Move to next value in list */
		pstPolicyList = pstPolicyList->m_pstNext;
	}
	/* encode opening tag for Policy */
	i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);
    	
	/* encode order enforced */
	i32Len += Encode_Context_Boolean(&pu8Apdu[i32Len],TAG_NO_1,
		(puAuthPolicy->m_bOrderEnforced));

	/* Encode timeout value */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len],TAG_NO_2,
		(puAuthPolicy->m_u32Timeout));
	
	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode bacnet Access Rule.
*    
* @param pu8APDU	  [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAccRule    [in]	base pointer of data to be encoded.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AccessRule(uint8_t *pu8Apdu,
							 ListOfBACnetAccessRule_t *puAccRule)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAccRule)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* Encode Time Range Specifier */
	i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_0,
		(puAccRule->m_eTimeRangeType));

	/* Encode Time Range if Time Range Specifier has the value "specified" */ 
	if( TIME_RANGE_SPECIFIER_SPECIFIED == puAccRule->m_eTimeRangeType)
	{
		/* encode opening tag for Time Range */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1);
		/* Encode Time Range value */
		i32Len += DT_Encode_DevObjPropReff(&pu8Apdu[i32Len], 
			&(puAccRule->m_stTimeRange));
		/* encode Closing tag for Time Range */
		i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1);
	}

	/* Encode Location Specifier */
	i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_2,
		(puAccRule->m_eLocationType));

	/* Encode Location if Time Range Specifier has the value "specified" */ 
	if( LOCATION_SPECIFIER_SPECIFIED == puAccRule->m_eLocationType)
	{
		/* encode opening tag for Location */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_3);
		/* Encode Time Range value */
		i32Len += DT_Encode_DevObjReff(&pu8Apdu[i32Len], 
			&(puAccRule->m_stLocation));
		/* encode Closing tag for Location */
		i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_3);
	}

	/* encode enable value */
	i32Len += Encode_Context_Boolean(&pu8Apdu[i32Len],TAG_NO_4,
		(puAccRule->m_bEnable));
  	
	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to bacnet Credential Authentication Factor.
*    
* @param pu8APDU	       [out] Buffer which will hold the encoded Bacnet octets.
* @param puCredAutehFact   [in]	 base pointer of data to be encoded.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_CredAutheFact(uint8_t *pu8Apdu, ListOfBACnetCredAuFactor_t *puCredAutehFact)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puCredAutehFact)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* Encode Disable Value */
	i32Len += Encode_Context_Enumerated(&pu8Apdu[i32Len],TAG_NO_0,
		(puCredAutehFact->m_eDisable));

	/* encode opening tag for Authentication factor */
	i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1);
	/* Encode Authentication factor */
	i32Len += DT_Encode_AuthFactor(&pu8Apdu[i32Len], &puCredAutehFact->m_stAuFactor);
  	/* encode Closing tag for Authentication factor */
	i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1);

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to bacnet Assigned Access Rights.
*    
* @param pu8APDU	      [out] Buffer which will hold the encoded Bacnet octets.
* @param puAssiAccRight   [in]	 base pointer of data to be encoded.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AssiAccRight(uint8_t *pu8Apdu, ListOfBACnetAssignedAccessRights_t *puAssiAccRight)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAssiAccRight)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* encode opening tag for Assigned Access Rights */
	i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);
	/* Encode Assigned Access Rights */
	i32Len += DT_Encode_DevObjReff(&pu8Apdu[i32Len], &puAssiAccRight->m_stAssAccessRights);
	/* encode Closing tag for Assigned Access Rights */
	i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);

	/* Encode Enable Value */
	i32Len += Encode_Context_Boolean(&pu8Apdu[i32Len], TAG_NO_1, puAssiAccRight->m_bEnable);

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Array of bacnet Authentication Factor format.
*    
* @param pu8APDU		   [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAuthFactFormArr [in]	base pointer to Array of data to be encoded.
* @param u32MaxInBufSize   [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	   [in]  Array Index Value.
* @param bArrIndxPresent   [in]  Array Index notification.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AuthFactForm_Array(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetAuFactorFormat_t *puAuthFactFormArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetAuFactorFormat_t *pstAuthFactFormArr = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAuthFactFormArr)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstAuthFactFormArr = puAuthFactFormArr->m_pstAuFactFormatList;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstAuthFactFormArr) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_AuthFactForm(&pu8Apdu[i32Len], pstAuthFactFormArr);
			/* move to next Array Index value */
			pstAuthFactFormArr = pstAuthFactFormArr->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&pu8Apdu[i32Len], puAuthFactFormArr->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstAuthFactFormArr)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstAuthFactFormArr = pstAuthFactFormArr->m_pstNext;
        }
        if(NULL != pstAuthFactFormArr)
        {
			i32Len = DT_Encode_AuthFactForm(&pu8Apdu[i32Len], pstAuthFactFormArr);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Array of bacnet Authentication Policy.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAuthPolicyArr [in]	base pointer to Array of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	 [in]  Array Index Value.
* @param bArrIndxPresent [in]  Array Index notification.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AuthPolicy_Array(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetAuPolicy_t *puAuthPolicyArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetAuPolicy_t *pstAuthPolicyArr = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAuthPolicyArr)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* get the value */
	pstAuthPolicyArr = puAuthPolicyArr->m_pstAuPolicy;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstAuthPolicyArr) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_AuthPolicy(
				&pu8Apdu[i32Len], pstAuthPolicyArr, u32MaxInBufSize);
			/* move to next Array Index value */
			pstAuthPolicyArr = pstAuthPolicyArr->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&pu8Apdu[i32Len], puAuthPolicyArr->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstAuthPolicyArr)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstAuthPolicyArr = pstAuthPolicyArr->m_pstNext;
        }
        if(NULL != pstAuthPolicyArr)
        {
			i32Len = DT_Encode_AuthPolicy(
				&pu8Apdu[i32Len], pstAuthPolicyArr, u32MaxInBufSize);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Array/List of access rule.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAccRuleArr    [in]	base pointer to Array of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	 [in]  Array Index Value.
* @param bArrIndxPresent [in]  Array Index notification.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AccessRule_Array(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetAccessRule_t *puAccRuleArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetAccessRule_t *pstAccRuleArr = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAccRuleArr)
	{
		return i32Len;
	}

	/* get the value */
	pstAccRuleArr = puAccRuleArr->m_pstAccessRuleArray;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstAccRuleArr) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_AccessRule(&pu8Apdu[i32Len], pstAccRuleArr);
			/* move to next Array Index value */
			pstAccRuleArr = pstAccRuleArr->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&pu8Apdu[i32Len], puAccRuleArr->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstAccRuleArr)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstAccRuleArr = pstAccRuleArr->m_pstNext;
        }
        if(NULL != pstAccRuleArr)
        {
			i32Len += DT_Encode_AccessRule(&pu8Apdu[i32Len], pstAccRuleArr);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode array of bacnet Credential Authentication Factor.
*    
* @param pu8APDU		   [out]	Buffer which will hold the encoded Bacnet octets.
* @param puCredAuFactArr   [in]	base pointer to Array of data to be encoded.
* @param u32MaxInBufSize   [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	   [in]  Array Index Value.
* @param bArrIndxPresent   [in]  Array Index notification.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_CredAutheFact_Array(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetCredAuFactor_t *puCredAuFactArr,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetCredAuFactor_t *pstCredAutheFact = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puCredAuFactArr)
	{
		return i32Len;
	}

	/* get the value */
	pstCredAutheFact = puCredAuFactArr->m_pstCredAuFactArray;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstCredAutheFact) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_CredAutheFact(&pu8Apdu[i32Len], pstCredAutheFact);
			/* move to next Array Index value */
			pstCredAutheFact = pstCredAutheFact->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&pu8Apdu[i32Len], puCredAuFactArr->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstCredAutheFact)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstCredAutheFact = pstCredAutheFact->m_pstNext;
        }
        if(NULL != pstCredAutheFact)
        {
			i32Len += DT_Encode_CredAutheFact(&pu8Apdu[i32Len], pstCredAutheFact);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode array of bacnet Assigned Access Rights.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param puAssiAccRight  [in]	base pointer to Array of data to be encoded.
* @param u32MaxInBufSize [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	 [in]  Array Index Value.
* @param bArrIndxPresent [in]  Array Index notification.
*
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_AssiAccRight_Array(
	uint8_t *pu8Apdu,
	Pr_ListOfBACnetAssignedAccessRights_t *puAssiAccRight,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;							/* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetAssignedAccessRights_t *pstAssiAccRight = NULL;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == puAssiAccRight)
	{
		return i32Len;
	}

	/* get the value */
	pstAssiAccRight = puAssiAccRight->m_pstAsngdAccessArray;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstAssiAccRight) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_AssiAccRight(&pu8Apdu[i32Len], pstAssiAccRight);
			/* move to next Array Index value */
			pstAssiAccRight = pstAssiAccRight->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&pu8Apdu[i32Len], puAssiAccRight->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstAssiAccRight)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstAssiAccRight = pstAssiAccRight->m_pstNext;
        }
        if(NULL != pstAssiAccRight)
        {
			i32Len += DT_Encode_AssiAccRight(&pu8Apdu[i32Len], pstAssiAccRight);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

#ifdef BACDEL_PR12

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode BACnet key identifier.
*    
* @param pu8APDU		 [out]	Buffer which will hold the encoded Bacnet octets.
* @param pstKeyIdentify  [in]	base pointer to Array of data to be encoded.*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_KeyIdentifier(
	uint8_t *pu8Apdu, 
	ListOfBACnetKeyId_t *pstKeyIdentify)
{
	/* local variables */
	int32_t i32Len = 0; /* Total Encoded Length */

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstKeyIdentify)
	{
		return i32Len;
	}

	/* Encode unsigned8 */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_0,
		pstKeyIdentify->m_u8Algorithm);

	/* Encode unsigned8 */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_1,
		pstKeyIdentify->m_u8KeyId);

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Security key set data type.
*    
* @param pu8Apdu		  [out]	pointer to save Encoded data.
* @param pstKeyIdentify	  [in]	pointer of data to be Encoded.
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_KeyIdentifier_List(
	uint8_t *pu8Apdu, 
	ListOfBACnetKeyId_t *pstKeyIdentify,
	int32_t u32MaxInBufSize)
{
	/* local variables */
	int32_t i32Len = 0;	  /* Total Encoded Length */	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstKeyIdentify)
	{
		return i32Len;
	}

	/* encode all elements in list & check max length is not exceeded */
	while((NULL != pstKeyIdentify) && (i32Len < u32MaxInBufSize))
	{
		i32Len += DT_Encode_KeyIdentifier(
			&pu8Apdu[i32Len], 
			pstKeyIdentify);

		/* move to next Array Index value */
		pstKeyIdentify = pstKeyIdentify->m_pstNext;
	}

	/* return total Encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Security key set data type.
*    
* @param u8TempAPDU		  [out]	pointer to save Encoded data.
* @param pstSecKeySet	  [in]	pointer of data to be Encoded.
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_SecurityKeySet(
	uint8_t *u8TempAPDU,
	BACnetSecurityKeySet_t *pstSecKeySet,
	int32_t u32MaxInBufSize)
{
	/* local variables */
	int32_t i32APDU_Len = 0; /* Total Encoded Length */

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstSecKeySet)
	{
		//i32APDU_Len = DT_ERR_INVALID_INPUTS;
		return i32APDU_Len;
	}

	/* Encode key revision */
    i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
		pstSecKeySet->m_u8KeyRevision);

	/* Opening Tag [1] */
	i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);
	
	/* Encode Activation date-time */
	i32APDU_Len = Encode_Application_Date(&u8TempAPDU[i32APDU_Len], 
		&(pstSecKeySet->m_stActivationTime.m_stDate));
	i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len], 
		&(pstSecKeySet->m_stActivationTime.m_stTime));							
									
    /* Closing Tag [1] */
	i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_1);

	/* Opening Tag [2] */
	i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);
	
	/* Encode Expiration date-time */
	i32APDU_Len = Encode_Application_Date(&u8TempAPDU[i32APDU_Len], 
		&(pstSecKeySet->m_stExpirationTime.m_stDate));
	i32APDU_Len += Encode_Application_Time(&u8TempAPDU[i32APDU_Len], 
		&(pstSecKeySet->m_stExpirationTime.m_stTime));							
		
	/* Closing Tag [2] */
	i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_2);

	/* Opening Tag [3] */
	i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_3);

	/* encode key id list */
	i32APDU_Len += DT_Encode_KeyIdentifier_List(&u8TempAPDU[i32APDU_Len],
		&pstSecKeySet->m_stKeyIds, u32MaxInBufSize);

	/* Closing Tag [3] */
	i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_3);
						
	/* return total Encoded data length */
	return i32APDU_Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Security key set data type.
*    
* @param u8TempAPDU		  [out]	pointer to save Encoded data.
* @param pstSecKeySet	  [in]	pointer of data to be Encoded.
* @param pstKeyIdentifier [in]	pointer of data to be Encoded.
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	  [in]  Array Index Value.
* @param bArrIndxPresent  [in]  Array Index notification.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_SecurityKeySet_Array(
	uint8_t *u8TempAPDU,
	Pr_ListOfBACnetSecurityKeySet_t *pstSecKeySet,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0; /* Total Encoded Length */
	uint8_t u8ByteCnt = 0; /* Used for for loop index */

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstSecKeySet)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		for(u8ByteCnt = 0; u8ByteCnt < BACNET_ARRAY_OF_TWO; u8ByteCnt++)
		{
			i32Len += DT_Encode_SecurityKeySet(&u8TempAPDU[i32Len],
				&pstSecKeySet->m_stSecurityKetSet[u8ByteCnt], u32MaxInBufSize);
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&u8TempAPDU[i32Len], pstSecKeySet->m_u32ArraySize);
    }
	else
    {
		if(u32ArrayIndex < BACNET_ARRAY_OF_TWO)
		{
			i32Len += DT_Encode_SecurityKeySet(&u8TempAPDU[i32Len],
				&pstSecKeySet->m_stSecurityKetSet[u32ArrayIndex-1], u32MaxInBufSize);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

#ifndef PROFILE_BOD_ONLY
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Network Security Policy - data type.*    
* @param u8TempAPDU		   [out]	pointer to save Encoded data.
* @param pstNetSecPlcyArr  [in]	    pointer of data to be Encoded.                               
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_NwSecurityPolicy(
	uint8_t *u8TempAPDU,
	ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcyArr)
{
	/* local variables */
	int32_t i32APDU_Len = 0; /* Total Encoded Length */
	
	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstNetSecPlcyArr)
	{
		return i32APDU_Len;
	}

	/* Encode unsigned8 */
	i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_0,
		pstNetSecPlcyArr->m_u8PortId);

	/* Encode Property Identifier */
	i32APDU_Len += Encode_Context_Enumerated(&u8TempAPDU[i32APDU_Len], TAG_NO_1,
		pstNetSecPlcyArr->m_eSecurityPolicy);	

	/* return total Encoded data length */
	return i32APDU_Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Network Security Policy - Array data type.
*    
* @param u8TempAPDU		  [out]	pointer to save Encoded data.
* @param pstNetSecPlcyArr [in]	pointer of data to be Encoded.
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	  [in]  Array Index Value.
* @param bArrIndxPresent  [in]  Array Index notification.*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_NwSecurityPolicy_Array(
	uint8_t *u8TempAPDU,
	Pr_ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcy,
	int32_t u32MaxInBufSize, 
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0; /* Total Encoded Length */
	uint8_t u8ByteCnt = 0;
	ListOfBACnetNwSecurityPolicy_t *pstNetSecPlcyArr = NULL;

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstNetSecPlcy)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	pstNetSecPlcyArr = pstNetSecPlcy->m_pstSecurityPolicyArr;

	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstNetSecPlcyArr) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_NwSecurityPolicy(&u8TempAPDU[i32Len], 
				pstNetSecPlcyArr);
			/* move to next Array Index value */
			pstNetSecPlcyArr = pstNetSecPlcyArr->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&u8TempAPDU[i32Len], pstNetSecPlcy->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstNetSecPlcyArr)
            {
                break;
			}
			/* move to next Array Index value */
			pstNetSecPlcyArr = pstNetSecPlcyArr->m_pstNext;
        }
        if(NULL != pstNetSecPlcyArr)
        {
			i32Len += DT_Encode_NwSecurityPolicy(&u8TempAPDU[i32Len], 
				pstNetSecPlcyArr);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}
#endif /* !PROFILE_BOD_ONLY */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Property Access Result - Array data type.
*    
* @param u8TempAPDU		  [out]	pointer to save Encoded data.
* @param pstPropAccRes    [in]	pointer of data to be Encoded.
* @param pstPropAccResArr [in]	pointer of data to be Encoded.                                 
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_PropAccessResult(
	uint8_t *u8TempAPDU,
	ListOfBACnetPropAccessRslt_t *pstPropAccResArr,
	int32_t u32MaxInBufSize,
	uint32_t u32SegSupport,
	uint16_t u16MaxApduLen)
{
	/* local variables */
	int32_t i32APDU_Len = 0; /* Total Encoded Length */

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstPropAccResArr)
	{
		//i32APDU_Len = DT_ERR_INVALID_INPUTS;
		return i32APDU_Len;
	}

	/* Encode Object Id  */
	i32APDU_Len += Encode_Context_Object_Id(
		&u8TempAPDU[i32APDU_Len], TAG_NO_0,
		(int32_t)pstPropAccResArr->m_eObjectType,
		pstPropAccResArr->m_u32ObjId);

	/* Encode Property id */
	i32APDU_Len += Encode_Context_Enumerated(
		&u8TempAPDU[i32APDU_Len], TAG_NO_1,
		pstPropAccResArr->m_ePropertyIdentifier);	

	/* check for array */
	if(ARRAY_INDEX_PRESENT == pstPropAccResArr->m_bIsArrayIndxPresent)
	{
		/* Encode array */
		i32APDU_Len += Encode_Context_Unsigned(&u8TempAPDU[i32APDU_Len], TAG_NO_2,
			pstPropAccResArr->m_u32ArrayIndex);
	}

	/* Check device id */
	if(OBJECT_DEVICE == pstPropAccResArr->m_eDevType)
	{
		i32APDU_Len += Encode_Context_Object_Id(
			&u8TempAPDU[i32APDU_Len], TAG_NO_3, 
			(int32_t)pstPropAccResArr->m_eDevType, 
			pstPropAccResArr->m_u32DevId);
	}

	/* encode property value or error */
	if(4 == pstPropAccResArr->m_u32UniMember)
	{
		/* Opening tag */
		i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_4);
		/* Encode Property value */
		i32APDU_Len += BACApp_Encode_Data_Type(&u8TempAPDU[i32APDU_Len], 
			(u32MaxInBufSize-i32APDU_Len), 
			pstPropAccResArr->m_stReadResult.m_stPropertyValue.m_pvConstrProp, 
			pstPropAccResArr->m_stReadResult.m_stPropertyValue.m_eData_Type, 
			pstPropAccResArr->m_u32ArrayIndex,
			0, u32SegSupport, pstPropAccResArr->m_bIsArrayIndxPresent,
			u16MaxApduLen, NULL, NULL, 0);
		// TODO - handle error 
		/* Closing Tag */
		i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_4);
	}

	/* Encode Error received */
	else if(5 == pstPropAccResArr->m_u32UniMember)
	{
		i32APDU_Len += Encode_Opening_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_5);
		/* Encode error class */
		i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len],
			pstPropAccResArr->m_stReadResult.m_stError.m_eErrorClass);
		/* Encode error code */
		i32APDU_Len += Encode_Application_Enumerated(&u8TempAPDU[i32APDU_Len],
			pstPropAccResArr->m_stReadResult.m_stError.m_eErrorCode);
		i32APDU_Len += Encode_Closing_Tag(&u8TempAPDU[i32APDU_Len], TAG_NO_5);
	}

	/* return total Encoded data length */
	return i32APDU_Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode Property Access Result - Array data type.
*    
* @param u8TempAPDU		  [out]	pointer to save Encoded data.
* @param pstPropAccRes    [in]	pointer of data to be Encoded.
* @param pstPropAccResArr [in]	pointer of data to be Encoded.   
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	  [in]  Array Index Value.
* @param bArrIndxPresent  [in]  Array Index notification.
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_PropAccessResult_Array(
	uint8_t *u8TempAPDU,
	Pr_ListOfBACnetPropAccessRslt_t *pstPropAccRes,
	int32_t u32MaxInBufSize,
	uint32_t u32SegSupport,
	uint16_t u16MaxApduLen,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0; /* Total Encoded Length */
	ListOfBACnetPropAccessRslt_t *pstPropAccResArr = NULL;
	uint8_t u8ByteCnt = 0;

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstPropAccRes)
	{
		return i32Len;
	}

	/* get property value pointer */
	pstPropAccResArr = pstPropAccRes->m_pstPropAccRslt;
	
	/* If none send , response with all */
    if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
    {
		/* encode all elements in list & check max length is not exceeded */
		while((NULL != pstPropAccResArr) && (i32Len < u32MaxInBufSize))
		{
			i32Len += DT_Encode_PropAccessResult(&u8TempAPDU[i32Len], pstPropAccResArr,
				(u32MaxInBufSize - i32Len), u32SegSupport, u16MaxApduLen);
			/* move to next Array Index value */
			pstPropAccResArr = pstPropAccResArr->m_pstNext;
		}
	}
	else if(0 == u32ArrayIndex)
    {
		/* encode the array size count */
        i32Len += Encode_Application_Unsigned(
			&u8TempAPDU[i32Len], pstPropAccRes->m_u32ArraySize);
    }
	else
    {
		for(u8ByteCnt = 0; u8ByteCnt < (u32ArrayIndex-1); u8ByteCnt++)
        {
			if(NULL == pstPropAccResArr)
            {
				i32Len = 0;
                break;
             }
             /* move to next Array Index value */
             pstPropAccResArr = pstPropAccResArr->m_pstNext;
        }
        if(NULL != pstPropAccResArr)
        {
			i32Len += DT_Encode_PropAccessResult(&u8TempAPDU[i32Len], pstPropAccResArr,
				u32MaxInBufSize, u32SegSupport, u16MaxApduLen);
		}
    }

	/* return total Encoded data length */
	return i32Len;
}

#endif /* BACDEL_PR12 */

#ifndef PROFILE_BOD_ONLY
/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode single node of special event array.
*    
* @param u8TempAPDU		    [out]	pointer to save Encoded data.
* @param pstSpecialEvent    [in]	pointer of data to be Encoded.
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_SpecialEvent(
	uint8_t *u8TempAPDU,
	BACnetSpecialEvent_t *pstSpecialEvent) 
{
	/* local variables */
	 BACnetTimeValue_t  *pstListOfTimeValues = NULL;
	 BACnetOctetStr_t stBACNET_OCTET_STRING = {0};
	 int32_t i32Len = 0; /* Total Encoded Length */

	 /* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstSpecialEvent)
	{
		return i32Len;
	}

	/* period */
    if(pstSpecialEvent->m_eStatusCalendar != STATUS_CAL_REFF)
    {
        i32Len += Encode_Opening_Tag(&u8TempAPDU[i32Len],TAG_NO_0);                        
        if(pstSpecialEvent->m_eStatusCalendar  == STATUS_DATE)
        {
			/* Encode date */
            i32Len += Encode_Context_Date(&u8TempAPDU[i32Len], TAG_NO_0, 
                &(pstSpecialEvent->m_stPeriod.m_stDate));
        }
        /* Date Range */
        else if(pstSpecialEvent->m_eStatusCalendar  == STATUS_DATE_RANGE)
        {   /* encode opening tag */
            i32Len += Encode_Opening_Tag(&u8TempAPDU[i32Len],TAG_NO_1);
            /* Encode start date */
            i32Len += Encode_Application_Date(&u8TempAPDU[i32Len], &(pstSpecialEvent->m_stPeriod.m_stDateRange.m_stStartDate));
            /* Encode End Date */
            i32Len += Encode_Application_Date(&u8TempAPDU[i32Len], &(pstSpecialEvent->m_stPeriod.m_stDateRange.m_stEndDate));
            /* Encode closeing tag */
            i32Len += Encode_Closing_Tag(&u8TempAPDU[i32Len],TAG_NO_1);
        }
        /* Week n Day */
        else if(pstSpecialEvent->m_eStatusCalendar == STATUS_WEEK_N_DAY)
        {
			stBACNET_OCTET_STRING.m_ai8OctetStr[0] = pstSpecialEvent->m_stPeriod.m_stWeekNDay.m_eMonth;
			stBACNET_OCTET_STRING.m_ai8OctetStr[1] = pstSpecialEvent->m_stPeriod.m_stWeekNDay.m_eWeekOfMonth;
			stBACNET_OCTET_STRING.m_ai8OctetStr[2] = pstSpecialEvent->m_stPeriod.m_stWeekNDay.m_eWeekNDay;
			stBACNET_OCTET_STRING.m_u32OctetCount = 3;
                i32Len += Encode_Context_Octet_String(&u8TempAPDU[i32Len],TAG_NO_2,
                &stBACNET_OCTET_STRING);
        }
        i32Len += Encode_Closing_Tag(&u8TempAPDU[i32Len],TAG_NO_0);
    }
    else
    {   /* Calendar reference */
        i32Len += Encode_Context_Object_Id(&u8TempAPDU[i32Len],TAG_NO_1,
			(int32_t)pstSpecialEvent->m_stPeriod.m_stCalReff.m_eObjectType,
            pstSpecialEvent->m_stPeriod.m_stCalReff.m_u32ObjId);
    }
    /* List of bacnetTimeValues */
    i32Len += Encode_Opening_Tag(&u8TempAPDU[i32Len],TAG_NO_2);
    pstListOfTimeValues = &(pstSpecialEvent->m_stListOfTimeValues);
    if(pstListOfTimeValues->bIsUsed)
    {
        while(NULL != pstListOfTimeValues)
        {  
            /* Time */
            i32Len += Encode_Application_Time(&u8TempAPDU[i32Len], 
                            &(pstListOfTimeValues->m_stTime));
            /* Values */
            i32Len += BACApp_Encode_Application_Data(&u8TempAPDU[i32Len],
                            &(pstListOfTimeValues->m_stPropValue));

            pstListOfTimeValues = pstListOfTimeValues->m_pstNext;
        }
    }
    i32Len += Encode_Closing_Tag(&u8TempAPDU[i32Len],TAG_NO_2);
    /* Event Priority */
    i32Len += Encode_Context_Unsigned(&u8TempAPDU[i32Len], TAG_NO_3,
        pstSpecialEvent->m_u32EventPriority);

	/* return total Encoded data length */
	return i32Len;
}
#endif /* !PROFILE_BOD_ONLY */

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode single node of optional character string.
*    
* @param u8TempAPDU		    [out]	pointer to save Encoded data.
* @param pstOptCharStr      [in]	pointer of data to be Encoded.
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_OptCharString(
	uint8_t *u8TempAPDU,
	ListOfOptCharStr_t *pstOptCharStr)
{
	/* local variables */
	int32_t i32APDU_Len = 0; /* Total Encoded Length */

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstOptCharStr)
	{
		//i32APDU_Len = DT_ERR_INVALID_INPUTS;
		return i32APDU_Len;
	}

	if(BACNET_APPLICATION_TAG_CHARACTER_STRING == pstOptCharStr->m_i8AppTag)
    {
        i32APDU_Len += Encode_Application_Character_String(
            &u8TempAPDU[i32APDU_Len], &pstOptCharStr->m_stCharStr);
    }
    else if(BACNET_APPLICATION_TAG_NULL == pstOptCharStr->m_i8AppTag)
    {
        i32APDU_Len += Encode_Application_Null(&u8TempAPDU[i32APDU_Len]);
    }

	/* return total Encoded data length */
	return i32APDU_Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to Encode single node of optional character string.
*    
* @param u8TempAPDU		    [out]	pointer to save Encoded data.
* @param pstArrayActCmd     [in]	pointer of data to be Encoded.
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_ActionList(
	uint8_t *u8TempAPDU,
	ListOfBACnetActionList_t  *pstArrayActCmd)
{
	/* local variables */
	int32_t i32APDU_Len = 0; /* Total Encoded Length */
	int32_t i32DecodeLen = 0;
	ListOfBACnetActionCommand_t *pstLstActCmd = NULL;
	uint32_t u32MaxInBufSize = 0;
	uint32_t u32SegSupport = 0;
	uint16_t u16MaxApduLen = 0;

	/* check input pointers */
	if(NULL == u8TempAPDU || NULL == pstArrayActCmd)
	{
		return i32APDU_Len;
	}

	pstLstActCmd = pstArrayActCmd->m_pstActionCommand;
	if(NULL == pstLstActCmd)
	{
		i32APDU_Len = DT_ERR_INVALID_INPUTS;
		return i32APDU_Len;
	}

	/* encoding opening tag of Action List*/
    i32APDU_Len += Encode_Opening_Tag(
		&u8TempAPDU[i32APDU_Len], TAG_NO_0);

	/* check for Device ID */
	if(pstLstActCmd->m_eDeviceType == OBJECT_DEVICE)
    {
        i32APDU_Len += Encode_Context_Object_Id(
			&u8TempAPDU[i32APDU_Len], TAG_NO_0,
			(int32_t)pstLstActCmd->m_eDeviceType,
            pstLstActCmd->m_u32DevId);
    }

    /* check for Object ID */
    i32APDU_Len += Encode_Context_Object_Id(
		&u8TempAPDU[i32APDU_Len], TAG_NO_1,
		(int32_t)pstLstActCmd->m_eObjectType,
         pstLstActCmd->m_u32ObjId);

    /* Encode Property Identifier */            
    i32APDU_Len += Encode_Context_Enumerated(
		&u8TempAPDU[i32APDU_Len], TAG_NO_2,
         pstLstActCmd->m_ePropertyIdentifier);
    /* Check for array */
    if(pstLstActCmd->m_bArrIndxFlag)
    {
        i32APDU_Len += Encode_Context_Unsigned(
			&u8TempAPDU[i32APDU_Len], TAG_NO_3,
            pstLstActCmd->m_u32ArrayIndex);
    }
	/* encode Opening tag for property Value */
    i32APDU_Len +=Encode_Opening_Tag(
		&u8TempAPDU[i32APDU_Len],TAG_NO_4); 
	 /* Encode Property Values */
	i32DecodeLen = BACApp_Encode_Data_Type(
		&u8TempAPDU[i32APDU_Len],
		(u32MaxInBufSize - i32APDU_Len),
		pstLstActCmd->m_stPropPointer.m_pvConstrProp,
		pstLstActCmd->m_stPropPointer.m_eData_Type,
		0, 0, u32SegSupport, false, u16MaxApduLen, NULL, NULL, 0);
	if(i32DecodeLen < 0)
	{
		//TODO - Error Need to be Handle
	}
	else
	{
		i32APDU_Len += i32DecodeLen;
	}

	/* encode Closing tag for property Value */
    i32APDU_Len += Encode_Closing_Tag(
		&u8TempAPDU[i32APDU_Len],TAG_NO_4);

    /* priority [5] Unsigned (1..16) OPTIONAL */
    if(pstArrayActCmd->m_pstActionCommand->m_bPriorityFlag)
    {
        i32APDU_Len += Encode_Context_Unsigned(
			&u8TempAPDU[i32APDU_Len], TAG_NO_5,
             pstLstActCmd->m_u32Priority);
    }
    /* postDelay [6] Unsigned OPTIONAL */
    if(pstArrayActCmd->m_pstActionCommand->m_bPostDelayFlag)
    {
        i32APDU_Len += Encode_Context_Unsigned(
			&u8TempAPDU[i32APDU_Len], TAG_NO_6,
             pstLstActCmd->m_u32PostDelay);
    }
    /* Encode QuitOnFailure */
    i32APDU_Len += Encode_Context_Boolean(
		&u8TempAPDU[i32APDU_Len], TAG_NO_7,
         pstLstActCmd->m_bQuitOnFailure);
    /* Encode WriteSuccessful */
    i32APDU_Len += Encode_Context_Boolean(
		&u8TempAPDU[i32APDU_Len], TAG_NO_8,
         pstLstActCmd->m_bWriteSuccess);

	/* encoding Closing tag of Action List */
    i32APDU_Len += Encode_Closing_Tag(
		&u8TempAPDU[i32APDU_Len], TAG_NO_0);

	return i32APDU_Len;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to encode address data type.
*    
* @param pu8APDU	[out]	pointer to save Encoded data.
* @param pstAddress	[in]	pointer of data to be Encoded.
*                                   
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_Address(
	uint8_t *pu8Apdu, 
	BACnetAddress_t *pstAddress)
{
	/* local variables */
	BACnetOctetStr_t stOctetStr = {0};
	int32_t i32Len = 0;	

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstAddress)
	{
		return i32Len;
	}

	/* encode network number */
	i32Len += Encode_Application_Unsigned(
		&pu8Apdu[i32Len], pstAddress->u16net);

	/* copy address */
	memcpy(&stOctetStr.m_ai8OctetStr[0], &pstAddress->u8IpAddrs[0], MAX_MAC_LEN);
	stOctetStr.m_u32OctetCount = pstAddress->u8mac_len;

	/* encode mac address */
	i32Len += Encode_Application_Octet_String(&pu8Apdu[i32Len], &stOctetStr);
	
	/* return total encoded data length */
	return i32Len;
}

/***
*                                                                    
* DESCRIPTION                                                                          
* Function to encode recipient data type.
*    
* @param pu8APDU		[out]	pointer to save Encoded data.
* @param pstRecipient	[in]	pointer to save recipient data.
*                                   
* @return [out] returns no of bytes decoded on success else -ve error value.
*
***/
int32_t DT_Encode_Recipient(
	uint8_t *pu8Apdu, 
	BACnetRecipient_t *pstRecipient)
{
	/* local variables */
	int32_t i32Len = 0;

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstRecipient)
	{
		return i32Len;
	}

	/* check if device id or device address */
	if(DESTINATION_IS_DEVICE_ID == pstRecipient->m_eDestinationType)
	{
		/* check for Object ID */
		i32Len += Encode_Context_Object_Id(
			&pu8Apdu[i32Len], TAG_NO_0,
			pstRecipient->BACnetRecipient_u.m_stObjId.m_u32ObjId,
			pstRecipient->BACnetRecipient_u.m_stObjId.m_u32ObjId);
	}
	else
	{
		/* encoding opening tag of address */
		i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_1);

		/* encode address */
		i32Len += DT_Encode_Address(&pu8Apdu[i32Len], 
			&pstRecipient->BACnetRecipient_u.m_stAddress);

		/* encoding closing tag of address */
		i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_1);
	}

	/* return total decoded data & length */
	return i32Len;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Encodes the property value. 
*    
* @param pstPropVal	 [in]  Pointer to property value structure.
* @param pu8APDUResp [out] Pointer to buffer to save encoded data.
*
* @return Number of Bytes consumed for encoding else -ve value.
*                                                                      
*/
int32_t DT_Encode_PropertyValue(
	property_value_t *pstPropVal, 
	uint8_t *pu8ApduData,
	uint32_t u32MaxInBufSize)
{
	/* local variables to save encoded length */
	int32_t i32Len = 0;
	int32_t i32EncodeLen = 0;

	/* check if empty property */
	if(BACNET_DT_EMPTY == pstPropVal->m_eData_Type)
	{
		/* no need to encode */
		return i32EncodeLen;
	}

	/* Encode PROPERTY ID */
	i32EncodeLen += Encode_Context_Enumerated(&pu8ApduData[i32EncodeLen],
		TAG_NO_0, pstPropVal->m_eObjectProperty);

	/* Encode ARRAY INDEX if present */
	if(ARRAY_INDEX_PRESENT == pstPropVal->m_bIsArrayIndxPresent)
	{
		i32EncodeLen += Encode_Context_Unsigned(&pu8ApduData[i32EncodeLen],
			TAG_NO_1, pstPropVal->m_i32PropertyArrayIndex);
	}

	/* Encode opening tag no 2 */
	i32EncodeLen += Encode_Opening_Tag(&pu8ApduData[i32EncodeLen], TAG_NO_2);
	
	/* Encode the property value */
	#ifdef NEW_RP_WP_INTERFACE
	i32Len = BACApp_Encode_Data_Type(&pu8ApduData[i32EncodeLen], 
		u32MaxInBufSize, pstPropVal->m_pvPropVal, pstPropVal->m_eData_Type, 
		pstPropVal->m_i32PropertyArrayIndex, i32EncodeLen, SEGMENTATION_BOTH,
		pstPropVal->m_bIsArrayIndxPresent, MAX_APDU_LENGTH_ACCEPTED, NULL, NULL, 0);
	if(i32Len < 0)
	{
		// TODO - handle the error
		i32Len = 0;
	}
	i32EncodeLen += i32Len;
	#endif

	/* Encode closing tag no 2 */
	i32EncodeLen += Encode_Closing_Tag(&pu8ApduData[i32EncodeLen], TAG_NO_2);

	/* Encode PRIORITY if present */
	if(BACNET_NO_PRIORITY != pstPropVal->m_u8Priority)
	{
		i32EncodeLen += Encode_Context_Unsigned(&pu8ApduData[i32EncodeLen],
			TAG_NO_3, pstPropVal->m_u8Priority);
	}

	/* return encoded data length */
	return i32EncodeLen;
}

#ifdef BACDEL_PR14
 
/*** 
* 
* DESCRIPTION                                                                           
* Function to Encode property list data type.
*    
* @param u8TempAPDU		  [out]	pointer to save Encoded data.
* @param pstPropertyList  [in]	pointer of data to be Encoded.
* @param u32MaxInBufSize  [in]	Maximum Buffer Size Value.
* @param u32ArrayIndex	  [in]  Array Index Value.
* @param bArrIndxPresent  [in]  Array Index notification.
*                                   
* @return [out] returns no of bytes Encoded on success else -ve error value.
*
***/
int32_t DT_Encode_PropertyList(
	uint8_t *pu8Apdu,
	Pr_BACnetPropertyList_t *pstPropertyList,
	int32_t u32MaxInBufSize,
	uint32_t u32ArrayIndex,
	bool bArrIndxPresent)
{
	/* local variables */
	int32_t i32Len = 0;
	uint8_t u8ByteCnt = 0;
	bool bReqPropEmpty = false;
	bool bOptPropEmpty = false;
	bool bEndOfReqPropList = false;
	bool bEndOfOptPropList = false;
	int32_t *psti32PropPtr = NULL;

	/* get pointer value of required property  */
	if(NULL != pstPropertyList)
	{
		psti32PropPtr = pstPropertyList->m_psti32RequiredProp;
	}
	else
	{
		/* invalid pointer, can't encode data */
		return i32Len;
	}

	/* check array index */
	if(ARRAY_INDEX_PRESENT != bArrIndxPresent)
	{
		/* encode all elements in list & check max length is not exceeded */
		while(NULL != psti32PropPtr && ENDOFPROPLIST != *psti32PropPtr)
		{
			/* skip encoding below 4 properties */
			if(PROP_OBJECT_NAME != *psti32PropPtr && 
				PROP_OBJECT_TYPE != *psti32PropPtr &&
				PROP_OBJECT_IDENTIFIER != *psti32PropPtr &&
				PROP_PROPERTY_LIST != *psti32PropPtr)
			{
				i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len], 
					(*psti32PropPtr));
			}
			/* move to next pointer value */
			psti32PropPtr++;
		}

		/* get pointer value of optional property  */
		psti32PropPtr = pstPropertyList->m_psti32OptionalProp;
		/* encode all elements in list & check max length is not exceeded */
		while(NULL != psti32PropPtr && ENDOFPROPLIST != *psti32PropPtr)
		{
			/* skip encoding below properties */
			if(PROP_PROPERTY_LIST != *psti32PropPtr)
			{
				i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len], 
					(*psti32PropPtr));
			}
			/* move to next pointer value */
			psti32PropPtr++;
		}

		/* get pointer value of Proprietary property  */
		psti32PropPtr = pstPropertyList->m_psti32ProprietaryProp;
		/* encode all elements in list & check max length is not exceeded */
		while(NULL != psti32PropPtr && ENDOFPROPLIST != *psti32PropPtr)
		{
			/* encode value */
			i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len], 
				(*psti32PropPtr));
			/* move to next pointer value */
			psti32PropPtr++;
		}
	}
	else if(u32ArrayIndex == 0)
	{
		/* encode count */
		i32Len += Encode_Application_Unsigned(&pu8Apdu[i32Len], 
				(pstPropertyList->m_u32PropCount));
	}
	else
	{
		/* encode requested data */
		for(u8ByteCnt = 0; u8ByteCnt < u32ArrayIndex; u8ByteCnt++)
        {
			/* validate the pointer */
			if(NULL == psti32PropPtr)
			{
				break;
			}

			/* encode all elements in list & check max length is not exceeded */
			if(ENDOFPROPLIST != *psti32PropPtr && !bReqPropEmpty && !bOptPropEmpty)
			{
				/* skip encoding below 4 properties */
				if(PROP_OBJECT_NAME == *psti32PropPtr || 
					PROP_OBJECT_TYPE == *psti32PropPtr ||
					PROP_OBJECT_IDENTIFIER == *psti32PropPtr ||
					PROP_PROPERTY_LIST == *psti32PropPtr)
				{
					/* decrement count value */
					u8ByteCnt--;
				}

				if(BACNET_ZERO != u8ByteCnt)
				{
					/* move to next pointer value */
					psti32PropPtr++;
					/* require property list is empty, go for optional property list */
					if(ENDOFPROPLIST == *psti32PropPtr)
					{
						/* array index not found under required property */
						bReqPropEmpty = true;
						bEndOfReqPropList = true;
						/* get pointer value of optional property  */
						psti32PropPtr = pstPropertyList->m_psti32OptionalProp;
					}
				}
			}

			/* search in optional property list */
			if(bReqPropEmpty && !bOptPropEmpty)
			{
				/* encode all elements in list & check max length is not exceeded */
				if(ENDOFPROPLIST != *psti32PropPtr)
				{
					/* skip encoding below properties */
					if(PROP_PROPERTY_LIST == *psti32PropPtr)
					{
						/* decrement count value */
						u8ByteCnt--;
					}
					
					/* get base pointer of list, for first element no need to increment pointer value */
					if(!bEndOfReqPropList)
					{
						/* move to next pointer value */
						psti32PropPtr++;
						/* optional property list is empty, go for proprietory property list */
						if(ENDOFPROPLIST == *psti32PropPtr)
						{
							/* array index not found under optional property */
							bOptPropEmpty = true;
							bEndOfOptPropList = true;
							/* get pointer value of proprietary property */
							psti32PropPtr = pstPropertyList->m_psti32ProprietaryProp;
						}
					}
					else
					{
						/* set flag value, so pointer value will be incremented from next value */
						bEndOfReqPropList = false;
					}
				}
			}

			/* search in proprietary property list */
			if(bOptPropEmpty)
			{
				/* encode all elements in list & check max length is not exceeded */
				if(ENDOFPROPLIST != *psti32PropPtr)
				{
					/* get base pointer of list, for first element no need to increment pointer value */
					if(!bEndOfOptPropList)
					{
						/* move to next pointer value */
						psti32PropPtr++;
					}
					else
					{
						/* set flag value, so pointer value will be incremented from next value */
						bEndOfOptPropList = false;
					}
				}
			}
		}

		/* encode array index value */
		if(NULL != psti32PropPtr && ENDOFPROPLIST != *psti32PropPtr)
		{
			/* encode value at specified index */
			i32Len += Encode_Application_Enumerated(&pu8Apdu[i32Len], (*psti32PropPtr));
		}
	}

	/* return total decoded data & length */
	return i32Len;
}
/**
*
* DESCRIPTION
* Function to encode bacnet host address data type.
*
* @param pu8APDU		[out]	pointer to save encoded data.
* @param pstHostAddress	[in]	pointer of data to be encoded.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_HostAddress(
	uint8_t *pu8Apdu,
	BACnetHostAddress_t *pstHostAddress)
{
	/* local variables */
	int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_HostAddress: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstHostAddress)
	{
		return i32Len;
	}

	/* encode host address */
	switch(pstHostAddress->m_eDataType)
	{
	case BACNET_DT_NULL:
	case BACNET_DT_NULL_CONTEXT:
	{
		/* null value */
		i32Len += Encode_Context_Null(&pu8Apdu[i32Len], TAG_NO_0);
	}
	break;

	case BACNET_DT_OCTETSTRING:
	{
		/* encode ip-address */
		i32Len += Encode_Context_Octet_String(&pu8Apdu[i32Len], TAG_NO_1,
			&pstHostAddress->BACnetHostAddr_u.m_stIPAddress);
	}
	break;

	case BACNET_DT_CHARSTRING:
	{
		/* encode name */
		i32Len += Encode_Context_Character_String(&pu8Apdu[i32Len], TAG_NO_2,
			&pstHostAddress->BACnetHostAddr_u.m_stName);
	}
	break;

	default:
		break;
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_HostAddress: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}
/**
*
* DESCRIPTION
* Function to encode bacnet host n port data type.
*
* @param pu8APDU		[out]	pointer to save encoded data.
* @param pstHostNPort	[in]	pointer of data to be encoded.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_HostNPort(
	uint8_t *pu8Apdu,
	BACnetHostNPort_t *pstHostNPort)
{
	/* local variables */
	int32_t i32Len = 0;
//	uint8_t u8Index = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_HostNPort: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstHostNPort)
	{
		//i32Len = DT_ERR_INVALID_INPUTS;
		return i32Len;
	}

	/* encode opening tag for host n port */
	i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);

	/* encode host address */
	i32Len += DT_Encode_HostAddress(&pu8Apdu[i32Len], &pstHostNPort->m_stHostAddress);

	/* encode closing tag for host n port */
	i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);

	/* encode port */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_1,
		pstHostNPort->m_u16Port);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_HostNPort: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}

/**
*
* DESCRIPTION
* Function to encode BDT entry data type.
*
* @param pu8APDU		[out]	pointer to save encoded data.
* @param pstBDTEntry	[in]	pointer of data to be encoded.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_BdtEntry(
	uint8_t *pu8Apdu,
	BACnetBDTEntry_t *pstBDTEntry)
{
	/* local variables */
	int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_BdtEntry: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstBDTEntry)
	{
		return i32Len;
	}

	/* do not encode if this entry is not used */
	if(!pstBDTEntry->m_bEntryUsed)
	{
		return i32Len;
	}

	/* encode opening tag for bbmd-address */
	i32Len += Encode_Opening_Tag(&pu8Apdu[i32Len], TAG_NO_0);

	/* encode bbmd address */
	i32Len += DT_Encode_HostNPort(&pu8Apdu[i32Len],
		&pstBDTEntry->m_stBBMDAddress);

	/* encode closing tag for bbmd-address */
	i32Len += Encode_Closing_Tag(&pu8Apdu[i32Len], TAG_NO_0);

	/* encode optional broadcast mask if present */
	if(pstBDTEntry->m_bBrdcastMaskFlag)
	{
		i32Len += Encode_Context_Octet_String(&pu8Apdu[i32Len], TAG_NO_1,
			&pstBDTEntry->m_stBrdcastMask);
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_BdtEntry: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}

/**
*
* DESCRIPTION
* Function to encode list of BDT entry data type.
*
* @param pu8Apdu			[out]	pointer to save encoded data.
* @param pstBDTEntryList	 [in]	pointer of data to be encoded.
* @param u32MaxInBufSize	 [in]	maximum buffer size value.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_BdtEntry_List(
	uint8_t *pu8Apdu,
	ListOfBACnetBDTEntry_t *pstBDTEntryList,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize)
{
	/* local variables */
	int32_t i32Len = 0;
	int32_t i32PrevLen = 0;
	bool bReadRangeFlag = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_BdtEntry_List: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstBDTEntryList)
	{
		return i32Len;
	}

	/* if encode function is called from ReadRange-B service
	make node count of nodes to 0 and set the flag */
	if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
	{
		*pu32NodeCnt = 0;
		bReadRangeFlag = TRUE;
	}

	/* encode all elements in list & check max length is not exceeded */
	while((NULL != pstBDTEntryList) && (i32Len < u32MaxInBufSize))
	{
		/* save previous encoded data length */
		i32PrevLen = i32Len;

		/* encode landing call status */
		i32Len += DT_Encode_BdtEntry(&pu8Apdu[i32Len],
			&pstBDTEntryList->m_stBDTEntry);

		/* move to next node */
		pstBDTEntryList = pstBDTEntryList->m_pstNext;

		/* if encode function called from ReadRange-B service, save the length of
		   each node in array */
		if(bReadRangeFlag)
		{
			if(*pu32NodeCnt < i32NodeLenArrSize)
			{
				pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32Len - i32PrevLen);
				(*pu32NodeCnt)++;
			}
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_BdtEntry_List: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}

/**
*
* DESCRIPTION
* Function to encode FDT entry data type.
*
* @param pu8APDU		[out]	pointer to save encoded data.
* @param pstFDTEntry	[in]	pointer of data to be encoded.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_FdtEntry(
	uint8_t *pu8Apdu,
	BACnetFDTEntry_t *pstFDTEntry)
{
	/* local variables */
	int32_t i32Len = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_FdtEntry: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstFDTEntry)
	{
		return i32Len;
	}

	/* do not encode if this entry is not used */
	if(!pstFDTEntry->m_bEntryUsed)
	{
		return i32Len;
	}

	/* encode ip address */
	i32Len += Encode_Context_Octet_String(&pu8Apdu[i32Len], TAG_NO_0,
		&pstFDTEntry->m_stIPAddress);

	/* encode time to live */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_1,
		pstFDTEntry->m_u16TimeToLive);

	/* encode remaining time to live */
	i32Len += Encode_Context_Unsigned(&pu8Apdu[i32Len], TAG_NO_2,
		pstFDTEntry->m_u16RemainingTimeToLive);

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_FdtEntry: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}

/**
*
* DESCRIPTION
* Function to encode list of FDT entry data type.
*
* @param pu8Apdu			[out]	pointer to save encoded data.
* @param pstFDTEntryList	 [in]	pointer of data to be encoded.
* @param u32MaxInBufSize	 [in]	maximum buffer size value.
*
* @return [out] returns no of bytes encoded on success else -ve error value.
*
*/
int32_t DT_Encode_FdtEntry_List(
	uint8_t *pu8Apdu,
	ListOfBACnetFDTEntry_t *pstFDTEntryList,
	int32_t u32MaxInBufSize,
	uint16_t *pu16NodeLenArr,
	int32_t *pu32NodeCnt,
	int32_t i32NodeLenArrSize)
{
	/* local variables */
	int32_t i32Len = 0;
	int32_t i32PrevLen = 0;
	bool bReadRangeFlag = FALSE;

	/* function entry */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE,
		"APDU: DT_Encode_FdtEntry_List: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pu8Apdu || NULL == pstFDTEntryList)
	{
		return i32Len;
	}

	/* if encode function is called from ReadRange-B service
	make node count of nodes to 0 and set the flag */
	if(NULL != pu32NodeCnt && NULL != pu16NodeLenArr && 0 != i32NodeLenArrSize)
	{
		*pu32NodeCnt = 0;
		bReadRangeFlag = TRUE;
	}

	/* encode all elements in list & check max length is not exceeded */
	while((NULL != pstFDTEntryList) && (i32Len < u32MaxInBufSize))
	{
		/* save previous encoded data length */
		i32PrevLen = i32Len;

		/* encode fdt entry list */
		i32Len += DT_Encode_FdtEntry(&pu8Apdu[i32Len],
			&pstFDTEntryList->m_stFDTEntry);

		/* move to next node */
		pstFDTEntryList = pstFDTEntryList->m_pstNext;

		/* if encode function called from ReadRange-B service, save the length of
		   each node in array */
		if(bReadRangeFlag)
		{
			if(*pu32NodeCnt < i32NodeLenArrSize)
			{
				pu16NodeLenArr[*pu32NodeCnt] = (uint16_t)(i32Len - i32PrevLen);
				(*pu32NodeCnt)++;
			}
		}
	}

	/* function exit */
	#if(defined DEBUG_PRINTF_1 && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE,
		"APDU: DT_Encode_FdtEntry_List: Exit \r\n");
	#endif
	/* return total encoded data length */
	return i32Len;
}
#endif /* BACDEL_PR14 */

/*********************** End of pduEncodeDecodeDT.c fle **********************/
