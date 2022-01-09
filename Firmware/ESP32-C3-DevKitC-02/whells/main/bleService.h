
#ifndef __BLESERVICE_H__
#define  __BLESERVICE_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
#include "freertos/queue.h"
#include "freertos/event_groups.h"

void BLE_Service_Start(void);
void Send_BLE_Notification_Task(void *pvParameter);


QueueHandle_t BLE_SendNotification_Queue;
EventGroupHandle_t BLE_event_group;
#define BIT_0	( 1 << 0 )


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

#endif