/*********************************************************************************************************
*
* File                : 24C02.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "camera/SCCB.h"

__IO uint32_t  DCMI_TIMEOUT_MAX = SCCB_Open_FLAG_TIMEOUT;

/*******************************************************************************
* Function Name  : SCCB_GPIO_Config
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void SCCB_GPIO_Config(void)
{
	I2C_InitTypeDef  SCCB_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(Open_SCCB_SDA_GPIO_CLK | Open_SCCB_SCL_GPIO_CLK,ENABLE);
	
	RCC_APB1PeriphClockCmd(Open_SCCB_CLK,ENABLE);
	
	GPIO_PinAFConfig(Open_SCCB_SDA_GPIO_PORT, Open_SCCB_SDA_SOURCE, Open_SCCB_SDA_AF);
	GPIO_PinAFConfig(Open_SCCB_SCL_GPIO_PORT, Open_SCCB_SCL_SOURCE, Open_SCCB_SCL_AF);
	
	GPIO_InitStructure.GPIO_Pin =  Open_SCCB_SDA_PIN | Open_SCCB_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	I2C_DeInit(Open_SCCB);
	SCCB_InitStructure.I2C_Mode = I2C_Mode_I2C;
	SCCB_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	SCCB_InitStructure.I2C_OwnAddress1 = SCCB_SLAVE_ADDRESS7;
	SCCB_InitStructure.I2C_Ack = I2C_Ack_Enable;
	SCCB_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	SCCB_InitStructure.I2C_ClockSpeed = SCCB_SPEED;
	
	I2C_Cmd(Open_SCCB, ENABLE);
	I2C_Init(Open_SCCB, &SCCB_InitStructure);
	
	I2C_AcknowledgeConfig(Open_SCCB, ENABLE);	
}

/**
  * @brief  Writes a byte at a specific Camera register
  * @param  Device: OV7670 write address.
  * @param  Addr: OV7670 register address. 
  * @param  Data: data to be written to the specific register 
  * @retval 0x00 if write operation is OK. 
  *         0xFF if timeout condition occured (device not connected or bus error).
  */
uint8_t DCMI_SingleRandomWrite(uint8_t Reg, uint8_t Data)
{
  uint32_t timeout = DCMI_TIMEOUT_MAX;
  
  /* Generate the Start Condition */
  I2C_GenerateSTART(Open_SCCB, ENABLE);

  /* Test on I2C2 EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
   /*-----------------------------------------------------------------------------------*/
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(Open_SCCB, OV7670_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter);
 
  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
   /*-----------------------------------------------------------------------------------*/
  /* Send I2C2 location address LSB */
  I2C_SendData(Open_SCCB, (uint8_t)(Reg));

  /* Test on I2C2 EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
   /*-----------------------------------------------------------------------------------*/
  /* Send Data */
  I2C_SendData(Open_SCCB, Data);    

  /* Test on I2C2 EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }  
   /*-----------------------------------------------------------------------------------*/
  /* Send I2C2 STOP Condition */
  I2C_GenerateSTOP(Open_SCCB, ENABLE);
  
  /* If operation is OK, return 0 */
  return 0;
}

/**
  * @brief  Reads a byte from a specific Camera register 
  * @param  Device: OV7670 write address.
  * @param  Addr: OV7670 register address. 
  * @retval data read from the specific register or 0xFF if timeout condition 
  *         occured. 
  */
uint8_t DCMI_SingleRandomRead(uint8_t Reg, uint8_t *Data)
{
  uint32_t timeout = DCMI_TIMEOUT_MAX;

    /* Clear AF flag if arised */
  I2C2->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(Open_SCCB, ENABLE);

  /* Test on I2C2 EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  /*-----------------------------------------------------------------------------------*/
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(Open_SCCB, OV7670_DEVICE_READ_ADDRESS, I2C_Direction_Transmitter);
 
  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  /*-----------------------------------------------------------------------------------*/
  /* Send I2C2 location address LSB */
  I2C_SendData(Open_SCCB, (uint8_t)(Reg));

  /* Test on I2C2 EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Clear AF flag if arised */
  I2C2->SR1 |= (uint16_t)0x0400;
  /*-----------------------------------------------------------------------------------*/    
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(Open_SCCB, ENABLE);

  /*-----------------------------------------------------------------------------------*/
  /* Generate the Start Condition */
  I2C_GenerateSTART(Open_SCCB, ENABLE);
  
  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  /*-----------------------------------------------------------------------------------*/
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(Open_SCCB, OV7670_DEVICE_READ_ADDRESS, I2C_Direction_Receiver);
   
  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
      
  /*-----------------------------------------------------------------------------------*/
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(Open_SCCB, DISABLE);

  /* Test on Open_SCCB EV7 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(Open_SCCB, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  /*-----------------------------------------------------------------------------------*/    
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(Open_SCCB, ENABLE); 

  /*-----------------------------------------------------------------------------------*/
  /* Receive the Data */
  *Data = I2C_ReceiveData(Open_SCCB);
  /* Clear AF flag if arised */
  I2C2->SR1 |= (uint16_t)0x0400;

  /* return the read data */
  return 0;
}
