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
*   analogoutput.c
*
*  AUTHOR
*
*  DESCRIPTION
*   Handles Services supported for Binary Input Object
*
*  RELEASE HISTORY
*   DATE                  NAME				DESCRIPTION
*
*   27-07-2011           Prashant Badgujar	File Created.
*	03-08-2011			 Prashant Badgujar	Commenting Add_Ao_Object API
*
*************************************************************************/

/** 
@file analogoutput.c Handles Services supported for Binary Input Object
*/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifdef BACDEL_OBJ_AO

/* include header files */
#include "objAnalogOutput.h"
#include "bacDELDeviceConfig.h"

/* These three arrays are used by the ReadPropertyMultiple handler */
const int32_t ai32AnalogOutputReqProp[] = 
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,
    PROP_EVENT_STATE,
    PROP_OUT_OF_SERVICE,
    PROP_UNITS,
    PROP_PRIORITY_ARRAY,
    PROP_RELINQUISH_DEFAULT,
	#ifdef BACDEL_PR18       //KV
	PROP_CURRENT_COMMAND_PRIORITY,
	#endif
    ENDOFPROPLIST
};

const int32_t ai32AnalogOutputOptProp[] = 
{
	PROP_DESCRIPTION,
	PROP_DEVICE_TYPE,
	PROP_RELIABILITY,
	PROP_MIN_PRES_VALUE,
	PROP_MAX_PRES_VALUE,
	PROP_RESOLUTION,
	PROP_COV_INCREMENT,

	//KV
	PROP_TIME_DELAY,
	PROP_NOTIFICATION_CLASS,
	PROP_HIGH_LIMIT,
	PROP_LOW_LIMIT,
	PROP_DEADBAND,
	PROP_LIMIT_ENABLE,
	PROP_EVENT_ENABLE,
	PROP_ACKED_TRANSITIONS,
	PROP_NOTIFY_TYPE,
	PROP_TIME_DELAY_NORMAL,
	PROP_EVENT_DETECTION_ENABLE,
	PROP_EVENT_TIME_STAMPS,
	ENDOFPROPLIST
};

const int32_t ai32AnalogOutputProProp[] = 
{
    ENDOFPROPLIST
};

const char *BN_AO_OBJ_Name[] = { BN_AO_0_OBJ_NAME ,	BN_AO_1_OBJ_NAME};

/**
*DISCREPTION
*   This function is used to assign Property list for Analog Input object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Analog_Output_Property_Lists(int32_t **pi32Required, 
                                 int32_t **pi32Optional,
                                 int32_t **pi32Proprietary)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Analog_Output_Property_Lists: entry \r\n");
	#endif

    if (pi32Required) 
    {
        *pi32Required = (int32_t *)ai32AnalogOutputReqProp;
    }
    if (pi32Optional) 
    {
        *pi32Optional = (int32_t *)ai32AnalogOutputOptProp;
    }
    if (pi32Proprietary) 
    {
        *pi32Proprietary = (int32_t *)ai32AnalogOutputProProp;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Analog_Output_Property_Lists: exit \r\n");
	#endif

    return;
}

#endif /* BACDEL_OBJ_AO */
