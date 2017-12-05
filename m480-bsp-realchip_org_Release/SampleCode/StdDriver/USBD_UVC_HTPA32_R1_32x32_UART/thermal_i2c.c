#include "M480.h"
#include "i2c.h"
#include "thermal_i2c.h"

/* @brief      Write a byte to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  data            Write a byte data to Slave
  * 
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master write a byte data to Slave. 
  *
  */

uint8_t I2C_WriteByte(I2C_T *i2c, uint8_t u8SlaveAddr, const uint8_t data)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
  
    I2C_START(i2c);
    while(u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                           /* Clear SI */            
                break;
            case 0x18:                                           /* Slave Address ACK */
                I2C_SET_DATA(i2c, data);                         /* Write data to I2CDAT */
                break;
            case 0x20:                                           /* Slave Address NACK */
            case 0x30:                                           /* Master transmit data NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                
                u8Err = 1;                
                break;             
            case 0x28:
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                           /* Arbitration Lost */
            default:                                             /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                  
                u8Err = 1;
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */ 
    } 
    return (u8Err | u8Xfering);                                  /* return (Success)/(Fail) status */
}

/**
  * @brief      Write multi bytes to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  *data           Pointer to array to write data to Slave
  * @param[in]  u32wLen         How many bytes need to write to Slave
  * 
  * @return     A length of how many bytes have been transmitted.
  *
  * @details    The function is used for I2C Master write multi bytes data to Slave. 
  *
  */

uint32_t I2C_WriteMultiBytes(I2C_T *i2c, uint8_t u8SlaveAddr, const uint8_t *data, uint32_t u32wLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32txLen = 0;
  
    I2C_START(i2c);                                              /* Send START */
    while(u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                           /* Clear SI */
                break;
            case 0x18:                                           /* Slave Address ACK */
            case 0x28:                                           
                if(u32txLen<u32wLen)
                    I2C_SET_DATA(i2c, data[u32txLen++]);                /* Write Data to I2CDAT */
                else
                {
                    u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                   /* Clear SI and send STOP */
                    u8Xfering = 0;
                }
                break;
            case 0x20:                                           /* Slave Address NACK */
            case 0x30:                                           /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                
                u8Err = 1;                
                break;                 
            case 0x38:                                           /* Arbitration Lost */
            default:                                             /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                  
                u8Err = 1;
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */ 
    }   
    return u32txLen;                                             /* Return bytes length that have been transmitted */
}

/**
  * @brief      Specify a byte register address and write a byte to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 byte) of data write to
  * @param[in]  data            A byte data to write it to Slave
  * 
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master specify a address that data write to in Slave.
  *
  */

uint8_t I2C_WriteByteOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, const uint8_t data)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32txLen = 0;
  
    I2C_START(i2c);                                              /* Send START */
    while(u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Send Slave address with write bit */
                u8Ctrl = I2C_CTL_SI;                           /* Clear SI */
                break;
            case 0x18:                                           /* Slave Address ACK */
                I2C_SET_DATA(i2c, u8DataAddr);                   /* Write Lo byte address of register */ 
                break;
            case 0x20:                                           /* Slave Address NACK */
            case 0x30:                                           /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                
                u8Err = 1;                
                break;             
            case 0x28:
                if(u32txLen < 1)
                {
                    I2C_SET_DATA(i2c, data);
                    u32txLen++;
                }
                else
                {
                    u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                   /* Clear SI and send STOP */
                    u8Xfering = 0;
                }                 
                break;
            case 0x38:                                           /* Arbitration Lost */
            default:                                             /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                  
                u8Err = 1;
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */        
    }
    return (u8Err | u8Xfering);                                  /* return (Success)/(Fail) status */
}


/**
  * @brief      Specify a byte register address and write multi bytes to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 byte) of data write to
  * @param[in]  *data           Pointer to array to write data to Slave
  * @param[in]  u32wLen         How many bytes need to write to Slave
  * 
  * @return     A length of how many bytes have been transmitted.
  *
  * @details    The function is used for I2C Master specify a byte address that multi data bytes write to in Slave.
  *
  */

uint32_t I2C_WriteMultiBytesOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, const uint8_t *data, uint32_t u32wLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32txLen = 0;
  
    I2C_START(i2c);                                              /* Send START */
    while(u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                 
                break;
            case 0x18:                                           /* Slave Address ACK */
                I2C_SET_DATA(i2c, u8DataAddr);                   /* Write Lo byte address of register */
                break;
            case 0x20:                                           /* Slave Address NACK */
            case 0x30:                                           /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                
                u8Err = 1;                
                break;             
            case 0x28:
                if(u32txLen < u32wLen)
                    I2C_SET_DATA(i2c, data[u32txLen++]);
                else
                {
                    u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                   /* Clear SI and send STOP */
                    u8Xfering = 0;  
                }                 
                break;
            case 0x38:                                           /* Arbitration Lost */
            default:                                             /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                       /* Clear SI and send STOP */                  
                u8Err = 1;  
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */ 
    } 
    
    return u32txLen;                                             /* Return bytes length that have been transmitted */
}

/**
  * @brief      Specify two bytes register address and Write a byte to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address (2 byte) of data write to
  * @param[in]  data            Write a byte data to Slave
  * 
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master specify two bytes address that data write to in Slave.
  *
  */

uint8_t I2C_WriteByteTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr, const uint8_t data)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Addr = 1, u8Ctrl = 0;
    uint32_t u32txLen = 0;
  
    I2C_START(i2c);                                                         /* Send START */
    while(u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
                break;
            case 0x18:                                                      /* Slave Address ACK */
                I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
                break; 
            case 0x20:                                                      /* Slave Address NACK */
            case 0x30:                                                      /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;                
                break;              
            case 0x28:
                if(u8Addr)
                {
                    I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                    u8Addr = 0;
                }
                else if((u32txLen < 1) && (u8Addr == 0))
                {
                    I2C_SET_DATA(i2c, data);
                    u32txLen++;
                }
                else
                {
                    u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                              /* Clear SI and send STOP */
                    u8Xfering = 0;
                }
                break;
            case 0x38:                                                      /* Arbitration Lost */
            default:                                                        /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                  
                u8Err = 1;
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */  
    }
    return (u8Err | u8Xfering);                                             /* return (Success)/(Fail) status */
}


/**
  * @brief      Specify two bytes register address and write multi bytes to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address (2 bytes) of data write to
  * @param[in]  *data           Pointer to array to write data to Slave
  * @param[in]  u32wLen         How many bytes need to write to Slave
  * 
  * @return     A length of how many bytes have been transmitted.
  *
  * @details    The function is used for I2C Master specify a byte address that multi data write to in Slave.
  *
  */

uint32_t I2C_WriteMultiBytesTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr, const uint8_t *data, uint32_t u32wLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Addr = 1, u8Ctrl = 0;
    uint32_t u32txLen = 0;
   
    I2C_START(i2c);                                                         /* Send START */
    while(u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */                 
                break;
            case 0x18:                                                      /* Slave Address ACK */
                I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
                break;
            case 0x20:                                                      /* Slave Address NACK */
            case 0x30:                                                      /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;                
                break;  
            case 0x28:
                if(u8Addr)
                {
                    I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                    u8Addr = 0;
                }
                else if((u32txLen < u32wLen) && (u8Addr == 0))
                    I2C_SET_DATA(i2c, data[u32txLen++]);                           /* Write data to Register I2CDAT*/
                else
                {
                    u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                              /* Clear SI and send STOP */
                    u8Xfering = 0;
                }                 
                break;
            case 0x38:                                                      /* Arbitration Lost */
            default:                                                        /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                  
                u8Err = 1;
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */ 
    }
    return u32txLen;                                                        /* Return bytes length that have been transmitted */
}

/**
  * @brief      Read a byte from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * 
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master to read a byte data from Slave. 
  *
  */
uint8_t I2C_ReadByte(I2C_T *i2c, uint8_t u8SlaveAddr)
{
    uint8_t u8Xfering = 1, u8Err = 0, rdata = 0, u8Ctrl = 0;
  
    I2C_START(i2c);                                                /* Send START */ 
    while(u8Xfering && (u8Err == 0))
    { 
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                             /* Clear SI */                      
                break;
            case 0x40:                                             /* Slave Address ACK */
                u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
                break;
            case 0x48:                                             /* Slave Address NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                
                u8Err = 1;                
                break;            
            case 0x58:
                rdata = (unsigned char) I2C_GET_DATA(i2c);         /* Receive Data */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                             /* Arbitration Lost */
            default:                                               /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                  
                u8Err = 1;
                break;                
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                          /* Write controlbit to I2C_CTL register */
    }
    if(u8Err)
        rdata = 0;                                                 /* If occurs error, return 0 */    
    return rdata;                                                  /* Return read data */     
}


/**
  * @brief      Read multi bytes from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[out] *rdata          Point to array to store data from Slave 
  * @param[in]  u32rLen         How many bytes need to read from Slave
  * 
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master to read multi data bytes from Slave. 
  *
  *
  */
uint32_t I2C_ReadMultiBytes(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t *rdata, uint32_t u32rLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0; 
    uint32_t u32rxLen = 0;
  
    I2C_START(i2c);                                                /* Send START */ 
    while(u8Xfering && (u8Err == 0))
    { 
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                             /* Clear SI */                   
                break;
            case 0x40:                                             /* Slave Address ACK */
                u8Ctrl = (I2C_CTL_SI | I2C_CTL_AA);                          /* Clear SI and set ACK */
                break;
            case 0x48:                                             /* Slave Address NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                
                u8Err = 1;                
                break;               
            case 0x50:
                rdata[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);    /* Receive Data */
                if(u32rxLen<(u32rLen-1))
                {
                    u8Ctrl = (I2C_CTL_SI | I2C_CTL_AA);                             /* Clear SI and set ACK */                          
                }
                else
                {
                    u8Ctrl = I2C_CTL_SI;                                /* Clear SI */  
                }
                break;
            case 0x58:
                rdata[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);    /* Receive Data */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                                    /* Arbitration Lost */
            default:                                                      /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                /* Clear SI and send STOP */                  
                u8Err = 1;
                break;               
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                 /* Write controlbit to I2C_CTL register */
    }     
    return u32rxLen;                                                      /* Return bytes length that have been received */
}


/**
  * @brief      Specify a byte register address and read a byte from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address(1 byte) of data read from
  * 
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master specify a byte address that a data byte read from Slave.
  *
  *
  */
uint8_t I2C_ReadByteOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr)
{
    uint8_t u8Xfering = 1, u8Err = 0, rdata = 0, u8Ctrl = 0;
  
    I2C_START(i2c);                                                /* Send START */ 
    while(u8Xfering && (u8Err == 0))
    { 
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1));      /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;  						/* Clear SI */   
                break;
            case 0x18:                                             /* Slave Address ACK */
                I2C_SET_DATA(i2c, u8DataAddr);                     /* Write Lo byte address of register */
                u8Ctrl = I2C_CTL_SI;    
							break;
            case 0x20:                                             /* Slave Address NACK */
            case 0x30:                                             /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                
                u8Err = 1;                
                break;               
            case 0x28:
                u8Ctrl = (I2C_CTL_STA | I2C_CTL_SI);                         /* Send repeat START */
                break;
            case 0x10:
                I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */            	
                u8Ctrl = I2C_CTL_SI;            	               /* Clear SI */  
                break;
            case 0x40:                                             /* Slave Address ACK */
                u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
                break; 
            case 0x48:                                             /* Slave Address NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                
                u8Err = 1;  
                break;            
            case 0x58:
                rdata = (uint8_t) I2C_GET_DATA(i2c);               /* Receive Data */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                             /* Arbitration Lost */
            default:                                               /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                  
                u8Err = 1; 
                break;                
        }
				
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                          /* Write controlbit to I2C_CTL register */
    }
    if(u8Err)
        rdata = 0;                                                 /* If occurs error, return 0 */    
    return rdata;                                                  /* Return read data */
}

/**
  * @brief      Specify a byte register address and read multi bytes from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 bytes) of data read from
  * @param[out] *rdata          Point to array to store data from Slave 
  * @param[in]  u32rLen         How many bytes need to read from Slave
  * 
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master specify a byte address that multi data bytes read from Slave.
  *
  *
  */
uint32_t I2C_ReadMultiBytesOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, uint8_t *rdata, uint32_t u32rLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0; 
    uint32_t u32rxLen = 0;
  
    I2C_START(i2c);                                                /* Send START */ 
    while(u8Xfering && (u8Err == 0))
    { 
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));      /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                             /* Clear SI */                   
                break;
            case 0x18:                                             /* Slave Address ACK */
                I2C_SET_DATA(i2c, u8DataAddr);                     /* Write Lo byte address of register */
                break;
            case 0x20:                                             /* Slave Address NACK */
            case 0x30:                                             /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                
                u8Err = 1;                
                break;               
            case 0x28:
                u8Ctrl = (I2C_CTL_STA | I2C_CTL_SI);                         /* Send repeat START */
                break;
            case 0x10:
                I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                             /* Clear SI */     
                break;
            case 0x40:                                             /* Slave Address ACK */
                u8Ctrl = (I2C_CTL_SI | I2C_CTL_AA);                          /* Clear SI and set ACK */
                break;
            case 0x48:                                             /* Slave Address NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                
                u8Err = 1;                
                break;            
            case 0x50:
                rdata[u32rxLen++] = (uint8_t) I2C_GET_DATA(i2c);   /* Receive Data */
                if(u32rxLen<(u32rLen-1))
                    u8Ctrl = (I2C_CTL_SI | I2C_CTL_AA);                      /* Clear SI and set ACK */
                else
                    u8Ctrl = I2C_CTL_SI;                         /* Clear SI */
                break;              
            case 0x58:
                rdata[u32rxLen++] = (uint8_t) I2C_GET_DATA(i2c);   /* Receive Data */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                             /* Arbitration Lost */
            default:                                               /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                         /* Clear SI and send STOP */                  
                u8Err = 1;
                break;                
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                          /* Write controlbit to I2C_CTL register */
    } 
    return u32rxLen;                                               /* Return bytes length that have been received */
}

/**
  * @brief      Specify two bytes register address and read a byte from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address(2 byte) of data read from
  * 
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master specify two bytes address that a data byte read from Slave.
  *
  *
  */
uint8_t I2C_ReadByteTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr)
{
    uint8_t u8Xfering = 1, u8Err = 0, rdata = 0, u8Addr = 1, u8Ctrl = 0;
 
    I2C_START(i2c);                                                         /* Send START */
    while(u8Xfering && (u8Err == 0))
    { 
        I2C_WAIT_READY(i2c);      
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */                   
                break;
            case 0x18:                                                      /* Slave Address ACK */
                I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
                break;
            case 0x20:                                                      /* Slave Address NACK */
            case 0x30:                                                      /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;                
                break;                
            case 0x28:
                if(u8Addr)
                {
                    I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                    u8Addr = 0; 
                }
                else            
                    u8Ctrl = (I2C_CTL_STA | I2C_CTL_SI);                              /* Clear SI and send repeat START */
                break;
            case 0x10:           
                I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));             /* Write SLA+R to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */                   
                break;
            case 0x40:                                                      /* Slave Address ACK */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
                break; 
            case 0x48:                                                      /* Slave Address NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;                
                break;            
            case 0x58:
                rdata = (unsigned char) I2C_GET_DATA(i2c);                  /* Receive Data */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                                      /* Arbitration Lost */
            default:                                                        /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;
                break;
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */
    }
    if(u8Err)
        rdata = 0;                                                          /* If occurs error, return 0 */    
    return rdata;                                                           /* Return read data */
}

/**
  * @brief      Specify two bytes register address and read multi bytes from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address (2 bytes) of data read from
  * @param[out] *rdata          Point to array to store data from Slave 
  * @param[in]  u32rLen         How many bytes need to read from Slave
  * 
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master specify two bytes address that multi data bytes read from Slave.
  *
  *
  */
uint32_t I2C_ReadMultiBytesTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr, uint8_t *rdata, uint32_t u32rLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Addr = 1, u8Ctrl = 0; 
    uint32_t u32rxLen = 0;
  
    I2C_START(i2c);                                                         /* Send START */
    while(u8Xfering && (u8Err == 0))
    { 
			
        I2C_WAIT_READY(i2c);
        switch(I2C_GET_STATUS(i2c))
        {
            case 0x08:
                I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */                 
                break;
            case 0x18:                                                      /* Slave Address ACK */
                I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
                break;
            case 0x20:                                                      /* Slave Address NACK */
            case 0x30:                                                      /* Master transmit data NACK */                
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;                
                break;               
            case 0x28:
                if(u8Addr)
                {
                    I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                    u8Addr = 0;                 
                }
                else
								{
                    CLK_SysTickDelay(1);
										u8Ctrl = (I2C_CTL_STA | I2C_CTL_SI);                              /* Clear SI and send repeat START */
								}
                break;
            case 0x10:               
                I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));             /* Write SLA+R to Register I2CDAT */
                u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */            
                break;
            case 0x40:                                                      /* Slave Address ACK */
                u8Ctrl = (I2C_CTL_SI | I2C_CTL_AA);                                   /* Clear SI and set ACK */
                break;
            case 0x48:                                                      /* Slave Address NACK */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                
                u8Err = 1;                
                break;            
            case 0x50:
                rdata[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);      /* Receive Data */
                if(u32rxLen<(u32rLen-1))
                    u8Ctrl = (I2C_CTL_SI | I2C_CTL_AA);                               /* Clear SI and set ACK */
                else
                    u8Ctrl = I2C_CTL_SI;                                  /* Clear SI */
                break;              
            case 0x58:
                rdata[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);      /* Receive Data */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */
                u8Xfering = 0;
                break;
            case 0x38:                                                      /* Arbitration Lost */
            default:                                                        /* Unknow status */
                u8Ctrl = (I2C_CTL_STO | I2C_CTL_SI);                                  /* Clear SI and send STOP */                  
                u8Err = 1;
                break;               
        }
        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */
				
    } 
    return u32rxLen;                                                        /* Return bytes length that have been received */
}

