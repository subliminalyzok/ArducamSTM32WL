#include "spi.h"

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
bool receive_OK = false;
bool USB_sendOK = true;
bool send_OK = true;
uint32_t sendlen = 0;
uint32_t haveRev = 0;
uint32_t noRev = 0;
uint8_t  EP2_SendFinish = 1;
uint8_t	Buf1[BUFFER_MAX_SIZE]={0}, Buf2[BUFFER_MAX_SIZE]={0};
extern uint16_t NumPackage;


uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
	uint8_t retry=0;
	while(__HAL_SPI_GET_FLAG(SPI1, SPI_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>200)return 0;
	}
	HAL_SPI_Transmit_IT(SPI1, TxData, 8);
	retry=0;

	while(__HAL_SPI_GET_FLAG(SPI1, SPI_FLAG_RXNE) == RESET)
	{
		retry++;
		if(retry>200)return 0;
	}
	return SPI_I2S_ReceiveData(SPI1);
}

void DMA1_RX(uint8_t *p , uint32_t len)
{
	CS_LOW();
	set_fifo_burst();
	PB_SPI_RX_DMA_Channel->CMAR = (u32)p;
	PB_SPI_RX_DMA_Channel->CNDTR = len;
	DMA1_Channel3->CMAR = (u32)p;
	DMA1_Channel3->CNDTR = len;
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(PB_SPI_RX_DMA_Channel, ENABLE);
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

void DMA1_SendtoUsart(uint8_t *p , uint32_t len)
{
	DMA1_Channel7->CMAR = (u32)p;
	DMA1_Channel7->CNDTR = len;
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel7, ENABLE);
}

void SendbyUSART1(void)
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
}
void SingleCapTransfer(void)
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
}

void StartBMPcapture(void)
{
	flush_fifo();
	clear_fifo_flag();
	start_capture();
	while(!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){;}
	//printf("ACK CMD capture done\r\n");
	length= read_fifo_length();
//	printf("ACK CMD the length is %d\r\n",length);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xff);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xaa);
	for(int temp = 0; temp < BMPIMAGEOFFSET; temp++)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, pgm_read_byte(&bmp_header[temp]));

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
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			USART_SendData(USART1, VL);
			delay_us(15);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			USART_SendData(USART1, VH);
			delay_us(15);
		}
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xbb);
	delay_us(12);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xcc);
	CS_HIGH();
}
