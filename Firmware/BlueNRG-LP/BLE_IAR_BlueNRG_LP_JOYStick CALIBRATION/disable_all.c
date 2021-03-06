#include "disable_all.h"
#include "adcjoy.h"
#include "rf_driver_ll_usart.h"
#include "bluenrg_lp_evb_com.h"
#include "Button.h"
#include "led.h"



extern ADC_HandleTypeDef adc_handle;


void disable_all(void){

  //Disable DMA
    HAL_NVIC_DisableIRQ(DMA_IRQn);  
    __HAL_RCC_DMA_CLK_DISABLE();
  
  //Disable ADC
  HAL_ADC_DeInit(&adc_handle);
  __HAL_RCC_ADCDIG_CLK_DISABLE();
  __HAL_RCC_ADCANA_CLK_DISABLE();

  //Disable UART
  BSP_COM_DeInit();
  
  //Disable GPIO
  BTN_deinit();

  LED_deinit();


/*

LL_PWR_DisablePDA(LL_PWR_PUPD_IO0);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO1);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO2);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO3);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO4);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO5);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO6);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO7);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO8);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO9);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO10);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO11);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO12);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO13);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO14);
LL_PWR_DisablePDA(LL_PWR_PUPD_IO15);

LL_PWR_DisablePDB(LL_PWR_PUPD_IO0);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO1);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO2);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO3);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO4);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO5);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO6);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO7);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO8);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO9);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO10);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO11);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO12);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO13);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO14);
LL_PWR_DisablePDB(LL_PWR_PUPD_IO15);






LL_PWR_DisablePUA(LL_PWR_PUPD_IO0);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO1);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO2);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO3);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO4);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO5);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO6);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO7);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO8);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO9);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO10);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO11);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO12);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO13);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO14);
LL_PWR_DisablePUA(LL_PWR_PUPD_IO15);

LL_PWR_DisablePUB(LL_PWR_PUPD_IO0);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO1);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO2);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO3);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO4);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO5);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO6);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO7);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO8);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO9);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO10);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO11);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO12);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO13);
LL_PWR_DisablePUB(LL_PWR_PUPD_IO14);
//LL_PWR_DisablePUB(LL_PWR_PUPD_IO15);
*/
  
  
  
  
  
  
  
  
  
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO0);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO1);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO2);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO3);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO4);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO5);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO6);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO7);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO8);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO9);
  //LL_PWR_EnablePDA(LL_PWR_PUPD_IO10);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO11);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO12);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO13);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO14);
  LL_PWR_EnablePDA(LL_PWR_PUPD_IO15);
  
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO0);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO1);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO2);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO3);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO4);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO5);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO6);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO7);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO8);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO9);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO10);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO11);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO12);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO13);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO14);
  LL_PWR_EnablePDB(LL_PWR_PUPD_IO15);
  














  //L_PWR_EnablePUB(LL_PWR_PUPD_IO4);
  LL_PWR_EnablePUA(LL_PWR_PUPD_IO10);


}
