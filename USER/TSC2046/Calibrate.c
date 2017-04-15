/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               TouchPanel.c
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

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h"
#include "TouchPanel.h"

#include "Calibrate.h"


//#define  GLCD_X_SIZE  800
//#define  GLCD_Y_SIZE  480
/******************************************************************************
* Function Name  : RGB565CONVERT
* Description    : 24位转换16位
* Input          : - red: R
*                  - green: G 
*				   - blue: B
* Output         : None
* Return         : RGB 颜色值
* Attention		 : None
*******************************************************************************/
#define RGB565CONVERT(red, green, blue)\
(uint16_t)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))


/* Private variables ---------------------------------------------------------*/
Matrix matrix ;
Coordinate  display ;

/* DisplaySample LCD坐标上对应的ads7843采样AD值 如：LCD 坐标45,45 应该的X Y采样ADC分别为3388,920 */	
Coordinate ScreenSample[5];
/* LCD上的坐标 */

Coordinate DisplaySample[5] =   {
                                            { 20,               20 },
											{ GLCD_X_SIZE - 20, 20 },
                                            { GLCD_X_SIZE - 20, GLCD_Y_SIZE - 20 },
											{ 20,               GLCD_Y_SIZE - 20 },
											{ GLCD_X_SIZE / 2,  GLCD_Y_SIZE / 2 },
	                            } ;

/*******************************************************************************
* Function Name  : DelayUS
* Description    : 延时1us
* Input          : - cnt: 延时值
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void DelayUS(uint32_t cnt)
{
  uint32_t i;
  for(i = 0;i<cnt;i++)
  {
     uint8_t us = 50; /* 设置值为12，大约延1微秒 */    
     while (us--)     /* 延1微秒	*/
     {
       ;   
     }
  }
}

/*******************************************************************************
* Function Name  : setCalibrationMatrix
* Description    : 计算出 K A B C D E F
* Input          : None
* Output         : None
* Return         : 返回1表示成功 0失败
* Attention		 : None
*******************************************************************************/
uint8_t setCalibrationMatrix( Coordinate * displayPtr,
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{

  uint8_t retTHRESHOLD = 0 ;
  /* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    retTHRESHOLD = 1;
  }
  else
  {
    /* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
	/* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
  return( retTHRESHOLD ) ;
}

/*******************************************************************************
* Function Name  : getDisplayPoint
* Description    : 通过 K A B C D E F 把通道X Y的值转换为液晶屏坐标
* Input          : None
* Output         : None
* Return         : 返回1表示成功 0失败
* Attention		 : None
*******************************************************************************/
uint8_t getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  uint8_t retTHRESHOLD = 0 ;

  if( matrixPtr->Divider != 0 )
  {
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    retTHRESHOLD = 1;
  }
  return(retTHRESHOLD);
} 


typedef struct {
      int x[5], xfb[5];
      int y[5], yfb[5];
      int a[7];
} calibration;

calibration cal;

int perform_calibration(calibration *cal) {
      int j;
      float n, x, y, x2, y2, xy, z, zx, zy;
      float det, a, b, c, e, f, i;
      float scaling = 65536.0;

// Get sums for matrix
      n = x = y = x2 = y2 = xy = 0;
      for(j=0;j<5;j++) {
            n += 1.0;
            x += (float)cal->x[j];
            y += (float)cal->y[j];
            x2 += (float)(cal->x[j]*cal->x[j]);
            y2 += (float)(cal->y[j]*cal->y[j]);
            xy += (float)(cal->x[j]*cal->y[j]);
      }
   //     printf("%f %f %f\n\n",x,y,n);
    //    printf("%f %f %f\n\n",x2,xy,x);
     //   printf("%f %f %f\n\n",xy,y2,y);
// Get determinant of matrix -- check if determinant is too small
      det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
      if(det < 0.1 && det > -0.1) {
    //        printf("ts_calibrate: determinant is too small -- %f\n",det);
            return 0;
      }

// Get elements of inverse matrix
      a = (x2*y2 - xy*xy)/det;
      b = (xy*y - x*y2)/det;
      c = (x*xy - y*x2)/det;
      e = (n*y2 - y*y)/det;
      f = (x*y - n*xy)/det;
      i = (n*x2 - x*x)/det;
        //printf("%f %f %f\n\n",a,b,c);
        //printf("%f %f %f\n\n",e,f,i);
// Get sums for x calibration
      z = zx = zy = 0;
      for(j=0;j<5;j++) {
            z += (float)cal->xfb[j];
            zx += (float)(cal->xfb[j]*cal->x[j]);
            zy += (float)(cal->xfb[j]*cal->y[j]);
      }
 //       printf("%f %f %f\n\n",z,zx,zy);
// Now multiply out to get the calibration for framebuffer x coord
      cal->a[2] = (int)((a*z + b*zx + c*zy)*(scaling));
      cal->a[0] = (int)((b*z + e*zx + f*zy)*(scaling));
      cal->a[1] = (int)((c*z + f*zx + i*zy)*(scaling));

 //     printf("%f %f %f\n",(a*z + b*zx + c*zy),
   //                     (b*z + e*zx + f*zy),
     //                   (c*z + f*zx + i*zy));

// Get sums for y calibration
      z = zx = zy = 0;
      for(j=0;j<5;j++) {
            z += (float)cal->yfb[j];
            zx += (float)(cal->yfb[j]*cal->x[j]);
            zy += (float)(cal->yfb[j]*cal->y[j]);
      }
  //      printf("\n\n%f %f %f\n\n",z,zx,zy);
// Now multiply out to get the calibration for framebuffer y coord
      cal->a[5] = (int)((a*z + b*zx + c*zy)*(scaling));
      cal->a[3] = (int)((b*z + e*zx + f*zy)*(scaling));
      cal->a[4] = (int)((c*z + f*zx + i*zy)*(scaling));

  //    printf("%f %f %f\n",(a*z + b*zx + c*zy),
   //                     (b*z + e*zx + f*zy),
     //                   (c*z + f*zx + i*zy));
       //   printf("%d %d %d \n\n%d %d %d\n\n",cal->a[0],cal->a[1],cal->a[2],cal->a[3],cal->a[4],cal->a[5],cal->a[6]);


// If we got here, we're OK, so assign scaling to a[6] and return
      cal->a[6] = (int)scaling;
      return 1;
}

/******************************************************************************
* Function Name  : PutChar
* Description    : 将Lcd屏上任意位置显示一个字符
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - ASCI: 显示的字符
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void __PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    __GetASCIICode(buffer,ASCI);  /* 取字模数据 */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                GLCD_SetPixel_16bpp( Xpos + j, Ypos + i, charColor );  /* 字符颜色 */
            }
            else
            {
                GLCD_SetPixel_16bpp( Xpos + j, Ypos + i, bkColor );  /* 背景颜色 */
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : 在指定座标显示字符串
* Input          : - Xpos: 行座标
*                  - Ypos: 列座标 
*				   - str: 字符串
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void __GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < GLCD_X_SIZE - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < GLCD_Y_SIZE - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}


/*******************************************************************************
* Function Name  : TouchPanel_Calibrate
* Description    : 校准触摸屏
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TouchPanel_Calibrate( uint16_t x_size, uint16_t y_size )
{
	uint8_t i;
	Coordinate * Ptr;
	
	DisplaySample[0].x = 20;              DisplaySample[0].y = 20;
	DisplaySample[1].x = x_size - 20;     DisplaySample[1].y = 20;
	DisplaySample[2].x = x_size - 20;     DisplaySample[2].y = y_size - 20;
	DisplaySample[3].x = 20;              DisplaySample[3].y = y_size - 20;
	DisplaySample[4].x = x_size / 2;      DisplaySample[4].y = y_size / 2;

	for(i=0;i<5;i++)
	{
		GLCD_Clear(Black);
		DelayUS( 100 * 1000 );
		DrawCross(DisplaySample[i].x,DisplaySample[i].y);
		do
		{
			Ptr = Read_Ads7846();
		}
		while( Ptr == (void*)0 );
		ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;
	}

	cal.xfb[0] = DisplaySample[0].x;
    cal.yfb[0] = DisplaySample[0].y;

	cal.x[0] = ScreenSample[0].x;
    cal.y[0] = ScreenSample[0].y;
	////////////////////////////////
	cal.xfb[1] = DisplaySample[1].x;
    cal.yfb[1] = DisplaySample[1].y;

	cal.x[1] = ScreenSample[1].x;
    cal.y[1] = ScreenSample[1].y;
	////////////////////////////////
	cal.xfb[2] = DisplaySample[2].x;
    cal.yfb[2] = DisplaySample[2].y;

	cal.x[2] = ScreenSample[2].x;
    cal.y[2] = ScreenSample[2].y;
	////////////////////////////////
	cal.xfb[3] = DisplaySample[3].x;
    cal.yfb[3] = DisplaySample[3].y;

	cal.x[3] = ScreenSample[3].x;
    cal.y[3] = ScreenSample[3].y;
	////////////////////////////////
	cal.xfb[4] = DisplaySample[4].x;
    cal.yfb[4] = DisplaySample[4].y;

	cal.x[4] = ScreenSample[4].x;
    cal.y[4] = ScreenSample[4].y;

	perform_calibration(&cal);

    matrix.An = cal.a[0];
	matrix.Bn = cal.a[1];
	matrix.Cn = cal.a[2];
	matrix.Dn = cal.a[3];
	matrix.En = cal.a[4];
	matrix.Fn = cal.a[5];
	matrix.Divider = cal.a[6];

 // GLCD_X_SIZE / 2,  GLCD_Y_SIZE  
//	setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix ) ;  /* 送入值得到参数 */	   
	GLCD_Clear(Black);
	__GUI_Text( (x_size - 280)/2, y_size/2-16, "Thank you for using HY-LPC1788-Core",Red,Black);
	__GUI_Text( (x_size - 248)/2, y_size/2, "www.powermcu.com www.hotmcu.com",Red,Black);

}

void calibrate(void)
{
        getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
		TP_DrawPoint(display.x,display.y);
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
