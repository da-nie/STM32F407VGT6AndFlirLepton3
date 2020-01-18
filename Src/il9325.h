#ifndef IL9325_H
#define IL9325_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>


//цвета
#define IL9325_BLACK     0x0000
#define IL9325_RED       0xF800
#define IL9325_ORANGE    0xFBE0
#define IL9325_YELLOW    0xFFE0
#define IL9325_GREEN     0x07E0
#define IL9325_CYAN      0x07FF
#define IL9325_BLUE      0x001F
#define IL9325_MAGENTA   0xF81F
#define IL9325_VIOLET    0x881F
#define IL9325_WHITE     0xFFFF


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

#define IL9325_WIDTH 240UL
#define IL9325_HEIGHT 320UL


//----------------------------------------------------------------------------------------------------
//прототипы функций
//----------------------------------------------------------------------------------------------------
void IL9325_InitGPIO(void);//инициализация портов LCD-экрана
void IL9325_Init(void);//инициализация LCD-экрана

void IL9325_CS_One(void);//выставить на CS 1
void IL9325_CS_Zero(void);//выставить на CS 0

void IL9325_RS_One(void);//выставить на RS 1
void IL9325_RS_Zero(void);//выставить на RS 0

void IL9325_WR_One(void);//выставить на WR 1
void IL9325_WR_Zero(void);//выставить на WR 0

void IL9325_RST_One(void);//выставить на RST 1
void IL9325_RST_Zero(void);//выставить на RST 0

void IL9325_RD_One(void);//выставить на RD 1
void IL9325_RD_Zero(void);//выставить на RD 0

void IL9325_SetData(uint8_t byte);//задать значение на шине данных

void IL9325_Write8(uint8_t byte);//передать байт
void IL9325_Write16(uint16_t value) ;//передать 16 бит 
void IL9325_SelectRegister(uint16_t reg);//выбрать регистр
void IL9325_WriteRegister16(uint16_t reg,uint16_t value) ;//записать в регистр 16 бит
void IL9325_WriteRegister8(uint8_t reg,uint8_t value);//записать в регистр 8 бит
void IL9325_Reset(void);//выполнить сброс
void IL9325_SetWindow(uint16_t x_left,uint16_t y_top,uint16_t x_right,uint16_t y_bottom);//задать окно

void IL9325_OutColor(uint16_t color);//передать цвет точки
void IL9325_PutPixel(int32_t x,int32_t y,uint16_t color);//вывести один пиксель
void IL9325_PutSymbol(int32_t x,int32_t y,char symbol,uint16_t color);//вывод символа в позицию
void IL9325_PutString(int32_t x,int32_t y,const char *string,uint16_t color);//вывод строчки в позицию
void IL9325_Print(const char *string,uint16_t color);//вывести текст в текущую позицию
void IL9325_Clear(uint16_t color);//сбросить текущую позицию


#endif  
