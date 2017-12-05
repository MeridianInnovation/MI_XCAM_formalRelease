/***************************************************************************
 *                                                                         									     *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              					     *
 *                                                                         									     *
 ***************************************************************************/

#include <stdio.h>
#include "wblib.h"
#include "MI_XCAM.h"
#include "demo.h"

UARTDEV_T UART0;	/*High speed */
UARTDEV_T* pUART0;
extern INT32 g_i16TempAvg, g_i16Ready,g_TDATA_index; 
extern short TDATA[2][1024];

void UartDataValid_Handler(UINT8* buf, UINT32 u32Len)
{
    PUINT32 pu32Cmd;
    UINT32 i;

    if(u32Len == 8)
    {
        pu32Cmd = (PUINT32) buf;
        if(pu32Cmd[0] == 0x444D4356)
        {
            if(pu32Cmd[1] == 0x4756413D)
            {	
                sysprintf("TempAvg %04X\n",g_i16TempAvg);
                pUART0->UartTransfer((char *)&g_i16TempAvg,  2);				
            }
            else if(pu32Cmd[1] == 0x4154533D)
            {	
                sysprintf("Status %d\n",g_i16Ready);
                pUART0->UartTransfer((char *)&g_i16Ready,  2);
            }	
            else if(pu32Cmd[1] == 0x5345523D)
            {	
                INT16 Buffer[2];
                sysprintf("Resolution %d x %d\n",WIDTH, HEIGHT);
                Buffer[0] = WIDTH;
                Buffer[1] = HEIGHT;
                pUART0->UartTransfer((char *)Buffer,  4);				
            }		
            else if(pu32Cmd[1] == 0x5245563D)
            {	
                UINT32 u32Ver;
                sysprintf("Version %X\n",VERSION);
                u32Ver = VERSION;
                pUART0->UartTransfer((char *)&u32Ver,  4);				
            }			
            else if(pu32Cmd[1] == 0x5441443D)
            {	
                sysprintf("Data\n");
                for(i=0;i<WIDTH*HEIGHT;i++)
                {
                    sysprintf("%2X ",TDATA[((g_TDATA_index^1) &0x01)][i]);	
                    if(((i+1) % WIDTH) == 0)
                        sysprintf("\n");	
                }
                pUART0->UartTransfer((char *)&TDATA[(g_TDATA_index ^ 1) &0x01],  WIDTH*HEIGHT);
            }						
        }
    }
}

int HUART_init(void)
{
    WB_UART_T uart;
    UINT32 u32ExtFreq;
    u32ExtFreq = sysGetExternalClock();
	
    register_uart_device(0, &UART0);
    pUART0 = &UART0;
    pUART0->UartPort(0);
    uart.uiFreq = u32ExtFreq*1000;
    uart.uiBaudrate = 115200;
    uart.uiDataBits = WB_DATA_BITS_8;
    uart.uiStopBits = WB_STOP_BITS_1;
    uart.uiParity = WB_PARITY_NONE;
    uart.uiRxTriggerLevel = LEVEL_8_BYTES;
    pUART0->UartInitialize(&uart);

    pUART0->UartInstallcallback(0, UartDataValid_Handler);	
   
    pUART0->UartEnableInt(UART_INT_RDA);	
    pUART0->UartEnableInt(UART_INT_RDTO);
    sysSetLocalInterrupt(ENABLE_IRQ); 		
    return 0;
} /* end main */
