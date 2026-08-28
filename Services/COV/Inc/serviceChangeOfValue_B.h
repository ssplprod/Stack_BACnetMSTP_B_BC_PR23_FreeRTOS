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
*   SoftDEL Systems Ltd.                     		    india@softdel.com         
*   3rd Floor, Pentagon P4,								http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
******************************************************************************/
/****************************************************************************** 
*                                                                         
*   FILE
*	 serviceChangeOfValue_B.h
*                                                                      
*   AUTHORS                                                                             
*	 Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	 Defines the COV_B functions for the BACnet application. 
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE        NAME		    DESCRIPTION
*   29/08/2011  Ashish Verma    File Created.
*   23/09/2011  Ashish Verma    Removed Initiate_Timer_Param() & modified 
*                               Update_COVLifeTime() to
*                               DvCOV_B_Cancellation_Thread()
*	11/11/2011 	Ashish Verma	Modified msgquedata_t structure.
*								Modified fucntion Create_Message_Queue_Struct()
******************************************************************************/
/* To Include/Exclude Change Of Value Service */
#ifdef BACDEL_SER_DS_COV_B

#include "serviceCommomChangeOfValue_B.h"

/* 
 * Handler for COV Subscribe Service request. Decodes the COV Subscribe service
 * request and either generates an error/simple acknowlwdge.
 */
BACNET_RETURN_TYPE COV_B_Subscribe_Handler(processInfo_t *pstProcQInfo,
                               uint8_t *pu8ServiceRequest,
                               int32_t i32ServiceLen);

/*
 *  Parse the request received & based on the status return Error packet
 *  or a simple acknowledge.
 */
BOOL COV_Subscribe_Request_Parser(virtualDevData_t *pstVirtualDev, processInfo_t * pstProcQInfo);

/*
 *  Registers the COV request to ActiveCOVSubscription list.
 */
bool Register_COV_Request(void *pvObjectIndex,
                          virtualDevData_t *pstVirtualDev,
                          ListOfBACnetCovSubs_t *pstCOVSubscribe,
						  BACNET_COV_SUBS *peCovSubType);

bool ValidateCovObject(ListOfBACnetCovSubs_t *pstCOVSubscribe,
                  virtualDevData_t *pVirtualDev,
                  processInfo_t *pstProcQInfo);

/* function to check if object supports cov increment property */
bool IsCOVIncrementSupported(BACNET_OBJECT_TYPE eObjectType);

#endif /* BACDEL_SER_DS_COV_B */
/*****************************************************************************/
