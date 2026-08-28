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
*   SoftDEL Systems Ltd.                india@softdel.com         
*   3rd Floor, Pentagon P4,             http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - objAnalogOutput.h
*
*   RELEASE HISTORY 
*   
*   DATE         NAME                 DESCRIPTION
*   04/07/2011   Prashant Badgujar    File Creation.
*   03-08-2011   M.Venu               Changed BacnetStatusFlags_t 
*                                     data types to Pr_BACnetBitStr_t
*   09/08/2011   Ashish Verma         Modified analogoutput_t structure.
******************************************************************************/

#ifndef AO_H
#define AO_H
#ifdef BACDEL_OBJ_AO

#include "bacDELDef.h"
#include "propertydef.h"

/**
*                                                                         
*Name - analogoutput                                         
*                                                                      
*DESCRIPTION 
*   BACnet Analog Output Required & Optional Properties.
*   
*/
typedef struct analogoutput
{
    /** Numeric code that is used to identify the object*/
    Pr_BACnetObjId_t		    m_stObjectID; 

    /** A name for the object */
    Pr_BACnetCharStr_t		    m_stObjName;  

    /** The value of this property shall be ANALOG_INPUT */
    Pr_BACnetObjType_t		    m_stObjectType;
   
    /** Represents four Boolean flags that indicate the general "health" of an analog 
	input */
    Pr_BACnetBitStr_t			m_stStatusFlag;

    /** A way to determine if this object has an active event state associated with it*/
    Pr_BACnetEventState_t		m_stEventState;   
    
    /** An indication whether (TRUE) or not (FALSE) the physical input that the object 
	represents is not in service.*/
    Pr_BACnetBool_t				m_stOutOfService;

    /** Measurement units of this object. */
    Pr_BACnetEnggUnits_t	    m_stUnit;

    /** An array of prioritized values */
    Pr_BACnetPriorityArray_t    m_stPriorityArray;

    /* This flag bit is added to inform application about callback registration */
    bool						bAppCallBack;

    /** The current value of the input being measured*/
    Pr_BACnetReal_t             m_stPresentValue;

    /** Dummy present vlaue when out of service is true */
    Pr_BACnetReal_t             m_stBackupPresentValue;
    
    /** This property is the default value to be used for the Present_Value property when 
    all command priority values in the Priority_Array property have a NULL value.*/
    Pr_BACnetReal_t             m_stRelinquishDefault;

#ifdef BACDEL_PR14
	/** property identifier for each property that exists within the object. 
	The Object_Name, Object_Type, Object_Identifier, and Property_List properties 
	are not included in the list */
    Pr_BACnetPropertyList_t		m_stPropertyList;
#endif

#ifdef BACDEL_PR18 //KV
	/* the value of this property indicates the currently active priority and shall
	be equal to the index of the entry in the Priority_Array from which the Present_Value’s
	value has been taken. */
	Pr_BACnetOptionalUnsigned_t m_stCurrentCmdPriority;
#endif

#if (defined OPTIONAL_PROPERTY || defined BACDEL_SER_AE_EN_B)

	/** Provides an indication of whether the Present_Value or the operation of
	the physical input in question is "reliable" as far as the BACnet 
    Device or operator can determine and, if not, why? **/
    Pr_BACnetReliability_t		m_stReliability;

#endif

#ifdef BACDEL_SER_AE_EN_B
    
    /** A limit that the Present_Value must exceed before an event is generated.*/
    Pr_BACnetReal_t				m_stHighLimit;

    /* A limit that the Present_Value must exceed before an event is generated */
    Pr_BACnetReal_t				m_stLowLimit;

    /* A range between the High_Limit and Low_Limit properties, which the Present_Value
	must remain within*/
	Pr_BACnetReal_t				m_stDeadBand;

	/** Specify the minimum period of time in seconds during which the Present_Value must
	remain equal to the Alarm_Value property before a TO-OFFNORMAL event is generated,
	or remain not equal to the Alarm_Value property before a TO-NORMAL event is 
	generated.*/
    Pr_BACnetUnsigned32_t		m_stTimeDelay;

    /* This property is required if intrinsic reporting is supported by this object.*/
    Pr_BACnetBitStr_t			m_stLimitEnable; 

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

	Pr_BACnetBool_t				m_stEventDetectionEnable;

	Pr_BACnetUnsigned32_t		m_stTimeDelayNormal;
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

    /** AI Object description */
    Pr_BACnetCharStr_t          m_stDescription;

    /** A text description of the physical device connected 
        to the analog input.*/    
    Pr_BACnetCharStr_t          m_stDeviceType;

    /** The lowest number in engineering units that can be reliably obtained for the 
	present_Value property of this object.*/
    Pr_BACnetReal_t				m_stMinPresValue;

    /** The highest number in engineering units that can be reliably obtained for the 
	present_Value property of this object.*/
    Pr_BACnetReal_t				m_stMaxPresValue;

    /** The smallest recognizable change in Present_Value. */
    Pr_BACnetReal_t				m_stResolution;  
	
    // KV
	Pr_BACnetUnsigned32_t  		m_stCurrentCommandPriority;

	#ifdef BACDEL_SER_DS_COV_B

    /** shall specify the minimum change in Present_Value that will cause a 
    COVNotification to be issued to subscriber COV-clients.*/
    Pr_BACnetReal_t             m_stCOVIncrement;

    /** The COV variable which describes the COV service status */
    int32_t						m_i32ObjCOVSupport;// COV Support flag

	#endif /* BACDEL_SER_DS_COV_B */
 
#endif       
	/** For dynamic object add/delete */
    struct  analogoutput *pstNext; 

}analogoutput_t;

/** function used in RPM service */
void Analog_Output_Property_Lists(int32_t **pi32Required, 
                                 int32_t **pi32Optional,
                                 int32_t **pi32Proprietary);

#endif /* BACDEL_OBJ_AO */
#endif /* AO_H */
