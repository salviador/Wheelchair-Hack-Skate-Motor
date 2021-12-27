#include "vesccan.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_types.h>
#include "esp_intr.h"
#include "freertos/queue.h"

#include "driver/twai.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/periph_ctrl.h"

#include "debugGPIO.h"
#include "freertos/semphr.h"

#define USB_DEVICE_CONF0_REG        (0x60043018)
#define USB_DEVICE_USB_PAD_ENABLE   (BIT(14))


#define TAGCAN "CANDUMP"

struct vesccanARRYMSG
{
    twai_message_t L;
    twai_message_t R;
}ARRAY_twai_message;

/*
SemaphoreHandle_t xSemaphore;


void twai_transmit_task(void *arg)
{
    struct vesccanARRYMSG tx_message;
    while(1) {
        if (xQueueReceive(xQueueTXcan, &tx_message, portMAX_DELAY) == pdPASS ) {


            if( xSemaphoreTake( xSemaphore, (10 / portTICK_PERIOD_MS) ) == pdTRUE )
            {
                        ESP_LOGI(TAGCAN, "L:  %x %x %x %x", tx_message.L.data[3],tx_message.L.data[2],tx_message.L.data[1],tx_message.L.data[0]);

                        debugGPIO3_TGL();
                        twai_transmit(&tx_message.L, 0);
                        twai_transmit(&tx_message.R, 0);

                xSemaphoreGive( xSemaphore );
            }
        
        
        
        }
    }
}
*/

void twai_receive_task(void *arg)
{
    twai_message_t rx_message;
    while(1) {
        if(twai_receive(&rx_message,portMAX_DELAY) == ESP_OK) {
        //if(twai_receive(&rx_message,pdMS_TO_TICKS(50)) == ESP_OK) {
            //ESP_LOGI(TAGCAN,"Received EXID: %x , run in CPU:%d", rx_message.identifier,xPortGetCoreID());
            /*
            for(int i = 0; i < rx_message.data_length_code; i++){            
                printf("%2x ", rx_message.data[i]);
            }
            printf("\n");
            */
        }
    }
}





void VESC_SET_DutyCycle(uint8_t EXIDL, float valueL,uint8_t EXIDR, float valueR){    // 0.0%  ---  100.0%  
    int32_t dudy_clycle_convL,dudy_clycle_convR; 
    struct vesccanARRYMSG message;

    dudy_clycle_convL = (int32_t)(100000 * valueL);

    message.L.identifier = EXIDL & 0x00FF;
    message.L.data_length_code = 4;
    message.L.data[3] = dudy_clycle_convL & 0x000000FF;
    message.L.data[2] = (dudy_clycle_convL & 0x0000FF00)>>8;
    message.L.data[1] = (dudy_clycle_convL & 0x00FF0000)>>16;
    message.L.data[0] = (dudy_clycle_convL & 0xFF000000)>>24;

message.L.rtr = 0;
message.L.ss = 0;
message.L.self = 0;
message.L.dlc_non_comp = 0;
message.L.flags = 0;
message.L.extd = 1;


    dudy_clycle_convR = (int32_t)(100000 * valueR);

    message.R.identifier = EXIDR & 0x00FF;
    message.R.data_length_code = 4;
    message.R.data[3] = dudy_clycle_convR & 0x000000FF;
    message.R.data[2] = (dudy_clycle_convR & 0x0000FF00)>>8;
    message.R.data[1] = (dudy_clycle_convR & 0x00FF0000)>>16;
    message.R.data[0] = (dudy_clycle_convR & 0xFF000000)>>24;
message.R.rtr = 0;
message.R.ss = 0;
message.R.self = 0;
message.R.dlc_non_comp = 0;
message.R.flags = 0;
message.R.extd = 1;

    
    //xQueueSend(xQueueTXcan, &message, (15 / portTICK_PERIOD_MS) );

   twai_transmit(&message.L,  10);
   twai_transmit(&message.R,  10 );
}



void VESC_SET_CurrentBREAK(uint8_t EXIDL, float valueL,uint8_t EXIDR, float valueR){    // 0.0%  ---  100.0%  
    int32_t dudy_clycle_convL,dudy_clycle_convR; 
    struct vesccanARRYMSG message;

    dudy_clycle_convL = (int32_t)(1000 * valueL);

    message.L.identifier = EXIDL & 0x00FF;
    message.L.identifier = message.L.identifier | 0x0200;

    message.L.data_length_code = 4;
    message.L.data[3] = dudy_clycle_convL & 0x000000FF;
    message.L.data[2] = (dudy_clycle_convL & 0x0000FF00)>>8;
    message.L.data[1] = (dudy_clycle_convL & 0x00FF0000)>>16;
    message.L.data[0] = (dudy_clycle_convL & 0xFF000000)>>24;

message.L.rtr = 0;
message.L.ss = 0;
message.L.self = 0;
message.L.dlc_non_comp = 0;
message.L.flags = 0;
message.L.extd = 1;


    dudy_clycle_convR = (int32_t)(1000 * valueR);

    message.R.identifier = EXIDR & 0x00FF;
    message.R.identifier = message.R.identifier | 0x0200;

    message.R.data_length_code = 4;
    message.R.data[3] = dudy_clycle_convR & 0x000000FF;
    message.R.data[2] = (dudy_clycle_convR & 0x0000FF00)>>8;
    message.R.data[1] = (dudy_clycle_convR & 0x00FF0000)>>16;
    message.R.data[0] = (dudy_clycle_convR & 0xFF000000)>>24;
message.R.rtr = 0;
message.R.ss = 0;
message.R.self = 0;
message.R.dlc_non_comp = 0;
message.R.flags = 0;
message.R.extd = 1;

    
    //xQueueSend(xQueueTXcan, &message, (15 / portTICK_PERIOD_MS) );

   twai_transmit(&message.L,  10);
   twai_transmit(&message.R,  10 );
}


void VESC_SET_Current(uint8_t EXIDL, float valueL,uint8_t EXIDR, float valueR){    // 0.0%  ---  100.0%  
    int32_t dudy_clycle_convL,dudy_clycle_convR; 
    struct vesccanARRYMSG message;

    dudy_clycle_convL = (int32_t)(1000 * valueL);

    message.L.identifier = EXIDL & 0x00FF;
    message.L.identifier = message.L.identifier | 0x0100;

    message.L.data_length_code = 4;
    message.L.data[3] = dudy_clycle_convL & 0x000000FF;
    message.L.data[2] = (dudy_clycle_convL & 0x0000FF00)>>8;
    message.L.data[1] = (dudy_clycle_convL & 0x00FF0000)>>16;
    message.L.data[0] = (dudy_clycle_convL & 0xFF000000)>>24;

message.L.rtr = 0;
message.L.ss = 0;
message.L.self = 0;
message.L.dlc_non_comp = 0;
message.L.flags = 0;
message.L.extd = 1;


    dudy_clycle_convR = (int32_t)(1000 * valueR);

    message.R.identifier = EXIDR & 0x00FF;
    message.R.identifier = message.R.identifier | 0x0100;

    message.R.data_length_code = 4;
    message.R.data[3] = dudy_clycle_convR & 0x000000FF;
    message.R.data[2] = (dudy_clycle_convR & 0x0000FF00)>>8;
    message.R.data[1] = (dudy_clycle_convR & 0x00FF0000)>>16;
    message.R.data[0] = (dudy_clycle_convR & 0xFF000000)>>24;
message.R.rtr = 0;
message.R.ss = 0;
message.R.self = 0;
message.R.dlc_non_comp = 0;
message.R.flags = 0;
message.R.extd = 1;

    
    //xQueueSend(xQueueTXcan, &message, (15 / portTICK_PERIOD_MS) );

   twai_transmit(&message.L,  10);
   twai_transmit(&message.R,  10 );
}



void VESC_SET_RPM(uint8_t EXIDL, int32_t valueL,uint8_t EXIDR, int32_t valueR){    // 500 = 500 RPM
    struct vesccanARRYMSG message;
    
    message.L.identifier = EXIDL & 0x00FF;
    message.L.identifier = message.L.identifier | 0x0300;

    message.L.data_length_code = 4;
    message.L.data[3] = valueL & 0x000000FF;
    message.L.data[2] = (valueL & 0x0000FF00)>>8;
    message.L.data[1] = (valueL & 0x00FF0000)>>16;
    message.L.data[0] = (valueL & 0xFF000000)>>24;

message.L.rtr = 0;
message.L.ss = 0;
message.L.self = 0;
message.L.dlc_non_comp = 0;
message.L.flags = 0;
message.L.extd = 1;


    message.R.identifier = EXIDR & 0x00FF;
    message.R.identifier = message.R.identifier | 0x0300;

    message.R.data_length_code = 4;
    message.R.data[3] = valueR & 0x000000FF;
    message.R.data[2] = (valueR & 0x0000FF00)>>8;
    message.R.data[1] = (valueR & 0x00FF0000)>>16;
    message.R.data[0] = (valueR & 0xFF000000)>>24;
message.R.rtr = 0;
message.R.ss = 0;
message.R.self = 0;
message.R.dlc_non_comp = 0;
message.R.flags = 0;
message.R.extd = 1;

    
    //xQueueSend(xQueueTXcan, &message, (15 / portTICK_PERIOD_MS) );

   twai_transmit(&message.L,  10);
   twai_transmit(&message.R,  10 );    
}






void VESC_setup(void)
{
    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_8, 0);

    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_18, GPIO_NUM_19, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS(); //TWAI_TIMING_CONFIG_250KBITS();
    
    //twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_filter_config_t f_config;
    f_config.acceptance_code = (5 << 3) ; //0x0005;
    //f_config.acceptance_mask = (0x1F00 << 3);
    f_config.acceptance_mask = (0x1F03 << 3);       // sia ID 5 che 6
    f_config.single_filter = true;


    //Install and start TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(TAGCAN, "Driver installed");

    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(TAGCAN, "Driver started");

    CLEAR_PERI_REG_MASK(USB_DEVICE_CONF0_REG, USB_DEVICE_USB_PAD_ENABLE);


    //xSemaphore = xSemaphoreCreateMutex();

    xQueueTXcan = xQueueCreate( 1, sizeof(ARRAY_twai_message) );

    xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, NULL, tskIDLE_PRIORITY, NULL, tskNO_AFFINITY);
    
    //xTaskCreatePinnedToCore(twai_transmit_task, "TWAI_tx", 4096, NULL, tskIDLE_PRIORITY +4, NULL, tskNO_AFFINITY);


}