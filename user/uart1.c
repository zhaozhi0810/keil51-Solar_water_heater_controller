/**********************************************/
/*			平台：Keil U4 + STC12C5A60S       */
/*			名称：串口通信					  */

/**********************************************/

#include <STC12C5A60S2.H>
#include "1302.h"
#include "uart1.h"

#define F_f 11059200UL   //晶振频率11.0692MHZ
#define Baud_rate 9600UL //波特率9600

//sfr AUXR = 0x8E; //定义特殊功能寄存器AUXR
//sfr BRT = 0x9C;	 //定义独立波特率发生寄存器BRT



bit busy;
static unsigned char uart1_recv_buf[4];   //3个字节的数据，1个字节校验和
static unsigned char uart1_recv_n = 0;
static unsigned char flag = 0;    //0bit time_recv 1bit date_recv
static unsigned char ret_flag = 0;    //用于串口收到数据后应答，1表示正常，2-255表示异常，0表示没有收到数据，不回复
#define TIME_HEAD 0xaa   //时间头部
#define DATE_HEAD 0xcc   //日期头部

void Send_char(char str)
{
	SBUF = str;
	while( !TI ); //等待发送完成
	TI = 0; 	  //清除发送中断请求
}

void Send_string(char *str)
{
	while( *str )
	{
		Send_char(*str++);
	}
}

void Uart1Init()
{
	PCON &= 0x7F; // 波特率不加倍
	SCON = 0x50;  // 设置串行通信工作方式1，并允许串行接受
	AUXR &= 0xFB; // 独立波特率发生器时钟12T
	BRT = 256 - F_f / (12 * Baud_rate * 32); //独立波特率发生器装载初值
	AUXR |= 0x11; // 允许独立波特率发生器作为串口波特率发生器，并开启
	EA = 1;	      // 开启总中断
	ES = 1;		  // 开启串行中断
}


unsigned char check_sum(unsigned char * dat,unsigned char len)
{
	unsigned char i;
	unsigned char sum = 0;
	
	for(i=0;i<len;i++)
	{
		sum += dat[i];
	}	
	
	return sum;
}




/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4
{
	unsigned char recv = 0;
    
	if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
		recv = SBUF;        //接收数据
		
		switch(flag)
		{
			case 0: //没有收到任何头部
				if(TIME_HEAD == recv)
				{
					flag = 1;  //收到时间设置头部					
				}
				else if (DATE_HEAD == recv)
				{
					flag = 2;  //收到日期设置头部
					//uart1_recv_n = 0;
				}
				uart1_recv_n = 0;
			break;
			case 1:
			case 2:
				uart1_recv_buf[uart1_recv_n++] = recv;
				if(uart1_recv_n > 3)
				{
					uart1_recv_n = 0;
					ret_flag = 1;   //收到一帧数据
				}
			
				//uart1_recv_buf[uart1_recv_n++] = recv;
				break;
			default:
				break;
		}
        //P0 = SBUF;          //P0 show UART data
        //bit9 = RB8;         //P2.2 show parity bit
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        busy = 0;           //Clear transmit busy flag
    }
}

/*----------------------------
Send a byte data to UART
Input: dat (data to be sent)
Output:None
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);           //Wait for the completion of the previous data is sent
    ACC = dat;              //Calculate the even parity bit P (PSW.0)

    busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void SendString(char *s)
{
    while (*s)              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr
    }
}






void uart1_data_handle(void)
{
	if(ret_flag == 1)
	{
		if (check_sum(uart1_recv_buf,3) == uart1_recv_buf[3])
		{
			//设置时间或日期
			if(flag == 1)
			{
				Ds1302SetTime(uart1_recv_buf[0],uart1_recv_buf[1],uart1_recv_buf[2]);				
			}
			else if(flag == 2)
			{				
				Ds1302SetDate(uart1_recv_buf[0],uart1_recv_buf[1],uart1_recv_buf[2]);
				//flag = 0;
			}			
			SendString("ok");	//串口应答		
		}
		else
			SendString("err");  //串口应答	

		flag = 0;
		ret_flag = 0; //已经处理
	}
}
