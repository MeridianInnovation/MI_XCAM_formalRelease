# MI_XCAM_171205_formalRelease

This is the user application project of using thermal sensor XCAM from Meridian Innovation Limited.
There are two series MCU: M480 & N329-

Please read XCAM_Documentation.docx for more details.

SDK Version: Release V2.00 171205

# API

ThermalSensor.h
void			M480_InitSensor(void);

void			M480_OpenSensor(void);

void			M480_StartSensor(void);

void			N329_InitSensor(void);

void			N329_OpenSensor(void);



MI_XCAM.h

void 			InitI2C(unsigned char mode);

void 			HighDensSequentialRead(unsigned short address,unsigned char* data, unsigned short numbytes);

void 			HighDensPageWrite(unsigned short address,unsigned char* data, unsigned short numbytes);



void 			ReadCalibDataN(void);

void			InitSensorDev(unsigned short TN);

void 			InitMBITTRIMN(unsigned char user);

void 			InitBIASTRIMN(unsigned char user);

void 			InitBPATRIMN(unsigned char user);

void 			InitPUTRIMN(unsigned char user);

void 			InitCLKTRIMN(unsigned char user);



void 			GetImageData(void);

unsigned short		GetTemp(unsigned int x, unsigned int y);

unsigned int 		CalcTO(unsigned int TAmb, signed int dig, signed long PiC, unsigned int dontCalcTA);

unsigned int		StartStreaming(int Mode, char Temps, char Stream);
