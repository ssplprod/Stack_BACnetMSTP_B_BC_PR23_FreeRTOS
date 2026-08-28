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
*   Multistate_Output.c 
*
*  AUTHOR
*
*  DESCRIPTION
*   Handles Services supported for Multistate Output Object
*
*  RELEASE HISTORY
*   DATE                  NAME          DESCRIPTION
*
*   06-07-2011           M.Venu         Initial Draft
*   12-07-2011           M.Venu         1. changed return type as
*   13-07-2011           M.Venu         Changes being made after unit test of 
*                                       read functionality
*   15-07-2011           M.Venu         Followed coding standards & added malloc
*                                       error checks
*   18-07-2011           M.Venu         next pointer modified with pstNext
*   22-07-2011           M.Venu         Added Init_MSOutput_Prop function
*   28-07-2011           M.Venu         Commented Init Property As common add 
*                                       property logic has been modified in 
*                                       objproperty.c file
*   01-08-2011          Harshal M       Implemented Function to find REquired, 
*                                       Optional and Proprietary properties 
*                                       for this object
*
*************************************************************************/

/** 
@file Multistate_Output.c Handles Services supported for Multistate Output
*/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifdef BACDEL_OBJ_MSO

/* include header files */
#include "objMultiStateOutput.h"
//#include "Debug.h"

/* These three arrays are used by the ReadPropertyMultiple handler */
const int Multistate_Output_Properties_Required[] = {
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,
    PROP_EVENT_STATE,
    PROP_OUT_OF_SERVICE,
    PROP_NUMBER_OF_STATES,
    PROP_PRIORITY_ARRAY,
    PROP_RELINQUISH_DEFAULT,

	//KV
	PROP_RELIABILITY,
	PROP_STATE_TEXT,
	PROP_TIME_DELAY,
	PROP_NOTIFICATION_CLASS,
	PROP_FEEDBACK_VALUE,
	PROP_EVENT_ENABLE,
	PROP_ACKED_TRANSITIONS,
	PROP_NOTIFY_TYPE,
	PROP_EVENT_DETECTION_ENABLE,
	PROP_TIME_DELAY_NORMAL,
	PROP_EVENT_TIME_STAMPS,
	PROP_CURRENT_COMMAND_PRIORITY,

    ENDOFPROPLIST
};

const int Multistate_Output_Properties_Optional[] = {
  ENDOFPROPLIST
};

const int Multistate_Output_Properties_Proprietary[] = {
    ENDOFPROPLIST
};

/**
*DESCRIPTION
*   This function is used to assign Property list for Multistate_Output object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Multistate_Output_Property_Lists(
		int32_t **pRequired,
		int32_t **pOptional,
		int32_t **pProprietary)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Multistate_Output_Property_Lists: entry \r\n");
	#endif

    if (pRequired)
        *pRequired = (int32_t *)Multistate_Output_Properties_Required;
    if (pOptional)
        *pOptional = (int32_t *)Multistate_Output_Properties_Optional;
    if (pProprietary)
        *pProprietary = (int32_t *)Multistate_Output_Properties_Proprietary;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Multistate_Output_Property_Lists: exit \r\n");
	#endif

    return;
}

#endif /* BACDEL_OBJ_MSO */

/* End of Multistate_Output.c File */
