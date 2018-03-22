/***************************************************************************
 *                                                                         									     *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              					     *
 *                                                                         									     *
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include "M480.h"
#include "ThermalSensor.h"

extern int g_i16DisTemp, g_i16Ready,g_TDATA_index; 
extern short TDATA[2][1024];

void UartDataValid_Handler(uint8_t* buf, uint32_t u32Len);

void UartDataValid_Handler(uint8_t* buf, uint32_t u32Len)
{
    uint32_t *pu32Cmd;
    uint32_t i;

    if(u32Len == 8)
    {
        pu32Cmd = (uint32_t *) buf;
        if(pu32Cmd[0] == 0x444D4356)
        {
            if(pu32Cmd[1] == 0x4756413D)
            {	
                printf("TempAvg %04X\n",g_i16DisTemp);
                UART_Write(UART1,(uint8_t *)&g_i16DisTemp,  2);				
            }
            else if(pu32Cmd[1] == 0x4154533D)
            {	
                printf("Status %d\n",g_i16Ready);
                UART_Write(UART1,(uint8_t *)&g_i16Ready,  2);
            }	
            else if(pu32Cmd[1] == 0x5345523D)
            {	
                int16_t Buffer[2];
                printf("Resolution %d x %d\n",WIDTH, HEIGHT);
                Buffer[0] = WIDTH;
                Buffer[1] = HEIGHT;
                UART_Write(UART1,(uint8_t *)Buffer,  4);				
            }		
            else if(pu32Cmd[1] == 0x5245563D)
            {	
                uint32_t u32Ver;
                printf("Version %X\n",VERSION);
                u32Ver = VERSION;
                UART_Write(UART1, (uint8_t *)&u32Ver,  4);				
            }			
            else if(pu32Cmd[1] == 0x5441443D)
            {	
                printf("Data\n");
                for(i=0;i<WIDTH*HEIGHT;i++)
                {
                    printf("%2X ",TDATA[((g_TDATA_index^1) &0x01)][i]);	
                    if(((i+1) % WIDTH) == 0)
                        printf("\n");	
                }
                UART_Write(UART1,(uint8_t *)&TDATA[(g_TDATA_index ^ 1) &0x01],  WIDTH*HEIGHT*2);
            }						
        }
    }
}

