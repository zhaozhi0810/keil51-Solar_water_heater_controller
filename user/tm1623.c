#include <STC12C5A60S2.H>
#include "intrins.h"
#include "1302.h"
#include "gpio.h"



//#define nop _nop_();_nop_();_nop_();_nop_();

sbit DIO = P1^0;    //橙色  3
sbit CLK = P1^1;    //蓝色  4
sbit STB = P1^2;    //紫色  5 


//{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,
const unsigned char BCD_LABLE_ALL[]={0x7e,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x70,0x7f,0x7b,0x00,0x1};



static unsigned char BCD_LABLE_ANODE[16] = {0};

static unsigned char cmd_dis_mode = 0x89;







static void send_8bit(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		CLK = 0;
		if(dat&1)
			DIO = 1;
		else
			DIO = 0;
		_nop_();_nop_();_nop_();
		CLK = 1;
		dat >>= 1;
		_nop_();_nop_();_nop_();
	}
	CLK = 0;
	DIO = 0;
}


static void send_command_nodata(unsigned char cmd)
{
	STB = 1;
	_nop_();
	STB = 0;
	send_8bit(cmd);
	_nop_();_nop_();_nop_();
	STB = 1;
}


static void send_command_withdata(unsigned char cmd)
{
	STB = 1;
	_nop_();
	STB = 0;
	send_8bit(cmd);
	_nop_();_nop_();_nop_();
}


static void write_string(unsigned char* str,unsigned char len)
{
	unsigned char i;
	for (i=0;i<len;i++)
		send_8bit(*str++);
}




void data_init(void)
{
	unsigned char i = 0;
//	for(i=0;i<14;i++)
//	{				
//		BCD_LABLE_ANODE[0] = 0x7f;
		BCD_LABLE_ANODE[1] = 0x04;
		//BCD_LABLE_ANODE[i] = BCD_LABLE_ALL[number];
//	}
//	BCD_LABLE_ANODE[0] = 0xff;

	BCD_LABLE_ANODE[2] = BCD_LABLE_ALL[2]|0x2;
//	BCD_LABLE_ANODE[3] = 7;
	BCD_LABLE_ANODE[4] = BCD_LABLE_ALL[2];
	BCD_LABLE_ANODE[6] = BCD_LABLE_ALL[3];
	BCD_LABLE_ANODE[8] = BCD_LABLE_ALL[4]| 0x80;
	BCD_LABLE_ANODE[10] = BCD_LABLE_ALL[5];
	BCD_LABLE_ANODE[12] = BCD_LABLE_ALL[6];
//	
//	
//	BCD_LABLE_ANODE[1] = 0x7;
//	BCD_LABLE_ANODE[3] = 0x7;
//	BCD_LABLE_ANODE[5] = 0x7;
//	BCD_LABLE_ANODE[7] = 0x7;
//	BCD_LABLE_ANODE[9] = 0x7;
//	BCD_LABLE_ANODE[11] = 0x7;
//	BCD_LABLE_ANODE[13] = 0x7;
}

//addr 从哪开始，显控寄存器的地址
//cnt  刷新几个数字？ cnt + addr 不能大于13 ，只有14个寄存器
static void display(unsigned char addr,unsigned char cnt)
{
	if(addr > 13 || (addr + cnt > 14))  //超过数组的范围
			return ;
	
	send_command_nodata(0x3);    //7位11段模式
	send_command_nodata(0x40);   //普通模式，地址自增，写到显示寄存器
	send_command_withdata(0xc0+addr);   //起始地址
	write_string(BCD_LABLE_ANODE+addr,cnt);  //写入数据
	//write_string(BCD_LABLE_ALL+number,1);  //写入数据
	send_command_nodata(cmd_dis_mode);

}




//addr   哪个地址，使用显示哪个寄存器0-13；
static void display_fixdaddr(unsigned char addr)
{
//	NE_C(BCD_LABLE_ALL[number],cnt1);
	
	if(addr > 13)  //超过范围。
		return;
	
	send_command_nodata(0x3);    //7位11段模式
	send_command_nodata(0x44);    //普通模式，地址固定，写到显示寄存器
	send_command_withdata(0xc0 + addr);
	send_8bit(BCD_LABLE_ANODE[addr]);

	send_command_nodata(cmd_dis_mode);
//	display(addr,1);

}


void display_time(unsigned char hour,unsigned char min)
{
	unsigned char msb;
	msb = hour>>4;
	if(msb)
	BCD_LABLE_ANODE[2] = BCD_LABLE_ALL[msb];
	else
		BCD_LABLE_ANODE[2] = 0;    //高位为0的时候不显示
	BCD_LABLE_ANODE[4] = BCD_LABLE_ALL[hour&0xf];
	msb = BCD_LABLE_ANODE[6] &0x80;
	BCD_LABLE_ANODE[6] = BCD_LABLE_ALL[min>>4] | msb;
	msb = BCD_LABLE_ANODE[8] &0x80;
	BCD_LABLE_ANODE[8] = BCD_LABLE_ALL[min&0xf] | msb;
	
	display(2,4*2);
}


void display_watertemp(char temp)
{
	unsigned char msb;
	
	
	if(temp > 110)
	{
		BCD_LABLE_ANODE[10] = BCD_LABLE_ALL[11];
		BCD_LABLE_ANODE[12] = BCD_LABLE_ALL[11];
	}
	 
	else if(temp == -1)   //低于下限
	{
		temp = 0;
	}
	else if(temp == -2)   //高于上限
	{
		temp = 99;
	}
	else
	{
		msb = BCD_LABLE_ANODE[10] &0x80;
		BCD_LABLE_ANODE[10] = BCD_LABLE_ALL[temp/10]| msb;
		msb = BCD_LABLE_ANODE[12] &0x80;
		BCD_LABLE_ANODE[12] = BCD_LABLE_ALL[temp%10]| msb;
	}
	display(10,2*2);
}




//state 非零表示选中，0表示没选中
//addr 显示寄存器编号
//whichbit 实际使用的位，只有一位有效！！！ 如0x80表示最高位
static void state_change(unsigned char addr,unsigned char whichbit,unsigned char state)
{
	if(state)  //置1某位。
	{
		if((BCD_LABLE_ANODE[addr] & whichbit) != whichbit)  //如果该位为0，则执行置1
		{
			BCD_LABLE_ANODE[addr] |= whichbit;
			display_fixdaddr(addr);
		}
	}
	else if(BCD_LABLE_ANODE[addr] & whichbit)  //如果该位为1，则执行清零
	{
		BCD_LABLE_ANODE[addr] &= ~whichbit;
		display_fixdaddr(addr);
	}
}

//定时上水显示控制
//state 非零表示选中，0表示没选中
void state_dingshi_shangshui(unsigned char state)
{
	state_change(6,0x80,state);
}

//上水灯显示控制
//state 非零表示选中，0表示没选中
void state_shangshui(unsigned char state)
{
	state_change(13,0x4,state);
}

//增压灯显示控制
//state 非零表示选中，0表示没选中
void state_zengya(unsigned char state)
{
	state_change(13,0x2,state);
}

//时间冒号灯显示控制
//state 非零表示选中，0表示没选中
void state_miao(unsigned char state)
{
	state_change(2,0x2,state);
}

//温控灯显示控制
void state_wenkong(unsigned char state)
{
	state_change(11,0x2,state);
}


#if 1
typedef enum {PERCENT0,PERCENT20,PERCENT50,PERCENT80,PERCENT100,LEVEL_ERR}Level_state_t;
Level_state_t level_stat = PERCENT0;


void state_water_level(unsigned char state)
{
	switch(level_stat)
	{
		case PERCENT100:
			state_change(0,0x80,1);  //显示100%
			state_change(1,0x7,1);   //显示20,50,80%
			if(state < 0x60)
				level_stat = PERCENT80;   //切换到80%
			else if(state > 0xa0)
				level_stat = LEVEL_ERR;   //切换到错误
			
			if(state > 0x6e)
				shangshui_enable(0);   //停止上水
			
			break;
		case PERCENT80:
			state_change(1,0x7,1);    //显示20,50,80%
			state_change(0,0x80,0);   //熄灭100%
			if(state > 0x70)
				level_stat = PERCENT100;   //切换到100%
			else if(state < 0x40)
				level_stat = PERCENT50;   //切换到50%
			break;
		case PERCENT50:
			state_change(0,0x80,0);  //熄灭100%
			state_change(1,0x1,0);		//熄灭80%
			state_change(1,0x6,1);   //显示20,50%
			if(state > 0x50)
				level_stat = PERCENT80;   //切换到80%
			else if(state < 0x20)
				level_stat = PERCENT20;   //切换到20%			
			break;
		case PERCENT20:
			state_change(0,0x80,0);  //熄灭100%
			state_change(1,0x3,0);	 //熄灭80，50%	
			state_change(1,0x4,1);   //显示20%
			if(state > 0x30)
				level_stat = PERCENT50;   //切换到50%
			else if(state < 0x10)
				level_stat = PERCENT0;   //切换到0%
			break;
		case PERCENT0:
			state_change(0,0x80,0); 
			state_change(1,0x7,0);
			if(state > 0x15)
				level_stat = PERCENT20;   //切换到20%
			else if(state < 0x2)
				level_stat = LEVEL_ERR;   //切换到错误
			break;
		case LEVEL_ERR:	   //故障
			state_change(0,0x80,1);   //100%
			state_change(1,0x3,0);
			state_change(1,0x4,1);	//20%					
			if(state > 0x5 && state < 0xa0)
				level_stat = PERCENT0;   //切换到50%
			break;
	}
}


# else
void state_water_level(unsigned char state)
{	
	if(state == 0) //故障，无连接		
	{
		state_change(0,0x80,1);   //100%
		state_change(1,0x3,0);
		state_change(1,0x4,1);	//20%
	}
	else if(state < 0x20 )   //显示20%
	{
		state_change(0,0x80,0);  //熄灭100%
		state_change(1,0x3,0);	 //熄灭80，50%	
		state_change(1,0x4,1);   //显示20%
	}
	else if(state < 0x40)
	{	
		state_change(0,0x80,0);  //熄灭100%
		state_change(1,0x1,0);		//熄灭80%
		state_change(1,0x6,1);   //显示20,50%
	}
	else if(state < 0x60)      //0x6c 显示100%,0x65-0x6c可以闪烁一下
	{
		state_change(1,0x7,1);    //显示20,50,80%
		state_change(0,0x80,0);   //熄灭100%
	}
//	else if(state < 0x6B)      //0x6c 显示100%,0x65-0x6c可以闪烁一下
//	{
//		state_change(1,0x7,1);    //显示20,50,80%
//		state_change(0,0x80,0);   //熄灭100%
//	}
	else if(state < 0xa0)    //150 2022-04-01
	{	
		state_change(0,0x80,1);  //显示100%
		state_change(1,0x7,1);   //显示20,50,80%
		
		if(state >= 0x6c)
		{
			//停止上水
		}
	}		
	else 	//所有的熄灭
	{
		state_change(0,0x80,0); 
		state_change(1,0x7,0);
	}	
			
}
#endif




void display_start(void)
{
	data_init();
	display(0,14);
	
	state_wenkong(1);
}






