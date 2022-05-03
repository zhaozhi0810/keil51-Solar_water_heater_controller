#ifndef _NTC_H
#define _NTC_H


typedef  signed    char    int8;    // 8位有符号整型数
typedef  signed    int     int16;   //16位有符号整型数
typedef  signed    long    int32;   //32位有符号整型数
typedef  unsigned  char    uint8;   // 8位无符号整型数
typedef  unsigned  int     uint16;  //16位无符号整型数
typedef  unsigned  long    uint32;  //32位无符号整型数

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

//#include "config.h"



//unsigned int NTC_ReadT(void);

char T_dis(unsigned int t_val);
#endif