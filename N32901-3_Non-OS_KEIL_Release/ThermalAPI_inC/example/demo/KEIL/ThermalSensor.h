#include "MI_XCAM.h"
#ifndef __HW_I2C__
	#include "DrvI2C.h"
#endif

extern EEPROM					EEPROMInfo;
extern SENSORSETTING			SensorInfo;
extern signed short 			Target[32][32];
extern int 						Image[10][192], Image_dot[];
extern INT32 					g_extend;
extern __align(4) signed short 	TDATA[2][1024];
extern	signed short			TAN;

// Color mode defination
#ifdef COLOR_ADAPTIVE						// Color palette please refers to Table_UVC.c	
	#define COLORPALETTE0_WIDERANGE			// 9 tables can be chosen, default Heated iron scale
#else
	#define COLORPALETTE0			
#endif

/*
*	UART handler command list
*	e.g: VCMD=TMP
*/
#define VCMD	0x444D4356
#define TMP		0x504D543D
#define AMT		0x544D413D
#define STA		0x4154533D
#define	RES 	0x5345523D
#define VER 	0x5245563D
#define	DAT		0x5441443D
#define SET		0x5445533D
#define POM		0x4D4F503D
#define UVC		0x4356553D

/*
*	Application functions
*/
void							N329_InitSensor(void);
void							N329_OpenSensor(void);
void 							N329_VideoStreaming(void);

int								N329_Interface_init(unsigned short interface);
void 							N329_UartDataValid_Handler(UINT8* buf, UINT32 u32Len);
