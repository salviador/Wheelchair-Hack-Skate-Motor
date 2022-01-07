#include "adcjoy.h"
#include <stdio.h>
#include <string.h>
#include "rf_driver_hal.h"


#ifndef DEBUG
#define DEBUG   1 // 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


#define USER_SAMPLERATE       (ADC_SAMPLE_RATE_28)
#define USER_DATAWIDTH        (ADC_DS_DATA_WIDTH_16_BIT)
#define USER_RATIO            (ADC_DS_RATIO_128)

/* If USE_CALIBRATION is 1 then the internal calibration
   points are used to compensate the ADC acquired sample */
#define USE_CALIBRATION       (1)

/* Private macro -------------------------------------------------------------*/
#define PRINT_INT(x)    ((int)(x))
#define PRINT_FLOAT(x)  (x>0)? ((int) (((x) - PRINT_INT(x)) * 1000)) : (-1*(((int) (((x) - PRINT_INT(x)) * 1000))))




#define ADC_DMA_BUF_LEN  9 //6  //30

ADC_HandleTypeDef adc_handle;
ADC_ConfigChannelTypeDef xChannel;
DMA_HandleTypeDef hdma_adc;

uint16_t ADC_DMA_buffer[ADC_DMA_BUF_LEN];
int8_t offset_vinp0 = 0;
uint8_t ADC_flag = 0;

extern struct CHART_data_TX ble_data_send;

struct CHART_data_TX joy_raw;
float Joy_x = 0.0, Joy_y = 0.0, vbattery = 0.0;
uint8_t j =0;
uint8_t ADCJOY_STATE = 0;

uint32_t ADCJOY_TIMER;

int16_t taratura_Y_MAX;
int16_t taratura_Y_MIN;
uint16_t campioni_saltare=0;



void ADC_JOY_Init(void){
  /* Configure ADC and DMA */
  MX_ADC_Init();
  MX_DMA_Init();
  
  ADC_flag = 0;
  ADCJOY_STATE = 0;
 
  taratura_Y_MAX = 0;
  taratura_Y_MIN = 0;
}



  /* @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{
  
  Joy_gpio_en_init();
  
  /* Enable the ADC peripheral */
  HAL_ADC_StructInit(&adc_handle);
  adc_handle.Init.DataRatio = USER_RATIO;
  adc_handle.Init.DataWidth = USER_DATAWIDTH;
  adc_handle.Init.SampleRate = USER_SAMPLERATE;
  adc_handle.Init.SamplingMode = ADC_SAMPLING_AT_START; 
  adc_handle.Init.SequenceLength = ADC_SEQ_LEN_03;
  adc_handle.DMA_Handle = &hdma_adc;
  
  if (HAL_ADC_Init(&adc_handle) != HAL_OK) {
  }
  
  // Set the 1 input channel //
  xChannel.ChannelType = ADC_CH_VINP0_TO_SINGLE_POSITIVE_INPUT;
  xChannel.SequenceNumber = ADC_SEQ_POS_01;
  xChannel.VoltRange = ADC_VIN_RANGE_3V6;
  if (HAL_ADC_ConfigChannel(&adc_handle, &xChannel)!= HAL_OK) {
  }
  
  
  // Set the 2 input channel 
  xChannel.ChannelType = ADC_CH_VINM0_TO_SINGLE_NEGATIVE_INPUT; // ADC_CH_VINP3_TO_SINGLE_POSITIVE_INPUT ;
  xChannel.SequenceNumber = ADC_SEQ_POS_02;
  xChannel.VoltRange = ADC_VIN_RANGE_3V6;
  if (HAL_ADC_ConfigChannel(&adc_handle, &xChannel)!= HAL_OK) {
  }

    // Set the 3 input channel 
  xChannel.ChannelType = ADC_CH_BATTERY_LEVEL_DETECTOR;
  xChannel.SequenceNumber = ADC_SEQ_POS_03;
  xChannel.VoltRange = ADC_VIN_RANGE_3V6;
  if (HAL_ADC_ConfigChannel(&adc_handle, &xChannel)!= HAL_OK) {
  }

  
  
#if (USE_CALIBRATION==1)
  if(LL_ADC_GET_CALIB_GAIN_FOR_VINPX_3V6() != 0xFFF) {
    LL_ADC_SetCalibPoint1Gain(ADC, LL_ADC_GET_CALIB_GAIN_FOR_VINPX_3V6() );
    
    offset_vinp0 = LL_ADC_GET_CALIB_OFFSET_FOR_VINPX_3V6();
    if(offset_vinp0 < -64 || offset_vinp0 > 63) {
      LL_ADC_SetCalibPoint1Offset(ADC, 0);
    }
    else {
      LL_ADC_SetCalibPoint1Offset(ADC, offset_vinp0);
      offset_vinp0 = 0;
    }
  }
  else {
    LL_ADC_SetCalibPoint1Gain(ADC, LL_ADC_DEFAULT_RANGE_VALUE_3V6);
  }
#else
  
  
  LL_ADC_SetCalibPoint1Gain(ADC, LL_ADC_DEFAULT_RANGE_VALUE_3V6);
#endif
  LL_ADC_SetCalibPointForSinglePos3V6(ADC, LL_ADC_CALIB_POINT_1);
}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA_CLK_ENABLE();

  /* Configure NVIC for DMA */
  HAL_NVIC_SetPriority(DMA_IRQn, 0); //0);
  HAL_NVIC_EnableIRQ(DMA_IRQn);  
}






void ADC_JOY_task(void){

  switch(ADCJOY_STATE){
    case 0:
       if(ADC_flag==0){
          /* Start ADC-DMA conversion */
           if (HAL_ADC_Start_DMA(&adc_handle, (uint32_t *)ADC_DMA_buffer, ADC_DMA_BUF_LEN) != HAL_OK) {
           }
           ADCJOY_STATE = 1;
       }else{
           ADCJOY_STATE = 1;
       }
    break;

    case 1:
      if(ADC_flag==1){
        ADC_flag = 0;
        
        //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);      
        
        j=0;
        Joy_x = 0.0; 
        Joy_y = 0.0; 
        vbattery = 0.0;
       
        for (int i=0; i<ADC_DMA_BUF_LEN/3; i++) {
          Joy_x += LL_ADC_GetADCConvertedValueSingle(ADC, ADC_DMA_buffer[j], LL_ADC_VIN_RANGE_3V6, USER_DATAWIDTH, offset_vinp0);
          Joy_y += LL_ADC_GetADCConvertedValueSingle(ADC, ADC_DMA_buffer[j+1], LL_ADC_VIN_RANGE_3V6, USER_DATAWIDTH, offset_vinp0);
          vbattery += LL_ADC_GetADCConvertedValueBatt(ADC, ADC_DMA_buffer[j+2], USER_DATAWIDTH, 0);
          j+=3;
        }
        Joy_x /= ADC_DMA_BUF_LEN/3;
        Joy_y /= ADC_DMA_BUF_LEN/3;
        vbattery /= ADC_DMA_BUF_LEN/3;
       
        //PRINTF("%d.%03d mV\t%d.%03d mV\t%d.%03d mV\r\n", PRINT_INT(Joy_x),PRINT_FLOAT(Joy_x), PRINT_INT(Joy_y),PRINT_FLOAT(Joy_y),PRINT_INT(vbattery),PRINT_FLOAT(vbattery), 248);
        
        
        joy_raw.uJoy_x = (int16_t) (Joy_x);
        joy_raw.uJoy_y = (int16_t)(Joy_y);
        joy_raw.uvbattery = (int16_t)(vbattery);
        
        Motor_Convert_Joy(&joy_raw, &ble_data_send);
         
   //     PRINTF("%d\t%d\t%d\r\n", ble_data_send.uJoy_x, ble_data_send.uJoy_y, ble_data_send.uvbattery);
        
        /*
        if(campioni_saltare > 1){
          campioni_saltare = 2;
          
          if(ble_data_send.uJoy_y > taratura_Y_MAX){
            taratura_Y_MAX = ble_data_send.uJoy_y;
          }
          if(ble_data_send.uJoy_y < taratura_Y_MIN){
            taratura_Y_MIN = ble_data_send.uJoy_y;
          }
        }
        campioni_saltare++;
        
        PRINTF("%d\t%d\r\n", taratura_Y_MAX, taratura_Y_MIN);
        */
        
        
        
        // Restart ADC DMA conversion
        if (HAL_ADC_Start_DMA(&adc_handle, (uint32_t *)&ADC_DMA_buffer[0], ADC_DMA_BUF_LEN) != HAL_OK) {
        }

        
        
        
        //TEST!!!!!!!!!
        
        
      //  int XValue = map(ble_data_send.uJoy_x, -1000, 1000, -127, 127);
      //  int YValue = map(ble_data_send.uJoy_y, -1000, 1000, -127, 127);
      

        //PRINTF("%d\t%d\t%d\t%d\r\n", XValue, YValue, leftMotor, rightMotor);
       // PRINTF("%d\t%d  \t\t %d  ,  %d\r\n", leftMotor, rightMotor, ble_data_send.uJoy_x, ble_data_send.uJoy_y);
        
              
        //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);      
       ADCJOY_STATE = 2;
       ADCJOY_TIMER = HAL_GetTick();
      }
    break;

    case 2:
      if((HAL_GetTick() - ADCJOY_TIMER) > 30){
        ADCJOY_STATE = 0;
      }
    break;


  }












}











//1000 e -1000
void Motor_Convert_Joy(struct CHART_data_TX *bleJOY, struct CHART_data_TX *wheelJOY){
    wheelJOY->uvbattery = bleJOY->uvbattery;

    //OFFSET
    //bleJOY->uvbattery = bleJOY->uvbattery - 0;

    //((joy / batt) * 2000) - 1000              //--> ( +1000   0  -1000 )
    wheelJOY->uJoy_x = (int16_t)((       ((float)((float)bleJOY->uJoy_x / (float)bleJOY->uvbattery) * 2000)) - 1000);
    wheelJOY->uJoy_y = (int16_t)((       ((float)((float)bleJOY->uJoy_y / (float)bleJOY->uvbattery) * 2000)) - 1000);

    //ADJUST OFFSET
   
    wheelJOY->uJoy_x = wheelJOY->uJoy_x + 88;
    wheelJOY->uJoy_y = wheelJOY->uJoy_y + 68;

     //LIMIT
    if(wheelJOY->uJoy_x > 1000)
      wheelJOY->uJoy_x = 1000;
    if(wheelJOY->uJoy_x < -1000)
      wheelJOY->uJoy_x = -1000;

    if(wheelJOY->uJoy_y > 1000)
      wheelJOY->uJoy_y = 1000;
    if(wheelJOY->uJoy_y < -1000)
      wheelJOY->uJoy_y = -1000;

    //Invert x
    wheelJOY->uJoy_x = wheelJOY->uJoy_x * -1;
   
}




long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}







void Joy_gpio_en_init(void){

  GPIO_InitTypeDef gpiojoyen;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  //---PA0
  gpiojoyen.Mode = GPIO_MODE_OUTPUT_PP;
  gpiojoyen.Pin = GPIO_PIN_0;
  gpiojoyen.Speed = GPIO_SPEED_FREQ_LOW;
  gpiojoyen.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpiojoyen);

  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
  
}










