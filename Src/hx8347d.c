#include "hx8347d.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//макроопределени€
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//контакты диспле€
#define HX8347D_GPIO_PIN_CS		GPIO_PIN_7
#define HX8347D_GPIO_CS				GPIOD

#define HX8347D_GPIO_PIN_RST	GPIO_PIN_1
#define HX8347D_GPIO_RST			GPIOE

#define HX8347D_GPIO_PIN_RS		GPIO_PIN_11
#define HX8347D_GPIO_RS				GPIOD

#define HX8347D_GPIO_PIN_WR		GPIO_PIN_5
#define HX8347D_GPIO_WR				GPIOD

#define HX8347D_GPIO_PIN_RD		GPIO_PIN_4
#define HX8347D_GPIO_RD				GPIOD


#define HX8347D_GPIO_PIN_D0		GPIO_PIN_14
#define HX8347D_GPIO_D0				GPIOD

#define HX8347D_GPIO_PIN_D1		GPIO_PIN_15
#define HX8347D_GPIO_D1				GPIOD

#define HX8347D_GPIO_PIN_D2		GPIO_PIN_0
#define HX8347D_GPIO_D2				GPIOD

#define HX8347D_GPIO_PIN_D3		GPIO_PIN_1
#define HX8347D_GPIO_D3				GPIOD

#define HX8347D_GPIO_PIN_D4		GPIO_PIN_7
#define HX8347D_GPIO_D4				GPIOE

#define HX8347D_GPIO_PIN_D5		GPIO_PIN_8
#define HX8347D_GPIO_D5				GPIOE

#define HX8347D_GPIO_PIN_D6		GPIO_PIN_9
#define HX8347D_GPIO_D6				GPIOE

#define HX8347D_GPIO_PIN_D7		GPIO_PIN_10
#define HX8347D_GPIO_D7				GPIOE


//шина данных
//#define LCD_PORT_DATA  PORTA
//#define LCD_DDR_DATA   DDRA

//код команды паузы в миллисекундах
#define DELAY_CODE 0xFFFF


//----------------------------------------------------------------------------------------------------
//глобальные переменные
//----------------------------------------------------------------------------------------------------

//инициализаци€ диспле€
const unsigned short HX8347D_InitList[]=
{
 0x00EA,0x0000, //PTBA[15:8]  
 0x00EB,0x0020, //PTBA[7:0]  
 0x00EC,0x000C, //STBA[15:8]  
 0x00ED,0x00C4, //STBA[7:0]  
 0x00E8,0x0040, //OPON[7:0]  
 0x00E9,0x0038, //OPON1[7:0]  
 0x00F1,0x0001, //OTPS1B  
 0x00F2,0x0010, //GEN  
 0x0027,0x00A3, //  
	
 //Gamma 2.2 Setting  
 0x0040,0x0000, //  
 0x0041,0x0000, //  
 0x0042,0x0001, //  
 0x0043,0x0012, //  
 0x0044,0x0010, //  
 0x0045,0x0026, //  
 0x0046,0x0008, //  
 0x0047,0x0053, //  
 0x0048,0x0002, //  
 0x0049,0x0015, //  
 0x004A,0x0019, //  
 0x004B,0x0019, //  
 0x004C,0x0016, //  
 0x0050,0x0019, //  
 0x0051,0x002F, //  
 0x0052,0x002D, //  
 0x0053,0x003E, //  
 0x0054,0x003F, //  
 0x0055,0x003F, //  
 0x0056,0x002C, //  
 0x0057,0x0077, //  
 0x0058,0x0009, //  
 0x0059,0x0006, //  
 0x005A,0x0006, //  
 0x005B,0x000A, //  
 0x005C,0x001D, //  
 0x005D,0x00CC, //  
	
 //Power Voltage Setting  
 0x001B,0x001B, //VRH=4.65V  
 0x001A,0x0001, //BT (VGH~15V,VGL~-10V,DDVDH~5V)  
 0x0024,0x002F, //VMH(VCOM High voltage ~3.2V)  
 0x0025,0x0057, //VML(VCOM Low voltage -1.2V)  
	
 //****VCOM offset**///  
 0x0023,0x0097, //for Flicker adjust //can reload from OTP 
 
 //Power on Setting  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ, Normal mode 75Hz  
 0x0019,0x0001, //OSC_EN='1', start Osc  
 0x0001,0x0000, //DP_STB='0', out deep sleep  
 0x001F,0x0088,// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x0080,// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x0090,// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x00D0,// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
	
 //262k/65k color selection  
 0x0017,0x0005, //default 0x0006 262k color // 0x0005 65k color  
	
 //SET PANEL  
 0x0036,0x0000, //SS_P, GS_P,REV_P,BGR_P  
	
 //Display ON Setting  
 0x0028,0x0038, //GON=1, DTE=1, D=1000  
 DELAY_CODE,40,  //delay(40)
 0x0028,0x003C, //GON=1, DTE=1, D=1100  
	
 //Set GRAM Area  
 0x0002,0x0000,  
 0x0003,0x0000, //Column Start  
 0x0004,0x0000,  
 0x0005,0x00EF, //Column End  
 0x0006,0x0000,  
 0x0007,0x0000, //Row Start  
 0x0008,0x0001,  
 0x0009,0x003F, //Row End  
 //WR_8B_FORMAT(0x0022, //Start GRAM write  
 // Power Off Setting  
 0x0028,0x0038, //GON=°ѓ1°ѓ DTE=°ѓ1°ѓ D[1:0]=°ѓ10°ѓ  
 DELAY_CODE,40,  //delay(40)
 0x001F,0x0089, // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=1  
 DELAY_CODE,40,  //delay(40)
 0x0028,0x0004, //GON=°ѓ0°ѓ DTE=°ѓ0°ѓ D[1:0]=°ѓ01°ѓ  
 DELAY_CODE,40,  //delay(40)
 0x0019,0x0000, //OSC_EN=°ѓ0°ѓ  
 DELAY_CODE,5,  //delay(5)
 //M51_WR_REG(U05_LCD_POWER_ON, 0x0000, //VCI & IOVCC OFF 
 
 //##################################################################  
 // ENTER IDLE mode Setting  
 0x0018,0x0034, //I/P_RADJ,N/P_RADJ, IDLE mode 55Hz  
 0x002F,0x0011, //IDLE mode line inversion  
 0x0001,0x0004, //IDLE='1' , enter IDLE mode  
	
 // EXIT IDLE mode Setting  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ, Normal mode 75Hz  
 0x002F,0x0011, //Normal mode line inversion  
 0x0001,0x0000, //IDLE='0', EXIT IDLE mode  
	
 //##################################################################  
 // ENTER Partial mode Setting 32line  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ Partial mode 75Hz  
 0x002F,0x0011, //Partial mode line inversion  
 0x0001,0x0001, //PTL='1', Enter Partial mode  
 0x000A,0x0000, //PSL[15:8]=0x0000  
 0x000B,0x0020, //PSL[7:0]=0x0020  
 0x000C,0x0000, //PEL[15:8]=0x0000  
 0x000D,0x0047, //PEL[7:0]=0x0047  
 0x0026,0x0001, //refresh cycle=5frame  
	
 // EXIT Partial mode Setting  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ, Normal mode 75Hz  
 0x002F,0x0011, //Normal mode, line inversion  
 0x0001,0x0000, //PTL='0', EXIT Partial mode  
	
 // ENTER Partial + IDLE mode Setting 32line  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ, Partial mode 55Hz  
 0x002F,0x0011, // Partial + IDLE mode line inversion  
 0x0001,0x0009, // PTL='1' IDLE=°ѓ1°ѓ, Enter Partial + IDLE mode  
 0x000A,0x0000, //PSL[15:8]=0x0000  
 0x000B,0x0020, //PSL[7:0]=0x0020  
 0x000C,0x0000, //PEL[15:8]=0x0000  
 0x000D,0x0047, //PEL[7:0]=0x0047  
 0x0026,0x0001, //refresh cycle=5frame  
	
 // EXIT Partial + IDLE mode Setting  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ ,Normal mode 75Hz  
 0x002F,0x0011, //Normal mode, line inversion  
 0x0001,0x0000, // PTL='0' IDLE=°ѓ0°ѓ, EXIT Partial + IDLE mode  
	
 //##################################################################  
 // Enter Sleep mode Setting  
 0x0028,0x00B8, //GON=°ѓ1°ѓ DTE=°ѓ1°ѓ D[1:0]=°ѓ10°ѓ  
 DELAY_CODE,40,  //delay(40)
 0x001F,0x0089, // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=1  
 DELAY_CODE,40,  //delay(40)
 0x0028,0x0004, //GON=°ѓ0°ѓ DTE=°ѓ0°ѓ D[1:0]=°ѓ01°ѓ  
 DELAY_CODE,40,  //delay(40)
 0x0019,0x0000, //OSC_EN=°ѓ0°ѓ  
 DELAY_CODE,5,  //delay(5)
 // Exit Sleep mode Setting  
 0x0018,0x0036, //I/P_RADJ,N/P_RADJ, Normal mode 75Hz  
 0x0019,0x0001, //OSC_EN='1', start Osc  
 0x001F,0x0088,// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x0080,// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x0090,// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x00D0,// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x0028,0x0038, //GON=1, DTE=1, D=1000  
 DELAY_CODE,40,  //delay(40)
 0x0028,0x003F, //GON=1, DTE=1, D=1100  
 
 // Enter Deep Sleep mode Setting  
 0x0028,0x00B8, //GON=°ѓ1°ѓ DTE=°ѓ1°ѓ D[1:0]=°ѓ10°ѓ  
 DELAY_CODE,40,  //delay(40)
 0x001F,0x0089, // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=1  
 DELAY_CODE,40,  //delay(40)
 0x0028,0x0004, //GON=°ѓ0°ѓ DTE=°ѓ0°ѓ D[1:0]=°ѓ01°ѓ  
 DELAY_CODE,40,  //delay(40)
 0x0019,0x0000, //OSC_EN=°ѓ0°ѓ  
 DELAY_CODE,5,  //delay(5)
 0x0001,0x00C0, //DP_STB[1:0]=°ѓ11°ѓ  
	
 // Exit Deep Sleep mode Setting  
 0x0001,0x0000, //DP_STB='0', out deep sleep  
 DELAY_CODE,10,  //delay(10)
 0x0019,0x0001, //OSC_EN='1', start Osc  
 0x001F,0x0088,// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x0080,// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x0090,// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x001F,0x00D0,// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0  
 DELAY_CODE,5,  //delay(5)
 0x0028,0x0038, //GON=1, DTE=1, D=1000  
 DELAY_CODE,40,  //delay(40)
 0x0028,0x003F //GON=1, DTE=1, D=1100  
};






//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//общие функции
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//инициализаци€ LCD-экрана
//----------------------------------------------------------------------------------------------------
void HX8347D_Init(void)
{
 //настраиваем порты

 __HAL_RCC_GPIOD_CLK_ENABLE();
 __HAL_RCC_GPIOE_CLK_ENABLE();
	
 //GPIO_Pin Ц номера выводов, которые конфигурируютс€. ѕример дл€ нескольких выводов:
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
 //GPIO_Speed Ц задает скорость дл€ выбранных выводов.
 //GPIO_Mode Ц задает режим работы выводов. 

 //настраиваем параметры портов
 GPIO_InitTypeDef HX8347D_GPIO_Init;

 HX8347D_GPIO_Init.Mode=GPIO_MODE_OUTPUT_PP;
 HX8347D_GPIO_Init.Pull=GPIO_NOPULL;
 HX8347D_GPIO_Init.Speed=GPIO_SPEED_FREQ_VERY_HIGH;

 

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_CS;
 HAL_GPIO_Init(HX8347D_GPIO_CS,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_RS;
 HAL_GPIO_Init(HX8347D_GPIO_RS,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_RST;
 HAL_GPIO_Init(HX8347D_GPIO_RST,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_RD;
 HAL_GPIO_Init(HX8347D_GPIO_RD,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_WR;
 HAL_GPIO_Init(HX8347D_GPIO_WR,&HX8347D_GPIO_Init);
 

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D0;
 HAL_GPIO_Init(HX8347D_GPIO_D0,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D1;
 HAL_GPIO_Init(HX8347D_GPIO_D1,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D2;
 HAL_GPIO_Init(HX8347D_GPIO_D2,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D3;
 HAL_GPIO_Init(HX8347D_GPIO_D3,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D4;
 HAL_GPIO_Init(HX8347D_GPIO_D4,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D5;
 HAL_GPIO_Init(HX8347D_GPIO_D5,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D6;
 HAL_GPIO_Init(HX8347D_GPIO_D6,&HX8347D_GPIO_Init);

 HX8347D_GPIO_Init.Pin=HX8347D_GPIO_PIN_D7;
 HAL_GPIO_Init(HX8347D_GPIO_D7,&HX8347D_GPIO_Init);

 //инициализируем дисплей
 HX8347D_Reset();
 HX8347D_CS_Zero();

 long command_amount=sizeof(HX8347D_InitList)/sizeof(unsigned short);
 for(long n=0;n<command_amount;)
 {
  unsigned short reg=HX8347D_InitList[n];
  n++;
  unsigned short value=HX8347D_InitList[n];
  n++;
  if (reg==DELAY_CODE)//пауза
  {
   HAL_Delay(value);  
   continue;
  }
  //значение дл€ регистра
  HX8347D_WriteRegister16(reg,value);  
 }
}
//----------------------------------------------------------------------------------------------------
//выставить на CS 1
//----------------------------------------------------------------------------------------------------
inline void HX8347D_CS_One(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_CS,HX8347D_GPIO_PIN_CS,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на CS 0
//----------------------------------------------------------------------------------------------------
inline void HX8347D_CS_Zero(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_CS,HX8347D_GPIO_PIN_CS,GPIO_PIN_RESET);
}
//----------------------------------------------------------------------------------------------------
//выставить на RS 1
//----------------------------------------------------------------------------------------------------
inline void HX8347D_RS_One(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_RS,HX8347D_GPIO_PIN_RS,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на RS 0
//----------------------------------------------------------------------------------------------------
inline void HX8347D_RS_Zero(void)
{
  HAL_GPIO_WritePin(HX8347D_GPIO_RS,HX8347D_GPIO_PIN_RS,GPIO_PIN_RESET);
}
//----------------------------------------------------------------------------------------------------
//выставить на WR 1
//----------------------------------------------------------------------------------------------------
inline void HX8347D_WR_One(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_WR,HX8347D_GPIO_PIN_WR,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на WR 0
//----------------------------------------------------------------------------------------------------
inline void HX8347D_WR_Zero(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_WR,HX8347D_GPIO_PIN_WR,GPIO_PIN_RESET);
}
//----------------------------------------------------------------------------------------------------
//выставить на RST 1
//----------------------------------------------------------------------------------------------------
inline void HX8347D_RST_One(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_RST,HX8347D_GPIO_PIN_RST,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на RST 0
//----------------------------------------------------------------------------------------------------
inline void HX8347D_RST_Zero(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_RST,HX8347D_GPIO_PIN_RST,GPIO_PIN_RESET);
}
//----------------------------------------------------------------------------------------------------
//выставить на RD 1
//----------------------------------------------------------------------------------------------------
inline void HX8347D_RD_One(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_RD,HX8347D_GPIO_PIN_RD,GPIO_PIN_SET);
}
//----------------------------------------------------------------------------------------------------
//выставить на RD 0
//----------------------------------------------------------------------------------------------------
inline void HX8347D_RD_Zero(void)
{
 HAL_GPIO_WritePin(HX8347D_GPIO_RD,HX8347D_GPIO_PIN_RD,GPIO_PIN_RESET);
}
//----------------------------------------------------------------------------------------------------
//задать значение на шине данных
//----------------------------------------------------------------------------------------------------
inline void HX8347D_SetData(unsigned char byte)
{
 //unsigned long inv_byte=0xFF^byte;
 //unsigned long pr_byte=byte;
 //HX8347D_GPIO_D0->BSRR=(pr_byte<<4UL)|(inv_byte<<(4UL+16UL));
	
 /*	
 unsigned long port_d=0;
 if (byte&1) port_d|=HX8347D_GPIO_PIN_D0;
	      else port_d|=HX8347D_GPIO_PIN_D0<<16UL;
 if (byte&2) port_d|=HX8347D_GPIO_PIN_D1;
	      else port_d|=HX8347D_GPIO_PIN_D1<<16UL;
 if (byte&4) port_d|=HX8347D_GPIO_PIN_D2;
	      else port_d|=HX8347D_GPIO_PIN_D2<<16UL;
 if (byte&8) port_d|=HX8347D_GPIO_PIN_D3;
	      else port_d|=HX8347D_GPIO_PIN_D3<<16UL;
	
 unsigned long port_e=0;
 if (byte&16) port_e|=HX8347D_GPIO_PIN_D4;
	      else port_e|=HX8347D_GPIO_PIN_D4<<16UL;
 if (byte&32) port_e|=HX8347D_GPIO_PIN_D5;
	      else port_e|=HX8347D_GPIO_PIN_D5<<16UL;
 if (byte&64) port_e|=HX8347D_GPIO_PIN_D6;
	      else port_e|=HX8347D_GPIO_PIN_D6<<16UL;
 if (byte&128) port_e|=HX8347D_GPIO_PIN_D7;
	      else port_e|=HX8347D_GPIO_PIN_D7<<16UL;

 HX8347D_GPIO_D0->BSRR=port_d;
 HX8347D_GPIO_D4->BSRR=port_e;
 */

 if (byte&1) HAL_GPIO_WritePin(HX8347D_GPIO_D0,HX8347D_GPIO_PIN_D0,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D0,HX8347D_GPIO_PIN_D0,GPIO_PIN_RESET);
 if (byte&2) HAL_GPIO_WritePin(HX8347D_GPIO_D1,HX8347D_GPIO_PIN_D1,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D1,HX8347D_GPIO_PIN_D1,GPIO_PIN_RESET);
 if (byte&4) HAL_GPIO_WritePin(HX8347D_GPIO_D2,HX8347D_GPIO_PIN_D2,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D2,HX8347D_GPIO_PIN_D2,GPIO_PIN_RESET);
 if (byte&8) HAL_GPIO_WritePin(HX8347D_GPIO_D3,HX8347D_GPIO_PIN_D3,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D3,HX8347D_GPIO_PIN_D3,GPIO_PIN_RESET);
 if (byte&16) HAL_GPIO_WritePin(HX8347D_GPIO_D4,HX8347D_GPIO_PIN_D4,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D4,HX8347D_GPIO_PIN_D4,GPIO_PIN_RESET);
 if (byte&32) HAL_GPIO_WritePin(HX8347D_GPIO_D5,HX8347D_GPIO_PIN_D5,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D5,HX8347D_GPIO_PIN_D5,GPIO_PIN_RESET);
 if (byte&64) HAL_GPIO_WritePin(HX8347D_GPIO_D6,HX8347D_GPIO_PIN_D6,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D6,HX8347D_GPIO_PIN_D6,GPIO_PIN_RESET);
 if (byte&128) HAL_GPIO_WritePin(HX8347D_GPIO_D7,HX8347D_GPIO_PIN_D7,GPIO_PIN_SET);
        else HAL_GPIO_WritePin(HX8347D_GPIO_D7,HX8347D_GPIO_PIN_D7,GPIO_PIN_RESET);
}

//----------------------------------------------------------------------------------------------------
//выполнить сброс
//----------------------------------------------------------------------------------------------------
void HX8347D_Reset(void)
{
 HX8347D_CS_One();
 HX8347D_RS_One();
 HX8347D_RD_One();
 HX8347D_WR_One();
 HX8347D_RST_One();    
 HAL_Delay(10);
 HX8347D_RST_Zero();
 HAL_Delay(10);
 HX8347D_RST_One();
 HAL_Delay(250);
}
//----------------------------------------------------------------------------------------------------
//передать байт
//----------------------------------------------------------------------------------------------------
inline void HX8347D_Write8(unsigned char byte)
{ 
 HX8347D_SetData(byte);
 HX8347D_WR_Zero();
 HX8347D_WR_One();
}
//----------------------------------------------------------------------------------------------------
//передать 16 бит 
//----------------------------------------------------------------------------------------------------
inline void HX8347D_Write16(unsigned short value) 
{ 
 HX8347D_Write8((value>>8)&0xFF);
 HX8347D_Write8(value&0xFF);
}
//----------------------------------------------------------------------------------------------------
//записать в регистр 16 бит
//----------------------------------------------------------------------------------------------------
inline void HX8347D_WriteRegister16(unsigned short reg,unsigned short value) 
{ 
 HX8347D_RS_Zero();
 HX8347D_Write16(reg);
 HX8347D_RS_One();
 HX8347D_Write16(value); 
}
//----------------------------------------------------------------------------------------------------
//задать окно
//----------------------------------------------------------------------------------------------------
inline void HX8347D_SetWindow(unsigned short x_left,unsigned short y_top,unsigned short x_right,unsigned short y_bottom)
{
 HX8347D_WriteRegister16(0x002,(x_left>>8)&0xFF);
 HX8347D_WriteRegister16(0x003,(x_left)&0xFF);

 HX8347D_WriteRegister16(0x004,(x_right>>8)&0xFF);
 HX8347D_WriteRegister16(0x005,(x_right)&0xFF);

 HX8347D_WriteRegister16(0x006,(y_top>>8)&0xFF);
 HX8347D_WriteRegister16(0x007,(y_top)&0xFF);

 HX8347D_WriteRegister16(0x008,(y_bottom>>8)&0xFF);
 HX8347D_WriteRegister16(0x009,(y_bottom)&0xFF);
 //выбираем регистр вывода изображени€
 HX8347D_RS_Zero();
 HX8347D_Write16(0x22); 
 //передаЄм данные картинки
 HX8347D_RS_One();
}
//------------------------------------------------------------------------------
//передать цвет точки
//------------------------------------------------------------------------------
void HX8347D_OutColor(unsigned short color)
{
 HX8347D_Write16(color);
}

//----------------------------------------------------------------------------------------------------
//вывод символа в позицию
//----------------------------------------------------------------------------------------------------
void HX8347D_PutSymbol(long x,long y,char symbol,unsigned short color)
{
}
//----------------------------------------------------------------------------------------------------
//вывод строчки в позицию
//----------------------------------------------------------------------------------------------------
void HX8347D_PutString(long x,long y,const char *string,unsigned short color)
{
}
//----------------------------------------------------------------------------------------------------
//вывести текст в текущую позицию
//----------------------------------------------------------------------------------------------------
void HX8347D_Print(const char *string,unsigned short color)
{
}
//----------------------------------------------------------------------------------------------------
//сбросить текущую позицию
//----------------------------------------------------------------------------------------------------
void HX8347D_Clear(void)
{
 HX8347D_SetWindow(0,0,HX8347D_WIDTH-1,HX8347D_HEIGHT-1);
 //выбираем регистр вывода изображени€
 HX8347D_RS_Zero();
 HX8347D_Write16(0x22); 
 //передаЄм данные картинки
 HX8347D_RS_One();
 for(long y=0;y<HX8347D_HEIGHT;y++)
 {
  for(long x=0;x<HX8347D_WIDTH;x++)
  {
   HX8347D_Write16(HX8347D_BLACK);
  }
 }
}












