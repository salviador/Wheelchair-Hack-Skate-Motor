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



struct VESC_DATACAN VSC_DATA_Left;
struct VESC_DATACAN VSC_DATA_Right;
/*

#define RX_BUFFER_SIZE 100
uint8_t rx_bufferLEFT[RX_BUFFER_SIZE];
uint8_t rx_bufferRIGHT[RX_BUFFER_SIZE];



const unsigned short crc16_tab[] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084,
    0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad,
    0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7,
    0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a,
    0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672,
    0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719,
    0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7,
    0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948,
    0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50,
    0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b,
    0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97,
    0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe,
    0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca,
    0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3,
    0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d,
    0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214,
    0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c,
    0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3,
    0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d,
    0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806,
    0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e,
    0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1,
    0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b,
    0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0,
    0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 };

*/

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

//https://github.com/skipper762/teensy_VESC_CANBUS
//https://dongilc.gitbook.io/openrobot-inc/tutorials/control-with-can


//SUL VSC ABILITARE SOLO
//"CAN_STATUS_1" 1Hz


void twai_receive_task(void *arg)
{
    twai_message_t rx_message;

    uint8_t idvsc;
    uint16_t commandvsc;
    uint8_t dalalenght;

    VSC_DATA_Right.time_update = 0;
    VSC_DATA_Left.time_update = 0;


    while(1) {
        if(twai_receive(&rx_message,portMAX_DELAY) == ESP_OK) {
            commandvsc = rx_message.identifier & 0xFF00;

            //ESP_LOGI(TAGCAN, "commandvsc = %4x", commandvsc);
    
            idvsc = rx_message.identifier & 0x00FF;
            dalalenght = rx_message.data_length_code;

            //ESP_LOGI(TAGCAN, "idvsc = %i", idvsc);
            //ESP_LOGI(TAGCAN, "dalalenght = %i", dalalenght);

            switch(commandvsc){
                case 0x0900:
                    //RPM
                    if(dalalenght == 8){
                        if(idvsc == 5){
                            //https://dongilc.gitbook.io/openrobot-inc/tutorials/control-with-can
                            VSC_DATA_Left.RPM = 0;
                            VSC_DATA_Left.RPM = rx_message.data[0] << 24;
                            VSC_DATA_Left.RPM = VSC_DATA_Left.RPM | rx_message.data[1] << 16;
                            VSC_DATA_Left.RPM = VSC_DATA_Left.RPM | rx_message.data[2] << 8;
                            VSC_DATA_Left.RPM = VSC_DATA_Left.RPM | rx_message.data[3];

                            VSC_DATA_Left.time_update = xTaskGetTickCount();


                            xQueueOverwrite(vsc_data_logL, &VSC_DATA_Left);


                            //ESP_LOGI(TAGCAN, "RPM Spped = %i", VSC_DATA_Left.RPM);
                        }else if(idvsc == 6){
                            //https://dongilc.gitbook.io/openrobot-inc/tutorials/control-with-can
                            VSC_DATA_Right.RPM = 0;
                            VSC_DATA_Right.RPM = rx_message.data[0] << 24;
                            VSC_DATA_Right.RPM = VSC_DATA_Right.RPM | rx_message.data[1] << 16;
                            VSC_DATA_Right.RPM = VSC_DATA_Right.RPM | rx_message.data[2] << 8;
                            VSC_DATA_Right.RPM = VSC_DATA_Right.RPM | rx_message.data[3];

                            VSC_DATA_Right.time_update = xTaskGetTickCount();

                            xQueueOverwrite(vsc_data_logR, &VSC_DATA_Right);

                            //ESP_LOGI(TAGCAN, "RPM Spped = %i", VSC_DATA_Right.RPM);
                        }
                    }                
                break;
            }



        }
    }
}

/*
void twai_receive_task(void *arg)
{
    twai_message_t rx_message;
    uint8_t idvsc;
    uint16_t commandvsc;
    struct VESC_DATACAN VSC_DATA_Left;
    struct VESC_DATACAN VSC_DATA_Right;
    uint8_t dataindice,dalalenght;
    int i;
    uint8_t crc_h, crc_l;
    uint16_t crc_16,crc_16calcolato;
    

    while(1) {
        if(twai_receive(&rx_message,portMAX_DELAY) == ESP_OK) {
            commandvsc = rx_message.identifier & 0x0F00;

            ESP_LOGI(TAGCAN, "commandvsc = %4x", commandvsc);

            if(commandvsc > 1024){
                idvsc = rx_message.identifier & 0x00FF;
                dalalenght = rx_message.data_length_code;

                ESP_LOGI(TAGCAN, "idvsc = %i", idvsc);
                ESP_LOGI(TAGCAN, "dalalenght = %i", dalalenght);


                switch(commandvsc){
                    case 0x0500:
                        if(dalalenght == 8){
                            //Colleziona dati
                            dataindice = rx_message.data[0];
                            if(dataindice > 0){
                                dataindice = dataindice + 1;
                            }

                            ESP_LOGI(TAGCAN, "dataindice = %2x", dataindice);

                            if(idvsc == 5){
                                //Left
                                for(i = 0; i < rx_message.data_length_code; i++){            
                                    rx_bufferLEFT[dataindice] = rx_message.data[i];
                                    dataindice++;
                                }
                            }else if(idvsc == 6){
                                //Right
                                for(i = 0; i < rx_message.data_length_code; i++){            
                                    rx_bufferRIGHT[dataindice] = rx_message.data[i];
                                    dataindice++;
                                }
                            }
                        }
                    break;
                    case 0x0600:
                        //Indice pacchetti                        
                            if(idvsc == 5){
                                //Left
                                rx_bufferLEFT[0x40] = rx_message.data[0];
                                rx_bufferLEFT[0x41] = rx_message.data[1];

                            }else if(idvsc == 6){
                                //Right
                                rx_bufferRIGHT[0x40] = rx_message.data[0];
                                rx_bufferRIGHT[0x41] = rx_message.data[1];
                            }
                    break;
                    case 0x0700:
                        //Fine invio pacchetto
                        if(rx_message.data[1]==0x01){

                            crc_h = rx_message.data[4];
                            crc_l = rx_message.data[5];
                            crc_16 = 0;
                            crc_16 = crc_h << 8;
                            crc_16 = crc_16 | crc_l;

                            if(idvsc == 5){
                                //Left
                                crc_16calcolato = crc16(rx_bufferLEFT, 0x42);

                                ESP_LOGI(TAGCAN, "crc_16calcolato = %i  ,  crc_16 = %i", crc_16calcolato,crc_16);


                                if(crc_16 == crc_16calcolato){
                                        //PROCESS BUFFERRRRRRR!!!!
                                    ESP_LOGI(TAGCAN, "Processsss Buffer x LEFT data");

                                }
//https://github.com/skipper762/teensy_VESC_CANBUS/blob/master/bldc_interface.c
//bldc_interface_process_packet

                            }else if(idvsc == 6){
                                //Right
                                crc_16calcolato = crc16(rx_bufferLEFT, 0x42);

                                ESP_LOGI(TAGCAN, "crc_16calcolato = %i  ,  crc_16 = %i", crc_16calcolato,crc_16);

                                if(crc_16 == crc_16calcolato){
                                        //PROCESS BUFFERRRRRRR!!!!
                                    ESP_LOGI(TAGCAN, "Processsss Buffer x RIGHT data");

                                }
                            }

                        }                    
                    break;
                    default:
                    break;
                }

            }

        }
    }
}
*/





void twai_get_data_task(void *arg)
{
    uint8_t idrequest = 0;


    while(1){
        switch(idrequest){
            case 0:
                
                ESP_LOGI(TAGCAN, "CAN REQUEST DAFA");

                twai_requestDATA_LeftMotor();
                idrequest = 1;
            break;

            case 1:
                //twai_requestDATA_RightMotor();
                idrequest = 0;
            break;
        }
        //vTaskDelay(250 / portTICK_PERIOD_MS);

        //SOLO x Debug
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}






void twai_requestDATA_LeftMotor(void){
        //Request data
    twai_message_t rqmessage;
    uint8_t idcan;
    idcan = 5;

    rqmessage.identifier = idcan & 0x00FF;
    rqmessage.identifier = rqmessage.identifier | 0x0800;
    
    rqmessage.data_length_code = 3;
    rqmessage.data[2] = 4;
    rqmessage.data[1] = 0;
    rqmessage.data[0] = 1;

    rqmessage.rtr = 0;
    rqmessage.ss = 0;
    rqmessage.self = 0;
    rqmessage.dlc_non_comp = 0;
    rqmessage.flags = 0;
    rqmessage.extd = 1;

    //Queue message for transmission
    twai_transmit(&rqmessage, 10);
    //vTaskDelay(2000 / portTICK_PERIOD_MS);
}
void twai_requestDATA_RightMotor(void){
        //Request data
    twai_message_t rqmessage;
    uint8_t idcan;
    idcan = 6;

    rqmessage.identifier = idcan & 0x00FF;
    rqmessage.identifier = rqmessage.identifier | 0x0800;
    
    rqmessage.data_length_code = 3;
    rqmessage.data[2] = 4;
    rqmessage.data[1] = 0;
    rqmessage.data[0] = 1;

    rqmessage.rtr = 0;
    rqmessage.ss = 0;
    rqmessage.self = 0;
    rqmessage.dlc_non_comp = 0;
    rqmessage.flags = 0;
    rqmessage.extd = 1;

    //Queue message for transmission
    twai_transmit(&rqmessage, 10);
    //vTaskDelay(2000 / portTICK_PERIOD_MS);
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
    //twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_18, GPIO_NUM_19, TWAI_MODE_NORMAL);

    twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = GPIO_NUM_18,
        .rx_io = GPIO_NUM_19,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 5,
        .rx_queue_len = 20,
        .alerts_enabled = TWAI_ALERT_NONE,
        .clkout_divider = 0,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };




    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS(); //TWAI_TIMING_CONFIG_250KBITS();
    
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    //twai_filter_config_t f_config;
    
    //0x400
    /*
        101 00000000
        110 00000000
        111 00000000
    */
    //f_config.acceptance_code = (5 << 21) ; //0x0005;


    //f_config.acceptance_code = (5 << 3) ; //0x0005;
    //f_config.acceptance_mask = (0x1F00 << 3);
    //f_config.acceptance_mask = (0x1F03 << 3);       // sia ID 5 che 6
    //f_config.single_filter = true;


    //Install and start TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(TAGCAN, "Driver installed");

    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(TAGCAN, "Driver started");

    CLEAR_PERI_REG_MASK(USB_DEVICE_CONF0_REG, USB_DEVICE_USB_PAD_ENABLE);


    //xSemaphore = xSemaphoreCreateMutex();

    xQueueTXcan = xQueueCreate( 1, sizeof(ARRAY_twai_message) );

    vsc_data_logL = xQueueCreate( 1, sizeof(VSC_DATA_Left) );
    vsc_data_logR = xQueueCreate( 1, sizeof(VSC_DATA_Right) );

    xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, NULL, tskIDLE_PRIORITY, NULL, tskNO_AFFINITY);
    
    //xTaskCreatePinnedToCore(twai_get_data_task, "TWAI_tx", 4096, NULL, tskIDLE_PRIORITY + 3, NULL, tskNO_AFFINITY);

 




}







/*
unsigned short crc16(unsigned char *buf, unsigned int len) {
  unsigned int i;
  unsigned short cksum = 0;
  for (i = 0; i < len; i++) {
    cksum = crc16_tab[(((cksum >> 8) ^ *buf++) & 0xFF)] ^ (cksum << 8);
  }
  return cksum;
}*/