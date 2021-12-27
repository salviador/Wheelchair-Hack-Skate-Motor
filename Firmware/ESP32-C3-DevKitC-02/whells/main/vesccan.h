

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


#endif

