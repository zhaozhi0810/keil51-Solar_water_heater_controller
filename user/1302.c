#include "1302.h"
#include "stdio.h"


/*
DS1302写入和读取时分秒的地址命令
秒分时日月周年 最低位读写位;
*/
code const uchar code READ_RTC_ADDR[7]=
        {0x81,0x83,0x85,0x87,0x89,0x8B,0x8D};
code const uchar code WRITE_RTC_ADDR[7]=
        {0x80,0x82,0x84,0x86,0x88,0x8A,0x8C};
/*
DS1302时钟初始化2022年2月27日星期天22点10分20秒。
存储顺序是秒分时日月周年,存储格式是用BCD码
*/
uchar TimeData[7]={20,10,0x22,0x027,0x02,0x07,0x22};

/*
DS1302是通过SPI串行总线跟单片机通信的，当进行一次读写操作时最少得读写两个字节，第一个字节是控制字节，就是一个命令，告诉DS1302是读还是写操作，是对RAM还是对CLOK寄存器操作。第二个字节就是要读或写的数据了。

单字节读写：
    只有在SCLK为低电平时，才能将CE置为高电平。
    所以在进行操作之前先将SCLK置低电平，然后将CE置为高电平，接着开始在IO上面放入要传送的电平信号，然后跳变SCLK。
    数据在SCLK上升沿时，DS1302读写数据，在SCLK下降沿时，DS1302放置数据到IO上

*/

sbit DS1302_RST = P2^0;      //实时时钟复位线引脚
sbit DS1302_IO = P2^1;      //实时时钟数据线引脚
sbit DS1302_CLK = P2^2;      //实时时钟时钟线引脚 


void ds1302_io_set_input(void)
{
	P2M1 |= 0x2;  //P2.1 设置为输入
	P2M0 &= ~0x2;   
}

void ds1302_io_set_output(void)
{
	P2M0 |= 0x2;  //P2.1 设置为推挽输出
	P2M1 &= ~0x2;  
}

//****************************************************
//实时时钟写入一字节(内部函数)
//****************************************************
static void DS1302_InputByte(unsigned char dat) 	
{ 
    unsigned char i;
	ds1302_io_set_input();
    for(i=0; i<8; i++)
    {
        DS1302_IO = dat & 0x01;   
		
		DS1302_CLK = 0;				//上升沿写入数据
		_nop_();
		_nop_();        
        
        DS1302_CLK = 1;
		_nop_();
		_nop_();
        dat >>= 1; 
    } 
}

//****************************************************
//实时时钟读取一字节(内部函数)
//****************************************************
static unsigned char DS1302_OutputByte(void) 	
{ 
    unsigned char i;
	unsigned char dat;
	ds1302_io_set_output();
    for(i=0; i<8; i++)
    {
		DS1302_CLK = 1;				    //下降沿读出数据
		_nop_();
		_nop_();
        DS1302_CLK = 0;
		_nop_();
		_nop_();
        dat >>= 1;         			
        if( DS1302_IO == 1 )
			dat |= 0x80;				//最高位置一
		else
			dat &= 0x7F;				//最高位清零       
    } 
    return(dat); 
}

//****************************************************
//ucAddr: DS1302地址, ucData: 要写的数据
//****************************************************
static void Ds1302Write(unsigned char ucAddr, unsigned char ucDa)	
{
    DS1302_RST = 0;
	_nop_();
    DS1302_CLK = 0;
	_nop_();
    DS1302_RST = 1;
	_nop_();
    DS1302_InputByte(ucAddr);       	// 地址，命令 
    DS1302_InputByte(ucDa);       		// 写1Byte数据
    DS1302_CLK = 1;
	_nop_();
    DS1302_RST = 0;
	_nop_();
}

//****************************************************
//读取DS1302某地址的数据
//****************************************************
static unsigned char Ds1302Read(unsigned char ucAddr)	
{
    unsigned char ucData;
    DS1302_RST = 0;
	_nop_();
    DS1302_CLK = 0;
	_nop_();
    DS1302_RST = 1;
	_nop_();
    DS1302_InputByte(ucAddr|0x01);        // 地址，命令 
    ucData = DS1302_OutputByte();         // 读1Byte数据
    DS1302_CLK = 1;
	_nop_();
    DS1302_RST = 0;
	_nop_();
    return(ucData);
}




void Ds1302SetTime(unsigned char hour,unsigned char min,unsigned char sec)
{   
	Ds1302Write(0x8e,0x00);//禁止写保护     
    Ds1302Write(WRITE_RTC_ADDR[0],sec);
    Ds1302Write(WRITE_RTC_ADDR[1],min);
	Ds1302Write(WRITE_RTC_ADDR[2],hour);
    Ds1302Write(0x8e,0x10);//打开写保护
}


void Ds1302SetDate(unsigned char year,unsigned char month,unsigned char day)
{
    Ds1302Write(0x8e,0x00);//禁止写保护
    Ds1302Write(WRITE_RTC_ADDR[3],day);
    Ds1302Write(WRITE_RTC_ADDR[4],month);
	Ds1302Write(WRITE_RTC_ADDR[6],year);
    Ds1302Write(0x8e,0x10);//打开写保护
}


void Ds1302readTime()//读取时钟信息
{
	int a=12,b=23,c=6;
	TimeData[0]=Ds1302Read(READ_RTC_ADDR[0]);
	TimeData[1]=Ds1302Read(READ_RTC_ADDR[1]);
	TimeData[2]=Ds1302Read(READ_RTC_ADDR[2]);
	
	a = TimeData[0];
	b = TimeData[1];
	c = TimeData[2];
	printf("time:%d:%d:%d\r\n",c,b,a);
}

//void Ds1302readDate()//读取时钟信息
//{
//	TimeData[3]=Ds1302Read(READ_RTC_ADDR[3]);
//	TimeData[4]=Ds1302Read(READ_RTC_ADDR[4]);
//	TimeData[6]=Ds1302Read(READ_RTC_ADDR[6]);
//}



