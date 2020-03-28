//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include "cdisplayil9325.h"
#include "stm32f4xx_hal.h"

//****************************************************************************************************
//глобальные переменные
//****************************************************************************************************

//****************************************************************************************************
//макроопределения
//****************************************************************************************************

//контакты дисплея
#define IL9325_GPIO_PIN_CS		GPIO_PIN_7
#define IL9325_GPIO_CS				GPIOD

#define IL9325_GPIO_PIN_RST	GPIO_PIN_1
#define IL9325_GPIO_RST			GPIOE

#define IL9325_GPIO_PIN_RS		GPIO_PIN_11
#define IL9325_GPIO_RS				GPIOD

#define IL9325_GPIO_PIN_WR		GPIO_PIN_5
#define IL9325_GPIO_WR				GPIOD

#define IL9325_GPIO_PIN_RD		GPIO_PIN_4
#define IL9325_GPIO_RD				GPIOD


#define IL9325_GPIO_PIN_D0		GPIO_PIN_14
#define IL9325_GPIO_D0				GPIOD

#define IL9325_GPIO_PIN_D1		GPIO_PIN_15
#define IL9325_GPIO_D1				GPIOD

#define IL9325_GPIO_PIN_D2		GPIO_PIN_0
#define IL9325_GPIO_D2				GPIOD

#define IL9325_GPIO_PIN_D3		GPIO_PIN_1
#define IL9325_GPIO_D3				GPIOD

#define IL9325_GPIO_PIN_D4		GPIO_PIN_7
#define IL9325_GPIO_D4				GPIOE

#define IL9325_GPIO_PIN_D5		GPIO_PIN_8
#define IL9325_GPIO_D5				GPIOE

#define IL9325_GPIO_PIN_D6		GPIO_PIN_9
#define IL9325_GPIO_D6				GPIOE

#define IL9325_GPIO_PIN_D7		GPIO_PIN_10
#define IL9325_GPIO_D7				GPIOE

//определяем адреса для записи данных
#define IL9325_DATA 0x60010000UL
// для записи команд
#define IL9325_REG  0x60000000UL

//код команды паузы в миллисекундах
#define ILI932X_DELAY_CODE 0xFFFF

//регистры дисплея

#define ILI932X_START_OSC          0x00
#define ILI932X_DRIV_OUT_CTRL      0x01
#define ILI932X_DRIV_WAV_CTRL      0x02
#define ILI932X_ENTRY_MOD          0x03
#define ILI932X_RESIZE_CTRL        0x04
#define ILI932X_DISP_CTRL1         0x07
#define ILI932X_DISP_CTRL2         0x08
#define ILI932X_DISP_CTRL3         0x09
#define ILI932X_DISP_CTRL4         0x0A
#define ILI932X_RGB_DISP_IF_CTRL1  0x0C
#define ILI932X_FRM_MARKER_POS     0x0D
#define ILI932X_RGB_DISP_IF_CTRL2  0x0F
#define ILI932X_POW_CTRL1          0x10
#define ILI932X_POW_CTRL2          0x11
#define ILI932X_POW_CTRL3          0x12
#define ILI932X_POW_CTRL4          0x13
#define ILI932X_GRAM_HOR_AD        0x20
#define ILI932X_GRAM_VER_AD        0x21
#define ILI932X_RW_GRAM            0x22
#define ILI932X_POW_CTRL7          0x29
#define ILI932X_FRM_RATE_COL_CTRL  0x2B
#define ILI932X_GAMMA_CTRL1        0x30
#define ILI932X_GAMMA_CTRL2        0x31
#define ILI932X_GAMMA_CTRL3        0x32
#define ILI932X_GAMMA_CTRL4        0x35
#define ILI932X_GAMMA_CTRL5        0x36
#define ILI932X_GAMMA_CTRL6        0x37
#define ILI932X_GAMMA_CTRL7        0x38
#define ILI932X_GAMMA_CTRL8        0x39
#define ILI932X_GAMMA_CTRL9        0x3C
#define ILI932X_GAMMA_CTRL10       0x3D
#define ILI932X_HOR_START_AD       0x50
#define ILI932X_HOR_END_AD         0x51
#define ILI932X_VER_START_AD       0x52
#define ILI932X_VER_END_AD         0x53
#define ILI932X_GATE_SCAN_CTRL1    0x60
#define ILI932X_GATE_SCAN_CTRL2    0x61
#define ILI932X_GATE_SCAN_CTRL3    0x6A
#define ILI932X_PART_IMG1_DISP_POS 0x80
#define ILI932X_PART_IMG1_START_AD 0x81
#define ILI932X_PART_IMG1_END_AD   0x82
#define ILI932X_PART_IMG2_DISP_POS 0x83
#define ILI932X_PART_IMG2_START_AD 0x84
#define ILI932X_PART_IMG2_END_AD   0x85
#define ILI932X_PANEL_IF_CTRL1     0x90
#define ILI932X_PANEL_IF_CTRL2     0x92
#define ILI932X_PANEL_IF_CTRL3     0x93
#define ILI932X_PANEL_IF_CTRL4     0x95
#define ILI932X_PANEL_IF_CTRL5     0x97
#define ILI932X_PANEL_IF_CTRL6     0x98


//****************************************************************************************************
//константы
//****************************************************************************************************

//инициализация дисплея
const unsigned short IL9325_InitList[]=
{
  ILI932X_START_OSC        , 0x0001, // Start oscillator
  ILI932X_DELAY_CODE             , 50,     // 50 millisecond delay
  ILI932X_DRIV_OUT_CTRL    , 0x0100,
  ILI932X_DRIV_WAV_CTRL    , 0x0700,
  ILI932X_ENTRY_MOD        , 0x1030,
  ILI932X_RESIZE_CTRL      , 0x0000,
  ILI932X_DISP_CTRL2       , 0x0202,
  ILI932X_DISP_CTRL3       , 0x0000,
  ILI932X_DISP_CTRL4       , 0x0000,
  ILI932X_RGB_DISP_IF_CTRL1, 0x0,
  ILI932X_FRM_MARKER_POS   , 0x0,
  ILI932X_RGB_DISP_IF_CTRL2, 0x0,
  ILI932X_POW_CTRL1        , 0x0000,
  ILI932X_POW_CTRL2        , 0x0007,
  ILI932X_POW_CTRL3        , 0x0000,
  ILI932X_POW_CTRL4        , 0x0000,
  ILI932X_DELAY_CODE       , 50,
  ILI932X_POW_CTRL1        , 0x1690,
  ILI932X_POW_CTRL2        , 0x0227,
  ILI932X_DELAY_CODE       , 50,
  ILI932X_POW_CTRL3        , 0x001A,
  ILI932X_DELAY_CODE       , 50,
  ILI932X_POW_CTRL4        , 0x1800,
  ILI932X_POW_CTRL7        , 0x002A,
  ILI932X_DELAY_CODE       , 50,
  ILI932X_GAMMA_CTRL1      , 0x0000,
  ILI932X_GAMMA_CTRL2      , 0x0000,
  ILI932X_GAMMA_CTRL3      , 0x0000,
  ILI932X_GAMMA_CTRL4      , 0x0206,
  ILI932X_GAMMA_CTRL5      , 0x0808,
  ILI932X_GAMMA_CTRL6      , 0x0007,
  ILI932X_GAMMA_CTRL7      , 0x0201,
  ILI932X_GAMMA_CTRL8      , 0x0000,
  ILI932X_GAMMA_CTRL9      , 0x0000,
  ILI932X_GAMMA_CTRL10     , 0x0000,
  ILI932X_GRAM_HOR_AD      , 0x0000,
  ILI932X_GRAM_VER_AD      , 0x0000,
  ILI932X_HOR_START_AD     , 0x0000,
  ILI932X_HOR_END_AD       , 0x00EF,
  ILI932X_VER_START_AD     , 0X0000,
  ILI932X_VER_END_AD       , 0x013F,
  ILI932X_GATE_SCAN_CTRL1  , 0xA700, // Driver Output Control (R60h)
  ILI932X_GATE_SCAN_CTRL2  , 0x0003, // Driver Output Control (R61h)
  ILI932X_GATE_SCAN_CTRL3  , 0x0000, // Driver Output Control (R62h)
  ILI932X_PANEL_IF_CTRL1   , 0x0010, // Panel Interface Control 1 (R90h)
  ILI932X_PANEL_IF_CTRL2   , 0x0000,
  ILI932X_PANEL_IF_CTRL3   , 0x0003,
  ILI932X_PANEL_IF_CTRL4   , 0x1100,
  ILI932X_PANEL_IF_CTRL5   , 0x0000,
  ILI932X_PANEL_IF_CTRL6   , 0x0000,
  ILI932X_DISP_CTRL1			 , 0x0133, // Main screen turn on	
//https://github.com/adafruit/TFTLCD-Library/blob/master/Adafruit_TFTLCD.cpp		
};

//****************************************************************************************************
//конструктор и деструктор
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//конструктор
//----------------------------------------------------------------------------------------------------
CDisplayIL9325::CDisplayIL9325(void)
{ 
}
//----------------------------------------------------------------------------------------------------
//деструктор
//----------------------------------------------------------------------------------------------------
CDisplayIL9325::~CDisplayIL9325()
{
}

//****************************************************************************************************
//закрытые функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//выставить на CS 1
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::CS_One(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_CS,IL9325_GPIO_PIN_CS,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на CS 0
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::CS_Zero(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_CS,IL9325_GPIO_PIN_CS,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RS 1
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::RS_One(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_RS,IL9325_GPIO_PIN_RS,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RS 0
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::RS_Zero(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_RS,IL9325_GPIO_PIN_RS,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на WR 1
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::WR_One(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_WR,IL9325_GPIO_PIN_WR,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на WR 0
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::WR_Zero(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_WR,IL9325_GPIO_PIN_WR,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RST 1
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::RST_One(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_RST,IL9325_GPIO_PIN_RST,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RST 0
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::RST_Zero(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_RST,IL9325_GPIO_PIN_RST,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RD 1
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::RD_One(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_RD,IL9325_GPIO_PIN_RD,GPIO_PIN_SET);	
}
//----------------------------------------------------------------------------------------------------
//выставить на RD 0
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::RD_Zero(void)
{
 HAL_GPIO_WritePin(IL9325_GPIO_RD,IL9325_GPIO_PIN_RD,GPIO_PIN_RESET);	
}
//----------------------------------------------------------------------------------------------------
//задать значение на шине данных
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::SetData(uint8_t byte)
{
}
//----------------------------------------------------------------------------------------------------
//передать байт
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::Write8(uint8_t byte)
{
 *((volatile uint8_t*)(IL9325_DATA))=byte;
}
//----------------------------------------------------------------------------------------------------
//передать 16 бит
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::Write16(uint16_t value)
{
 Write8((value>>8)&0xFF);
 Write8(value&0xFF);	
}
//----------------------------------------------------------------------------------------------------
//записать в регистр 16 бит
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::WriteRegister16(uint16_t reg,uint16_t value)
{
 *((volatile uint8_t*)(IL9325_REG))=(reg>>8)&0xFF;
 *((volatile uint8_t*)(IL9325_REG))=reg&0xFF;
 Write16(value);
}
//----------------------------------------------------------------------------------------------------
//выполнить сброс
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::Reset(void)
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
void CDisplayIL9325::InitGPIO(void)
{
 //настраиваем порты
 __HAL_RCC_GPIOD_CLK_ENABLE();
 __HAL_RCC_GPIOE_CLK_ENABLE();
	
 //GPIO_Pin – номера выводов, которые конфигурируются. Пример для нескольких выводов:
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
 //GPIO_Speed – задает скорость для выбранных выводов.
 //GPIO_Mode – задает режим работы выводов. 

 //настраиваем параметры портов
 GPIO_InitTypeDef IL9325_GPIO_Init;

 IL9325_GPIO_Init.Mode=GPIO_MODE_OUTPUT_PP;
 IL9325_GPIO_Init.Pull=GPIO_NOPULL;
 IL9325_GPIO_Init.Speed=GPIO_SPEED_FREQ_VERY_HIGH;


 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_CS;
 HAL_GPIO_Init(IL9325_GPIO_CS,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_RS;
 HAL_GPIO_Init(IL9325_GPIO_RS,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_RST;
 HAL_GPIO_Init(IL9325_GPIO_RST,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_RD;
 HAL_GPIO_Init(IL9325_GPIO_RD,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_WR;
 HAL_GPIO_Init(IL9325_GPIO_WR,&IL9325_GPIO_Init);
 

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D0;
 HAL_GPIO_Init(IL9325_GPIO_D0,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D1;
 HAL_GPIO_Init(IL9325_GPIO_D1,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D2;
 HAL_GPIO_Init(IL9325_GPIO_D2,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D3;
 HAL_GPIO_Init(IL9325_GPIO_D3,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D4;
 HAL_GPIO_Init(IL9325_GPIO_D4,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D5;
 HAL_GPIO_Init(IL9325_GPIO_D5,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D6;
 HAL_GPIO_Init(IL9325_GPIO_D6,&IL9325_GPIO_Init);

 IL9325_GPIO_Init.Pin=IL9325_GPIO_PIN_D7;
 HAL_GPIO_Init(IL9325_GPIO_D7,&IL9325_GPIO_Init);
}

//----------------------------------------------------------------------------------------------------
//инициализация LCD-экрана
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::Init(void)
{
 //инициализируем дисплей
 Reset();
 CS_Zero();
 
 long command_amount=sizeof(IL9325_InitList)/sizeof(unsigned short);
 for(long n=0;n<command_amount;)
 {
  unsigned short reg=IL9325_InitList[n];
  n++;
  unsigned short value=IL9325_InitList[n];
  n++;
  if (reg==ILI932X_DELAY_CODE)//пауза
  {
   HAL_Delay(value);  
   continue;
  }
  //значение для регистра
  WriteRegister16(reg,value);  
 }	
}
//----------------------------------------------------------------------------------------------------
//задать окно вывода пикселей
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::SetWindow(uint16_t x_left,uint16_t y_top,uint16_t x_right,uint16_t y_bottom)
{
 //указываем окно	
 WriteRegister16(ILI932X_HOR_START_AD,x_left);
 WriteRegister16(ILI932X_HOR_END_AD,x_right);
 WriteRegister16(ILI932X_VER_START_AD,y_top);
 WriteRegister16(ILI932X_VER_END_AD,y_bottom);
 //указываем текущие координаты	
 WriteRegister16(ILI932X_GRAM_HOR_AD,x_left);
 WriteRegister16(ILI932X_GRAM_VER_AD,y_top);
 //выбираем регистр вывода изображения
 *((volatile uint8_t*)(IL9325_REG))=(ILI932X_RW_GRAM>>8)&0xFF;
 *((volatile uint8_t*)(IL9325_REG))=ILI932X_RW_GRAM&0xFF;
 //передаём данные картинки
}
//----------------------------------------------------------------------------------------------------
//передать цвет точки
//----------------------------------------------------------------------------------------------------
void CDisplayIL9325::OutColor(uint16_t color)
{
 Write16(color);
}
