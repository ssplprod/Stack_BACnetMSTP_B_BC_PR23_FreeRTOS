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
*   SoftDEL Systems Ltd.						india@softdel.com      
*   3rd Floor, Pentagon P4,						http://www.softdel.com
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - bacDELDeviceConfig.h
*
*   AUTHORS
*		Pratham N. Murkute
*
*   RELEASE HISTORY                                                             
*   DATE				NAME					DESCRIPTION
*   28/09/2012			Pratham Murkute			File Creation
*
******************************************************************************/

#ifndef SOFTDEL_BACNET_DEVICE_CONFIG_H
#define SOFTDEL_BACNET_DEVICE_CONFIG_H

/****************************************************************************
 *
 * MACROS, TYPEDEFS AND ENUMERATIONS.
 * Application specific definitions.
 * These definitions should be adjusted for your particular application 
 * requirements.
 *
 ****************************************************************************/

/*** Default device password */
#define DEFAULT_PASSWORD                "softDEL"
#define DEFAULT_PASSWORD_LEN            7

/********** Device Object default values for properties **********/
#define DEVICE_NAME                     "Sofdel"
#define REMOTE_DEVICE_NAME              "BACnet-Embedded IP"
#define VENDOR_NAME                     "SoftDEL systems"
#define VENDOR_IDENTIFIER               632
#define MODEL_NAME                      "System-1000"
#define FIRMWARE_REV                    "1.1" 
#define APP_SOFTWARE_VERSION            "0.1"
#define PROTOCOL_VERSION                1
#define SYSTEM_STATUS                   STATUS_OPERATIONAL  
#define DATABASE_REVISION               0
#define NUMBER_OF_APDU_RETRIES          3
#define APDU_TIMEOUT                    3000
#define SEG_SUPPORTED                   SEGMENTATION_BOTH

/****** Set the Protocol Revision support *******/
#ifdef BACDEL_PR23
#define PROTOCOL_REVISION               23
#elif defined BACDEL_PR14
#define PROTOCOL_REVISION               19
#elif defined BACDEL_PR12
#define PROTOCOL_REVISION               12
#else
#define PROTOCOL_REVISION               10
#endif

/********** Optional Properties for Device Object *********/
/** Properties required for segmentation */
#ifdef SEGMENTATION_SUPPORTED
#define APDU_SEGMENT_TIMEOUT	        6000
#define MAX_SEGMENTS_ACCEPTED	        DEFAULT_SEGMENTS_RX
#endif 
#define LOCATION                        "Pune Location"
#define DEV_DESCRIPTION                 "Device Object Description"
#define OBJ_DESCRIPTION                 "BACnet-Embedded IP Object"
#define PROFILE_NAME                    "AAC Profile"
#define LOCAL_TIME_HR					12
#define LOCAL_TIME_MIN					0
#define LOCAL_TIME_SEC					0
#define LOCAL_TIME_HUNSEC				0
#define LOCAL_DATE_YEAR					2017
#define LOCAL_DATE_MONTH				1
#define LOCAL_DATE_DAY					1
#define LOCAL_DATE_WEEKDAY				1
#ifdef BACDEL_PR14
#define SERIAL_NUMBER					"Sr.No. 123-000001"
#endif
/** default values for time sync & UTC time sync related properties */
/** default value as per IST is +330 */
/** refer clause 12.11.24 */
#define UTC_OFFSET			            (-330)
#define DAY_LIGHT_SAVING	            TRUE //FALSE
#define TIME_SYNC_INTERVAL	            0
#define	ALIGN_INTERVALS		            FALSE
#define	INTERVAL_OFFSET		            0
/** properties required for BackUp & Restore service */
#define DEFAULT_BACKUP_STATE BACKUP_STATE_IDLE
#define BACKUP_FAILURE_TIMEOUT          120
#define BACKUP_PREPARATION_TIME         10
#define RESTORE_PREPARATION_TIME        10
#define RESTORE_COMPLETION_TIME         10

/********** MSTP related Properties for Device Object *********/
/** properties required for MSTP device */
#define DEV_ID 											20
#define DEV_ID_MAX_VALUE 								4194302			
#define DEV_ID_MIN_VALUE 								1			
/********** MSTP related Properties for Device Object *********/
/** properties required for MSTP device */

//#define MAX_MASTER						127
#define MAX_INFO_FRAMES					1
#define MAX_INFO_FRAMES_HIGH_LIMIT		5
#define MAX_INFO_FRAMES_LOW_LIMIT		0
#define MAC_ID 							10


/********** Default values for Required Properties **********/
#define ANALOG_PRESENT_VALUE	        "10.0"
#define BINARY_PRESENT_VALUE            "0"
#define MULTISTATE_PRESENT_VALUE        "1"
#define CHARACTER_STRING_PRESENT_VALUE  " "
#define INTEGER_PRESENT_VALUE	        "0"
#define LARGE_ANALOG_VAL_PV         	"0.0"
#define POSITIVE_INTEGER_PRESENT_VALUE	"0"
#define STATUS_FLAG		                "0"
#define EVENT_STATE		                EVENT_STATE_NORMAL//"0"
#define OUT_OF_SERVICE                  FALSE//"0"
#define UNITS                           UNITS_NO_UNITS //UNITS_VOLTS "95"
#define AI_COV_INCREMENT                "1.0"
#define AO_COV_INCREMENT                "1.0"
#define AV_COV_INCREMENT                "1.0"
#define POLARITY                        POLARITY_NORMAL//"0"
#define NUMBER_OF_STATE	                "5"
#define PRIORITY_ARRAY_DEFAULT          0
#define OBJ_UPDATE_INTERVAL				"100"
#define CURRENT_COMMAND_PRIORITY		"0"
/********* Default values for Optional Properties **********/
#define ACTIVE_TEXT				        "ON"
#define INACTIVE_TEXT			        "OFF"
#define CHANGE_OF_STATE_TIME            "xx/xx/xx/xxxx xx:xx:xx:xx"
#define CHANGE_OF_STATE_COUNT           "0"
#define ELAPSED_ACTIVE_TIME             "0"
#define UPDATE_INTERVAL			        "0"
#define MIN_PRESENT_VALUE		        "5"
#define MAX_PRESENT_VALUE		        "100"
#define RESOLUTION				        "0.000001"
#define MIN_ON_TIME				        "0"
#define MIN_OFF_TIME			        "0"

/********* Default values for properties required for intrinsic reporting */
#define RELIABILITY			            RELIABILITY_NO_FAULT_DETECTED
#define TIME_DELAY			            "0"
#define NOTIFICATION_CLASS	            "0"
#define EVENT_ENABLE		            "0,0,0"
#define ACKED_TRANSITIONS	            "1,1,1"
#define NOTIFY_TYPE			            "0"
#ifdef BACDEL_PR14
#define RELIABILITY_EVALUATION_INHIBIT	"0"
#define TIME_DELAY_NORMAL			    "0"
#define EVENT_DETECTION_ENABLE          "0"
#define EVENT_ALGO_INHIBIT				"0"
#endif 
/** ---- AI AO AV ------ */
#define HIGH_LIMIT			            "50.0"
#define LOW_LIMIT			            "10.0"
#define FAULT_HIGH_LIMIT			     "100.0"
#define FAULT_LOW_LIMIT			         "5.0"
#define DEADBAND			            "0.0"
#define LIMIT_ENABLE		            "0"
/** ---- BI BV ------- */
#define ALARM_VALUE			            "1"
/** ---- BO ------ */
#define FEEDBACK_VALUE_BINARY_PV	    "0"
/** ---- MSI MSV ------ */
#define ALARM_VALUES	                "2"
#define FAULT_VALUES	                "3"
/** ---- MSO ------ */
#define FEEDBACK_VALUE_UNSIGNED		    "1"
/** ---- NC ----- */
#define PRIORITY		                "0,0,0"
#define ACK_REQUIRED	                "0"
/** ----- EE ------*/
#define EVENT_TYPE		                "5"

/********** Default value for Calendar & Schedule Object Properties *********/
#define CALENDAR_PRESENT_VALUE	        "0"
#define PRIORITY_FOR_WRITING	        "16"
#define SCHDULE_PRESENT_VALUE	        "0" 


/****************************************************************************
 *
 * Vendor Specific default values
 * 
 * These definitions should be adjusted for your application with Vendor
 * requirements.
 *
 ****************************************************************************/

/******** Analog input Object instance Name *******/
#ifdef BACDEL_OBJ_AI

#define ANALOG_INPUT_PRESENT_VALUE	    "0.0"
#define ANALOG_INPUT_DESCRIPTION		"Analog Input Object"
#define AI_DEVICE_TYPE					"AI Device Type"
#define ANALOG_INPUT_PROP_UNITS			UNITS_VOLTS
#define BN_AI_OBJ_MAX_INSTANCE			2		//20

#define BN_AI_0_OBJ_NAME		"Flow Input-A"
#define BN_AI_1_OBJ_NAME		"Flow Input-B"
#define BN_AI_2_OBJ_NAME		"Flow Input-C"


extern const char *BN_AI_OBJ_Name[];

#endif


/******** Analog Output Object instance Name *******/
#ifdef BACDEL_OBJ_AO

#define AO_DEVICE_TYPE				"AO Device Type"
#define ANALOG_OUTPUT_DESCRIPTION	"Analog Output Object"
#define BN_AO_OBJ_MAX_INSTANCE		2 //9

#define BN_AO_0_OBJ_NAME		"Analog Output-A"
#define BN_AO_1_OBJ_NAME		"Analog Output-B"
#define BN_AO_2_OBJ_NAME		"Analog Output-C"


extern const char *BN_AO_OBJ_Name[];

#endif

/******** Analog Value Object instance Name *******/
#ifdef BACDEL_OBJ_AV

#define ANALOG_VALUE_DESCRIPTION	"Analog Value Object"
#define BN_AV_OBJ_MAX_INSTANCE		2 //30

#define BN_AV_0_OBJ_NAME					"AV_0 Obj Name"
#define BN_AV_1_OBJ_NAME					"AV_1 Obj Name"
#define BN_AV_2_OBJ_NAME					"AV_2 Obj Name"

extern const char *BN_AV_OBJ_Name[];

#endif


/******** Binary input Object instance Name *******/
#ifdef BACDEL_OBJ_BI

#define BI_DEVICE_TYPE		"Input Contact 1"
#define BINARY_INPUT_DESCRIPTION "Binary Input Object"
#define BN_BI_OBJ_MAX_INSTANCE  2//8

#define BN_BI_0_OBJ_NAME		"Aux Input 1"
#define BN_BI_1_OBJ_NAME		"Aux Input 2"
#define BN_BI_2_OBJ_NAME		"Aux Input 3"


extern const char *BN_BI_OBJ_Name[];

#endif

/******** Binary Ouput Object instance Name *******/

#ifdef BACDEL_OBJ_BO

#define BO_DEVICE_TYPE		"BO Device Type"
#define BINARY_OUTPUT_DESCRIPTION "Binary Output Object"
#define BN_BO_OBJ_MAX_INSTANCE		2 //8

#define BN_BO_0_OBJ_NAME		"Aux Output 1"
#define BN_BO_1_OBJ_NAME		"Aux Output 2"
#define BN_BO_2_OBJ_NAME		"Aux Output 3"


extern const char *BN_BO_OBJ_Name[];
#endif

/******** Binary Value Object instance Name *******/

#ifdef BACDEL_OBJ_BV

#define BINARY_VALUE_DESCRIPTION "Binary Value Object"
#define BN_BV_OBJ_MAX_INSTANCE	2	//36

#define BN_BV_0_OBJ_NAME		"Flow Meter Active-A"
#define BN_BV_1_OBJ_NAME		"Flow Meter Active-B"
#define BN_BV_2_OBJ_NAME		"Flow Meter Active-C"

extern const char *BN_BV_OBJ_Name[];
#endif



/******** Binary Value Object instance Name *******/

#ifdef BACDEL_OBJ_MSV

#define MULTI_STATE_VALUE_DESCRIPTION	"Multi State Value Object"
#define BN_MSV_OBJ_MAX_INSTANCE		2	//4

#define BN_MSV_0_OBJ_NAME		"Meter Status-A"
#define BN_MSV_1_OBJ_NAME		"Meter Status-B"
#define BN_MSV_2_OBJ_NAME		"Operating Mode-A"

extern const char *BN_MSV_OBJ_Name[];

#endif

typedef enum {
	BACDEL_OBJ_INSTANCE_0 = 0,
	BACDEL_OBJ_INSTANCE_1, 
	BACDEL_OBJ_INSTANCE_2,
	BACDEL_OBJ_INSTANCE_3,
	BACDEL_OBJ_INSTANCE_4, 
	BACDEL_OBJ_INSTANCE_5, 
	BACDEL_OBJ_INSTANCE_6, 
	BACDEL_OBJ_INSTANCE_7, 
	BACDEL_OBJ_INSTANCE_8,
	BACDEL_OBJ_INSTANCE_9, 
	BACDEL_OBJ_INSTANCE_10,
	BACDEL_OBJ_INSTANCE_11,
	BACDEL_OBJ_INSTANCE_12,
	BACDEL_OBJ_INSTANCE_13,
	BACDEL_OBJ_INSTANCE_14,
	BACDEL_OBJ_INSTANCE_15,
	BACDEL_OBJ_INSTANCE_16,
	BACDEL_OBJ_INSTANCE_17,
	BACDEL_OBJ_INSTANCE_18,
	BACDEL_OBJ_INSTANCE_19,
	BACDEL_OBJ_INSTANCE_20,
	BACDEL_OBJ_INSTANCE_21,
	BACDEL_OBJ_INSTANCE_22,
	BACDEL_OBJ_INSTANCE_23,
	BACDEL_OBJ_INSTANCE_24,
	BACDEL_OBJ_INSTANCE_25,
	BACDEL_OBJ_INSTANCE_26,
	BACDEL_OBJ_INSTANCE_27,
	BACDEL_OBJ_INSTANCE_28,
	BACDEL_OBJ_INSTANCE_29,
	BACDEL_OBJ_INSTANCE_30,
	BACDEL_OBJ_INSTANCE_31,
	BACDEL_OBJ_INSTANCE_32,
	BACDEL_OBJ_INSTANCE_33,
	BACDEL_OBJ_INSTANCE_34,
	BACDEL_OBJ_INSTANCE_35,
	BACDEL_OBJ_INSTANCE_36,
	BACDEL_OBJ_INSTANCE_37,
	BACDEL_OBJ_INSTANCE_38,
	BACDEL_OBJ_INSTANCE_39,
	BACDEL_OBJ_INSTANCE_40,
	BACDEL_OBJ_INSTANCE_41,
	BACDEL_OBJ_INSTANCE_42,
	BACDEL_OBJ_INSTANCE_43,
	BACDEL_OBJ_INSTANCE_44,
	BACDEL_OBJ_INSTANCE_45,
	BACDEL_OBJ_INSTANCE_46,
	BACDEL_OBJ_INSTANCE_47,
	BACDEL_OBJ_INSTANCE_48,
	BACDEL_OBJ_INSTANCE_49,
	BACDEL_OBJ_INSTANCE_50,
	BACDEL_OBJ_INSTANCE_51,
	BACDEL_OBJ_INSTANCE_52,
	BACDEL_OBJ_INSTANCE_53,
	BACDEL_OBJ_INSTANCE_54,
	BACDEL_OBJ_INSTANCE_55,
	BACDEL_OBJ_INSTANCE_56,
	BACDEL_OBJ_INSTANCE_57,
	BACDEL_OBJ_INSTANCE_58,
	BACDEL_OBJ_INSTANCE_59,
	BACDEL_OBJ_INSTANCE_60,
	BACDEL_OBJ_INSTANCE_61,
	BACDEL_OBJ_INSTANCE_62,
	BACDEL_OBJ_INSTANCE_63,
	BACDEL_OBJ_INSTANCE_64,
	BACDEL_OBJ_INSTANCE_65,
	BACDEL_OBJ_INSTANCE_66,
	BACDEL_OBJ_INSTANCE_67,
	BACDEL_OBJ_INSTANCE_68,
	BACDEL_OBJ_INSTANCE_69,
	BACDEL_OBJ_INSTANCE_70,
	BACDEL_OBJ_INSTANCE_71,
	BACDEL_OBJ_INSTANCE_72,
	BACDEL_OBJ_INSTANCE_73,
	BACDEL_OBJ_INSTANCE_74,
	BACDEL_OBJ_INSTANCE_75,
	BACDEL_OBJ_INSTANCE_76,
	BACDEL_OBJ_INSTANCE_77,
	BACDEL_OBJ_INSTANCE_78,
	BACDEL_OBJ_INSTANCE_79,
	BACDEL_OBJ_INSTANCE_80,
	BACDEL_OBJ_INSTANCE_81,
	BACDEL_OBJ_INSTANCE_82,
	BACDEL_OBJ_INSTANCE_83,
	BACDEL_OBJ_INSTANCE_84,
	BACDEL_OBJ_INSTANCE_85,
	BACDEL_OBJ_INSTANCE_86,
	BACDEL_OBJ_INSTANCE_87,
	BACDEL_OBJ_INSTANCE_88,
}BACDEL_OBJ_INSTANCE_t;

#endif /* SOFTDEL_BACNET_DEVICE_CONFIG_H */
