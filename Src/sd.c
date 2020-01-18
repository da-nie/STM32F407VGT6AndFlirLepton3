//----------------------------------------------------------------------------------------------------
//������������ ����������
//----------------------------------------------------------------------------------------------------
#include "sd.h"
#include "stm32f4xx_hal.h"

//----------------------------------------------------------------------------------------------------
//����������������
//----------------------------------------------------------------------------------------------------

//****************************************************************************************************
//��������� SD-�����
//****************************************************************************************************


#define SD_OUTPUT_GPIO_PIN_CS		GPIO_PIN_4
#define SD_OUTPUT_GPIO_CS				GPIOC

#define SD_OUTPUT_GPIO_PIN_DI		GPIO_PIN_3
#define SD_OUTPUT_GPIO_DI				GPIOC

#define SD_INPUT_GPIO_PIN_DO		GPIO_PIN_2
#define SD_INPUT_GPIO_DO				GPIOC

#define SD_OUTPUT_GPIO_PIN_SCK	GPIO_PIN_10
#define SD_OUTPUT_GPIO_SCK			GPIOB

//���� ������
#define CMD0  0x40
#define CMD1  (CMD0+1)
#define CMD8  (CMD0+8)
#define CMD9  (CMD0+9)
#define CMD16 (CMD0+16)
#define CMD17 (CMD0+17)
#define CMD24 (CMD0+24)
#define CMD55 (CMD0+55)
#define CMD58 (CMD0+58)

//----------------------------------------------------------------------------------------------------
//���������
//----------------------------------------------------------------------------------------------------

//������ ������
static const uint8_t ANSWER_R1_SIZE=1;
static const uint8_t ANSWER_R3_SIZE=5;

//----------------------------------------------------------------------------------------------------
//������������
//----------------------------------------------------------------------------------------------------

//���� SD-����
typedef enum 
{
 SD_TYPE_NONE=0,
 SD_TYPE_MMC_V3=1,
 SD_TYPE_SD_V1=2,
 SD_TYPE_SD_V2=3,
 SD_TYPE_SD_V2_HC=4
} SD_TYPE;

//----------------------------------------------------------------------------------------------------
//���������� ����������
//----------------------------------------------------------------------------------------------------

uint16_t BlockByteCounter=512;//��������� ���� �����
SD_TYPE SDType=SD_TYPE_NONE;//��� ����� ������
static SPI_HandleTypeDef hspi;//SPI

//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
static inline uint8_t SD_TransmitData(uint8_t data);//������� ������ SD-����� � ������� �����
uint16_t inline GetBits(uint8_t *data,uint8_t begin,uint8_t end);//�������� ���� � begin �� end ������������

//----------------------------------------------------------------------------------------------------
//�������� ���� � begin �� end ������������
//----------------------------------------------------------------------------------------------------
uint16_t GetBits(uint8_t *data,uint8_t begin,uint8_t end)
{
 uint16_t bits=0;
 uint8_t size=1+begin-end; 
 for(uint8_t i=0;i<size;i++) 
 {
  uint8_t position=end+i;
  uint16_t byte=15-(position>>3);
  uint16_t bit=position&0x7;
  uint16_t value=(data[byte]>>bit)&1;
  bits|=value<<i;
 }
 return(bits);
}
//----------------------------------------------------------------------------------------------------
//������� ������ SD-����� � ������� �����
//----------------------------------------------------------------------------------------------------
static inline uint8_t SD_TransmitData(uint8_t data)
{ 	
 uint8_t res; 	
 HAL_SPI_TransmitReceive(&hspi,&data,&res,sizeof(uint8_t),0x1000);	
 return(res);
}

//----------------------------------------------------------------------------------------------------
//������������� ����� ������
//----------------------------------------------------------------------------------------------------
SD_ANSWER SD_Init(void)
{	
 //����������� �����
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
	
 //GPIO_Pin � ������ �������, ������� ���������������. ������ ��� ���������� �������:
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
 //GPIO_Speed � ������ �������� ��� ��������� �������.
 //GPIO_Mode � ������ ����� ������ �������. 

 //����������� ��������� ������
 GPIO_InitTypeDef SD_GPIO_Init;

 SD_GPIO_Init.Mode=GPIO_MODE_OUTPUT_PP;
 SD_GPIO_Init.Pull=GPIO_NOPULL;
 SD_GPIO_Init.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	
 SD_GPIO_Init.Pin=SD_OUTPUT_GPIO_PIN_CS;
 HAL_GPIO_Init(SD_OUTPUT_GPIO_CS,&SD_GPIO_Init);
/*	
 SD_GPIO_Init.Pin=SD_OUTPUT_GPIO_PIN_DI;
 HAL_GPIO_Init(SD_OUTPUT_GPIO_DI,&SD_GPIO_Init);	

 SD_GPIO_Init.Pin=SD_OUTPUT_GPIO_PIN_SCK;
 HAL_GPIO_Init(SD_OUTPUT_GPIO_SCK,&SD_GPIO_Init);	

 SD_GPIO_Init.Mode=GPIO_MODE_INPUT;
 SD_GPIO_Init.Pin=SD_INPUT_GPIO_PIN_DO;
 HAL_GPIO_Init(SD_INPUT_GPIO_DO,&SD_GPIO_Init);	
 */

 //����������� SPI 	
 hspi.Instance=SPI2;
 hspi.Init.Mode=SPI_MODE_MASTER;
 hspi.Init.Direction=SPI_DIRECTION_2LINES;
 hspi.Init.DataSize=SPI_DATASIZE_8BIT;
 hspi.Init.CLKPolarity=SPI_POLARITY_LOW;
 hspi.Init.CLKPhase=SPI_PHASE_1EDGE;
 hspi.Init.NSS=SPI_NSS_SOFT;
 hspi.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16;
 hspi.Init.FirstBit=SPI_FIRSTBIT_MSB;
 hspi.Init.TIMode=SPI_TIMODE_DISABLE;
 hspi.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
 hspi.Init.CRCPolynomial=10;
 if (HAL_SPI_Init(&hspi)!=HAL_OK) return(SD_ANSWER_SPI_ERROR); 


 //�������������� �����		
 //��� �� ����� 74 ��������� ������������� ��� ������� ������ �� CS � DI 
 SD_CS_Zero();
 //����� SPI SS � ������ MASTER ��������������� ��� ����� � �� SPI �� ������
 HAL_Delay(1000);//�����, ���� ����� �� ���������
 SD_CS_One();
 uint16_t n;
 for(n=0;n<250;n++)
 {
  SD_TransmitData(255);
  HAL_Delay(1);
 }
 SD_CS_Zero();
 
 uint8_t answer[ANSWER_R3_SIZE];//����� �� �����
 bool res;
 //��� CMD0
 res=SD_SendCommand(CMD0,0x00,0x00,0x00,0x00,ANSWER_R1_SIZE,answer);
 if (res==false || answer[0]!=1) return(SD_ANSWER_SPI_NOT_SUPPORTED);//������

 //���������� ��� �����
 SDType=SD_TYPE_NONE;
 //��� CMD8 (����� ����� R3 � ������ SPI)
 res=SD_SendCommand(CMD8,0xAA,0x01,0x00,0x00,ANSWER_R3_SIZE,answer);
 if (res==true && answer[0]==0x01)
 {  
  if (!((answer[ANSWER_R3_SIZE-2]&0x0F)==0x01 && answer[ANSWER_R3_SIZE-1]==0xAA)) return(SD_ANSWER_NO_RESPONSE);
  //��� ACMD41
  for(n=0;n<65535;n++)
  {
   //��� ACMD41
   res=SD_SendCommand(CMD55,0x00,0x00,0x00,0x00,ANSWER_R1_SIZE,answer);
   if (res==false || (answer[0]!=0x00 && answer[0]!=0x01)) return(SD_ANSWER_NO_RESPONSE);
   res=SD_SendCommand(CMD1,0x00,0x00,0x00,0x40,ANSWER_R1_SIZE,answer);
   if (res==true && answer[0]==0x00) break;
  }
  if (n==65535) return(SD_ANSWER_NO_RESPONSE);
  //��� CMD58
  res=SD_SendCommand(CMD58,0x00,0x00,0x00,0x00,ANSWER_R3_SIZE,answer);
  if (res==false) return(SD_ANSWER_NO_RESPONSE);
  if (answer[1]&0x40) SDType=SD_TYPE_SD_V2_HC;
                 else SDType=SD_TYPE_SD_V2;  
 } 
 else//����� �� �������� �� CMD8
 {
  //��������� ����������� ����� ACMD41, �� ������ ������ ����� �� 16 �� ���������� � ��� ��������, ��� ���� ������ ��� �� ���������.
  //����� ����� ����� ���������������� ������ ����� CMD1, � �� ����� ACMD41
  //��� ACMD41
  for(n=0;n<65535;n++)
  {   
   /*
   //��� ACMD41, ���� ������ ��� �� MMC V3
   if (SDType!=SD_TYPE_MMC_V3)
   {
	res=SD_SendCommand(CMD55,0x00,0x00,0x00,0x00,ANSWER_R1_SIZE,answer);
    if (res==false || (answer[0]!=0x00 && answer[0]!=0x01)) SDType=SD_TYPE_MMC_V3;//����� �� ����� ACM41, ��� ��� ������ ��� ������ CMD1
   }
   */
   res=SD_SendCommand(CMD1,0x00,0x00,0x00,0x00,ANSWER_R1_SIZE,answer);
   if (res==true && answer[0]==0x00) break;
  }
  if (n==65535) return(SD_ANSWER_NO_RESPONSE);
  if (SDType!=SD_TYPE_MMC_V3) SDType=SD_TYPE_SD_V1;
 }
 //����� ������ ����� 512 ����
 res=SD_SendCommand(CMD16,0x00,0x00,0x02,0x00,ANSWER_R1_SIZE,answer);

 //������������� ������� �������
 if (SDType!=SD_TYPE_SD_V2_HC)//������ ����� ����� ������
 {
  //������ ����� ����� ������
  uint32_t SD_Size=0;
  if (SD_GetSize(&SD_Size)==false) return(SD_ANSWER_SIZE_ERROR);//������
  uint16_t size=(uint16_t)(SD_Size>>20);
 }
 for(uint16_t m=0;m<1024;m++) SD_TransmitData(0xff);
 return(SD_ANSWER_OK);
}

//----------------------------------------------------------------------------------------------------
//��������� �� CS 1
//----------------------------------------------------------------------------------------------------
void SD_CS_One(void)
{
 HAL_GPIO_WritePin(SD_OUTPUT_GPIO_CS,SD_OUTPUT_GPIO_PIN_CS,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//��������� �� CS 0
//----------------------------------------------------------------------------------------------------
void SD_CS_Zero(void)
{
 HAL_GPIO_WritePin(SD_OUTPUT_GPIO_CS,SD_OUTPUT_GPIO_PIN_CS,GPIO_PIN_RESET);
}

//----------------------------------------------------------------------------------------------------
//��������� �� DI 1
//----------------------------------------------------------------------------------------------------
void SD_DI_One(void)
{
 HAL_GPIO_WritePin(SD_OUTPUT_GPIO_DI,SD_OUTPUT_GPIO_PIN_DI,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//��������� �� DI 0
//----------------------------------------------------------------------------------------------------
void SD_DI_Zero(void)
{
 HAL_GPIO_WritePin(SD_OUTPUT_GPIO_DI,SD_OUTPUT_GPIO_PIN_DI,GPIO_PIN_RESET);		
}

//----------------------------------------------------------------------------------------------------
//��������� �� SCK 1
//----------------------------------------------------------------------------------------------------
void SD_SCK_One(void)
{
 HAL_GPIO_WritePin(SD_OUTPUT_GPIO_SCK,SD_OUTPUT_GPIO_PIN_SCK,GPIO_PIN_SET);		
}
//----------------------------------------------------------------------------------------------------
//��������� �� SCK 0
//----------------------------------------------------------------------------------------------------
void SD_SCK_Zero(void)
{
 HAL_GPIO_WritePin(SD_OUTPUT_GPIO_SCK,SD_OUTPUT_GPIO_PIN_SCK,GPIO_PIN_RESET);			
}

//----------------------------------------------------------------------------------------------------
//������� ������� � �������� �����
//----------------------------------------------------------------------------------------------------
bool SD_SendCommand(uint8_t cmd,uint8_t b0,uint8_t b1,uint8_t b2,uint8_t b3,uint8_t answer_size,uint8_t *answer)
{ 
 //���������� ������� � ������� � CRC7
 uint8_t crc7=0; 
 uint8_t cmd_buf[5]={cmd,b3,b2,b1,b0};
 uint16_t n;
 for(n=0;n<5;n++)
 {
  SD_TransmitData(cmd_buf[n]);
  
  uint8_t b=cmd_buf[n]; 
  for (uint8_t i=0;i<8;i++) 
  { 
   crc7<<=1; 
   if ((b&0x80)^(crc7&0x80)) crc7^=0x09; 
   b<<=1; 
  }
 }
 crc7=crc7<<1;
 crc7|=1; 
 SD_TransmitData(crc7);//CRC
 //����� ����� �������� �� �����
 //��������� ����� R1 (������� ��� ������ 0)
 for(n=0;n<65535;n++)
 {
  uint8_t res=SD_TransmitData(0xff);
  if ((res&128)==0)
  {  
   answer[0]=res; 
   break;
  }
	HAL_Delay(1);
 }
 if (n==65535) return(false);
 for(n=1;n<answer_size;n++)
 {
  answer[n]=SD_TransmitData(0xff);
 }
 SD_TransmitData(0xff);
 return(true);//����� ������
}
//----------------------------------------------------------------------------------------------------
//�������� ����� SD-����� � ������
//----------------------------------------------------------------------------------------------------
bool SD_GetSize(uint32_t *size)
{
 uint16_t n;
 uint8_t answer[ANSWER_R1_SIZE];
 if (SD_SendCommand(CMD9,0x00,0x00,0x00,0x00,ANSWER_R1_SIZE,answer)==false) return(false);//����� �� ������
 //��������� 16 ���� ������ ������ R1
 uint8_t byte=0;
 for(n=0;n<65535;n++)
 {
  byte=SD_TransmitData(0xff);
  if (byte!=0xff) break;
 }
 if (n==65535) return(false);//����� �� ������
 uint8_t b[16];
 n=0;
 if (byte!=0xfe)//�������� ���� �� ��� ������ �������� ������ ������
 {
  b[0]=byte;
  n++;
 }
 for(;n<16;n++) b[n]=SD_TransmitData(0xff);
 //������� ������ ����� ������
 uint32_t blocks=0;
 if (SDType==SD_TYPE_SD_V2_HC)//������� CSD ������
 {
  blocks=GetBits(b,69,48);
  uint32_t read_bl_len=GetBits(b,83,80);
  uint32_t block_size=(1UL<<read_bl_len);
  //���� ��� ����������, ��� �� ���� ���� �������� ����� ����� ������ 

 
 }
 else//������� SD-�����
 {
  //���������, �������� SDCardManual
  uint32_t read_bl_len=GetBits(b,83,80);
  uint32_t c_size=GetBits(b,73,62);
  uint32_t c_size_mult=GetBits(b,49,47);
  blocks=(c_size+1UL)*(1UL<<(c_size_mult+2UL));
  blocks*=(1UL<<read_bl_len);
 }  
 *size=blocks;
 return(true);
}
//----------------------------------------------------------------------------------------------------
//������ ������ �����
//----------------------------------------------------------------------------------------------------
bool SD_BeginReadBlock(uint32_t BlockAddr)
{
 if (SDType!=SD_TYPE_SD_V2_HC) BlockAddr<<=9;//�������� �� 512 ��� ������ ���� ������
 //��� ������� ������ �����
 uint8_t a1=(uint8_t)((BlockAddr>>24)&0xff);
 uint8_t a2=(uint8_t)((BlockAddr>>16)&0xff);
 uint8_t a3=(uint8_t)((BlockAddr>>8)&0xff);
 uint8_t a4=(uint8_t)(BlockAddr&0xff);
 uint8_t answer[ANSWER_R1_SIZE];
 bool ret=SD_SendCommand(CMD17,a4,a3,a2,a1,ANSWER_R1_SIZE,answer);//�������� CMD17
 if (ret==false || answer[0]!=0) return(false);//������ �������
 SD_TransmitData(0xff);//�������� ����������
 //��� ������ ����������� ������
 uint16_t n;
 for(n=0;n<65535;n++)
 {
  uint8_t res=SD_TransmitData(0xff);
  if (res==0xfe) break;//������ �������
  HAL_Delay(1);
 }
 if (n==65535) return(false);//������ ������ ������ �� �������
 BlockByteCounter=0;
 return(true);
}
//----------------------------------------------------------------------------------------------------
//������� ���� �����
//----------------------------------------------------------------------------------------------------
bool SD_ReadBlockByte(uint8_t *byte)
{
 if (BlockByteCounter>=512) return(false);
 *byte=SD_TransmitData(0xff);//������ ���� � SD-�����
 BlockByteCounter++;
 if (BlockByteCounter==512)
 {
  //��������� CRC
  SD_TransmitData(0xff);
  SD_TransmitData(0xff); 
 }
 return(true);
}
//----------------------------------------------------------------------------------------------------
//������� ���� � 512 ���� � ������
//----------------------------------------------------------------------------------------------------
bool SD_ReadBlock(uint32_t BlockAddr,uint8_t *Addr)
{
 if (SDType!=SD_TYPE_SD_V2_HC) BlockAddr<<=9;//�������� �� 512 ��� ������ ���� ������
 //��� ������� ������ �����
 uint8_t a1=(uint8_t)((BlockAddr>>24)&0xff);
 uint8_t a2=(uint8_t)((BlockAddr>>16)&0xff);
 uint8_t a3=(uint8_t)((BlockAddr>>8)&0xff);
 uint8_t a4=(uint8_t)(BlockAddr&0xff);
 uint8_t answer[ANSWER_R1_SIZE];
 bool ret=SD_SendCommand(CMD17,a4,a3,a2,a1,ANSWER_R1_SIZE,answer);//�������� CMD17
 if (ret==false || answer[0]!=0) return(false);//������ �������
 SD_TransmitData(0xff);//�������� ����������
 //��� ������ ����������� ������
 uint16_t n;
 for(n=0;n<65535;n++)
 {
  uint8_t res=SD_TransmitData(0xff);
  if (res==0xfe) break;//������ �������
  HAL_Delay(1);
 }
 if (n==65535) return(false);//������ ������ ������ �� �������
 for(n=0;n<512;n++,Addr++)
 {
  *Addr=SD_TransmitData(0xff);//������ ���� � SD-�����
 }
 //��������� CRC
 SD_TransmitData(0xff);
 SD_TransmitData(0xff); 
 return(true);
}

//----------------------------------------------------------------------------------------------------
//�������� ���� � 512 ���� �� ������
//----------------------------------------------------------------------------------------------------
bool SD_WriteBlock(uint32_t BlockAddr,const uint8_t *Addr)
{
 if (SDType!=SD_TYPE_SD_V2_HC) BlockAddr<<=9;//�������� �� 512 ��� ������ ���� ������
 //��� ������� ������ �����
 uint8_t a1=(uint8_t)((BlockAddr>>24)&0xff);
 uint8_t a2=(uint8_t)((BlockAddr>>16)&0xff);
 uint8_t a3=(uint8_t)((BlockAddr>>8)&0xff);
 uint8_t a4=(uint8_t)(BlockAddr&0xff);
 uint8_t answer[ANSWER_R1_SIZE];
 bool ret=SD_SendCommand(CMD24,a4,a3,a2,a1,ANSWER_R1_SIZE,answer);//�������� CMD24
 if (ret==false || answer[0]!=0) return(false);//������ �������
 SD_TransmitData(0xff);//�������� ����������
 SD_TransmitData(0xfe);//������ ������ ������ �����
 uint16_t CRC16=0;	
 for(uint16_t n=0;n<512;n++,Addr++) 
 {
  uint8_t b=*Addr;
	SD_TransmitData(b);//�������� ���� SD-�����
  CRC16^=b<<8;
  for (uint8_t i=0;i<8;i++) 
  { 
   if (CRC16&0x8000) CRC16=(CRC16<<1)^0x1021;
                else CRC16<<=1;	 
  }
 }
 //�������� CRC
 SD_TransmitData((CRC16>>8)&0xff);
 SD_TransmitData(CRC16&0xff);
 //�������� ����� �����
 uint8_t res=SD_TransmitData(0xff);
 if ((res&0x05)!=0x05) return(false);//���� ����� �� "������ �������"
 SD_TransmitData(0xff);//�������� ����������
 SD_TransmitData(0xff);//�������� ����������
 //��� ���������� ������ (����� DO ��������� � 0, ���� ����� ������)
 for(uint16_t n=0;n<65535;n++)
 {
  res=SD_TransmitData(0xff);
  if (res==0xff) break;//����� DO ����� � 1; ����� ��������� ������
	HAL_Delay(1);
 } 
 return(true);
}
