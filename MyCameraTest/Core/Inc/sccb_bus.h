
#ifndef _SCCB_BUS_H_
#define _SCCB_BUS_H_


#include "stm32wlxx.h"
#include "sys.h"
#include "main.h"
#define I2C_TIM 1

#define SCCB_SIC_H()      HAL_GPIO_WritePin(I2C3_SCL_Port, I2C3_SCL_Pin, GPIO_PIN_SET)//PBout(13) =1	 	//SCL H
#define SCCB_SIC_L()      HAL_GPIO_WritePin(I2C3_SCL_Port, I2C3_SCL_Pin, GPIO_PIN_RESET)//PBout(13) = 0	 	//SCL H
#define SCCB_SID_H()      HAL_GPIO_WritePin(I2C3_SDA_Port, I2C3_SDA_Pin, GPIO_PIN_SET)//PBout(4) = 1   //SDA	H
#define SCCB_SID_L()      HAL_GPIO_WritePin(I2C3_SDA_Port, I2C3_SDA_Pin, GPIO_PIN_RESET)//PBout(4) = 0  //SDA	H

//#define SCCB_DATA_IN     {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00008000;}
//#define SCCB_DATA_OUT    {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00003000;}
#define SCCB_SID_STATE	  PBin(4)//PBin(11)// Gets state of data pin (in / out). MAY NOT BE CORRECT, Need addr of the Input Data Register on the WL
//Can also have a var that keeps track of the state instead. State should only change in sccb_in/out functions.
void sccb_bus_init(void);
void sccb_bus_start(void);
void sccb_bus_stop(void);
void sccb_bus_send_noack(void);
void sccb_bus_send_ack(void);
uint8_t sccb_bus_write_byte(uint8_t data);
uint8_t sccb_bus_read_byte(void);
void sccb_data_in(void);
void sccb_data_out(void);


#endif /* _SCCB_BUS_H_ */

/******************* (C) COPYRIGHT 2015 WangBao *****END OF FILE****/
