/*
 *	SDK Version: Release V2.00 171204
 *	Copyright (C) 2017 MERIDIAN Innovation Limited. All rights reserved.
 */
 
#include "stdint.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#if MCU == 329
#include <stdlib.h>
#include "w55fa93_i2c.h"		// Header of i2c- related functions
#include "wbio.h"
#include "wblib.h"

#define Pixelforth 			256
#define SENSOR_ADDRESS 		0x1A
#define EEPROM_ADDRESS 		0x50

typedef unsigned char uchar;

#elif MCU == 480
#include "M480.h"
#include "thermal_i2c.h"
#include "usbd_video.h"

#define Pixelforth 			256
#define TIME_FREQUENCY 		30
#define SENSOR_ADDRESS 		0x1A
#define EEPROM_ADDRESS 		0x50
#endif


#define DEAD_PIXEL_PRO
#define AdrPixCMin 			0x00
#define AdrPixCMin 			0x00
#define AdrPixCMax 			0x04
#define AdrGradScale 		0x08
#define AdrExponent 		0x0B
#define AdrTableNumber 		0x0B 		//changed to 0x0B with Rev 0.14 and adpated TN readout
#define AdrEpsilon 			0x0D

#define AdrMBITPixC 		0x1A
#define AdrBIASPixC 		0x1B
#define AdrCLKPixC  		0x1C
#define AdrBPAPixC  		0x1D
#define AdrPUPixC   		0x1E

#define AdrPTATGrad 		0x34

#define AdrVddScaling 		0x4E
#define AdrVddScalingOff 	0x4F
#define AdrMBITUser 		0x60
#define AdrBIASUser 		0x61
#define AdrCLKUser 			0x62
#define AdrBPAUser			0x63
#define AdrPUUser 			0x64

#define AdrDevID 			0x74
#define AdrNrOfDefPix 		0x7F
#define AdrDeadPix 			0x80		//currently reserved space for 24 Pixel
#define AdrDeadPixMask 		0xB0	//currently reserved space for 24 Pixel

#define AdrGlobalOffset 	0x54
#define AdrGlobalGain 		0x55
#define AdrVddCompValues2 	0x340
#define AdrVddCompValues 	0x540
#define AdrTh1 				0x740
#define AdrTh2 				0xF40
#define AdrPixC 			0x1740

#define BIAScurrentDefault 	0x05
#define CLKTRIMDefault 		0x15 //0x20 to let it run with 10 Hz
#define BPATRIMDefault 		0x0C
#define PUTRIMDefault		0x88


//pixelcount etc. for 32x32d
#define VERSION				0x12345678
#define Pixel 				1024				//=32x32
#define PixelEighth 		128
#define ROW 				32
#define COLUMN 				32
#define WIDTH	 	  		32
#define HEIGHT 				32
#define W_WIDTH 			2
#define W_HEIGHT 			2
#define PTATamount 			8
#define ELOFFSET 			1024			//start address of el. Offset
#define ELAMOUNT 			256
#define ELAMOUNTHALF 		128
#define StackSize 			16
#define PTATSTARTADSRESS 	1282//1090
#define VDDADDRESS 			1280

#define GetElEveryFrameX 	10		//amount of normal frames to capture after which the el. Offset is fetched
#define STACKSIZEPTAT 		30		//should be an even number
#define STACKSIZEVDD 		50			//should be an even number
#define VddStackAmount 		30
#define MAXNROFDEFECTS  	24

typedef struct REGISTERSETTING{
	unsigned short 			MBIT;
	unsigned short 			BIAS;
	unsigned short 			CLOCK;
	unsigned short 			BPA;
	unsigned short 			PU;
} REGISTERSETTING;

typedef struct EEPROM{
	float 					PixCMin;
	float 					PixCMax;
	unsigned short			gradScale;
	unsigned short 			TableNumberSensor;
	unsigned short 			epsilon;
	REGISTERSETTING			calibRegister;
	unsigned short  		VddRef;
	float 					PTATGrad;
	float					PTATOff;
	unsigned char 			VddScaling;
	unsigned short 			VddScalingOff;
	unsigned char 			GlobalOffset;
	unsigned short 			GlobalGain;
	REGISTERSETTING			userRegister;
	unsigned short			DevID;
	unsigned char 			NrOfDefPix;
	unsigned short 			DeadPixAdr[MAXNROFDEFECTS];
	unsigned char 			DeadPixMask[MAXNROFDEFECTS];
	signed short			VddGrad[ELAMOUNT];
	signed short  			VddOff[ELAMOUNT];
	signed short 			ThGradN[Pixel];
	signed short 			ThOffN[Pixel];
	unsigned long 			PixCN[Pixel];
} EEPROM;

typedef struct SENSORSETTING{
	signed long 			TABLENUMBER;
	signed long long 		PCSCALEVAL;		
	signed long  			NROFTAELEMENTS;
	signed long  			NROFADELEMENTS;		
	signed long  			TAEQUIDISTANCE;			
	signed long  			ADEQUIDISTANCE;			
	signed long  			ADEXPBITS;						
	signed long  			TABLEOFFSET;
	unsigned char 			MBITTRIMDefault;
	signed long  			SensRv;
	unsigned int*	 		TempTable;
	unsigned int* 			XTATemps;
	unsigned int* 			YADValues;
} SENSORSETTING;

void 			InitI2C(unsigned char mode);
void 			HighDensSequentialRead(unsigned short address,unsigned char* data, unsigned short numbytes);
void 			HighDensPageWrite(unsigned short address,unsigned char *data, unsigned short numbytes);

void 			ReadCalibDataN(void);
void			InitSensorDev(unsigned short TN);
void 			InitMBITTRIMN(unsigned char user);
void 			InitBIASTRIMN(unsigned char user);
void 			InitBPATRIMN(unsigned char user);
void 			InitPUTRIMN(unsigned char user);
void 			InitCLKTRIMN(unsigned char user);

void 			GetImageData(void);
unsigned short 	GetTemp(unsigned int x, unsigned int y);
unsigned int 	CalcTO(unsigned int TAmb, signed int dig, signed long PiC, unsigned int dontCalcTA);
unsigned int 	StartStreaming(int Mode, char Temps,char Stream);
