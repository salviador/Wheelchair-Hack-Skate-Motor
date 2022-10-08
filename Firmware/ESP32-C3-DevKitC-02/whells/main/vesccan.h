

#ifndef __VESCCAN_H__
#define  __VESCCAN_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "driver/twai.h"
    #include "freertos/queue.h"

    QueueHandle_t xQueueTXcan;

    void VESC_setup(void);
    void twai_transmit_task(void *arg);
    void twai_receive_task(void *arg);

    void VESC_SET_DutyCycle(uint8_t EXIDL, float valueL,uint8_t EXIDR, float valueR);      // 0.0%  ---  100.0%  
    void VESC_SET_CurrentBREAK(uint8_t EXIDL, float valueL,uint8_t EXIDR, float valueR);    // 0.0%  ---  100.0%  
    void VESC_SET_Current(uint8_t EXIDL, float valueL,uint8_t EXIDR, float valueR);    // 0.0%  ---  100.0%  
    void VESC_SET_RPM(uint8_t EXIDL, int32_t valueL,uint8_t EXIDR, int32_t valueR);    // 500 = 500 RPM


    void twai_requestDATA_LeftMotor(void);
    void twai_requestDATA_RightMotor(void);



    void twai_get_data_task(void *arg);

    QueueHandle_t vsc_data_logL;
    QueueHandle_t vsc_data_logR;
    
    
    struct VESC_DATACAN{
        int16_t RPM;

        //uint8_t Voltage; 
        unsigned long time_update;
    };




    //unsigned short crc16(unsigned char *buf, unsigned int len);
 





    struct VESC_DATACAN_CAN_PACKET_STATUS_1{
        //dati x far riconoscere al BLE che pacchetto è e che tipo
        uint8_t type_message;
     //   uint8_t len_message;

        //
        int16_t RPM;
        float Current; 
        float Duty_Motor; 
    }__attribute__((packed));
    struct VESC_DATACAN_CAN_PACKET_STATUS_2{
        //dati x far riconoscere al BLE che pacchetto è e che tipo
        uint8_t type_message;
     //   uint8_t len_message;

        float Amp_Hours; 
    }__attribute__((packed));
    struct VESC_DATACAN_CAN_PACKET_STATUS_4{
        //dati x far riconoscere al BLE che pacchetto è e che tipo
        uint8_t type_message;
      //  uint8_t len_message;

        float temperature_FET; 
        float temperature_MOTOR; 
        float Current_IN; 
    }__attribute__((packed));
    struct VESC_DATACAN_CAN_PACKET_STATUS_5{
        //dati x far riconoscere al BLE che pacchetto è e che tipo
        uint8_t type_message;
     //   uint8_t len_message;

        int32_t Tachimetro;
        float Vbattery; 
    }__attribute__((packed));
















#endif

