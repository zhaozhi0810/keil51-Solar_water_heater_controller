

#ifndef __AD_H__
#define __AD_H__

void InitADC();

void ADC_start(void);

//unsigned char GetADCResult();
unsigned char GetADCResult(unsigned int * res);
//void ShowResult(unsigned char ch);
unsigned int get_temp_res(void);


unsigned int get_level_res(void);

#endif
