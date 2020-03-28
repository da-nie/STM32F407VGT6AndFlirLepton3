//----------------------------------------------------------------------------------------------------
//подключаемые библиотеки
//----------------------------------------------------------------------------------------------------
#include "processing.h"
#include "stm32f4xx_hal.h"
//#include "cdisplayil9325.h"
//#include "cdisplayspfd5408.h"
#include "cdisplayhx8347d.h"
#include "ff.h"
#include "sd.h"
#include "button.h"
#include "leptoncontrol.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

//----------------------------------------------------------------------------------------------------
//макроопределения
//----------------------------------------------------------------------------------------------------

//размер принимаемых блоком данных по SPI
#define SPI_READ_VOSPI_AMOUNT 59
//размер палитры
#define PALETTE_SIZE 256
//максимальный индекс сохраняемого кадра
#define MAX_FRAME_INDEX 65536

//----------------------------------------------------------------------------------------------------
//константы
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//перечисления
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//глобальные переменные
//----------------------------------------------------------------------------------------------------
static uint8_t buffer[VOSPI_PACKAGE_SIZE*SPI_READ_VOSPI_AMOUNT];//буфер сборки изображения
static uint16_t ColorMap[PALETTE_SIZE];//палитра

static I2C_HandleTypeDef hi2c1;
static SPI_HandleTypeDef hspi1;
static SRAM_HandleTypeDef hsram1;
static uint32_t FrameIndex=0;//индекс сохраняемого кадра
static bool SD_Enabled=false;//есть ли SD-карта

//указатель на дисплей
CDisplayHX8347D cDisplay;
//CDisplaySPFD5408 cDisplay;
//CDisplayIL9325 cDisplay;
IDisplay *iDisplay_Ptr=&cDisplay;


//----------------------------------------------------------------------------------------------------
//прототипы функций
//----------------------------------------------------------------------------------------------------
static void CreatePalette(void);//создание палитры
static void FindFirstFrameIndex(void);//поиск первого кадра
static bool GetTemperature(uint16_t raw14,float *ret_value);//вычисление температуры по показаниям болометра
static void SaveFrame(void);//сохранение кадра
static void CreateImage(void);//создание и вывод изображения 
static void MX_I2C1_Init(void);//инициализация I2C
static void MX_SPI1_Init(void);//инициализация SPI1
static void MX_GPIO_Init(void);//инициализация портов
static void MX_FSMC_Init(void);//инициализация FSMC
static void FindSD(void);//поиск и инициализация SD-карты
static FATFS fs; 

//------------------------------------------------------------------------------------------
//создание палитры
//------------------------------------------------------------------------------------------
void CreatePalette(void)
{
 for(uint16_t n=0;n<PALETTE_SIZE;n++)
 {	
  uint16_t color=n>>3;
  color<<=6;
	color|=n>>2;
	color<<=5;
	color|=n>>3;
  ColorMap[n]=color;
 }
 if (SD_Enabled==false) return;
 
 iDisplay_Ptr->Print("Загрузка палитры",IDisplay::COLOR_YELLOW);
 FIL file;
 if (f_open(&file,"Palette.pal",FA_READ)==FR_OK)
 {
  iDisplay_Ptr->Print("Файл Palette.pal найден",IDisplay::COLOR_YELLOW);
	int32_t n;
  for(n=0;n<PALETTE_SIZE;n++)
  {	
   uint8_t r;
   uint8_t g;
   uint8_t b;
   UINT readen;
   if (f_read(&file,&r,sizeof(uint8_t),&readen)!=FR_OK) break;
   if (readen==0) break;	 
   if (f_read(&file,&g,sizeof(uint8_t),&readen)!=FR_OK) break;
   if (readen==0) break;
   if (f_read(&file,&b,sizeof(uint8_t),&readen)!=FR_OK) break;
   if (readen==0) break;
		
	 r>>=3;
	 g>>=2;
	 b>>=3;
   uint16_t color=r;
   color<<=6;
   color|=g;
 	 color<<=5;
 	 color|=b;
   ColorMap[n]=color;
  } 	 
  f_close(&file);
	if (n!=PALETTE_SIZE) iDisplay_Ptr->Print("Ошибка загрузки палитры",IDisplay::COLOR_YELLOW);
	                else iDisplay_Ptr->Print("Палитра загружена",IDisplay::COLOR_YELLOW);
 }
 else
 {
	iDisplay_Ptr->Print("Загрузка палитры не удалась",IDisplay::COLOR_YELLOW);
	HAL_Delay(2000);
 } 
}
//------------------------------------------------------------------------------------------
//поиск первого кадра
//------------------------------------------------------------------------------------------
void FindFirstFrameIndex(void)
{
 if (SD_Enabled==false) return;	
 //определяем номер следующего изображения для записи
 char name[20];
 FrameIndex=0;
 for(;FrameIndex<MAX_FRAME_INDEX;FrameIndex++)
 {
  sprintf(name,"%i.raw",FrameIndex);
  FIL file;
  if (f_open(&file,name,FA_READ)==FR_OK)
	{
   f_close(&file);
   continue;    	 
  }
  break;
 } 
}

//------------------------------------------------------------------------------------------
//вычисление температуры по показаниям болометра
//------------------------------------------------------------------------------------------
bool GetTemperature(uint16_t raw14,float *ret_value)
{
 static const double EPS=0.00000001;
 static const double PlanckR1=16528.178;
 static const double PlanckB=1427.5;
 static const double PlanckF=1.0;
 static const double PlanckO=-1307.0;
 static const double PlanckR2=0.012258549;
 static const double TempReflected=20;
 static const double Emissivity=0.95;
	
 //if (raw14>0x10000) return(false);
 raw14*=4;
 double zn=(PlanckR2*(exp(PlanckB/(TempReflected+273.15))-PlanckF));
 if (fabs(zn)<EPS) return(false);
 double RAWrefl=PlanckR1/zn-PlanckO;
 double RAWobj=(raw14-(1-Emissivity)*RAWrefl)/Emissivity;
 double lgr=PlanckR1/(PlanckR2*(RAWobj+PlanckO)+PlanckF);
 if (lgr<=EPS) return(false);
 *ret_value=PlanckB/log(lgr)-273.15; 
 return(true);	
}
//------------------------------------------------------------------------------------------
//сохранение кадра
//------------------------------------------------------------------------------------------
void SaveFrame(void)
{
 if (SD_Enabled==false) return;
 uint16_t *raw14_ptr=LEPTONCONTROL_GetRAW14Ptr();
 //сохраняем кадр
 FIL file_out;
 char name[20];
 sprintf(name,"%i.raw",FrameIndex);
 if (f_open(&file_out,name,FA_WRITE|FA_CREATE_ALWAYS)==FR_OK)
 {   
  #pragma pack(1)
  typedef struct
  {
   uint32_t Width;//ширина
   uint32_t Height;//высота
   //параметры для расчёта температуры
   double PlanckR1;
   double PlanckB;
   double PlanckF;
   double PlanckO;
   double PlanckR2;
   double TempReflected;//температура болометров
   double Emissivity;//коэффициент излучения
  } SRAWHeader;//параметры RAW-изображения
  #pragma pack()
		
	SRAWHeader sRAWHeader;
	 
  sRAWHeader.Width=LEPTON_ORIGINAL_IMAGE_WIDTH;
  sRAWHeader.Height=LEPTON_ORIGINAL_IMAGE_HEIGHT;
  sRAWHeader.PlanckR1=16528.178;
  sRAWHeader.PlanckB=1427.5;
  sRAWHeader.PlanckF=1.0;
  sRAWHeader.PlanckO=-1307.0;
  sRAWHeader.PlanckR2=0.012258549;
  sRAWHeader.TempReflected=20;
  sRAWHeader.Emissivity=0.95;
		
  UINT writen;
  f_write(&file_out,(uint8_t*)&sRAWHeader,sizeof(SRAWHeader),&writen);//записываем заголовок
  f_write(&file_out,(uint8_t*)(raw14_ptr),sRAWHeader.Width*sRAWHeader.Height*sizeof(uint16_t),&writen);
  f_close(&file_out); 	
	 
	FrameIndex++;
 }
}


//------------------------------------------------------------------------------------------
//создание и вывод изображения 
//------------------------------------------------------------------------------------------
void CreateImage(void)
{  	
 uint16_t *raw14_ptr=LEPTONCONTROL_GetRAW14Ptr();
 uint32_t min=0x10000;
 uint32_t max=0;
 uint32_t y;
 uint32_t x;
 uint16_t *raw14_local_ptr;
 raw14_local_ptr=raw14_ptr;
 uint32_t width=LEPTON_ORIGINAL_IMAGE_WIDTH;
 uint32_t height=LEPTON_ORIGINAL_IMAGE_HEIGHT;
 for(y=0;y<height;y++)
 {
  for(x=0;x<width;x++,raw14_local_ptr++)
  {
   uint16_t raw14=*raw14_local_ptr;
   if (raw14>max) max=raw14;
   if (raw14<min) min=raw14;
  }
 }
 if (BUTTON_GetSaveState()==true) SaveFrame();
 /*
 float t_min=0;
 float t_max=0;
 //вычисляем диапазон температур
 if (GetTemperature(min,&t_min)==true && GetTemperature(max,&t_max)==true)
 {
  //отсекаем объекты с температурой выше температуры человека
  float delta_t=t_max-t_min;
	float delta=max-min;
	if (fabs(delta_t)<0.00001) delta_t=1;
	float k=delta/delta_t;
	//if (t_max>40) max=max-k*(t_max-40);	 
	//if (t_min<20) min=min+k*(20-t_min);	 
 } 
 
 */
 int32_t delta=max-min;
 if (delta==0) delta=1;
 raw14_local_ptr=raw14_ptr;
 //выполняем перекодирование к палитре (предварительная обработка позволит не ждать при выводе пикселей)
 for(y=0;y<height;y++)
 {
  for(x=0;x<width;x++,raw14_local_ptr++)
  {
   int32_t value=*raw14_local_ptr;
   value-=min;
   value=(value*255)/delta;
   if (value>255) value=255;
   if (value<0) value=0;
	 *raw14_local_ptr=ColorMap[value];		
	} 
 }
 //выводим раскрашенное изображение с удвоением строк и столбцов
 iDisplay_Ptr->SetWindow(0,0,IDisplay::DISPLAY_WIDTH-1,IDisplay::DISPLAY_HEIGHT-1); 
 //идем по экрану с переворотом изображения
 raw14_local_ptr=raw14_ptr;
  
 for(y=0;y<IDisplay::DISPLAY_HEIGHT/2;y++,raw14_local_ptr++)//0...319
 {
  for(int32_t n=0;n<2;n++)
  {	 
	 uint16_t *raw14_color_ptr=raw14_local_ptr+width*(IDisplay::DISPLAY_WIDTH/2-1);
	 for(x=0;x<IDisplay::DISPLAY_WIDTH/2;x++,raw14_color_ptr-=width)//0..239
   {
    uint16_t color=*raw14_color_ptr;
		iDisplay_Ptr->OutColor(color);
		iDisplay_Ptr->OutColor(color);
   }
  }
 }
 /*
 char str[50];
 sprintf(str,"%.1f..%.1f",t_min,t_max);
 iDisplay_Ptr->PutString(0,0,str,IDisplay::COLOR_CYAN);//вывод строчки в позицию 
 sprintf(str,"%i..%i",min,max);
 iDisplay_Ptr->PutString(0,IDisplay::FONT_HEIGHT,str,IDisplay::COLOR_CYAN);//вывод строчки в позицию 
 */
}

//------------------------------------------------------------------------------------------
//переопределенная функция получения времени
//------------------------------------------------------------------------------------------
DWORD get_fattime(void)
{
 uint32_t wYear=2019;
 uint32_t wMonth=12;
 uint32_t wDay=27;
 uint32_t wHour=9;
 uint32_t wMinute=23;
 uint32_t wSecond=20;
	
 DWORD ret=((DWORD)(wYear-1980)<<25);	
 ret|=((DWORD)wMonth<<21);
 ret|=((DWORD)wDay<<16);
 ret|=(WORD)(wHour<<11);
 ret|=(WORD)(wMinute<<5);
 ret|=(WORD)(wSecond>>1);
 return(ret);
}

//------------------------------------------------------------------------------------------
//инициализация
//------------------------------------------------------------------------------------------
void Init(void)
{
 //инициализируем порты LCD-экрана (это обязательно нужно делать до MX_FSMC_Init!)		
 iDisplay_Ptr->InitGPIO();
 //инициализируем кнопки	
 BUTTON_Init();	
 MX_GPIO_Init();	
 MX_I2C1_Init();
 MX_FSMC_Init();
 MX_SPI1_Init();	
}	
	

//------------------------------------------------------------------------------------------
//инициализация I2C
//------------------------------------------------------------------------------------------
static void MX_I2C1_Init(void)
{
 hi2c1.Instance=I2C1;
 hi2c1.Init.ClockSpeed=100000;
 hi2c1.Init.DutyCycle=I2C_DUTYCYCLE_2;
 hi2c1.Init.OwnAddress1=0;
 hi2c1.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
 hi2c1.Init.DualAddressMode=I2C_DUALADDRESS_DISABLE;
 hi2c1.Init.OwnAddress2=0;
 hi2c1.Init.GeneralCallMode=I2C_GENERALCALL_DISABLE;
 hi2c1.Init.NoStretchMode=I2C_NOSTRETCH_DISABLE;
 if (HAL_I2C_Init(&hi2c1)!=HAL_OK)
 {
  while(1);
 }
}

//------------------------------------------------------------------------------------------
//инициализация SPI1
//------------------------------------------------------------------------------------------
static void MX_SPI1_Init(void)
{
 hspi1.Instance=SPI1;
 hspi1.Init.Mode=SPI_MODE_MASTER;
 hspi1.Init.Direction=SPI_DIRECTION_2LINES;
 hspi1.Init.DataSize=SPI_DATASIZE_8BIT;
 hspi1.Init.CLKPolarity=SPI_POLARITY_LOW;
 hspi1.Init.CLKPhase=SPI_PHASE_1EDGE;
 hspi1.Init.NSS=SPI_NSS_SOFT;
 hspi1.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;
 hspi1.Init.FirstBit=SPI_FIRSTBIT_MSB;
 hspi1.Init.TIMode=SPI_TIMODE_DISABLE;
 hspi1.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
 hspi1.Init.CRCPolynomial=10;
 if (HAL_SPI_Init(&hspi1)!=HAL_OK)
 {
  while(1);
 }
}

//------------------------------------------------------------------------------------------
//инициализация портов
//------------------------------------------------------------------------------------------
static void MX_GPIO_Init(void)
{
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOH_CLK_ENABLE();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOE_CLK_ENABLE();
 __HAL_RCC_GPIOD_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
}

//------------------------------------------------------------------------------------------
//инициализация FSMC
//------------------------------------------------------------------------------------------
static void MX_FSMC_Init(void)
{
 FSMC_NORSRAM_TimingTypeDef Timing;
	
 hsram1.Instance=FSMC_NORSRAM_DEVICE;
 hsram1.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;
  
 hsram1.Init.NSBank=FSMC_NORSRAM_BANK1;
 hsram1.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE;
 hsram1.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;
 hsram1.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_8;
 hsram1.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE;
 hsram1.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;
 hsram1.Init.WrapMode=FSMC_WRAP_MODE_DISABLE;
 hsram1.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;
 hsram1.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;
 hsram1.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;
 hsram1.Init.ExtendedMode=FSMC_EXTENDED_MODE_DISABLE;
 hsram1.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;
 hsram1.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;
 hsram1.Init.PageSize=FSMC_PAGE_SIZE_NONE;

 Timing.AddressSetupTime=2;
 Timing.AddressHoldTime=15;
 Timing.DataSetupTime=5;
 Timing.BusTurnAroundDuration=0;
 Timing.CLKDivision=16;
 Timing.DataLatency=17;
 Timing.AccessMode=FSMC_ACCESS_MODE_A;

 if (HAL_SRAM_Init(&hsram1,&Timing,NULL)!=HAL_OK)
 {
  while(1);		
 }
}
//------------------------------------------------------------------------------------------
//поиск и инициализация SD-карты
//------------------------------------------------------------------------------------------
void FindSD(void)
{
 SD_Enabled=false;	
 iDisplay_Ptr->Print("Инициализация SD-карты",IDisplay::COLOR_YELLOW);
 SD_ANSWER sd_answer=SD_Init();
 if (sd_answer==SD_ANSWER_OK) iDisplay_Ptr->Print("SD:готова",IDisplay::COLOR_YELLOW);
 if (sd_answer==SD_ANSWER_ERROR) iDisplay_Ptr->Print("SD:не готова",IDisplay::COLOR_YELLOW);
 if (sd_answer==SD_ANSWER_SPI_ERROR) iDisplay_Ptr->Print("SD:настройка spi не удалась",IDisplay::COLOR_YELLOW);
 if (sd_answer==SD_ANSWER_SPI_NOT_SUPPORTED)iDisplay_Ptr->Print("SD:spi не поддерживается",IDisplay::COLOR_YELLOW);
 if (sd_answer==SD_ANSWER_NO_RESPONSE) iDisplay_Ptr->Print("SD:нет ответа от карты",IDisplay::COLOR_YELLOW);
 if (sd_answer==SD_ANSWER_SIZE_ERROR) iDisplay_Ptr->Print("SD:ошибка получения размера",IDisplay::COLOR_YELLOW);
 if (sd_answer!=SD_ANSWER_OK) 
 {
  HAL_Delay(2000);	 
	return;
 }
	
 FRESULT res; 
 res=f_mount(&fs,"",1);
 if (res==FR_INVALID_DRIVE) iDisplay_Ptr->Print("FR_INVALID_DRIVE",IDisplay::COLOR_YELLOW);
 if (res==FR_DISK_ERR) iDisplay_Ptr->Print("FR_DISK_ERR",IDisplay::COLOR_YELLOW);
 if (res==FR_NOT_READY) iDisplay_Ptr->Print("FR_NOT_READY",IDisplay::COLOR_YELLOW);
 if (res==FR_NO_FILESYSTEM) iDisplay_Ptr->Print("FR_NO_FILESYSTEM",IDisplay::COLOR_YELLOW);
 if (res!=FR_OK) 
 {
  HAL_Delay(2000);	 
	return;
 }
 iDisplay_Ptr->Print("Файловая система найдена",IDisplay::COLOR_YELLOW);	
 SD_Enabled=true;
}

//------------------------------------------------------------------------------------------
//основной цикл
//------------------------------------------------------------------------------------------
void Processing(void)
{
 //инициализируем модуль управления lepton3
 LEPTONCONTROL_Init();
 //инициализируем экран	
 iDisplay_Ptr->Init();
 //очищаем экран
 iDisplay_Ptr->Clear(IDisplay::COLOR_BLACK);	
 //делаем паузу для запуска lepton3
 HAL_Delay(100);	
 //запускам SD-карту	
 FindSD();
 //загружаем палитру
 CreatePalette();
 //ищем первый индекс кадра
 FindFirstFrameIndex();	
  
 while(1)
 {
  //ищем начало кадра		
  while(1) 
  {
   HAL_SPI_Receive(&hspi1,buffer,VOSPI_PACKAGE_SIZE,0x1000);			 
   bool first_line=false;
   uint8_t *buffer_ptr=buffer;
   LEPTONCONTROL_PushVoSPI(buffer_ptr,&first_line);
   if (first_line==true) break;
  }
  //читаем остаток пакета от lepton3
  uint8_t *buffer_ptr=buffer;
  HAL_SPI_Receive(&hspi1,buffer_ptr,VOSPI_PACKAGE_SIZE*SPI_READ_VOSPI_AMOUNT,0x1000);
  buffer_ptr=buffer;
  for(uint32_t n=0;n<SPI_READ_VOSPI_AMOUNT;n++,buffer_ptr+=VOSPI_PACKAGE_SIZE) 
  {
   bool first_line=false;
   bool res=LEPTONCONTROL_PushVoSPI(buffer_ptr,&first_line);
   if (res==true) CreateImage();
  } 
 }
}




