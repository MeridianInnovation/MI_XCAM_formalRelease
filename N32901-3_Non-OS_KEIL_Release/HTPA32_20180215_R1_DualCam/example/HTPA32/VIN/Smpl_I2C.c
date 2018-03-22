#include "wblib.h"
#include "W55FA93_VideoIn.h"
#include "W55FA93_GPIO.h"
#include "DrvI2C.h"
#include "vin_demo.h"

BOOL 
I2C_Write_8bitSlaveAddr_8bitReg_8bitData(UINT8 uAddr, UINT8 uRegAddr, UINT8 uData)
{
    // 3-Phase(ID address, regiseter address, data(8bits)) write transmission
    volatile u32Delay = 0x100;
    DrvI2C_SendStart();
    while(u32Delay--);		

    if ( (DrvI2C_WriteByte(FALSE, uAddr,TRUE,FALSE)==FALSE) ||			// Write ID address to sensor
        (DrvI2C_WriteByte(FALSE, uRegAddr,TRUE,FALSE)==FALSE) ||	// Write register address to sensor
        (DrvI2C_WriteByte(FALSE, uData,TRUE,FALSE)==FALSE) )		// Write data to sensor	 
	    {
        DrvI2C_SendStop();
        return FALSE;
    }
    DrvI2C_SendStop();

    if (uRegAddr==0x12 && (uData&0x80)!=0)
    {
 //       Delay(1000);			
    }
    return TRUE;
}

UINT8 I2C_Read_8bitSlaveAddr_8bitReg_8bitData(UINT8 uAddr, UINT8 uRegAddr)
{
    UINT8 u8Data;

    // 2-Phase(ID address, register address) write transmission
    DrvI2C_SendStart();
    DrvI2C_WriteByte(FALSE, uAddr,TRUE, FALSE);		// Write ID address to sensor
    DrvI2C_WriteByte(FALSE, uRegAddr,TRUE, FALSE);	// Write register address to sensor
    DrvI2C_SendStop();

    //sysDelay(1);
		
    // 2-Phase(ID-address, data(8bits)) read transmission
    DrvI2C_SendStart();
    DrvI2C_WriteByte(FALSE,uAddr|0x01,TRUE, FALSE);		// Write ID address to sensor
    DrvI2C_ReadByte(FALSE, &u8Data,FALSE, FALSE);		// Read data from sensor
    DrvI2C_SendStop();
    return u8Data;
}


