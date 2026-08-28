/**
* \file 	DefineDevice.h
*/
#ifndef DefineDevice_H_
#define DefineDevice_H_




#define ID_REGISTER_LENGTH			12							//96/8
#define ID_REGISTER_BASE_ADDRESS	(uint8_t*)	0x1FFF7A10		// one uint32_t are stored here
#define ID_REGISTER_OFFSET			(uint8_t)	0x04			// two uint32_t are stored @ base address + OFFSET
#define SRAM_ADDRESS_DFU			(unsigned long *)0x2002FFF0	// Near Ram end

#endif /* DefineDevice_H_ */
