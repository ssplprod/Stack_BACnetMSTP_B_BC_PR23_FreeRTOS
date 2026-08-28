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
*   File Name - propertyValueRead.h
*
*   RELEASE HISTORY                                                       *      
*        DATE                  NAME                    DESCRIPTION
*
*   12/07/2011              Prashant Badgujar         File Created & Added
*                                                     generic read API.
*   03/08/2011              M.Venu                    Added ValidateAccessAndIndex
*                                                     Function prototype
*   16/09/2011              M.Venu                    Input paramters modified 
*                                                     for function
*                                                     Read_Common_Properties
*   23/09/2011              Ashish Verma              Modified fucntion
*                                                     Read_Common_Properties.
******************************************************************************/

#ifndef READ_H
#define READ_H

#include "objDevice.h"

/**                                                                      
*DESCRIPTION                                                              *            
*   This API reads object property for given object type.
*
*@param pVirtualDev [in] Virtual device data structure
*       containing service request date with object type,
*       object id and Object Property to read.
*@param pRpData [in/out] Read Property data structure which 
*       will give object type to read & application data to 
*       point property read.
*
*@return None
*
*/
BACNET_ERROR_CODE ValidateNonArrayTypeProperty(BACNET_PROPERTY_ID eObjectProperty,
                                             PROP_ACCESS_TYPE ePremission ,
                                             uint32_t i32ArrayIndex,
                                             virtualDevData_t *pVirtualDev);

void *Find_Object(BACNET_OBJECT_TYPE  eObjectType,
                         uint32_t u32InObjID,
                         virtualDevData_t *pVirtualDev);

/** api to validete array index */
BACNET_ERROR_CODE ValidateArrayIndex(
                BACNET_PROPERTY_ID eObjectProperty,
                BACNET_OBJECT_TYPE eObjType,
                uint32_t u32ArrayIndex,
                void *pvObject, bool bIsArrayIndexPresent,
				ulong32_t ul32PropOffsetAddr);

/** api to validate access type */
BACNET_ERROR_CODE ValidateAccessType(
                BACNET_PROPERTY_ID eObjectProperty,
                BACNET_OBJECT_TYPE eObjType,
                void *pvObject,
                ulong32_t *pul32PropOffsetAddr);

/** api to validate access type & array index both */
BACNET_ERROR_CODE ValidateAccessAndIndex(
                BACNET_PROPERTY_ID eObjectProperty,
                BACNET_OBJECT_TYPE eObjType,
                uint32_t u32ArrayIndex,
                void *pvObject,
                ulong32_t *pul32PropOffsetAddr,
                bool bIsArrayIndexPresent);

void UpdateValuePropDataType(BACNET_OBJECT_TYPE eObjectType,
                                    BACNET_DATA_TYPE *peDataType);
                                    
#ifndef PROFILE_BAWS_ONLY
BACNET_ERROR_CODE Read_Common_Properties(BACNET_OBJECT_TYPE eObjectType,
                                         BACNET_PROPERTY_ID eObjectProperty ,
                                         uint32_t u32ArrayIndex,
                                         BACNET_DATA_TYPE *peData_Type ,
                                         void **pvReadPropValue, 
                                         void *pvObject,
										 void *pVirtualDev,
                                         bool bArrayIndexPresent);
#endif

BACNET_ERROR_CODE Read_All_Properties(BACNET_OBJECT_TYPE eObjectType,
                                         BACNET_PROPERTY_ID eObjectProperty ,
                                         uint32_t u32ArrayIndex,
                                         BACNET_DATA_TYPE *peData_Type,
                                         void **pvReadPropValue, 
                                         void *pvObject,
										 void *pVirtualDev,
                                         uint32_t u32ObjId,
                                         bool bArrayIndexPresent);

void Generate_RPB_Data(uint32_t u32ObjId, 
                       uint8_t u8ObjectType, 
                       uint8_t u8PropId, 
                       int32_t i32ArrayIndex,  
                       BACNET_CONF_DATA *pRpData,
                       bool bIsArrayIndexPresent);

/** Api to check if property is of type list or array of list. */
BACNET_ERROR_CODE ValidateListTypeProperty(
    BACNET_PROPERTY_ID ePropId, BACNET_OBJECT_TYPE eObjectType);

/** function for list of properties for RPM */
BACNET_PROPERTY_ID RPM_Object_Property(
    special_property_list_t *pstPropertyList,
    BACNET_PROPERTY_ID eSpecialProperty,
    uint32_t u32Index);

/** function for count of properties for RPM */
uint32_t RPM_Object_Property_Count(
    special_property_list_t *pstPropertyList,
    BACNET_PROPERTY_ID eSpecialProperty);

/** function for RPM fallback mechanism */
bool RPMA_Special_FallBack(
	Bacnet_Multiple_Data_t* pstRpmReq, 
	bool bGetUnSupported);

/** function used in RPM fallback mechanism */
Bacnet_Multiple_Data_t * UpdateRpmDataForFallBack(
	Bacnet_Multiple_Data_t *pstRpmData,
    BACNET_PROPERTY_ID    ePropID,
    bool bLastProp);

/** function reads RP callback config value based on property ID and object ID */
BACNET_CALLBACK_CONFIG_TYPE GetPropertyRpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty);

/** function writes RP callback config value based on property ID and object ID */
BACNET_CALLBACK_CONFIG_TYPE SetPropertyRpCallback(
	BACNET_OBJECT_TYPE eObjectType, 
    BACNET_PROPERTY_ID eObjectProperty,
    BACNET_CALLBACK_CONFIG_TYPE ePropCBType);

/** function to check whether property is readable */
bool IsPropertyReadable(
    BACNET_PROPERTY_ID ePropId, 
    BACNET_OBJECT_TYPE eObjType);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function to validate property of RP/RPM request.
*    
* @param pVirtualDev	[in]	  virtual device data.
* @param pstRpData		[in/out]  request pointer to save error.
*                                   
* @return TRUE/FALSE	[out]	  FALSE - If any error occur in request.
*								  TRUE  - If success.
*	
*/
bool Validate_ObjId_PropId_ArrayIndx(
	BACNET_CONF_DATA *pstRpData,
	virtualDevData_t *pVirtualDev);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function to get property values base address for encoding 
* for RP/RPM property value response.
* Function creates duplicate copy of property if array or list type.
* All special handling related to properties are handled in this function.
*    
* @param pVirtualDev	[in]	  virtual device data.
* @param pstReqData	    [in/out]  RP/RPM request data pointer to save value/error
*                                   
*/
void Read_All_Property_Value(
	BACNET_CONF_DATA *pstReqData,
	virtualDevData_t *pVirtualDev);
	
/**
*
* DESCRIPTION
* Function to handle special cases depending upon property type.
* These propetries are mainly updated when a read request is received.
*
* @param eObjectType		[in]	  object type.
* @param eObjectProperty	[in/out]  property ID.
* @param pvObject			[in]	  base addrees of object.
* @param ul32OffsetAddr		[in]	  offset addrees/ property address.
* @param pVirtualDev		[in]	  virtual device data.
* @param peData_Type		[out]	  property data type
* @param pvReadPropValue	[out]	  pointer to save property value.
*
* @return MAX_BACNET_ERROR_CODE	on success.
*
*/
BACNET_ERROR_CODE Update_Special_Property(
BACNET_OBJECT_TYPE eObjectType,
uint32_t u32ObjectInstance,
BACNET_PROPERTY_ID eObjectProperty,
void *pvObject,
ulong32_t ul32PropOffsetAddr,
void *pVirtualDev,
BACNET_DATA_TYPE *peData_Type,
void **pvReadPropValue);

/**
*
* DESCRIPTION
* Function to copy list type or array (array of N) type properties.
*
* @param pvSrcPtr		 [in]	Pointer of source list
* @param eData_Type		 [in]	property data type
* @param pvReadPropValue [out]	pointer of destination list.
*
* @return MAX_BACNET_ERROR_CODE on success.
*
*/
BACNET_ERROR_CODE Copy_List_Type_properties(
void *pvSrcPtr, BACNET_DATA_TYPE eDataType, void **pvReadPropValue);

/**
*DESCRIPTION
*   Validates is array index is empty or not
*
*@param u32ArrayIndex        [in]  Array index
*@param bIsArrayIndexPresent [in]  Indicates if array index is present or not.
*@return BACNET_ERROR_CODE   [out] ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY
*
*/
BACNET_ERROR_CODE IsNoIndex(uint32_t u32ArrayIndex, bool bIsArrayIndexPresent);


#endif /* READ_H */
