/******************************************************************************
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
******************************************************************************/
/****************************************************************************** 
*                                                                         
* FILE
*   Binary_Value.c 
*
*  AUTHOR
*
*  DESCRIPTION
*   Handles Services supported for Binary Value Object
*
*  RELEASE HISTORY
*   DATE        NAME        DESCRIPTION
*
*   30-06-2011  M.Venu      Initial Draft
*   04-07-2011  M.Venu      Added Tag validation for write 
*                           functionality
*   06-07-2011  M.Venu      1. Changed return type for Read function
*   12-07-2011  M.Venu      1. changed return type as void for 
*                           Read/Write functions
*   13-07-2011  M.Venu      Added #if 0 as unit testing pending 
*                           after changes in structure members
*   14-07-2011  M.Venu      Added Init_BinaryValue_Prop
*   15-07-2011  M.Venu      Added malloc error checks 
*   18-07-2011  M.Venu      Polarity property data type modified
*   19-07-2011  Harshal M   Implemented Function to find REquired, 
*                           Optional and Proprietary properties 
*                           for this object
*   27-07-2011  M.Venu      Commented Read And Write functionality
*   28-07-2011  M.Venu      Commented Init Property As common add 
*                           property logic has been modified in 
*                           objproperty.c file
******************************************************************************/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifdef BACDEL_OBJ_BV

/* include header files */
#include "objBinaryValue.h"
#include "bacDELDeviceConfig.h"

/* These three arrays are used by the ReadPropertyMultiple handler */
const int Binary_Value_Properties_Required[] = {
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,
    PROP_EVENT_STATE,
    PROP_OUT_OF_SERVICE,
    ENDOFPROPLIST
};

const int Binary_Value_Properties_Optional[] = {
	PROP_DESCRIPTION,
	PROP_RELIABILITY,

	PROP_TIME_DELAY,
	PROP_NOTIFICATION_CLASS,
	PROP_ALARM_VALUE,
	PROP_EVENT_ENABLE,
	PROP_ACKED_TRANSITIONS,
	PROP_NOTIFY_TYPE,
	PROP_TIME_DELAY_NORMAL,
	PROP_EVENT_DETECTION_ENABLE,
	PROP_EVENT_TIME_STAMPS,
  ENDOFPROPLIST
};

const int Binary_Value_Properties_Proprietary[] = {ENDOFPROPLIST};
	
const char *BN_BV_OBJ_Name[] = {BN_BV_0_OBJ_NAME,	BN_BV_1_OBJ_NAME };

/**
*DISCREPTION
*   This function is used to assign Property list for Binary_Value object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Binary_Value_Property_Lists(int32_t **pi32Required,
                                 int32_t **pi32Optional,
                                 int32_t **pi32Proprietary)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Binary_Value_Property_Lists: entry \r\n");
	#endif

    if (pi32Required)
        *pi32Required = (int32_t *)Binary_Value_Properties_Required;
    if (pi32Optional)
        *pi32Optional = (int32_t *)Binary_Value_Properties_Optional;
    if (pi32Proprietary)
        *pi32Proprietary = (int32_t *)Binary_Value_Properties_Proprietary;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Binary_Value_Property_Lists: exit \r\n");
	#endif

    return;
}

#endif /* BACDEL_OBJ_BV */
/* End of Binary_Value.c File */
