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
*   objBinaryInput.h 
*
*  AUTHOR
*
*  DESCRIPTION
*   Header file with all supported definations/declarations form Binary_Input.c
*
*  RELEASE HISTORY
*   DATE         NAME           DESCRIPTION
*
*   21-06-2011   M.Venu         Initial Draft
*   05-07-2011   M.Venu         1. Added space between Properties. 
*                               2. Return type changed for Write object
*   06-07-2011   M.Venu         Retrun Type changed for function
*                               Read_BinaryInput_Property
*   13-07-2011   M.Venu         Binary Input Structure members updated 
*                               As per Coding standards.
*   14-07-2011   M.Venu         Added Init_BinaryInput_Prop
*   18-07-2011   M.Venu         Polarity property data type modified
*   03-08-2011   M.Venu         Changed BacnetStatusFlags_t data types
*                               to Pr_BACnetBitStr_t data type
*   04-08-2011   M.Venu         Removed Relinquish default property
*   09-08-2011   Ashish Verma   Modified BinaryInput_t structure.
*   20-09-2011   M.Venu         m_stChangeOfStateCount property data type 
*                               changed to unsigned 16 byte
*   22-09-2011   M.Venu         m_stElaspsedActiveTime property data type 
*                               changed to bacnetActiveElapse_t
*************************************************************************/

/** 
@file objBinaryInput.h Contain all definations/declarations of Binary_Input.c file
*/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifndef BI_H
#define BI_H
#ifdef BACDEL_OBJ_BI

#include "bacDELDef.h"
#include "propertydef.h"

/**
*
*Name - BinaryInput
* 
*DESCRIPTION 
*   BACnet Binary Input structure with All its properties
*   
*/
typedef struct BinaryInput
{
    /** Object Identifier */
    Pr_BACnetObjId_t			m_stObjectID;
    
    /** Object Name*/
    Pr_BACnetCharStr_t			m_stObjName;
    
    /** Object Type */
    Pr_BACnetObjType_t			m_stObjectType;
    
    /** Object Present value */
    Pr_BinaryEnumPV_t			m_stPresentValue;
    
     /** Dummy present vlaue when out of service is true */
    Pr_BinaryEnumPV_t			m_stBackupPresentValue;
    
    /** Represents four Boolean flags {IN_ALARM, FAULT, OVERRIDDEN, 
        OUT_OF_SERVICE} that indicate the general "health" of a binary input */
    Pr_BACnetBitStr_t			m_stStatusFlag;
    
    /** In order to provide a way to determine if this object has an
        active event state associated with it.Events can be any one of 
        NORMAL, NO_FAULT_DETECTED ,FAULT. states */
    Pr_BACnetEventState_t		m_stEventState;    
    
    /** Object out of service */
    Pr_BACnetBool_t				m_stOutOfService;

    /* This flag bit is added to inform application about callback registration */
    bool						bAppCallBack;
    
    /** Object polarity */
    Pr_BACnetPolarity_t			m_stPolarity;   
#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object. 
	The Object_Name, Object_Type, Object_Identifier, and Property_List properties 
	are not included in the list */
    Pr_BACnetPropertyList_t     m_stPropertyList;
#endif

 
#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B)

	/** Provides an indication of whether the Present_Value or the operation of
	the physical input in question is "reliable" as far as the BACnet 
    Device or operator can determine and, if not, why? **/
    Pr_BACnetReliability_t		m_stReliability;

	Pr_BACnetUnsigned32_t		m_stTimeDelayNormal;
	Pr_BACnetBool_t				m_stEventDetectionEnable;

#endif

#ifdef BACDEL_SER_AE_EN_B   

    /** Specify the value that the Present Value must have before
        an event is generated.*/
    Pr_BinaryEnumPV_t			m_stAlarmValue;

	/** Specify the minimum period of time in seconds during which the 
        Present_Value must remain equal to the Alarm_Value property before
        a TO-OFFNORMAL event is generated,or remain not equal to the
        Alarm_Value property before a TO-NORMAL event is generated.*/
    Pr_BACnetUnsigned32_t       m_stTimeDelay;

	/** The char string that the object will send in event notification
		whenever it is generated. This string is to be set using an 
		exported api. */
    NotificationString_t		m_stNotificationMsg; 
    
#endif /* BACDEL_SER_AE_EN_B */ 

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ASUM_B))
    
	/** This field should convey whether the notifications generated by the object 
    should be Events or Alarms.*/
    Pr_BACnetNotifyType_t		m_stNotifyType;

#endif

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B) || \
	(defined BACDEL_SER_AE_ESUM_A) || (defined BACDEL_SER_AE_ESUM_B))

    /** The notification class to be used when handling and generating event
	notifications for this object.*/
    Pr_BACnetUnsigned32_t		m_stNotificationClass;

#endif

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_GEI_B))

    /** Shall convey three flags that separately enable and disable reporting of TO-
	OFFNORMAL, TO-FAULT, and TO-NORMAL events.*/
    Pr_BACnetBitStr_t			m_stEventEnable;

#endif

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B))

	/** Convey the times of the last event notifications for TO-OFFNORMAL, TO-FAULT, 
    and TO-NORMAL events, respectively.*/
    Pr_BACnetEventTimeStamp_t   m_stEventTimeStamps;

#endif

#if ((defined BACDEL_SER_AE_EN_B) || (defined BACDEL_SER_AE_AA_B) || \
	(defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_A))

	/** Shall convey three flags that separately enable and disable reporting of TO-
    OFFNORMAL, TO-FAULT, and TO-NORMAL events.*/
    Pr_BACnetBitStr_t			m_stAckedTransitions;

#endif

#if ((defined BACDEL_SER_AE_GEI_B) || (defined BACDEL_SER_AE_ASUM_B) || \
	(defined BACDEL_SER_AE_ESUM_B))
    
	/** The Summary variable which describes the GEI and GAS service status */
    int32_t						m_i32GetSummary; 

#endif

#ifdef OPTIONAL_PROPERTY

    /** Description about obj*/
    Pr_BACnetCharStr_t			m_stDescription;
    
    /** Name of Device(sensor name) connected as Binary Input */
    Pr_BACnetCharStr_t			m_stDeviceType;

    /** Text to display when BIInactive*/
    Pr_BACnetCharStr_t			m_stInactive_Text;
    
    /** Text to display when BI Active*/
    Pr_BACnetCharStr_t			m_stActive_Text;

    /** Represents the date and time at which the most recent change of state 
       occurred */
    Pr_BACnetDateTime_t			m_stChangeOfStateTime;
    
    /** Represents the number of times that the Present_Value property
       has changed*/
    Pr_BACnetUnsigned16_t       m_stChangeOfStateCount; 
    
    /** Status change time reset time */
    Pr_BACnetDateTime_t			m_stCountResetTime;
    
    /** Accumulated number of secondsthat the Present_Value property
        has had the value ACTIVE*/
    bacnetActiveElapse_t		m_stElaspsedActiveTime;
    
    /** Active_Time_Reset Time */
    Pr_BACnetDateTime_t			m_stActiveTimeResetTime;
	
#ifdef BACDEL_SER_DS_COV_B

     /** The COV variable which describes the COV service status */
    int32_t						m_i32ObjCOVSupport;// COV Support flag 

#endif

#endif /* OPTIONAL_PROPERTY */  
    /** For dynamic object add/delete */
    struct BinaryInput *pstNext;

}BinaryInput_t;


/** function used in RPM service */
void Binary_Input_Property_Lists( int32_t **pi32Required,
                                  int32_t **pi32Optional,
                                  int32_t **pi32Proprietary);

#endif /* BACDEL_OBJ_BI */				
#endif /* BI_H */
/* End of objBinaryInput.h file */
