#include "MI_XCAM.h"
#ifndef __HW_I2C__
	#include "DrvI2C.h"
#endif

extern EEPROM					EEPROMInfo;
extern SENSORSETTING			SensorInfo;
extern signed short 			Target[32][32];
extern int 						Image[10][192], Image_dot[];
extern INT32 					g_extend;
extern __align(4) short 		TDATA[2][1024];

/*
*	Application functions
*/
void							N329_InitSensor(void);
void							N329_OpenSensor(void);

int								N329_Interface_init(unsigned short interface);
void 							N329_UartDataValid_Handler(UINT8* buf, UINT32 u32Len);
