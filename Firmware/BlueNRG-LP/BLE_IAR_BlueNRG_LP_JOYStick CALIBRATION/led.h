#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rf_driver_hal.h"
#include "BLE_GATT_Client.h"

  
#include <stdint.h>
#include <stdbool.h>

  enum {
    LED_IDLE = 0,
    
    LED_TASK_STARTTOGGLE,
        
    LED_TASK_ON,
    LED_TASK_ON_DELAY,
    LED_TASK_OFF_DELAY,
  
  
  
  };

  
  void LED_JOY_Init(void);
  void LED_deinit (void);

  void LED_JOY_Task(void);
  
  void LED_START_TOGGLE(uint8_t count, uint16_t TIME_on, uint16_t TIME_off);
  void LED_STOP_TOGGLE(void);
  
  
  
  
  
#ifdef __cplusplus
}
#endif




#endif
