#ifndef SD_H
#define SD_H

//----------------------------------------------------------------------------------------------------
//������������ ����������
//----------------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

//----------------------------------------------------------------------------------------------------
//������������
//----------------------------------------------------------------------------------------------------
//���� �������
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
//��������� �������
//----------------------------------------------------------------------------------------------------
SD_ANSWER SD_Init(void);//������������� ����� ������

void SD_CS_One(void);//��������� �� CS 1
void SD_CS_Zero(void);//��������� �� CS 0

void SD_DI_One(void);//��������� �� DI 1
void SD_DI_Zero(void);//��������� �� DI 0

void SD_SCK_One(void);//��������� �� SCK 1
void SD_SCK_Zero(void);//��������� �� SCK 0

bool SD_SendCommand(uint8_t cmd,uint8_t b0,uint8_t b1,uint8_t b2,uint8_t b3,uint8_t answer_size,uint8_t *answer);//������� ������� � �������� ����� �� SD-�����
bool SD_GetSize(uint32_t *size);//�������� ����� SD-����� � ������
bool SD_BeginReadBlock(uint32_t BlockAddr);//������ ������ �����
bool SD_ReadBlockByte(uint8_t *byte);//������� ���� �����
bool SD_ReadBlock(uint32_t BlockAddr,uint8_t *Addr);//������� ���� � 512 ���� � ������
bool SD_WriteBlock(uint32_t BlockAddr,const uint8_t *Addr);//�������� ���� � 512 ���� �� ������

#endif



