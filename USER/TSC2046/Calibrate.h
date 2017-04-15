/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               TouchPanel.h
** Descriptions:            The TouchPanel application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef _CALIBRATE_H_
#define _CALIBRATE_H_

/* Includes ------------------------------------------------------------------*/
#include "lpc177x_8x.h"
#include "lpc_types.h"
#include "lpc177x_8x_ssp.h"
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_pinsel.h"

/* Private typedef -----------------------------------------------------------*/

typedef struct Matrix 
{						
int  
     An,       /* Touch screen calibration parameters */
     Bn,       /* Touch screen calibration parameters */
     Cn,       /* Touch screen calibration parameters */
     Dn,       /* Touch screen calibration parameters */
     En,       /* Touch screen calibration parameters */
     Fn,       /* Touch screen calibration parameters */
     Divider;  /* Touch screen calibration parameters */
} Matrix ;

/* Private variables ---------------------------------------------------------*/
extern Coordinate ScreenSample[5];
extern Coordinate DisplaySample[5];
extern Matrix matrix ;
extern Coordinate  display ;

/* Private function prototypes -----------------------------------------------*/				
void TouchPanel_Calibrate( uint16_t x_size, uint16_t y_size );
void calibrate(void);
void DrawCross(uint16_t Xpos,uint16_t Ypos);
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos);
uint8_t setCalibrationMatrix( Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr);
uint8_t getDisplayPoint(Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr );

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


