/*
 * Tsk_Bacnet.h
 *
 *  Created on:
 *      Author:
 */

#ifndef TSK_BACNET_H_
#define TSK_BACNET_H_

//PP
//void DvProcessThread_Task(void const * argument);
void DvManagementThread(void const * argument);
void Bacnet_Initiate_Thread(void const * argument);
void ClearInitiateQTimer_Timer(void const * argument);
void ClearBBMD_FDT_Timer_Timer(void const * argument);
void SwitchFromStaticToDHCP();
void SwitchFromDHCPToStatic();

void TaskEthInterpreter(void const * argument);

void app_bacnet_init(void);
void MSTP_Recieve_Thread(void const * argument);

#endif /* TSK_BACNET_H_ */

