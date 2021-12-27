

#ifndef __WHEELS_H__
#define  __WHEELS_H__

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
    #include "bleService.h"

    #define EXID_LEFT 5
    #define EXID_RIGHT 6



    typedef enum {
        M_IDLE = 0,
        SELECT_MODALITA_JOY, 
        M_JOY_SECURITY,
        M_START,

        M_DUTYCYCLE_MODE,
        M_CURRENT_MODE,
        M_RPM_MODE,


    } MOTOR_STATE_ENUM;




    struct WHEEL_VAR{
        uint8_t joy_security; 
        TickType_t time_Start;

        //Joy var
        int16_t joyX; 
        int16_t joyY; 

        uint8_t FREE_RUN_REPEAT;
        uint8_t FREE_RUN_REPEAT_TIMEOUT;

        float vescValueLeft;
        float vescValueright;
        int16_t ivescValueLeft;
        int16_t ivescValueright;

    };



    struct LINEAR_ACC_VAR{
        float vsec_acc_right;
        float vsec_acc_left;
        float vescValueLeft;
        float vescValueright;


        int16_t ivsec_acc_right;
        int16_t ivsec_acc_left;
        int16_t ivescValueLeft;
        int16_t ivescValueright;

    };








    QueueHandle_t xQueueJOYData;

    void wheels_setup(void);

    void wheels_task(void *arg);

    void Motor_Free(void);

    void Motor_Convert_Joy(struct CHART_data_TX *bleJOY, struct WHEEL_VAR *wheelJOY);

    float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
    long map(long x, long in_min, long in_max, long out_min, long out_max);
    
    void linear_acceleration(struct LINEAR_ACC_VAR *var, struct WHEEL_VAR *wheelvar, float linear_constant );
    void linear_acceleration_var_reset(struct LINEAR_ACC_VAR *var);

    void linear_accelerationINT(struct LINEAR_ACC_VAR *var, struct WHEEL_VAR *wheelvar, int16_t linear_constant );
    void linear_acceleration_var_resetINT(struct LINEAR_ACC_VAR *var);


#endif

