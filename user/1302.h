
#ifndef DS1302_H
#define DS1302_H

#include <STC12C5A60S2.H>
#include "intrins.h"

#ifndef  uchar //同理于c文件中的typedef unsigned char  uchar；
#define uchar unsigned char
#endif
#ifndef  uint 
#define uint unsigned int 
#endif

extern uchar TimeData[7];

#if 0
sbit SCLK = P2^2;
sbit DSIO = P2^1;
sbit RST = P2^0;


void Ds1302Write(uchar addr,uchar dat);
uchar  Ds1302Read(uchar addr);
void Ds1302Init();
void Ds1302readTime();


void Ds1302SetTime(unsigned char hour,unsigned char min,unsigned char sec);
void Ds1302SetDate(unsigned char year,unsigned char month,unsigned char day);

#else
void Ds1302readTime();
void Set_DS1302_Time(unsigned char hour,unsigned char min,unsigned char sec);
//extern uchar TimeData[7];

#endif

#endif
