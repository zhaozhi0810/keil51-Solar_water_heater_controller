#ifndef __UART1_H__
#define __UART1_H__


#include <STC12C5A60S2.H>

typedef unsigned char BYTE;
typedef unsigned int WORD;

void Uart1Init();

void uart1_data_handle(void);

void Send_string(char *str);

void SendData(BYTE dat);
#endif
