/******************************************************************************
 * @file     descriptors.c
 * @brief    NuMicro series USBD driver source file
 * @version  1.0.0
 * @date     3, May, 2017
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
/*!<Includes */
#include "M480.h"
#include "usbd_video.h"

/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8DeviceDescriptor[] = {
#else
__align(4) uint8_t gu8DeviceDescriptor[] = {
#endif
  LEN_DEVICE,       /* bLength */
  DESC_DEVICE,      /* bDescriptorType */
  0x00, 0x02,       /* bcdUSB */
  0xEF,             /* bDeviceClass */
  0x02,             /* bDeviceSubClass */
  0x01,             /* bDeviceProtocol */
  CEP_MAX_PKT_SIZE, /* bMaxPacketSize0 */
  /* idVendor */
  USBD_VID & 0x00FF,
  (USBD_VID & 0xFF00) >> 8,
  /* idProduct */
  USBD_PID & 0x00FF,
  (USBD_PID & 0xFF00) >> 8,
  0x00, 0x01,       /* bcdDevice */
  0x01,             /* iManufacture */
  0x02,             /* iProduct */
  0x00,             /* iSerialNumber - no serial */
  0x01              /* bNumConfigurations */
};

/*!<USB Qualifier Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8QualifierDescriptor[] = {
#else
__align(4) uint8_t gu8QualifierDescriptor[] = {
#endif
  LEN_QUALIFIER,      /* bLength */
  DESC_QUALIFIER,     /* bDescriptorType */
  0x00, 0x02,         /* bcdUSB */
  0x00,               /* bDeviceClass */
  0x00,               /* bDeviceSubClass */
  0x00,               /* bDeviceProtocol */
  CEP_MAX_PKT_SIZE_FS,/* bMaxPacketSize0 */
  0x01,               /* bNumConfigurations */
  0x00
};

/*!<USB Configure Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8ConfigDescriptor[] = {
#else
__align(4) uint8_t gu8ConfigDescriptor[] = {
#endif
  0x09,    /* bLength */
  0x02,    /* bDescriptorType */
#ifdef UVC_FORMAT_BOTH
  0x1A,    /* wTotalLength */
  0x02,
#elif defined UVC_FORMAT_YUV
  0x66,    /* wTotalLength */
  0x01,
#elif defined UVC_FORMAT_MJPEG
  0x56,    /* wTotalLength */
  0x01,
#endif    
  0x02,    /* bNumInterfaces */
  0x01,    /* bConfigurationValue */
  0x00,    /* iConfiguration */
  0xC0,    /* bmAttributes */
  0x00,    /* bMaxPower */
	
/* Standard Video Interface Collection IAD(interface Association Descriptor) */
  0x08,    /* bLength */
  0x0B,    /* bDescriptorType */
  0x00,    /* bFirstInterface */
  0x02,    /* bInterfaceCount */
  0x0E,    /* bFunctionClass */
  0x03,    /* bFunctionSubClass */
  0x00,    /* bFunctionProtocol */
  0x02,    /* iFunction */
   
/* Standard VideoControl Interface Descriptor */
  0x09,    /* bLength */
  0x04,    /* bDescriptorType */
  0x00,    /* bInterfceNumber */
  0x00,    /* bAlternateSetting */
  0x03,    /* bNumEndpoints */		
  0x0E,    /* bInterfaceClass */
  0x01,    /* bInterfaceSubClass */
  0x00,    /* bInterfaceProtocol */
  0x02,    /* iInterface */
  
/* Class-specific VideoControl Interface Descriptor */
  0x0D,                   /* bLength */
  0x24,                   /* bDescriptorType */
  0x01,                   /* bDescriptorSubType */
  0x00, 0x01,             /* bcdUVC */	
  0x32, 0x00,             /* wTotalLength */
  0x80, 0x8D, 0x5B, 0x00, /* dwClockFrequency */
  0x01,                   /* bInCollection */
  0x01,                   /* baInterfaceNr */
 
/* Output Terminal Descriptor */
  0x09,       /* bLength */
  0x24,       /* bDescriptorType */
  0x03,       /* bDescriptorSubType */
  0x03,       /* bTerminalID */
  0x01, 0x01, /* wTerminalType */	
  0x00,       /* bAssocTerminal */
  0x05,       /* bSourceID */
  0x00,       /* iTerminal */

/* Input Terminal Descriptor (Camera) */
  0x11,       /* bLength */
  0x24,       /* bDescriptorType */
  0x02,       /* bDescriptorSubType */
  0x01,       /* bTerminalID */
  0x01, 0x02, /* wTerminalType */	
  0x00,       /* bAssocTerminal */
  0x00,       /* iTerminal */
  0x00,       /* wObjectiveFocalLengthMin */
  0x00,
  0x00, 0x00, /* wObjectiveFocalLengthMax */	
  0x00, 0x00, /* wOcularFocalLength */	
  0x02,       /* bControlSize */
  0x00, 0x00, /* bmControls */

/* Processing Uint Descriptor */
  0x0B,        /* bLength */
  0x24,        /* bDescriptorType */
  0x05,        /* bDescriptorSubType */
  0x05,        /* bUnitID */
  0x01,        /* bSourceID */
  0x00, 0x00,  /* wMaxMultiplier */
  0x02,        /* bControlSize */
  0x3F, 0x05,  /* bmControls */	
  0x00,        /* iProcessing */    

/* Standard Interrupt Endpoint Descriptor */
  0x07,                       /* bLength */
  0x05,                       /* bDescriptorType */
  (EP_INPUT |INT_IN_EP_NUM),  /* bEndpointAddress */
  0x03,                       /* bmAttributes */
  0x10, 0x00,                 /* wMaxPacketSize */	
  0x06,                       /* bInterval */

/* Class-specific Interrupt Endpoint Descriptor */
  0x05,        /* bLength */
  0x25,        /* bDescriptorType */
  0x03,        /* bDescriptorSubType */
  0x10,	0x00,  /* wMaxPacketSize */
	
  /* EP Descriptor: bulk in. */
  LEN_ENDPOINT,   /* bLength */
  DESC_ENDPOINT,  /* bDescriptorType */
  (EP_INPUT | MFi_BULK_IN_EP_NUM),    /* bEndpointAddress */
  EP_BULK,        /* bmAttributes */
  /* wMaxPacketSize */
  0x40,
  0x00,
  0x00,           /* bInterval */

  /* EP Descriptor: bulk out. */
  LEN_ENDPOINT,   /* bLength */
  DESC_ENDPOINT,  /* bDescriptorType */
  MFi_BULK_OUT_EP_NUM,  /* bEndpointAddress */
  EP_BULK,        /* bmAttributes */
  /* wMaxPacketSize */
  0x40,//0x00,cx
  0x00,//             0x02,cx
  0x00,                                 /* bInterval */

/* Standard VideoStreaming Interface Descriptor */
  0x09,    /* bLength */
  0x04,    /* bDescriptorType */
  0x01,    /* bInterfceNumber */
  0x00,    /* bAlternateSetting */
  0x00,    /* bNumEndpoints */
  0x0E,    /* bInterfaceClass */
  0x02,    /* bInterfaceSubClass */
  0x00,    /* bInterfaceProtocol */
  0x00,    /* iInterface */

/* Class-specific VideoStreaming Header Descriptor */
#ifdef UVC_FORMAT_BOTH
  0x0F,        /* bLength */
#else   
  0x0E,        /* bLength */
#endif
  0x24,        /* bDescriptorType */
  0x01,        /* bDescriptorSubType */
#ifdef UVC_FORMAT_BOTH
  0x02,        /* bNumFormats */             
  0x8B, 0x01,  /* wTotalLength */	
#elif defined UVC_FORMAT_YUV
  0x01,        /* bNumFormats */              
  0xD7, 0x00,  /* wTotalLength */	
#elif defined UVC_FORMAT_MJPEG
  0x01,        /* bNumFormats */             
  0xC7, 0x00,  /* wTotalLength */	
#endif       
  (EP_INPUT | ISO_IN_EP_NUM),  /* bEndpointAddress */

  0x00,        /* bmInfo */
  0x03,        /* bTerminalLink */
  0x02,        /* bStillCaptureMethod */    
  0x01,        /* bTriggerSupport */
  0x00,        /* bTriggerUsage */
  0x01,        /* bControlSize */
#ifdef UVC_FORMAT_BOTH    
  0x00,        /* bmaControls */
#endif    
  0x00,        /* bmaControls */   
#ifndef UVC_FORMAT_MJPEG
/* Uncompressed Video YUV422 */
/* Class-specific VideoStreaming Format Descriptor */
  0x1B,    /* bLength */
  0x24,    /* bDescriptorType */
  0x04,    /* bDescriptorSubType */
  0x01,    /* bFormatIndex */
  0x03,    /* bNumFrameDescriptors */      
  0x59,0x55,0x59,0x32,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71,
  0x10,
  0x01,    /* bDefaultFrameIndex */
  0x00,    /* bAspectRatioX */
  0x00,    /* bAspectRatioY */
  0x00,    /* bmInterlaceFlags */    
  0x00,    /* bCopyProtect */

/* Class-specific VideoStreaming Frame 1 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x05,                                              /* bDescriptorSubType */
  0x01,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH1 & 0xFF, (FRAME_WIDTH1 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME1 & 0xFF,	                           /* dwMinBitRate */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,  
  (UVC_SIZE_FRAME1*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME1*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME1*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME1*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME1 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 2 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x05,                                              /* bDescriptorSubType */
  0x02,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH2 & 0xFF, (FRAME_WIDTH2 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME2 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,  
  (UVC_SIZE_FRAME2*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME2*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME2*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME2*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME2 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 3 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x05,                                              /* bDescriptorSubType */
  0x03,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH3 & 0xFF, (FRAME_WIDTH3 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME3 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,  
  (UVC_SIZE_FRAME3*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME3*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME3*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME3*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME3 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific Still Image Frame Descriptor */
  0x12,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x03,                                              /* bDescriptorSubType */
  0x00,                                              /* bEndpointAddress */
  0x03,                                              /* bNumImageSizePatterns */
  FRAME_WIDTH1 & 0xFF,  (FRAME_WIDTH1 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH2 & 0xFF,  (FRAME_WIDTH2 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH3 & 0xFF,  (FRAME_WIDTH3 >> 8) & 0xFF,  /* wWidth */
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */
  0x00,                                              /* bNumCompressionPtn */           
#endif
#if defined UVC_FORMAT_BOTH || defined UVC_FORMAT_MJPEG
/* MJPEG */
/* Class-specific VideoStreaming Format Descriptor */
  0x0B,   /* bLength */
  0x24,   /* bDescriptorType */
  0x06,   /* bDescriptorSubType */
  0x02,   /* bFormatIndex */
  0x03,   /* bNumFrameDescriptors */
  0x01,   /* bmFlags */
  0x01,   /* bDefaultFrameIndex */
  0x00,   /* bAspectRatioX */
  0x00,   /* bAspectRatioY */
  0x00,   /* bmInterlaceFlags */
  0x00,   /* bCopyProtect */

/* Class-specific VideoStreaming Frame 1 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x07,                                              /* bDescriptorSubType */
  0x01,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH1 & 0xFF, (FRAME_WIDTH1 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME1 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,  
  (UVC_SIZE_FRAME1*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME1*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME1*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME1*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME1 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 2 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x07,                                              /* bDescriptorSubType */
  0x02,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH2 & 0xFF, (FRAME_WIDTH2 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME2 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,  
  (UVC_SIZE_FRAME2*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME2*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME2*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME2*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME2 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 3 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x07,                                              /* bDescriptorSubType */
  0x03,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH3 & 0xFF, (FRAME_WIDTH3 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME3 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,  
  (UVC_SIZE_FRAME3*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME3*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME3*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME3*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME3 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific Still Image Frame Descriptor */
  0x12,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x03,                                              /* bDescriptorSubType */
  0x00,                                              /* bEndpointAddress */
  0x03,                                              /* bNumImageSizePatterns */
  FRAME_WIDTH1 & 0xFF,  (FRAME_WIDTH1 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH2 & 0xFF,  (FRAME_WIDTH2 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH3 & 0xFF,  (FRAME_WIDTH3 >> 8) & 0xFF,  /* wWidth */
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */
  0x00,                                              /* bNumCompressionPtn */ 
#endif
/* Color Matching Descriptor */
  0x06,   /* bLength */
  0x24,   /* bDescriptorType */
  0x0D,   /* bDescriptorSubType */
  0x01,   /* bColorPrimaries */
  0x01,   /* bTransferCharacteristics */
  0x04,   /* bMatrixCoefficients */

/* Standard VideoStreaming Interface Descriptor  (Num 1, Alt 3) */
  0x09,   /* bLength */
  0x04,   /* bDescriptorType */
  0x01,   /* bInterfceNumber */
  0x01,   /* bAlternateSetting */
  0x01,   /* bNumEndpoints */     
  0x0E,   /* bInterfaceClass */
  0x02,   /* bInterfaceSubClass */
  0x00,   /* bInterfaceProtocol */
  0x00,   /* iInterface */
	
/* Standard VideoStreaming Iso Video Data Endpoint Descriptor */
  0x07,                                                                 /* bLength */
  0x05,                                                                 /* bDescriptorType */
  (EP_INPUT | ISO_IN_EP_NUM),                                           /* bEndpointAddress */
  0x05,                                                                 /* bmAttributes */
  MAX_PACKET_SIZE_HS & 0xFF, ((MAX_PACKET_SIZE_HS >> 8) & 0xFF) | HSHB, /* wMaxPacketSize */
  0x01,                                                                 /* bInterval */

/* Standard VideoStreaming Interface Descriptor  (Num 1, Alt 4) */
  0x09,   /* bLength */
  0x04,   /* bDescriptorType */
  0x01,   /* bInterfceNumber */
  0x02,   /* bAlternateSetting */
  0x01,   /* bNumEndpoints */    
  0x0E,   /* bInterfaceClass */
  0x02,   /* bInterfaceSubClass */
  0x00,   /* bInterfaceProtocol */
  0x00,   /* iInterface */

/* Standard VideoStreaming Iso Video Data Endpoint Descriptor */
  0x07,                                                                 /* bLength */
  0x05,                                                                 /* bDescriptorType */
  (EP_INPUT | ISO_IN_EP_NUM),                                           /* bEndpointAddress */
  0x05,                                                                 /* bmAttributes */
  MAX_PACKET_SIZE_HS & 0xFF, ((MAX_PACKET_SIZE_HS >> 8) & 0xFF) | HSHB, /* wMaxPacketSize */
  0x01                                                                  /* bInterval */
};

/*!<USB Other Speed Configure Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8OtherConfigDescriptor[] = {
#else
__align(4) uint8_t gu8OtherConfigDescriptor[] = {
#endif
  0x09,    /* bLength */
  0x02,    /* bDescriptorType */
#ifdef UVC_FORMAT_BOTH
  0x1A,    /* wTotalLength */
  0x02,
#elif defined UVC_FORMAT_YUV
  0x66,    /* wTotalLength */
  0x01,
#elif defined UVC_FORMAT_MJPEG
  0x56,    /* wTotalLength */
  0x01,
#endif    
  0x02,    /* bNumInterfaces */
  0x01,    /* bConfigurationValue */
  0x00,    /* iConfiguration */
  0xC0,    /* bmAttributes */
  0x00,    /* bMaxPower */
	
/* Standard Video Interface Collection IAD(interface Association Descriptor) */
  0x08,    /* bLength */
  0x0B,    /* bDescriptorType */
  0x00,    /* bFirstInterface */
  0x02,    /* bInterfaceCount */
  0x0E,    /* bFunctionClass */
  0x03,    /* bFunctionSubClass */
  0x00,    /* bFunctionProtocol */
  0x02,    /* iFunction */
   
/* Standard VideoControl Interface Descriptor */
  0x09,    /* bLength */
  0x04,    /* bDescriptorType */
  0x00,    /* bInterfceNumber */
  0x00,    /* bAlternateSetting */
  0x03,    /* bNumEndpoints */		
  0x0E,    /* bInterfaceClass */
  0x01,    /* bInterfaceSubClass */
  0x00,    /* bInterfaceProtocol */
  0x02,    /* iInterface */
  
/* Class-specific VideoControl Interface Descriptor */
  0x0D,                   /* bLength */
  0x24,                   /* bDescriptorType */
  0x01,                   /* bDescriptorSubType */
  0x00, 0x01,             /* bcdUVC */	
  0x32, 0x00,             /* wTotalLength */
  0x80, 0x8D, 0x5B, 0x00, /* dwClockFrequency */
  0x01,                   /* bInCollection */
  0x01,                   /* baInterfaceNr */
 
/* Output Terminal Descriptor */
  0x09,       /* bLength */
  0x24,       /* bDescriptorType */
  0x03,       /* bDescriptorSubType */
  0x03,       /* bTerminalID */
  0x01, 0x01, /* wTerminalType */	
  0x00,       /* bAssocTerminal */
  0x05,       /* bSourceID */
  0x00,       /* iTerminal */

/* Input Terminal Descriptor (Camera) */
  0x11,       /* bLength */
  0x24,       /* bDescriptorType */
  0x02,       /* bDescriptorSubType */
  0x01,       /* bTerminalID */
  0x01, 0x02, /* wTerminalType */	
  0x00,       /* bAssocTerminal */
  0x00,       /* iTerminal */
  0x00,       /* wObjectiveFocalLengthMin */
  0x00,
  0x00, 0x00, /* wObjectiveFocalLengthMax */	
  0x00, 0x00, /* wOcularFocalLength */	
  0x02,       /* bControlSize */
  0x00, 0x00, /* bmControls */

/* Processing Uint Descriptor */
  0x0B,        /* bLength */
  0x24,        /* bDescriptorType */
  0x05,        /* bDescriptorSubType */
  0x05,        /* bUnitID */
  0x01,        /* bSourceID */
  0x00, 0x00,  /* wMaxMultiplier */
  0x02,        /* bControlSize */
  0x3F, 0x05,  /* bmControls */	
  0x00,        /* iProcessing */    

/* Standard Interrupt Endpoint Descriptor */
  0x07,                        /* bLength */
  0x05,                        /* bDescriptorType */
 (EP_INPUT |INT_IN_EP_NUM),    /* bEndpointAddress */
  0x03,                        /* bmAttributes */
  0x10, 0x00,                  /* wMaxPacketSize */	
  0x06,                        /* bInterval */

/* Class-specific Interrupt Endpoint Descriptor */
  0x05,        /* bLength */
  0x25,        /* bDescriptorType */
  0x03,        /* bDescriptorSubType */
  0x10,	0x00,  /* wMaxPacketSize */

 /* EP Descriptor: bulk in. */
  LEN_ENDPOINT,   /* bLength */
  DESC_ENDPOINT,  /* bDescriptorType */
  (EP_INPUT | MFi_BULK_IN_EP_NUM),    /* bEndpointAddress */
  EP_BULK,        /* bmAttributes */
  /* wMaxPacketSize */
  0x40,
  0x00,
  0x00,           /* bInterval */

  /* EP Descriptor: bulk out. */
  LEN_ENDPOINT,   /* bLength */
  DESC_ENDPOINT,  /* bDescriptorType */
  MFi_BULK_OUT_EP_NUM,  /* bEndpointAddress */
  EP_BULK,        /* bmAttributes */
  /* wMaxPacketSize */
  0x40,//0x00,cx
  0x00,//             0x02,cx
  0x00,                                 /* bInterval */

/* Standard VideoStreaming Interface Descriptor */
  0x09,    /* bLength */
  0x04,    /* bDescriptorType */
  0x01,    /* bInterfceNumber */
  0x00,    /* bAlternateSetting */
  0x00,    /* bNumEndpoints */
  0x0E,    /* bInterfaceClass */
  0x02,    /* bInterfaceSubClass */
  0x00,    /* bInterfaceProtocol */
  0x00,    /* iInterface */

/* Class-specific VideoStreaming Header Descriptor */
#ifdef UVC_FORMAT_BOTH
  0x0F,        /* bLength */
#else   
  0x0E,        /* bLength */
#endif
  0x24,        /* bDescriptorType */
  0x01,        /* bDescriptorSubType */
#ifdef UVC_FORMAT_BOTH
  0x02,        /* bNumFormats */             
  0x8B, 0x01,  /* wTotalLength */	
#elif defined UVC_FORMAT_YUV
  0x01,        /* bNumFormats */              
  0xD7, 0x00,  /* wTotalLength */	
#elif defined UVC_FORMAT_MJPEG
  0x01,        /* bNumFormats */             
  0xC7, 0x00,  /* wTotalLength */	
#endif       
  (EP_INPUT | ISO_IN_EP_NUM), /* bEndpointAddress */

  0x00,        /* bmInfo */
  0x03,        /* bTerminalLink */
  0x02,        /* bStillCaptureMethod */    
  0x01,        /* bTriggerSupport */
  0x00,        /* bTriggerUsage */
  0x01,        /* bControlSize */
#ifdef UVC_FORMAT_BOTH    
  0x00,        /* bmaControls */
#endif    
  0x00,        /* bmaControls */   
#ifndef UVC_FORMAT_MJPEG
/* Uncompressed Video YUV422 */
/* Class-specific VideoStreaming Format Descriptor */
  0x1B,    /* bLength */
  0x24,    /* bDescriptorType */
  0x04,    /* bDescriptorSubType */
  0x01,    /* bFormatIndex */
  0x03,    /* bNumFrameDescriptors */      
  0x59,0x55,0x59,0x32,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71,
  0x10,
  0x01,    /* bDefaultFrameIndex */
  0x00,    /* bAspectRatioX */
  0x00,    /* bAspectRatioY */
  0x00,    /* bmInterlaceFlags */    
  0x00,    /* bCopyProtect */

/* Class-specific VideoStreaming Frame 1 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x05,                                              /* bDescriptorSubType */
  0x01,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH1 & 0xFF, (FRAME_WIDTH1 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME1 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,  
  (UVC_SIZE_FRAME1*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME1*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME1*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME1*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME1 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 2 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x05,                                              /* bDescriptorSubType */
  0x02,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH2 & 0xFF, (FRAME_WIDTH2 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME2 & 0xFF,                            /* dwMinBitRate */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,  
  (UVC_SIZE_FRAME2*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME2*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME2*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME2*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME2 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 3 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x05,                                              /* bDescriptorSubType */
  0x03,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH3 & 0xFF, (FRAME_WIDTH3 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME3 & 0xFF,	                         /* dwMinBitRate */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,  
  (UVC_SIZE_FRAME3*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME3*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME3*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME3*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME3 & 0xFF,	                         /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,	                         /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,			                 /* dwMinFrameInterval */

/* Class-specific Still Image Frame Descriptor */
  0x12,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x03,                                              /* bDescriptorSubType */
  0x00,                                              /* bEndpointAddress */
  0x03,                                              /* bNumImageSizePatterns */
  FRAME_WIDTH1 & 0xFF,  (FRAME_WIDTH1 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH2 & 0xFF,  (FRAME_WIDTH2 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH3 & 0xFF,  (FRAME_WIDTH3 >> 8) & 0xFF,  /* wWidth */
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */
  0x00,                                              /* bNumCompressionPtn */           
#endif
#if defined UVC_FORMAT_BOTH || defined UVC_FORMAT_MJPEG
/* MJPEG */
/* Class-specific VideoStreaming Format Descriptor */
  0x0B,   /* bLength */
  0x24,   /* bDescriptorType */
  0x06,   /* bDescriptorSubType */
  0x02,   /* bFormatIndex */
  0x03,   /* bNumFrameDescriptors */
  0x01,   /* bmFlags */
  0x01,   /* bDefaultFrameIndex */
  0x00,   /* bAspectRatioX */
  0x00,   /* bAspectRatioY */
  0x00,   /* bmInterlaceFlags */
  0x00,   /* bCopyProtect */

/* Class-specific VideoStreaming Frame 1 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x07,                                              /* bDescriptorSubType */
  0x01,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH1 & 0xFF, (FRAME_WIDTH1 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME1 & 0xFF,	                         /* dwMinBitRate */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,  
  (UVC_SIZE_FRAME1*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME1*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME1*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME1*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME1 & 0xFF,	                         /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME1 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME1 >>16) & 0xFF,
  (UVC_SIZE_FRAME1 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 2 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x07,                                              /* bDescriptorSubType */
  0x02,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH2 & 0xFF, (FRAME_WIDTH2 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME2 & 0xFF,	                         /* dwMinBitRate */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,  
  (UVC_SIZE_FRAME2*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME2*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME2*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME2*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME2 & 0xFF,	                         /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME2 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME2 >>16) & 0xFF,
  (UVC_SIZE_FRAME2 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific VideoStreaming Frame 3 Descriptor */
  0x32,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x07,                                              /* bDescriptorSubType */
  0x03,                                              /* bFrameIndex */           
  0x00,                                              /* bmCapabilities */    
  FRAME_WIDTH3 & 0xFF, (FRAME_WIDTH3 >> 8) & 0xFF,   /* wWidth */	
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */	      	     	
  UVC_SIZE_FRAME3 & 0xFF,	                           /* dwMinBitRate */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,  
  (UVC_SIZE_FRAME3*30) & 0xFF,	                     /* dwMaxBitRate */
  ((UVC_SIZE_FRAME3*30) >> 8) & 0xFF,
  ((UVC_SIZE_FRAME3*30) >>16) & 0xFF,
  ((UVC_SIZE_FRAME3*30)	>>24) & 0xFF,	
  UVC_SIZE_FRAME3 & 0xFF,                            /* dwMaxVideoFrameBufSize */
  (UVC_SIZE_FRAME3 >> 8) & 0xFF,    
  (UVC_SIZE_FRAME3 >>16) & 0xFF,
  (UVC_SIZE_FRAME3 >>24) & 0xFF,		
  0x15, 0x16, 0x05, 0x00,                            /* dwDefaultFrameInterval */
  0x06,                                              /* bFrameIntervalType */
  0x15, 0x16, 0x05, 0x00,                            /* dwMinFrameInterval */
  0x20, 0xA1, 0x07, 0x00,                            /* dwMinFrameInterval */
  0x2A, 0x2C, 0x0A,	0x00,                            /* dwMinFrameInterval */
  0x40, 0x42, 0x0F, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x84, 0x1E, 0x00,                            /* dwMinFrameInterval */
  0x80, 0x96, 0x98, 0x00,                            /* dwMinFrameInterval */

/* Class-specific Still Image Frame Descriptor */
  0x12,                                              /* bLength */
  0x24,                                              /* bDescriptorType */
  0x03,                                              /* bDescriptorSubType */
  0x00,  			                                       /* bEndpointAddress */
  0x03,                                              /* bNumImageSizePatterns */
  FRAME_WIDTH1 & 0xFF,  (FRAME_WIDTH1 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT1 & 0xFF, (FRAME_HEIGHT1 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH2 & 0xFF,  (FRAME_WIDTH2 >> 8) & 0xFF,	 /* wWidth */
  FRAME_HEIGHT2 & 0xFF, (FRAME_HEIGHT2 >> 8) & 0xFF, /* wHeight */
  FRAME_WIDTH3 & 0xFF,  (FRAME_WIDTH3 >> 8) & 0xFF,  /* wWidth */
  FRAME_HEIGHT3 & 0xFF, (FRAME_HEIGHT3 >> 8) & 0xFF, /* wHeight */
  0x00,                                              /* bNumCompressionPtn */ 
#endif
/* Color Matching Descriptor */
  0x06,   /* bLength */
  0x24,   /* bDescriptorType */
  0x0D,   /* bDescriptorSubType */
  0x01,   /* bColorPrimaries */
  0x01,   /* bTransferCharacteristics */
  0x04,   /* bMatrixCoefficients */

/* Standard VideoStreaming Interface Descriptor  (Num 1, Alt 3) */
  0x09,   /* bLength */
  0x04,   /* bDescriptorType */
  0x01,   /* bInterfceNumber */
  0x01,   /* bAlternateSetting */
  0x01,   /* bNumEndpoints */     
  0x0E,   /* bInterfaceClass */
  0x02,   /* bInterfaceSubClass */
  0x00,   /* bInterfaceProtocol */
  0x00,   /* iInterface */
	
/* Standard VideoStreaming Iso Video Data Endpoint Descriptor */
  0x07,                                                        /* bLength */
  0x05,                                                        /* bDescriptorType */
  (EP_INPUT | ISO_IN_EP_NUM),                                  /* bEndpointAddress */
  0x05,                                                        /* bmAttributes */
  MAX_PACKET_SIZE_FS & 0xFF, (MAX_PACKET_SIZE_FS >> 8) & 0xFF, /* wMaxPacketSize */
  0x01,                                                        /* bInterval */

/* Standard VideoStreaming Interface Descriptor  (Num 1, Alt 4) */
  0x09,   /* bLength */
  0x04,   /* bDescriptorType */
  0x01,   /* bInterfceNumber */
  0x02,   /* bAlternateSetting */
  0x01,   /* bNumEndpoints */    
  0x0E,   /* bInterfaceClass */
  0x02,   /* bInterfaceSubClass */
  0x00,   /* bInterfaceProtocol */
  0x00,   /* iInterface */

/* Standard VideoStreaming Iso Video Data Endpoint Descriptor */
  0x07,                                                        /* bLength */
  0x05,                                                        /* bDescriptorType */
  (EP_INPUT | ISO_IN_EP_NUM),                                  /* bEndpointAddress */
  0x05,                                                        /* bmAttributes */
  MAX_PACKET_SIZE_FS & 0xFF, (MAX_PACKET_SIZE_FS >> 8) & 0xFF, /* wMaxPacketSize */
  0x01,                                                        /* bInterval */
};


/*!<USB Language String Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8StringLang[4] = {
#else
__align(4) uint8_t gu8StringLang[4] = {
#endif
  4,              /* bLength */
  DESC_STRING,    /* bDescriptorType */
  0x09, 0x04
};

/*!<USB Vendor String Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8VendorStringDesc[] = {
#else
__align(4) uint8_t gu8VendorStringDesc[] = {
#endif
  16,
  DESC_STRING,
  'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8ProductStringDesc[] = {
#else
__align(4) uint8_t gu8ProductStringDesc[] = {
#endif
  20,
  DESC_STRING,
  'U', 0, 'S', 0, 'B', 0, ' ', 0, 'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0
};

/*!<USB Product String Descriptor */
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t gu8ChipStringDesc[] = {
#else
__align(4) uint8_t gu8ChipStringDesc[] = {
#endif
  17,
  DESC_STRING,
  'N', 0, 'U', 0, 'C', 0, '5', 0, '0', 0, '5'
};


uint8_t *gpu8UsbString[5] = {
  gu8StringLang,
  gu8VendorStringDesc,
  gu8ProductStringDesc,
  gu8ChipStringDesc,
  NULL,
};

uint8_t *gu8UsbHidReport[3] = {
  NULL,
  NULL,
  NULL,
};

uint32_t gu32UsbHidReportLen[3] = {
  0,
  0,
  0,
};

S_HSUSBD_INFO_T gsHSInfo = {
  gu8DeviceDescriptor,
  gu8ConfigDescriptor,
  gpu8UsbString,
  gu8QualifierDescriptor,
  gu8OtherConfigDescriptor,
  gu8OtherConfigDescriptor,
  gu8OtherConfigDescriptor,
  gu8UsbHidReport,
  gu32UsbHidReportLen,
  NULL,
  NULL,
};
