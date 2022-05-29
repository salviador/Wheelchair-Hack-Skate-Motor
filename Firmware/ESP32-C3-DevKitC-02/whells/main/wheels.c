#include "wheels.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_types.h>
#include "esp_intr.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_log.h"

#include "vesccan.h"
#include "bleService.h"
#include "DifferentialSteering.h"
#include "buzzer.h"

#include "debugGPIO.h"
#include "freertos/semphr.h"




SemaphoreHandle_t xSemaphoreWHEELS;

struct BUZZER_queue BuzMsg;


struct CHART_data_TX Joystick_rec;
struct WHEEL_VAR wheel_var;
struct LINEAR_ACC_VAR linear_acce;



uint8_t MOTOR_STATE;
uint8_t tempMOTORSTATE;

static const char *TAGWHEELS = "WHEELS";


void wheels_setup(void){

    xQueueJOYData = xQueueCreate( 1, sizeof( Joystick_rec ) );
    DifferentialSteering_setup(10);
    MOTOR_STATE = M_IDLE;
    wheel_var.joy_security = 1;
    wheel_var.time_Start = xTaskGetTickCount();
    Motor_Free();

    xSemaphoreWHEELS = xSemaphoreCreateMutex();

    //Reset Var ACCELERATION
    linear_acceleration_var_reset(&linear_acce);
    

    wheel_var.FREE_RUN_REPEAT = 3;
    wheel_var.FREE_RUN_REPEAT_TIMEOUT = 5 ;
}


void wheels_task(void *arg){
    //Code x sedia 

    while (1)
    {
        //Leggi Joystick
    
        //if (xQueueReceive(xQueueJOYData, &Joystick_rec, portMAX_DELAY) == pdPASS ) {
        if (xQueueReceive(xQueueJOYData, &Joystick_rec, (300/portTICK_PERIOD_MS) ) == pdPASS ) {
            wheel_var.FREE_RUN_REPEAT_TIMEOUT = 5 ;
                
            //if( xSemaphoreTake( xSemaphoreWHEELS, (1 / portTICK_PERIOD_MS) ) == pdTRUE ){
            if( xSemaphoreTake( xSemaphoreWHEELS, 0 ) == pdTRUE ){


                //Motor_Convert_Joy(&Joystick_rec, &wheel_var);   //+1000 a -1000
                wheel_var.joyX = Joystick_rec.uJoy_x;  //-Joystick_rec.uJoy_x;
                wheel_var.joyY = Joystick_rec.uJoy_y;

                //ESP_LOGI(TAGWHEELS, "%d", Joystick_rec.uvbattery);

                //ESP_LOGI(TAGWHEELS, "1--= %d,%d, %x",  wheel_var.joyX , wheel_var.joyY, Joystick_rec.buttons );
                /*
                ESP_LOGI(TAGWHEELS, "%d,%d,%d,%d, %d,%d,%d,%d",  
                                                                (Joystick_rec.buttons & 0x80),
                                                                (Joystick_rec.buttons & 0x40),
                                                                (Joystick_rec.buttons & 0x20),
                                                                (Joystick_rec.buttons & 0x10),
                                                                
                                                                (Joystick_rec.buttons & 0x08),
                                                                (Joystick_rec.buttons & 0x04),
                                                                (Joystick_rec.buttons & 0x02),
                                                                (Joystick_rec.buttons & 0x01) );
                */

                DifferentialSteering_computeMotors(wheel_var.joyX, wheel_var.joyY);
                // The output range will be [-1000, 1000]
                int16_t leftMotor = DifferentialSteering_computedLeftMotor();
                int16_t rightMotor = DifferentialSteering_computedRightMotor();

                //ESP_LOGI(TAGWHEELS, "1--= %d,%d",  leftMotor, rightMotor);



                //----------Freno Priorità assoluta !!!!----------
                    if(Joystick_rec.buttons & BUTTON_FRENO){

                        ESP_LOGI(TAGWHEELS, "[MODE]...Freno");


                        float vescValueLeftBRK;
                        float vescValuerightBRK;
                        
                        if(leftMotor >=0)
                            vescValueLeftBRK = mapfloat((float)leftMotor, 1000.0 , 0.000, 1.100, 40.0);
                        else
                            vescValueLeftBRK = mapfloat((float)leftMotor, -1000.0 , 0.000, 1.100, 40.0);
                            
                        if(rightMotor >=0)
                            vescValuerightBRK = mapfloat((float)rightMotor, 1000.0 , 0.000, 1.100, 40.0);
                        else
                            vescValuerightBRK = mapfloat((float)rightMotor, -1000.0 , 0.000, 1.100, 40.0);

                        //DECELERATION


                        ESP_LOGI("FRENO", "%f, %f", vescValueLeftBRK , vescValuerightBRK);


                        //Send VESC
                        VESC_SET_CurrentBREAK(EXID_LEFT, vescValueLeftBRK, EXID_RIGHT, vescValuerightBRK);

                        wheel_var.FREE_RUN_REPEAT = 3;

                        //Reset Var ACCELERATION
                        linear_acceleration_var_reset(&linear_acce);

                    }else{

                        //----------Normal Mode !!!!----------

                        switch(MOTOR_STATE){
                            case M_IDLE:
                                //buzzer_beep(2, 150);
                                /*
                                BuzMsg.state = 1;
                                BuzMsg.timeWaitON = 250;
                                BuzMsg.timeWaitOFF = 150;
                                xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
                                */
                                wheel_var.joy_security = 1;
                                wheel_var.time_Start = xTaskGetTickCount();
                                Motor_Free();
                                tempMOTORSTATE = M_DUTYCYCLE_MODE;

                                MOTOR_STATE = SELECT_MODALITA_JOY;
                            break;

                            case SELECT_MODALITA_JOY:

                                tempMOTORSTATE = M_DUTYCYCLE_MODE;

                                //Leggi JOY x cambiare modalità
                                if(wheel_var.joyX < -150) {
                                    tempMOTORSTATE = M_CURRENT_MODE;
                                    Buzzer_CurrentMode();
                                }else if(wheel_var.joyX > 150) {
                                    tempMOTORSTATE = M_RPM_MODE;
                                    Buzzer_RPMMode();
                                }else{
                                    Buzzer_DutyMode();
                                }

                                MOTOR_STATE = M_JOY_SECURITY;
                            break;

                            case M_JOY_SECURITY:
                                //Aspetta il JOY in Stato di sicurezza
                                //Reset Var ACCELERATION
                                linear_acceleration_var_reset(&linear_acce);
                                linear_acceleration_var_resetINT(&linear_acce);

                                if( (wheel_var.joyX > 100) | (wheel_var.joyX < -100) | (wheel_var.joyY > 100) | (wheel_var.joyY < -100) ){
                                    wheel_var.time_Start = xTaskGetTickCount();
                                    ESP_LOGI(TAGWHEELS, ".....JOY non IN SICUREZZA...%d,%d...",wheel_var.joyX , wheel_var.joyY);


                                }else if((pdTICKS_TO_MS(xTaskGetTickCount() - wheel_var.time_Start)) > 1000){
                                    MOTOR_STATE = tempMOTORSTATE; //M_DUTYCYCLE_MODE;

                                    ESP_LOGI(TAGWHEELS, "STATO= %d", MOTOR_STATE);

                                    wheel_var.FREE_RUN_REPEAT = 3;

/*                                    BuzMsg.state = 1;
                                    BuzMsg.timeWaitON = 150;
                                    BuzMsg.timeWaitOFF = 150;
                                    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
                                    BuzMsg.state = 1;
                                    BuzMsg.timeWaitON = 150;
                                    BuzMsg.timeWaitOFF = 150;
                                    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
*/                                    
                                }
                            break;

                            case M_START:
                                    ESP_LOGI(TAGWHEELS, "DA FARE..........");
                            break;


                            case M_DUTYCYCLE_MODE:
                                //ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle");
                                if(Joystick_rec.buttons & BUTTON_CURRENT_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;                                    
                                    tempMOTORSTATE = M_CURRENT_MODE;
                                    Buzzer_CurrentMode();
                                    //reset acceleration
                                    linear_acceleration_var_reset(&linear_acce);
                                }else if(Joystick_rec.buttons & BUTTON_RPM_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;
                                    tempMOTORSTATE = M_RPM_MODE;
                                    Buzzer_RPMMode();
                                    linear_acceleration_var_resetINT(&linear_acce);
                                }


                                if(Joystick_rec.buttons & BUTTON_TURBO){
                                    //HARD MODE
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -0.80, 0.80);
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -0.80, 0.80);
                                }else{
                                    //SOFT MODE
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -0.15, 0.15); //0.20
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -0.15, 0.15);
                                    //wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -0.15, 0.15);
                                    //wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -0.15, 0.15);
                                    /*
                                    if((leftMotor < 0)&(rightMotor < 0)){
                                        float tempstr;
                                        tempstr = wheel_var.vescValueLeft;
                                        wheel_var.vescValueLeft = wheel_var.vescValueright;
                                        wheel_var.vescValueright = tempstr;
                                    }
                                    */
                                }
                        
                                //Acceleration
                                //https://github.com/adafruit/AccelStepper/blob/master/AccelStepper.cpp
                                //float AccelStepper::desiredSpeed()

                                //Calc Linear ACCELERATION
                                linear_acceleration(&linear_acce, &wheel_var, 0.05 );


                                //Motor
                                if( (wheel_var.vescValueLeft < 0.005) & (wheel_var.vescValueLeft > -0.005)  & (wheel_var.vescValueright < 0.005) & (wheel_var.vescValueright > -0.005) ){
                                    if(wheel_var.FREE_RUN_REPEAT > 0){
                                        ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle free run %f , %f ",  wheel_var.vescValueLeft,wheel_var.vescValueright);
                                        Motor_Free();
                                        wheel_var.FREE_RUN_REPEAT = wheel_var.FREE_RUN_REPEAT - 1;

                                        linear_acceleration_var_reset(&linear_acce);
                                    }
                                }else{
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle %f , %f ",  wheel_var.vescValueLeft,wheel_var.vescValueright);
                                    VESC_SET_DutyCycle(EXID_LEFT, linear_acce.vsec_acc_left, EXID_RIGHT, linear_acce.vsec_acc_right);
                                    wheel_var.FREE_RUN_REPEAT = 3;
                                }
                            break;



                            case M_CURRENT_MODE:
                                //ESP_LOGI(TAGWHEELS, "[MODE]...Current Mode");
                                if(Joystick_rec.buttons & BUTTON_DUDY_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;                                    
                                    tempMOTORSTATE = M_DUTYCYCLE_MODE;
                                    Buzzer_DutyMode();
                                    //reset acceleration
                                    linear_acceleration_var_reset(&linear_acce);
                                }else if(Joystick_rec.buttons & BUTTON_RPM_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;
                                    tempMOTORSTATE = M_RPM_MODE;
                                    Buzzer_RPMMode();
                                    linear_acceleration_var_resetINT(&linear_acce);
                                }

                                if(Joystick_rec.buttons & BUTTON_TURBO){
                                    //HARD MODE
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -20.00, 20.00);
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -20.00, 20.00);
                                }else{
                                    //SOFT MODE
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -5.00, 5.00);
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -5.00, 5.00);
                                }
                                /*
                                if((leftMotor < 0)&(rightMotor < 0)){
                                    float tempstr;
                                    tempstr = wheel_var.vescValueLeft;
                                    wheel_var.vescValueLeft = wheel_var.vescValueright;
                                    wheel_var.vescValueright = tempstr;
                                }
                                */

                                //Acceleration
                                //https://github.com/adafruit/AccelStepper/blob/master/AccelStepper.cpp
                                //float AccelStepper::desiredSpeed()

                                //Calc Linear ACCELERATION
                                linear_acceleration(&linear_acce, &wheel_var, 0.85 );


                                //Motor
                                if( (wheel_var.vescValueLeft < 0.04) & (wheel_var.vescValueLeft > -0.04)  & (wheel_var.vescValueright < 0.04) & (wheel_var.vescValueright > -0.04) ){
                                    if(wheel_var.FREE_RUN_REPEAT > 0){
                                        ESP_LOGI(TAGWHEELS, "[MODE]...CURRENT free run %f , %f ",  wheel_var.vescValueLeft,wheel_var.vescValueright);

                                        Motor_Free();
                                        wheel_var.FREE_RUN_REPEAT = wheel_var.FREE_RUN_REPEAT - 1;

                                        linear_acceleration_var_reset(&linear_acce);
                                    }
                                }else{
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...CURRENT %f , %f ",  wheel_var.vescValueLeft,wheel_var.vescValueright);

                                    VESC_SET_Current(EXID_LEFT, linear_acce.vsec_acc_left, EXID_RIGHT, linear_acce.vsec_acc_right);
                                    wheel_var.FREE_RUN_REPEAT = 3;
                                }
                            break;

                            case M_RPM_MODE:
                                //ESP_LOGI(TAGWHEELS, "[MODE]...Current Mode");
                                if(Joystick_rec.buttons & BUTTON_DUDY_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;                                    
                                    tempMOTORSTATE = M_DUTYCYCLE_MODE;
                                    Buzzer_DutyMode();
                                    //reset acceleration
                                    linear_acceleration_var_reset(&linear_acce);
                                }else if(Joystick_rec.buttons & BUTTON_CURRENT_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;
                                    tempMOTORSTATE = M_CURRENT_MODE;
                                    Buzzer_CurrentMode();
                                    linear_acceleration_var_reset(&linear_acce);
                                }
                                //SELCET HARD MODE or SOFT MODE ?????????????

                                if(Joystick_rec.buttons & BUTTON_TURBO){
                                    //HARD MODE
                                    wheel_var.ivescValueLeft = map(leftMotor, -1000, 1000, -2500, 2500);
                                    wheel_var.ivescValueright = map(rightMotor, -1000, 1000, -2500, 2500);
                                }else{
                                    //SOFT MODE
                                    wheel_var.ivescValueLeft = map(leftMotor, -1000, 1000, -1500, 1500);  //700
                                    wheel_var.ivescValueright = map(rightMotor, -1000, 1000, -1500, 1500);
                                }
                                /*
                                if((leftMotor < 0)&(rightMotor < 0)){
                                    int16_t tempstrint;
                                    tempstrint = wheel_var.ivescValueLeft;
                                    wheel_var.ivescValueLeft = wheel_var.ivescValueright;
                                    wheel_var.ivescValueright = tempstrint;
                                }
                                */


                                //Acceleration
                                //https://github.com/adafruit/AccelStepper/blob/master/AccelStepper.cpp
                                //float AccelStepper::desiredSpeed()

                                //Calc Linear ACCELERATION
                                linear_accelerationINT(&linear_acce, &wheel_var, 200);

                                //Motor
                                if( (wheel_var.ivescValueLeft < 50) & (wheel_var.ivescValueLeft > -50)  & (wheel_var.ivescValueright < 50) & (wheel_var.ivescValueright > -50) ){
                                    if(wheel_var.FREE_RUN_REPEAT > 0){
                                        ESP_LOGI(TAGWHEELS, "[MODE]...RPM free run %d , %d ",  wheel_var.vescValueLeft,wheel_var.vescValueright);

                                        Motor_Free();
                                        wheel_var.FREE_RUN_REPEAT = wheel_var.FREE_RUN_REPEAT - 1;

                                        linear_acceleration_var_resetINT(&linear_acce);
                                    }
                                }else{
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...RPM %d , %d ",  linear_acce.ivsec_acc_left, linear_acce.ivsec_acc_right);

                                    VESC_SET_RPM(EXID_LEFT, linear_acce.ivsec_acc_left, EXID_RIGHT, linear_acce.ivsec_acc_right);
                                    wheel_var.FREE_RUN_REPEAT = 3;
                                }
                    
                            break;








                        }//switch



                    }//FINE FRENO else NORMAL MODE


                xSemaphoreGive( xSemaphoreWHEELS );

            }//SEMAFORO




        }else{
            if(wheel_var.FREE_RUN_REPEAT_TIMEOUT > 0){
                linear_acceleration_var_reset(&linear_acce);
                Motor_Free();
                wheel_var.FREE_RUN_REPEAT_TIMEOUT = wheel_var.FREE_RUN_REPEAT_TIMEOUT - 1;
                ESP_LOGI(TAGWHEELS, " t i m e  o u t ");

                MOTOR_STATE = M_IDLE;
                wheel_var.joy_security = 1;
            }
            //TIMEOUT        
            //Motor_Free();

        }//FINE QUEEQUE 




    }
 }




/*
void Motor_Convert_Joy( struct CHART_data_TX *bleJOY,  struct WHEEL_VAR *wheelJOY){

    ESP_LOGI(TAGWHEELS, "1-ORIG= %d,%d",  bleJOY->uJoy_x, bleJOY->uJoy_y);
    //OFFSET
    bleJOY->uvbattery = bleJOY->uvbattery - 100;

    //((joy / batt) * 2000) - 1000              //--> ( +1000   0  -1000 )
    wheelJOY->joyX = (int16_t)((       ((float)((float)bleJOY->uJoy_x / (float)bleJOY->uvbattery) * 2000)) - 1000);
    wheelJOY->joyY = (int16_t)((       ((float)((float)bleJOY->uJoy_y / (float)bleJOY->uvbattery) * 2000)) - 1000);

    //ADJUST OFFSET
    wheelJOY->joyX = wheelJOY->joyX + 21;
    wheelJOY->joyY = wheelJOY->joyY + 221;

    ESP_LOGI(TAGWHEELS, "2-MOMOD= %d,%d",  wheelJOY->joyX, wheelJOY->joyY);

}
*/



long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

 void Motor_Free(void){
     //SET_CURRENT 0,00
    VESC_SET_Current(EXID_LEFT, 0.0000, EXID_RIGHT, 0.0000);
 }


 void linear_acceleration(struct LINEAR_ACC_VAR *var, struct WHEEL_VAR *wheelvar, float linear_constant ){
     if(var->vsec_acc_right >  wheelvar->vescValueright){
        var->vsec_acc_right = var->vsec_acc_right - linear_constant;
        if(var->vsec_acc_right < wheelvar->vescValueright) 
            var->vsec_acc_right = wheelvar->vescValueright;
     }
     if(var->vsec_acc_right <  wheelvar->vescValueright){
        var->vsec_acc_right = var->vsec_acc_right + linear_constant;
        if(var->vsec_acc_right > wheelvar->vescValueright) 
            var->vsec_acc_right = wheelvar->vescValueright;
     }

     if(var->vsec_acc_left >  wheelvar->vescValueLeft){
        var->vsec_acc_left = var->vsec_acc_left - linear_constant;
        if(var->vsec_acc_left < wheelvar->vescValueLeft) 
            var->vsec_acc_left = wheelvar->vescValueLeft;
     }
     if(var->vsec_acc_left <  wheelvar->vescValueLeft){
        var->vsec_acc_left = var->vsec_acc_left + linear_constant;
        if(var->vsec_acc_left > wheelvar->vescValueLeft) 
            var->vsec_acc_left = wheelvar->vescValueLeft;
     }
 }

void linear_acceleration_var_reset(struct LINEAR_ACC_VAR *var){
    var->vsec_acc_right = 0.0;
    var->vsec_acc_left = 0.0;
    var->vescValueLeft = 0.0;
    var->vescValueright = 0.0;
}



 void linear_accelerationINT(struct LINEAR_ACC_VAR *var, struct WHEEL_VAR *wheelvar, int16_t linear_constant ){
     if(var->ivsec_acc_right >  wheelvar->ivescValueright){
        var->ivsec_acc_right = var->ivsec_acc_right - linear_constant;
        if(var->ivsec_acc_right < wheelvar->ivescValueright) 
            var->ivsec_acc_right = wheelvar->ivescValueright;
     }
     if(var->ivsec_acc_right <  wheelvar->ivescValueright){
        var->ivsec_acc_right = var->ivsec_acc_right + linear_constant;
        if(var->ivsec_acc_right > wheelvar->ivescValueright) 
            var->ivsec_acc_right = wheelvar->ivescValueright;
     }

     if(var->ivsec_acc_left >  wheelvar->ivescValueLeft){
        var->ivsec_acc_left = var->ivsec_acc_left - linear_constant;
        if(var->ivsec_acc_left < wheelvar->ivescValueLeft) 
            var->ivsec_acc_left = wheelvar->ivescValueLeft;
     }
     if(var->ivsec_acc_left <  wheelvar->ivescValueLeft){
        var->ivsec_acc_left = var->ivsec_acc_left + linear_constant;
        if(var->ivsec_acc_left > wheelvar->ivescValueLeft) 
            var->ivsec_acc_left = wheelvar->ivescValueLeft;
     }
 }

void linear_acceleration_var_resetINT(struct LINEAR_ACC_VAR *var){
    var->ivsec_acc_right = 0;
    var->ivsec_acc_left = 0;
    var->ivescValueLeft = 0;
    var->ivescValueright = 0;
}





void Buzzer_CurrentMode(void){
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 200;
    BuzMsg.timeWaitOFF = 200;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
/*    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 150;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 450;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 450;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 150;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
*/
}

void Buzzer_RPMMode(void){
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 200;
    BuzMsg.timeWaitOFF = 200;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
  /*
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 150;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 150;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 350;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
*/
}


void Buzzer_DutyMode(void){
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 200;
    BuzMsg.timeWaitOFF = 200;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
/*
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 150;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 150;
    BuzMsg.timeWaitOFF = 150;
    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);  
*/
}