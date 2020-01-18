#ifndef LEPTON_CONTROL_H
#define LEPTON_CONTROL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//исходные размеры изображения (не перевёрнутое)
#define LEPTON_ORIGINAL_IMAGE_WIDTH 160
#define LEPTON_ORIGINAL_IMAGE_HEIGHT 120

//высота кадра VoSPI
#define VOSPI_FRAME_HEIGHT 60
//ширина кадра VoSPI
#define VOSPI_FRAME_WIDTH 80
//размер пакета VoSPI а байтах (164 для RAW14 и 244 для RGB)
#define VOSPI_PACKAGE_SIZE 164
//размер строки пакета VoSPI в байтах
#define VOSPI_PACKAGE_LINE_SIZE 160

#define VOSPI_SEGMENT_LINE_AMOUNT 60

void LEPTONCONTROL_Init(void);//инициализация
void LEPTONCONTROL_CalculateCRC(uint16_t *crc,uint8_t byte);//вычислить crc
bool LEPTONCONTROL_PushVoSPI(uint8_t data[VOSPI_PACKAGE_SIZE],bool *first_line);//подать данные одного пакета VoSPI на вход модуля
uint16_t *LEPTONCONTROL_GetRAW14Ptr(void);//получить указатель на данные собранного изображения

#endif
