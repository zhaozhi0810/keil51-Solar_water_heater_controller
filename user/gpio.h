

#ifndef __GPIO_H__
#define __GPIO_H__

void gpio_init(void);
void shangshui_enable(char en);

//由定时器10ms查询，
void keys_scan(void);

//由main函数循环扫描处理
void key_poll_handle(void);

#endif


