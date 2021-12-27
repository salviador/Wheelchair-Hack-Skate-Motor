#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rf_driver_hal.h"

  
#include <stdint.h>
#include <stdbool.h>
#ifdef __ICCARM__
#include "rf_driver_hal.h"
#endif        
#ifdef __XC
#include "varie.h"
#endif   


#define NUM_PULSANTI 8


#define DELAY_DEBOUNCE 30
    
  





   struct SWITCH{
        //Hardware Pin assigned
        #ifdef __ICCARM__
          GPIO_TypeDef  *port;
          uint32_t pin;
        #endif        
        #ifdef __XC
            unsigned char* port;
            unsigned char pin;
        #endif        
        //Debounce variable     
        uint8_t state_debounce;
        uint32_t timewaitdebounce;
        uint8_t laststato;
    
        //Signal
        bool changeState;       //true a ogni cambio di stato, AZZERARE DA SOFTWARE !!!
        bool State;             //Stato dello switch, ON o OFF;
        uint32_t time_ONstate;  //start time state on
        uint32_t time_OFFstate; //start time state off       
    };
    
    void pulsante_INIT_debounce(struct SWITCH* p);
    void pulsanti_alldebounce_task(struct SWITCH* switchs[], uint8_t size);
    void pulsante_debounce(struct SWITCH* p);


    
    
    

  void BTN_init(void);
  void BTN_deinit(void);
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif




#endif
