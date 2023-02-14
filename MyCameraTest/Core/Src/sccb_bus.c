/**
  * @file    sccb_bus.c 
  * @author  Arducam
  * @version V0.1
  * @date    2018.06.22
  * @brief   sccb bus
  */
#include "sccb_bus.h"
#include "delay.h"
#include "main.h"
 uint32_t ntime;


/**
  * @brief  init i2c bus
  * @param  None
  * @retval None
  */
void sccb_bus_init(void)//Once GPIO is gone, this turns into sccb_data_out();. May just want to replace-all
{
	/*GPIO_InitTypeDef  GPIO_InitStructure;
 	//Obviously initialize with the rest of the GPIO. Don't need to give extern
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//Port B
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;      //Pin 11
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	//Internal Pull Up (?)
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//Fast Speed
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_11);			//PortB Pin 11. This is likely the I2C input, see general setup info

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			//Port B Pin 10
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Push Pull (Normal Output, not open drain)
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_10);	*/
	sccb_data_out();

}


void sccb_data_in()(void)//Replace sccb_data_in()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};//Created by ChatGPT, may not be correct
	GPIO_InitStruct.Pin = GPIO_PIN_11; // SCCB data pins
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


//MY ANALYSIS:
/*
 * Our SCCB line for the camera module is likely two lines (verify in datasheet)
 * SIO_C and SIO_D. SIO_C is unidirectional control signal. SIO_D is a bi-directional data signal.
 * I think that the data-in and data-out functions are intended to implement this bi-directional functionality.
 * If so, then only one pin, 10 or 11, should be changed, not both. According to the CRH reg on a forum,
 * bits 12 and 13 as 1s set the Pin11 to 50MHz speed (out) and bit 15 sets to push-pull(?). I think what ChatGBT did makes the most sense,
 * just make it pin11 not both.
 * */

/*
 * This code initializes the GPIOB pins 13 and 14 (the SCCB data pins)
 * for output with push-pull mode, no pull-up or pull-down resistors,
 * and a high-speed output. Note that the specific GPIO pins used in
 * this code might differ depending on your particular STM32 board and
 * how it is connected to the Arducam module.
 *
 * Would be worth checking the arduino equivalent.
 * */
void sccb_data_out()(void)//Replace sccb_data_out()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};//Created by ChatGPT
	GPIO_InitStruct.Pin = GPIO_PIN_11; // SCCB data pins. Changed to 10 and 11 then to just 11
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void sccb_bus_start(void)
{
    SCCB_SID_H();             
    us_delay(I2C_TIM);
    SCCB_SIC_H();	           
    us_delay(I2C_TIM);
    SCCB_SID_L();
    us_delay(I2C_TIM);
    SCCB_SIC_L();	           
    us_delay(I2C_TIM);
}


void sccb_bus_stop(void)
{
    SCCB_SID_L();
    us_delay(I2C_TIM);
    SCCB_SIC_H();	
    us_delay(I2C_TIM);
    SCCB_SID_H();	
    us_delay(I2C_TIM);
}


void sccb_bus_send_noack(void)
{	
	SCCB_SID_H();	
	us_delay(I2C_TIM);
	SCCB_SIC_H();	
	us_delay(I2C_TIM);
	SCCB_SIC_L();	
	us_delay(I2C_TIM);
	SCCB_SID_L();	
	us_delay(I2C_TIM);
}

void sccb_bus_send_ack(void)
{	
	SCCB_SID_L();	
	us_delay(I2C_TIM);
	SCCB_SIC_L();	
	us_delay(I2C_TIM);
	SCCB_SIC_H();	
	us_delay(I2C_TIM);
	SCCB_SIC_L();	
	us_delay(I2C_TIM);
	SCCB_SID_L();	
	us_delay(I2C_TIM);
}

uint8_t sccb_bus_write_byte(uint8_t data)
{
	uint32_t i;
	uint8_t tem;

	for(i = 0; i < 8; i++) 
	{
		if((data<<i) & 0x80)
		{
			SCCB_SID_H();
		}
		else 
		{
			SCCB_SID_L();
		}
		us_delay(I2C_TIM);
		SCCB_SIC_H();	
		us_delay(I2C_TIM);
		SCCB_SIC_L();	

	}
	sccb_data_in();
	us_delay(I2C_TIM);
	SCCB_SIC_H();	
	us_delay(I2C_TIM);
	if(SCCB_SID_STATE)
	{
		tem = 0;               
	}
	else 
	{
		tem = 1;                
	}

	SCCB_SIC_L();	
	us_delay(I2C_TIM);
	sccb_data_out();
	return tem;  
}

uint8_t sccb_bus_read_byte(void)
{	
	uint32_t i;
	uint8_t read = 0;
	
	sccb_data_in();
	us_delay(I2C_TIM);
	for(i = 8; i > 0; i--) 
	{		     
		us_delay(I2C_TIM);
		SCCB_SIC_H();
		us_delay(I2C_TIM);
		read = read << 1;
		if(SCCB_SID_STATE)
		{
			read += 1; 
		}
		SCCB_SIC_L();
		us_delay(I2C_TIM);
	}	
    sccb_data_out();
	return read;
}

