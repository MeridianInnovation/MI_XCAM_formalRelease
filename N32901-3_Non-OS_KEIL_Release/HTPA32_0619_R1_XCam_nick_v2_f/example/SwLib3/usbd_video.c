#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wblib.h"
#include "MI_XCAM.h"
#include "usbd.h"
#include "videoclass_HTPA32.h"

/* Current Frame Index */
UINT32 u32CurFrameIndex = 1;
UINT32 u32Index = 0;

/* Current Format Index */
UINT32 u32CurFormatIndex = 1;

__align(4) UINT32 u32Data[2][0x25800];

#ifdef __PANEL__
__align(4) UINT32 u32FrameData[0x25800];
#endif

extern INT32 g_TDATA_index;  
extern __align(4) short TDATA[2][1024];

INT32 g_extend;   

/* UVC event */
void uvcdEvent(UINT8 u8index)
{
    int i;
    if (usbdStatus.appConnected == 1)
    { 						
        if(u32CurFrameIndex != uvcStatus.FrameIndex ||u32CurFormatIndex != uvcStatus.FormatIndex)
        {
            /* Frame Size Changed */
            switch(uvcStatus.FrameIndex)
            {										
                case UVC_192:
                    u32CurFrameIndex = uvcStatus.FrameIndex;
                    u32CurFormatIndex = uvcStatus.FormatIndex;	
                    g_extend = 192/2/WIDTH;
                    break;										
                case UVC_320:								
                    u32CurFrameIndex = uvcStatus.FrameIndex;
                    u32CurFormatIndex = uvcStatus.FormatIndex;	
                    g_extend = 320/2/WIDTH;		
                    break;			
                case UVC_640:						
                    for(i=0;i<153600;i++)
										{
                        u32Data[0][i] = 0x80008000;//0x80FF80FF;
                        u32Data[1][i] = 0x80008000;//0x80FF80FF;
											
										}
                    u32CurFrameIndex = uvcStatus.FrameIndex;
                    u32CurFormatIndex = uvcStatus.FormatIndex;						
                    g_extend = 448/2/WIDTH;			
                    break;													
            } 				
        }			
        /* Send Image */	
        uvcdSendImage((UINT32)&u32Data[u8index], uvcStatus.MaxVideoFrameSize, uvcStatus.StillImage);
		

    }  
}
