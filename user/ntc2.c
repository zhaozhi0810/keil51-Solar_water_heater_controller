
#include "ntc.h"
#include "ad.h"
#include "uart1.h"

uint16 adc_voltage;   //采集的电压
//温度与NTC电阻阻值对应关系表
//MF52 10K3435 温度特性表
uint16 code ntc10k_tab[] = {

// 阻值（Ω），温度
// 190556, /* -40 */ 
// 183413, /* -39 */ 
// 175674, /* -38 */ 
// 167647, /* -37 */ 
// 159565, /* -36 */ 
// 151598, /* -35 */ 
// 143862, /* -34 */ 
// 136436, /* -33 */ 
// 129364, /* -32 */ 
// 122668, /* -31 */ 
// 116352, /* -30 */ 
// 110410, /* -29 */ 
// 104827, /* -28 */ 
// 99585, /* -27 */ 
// 94661, /* -26 */ 
// 90033, /* -25 */ 
// 85678, /* -24 */ 
// 81575, /* -23 */ 
// 77703, /* -22 */ 
// 74044, /* -21 */ 
// 70581, /* -20 */ 
// 67299, /* -19 */ 
// 64183, /* -18 */ 
// 61223, /* -17 */ 
// 58408, /* -16 */ 
// 55728, /* -15 */ 
// 53177, /* -14 */ 
// 50746, /* -13 */ 
// 48429, /* -12 */ 
// 46222, /* -11 */ 
//44120, /* -10 */ 
//42118, /* -9 */ 
//40212, /* -8 */ 
//38399, /* -7 */ 
//36675, /* -6 */ 
//35036, /* -5 */ 
//33480, /* -4 */ 
//32004, /* -3 */ 
//30603, /* -2 */ 
//29275, /* -1 */ 
28017, /* 0 */ 
26826, /* 1 */ 
25697, /* 2 */ 
24629, /* 3 */ 
23618, /* 4 */ 
22660, /* 5 */ 
21752, /* 6 */ 
20892, /* 7 */ 
20075, /* 8 */ 
19299, /* 9 */ 
18560, /* 10 */ 
18482, /* 11 */ 
18149, /* 12 */ 
17632, /* 13 */ 
16992, /* 14 */ 
16280, /* 15 */ 
15535, /* 16 */ 
14787, /* 17 */ 
14055, /* 18 */ 
13354, /* 19 */ 
12690, /* 20 */ 
12068, /* 21 */ 
11490, /* 22 */ 
10954, /* 23 */ 
10458, /* 24 */ 
10000, /* 25 */ 
9576, /* 26 */ 
9184, /* 27 */ 
8819, /* 28 */ 
8478, /* 29 */ 
8160, /* 30 */ 
7861, /* 31 */ 
7579, /* 32 */ 
7311, /* 33 */ 
7056, /* 34 */ 
6813, /* 35 */ 
6581, /* 36 */ 
6357, /* 37 */ 
6142, /* 38 */ 
5934, /* 39 */ 
5734, /* 40 */ 
5541, /* 41 */ 
5353, /* 42 */ 
5173, /* 43 */ 
4998, /* 44 */ 
4829, /* 45 */ 
4665, /* 46 */ 
4507, /* 47 */ 
4355, /* 48 */ 
4208, /* 49 */ 
4065, /* 50 */ 
3927, /* 51 */ 
3794, /* 52 */ 
3664, /* 53 */ 
3538, /* 54 */ 
3415, /* 55 */ 
3294, /* 56 */ 
3175, /* 57 */ 
3058, /* 58 */ 
2941, /* 59 */ 
2825, /* 60 */ 
2776, /* 61 */ 
2718, /* 62 */ 
2652, /* 63 */ 
2582, /* 64 */ 
2508, /* 65 */ 
2432, /* 66 */ 
2356, /* 67 */ 
2280, /* 68 */ 
2207, /* 69 */ 
2135, /* 70 */ 
2066, /* 71 */ 
2000, /* 72 */ 
1938, /* 73 */ 
1879, /* 74 */ 
1823, /* 75 */ 
1770, /* 76 */ 
1720, /* 77 */ 
1673, /* 78 */ 
1628, /* 79 */ 
1586, /* 80 */ 
1546, /* 81 */ 
1508, /* 82 */ 
1471, /* 83 */ 
1435, /* 84 */ 
1401, /* 85 */ 
1367, /* 86 */ 
1334, /* 87 */ 
1301, /* 88 */ 
1268, /* 89 */ 
1236, /* 90 */ 
1204, /* 91 */ 
1171, /* 92 */ 
1139, /* 93 */ 
1107, /* 94 */ 
1074, /* 95 */ 
1042, /* 96 */ 
1010, /* 97 */ 
979, /* 98 */ 
948, /* 99 */ 
// 918, /* 100 */ 
// 889, /* 101 */ 
// 861, /* 102 */ 
// 835, /* 103 */ 
// 810, /* 104 */ 
// 787, /* 105 */ 
// 767, /* 106 */ 
// 749, /* 107 */ 
// 733, /* 108 */ 
// 721, /* 109 */ 
// 713, /* 110 */ 
};

int8 NTC_bSearch(uint16 *tab, uint8 tab_size, int16 val)
{
    uint8 mid	= 0; 
    uint8 left	= 0; 
	uint8 right	= tab_size - 1;

	if(val > tab[0]) return -1; // 温度下限
	else if(val < tab[tab_size - 1]) return -2; // 温度上限

    while(left <= right)
	{ 
        mid = (right + left) / 2;
        if(val == tab[mid])
            return mid;
        else if (val > tab[mid])
            right = mid - 1;
        else if (val < tab[mid])
            left = mid + 1;
	}

    return mid;
}


/*
根据ADC电压值计算NTC10K电阻值
r_ref为1%精度 10k电阻阻值，如 9963 Ω
adc_voltage是读取到的NTC10K电压
vdd_voltage是读取到的系统电压
*/
uint16 NTC_ReadR(uint16 adc_voltage)
{
    //计算ADC通道的电阻值（欧姆），参考分压电阻值r_ref欧姆
	return 24000.0* adc_voltage / (1024 - adc_voltage) ;
}

int16 NTC_ReadT(void)
{
	uint16 ntc_r = 0;
	uint16 ref_r = 0;
	int16  ntc_t = 0;
	uint8 idx  = 0;
	int16 dec  = 0;

//	ADC_ReadBandGapVoltage();
//	if(DEBUG_MODE) printf(">>>NTC_ReadT: bdg_voltage = %5.2f mV\r\n", (float)bdg_voltage);

//	ADC_ReadCh9Val();
//	if(DEBUG_MODE) printf(">>>NTC_ReadT: adc_ch9_val = %u\r\n", adc_ch9_val);
	
	//指定ADC端口P1.4读取NTC电阻电压
	ntc_t=ADC_GetResult(7); 
	if(ntc_t > 1000)
		return 160;
	
	//ntc_r=4240/1024*ntc_t;
	SendData(adc_voltage>>8); 
	SendData(adc_voltage); 
	SendData(0xff);
//	if(DEBUG_MODE) printf(">>>NTC_ReadT: adc_voltage = %5.2f mV\r\n", adc_voltage);

	//ADC_ReadVddVoltage();
//	if(DEBUG_MODE) printf(">>>NTC_ReadT: vdd_voltage = %5.2f mV\r\n", vdd_voltage);
	
	ntc_r = NTC_ReadR(ntc_t);
//	if(DEBUG_MODE) printf(">>>NTC_ReadT: ntc_r = %u ohm\r\n", ntc_r);
	SendData(ntc_r>>8); 
	SendData(ntc_r);
	idx = NTC_bSearch(ntc10k_tab, ARRAY_SIZE(ntc10k_tab), ntc_r);
	//SendData(idx); 
	//SendData(0); 
	if(-1 == idx) 		return -99;   //低于0
	else if(-2 == idx)	return 150;   //高于99
	
	//if(DEBUG_MODE) printf(">>>NTC_bSearch: ntc10k_tab[%bu] = %d ohm\r\n", idx, ntc10k_tab[idx]);

//	if(ntc_r != ntc10k_tab[idx])
//	{
//		// 使用相邻阻值区间估算温度值小数部分，四舍五入
//		if(ntc_r > ntc10k_tab[idx] && idx > 0)
//		{
//			dec = 100 * (ntc_r - ntc10k_tab[idx])/(ntc10k_tab[idx-1] - ntc10k_tab[idx]);
//			//if(DEBUG_MODE) printf(">>>NTC_ReadT: dec = %d = 100 * (%d)/(%d)\r\n", dec, (ntc_r - ntc10k_tab[idx]), (ntc10k_tab[idx-1] - ntc10k_tab[idx]));
//		}
//		else if(ntc_r < ntc10k_tab[idx] && idx < ARRAY_SIZE(ntc10k_tab))
//		{
//			dec = 100 * (ntc10k_tab[idx] - ntc_r)/(ntc10k_tab[idx] - ntc10k_tab[idx+1]);
//			//if(DEBUG_MODE) printf(">>>NTC_ReadT: dec = %d = 100 * (%d)/(%d)\r\n", dec, (ntc10k_tab[idx] - ntc_r), (ntc10k_tab[idx] - ntc10k_tab[idx+1]));
//		}
////		if(5 < dec % 10) 
////			dec += 10;
////		dec /= 10;

////		if(ntc_r > ntc10k_tab[idx])
////		{
////			dec *= -1;
////		}
//	}

//	if(DEBUG_MODE) printf(">>>NTC_ReadT:   int = %4.1f c\r\n", (float)(idx-9));
//	if(DEBUG_MODE) printf(">>>NTC_ReadT:   dec = %2.1f c\r\n", (float)dec/10);

	ntc_t = (idx-9); //+ dec;  //小数不考虑

	//if(DEBUG_MODE) printf(">>>NTC_ReadT: ntc_t = %4.1f c\r\n", (float)ntc_t/10);

	return ntc_t;
}

