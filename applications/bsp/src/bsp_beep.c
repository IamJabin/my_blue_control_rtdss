#include "bsp_beep.h"
#include "bsp.h"
#include "n32wb03x_gpio.h"

#include "ns_log.h"

bsp_beep_mode_t beep_status = BSP_BEEP_MODE_STOP;

/**
 * @brief 蜂鸣器初始化
 * 
 */
void bsp_beep_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(BSP_BEEP_GPIO_PORT));

    /* Enable the GPIO Clock */
    if (BSP_BEEP_GPIO_PORT == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    }
    else if (BSP_BEEP_GPIO_PORT == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    }

    /* Configure the GPIO pin */
    if (BSP_BEEP_GPIO_PIN <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin = BSP_BEEP_GPIO_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitPeripheral(BSP_BEEP_GPIO_PORT, &GPIO_InitStructure);
    }    
    bsp_beep_off();
}

/**
 * @brief 开启蜂鸣器
 * 
 */
void bsp_beep_on(void)
{
    GPIO_SetBits(BSP_BEEP_GPIO_PORT, BSP_BEEP_GPIO_PIN);
}

/**
 * @brief 关闭蜂鸣器
 * 
 */
void bsp_beep_off(void)
{
    GPIO_ResetBits(BSP_BEEP_GPIO_PORT, BSP_BEEP_GPIO_PIN);
}

/**
 * @brief 非阻塞式蜂鸣器函数，使用时需要将bsp_beep_process()放入while(1)中
 * 
 * @param time 
 * @return bsp_beep_status_t 如果蜂鸣器当前处于运行模式则不可再次设置，返回ERROR
 */
bsp_beep_status_t bsp_beep_on_nonblock(uint32_t time)
{
    if (BSP_BEEP_MODE_RUNNING == beep_status)
    {
       return BSP_BEEP_STATUS_ERROR;
    }

  //  time_out = SystickGetime() + time;
    bsp_beep_on();
    beep_status = BSP_BEEP_MODE_RUNNING;

    return BSP_BEEP_STATUS_OK;
}

/**
 * @brief 用于实现蜂鸣器非阻塞，在使用非阻函数时，需要将该函数放入while(1)中
 * 
 */
void bsp_beep_process(uint32_t *time_out, uint32_t *beep_status)
{
    // 确保 beep_status 和 time_out 已正确初始化
    if (BSP_BEEP_MODE_RUNNING == *beep_status)
    {
        // 获取当前系统时间并存储到局部变量中，减少重复调用
        uint32_t current_time = SystickGetime();

        // 检查时间是否超时
        if (current_time > *time_out)
        {
            // 防止 time_out 被设置为非法值
            *time_out = 0;

            // 停止蜂鸣器状态
            *beep_status = BSP_BEEP_MODE_STOP;

            // 关闭蜂鸣器
            bsp_beep_off();
        }
    }
}


// Path: BSP/bsp_led.c
