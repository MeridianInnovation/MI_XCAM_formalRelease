/******************************************************************************
 * @file     usbd_audio.c
 * @brief    NuMicro series USBD driver Sample file
 * @version  1.0.0
 * @date     23, December, 2013
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "ThermalSensor.h"
#include "demo.h"

__align(4) uint32_t u32Data[TRANSFER_BUFFER/4];
 
__align(4) volatile UVC_INFO_T 		uvcInfo;
__align(4) volatile UVC_STATUS_T 	uvcStatus;
__align(4) volatile UVC_PU_INFO_T	uvcPuInfo = {0,	70, 7, -10,10, 1, 0, 20, 2, -30, 30, 0, 0, 40, 4, 0, 50, 5, 1, 5, 3, 0, 2, 2};
volatile uint8_t g_u8UVC_FID = 0;
volatile uint8_t g_u8UVC_PD = 0;
volatile uint8_t g_final = 0;
uint32_t g_u32AltInterface = 0;
uint32_t g_max_packet_size;
uint32_t volatile g_extend;  
int g_i16TempAvg, g_i16Ready = 0, g_TDATA_index = 0; 
	
/* Current Frame Index */
uint32_t g_u32CurFrameIndex = 1;

/* Current Format Index */
uint32_t g_u32CurFormatIndex = 1;
	
/* draw rectangle */	
uint8_t g_u8RectUp[2],	g_u8RectBottom[2], g_u8RectUpSide[RECT_SIZE] , g_u8RectBottomSide[RECT_SIZE] ;
uint32_t g_rect_up_start[2], g_rect_bottom_start[2], g_rect_up_side_start[RECT_SIZE], g_rect_bottom_side_start[RECT_SIZE];	
uint32_t g_u32transfer_row;	/* Row number (Before extended) per transfer */
uint32_t g_u32TempOffset[24];
uint8_t g_u8Temp[24];
uint8_t g_u8temp_display = 0;
uint32_t g_u32UVCWidth, g_offset_UVC;	

signed short tempBuff[32][32];
signed short* ptempBuff = (signed short*)&tempBuff;
	
/* Size per USB transfer */	
uint32_t g_transfer_size;	
extern short TDATA[2][1024];

#define SetUVCHEADER(data) 			HSUSBD->RESERVE2[1] = data
#define SetUVCCount(no) 				HSUSBD->RESERVE2[4] = no


	
void MFi_BULK_IN_Handler(void)
{
	
}

void MFi_BULK_OUT_Handler(void)
{
	
}

/*--------------------------------------------------------------------------*/
/**
 * @brief       USBD Interrupt Service Routine
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is the USBD ISR
 */
void USBD20_IRQHandler(void)
{
    __IO uint32_t IrqStL, IrqSt;

    IrqStL = HSUSBD->GINTSTS & HSUSBD->GINTEN;    /* get interrupt status */

    if (!IrqStL)    return;

    /* USB interrupt */
    if (IrqStL & HSUSBD_GINTSTS_USBIF_Msk) {
        IrqSt = HSUSBD->BUSINTSTS & HSUSBD->BUSINTEN;

        if (IrqSt & HSUSBD_BUSINTSTS_SOFIF_Msk)
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SOFIF_Msk);

        if (IrqSt & HSUSBD_BUSINTSTS_RSTIF_Msk) {
            HSUSBD_SwReset();
            HSUSBD_ResetDMA();
	    if (HSUSBD->OPER & 0x04)  /* high speed */
                UVC_InitForHighSpeed();
            else                    /* full speed */
                UVC_InitForFullSpeed();
						
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            HSUSBD_SET_ADDR(0);
									
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk|HSUSBD_BUSINTEN_RESUMEIEN_Msk|HSUSBD_BUSINTEN_SUSPENDIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RSTIF_Msk);
            HSUSBD_CLR_CEP_INT_FLAG(0x1ffc);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_RESUMEIF_Msk) {
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk|HSUSBD_BUSINTEN_SUSPENDIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RESUMEIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_SUSPENDIF_Msk) {
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SUSPENDIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_HISPDIF_Msk) {
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_HISPDIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_DMADONEIF_Msk) {
            g_hsusbd_DmaDone = 1;
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_DMADONEIF_Msk);

            if (!(HSUSBD->DMACTL & HSUSBD_DMACTL_DMARD_Msk)) {
                HSUSBD_ENABLE_EP_INT(EPB, HSUSBD_EPINTEN_RXPKIEN_Msk);
            }

            if (HSUSBD->DMACTL & HSUSBD_DMACTL_DMARD_Msk) {
                UVC_DMACompletion();
            }
        }

        if (IrqSt & HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk)
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk);

        if (IrqSt & HSUSBD_BUSINTSTS_VBUSDETIF_Msk) {
            if (HSUSBD_IS_ATTACHED()) {
                /* USB Plug In */
                HSUSBD_ENABLE_USB();
                printf("Plug In\n");
            } else {
                /* USB Un-plug */
                HSUSBD_DISABLE_USB();
                g_u32AltInterface = 0;
                uvcStatus.appConnected = 0;
                printf("Un-Plug\n");
                HSUSBD_ResetDMA();    
            }
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_VBUSDETIF_Msk);
        }
    }

    if (IrqStL & HSUSBD_GINTSTS_CEPIF_Msk) {
        IrqSt = HSUSBD->CEPINTSTS & HSUSBD->CEPINTEN;

        if (IrqSt & HSUSBD_CEPINTSTS_SETUPTKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPTKIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_SETUPPKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPPKIF_Msk);
            HSUSBD_ProcessSetupPacket();
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_OUTTKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_OUTTKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_INTKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
            if (!(IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk)) {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk);
                HSUSBD_CtrlIn();
            } else {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            }
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_PINGIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_PINGIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_TXPKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            if (g_hsusbd_CtrlInSize) {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
            } else {
                if (g_hsusbd_CtrlZero == 1)
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_ZEROLEN);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            }
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_RXPKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_RXPKIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_NAKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_NAKIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_STALLIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STALLIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_ERRIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_ERRIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk) {
            HSUSBD_UpdateDeviceState();
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_BUFFULLIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFFULLIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk);
            return;
        }
    }

    /* Isochronous in */
    if (IrqStL & HSUSBD_GINTSTS_EPAIF_Msk) {
        IrqSt = HSUSBD->EP[EPA].EPINTSTS & HSUSBD->EP[EPA].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPA, IrqSt);
    }
    /* Isochronous out */
    if (IrqStL & HSUSBD_GINTSTS_EPBIF_Msk) {
        IrqSt = HSUSBD->EP[EPB].EPINTSTS & HSUSBD->EP[EPB].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPB, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPCIF_Msk) {
        IrqSt = HSUSBD->EP[EPC].EPINTSTS & HSUSBD->EP[EPC].EPINTEN;
				MFi_BULK_IN_Handler();			
        HSUSBD_CLR_EP_INT_FLAG(EPC, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPDIF_Msk) {
        IrqSt = HSUSBD->EP[EPD].EPINTSTS & HSUSBD->EP[EPD].EPINTEN;
				MFi_BULK_OUT_Handler();
        HSUSBD_CLR_EP_INT_FLAG(EPD, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPEIF_Msk) {
        IrqSt = HSUSBD->EP[EPE].EPINTSTS & HSUSBD->EP[EPE].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPE, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPFIF_Msk) {
        IrqSt = HSUSBD->EP[EPF].EPINTSTS & HSUSBD->EP[EPF].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPF, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPGIF_Msk) {
        IrqSt = HSUSBD->EP[EPG].EPINTSTS & HSUSBD->EP[EPG].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPG, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPHIF_Msk) {
        IrqSt = HSUSBD->EP[EPH].EPINTSTS & HSUSBD->EP[EPH].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPH, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPIIF_Msk) {
        IrqSt = HSUSBD->EP[EPI].EPINTSTS & HSUSBD->EP[EPI].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPI, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPJIF_Msk) {
        IrqSt = HSUSBD->EP[EPJ].EPINTSTS & HSUSBD->EP[EPJ].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPJ, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPKIF_Msk) {
        IrqSt = HSUSBD->EP[EPK].EPINTSTS & HSUSBD->EP[EPK].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPK, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPLIF_Msk) {
        IrqSt = HSUSBD->EP[EPL].EPINTSTS & HSUSBD->EP[EPL].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPL, IrqSt);
    }
}

/* Process Unit Control */
uint32_t ProcessUnitControl(uint32_t u32ItemSelect, uint32_t u32Value)
{
	switch(u32ItemSelect)
	{
		case PU_BACKLIGHT_COMPENSATION_CONTROL:
				printf("Set Backlight -> %d\n",u32Value);
				break;			
		case PU_BRIGHTNESS_CONTROL:
				printf("Set Brightness -> %d\n",u32Value);
				break;			
		case PU_CONTRAST_CONTROL:
				printf("Set Contrast -> %d\n",u32Value);	
			 	break;			
		case PU_HUE_CONTROL:
				printf("Set Hue -> %d\n",u32Value);	
			 	break;			
		case PU_SATURATION_CONTROL:
				printf("Set Saturation -> %d\n",u32Value);	
			 	break;			
		case PU_SHARPNESS_CONTROL:
				printf("Set Sharpness -> %d\n",u32Value);	
			 	break;			
		case PU_GAMMA_CONTROL:		
				printf("Set Gamma -> %d\n",u32Value);			
			 	break;
		case PU_POWER_LINE_FREQUENCY_CONTROL:	
				printf("Set Power Line Frequency -> %d\n",u32Value);		
			 	break;			 	
	}
	return 0;
}
void UVC_Init(void)
{
    uvcInfo.u8ErrCode = 0;                                                
    uvcInfo.bChangeData = 0;
    uvcStatus.StillImage = 0;
    uvcInfo.PU_CONTROL = ProcessUnitControl;	
    uvcInfo.IsoMaxPacketSize[0] = g_max_packet_size;
    uvcInfo.IsoMaxPacketSize[1] = g_max_packet_size; 
    uvcInfo.IsoMaxPacketSize[2] = g_max_packet_size; 	
	
    //Initialize Video Capture Filter Control Value               	
    uvcInfo.CapFilter.Brightness = 1;  	
    uvcInfo.CapFilter.POWER_LINE_FREQUENCY = 2;
  	uvcInfo.CapFilter.Brightness = 1;
    uvcInfo.CapFilter.Contrast = 2;
   	uvcInfo.CapFilter.Hue = 3;
    uvcInfo.CapFilter.Saturation = 4;
    uvcInfo.CapFilter.Sharpness = 5;
    uvcInfo.CapFilter.Gamma = 3;
    uvcInfo.CapFilter.Backlight = 7;

    /* Initialize Video Probe and Commit Control data */
    memset((uint8_t *)&uvcInfo.VSCmdCtlData,0x0,sizeof(VIDEOSTREAMCMDDATA));
    uvcInfo.VSCmdCtlData.bmHint = 0x0100;
    uvcInfo.VSCmdCtlData.bFormatIndex = 1;
    uvcInfo.VSCmdCtlData.bFrameIndex = 1;
    uvcStatus.FormatIndex = 1;
    uvcStatus.FrameIndex = 1;    
    uvcStatus.MaxVideoFrameSize = UVC_SIZE_FRAME1;
    uvcInfo.VSCmdCtlData.dwFrameInterval = 0x051615;

    /* Initialize Still Image Command data */
    memset((uint8_t *)&uvcInfo.VSStillCmdCtlData,0x0,sizeof(VIDEOSTREAMSTILLCMDDATA));

    uvcInfo.VSStillCmdCtlData.bFormatIndex = 1;
    uvcInfo.VSStillCmdCtlData.bFrameIndex = 1;   
    uvcStatus.snapshotFormatIndex = 1;
    uvcStatus.snapshotFrameIndex = 1;	
}

/*--------------------------------------------------------------------------*/
/**
 * @brief       UAC Class Initial
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to configure endpoints for UAC class
 */
void UVC_InitForHighSpeed(void)
{
    /* Configure USB controller */
    /* Enable USB BUS, CEP and EPA , EPB global interrupt */
    HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk|HSUSBD_GINTEN_CEPIEN_Msk);
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk|HSUSBD_BUSINTEN_RESUMEIEN_Msk|HSUSBD_BUSINTEN_RSTIEN_Msk|HSUSBD_BUSINTEN_VBUSDETIEN_Msk);
    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);

    g_max_packet_size = EPA_MAX_PKT_SIZE;
    UVC_Init();
    /*****************************************************/
    /* Control endpoint */
    HSUSBD_SetEpBufAddr(CEP, CEP_BUF_BASE, CEP_BUF_LEN);
    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);

    /*****************************************************/
    /* EPA ==> ISO IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPA, EPA_BUF_BASE, EPA_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPA, EPA_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPA, ISO_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_ISO, HSUSBD_EP_CFG_DIR_IN);
#ifdef HSHB_MODE
    HSUSBD->EP[EPA].EPCFG = HSUSBD->EP[EPA].EPCFG | 0x100;
#endif	
    /* EPB ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPB, EPB_BUF_BASE, EPB_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPB, EPB_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPB, INT_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
		
    /* EPC ==> Bulk IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPC, EPC_BUF_BASE, EPC_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPC, EPC_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPC, MFi_BULK_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);			
    HSUSBD_ENABLE_EP_INT(EPC, HSUSBD_EPINTEN_INTKIEN_Msk);		
		
    /* EPD ==> Bulk OUT endpoint, address 4 */
    HSUSBD_SetEpBufAddr(EPD, EPD_BUF_BASE, EPD_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPD, EPD_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPD, MFi_BULK_OUT_EP_NUM, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPD, HSUSBD_EPINTEN_RXPKIEN_Msk|HSUSBD_EPINTEN_SHORTRXIEN_Msk);	
}
void UVC_InitForFullSpeed(void)
{
    /* Configure USB controller */
    /* Enable USB BUS, CEP and EPA , EPB global interrupt */
    HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk|HSUSBD_GINTEN_CEPIEN_Msk);
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk|HSUSBD_BUSINTEN_RESUMEIEN_Msk|HSUSBD_BUSINTEN_RSTIEN_Msk|HSUSBD_BUSINTEN_VBUSDETIEN_Msk);
    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);
	  
    g_max_packet_size = EPA_MAX_PKT_SIZE_FS;
    UVC_Init();
	
    /*****************************************************/
    /* Control endpoint */
    HSUSBD_SetEpBufAddr(CEP, CEP_BUF_BASE_FS, CEP_BUF_LEN_FS);
    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);

    /*****************************************************/
    /* EPA ==> ISO IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPA, EPA_BUF_BASE_FS, EPA_BUF_LEN_FS);
    HSUSBD_SET_MAX_PAYLOAD(EPA, EPA_MAX_PKT_SIZE_FS);
    HSUSBD_ConfigEp(EPA, ISO_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_ISO, HSUSBD_EP_CFG_DIR_IN);

   /* EPB ==> Interrupt IN endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPB, EPB_BUF_BASE_FS, EPB_BUF_LEN_FS);
    HSUSBD_SET_MAX_PAYLOAD(EPB, EPB_MAX_PKT_SIZE_FS);
    HSUSBD_ConfigEp(EPB, INT_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);

    /* EPC ==> Bulk IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPC, EPC_BUF_BASE_FS, EPC_BUF_LEN_FS);
    HSUSBD_SET_MAX_PAYLOAD(EPC, EPC_MAX_PKT_SIZE_FS);
    HSUSBD_ConfigEp(EPC, MFi_BULK_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);
		
    /* EPD ==> Bulk OUT endpoint, address 4 */
    HSUSBD_SetEpBufAddr(EPD, EPD_BUF_BASE_FS, EPD_BUF_LEN_FS);
    HSUSBD_SET_MAX_PAYLOAD(EPD, EPD_MAX_PKT_SIZE_FS);
    HSUSBD_ConfigEp(EPD, MFi_BULK_OUT_EP_NUM, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
}


void uvcdPU_Control(int Value)
{
    uvcInfo.u8ErrCode = 0;
    switch(gUsbCmd.wValue)
    {
        case PU_BACKLIGHT_COMPENSATION_CONTROL:
            if(Value >= uvcPuInfo.PU_BACKLIGHT_COMPENSATION_MIN && Value <= uvcPuInfo.PU_BACKLIGHT_COMPENSATION_MAX)
            {
                uvcInfo.CapFilter.Backlight = Value;
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_BACKLIGHT_COMPENSATION_CONTROL,Value);
                return;
            }
            break;			
        case PU_BRIGHTNESS_CONTROL:
            if(Value >= uvcPuInfo.PU_BRIGHTNESS_MIN && Value <= uvcPuInfo.PU_BRIGHTNESS_MAX)
            {
                uvcInfo.CapFilter.Brightness = Value;	
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_BRIGHTNESS_CONTROL,Value);					
                return;
            }
            break;			
        case PU_CONTRAST_CONTROL:
            if(Value >= uvcPuInfo.PU_CONTRAST_MIN && Value <= uvcPuInfo.PU_CONTRAST_MAX)
            {				
                uvcInfo.CapFilter.Contrast = Value;
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_CONTRAST_CONTROL,Value);	
                return;
            }
            break;			
        case PU_HUE_CONTROL:
            if(Value >= uvcPuInfo.PU_HUE_MIN && Value <= uvcPuInfo.PU_HUE_MAX)
            {			
                uvcInfo.CapFilter.Hue = Value;
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_HUE_CONTROL,Value);	
                return;
            }
            break;			
        case PU_SATURATION_CONTROL:
            if(Value >= uvcPuInfo.PU_SATURATION_MIN && Value <= uvcPuInfo.PU_SATURATION_MAX)
            {
                uvcInfo.CapFilter.Saturation = Value;	
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_SATURATION_CONTROL,Value);	
                return;
            }
            break;			
        case PU_SHARPNESS_CONTROL:
            if(Value >= uvcPuInfo.PU_SHARPNESS_MIN && Value <= uvcPuInfo.PU_SHARPNESS_MAX)
            {				
                uvcInfo.CapFilter.Sharpness = Value;
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_SHARPNESS_CONTROL,Value);	
                return;
            }   
            break;			
        case PU_GAMMA_CONTROL:
            if(Value >= uvcPuInfo.PU_GAMMA_MIN && Value <= uvcPuInfo.PU_GAMMA_MAX)
            {				
                uvcInfo.CapFilter.Gamma = Value;
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_GAMMA_CONTROL,Value);	
                return;
            }				
            break;
        case PU_POWER_LINE_FREQUENCY_CONTROL:
            if(Value >= uvcPuInfo.PU_POWER_LINE_FREQUENCY_MIN && Value <= uvcPuInfo.PU_POWER_LINE_FREQUENCY_MAX)
            {				
                uvcInfo.CapFilter.POWER_LINE_FREQUENCY = Value;
                if(uvcInfo.PU_CONTROL!=NULL)
                    uvcInfo.PU_CONTROL(PU_POWER_LINE_FREQUENCY_CONTROL,Value);	
                return;
            }	
            break;			 	
    }
    uvcInfo.u8ErrCode = EC_Out_Of_Range;
    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
    return;    
}

uint32_t uvcdPU_Info(uint32_t Req, uint32_t Unit)
{
    uvcInfo.u8ErrCode = 0;
    switch(Req)
   	{
        case GET_CUR:
            switch(Unit)
            {		    
                case PU_BACKLIGHT_COMPENSATION_CONTROL:
                    return uvcInfo.CapFilter.Backlight;
                case PU_BRIGHTNESS_CONTROL:
                    return uvcInfo.CapFilter.Brightness;
                case PU_CONTRAST_CONTROL:
                    return uvcInfo.CapFilter.Contrast;
                case PU_HUE_CONTROL:
                    return uvcInfo.CapFilter.Hue;				
                case PU_SATURATION_CONTROL:
                    return uvcInfo.CapFilter.Saturation;
                case PU_SHARPNESS_CONTROL:
                    return uvcInfo.CapFilter.Sharpness;
                case PU_GAMMA_CONTROL:
                    return uvcInfo.CapFilter.Gamma;
                case PU_POWER_LINE_FREQUENCY_CONTROL:
                    return uvcInfo.CapFilter.POWER_LINE_FREQUENCY;
                default:
                    uvcInfo.u8ErrCode = EC_Invalid_Control;							
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);			
                    break;
            }
			     	break;
        case GET_MIN:			 
            switch(Unit)
            {		    
                case PU_BACKLIGHT_COMPENSATION_CONTROL:
                    return uvcPuInfo.PU_BACKLIGHT_COMPENSATION_MIN;
                case PU_BRIGHTNESS_CONTROL:
                    return uvcPuInfo.PU_BRIGHTNESS_MIN;
                case PU_CONTRAST_CONTROL:
                    return uvcPuInfo.PU_CONTRAST_MIN;
                case PU_HUE_CONTROL:
                    return uvcPuInfo.PU_HUE_MIN;		
                case PU_SATURATION_CONTROL:
                    return uvcPuInfo.PU_SATURATION_MIN;
                case PU_SHARPNESS_CONTROL:
                    return uvcPuInfo.PU_SHARPNESS_MIN;
                case PU_GAMMA_CONTROL:
                    return uvcPuInfo.PU_GAMMA_MIN;
                case PU_POWER_LINE_FREQUENCY_CONTROL:
                    return uvcPuInfo.PU_POWER_LINE_FREQUENCY_MIN;	
                default:
                    uvcInfo.u8ErrCode = EC_Invalid_Control;						
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);			
                    return 6;		 
            }
        case GET_MAX:
            switch(Unit)
            {		    
                case PU_BACKLIGHT_COMPENSATION_CONTROL:
                    return uvcPuInfo.PU_BACKLIGHT_COMPENSATION_MAX;
                case PU_BRIGHTNESS_CONTROL:
                    return uvcPuInfo.PU_BRIGHTNESS_MAX;
                case PU_CONTRAST_CONTROL:
                    return uvcPuInfo.PU_CONTRAST_MAX;
                case PU_HUE_CONTROL:
                    return uvcPuInfo.PU_HUE_MAX;					 
                case PU_SATURATION_CONTROL:
                    return uvcPuInfo.PU_SATURATION_MAX;
                case PU_SHARPNESS_CONTROL:
                    return uvcPuInfo.PU_SHARPNESS_MAX;
                case PU_GAMMA_CONTROL:
                    return uvcPuInfo.PU_GAMMA_MAX;
                case PU_POWER_LINE_FREQUENCY_CONTROL:
                    return uvcPuInfo.PU_POWER_LINE_FREQUENCY_MAX;	
                default:
                    uvcInfo.u8ErrCode = EC_Invalid_Control;						
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);				
                    return 6;	 			 
            }
        case GET_RES:
            return 1;
        case GET_INFO:
            switch(Unit)
            {		    
                case PU_BACKLIGHT_COMPENSATION_CONTROL:
                case PU_BRIGHTNESS_CONTROL:
                case PU_CONTRAST_CONTROL: 
                case PU_HUE_CONTROL:
                case PU_SATURATION_CONTROL:
                case PU_SHARPNESS_CONTROL:
                case PU_GAMMA_CONTROL:
                case PU_POWER_LINE_FREQUENCY_CONTROL:
                    return 3;
                default:
                    uvcInfo.u8ErrCode = EC_Invalid_Control;							 
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);			
                    return 0;			 
            }
        case GET_DEF:  
            switch(Unit)
            {		    
                case PU_BACKLIGHT_COMPENSATION_CONTROL:
                    return uvcPuInfo.PU_BACKLIGHT_COMPENSATION_DEF;
                case PU_BRIGHTNESS_CONTROL:
                    return uvcPuInfo.PU_BRIGHTNESS_DEF;
                case PU_CONTRAST_CONTROL:
                    return uvcPuInfo.PU_CONTRAST_DEF;
                case PU_HUE_CONTROL:
                    return uvcPuInfo.PU_HUE_DEF;					 
                case PU_SATURATION_CONTROL:
                    return uvcPuInfo.PU_SATURATION_DEF;
                case PU_SHARPNESS_CONTROL:
                return uvcPuInfo.PU_SHARPNESS_DEF;
                case PU_GAMMA_CONTROL:
                    return uvcPuInfo.PU_GAMMA_DEF;
                case PU_POWER_LINE_FREQUENCY_CONTROL:
                    return uvcPuInfo.PU_POWER_LINE_FREQUENCY_DEF;
                default:
                    uvcInfo.u8ErrCode = EC_Invalid_Control;								
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);		 		
                    return 6;	 
            }
        case GET_LEN:
            uvcInfo.u8ErrCode = EC_Invalid_Request;										
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);		
					  return 6;	
    } 
   	return 6;
}
/**
 * @brief       UAC class request
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process UAC class requests
 */
uint32_t volatile Data;  

void UVC_ClassRequest(void)
{
    if (gUsbCmd.bmRequestType & 0x80)   /* request data transfer direction */
    {
        /* Video Control Requests */
        /* Unit and Terminal Control Requests */
  	    /* Interface Control Requests */
  	    if(gUsbCmd.wIndex == 0x0000 && gUsbCmd.wValue == VC_REQUEST_ERROR_CODE_CONTROL ) /* Only send to Video Control interface ID(00) */
  	    {
  	        Data = uvcInfo.u8ErrCode;	
  	        HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);
  	        HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);					
  	    }
	    /* Processing Unit Control Requests */
   	    else if(gUsbCmd.wIndex == 0x0500) /* Processing Unit ID(05) and Video Control interface ID(00) */
   	    {				 
  	        Data = uvcdPU_Info(gUsbCmd.bRequest,gUsbCmd.wValue);				
  	        if(uvcInfo.u8ErrCode == 0)
  	        {
  	            HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);
  	            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
  	        }
  	    }
	    /* Camera Terminal Control Requests	*/
   	    else if(gUsbCmd.wIndex == 0x0100) /* Camera Terminal ID(01) and Video Control interface ID(00) */
   	    {	
   	        uvcInfo.u8ErrCode = EC_Invalid_Control;			
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
   	    }
        /* Videostreaming Requests */	
      	else if(gUsbCmd.wIndex == 0x0001) /* VideoStreaming interface(0x01) */
   	    {
   	        if(gUsbCmd.wValue == VS_PROBE_CONTROL || gUsbCmd.wValue == VS_COMMIT_CONTROL)
   	        {      										
   	            if(gUsbCmd.bRequest == GET_INFO || gUsbCmd.bRequest == GET_LEN)
   	            {
   	                if(gUsbCmd.bRequest == GET_INFO)			
   	           	        Data = 3; /* Info */
   	                else
   	           	        Data = 26;/* Length	*/
   	                HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);
   	           	    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
   	            }
   	            else
   	            {	
   	                if(uvcInfo.VSCmdCtlData.bFormatIndex != uvcInfo.u32FormatIndx || uvcInfo.VSCmdCtlData.bFrameIndex != uvcInfo.u32FrameIndx) 
   	           	    {
   	           	        uvcStatus.FormatIndex = uvcInfo.u32FormatIndx = uvcInfo.VSCmdCtlData.bFormatIndex;
   	           	        uvcStatus.FrameIndex = uvcInfo.u32FrameIndx = uvcInfo.VSCmdCtlData.bFrameIndex;					
   	           	        uvcInfo.bChangeData = 1;
   	           	    }								      
   	           	    switch(uvcInfo.VSCmdCtlData.bFrameIndex)
   	           	    {
   	           	        case UVC_FRAME1:
   	           	            uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_FRAME1;
   	           	            break;
   	           	        case UVC_FRAME2:
   	           	            uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_FRAME2;
   	           	            break;
   	           	       case UVC_FRAME3:
   	           	            uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_FRAME3;
   	           	            break;
   	           	    }					    			    				      
   	           	    uvcInfo.VSCmdCtlData.dwMaxPayloadTransferSize = uvcInfo.IsoMaxPacketSize[g_u32AltInterface]; 
   	           	    uvcStatus.MaxVideoFrameSize = uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize;
   	           	    uvcInfo.VSCmdCtlData.dwMaxPayloadTransferSize = g_max_packet_size;
   	           	    uvcInfo.VSCmdCtlData.wCompQuality = 0x3D;
   	           	    HSUSBD_PrepareCtrlIn((uint8_t *)&uvcInfo.VSCmdCtlData, gUsbCmd.wLength);		
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);										
   	           	}	
   	        }
   	        else if(gUsbCmd.wValue == VS_STILL_PROBE_CONTROL || gUsbCmd.wValue == VS_STILL_COMMIT_CONTROL)
   	        {
   	            if(gUsbCmd.bRequest == GET_INFO || gUsbCmd.bRequest == GET_LEN)
   	            {
   	                if(gUsbCmd.bRequest == GET_INFO)			
   	           	        Data = 3;  /* Info */
   	                else
   	           	        Data = 11; /* Length */					
   	               HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);				
                   HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                   HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);									 
   	            }
   	            else
   	            {						
                    switch(uvcInfo.VSStillCmdCtlData.bFrameIndex)
   	           	    {
   	           	        case UVC_STILL_FRAME1:
   	           	            uvcInfo.VSStillCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_FRAME1;
   	           	            break;
   	           	        case UVC_STILL_FRAME2:
   	           	            uvcInfo.VSStillCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_FRAME2;
   	           	            break;
   	           	        case UVC_STILL_FRAME3:
   	           	            uvcInfo.VSStillCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_FRAME3;
   	              	        break;	    
   	           	    }				   	
                    uvcStatus.snapshotFormatIndex = uvcInfo.VSStillCmdCtlData.bFormatIndex;
                    uvcStatus.snapshotFrameIndex = uvcInfo.VSStillCmdCtlData.bFrameIndex;			    		
                    uvcStatus.snapshotMaxVideoFrameSize = uvcInfo.VSStillCmdCtlData.dwMaxPayloadTranferSize = uvcInfo.IsoMaxPacketSize[g_u32AltInterface];				          			
                    HSUSBD_PrepareCtrlIn((uint8_t *)&uvcInfo.VSStillCmdCtlData, gUsbCmd.wLength); /* ??????? */												 
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);									 
                }					
   	        }
   	        else if(gUsbCmd.wValue == VS_STILL_IMAGE_TRIGGER_CONTROL)
   	        {			
   	            if(gUsbCmd.bRequest == GET_INFO)			
   	                Data = 3;    /* Info */
   	            else
   	           	    Data = uvcStatus.StillImage; /* Trigger */
   	           HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);	
               HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
               HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);							 
   	        }
   	    }  				
   	    else
   	    {
   	        /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
        }			
    }    
    else
    {
        /* Video Probe Control Request */
   	    if(gUsbCmd.wIndex == 0x0500)
   	    {   
            uint16_t OutValue =0;
		
   	        if(gUsbCmd.bRequest == SET_CUR)     	 
   	        {  	
   	            HSUSBD_CtrlOut((uint8_t *)&OutValue, gUsbCmd.wLength);
   	            uvcdPU_Control(OutValue);             
   	        }						   
   	    }
   	    else if(gUsbCmd.wIndex == 0x0100)
   	    {
   	        uvcInfo.u8ErrCode = EC_Invalid_Request;
   	        HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
       	}
      	else if(gUsbCmd.wIndex == 0x0001)
      	{ 
   	        uint8_t *CmdDataPtr = 0;
	    
   	        if(gUsbCmd.wValue == VS_STILL_IMAGE_TRIGGER_CONTROL)
   	        {	
   	            CmdDataPtr = (uint8_t *)&uvcStatus.StillImage;				
   	            HSUSBD_CtrlOut(CmdDataPtr, gUsbCmd.wLength);	
   	        }
   	        else if(gUsbCmd.wValue == VS_PROBE_CONTROL || gUsbCmd.wValue == VS_COMMIT_CONTROL )
   	        {
   	            CmdDataPtr = (uint8_t *)&uvcInfo.VSCmdCtlData;				
   	            HSUSBD_CtrlOut(CmdDataPtr, gUsbCmd.wLength);	
   	        }
   	        else if(gUsbCmd.wValue == VS_STILL_PROBE_CONTROL || gUsbCmd.wValue == VS_STILL_COMMIT_CONTROL )
   	        {
   	            CmdDataPtr = (uint8_t *)&uvcInfo.VSStillCmdCtlData;	
   	            HSUSBD_CtrlOut(CmdDataPtr, gUsbCmd.wLength);	
   	        }              			    
   	    }	  	  

   	    /* Status stage */
   	    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
   	    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
   	    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
    }
}


/**
 * @brief       Set Interface standard request
 *
 * @param[in]   u32AltInterface Interface
 *
 * @return      None
 *
 * @details     This function is used to set UAC Class relative setting
 */
void UVC_SetInterface(uint32_t u32AltInterface)
{
    if ((gUsbCmd.wIndex & 0xff) == 1)
    { /* Audio Iso IN interface */
        if (u32AltInterface == 1)
        {
   	        g_u32AltInterface = 1;
   	        uvcStatus.appConnected = 1;
   	        printf("Set Inter 1\n");
        }
        else if (u32AltInterface == 0)
        {
   	        g_u32AltInterface = 0;
   	        uvcStatus.appConnected = 0;

   	        HSUSBD_ResetDMA();        
   	        HSUSBD->EP[EPA].EPRSPCTL |= HSUSBD_EPRSPCTL_FLUSH_Msk;					
   	        printf("Set Inter 0\n");
        }
    }
}

void UVC_DMACompletion(void)
{	
    int32_t volatile TMP;
    uint32_t timeout = 0;
    uint32_t volatile status;
    TMP =  HSUSBD->EP[EPA].EPDATCNT & 0xFFFF;	
		
    HSUSBD->EP[EPA].EPINTSTS  = HSUSBD_EPINTSTS_INTKIF_Msk;
    while(TMP >= g_max_packet_size)
    {
        TMP = HSUSBD->EP[EPA].EPDATCNT & 0xFFFF;
        if(g_u32AltInterface == 0 || (HSUSBD_IS_ATTACHED() == 0))		
            return;					
        if(HSUSBD->EP[EPA].EPINTSTS & HSUSBD_EPINTSTS_INTKIF_Msk)
        {		
            HSUSBD->EP[EPA].EPINTSTS  = HSUSBD_EPINTSTS_INTKIF_Msk;
            timeout = 0;
        }
        else
            timeout++;
        if(timeout > 50000)
        {
            printf("Timeout\n");
            return;
        }				
    }	
    if(g_final)
    {
//        HSUSBD->UVCHDAT0 = (((g_u8UVC_PD | UVC_PH_EndOfFrame | (g_u8UVC_FID &0x01)) <<8) | 0x02); /* End Of Frame */				
        SetUVCHEADER((((g_u8UVC_PD | UVC_PH_EndOfFrame | (g_u8UVC_FID &0x01)) <<8) | 0x02)); /* End Of Frame */				

//        HSUSBD->UVCEPAHCNT = 0x02;
        SetUVCCount(0x02);			

        HSUSBD->EP[EPA].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN; /* Set Packet End */
        
        HSUSBD->EP[EPA].EPINTSTS  = HSUSBD_EPINTSTS_INTKIF_Msk;
        timeout = 0;
        while(HSUSBD->EP[EPA].EPDATCNT& 0xFFFF)	
        {
            if(g_u32AltInterface == 0 || (HSUSBD_IS_ATTACHED() == 0))		
                return;		
            if(HSUSBD->EP[EPA].EPINTSTS & HSUSBD_EPINTSTS_INTKIF_Msk)
            {		
                HSUSBD->EP[EPA].EPINTSTS  = HSUSBD_EPINTSTS_INTKIF_Msk;
                timeout = 0;
            }
            else
                timeout++;
            if(timeout > 50000)
            {
                printf("Timeout\n");
                return;
            }								
        }
        g_u8UVC_FID++;  
			
    }	
    uvcStatus.bReady = 1;
}

void uvcd_Transfer(uint32_t DRAM_Addr ,uint32_t Tran_Size)
{	
    if(Tran_Size != 0)
    {	
        if(uvcInfo.bChangeData ==1)
        {			
            uvcInfo.bChangeData = 0;
            if(HSUSBD->DMACTL & HSUSBD_DMACTL_DMAEN_Msk)
            {
                HSUSBD_ResetDMA();                            
                uvcStatus.bReady = 1; 
            }		
        }
		
        HSUSBD_SET_DMA_READ(ISO_IN_EP_NUM);
        HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk|HSUSBD_BUSINTEN_SUSPENDIEN_Msk|HSUSBD_BUSINTEN_RSTIEN_Msk|HSUSBD_BUSINTEN_VBUSDETIEN_Msk);
        HSUSBD_SET_DMA_ADDR((uint32_t)DRAM_Addr);
        HSUSBD_SET_DMA_LEN(Tran_Size);
        HSUSBD_ENABLE_DMA();
    }
}

int uvcdIsReady(void)
{
    if(!uvcStatus.bReady)
    {			
        /* Application disconnected */
        if(g_u32AltInterface == 0 || (HSUSBD_IS_ATTACHED() == 0))		
        {
            if (HSUSBD->DMACTL & HSUSBD_DMACTL_DMAEN_Msk)
            {
                HSUSBD_ResetDMA();        
                HSUSBD->EP[EPA].EPRSPCTL |= HSUSBD_EPRSPCTL_FLUSH_Msk;
            }	   
            uvcStatus.bReady = 1; 			
        }
    }	
    return uvcStatus.bReady;	
}


int uvcdSendImage(uint32_t u32Addr, uint32_t u32transferSize, int bStillImage, int final)  /* Send image */
{
    if(u32transferSize != 0)
    {
        uvcStatus.bReady = 0;        
        HSUSBD_ResetDMA();       			    
        g_final = final;
        if(g_u32AltInterface == 1)
        {
            if(bStillImage) 
            {    		
                g_u8UVC_PD = UVC_PH_Snapshot;		/* Payload Header for Snapshot */    		       			    			
                uvcStatus.StillImage = 0; 
            }
            else 		
                g_u8UVC_PD = UVC_PH_Preview;		/* Payload Header for Preview */ 			 

            /* Start of Transfer Payload Data */					
//            HSUSBD->UVCHDAT0 = (((g_u8UVC_PD | (g_u8UVC_FID &0x01)) <<8)| 0x02);  
						SetUVCHEADER((((g_u8UVC_PD | (g_u8UVC_FID &0x01)) <<8)| 0x02));					
//            HSUSBD->UVCEPAHCNT = 0x02; 
						SetUVCCount(2);						
            uvcd_Transfer(u32Addr , u32transferSize);	    
            return 1;
        }
        else
        {
            if(HSUSBD->DMACTL & HSUSBD_DMACTL_DMAEN_Msk)
                HSUSBD_ResetDMA();                        
            uvcStatus.bReady = 1;
        }      
    }	 
    return 0;
}

uint32_t TempAvg(int index)  /* Temperature Average */
{
    uint32_t start, sum = 0,i,j;
		int val;
    /* Temperature Average */	
    start =(HEIGHT - W_HEIGHT) / 2 * WIDTH +  (WIDTH - W_WIDTH) / 2;			
    for(i=0;i<W_HEIGHT;i++)
        for(j=0;j<W_WIDTH;j++)
				{	
					val = Target[index][start + i* WIDTH + j] - 2732;
					if ( val < 0 )
						val = 0;
          sum = sum + val;
				}
    sum = sum / W_HEIGHT / W_WIDTH;
		
    return sum;
}

void TempAvgDisplay(uint32_t u32Count, uint32_t sum, int mode)  /* Temperature Average Display */
{
    int i,j;
    uint32_t u32Index0 = 0, u32Index1 = 0, u32Index2 = 0;
    u32Count = u32Count /4;
	
	// Single pixel
//	pixTemp = GetTemp(1,1);
	
	// pointer to 2D-array
//	ptempBuff = (signed short*) &Target;
//	sum = (uint32_t) *(ptempBuff +  1*(sizeof(Target[0])) + 1 * (sizeof(signed short))) - 2732;
	
    if(mode == 1)
    {
        u32Index0 = sum /100;
        u32Index1 = (sum /10) % 10;
        u32Index2 = sum % 10;
    }
		
    for(i = 0;i< 24;i++)
    {
        if(mode == 1)
        {
            if(g_u8Temp[i] == 0)
            {
                if(u32Count >= g_u32TempOffset[i])
                {
                    memcpy((void *)&u32Data[(g_u32TempOffset[i] + 0)%(g_transfer_size/4)],(void *) &Image[u32Index0][i*8],32);
                    memcpy((void *)&u32Data[(g_u32TempOffset[i] + 8)%(g_transfer_size/4)],(void *) &Image[u32Index1][i*8],32);
                    memcpy((void *)&u32Data[(g_u32TempOffset[i] + 16)%(g_transfer_size/4)],(void *) &Image_dot[i*4],16);
                    memcpy((void *)&u32Data[(g_u32TempOffset[i] + 20)%(g_transfer_size/4)],(void *) &Image[u32Index2][i*8],32);
                    g_u8Temp[i] = 1;
                    if(i==23)
                        g_u8temp_display = 1;
                }
            }
        }
        else
        {
            for(j=0;j<28;j++)
                u32Data[(g_u32TempOffset[i] + j)%(g_transfer_size/4)] = 0x80008000;
        }
    }
}
void UVC_Transfer_Init(void)
{
    int i;
    uint32_t start_UVC;
    if((uvcStatus.MaxVideoFrameSize % TRANSFER_BUFFER) == 0)
        g_u32transfer_row = HEIGHT / (uvcStatus.MaxVideoFrameSize / TRANSFER_BUFFER);					
    else
        g_u32transfer_row = HEIGHT / (uvcStatus.MaxVideoFrameSize / TRANSFER_BUFFER + 1);		
				
    if(g_u32transfer_row == 0)						
        g_u32transfer_row = 1;
				
    if(uvcStatus.FrameIndex == UVC_FRAME3)	
    {
        g_u32UVCWidth = (640/2/WIDTH)*WIDTH*2;				
        g_offset_UVC = (640 - 448) / 4;			
        g_rect_up_start[0] = (((WIDTH - W_HEIGHT) * 7 - 1) * (640) + (WIDTH - W_WIDTH) * 7)/2 + g_offset_UVC;
        g_rect_up_start[1] = (((WIDTH - W_HEIGHT) * 7) * (640) + (WIDTH - W_WIDTH) * 7)/2 + g_offset_UVC;
        for(i=0;i<RECT_SIZE;i++)
            g_rect_up_side_start[i] =  (((WIDTH - W_HEIGHT) * 7 + i) * (640) + (WIDTH - W_WIDTH) * 7)/2 + g_offset_UVC;
					
        g_rect_bottom_start[0] = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2 -1) * (640) + (WIDTH - W_WIDTH) * 7)/2 + g_offset_UVC;	
        g_rect_bottom_start[1] = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2) * (640) + (WIDTH - W_WIDTH) * 7)/2 + g_offset_UVC;		
        g_transfer_size = (g_u32transfer_row * (448/2/WIDTH) * 2) * g_u32UVCWidth * 2;	
        for(i=0;i<RECT_SIZE;i++)
            g_rect_bottom_side_start[i] = (((WIDTH - W_HEIGHT) * 7 + W_HEIGHT *7*2 - i) * (640) + (WIDTH - W_WIDTH) * 7)/2 + g_offset_UVC;
									
        start_UVC = (640 - 56) / 2;	    		
        for(i = 0;i< 24;i++)
            g_u32TempOffset[i] = start_UVC + i * 640/2;		
// printf
//				printf("Total width = %d\n", g_transfer_size);
    }
    else
    {
        g_u32UVCWidth = g_extend*WIDTH*2;		
        g_rect_up_start[0] = (((WIDTH - W_HEIGHT) * g_extend - 1) * g_u32UVCWidth + (WIDTH - W_WIDTH) * g_extend)/2;		
        g_rect_up_start[1] = (((WIDTH - W_HEIGHT) * g_extend) * g_u32UVCWidth + (WIDTH - W_WIDTH) * g_extend)/2;						  
        for(i=0;i<RECT_SIZE;i++)
            g_rect_up_side_start[i] =  (((WIDTH - W_HEIGHT) * g_extend + i) * g_u32UVCWidth + (WIDTH - W_WIDTH) * g_extend)/2;				
					
        g_rect_bottom_start[0] = (((WIDTH - W_HEIGHT) * g_extend + W_HEIGHT *g_extend*2 - 1) * g_u32UVCWidth + (WIDTH - W_WIDTH) * g_extend)/2;					
        g_rect_bottom_start[1] = (((WIDTH - W_HEIGHT) * g_extend + W_HEIGHT *g_extend*2) * g_u32UVCWidth + (WIDTH - W_WIDTH) * g_extend)/2;					       			
        g_transfer_size = (g_u32transfer_row * g_extend * 2) * g_u32UVCWidth * 2;		      
        for(i=RECT_SIZE-1;i>=0;i--)
            g_rect_bottom_side_start[i] =  (((WIDTH - W_HEIGHT) * g_extend + W_HEIGHT *g_extend*2 - i) * g_u32UVCWidth + (WIDTH - W_WIDTH) * g_extend)/2;										
				
        start_UVC = (g_u32UVCWidth - 56) / 2;	    		
        for(i = 0;i< 24;i++)
            g_u32TempOffset[i] = start_UVC + i * g_u32UVCWidth/2;		
// printf
//				printf("Total width = %d\n", g_transfer_size);			
    }						
}

void uvcdEvent(int index)  /* UVC event */
{
    uint32_t volatile i, j, k, l, transfer_row_index, temp_avg, u32DataCount;
    uint32_t u32RemainSize;   
    uint32_t value, index_offset;
    int32_t val,count;

    if (uvcStatus.appConnected == 1)
    { 	
        if(g_u32CurFrameIndex != uvcStatus.FrameIndex ||g_u32CurFormatIndex != uvcStatus.FormatIndex)
        {					
            g_u32CurFrameIndex = uvcStatus.FrameIndex;
            g_u32CurFormatIndex = uvcStatus.FormatIndex;	
            /* Frame Size Changed */
            switch(uvcStatus.FrameIndex)
            {										
                case UVC_FRAME1:
                    g_extend = 192/2/WIDTH;
                    break;										
                case UVC_FRAME2:								
                    g_extend = 320/2/WIDTH;		
                    break;			
                case UVC_FRAME3:				
                    g_extend = 448/2/WIDTH;	
                    for(i=0;i<TRANSFER_BUFFER/4;i++)
                        u32Data[i] = 0x80008000;//0x80FF80FF;											
                    break;													
            } 				
						UVC_Transfer_Init();
        }
				
        u32DataCount = 0;
        g_u8temp_display = 0;
        u32RemainSize = uvcStatus.MaxVideoFrameSize;
				
        draw_init();
        temp_avg = TempAvg(index);	
				
        if(uvcStatus.FrameIndex == UVC_FRAME3)
        {
            uint32_t u32Count = 0;
            uint32_t u32Size = (480 - 448) * 640 * 2;									
            while(u32Size > g_transfer_size)
            {
                u32Count+= g_transfer_size;
                draw_display(u32Count, temp_avg);										
                uvcdSendImage((uint32_t)u32Data, g_transfer_size, uvcStatus.StillImage,0);
                u32Size -= g_transfer_size;														
                /* Wait for Complete */ 	
                while(!uvcdIsReady());		
							
                TempAvgDisplay(u32Count, temp_avg,0);
            }						
            u32Count+= u32Size;						
            draw_display(u32Count, temp_avg);									
            uvcdSendImage((uint32_t)u32Data, u32Size, uvcStatus.StillImage,0);						
            /* Wait for Complete */ 	
            while(!uvcdIsReady());						
            TempAvgDisplay(u32Count, temp_avg,0);
            u32RemainSize -= u32Count;
        }
        transfer_row_index = 0;
        count = 0;
        for(i=0;i<HEIGHT;i++)
        {
            for(j=0;j<WIDTH;j++)
            {
							val = TDATA[index][count] = Target[j][i] - 2732;    
							if ( val < 0 )
							{            
			val = TDATA[index][count] = 0;
		}
		val += COLOR_OFFSET;
		if (val >= TOTAL_COLOR_NUM)
			val = TOTAL_COLOR_NUM-1;						 
		count++;			 
		value = YUV_ColorTable[val].YUVData;
                if(uvcStatus.FrameIndex == UVC_FRAME3)
                {									
                    index_offset = transfer_row_index*7*640 + g_offset_UVC + j*7; 
										
                    for(k=0;k<14;k++)
                        for(l=0;l<7;l++)
                        {
                            u32Data[index_offset + k*640/2 + l] = value;		
                        }
                }
                else			
                {
                    index_offset = transfer_row_index*g_extend*g_u32UVCWidth + j*g_extend;
                    /* UVC */	
                    for(k=0;k<g_extend*2;k++)
                        for(l=0;l<g_extend;l++)
                        {
                            u32Data[index_offset + k*g_extend*WIDTH + l] = value;	
                        }
                }						 
            }
            if(transfer_row_index == (g_u32transfer_row -1) || (i == HEIGHT - 1))
            {												
                transfer_row_index = 0;
                /* Send Image */													
                if(i == HEIGHT - 1)
                {
                    if(uvcStatus.FrameIndex == UVC_FRAME3)
                    {											
                        for(i=0;i<TRANSFER_BUFFER/4;i++)
                            u32Data[i] = 0x80008000;//0x80FF80FF;			

                        while(u32RemainSize > g_transfer_size)
                        {													
                            u32DataCount += g_transfer_size; 
                            draw_display(u32DataCount, temp_avg);
                            uvcdSendImage((uint32_t)u32Data, g_transfer_size, uvcStatus.StillImage,0);
                            u32RemainSize -= g_transfer_size;														
                            /* Wait for Complete */ 	
                            while(!uvcdIsReady());		
                        }
                        u32DataCount += u32RemainSize; 
                        draw_display(u32DataCount, temp_avg);				
                        uvcdSendImage((uint32_t)u32Data, u32RemainSize, uvcStatus.StillImage,1);			
                        	
                    }
                    else
                    {											
                        u32DataCount += u32RemainSize; 
                        draw_display(u32DataCount, temp_avg);
                        uvcdSendImage((uint32_t)u32Data, u32RemainSize, uvcStatus.StillImage,1);										
                    }
                }
                else					
                {
                    u32DataCount += g_transfer_size; 
                    draw_display(u32DataCount, temp_avg);
                    uvcdSendImage((uint32_t)u32Data, g_transfer_size, uvcStatus.StillImage,0);
                    u32RemainSize -= g_transfer_size;
                }
                /* Wait for Complete */ 	
                while(!uvcdIsReady());		
            }
            else
                transfer_row_index++;	
        }	 
    }  
}
void draw_init(void)
{
    int i;
    for(i=0;i<2;i++)
  	{
        g_u8RectUp[i] = 0;
        g_u8RectBottom[i] = 0;
    }
    for(i=0;i<24;i++)
        g_u8Temp[i] = 0;
		
    for(i=0;i<RECT_SIZE;i++)
    {
        g_u8RectUpSide[i] = 1;
        g_u8RectBottomSide[i] = 1;
    }
    for(i=0;i<2*g_extend;i++)
    {
        g_u8RectUpSide[i] = 0;
        g_u8RectBottomSide[i] = 0;
    }
}
void draw_display(uint32_t u32Count, uint32_t temp_avg)
{
    if(g_u8temp_display == 0)
        TempAvgDisplay(u32Count, temp_avg, 1);
								
    draw_rectangle(u32Count);	
}

void draw_rectangle(uint32_t u32Count)
{
    int i, j, extend;	
    /* Up */
    u32Count = u32Count / 4;

    for(i=0;i<2;i++)
    {
        if(g_u8RectUp[i] == 0)
        {
            if(u32Count >= g_rect_up_start[i]) 
            {
                if(uvcStatus.FrameIndex == UVC_FRAME3)	
                    extend = 7;							
                else
                    extend = g_extend;
						
                for(j=0;j<1*extend;j++)
                    u32Data[(g_rect_up_start[i]+j)% (g_transfer_size/4)] = 0x80008000;
                for(j=0;j<1*extend;j++)
                    u32Data[(g_rect_up_start[i]+extend*W_WIDTH-j)% (g_transfer_size/4)] = 0x80008000;
						          		
                g_u8RectUp[i] = 1;
            }	
        }
        /* Bottom */	
        if(g_u8RectBottom[i] == 0)
        {        	
            if(u32Count >= g_rect_bottom_start[i]) 
            {
                if(uvcStatus.FrameIndex == UVC_FRAME3)	
                    extend = 7;							
                else
                    extend = g_extend;
						
                for(j=0;j<1*extend;j++)
                    u32Data[(g_rect_bottom_start[i]+j)% (g_transfer_size/4)] = 0x80008000;
                for(j=0;j<1*extend;j++)
                    u32Data[(g_rect_bottom_start[i]+extend*W_WIDTH-j)% (g_transfer_size/4)] = 0x80008000;
						          							
                g_u8RectBottom[i] = 1;
            }
        }		
    }

    for(i=0;i<RECT_SIZE;i++)
    {
        if(g_u8RectUpSide[i] == 0)
        {
            if(u32Count >= g_rect_up_side_start[i]) 
            {
                if(uvcStatus.FrameIndex == UVC_FRAME3)	
                    extend = 7;							
                else
                    extend = g_extend;
						
                u32Data[(g_rect_up_side_start[i])% (g_transfer_size/4)] = 0x80008000;
                u32Data[(g_rect_up_side_start[i] + extend*W_WIDTH)% (g_transfer_size/4)] = 0x80008000;
								
                g_u8RectUpSide[i] = 1;
						}
        }					
        if(g_u8RectBottomSide[i] == 0)
        {
            if(u32Count >= g_rect_bottom_side_start[i]) 
            {
                if(uvcStatus.FrameIndex == UVC_FRAME3)	
                    extend = 7;							
                else
                    extend = g_extend;
						
                u32Data[(g_rect_bottom_side_start[i])% (g_transfer_size/4)] = 0x80008000;
                u32Data[(g_rect_bottom_side_start[i] + extend*W_WIDTH)% (g_transfer_size/4)] = 0x80008000;
                
                g_u8RectBottomSide[i] = 1;
            }
        }								
    }		
}

void create_color_table(void) /* Create Color table and data for transfer */
{
    int i,j;	
    uint8_t Color_Y,Color_U,Color_V;
		uint8_t RVal, GVal, BVal;
    for(i=0;i<60;i++)
    {
			for(j=0;j<20;j++)
			{
				RVal = (unsigned char)((float)RGB_ColorTable0[i].R  + ((float)RGB_ColorTable0[i+1].R - (float)RGB_ColorTable0[i].R) /20*j); 	
				GVal = (unsigned char)((float)RGB_ColorTable0[i].G  + ((float)RGB_ColorTable0[i+1].G - (float)RGB_ColorTable0[i].G) /20*j); 	
				BVal = (unsigned char)((float)RGB_ColorTable0[i].B  + ((float)RGB_ColorTable0[i+1].B - (float)RGB_ColorTable0[i].B) /20*j); 
				Color_Y = (unsigned char)(RVal * 0.299 	+ GVal * 0.587	+ BVal * 0.114);
				Color_U = (unsigned char)(RVal * (-0.169) 	- GVal * 0.332 	+ BVal * 0.500 + 128);
				Color_V = (unsigned char)(RVal * 0.5		- GVal * 0.419 	- BVal * 0.0813 + 128);
				YUV_ColorTable[i*20+j].YUVData = (Color_V << 24) | ((Color_Y << 16)) | (Color_U << 8) | (Color_Y);
			}				
    }	
	
}

