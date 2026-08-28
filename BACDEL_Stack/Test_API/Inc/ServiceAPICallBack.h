/*
 * ServiceAPICallBack.h
 *
 *  Created on: 25-May-2020
 *      Author: Pranav.Phadatare
 */

#ifndef BACDEL_STACK_TEST_API_INC_SERVICEAPICALLBACK_H_
#define BACDEL_STACK_TEST_API_INC_SERVICEAPICALLBACK_H_

/** callback function declaration for RP */
BACNET_RETURN_TYPE Callback_Function_RP(bacnetip_arguments_t *pstIpArgs, uint8_t u8Reason);

/** callback function declaration for RPM */
BACNET_RETURN_TYPE Callback_Function_RPM(bacnetip_arguments_t *pstIpArgs, uint8_t u8Reason);

/** function to receive WP-A callbacks */
BACNET_RETURN_TYPE Callback_Function_WP(bacnetip_arguments_t *pstIpArgs, uint8_t u8Reason);

#endif /* BACDEL_STACK_TEST_API_INC_SERVICEAPICALLBACK_H_ */
