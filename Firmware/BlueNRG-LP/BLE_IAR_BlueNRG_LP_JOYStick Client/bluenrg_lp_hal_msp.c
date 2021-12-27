/**
  ******************************************************************************
  * File Name          : bluenrg_lp_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the 
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rf_driver_hal.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
 
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;

/* Private function prototypes -----------------------------------------------*/


/* External functions --------------------------------------------------------*/



/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0);

}


void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Parameters for ADC initialization */
  __HAL_RCC_ADCDIG_CLK_ENABLE();
  __HAL_RCC_ADCANA_CLK_ENABLE();
  
    /* Configure ADC PINs */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure ADC PINs */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* DMA controller clock enable */
  __HAL_RCC_DMA_CLK_ENABLE();

  hdma_adc.Instance = DMA1_Channel1;
  hdma_adc.Init.Request = DMA_REQUEST_ADC_DS;
  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc.Init.Mode = DMA_NORMAL;
  hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
  
  if(HAL_DMA_Init(&hdma_adc) != HAL_OK) {
  }
  
  __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);
  
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
