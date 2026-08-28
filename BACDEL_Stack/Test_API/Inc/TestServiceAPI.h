/*
 * TestServiceAPI.h
 *
 *  Created on: 25-May-2020
 *      Author: Pranav.Phadatare
 */

#ifndef BACDEL_STACK_TEST_API_INC_TESTSERVICEAPI_H_
#define BACDEL_STACK_TEST_API_INC_TESTSERVICEAPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "bacDELDef.h"
/** variable to save Token Id */
extern uint32_t g_u32TokenId;
/** boolean flag for RP function */
extern bool g_bRpflag;
/** boolean flag for RP function */
extern bool g_bRpmflag;

extern bool g_bWpflag;

BACNET_RETURN_TYPE RP_A_Request(void);

BACNET_RETURN_TYPE RPM_A_Request(void);

BACNET_RETURN_TYPE WP_A_Request(void);

BACNET_RETURN_TYPE WhoIs_A_Request(void);

BACNET_RETURN_TYPE IAm_Request(void);

uint32_t Generate_Request_ID();

/* Function to convert strings to structure of given data type and
*  return apprppriate data type structure pointer*/

void *Convert_StrVal_to_Strucure(BACNET_DATA_TYPE eDataType,
	void *pvInData,
	int32_t i32ArrIndx,
	uint32_t u32Option,
	BACNET_RETURN_TYPE *pstReturnType);

/**  function declaration of Function to typecast data types */
void DataType_Typecast(void* pvPropVal, BACNET_DATA_TYPE eDataType);

#endif /* BACDEL_STACK_TEST_API_INC_TESTSERVICEAPI_H_ */
