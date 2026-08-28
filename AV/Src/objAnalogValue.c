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
*   analogvalue.c 
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
*	03-08-2011			 Prashant Badgujar	Commenting Add_Av_Object API
*
*************************************************************************/

/** 
@file analogvalue.c Handles Services supported for Binary Input Object
*/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifdef BACDEL_OBJ_AV

/* include header files */
#include "objAnalogValue.h"
#include "bacDELDeviceConfig.h"

/* These three arrays are used by the ReadPropertyMultiple handler */
const int32_t ai32AnalogValueReqProp[] = 
{
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,
    PROP_EVENT_STATE,
    PROP_OUT_OF_SERVICE,
    PROP_UNITS,
    ENDOFPROPLIST
};

const int32_t ai32AnalogValueReqOptional[] = 
{
	PROP_DESCRIPTION,
	PROP_RELIABILITY,
	PROP_COV_INCREMENT,
	PROP_MIN_PRES_VALUE,
	PROP_MAX_PRES_VALUE,
	PROP_RESOLUTION,

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
	PROP_FAULT_HIGH_LIMIT,
	PROP_FAULT_LOW_LIMIT,
	ENDOFPROPLIST
};

const int32_t ai32AnalogValueProProp[] = 
{
    ENDOFPROPLIST
};

const char *BN_AV_OBJ_Name[] = { BN_AV_0_OBJ_NAME,	BN_AV_1_OBJ_NAME} ;
													
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
void Analog_Value_Property_Lists(int32_t **pi32Required, 
                                 int32_t **pi32Optional,
                                 int32_t **pi32Proprietary)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Analog_Value_Property_Lists: entry \r\n");
	#endif

    if (pi32Required) 
    {
        *pi32Required = (int32_t *)ai32AnalogValueReqProp;
    }
    if (pi32Optional) 
    {
        *pi32Optional = (int32_t *)ai32AnalogValueReqOptional;
    }
    if (pi32Proprietary) 
    {
        *pi32Proprietary = (int32_t *)ai32AnalogValueProProp;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Analog_Value_Property_Lists: exit \r\n");
	#endif

    return;
}

#endif /* BACDEL_OBJ_AV */
