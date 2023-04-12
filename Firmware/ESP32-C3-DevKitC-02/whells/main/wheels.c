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
/*
//DEBUG********TIME
    unsigned long time_debug_1 = 0;
    unsigned long time_debug_now = 0;
                                    time_debug_now = xTaskGetTickCount();
                                    ESP_LOGI(TAGWHEELS, "Time ====  %i", pdTICKS_TO_MS(time_debug_now - time_debug_1));
                                    time_debug_1 = xTaskGetTickCount();
//DEBUG********
*/


SemaphoreHandle_t xSemaphoreWHEELS;

struct BUZZER_queue BuzMsg;


struct CHART_data_TX Joystick_rec;
struct WHEEL_VAR wheel_var;
struct LINEAR_ACC_VAR linear_acce;


uint8_t Button_piu_meno_relase = 0;
float duty_soft_var = 0.20;
//float current_soft_var = 5.00;
struct CURRENT_MODE_DATA cuurent_mode_data;
uint8_t current_guida_auto = 0;
unsigned long time_Current_presslong_button; // = xTaskGetTickCount();
uint8_t Button_piu_longpress_relase = 0;

int current_diff_sterzing_auto = 0;
int current_media_spped = 0;
int current_set_speed = 0;
int current_sterzing_max = 0;


uint8_t rpm_guida_auto = 0;
unsigned long time_RPM_auto; // = xTaskGetTickCount();
int rpm_media_spped = 0;
int rpm_set_speed = 0;
int rpm_diff_sterzing_auto = 0;
int rpm_sterzing_max = 0;
int intRoundfloat = 0;
int rpm_max_setting_spped;

uint8_t MOTOR_STATE;
uint8_t tempMOTORSTATE;

static const char *TAGWHEELS = "WHEELS";

extern struct VESC_DATACAN VSC_DATA_Left;
extern struct VESC_DATACAN VSC_DATA_Right;





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
    
    Button_piu_meno_relase = 0;
    duty_soft_var = 0.20;
    cuurent_mode_data.current_soft_var = 5.00;
    cuurent_mode_data.SPUNTO_PARTENZA_H = 1350;
    cuurent_mode_data.SPUNTO_PARTENZA_L = 600;  //300
    cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI = cuurent_mode_data.SPUNTO_PARTENZA_H;
    current_guida_auto = 0;
    current_diff_sterzing_auto = 0;
    current_media_spped = 0;
    current_set_speed = 0;
    current_sterzing_max = 0;

    rpm_guida_auto = 0;
    time_RPM_auto = xTaskGetTickCount();
    rpm_media_spped = 0;
    rpm_set_speed = 0;
    rpm_diff_sterzing_auto = 0;
    rpm_sterzing_max = 0;

    wheel_var.FREE_RUN_REPEAT = 3;
    wheel_var.FREE_RUN_REPEAT_TIMEOUT = 5 ;
}

long timeTDBGelaspe=0;


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
                
                /*
                long timeTDBGelaspestop = xTaskGetTickCount() - timeTDBGelaspe;               
                timeTDBGelaspe = xTaskGetTickCount();

                ESP_LOGI(TAGWHEELS, "1--= %d,%d, %x,  [ %lu ]",  wheel_var.joyX , wheel_var.joyY, Joystick_rec.buttons,  (timeTDBGelaspestop*10)  );
                */

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
                        current_guida_auto = 0;
                        current_diff_sterzing_auto = 0;
                        current_set_speed = 0;
                        rpm_guida_auto = 0;
                        rpm_diff_sterzing_auto = 0;
                        rpm_set_speed = 0;

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
                        linear_acceleration_var_resetINT(&linear_acce);


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
                                rpm_guida_auto = 0;
                                current_guida_auto = 0;
                                current_diff_sterzing_auto = 0;
                                current_set_speed = 0;
                                rpm_diff_sterzing_auto = 0;
                                rpm_set_speed = 0;
                                rpm_media_spped = 0;
                                current_media_spped = 0;

                                cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI = cuurent_mode_data.SPUNTO_PARTENZA_H;

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
                                    current_guida_auto = 0;
                                }else if(Joystick_rec.buttons & BUTTON_RPM_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;
                                    tempMOTORSTATE = M_RPM_MODE;
                                    Buzzer_RPMMode();
                                    linear_acceleration_var_resetINT(&linear_acce);
                                    rpm_guida_auto = 0;
                                }


                                if(Joystick_rec.buttons & BUTTON_TURBO){
                                    //HARD MODE
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -0.80, 0.80);
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -0.80, 0.80);
                                }else{
                                    //SOFT MODE
                                    //wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -0.15, 0.15); //0.20
                                    //wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -0.15, 0.15);
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -duty_soft_var, duty_soft_var); //0.20
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -duty_soft_var, duty_soft_var);
                                    
                                    
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
                        
                                if((Joystick_rec.buttons & BUTTON_MENO)&&(Button_piu_meno_relase==0)) {
                                    duty_soft_var = duty_soft_var - 0.050000;
                                    if(duty_soft_var < 0.10000){
                                        duty_soft_var = 0.10000;
                                        Buzzer_short_beep();
                                    }

                                    intRoundfloat =  duty_soft_var * 100.00;
                                    if((intRoundfloat == 19)||(intRoundfloat == 20)){
                                        Buzzer_short_beepbeep();
                                    }
                                    Button_piu_meno_relase = 1;
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle <Meno> %f , %i ",  duty_soft_var, intRoundfloat);
                                }
                                if((Joystick_rec.buttons & BUTTON_PIU)&&(Button_piu_meno_relase==0)){
                                    duty_soft_var = duty_soft_var + 0.050000;
                                    if(duty_soft_var > 0.50000){
                                        duty_soft_var = 0.50000;
                                        Buzzer_short_beep();
                                    }
                                    intRoundfloat =  (float)duty_soft_var * 100.00;
                                    if(intRoundfloat == 20){
                                        Buzzer_short_beepbeep();
                                    }
                                    Button_piu_meno_relase = 1;
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle <Piu> %f , %i",  duty_soft_var,intRoundfloat);
                                }
                                if((Button_piu_meno_relase == 1)&&
                                    ((Joystick_rec.buttons & BUTTON_MENO)==0x00) && 
                                    ((Joystick_rec.buttons & BUTTON_PIU)==0x00) )
                                {
                                    Button_piu_meno_relase = 0;
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle <RELASE>");
                                }

                                //Acceleration
                                //https://github.com/adafruit/AccelStepper/blob/master/AccelStepper.cpp
                                //float AccelStepper::desiredSpeed()

                                //Calc Linear ACCELERATION
                                linear_acceleration(&linear_acce, &wheel_var, 0.030 ); //0.05


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
                                    current_guida_auto = 0;
                                    rpm_diff_sterzing_auto = 0;
                                    current_set_speed = 0;
                                    //reset acceleration
                                    linear_acceleration_var_reset(&linear_acce);
                                }else if(Joystick_rec.buttons & BUTTON_RPM_MODE){
                                    MOTOR_STATE = M_JOY_SECURITY;
                                    tempMOTORSTATE = M_RPM_MODE;
                                    Buzzer_RPMMode();
                                    linear_acceleration_var_resetINT(&linear_acce);
                                    current_guida_auto = 0;
                                    rpm_guida_auto = 0;
                                    rpm_diff_sterzing_auto = 0;
                                    current_set_speed = 0;
                                }else if(Joystick_rec.buttons & BUTTON_CURRENT_MODE){
                                    current_guida_auto = 0;
                                    rpm_guida_auto = 0;
                                    rpm_diff_sterzing_auto = 0;
                                    current_set_speed = 0;
                                    current_diff_sterzing_auto = 0;
                                    Buzzer_short_beep();
                                }

                                if(Joystick_rec.buttons & BUTTON_TURBO){
                                    //HARD MODE
                                    wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -20.00, 20.00);
                                    wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -20.00, 20.00);
                                    current_guida_auto = 0;
                                    current_set_speed = 0;
                                    current_diff_sterzing_auto = 0;

                                    //Correct sterzing friction per girare meglio ************************************
                                    int valxjoy1 = 0;
                                    float valxjoy1f  = 0.00000;

                                    float valxjoy1fper  = 0.00000;
                                    

                                    if(wheel_var.joyY > 100) {
                                        valxjoy1 =  abs(wheel_var.joyX);
                                        valxjoy1f  = mapfloat((float)valxjoy1, 0.0, 1000, 0.0 , 15.000);

                                //        ESP_LOGI(TAGWHEELS, "p> %f , %f    ; (%f)", wheel_var.vescValueLeft, wheel_var.vescValueright,
                                //                               valxjoy1f);

                                        valxjoy1fper = ((valxjoy1f / 1000.0) * wheel_var.joyY) * 2.000;     // FAttore moltiplicazione

                                        if (wheel_var.joyX > 50) {
                                            wheel_var.vescValueright = wheel_var.vescValueright - valxjoy1fper; //valxjoy1f;
                                            if(wheel_var.vescValueright < -2.00){
                                                wheel_var.vescValueright = -2.00;
                                            }

                                        }else if(wheel_var.joyX < 50) {
                                            wheel_var.vescValueLeft = wheel_var.vescValueLeft - valxjoy1fper; //valxjoy1f;
                                            if(wheel_var.vescValueLeft < -2.00){
                                                wheel_var.vescValueLeft = -2.00;
                                            }
                                        }
                                    }

/*
                                    ESP_LOGI(TAGWHEELS, "> %d , %d    ;  %d , %d   ; %d (%f)", leftMotor, rightMotor,  wheel_var.joyX, wheel_var.joyY,  valxjoy1,valxjoy1f);
                                    ESP_LOGI(TAGWHEELS, "> %f , %f    ; (%f) (%f)", wheel_var.vescValueLeft, wheel_var.vescValueright,
                                                                    valxjoy1f , valxjoy1fper);
                                    
                                    ESP_LOGI(TAGWHEELS, "\n");

*/



                                }else{


                                    if(current_guida_auto == 1){

                                        //traking joy

                                        if( (leftMotor > 100)&&(rightMotor > 100)&&((leftMotor-rightMotor)<200)){
                                            //joy avanzamento
                                            //set avanzamento
                                            current_diff_sterzing_auto = 0;

                                            current_media_spped = (leftMotor+rightMotor)>>1;
                                            if(current_media_spped > 1000){
                                                current_media_spped = 1000;
                                            }


                                            if((pdTICKS_TO_MS(xTaskGetTickCount() - time_RPM_auto)) > 500){
                                                time_RPM_auto = xTaskGetTickCount();


                                                if(current_media_spped > current_set_speed){
                                                    current_set_speed = current_media_spped;
                                                }  
                                            }


                                            //ESP_LOGI(TAGWHEELS, "[AUTO-Current]...current_set_speed: %i",current_set_speed);


                                        }else if( (leftMotor < -100)&&(rightMotor < -100)&&((leftMotor-rightMotor)<200)){
                                            //joy avanzamento
                                            //set avanzamento

                                            current_diff_sterzing_auto = 0;

                                            current_media_spped = (leftMotor+rightMotor)>>2;
 
                                            if((pdTICKS_TO_MS(xTaskGetTickCount() - time_RPM_auto)) > 500){
                                                time_RPM_auto = xTaskGetTickCount();

                                                current_set_speed = current_set_speed + current_media_spped;
                                                if(current_set_speed < 0){
                                                    current_set_speed = 0;
                                                    
                                                    current_guida_auto = 0;
                                                    Buzzer_short_beep();

                                                }
                                            }

                                            //ESP_LOGI(TAGWHEELS, "[AUTO-Current]...current_set_speed: %i",current_set_speed);
                                            
                                        }else{

                                            current_diff_sterzing_auto = (rightMotor - leftMotor)>>1; //-500 ; +500
                                            //max 42% di sterzata
                                            current_sterzing_max = (current_set_speed * 42)/100;

                                            current_diff_sterzing_auto = map(current_diff_sterzing_auto, -800, 800, -current_sterzing_max, current_sterzing_max);  //700

                                            //ESP_LOGI(TAGWHEELS, "[AUTO-Current]...current_set_speed= %i ;; current_diff_sterzing_auto= %i",current_set_speed,current_diff_sterzing_auto);

                                        }

                                        wheel_var.vescValueLeft = mapfloat((float)current_set_speed - current_diff_sterzing_auto, -1000, 1000, -cuurent_mode_data.current_soft_var, cuurent_mode_data.current_soft_var); //5.0
                                        wheel_var.vescValueright = mapfloat((float)current_set_speed + current_diff_sterzing_auto, -1000, 1000, -cuurent_mode_data.current_soft_var , cuurent_mode_data.current_soft_var);

                                        //ESP_LOGI(TAGWHEELS, "[MODE]...CURRENT <AUTO> (set_speed) %i    (leftMotor,rightMotor),%i,%i,    (rpm_diff_sterzing_auto)%i",  rpm_set_speed,   leftMotor,rightMotor, rpm_diff_sterzing_auto);


                                    



                                    }else{

                                        //SOFT MODE

    /*
                                        if ((leftMotor < 0)&&(rightMotor < 0)&&
                                            ((leftMotor - rightMotor) < 200) ) { //35%
                                                //sto utilizzando la corrente in frenata
                                            wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -8.00, current_soft_var); //5.0
                                            wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -8.00 , current_soft_var);
                                        }else{
                                            wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -current_soft_var, current_soft_var); //5.0
                                            wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -current_soft_var , current_soft_var);
                                        }

    */
                                        //Start Spunto in partenza solo se 
                                        //Leggi RPM , stai fermo?

                                        xQueueReceive(vsc_data_logL, &VSC_DATA_Left, 1 );
                                        xQueueReceive(vsc_data_logR, &VSC_DATA_Right, 1 );

                                        //ESP_LOGI(TAGWHEELS, "%i, %i",VSC_DATA_Left.RPM, cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI);


                                        if( (VSC_DATA_Left.RPM > cuurent_mode_data.SPUNTO_PARTENZA_H) & (VSC_DATA_Right.RPM > cuurent_mode_data.SPUNTO_PARTENZA_H) ){ 
                                            cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI = cuurent_mode_data.SPUNTO_PARTENZA_L;
                                        }

                                        if( (VSC_DATA_Left.RPM < cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI) & (VSC_DATA_Left.RPM > -cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI) & 
                                            (VSC_DATA_Right.RPM < cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI) & (VSC_DATA_Right.RPM > -cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI) ){ 
                                            cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI = cuurent_mode_data.SPUNTO_PARTENZA_H;
                                            //ESP_LOGI(TAGWHEELS, "Current <SPUNTO>");

                                            //Start Spunto in partenza solo se 
                                            if(cuurent_mode_data.current_soft_var > 8.50){
                                                wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -12.00, 12.00); //5.0
                                                wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -12.00 , 12.00);
                                            }else{
                                                wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -8.50, 8.50); //5.0
                                                wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -8.50 , 8.50);
                                            }
                                        }else if( (VSC_DATA_Left.RPM > cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI) & 
                                                            (VSC_DATA_Right.RPM > cuurent_mode_data.SPUNTO_PARTENZA_ISTERESI) ){ 
                                            
                                            //sto utilizzando la corrente in frenata
                                            if ((leftMotor < 0)&&(rightMotor < 0)&&
                                                ((leftMotor - rightMotor) < 200) ) { //35%
                                                    //sto utilizzando la corrente in frenata
                                                wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -8.00, cuurent_mode_data.current_soft_var); //5.0
                                                wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -8.00 , cuurent_mode_data.current_soft_var);

                                                //ESP_LOGI(TAGWHEELS, "Current <FRENATA>");

                                            }else{
                                                wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -cuurent_mode_data.current_soft_var, cuurent_mode_data.current_soft_var); //5.0
                                                wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -cuurent_mode_data.current_soft_var , cuurent_mode_data.current_soft_var);

                                                //ESP_LOGI(TAGWHEELS, "Current <NORMAL 1> %f",cuurent_mode_data.current_soft_var);

                                            }
                                        }else{
                                            wheel_var.vescValueLeft = mapfloat((float)leftMotor, -1000, 1000, -cuurent_mode_data.current_soft_var, cuurent_mode_data.current_soft_var); //5.0
                                            wheel_var.vescValueright = mapfloat((float)rightMotor, -1000, 1000, -cuurent_mode_data.current_soft_var , cuurent_mode_data.current_soft_var);
                                            //ESP_LOGI(TAGWHEELS, "Current <NORMAL 2> %f",cuurent_mode_data.current_soft_var);
                                        }



                                        //Correct sterzing friction per girare meglio
                                        int valxjoy1 = 0;
                                        float valxjoy1f  = 0.00000;
                                        float valxjoy1fper  = 0.00000;


                                        if(wheel_var.joyY > 300) {
                                            valxjoy1 =  abs(wheel_var.joyX);
                                            valxjoy1f  = mapfloat((float)valxjoy1, 0.0, 1000, 0.0 , cuurent_mode_data.current_soft_var); //2.5

                                            valxjoy1fper = ((valxjoy1f / 1000.0) * wheel_var.joyY )  * 2.000 ;      // FAttore moltiplicazione



                                         //   ESP_LOGI(TAGWHEELS, "p> %f , %f    ; (%f)", wheel_var.vescValueLeft, wheel_var.vescValueright,
                                           //                          valxjoy1f);



                                            if (wheel_var.joyX > 50) {
                                                wheel_var.vescValueright = wheel_var.vescValueright - valxjoy1fper; //valxjoy1f;
                                                if(wheel_var.vescValueright < -1.50){
                                                    wheel_var.vescValueright = -1.50;
                                                }
                                            }else if(wheel_var.joyX < 50) {
                                                wheel_var.vescValueLeft = wheel_var.vescValueLeft - valxjoy1fper; //valxjoy1f;
                                                if(wheel_var.vescValueLeft < -1.50){
                                                    wheel_var.vescValueLeft = -1.50;
                                                }
                                            }
                                        }


/*
                                        ESP_LOGI(TAGWHEELS, "> %d , %d    ;  %d , %d   ; %d (%f)", leftMotor, rightMotor,  wheel_var.joyX, wheel_var.joyY,  valxjoy1,valxjoy1f);
                                        ESP_LOGI(TAGWHEELS, "> %f , %f    ; (%f) (%f)", wheel_var.vescValueLeft, wheel_var.vescValueright,
                                                                    valxjoy1f , valxjoy1fper);


                                        ESP_LOGI(TAGWHEELS, "%f \n", cuurent_mode_data.current_soft_var);
*/






                                    }
                                }


                                if((Joystick_rec.buttons & BUTTON_MENO)&&(Button_piu_meno_relase==0)) {
                                    cuurent_mode_data.current_soft_var = cuurent_mode_data.current_soft_var - 1.00;
                                    if(cuurent_mode_data.current_soft_var < 2.00){
                                        cuurent_mode_data.current_soft_var = 2.00;
                                        Buzzer_short_beep();
                                    }

                                    intRoundfloat =  cuurent_mode_data.current_soft_var * 100;
                                    if(intRoundfloat == 500){
                                        Buzzer_short_beepbeep();
                                    }

                                    Button_piu_meno_relase = 1;

                                    //ESP_LOGI(TAGWHEELS, "[MODE]...DutyCycle <Meno> %f , %i",  cuurent_mode_data.current_soft_var, intRoundfloat);
                                }

                                //short press , long press
                                // short press !!!!!!!!!

                                if((Joystick_rec.buttons & BUTTON_PIU)&&(Button_piu_meno_relase==0)){
                                    time_Current_presslong_button = xTaskGetTickCount();

                                    cuurent_mode_data.current_soft_var = cuurent_mode_data.current_soft_var + 1.00;
                                    if(cuurent_mode_data.current_soft_var > 10.00){
                                        cuurent_mode_data.current_soft_var = 10.00;
                                        Buzzer_short_beep();
                                    }
                                    intRoundfloat =  cuurent_mode_data.current_soft_var * 100;
                                    if(intRoundfloat == 500){
                                        Buzzer_short_beepbeep();
                                    }
                                    Button_piu_meno_relase = 1;
                                    
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...Current <Piu Short> %f , %i",  cuurent_mode_data.current_soft_var, intRoundfloat);
                                    
                                    Button_piu_longpress_relase = 1;

                                }

                                // long press !!!!!!!!!

                                if((Joystick_rec.buttons & BUTTON_PIU)&&(Button_piu_meno_relase==1)&&(Button_piu_longpress_relase==1)){
                                    if((pdTICKS_TO_MS(xTaskGetTickCount() - time_Current_presslong_button)) > 1500){
                                                                               
                                        Button_piu_longpress_relase = 0;
                                       
                                        cuurent_mode_data.current_soft_var = cuurent_mode_data.current_soft_var - 1.00;
                                        if(cuurent_mode_data.current_soft_var < 2.00){
                                            cuurent_mode_data.current_soft_var = 2.00;
                                        }

                                        current_guida_auto = 1;
                                        
                                        Buzzer_short_beepbeepbeep();

                                        //ESP_LOGI(TAGWHEELS, "[MODE]...Current <Piu *Long Press*> %f",  cuurent_mode_data.current_soft_var);

                                    }
                                }


                                if((Button_piu_meno_relase == 1)&&
                                    ((Joystick_rec.buttons & BUTTON_MENO)==0x00) && 
                                    ((Joystick_rec.buttons & BUTTON_PIU)==0x00) )
                                {
                                    Button_piu_meno_relase = 0;
                                    Button_piu_longpress_relase = 0;
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...Current <RELASE>");
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
                                if( (wheel_var.vescValueLeft < 0.1) & (wheel_var.vescValueLeft > -0.1)  & (wheel_var.vescValueright < 0.1) & (wheel_var.vescValueright > -0.1) ){ //0.04
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
                                    rpm_guida_auto = 0;
                                    rpm_diff_sterzing_auto = 0;
                                    rpm_set_speed = 0;
                                    MOTOR_STATE = M_JOY_SECURITY;                                    
                                    tempMOTORSTATE = M_DUTYCYCLE_MODE;
                                    Buzzer_DutyMode();
                                    //reset acceleration
                                    linear_acceleration_var_reset(&linear_acce);
                                }else if(Joystick_rec.buttons & BUTTON_CURRENT_MODE){
                                    rpm_guida_auto = 0;
                                    rpm_diff_sterzing_auto = 0;
                                    rpm_set_speed = 0;
                                    MOTOR_STATE = M_JOY_SECURITY;
                                    tempMOTORSTATE = M_CURRENT_MODE;
                                    Buzzer_CurrentMode();
                                    linear_acceleration_var_reset(&linear_acce);
                                    current_guida_auto = 0;
                                }
                                //SELCET HARD MODE or SOFT MODE ?????????????

                                if(Joystick_rec.buttons & BUTTON_TURBO){
                                    //HARD MODE
                                    rpm_guida_auto = 0;
                                    rpm_diff_sterzing_auto = 0;
                                    rpm_set_speed = 0;

                                    wheel_var.ivescValueLeft = map(leftMotor, -1000, 1000, -3000, 3000); //2500
                                    wheel_var.ivescValueright = map(rightMotor, -1000, 1000, -3000, 3000);

                                    rpm_max_setting_spped = 3000;
                                }else{
                                    //SOFT MODE
                                    //wheel_var.ivescValueLeft = map(leftMotor, -1000, 1000, -1800, 1800);  //700
                                    //wheel_var.ivescValueright = map(rightMotor, -1000, 1000, -1800, 1800);
                                
                                    if(rpm_guida_auto == 1){
                                        //traking joy

                                        if( (leftMotor > 100)&&(rightMotor > 100)&&((leftMotor-rightMotor)<200)){
                                            //joy avanzamento
                                            //set avanzamento
                                            rpm_diff_sterzing_auto = 0;

                                            rpm_media_spped = (leftMotor+rightMotor)>>1;
                                            if(rpm_media_spped > 1000){
                                                rpm_media_spped = 1000;
                                            }

                                            if((pdTICKS_TO_MS(xTaskGetTickCount() - time_RPM_auto)) > 500){
                                                time_RPM_auto = xTaskGetTickCount();

                                                if(rpm_media_spped > rpm_set_speed){
                                                    rpm_set_speed = rpm_media_spped;
                                                }  
                                            }
                                        }else if( (leftMotor < -100)&&(rightMotor < -100)&&((leftMotor-rightMotor)<200)){
                                            //joy avanzamento
                                            //set avanzamento
                                            rpm_diff_sterzing_auto = 0;

                                            rpm_media_spped = (leftMotor+rightMotor)>>2;
 
                                            if((pdTICKS_TO_MS(xTaskGetTickCount() - time_RPM_auto)) > 500){
                                                time_RPM_auto = xTaskGetTickCount();

                                                rpm_set_speed = rpm_set_speed + rpm_media_spped ;
                                                if(rpm_set_speed < 0){
                                                    rpm_set_speed = 0;
                                                }
                                            }
                                            
                                        }else{

                                            rpm_diff_sterzing_auto = (rightMotor - leftMotor)>>1; //-500 ; +500
                                            //max 42% di sterzata
                                            rpm_sterzing_max = (rpm_set_speed * 42)/100;

                                            rpm_diff_sterzing_auto = map(rpm_diff_sterzing_auto, -500, 500, -rpm_sterzing_max, rpm_sterzing_max);  //700
                                        }

                                        wheel_var.ivescValueLeft = map( rpm_set_speed - rpm_diff_sterzing_auto, -1000, 1000, -1800, 1800);  //700
                                        wheel_var.ivescValueright = map(rpm_set_speed + rpm_diff_sterzing_auto, -1000, 1000, -1800, 1800);
                                        rpm_max_setting_spped = 1800;

                                        //ESP_LOGI(TAGWHEELS, "[MODE]...RPM <AUTO> (rpm_set_speed) %i    (leftMotor,rightMotor),%i,%i,    (rpm_diff_sterzing_auto)%i",  rpm_set_speed,   leftMotor,rightMotor, rpm_diff_sterzing_auto);


                                    }else{
                                        wheel_var.ivescValueLeft = map(leftMotor, -1000, 1000, -2500, 2500);  //700
                                        wheel_var.ivescValueright = map(rightMotor, -1000, 1000, -2500, 2500);
                                        rpm_max_setting_spped = 2500;
                                    }                              
                                }
                                /*
                                if((leftMotor < 0)&(rightMotor < 0)){
                                    int16_t tempstrint;
                                    tempstrint = wheel_var.ivescValueLeft;
                                    wheel_var.ivescValueLeft = wheel_var.ivescValueright;
                                    wheel_var.ivescValueright = tempstrint;
                                }
                                */


                                //ESP_LOGI(TAGWHEELS, "[MODE]...P %d , %d  ; (%d)",  wheel_var.ivescValueLeft, wheel_var.ivescValueright, (wheel_var.ivescValueLeft-wheel_var.ivescValueright) );
                                //ESP_LOGI(TAGWHEELS, "");


                                //Correct sterzing friction per girare meglio
                                int valxjoy1 = 0;
                                int valxjoy1f  = 0;
                                float valxjoy1fper  = 0.0;


                                if(wheel_var.joyY > 100) {
                                    valxjoy1 =  abs(wheel_var.joyX);
                                    valxjoy1f  = map(valxjoy1, 0, 1000, 0 , rpm_max_setting_spped); //2.5

                                    if(rpm_max_setting_spped < 2000){
                                        valxjoy1fper =( ((float)valxjoy1f / (float)rpm_max_setting_spped) * wheel_var.joyY ) * 1.150; //1.20;
                                    }else{
                                        valxjoy1fper =( ((float)valxjoy1f / (float)rpm_max_setting_spped) * wheel_var.joyY ) * 2.250;
                                    }


/*
                                    ESP_LOGI(TAGWHEELS, "p> %d , %d    ; (%d)", wheel_var.ivescValueLeft, wheel_var.ivescValueright,
                                                             wheel_var.ivescValueLeft - wheel_var.ivescValueright); //valxjoy1f);
*/


                                    if (wheel_var.joyX > 10) {
                                        wheel_var.ivescValueright = wheel_var.ivescValueright + valxjoy1fper; //valxjoy1f;
                                    }else if(wheel_var.joyX < 10) {
                                        wheel_var.ivescValueLeft = wheel_var.ivescValueLeft + valxjoy1fper; // valxjoy1f;
                                    }
                                }



/*
                                    ESP_LOGI(TAGWHEELS, "d> %d , %d    ; (%d)", wheel_var.ivescValueLeft, wheel_var.ivescValueright,
                                                             wheel_var.ivescValueLeft - wheel_var.ivescValueright); //valxjoy1f);
                                    ESP_LOGI(TAGWHEELS,"%d",rpm_max_setting_spped);

*/




                                if((Joystick_rec.buttons & BUTTON_MENO)&&(Button_piu_meno_relase==0)) {
                                    Button_piu_meno_relase = 1;

                                    rpm_guida_auto = 0;
                                    rpm_set_speed = 0;
                                    rpm_media_spped = 0;
                                    Buzzer_short_beepbeep();

                                    //ESP_LOGI(TAGWHEELS, "[MODE]...RPM <Meno> %f ",  current_soft_var);
                                }
                                if((Joystick_rec.buttons & BUTTON_PIU)&&(Button_piu_meno_relase==0)){
                                    Button_piu_meno_relase = 1;
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...RPM <Piu> %f ",  current_soft_var);

                                    if(rpm_guida_auto == 0){
                                        if( (wheel_var.ivescValueLeft < 50) & (wheel_var.ivescValueLeft > -50)  & (wheel_var.ivescValueright < 50) & (wheel_var.ivescValueright > -50) ){
                                            //motori neutrale
                                            //attiva automatico
                                            rpm_guida_auto = 1;
                                            Buzzer_short_beepbeep();
                                            rpm_diff_sterzing_auto = 0;
                                            rpm_set_speed = 0;
                                            rpm_media_spped = 0;
                                        }
                                    }else{
                                        rpm_guida_auto = 0;
                                        rpm_set_speed = 0;
                                        rpm_media_spped = 0;
                                    }
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...RPM <Piu> %f ",  current_soft_var);
                                }
                                if((Button_piu_meno_relase == 1)&&
                                    ((Joystick_rec.buttons & BUTTON_MENO)==0x00) && 
                                    ((Joystick_rec.buttons & BUTTON_PIU)==0x00) )
                                {
                                    Button_piu_meno_relase = 0;
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...RPM <RELASE>");
                                }


                                //Acceleration
                                //https://github.com/adafruit/AccelStepper/blob/master/AccelStepper.cpp
                                //float AccelStepper::desiredSpeed()

                                //Calc Linear ACCELERATION
                                                                 
                                linear_accelerationINT(&linear_acce, &wheel_var, 500);

                                //Motor
                                if( (wheel_var.ivescValueLeft < 50) & (wheel_var.ivescValueLeft > -50)  & (wheel_var.ivescValueright < 50) & (wheel_var.ivescValueright > -50) ){
                                    if(wheel_var.FREE_RUN_REPEAT > 0){
                                        ESP_LOGI(TAGWHEELS, "[MODE]...RPM free run %d , %d ",  wheel_var.vescValueLeft,wheel_var.vescValueright);

                                        Motor_Free();
                                        wheel_var.FREE_RUN_REPEAT = wheel_var.FREE_RUN_REPEAT - 1;

                                        linear_acceleration_var_resetINT(&linear_acce);
                                    }
                                }else{
                                    //ESP_LOGI(TAGWHEELS, "[MODE]...RPM %d , %d  ; (%d) ",  linear_acce.ivsec_acc_left, linear_acce.ivsec_acc_right, (linear_acce.ivsec_acc_left-linear_acce.ivsec_acc_right));
                                    VESC_SET_RPM(EXID_LEFT, linear_acce.ivsec_acc_left, EXID_RIGHT, linear_acce.ivsec_acc_right);
                                    wheel_var.FREE_RUN_REPEAT = 3;
                                }
                    
                            break;




                        }//switch



                    }//FINE FRENO else NORMAL MODE

                    //ESP_LOGI(TAGWHEELS, "task stack: %d", uxTaskGetStackHighWaterMark(NULL));
                    //ESP_LOGI(TAGWHEELS, "task heap: %d", xPortGetFreeHeapSize());


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
    BuzMsg.timeWaitON = 50;
    BuzMsg.timeWaitOFF = 50;
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
    BuzMsg.timeWaitON = 50;
    BuzMsg.timeWaitOFF = 50;
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
    BuzMsg.timeWaitON = 50;
    BuzMsg.timeWaitOFF = 50;
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





void Buzzer_short_beep(void){
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 70;
    BuzMsg.timeWaitOFF = 70;
    xQueueSend(buzzer_Queue, &BuzMsg, 1);
}

void Buzzer_short_beepbeep(void){
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 60;
    BuzMsg.timeWaitOFF = 100;
    xQueueSend(buzzer_Queue, &BuzMsg, 1);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 60;
    BuzMsg.timeWaitOFF = 100;
    xQueueSend(buzzer_Queue, &BuzMsg, 1);
}


void Buzzer_short_beepbeepbeep(void){
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 60;
    BuzMsg.timeWaitOFF = 100;
    xQueueSend(buzzer_Queue, &BuzMsg, 1);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 60;
    BuzMsg.timeWaitOFF = 100;
    xQueueSend(buzzer_Queue, &BuzMsg, 1);
    BuzMsg.state = 1;
    BuzMsg.timeWaitON = 60;
    BuzMsg.timeWaitOFF = 100;
    xQueueSend(buzzer_Queue, &BuzMsg, 1);
}

