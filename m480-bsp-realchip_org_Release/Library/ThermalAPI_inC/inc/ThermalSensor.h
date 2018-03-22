#include "MI_XCAM.h"

extern short 			TDATA[2][1024];
extern signed short 	Target[32][32];
extern YUV_COLOR_INFO_T YUV_ColorTable[];
extern RGB_COLOR_INFO_T RGB_ColorPalette[];
// Temperature value display
extern const int 		Image_dot[];
extern const int 		Image[10][192];
/*
*	Application functions
*/
void			M480_InitSensor(void);
void			M480_OpenSensor(void);
void			M480_StartSensor(void);

