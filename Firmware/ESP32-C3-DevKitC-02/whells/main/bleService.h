
#ifndef __BLESERVICE_H__
#define  __BLESERVICE_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"


#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"


void BLE_Service_Start(void);
void Send_BLE_Notification_Task(void *pvParameter);
void disconnected(esp_ble_gatts_cb_param_t *param);


//QueueHandle_t BLE_SendNotification_Queue;
MessageBufferHandle_t BLE_SendNotification_Queue;

EventGroupHandle_t BLE_event_group;
#define BIT_0	( 1 << 0 )
#define BIT_1	( 1 << 1 )
#define BIT_2	( 1 << 2 )


    /* Attributes State Machine */
    enum
    {
        IDX_SVC,
        IDX_CHAR_A,
        IDX_CHAR_VAL_A,
        IDX_CHAR_CFG_A,

        IDX_CHAR_B,
        IDX_CHAR_VAL_B,


        /*
        IDX_CHAR_C,
        IDX_CHAR_VAL_C,
        */
        IDX_CHAR_TELEMETRIA,        
        IDX_CHAR_VAL_TELEMETRIA,
        IDX_CHAR_CFG_TELEMETRIA,




        HRS_IDX_NB,
    };



/*
        ****BUTTON MAP
       MSB                                   LSB
        0    0    0    0       0    0    0    0  

                                         c    z 

                                       BOOST  FRENO






*/


    struct CHART_data_TX{
        int16_t uJoy_x; 
        int16_t uJoy_y;
        int16_t uvbattery;
        uint8_t buttons;
    }__attribute__((packed));


    //MAPPA JOYSTICK PULSANTI

    #define BUTTON_FRENO    0x01  
    #define BUTTON_TURBO    0x02  


    #define BUTTON_CURRENT_MODE    0x08  
    #define BUTTON_DUDY_MODE       0x04  
    #define BUTTON_RPM_MODE        0x10 


    #define BUTTON_MENO        0x20 
    #define BUTTON_PIU        0x40 




void connected(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

   // void BLE_Send_Data (void *pvParameter);








    struct JOYSTICK_BATTERY_DATA{
        //dati x far riconoscere al BLE che pacchetto è e che tipo
        uint8_t type_message;
     //   uint8_t len_message;
        int16_t Battery;
    }__attribute__((packed));









#endif