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
*   Binary_Output.c 
*
*  AUTHOR
*
*  DESCRIPTION
*   Handles Services supported for Binary Output Object
*
*  RELEASE HISTORY
*   DATE        NAME        DESCRIPTION
*
*   21-06-2011  M.Venu      Initial Draft
*   30-06-2011  M.Venu      Added Write Functionality
*   04-07-2011  M.Venu      Added Tag validation for write 
*                           functionality
*   06-07-2011  M.Venu      1.Changed return type for Read function
*                           2.Added ERROR_CODE_UNKNOWN_PROPERTY
*   12-07-2011  M.Venu      1.changed return type as void for 
*                           Read/Write functions
*   13-07-2011  M.Venu      Changes being made after unit test of 
*                           read functionality
*   14-07-2011  M.Venu      Added Init_BinaryOutput_Prop
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

/** 
@file Binary_Output.c Handles Services supported for Binary Output Object
*/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifdef BACDEL_OBJ_BO

/* include header files */
#include "objBinaryOutput.h"
#include "bacDELDeviceConfig.h"

					
/* These three arrays are used by the ReadPropertyMultiple handler */
 const int Binary_Output_Properties_Required[] = {
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,
    PROP_EVENT_STATE,
    PROP_OUT_OF_SERVICE,
    PROP_POLARITY,
    PROP_PRIORITY_ARRAY,
    PROP_RELINQUISH_DEFAULT,
	#ifdef BACDEL_PR18
	PROP_CURRENT_COMMAND_PRIORITY,
	#endif
    ENDOFPROPLIST
};

const int Binary_Output_Properties_Optional[] = {
	PROP_DESCRIPTION,
	PROP_DEVICE_TYPE,
	PROP_RELIABILITY,
	PROP_FEEDBACK_VALUE,

	PROP_TIME_DELAY,
	PROP_NOTIFICATION_CLASS,
	PROP_EVENT_ENABLE,
	PROP_ACKED_TRANSITIONS,
	PROP_NOTIFY_TYPE,
	PROP_TIME_DELAY_NORMAL,
	PROP_EVENT_DETECTION_ENABLE,
	PROP_EVENT_TIME_STAMPS,
    ENDOFPROPLIST
};

const int Binary_Output_Properties_Proprietary[] = { ENDOFPROPLIST };

const char *BN_BO_OBJ_Name[]= { BN_BO_0_OBJ_NAME ,	BN_BO_1_OBJ_NAME};

/**
*DISCREPTION
*   This function is used to assign Property list for Binary_Output object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Binary_Output_Property_Lists(int32_t **pi32Required,
                                  int32_t **pi32Optional,
                                  int32_t **pi32Proprietary)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Binary_Output_Property_Lists: entry \r\n");
	#endif

    if (pi32Required)
        *pi32Required = (int32_t *)Binary_Output_Properties_Required;
    if (pi32Optional)
        *pi32Optional = (int32_t *)Binary_Output_Properties_Optional;
    if (pi32Proprietary)
        *pi32Proprietary = (int32_t *)Binary_Output_Properties_Proprietary;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Binary_Output_Property_Lists: exit \r\n");
	#endif

    return;
}

#endif /* BACDEL_OBJ_BO */
/* End of Binary_Output.c file */
