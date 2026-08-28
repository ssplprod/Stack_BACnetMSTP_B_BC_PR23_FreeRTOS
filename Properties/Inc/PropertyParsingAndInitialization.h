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
*   3rd Floor, Pentagon P4,                                http://www.softdel.com  
*    Magarpatta City, Hadapsar
*    Pune - 411 028                                
*                                                                         
******************************************************************************/
/**
*********************************************************************************
*
*   FILE
*      PropertyParsingAndInitialization.c
*                                                                      
*   AUTHORS                                                                     
*       
*                                                                         
*   DESCRIPTION                                                            
*        Property initialization parsing functions. 
*                                                                         
*   RELEASE HISTORY                                                                 
*        DATE           NAME                DESCRIPTION
*       
*********************************************************************************/
#ifndef PROP_INIT_H
#define PROP_INIT_H

/**	Include Files */
#include "bacDELDef.h"
#include "propertydef.h"
#include "objDevice.h" 

#ifdef BACDEL_OBJ_NC
void FromTime(uint8_t *pui8time, ListOfBACnetDestination_t *pstBacnetDestinations);
void ToTime(uint8_t *pui8time,ListOfBACnetDestination_t *pstBacnetDestinations);
void day(uint8_t *pui8day,ListOfBACnetDestination_t *pstBacnetDestinations);
char * event_transition(uint8_t *pui8trans,ListOfBACnetDestination_t *pstBacnetDestinations);
char * getDays(uint8_t* pui8day,ListOfBACnetDestination_t *pstBacnetDestinations);
uint8_t * getTime(uint8_t *pui8time, ListOfBACnetDestination_t *pstBacnetDestinations); 
uint8_t *GetObjectid(uint8_t *pInData, ListOfBACnetDestination_t *pstBacnetDestinations);
#endif /* BACDEL_OBJ_NC */

#if (defined BACDEL_OBJ_CAL || defined BACDEL_OBJ_SDL)
void getDateList(ListOfBACnetCalendarEntry_t *pstListOfCalendar,uint8_t *pInData);
uint8_t * FillDatestructure(uint8_t *pInData, BACnetDate_t *stempDate, bool* bWeekOrDay);
#endif

#ifdef BACDEL_OBJ_SDL
void getEffectivePeriod(Pr_BACnetDateRange_t  *EffectivePeriod,uint8_t *pInData);
void GetLISTObjPropReff(ListOfBACnetDevObjPropRef_t    *pstListOfBACnetDevObjPropReff, uint8_t *pInData);
void GetWeeklySchedParsing(Pr_ListOfBACnetDailySchedule_t  *stWeeklySchedule, uint8_t *pInData);
bool GetExcepSchedParsing(ListOfSpecialEvent_t *pstExceptionSchedule, uint8_t *pInData);
void GetDefaultValuePars(AnyValue_t *pstPresentDefaultValue,uint8_t *pInData);
#endif /* BACDEL_OBJ_SDL */

uint8_t *getDeviceobjectPropReff(BACnetDevObjPropRef_t   *pstDevObjPropRef,uint8_t *pInData);
uint8_t * GetTimeHMSH(BACnetTime_t *pstTime, uint8_t *pInData);
uint8_t * GetDateYMDW(BACnetDate_t *pstDate, uint8_t *pInData);
uint8_t *SetStatusFlag(uint8_t *pu8TransBits,uint8_t *pInData);


/** function to parse alarm & fault values properties of type list unsigned */
void * Parse_AF_Values(uint8_t *pInData);

/** function to parse property values of type enumerated list  */
void * Parse_Enum_List_Values(uint8_t *pInData);

/** function to get Object prop reference */
uint8_t *GetObjPropReff(Pr_BACnetObjPropRef_t   *pstObjPropRef,uint8_t *pInData);

#endif /* PROP_INIT_H */
