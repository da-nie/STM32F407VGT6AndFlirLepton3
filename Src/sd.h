#ifndef SD_H
#define SD_H

//----------------------------------------------------------------------------------------------------
//подключаемые библиотеки
//----------------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

//----------------------------------------------------------------------------------------------------
//перечисления
//----------------------------------------------------------------------------------------------------
//типы ответов
typedef enum 
{
 SD_ANSWER_OK=0,
 SD_ANSWER_ERROR,
 SD_ANSWER_SPI_ERROR,
 SD_ANSWER_SPI_NOT_SUPPORTED,
 SD_ANSWER_NO_RESPONSE,
 SD_ANSWER_SIZE_ERROR
} SD_ANSWER;


//----------------------------------------------------------------------------------------------------
//прототипы функций
//----------------------------------------------------------------------------------------------------
SD_ANSWER SD_Init(void);//инициализация карты памяти

void SD_CS_One(void);//выставить на CS 1
void SD_CS_Zero(void);//выставить на CS 0

void SD_DI_One(void);//выставить на DI 1
void SD_DI_Zero(void);//выставить на DI 0

void SD_SCK_One(void);//выставить на SCK 1
void SD_SCK_Zero(void);//выставить на SCK 0

bool SD_SendCommand(uint8_t cmd,uint8_t b0,uint8_t b1,uint8_t b2,uint8_t b3,uint8_t answer_size,uint8_t *answer);//послать команду и получить ответ от SD-карты
bool SD_GetSize(uint32_t *size);//получить объём SD-карты в байтах
bool SD_BeginReadBlock(uint32_t BlockAddr);//начать чтение блока
bool SD_ReadBlockByte(uint8_t *byte);//считать байт блока
bool SD_ReadBlock(uint32_t BlockAddr,uint8_t *Addr);//считать блок в 512 байт в память
bool SD_WriteBlock(uint32_t BlockAddr,const uint8_t *Addr);//записать блок в 512 байт из памяти

#endif



