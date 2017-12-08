#include "wblib.h"
#include "w55fa93_reg.h"

__packed  typedef struct{
	int R;
	int G;
	int B;
}RGB_COLOR_INFO_T;

__packed  typedef struct{
	int Y;
	int U;
	int V;
	UINT32 Data;
}YUV_COLOR_INFO_T;
#define VERSION		0x12345678
extern YUV_COLOR_INFO_T YUV_ColorTable[];

extern RGB_COLOR_INFO_T RGB_ColorTable[];
extern RGB_COLOR_INFO_T RGB_ColorTable0[];
extern RGB_COLOR_INFO_T RGB_ColorTableRef[];

#define PANEL_WIDTH		320					/* PANEL Width (Raw data output width for Panel Test) */
#define PANEL_HEIGHT	240     			/* PANEL Height (Raw data output height for Panel Test) */

void uvcdEvent(UINT8 u8index);
