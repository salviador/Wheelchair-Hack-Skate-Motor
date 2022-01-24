

#ifndef __BLE_GATT_CLIENT_H__
#define __BLE_GATT_CLIENT_H__





typedef enum{
  IDLE = 0,
  SCAN_START,
  SCANNING,
  SCANNING_DONE,
  
  EXCHANGE_CONFIG,
  EXCHANGING_CONFIG,
  START_SERVICE_DISCOVERY,
  DISCOVERING_SERVICES,
  START_CHAR_DISCOVERY,
  DISCOVERING_CHARATTERISTIC,
  
    //Abilita notifiche se ci sono
  ENABLE_TX_CHAR_NOTIFICATIONS,
  ENABLING_TX_CHAR_NOTIFICATIONS,
  
  DONE,
  CONNECT,
  DISCONNECT,
  
  DONE_CONNESSO,
  
  FINISH,
  
} MasterState;



typedef enum{
  PAIRING_IDLE = 0,
  START_PAIRING,
  PAIRING,
  PAIRING_DONE
} MasterPairingState;


// Type of the structure used to store the state related to each server/slave
typedef struct _slave {
  uint8_t  address_type;
  uint8_t  address[6];
  uint16_t conn_handle;
  MasterState state;
  MasterState resume_state;
  MasterPairingState pairing_state;
  uint8_t is_in_pairing_mode;
  
    //sostituire nomi con i miei servizi
  uint16_t service_start_handle;
  uint16_t service_end_handle;
  
  //charatteristic uuid da aggiungere come servono
  
  uint16_t tx_handle;
  uint16_t rx_handle;
  
  
  
}slave_device;








uint8_t  BLE_GATT_Client_DeviceInit();
void APP_Tick(void);

void Connect(void);
void CancelConnect(void);
void State_DisconnectRequest(void);

void print_uuid(uint8_t *uuid);


extern uint8_t Application_Max_Attribute_Records[]; 







struct CHART_data_TX{
  int16_t uJoy_x; 
  int16_t uJoy_y;
  int16_t uvbattery;
  uint8_t buttons;
  
}__attribute__((packed));;








#endif /* _SENSOR_H_ */

