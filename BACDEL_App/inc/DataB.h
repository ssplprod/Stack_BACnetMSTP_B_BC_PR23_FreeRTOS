#ifndef DATAB_H_
#define DATAB_H_

#include "definestruct.h"


void DataB_Init();
void ResetToFactoryDefault();


DB_t 		*GetpSMCfg();


void SaveFactoryDatatoNV();

#endif /* DATAB_H_ */
