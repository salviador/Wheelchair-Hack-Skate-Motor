/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_err.h"
#include "esp_log.h"

#include "driver/periph_ctrl.h"
#include <esp_types.h>
#include "esp_intr.h"
#include "freertos/queue.h"
#include "freertos/message_buffer.h"


#include "main.h"
#include "bleService.h"
#include "ledRGB.h"
#include "vesccan.h"
#include "wheels.h"
#include "buzzer.h"

#include "debugGPIO.h"
#include "wii.h"

//https://www.lucadentella.it/2017/01/16/esp32-6-collegamento-ad-una-rete-wifi/
//static EventGroupHandle_t wifi_event_group; 


static const char *TAGMAIN = "MAIN";

struct Led_queue msgLED;






void app_main(void)
{
    //Debug GPIO 3
    debugGPIO3_setup();

    //Led Setup
    led_Queue = xQueueCreate(8, sizeof(msgLED));
    xTaskCreate(&LedRGB_Task, "LedRGB_task", 2048, NULL, tskIDLE_PRIORITY, NULL);

    //BUZZER SETUP
    buzzer_setup();

    //VES CAN Setup
    VESC_setup();

    //WHEELS
    wheels_setup();
    xTaskCreatePinnedToCore(wheels_task, "WHEELS", 4096, NULL, tskIDLE_PRIORITY+4, NULL, tskIDLE_PRIORITY+3);

    //wii joystick
    wii_setup();


    //BLE GATT SERVER Setup
    BLE_Service_Start();


    //Solo x Test
//    xTaskCreate(BLE_Send_Data, "SEND", 2048, NULL, tskIDLE_PRIORITY, NULL);


}


























    /*
    uint8_t dataBLE[4] = {0x00,0x00,0x00,0x00};

    //Test
    uint32_t dataToTEst; 
    dataToTEst = 0;
    xQueueTest = xQueueCreate( 1, sizeof( uint32_t ) );

    xTaskCreate(&xQueueTest_Task, "xQueueTest_Task", 2048, NULL, tskIDLE_PRIORITY, NULL);
    


    while(1){
       //xQueueOverwrite(xQueueTest, &dataToTEst);
       dataToTEst++;
       //ESP_LOGI(TAGMAIN,"dataToTEst: %d", dataToTEst);


       dataBLE[0] = dataBLE[0] + 1;
       BLE_Send_Data(&dataBLE[0]);

       vTaskDelay(1250 / portTICK_PERIOD_MS);

    }
    */

   /*
    //Test
    QueueHandle_t xQueueTest;
    void xQueueTest_Task(void *pvParameter) {
        uint32_t ToTEst; 


        while(1){
            if (xQueueReceive(xQueueTest, &ToTEst, portMAX_DELAY) == pdPASS ) {

               ESP_LOGI("TASK xQueueTest_Task" , "ToTEst: %d", ToTEst);
            }
            ESP_LOGI("TASK xQueueTest_Task", "<Loop>");
    
            if((ToTEst % 10)==0){
                vTaskDelay(5000 / portTICK_PERIOD_MS);
            }

        }
    }  
   */



    /*
    //VES CAN Setup
    VESC_setup();

    while (1)
    {

        
        VESC_SET_DutyCycle(5,0.005,6,-0.3);



       vTaskDelay(50 / portTICK_PERIOD_MS);

    }
    */




