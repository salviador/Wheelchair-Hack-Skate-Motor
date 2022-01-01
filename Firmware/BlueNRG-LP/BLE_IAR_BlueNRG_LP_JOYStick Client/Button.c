#include "Button.h"

struct SWITCH PB6__BTN1;
struct SWITCH PB15__BTN2;
struct SWITCH PB0__BTN3;
struct SWITCH PA1__BTN4;
struct SWITCH PB4__BTN5;
struct SWITCH PB5__BTN6;
struct SWITCH PB13__BTN7;
struct SWITCH PA10__BTNBOOT;

struct SWITCH *pulsanti[] = {&PB6__BTN1, &PB15__BTN2, &PB0__BTN3, &PA1__BTN4, &PB4__BTN5, &PB5__BTN6, &PB13__BTN7, &PA10__BTNBOOT};
//struct SWITCH *pulsanti[] = {&PB0__BTN3};



void BTN_init(void){
  GPIO_InitTypeDef gpioBTNx;

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


  //---PB6--- BTN1
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_6;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  PB6__BTN1.port = GPIOB;
  PB6__BTN1.pin = GPIO_PIN_6;

  //---PB15--- BTN2
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_15;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  PB15__BTN2.port = GPIOB; 
  PB15__BTN2.pin = GPIO_PIN_15;
  
  //---PB0--- BTN3
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_0;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  PB0__BTN3.port = GPIOB;
  PB0__BTN3.pin = GPIO_PIN_0;
  
  //---PA1--- BTN4
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_1;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);
  PA1__BTN4.port = GPIOA;
  PA1__BTN4.pin = GPIO_PIN_1;
  
  //---PB4--- BTN5
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_4;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  PB4__BTN5.port = GPIOB;
  PB4__BTN5.pin = GPIO_PIN_4;

  //---PB5--- BTN6
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_5;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  PB5__BTN6.port = GPIOB;
  PB5__BTN6.pin = GPIO_PIN_5;
  
  //---PB13--- BTN7
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_13;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  PB13__BTN7.port = GPIOB;
  PB13__BTN7.pin = GPIO_PIN_13;
  
  //---PA10---BTN/BOOT
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_10;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);
  PA10__BTNBOOT.port = GPIOA;
  PA10__BTNBOOT.pin = GPIO_PIN_10;
  
  pulsante_INIT_debounce(&PB6__BTN1);
  pulsante_INIT_debounce(&PB15__BTN2);
  pulsante_INIT_debounce(&PB0__BTN3);
  pulsante_INIT_debounce(&PA1__BTN4);
  pulsante_INIT_debounce(&PB4__BTN5);
  pulsante_INIT_debounce(&PB5__BTN6);
  pulsante_INIT_debounce(&PB13__BTN7);
  pulsante_INIT_debounce(&PA10__BTNBOOT);
}


//DA RIVEDERE x WAKE UP !!!!!!!!!!!!!!!!!!!!!
void BTN_deinit(void){

  GPIO_InitTypeDef gpioBTNx;

  //---PB6--- BTN1
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_6;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);

  //---PB15--- BTN2
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_15;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  
  //---PB0--- BTN3
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_0;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);

  //---PA1--- BTN4
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_1;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);
  
  //---PB4--- BTN5
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_4;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);

  //---PB5--- BTN6
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_5;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  
  //---PB13--- BTN7
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_13;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  /*
  //---PA10---BTN/BOOT
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_10;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);
  */
    
  //---ADC
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_2;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_3;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &gpioBTNx);
  
  //---PA0  Joy_en
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_0;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);
  
  //---PA11  LED
  gpioBTNx.Mode = GPIO_MODE_INPUT;
  gpioBTNx.Pin = GPIO_PIN_11;
  gpioBTNx.Speed = GPIO_SPEED_FREQ_LOW;
  gpioBTNx.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioBTNx);
  
  
  __HAL_RCC_GPIOB_CLK_DISABLE();
  //__HAL_RCC_GPIOA_CLK_DISABLE();
  
}










void pulsante_INIT_debounce(struct SWITCH* p) {
    //Read Pin   
#ifdef __ICCARM__
    uint8_t value;
    value = HAL_GPIO_ReadPin(p->port, p->pin);
#endif        
#ifdef __XC
    uint8_t value, pinmask;
    pinmask = (1<<p->pin);
    value = *p->port & pinmask;
#endif       
    
    p->state_debounce = 0;
    p->laststato = value;
    p->timewaitdebounce = 0;
    p->changeState = false;
    if(value){
        p->State = true;
    }else{
        p->State = false;
    }
}

void pulsanti_alldebounce_task(struct SWITCH* switchs[], uint8_t size) {
    uint8_t i;
   // struct SWITCH *v;
    for(i=0;i<size; i++){
         //v = switchs[i];
        pulsante_debounce(switchs[i]);//*v);
    }
}






//Debounce
void pulsante_debounce(struct SWITCH* p){
    //Read Pin
#ifdef __ICCARM__
    uint8_t value;
    value = HAL_GPIO_ReadPin(p->port, p->pin);
#endif        
#ifdef __XC
    uint8_t value, pinmask;
    pinmask = (1<<p->pin);
    value = *p->port & pinmask;
#endif       
    switch(p->state_debounce){
        case 0:
            if(value != p->laststato){
                //Stato Cambiato stato
                #ifdef __ICCARM__
                    p->timewaitdebounce = HAL_GetTick();
                #endif        
                #ifdef __XC
                    p->timewaitdebounce = millis();
                #endif                                 
                p->state_debounce = 1;
                //p.laststato = value;
            }
        break;
        case 1:
        #ifdef __ICCARM__
            if((HAL_GetTick() - p->timewaitdebounce)>=DELAY_DEBOUNCE){
        #endif        
        #ifdef __XC
            if((millis() - p->timewaitdebounce)>=DELAY_DEBOUNCE){
        #endif                       
                if(value != p->laststato){
                    p->laststato = value;
                    //Notifica Cambio Stato
                    p->changeState = true;
                    if(value){
                        p->State = true;
                        #ifdef __ICCARM__
                            p->time_ONstate = HAL_GetTick();
                        #endif        
                        #ifdef __XC
                            p->time_ONstate = millis();
                        #endif                                 
                        
                    }else{
                        p->State = false;
                        #ifdef __ICCARM__
                            p->time_OFFstate = HAL_GetTick();
                        #endif        
                        #ifdef __XC
                            p->time_OFFstate = millis();
                        #endif                                                         
                    }
                }
                p->state_debounce = 0;
            }
        break;
    }
 
 }
  
  
  
  
  