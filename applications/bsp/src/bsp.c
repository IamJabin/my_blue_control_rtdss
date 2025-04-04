#include "bsp.h"
#include "bsp_key.h"

#define DEMO_STRING  "\r\n device init : \r\n"

#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_100MS ((uint32_t)100000)
#define SYSTICK_1S  ((uint32_t)1000000)

#define BSP_SYSTICK_TICK       SYSTICK_100MS

volatile uint32_t Tick_num = 0;
void SysTick_Handler(void)
{
    Tick_num++;
}

// 实现 SystickGetime 函数
uint32_t SystickGetime(void)
{
    return Tick_num; // 返回当前的 SysTick 滴答计数值
}

int bsp_init(void)
{
    drv_soft_iic_init();

    bsp_DigitronPowerBrightnessSet(BSP_DIGITRON_POWER_ON, BSP_DIGITRON_BRIGHTNESS_8); // 启动数码管、设置显示亮度为8级
    bsp_DigitronDisplayTime(0); 	// 数码管默认显示00:03

    SystemCoreClockUpdate(); // 更新系统时钟频率
    SysTick_Config(SystemCoreClock / BSP_SYSTICK_TICK);  // 配置 SysTick 定时器，默认64MHz

    bsp_key_init();
    return true;
}


void bsp_run(void)
{
    static uint32_t old_time = 0;
    static uint32_t old_sec_time = 0;
    static uint32_t digit_number = 3;
    bsp_DigitronDisplayTime(digit_number);
    //如果按键1按下显示值加一，按键2按下显示值减一
    uint8_t key_status = bsp_key_scan();
    if(key_status == KEY1_OK)
    {
        digit_number++;
        if(digit_number > 99)
        {
            digit_number = 0;
        }
    }
    else if(key_status == KEY2_OK)
    {
        digit_number--;
        if(digit_number > 99)
        {
            digit_number = 99;
        }
    }

}