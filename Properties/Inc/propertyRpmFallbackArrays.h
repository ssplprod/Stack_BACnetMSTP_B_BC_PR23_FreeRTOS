/*************************************************************************************
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
*   SoftDEL Systems Ltd.                    india@softdel.com
*   3rd Floor, Pentagon P4,                 http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*
*   File Name - propertyRpmFallbackArrays.h
*
*   DESCRIPTION
*   File consists of property support (required / optional / propprietary) 
*   arrya's required for RPM fallback mechanism.
*
*   RELEASE HISTORY
*   DATE                NAME                DESCRIPTION
*   29th July 2013      Pratham N. M.       File Creation.
*   29th July 2013      Pratham N. M.       Added function prototypes for 
*                                           accessing array's for all supported 
*                                           objects.
*
************************************************************************************/

#ifndef PROP_RPM_FALLBACK_H
#define PROP_RPM_FALLBACK_H

/** include required header files */
#include "bacDELStackConfig.h"


/** accumulator object */
#ifndef BACDEL_OBJ_ACC
/**
*DISCREPTION
*   This function is used to assign Property list for Accumulator object. 
*   This list is used in RPM service to determine all supported properties.
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Accumulator_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** analog input object */
#ifndef BACDEL_OBJ_AI
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
void Analog_Input_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** analog output object */
#ifndef BACDEL_OBJ_AO
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
void Analog_Output_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** analog value object */
#ifndef BACDEL_OBJ_AV
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
void Analog_Value_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** averaging object */
#ifndef BACDEL_OBJ_AVG
/**
*DISCREPTION
*   This function is used to assign Property list for Averaging object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required	[Out] Pointer to required property list
*@param pi32Optional	[Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Averaging_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** binary input object */
#ifndef BACDEL_OBJ_BI
/**
*DISCREPTION
*   This function is used to assign Property list for Binary_Input object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Binary_Input_Property_Lists(
    int32_t **pi32Required,
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** binary output object */
#ifndef BACDEL_OBJ_BO
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
void Binary_Output_Property_Lists(
    int32_t **pi32Required,
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** binary value object */
#ifndef BACDEL_OBJ_BV
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
void Binary_Value_Property_Lists(
    int32_t **pi32Required,
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** bitstring value object */
#ifndef BACDEL_OBJ_BSV
/**
*DISCREPTION
*   This function is used to assign Property list for BitStringValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void BitStringValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** calendar object */
#ifndef BACDEL_OBJ_CAL
/**
*DISCREPTION
*   This function is used to assign Property list for Calendar object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required	[Out] Pointer to required property list
*@param pi32Optional	[Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Calendar_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** character string value object */
#ifndef BACDEL_OBJ_CSV
/**
*DISCREPTION
*   This function is used to assign Property list for CharStringValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void CharStringValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** command object */
#ifndef BACDEL_OBJ_CMD
/**
*DISCREPTION
*   This function is used to assign Property list for Command object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Command_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** date pattern value object */
#ifndef BACDEL_OBJ_DPV
/**
*DISCREPTION
*   This function is used to assign Property list for DatePatternValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void DatePatternValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** datetime pattern value object */
#ifndef BACDEL_OBJ_DTPV
/**
*DISCREPTION
*   This function is used to assign Property list for DateTimePatternValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void DateTimePattVal_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** datetime value object */
#ifndef BACDEL_OBJ_DTV
/**
*DISCREPTION
*   This function is used to assign Property list for DateTimeValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void DateTimeValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** date value object */
#ifndef BACDEL_OBJ_DV
/**
*DISCREPTION
*   This function is used to assign Property list for DateValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void DateValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** event enrollment object */
#ifndef BACDEL_OBJ_EE
/**
*DISCREPTION
*   This function is used to assign Property list for Event Enrollment object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required	[Out] Pointer to required property list
*@param pi32Optional	[Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Event_Enrollment_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** evnet log object */
#ifndef BACDEL_OBJ_EL
/**
* DISCREPTION
* Function is used to assign Property list for Event_Log object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Event_Log_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif


/** file object */
#ifndef BACDEL_OBJ_FILE
/**
*DISCREPTION
*   This function is used to assign Property list for File object. 
*   This list is used in RPM service to determine all supported properties.
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
**/
void File_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** global group object */
#ifndef BACDEL_OBJ_GGP
/**
*DISCREPTION
*   This function is used to assign Property list for Global group object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void GlobalGroup_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** group object */
#ifndef BACDEL_OBJ_GP
/**
*DISCREPTION
*   This function is used to assign Property list for Group object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Group_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** integer object */
#ifndef BACDEL_OBJ_INT
/**
*DISCREPTION
*   This function is used to assign Property list for Integer Value object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Integer_Value_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** large analog value object */
#ifndef BACDEL_OBJ_LAV
/**
*DISCREPTION
*   This function is used to assign Property list for LargeAnalogVal object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void LargeAnalogVal_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** loop object */
#ifndef BACDEL_OBJ_LOOP
/**
*DISCREPTION
*   This function is used to assign Property list for Loop object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required	[Out] Pointer to required property list
*@param pi32Optional	[Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Loop_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** multistate input object */
#ifndef BACDEL_OBJ_MSI
/**
*DISCREPTION
*   This function is used to assign Property list for Multistate_Input object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Multistate_Input_Property_Lists(
    int32_t **pRequired,
    int32_t **pOptional,
    int32_t **pProprietary);
#endif


/** multistate output object */
#ifndef BACDEL_OBJ_MSO
/**
*DISCREPTION
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
    int32_t **pProprietary);
#endif


/** multistate value object */
#ifndef BACDEL_OBJ_MSV
/**
*DISCREPTION
*   This function is used to assign Property list for Multistate_Value object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Multistate_Value_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif


/** notification class object */
#ifndef BACDEL_OBJ_NC
/**
*DISCREPTION
*   This function is used to assign Property list for Notification Class object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required	[Out] Pointer to required property list
*@param pi32Optional	[Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Notification_Class_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** octet string value object */
#ifndef BACDEL_OBJ_OSV
/**
*DISCREPTION
*   This function is used to assign Property list for BitStringValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void OctectStringValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** positive integer object */
#ifndef BACDEL_OBJ_PINT
/**
*DISCREPTION
*   This function is used to assign Property list for Positive Integer Value object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void PositiveIntVal_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** pulse converter object */
#ifndef BACDEL_OBJ_PC
/**
*DISCREPTION
*   This function is used to assign Property list for pulse converter object. 
*   This list is used in RPM service to determine all supported properties.
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void PulseConverter_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** schedule object */
#ifndef BACDEL_OBJ_SDL
/**
*DISCREPTION
*   This function is used to assign Property list for Schedule object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required	[Out] Pointer to required property list
*@param pi32Optional	[Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void Schedule_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** time pattern value object */
#ifndef BACDEL_OBJ_TPV
/**
*DISCREPTION
*   This function is used to assign Property list for TimePatternValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void TimePatternValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** time value object */
#ifndef BACDEL_OBJ_TV
/**
*DISCREPTION
*   This function is used to assign Property list for TimeValue object 
*   This list is used in RPM service to determine all supported properties
*
*@param pi32Required [Out] Pointer to required property list
*@param pi32Optional [Out] Pointer to Optional property list
*@param pi32Proprietary [Out] Pointer to Proprietary property list
*
*/
void TimeValue_Property_Lists(
    int32_t **pi32Required, 
    int32_t **pi32Optional,
    int32_t **pi32Proprietary);
#endif


/** trend log object */
#ifndef BACDEL_OBJ_TL
/**
*DISCREPTION
*   This function is used to assign Property list for Trend_Log object 
*   This list is used in RPM service to determine all supported properties
*
*@param pRequired [Out] Pointer to required property list
*@param pOptional [Out] Pointer to Optional property list
*@param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Trend_Log_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif


/** trend log multiple object */
#ifndef BACDEL_OBJ_TLM
/**
* DISCREPTION
* Function is used to assign Property list for Trend_Log_Multiple object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Trend_Log_Multiple_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_TLM */


/** program object */
#ifndef BACDEL_OBJ_PROG
/**
* DISCREPTION
* Function is used to assign Property list for Program object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void Program_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_PROG */


/** Load Control object */
#ifndef BACDEL_OBJ_LC
/**
* DISCREPTION
* Function is used to assign Property list for LoadControl object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void LoadControl_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_LC */


/** Structured View object */
#ifndef BACDEL_OBJ_SV
/**
* DISCREPTION
* Function is used to assign Property list for Structured View object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void StructuredView_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_SV */


/** Life Safety Point */
#ifndef BACDEL_OBJ_LSP
/**
* DISCREPTION
* Function is used to assign Property list for Life Safety Point object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void LifeSafetyPoint_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_LSP */


/** Life Safety Zone object */
#ifndef BACDEL_OBJ_LSZ
/**
* DISCREPTION
* Function is used to assign Property list for Life Safety Zone object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void LifeSafetyZone_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_LSZ */


/** Access Door object */
#ifndef BACDEL_OBJ_AD
/**
* DISCREPTION
* Function is used to assign Property list for Access Door object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void AccessDoor_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_AD */


/** Access Point object */
#ifndef BACDEL_OBJ_AP
/**
* DISCREPTION
* Function is used to assign Property list for Access Point object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void AccessPoint_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_AP */


/** Access Zone object */
#ifndef BACDEL_OBJ_AZ
/**
* DISCREPTION
* Function is used to assign Property list for Access Zone object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void AccessZone_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_AZ */


/** Access User object */
#ifndef BACDEL_OBJ_AU
/**
* DISCREPTION
* Function is used to assign Property list for Access User object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void AccessUser_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_AU */


/** Access Rights object */
#ifndef BACDEL_OBJ_AR
/**
* DISCREPTION
* Function is used to assign Property list for Access Rights object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void AccessRights_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_AR */


/** Access Credential object */
#ifndef BACDEL_OBJ_AC
/**
* DISCREPTION
* Function is used to assign Property list for Access Credential object.
* This list is used in RPM service to determine all supported properties.
*
* @param pRequired [Out] Pointer to required property list
* @param pOptional [Out] Pointer to Optional property list
* @param pProprietary [Out] Pointer to Proprietary property list
*
*/
void AccessCredential_Property_Lists(
    int **pRequired,
    int **pOptional,
    int **pProprietary);
#endif /* BACDEL_OBJ_AC */


#endif /* PROP_RPM_FALLBACK_H */
/******** end of file ***************/