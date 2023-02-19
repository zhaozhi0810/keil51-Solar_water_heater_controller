
/*
	GPIO包括以下内容：
	1.4个按键读取（设置，加热，保温，上水）
	2.上水开关电磁阀控制，高电平有效   P04
	3.加热继电器控制					P05
	4.增压继电器控制					P06
	5.蜂鸣器控制						P07

	使用P0接口，P00-P03为按键.
*/
#include <STC12C5A60S2.H>

#include "gpio.h"

typedef struct btn_info{
	unsigned char  which;
	unsigned char  value;	     //值，0表示松开，1表示按下
	unsigned char  reportEn;   //1，消抖检测到了，0没有检测到按键
	unsigned int  pressCnt;     //长按区分
}BTN_INFO;


static BTN_INFO btns;   //只有4个按键
//static char btn_state;

sbit ShangShui_En = P1^4;   //上水使能

#define ShangShuiBtn 1   //上水按钮引脚编号
#define SheZhiBtn 2		 //	设置按钮引脚编号
#define JiaReBtn 3		//加热按钮引脚编号
#define BaoWenBtn 4		//保温按钮引脚编号	


void gpio_init(void)
{
	P0M1 = 0xf, P0M0 = 0xf0;  // P0.0-P0.3端口设置为高阻输入模式，读取按键
							  // P0.4-P0.7 为推挽输出	
	
	P1M1 &= ~(1<<4), P1M0 |= (1<<4);   //P1.4 推挽输出
	
}




void shangshui_enable(char en)
{
	ShangShui_En = !!en;    //非0使能，0去使能
}




//由定时器10ms查询，
void keys_scan(void)
{
	char i;
	if((P0 & 0xf) != 0xf)  //关机之后继续扫描
	{
		for(i=0;i<4;i++)
		{
			if(P0 & (1<<i) == 0)   //被按下
			{	
				btns.pressCnt++;
				
				if(btns.pressCnt > 3000)   //识别到按键
				{
					btns.value = 2;      //长按
					btns.which = i+1;
					btns.pressCnt = 3000; //防止长按越界
				}
				else if(btns.pressCnt>3)   //识别到按键
				{
					btns.value = 1;      //短按
					btns.which = i+1;
				}
								
			}
			else  //松开
			{
				btns.pressCnt = 0;   //松开，计算清零
//				if(btns.value > 1) //检测到短按
//				{
//					//长按处理	
//					btns.reportEn = 2;	  //定时器中不继续处理了				
//				}
//				else if(btns.value == 1)
//				{
//					//短按处理
//					btns.reportEn = 1;   //定时器中不继续处理了
//				}					
			//	btn2.value = 0; //值被清零
			}				
		}	
	}
}

//由main函数循环扫描处理
void key_poll_handle(void)
{
	if(btns.value)
	{
		if(ShangShuiBtn == btns.which)  //上水被按下
		{ //暂时没有处理长按和短按。按一次上水，再按一次取消上水
			ShangShui_En = !ShangShui_En;   //取反
			btns.which = 0;   //清除这个值
		}
		
		
		btns.value = 0;
	}
}




