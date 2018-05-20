#ifndef HX8347D_H
#define HX8347D_H

#include "stm32f4xx_hal.h"

#ifndef bool 
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

//�����
#define HX8347D_BLACK     0x0000
#define HX8347D_RED       0xF800
#define HX8347D_ORANGE    0xFBE0
#define HX8347D_YELLOW    0xFFE0
#define HX8347D_GREEN     0x07E0
#define HX8347D_CYAN      0x07FF
#define HX8347D_BLUE      0x001F
#define HX8347D_MAGENTA   0xF81F
#define HX8347D_VIOLET    0x881F
#define HX8347D_WHITE     0xFFFF


#define HX8347D_WIDTH 240UL
#define HX8347D_HEIGHT 320UL


//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void HX8347D_Init(void);//������������� LCD-������

void HX8347D_CS_One(void);//��������� �� CS 1
void HX8347D_CS_Zero(void);//��������� �� CS 0

void HX8347D_RS_One(void);//��������� �� RS 1
void HX8347D_RS_Zero(void);//��������� �� RS 0

void HX8347D_WR_One(void);//��������� �� WR 1
void HX8347D_WR_Zero(void);//��������� �� WR 0

void HX8347D_RST_One(void);//��������� �� RST 1
void HX8347D_RST_Zero(void);//��������� �� RST 0

void HX8347D_RD_One(void);//��������� �� RD 1
void HX8347D_RD_Zero(void);//��������� �� RD 0

void HX8347D_SetData(unsigned char byte);//������ �������� �� ���� ������

void HX8347D_Write8(unsigned char byte);//�������� ����
void HX8347D_Write16(unsigned short value) ;//�������� 16 ��� 
void HX8347D_WriteRegister16(unsigned short reg,unsigned short value) ;//�������� � ������� 16 ���
void HX8347D_Reset(void);//��������� �����
void HX8347D_SetWindow(unsigned short x_left,unsigned short y_top,unsigned short x_right,unsigned short y_bottom);//������ ����

void HX8347D_OutColor(unsigned short color);//�������� ���� �����
void HX8347D_PutSymbol(long x,long y,char symbol,unsigned short color);//����� ������� � �������
void HX8347D_PutString(long x,long y,const char *string,unsigned short color);//����� ������� � �������
void HX8347D_Print(const char *string,unsigned short color);//������� ����� � ������� �������
void HX8347D_Clear(void);//�������� ������� �������


#endif  
