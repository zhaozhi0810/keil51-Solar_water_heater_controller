#include <STC12C5A60S2.H>

#include "gpio.h"


typedef unsigned char BYTE;
typedef unsigned int WORD;

//-----------------------------------------------

/* define constants */
#define FOSC 11059200L
#define MODE1T                      //Timer clock mode, comment this line is 12T mode, uncomment is 1T mode

#ifdef MODE1T
#define T1MS (65536-FOSC/2210)      //1ms timer calculation method in 1T mode
#else
#define T1MS (65536-FOSC/12/1000)   //1ms timer calculation method in 12T mode
#endif

/* define SFR */
//sfr AUXR = 0x8e;                    //Auxiliary register
//sbit TEST_LED = P0^0;               //work LED, flash once per second

/* define variables */
unsigned int timer0_count = 0;                         //1000 times counter

extern unsigned char ceshi_flag ;  //0 表示测水位，1表示测水温


void get_temp(void)
{
	P15 = 0;   //低
	P16 = 1;   //高
//	ADC_start();
}

void get_level(void)
{
	P15 = 1;
	P16 = 0;
//	ADC_start();
}

void out_lower(void)
{
	P15 = 0;
	P16 = 0;
}

//-----------------------------------------------

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1
{
    TL0 = T1MS;                     //reload timer0 low byte
    TH0 = T1MS >> 8;                //reload timer0 high byte
 
	timer0_count ++;    //1ms
	
	if(timer0_count > 2000)   //2s 超过6万，60s
		timer0_count = 0;
	
	//只有测试水位时，使用脉冲信号，500hz
	if(!ceshi_flag) //水位，
	{	
		if(timer0_count%2 ) 
			get_level();
		else
			out_lower();
	}
	else				
		get_temp();
	
	
	//按键扫描
	if(timer0_count % 22 == 11)   //最好是10ms扫描一次
		keys_scan();
}

//-----------------------------------------------

/* main program */
void timer0_init(void)
{
#ifdef MODE1T
    AUXR |= 0x80;                    //timer0 work in 1T mode
#endif
    TMOD = 0x01;                    //set timer0 as mode1 (16-bit)
    TL0 = T1MS;                     //initial timer0 low byte
    TH0 = T1MS >> 8;                //initial timer0 high byte
    TR0 = 1;                        //timer0 start running
    ET0 = 1;                        //enable timer0 interrupt
    EA = 1;                         //open global interrupt switch
    timer0_count = 0;                      //initial counter

}
