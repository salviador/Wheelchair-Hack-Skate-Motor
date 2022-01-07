#include "buzzer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_BUZZER    3
#define GPIO_OUTPUT_BUZZER_PIN_SEL  (1ULL<<GPIO_OUTPUT_BUZZER)


void buzzer_setup(void){
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_BUZZER_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_OUTPUT_BUZZER, 0);

    xTaskCreatePinnedToCore(buzzer_Task, "BUZZER", 4096, NULL, tskIDLE_PRIORITY, NULL, tskIDLE_PRIORITY);
}





void buzzer_Task(void *pvParameter) {
    struct BUZZER_queue msg;

    buzzer_Queue = xQueueCreate(10, sizeof(msg));

    while(1) { 
        if (xQueueReceive(buzzer_Queue, (void *)&msg, portMAX_DELAY) == pdPASS ) {

            if(msg.state){
                gpio_set_level(GPIO_OUTPUT_BUZZER, 1);
                vTaskDelay(msg.timeWaitON / portTICK_RATE_MS);		
                gpio_set_level(GPIO_OUTPUT_BUZZER, 0);
                vTaskDelay(msg.timeWaitOFF / portTICK_RATE_MS);		
            }else{
                gpio_set_level(GPIO_OUTPUT_BUZZER, 0);
            }
        }
    }
}