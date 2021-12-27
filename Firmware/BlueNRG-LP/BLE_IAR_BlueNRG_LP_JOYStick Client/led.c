#include "led.h"



uint8_t LED_TASK_STATE=0;
uint32_t LED_TIME_1=0;

uint16_t LED_TIME_ON_mS = 0; 
uint16_t LED_TIME_OFF_mS = 0; 
uint8_t LED_COUNT_LAMP = 0;



void LED_JOY_Init(void){

  GPIO_InitTypeDef gpioLed;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  //---PA11
  gpioLed.Mode = GPIO_MODE_OUTPUT_PP;
  gpioLed.Pin = GPIO_PIN_11;
  gpioLed.Speed = GPIO_SPEED_FREQ_LOW;
  gpioLed.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioLed);

  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
  
  LED_TASK_STATE= 0;
}



void LED_JOY_Task(void){

  switch (LED_TASK_STATE){

  case LED_IDLE:
    
   
  break;
   
  case LED_TASK_STARTTOGGLE:
      LED_TASK_STATE = LED_TASK_ON;

  break;
  
  case LED_TASK_ON:
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
    
    LED_TIME_1 = HAL_GetTick();
    LED_TASK_STATE = LED_TASK_ON_DELAY;
  break;
  
  
  case LED_TASK_ON_DELAY:
    if((HAL_GetTick() - LED_TIME_1) >  LED_TIME_ON_mS ){
    
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
      
      LED_TIME_1 = HAL_GetTick();
      LED_TASK_STATE = LED_TASK_OFF_DELAY;
    }
  break;
  
  case LED_TASK_OFF_DELAY:
    if((HAL_GetTick() - LED_TIME_1) >  LED_TIME_OFF_mS ){
      LED_TIME_1 = HAL_GetTick();
      LED_TASK_STATE = LED_TASK_OFF_DELAY;
      
      if(LED_COUNT_LAMP>0){
        LED_COUNT_LAMP--;
        LED_TASK_STATE = LED_TASK_ON;
      }else{
        //LED_COUNT_LAMP == 0
        //fine
        LED_TASK_STATE = LED_IDLE;
      }
     
    }
  break;
  
  
  
  
  
  
  
  }


}



void LED_START_TOGGLE(uint8_t count, uint16_t TIME_on, uint16_t TIME_off){
  LED_TIME_ON_mS = TIME_on; 
  LED_TIME_OFF_mS = TIME_off; 
  LED_COUNT_LAMP = count;

  LED_TASK_STATE = LED_TASK_STARTTOGGLE;
}





void LED_STOP_TOGGLE(void){
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);

  LED_TASK_STATE = LED_IDLE;
}





void LED_deinit (void){
  GPIO_InitTypeDef gpioBTNx;

  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);

  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_11;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);


}







