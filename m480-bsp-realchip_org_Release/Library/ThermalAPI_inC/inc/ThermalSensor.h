#include "MI_XCAM.h"

extern short 			TDATA[2][1024];
extern signed short 	Target[32][32];
extern YUV_COLOR_INFO_T YUV_ColorTable[];
extern RGB_COLOR_INFO_T RGB_ColorPalette[];
// Temperature value display
extern const int 		Image_dot[];
extern const int 		Image[10][192];

//#define COLOR_ADAPTIVE							// Work well for adaptive color palette
#ifdef COLOR_ADAPTIVE								// If color_adaptive is defined, change color palette here
	#define	COLORPALETTE_BW_ADAPTIVE	
#else
	#define COLORPALETTE0
	//#define COLORPALETTE0_WIDERANGE					// Color palette please refers to Table_UVC.c
#endif

/*
*	Application functions
*/
void			M480_InitSensor(void);
void			M480_OpenSensor(void);
void			M480_StartSensor(void);

