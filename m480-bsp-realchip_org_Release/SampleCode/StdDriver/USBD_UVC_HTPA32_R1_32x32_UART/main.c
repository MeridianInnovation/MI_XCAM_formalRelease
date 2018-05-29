#include "ThermalSensor.h"

uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;
volatile uint32_t g_u32comRbytes0 = 0;
volatile uint32_t g_u32comRhead0  = 0;
volatile uint32_t g_u32comRtail0  = 0;

void UART_TEST_HANDLE(void);

extern uint32_t volatile g_extend;
extern uint32_t 	g_i16DisTemp, g_i16Ready, g_TDATA_index; 
extern YUV_COLOR_INFO_T YUV_ColorTable[1200];
extern signed short Target[32][32];
extern int time_no;
extern void UartDataValid_Handler(uint8_t* buf, uint32_t u32Len);
extern void UVC_Transfer_Init(void);

void SYS_Init(void)
{
	int i;
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(48000000);
    CLK->PCLKDIV = (CLK_PCLKDIV_PCLK0DIV2 | CLK_PCLKDIV_PCLK1DIV2); // PCLK divider set 2

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Select USBD */
    SYS->USBPHY = (SYS->USBPHY & ~SYS_USBPHY_USBROLE_Msk) | SYS_USBPHY_USBEN_Msk | SYS_USBPHY_SBO_Msk;

    /* Select IP clock source */
    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_USBDIV_Msk) | CLK_CLKDIV0_USB(4);
		
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(UART1_MODULE);
		
    /* Select UART clock source is HXT */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART1SEL_Msk) | (0x0 << CLK_CLKSEL1_UART1SEL_Pos);
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk) | (0x0 << CLK_CLKSEL1_UART0SEL_Pos);

    /* Enable IP clock */
    CLK_EnableModuleClock(HSUSBD_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(I2C0_MODULE);

 
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);

    /* Set I2C0 multi-function pins */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA4MFP_Msk | SYS_GPA_MFPL_PA5MFP_Msk)) |
                    (SYS_GPA_MFPL_PA4MFP_I2C0_SDA | SYS_GPA_MFPL_PA5MFP_I2C0_SCL);	

 
   /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
      /* Set PD multi-function pins for UART0 RXD and TXD */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk | SYS_GPD_MFPL_PD3MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD2MFP_UART0_RXD | SYS_GPD_MFPL_PD3MFP_UART0_TXD);

   /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

  /* Set PA multi-function pins for UART1 TXD, RXD, CTS and RTS */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);
    SYS->GPA_MFPL |= (0x8 << SYS_GPA_MFPL_PA2MFP_Pos) | (0x8 << SYS_GPA_MFPL_PA3MFP_Pos);

	SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;    /* select HSUSBD */
    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSUSBEN_Msk;
    for (i=0; i<0x1000; i++);      // delay > 10 us
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
}

void UART0_Init(void)
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);    
	
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void UART1_Init(void)
{
    /* Reset IP */
    SYS_ResetModule(UART1_RST);    
	
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART1, 115200);
}

void UART1_IRQHandler(void)
{
    UART_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST_HANDLE()
{
    uint8_t u8InChar = 0xFF;
    uint32_t u32IntSts = UART1->INTSTS;

    if(u32IntSts & UART_INTSTS_RDAINT_Msk) {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART1)) {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART1);
             /* Check if buffer full */
            if(g_u32comRbytes < RXBUFSIZE) {
                /* Enqueue the character */
                g_u8RecData[g_u32comRtail] = u8InChar;
                g_u32comRtail = (g_u32comRtail == (RXBUFSIZE - 1)) ? 0 : (g_u32comRtail + 1);
                g_u32comRbytes++;
            }
        }
    }	
}

int main(void)
{	
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
	
    /* Init UART to 115200-8n1 for print message */
    UART0_Init();   
    UART1_Init();
	
	M480_InitSensor();
	M480_OpenSensor();
    Create_color_table(RGB_ColorPalette,YUV_ColorTable);
		
    HSUSBD_Open(&gsHSInfo, UVC_ClassRequest, UVC_SetInterface); //High Speed USB Device Controller
		
    /* Endpoint configuration */
    UVC_InitForHighSpeed(); //UVC related to USB
    NVIC_EnableIRQ(USBD20_IRQn);
    HSUSBD_Start();
    printf("M487 UVC code\n");		

    g_extend = 192/2/WIDTH;
    UVC_Transfer_Init();
	
	// Hide temperature display
	SetTempDisplay(1);
		
	while(true) {
		ResetFramePOIs();
		M480_StartSensor();
	}			
}
