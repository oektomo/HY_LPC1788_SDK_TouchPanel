/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			SST25VF016B.h
** Descriptions:		SST25VF016Bͷ���� 
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

#ifndef __SST25VF016B_H 
#define __SST25VF016B_H

/* Includes ------------------------------------------------------------------*/
#include "lpc_types.h"
#include "lpc177x_8x_ssp.h"
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_pinsel.h"


/* Private typedef -----------------------------------------------------------*/
/* �������SST25VF016B��������� */
typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;

/* Private define ------------------------------------------------------------*/
#define MAX_ADDR		0x1FFFFF	/* ����оƬ�ڲ�����ַ */
#define	SEC_MAX     	511         /* ������������� */
#define SEC_SIZE		0x1000      /* ������С	*/

/* ��,���ڶ���SST25VF016B��Ƭѡ���� */
/* PORT number that /CS pin assigned on */
#define CS_PORT_NUM		2
/* PIN number that  /CS pin assigned on */
#define CS_PIN_NUM		23

#define SPI_FLASH_CS_LOW()    GPIO_ClearValue(CS_PORT_NUM, (1<<CS_PIN_NUM));   
#define SPI_FLASH_CS_HIGH()   GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));   

/* Private function prototypes -----------------------------------------------*/
void  SPI_FLASH_Init(void);
uint8_t SSTF016B_RD(uint32_t Dst, uint8_t* RcvBufPt ,uint32_t NByte);
uint8_t SSTF016B_RdID(idtype IDType,uint32_t* RcvbufPt);
uint8_t SSTF016B_WR(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte);

uint8_t SSTF016B_Erase(uint32_t sec1, uint32_t sec2);
void SPI_FLASH_Test(void);

#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

