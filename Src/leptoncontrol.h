#ifndef LEPTON_CONTROL_H
#define LEPTON_CONTROL_H

#include <stdbool.h>
#include <stdio.h>


//�������� ������� ����������� (�� �����������)
#define LEPTON_ORIGINAL_IMAGE_WIDTH 160
#define LEPTON_ORIGINAL_IMAGE_HEIGHT 120

//������ ����� VoSPI
#define VOSPI_FRAME_HEIGHT 60
//������ ����� VoSPI
#define VOSPI_FRAME_WIDTH 80
//������ ������ VoSPI � ������ (164 ��� RAW14 � 244 ��� RGB)
#define VOSPI_PACKAGE_SIZE 164
//������ ������ ������ VoSPI � ������
#define VOSPI_PACKAGE_LINE_SIZE 160

#define VOSPI_SEGMENT_LINE_AMOUNT 60

void LEPTONCONTROL_Init(void);//�������������
void LEPTONCONTROL_CalculateCRC(unsigned short *crc,unsigned char byte);//��������� crc
bool LEPTONCONTROL_PushVoSPI(unsigned char data[VOSPI_PACKAGE_SIZE],bool *first_line);//������ ������ ������ ������ VoSPI �� ���� ������
unsigned short *LEPTONCONTROL_GetRAW14Ptr(void);//�������� ��������� �� ������ ���������� �����������

#endif
