/******************************************************************************
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
*   SoftDEL Systems Ltd.                                 india@softdel.com 
*   3rd Floor, Pentagon P4,                             http://www.softdel.com  
*    Magarpatta City, Hadapsar
*    Pune - 411 028       
*
*    File Name - propertydef.h
*
*   RELEASE HISTORY                                                             
*        DATE               NAME                 DESCRIPTION
*       25/05/2011          Harshal Mangle       File Created  
*       20/06/2011          Prashant Badgujar    Added Property Datatypes.
*       06-07-2011          M.Venu               Added MAX_MULTISTATES and
*                                                MAX_PRIORITY_VAL
*       12-07-2011          M.Venu               Added all Binary and 
*                                                Mulistate object data types
*       13-07-2011          M.Venu               Added all DateTime, 
*                                                Time Stamp structures and 
*                                                updated Priority array
*       15-07-2011          M.Venu               Change m_u8StrLen to m_u32StrLen
*       18-07-2011          M.Venu               Change members of Priority 
*                                                array and Event stamp data types
*       22-07-2011          M.Venu               Added Pr_ListOfUnsigned_t and 
*                                                Pr_ListOfCharStr_t data types 
*       27-07-2011          M.Venu               1. Added Pr_BACnetBitStr_t and 
*                                                bacnetArrayIndex_t structus
*                                                2. Changed Char pointers to 
*                                                   Array of Chars
*       26-07-2011          Ashish Verma         Changed name of bacnetAddBind_t
*                                                to Pr_ListOfBACnetAddrBinding_t & modified 
*                                                the structure.
*       28/07/2011          Prashant Badgujar    Adding Pr_BACnetEnggUnits_t &
*                                                bacnetLimitEnable_t
*       28/07/2011          Ashish verma         Modified Pr_ListOfBACnetAddrBinding_t.
*       01/08/2011          Prashant Badgujar    Modified BACnetObjectTypesSupported_t.
*       03/08/2011          M.Venu               1. Added MAX_PRIORITY_STRING,
*                                                SPACE_ELEMENT macro definations
*                                                2.Changed BacnetStatusFlags_t 
*                                                data types to Pr_BACnetBitStr_t
*                                                3.  Added Pr_ListOfCharStr_t, 
*                                                Pr_ListOfUnsigned_t,Pr_ListOfCharStr_t 
*                                                and ListOfCharStr_t
*       04/08/2011          M.Venu               Added eventTimeStamp_u for 
*                                                Event Time stamp structure
*       05/08/2011          M.Venu               Added  STAMP_AS_TIME,STAMP_AS_SEQUENCE
*                                                and STAMP_AS_DATETIME macros for
*                                                supporting event time stamp property
*       08/08/2011          Ashish Verma         Added comments to structure.
*		13/09/2011			Prashant Badgujar	 Changing prototype for array of object id.
*       21/09/2011          M.Venu               Added bacnetActiveElapse_t stucture
*       08/03/2013          Pratham N. Murkute   Updated structures, sorting as per 
*                                                structure type & use.
*
******************************************************************************/

#ifndef PROPERTY_DEF_H
#define PROPERTY_DEF_H

/* header file includes */
#include "osalFreeRTOS.h"
#include "bacDELPropertyDef.h"

/* 
* Note :
* Structures within this file are moved into "bacDELPropertyDef.h" header file
*
*/

#endif /* PROPERTY_DEF_H */
