#ifndef __ADC_H
#define __ADC_H		

//#include "config.h"
//#include "delay.h"


//ADC相关设定参数
#define ADC_POWER   0x80            //ADC电源控制位
#define ADC_FLAG    0x10            //ADC完成标志
#define ADC_START   0x08            //ADC起始控制位
#define ADC_SPEEDLL 0x00            //540个时钟
#define ADC_SPEEDL  0x20            //360个时钟
#define ADC_SPEEDH  0x40            //180个时钟
#define ADC_SPEEDHH 0x60            //90个时钟

//-----------------------------------------
//BandGap相关参数
//注意:需要在下载代码时选择"在ID号前添加重要测试参数"选项,才可在程序中获取此参数
//#define ID_ADDR_ROM 0x03f7      //1K程序空间的MCU
//#define ID_ADDR_ROM 0x07f7      //2K程序空间的MCU
//#define ID_ADDR_ROM 0x0bf7      //3K程序空间的MCU
//#define ID_ADDR_ROM 0x0ff7      //4K程序空间的MCU
//#define ID_ADDR_ROM 0x13f7      //5K程序空间的MCU
#define ID_ADDR_ROM 0x1ff7      //8K程序空间的MCU
//#define ID_ADDR_ROM 0x27f7      //10K程序空间的MCU
//#define ID_ADDR_ROM 0x2ff7      //12K程序空间的MCU
//#define ID_ADDR_ROM 0x3ff7      //16K程序空间的MCU
//#define ID_ADDR_ROM 0x4ff7      //20K程序空间的MCU
//#define ID_ADDR_ROM 0x5ff7      //24K程序空间的MCU
//#define ID_ADDR_ROM 0x6ff7      //28K程序空间的MCU
//#define ID_ADDR_ROM 0x7ff7      //32K程序空间的MCU
//#define ID_ADDR_ROM 0x9ff7      //40K程序空间的MCU
//#define ID_ADDR_ROM 0xbff7      //48K程序空间的MCU
//#define ID_ADDR_ROM 0xcff7      //52K程序空间的MCU
//#define ID_ADDR_ROM 0xdff7      //56K程序空间的MCU
//#define ID_ADDR_ROM 0xeff7      //60K程序空间的MCU

extern unsigned int  adc_ch9_val;	//通过通道9读取的内部BandGap电压相对MCU系统供电电压VDD的ADC读数
extern unsigned int  adc_chx_val;	//指定ADC端口的外部输入电压相对MCU系统供电电压VDD的ADC读数
extern unsigned int  bdg_voltage;	//从flash程序区中读取MCU的BandGap电压，单位毫伏
extern float vdd_voltage;	// MCU系统供电电压，单位毫伏
extern float adc_voltage;	// 指定ADC端口电压，单位毫伏

void ADC_Init(void);
unsigned int  ADC_ReadCh9Val(void);
unsigned int  ADC_ReadBandGapVoltage(void);
float ADC_ReadVddVoltage(void);
float ADC_ReadChxVoltage(unsigned char ch);

#endif
