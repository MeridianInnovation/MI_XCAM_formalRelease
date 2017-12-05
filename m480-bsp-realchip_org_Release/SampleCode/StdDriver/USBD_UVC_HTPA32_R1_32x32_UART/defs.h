#define Pixelforth 256

#define AdrPixCMin 0x00
#define AdrPixCMin 0x00
#define AdrPixCMax 0x04
#define AdrGradScale 0x08
#define AdrExponent 0x0B
#define AdrTableNumber 0x0B 		//changed to 0x0B with Rev 0.14 and adpated TN readout
#define AdrEpsilon 0x0D

#define AdrMBITPixC 0x1A
#define AdrBIASPixC 0x1B
#define AdrCLKPixC  0x1C
#define AdrBPAPixC  0x1D
#define AdrPUPixC   0x1E

#define AdrPTATGrad 0x34

#define AdrVddScaling 0x4E
#define AdrVddScalingOff 0x4F
#define AdrMBITUser 0x60
#define AdrBIASUser 0x61
#define AdrCLKUser 0x62
#define AdrBPAUser 0x63
#define AdrPUUser 0x64

#define AdrDevID 0x74
#define AdrNrOfDefPix 0x7F

#define AdrGlobalOffset 0x54
#define AdrGlobalGain 0x55
#define AdrVddCompValues2 0x340
#define AdrVddCompValues 0x540
#define AdrTh1 0x740
#define AdrTh2 0xF40
#define AdrPixC 0x1740

#define BIAScurrentDefault 0x05
#define CLKTRIMDefault 0x15 //0x20 to let it run with 10 Hz
#define BPATRIMDefault 0x0C
//#define MBITTRIMDefault 0x2C
#define PUTRIMDefault	0x88


//pixelcount etc. for 32x32d
#define Pixel 1024				//=32x32
#define PixelEighth 128
#define LINE 32
#define COLUMN 32
#define DATALength 1292//1098					//length of first packet
#define DATALength2 1288//1096					//lenght of second/last packet
#define DataLengthHalf 646
#define PTATamount 8
#define ELOFFSET 1024			//start address of el. Offset
#define ELAMOUNT 256
#define ELAMOUNTHALF 128
#define StackSize 16
#define PTATSTARTADSRESS 1282//1090
#define VDDADDRESS 1280

#define GetElEveryFrameX 10		//amount of normal frames to capture after which the el. Offset is fetched
#define STACKSIZEPTAT 30		//should be an even number
#define STACKSIZEVDD 50			//should be an even number
#define VddStackAmount 30

//#define ReadToFromTable

#ifdef ReadToFromTable

//#define HTPA32x32dL2_1HiSiF5_0
//#define HTPA32x32dL2_1HiSiF5_0_withSiFilter
//#define HTPA32x32dL3_6HiSi
//#define HTPA32x32dL3_6HiSi_Rev1
//#define HTPA32x32dL7_0HiSi
//#define HTPA32x32dL5_0HiGeF7_7
//#define HTPA32x32dL1_6HiGe
//#define HTPA32x32dR1L1_6HiGe_Gain3k3
//#define HTPA32x32dL2_1HiSiF5_0_Gain3k3			//is used for SensorRev. 1		
//#define HTPA32x32dR1L2_1HiSiF5_0_Precise		
//#define HTPA32x32dR1L3_6HiSi_Rev1_Gain3k3
//#define HTPA32x32dR1L5_0HiGeF7_7_Gain3k3		
#define HTPA32x32dR1L7_0HiSi_Gain3k3

    #ifdef HTPA32x32dL5_0HiGe
	 	#define TABLENUMBER		79
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		512
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
    #endif

    #ifdef HTPA32x32dL5_0HiGeF7_7
	 	#define TABLENUMBER		92
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
    #endif
    
	#ifdef HTPA32x32dR1L5_0HiGeF7_7_Gain3k3
	 	#define TABLENUMBER		113
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	1595		//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100			//dK
		#define ADEQUIDISTANCE	64			//dig
		#define ADEXPBITS		6						//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		1024
		#define EQUIADTABLE							//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#define AdjustOffsetGain
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
		#define MBITTRIMDefault 0x2C
		#define SensRv 1
	#endif	
	    

    #ifdef HTPA32x32dL1_6HiGe
	 	#define TABLENUMBER		101	
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471		//possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
    #endif

	#ifdef HTPA32x32dR1L1_6HiGe_Gain3k3
	 	#define TABLENUMBER		119
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	1595	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		1024
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
		#define MBITTRIMDefault 0x2C
		#define SensRv 1
	#endif
    
    #ifdef HTPA32x32dL2_1HiSi
	 	#define TABLENUMBER		80
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
    #endif

    #ifdef HTPA32x32dL2_1HiSiF5_0
	 	#define TABLENUMBER		96
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
    #endif

	#ifdef HTPA32x32dL2_1HiSiF5_0_Gain3k3
	 	#define TABLENUMBER		114
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	1595	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		1024
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
		#define MBITTRIMDefault 0x2C	//use REF_CAL=2 here. Table does not match, so GlobalGain ist set to 50 % to compensate this.
		#define SensRv 1				//Sensor Revision is set to 1 (Redesign)
	#endif	
	
	#ifdef HTPA32x32dR1L2_1HiSiF5_0_Precise
	 	#define TABLENUMBER		116
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	22
		#define NROFADELEMENTS 	1000	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	50		//dK
		#define ADEQUIDISTANCE	32		//dig
		#define ADEXPBITS		5		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		1024
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
		#define MBITTRIMDefault 0x2C
		#define SensRv 1
	#endif

    #ifdef HTPA32x32dL2_1HiSiF5_0_withSiFilter
	 	#define TABLENUMBER		97
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
    #endif

    #ifdef HTPA32x32dL3_6HiSi
	 	#define TABLENUMBER		81
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
	#endif

    #ifdef HTPA32x32dL3_6HiSi_Rev1
	 	#define TABLENUMBER		106
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
	#endif

	#ifdef HTPA32x32dR1L3_6HiSi_Rev1_Gain3k3
	 	#define TABLENUMBER		117
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	1595	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		1024
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
		#define MBITTRIMDefault 0x2C
		#define SensRv 1
	#endif	
	
    #ifdef HTPA32x32dL7_0HiSi
	 	#define TABLENUMBER		107
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
	#endif

	#ifdef HTPA32x32dR1L7_0HiSi_Gain3k3
	 	#define TABLENUMBER		118
		#define PCSCALEVAL		100000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	1595	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		640
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif
		#define MBITTRIMDefault 0x2C
		#define SensRv 1
	#endif	
	
    #ifdef HTPA32x32dL2_1HiSiDLC
		#define TABLENUMBER		83
		#define PCSCALEVAL		100000000		//PixelConst scale value for table
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	64		//dig
		#define ADEXPBITS		6		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		512
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE 
			#undef FLOATTABLE
		#endif
    #endif

    #ifdef HTPA32x32dL2_1Si_withSiFilter
		#define TABLENUMBER		88
		#define PCSCALEVAL		100000000		//PixelConst scale value for table
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	471
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	8		//dig
		#define ADEXPBITS		3		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		64
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE 
			#undef FLOATTABLE
		#endif
    #endif

   #ifdef HTPA32x32dL5_0HiGeMult
	 	#define TABLENUMBER0		79
		#define PCSCALEVAL		100000000 //327000000000		//PixelConst scale value for table... lower 'L' for (long)
		#define NROFTAELEMENTS 	7
		#define NROFADELEMENTS 	251	//130 possible due to Program memory, higher values possible if NROFTAELEMENTS is decreased
		#define TAEQUIDISTANCE	100		//dK
		#define ADEQUIDISTANCE	128		//dig
		#define ADEXPBITS		7		//2^ADEXPBITS=ADEQUIDISTANCE
		#define TABLEOFFSET		256
		#define EQUIADTABLE		//if defined, ADELEMENTS have to be 2^N quantizied! else more CPU Power is needed
		#ifdef EQUIADTABLE
			#undef FLOATTABLE
		#endif   
    #endif
#endif

