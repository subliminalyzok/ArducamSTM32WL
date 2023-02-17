#include "spi.h"
#include "stdbool.h"
#define BMPIMAGEOFFSET 66
#define pgm_read_byte(x)        (*((char *)x))

const char bmp_header[BMPIMAGEOFFSET] =
{
  0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00
};

uint8_t	*picbuf = 0;
uint8_t receive_OK = 0;
uint8_t USB_sendOK = 1;
uint8_t send_OK = 1;
uint32_t sendlen = 0;
uint32_t haveRev = 0;
uint32_t noRev = 0;
uint8_t  EP2_SendFinish = 1;
uint8_t	Buf1[BUFFER_MAX_SIZE]={0}, Buf2[BUFFER_MAX_SIZE]={0};
extern uint16_t NumPackage;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;
/*void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE );
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	SPI1_ReadWriteByte(0xff);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(PB_SPI_RX_DMA_Channel, &DMA_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = PB_SPI_DMA_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);

	DMA_ITConfig(PB_SPI_RX_DMA_Channel, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
}*/
 
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{		
	//uint8_t retry=0;
	/*while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>200)return 0;
	}*/
	//SPI_I2S_SendData(SPI1, TxData);//Appears to send 1 byte. STM32F4 function
	HAL_SPI_Transmit(&hspi1, &TxData, 1, 400);//Should handle the blocking above
	//retry=0;

	/*while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		if(retry>200)return 0;
	}*/
	HAL_SPI_Receive(&hspi1, &TxData, 1, 400);
	return TxData;
			//SPI_I2S_ReceiveData(SPI1);
}

/*void DMA1_RX(uint8_t *p , uint32_t len)
{		
	CS_LOW();
	set_fifo_burst();
	PB_SPI_RX_DMA_Channel->CMAR = (uint32_t)p;
	PB_SPI_RX_DMA_Channel->CNDTR = len;
	DMA1_Channel1->CMAR = (uint32_t)p;
	DMA1_Channel1->CNDTR = len;
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);		
	DMA_Cmd(PB_SPI_RX_DMA_Channel, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void DMA1_SendtoUsart(uint8_t *p , uint32_t len)
{		
	DMA1_Channel7->CMAR = (uint32_t)p;
	DMA1_Channel7->CNDTR = len;
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);		
	DMA_Cmd(DMA1_Channel7, ENABLE);
}*/

/*void SendbyUSART1(void)
{	
	uint8_t	*sdbuf;
	haveRev += sendlen;
    if(haveRev < length)
	{	
		if(picbuf == Buf1)
		{		
			sdbuf = Buf1;	  picbuf = Buf2;	
		}
		else
		{
			sdbuf = Buf2;	  picbuf = Buf1;
		}
		UART1_BulkOut(sendlen,sdbuf);
		noRev	= length - haveRev;		
		sendlen	= (noRev>=BUFFER_MAX_SIZE) ? BUFFER_MAX_SIZE : noRev;	
		DMA1_RX(picbuf, sendlen);	
	}
	else
	{
		UART1_BulkOut(sendlen, picbuf);
		send_OK = 1;
	}			 	 					 	 	
}*/
/*void SingleCapTransfer(void)
{
	flush_fifo();
	clear_fifo_flag();
	start_capture(); 
	while(!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){;}
	//printf("ACK CMD capture done\r\n");
	length= read_fifo_length();
	//printf("ACK CMD the length is %d\r\n",length);
	sendlen = (length>=BUFFER_MAX_SIZE) ? BUFFER_MAX_SIZE : length;
	picbuf = Buf1;
	haveRev = 0;
	DMA1_RX(picbuf, sendlen);
}*/

void StartBMPcapture(void)
{
	flush_fifo();
	clear_fifo_flag();
	start_capture(); 
	uint8_t d1 = 0xff;
	uint8_t d2 = 0xaa;
	uint8_t data_to_send;
	while(!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){;}
	//printf("ACK CMD capture done\r\n");
	length= read_fifo_length();
//	printf("ACK CMD the length is %d\r\n",length);		
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//USART_SendData(USART1, 0xff);
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//USART_SendData(USART1, 0xaa);
	HAL_UART_Transmit(&huart2, &d1, 1, 400);
	HAL_UART_Transmit(&huart2, &d2, 1, 400);
	for(int temp = 0; temp < BMPIMAGEOFFSET; temp++)
	{
		//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		//USART_SendData(USART1, pgm_read_byte(&bmp_header[temp]));
		data_to_send = pgm_read_byte(&bmp_header[temp]);
		HAL_UART_Transmit(&huart2, &data_to_send, 1, 400);

	}
	CS_LOW();
	set_fifo_burst();
	char VH, VL;
	int i = 0, j = 0;
	for (i = 0; i < 240; i++)
	{
		for (j = 0; j < 320; j++)
		{
			VH = SPI1_ReadWriteByte(0x00);			
			VL = SPI1_ReadWriteByte(0x00);		
			//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			//USART_SendData(USART1, VL);
			HAL_UART_Transmit(&huart2, &VL, 1, 400);
			us_delay(15);
			//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			//USART_SendData(USART1, VH);
			HAL_UART_Transmit(&huart2, &VH, 1, 400);
			us_delay(15);
		}
	}
	d1 = 0xbb;
	d2 = 0xcc;
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//USART_SendData(USART1, 0xbb);
	HAL_UART_Transmit(&huart2, &d1, 1, 400);
	us_delay(12);
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//USART_SendData(USART1, 0xcc);
	HAL_UART_Transmit(&huart2, &d2, 1, 400);
	CS_HIGH();
}

/*void PB_SPI_DMA_IRQHandler(void)
{ 	
	if(DMA_GetITStatus(DMA1_IT_TC2))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL4 | DMA1_IT_TC2 | DMA1_IT_GL5 | DMA1_IT_TC5);
		DMA_Cmd(PB_SPI_RX_DMA_Channel, DISABLE);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
		CS_HIGH();
		receive_OK =1;
	}
}

void DMA1_Channel7_IRQHandler(void)
{ 	
	if(DMA_GetITStatus(DMA1_IT_TC7))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL7 | DMA1_IT_TC7);
		DMA_Cmd(DMA1_Channel7, DISABLE);
		USART_DMACmd(USART2, USART_DMAReq_Tx , DISABLE);
	}
}*/

