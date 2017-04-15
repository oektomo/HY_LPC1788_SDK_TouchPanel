/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			SST25VF016B.c
** Descriptions:		SST25VF016B操作函数库 
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-1-26
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "TSC2046.h"

/*******************************************************************************
* Function Name  : LPC17xx_SPI_SendRecvByte
* Description    : Send one byte then recv one byte of response
* Input          : - byte_s: byte_s
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
uint8_t LPC17xx_SPI_SendRecvByte (uint8_t byte_s)
{
	/* wait for current SSP activity complete */
	while (SSP_GetStatus(LPC_SSP0, SSP_STAT_BUSY) ==  SET);

	SSP_SendData(LPC_SSP0, (uint16_t) byte_s);

	while (SSP_GetStatus(LPC_SSP0, SSP_STAT_RXFIFO_NOTEMPTY) == RESET);
	return (SSP_ReceiveData(LPC_SSP0));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : 初始化控制SSI的管脚
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void SPI_FLASH_Init(void)
{
    SSP_CFG_Type SSP_ConfigStruct;
	/*
	 * Initialize SPI pin connect
	 * P2.23 - TP_CS - used as GPIO
	 * P2.22 - SCK
	 * P2.26 - MISO
	 * P2.27 - MOSI
	 */

	PINSEL_ConfigPin(2, 23, 0);
	PINSEL_ConfigPin(2, 22, 2);
	PINSEL_ConfigPin(2, 26, 2);
	PINSEL_ConfigPin(2, 27, 2);

    /* P0.16 CS is output */
    GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
	GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));  

	/* initialize SSP configuration structure to default */
	SSP_ConfigStructInit(&SSP_ConfigStruct);
	/* Initialize SSP peripheral with parameter given in structure above */
	SSP_Init(LPC_SSP0, &SSP_ConfigStruct);
	/* Enable SSP peripheral */
	SSP_Cmd(LPC_SSP0, ENABLE);
}

/*****************************************************************************
* Function Name  : Flash_ReadWriteByte
* Description    : 通过硬件SPI发送一个字节到SST25VF016B
* Input          : - data: 发送的数据
* Output         : None
* Return         : SST25VF016B 返回的数据
* Attention		 : None
*******************************************************************************/
uint8_t Flash_ReadWriteByte(uint8_t data)		
{
   return LPC17xx_SPI_SendRecvByte( data );                                
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

