#ifndef __DEV_H
#define __DEV_H

#include "n32wb03x.h"

#include "bsp_gpio.h"
#include "bsp_smg.h"

typedef enum{
    fuc_back_ok,
    fuc_back_err
}fuc_back_t;

int bsp_init(void);
void bsp_run(void);
void SysTick_Handler(void);
uint32_t SystickGetime(void);

#endif
