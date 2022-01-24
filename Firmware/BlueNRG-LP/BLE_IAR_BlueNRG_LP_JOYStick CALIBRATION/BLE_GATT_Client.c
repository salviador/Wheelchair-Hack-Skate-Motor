#include <stdio.h>
#include <string.h>
#include "ble_const.h" 
#include "bluenrg_lp_stack.h"
//#include "OTA_btl.h" 

#include "BLE_GATT_Client.h"
#include "rf_driver_hal_vtimer.h"
#include "gap_profile.h"
#include "clock.h"
#include "BLE_GATT_Service_config.h"


#include <stdlib.h>

#include "bluenrg_lp_it.h"
#include "ble_const.h"

#include "bluenrg_lp_evb_com.h"
#include "bluenrg_lp_evb_button.h"
#include "gap_profile.h"
#include "gatt_profile.h"
#include "led.h"
#include "vibration.h"

struct CHART_data_TX ble_data_send;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define UPDATE_CONN_PARAM 1 //0
#define ADV_INTERVAL_MIN_MS  200
#define ADV_INTERVAL_MAX_MS  200

#define CONN_INTERVAL_MIN       (20*100/125)    //   20 ms
#define CONN_INTERVAL_MAX       (20*100/125)    //   20 ms
#define SUPERVISION_TIMEOUT     (1000/10)       // 1000 ms
#define CE_LENGTH               (7*1000/625)



//MIO INDIRIZZO DEL JOYSTICK
#define BD_ADDR_MASTER      0xe0, 0x00, 0x00, 0xE1, 0x80, 0x02

//INDIRIZZO REMOTO0
#define BD_ADDR_SLAVE       0x35, 0x85, 0xB3, 0xA1, 0xDF, 0x7C



//#define SCAN_INTERVAL           (100*1000/625)  //  100 ms
//#define SCAN_WINDOW             (100*1000/625)  //  100 ms
#define SCAN_INTERVAL       ((uint16_t)(100/0.625))     // 100 ms
#define SCAN_WINDOW         ((uint16_t)(100/0.625))     // 100 ms


static uint8_t Service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf9,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};

static uint8_t Char_TX_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf8,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};

static uint8_t Char_RX_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf7,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};








/* States of the state machine used to discover services, enable notifications and
  write the offset into the slaves.
*/





slave_device slaves;










//***************************** GATT ___INIT___*****************************
//***************************** GATT ___INIT___*****************************
//***************************** GATT ___INIT___*****************************
//***************************** GATT ___INIT___*****************************
//***************************** GATT ___INIT___*****************************
//***************************** GATT ___INIT___*****************************


uint8_t BLE_GATT_Client_DeviceInit()
{
  uint8_t ret;

  uint8_t bdaddr[] = {BD_ADDR_MASTER};

  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  uint8_t device_name[] = {'B', 'L', 'E', ' ', 'T', 'e', 's', 't'};
  
  //static uint8_t adv_data[] = {0x02,AD_TYPE_FLAGS, FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED,
  //                           9, AD_TYPE_COMPLETE_LOCAL_NAME,'B', 'L', 'E', ' ', 'T', 'e', 's', 't'};

  
  /* Configure Public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_write_config_data() failed: 0x%02x\r\n", ret);
    return ret;
  }

  
  /* Set the TX power 0 dBm */
  aci_hal_set_tx_power_level(0, 25);
  //aci_hal_set_tx_power_level(0, 10);
  
  /* GATT Init */
  ret = aci_gatt_srv_init();
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_srv_init() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* GAP Init */
  ret = aci_gap_init(GAP_CENTRAL_ROLE, 0, 0x07, 0x00, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed: 0x%02x\r\n", ret);
    return ret;
  }
 
  /* Update device name */
  Gap_profile_set_dev_name(0, sizeof(device_name), device_name);
  
  /* Set the IO capability */
  ret = aci_gap_set_io_capability(IO_CAP_DISPLAY_ONLY);
  if(ret){
    PRINTF("aci_gap_set_io_capability() failed: 0x%02x\r\n", ret);
    return ret;
  }

/*
  ret = aci_gap_set_authentication_requirement(BONDING,
                                               MITM_PROTECTION_REQUIRED,
                                               SC_IS_NOT_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7, 
                                               16,
                                               USE_FIXED_PIN_FOR_PAIRING,
                                               123456);
*/
/*
  ret = aci_gap_set_authentication_requirement(NO_BONDING,
                                               MITM_PROTECTION_NOT_REQUIRED,
                                               SC_IS_NOT_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7, 
                                               16,
                                               USE_FIXED_PIN_FOR_PAIRING,
                                               123456);
  //Per password FIXED
  ret= aci_gap_set_auth_requirement(BONDING, 
                                    MITM_PROTECTION_REQUIRED,
                                    SC_IS_SUPPORTED, 
                                    KEYPRESS_IS_NOT_SUPPORTED,
                                    7, 
                                    16, 
                                    0x00, 
                                    0x123456, 
                                    0x00 );

  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_authentication_requirement()failed: 0x%02x\r\n", ret);
    return ret;
  }
  */
  
  ret = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, SCAN_ACCEPT_ALL, LE_1M_PHY_BIT, PASSIVE_SCAN, SCAN_INTERVAL, SCAN_WINDOW);
  printf("Scan configuration for LE_1M_PHY: 0x%02X\n", ret);
  
  ret = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, SCAN_ACCEPT_ALL, LE_CODED_PHY_BIT, PASSIVE_SCAN, SCAN_INTERVAL, SCAN_WINDOW);  
  printf("Scan configuration for LE_CODED_PHY: 0x%02X\n", ret);
  
  ret = aci_gap_set_connection_configuration(LE_1M_PHY_BIT, CONN_INTERVAL_MIN, CONN_INTERVAL_MAX, 0, SUPERVISION_TIMEOUT, CE_LENGTH, CE_LENGTH);
  printf("Connection configuration for LE_1M_PHY:  0x%02X\n", ret);
  
  ret = aci_gap_set_connection_configuration(LE_CODED_PHY_BIT, CONN_INTERVAL_MIN, CONN_INTERVAL_MAX, 0, SUPERVISION_TIMEOUT, CE_LENGTH, CE_LENGTH);  
  printf("Connection configuration for LE_CODED_PHY: 0x%02X\n", ret);

    
  PRINTF("BLE Stack Initialized with SUCCESS\n");

  /*
    hci_le_extended_advertising_report_event(). The end of the procedure is indicated by
    aci_gap_proc_complete_event() event callback with Procedure_Code parameter equal to
    GAP_GENERAL_DISCOVERY_PROC (0x1).
  */
 
   slaves.pairing_state = PAIRING_IDLE;
   
   slaves.tx_handle = 0;
   slaves.rx_handle = 0;
   slaves.conn_handle = 0;
   slaves.service_start_handle = 0;
   slaves.service_end_handle = 0;

   
   
    // CONNETTI o SCANSIONA
   //slaves.state = CONNECT;
   slaves.state = SCAN_START;


   
  return BLE_STATUS_SUCCESS;
}



void Connect(void)
{  
  tBleStatus ret;
  //uint8_t phy_bit;
  
  tBDAddr bdaddr = {BD_ADDR_SLAVE}; 
  
  ret = aci_gap_create_connection(LE_1M_PHY_BIT, PUBLIC_ADDR, bdaddr);
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error while starting connection: 0x%02x\r\n", ret);
    return;
  }
  
  printf("Connecting \r\n");
  
}



void CancelConnect(void)
{
   tBleStatus ret;

  //aci_gap_terminate_proc(GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC);
  //ret = aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
  ret = aci_gap_terminate(slaves.conn_handle,0x13);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error while starting disconnection: 0x%02x\r\n", ret);
    return;
  }
  ret = aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error while starting disconnection: 0x%02x\r\n", ret);
    return;
  }

  
}


void State_DisconnectRequest(void)
{
  slaves.state = DISCONNECT;
}



tClockTime temp_TIME1;


void APP_Tick(void)
{
  uint8_t ret = 0; 

  
  
    //---- STATE Machine Master
  

   // == Future Paring State ==
  switch(slaves.pairing_state){
    case START_PAIRING:

      break;
    default:

      break;
  }

   // == BLE master==
  switch(slaves.state){
    case IDLE:
      __asm("nop");
    break;
  
    case SCAN_START:
      //Se trova UUID Corrispondente si connette in automatico , se si vuole connettere manuale COMMENTARE il contenuto di--> hci_le_advertising_report_event
       aci_gap_start_procedure(0x01,0x01,0,0);               //START SCAN !!!! response EVENT pag 67 
       //aci_gap_start_procedure(0x01,0x01,100,0);               //START SCAN !!!! response EVENT pag 67 
       slaves.state = SCANNING;
       
       LED_START_TOGGLE(255,200,200);
    break;
    
    case SCANNING:
      __asm("nop");
    break;
    
    case SCANNING_DONE:
      //Finita la scansione, non ho trovato nulla, se trovavo qualcosa mi connettevo!
      
      //VAi IN STANDBY , in main.c
      __asm("nop");
      __asm("nop");
      __asm("nop");
    break;

    
    case CONNECT:
      Connect();
    break;
  
    case DISCONNECT:
      CancelConnect();
      slaves.state = IDLE;      //Disconnessione Manuale  
     break;
    
    case EXCHANGE_CONFIG:
    {
      /* Exchange ATT MTU */        
      ret = aci_gatt_clt_exchange_config(slaves.conn_handle);
      if(ret == BLE_STATUS_SUCCESS){
        slaves.state = EXCHANGING_CONFIG;               //wait event -> aci_gatt_clt_proc_complete_event -> START_SERVICE_DISCOVERY
      }
      else {
        slaves.state = DISCONNECT;
      }        
    }
    break;      

    case START_SERVICE_DISCOVERY:
    {
      /* Start discovery of all primary services */
      ret = aci_gatt_clt_disc_all_primary_services(slaves.conn_handle); //wait 2 event -> aci_att_clt_read_by_group_type_resp_event & aci_gatt_clt_proc_complete_event
      if(ret == BLE_STATUS_SUCCESS){
        slaves.state = DISCOVERING_SERVICES;
      }
      else {
        slaves.state = DISCONNECT;
      }      
    }
    break;

    case START_CHAR_DISCOVERY:
      {
        /* Start characteristic discovery for Serial Port Service */
        ret = aci_gatt_clt_disc_all_char_of_service(slaves.conn_handle, slaves.service_start_handle, slaves.service_end_handle);
        //PRINTF_DBG2("aci_gatt_clt_disc_all_char_of_service() for Serial Port service: 0x%02X\r\n", ret);
        if(ret == 0){
          slaves.state = DISCOVERING_CHARATTERISTIC;
        }
        else {
          slaves.state = DISCONNECT;
        }      
      }
      break;

      
    //Se ci sono abilita le notifiche sui charatteristic    
    case ENABLE_TX_CHAR_NOTIFICATIONS:
      {
        /* Enable notifications for TX characteristic */
        
        static uint8_t client_char_conf_data[] = {0x01, 0x00}; // Enable notifications
        
        ret = aci_gatt_clt_write(slaves.conn_handle, slaves.rx_handle+2, 2, client_char_conf_data);

        if(ret == 0){
          slaves.state = ENABLING_TX_CHAR_NOTIFICATIONS;
        }
        else if(ret == BLE_STATUS_INSUFFICIENT_RESOURCES){
          // Retry later
        }
        else {
          slaves.state = IDLE;
        }
      }
      break;

      
    case DONE_CONNESSO:
      
      
      if( (Clock_Time() - temp_TIME1 ) >=  100){  //50){
        temp_TIME1 = Clock_Time();

        //HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_4);

        //PRINTF("CONNESSO DONE.... !!");

        
        //static uint8_t client_char_conf_data[] = {0xCC, 0xDD}; // Enable notifications
        
        
        uint8_t *datasend;
        
        datasend = (uint8_t *)&ble_data_send;
        
        
        //ret = aci_gatt_clt_write(slaves.conn_handle, slaves.tx_handle+1, sizeof(ble_data_send), datasend);     //Conferma di ricevuta in --> aci_gatt_clt_proc_complete_event
        ret = aci_gatt_clt_write_without_resp(slaves.conn_handle, slaves.tx_handle+1, sizeof(ble_data_send), datasend);  //Invio SENZA conferma    
/*
        if(ret == 0){
                  PRINTF("write Ok!!");
        }
        else {
                  PRINTF("write error!!");

        }
*/  
      
/*

        ret = aci_gatt_clt_read(slaves.conn_handle, slaves.rx_handle+1); // -->aci_att_clt_read_resp_event
        if(ret == 0){
                  PRINTF("Read Ok!!");
        }
        else {
                  PRINTF("Read error!!");

        }



*/






        
      }
      
      
      
      
    break;
    
    case FINISH:
      //Finita la connessione o interrotta dal serve
      //VAi IN STANDBY , in main.c

      __asm("nop");
      __asm("nop");
      __asm("nop");
    break;
    
    
    
  }

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}







/* ***************** BlueNRG-LP Stack Callbacks ********************************/
/* ***************** BlueNRG-LP Stack Callbacks ********************************/
/* ***************** BlueNRG-LP Stack Callbacks ********************************/
/* ***************** BlueNRG-LP Stack Callbacks ********************************/
/* ***************** BlueNRG-LP Stack Callbacks ********************************/
/* ***************** BlueNRG-LP Stack Callbacks ********************************/
/* ***************** BlueNRG-LP Stack Callbacks ********************************/





/* ***************** SCAN Callbacks *****************************/
/* ***************** SCAN Callbacks *****************************/
/* ***************** SCAN Callbacks *****************************/
/* ***************** SCAN Callbacks *****************************/
/* ***************** SCAN Callbacks *****************************/
/* ***************** SCAN Callbacks *****************************/

//Quando finisce la scansione
/*******************************************************************************
 * Function Name  : aci_gap_proc_complete_event.
 * Description    : This event indicates the end of a GAP procedure.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gap_proc_complete_event(uint8_t Procedure_Code,
                                 uint8_t Status,
                                 uint8_t Data_Length,
                                 uint8_t Data[])
{

  //slaves.state = SCANNING_DONE;
  PRINTF("[Event]       aci_gap_proc_complete_event\r\n");
  
  if(slaves.state == SCANNING){
    
    slaves.state = SCANNING_DONE;
  
  }
  
  
}


// CONNETTI x UUID

void hci_le_advertising_report_event(uint8_t Num_Reports,
                                     Advertising_Report_t Advertising_Report[])
{
  uint8_t AD_len, AD_type;
  uint8_t i = 0;
  tBleStatus ret;
    
  
  PRINTF("Address: ");
  PRINTF("%X ",Advertising_Report[0].Address[0]);
  PRINTF("%X ",Advertising_Report[0].Address[1]);
  PRINTF("%X ",Advertising_Report[0].Address[2]);
  PRINTF("%X ",Advertising_Report[0].Address[3]);
  PRINTF("%X ",Advertising_Report[0].Address[4]);
  PRINTF("%X ",Advertising_Report[0].Address[5]);
  PRINTF("\r\n");
  
  
  while(i < Advertising_Report[0].Data_Length){
    AD_len = Advertising_Report[0].Data[i];
    AD_type = Advertising_Report[0].Data[i+1];    
    
    PRINTF("AD_type: ");
    PRINTF("%d ",AD_type);
    PRINTF("\r\n");
  
    
    if(AD_type == 0x07){ //AD_TYPE_128_BIT_UUID_SERVICE_DATA){ //0x07){ //AD_TYPE_128_BIT_UUID_SERVICE_DATA){
      
      // Search for Service UUID
      
      if(memcmp(&Advertising_Report[0].Data[i+2], Service_uuid, sizeof(Service_uuid))==0 ){ //&& Advertising_Report[0].Data[i+18] == SERVICE_DATA_TO_SEARCH_FOR
        // Device found!
        aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
        aci_gap_terminate_proc(0x01);
        
        ret = aci_gap_create_connection(LE_1M_PHY_BIT, Advertising_Report[0].Address_Type, Advertising_Report[0].Address);        
        PRINTF("aci_gap_create_connection %02X\r\n", ret);

        
        //Connect();
        slaves.state = IDLE;

        
        return;
      }
      
      
    }
    i += AD_len+1;
  }

  PRINTF("BLE *******SEARCH******* \r\n");
}

/* This callback is called when an advertising report is received */
void hci_le_extended_advertising_report_event(uint8_t Num_Reports,
                                              Extended_Advertising_Report_t Extended_Advertising_Report[])
{
  Advertising_Report_t Advertising_Report;
  Advertising_Report.Address_Type = Extended_Advertising_Report[0].Address_Type;
  memcpy(Advertising_Report.Address, Extended_Advertising_Report[0].Address, 6);
  Advertising_Report.Data_Length = Extended_Advertising_Report[0].Data_Length;
  Advertising_Report.Data = Extended_Advertising_Report[0].Data;
  Advertising_Report.RSSI = Extended_Advertising_Report[0].RSSI;
  hci_le_advertising_report_event(1, &Advertising_Report);
}




void aci_gap_bond_lost_event(void)
{  
  PRINTF("aci_gap_bond_lost_event\r\n");
}










/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/
/* ***************** Callbacks WRITE , READ , NOTIFICATION ***** PAG-69 ****************/


/*******************************************************************************
 * Function Name  : aci_gatt_clt_notification_event.
 * Description    : This event occurs when a notification is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_clt_notification_event(uint16_t Connection_Handle,
                                     uint16_t Attribute_Handle,
                                     uint16_t Attribute_Value_Length,
                                     uint8_t Attribute_Value[])
{ 
  //uint16_t attr_handle;
 
  if(Attribute_Handle == slaves.rx_handle + 1)
  {
    uint32_t counter;
    
    counter = LE_TO_HOST_32(Attribute_Value);
    
    PRINTF("Notification from server : %d (%d bytes)\n", counter, Attribute_Value_Length);
    
  }  

  PRINTF("[EVENT]        aci_gatt_clt_notification_event\r\n");

}


void aci_att_clt_read_resp_event(uint16_t Connection_Handle,
                                 uint16_t Event_Data_Length,
                                 uint8_t Attribute_Value[])
{
  
  
   PRINTF("[EVENT]        aci_att_clt_read_resp_event\r\n");

   PRINTF("read: ");

   for(int i = 0 ; i < Event_Data_Length; i++){
      PRINTF("%2X", Attribute_Value[i]);
   }
   PRINTF("\r\n");

   

}












/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/
/* ***************** Callbacks DISCOVERY SERVICE and CHARATTERISTIC ****************/





/*******************************************************************************
 * Function Name  : aci_gatt_clt_disc_read_char_by_uuid_resp_event.
 * Description    : This event occurs when a discovery read characteristic by UUID response.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_clt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
    PRINTF("[EVENT]        aci_gatt_clt_disc_read_char_by_uuid_resp_event\r\n");
}


void print_uuid(uint8_t *uuid)
{
  for(int i = 0; i < 16; i++)
    PRINTF("%02X",uuid[i]);
}


void aci_att_clt_read_by_type_resp_event(uint16_t Connection_Handle,
                                     uint8_t Handle_Value_Pair_Length,
                                     uint16_t Data_Length,
                                     uint8_t Handle_Value_Pair_Data[])
{
  uint16_t handle;
  
  switch(slaves.state){
    case DISCOVERING_CHARATTERISTIC:
      for(int i = 0; i < Data_Length; i += Handle_Value_Pair_Length){
        if(Handle_Value_Pair_Length == 21){ // 128-bit UUID
    
          handle = LE_TO_HOST_16(&Handle_Value_Pair_Data[i]);
          
          print_uuid(&Handle_Value_Pair_Data[i+5]);
          
          if(memcmp(&Handle_Value_Pair_Data[i+5], Char_TX_uuid, 16) == 0){
            slaves.tx_handle = handle;
            PRINTF("TX Char handle for slave 0x%04X\r\n", handle);
          }
          else if(memcmp(&Handle_Value_Pair_Data[i+5], Char_RX_uuid, 16) == 0){
            slaves.rx_handle = handle;
             PRINTF("RX Char Handle for slave 0x%04X\r\n", handle);
          }
          
        }
      }
      break;    
  }
  
      PRINTF("[EVENT]        aci_att_clt_read_by_type_resp_event\r\n");

}



/*******************************************************************************
 * Function Name  : aci_gatt_clt_proc_complete_event.
 * Description    : This event occurs when a GATT procedure complete is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_clt_proc_complete_event(uint16_t Connection_Handle,
                                      uint8_t Error_Code)
{ 
  
  PRINTF("[EVENT]        aci_gatt_clt_proc_complete_event\r\n");


  if(Error_Code != BLE_STATUS_SUCCESS){
    PRINTF("ERROR (aci_gatt_clt_proc_complete_event) 0x%02X (0x%04X).\r\n", Error_Code, slaves.conn_handle);
    slaves.state = DISCONNECT;
    return;
  }
  
  switch(slaves.state){    
  case EXCHANGING_CONFIG:
    PRINTF("Configuration exchanged (aci_gatt_clt_proc_complete_event) (0x%04X).\r\n", slaves.conn_handle);    
    slaves.state = START_SERVICE_DISCOVERY;
    break;    
  case DISCOVERING_SERVICES:
    PRINTF("Discovering services ended (aci_gatt_clt_proc_complete_event) (0x%04X).\r\n", slaves.conn_handle);
    if(slaves.service_start_handle != 0)
      slaves.state = START_CHAR_DISCOVERY;
    else
      slaves.state = DISCONNECT;
    break;    
  case DISCOVERING_CHARATTERISTIC:
    PRINTF("Discovering SerialPort Service characteristics ended (aci_gatt_clt_proc_complete_event) (0x%04X).\r\n", slaves.conn_handle);
    if(slaves.tx_handle != 0)
      slaves.state = ENABLE_TX_CHAR_NOTIFICATIONS;       //abilita notifiche se necessarie
      //slaves.state = DONE_CONNESSO;
    else 
      slaves.state = DISCONNECT;
    break;
  case ENABLING_TX_CHAR_NOTIFICATIONS:
    PRINTF("Notifications for TX Charac enabled (aci_gatt_clt_proc_complete_event) (0x%04X).\r\n", slaves.conn_handle);
      slaves.state = DONE_CONNESSO;
      
      LED_STOP_TOGGLE();
      VIBRATION_START_TOGGLE(2, 100, 100);
      
    break;
   default:
    break;
  }

}


                                                                                        
void aci_att_clt_read_by_group_type_resp_event(uint16_t Connection_Handle,
                                           uint8_t Attribute_Data_Length,
                                           uint16_t Data_Length,
                                           uint8_t Attribute_Data_List[])
{
  
  switch(slaves.state){

    case DISCOVERING_SERVICES:
      if(Attribute_Data_Length == 20){ // Only 128bit UUIDs
        for(int i = 0; i < Data_Length; i += Attribute_Data_Length){
          
          // Check UUID Service 
          
          if(memcmp(&Attribute_Data_List[i+4],Service_uuid,16) == 0){
          
            memcpy(&slaves.service_start_handle, &Attribute_Data_List[i], 2);
            memcpy(&slaves.service_end_handle, &Attribute_Data_List[i+2], 2);
            
            PRINTF("Service handles: 0x%04X 0x%04X\r\n", slaves.service_start_handle, slaves.service_end_handle);
          }
          
        }
      }
    break;
    default:
    break;
   }
  
  PRINTF("[EVENT]        aci_att_clt_read_by_group_type_resp_event\r\n");
}





/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/
/* ***************** CONNESSIONE Callbacks *****************************/



/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
  
  if(Status != BLE_STATUS_SUCCESS){
    slaves.state = DISCONNECT;
    return;
  }
  
  if(Role == 0x00) { // Master role 

    slaves.address_type = Peer_Address_Type;
    memcpy(slaves.address, Peer_Address, 6);         
    slaves.conn_handle = Connection_Handle;

    slaves.state = EXCHANGE_CONFIG;               //Start procedura di discovery

  }  
  
  PRINTF("CONNESSIONE CLIENT ..OK\n");
  PRINTF("Address: %2x,%2x,%2x,%2x,%2x,%2x  \n",Peer_Address[0],Peer_Address[1],Peer_Address[2],Peer_Address[3],Peer_Address[4],Peer_Address[5]);

  
}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_le_enhanced_connection_complete_event.
 * Description    : This event indicates that a new connection has been created
 * Input          : See file bluenrg_lp_events.h
 * Output         : See file bluenrg_lp_events.h
 * Return         : See file bluenrg_lp_events.h
 *******************************************************************************/
void hci_le_enhanced_connection_complete_event(uint8_t Status,
                                               uint16_t Connection_Handle,
                                               uint8_t Role,
                                               uint8_t Peer_Address_Type,
                                               uint8_t Peer_Address[6],
                                               uint8_t Local_Resolvable_Private_Address[6],
                                               uint8_t Peer_Resolvable_Private_Address[6],
                                               uint16_t Conn_Interval,
                                               uint16_t Conn_Latency,
                                               uint16_t Supervision_Timeout,
                                               uint8_t Master_Clock_Accuracy)
{
  
  hci_le_connection_complete_event(Status,
                                   Connection_Handle,
                                   Role,
                                   Peer_Address_Type,
                                   Peer_Address,
                                   Conn_Interval,
                                   Conn_Latency,
                                   Supervision_Timeout,
                                   Master_Clock_Accuracy);

  PRINTF("CONNESSIONE AL SERVER ..COMPLETATO\n");
  PRINTF("Local_Resolvable_Private_Address : %2x,%2x,%2x,%2x,%2x,%2x  \n",Local_Resolvable_Private_Address[0],Local_Resolvable_Private_Address[1],Local_Resolvable_Private_Address[2],Local_Resolvable_Private_Address[3],Local_Resolvable_Private_Address[4],Local_Resolvable_Private_Address[5]);
  PRINTF("Peer_Resolvable_Private_Address : %2x,%2x,%2x,%2x,%2x,%2x  \n",Peer_Resolvable_Private_Address[0],Peer_Resolvable_Private_Address[1],Peer_Resolvable_Private_Address[2],Peer_Resolvable_Private_Address[3],Peer_Resolvable_Private_Address[4],Peer_Resolvable_Private_Address[5]);

}

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{

  if(Status != 0){
    return;
  }

  slaves.conn_handle = 0;
  
  slaves.state = FINISH;

  if(Reason == BLE_ERROR_TERMINATED_LOCAL_HOST){
      slaves.state = IDLE;
  }
  
  
  PRINTF("CONNESSIONE TERMINATA , %d\r\n", Reason);
  
}/* end hci_disconnection_complete_event() */


void aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                    uint8_t Status,
                                    uint8_t Reason)
{
 
    
    // Pairing as a slave
    PRINTF("Paired with master (0x%04X, 0x%02X, 0x%02X).\n", Connection_Handle, Status, Reason);    
   // aci_gap_configure_white_and_resolving_list(0x01);
    
  
  
  if(Status != 0){
    PRINTF("Pairing failed. Status %02X, Reason: %02X\r\n", Status, Reason);
    
    //aci_gap_remove_bonded_device(slaves[slave_index].address_type, slaves[slave_index].address);
    
    //aci_gap_terminate(slaves[slave_index].conn_handle, BLE_ERROR_TERMINATED_REMOTE_USER);
    
    return;
  }
  
  
  PRINTF("Pairing complete (slave).\n");
  PRINTF("Update White List\n");
  aci_gap_configure_white_and_resolving_list(0x01);

}


#define INSUFFICIENT_ENCRYPTION 0x0F

void aci_gatt_clt_error_resp_event(uint16_t Connection_Handle,
                               uint8_t Req_Opcode,
                               uint16_t Attribute_Handle,
                               uint8_t Error_Code)
{
  
  
  //PRINTF_DBG2("aci_gatt_clt_error_resp_event.\r\n");
  
  PRINTF("[EVENT]   *error*    aci_gatt_clt_error_resp_event\r\n");
  PRINTF("aci_gatt_clt_error_resp_event %04X %02X %04X %02X\n", Connection_Handle, Req_Opcode, Attribute_Handle, Error_Code);

    
  if(Error_Code == INSUFFICIENT_ENCRYPTION){
  PRINTF("INSUFFICIENT_ENCRYPTION   \r\n");

    // Start pairing
    //StartPairing(i, Connection_Handle, (MasterState)(slaves[i].state - 1)); // After pairing go one state back
    return;
  }  
}

