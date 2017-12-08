# MI_XCAM_171205_formalRelease

This is the user application project of using thermal sensor XCAM from Meridian Innovation Limited.
There are two series MCU: M480 & N329-

Please read XCAM_Documentation.docx for more details.

SDK Version: Release V2.01 171208

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
		
# Known Issues

-	Could not open file with correct existing file path

.\MI_Thermal\MI_XCAM_N329_SW_I2C.axf: error: L6002U: Could not open file ..\..\..\..\ThermalAPI_inC\example\demo\KEIL\demo_Data\N32901_SingleTask\ThermalSensorAPI_N329.lib: No such file or directory

Soultion: Probably it is because the path is too long, which may cause problem. Relocate the file to somewhere with shorter path.

-	(N329) Can I output Uart/Huart data without opening XCAM?

Solution: You may try to move "TempCal()" to outside the usb connected checking statement "if (usbdStatus.appConnected == 1)".
