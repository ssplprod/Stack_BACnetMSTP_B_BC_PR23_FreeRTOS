/*************************************************************************
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
*   File Name - propertyValueWrite.h
*
*   RELEASE HISTORY
*      
*        DATE                  NAME               DESCRIPTION
*
*   12/07/2011              Prashant Badgujar     File Created & Added
*                                                 generic read API.
*   26/07/2011              M.Venu                Write Funciton return type
*                                                 modified to BACNET_RETURN_TYPE
*************************************************************************/

#ifndef WRITE_H
#define WRITE_H

#include "objDevice.h"

PROP_ACCESS_TYPE Get_Prop_Access_Type(virtualDevData_t *pVirtualDev,
                           BACNET_CONF_DATA *pWpData);

bool Is_Context_specific_prop(BACNET_PROPERTY_ID  eObjectProperty, BACNET_OBJECT_TYPE eObjectType);

void RemovePreviousElement(BACNET_CONF_DATA *pWpData, void *pvObject);

/** Function returns property tag-type based on property ID and object ID */
BACNET_APPLICATION_TAG GetPropertyTag(
	BACNET_OBJECT_TYPE eObjectType, 
	BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex,
	bool bArrIndxPresent);

/** Function returns property data-type based on property ID and object ID */
BACNET_DATA_TYPE GetPropertyDataType(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty,
    uint32_t u32ArrayIndex,
    bool bArrIndxPresent);

void GetPriorityArrTag(BACNET_OBJECT_TYPE eObjectType,
                              BACNET_APPLICATION_TAG *peTagType);

/**
* Description:
*   Check Range of input Proprty Value 
*/
BACNET_ERROR_CODE CheckPropertyValRange(void *pvObjectPrt, uint32_t ObjType,
                             uint32_t ObjInstance, BACNET_PROPERTY_ID ObjProperty, 
                             BACNET_PROPERTY_VALUE *pstAppDataValue,
                             void  *pvContextValue,
                             uint32_t u32ArrayIndex, uint32_t u32DevID,
                             bool bArrIndxPresent, bool bIsReqFromApp, 
                             void *pVirtualDev, 
							 BACNET_DATA_TYPE eDataType);

BACNET_RETURN_TYPE ValidateWriteTag                
                (BACNET_OBJECT_TYPE eObjectType, 
                BACNET_PROPERTY_ID eObjectProperty , 
                BACNET_PROPERTY_VALUE *appDataValue,
                uint32_t u32ArrayIndex,
                bool bArrIndxPresent);

/** Api to check if property is writable or not, even though its access type 
    states that it is read only. */
bool Is_Property_Writable(BACNET_PROPERTY_ID ePropId, BACNET_OBJECT_TYPE eObjTYpe, 
                     void *pvObjectPrt, void *pVirtualDev);

/** function to validate if property supports writting null value */
BACNET_ERROR_CODE ValidateNullTag(
	BACNET_PROPERTY_ID ePropId,
	BACNET_OBJECT_TYPE eObjectType,
	PROP_ACCESS_TYPE eAccessType);

/* function to copy the pointer received from application as per the data type */
BACNET_RETURN_TYPE Copy_PropVal_AsPer_DataType(
	void *pvPropVal,
	BACNET_DATA_TYPE eDataType,
	void **pvOutData);

/** function reads WP callback config value based on property ID and object ID */
BACNET_CALLBACK_CONFIG_TYPE GetPropertyWpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty);

/** function writes WP callback config value based on property ID and object ID */
BACNET_CALLBACK_CONFIG_TYPE SetPropertyWpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty,
	BACNET_CALLBACK_CONFIG_TYPE ePropCBType);

/**
*
* DESCRIPTION                                                                          
* This API function to copy the property value from pointer, 
* received from application as per the data type.
*
* @param eDataType		   [in] data type of property.
* @param pvInData		   [in] input data pointer.
* @param pvOutData		   [out]pointer to copy out data.
* @param i32ArrayIdx	   [in] array index 

* @return MAX_BACNET_ERROR_CODE on success
*
*/
BACNET_ERROR_CODE Convert_PropVal_AsPer_DataType(
	BACNET_DATA_TYPE eDataType, 
	void *pvInData, 
	void **pvOutData, 
	int32_t i32ArrayIdx, 
	bool bArrayIndexPresent,
	BACNET_PROPERTY_VALUE *pstPropVal,
	bool *pbWriteNULL,
	bool bCreateCopy);

/**
*
* DESCRIPTION                                                                          
* Function to set property value of the device.
*
* @param eObjType		   [in] object type.
* @param u32ObjectID	   [in] device identifier.
* @param pVirtualDev	   [in] virtual device data.
* @param eDevProp	       [in] device property.
* @param pvWritePropVal    [in] data value pointer.
* @param bArrayIndexPresent[in] flag for array index.
* @param eDataType         [in] property data type.
*
* @return MAX_BACNET_ERROR_CODE on success
*/
BACNET_ERROR_CODE Set_Property_Data(
	uint32_t u32DevId,
	BACNET_OBJECT_TYPE eObjType,
	uint32_t u32ObjectID,
	virtualDevData_t *pVirtualDev,
	BACNET_PROPERTY_ID eDevProp,
	void *pvWritePropVal,
	bool bArrayIndexPresent,
	int32_t i32ArrayIndex,
	uint8_t u8Priority,
	BACNET_DATA_TYPE eDataType);

/**
*
* DESCRIPTION                                                                          
* Function to get current count of number of elements in 
* array type property.
*
* @param pvData			   [in]  pointer having property offset.
* @param eDataType	   [in/out]  property data type.
* @param pstUnsignedVal   [out]  unsigned structure.
*
* @returns MAX_BACNET_ERROR_CODE on success
*
*/
BACNET_ERROR_CODE Get_Array_DataType_Count(
	BACNET_DATA_TYPE *peDataType,
	void *pvData,
	Pr_BACnetUnsigned32_t *pstUnsignedVal);

/**
*
* DESCRIPTION                                                                          
* Function to validate received data type against stack data type.
*
* @param eDevProp    [in] device property.
* @param eObjType    [in] object type.
* @param eDataType	 [in] property data type.
*
* @returns MAX_BACNET_ERROR_CODE on success
*
*/
BACNET_ERROR_CODE Validate_DataType(
	BACNET_PROPERTY_ID eDevProp,
	BACNET_OBJECT_TYPE eObjType,
	BACNET_DATA_TYPE eDataType,
	PROP_ACCESS_TYPE eAccessType);

#if (defined BACDEL_SER_DS_WP_B || defined BACDEL_SER_DS_WPM_B)
/**
*                                                                    
* DESCRIPTION                                                                          
* This function to read property of any object.
*    
* @param pVirtualDev	[in]	  virtual device data.
* @param pWpData		[in/out]  pointer to save property information.
* @param pstRmtDvAddr   [in]	  remote device address
* @param pstTimeStamp   [in]	  time stamp at which request received 
* @param u8ThreadNo     [in]	  thread number
*                                   
* @return TRUE/FALSE	[out]	  true or false.
*	
*/
BACNET_RETURN_TYPE WP_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	BACNET_CONF_DATA *pWpData,
	BACnetAddress_t *pstRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp,
	uint8_t u8ThreadNo);
#endif

/** 
*
* DESCRIPTION
* Compare received PV and actual PV value in stack. 
* 
* @param  pWpData         [in]     Confirm service data pointer
* @param  pvappValue      [in]     Context data pointer
* @param  i8TagType       [in]     Application tag number
*
* @return pbSameDataCopy  [out]    If same data is received then return 
*								   True else false
* @return pbWriteNULL     [out]    If NULL data is received then return 
*								   True else false
*/
void WP_B_Compare_PropVal_AsPer_DataType(
	bool *pbSameDataCopy,
	bool *pbWriteNULL,
	BACNET_CONF_DATA *pWpData,
	void *pvNewPropVal,
	BACNET_APPLICATION_TAG eAppTagType,
	BACNET_DATA_TYPE eOrgDataType);
	
/**
*
* DESCRIPTION
* Function to validate read only property.
*
* @param eObjType  [in] object type.
* @param eDevProp  [in] device property.
*
* @return True if read-only property
*
*/
bool Validate_ReadOnlyProperty(
BACNET_OBJECT_TYPE eObjType,
BACNET_PROPERTY_ID eDevProp);

/**
*   Function to check if property of array type is empty.
*
*   @param ePropId [in] property id.
*   @param eObjectType [in] object type.
*   @returns false if empty datatype is allowed.
*
*/
bool ValidateEmptyTag(
BACNET_PROPERTY_ID eObjectProperty,
BACNET_OBJECT_TYPE eObjectType);

/**
*
* DESCRIPTION
* Function to validate read only properties.
* These properties can be updated only before stack initialization.
*
* @param eObjType  [in] object type.
* @param eDevProp  [in] device property.
*
* @return True if property can be written.
*
*/
bool Is_ReadOnly_Property_Writable(
BACNET_OBJECT_TYPE eObjType,
BACNET_PROPERTY_ID eDevProp);


/**
*
* DESCRIPTION
* This function to copy list of recipient info in COV subscription.
*
* @param pstDestCovRcpt		 [in] address to destination pointer.
* @param pstSrcCovRcpt		 [in] source pointer.
*
* @return BACNET_RETURN_TYPE [out] either success or error.
*
*/
BACNET_RETURN_TYPE Copy_COVRecipeint_Info(
CovRecipientInfo_t **pstDestCovRcpt,
CovRecipientInfo_t *pstSrcCovRcpt);

/**
*
* Function to Set NULL value to internal pointer as per data types.
*
* @param peDataType [in] base pointer of data type.
* @param ppPropValue [in] base pointer to free Memory.
* @return - void;
*
*/
void Set_Null_Value_AsPer_DataType(
BACNET_DATA_TYPE eDataType,
void *pvPropValue);	

#endif /* WRITE_H */
