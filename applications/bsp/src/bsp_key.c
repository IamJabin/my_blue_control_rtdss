#include "bsp_key.h"
#include "n32wb03x_gpio.h"
#include "ns_log.h"
#include "bsp_beep.h"

extern uint32_t SystickGetime(void);

void bsp_key_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(BSP_BEEP_GPIO_PORT));

    /* Enable the GPIO Clock */
    if (BSP_KEY_GPIO_PORT == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    }
    else if (BSP_KEY_GPIO_PORT == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    }   

    /* Configure the GPIO pin */
    if (BSP_KEY1_GPIO_PIN <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin = BSP_KEY1_GPIO_PIN | BSP_KEY2_GPIO_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
        GPIO_InitPeripheral(BSP_KEY_GPIO_PORT, &GPIO_InitStructure);
    }    
}

static uint32_t beep_start_time = 0;
static uint8_t beep_active = 0;

static uint8_t check_key(uint32_t *key_time, uint8_t *key_status, uint16_t key_pin, uint8_t key_code)
{
    static uint32_t time_now = 0;

    time_now = SystickGetime();

    if (time_now - *key_time > 10)
    {
        *key_time = time_now;
        if (GPIO_ReadInputDataBit(BSP_KEY_GPIO_PORT, key_pin))
        {
            if (*key_status == 0)
            {
                *key_status = 1;
                // NS_LOG_INFO("key down\r\n");
                return key_code;
            }
        }
        else
        {
            if (*key_status == 1)
            {
                *key_status = 0;
                // NS_LOG_INFO("key up\r\n");
            }
        }
    }

    return NO_KEY_PRESSED;
}

uint8_t bsp_key_scan(void)
{
    static uint8_t key1_status = 0;
    static uint8_t key2_status = 0;
    static uint32_t key1_time = 0;
    static uint32_t key2_time = 0;

    uint8_t key_pressed = NO_KEY_PRESSED;

    key_pressed = check_key(&key1_time, &key1_status, BSP_KEY1_GPIO_PIN, KEY1_OK);
    if (key_pressed == NO_KEY_PRESSED)
    {
        key_pressed = check_key(&key2_time, &key2_status, BSP_KEY2_GPIO_PIN, KEY2_OK);
    }

    // 检查蜂鸣器是否需要启动
    if (key_pressed != NO_KEY_PRESSED && !beep_active)
    {
        beep_start_time = SystickGetime();
        beep_active = 1;
        bsp_beep_on();
    }

    // 检查蜂鸣器是否需要停止
    if (beep_active && (SystickGetime() - beep_start_time > 50))
    {
        beep_active = 0;
        bsp_beep_off();
    }

    return key_pressed;
}