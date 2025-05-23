/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file main.c
 * @author Nations Firmware Team
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/** @addtogroup 
 * @{
 */
 
/* Includes ------------------------------------------------------------------*/
#include "n32wb03x.h"
#include "rwip.h"
#include "ns_ble.h"
#include "ns_sleep.h"
#include "ns_delay.h"
#include "ns_log.h"
#include "bsp_usart.h"
#include "bsp_gpio.h"
#include "app_ble.h"
#if  (CFG_APP_NS_IUS)
#include "ns_dfu_boot.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEMO_STRING  "\r\n Nations raw data transfer server(128bit UUID) demo \r\n"

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
 * @brief  main function
 * @param   
 * @return 
 * @note   Note
 */
#if(1)
int main(void)
{
    borad_init();
    bsp_init();
    while(1)
    {
        /*schedule all pending events*/
        rwip_schedule();
        //ns_sleep();
        ble_rx_msg();
        bsp_run();
    }
}

#else
int main(void)
{
    //for hold the SWD before sleep
    delay_n_10us(200*1000);
    

    NS_LOG_INIT();

    #if  (CFG_APP_NS_IUS)
    if(CURRENT_APP_START_ADDRESS == NS_APP1_START_ADDRESS){
        NS_LOG_INFO("application 1 start new ...\r\n");
    }else if(CURRENT_APP_START_ADDRESS == NS_APP2_START_ADDRESS){
        NS_LOG_INFO("application 2 start new ...\r\n");
    }
    #endif
    app_ble_init();
    
    NS_LOG_INFO(DEMO_STRING);

    // periph init 
    LedInit(LED1_PORT,LED1_PIN);  // power led
    LedInit(LED2_PORT,LED2_PIN);  //connection state
    LedInit(LED3_PORT,LED3_PIN);
    LedOff(LED1_PORT,LED1_PIN);   
    LedOff(LED2_PORT,LED2_PIN);  
    LedOff(LED3_PORT,LED3_PIN);   
    app_usart_dma_enable(ENABLE);
    //init text
    usart_tx_dma_send((uint8_t*)DEMO_STRING, sizeof(DEMO_STRING)); 

    delay_n_10us(500);
    //disable usart for enter sleep
    app_usart_dma_enable(DISABLE);
    

    while (1)
    {
        /*schedule all pending events*/
        rwip_schedule();
        LedOff(LED3_PORT,LED3_PIN);
        delay_n_ms(500); 
        LedOn(LED3_PORT,LED3_PIN);
				delay_n_ms(500); 
        //ns_sleep();

    }
}

/**
 * @brief  user handle before enter sleep mode
 * @param  
 * @return 
 * @note   
 */
void app_sleep_prepare_proc(void)
{

}

/**
 * @brief  user handle after wake up from sleep mode
 * @param  
 * @return 
 * @note   
 */
void app_sleep_resume_proc(void)
{
    
    
}



/**
 * @}
 */
#endif
