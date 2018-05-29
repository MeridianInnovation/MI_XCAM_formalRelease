/******************************************************************************
 * @file     usbd_audio.h
 * @brief    NuMicro series USB driver header file
 * @version  1.0.0
 * @date     22, December, 2013
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __USBD_UAC_H__
#define __USBD_UAC_H__

//#define UVC_FORMAT_BOTH
#define UVC_FORMAT_YUV
//#define UVC_FORMAT_MJPEG

#define USBD_VID        0x0416
#define USBD_PID        0x1284

#define HSHB_MODE
#ifdef HSHB_MODE
//	#define	MAX_PACKET_SIZE_HS     	0x384
//	#define HSHB										0x8
	#define	MAX_PACKET_SIZE_HS     	1022    //1020
	#define HSHB										0x10   //0x08    //0x08
#else
	#define	MAX_PACKET_SIZE_HS     	0x3E8
	#define HSHB										0x0
#endif	

#define   MAX_PACKET_SIZE_FS     1023    //0x3A0


/*-------------------------------------------------------------*/
/* Define EP maximum packet size */
#define CEP_MAX_PKT_SIZE        64
#define CEP_MAX_PKT_SIZE_FS	    64
#define EPA_MAX_PKT_SIZE        MAX_PACKET_SIZE_HS
#define EPA_MAX_PKT_SIZE_FS     MAX_PACKET_SIZE_FS
#define EPB_MAX_PKT_SIZE        16
#define EPB_MAX_PKT_SIZE_FS     16
#define EPC_MAX_PKT_SIZE        64
#define EPC_MAX_PKT_SIZE_FS     64
#define EPD_MAX_PKT_SIZE        64
#define EPD_MAX_PKT_SIZE_FS     64

#define CEP_BUF_BASE      0
#define CEP_BUF_LEN       CEP_MAX_PKT_SIZE
#define EPB_BUF_BASE      (0 + CEP_BUF_LEN)
#define EPB_BUF_LEN       EPB_MAX_PKT_SIZE
#define EPC_BUF_BASE      (EPB_BUF_BASE + EPB_BUF_LEN)
#define EPC_BUF_LEN       EPC_MAX_PKT_SIZE
#define EPD_BUF_BASE      (EPC_BUF_BASE + EPC_BUF_LEN)
#define EPD_BUF_LEN       EPD_MAX_PKT_SIZE
#define EPA_BUF_BASE      (EPD_BUF_BASE + EPD_BUF_LEN)
#define EPA_BUF_LEN       1022*2 // 3072

#define CEP_BUF_BASE_FS   0
#define CEP_BUF_LEN_FS    CEP_MAX_PKT_SIZE_FS
#define EPB_BUF_BASE_FS   (0 + CEP_BUF_LEN_FS)
#define EPB_BUF_LEN_FS    EPB_MAX_PKT_SIZE_FS
#define EPC_BUF_BASE_FS   (EPB_BUF_BASE_FS + EPB_BUF_LEN_FS)
#define EPC_BUF_LEN_FS    EPC_MAX_PKT_SIZE_FS
#define EPD_BUF_BASE_FS   (EPC_BUF_BASE_FS + EPC_BUF_LEN_FS)
#define EPD_BUF_LEN_FS    EPD_MAX_PKT_SIZE_FS
#define EPA_BUF_BASE_FS   (EPD_BUF_BASE_FS + EPD_BUF_LEN_FS)
#define EPA_BUF_LEN_FS    1023 //(0x1000 - CEP_BUF_LEN_FS - EPB_BUF_LEN_FS - EPC_BUF_LEN_FS - EPD_BUF_LEN_FS)

/* Preview Image Frame Index */
#define UVC_FRAME1		1
#define UVC_FRAME2		2
#define UVC_FRAME3		3		

/* Still Image Frame Index */
#define UVC_STILL_FRAME1		1
#define UVC_STILL_FRAME2		2
#define UVC_STILL_FRAME3		3	

#define FRAME_WIDTH1		192
#define FRAME_HEIGHT1		192

#define FRAME_WIDTH2		320
#define FRAME_HEIGHT2		320

#define FRAME_WIDTH3		640
#define FRAME_HEIGHT3		480


/* Image Frame Size */
#define UVC_SIZE_FRAME1		FRAME_WIDTH1 * FRAME_HEIGHT1 * 2 	/* 192*192 */
#define UVC_SIZE_FRAME2		FRAME_WIDTH2 * FRAME_HEIGHT2 * 2	/* 320*320 */
#define UVC_SIZE_FRAME3		FRAME_WIDTH3 * FRAME_HEIGHT3 * 2	/* 640*480 */

/* Preview and Snapshot Format Index */	  	
#define UVC_Format_YUY2		1
#define UVC_Foramt_MJPEG	2


void UVC_InitForHighSpeed(void);
void UVC_InitForFullSpeed(void);
void UVC_ClassRequest(void);
void UVC_SetInterface(uint32_t u32AltInterface);
void UVC_DMACompletion(void);
typedef unsigned int (PFN_UVCD_PUCONTROL_CALLBACK)(unsigned int u32ItemSelect,unsigned int u32Value);

typedef struct{
	int32_t	PU_BACKLIGHT_COMPENSATION_MIN;
	int32_t	PU_BACKLIGHT_COMPENSATION_MAX;
	int32_t	PU_BACKLIGHT_COMPENSATION_DEF;
	int32_t	PU_BRIGHTNESS_MIN;
	int32_t	PU_BRIGHTNESS_MAX;
	int32_t	PU_BRIGHTNESS_DEF;
	int32_t	PU_CONTRAST_MIN;
	int32_t	PU_CONTRAST_MAX;
	int32_t	PU_CONTRAST_DEF;
	int32_t	PU_HUE_MIN;
	int32_t	PU_HUE_MAX;
	int32_t	PU_HUE_DEF;
	int32_t	PU_SATURATION_MIN;
	int32_t	PU_SATURATION_MAX;
	int32_t	PU_SATURATION_DEF;
	int32_t	PU_SHARPNESS_MIN;
	int32_t	PU_SHARPNESS_MAX;
	int32_t	PU_SHARPNESS_DEF;
	int32_t	PU_GAMMA_MIN;
	int32_t	PU_GAMMA_MAX;
	int32_t	PU_GAMMA_DEF;
	int32_t	PU_POWER_LINE_FREQUENCY_MIN;
	int32_t	PU_POWER_LINE_FREQUENCY_MAX;
	int32_t	PU_POWER_LINE_FREQUENCY_DEF; 
}UVC_PU_INFO_T;

__packed  typedef struct _CaptureFilter{
	uint16_t   volatile Brightness;
	uint16_t   volatile Contrast;
	uint16_t   volatile Hue;
	uint8_t    volatile POWER_LINE_FREQUENCY;
	uint16_t   volatile Saturation;
	uint16_t   volatile Sharpness;
	uint16_t   volatile Gamma;
	uint16_t   volatile Backlight;
}CaptureFilter;

__packed typedef struct _VIDEOSTREAMCMDDATA {
    uint16_t   bmHint;
    uint8_t    bFormatIndex;
    uint8_t    bFrameIndex;    
    uint32_t   dwFrameInterval;        
    uint16_t   dwKeyFrameRate;    
    uint16_t   wPFrameRate;    
    uint16_t   wCompQuality;     
    uint16_t   wCompWindowSize;    
    uint16_t   wDelay;    
    uint32_t   dwMaxVideoFrameSize; 
    uint32_t   dwMaxPayloadTransferSize;           
} VIDEOSTREAMCMDDATA;

__packed typedef struct _VIDEOSTREAMSTILLCMDDATA {
    uint8_t    bFormatIndex;
    uint8_t    bFrameIndex;    
    uint8_t 	 bCompressionIndex;
    uint32_t   dwMaxVideoFrameSize;        
    uint32_t  dwMaxPayloadTranferSize;   
} VIDEOSTREAMSTILLCMDDATA;

typedef struct{
	VIDEOSTREAMCMDDATA  		     VSCmdCtlData;
	uint16_t                     u16Dummy;
	VIDEOSTREAMSTILLCMDDATA      VSStillCmdCtlData;
	CaptureFilter 				       CapFilter;	
	uint32_t 	                   u32FrameIndx;		    /* Current Frame index */
	uint32_t 	                   u32FormatIndx;		    /* Current Format index */
	uint32_t 	                   IsoMaxPacketSize[5];
	uint8_t	                     u8ErrCode;			      /* UVC Error Code */
	uint32_t 	                   bChangeData;		      /* Format or Frame index Change glag */	
	PFN_UVCD_PUCONTROL_CALLBACK  *PU_CONTROL;  
}UVC_INFO_T;

typedef struct{
	uint32_t	MaxVideoFrameSize;
	uint32_t 	snapshotMaxVideoFrameSize;	
	uint32_t 	FormatIndex;
	uint32_t	FrameIndex;
	uint32_t 	snapshotFormatIndex;
	uint32_t	snapshotFrameIndex;	
	uint32_t	StillImage;
	uint32_t	bReady;
	uint32_t  appConnected;
}UVC_STATUS_T;

/* Payload Header */
#define UVC_PH_EndOfFrame		0x02
#define UVC_PH_Preview			0x00
#define UVC_PH_Snapshot			0x20

/* Video Class-Specific Request Code */
#define SET_CUR			0x01
#define GET_CUR			0x81
#define GET_MIN			0x82
#define GET_MAX			0x83
#define GET_RES			0x84
#define GET_LEN			0x85
#define GET_INFO		0x86
#define GET_DEF			0x87

/* Control Selector Codes */
/* VideoControl Interface Control Selectors */
#define VC_CONTROL_UNDEFINED				    0x0000
#define VC_VIDEO_POWER_MODE_CONTROL			0x0100
#define VC_REQUEST_ERROR_CODE_CONTROL		0X0200
#define Reserved						          	0x0300

/* Terminal Control Selectors */
#define TE_CONTROL_UNDEFINED				0x00

/* Selector Unit Control Selectors */
#define	SU_CONTROL_UNDEFINED				  0x00
#define SU_INPUT_SELECT_CONTROL				0x01

/* Camera Terminal Control Selectors */
#define CT_CONTROL_UNDEFINED				      0x00
#define CT_SCANNING_MODE_CONTROL			    0x01
#define CT_AE_MODE_CONTROL					      0x02
#define CT_AE_PRIORITY_CONTROL				    0x03
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL	0x04
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL	0x05
#define CT_FOCUS_ABSOLUTE_CONTROL			    0x06
#define CT_FOCUS_RELATIVE_CONTROL		    	0x07
#define CT_FOCUS_AUTP_CONTROL				      0x08
#define CT_IRIS_ABSOLUTE_CONTROL			    0x09
#define CT_IRIS_RELATIVE_CONTROL			    0x0A
#define CT_ZOOM_ABSOLUTE_CONTROL		    	0x0B
#define CT_ZOOM_RELATIVE_CONTROL			    0x0C
#define CT_PANTILT_ABSOLUTE_CONTROL			  0x0D
#define CT_PANTILT_RELATIVE_CONTROL			  0x0E
#define CT_ROLL_ABSOLUTE_CONTROL			    0x0F
#define CT_ROLL_RELATIVE_CONTROL			    0x10
#define CT_PRIVACY_CONTROL					      0x11

/* Processing Unit Control Selectors */
#define	PU_CONTROL_UNDEFINED					          	0x0000
#define	PU_BACKLIGHT_COMPENSATION_CONTROL			    0x0100
#define	PU_BRIGHTNESS_CONTROL						          0x0200
#define	PU_CONTRAST_CONTROL							          0x0300
#define	PU_GAIN_CONTROL								            0x0400	
#define	PU_POWER_LINE_FREQUENCY_CONTROL			     	0x0500
#define	PU_HUE_CONTROL								            0x0600
#define	PU_SATURATION_CONTROL					          	0x0700
#define	PU_SHARPNESS_CONTROL					            0x0800
#define	PU_GAMMA_CONTROL							            0x0900
#define	PU_WHITE_BALANCE_TEMPERATURE_CONTROL	   	0x0A00
#define	PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	0x0B00
#define	PU_WHITE_BALANCE_COMPONENT_CONTROL		   	0x0C00
#define	PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	  0x0D00
#define	PU_DIGITAL_MULTIPLIER_CONTROL				      0x0E00
#define	PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL			  0x0F00
#define	PU_HUE_AUTO_CONTROL							          0x1000
#define	PU_ANALOG_VIDEO_STANDARD_CONTROL			    0x1100
#define	PU_ANALOG_LOCK_STATUS_CONTROL				      0x1200

// define audio for interface 2
#define PU_MUTE_CONTROL								  0x0102
#define PU_VOLUME_CONTROL							  0x0202
/* Extension Unit Control Selectors */
#define XU_CONTROL_UNDEFINED						0x00

/* VideoStreaming Interface Control Selectors */
#define	VS_CONTROL_UNDEFINED						  0x0000
#define	VS_PROBE_CONTROL							    0x0100
#define	VS_COMMIT_CONTROL						  	  0x0200
#define	VS_STILL_PROBE_CONTROL					  0x0300
#define	VS_STILL_COMMIT_CONTROL					  0x0400
#define	VS_STILL_IMAGE_TRIGGER_CONTROL	  0x0500
#define	VS_STREAM_ERROR_CODE_CONTROL			0x0600
#define	VS_GENERATE_KEY_FRAME_CONTROL			0x0700
#define	VS_UPDATE_FRAME_SEGMENT_CONTROL		0x0800
#define	VS_SYNCH_DELAY_CONTROL						0x0900

/* Request Error Code */
#define EC_No_Error			    0x00
#define EC_Not_Ready		    0x01
#define EC_Wrong_State	  	0x02
#define EC_Power			      0x03
#define EC_Out_Of_Range		  0x04
#define EC_Invalid_Uint		  0x05
#define EC_Invalid_Control	0x06
#define EC_Invalid_Request	0x07

/* Define the interrupt In EP number */
#define ISO_IN_EP_NUM    0x01
#define INT_IN_EP_NUM    0x02

#define MFi_BULK_IN_EP_NUM     0x03
#define MFi_BULK_OUT_EP_NUM    0x04

extern __align(4) volatile UVC_STATUS_T uvcStatus;
int uvcdSendImage(uint32_t u32Addr, uint32_t u32transferSize, int bStillImage, int final);
int uvcdIsReady(void);
void uvcdEvent(int index);
void draw_rectangle(uint32_t u32Count);

#endif  /* __USBD_UAC_H_ */

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
