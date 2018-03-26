/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "RH_RF95.h" // #include <RadioHead.h>


#define RFM95_CS 5   // LORA_CS_PIN 
#define RFM95_RST 36 // LORA_RST_PIN is 36
#define RFM95_INT 26 // M5 LORA_IRQ_PIN  
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

void blink_task(void *pvParameter)
{
	gpio_pad_select_gpio(static_cast<gpio_num_t>(BLINK_GPIO));
    /* Set the GPIO as a push/pull output */
	gpio_set_direction(static_cast<gpio_num_t>(BLINK_GPIO), GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
	    gpio_set_level(static_cast<gpio_num_t>(BLINK_GPIO), 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
	    gpio_set_level(static_cast<gpio_num_t>(BLINK_GPIO), 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main()
{
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
