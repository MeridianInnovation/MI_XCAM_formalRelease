#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wblib.h"
#include "usbd.h"
#include "videoclass_HTPA32.h"
#include "vin_demo.h"
#include "W55FA93_VideoIn.h"

/* Current Frame Index */
UINT32 u32CurFrameIndex = 1;
UINT32 u32Index = 0;

/* Current Format Index */
UINT32 u32CurFormatIndex = 1;

extern INT32 g_TDATA_index;  
extern __align(4) short TDATA[2][1024];
extern UINT32 u32UvcBufferIndex;

INT32 g_extend;   

extern __align(4) UINT32 u32PacketFrameBuffer[2][640*480/2];

INT8 u8SendThermaldata=0;

/* UVC event */
void uvcdEvent(UINT8 u8index)
{
    int i;
    UINT32 u32GCD;
	    
    if (usbdStatus.appConnected == 1)
    { 		
        if(u32CurFrameIndex != uvcStatus.FrameIndex ||u32CurFormatIndex != uvcStatus.FormatIndex)
        {
            /* Frame Size Changed */
            switch(uvcStatus.FrameIndex)
            {			
                case UVC_QQVGA:
                    u32CurFrameIndex = uvcStatus.FrameIndex;
                    u32CurFormatIndex = uvcStatus.FormatIndex;	
                    
                    u32GCD = GCD(120, OPT_CROP_HEIGHT);						 							 
                    videoinIoctl(VIDEOIN_IOCTL_VSCALE_FACTOR,
                        eVIDEOIN_PACKET,			//272/480
                        120/u32GCD,
                        OPT_CROP_HEIGHT/u32GCD);	
							
                    u32GCD = GCD(160, OPT_CROP_WIDTH);																
                    videoinIoctl(VIDEOIN_IOCTL_HSCALE_FACTOR,
                        eVIDEOIN_PACKET,			//364/640
                        160/u32GCD,
                        OPT_CROP_WIDTH/u32GCD);	
				                 
                    /* Set Pipes stride */				
                    videoinIoctl(VIDEOIN_IOCTL_SET_STRIDE,										
                        160,				// packet stride
                        OPT_ENCODE_WIDTH,		// Planar stride
                        0); 
                    break;										
                case UVC_QVGA:						
                    u32CurFrameIndex = uvcStatus.FrameIndex;
                    u32CurFormatIndex = uvcStatus.FormatIndex;	                    
                    u32GCD = GCD(240, OPT_CROP_HEIGHT);						 							 
                    videoinIoctl(VIDEOIN_IOCTL_VSCALE_FACTOR,
                        eVIDEOIN_PACKET,			//272/480
                        240/u32GCD,
                        OPT_CROP_HEIGHT/u32GCD);								
                    u32GCD = GCD(320, OPT_CROP_WIDTH);																
                    videoinIoctl(VIDEOIN_IOCTL_HSCALE_FACTOR,
                        eVIDEOIN_PACKET,			//364/640
                        320/u32GCD,
                        OPT_CROP_WIDTH/u32GCD);	
				                 
                    /* Set Pipes stride */				
                    videoinIoctl(VIDEOIN_IOCTL_SET_STRIDE,										
                        320,				// packet stride
                        OPT_ENCODE_WIDTH,		// Planar stride
                        0);        	
                    break;			
                case UVC_VGA:		
                    u32CurFrameIndex = uvcStatus.FrameIndex;
                    u32CurFormatIndex = uvcStatus.FormatIndex;	
                    
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
                        640,				// packet stride
                        0,		// Planar stride
                        0);     
                    break;													
            } 				
        }			
	 
        if(u8SendThermaldata) {   
            /* Send Image */    	
            uvcdSendImage((UINT32)&u32PacketFrameBuffer[u32UvcBufferIndex][0], uvcStatus.MaxVideoFrameSize, uvcStatus.StillImage);
            /* Wait for Complete */ 			
            while(!uvcdIsReady());		 
            u8SendThermaldata = 0;    	
        }
        u8SendThermaldata = 1;	
    }  
}
