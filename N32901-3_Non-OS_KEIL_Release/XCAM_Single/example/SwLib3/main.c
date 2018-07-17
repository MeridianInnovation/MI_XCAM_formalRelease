#include "ThermalSensor.h"
#include "w55fa93_vpost.h"
#include "videoclass_HTPA32.h"

VOID TempCal(int extend);
void uvcdEvent(UINT8 u8index);

extern __align(4) INT32 u32Data[2][0x25800];
#ifdef __PANEL__
extern __align(4) INT32 u32FrameData[];
#endif
extern INT32 g_i16DisTemp, g_i16Ready, g_TDATA_index;  

extern YUV_COLOR_INFO_T YUV_ColorTable[];
#ifdef COLORPALETTE0_WIDERANGE
	extern RGB_COLOR_INFO_T RGB_ColorPalette[COLORPALETTEADAPTIVESIZE];
#else
	extern RGB_COLOR_INFO_T RGB_ColorPalette[COLORPALETTESIZE];
#endif
extern FRAMEPOIS framePOIs;
extern signed short TAN;		// Ambient Temperature = TAN - 2732; 

LCDFORMATEX lcdInfo;

int main (void)
{    
    INT 		index;   
	UINT8 		u8SendThermaldata = 0;

    sysEnableCache(CACHE_WRITE_BACK);	
    sysSetSystemClock(eSYS_UPLL, 	//E_SYS_SRC_CLK eSrcClk,	
            192000,	   				//UINT32 u32PllKHz, 	
             96000,		  			//UINT32 u32SysKHz,
             96000,		  			//UINT32 u32CpuKHz,
             96000,		  			//UINT32 u32HclkKHz,
             48000);				//UINT32 u32ApbKHz		
			 
    Create_color_table(RGB_ColorPalette,YUV_ColorTable);
	
#ifdef __PANEL__	
    /* Init Panel */	
    lcdInfo.ucVASrcFormat = DRVVPOST_FRAME_YCBYCR;	
    lcdInfo.nScreenWidth = PANEL_WIDTH;	
    lcdInfo.nScreenHeight = PANEL_HEIGHT;
    vpostLCMInit(&lcdInfo, (UINT32*)((UINT32)u32FrameData | 0x80000000));		
#else
	outp32(REG_AHBCLK, inp32(REG_AHBCLK) & ~VPOST_CKE);
#endif

#ifdef __PANEL__	
    /* Init Frame Buffer */
    for(i=0;i<38400;i++)
        u32FrameData[i] = 0x80008000;//0x80FF80FF;
#endif			

	// UART/HUART interface initialization
	N329_Interface_init(HUART);	

	N329_InitSensor();
	N329_OpenSensor();
	
	/* Open USB Device */
	udcOpen();

	/* Init USB Device Setting as Video Class Device */
	uvcdInit(NULL);		

	/* Init USB Device */	
	udcInit();

	g_extend = 192/2/WIDTH;
	
	// Enable/Disable temperature display
	SetTempDisplay(1);
	
	// Set to show particular pixel values
	//SetTargetPixelIndex(WIDTH-1);
	//SetTargetPixelIndex(WIDTH*HEIGHT - (WIDTH-1));
	
	while(1)
	{		
		// Reset POI records for every frame
		ResetFramePOIs();
		
		// Get Image Data 			
		index = StartStreaming(0, 1, 1);

		if(index != 0xFF)
		{				
			if (usbdStatus.appConnected == 1)
			{
				TempCal(g_extend);
				if(u8SendThermaldata)
				{
					uvcdSendImage((UINT32)&TDATA[g_TDATA_index], 2048, uvcStatus.StillImage);						
					// Wait for Complete  			
					while(!uvcdIsReady());		 
					u8SendThermaldata = 0;
					while(!uvcdIsReady());		
				}
				uvcdEvent(g_TDATA_index);	
				u8SendThermaldata = 1;	
			}
		}
	}
}

/**
 * Function for second temperature display;
 * @param xDisp xCoord for temperature display
 * @param yDisp yCoord for temperature display
 * @param x xCoord for temperature (top left corner)
 * @param y yCoord for temperature (top left corner)
 **/
void TempDisplay(UINT32 u32UVCWidth, UINT32 offset_LCD, float xDisp, float yDisp, UINT32 x, UINT32 y) //x-y goes 0-31
{
	int i,j;
	int sum = 0;
	UINT32 u32start,u32Index0,u32Index1,u32Index2,u32Index3;
	UINT32 start_UVC, start_LCD, offset_UVC, offset_LCD_tmp;

	/* Temperature Average */	
	if(GetTargetPixelIndex() == -1) {		
	  u32start = y * WIDTH + x; 		//starting location
	  for(i=0;i<W_HEIGHT;i++) 			//get sum of the pixels in box
	  {
			for(j=0;j<W_WIDTH;j++)					    
				sum = sum + TDATA[g_TDATA_index][u32start + i* WIDTH + j]; //TDATA[0][ 0-31*31] from what I can see
		}	
	  sum = sum / W_HEIGHT / W_WIDTH;
	}
	else
		sum = GetTemp(GetTargetPixelIndex()%WIDTH,GetTargetPixelIndex()/WIDTH);
		
	// Single pixel
	//sum = GetTemp(31,0);
	//sum = EEPROMInfo.TableNumberSensor;
	
	
// Left Temp display (Avg/Max)
#ifdef MIN_MAX_TEMP_DISPLAY
	sum = framePOIs.maxTemPixel.Tmp;
#endif
	
	g_i16DisTemp = sum;
	g_i16Ready = 1;
	u32Index0 = (sum /1000);
	u32Index1 = (sum /100) % 10;
	u32Index2 = (sum /10) % 10;
	u32Index3 = sum % 10;

	/* Temperature display */	
	if(uvcStatus.FrameIndex == UVC_640) //display location for phone app
		start_UVC = ((yDisp + 1) * 7 * 640) + ((xDisp + 7) * 7);
	//display is 2 pixels high, 4 long
	else
		start_UVC = (yDisp * 96 * 6) + (96 * (xDisp / 32.0)); //handles display location for amcam
	//display is 4 pixels high; 9.5 long; xDisp, yDisp, xTemp, yTemp

	start_LCD = (2*WIDTH*3 - 56) / 2;	
		
	for(i = 0;i< 24;i++)
	{
		if(uvcStatus.FrameIndex == UVC_640)
		  offset_UVC = start_UVC + i * 640/2;			
		else
			  offset_UVC = start_UVC + i * u32UVCWidth/2 + 20 * 3;	

		offset_LCD_tmp = start_LCD + 28 + i * PANEL_WIDTH/2 + offset_LCD;												
				
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 0],(void *) &Image[u32Index0][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 0],(void *) &Image[u32Index0][i*8],32);
#endif		
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 8],(void *) &Image[u32Index1][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 8],(void *) &Image[u32Index1][i*8],32);
#endif		
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 16],(void *) &Image[u32Index2][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 16],(void *) &Image[u32Index2][i*8],32);	
#endif			
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 24],(void *) &Image_dot[i*4],16);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 24],(void *) &Image_dot[i*4],16);		
#endif		
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 28],(void *) &Image[u32Index3][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 28],(void *) &Image[u32Index3][i*8],32);	
#endif			
	}	
	
	
// Right temperature display (Min)
#ifdef MIN_MAX_TEMP_DISPLAY
	sum = framePOIs.minTemPixel.Tmp;
	u32Index0 = (sum /1000);
	u32Index1 = (sum /100) % 10;
	u32Index2 = (sum /10) % 10;
	u32Index3 = sum % 10;
	for(i = 0;i< 24;i++)
	{
		if(uvcStatus.FrameIndex == UVC_640)
		  offset_UVC = start_UVC + i * 640/2;			
		else
			  offset_UVC = start_UVC + i * u32UVCWidth/2;	

		offset_LCD_tmp = start_LCD + 28 + i * PANEL_WIDTH/2 + offset_LCD;												
				
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 0],(void *) &Image[u32Index0][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 0],(void *) &Image[u32Index0][i*8],32);
#endif		
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 8],(void *) &Image[u32Index1][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 8],(void *) &Image[u32Index1][i*8],32);
#endif		
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 16],(void *) &Image[u32Index2][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 16],(void *) &Image[u32Index2][i*8],32);	
#endif			
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 24],(void *) &Image_dot[i*4],16);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 24],(void *) &Image_dot[i*4],16);		
#endif		
		memcpy((void *)&u32Data[g_TDATA_index][offset_UVC + 28],(void *) &Image[u32Index3][i*8],32);
#ifdef __PANEL__				
		memcpy((void *)&u32FrameData[offset_LCD_tmp + 28],(void *) &Image[u32Index3][i*8],32);	
#endif			
	}	
#endif

}

/**
 * Draws a box
 * @param x xCoord of where to draw box(top left) 32x32 COORDINATES ONLY; 0-31
 * @param y yCoord of where to draw box(top left)
 **/
VOID Draw_Area(UINT32 extend, UINT32 u32UVCWidth, UINT32 offset_UVC, UINT32 offset_LCD, float x, float y)
{
    UINT32 start_UVC;
	  INT j;
	  /* UVC - Up */		
    if(uvcStatus.FrameIndex == UVC_640) //phone
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * 7) * (640) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
		start_UVC = (y * 7 * 640) + (x * 7) + offset_UVC;
        for(j=0;j<1 * 7;j++)
        {
            u32Data[g_TDATA_index][start_UVC + j] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + j + 640/2] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + 7*W_WIDTH] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + 7*W_WIDTH + 640/2] = 0x80008000;
        }				
    }
    else //amcam
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * extend) * u32UVCWidth + (WIDTH - W_WIDTH) * extend)/2;
				start_UVC = (y * 96 * 6) + (96 * (x / 32.0)); //calculate height + width (y * 96 * 2)
        for(j=0;j<1 * extend;j++)
        {
            u32Data[g_TDATA_index][start_UVC + j] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + j + u32UVCWidth/2] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + extend*W_WIDTH] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + extend*W_WIDTH + u32UVCWidth/2] = 0x80008000;
        }				
    }			
#ifdef __PANEL__ //skipped
    /* LCD - Up */
    start_LCD = (((WIDTH - W_HEIGHT) * 3) * (PANEL_WIDTH) + (WIDTH - W_WIDTH) * 3)/2;
    for(j=0;j<3;j++)
    {
        u32FrameData[start_LCD + offset_LCD + j] = 0x80008000;
		    u32FrameData[start_LCD + offset_LCD - j + 3*W_WIDTH] = 0x80008000;
        u32FrameData[start_LCD + offset_LCD + j - PANEL_WIDTH/2] = 0x80008000;
		    u32FrameData[start_LCD + offset_LCD - j + 3*W_WIDTH - PANEL_WIDTH/2] = 0x80008000;			
		}
#endif
    /* UVC - Up Side */
    if(uvcStatus.FrameIndex == UVC_640)
    {
        //start_UVC =  (((WIDTH - W_HEIGHT) * 7) * (640) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
		start_UVC = (y * 7 * 640) + (x * 7) + offset_UVC;
        for(j=0;j<7*2;j++)
        {
            u32Data[g_TDATA_index][start_UVC + j * 640/2] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + W_WIDTH * 7 + j * (640/2)] =0x80008000;		
        }	
    }
    else
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * extend) * u32UVCWidth + (WIDTH - W_WIDTH) * extend)/2;
				start_UVC = (y * 96 * 6) + (96 * (x / 32.0)); //calculate height + width (top | |)
        for(j=0;j<extend*2;j++)
        {
            u32Data[g_TDATA_index][start_UVC + j * (extend*WIDTH)] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + W_WIDTH * extend + j * (extend*WIDTH)] =0x80008000;
        }			
    }		
		
    /* UVC - Bottom Side */
    if(uvcStatus.FrameIndex == UVC_640)
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2) * (640) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
				start_UVC = ((y + W_HEIGHT) * 7 * 640) + (x * 7) + offset_UVC;
        for(j=7*2;j>=0;j--)
        {
            u32Data[g_TDATA_index][start_UVC - j * 640/2] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + W_WIDTH * 7 - j * (640/2)] =0x80008000;		
        }	
    }
    else
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * extend + W_HEIGHT *extend*2) * u32UVCWidth + (WIDTH - W_WIDTH) * extend)/2;
				start_UVC = ((y + W_HEIGHT) * 96 * 6) + (96 * (x / 32.0));//bottom | |
        for(j=extend*2;j>=0;j--)
        {
            u32Data[g_TDATA_index][start_UVC - j * (extend*WIDTH)] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + W_WIDTH * extend - j * (extend*WIDTH)] =0x80008000;
        }			
    }	
#ifdef __PANEL__ //skipped
	  /* LCD - Up Side */
	  start_LCD = (((WIDTH - W_HEIGHT) * 3) * (PANEL_WIDTH) + (WIDTH - W_WIDTH) * 3)/2 + offset_LCD;
	  for(j=0;j<6;j++)
	  {		
   		 u32FrameData[start_LCD + j * (PANEL_WIDTH/2)] = 0x80008000;
		   u32FrameData[start_LCD + W_WIDTH * 3 + j * (PANEL_WIDTH/2)] =0x80008000;
	  }		
    /* LCD - Bottom Side */		
		start_LCD = (((WIDTH - W_HEIGHT) * 3 + W_HEIGHT *3*2) * (PANEL_WIDTH) + (WIDTH - W_WIDTH) * 3)/2 + offset_LCD;	 
	  for(j=5;j>=0;j--)
	  {		
   		 u32FrameData[start_LCD - j * (PANEL_WIDTH/2)] = 0x80008000;
		   u32FrameData[start_LCD + W_WIDTH * 3 - j * (PANEL_WIDTH/2)] =0x80008000;
	  }				
#endif		
    /* UVC - Bottom */	
    if(uvcStatus.FrameIndex == UVC_640)
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2) * (640) + (WIDTH - W_WIDTH) * 7)/2 + offset_UVC;
				start_UVC = ((y + W_HEIGHT) * 7 * 640) + (x * 7) + offset_UVC;
        for(j=0;j<1 * 7;j++)
        {
            u32Data[g_TDATA_index][start_UVC + j] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + j + 640/2] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + 7*W_WIDTH] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + 7*W_WIDTH + 640/2] = 0x80008000;
        }				
    }
    else		
    {
        //start_UVC = (((WIDTH - W_HEIGHT) * extend + W_HEIGHT *extend*2) * u32UVCWidth + (WIDTH - W_WIDTH) * extend)/2;
				start_UVC = ((y + W_HEIGHT) * 96 * 6) + (96 * (x / 32.0));
        for(j=0;j<1 * extend;j++)
        {
            u32Data[g_TDATA_index][start_UVC + j] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC + j + u32UVCWidth/2] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + extend*W_WIDTH] = 0x80008000;
            u32Data[g_TDATA_index][start_UVC - j + extend*W_WIDTH + u32UVCWidth/2] = 0x80008000;
        }		
    }
#ifdef __PANEL__ //skipped
	  /* LCD - Bottom*/
	  start_LCD = (((WIDTH - W_HEIGHT) * 3 + W_HEIGHT *3*2) * (PANEL_WIDTH) + (WIDTH - W_WIDTH) * 3)/2 + offset_LCD;
	  for(j=0;j<3;j++)
	  {
    		u32FrameData[start_LCD + j] = 0x80008000;
		    u32FrameData[start_LCD - j + 3*W_WIDTH] = 0x80008000;			
    		u32FrameData[start_LCD + j + PANEL_WIDTH/2] = 0x80008000;
		    u32FrameData[start_LCD - j + 3*W_WIDTH + PANEL_WIDTH/2] = 0x80008000;
    }
#endif
}


VOID TempCal(int extend)
{
	  int i,j,k,l,offset_LCD,offset_UVC;
	  unsigned int index,min,max;
	  UINT32 value,count = 0;
	  UINT32 u32UVCWidth;
	  offset_LCD = ((PANEL_HEIGHT - (3*WIDTH*2)) * PANEL_WIDTH /4) + ((PANEL_WIDTH - (3*HEIGHT*2)) /4);
	  offset_UVC = ((480 - 448) * 640 /4) + ((640 - 448) /4);
	
	  u32UVCWidth = extend*WIDTH*2;
      g_i16Ready = 0;
	  g_TDATA_index = (g_TDATA_index ^1) & 0x01;
	  min = GetFramePOIs().minTemPixel.Tmp;
	  max = GetFramePOIs().maxTemPixel.Tmp;
	  for(i=0;i<HEIGHT;i++)
	  { 
		    for(j=0;j<WIDTH;j++)
		    {
			      int index_offset;
			      TDATA[g_TDATA_index][count] = Target[j][i] - 2732;
				  
#ifdef COLOR_ADAPTIVE
				value = TDATA[g_TDATA_index][count];
				index = abs((signed int)value - min) * (COLORTABLESIZE - 1) / abs(max - min);
#else
	#ifdef COLORPALETTE0_WIDERANGE
				value  =  TDATA[g_TDATA_index][count];
	#else
				value  =  TDATA[g_TDATA_index][count] + 600; 
	#endif
				index = value;
#endif			
				// entry boundary check
				if(index >= COLORTABLESIZE - 1)
					index = COLORTABLESIZE - 1;
				else if(index <= 0)
					index = 0;
					
				value = YUV_ColorTable[index].YUVData;
				count++;
				
				if(uvcStatus.FrameIndex == UVC_640)
				{
					index_offset = i*7*640 + offset_UVC + j*7;
					
					for(k=0;k<14;k++)
						for(l=0;l<7;l++)
							u32Data[g_TDATA_index][index_offset + k*640/2 + l] = value;				
				}
				else			
				{
					index_offset = i*extend*u32UVCWidth + j*extend;
					/* UVC */	
					for(k=0;k<extend*2;k++)
						for(l=0;l<extend;l++)
							u32Data[g_TDATA_index][index_offset + k*extend*WIDTH + l] = value;
					
				}
#ifdef __PANEL__					
				/* LCD */		
				index_offset = i*3*PANEL_WIDTH + offset_LCD + j*3;
				for(k=0;k<6;k++){
					for(l=0;l<3;l++){
						u32FrameData[index_offset + k*PANEL_WIDTH/2 + l] = value;
					}
				}
#endif						
			}
		}
	if(GetTempDisplay() == 1) {
		TempDisplay(u32UVCWidth, offset_LCD, 0, 0, 15, 15);	
		Draw_Area(extend, u32UVCWidth, offset_UVC, offset_LCD, 15, 15);
	}
}
