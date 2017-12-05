#include <stdio.h>
#include <string.h>
#include "M480.h"

//#define TRANSFER_BUFFER  0x0C000 	/* Minimum Transfer buffer is 0x4600 for VGA */
#define TRANSFER_BUFFER  	32*32*4  //32*32
#define WIDTH	 	  		32
#define HEIGHT 				32
#define RECT_SIZE 			14
#define REF_COLOR_NUM		60
#define TOTAL_COLOR_NUM		1200
#define COLOR_OFFSET 		600
#define VERSION				0x12345678

extern signed short Target[32][32];
extern const int Image_dot[];
extern const int Image[10][192];
extern uint32_t volatile g_extend;
int32_t thermal_sensor_init(void);
void draw_init(void);
//int32_t StartStreaming(int Mode,char Temps,char Stream);
void UVC_Transfer_Init(void);
void draw_display(uint32_t u32Count, uint32_t temp_avg);

typedef struct{
	unsigned int YUVData;
}YUV_COLOR_INFO_T;

typedef struct{
	unsigned char R;
	unsigned char G;
	unsigned char B;
}RGB_COLOR_INFO_T;

extern YUV_COLOR_INFO_T YUV_ColorTable[];
extern const RGB_COLOR_INFO_T RGB_ColorTable0[];
