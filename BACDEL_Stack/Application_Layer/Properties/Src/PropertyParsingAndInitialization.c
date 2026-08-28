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
*        DATE            NAME                DESCRIPTION
*        26/04/2012      Heramb Joshi        File Created. 
*        09/05/2012      Heramb Joshi        Event Enrollment.
*        02/08/2012      Heramb Joshi        Calendar object Parsing.   
*        13/08/2012      Heramb Joshi        Schedule object Parsing.
*        30/08/2012      Heramb Joshi        Schedule Internal And External.
*        16/10/2012      Heramb Joshi        TrendLog Object Parsing.
*       
*********************************************************************************/

#include "PropertyParsingAndInitialization.h"
#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"
#include "propertyValueRead.h"
#include "pduDateTime.h"
#include "pduDateTime.h"
#include <stdlib.h>
#include <string.h>

#ifdef BACDEL_OBJ_NC

static uint8_t ui8temp[10],ui8temp1[10];
/**
*                                                                         
*DESCRIPTION                                                              
*   This fun lode FromTime of PROP_RECIPIENT_LIST.
*    
*@param pui8time -  Pointer to Tokan containing FromTime
*
*/ 
void FromTime(uint8_t *pui8time, ListOfBACnetDestination_t *pstBacnetDestinations)
{	
	char i,j;
	i=(char)(atoi((const char *)strtok ((char *)pui8time,":")));
	j=(char)(atoi((const char *)strtok (NULL,":")));
	memcpy(&(pstBacnetDestinations->m_stFromTime.m_u8Hour),&i ,sizeof(uint8_t));
	memcpy(&(pstBacnetDestinations->m_stFromTime.m_u8Min),&j ,sizeof(uint8_t));
	pstBacnetDestinations->m_stFromTime.m_u8Sec = 0; 
	pstBacnetDestinations->m_stFromTime.m_u8Hundredths = 0;
}

/**
*                                                                         
*DESCRIPTION                                                              
*   This fun lode ToTime of PROP_RECIPIENT_LIST.
*    
*@param pui8time -  Pointer to Tokan containing ToTime.
*
*/ 
void ToTime(uint8_t *pui8time,ListOfBACnetDestination_t *pstBacnetDestinations)
{
	char i,j;
	i=(char)(atoi((const char *)strtok ((char *)pui8time,":")));
	j=(char)(atoi((const char *)strtok (NULL,":")));
	memcpy(&pstBacnetDestinations->m_stToTime.m_u8Hour, &i,sizeof(uint8_t));
	memcpy(&pstBacnetDestinations->m_stToTime.m_u8Min, &j,sizeof(uint8_t));
	pstBacnetDestinations->m_stToTime.m_u8Sec = 0; 
	pstBacnetDestinations->m_stToTime.m_u8Hundredths = 0;
}
	
/**
*                                                                         
*DESCRIPTION                                                              
*   This fun lode DaysOfWeek of PROP_RECIPIENT_LIST.
*    
*@param pui8day -  Pointer to Tokan containing DaysOfWeek.
*
*/ 
void day(uint8_t *pui8day,ListOfBACnetDestination_t *pstBacnetDestinations)
{
	if(!strcmp((const char *) pui8day, "Monday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] | 0x02);
	else if(!strcmp((const char *)pui8day, "Tuesday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] |0x04);
	else if(!strcmp((const char *) pui8day, "Wednesday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] | 0x08);
	else if(!strcmp((const char *) pui8day, "Thursday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] | 0x10);
	else if(!strcmp((const char *) pui8day, "Friday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] | 0x20);
	else if(!strcmp((const char *) pui8day, "Saturday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] | 0x40);
	else if(!strcmp((const char *) pui8day, "Sunday"))
		pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] = 
		(pstBacnetDestinations->m_stDaysOfWeek.m_u8TransBits[0] | 0x80);
}

/**
*                                                                         
*DESCRIPTION                                                              
*   This fun lode Transitions of PROP_RECIPIENT_LIST.
*    
*@param pui8trans -  Pointer to Tokan containing Transitions.
*
*/ 
char * event_transition(uint8_t *pui8trans,ListOfBACnetDestination_t *pstBacnetDestinations)
{
	pui8trans = (uint8_t *)strtok (NULL,"(, ");
	if(!strcmp((const char *) pui8trans,"TRUE"))
		pstBacnetDestinations->m_stTransitions.m_u8TransBits[0] = 
			pstBacnetDestinations->m_stTransitions.m_u8TransBits[0]  | 0x01;
	pui8trans = (uint8_t *)strtok (NULL, ", ");
	if(!strcmp((const char *) pui8trans,"TRUE"))
		pstBacnetDestinations->m_stTransitions.m_u8TransBits[0] = 
			pstBacnetDestinations->m_stTransitions.m_u8TransBits[0]  | 0x02;
	pui8trans = (uint8_t *)strtok (NULL, ") ");
	if(!strcmp((const char *) pui8trans,"TRUE"))
		pstBacnetDestinations->m_stTransitions.m_u8TransBits[0] = 
			pstBacnetDestinations->m_stTransitions.m_u8TransBits[0]  | 0x04;

	pstBacnetDestinations->m_stTransitions.m_u8ByteCnt = 1;
	pstBacnetDestinations->m_stTransitions.m_u8UnusedBits = 5;
	return ((char *)pui8trans);
}

/**
*                                                                         
*DESCRIPTION                                                              
*   This fun lode DaysOfWeek of PROP_RECIPIENT_LIST, call day.
*    
*@param pui8trans -  Pointer to Tokan containing DaysOfWeek.
*
*/ 
char * getDays(uint8_t* pui8day,ListOfBACnetDestination_t *pstBacnetDestinations)
{
	pui8day = (uint8_t *)strtok ((char *)pui8day,",");
	while (*pui8day != ')')
	{ 
		if((*((pui8day+strlen((const char *)pui8day))-1)) == ')')
		{ /*replace Last Element ")" with NULL Charara*/
			*((pui8day+strlen((const char *)pui8day))-1) = '\0';
			day(pui8day, pstBacnetDestinations);
			break;
		}
		day(pui8day,pstBacnetDestinations);
		pui8day = (uint8_t *)strtok (NULL, ", ");
	}
	pstBacnetDestinations->m_stDaysOfWeek.m_u8ByteCnt = 1;
	pstBacnetDestinations->m_stDaysOfWeek.m_u8UnusedBits = 1;
	return((char *)pui8day);
}

/**
*                                                                         
*DESCRIPTION                                                              
*   This fun loads from time & to time in recipient list.
*    
*@param pui8time -  Pointer to Tokan containing above times.
*@parampstBacnetDestinations - recipient list struct.
*
*/ 
uint8_t * getTime(uint8_t *pui8time, ListOfBACnetDestination_t *pstBacnetDestinations)
{			
	/*get From Time*/
	pui8time = (uint8_t *)strtok ((char *)pui8time,", ");
	Strcopy((void *)ui8temp, (void *)pui8time, sizeof(ui8temp));	

	/*get To Time*/
	pui8time = (uint8_t *)strtok (NULL, ", ");
	//strcpy((char *)ui8temp1, (const char *)pui8time);
	Strcopy((void *)ui8temp1, (void *)pui8time, sizeof(ui8temp1));

	FromTime(&ui8temp[0],pstBacnetDestinations);
	ToTime(&ui8temp1[0],pstBacnetDestinations);
			
	return(pui8time);
}

/**
*                                                                         
*DESCRIPTION                                                              
*   This fun loads object type & obj ID in recipient list.
*    
*@param pInData -  Pointer to Token containing above obj type & id.
*@parampstBacnetDestinations - recipient list struct.
*
*/ 
uint8_t *GetObjectid(uint8_t *pInData, ListOfBACnetDestination_t *pstBacnetDestinations)
{
	pstBacnetDestinations->m_stRecipient.BACnetRecipient_u.m_stObjId.m_eObjectType = 
		(BACNET_OBJECT_TYPE)(atoi((const char *) strtok ((char *)pInData,"(, ")));
    pInData = (uint8_t *)strtok (NULL,") ");
	pstBacnetDestinations->m_stRecipient.BACnetRecipient_u.m_stObjId.m_u32ObjId = atoi((const char *) pInData);
    return(pInData);
}

#endif	/* BACDEL_OBJ_NC */


#if (defined BACDEL_OBJ_CAL || defined BACDEL_OBJ_SDL)
void getDateList(ListOfBACnetCalendarEntry_t *pstListOfCalendar,uint8_t *pInData)
{
    BACnetDate_t stempDate = {0};
    bool     bWeekOrDay;
  
    while(*pInData != '\0')
    {
   
        pInData = (uint8_t *)strtok((char *)pInData,"( ");
        pInData = FillDatestructure(pInData,&stempDate,&bWeekOrDay);

        if(!bWeekOrDay)
        {
            if((*pInData) == '-')
            {
                pstListOfCalendar->m_eStatusCalendar = STATUS_DATE_RANGE;
                pstListOfCalendar->m_stCalendar.m_stDateRange.m_stStartDate.m_u16Year = stempDate.m_u16Year;
                pstListOfCalendar->m_stCalendar.m_stDateRange.m_stStartDate.m_u8Month = stempDate.m_u8Month;
                pstListOfCalendar->m_stCalendar.m_stDateRange.m_stStartDate.m_u8Day = stempDate.m_u8Day;

                pInData = FillDatestructure(pInData+2,&stempDate,&bWeekOrDay);

                pstListOfCalendar->m_stCalendar.m_stDateRange.m_stEndDate.m_u16Year = stempDate.m_u16Year;
                pstListOfCalendar->m_stCalendar.m_stDateRange.m_stEndDate.m_u8Month = stempDate.m_u8Month;
                pstListOfCalendar->m_stCalendar.m_stDateRange.m_stEndDate.m_u8Day = stempDate.m_u8Day;
                if(*(pInData+1) == ',')
                {
                pInData += 2;
                }
                else if(*(pInData+1) == ')')
                {
                    break;
                }
                else
                {
                pInData++;
                }
            }
            else if((*pInData) == ',' || (*pInData) == ')' || *pInData == '\0')
            {   
                pstListOfCalendar->m_eStatusCalendar = STATUS_DATE;
                pstListOfCalendar->m_stCalendar.m_stDate.m_u16Year = stempDate.m_u16Year;
                pstListOfCalendar->m_stCalendar.m_stDate.m_u8Month = stempDate.m_u8Month;
                pstListOfCalendar->m_stCalendar.m_stDate.m_u8Day = stempDate.m_u8Day;
                if(*(pInData) == ',')
                {
                pInData += 2;
                }
                else if(*pInData == '\0')
                    break;
                else
                {
                    pInData++;
                }
            }
        }
        else 
        {
            pstListOfCalendar->m_eStatusCalendar = STATUS_WEEK_N_DAY;
            pstListOfCalendar->m_stCalendar.m_stWeekNDay.m_eMonth = (BACNET_MONTH)stempDate.m_u8Day;
            pstListOfCalendar->m_stCalendar.m_stWeekNDay.m_eWeekOfMonth = (BACNET_WEEK_OF_MONTH)stempDate.m_u8Month;
			pstListOfCalendar->m_stCalendar.m_stWeekNDay.m_eWeekNDay = (BACNET_WEEKDAY)stempDate.m_u16Year;
            if(*(pInData) == ',')
            {
                pInData += 2;
            }
            else if(*pInData == '\0')
                break;
            else
            {
                pInData++;
            }
        }
      /*  else if(*pInData == '\0')
          break;*/

        if(*pInData != '\0')
        {
            pstListOfCalendar->m_pstNext = (ListOfBACnetCalendarEntry_t*)malloc(sizeof(ListOfBACnetCalendarEntry_t));
            pstListOfCalendar = pstListOfCalendar->m_pstNext;
            pstListOfCalendar->m_pstNext = NULL;
        }

    }/*while*/
}/*getDateList()*/

uint8_t * FillDatestructure(uint8_t *pInData, BACnetDate_t *stempDate, bool* bWeekOrDay)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: FillDatestructure : Entry \r\n");
	#endif

    if(*(pInData+1) == '/' || *(pInData+2) == '/')
    {
        if(*pInData != '\0')
        stempDate->m_u8Day = (uint8_t)(atoi((const char *)(strtok((char *)pInData,"/ "))));
        stempDate->m_u8Month = (uint8_t)(atoi((const char *)(strtok(NULL,"/ "))));
        pInData = (uint8_t *)strtok(NULL,")");
        stempDate->m_u16Year = (uint16_t)(atoi((const char *)pInData));
        *bWeekOrDay = 1;
    }
    else
    {
        if(*pInData != '\0')
        stempDate->m_u8Day = (uint8_t)(atoi((const char *)(strtok((char *)pInData,"- "))));
        stempDate->m_u8Month = (uint8_t)(atoi((const char *)(strtok(NULL,"- "))));
        pInData = (uint8_t *)strtok(NULL,")");
        stempDate->m_u16Year = (uint16_t)(atoi((const char *)pInData));
         *bWeekOrDay = 0;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: FillDatestructure : Exit \r\n");
	#endif
    return((pInData+strlen((const char *)pInData))+1);
}

#endif	/* (defined BACDEL_OBJ_CAL || defined BACDEL_OBJ_SDL) */


/*
**
*                                                                         
*DESCRIPTION                                                              
* parsing Function For Effective_Period of schedule object 
*     
*@param EffectivePeriod -  pointer to Effective_Period of schedule object
*@param pInData - input character string pointer
*
*/
#ifdef BACDEL_OBJ_SDL
void getEffectivePeriod(Pr_BACnetDateRange_t  *EffectivePeriod,uint8_t *pInData)
{
    BACnetDate_t stempDate = {0};
    bool     bWeekOrDay;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: getEffectivePeriod : Entry \r\n");
	#endif

    while(*pInData != '\0')
    {   
        pInData = (uint8_t *)strtok((char *)pInData,"( ");
        pInData = FillDatestructure(pInData,&stempDate,&bWeekOrDay);
        if((*pInData) == '-')
        {
            EffectivePeriod->m_stDateRange.m_stStartDate.m_u16Year = stempDate.m_u16Year;
            EffectivePeriod->m_stDateRange.m_stStartDate.m_u8Month = stempDate.m_u8Month;
            EffectivePeriod->m_stDateRange.m_stStartDate.m_u8Day = stempDate.m_u8Day;

            EffectivePeriod->m_stDateRange.m_stStartDate.m_u8Wday = 
                Day_Of_Week(EffectivePeriod->m_stDateRange.m_stStartDate.m_u16Year,
			                    EffectivePeriod->m_stDateRange.m_stStartDate.m_u8Month, 
                                EffectivePeriod->m_stDateRange.m_stStartDate.m_u8Day);

            pInData = FillDatestructure(pInData+2,&stempDate,&bWeekOrDay);

            EffectivePeriod->m_stDateRange.m_stEndDate.m_u16Year = stempDate.m_u16Year;
            EffectivePeriod->m_stDateRange.m_stEndDate.m_u8Month = stempDate.m_u8Month;
            EffectivePeriod->m_stDateRange.m_stEndDate.m_u8Day = stempDate.m_u8Day;

            EffectivePeriod->m_stDateRange.m_stEndDate.m_u8Wday = 
                Day_Of_Week(EffectivePeriod->m_stDateRange.m_stEndDate.m_u16Year,
			                    EffectivePeriod->m_stDateRange.m_stEndDate.m_u8Month, 
                                EffectivePeriod->m_stDateRange.m_stEndDate.m_u8Day);
        } 
        if((*pInData) == ')')
            break;
    }/*while*/

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: getEffectivePeriod : Exit \r\n");
	#endif
}

/*
**
*                                                                         
*DESCRIPTION                                                              
* parsing Function For List_Of_Object_Property_Reference of schedule object 
*     
*@param pstListOfObjectPropertyReference -  pointer to List_Of_Object_Property_Reference of schedule object
*@param pInData - input character string pointer
*
*/

void GetLISTObjPropReff(ListOfBACnetDevObjPropRef_t    *pstListOfBACnetDevObjPropReff, uint8_t *pInData)
{   
    BACnetDevObjPropRef_t   *pstDevObjPropRef = NULL;  
    pstDevObjPropRef = &(pstListOfBACnetDevObjPropReff->m_stDevObjPropRef);

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetLISTObjPropReff : Entry \r\n");
	#endif

    while(*pInData != '\0')
    {
        pInData = getDeviceobjectPropReff(pstDevObjPropRef,pInData);
        if(*pInData != '\0')
        {
            pstListOfBACnetDevObjPropReff->m_pstNext = OSAL_Malloc(
				sizeof(ListOfBACnetDevObjPropRef_t),__FILE__,__FUNCTION__, __LINE__);
            if(NULL == pstListOfBACnetDevObjPropReff->m_pstNext)
            {
                break;
            }
            pstListOfBACnetDevObjPropReff = pstListOfBACnetDevObjPropReff->m_pstNext;
            pstListOfBACnetDevObjPropReff->m_pstNext = NULL;
            pstDevObjPropRef = &(pstListOfBACnetDevObjPropReff->m_stDevObjPropRef);
            pInData = pInData+1;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetLISTObjPropReff : Exit \r\n");
	#endif
}


/*
**
*                                                                         
*DESCRIPTION                                                              
* parsing Function For Weekly_Schedule of schedule object 
*     
*@param pstWeeklySchedule -  pointer to Weekly_Schedule of schedule object
*@param pInData - input character string pointer
*
*/
/*{" ((8:00,1),(17:00,0)),((8:00,1)),((8:00,0),(17:00,0)),((8:00,1),(17:00,0),(19:00,1),(23:30,0)),\
                                        ((8:00,1),(17:00,0)),\
                                        ((10:00,1),(17:00,0))"};*/
void GetWeeklySchedParsing(Pr_ListOfBACnetDailySchedule_t  *pstWeeklySchedule, uint8_t *pInData)
{
    BACnetTimeValue_t  *pstTempWeekSched = NULL;
    int32_t i32count = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetWeeklySchedParsing : Entry \r\n");
	#endif

    /*The array elements 1-7 correspond to the days Monday - Sunday*/
    for(i32count = 0; ((i32count<BACNET_ARRAY_OF_SEVEN) && (NULL != pInData)); i32count++)
    {   /*Get tag type for data type it is assumed to be boolen*/ 
        pstWeeklySchedule->m_stTimeValue[i32count].m_stPropValue.m_TagType = 4;
        pInData =(uint8_t *) strtok((char *)pInData,"(, ");
        /*get Weekly_Schedule time*/

        if(NULL != pInData)
            pstWeeklySchedule->m_stTimeValue[i32count].bIsUsed = 1;

        pInData = GetTimeHMSH(&(pstWeeklySchedule->m_stTimeValue[i32count].m_stTime),pInData);
        pstWeeklySchedule->m_stTimeValue[i32count].m_stPropValue.uValue.m_Boolean = (bool)(atoi((const char *)strtok((char *)pInData,") ")));
        pInData = (pInData+strlen((const char *)pInData)+1);
        /*get next element*/
        while(*pInData == ',')
        {  
            if(NULL == pstWeeklySchedule->m_stTimeValue[i32count].m_pstNext)
            {
                pstWeeklySchedule->m_stTimeValue[i32count].m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),__FILE__,__FUNCTION__, __LINE__);
                pstTempWeekSched = pstWeeklySchedule->m_stTimeValue[i32count].m_pstNext;
                pstTempWeekSched->m_pstNext = NULL;
            }
            else
            {
                pstTempWeekSched->m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),__FILE__,__FUNCTION__, __LINE__);
                pstTempWeekSched = pstTempWeekSched->m_pstNext;
                pstTempWeekSched->m_pstNext = NULL;
            }
            pInData = (uint8_t *)strtok((char *)pInData,"(, ");
            pInData = GetTimeHMSH(&(pstTempWeekSched->m_stTime),pInData);
            pstTempWeekSched->m_stPropValue.uValue.m_Boolean = (bool)(atoi((const char *)strtok((char *)pInData,") ")));
            pstTempWeekSched->m_stPropValue.m_TagType = 4;
            pInData = (pInData+strlen((const char *)pInData)+1);
            if(*pInData == ')')
            {
                break;
            }
        }
        pInData = (uint8_t *)strtok((char *)pInData,"), ");
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetWeeklySchedParsing : Exit \r\n");
	#endif
}


/*(((23-9-1995)),(0:00,0),10),(((5-3-1996)-(7-3-1996)),(0:00,1),3),((8-3-1996),((10:00,0),(11:00,0)),7)*/
bool GetExcepSchedParsing(ListOfSpecialEvent_t *pstExceptionSchedule, uint8_t *pInData)
{
    BACnetTimeValue_t  *pstTempWeekSched = NULL;
    ListOfBACnetCalendarEntry_t    stListOfCalendar = {0};
    int32_t i32count = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetWeeklySchedParsing : Entry \r\n");
	#endif

    pInData = (uint8_t *)strtok((char *)pInData,", ");
    while(NULL != pInData)
    {       
        i32count = strlen((const char *)pInData);
        getDateList(&stListOfCalendar,pInData);
        pstExceptionSchedule->m_stListSpecialEvent.m_eStatusCalendar = stListOfCalendar.m_eStatusCalendar;
        memcpy(&(pstExceptionSchedule->m_stListSpecialEvent.m_stPeriod), &(stListOfCalendar.m_stCalendar),
            (sizeof(Calendar_u)));
        
        pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.bIsUsed = 1;

        pInData = pInData+i32count+1;
        pInData = (uint8_t *)strtok((char *)pInData,"(, ");
        pInData = GetTimeHMSH(&(pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.m_stTime),pInData);
        pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.
            m_stPropValue.uValue.m_Boolean = (bool)(atoi((const char *)strtok((char *)pInData,") ")));
        
        pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.m_stPropValue.m_TagType = 4;

        pInData = (pInData+strlen((const char *)pInData)+1);
        /*get next element*/
        while(*pInData == ',')
        {  
            if(NULL == pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.m_pstNext)
            {
                pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),__FILE__,__FUNCTION__, __LINE__);
                pstTempWeekSched = pstExceptionSchedule->m_stListSpecialEvent.m_stListOfTimeValues.m_pstNext;
                pstTempWeekSched->m_pstNext = NULL;
            }
            else
            {
                pstTempWeekSched->m_pstNext = OSAL_Malloc(sizeof(BACnetTimeValue_t),__FILE__,__FUNCTION__, __LINE__);
                pstTempWeekSched = pstTempWeekSched->m_pstNext;
                pstTempWeekSched->m_pstNext = NULL;
            }
            pInData = (uint8_t *)strtok((char *)pInData,"(, ");
            pInData = GetTimeHMSH(&(pstTempWeekSched->m_stTime),pInData);
            pstTempWeekSched->m_stPropValue.uValue.m_Boolean = (bool)(atoi((const char *)strtok((char *)pInData,") ")));
            pstTempWeekSched->m_stPropValue.m_TagType = 4;
            pInData = (pInData+strlen((const char *)pInData)+1);
            if(*pInData == ')')
            {
                break;
            }
        }

        pInData = (uint8_t *)strtok(NULL,",) ");
        pstExceptionSchedule->m_stListSpecialEvent.m_u32EventPriority = atoi((const char *)pInData);
        pInData = (uint8_t *)strtok(NULL,", ");
        if(NULL != pInData)
        {
            pstExceptionSchedule->m_pstNext =  OSAL_Malloc(sizeof(ListOfSpecialEvent_t),__FILE__,__FUNCTION__, __LINE__);
            if(NULL == pstExceptionSchedule->m_pstNext)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: GetExcepSchedParsing : Malloc failed \r\n");
				#endif
                return false;
            }
            pstExceptionSchedule = pstExceptionSchedule->m_pstNext;
            pstExceptionSchedule->m_pstNext = NULL;
        }
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetWeeklySchedParsing : Exit \r\n");
	#endif
    return true;
   // getEffectivePeriod
   

}

/*
**
*                                                                         
*DESCRIPTION                                                              
* parsing Function For Default_Value of schedule object 
* assigned value to union on the basis of tag type 
*    
*@param pstPresentDefaultValue -  pointer to present value of schedule object
*@param pInData - input character string pointer
*
*/

void GetDefaultValuePars(AnyValue_t *pstPresentDefaultValue,uint8_t *pInData)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:GetDefaultValuePars : Entry \r\n");
	#endif

    pstPresentDefaultValue->m_stValue.m_TagType = (int8_t)(atoi((const char * ) strtok ((char *)pInData,", ")));
    switch(pstPresentDefaultValue->m_stValue.m_TagType)
    {
        case BACNET_APPLICATION_TAG_BOOLEAN:
                pstPresentDefaultValue->m_stValue.uValue.m_Boolean = (bool)atoi((const char * ) strtok (NULL,", "));
            break;  

        case BACNET_APPLICATION_TAG_UNSIGNED_INT:
                pstPresentDefaultValue->m_stValue.uValue.m_Unsigned_Int =atoi((const char * ) strtok (NULL,", "));
           break;

        case BACNET_APPLICATION_TAG_SIGNED_INT:
                pstPresentDefaultValue->m_stValue.uValue.m_Signed_Int = atoi((const char * ) strtok (NULL,", "));
            break;

        case BACNET_APPLICATION_TAG_REAL:
                pstPresentDefaultValue->m_stValue.uValue.m_Real = (Float_t)atof((const char * ) strtok (NULL,", "));
            break;

        case BACNET_APPLICATION_TAG_DOUBLE:
                pstPresentDefaultValue->m_stValue.uValue.m_Double = atof((const char * ) strtok (NULL,", "));
            break;

        case BACNET_APPLICATION_TAG_OCTET_STRING:
			pstPresentDefaultValue->m_stValue.uValue.m_stOctet_String.m_ai8OctetStr[0] = (uint8_t)(atoi((const char * ) strtok (NULL,", ")));
			pstPresentDefaultValue->m_stValue.uValue.m_stOctet_String.m_u32OctetCount = 1;
            break;

        case BACNET_APPLICATION_TAG_CHARACTER_STRING:
            {
                uint32_t u32strsize;
                pInData = (uint8_t *)strtok (NULL,", ");
                u32strsize = Strnlen(pInData,MAX_CHARACTER_STRING_BYTES);           
                Strcopy(&(pstPresentDefaultValue->m_stValue.uValue.m_stCharacter_String.m_pu8CharStr), pInData, u32strsize);
                pstPresentDefaultValue->m_stValue.uValue.m_stCharacter_String.m_u32StrLen = u32strsize;
            }            
            break;

        case BACNET_APPLICATION_TAG_BIT_STRING:
			pstPresentDefaultValue->m_stValue.uValue.m_stBit_String.m_u8TransBits[0] = (uint8_t)(atoi((const char * ) strtok (NULL,", ")));
			pstPresentDefaultValue->m_stValue.uValue.m_stBit_String.m_u8UnusedBits = 4;
			pstPresentDefaultValue->m_stValue.uValue.m_stBit_String.m_u8ByteCnt = 1;
            break;

        case BACNET_APPLICATION_TAG_ENUMERATED:
                pstPresentDefaultValue->m_stValue.uValue.m_Enumerated = atoi((const char * ) strtok (NULL,", "));
            break;

        case BACNET_APPLICATION_TAG_DATE:
                pInData = (uint8_t *)strtok (NULL,", ");
                GetDateYMDW(&(pstPresentDefaultValue->m_stValue.uValue.m_stDate),pInData);           
            break;

        case BACNET_APPLICATION_TAG_TIME:
                pInData = (uint8_t *)strtok (NULL,", ");
                GetTimeHMSH(&(pstPresentDefaultValue->m_stValue.uValue.m_stTime),pInData);
            break;
        default :
		   #ifdef DEBUG_PRINTF
           Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:GetDefaultValuePars : Data type not supported \r\n");
		   #endif
            break;

           }

	 /* function exit */
	 #ifdef DEBUG_PRINTF
     Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:GetDefaultValuePars : Exit \r\n");
	 #endif
}
#endif

/**
*                                                                         
*DESCRIPTION                                                              
* XML Parsing function For Device object prop reff 
*    
*@param pstDevObjPropRef -  Pointer to Device object prop reff.
*@paramp pInData - input string .
*
*/
/*"((8,12),(4,9), 85)"*/

uint8_t *getDeviceobjectPropReff(BACnetDevObjPropRef_t   *pstDevObjPropRef,uint8_t *pInData)
{
	 /* function entry */
	 #ifdef DEBUG_PRINTF
     Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: getDeviceobjectPropReff : Entry \r\n");
	 #endif

     pInData = (uint8_t *)strtok((char *)pInData,"(, ");
     /*if(*pInData == '8')
     {*/
		 pstDevObjPropRef->m_eDeviceType = (BACNET_OBJECT_TYPE)(atoi((const char *)pInData));
         pstDevObjPropRef->m_u32DeviceInstace = atoi((const char *)strtok(NULL,",) "));
     /*}*/
	 pstDevObjPropRef->m_eObjectType = (BACNET_OBJECT_TYPE)(atoi((const char *)strtok(NULL,",( ")));
     pstDevObjPropRef->m_u32ObjId = atoi((const char *)strtok(NULL,",) "));
     pInData = (uint8_t *)strtok(NULL,",) ");
     pstDevObjPropRef->m_ePropertyIdentifier = atoi((const char *)pInData);
     pInData = pInData+3;
	 pstDevObjPropRef->m_u32ArrayIndex = BACNET_ARRAY_ALL;
     pstDevObjPropRef->m_bArrIndxPresent = ARRAY_INDEX_ABSENT;

	 /* function exit */
	 #ifdef DEBUG_PRINTF
     Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: getDeviceobjectPropReff : Exit \r\n");
	 #endif
	 return(pInData);
}

/**
*                                                                         
*DESCRIPTION                                                              
* XML Parsing function To get time 
*    
*@param pstTime -  Pointer to time.
*@paramp pInData - input string .
*
*/
uint8_t * GetTimeHMSH(BACnetTime_t *pstTime, uint8_t *pInData)
{
    int8_t      *pTokens = NULL;
    int32_t     i8TokenCount =0;
    uint8_t     *pRetAddres = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetTimeHMSH : Entry \r\n");
	#endif

    pRetAddres = (pInData+strlen((const char *)pInData)+1);

    pTokens = (int8_t *)strtok((char *)pInData, ":");
    do
    {
        if((NULL != pTokens) && (!strcmp((const char *)pTokens,":")))
        {
            switch(i8TokenCount)
            {
            case 0:
                pstTime->m_u8Hour = (uint8_t)(atoi((const char*)pTokens));
                break;
            case 1:
                pstTime->m_u8Min = (uint8_t)(atoi((const char*)pTokens));
                break;
            case 2:
                pstTime->m_u8Sec = (uint8_t)(atoi((const char*)pTokens));
                break;
            case 3:
                pstTime->m_u8Hundredths = (uint8_t)(atoi((const char*)pTokens));
                break;
            default:
                break;
            }
        }
        pTokens = (int8_t *)strtok(NULL, ":");
        i8TokenCount++;
    }while(NULL != pTokens);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetTimeHMSH : Exit \r\n");
	#endif
	return(pRetAddres);
}

/**
*                                                                         
*DESCRIPTION                                                              
* XML Parsing function To get Date 
*    
*@param pstTime -  dst Pointer to Date .
*@paramp pInData - input string .
*
*/
uint8_t * GetDateYMDW(BACnetDate_t *pstDate, uint8_t *pInData)
{
    int8_t      *pTokens = NULL;
    int32_t     i8TokenCount =0;
    uint8_t     *pRetAddres = NULL;

	/* function entry*/
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetDateYMDW : Entry \r\n");
	#endif

    pRetAddres = (pInData+strlen((const char *)pInData)+1);
    pTokens = (int8_t *)strtok((char *)pInData, "/");
    do
    {
        if((NULL != pTokens) && (strcmp((const char *)pTokens,"/")))
        {
            switch(i8TokenCount)
            {
                case 0:
                    pstDate->m_u16Year = (uint16_t)(atoi((const char*)pTokens));
                break;
                case 1:
                    pstDate->m_u8Month = (uint8_t)(atoi((const char*)pTokens));
                break;
                case 2:
                    pstDate->m_u8Day = (uint8_t)(atoi((const char*)pTokens));
                break;
                case 3:
                    pstDate->m_u8Wday = (uint8_t)(atoi((const char*)pTokens));
                break;
                default:
                break;
            }
        }
        pTokens = (int8_t *)strtok(NULL, "/ :");
        i8TokenCount++;
    }while(NULL != pTokens);

	 /* function exit */
	 #ifdef DEBUG_PRINTF
     Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: GetDateYMDW : Exit \r\n");
	 #endif
    return(pRetAddres);
}

/* 0,0,0,0 */
 uint8_t *SetStatusFlag(uint8_t *pu8TransBits,uint8_t *pInData)
 {
    uint8_t *pRetAddres = NULL;
    uint8_t u8count = 0;
    pRetAddres = (pInData+strlen((const char *)pInData)+1);
    pInData = (uint8_t *)strtok((char *)pInData,", ");
    while(NULL != pInData)
    {
        if('1' == *pInData)
        BIT_SET(*pu8TransBits,u8count);
        u8count++;
        pInData = (uint8_t *)strtok(NULL,", ");
    }
    return(pRetAddres);
 }

/** function to parse alarm & fault values properties of type list unsigned */
void * Parse_AF_Values(uint8_t *pInData)
{
    /* locals */
    ListOfUnsigned_t *pstUnsignVal = NULL;
    void *pvReturnVal = NULL;
    uint8_t *pu8Token = NULL;
    int8_t ai8Data[1000] = {0};

	/* check null pointers */
	if(NULL == pInData)
        return pvReturnVal;

    memcpy(ai8Data, pInData, Strnlen(pInData, sizeof(ai8Data)));

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Parse_AF_Values : entry \r\n");
	#endif

    /* allocate memory */
    pstUnsignVal = OSAL_Malloc(sizeof(ListOfUnsigned_t), __FILE__,__FUNCTION__, __LINE__);
    if(NULL == pstUnsignVal)
        return pvReturnVal;
    else
        pvReturnVal = pstUnsignVal;

    /* get the 1st value */
    pu8Token = (uint8_t *)strtok ((char *)ai8Data,",");
    if(NULL == pu8Token)
    {
    	;
    }
    else
    	 pstUnsignVal->m_u32Value = atoi((const char * )ai8Data);


    /* get all other value to add to list */
    while(NULL != pu8Token)
    {
        /* get the string to extract value */
    	pu8Token = (uint8_t *)strtok (NULL,", ");

        /* if no more values, then break the loop */
        if(NULL == pu8Token)
            break;

        /* allocate memory */
        pstUnsignVal->m_pstNext = OSAL_Malloc(sizeof(ListOfUnsigned_t),
            __FILE__,__FUNCTION__, __LINE__);
        if(NULL == pstUnsignVal->m_pstNext)
            break;

        /* move to the new node */
        pstUnsignVal = pstUnsignVal->m_pstNext;
        pstUnsignVal->m_pstNext = NULL;

        /* save the value */
        pstUnsignVal->m_u32Value = atoi((const char *)ai8Data);
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Parse_AF_Values : exit \r\n");
	#endif
    return pvReturnVal;
}

/** function to parse property values of type enumerated list  */
void * Parse_Enum_List_Values(uint8_t *pInData)
{
    /* locals */
    ListOfEnum_t *pstEnumVal = NULL;
    void *pvReturnVal = NULL;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Parse_Enum_List_Values : entry \r\n");
	#endif

    /* allocate memory */
    pstEnumVal = OSAL_Malloc(sizeof(ListOfEnum_t), __FILE__,__FUNCTION__, __LINE__);
    if(NULL == pstEnumVal || NULL == pInData)
	{
		if(NULL != pstEnumVal)
		{
			/* free allocated memory */
			OSAL_Free(pstEnumVal, __FILE__, __FUNCTION__, __LINE__);
		}
        return pvReturnVal;
	}
    else
        pvReturnVal = pstEnumVal;

    /* get the 1st value */
	pstEnumVal->m_eVal = atoi((const char *) strtok ((char *)pInData,", "));

    /* get all other value to add to list */
    while(NULL != pInData)
    {
        /* get the string to extract value */
        pInData = (uint8_t *)strtok (NULL,", ");

        /* if no more values, then break the loop */
        if(NULL == pInData)
            break;

        /* allocate memory */
		pstEnumVal->m_pstNext = OSAL_Malloc(sizeof(ListOfEnum_t),
            __FILE__,__FUNCTION__, __LINE__);
        if(NULL == pstEnumVal->m_pstNext)
            break;

        /* move to the new node */
        pstEnumVal = pstEnumVal->m_pstNext;
        pstEnumVal->m_pstNext = NULL;

        /* save the value */
		pstEnumVal->m_eVal = atoi((const char *)pInData);
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Parse_Enum_List_Values : exit \r\n");
	#endif
    return pvReturnVal;
}

