#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include <stdint.h>

#define BSP_KEY_GPIO_PORT  GPIOA

#define BSP_KEY1_GPIO_PIN   GPIO_PIN_0
#define BSP_KEY2_GPIO_PIN   GPIO_PIN_1


// 宏定义按键状态
#define KEY1_OK 1
#define KEY2_OK 2
#define NO_KEY_PRESSED 0

void bsp_key_init(void);
uint8_t bsp_key_scan(void);



#endif // !__BSP_KEY_H__