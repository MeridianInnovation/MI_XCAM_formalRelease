#include "ThermalSensor.h"

#include "usbd.h"
#include "videoclass_HTPA32.h"
#include "w55fa93_vpost.h"
#ifndef __HW_I2C__
#include "W55FA93_GPIO.h"
#include "DrvI2C.h"
#endif
#include "vin_demo.h"

extern __align(4) UINT32 		u32PacketFrameBuffer[2][640*480/2];
extern UINT32 					u32Lock,u32UvcBufferIndex;
extern INT32 g_i16DisTemp, g_i16Ready, g_TDATA_index;  

extern YUV_COLOR_INFO_T YUV_ColorTable[];
extern RGB_COLOR_INFO_T RGB_ColorPalette[];

extern FRAMEPOIS framePOIs;

void 							uvcdEvent(UINT8 u8index);
int 							VIN_main(void);

void create_color_table(void)
{
    int i,j;
	unsigned char RVal,GVal,BVal, Color_Y, Color_U, Color_V;
    // Create Color Table 
    for(i=0;i<60;i++)
    {
        for(j=0;j<20;j++)
        {
			RVal = (unsigned char)((float)RGB_ColorPalette[i].R  + ((float)RGB_ColorPalette[i+1].R - (float)RGB_ColorPalette[i].R) /20*j); 	
			GVal = (unsigned char)((float)RGB_ColorPalette[i].G  + ((float)RGB_ColorPalette[i+1].G - (float)RGB_ColorPalette[i].G) /20*j); 	
			BVal = (unsigned char)((float)RGB_ColorPalette[i].B  + ((float)RGB_ColorPalette[i+1].B - (float)RGB_ColorPalette[i].B) /20*j); 
			Color_Y = (unsigned char)(RVal * 0.299 	+ GVal * 0.587	+ BVal * 0.114);
			Color_U = (unsigned char)(RVal * (-0.169) 	- GVal * 0.332 	+ BVal * 0.500 + 128);
			Color_V = (unsigned char)(RVal * 0.5		- GVal * 0.419 	- BVal * 0.0813 + 128);
			YUV_ColorTable[i*20+j].YUVData = (Color_V << 24) | ((Color_Y << 16)) | (Color_U << 8) | (Color_Y);
        }		
    }
}
void TempDisplay(UINT32 u32UVCWidth, UINT32 offset_LCD)
{
    int i,j;
	int sum = 0;
	UINT32 u32start,u32Index0,u32Index1,u32Index2;
	UINT32 start_UVC,  offset_UVC;
    UINT32 *u32Data;	
	
    /* Temperature Average */	
	  u32start =(HEIGHT - W_HEIGHT) / 2 * WIDTH +  (WIDTH - W_WIDTH) / 2;			
	  for(i=0;i<W_HEIGHT;i++)
	  {
		    for(j=0;j<W_WIDTH;j++)					    
			    sum = sum + TDATA[g_TDATA_index][u32start + i* WIDTH + j];
		}	
	  sum = sum / W_HEIGHT / W_WIDTH;
	  
		
	  g_i16DisTemp = sum;
	  g_i16Ready = 1;
	  u32Index0 = sum /100;

	  u32Index1 = (sum /10) % 10;

	  u32Index2 = sum % 10;
		
    /* Temperature display */	
    start_UVC = (u32UVCWidth - 56) / 2 + 64 * u32UVCWidth / 2;	
		u32Data = (UINT32 *)((UINT32)&u32PacketFrameBuffer[u32UvcBufferIndex][0] | BIT31);		
	  for(i = 0;i< 24;i++)
	  {
	  	  offset_UVC = start_UVC + i * u32UVCWidth/2;							
	  	  memcpy((void *)&u32Data[offset_UVC + 0],(void *) &Image[u32Index0][i*8],32);			
	  	  memcpy((void *)&u32Data[offset_UVC + 8],(void *) &Image[u32Index1][i*8],32);				
	  	  memcpy((void *)&u32Data[offset_UVC + 16],(void *) &Image_dot[i*4],16);				
	  	  memcpy((void *)&u32Data[offset_UVC + 20],(void *) &Image[u32Index2][i*8],32);				
    }	
}

VOID Draw_Area(UINT32 extend, UINT32 u32UVCWidth, UINT32 offset_UVC, UINT32 offset_LCD)
{
    UINT32 start_UVC;
	INT j;    	
	
	  /* UVC - Up */		
    if(uvcStatus.FrameIndex == UVC_VGA)
    {
        start_UVC = (((WIDTH - W_HEIGHT) * 7) * (u32UVCWidth) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
			
        for(j=0;j<1 * 7;j++)
        {
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + j] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + j + u32UVCWidth/2] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC - j + 7*W_WIDTH] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC - j + 7*W_WIDTH + 640/2] = 0x80008000;
        }		
    /* UVC - Up Side */
        start_UVC =  (((WIDTH - W_HEIGHT) * 7) * (u32UVCWidth) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
        for(j=0;j<7*2;j++)
        {
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + j * u32UVCWidth/2] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + W_WIDTH * 7 + j * (u32UVCWidth/2)] =0x80008000;		
        }	
    /* UVC - Bottom Side */
        start_UVC = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2) * (u32UVCWidth) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
        for(j=7*2;j>=0;j--)
        {
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC - j * u32UVCWidth/2] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + W_WIDTH * 7 - j * (u32UVCWidth/2)] =0x80008000;	
        }	
    /* UVC - Bottom */	
        start_UVC = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2) * (u32UVCWidth) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
        for(j=0;j<1 * 7;j++)
        {
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + j] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC + j + u32UVCWidth/2] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC - j + 7*W_WIDTH] = 0x80008000;
            u32PacketFrameBuffer[u32UvcBufferIndex][start_UVC - j + 7*W_WIDTH + u32UVCWidth/2] = 0x80008000;
        }				
    }
}

VOID TempCal(int extend)
{
	int i,j,k,l,offset_UVC;
	UINT8 *u8Data;	
    UINT32 *u32Data;	
	UINT32 value,count = 0,index;
	UINT32 u32UVCWidth, u32Width, u32Height;
//    UINT8 u8Sign = 0, hundreds, tens, units;  	
	
    g_i16Ready = 0;
    g_TDATA_index = (g_TDATA_index ^1) & 0x01;
			
    if(uvcStatus.FrameIndex == UVC_VGA)
    {							
        u32Width = 640; 				
        u32Height = 480; 								
    }
    else if(uvcStatus.FrameIndex == UVC_QVGA)
    {							
        u32Width = 320; 				
        u32Height = 240; 								
    }    
    else if(uvcStatus.FrameIndex == UVC_QQVGA)
    {							
        u32Width = 160; 				
        u32Height = 120; 								
    }    	
		u32UVCWidth = u32Width;
		
    offset_UVC = (u32Width - 32);
	
    u32Data = (UINT32 *)( (UINT32)&u32PacketFrameBuffer[u32UvcBufferIndex][0] | BIT31);		
	  for(i=0;i<HEIGHT;i++)
	  { 
		    for(j=0;j<WIDTH;j++)
		    {
			      int index_offset;
			      TDATA[g_TDATA_index][count] = Target[j][i] - 2732;
			      if(TDATA[g_TDATA_index][count] < 0)
			      	TDATA[g_TDATA_index][count] = 0;
			      if(TDATA[g_TDATA_index][count] >= 1200)
			      	TDATA[g_TDATA_index][count] = 1199;
			      count++;
        }
    }
		index = 1;
#if 1	
    u8Data = (UINT8 *)( (UINT32)&u32PacketFrameBuffer[u32UvcBufferIndex][0] | BIT31);		
		for(i=0;i<1024;i++)
		{
			if(TDATA[g_TDATA_index][i] >= 0)			 
				u8Data[index] = u8Data[index] & ~0x1;
			else            	
				u8Data[index] = u8Data[index] | 0x1;
					
			value = abs(TDATA[g_TDATA_index][i]);				
									
			index += 2;
			for(j=9;j>=0;j--)
			{					
				if(value & (1 << j))					
					u8Data[index] = u8Data[index] | 0x1;
							else
					u8Data[index] = u8Data[index] & ~0x1;
							
						index += 2;
			}
		}	
#endif		
    offset_UVC = ((u32Height - 448) * u32Width /4) + ((u32Width - 448) /4);
	if(GetTempDisplay() == 1) {
		TempDisplay(u32UVCWidth, 0);
		Draw_Area(extend, u32UVCWidth, offset_UVC, 0);		
	}
}

int main (void)
{
    WB_UART_T 	uart;
    UINT32 		u32ExtFreq, count = 5;	    
    INT index;   
    sysEnableCache(CACHE_WRITE_BACK);	

    sysSetSystemClock(eSYS_UPLL, 	//E_SYS_SRC_CLK eSrcClk,	
            192000,	   	//UINT32 u32PllKHz, 	
             96000,		  //UINT32 u32SysKHz,
             96000,		  //UINT32 u32CpuKHz,
             96000,		  //UINT32 u32HclkKHz,
             48000);		//UINT32 u32ApbKHz		
	
    u32ExtFreq = sysGetExternalClock();    	/* KHz unit */	
    create_color_table();

	outp32(REG_AHBCLK, inp32(REG_AHBCLK) & ~VPOST_CKE);
				
    VIN_main();	

    sysSetSystemClock(eSYS_UPLL, 	//E_SYS_SRC_CLK eSrcClk,	
            192000,	   	//UINT32 u32PllKHz, 	
             96000,		  //UINT32 u32SysKHz,
             96000,		  //UINT32 u32CpuKHz,
             96000,		  //UINT32 u32HclkKHz,
             48000);		//UINT32 u32ApbKHz		
						 
	N329_InitSensor();
	N329_OpenSensor();
			
	  N329_Interface_init(UART);
		
	  /* Open USB Device */
	  udcOpen();
	  
		/* Init USB Device Setting as Video Class Device */
	  uvcdInit(NULL);		
	  
	// Enable/Disable temperature display
	SetTempDisplay(0);							// UI can be done on firmware/software side
	
    while(count)
		{
        /* Get Image Data */			
	  	  index = StartStreaming(0, 1, 1);
			
	  	  if(index != 0xFF)			
	  	      count--;				
		}	
	
	  /* Init USB Device */	
	  udcInit();
		
	  while(1)
	  {	
        /* Get Image Data */			
	  	  index = StartStreaming(0, 1, 1);
			
	  	  if(index != 0xFF)
	  	  {				
            u32Lock = 1; 					 
			TempCal(g_extend);	
		
			if (usbdStatus.appConnected == 1)
			{								
				uvcdEvent(g_TDATA_index);	
			}
			u32Lock = 0;
	  	  }
	  }	
}
