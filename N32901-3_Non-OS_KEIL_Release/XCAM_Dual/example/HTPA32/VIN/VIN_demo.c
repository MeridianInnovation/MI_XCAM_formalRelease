/***************************************************************************
 *                                                                         									     *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              					     *
 *                                                                         									     *
 ***************************************************************************/

#include <stdio.h>
#include "wblib.h"
#include "W55FA93_GPIO.h"
#include "W55FA93_VideoIn.h"

#include "vin_demo.h"

__align(4) UINT32 u32PacketFrameBuffer[2][640*480/2] = {0x80008000};		//Keep 720*480*2 RGB565 frame buffer

volatile UINT32 g_u32FrameCount = 0;
UINT32 u32UvcBufferIndex=0; /* 0 means buffer is clean */
//UINT32 u32VideoInIdx = 0;
UINT32 u32Lock = 0;

void VideoIn_InterruptHandler(void)
{
	
	  if(u32Lock)
		  return;
    switch(u32UvcBufferIndex)
    {//Current Frame
        case 0:		
            videoinIoctl(VIDEOIN_IOCTL_SET_BUF_ADDR,
                eVIDEOIN_PACKET,			
								0, 					
								(UINT32)&u32PacketFrameBuffer[0][0] + UVC_OFFSET);								
				
            u32UvcBufferIndex = 1;  				
            break; 	 				
        case 1:		
            videoinIoctl(VIDEOIN_IOCTL_SET_BUF_ADDR,
                eVIDEOIN_PACKET,			
                0, 					
                (UINT32)&u32PacketFrameBuffer[1][0] + UVC_OFFSET);					
					
            u32UvcBufferIndex = 0; 
            break; 	 				
    }
    g_u32FrameCount = g_u32FrameCount+1;
}
UINT32 VideoIn_GetCurrFrameCount(void)
{
    return g_u32FrameCount;
}
void VideoIn_ClearFrameCount(void)
{
    g_u32FrameCount = 0;
}


IQ_S SensorIQ = {0};
IQ_S* pSensorIQ;

int VIN_main(void)
{
	// Min : 86k (0.35W), Max: 200k(0.5W)
    sysSetSystemClock(eSYS_UPLL, 	//E_SYS_SRC_CLK eSrcClk,	
        86000,			//UINT32 u32PllKHz, 	
        86000,			//UINT32 u32SysKHz,
        86000/2,		//UINT32 u32CpuKHz,
        86000/4,		//UINT32 u32HclkKHz,
        86000/4);	//UINT32 u32ApbKHz				  
	
    pSensorIQ = &SensorIQ; 

    outp32(REG_GPCFUN, inp32(REG_GPCFUN) & 0x0000FFFF | 0x55550000);
    Smpl_OV7740();	
		
   	return 0;
    	
} /* end main */



