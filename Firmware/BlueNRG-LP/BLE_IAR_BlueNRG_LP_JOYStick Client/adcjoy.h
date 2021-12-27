#ifndef __ADCJOY_H__
#define __ADCJOY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rf_driver_hal.h"
#include "BLE_GATT_Client.h"

  
#include <stdint.h>
#include <stdbool.h>
#ifdef __ICCARM__
#include "rf_driver_hal.h"
#endif        
#ifdef __XC
#include "varie.h"
#endif   



  void ADC_JOY_Init(void);

  static void MX_ADC_Init(void);
  static void MX_DMA_Init(void);
  void ADC_JOY_task(void);

  void Motor_Convert_Joy(struct CHART_data_TX *bleJOY, struct CHART_data_TX *wheelJOY);
  long map(long x, long in_min, long in_max, long out_min, long out_max);

  
  void Joy_gpio_en_init(void);
  
  
  
  
#ifdef __cplusplus
}
#endif




#endif
