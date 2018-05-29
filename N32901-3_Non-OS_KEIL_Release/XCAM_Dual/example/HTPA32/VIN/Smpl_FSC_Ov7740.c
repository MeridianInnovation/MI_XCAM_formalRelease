#include "wblib.h"
#include "W55FA93_VideoIn.h"
#include "W55FA93_GPIO.h"
#include "DrvI2C.h"
#include "vin_demo.h"

#define __2ND_PORT__
#define __GPIO_PIN__

typedef void (*PFN_DRVI2C_TIMEDELY)(UINT32);


typedef struct
{
	UINT32 u32Width;
	UINT32 u32Height;
	char* pszFileName;
}S_VIDEOIN_REAL;
typedef struct
{
	UINT32 u32Width;
	UINT32 u32Height;
	E_VIDEOIN_OUT_FORMAT eFormat;
	char* pszFileName;
}S_VIDEOIN_PACKET_FMT;

struct OV_RegValue
{
	UINT8	u8RegAddr;		//Register Address
	UINT8	u8Value;			//Register Data
};

struct OV_RegTable{
	struct OV_RegValue *sRegTable;
	UINT32 u32TableSize;
};


extern __align(4) UINT32 u32PacketFrameBuffer[2][640*480/2];

#define _REG_TABLE_SIZE(nTableName)	(sizeof(nTableName)/sizeof(struct OV_RegValue))

struct OV_RegValue g_sOV7740_RegValue[] = 
{ // VGA 30fps @24MHz
/*
	{0x12, 0x80},{0x13, 0x00},{0x11, 0x01},{0x12, 0x00},{0xd5, 0x10},{0x0c, 0x52},{0x0d, 0x34},
	{0x17, 0x24},{0x18, 0xa0},{0x19, 0x03},{0x1A, 0xf0},{0x1b, 0x89},{0x1e, 0x13},{0x22, 0x03},{0x29, 0x17},
	{0x2b, 0xf8},{0x2c, 0x01},{0x31, 0xa0},{0x32, 0xf0},{0x33, 0xc4},{0x3a, 0xb4},{0x36, 0x3f},
	{0x04, 0x60},{0x27, 0x80},{0x3d, 0x0f},{0x3e, 0x82},{0x3f, 0x40},{0x40, 0x7f},{0x41, 0x6a},
	{0x42, 0x29},{0x44, 0xe5},{0x45, 0x41},{0x47, 0x42},{0x48, 0x00},{0x49, 0x61},{0x4a, 0xa1},
	{0x4b, 0x46},{0x4c, 0x18},{0x4d, 0x50},{0x4e, 0x13},{0x64, 0x00},{0x67, 0x88},{0x68, 0x1a},
	{0x14, 0x38},{0x24, 0x3c},{0x25, 0x30},{0x26, 0x72},{0x50, 0x97},{0x51, 0x7e},{0x52, 0x00},
	{0x53, 0x00},{0x20, 0x00},{0x21, 0x23},{0x38, 0x14},{0xe9, 0x00},{0x56, 0x55},{0x57, 0xff},
	{0x58, 0xff},{0x59, 0xff},{0x5f, 0x04},{0x13, 0xff},{0x80, 0x7d},{0x81, 0x3f},{0x82, 0x32},
	{0x83, 0x03},{0x38, 0x11},{0x84, 0x70},{0x85, 0x00},{0x86, 0x03},{0x87, 0x01},{0x88, 0x05},
	{0x89, 0x30},{0x8d, 0x30},{0x8f, 0x85},{0x93, 0x30},{0x95, 0x85},{0x99, 0x30},{0x9b, 0x85},
	{0x9c, 0x08},{0x9d, 0x12},{0x9e, 0x23},{0x9f, 0x45},{0xa0, 0x55},{0xa1, 0x64},{0xa2, 0x72},
	{0xa3, 0x7f},{0xa4, 0x8b},{0xa5, 0x95},{0xa6, 0xa7},{0xa7, 0xb5},{0xa8, 0xcb},{0xa9, 0xdd},
	{0xaa, 0xec},{0xab, 0x1a},{0xce, 0x78},{0xcf, 0x6e},{0xd0, 0x0a},{0xd1, 0x0c},{0xd2, 0x84},
	{0xd3, 0x90},{0xd4, 0x1e},{0x5a, 0x24},{0x5b, 0x1f},{0x5c, 0x88},{0x5d, 0x60},{0xac, 0x6e},
	{0xbe, 0xff},{0xbf, 0x00},{0x70, 0x00},{0x71, 0x34},{0x74, 0x28},{0x75, 0x98},{0x76, 0x00},
	{0x77, 0x08},{0x78, 0x01},{0x79, 0xc2},{0x7d, 0x02},{0x7a, 0x9c},{0x7b, 0x40},{0xec, 0x02},
	{0x7c, 0x0c},{0x16, 0x01}
*/
	// ~15 fps
	{0x12, 0x80},{0x13, 0x00},{0x11, 0x03},{0x12, 0x00},{0xd5, 0x10},{0x0c, 0x52},{0x0d, 0x34},
	{0x17, 0x24},{0x18, 0xa0},{0x19, 0x03},{0x1A, 0xf0},{0x1b, 0x89},{0x1e, 0x13},{0x22, 0x03},{0x29, 0x17},
	{0x2b, 0xf8},{0x2c, 0x01},{0x31, 0xa0},{0x32, 0xf0},{0x33, 0xc4},{0x3a, 0xb4},{0x36, 0x3f},
	{0x04, 0x60},{0x27, 0x80},{0x3d, 0x0f},{0x3e, 0x82},{0x3f, 0x40},{0x40, 0x7f},{0x41, 0x6a},
	{0x42, 0x29},{0x44, 0xe5},{0x45, 0x41},{0x47, 0x42},{0x48, 0x00},{0x49, 0x61},{0x4a, 0xa1},
	{0x4b, 0x46},{0x4c, 0x18},{0x4d, 0x50},{0x4e, 0x13},{0x64, 0x00},{0x67, 0x88},{0x68, 0x1a},
	{0x14, 0x38},{0x24, 0x3c},{0x25, 0x30},{0x26, 0x72},{0x50, 0x4c},{0x51, 0x3f},{0x52, 0x00},
	{0x53, 0x00},{0x20, 0x00},{0x21, 0x57},{0x38, 0x14},{0xe9, 0x00},{0x56, 0x55},{0x57, 0xff},
	{0x58, 0xff},{0x59, 0xff},{0x5f, 0x04},{0x13, 0xff},{0x80, 0x7d},{0x81, 0x3f},{0x82, 0x32},
	{0x83, 0x03},{0x38, 0x11},{0x84, 0x70},{0x85, 0x00},{0x86, 0x03},{0x87, 0x01},{0x88, 0x05},
	{0x89, 0x30},{0x8d, 0x30},{0x8f, 0x85},{0x93, 0x30},{0x95, 0x85},{0x99, 0x30},{0x9b, 0x85},
	{0x9c, 0x08},{0x9d, 0x12},{0x9e, 0x23},{0x9f, 0x45},{0xa0, 0x55},{0xa1, 0x64},{0xa2, 0x72},
	{0xa3, 0x7f},{0xa4, 0x8b},{0xa5, 0x95},{0xa6, 0xa7},{0xa7, 0xb5},{0xa8, 0xcb},{0xa9, 0xdd},
	{0xaa, 0xec},{0xab, 0x1a},{0xce, 0x78},{0xcf, 0x6e},{0xd0, 0x0a},{0xd1, 0x0c},{0xd2, 0x84},
	{0xd3, 0x90},{0xd4, 0x1e},{0x5a, 0x24},{0x5b, 0x1f},{0x5c, 0x88},{0x5d, 0x60},{0xac, 0x6e},
	{0xbe, 0xff},{0xbf, 0x00},{0x70, 0x00},{0x71, 0x34},{0x74, 0x28},{0x75, 0x98},{0x76, 0x00},
	{0x77, 0x08},{0x78, 0x01},{0x79, 0xc2},{0x7d, 0x02},{0x7a, 0x9c},{0x7b, 0x40},{0xec, 0x02},
	{0x7c, 0x0c},{0x16, 0x01}
};

static struct OV_RegTable g_OV_InitTable[] =
{//8 bit slave address, 8 bit data. 
	{0, 0},
	{0, 0},//{g_sOV6880_RegValue,	_REG_TABLE_SIZE(g_sOV6880_RegValue)},		
	{0, 0},//{g_sOV7648_RegValue,	_REG_TABLE_SIZE(g_sOV7648_RegValue)},
	{0, 0},//{g_sov7670_RegValue,	_REG_TABLE_SIZE(g_sov7670_RegValue)},
	{0, 0},//{g_sOV2640_RegValue,	_REG_TABLE_SIZE(g_sOV2640_RegValue)},	
	{0, 0},//{g_sOV9660_RegValue,	_REG_TABLE_SIZE(g_sOV9660_RegValue)},
	{g_sOV7740_RegValue,	_REG_TABLE_SIZE(g_sOV7740_RegValue)},	
	{0, 0}	
};

static UINT8 g_uOvDeviceID[]= 
{
	0x00,		// not a device ID
	0xc0,		// ov6680
	0x42,		// ov7648
	0x42,		// ov7670
	0x60,		// ov2640
	0x60,		// 0v9660
	0x42,		// 0v7740	
	0x00		// not a device ID
};


void Vin_Delay(UINT32 nCount)
{
#if 0
	volatile UINT32 i;
	for(;nCount!=0;nCount--)
		for(i=0;i<5;i++);
#else
	for(;nCount!=0;nCount--);
#endif		

}

#if 1//#ifdef __3RD_PORT__
/*
	Sensor power down and reset may default control on sensor daughter board.
	Reset by RC.
	Sensor alway power on (Keep low)

*/
static void SnrReset(void)
{
	
}

static void SnrPowerDown(BOOL bIsEnable)
{/* GPB4 power down, HIGH for power down */

    outp32(REG_GPBFUN, inp32(REG_GPBFUN) & (~MF_GPB4));
	
    gpio_setportval(GPIO_PORTB, 1<<4, 1<<4);		//GPIOB 4 set high default
    gpio_setportpull(GPIO_PORTB, 1<<4, 1<<4);		//GPIOB 4 pull-up 
    gpio_setportdir(GPIO_PORTB, 1<<4, 1<<4);		//GPIOB 4 output mode 				
    if(bIsEnable)
        gpio_setportval(GPIO_PORTB, 1<<4, 1<<4);	//GPIOB 4 set high
    else				
        gpio_setportval(GPIO_PORTB, 1<<4, 0);		//GPIOB 4 set low
}
#endif 


VOID OV7740_Init(UINT32 nIndex)
{
    UINT32 u32Idx;
    UINT32 u32TableSize;
    UINT8  u8DeviceID;
    UINT8 u8ID;
    UINT8 u8CharIn;
    struct OV_RegValue *psRegValue;
    DBG_PRINTF("Sensor ID = %d\n", nIndex);
    if ( nIndex >= (sizeof(g_uOvDeviceID)/sizeof(UINT8)) )
        return;
    videoIn_Open(48000, 24000);								/* For sensor clock output */	

#if 1 //#ifdef __3RD_PORT__
    SnrPowerDown(FALSE);
    SnrReset();		 
#endif
		
    u32TableSize = g_OV_InitTable[nIndex].u32TableSize;
    psRegValue = g_OV_InitTable[nIndex].sRegTable;
    u8DeviceID = g_uOvDeviceID[nIndex];
//u8DeviceID >>=1;	
    DBG_PRINTF("Device Slave Addr = 0x%x\n", u8DeviceID);
    if ( psRegValue == 0 )
        return;	
		
    DBG_PRINTF("Set GPIO Open\n");	
#ifdef __GPIO_PIN__		
//	gpio_open(GPIO_PORTD, 2);				//GPIOD 2 as GPIO, GPD2 --> OV_SDA
//	gpio_open(GPIO_PORTD, 1);				//GPIOD 1 as GPIO, GPD1 --> OV_SCK 
//	gpio_open(GPIO_PORTD);				//GPIOD 1 as GPIO, GPD1 --> OV_SCK 
//  gpio_open(GPIO_PORTA);				//GPIOA 1 as GPIO, GPD1 --> OV_SCK 	
    outpw(REG_GPAFUN , inpw(REG_GPAFUN) & ~(0x00003C0));
#else	
    gpio_open(GPIO_PORTB);				//GPIOB as GPIO	
#endif 	
	
    DBG_PRINTF("I2C Open\n");
    DrvI2C_Open(eDRVGPIO_GPIOA, 	        // SCK(GPD1 --> GPA3(V07))				
				eDRVGPIO_PIN3, 
				eDRVGPIO_GPIOA,             // SDA(GPD2 --> GPA4(V07))
				eDRVGPIO_PIN4, 
				(PFN_DRVI2C_TIMEDELY)Vin_Delay);
				
									
    for(u32Idx=0;u32Idx<u32TableSize; u32Idx++, psRegValue++)
    {
        I2C_Write_8bitSlaveAddr_8bitReg_8bitData(u8DeviceID, (psRegValue->u8RegAddr), (psRegValue->u8Value));	
        if ((psRegValue->u8RegAddr)==0x12 && (psRegValue->u8Value)==0x80)
        {	
            Vin_Delay(8000);		
            DBG_PRINTF("Delay A loop\n");
        }	
    }
	
	
    u8ID = I2C_Read_8bitSlaveAddr_8bitReg_8bitData(u8DeviceID, 0x0A);
    DBG_PRINTF("Sensor ID0(A) = 0x%x\n", u8ID);	// should be 0x77 for OV7742

    u8ID = I2C_Read_8bitSlaveAddr_8bitReg_8bitData(u8DeviceID, 0x0b);
    DBG_PRINTF("Sensor ID0(B) = 0x%x\n", u8ID);	// should be 0x42 for OV7742


    DrvI2C_Close();	
}


/*===================================================================
	LCD dimension = (OPT_LCD_WIDTH, OPT_LCD_HEIGHT)	
	Packet dimension = (OPT_PREVIEW_WIDTH*OPT_PREVIEW_HEIGHT)	
	Stride should be LCM resolution  OPT_LCD_WIDTH.
	Packet frame start address = VPOST frame start address + (OPT_LCD_WIDTH-OPT_PREVIEW_WIDTH)/2*2 	
=====================================================================*/
UINT32 Smpl_OV7740(VOID)
{
    PFN_VIDEOIN_CALLBACK pfnOldCallback;
    UINT32 u32GCD;
		
#ifdef __1ST_PORT__	
    videoIn_Init(TRUE, 0, 24000, eVIDEOIN_SNR_CCIR601);
#endif
#ifdef __2ND_PORT__
    videoIn_Init(TRUE, 0, 24000, eVIDEOIN_2ND_SNR_CCIR601);	
#endif	
#ifdef __3RD_PORT__
    videoIn_Init(TRUE, 0, 24000, eVIDEOIN_3RD_SNR_CCIR601);	
#endif	

    OV7740_Init(6);			
    videoIn_Open(64000, 24000);		
    videoIn_EnableInt(eVIDEOIN_VINT);
	
    videoIn_InstallCallback(eVIDEOIN_VINT, 
        (PFN_VIDEOIN_CALLBACK)VideoIn_InterruptHandler,
        &pfnOldCallback	);	//Frame End interrupt
						
    videoIn_SetPacketFrameBufferControl(FALSE, FALSE);	
	
    videoinIoctl(VIDEOIN_IOCTL_SET_POLARITY,
        TRUE,
				FALSE,							//Polarity.	
				TRUE);							
												
    videoinIoctl(VIDEOIN_IOCTL_ORDER_INFMT_OUTFMT,								
				eVIDEOIN_IN_UYVY, 			//Input Order 
				eVIDEOIN_IN_YUV422	,		//Intput format
				eVIDEOIN_OUT_YUV422);		//Output format for packet 														
		
    videoinIoctl(VIDEOIN_IOCTL_SET_CROPPING_START_POSITION,				
				0,							//Vertical start position
				0,							//Horizontal start position	
				0);							//Useless
    videoinIoctl(VIDEOIN_IOCTL_CROPPING_DIMENSION,				
				OPT_CROP_HEIGHT,							//UINT16 u16Height, 
				OPT_CROP_WIDTH,							//UINT16 u16Width;	
				0);							//Useless
				
    /* Set Packet dimension */			
    u32GCD = GCD(OPT_PREVIEW_HEIGHT, OPT_CROP_HEIGHT);						 							 
    videoinIoctl(VIDEOIN_IOCTL_VSCALE_FACTOR,
				eVIDEOIN_PACKET,			//272/480
				OPT_PREVIEW_HEIGHT/u32GCD,
				OPT_CROP_HEIGHT/u32GCD);		
    u32GCD = GCD(OPT_PREVIEW_WIDTH, OPT_CROP_WIDTH);																
    videoinIoctl(VIDEOIN_IOCTL_HSCALE_FACTOR,
				eVIDEOIN_PACKET,			//364/640
				OPT_PREVIEW_WIDTH/u32GCD,
				OPT_CROP_WIDTH/u32GCD);	
					
    /* Set Pipes stride */				
    videoinIoctl(VIDEOIN_IOCTL_SET_STRIDE,										
				OPT_STRIDE,	
				0,
				0);
    /* Set Packet Buffer Address */					
    videoinIoctl(VIDEOIN_IOCTL_SET_BUF_ADDR,
				eVIDEOIN_PACKET,			
				0, 							//Packet buffer addrress 0	
				(UINT32)((UINT32)&u32PacketFrameBuffer[0][0]) + UVC_OFFSET);

    videoinIoctl(VIDEOIN_IOCTL_SET_PIPE_ENABLE,
				TRUE, 						// Engine enable ?
				eVIDEOIN_PACKET,			// which packet was enable. 											
				0 );							//Useless		
							
    sysSetLocalInterrupt(ENABLE_IRQ);						
														
    return Successful;			
}	