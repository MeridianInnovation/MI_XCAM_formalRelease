/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#include <stdlib.h>
#include "DrvI2C.h"
#include "W55FA93_reg.h"
#include "wblib.h"

extern void Vin_Delay(UINT32 nCount);

typedef struct{
	//INT32 base;		/* i2c bus number */
	INT32 openflag;
	INT volatile state;
	INT addr;
	UINT last_error;
	
	UINT subaddr;
	INT subaddr_len;

	UCHAR buffer[I2C_MAX_BUF_LEN];
	UINT volatile pos, len;

}i2c_dev;

static i2c_dev volatile i2c_device;
static INT _i2cSpeed = 100;

//-------------------
// I2C functions
//-------------------

static S_I2C_Channel s_sChannel;
static PFN_DRVI2C_TIMEDELY pfntimedelay=NULL; 

static INT _i2cSetSpeed(INT sp)  
{
	UINT d;
	E_SYS_SRC_CLK eSrcClk;	
	UINT32 u32PllKHz;	
	UINT32 u32SysKHz;
	UINT32 u32CpuKHz;
	UINT32 u32HclkKHz;
	UINT32 u32ApbKHz;	
	
	//if( sp != 100 && sp != 400)
		//return(I2C_ERR_NOTTY);	

	sysGetSystemClock(&eSrcClk,
				 	&u32PllKHz,	
					&u32SysKHz,
					&u32CpuKHz,
					&u32HclkKHz,
					&u32ApbKHz);	
	
	
	d = ((u32ApbKHz)/(sp * 5)) - 1;

	outpw(REG_I2C_DIVIDER, d & 0xffff);

	//MSG2("Set Speed = %d\n", sp);

	return 0;
}

/* init i2c_dev after open */
static VOID _i2cReset(i2c_dev *dev)  
{
	dev->addr = -1;
	dev->last_error = 0;
	dev->subaddr = 0;
	dev->subaddr_len = 0;

	
	return;	
}

INT32 i2cOpen(VOID)  
{
	i2c_dev *dev;
	
	dev = (i2c_dev *)((UINT32)&i2c_device);
	
	if( dev->openflag != 0 )		/* a card slot can open only once */
		return(I2C_ERR_BUSY);
							
	memset(dev, 0, sizeof(i2c_dev));
	
	_i2cReset(dev);
	
	dev->openflag = 1;
	
	return 0;
}

INT32 i2cClose(VOID)  
{
	i2c_dev *dev;

	dev = (i2c_dev *)( (UINT32)&i2c_device );	
		
	dev->openflag = 0;		

	return 0;
}


INT32 i2cIoctl(UINT32 cmd, UINT32 arg0, UINT32 arg1)
{
	i2c_dev *dev;
	
	dev = (i2c_dev *)((UINT32)&i2c_device);
	
	if(dev->openflag == 0)
		return(I2C_ERR_IO);	

	switch(cmd){
		case I2C_IOC_SET_DEV_ADDRESS:
			dev->addr = arg0;
		//	sysprintf("Address : %02x\n", arg0&0xff);
			break;
			
		case I2C_IOC_SET_SPEED:
			_i2cSpeed = (INT)arg0;
			return(_i2cSetSpeed((INT)arg0));			

		case I2C_IOC_SET_SUB_ADDRESS:

			if(arg1 > 4){
				return(I2C_ERR_NOTTY);
			}

			dev->subaddr = arg0;
			dev->subaddr_len = arg1;
			//sysprintf("Sub Address = %02x, length = %d\n",arg0, arg1);
						
			break;

		default:
			return(I2C_ERR_NOTTY);
	}

	return (0);	
}

static VOID _i2cCalcAddr(i2c_dev *dev, INT mode)
{
	INT i;
	UINT subaddr;

	subaddr = dev->subaddr;

	dev->buffer[0] = (((dev->addr << 1) & 0xfe) | I2C_WRITE) & 0xff;

	for(i = dev->subaddr_len; i > 0; i--){
		dev->buffer[i] = subaddr & 0xff;
		subaddr >>= 8;
	}

	if(mode == I2C_STATE_READ){
		i = dev->subaddr_len + 1;
		dev->buffer[i] = (((dev->addr << 1) & 0xfe)) | I2C_READ;
	}

	return;		
}

ERRCODE
DrvI2C_Open(
	UINT32 u32SCKPortIndex,
	UINT32 u32SCKPinMask,
	UINT32 u32SDAPortIndex,
	UINT32 u32SDAPinMask,
	PFN_DRVI2C_TIMEDELY pfnDrvI2C_Delay	
)
{
	// switch pin function	
	s_sChannel.u32SCKPortIndex    = u32SCKPortIndex;
	s_sChannel.u32SCKPinMask     = u32SCKPinMask;
	s_sChannel.u32SDAPortIndex    = u32SDAPortIndex;
	s_sChannel.u32SDAPinMask     = u32SDAPinMask;
	
	// 1.Check I/O pins. If I/O pins are used by other IPs, return error code.
	// 2.Enable IP¡¦s clock
	// 3.Reset IP
	// 4.Configure IP according to inputted arguments.
	// 5.Enable IP I/O pins
	// eq:GPIOB pin1, pin2 as output mode ( DRVGPIO_PIN1 | DRVGPIO_PIN2)
	// eq:Let clock pin and data pin to be high
	
	_DRVI2C_SCK_SETOUT(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask );
	_DRVI2C_SDA_SETOUT(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask );
	_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
	_DRVI2C_SDA_SETHIGH(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
	outp32(REG_GPIOA_PUEN+(s_sChannel.u32SDAPortIndex<<4), inp32(REG_GPIOA_PUEN+(s_sChannel.u32SDAPortIndex<<4))|s_sChannel.u32SDAPinMask);
	
	pfntimedelay=pfnDrvI2C_Delay;
	// 6.Return 0 to present success
	return Successful;

}	//DrvI2C_Open()

void DrvI2C_Close(void)
{
	// 1.Disable IP I/O pins
	// 2.Disable IP¡¦s clock
}
/*
static void DrvI2C_Delay( 
	UINT32 nCount 
)
{
	volatile UINT32 i;
	if (pfntimedelay!=NULL)
	{
		pfntimedelay(nCount);		
	}else{
		for(;nCount!=0;nCount--)
			for(i=0;i<20;i++);
	}
}
*/
void DrvI2C_SendStart(void)
{
	// Assert start bit
	_DRVI2C_SDA_SETOUT(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
	_DRVI2C_SDA_SETHIGH(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);		// serial data pin high
//Vin_Delay(50);
	_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);		// serial clock pin high
//Vin_Delay(50);
	_DRVI2C_SDA_SETLOW(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);		// serial data pin low
//Vin_Delay(50);
	_DRVI2C_SCK_SETLOW(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);		// serial clock pin low
//Vin_Delay(50);
}

void DrvI2C_SendStop(void)
{
	// Assert stop bit
	_DRVI2C_SDA_SETOUT(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
	_DRVI2C_SDA_SETLOW(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);		// serial data pin low
//Vin_Delay(50);
	_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);		// serial clock pin high
//Vin_Delay(50);
	_DRVI2C_SDA_SETHIGH(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);		// serial data pin high
//Vin_Delay(50);
	//DrvI2cDisable(s_sChannel.u32EnablePortIndex, s_sChannel.u32EnablePinMask);
}

//-------------------------
//master write bytes to slave device
ERRCODE 
DrvI2C_WriteByte(
	BOOL bStart, 
	UINT8 u8Data, 
	BOOL bCheckAck, 
	BOOL bStop
)
{
	UINT8   u8DataCount;
	UINT32 	i32HoldPinValue;
	UINT8 saveValue;
	
saveValue = u8Data;
 	
	if(bStart)
		DrvI2C_SendStart();
		
	_DRVI2C_SDA_SETOUT(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
	// Write data to device and the most signification bit(MSB) first
	for ( u8DataCount=0; u8DataCount<8; u8DataCount++ )
	{
		if ( u8Data&0x80 )
			_DRVI2C_SDA_SETHIGH(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
		else
			_DRVI2C_SDA_SETLOW(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
		//Vin_Delay(50);
		_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
		u8Data<<=1;
		//Vin_Delay(50);
		_DRVI2C_SCK_SETLOW(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
		//Vin_Delay(50);
	}
	
	// No Ack 
	if (bCheckAck)
	{
		// Have a Ack
		// Wait Device Ack bit
		_DRVI2C_SDA_SETLOW(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
		_DRVI2C_SDA_SETIN(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
		//Vin_Delay(50);
		_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
		//Vin_Delay(50);
		i32HoldPinValue = _DRVI2C_SDA_GETVALUE(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
		
		_DRVI2C_SCK_SETLOW(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
		//Vin_Delay(50);
		
	}
	
	
	if(bStop)
		DrvI2C_SendStop();
		//Vin_Delay(50);

	if (i32HoldPinValue)
	{
		sysprintf("Ack Fail at u8Data = 0x%x\n", saveValue);
	//	sysGetChar();
		return FALSE;
	}	
	
	return TRUE;
		
}

//-------------------------------
//master read bytes data from slave device
UINT32 
DrvI2C_ReadByte(
	BOOL bStart, 
	PUINT8 pu8ReadData, 
	BOOL bSendAck, 
	BOOL bStop
)
{
	UINT32   u32Data=0;
	UINT8    u8DataCount;
	UINT32 u32ErrCode = Successful;
	
	if(bStart)
		DrvI2C_SendStart();	
	
	_DRVI2C_SDA_SETIN(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
	// Read data from slave device and the most signification bit(MSB) first
	for ( u8DataCount=0; u8DataCount<8; u8DataCount++ )
	{	
		
		u32Data = u32Data<<1;
//Vin_Delay(50);
		_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);		
		if (_DRVI2C_SDA_GETVALUE(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask)==s_sChannel.u32SDAPinMask)
			u32Data = u32Data|0x01;
//Vin_Delay(50);
		_DRVI2C_SCK_SETLOW(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
//Vin_Delay(50);
	}
	*pu8ReadData = u32Data;
	if(bSendAck)
	{
		// Have a Ack
		// write a ACK bit to slave device 
		_DRVI2C_SDA_SETOUT(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
		_DRVI2C_SDA_SETLOW(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
//Vin_Delay(50);
		_DRVI2C_SCK_SETHIGH(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
//Vin_Delay(50);
		_DRVI2C_SCK_SETLOW(s_sChannel.u32SCKPortIndex, s_sChannel.u32SCKPinMask);
//Vin_Delay(50);
		_DRVI2C_SDA_SETLOW(s_sChannel.u32SDAPortIndex, s_sChannel.u32SDAPinMask);
	}
	
	if(bStop)
		DrvI2C_SendStop();
//Vin_Delay(50);
	return u32ErrCode;
}


INT32 i2cWrite(PUINT8 buf, UINT32 len)
{
	i2c_dev *dev;
	INT32 i;
	
	dev = (i2c_dev *)( (UINT32)&i2c_device);	
			
	if(dev->openflag == 0)
		return(I2C_ERR_IO);			

	if(len == 0)
		return 0;
		
	if(len > I2C_MAX_BUF_LEN - 10)
		len = I2C_MAX_BUF_LEN - 10;

	dev->state = I2C_STATE_WRITE;
	dev->pos = 1;
	dev->len = dev->subaddr_len + 1 + len;
	dev->last_error = 0;

	_i2cCalcAddr(dev, I2C_STATE_WRITE);	

	/* Send START & Chip Address & check ACK */
	dev->last_error = DrvI2C_WriteByte(TRUE, dev->buffer[0], TRUE, FALSE);
	if(dev->last_error) 
	{
		sysprintf("Write uAddr fail\n");
		goto exit_write; 
	}
	
	/* Send address & check ACK */	
	for (i = 1; i < (dev->subaddr_len + 1); i++)
	{
		dev->last_error = DrvI2C_WriteByte(FALSE, dev->buffer[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			sysprintf("Write uRegAddr fail\n");
			goto exit_write;  
		}
	}

	/* Send data & check ACK & STOP */	
	for (i = 0; i < len; i++)
	{		
		if(i == (len -1))
			dev->last_error = DrvI2C_WriteByte(FALSE, buf[i], TRUE, TRUE);
		else
			dev->last_error = DrvI2C_WriteByte(FALSE, buf[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			sysprintf("Write uData fail\n");
			goto exit_write;   
		}
	}	
	
	return len;
	
exit_write:

	DrvI2C_SendStop();

	return(dev->last_error);
	
}


INT32 i2cRead(PUINT8 buf, UINT32 len)
{
	i2c_dev *dev;	
	INT32 i;	
			
	dev = (i2c_dev *)( (UINT32)&i2c_device);	
	
	if(dev->openflag == 0)
		return(I2C_ERR_IO);			

	if(len == 0)
		return 0;

		
	if(len > I2C_MAX_BUF_LEN - 10)
		len = I2C_MAX_BUF_LEN - 10;

	dev->state = I2C_STATE_READ;
	dev->pos = 1;
	                                            
	dev->len = dev->subaddr_len + 1 + len + 1;  
	dev->last_error = 0;

	_i2cCalcAddr(dev, I2C_STATE_READ);	

	/* Send START & Chip Address & check ACK */
	dev->last_error = DrvI2C_WriteByte(TRUE, dev->buffer[0], TRUE, FALSE);	
	if(dev->last_error) 
	{
		sysprintf("Write uAddr fail\n");	
		goto exit_read;
	}

	/* Send address & check ACK */
	for (i = 1; i < (dev->subaddr_len + 1); i++)
	{
		dev->last_error = DrvI2C_WriteByte(FALSE, dev->buffer[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			sysprintf("Write uRegAddr fail\n");
			goto exit_read;  
		}
	}
	
	/* Send START & chip address with read & check ACK */
	dev->last_error = DrvI2C_WriteByte(TRUE, dev->buffer[dev->subaddr_len + 1], TRUE, FALSE);	
	if(dev->last_error) 
	{
		sysprintf("Write uAddr fail\n");
		goto exit_read;
	}

	/* Read byte & NAK & STOP  */	
	for (i = 0; i < len; i++)
	{		
		if (i == (len -1))
			dev->last_error = DrvI2C_ReadByte(FALSE, &buf[i], FALSE, TRUE);
		else
			dev->last_error = DrvI2C_ReadByte(FALSE, &buf[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			sysprintf("Read fail\n");
			goto exit_read;
		}
	}	
	
	return len;

exit_read:

	DrvI2C_SendStop();
	
	return(dev->last_error);
	
}






























