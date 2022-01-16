#ifndef __VIBRATION_H__
#define __VIBRATION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rf_driver_hal.h"
#include "BLE_GATT_Client.h"

  
#include <stdint.h>
#include <stdbool.h>



#include <stdint.h>
#include <stdbool.h>

  enum {
    VIBRATION_IDLE = 0,
    
    VIBRATION_TASK_STARTTOGGLE,
        
    VIBRATION_TASK_ON,
    VIBRATION_TASK_ON_DELAY,
    VIBRATION_TASK_OFF_DELAY,
  
  
  
  };

  
void vibration_init(void);  
void VIBRATION_JOY_Task(void);
void VIBRATION_START_TOGGLE(uint8_t count, uint16_t TIME_on, uint16_t TIME_off);
void VIBRATION_STOP_TOGGLE(void);



  
#ifdef __cplusplus
}
#endif




#endif
