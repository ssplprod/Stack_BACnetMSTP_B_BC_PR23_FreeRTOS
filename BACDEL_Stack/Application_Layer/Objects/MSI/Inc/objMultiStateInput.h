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
*   objMultiStateInput.h 
*
*  AUTHOR
*
*  DESCRIPTION
*   Header file with all supported definations/declarations form 
*       Multistate_Input.c
*
*  RELEASE HISTORY
*   DATE         NAME          DESCRIPTION
*
*   01-07-2011   M.Venu        Initial Draft
*   06-07-2011   M.Venu        1. Retrun Type changed for function
*                                 Read_MultistateInput_Property
*                                 Write_MultistateInput_Property
*                              2. MAX_MULTISTATES moved to propertydef.h
*   12-07-2011   M.Venu        1. Updated comments in sync with 
*                                 objDevice.h header
*   18-07-2011   M.Venu        Structur modifications as per data types 
*                              defined specific to BACnet standard
*   22-07-2011   M.Venu        Added Init_MSInput_Prop function
*   03-08-2011   M.Venu        Changed BacnetStatusFlags_t data types
*                              to Pr_BACnetBitStr_t data type
*   09/08/2011   Ashish Verma  Modified MultistateInput_t structure.
*************************************************************************/

/** 
@file objMultiStateInput.h Contain all definations/declarations of 
                         Multistate_Input.c file
*/

/*
*******************************************************************************
*                            Include Files
*******************************************************************************
*/
#ifndef MSI_H
#define MSI_H
#ifdef BACDEL_OBJ_MSI

#include "bacDELDef.h"
#include "propertydef.h"

/**
*
*Name - MultistateInput
*
*DESCRIPTION 
*   BACnet Multistate Input structure with All properties
*   
*/

typedef struct MultistateInput
{
    /** Object Identifier */
    Pr_BACnetObjId_t			m_stObjectID;
    
    /** Object Name*/
    Pr_BACnetCharStr_t			m_stObjName;
    
    /** Object Type */
    Pr_BACnetObjType_t			m_stObjectType;
    
    /** Object Present value */
    Pr_BACnetUnsigned32_t       m_stPresentValue;
    
    /** Dummy present vlaue when out of service is true */
    Pr_BACnetUnsigned32_t       m_stBackupPresentValue;

    /** Number of states that the Present_Value may have*/
    Pr_BACnetUnsigned32_t       m_stNumberOfStates;
    
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
    
#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object. 
	The Object_Name, Object_Type, Object_Identifier, and Property_List properties 
	are not included in the list */
    Pr_BACnetPropertyList_t     m_stPropertyList;
#endif

#ifdef BACDEL_SER_DS_COV_B

    /** The COV variable which describes the COV service status */
    int32_t						m_i32ObjCOVSupport;// COV Support flag   

#endif /* BACDEL_SER_DS_COV_B */ 

#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B)

	/** Provides an indication of whether the Present_Value or the operation of
	the physical input in question is "reliable" as far as the BACnet 
    Device or operator can determine and, if not, why? **/
    Pr_BACnetReliability_t		m_stReliability;

#endif

#ifdef BACDEL_SER_AE_EN_B    
   
    /**Specify any states the Present_Value must equal before
       a TO-OFFNORMAL event is generated*/
    Pr_ListOfUnsigned_t			m_stAlarmValues;
    
    /**Specify any states the Present_Value must equal before a 
       TO-FAULT event is generated*/
    Pr_ListOfUnsigned_t			m_stFaultValues;

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
                                              
    /** For Profile name */
    Pr_BACnetCharStr_t			m_stProfileName;

     /** Description about the object */
    Pr_BACnetCharStr_t			m_stDescription;
    
    /** Type of Device connected as MSI*/
    Pr_BACnetCharStr_t			m_stDeviceType;

     /** Text Desciption of of each present value state */
    Pr_ListOfCharStr_t			m_stStateText;

    //KV
	Pr_BACnetUnsigned32_t		m_stTimeDelayNormal;

    Pr_BACnetBool_t 			m_stEventDetectionEnable;

#endif /* OPTIONAL_PROPERTY */ 
    /** For dynamic object add/delete*/
    struct MultistateInput  *pstNext;

}MultistateInput_t;

/** Function Prototypes used in RPM service */
void Multistate_Input_Property_Lists(
		int32_t **pRequired,
		int32_t **pOptional,
		int32_t **pProprietary);


#endif /* BACDEL_OBJ_MSI */
#endif /* MSI_H */
/* End of objMultiStateInput.h file */
