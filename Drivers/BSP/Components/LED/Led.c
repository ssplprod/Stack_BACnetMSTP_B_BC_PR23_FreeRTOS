#include "Led.h"

void BSP_Set_COM_LED(uint16_t Red,uint16_t Green, uint16_t Blue )
{
	if (1 == Red)
	{
		BSP_Set_COM_LED_R_on();
	}
	else
	{
		BSP_Set_COM_LED_R_off();
	}

	if (1 == Green)
	{
		BSP_Set_COM_LED_G_on();
	}
	else
	{
		BSP_Set_COM_LED_G_off();
	}

	if (1 == Blue)
	{
		BSP_Set_COM_LED_B_on();
	}
	else
	{
		BSP_Set_COM_LED_B_off();
	}
}

void BSP_Set_STATUS_LED(uint16_t Red,uint16_t Green, uint16_t Blue )
{
//	if (1 == Red)
//	{
//		BSP_Set_STATUS_LED_R_on();
//	}
//	else
//	{
//		BSP_Set_STATUS_LED_R_off();
//	}
//
//	if (1 == Green)
//	{
//		BSP_Set_STATUS_LED_G_on();
//	}
//	else
//	{
//		BSP_Set_STATUS_LED_G_off();
//	}
//
//	if (1 == Blue)
//	{
//		BSP_Set_STATUS_LED_B_on();
//	}
//	else
//	{
//		BSP_Set_STATUS_LED_B_off();
//	}
}

void BSP_Set_ALL_LED_On()
{
	BSP_Set_COM_LED (1,0,0);
	BSP_Set_STATUS_LED (0,0,1);
	//BSP_Set_ETH_LED_on();

//	//BSP_Set_L1_LED_Y_on();
//	BSP_Set_L1_LED_G_on();
//	BSP_Set_L2_LED_G_on();
//	BSP_Set_L3_LED_G_on();
//	BSP_Set_L4_LED_G_on();
//	BSP_Set_L5_LED_G_on();
//	//BSP_Set_L5_LED_G_on();
}

void BSP_Set_ALL_LED_Off()
{
	BSP_Set_COM_LED (0,0,0);
	BSP_Set_STATUS_LED (0,0,0);
//	BSP_Set_ETH_LED_off();
//
//	BSP_Set_L1_LED_Y_off();
//	BSP_Set_L1_LED_G_off();
//	BSP_Set_L2_LED_G_off();
//	BSP_Set_L3_LED_G_off();
//	BSP_Set_L4_LED_G_off();
//	BSP_Set_L5_LED_Y_off();
//	BSP_Set_L5_LED_G_off();
}

void BSP_Display_Percent(uint32_t ui32Percent)
{
//	BSP_Set_L1_LED_Y_off();
//	BSP_Set_L1_LED_G_off();
//	BSP_Set_L2_LED_G_off();
//	BSP_Set_L3_LED_G_off();
//	BSP_Set_L4_LED_G_off();
//	BSP_Set_L5_LED_Y_off();
//	BSP_Set_L5_LED_G_off();
//
//	if ( ui32Percent <= 5 )
//		BSP_Set_L1_LED_Y_on();
//	else
//	{
//		BSP_Set_L1_LED_G_on();
//
//		if ( ui32Percent > 20 )
//			BSP_Set_L2_LED_G_on();
//		if ( ui32Percent > 40 )
//			BSP_Set_L3_LED_G_on();
//		if ( ui32Percent > 60 )
//			BSP_Set_L4_LED_G_on();
//		if ( ui32Percent > 80 && ui32Percent < 95 )
//			BSP_Set_L5_LED_G_on();
//		else if (ui32Percent >= 95 )
//			BSP_Set_L5_LED_Y_on();
//	}
}
