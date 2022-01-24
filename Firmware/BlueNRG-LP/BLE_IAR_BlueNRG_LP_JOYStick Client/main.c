/*

----------------------BlueNRG-LP V 1.1.0----------------------

*/

#include <stdio.h>
#include <string.h>
#include "bluenrg_lp_it.h"
#include "ble_const.h"
#include "bluenrg_lp_stack.h"
#include "rf_driver_hal_power_manager.h"
#include "rf_driver_hal_vtimer.h"
#include "bluenrg_lp_evb_com.h"

#include "BLE_GATT_Client.h"
#include "BLE_GATT_Service_config.h"

#include "bleplat.h"
#include "nvm_db.h"
//#include "OTA_btl.h"
#include "pka_manager.h"
#include "rng_manager.h"
#include "aes_manager.h"
#include "ble_controller.h"
#include "clock.h" 
#include "rf_driver_hal.h"

#include "Button.h"
#include "adcjoy.h"
#include "disable_all.h"
#include "led.h"
#include "vibration.h"

#include "main.h"


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

extern struct CHART_data_TX ble_data_send;
extern slave_device slaves;


uint8_t STANBY_STATE=0, JOY_CLIENT_BUSY = 1;
uint32_t TIMER_STANDBY=0;

uint32_t TIMEOUT_STANDBY=0;
uint32_t TIMEOUT_SECONDI=0;


GPIO_InitTypeDef gpioDEBUG;


extern struct SWITCH PB6__BTN1;
extern struct SWITCH PB15__BTN2;
extern struct SWITCH PB0__BTN3;
extern struct SWITCH PA1__BTN4;
extern struct SWITCH PB4__BTN5;
extern struct SWITCH PB5__BTN6;
extern struct SWITCH PB13__BTN7;
extern struct SWITCH PA10__BTNBOOT;
extern struct SWITCH *pulsanti[];

extern uint8_t ADC_flag;
extern ADC_HandleTypeDef adc_handle;
   
NO_INIT(uint32_t dyn_alloc_a[DYNAMIC_MEMORY_SIZE>>2]);



void ModulesInit(void)
{
  uint8_t ret;
  BLE_STACK_InitTypeDef BLE_STACK_InitParams = BLE_STACK_INIT_PARAMETERS;
  
  LL_AHB_EnableClock(LL_AHB_PERIPH_PKA|LL_AHB_PERIPH_RNG);
  
  /* BlueNRG-LP stack init */
  ret = BLE_STACK_Init(&BLE_STACK_InitParams);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BLE_STACK_Init() 0x%02x\r\n", ret);
    while(1);
  }
  
  BLECNTR_InitGlobal();

  HAL_VTIMER_InitType VTIMER_InitStruct = {HS_STARTUP_TIME, INITIAL_CALIBRATION, CALIBRATION_INTERVAL};
  HAL_VTIMER_Init(&VTIMER_InitStruct);
  
  BLEPLAT_Init();  
  if (PKAMGR_Init() == PKAMGR_ERROR)
  {
      while(1);
  }
  if (RNGMGR_Init() != RNGMGR_SUCCESS)
  {
      while(1);
  }
  
    /* Init the AES block */
  AESMGR_Init();
}

void ModulesTick(void)
{
  /* Timer tick */
  HAL_VTIMER_Tick();
  
  /* Bluetooth stack tick */
  BLE_STACK_Tick();
  
  /* NVM manager tick */
  NVMDB_Tick();
}




////CONFIG_HW_SMPS_10uH


int main(void)  
{
  uint8_t ret;
  WakeupSourceConfig_TypeDef wakeupIO;
  PowerSaveLevels stopLevel;

 
  if (SystemInit(SYSCLK_64M, BLE_SYSCLK_32M) != SUCCESS) 
  {
    while(1);
  }
  
  HAL_Init();           //PIN DEFINITION !!!!!!!!!!!!!!!!!!!!!!!!!!!!
   
  /* Configure IOs for power save modes */
  BSP_IO_Init();
  
  /* Init Clock */
  Clock_Init();
  
  /* Configure I/O communication channel */
  BSP_COM_Init(BSP_COM_RxDataUserCb);           //UART
  
  ModulesInit(); 

  ret = BLE_GATT_Client_DeviceInit();

  //ret = Sensor_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    //BSP_LED_On(BSP_LED3);
    while(1);
  }

  /* Wakeup Source on accelerometer free fall detection */
  wakeupIO.IO_Mask_High_polarity = 0;
  wakeupIO.IO_Mask_Low_polarity = WAKEUP_PA10;
  wakeupIO.RTC_enable = 0;
  wakeupIO.LPU_enable = 0;
  
  /* Button Task */
  BTN_init();
    
  ADC_JOY_Init();
  LED_JOY_Init();  
  vibration_init();
  //GPIO_DEBUG_Init();
  
  __asm("NOP");
  ble_data_send.buttons = 0x00;
  TIMEOUT_SECONDI=0;
  
  JOY_CLIENT_BUSY = 1;
  
PRINTF("SYSTEM STARTTTTTTTTTTTTTTTTTTTTTTTTTTT\r\n");


  while(1)
  {
    ModulesTick();
        
    /* Application Tick */
    APP_Tick();
    
    LED_JOY_Task();
    VIBRATION_JOY_Task();
    
    /* Button Task */
    pulsanti_alldebounce_task(&pulsanti[0], NUM_PULSANTI);

    //----BTN FRENO-------
    if(PB0__BTN3.changeState){
      PB0__BTN3.changeState = false;
      if(PB0__BTN3.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x01;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xFE;
      }
    }    

    //----BTN TURBO -------
    if(PB4__BTN5.changeState){
      PB4__BTN5.changeState = false;
      if(PB4__BTN5.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x02;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xFD;
      }
    }        
  
    
    //----SUD-------
    if(PB13__BTN7.changeState){
      PB13__BTN7.changeState = false;
      if(PB13__BTN7.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x04;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xFB;
      }
    }        

    //----EST-------
    if(PB15__BTN2.changeState){
      PB15__BTN2.changeState = false;
      if(PB15__BTN2.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x08;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xF7;
      }
    }        

    //----OVEST-------
    if(PB6__BTN1.changeState){
      PB6__BTN1.changeState = false;
      if(PB6__BTN1.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x10;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xEF;
      }
    }        

    //----ALTO LEFT-------
    if(PA1__BTN4.changeState){
      PA1__BTN4.changeState = false;
      if(PA1__BTN4.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x20;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xDF;
      }
    }        

    //----ALTO RIGHT -------
    if(PB5__BTN6.changeState){
      PB5__BTN6.changeState = false;
      if(PB5__BTN6.State == false){     //Pulsante premuto a massa
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons | 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons | 0x40;
      }else{                            //Pulsante a Vcc
        //Mappare --> ble_data_send.buttons = ble_data_send.buttons & 0bxxxxxx;
        ble_data_send.buttons = ble_data_send.buttons & 0xBF;
      }
    }        

    




    
    
    ADC_JOY_task();
    
    
    
    
    //---------------------STANDBY----------------------
    //---------------------STANDBY----------------------
    //---------------------STANDBY----------------------
    //---------------------STANDBY----------------------
    //---------------------STANDBY----------------------
    

    if ((slaves.state == SCANNING_DONE)|| (slaves.state == FINISH)){ 
      TIMER_STANDBY = HAL_GetTick();
      if(STANBY_STATE!=4)
        STANBY_STATE = 3;
      //LED_START_TOGGLE(30,100,100);
      //VIBRATION_START_TOGGLE(1,200,100);
      slaves.state = IDLE;
    }
              

  
    // time out
    if((HAL_GetTick() - TIMEOUT_STANDBY) >= 1000){
      TIMEOUT_STANDBY = HAL_GetTick();
    
      //Secondi Counter
      TIMEOUT_SECONDI++;
      
      if(TIMEOUT_SECONDI > 120){
          TIMEOUT_SECONDI = 0;
          
          //GOOOO STANDBY
          State_DisconnectRequest();
          TIMER_STANDBY = HAL_GetTick();
          if(STANBY_STATE!=4)
            STANBY_STATE = 3;
          
          //LED_START_TOGGLE(5,200,100);
          //VIBRATION_START_TOGGLE(1,200,100);

      }
    }
    
    //Check joy in free run
    if((ble_data_send.uJoy_x > 10)||(ble_data_send.uJoy_x < -10)||
       (ble_data_send.uJoy_y > 10)||(ble_data_send.uJoy_y < -10)){

         TIMEOUT_SECONDI = 0;
    }                   

    
    
    
    // switch
    
    switch(STANBY_STATE){
      
      case 0:
          if(PA10__BTNBOOT.State == true){     //Pulsante a Vcc
              STANBY_STATE = 1;
          }
              
      break;
      
      case 1:
          if(PA10__BTNBOOT.changeState){
            PA10__BTNBOOT.changeState = false;
            //if(PA10__BTNBOOT.State == false){     //Pulsante premuto a massa
          }
              
              if((PA10__BTNBOOT.State == false)&&((HAL_GetTick()-PA10__BTNBOOT.time_OFFstate)>=1000))  {     //Pulsante premuto a massa x piu di 1000 mS
              
                //Check joy in free run
                if((ble_data_send.uJoy_x < 10)&&(ble_data_send.uJoy_x > -10)&&
                   (ble_data_send.uJoy_y < 10)&&(ble_data_send.uJoy_y > -10)){
                      State_DisconnectRequest();
                      TIMER_STANDBY = HAL_GetTick();
                      STANBY_STATE = 2;
                    
                      //LED_START_TOGGLE(30,100,100);
                      //VIBRATION_START_TOGGLE(1,200,100);
                      
                   }                   
               }
                  
      break;

      case 2:
          if(PA10__BTNBOOT.changeState){
            PA10__BTNBOOT.changeState = false;

            if(PA10__BTNBOOT.State == true)  {     //Pulsante relase a vcc
                TIMER_STANDBY = HAL_GetTick();           
                STANBY_STATE = 3;
            }
          }
      break;
      
      case 3:
        LED_START_TOGGLE(30,100,100);
        VIBRATION_START_TOGGLE(0,250,100);

        TIMER_STANDBY = HAL_GetTick();
        STANBY_STATE = 4;
      break;
      
      case 4:
        if((HAL_GetTick() - TIMER_STANDBY) > 1000){
            TIMER_STANDBY = HAL_GetTick();
            STANBY_STATE = 5;
            //PRINTF("GO SLEEEEPPPPPPPPPPPPPPPP \r\n");
            
            disable_all();
            
            JOY_CLIENT_BUSY = 0;
        }
      break;
        
      case 5:
        /*
          ret = HAL_PWR_MNGR_Request(POWER_SAVE_LEVEL_STOP_NOTIMER, wakeupIO, &stopLevel);

          if (ret == SUCCESS){
           //PRINTF("OK vado in sleep");
            //STANBY_STATE = 3;
          }
          
        
          if (stopLevel >= POWER_SAVE_LEVEL_STOP_WITH_TIMER) {
              PRINTF("WAKEEEEEEEEE \r\n");
              uint32_t wakeupSources = HAL_PWR_MNGR_WakeupSource();
              PrintWakeupSource(wakeupSources);
              if(wakeupSources  == WAKEUP_PB4){
                NVIC_SystemReset();
              }

          }                  
          */
      break;
    }
    
    
    
    
    
    
    
    //Request sleep only "JOY_CLIENT_BUSY = 0"
    
    HAL_PWR_MNGR_Request(POWER_SAVE_LEVEL_STOP_NOTIMER, wakeupIO, &stopLevel);
    if (stopLevel >= POWER_SAVE_LEVEL_STOP_WITH_TIMER) {
        uint32_t wakeupSources = HAL_PWR_MNGR_WakeupSource();
        PrintWakeupSource(wakeupSources);
        if(wakeupSources  == WAKEUP_PA10){
          NVIC_SystemReset();
        }

    }             
    
    
    
    
    
  }/* while (1) */
}





void GPIO_DEBUG_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  gpioDEBUG.Mode = GPIO_MODE_OUTPUT_PP;
  gpioDEBUG.Pin = GPIO_PIN_4;
  gpioDEBUG.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpioDEBUG.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioDEBUG);
  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);

}



/* User callback if an interrupt is associated to the wakeup source */
void HAL_PWR_MNGR_WakeupIOCallback(uint32_t source)
{
  if (source & WAKEUP_PB4) {    
     /* When connected BUTTON_1 allows to disconnect */
    //  NVIC_SystemReset();

  }
}

/* ***************** BlueNRG-LP Power Management Callback ******************************* */

PowerSaveLevels App_PowerSaveLevel_Check(PowerSaveLevels level)
{
  
   
  /*
  if( ((slaves.state == SCANNING_DONE)|| (slaves.state == FINISH))  && ( (!BSP_COM_TxFifoNotEmpty()) || (!BSP_COM_UARTBusy()) )  ) { 
    disable_all();
    return POWER_SAVE_LEVEL_STOP_NOTIMER;
  }
  */
  
  if( BSP_COM_TxFifoNotEmpty() || BSP_COM_UARTBusy() || (JOY_CLIENT_BUSY==1) )
    return POWER_SAVE_LEVEL_RUNNING;
  
   
  return POWER_SAVE_LEVEL_STOP_NOTIMER;
}

/* Hardware Error event. 
   This event is used to notify the Host that a hardware failure has occurred in the Controller. 
   Hardware_Code Values:
   - 0x01: Radio state error
   - 0x02: Timer overrun error
   - 0x03: Internal queue overflow error
   - 0x04: Late Radio ISR
   After this event with error code 0x01, 0x02 or 0x03, it is recommended to force a device reset. */

void hci_hardware_error_event(uint8_t Hardware_Code)
{
  if (Hardware_Code <= 0x03)
  {
    NVIC_SystemReset();
  }
}


/**
  * This event is generated to report firmware error informations.
  * FW_Error_Type possible values: 
  * Values:
  - 0x01: L2CAP recombination failure
  - 0x02: GATT unexpected response
  - 0x03: GATT unexpected request
    After this event with error type (0x01, 0x02, 0x3) it is recommended to disconnect. 
*/
void aci_hal_fw_error_event(uint8_t FW_Error_Type,
                            uint8_t Data_Length,
                            uint8_t Data[])
{
  if (FW_Error_Type <= 0x03)
  {
    uint16_t connHandle;
    
    /* Data field is the connection handle where error has occurred */
    connHandle = LE_TO_HOST_16(Data);
    
    aci_gap_terminate(connHandle, BLE_ERROR_TERMINATED_REMOTE_USER); 
  }
}




/**
  * @brief  Conversion complete callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  //HAL_ADC_Stop(&adc_handle);
  HAL_ADC_Stop_DMA(&adc_handle);
  ADC_flag = 1;
  
}



/**
  * @brief  Display the Stop Level negotiated.
  * @param  stopLevel negotiated Stop level
  * @retval None
  */
void PrintNegotiatedLevel(uint8_t stopLevel)
{
  PRINTF("Power save level negotiated: ");
  switch (stopLevel)
  { 
  case POWER_SAVE_LEVEL_RUNNING:
    PRINTF ("RUNNING\r\n");
    break;
  case POWER_SAVE_LEVEL_CPU_HALT:
    PRINTF ("CPU_HALT\r\n");
    break;
  case POWER_SAVE_LEVEL_STOP_WITH_TIMER:
    PRINTF ("STOP_WITH_TIMER\r\n");
    break;
  case POWER_SAVE_LEVEL_STOP_NOTIMER:
    PRINTF ("STOP_NOTIMER\r\n");
    break;
  }
}

/**
  * @brief  Display the Wakeup Source.
  * @param  wakeupSource Wakeup Sources
  * @retval None
  */
void PrintWakeupSource(uint32_t wakeupSources)
{
  PRINTF("Wakeup Source : ");
  switch (wakeupSources)
  {
  case WAKEUP_RTC:
    PRINTF("WAKEUP_RTC ");
    break;
  case WAKEUP_BLE_HOST_TIMER:
    PRINTF("WAKEUP_BLE_HOST_TIMER ");
    break;
  case WAKEUP_BLE:
    PRINTF("WAKEUP_BLE ");
    break;
  case WAKEUP_PA11:
    PRINTF("WAKEUP_PA11 ");
    break;
  case WAKEUP_PA10:
    PRINTF("WAKEUP_PA10 ");
    break;
  case WAKEUP_PA9:
    PRINTF("WAKEUP_PA9 ");
    break;
  case WAKEUP_PA8:
    PRINTF("WAKEUP_PA8 ");
    break;
  case WAKEUP_PB7:
    PRINTF("WAKEUP_PB7 ");
    break;
  case WAKEUP_PB6:
    PRINTF("WAKEUP_PB6 ");
    break;
  case WAKEUP_PB5:
    PRINTF("WAKEUP_PB5 ");
    break;
  case WAKEUP_PB4:
    PRINTF("WAKEUP_PB4 ");
    break;
  case WAKEUP_PB3:
    PRINTF("WAKEUP_PB3 ");
    break;
  case WAKEUP_PB2:
    PRINTF("WAKEUP_PB2 ");
    break;
  case WAKEUP_PB1:
    PRINTF("WAKEUP_PB1 ");
    break;
  case WAKEUP_PB0:
    PRINTF("WAKEUP_PB0 ");
    break;
  default:
    PRINTF("0x%08x ", wakeupSources);
  }
  PRINTF("\r\n");
}


/***************************************************************************************/

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    /* Infinite loop */
    while (1)
    {}
}
#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
