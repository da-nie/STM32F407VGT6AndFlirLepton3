#include "leptoncontrol.h"
#include "stm32f4xx_hal.h"

static uint16_t RAW14Image[LEPTON_ORIGINAL_IMAGE_HEIGHT*LEPTON_ORIGINAL_IMAGE_WIDTH];//собираемое изображение
static uint16_t CRCTable[256];//таблица для расчета CRC16

//время для ресинхронизации в мс
#define RESYNC_TIMEOUT_MS 19
//код: нет сегмента
#define NO_SEGMENT -1
//код: ошибка пакета
#define ERROR_PACKAGE -2

//----------------------------------------------------------------------------------------------------
//инициализация
//----------------------------------------------------------------------------------------------------
void LEPTONCONTROL_Init(void)
{
 //инициализируем таблицу для вычисления CRC	
 uint16_t code;
 for(int32_t n=0;n<256;n++)
 {
  code=((uint16_t)n)<<8;
  for(uint8_t m=0;m<8;m++)
  {
   if(code&(1<<15)) code=(code<<1)^0x1021;
               else code=code<<1;
  }
  CRCTable[n]=code;
 }
}

//----------------------------------------------------------------------------------------------------
//вычислить crc
//----------------------------------------------------------------------------------------------------
void LEPTONCONTROL_CalculateCRC(uint16_t *crc,uint8_t byte)
{
 *crc=CRCTable[(((*crc)>>8)^byte++)&0xFF]^((*crc)<<8);
/*	
 (*crc)^=(byte<<8);
 for(uint8_t n=0;n<8;n++) 
 { 
  if ((*crc)&0x8000) *crc=((*crc)<<1)^0x1021;
                else (*crc)<<=1;
 }
	*/
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
int32_t LEPTONCONTROL_ReadSegment(uint16_t *raw14_ptr,uint8_t data[VOSPI_PACKAGE_SIZE],bool *first_line)
{
 static int32_t current_package=-1;
 static int32_t segment=-1;
 int32_t n;
 *first_line=false;
 if ((data[0]&0x0F)==0x0F) return(NO_SEGMENT);//отбрасываемый пакет
 uint16_t crc=data[2];
 crc<<=8;
 crc|=data[3];
 //считаем CRC
 uint16_t crc16=0;
 LEPTONCONTROL_CalculateCRC(&crc16,data[0]&0x0F);
 LEPTONCONTROL_CalculateCRC(&crc16,data[1]);
 LEPTONCONTROL_CalculateCRC(&crc16,0);
 LEPTONCONTROL_CalculateCRC(&crc16,0);
 for(n=4;n<VOSPI_PACKAGE_SIZE;n++) LEPTONCONTROL_CalculateCRC(&crc16,data[n]);
 if (crc16!=crc) return(ERROR_PACKAGE);//ошибка CRC

 //определяем номер пакета
 uint16_t package=data[0]&0x0F;
 package<<=8;
 package|=data[1];
 if (package==0)
 {
  *first_line=true;
  current_package=0;
 }
 if (package==20)
 {
  uint8_t ttt=(data[0]&0x70)>>4;//номер кадра бывает только в 20 пакете
  segment=ttt;
 }
 if (current_package<0) return(NO_SEGMENT);
 if (current_package!=package)
 { 
  current_package=-1;
  return(ERROR_PACKAGE);
 }
 uint16_t *raw_ptr=raw14_ptr+current_package*VOSPI_PACKAGE_LINE_SIZE/2;
 for(n=0;n<VOSPI_PACKAGE_LINE_SIZE/2;n++,raw_ptr++)
 {
  //байты заданы в порядке big-endian: старший, младший
  uint16_t value=data[n*sizeof(int16_t)+4];
  value<<=8;
  value|=data[n*sizeof(int16_t)+5];
  *raw_ptr=value;
 }
 current_package++;
 if (current_package!=VOSPI_FRAME_HEIGHT) return(NO_SEGMENT);
 current_package=-1;
 return(segment); 
}

//----------------------------------------------------------------------------------------------------
//подать данные одного пакета VoSPI на вход модуля
//----------------------------------------------------------------------------------------------------
bool LEPTONCONTROL_PushVoSPI(uint8_t data[VOSPI_PACKAGE_SIZE],bool *first_line)
{
 *first_line=false;
 static int32_t waitable_segment=1;
 int32_t segment=LEPTONCONTROL_ReadSegment(RAW14Image+(waitable_segment-1)*VOSPI_FRAME_WIDTH*VOSPI_SEGMENT_LINE_AMOUNT,data,first_line);
 if (segment==ERROR_PACKAGE) HAL_Delay(RESYNC_TIMEOUT_MS);
 if (segment==ERROR_PACKAGE || segment==0) waitable_segment=1;
 if (segment==ERROR_PACKAGE || segment==NO_SEGMENT || segment==0) return(false);
 
 if (segment!=waitable_segment)
 {
  waitable_segment=1;
  if (segment!=1) return(false);
 }
 waitable_segment++;
 if (waitable_segment!=5) return(false);
 waitable_segment=1; 
 return(true);
}
//----------------------------------------------------------------------------------------------------
//получить указатель на данные собранного изображения
//----------------------------------------------------------------------------------------------------
uint16_t *LEPTONCONTROL_GetRAW14Ptr(void)
{
 return(RAW14Image);
}
