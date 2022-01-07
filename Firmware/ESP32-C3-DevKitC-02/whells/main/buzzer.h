

#ifndef __BUZZER_H__
#define  __BUZZER_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "sdkconfig.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include "esp_spi_flash.h"
    #include <esp_types.h>
    #include "esp_intr.h"    
    #include "freertos/queue.h"
    #include "driver/gpio.h"


    struct BUZZER_queue
    {
        uint8_t   state;

        uint16_t  timeWaitON; //mS
        uint16_t  timeWaitOFF; //mS
    };

    QueueHandle_t buzzer_Queue;




    void buzzer_setup(void);
    void buzzer_Task(void *pvParameter);






#endif

