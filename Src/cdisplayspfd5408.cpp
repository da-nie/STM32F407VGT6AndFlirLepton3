//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include "cdisplayspfd5408.h"
#include "stm32f4xx_hal.h"

//****************************************************************************************************
//глобальные переменные
//****************************************************************************************************

//****************************************************************************************************
//макроопределения
//****************************************************************************************************

//контакты дисплея
#define SPFD5408_GPIO_PIN_CS		GPIO_PIN_7
#define SPFD5408_GPIO_CS				GPIOD

#define SPFD5408_GPIO_PIN_RST	GPIO_PIN_1
#define SPFD5408_GPIO_RST			GPIOE

#define SPFD5408_GPIO_PIN_RS		GPIO_PIN_11
#define SPFD5408_GPIO_RS				GPIOD

#define SPFD5408_GPIO_PIN_WR		GPIO_PIN_5
#define SPFD5408_GPIO_WR				GPIOD

#define SPFD5408_GPIO_PIN_RD		GPIO_PIN_4
#define SPFD5408_GPIO_RD				GPIOD


#define SPFD5408_GPIO_PIN_D0		GPIO_PIN_14
#define SPFD5408_GPIO_D0				GPIOD

#define SPFD5408_GPIO_PIN_D1		GPIO_PIN_15
#define SPFD5408_GPIO_D1				GPIOD

#define SPFD5408_GPIO_PIN_D2		GPIO_PIN_0
#define SPFD5408_GPIO_D2				GPIOD

#define SPFD5408_GPIO_PIN_D3		GPIO_PIN_1
#define SPFD5408_GPIO_D3				GPIOD

#define SPFD5408_GPIO_PIN_D4		GPIO_PIN_7
#define SPFD5408_GPIO_D4				GPIOE

#define SPFD5408_GPIO_PIN_D5		GPIO_PIN_8
#define SPFD5408_GPIO_D5				GPIOE

#define SPFD5408_GPIO_PIN_D6		GPIO_PIN_9
#define SPFD5408_GPIO_D6				GPIOE

#define SPFD5408_GPIO_PIN_D7		GPIO_PIN_10
#define SPFD5408_GPIO_D7				GPIOE

//определяем адреса для записи данных
#define SPFD5408_DATA 0x60010000UL
// для записи команд
#define SPFD5408_REG  0x60000000UL

//регистры дисплея

#define ILI9341_SOFTRESET          0x01
#define ILI9341_SLEEPIN            0x10
#define ILI9341_SLEEPOUT           0x11
#define ILI9341_NORMALDISP         0x13
#define ILI9341_INVERTOFF          0x20
#define ILI9341_INVERTON           0x21
#define ILI9341_GAMMASET           0x26
#define ILI9341_DISPLAYOFF         0x28
#define ILI9341_DISPLAYON          0x29
#define ILI9341_COLADDRSET         0x2A
#define ILI9341_PAGEADDRSET        0x2B
#define ILI9341_MEMORYWRITE        0x2C
#define ILI9341_PIXELFORMAT        0x3A
#define ILI9341_FRAMECONTROL       0xB1
#define ILI9341_DISPLAYFUNC        0xB6
#define ILI9341_ENTRYMODE          0xB7
#define ILI9341_POWERCONTROL1      0xC0
#define ILI9341_POWERCONTROL2      0xC1
#define ILI9341_VCOMCONTROL1       0xC5
#define ILI9341_VCOMCONTROL2       0xC7
#define ILI9341_MEMCONTROL         0x36
#define ILI9341_MADCTL             0x36

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

//****************************************************************************************************
//константы
//****************************************************************************************************

//****************************************************************************************************
//конструктор и деструктор
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//конструктор
//----------------------------------------------------------------------------------------------------
CDisplaySPFD5408::CDisplaySPFD5408(void)
{ 
}
//----------------------------------------------------------------------------------------------------
//деструктор
//----------------------------------------------------------------------------------------------------
CDisplaySPFD5408::~CDisplaySPFD5408()
{
}

//****************************************************************************************************
//закрытые функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//выставить на CS 1
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::CS_One(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_CS,SPFD5408_GPIO_PIN_CS,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на CS 0
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::CS_Zero(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_CS,SPFD5408_GPIO_PIN_CS,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RS 1
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::RS_One(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_RS,SPFD5408_GPIO_PIN_RS,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RS 0
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::RS_Zero(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_RS,SPFD5408_GPIO_PIN_RS,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на WR 1
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::WR_One(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_WR,SPFD5408_GPIO_PIN_WR,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на WR 0
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::WR_Zero(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_WR,SPFD5408_GPIO_PIN_WR,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RST 1
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::RST_One(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_RST,SPFD5408_GPIO_PIN_RST,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RST 0
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::RST_Zero(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_RST,SPFD5408_GPIO_PIN_RST,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RD 1
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::RD_One(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_RD,SPFD5408_GPIO_PIN_RD,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RD 0
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::RD_Zero(void)
{
 HAL_GPIO_WritePin(SPFD5408_GPIO_RD,SPFD5408_GPIO_PIN_RD,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//задать значение на шине данных
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::SetData(uint8_t byte)
{
 uint32_t inv_byte=0xFF^byte;
 uint32_t pr_byte=byte;
 SPFD5408_GPIO_D0->BSRR=(pr_byte<<4UL)|(inv_byte<<(4UL+16UL));
}
//----------------------------------------------------------------------------------------------------
//передать байт
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::Write8(uint8_t byte)
{
 *((volatile uint8_t*)(SPFD5408_DATA))=byte;
}
//----------------------------------------------------------------------------------------------------
//передать 16 бит
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::Write16(uint16_t value)
{
 Write8((value>>8)&0xFF);
 Write8(value&0xFF);	
}
//----------------------------------------------------------------------------------------------------
//записать в регистр 8 бит
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::WriteRegister8(uint8_t reg,uint8_t value) 
{ 
 *((volatile uint8_t*)(SPFD5408_REG))=reg;
 Write8(value); 
}
//----------------------------------------------------------------------------------------------------
//записать в регистр 16 бит
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::WriteRegister16(uint16_t reg,uint16_t value)
{
 *((volatile uint8_t*)(SPFD5408_REG))=(reg>>8)&0xFF;
 *((volatile uint8_t*)(SPFD5408_REG))=reg&0xFF;
 Write16(value);
}

//----------------------------------------------------------------------------------------------------
//выполнить сброс
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::Reset(void)
{
 CS_One();
 RS_One();
 RD_One();
 WR_One();
 RST_One();    
 HAL_Delay(10);
 RST_Zero();
 HAL_Delay(10);
 RST_One();
 HAL_Delay(250);
}

//****************************************************************************************************
//открытые функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//инициализация контактов дисплея
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::InitGPIO(void)
{
 //настраиваем порты
 __HAL_RCC_GPIOD_CLK_ENABLE();
 __HAL_RCC_GPIOE_CLK_ENABLE();
	
 //GPIO_Pin – номера выводов, которые конфигурируются. Пример для нескольких выводов:
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
 //GPIO_Speed – задает скорость для выбранных выводов.
 //GPIO_Mode – задает режим работы выводов. 

 //настраиваем параметры портов
 GPIO_InitTypeDef SPFD5408_GPIO_Init;

 SPFD5408_GPIO_Init.Mode=GPIO_MODE_OUTPUT_PP;
 SPFD5408_GPIO_Init.Pull=GPIO_NOPULL;
 SPFD5408_GPIO_Init.Speed=GPIO_SPEED_FREQ_VERY_HIGH;


 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_CS;
 HAL_GPIO_Init(SPFD5408_GPIO_CS,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_RS;
 HAL_GPIO_Init(SPFD5408_GPIO_RS,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_RST;
 HAL_GPIO_Init(SPFD5408_GPIO_RST,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_RD;
 HAL_GPIO_Init(SPFD5408_GPIO_RD,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_WR;
 HAL_GPIO_Init(SPFD5408_GPIO_WR,&SPFD5408_GPIO_Init);
 

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D0;
 HAL_GPIO_Init(SPFD5408_GPIO_D0,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D1;
 HAL_GPIO_Init(SPFD5408_GPIO_D1,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D2;
 HAL_GPIO_Init(SPFD5408_GPIO_D2,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D3;
 HAL_GPIO_Init(SPFD5408_GPIO_D3,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D4;
 HAL_GPIO_Init(SPFD5408_GPIO_D4,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D5;
 HAL_GPIO_Init(SPFD5408_GPIO_D5,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D6;
 HAL_GPIO_Init(SPFD5408_GPIO_D6,&SPFD5408_GPIO_Init);

 SPFD5408_GPIO_Init.Pin=SPFD5408_GPIO_PIN_D7;
 HAL_GPIO_Init(SPFD5408_GPIO_D7,&SPFD5408_GPIO_Init);
}

//----------------------------------------------------------------------------------------------------
//инициализация LCD-экрана
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::Init(void)
{
 //инициализируем дисплей
 Reset();
 CS_Zero();
 
 WriteRegister8(ILI9341_SOFTRESET,0);
 HAL_Delay(50);
 WriteRegister8(ILI9341_DISPLAYOFF,0);
 WriteRegister8(ILI9341_POWERCONTROL1,0x23);
 WriteRegister8(ILI9341_POWERCONTROL2,0x10);
 WriteRegister16(ILI9341_VCOMCONTROL1,0x2B2B);
 WriteRegister8(ILI9341_VCOMCONTROL2,0xC0);
 WriteRegister8(ILI9341_MEMCONTROL,ILI9341_MADCTL_MY|ILI9341_MADCTL_BGR);
 WriteRegister8(ILI9341_PIXELFORMAT,0x55);
 WriteRegister16(ILI9341_FRAMECONTROL,0x001B);    
 WriteRegister8(ILI9341_ENTRYMODE,0x07);
 WriteRegister8(ILI9341_SLEEPOUT,0);
 HAL_Delay(150);
 WriteRegister8(ILI9341_DISPLAYON,0);
 HAL_Delay(500);	
}
//----------------------------------------------------------------------------------------------------
//задать окно вывода пикселей
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::SetWindow(uint16_t x_left,uint16_t y_top,uint16_t x_right,uint16_t y_bottom)
{
 *((volatile uint8_t*)(SPFD5408_REG))=ILI9341_COLADDRSET&0xFF;
 Write16(x_left);	
 Write16(x_right);	

 *((volatile uint8_t*)(SPFD5408_REG))=ILI9341_PAGEADDRSET&0xFF;
 Write16(y_top);	
 Write16(y_bottom);	

 *((volatile uint8_t*)(SPFD5408_REG))=ILI9341_MEMORYWRITE&0xFF;
}
//----------------------------------------------------------------------------------------------------
//передать цвет точки
//----------------------------------------------------------------------------------------------------
void CDisplaySPFD5408::OutColor(uint16_t color)
{
 Write16(color);
}

