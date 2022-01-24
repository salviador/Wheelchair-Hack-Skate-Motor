/**
  ******************************************************************************
  * @file    bluenrg_lp_it.h
  * @author  AMS RF Application Team
  * @version V1.1.0
  * @date    22-April-2021
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2021 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rf_driver_hal.h"
#include "BLE_GATT_Client.h"

/* Private includes ----------------------------------------------------------*/
#include "bluenrg_lp_evb_config.h"

/* Exported types ------------------------------------------------------------*/
void GPIO_DEBUG_Init(void);
/* Exported constants --------------------------------------------------------*/

void PrintNegotiatedLevel(uint8_t stopLevel);
void PrintWakeupSource(uint32_t wakeupSources);


/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif






#endif /* BlueNRGLP_IT_H */

/******************* (C) COPYRIGHT 2021 STMicroelectronics *****END OF FILE****/
