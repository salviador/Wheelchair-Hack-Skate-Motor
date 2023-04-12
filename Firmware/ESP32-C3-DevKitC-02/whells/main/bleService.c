/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

//http://www.iotsharing.com/2017/07/how-to-use-ble-in-arduino-esp32.html
//https://gitea.edwinclement08.com/espressif/esp-idf/src/commit/5d0eb9513055ef960407a22b7dead30560777fae/examples/bluetooth/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md
//https://neocode.jp/2020/10/13/esp32-wrover-ble-comm-custom-characteristic/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "bleService.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"

#include "ledRGB.h"
#include "wheels.h"
#include "wii.h"
#include "buzzer.h"
#include "debugGPIO.h"
#include "vesccan.h"

//#define FILTER_MAC  1
#define MULTICONNECTION
//#define BLEPASSORD
#define EXTENDED_ADVERSING_5



struct Led_queue msgLED;
struct BUZZER_queue BuzMsg;


const uint8_t MAC_REMOTO_JOYSTICK[6] =   {  0x02, 0x80, 0xe1, 0x00, 0x00, 0xe0 };
//const uint8_t MAC_REMOTO_TELEMETRIA[6] = {  0x7b, 0x63, 0x83, 0x18, 0x8c, 0x3d  };

 


struct _clientsConnection
{
    uint8_t smartphone_en;
    uint8_t smartphone_connid;
    
    uint8_t joy_en;
    uint8_t joy_connid;


    uint8_t login_connid;


    uint8_t numero_connessioni;

};
struct _clientsConnection clientsConnection;

const int BLECONNECTED_BIT   =  BIT_0;
const int BLECONNECTED_BIT_JOYSTICK   =  BIT_1;
const int BLECONNECTED_BIT_TELEMETRIA =  BIT_2;



struct JOYSTICK_BATTERY_DATA JOYSTICK_BATTERY_DATA_telemetry; 
unsigned long time_JOY_Send_Telemetry = 0;



#define GATTS_TABLE_TAG "SEC_GATTS_DEMO"

#define JOYSTICK_PROFILE_NUM                      1//1
#define JOYSTICK_PROFILE_APP_IDX                  0
#define ESP_JOYSTICK_APP_ID                      0x55
#define HEART_RATE_SVC_INST_ID                    0
#define EXT_ADV_HANDLE                            0
#define NUM_EXT_ADV_SET                           1
#define EXT_ADV_DURATION                          0
#define EXT_ADV_MAX_EVENTS                        0

#define GATTS_DEMO_CHAR_VAL_LEN_MAX               0x40

static uint16_t BLE_wheels_handle_table[HRS_IDX_NB];


struct CHART_data_TX ble_data_receiver;
struct CHART_data_TX *pble_data_receiver;


const static char DEVICE_NAME[] = {'W', 'H', 'E', 'E', 'L', 'S', ' ', 'M', 'i', 'c'};


/* Service */
const uint8_t service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf9,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};

//Usata da JOYSTICK !!!!!!!!!!!!!!!!!!
static uint8_t gatt_char_joyTX_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf8,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};
static uint8_t gatt_char_joyRX_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf7,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};


static uint8_t gatt_char_TELEMETRIA_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
        0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf6,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda
};


#ifdef EXTENDED_ADVERSING_5
//X Extend Adversing
//https://docs.silabs.com/bluetooth/latest/general/adv-and-scanning/bluetooth-adv-data-basics
static uint8_t ext_adv_raw_data[] = {
        0x02, 0x01, 0x06,
        0x11, 0x07, 
        
        //SERVICE
        // LSB <--------------------------------------------------------------------------------> MSB 
        //0x3d, 0x23, 0x33, 0xa0, 0xde, 0xf9,     0x42, 0x88,     0x30, 0x32, 0x12, 0xd7,     0x11, 0x37, 0x34, 0xda,
        service_uuid[0], service_uuid[1], service_uuid[2], service_uuid[3], service_uuid[4], service_uuid[5], service_uuid[6], service_uuid[7], service_uuid[8], service_uuid[9], service_uuid[10], service_uuid[11], service_uuid[12], service_uuid[13], service_uuid[14], service_uuid[15],

        0x0B, 0X09, 'W', 'H', 'E', 'E', 'L', 'S', ' ', 'M', 'i', 'c',
};

static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {EXT_ADV_HANDLE, EXT_ADV_DURATION, EXT_ADV_MAX_EVENTS},
};

esp_ble_gap_ext_adv_params_t ext_adv_params_2M = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = 0x20,
    .interval_max = 0x20,
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
};
#else


static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {EXT_ADV_HANDLE, EXT_ADV_DURATION, EXT_ADV_MAX_EVENTS},
};

static uint8_t adv_config_done = 0;
#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

static uint8_t raw_adv_data[] = {
        /* flags */
        0x02, 0x01, 0x06,


        /* service uuid */
        //0x03, 0x03, 0xFF, 0x00,

        //SERVICE
        0x11, 0x07, 
        // LSB <--------------------------------------------------------------------------------> MSB 
        service_uuid[0], service_uuid[1], service_uuid[2], service_uuid[3], service_uuid[4], service_uuid[5], service_uuid[6], service_uuid[7], service_uuid[8], service_uuid[9], service_uuid[10], service_uuid[11], service_uuid[12], service_uuid[13], service_uuid[14], service_uuid[15],


        /* device name */
        0x06, 0X09, 'W', 'H', 'E', 'E', 'L',

};
static uint8_t raw_scan_rsp_data[] = {  
        /* flags */
        0x02, 0x01, 0x06,
        /* tx power */
        0x02, 0x0a, 0xeb,
        //SERVICE
        0x11, 0x07, 
        // LSB <--------------------------------------------------------------------------------> MSB 
        service_uuid[0], service_uuid[1], service_uuid[2], service_uuid[3], service_uuid[4], service_uuid[5], service_uuid[6], service_uuid[7], service_uuid[8], service_uuid[9], service_uuid[10], service_uuid[11], service_uuid[12], service_uuid[13], service_uuid[14], service_uuid[15],

        0x0B, 0X09, 'W', 'H', 'E', 'E', 'L', 'S', ' ', 'M', 'i', 'c',
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x20,
    .adv_int_max         = 0x40,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
#endif /* CONFIG_SET_RAW_ADV_DATA */



struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst Joystick_profile_tab[JOYSTICK_PROFILE_NUM] = {
    [JOYSTICK_PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },

};


static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read                = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
//static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read_write_notify2   = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;


static const uint8_t char_notification_ccc[2]      = {0x00, 0x00};
static const uint8_t char_value_charRX[]                 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_value_charTX[]                 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_value_TELEMETRIA[]             = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))
#define SVC_INST_ID                 0

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, //ESP_GATT_PERM_READ_ENCRYPTED
      sizeof(service_uuid), sizeof(service_uuid), (uint8_t *)&service_uuid}},

    /* Characteristic Declaration */
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&gatt_char_joyRX_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_charRX), (uint8_t *)char_value_charRX}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(char_notification_ccc), (uint8_t *)char_notification_ccc}},

    /* Characteristic Declaration */
#ifdef BLEPASSORD
    [IDX_CHAR_B]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},
#else
    [IDX_CHAR_B]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},
#endif

    /* Characteristic Value */
    //Usata dal ------------JOYSTICK-------------
#ifdef BLEPASSORD
    [IDX_CHAR_VAL_B]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&gatt_char_joyTX_uuid, ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED, // ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_charTX), (uint8_t *)char_value_charTX}},
#else
    [IDX_CHAR_VAL_B]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&gatt_char_joyTX_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_charTX), (uint8_t *)char_value_charTX}},
#endif

    /*
    // Characteristic Declaration 
    [IDX_CHAR_C]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

    // Characteristic Value 
    [IDX_CHAR_VAL_C]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_C, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},
    */

    [IDX_CHAR_TELEMETRIA]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify2}},


    /* Characteristic Value */
    [IDX_CHAR_VAL_TELEMETRIA] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&gatt_char_TELEMETRIA_uuid, ESP_GATT_PERM_READ ,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_TELEMETRIA), (uint8_t *)char_value_TELEMETRIA}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_TELEMETRIA]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(char_notification_ccc), (uint8_t *)char_notification_ccc}},





};


static char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
   char *key_str = NULL;
   switch(key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;

   }

   return key_str;
}

//era commentato
static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_NO_BOND");

        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_BOND");
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_REQ_MITM");
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_REQ_BOND_MITM");
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_REQ_SC_ONLY");
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_REQ_SC_BOND");
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_REQ_SC_MITM");
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_LE_AUTH_REQ_SC_MITM_BOND");
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        ESP_LOGI(GATTS_TABLE_TAG, "INVALID BLE AUTH REQ");
        break;
   }

   return auth_str;
}


static void show_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    ESP_LOGI(GATTS_TABLE_TAG, "Bonded devices number : %d", dev_num);
    ESP_LOGI(GATTS_TABLE_TAG, "Bonded devices list : %d\n", dev_num);    
    for (int i = 0; i < dev_num; i++) {
        esp_log_buffer_hex(GATTS_TABLE_TAG, (void *)dev_list[i].bd_addr, sizeof(esp_bd_addr_t));
    }

    free(dev_list);
}
//fine commentato

static void __attribute__((unused)) remove_all_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {

        #ifndef EXTENDED_ADVERSING_5

            case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
                adv_config_done &= (~ADV_CONFIG_FLAG);
                if (adv_config_done == 0){
                    esp_ble_gap_start_advertising(&adv_params);
                }
                break;
            case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
                adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
                if (adv_config_done == 0){
                    esp_ble_gap_start_advertising(&adv_params);
                }
                break;

        #endif

            case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
                /* advertising start complete event to indicate advertising start successfully or failed */
                if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                    ESP_LOGE(GATTS_TABLE_TAG, "advertising start failed");
                }else{
                    ESP_LOGI(GATTS_TABLE_TAG, "advertising start successfully");
                }
                break;
            case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
                if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                    ESP_LOGE(GATTS_TABLE_TAG, "Advertising stop failed");
                }
                else {
                    ESP_LOGI(GATTS_TABLE_TAG, "Stop adv successfully\n");
                }
                break;



    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        ESP_LOGI(GATTS_TABLE_TAG,"ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT status %d",  param->ext_adv_set_params.status);
        #ifdef EXTENDED_ADVERSING_5
            esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE,  sizeof(ext_adv_raw_data), &ext_adv_raw_data[0]);
        #endif
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
         ESP_LOGI(GATTS_TABLE_TAG,"ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT status %d",  param->ext_adv_data_set.status);
         #ifdef EXTENDED_ADVERSING_5
            esp_ble_gap_ext_adv_start(NUM_EXT_ADV_SET, &ext_adv[0]);
        #endif
         break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
         ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status = %d", param->ext_adv_data_set.status);
        break;
    case ESP_GAP_BLE_ADV_TERMINATED_EVT:
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_ADV_TERMINATED_EVT, status = %d", param->adv_terminate.status);
        if(param->adv_terminate.status == 0x00) {
            ESP_LOGI(GATTS_TABLE_TAG, "ADV successfully ended with a connection being created");
        }
        break;
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
        /* Call the following function to input the passkey which is displayed on the remote device */
        esp_ble_passkey_reply(param->ble_security.ble_req.bd_addr, true, 0);
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT:                               /* BLE local IR event */
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_LOCAL_IR_EVT");
        break;
    case ESP_GAP_BLE_LOCAL_ER_EVT:                               /* BLE local ER event */
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_LOCAL_ER_EVT");
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        show the passkey number to the user to confirm it with the number displayed by peer device. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%d", param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* send the positive(true) security response to the peer device to accept the security request.
        If not accept the security request, should send the security response with negative(false) accept value*/
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);   //era commentato
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer device.
        ESP_LOGI(GATTS_TABLE_TAG, "The passkey Notify number:%06d", param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_KEY_EVT:
        //shows the ble key info share with peer device to the user.
        ESP_LOGI(GATTS_TABLE_TAG, "key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_AUTH_CMPL_EVT");
        
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTS_TABLE_TAG, "remote BD_ADDR: %08x%04x",\
                (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                (bd_addr[4] << 8) + bd_addr[5]);
        ESP_LOGI(GATTS_TABLE_TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
        ESP_LOGI(GATTS_TABLE_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if(!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(GATTS_TABLE_TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        } else {
            ESP_LOGI(GATTS_TABLE_TAG, "auth mode = %s",esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
        }
        show_bonded_devices();
        //era commentato
#ifdef BLEPASSORD
        if(param->ble_security.auth_cmpl.success){
            connected(param);
        }else{
            disconnected(param);        
        }
        
        if(param->ble_security.auth_cmpl.fail_reason==0x50)
            disconnected(param);  

#endif

        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: {
        ESP_LOGD(GATTS_TABLE_TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT status = %d", param->remove_bond_dev_cmpl.status);
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV");
        ESP_LOGI(GATTS_TABLE_TAG, "-----ESP_GAP_BLE_REMOVE_BOND_DEV----");
        esp_log_buffer_hex(GATTS_TABLE_TAG, (void *)param->remove_bond_dev_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTS_TABLE_TAG, "------------------------------------");
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT, tatus = %x", param->local_privacy_cmpl.status);
        #ifdef EXTENDED_ADVERSING_5
            esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params_2M);
        #endif
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
         ESP_LOGI(GATTS_TABLE_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_REG_EVT");

            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(DEVICE_NAME);
            if (set_dev_name_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "set device name failed, error code = %x", set_dev_name_ret);
            }


            #ifndef EXTENDED_ADVERSING_5

                esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
                if (raw_adv_ret){
                    ESP_LOGE(GATTS_TABLE_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
                }
                adv_config_done |= ADV_CONFIG_FLAG;
                /*esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
                if (raw_scan_ret){
                    ESP_LOGE(GATTS_TABLE_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
                }
                adv_config_done |= SCAN_RSP_CONFIG_FLAG;
                */
            #endif


            //generate a resolvable random address
            esp_ble_gap_config_local_privacy(true);
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
            break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_READ_EVT");

            // ***************** Qui aggiorno le variabili su richiesta di LEttura del clinet
            /*
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = 6;
            rsp.attr_value.value[0] = 0xF6;
            rsp.attr_value.value[1] = 0xF7;
            rsp.attr_value.value[2] = 0xF8;
            rsp.attr_value.value[3] = 0xF9;
            rsp.attr_value.value[4] = 0xF9;
            rsp.attr_value.value[5] = 0xA3;
            
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,ESP_GATT_OK, &rsp);
            */

            break;
        case ESP_GATTS_WRITE_EVT:
            if(!wii_IS_present()){
                if(clientsConnection.joy_en){
                    if( BLE_wheels_handle_table[IDX_CHAR_VAL_B] == param->write.handle && param->write.len == sizeof(ble_data_receiver) ) {
                        memcpy(&ble_data_receiver, param->write.value, param->write.len);
                        //xQueueSend(xQueueJOYData, &ble_data_receiver, (15 / portTICK_PERIOD_MS) );
                        xQueueOverwrite(xQueueJOYData, &ble_data_receiver);

                        

                        //Aggiorna la Telemetria del Joystick(Battery Level) sulla APP Android

                        if((pdTICKS_TO_MS(xTaskGetTickCount() - time_JOY_Send_Telemetry)) > 2000){
                            time_JOY_Send_Telemetry = xTaskGetTickCount();

                            //ESP_LOGI(GATTS_TABLE_TAG, "time_JOY_Send_Telemetry **UPDATE**");

                            JOYSTICK_BATTERY_DATA_telemetry.type_message = 't';
                            JOYSTICK_BATTERY_DATA_telemetry.Battery = ble_data_receiver.uvbattery;
                            if(BLE_SendNotification_Queue != NULL){
                                if(xMessageBufferSpacesAvailable(BLE_SendNotification_Queue) > sizeof(JOYSTICK_BATTERY_DATA_telemetry)){
                                    uint8_t * bytePtr = (uint8_t*)&JOYSTICK_BATTERY_DATA_telemetry;
                                    size_t xBytesSent = xMessageBufferSend(BLE_SendNotification_Queue, bytePtr, sizeof(JOYSTICK_BATTERY_DATA_telemetry) , 0);
                                }
                            }

                        }





                    }
                }
            }
/*
            //uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
            if ((param->write.value[1]<<8 | param->write.value[0]) == 0x0001){
                        ESP_LOGI(GATTS_TABLE_TAG, "notify enable");
                        uint8_t notify_data[1] = {2};
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, Joystick_profile_tab[JOYSTICK_PROFILE_APP_IDX].char_handle,
                                                sizeof(notify_data), notify_data, false);
            }
*/
/*
      //      esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id,ESP_GATT_OK, NULL);

            //if (param->write.handle== gl_profile_tab[0].char_handle) {  //oppure   BLE_wheels_handle_table[IDX_CHAR_VAL_A]

 
            if(param->write.len == sizeof(ble_data_receiver)){
                memcpy(&ble_data_receiver, param->write.value, param->write.len);
                //ESP_LOGI(GATTS_TABLE_TAG, "X= %d, Y= %d  BATT= %d", ble_data_receiver.uJoy_x,ble_data_receiver.uJoy_y,ble_data_receiver.uvbattery );
                    //debugGPIO3_ON();
                //Invia Dati del joystick al wheels.c
                    //xQueueOverwrite(xQueueJOYData, &ble_data_receiver);

    xQueueSend(xQueueJOYData, &ble_data_receiver, (15 / portTICK_PERIOD_MS) );

                    //debugGPIO3_TGL();

            }else{
                ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_WRITE_EVT, write value: ");
                esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
            }
*/



            //Qui Ho la lettura da dati che arrivano dal client

            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_EXEC_WRITE_EVT, write value:");
            esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
            break;
        case ESP_GATTS_MTU_EVT:
            break;
        case ESP_GATTS_CONF_EVT:
            break;
        case ESP_GATTS_UNREG_EVT:
            break;
        case ESP_GATTS_DELETE_EVT:
            break;
        case ESP_GATTS_START_EVT:
            break;
        case ESP_GATTS_STOP_EVT:
            break;
        case ESP_GATTS_CONNECT_EVT:
#ifdef BLEPASSORD
            clientsConnection.login_connid = param->connect.conn_id;
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT ID %d",param->connect.conn_id);
            esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_NO_MITM); // ESP_BLE_SEC_ENCRYPT_MITM);
#else
            connected(gatts_if , param);
#endif        
/*
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT ID %d",param->connect.conn_id);

            ESP_LOGI(GATTS_TABLE_TAG, "CONNECT ADDDDDRESS= 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x",
                         param->connect.remote_bda[0], param->connect.remote_bda[1], 
                         param->connect.remote_bda[2], param->connect.remote_bda[3],
                         param->connect.remote_bda[4], param->connect.remote_bda[5]);



            if( memcmp(param->connect.remote_bda, MAC_REMOTO_TELEMETRIA, 6) == 0 ){
                clientsConnection.smartphone_en = 1;
                clientsConnection.smartphone_connid = param->connect.conn_id;

                xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT_TELEMETRIA);


                ESP_LOGI(GATTS_TABLE_TAG, "SMARTHPHONE ******COLLEGATO******");
            }else if( memcmp(param->connect.remote_bda, MAC_REMOTO_JOYSTICK, 6) == 0 ){
                clientsConnection.joy_en = 1;
                clientsConnection.joy_connid = param->connect.conn_id;

                xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT_JOYSTICK);

                ESP_LOGI(GATTS_TABLE_TAG, "JOYSTICK ******COLLEGATO******");
            }


#ifdef FILTER_MAC
                if(param->connect.remote_bda[0] == 0x02 && 
                    param->connect.remote_bda[1] == 0x80 && 
                    param->connect.remote_bda[2] == 0xE1 && 
                    param->connect.remote_bda[3] == 0x00 && 
                    param->connect.remote_bda[4] == 0x00 && 
                    param->connect.remote_bda[5] == 0xE0){
#endif

                    //MAC UGUALE ******CONNETTI******

//                    xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT);

                    BuzMsg.state = 1;
                    BuzMsg.timeWaitON = 90;
                    BuzMsg.timeWaitOFF = 250;
                    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);

                    //Joystick_profile_tab[JOYSTICK_PROFILE_APP_IDX].conn_id = param->connect.conn_id;
                    
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 0;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 0;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 10;
                    msgLED.timeWait = 10;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
#ifdef FILTER_MAC
            }else{

                ESP_LOGI(GATTS_TABLE_TAG, "**** RIFIUTO CONNESSIONEE ****");
                ESP_LOGI(GATTS_TABLE_TAG, "**** RIFIUTO CONNESSIONEE ****");
                ESP_LOGI(GATTS_TABLE_TAG, "**** RIFIUTO CONNESSIONEE ****");

                //Rifiuta connessione
                esp_ble_gatts_close(gatts_if, param->connect.conn_id);
            }
#endif

#ifdef MULTICONNECTION
                //rescanning for multiple connection
                esp_ble_gap_ext_adv_start(NUM_EXT_ADV_SET, &ext_adv[0]);
#endif

         //   esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
*/
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);

            /*if(param->disconnect.reason == 0x16){
                disconnected(param);
            }*/


            esp_ble_gatts_close(gatts_if, clientsConnection.login_connid);
            clientsConnection.login_connid = 0xFF;

            if(param->disconnect.reason < 300){
                disconnected(param);
            }
/*
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
 	        //Joystick_profile_tab[JOYSTICK_PROFILE_APP_IDX].conn_id = 0;

            if( clientsConnection.smartphone_connid == param->connect.conn_id ){
                clientsConnection.smartphone_en = 0;
                clientsConnection.smartphone_connid = 0xFF;
                xEventGroupClearBits(BLE_event_group, BLECONNECTED_BIT_TELEMETRIA);
                ESP_LOGI(GATTS_TABLE_TAG, "SMARTHPHONE ******S-COLLEGATO******");
            }else if( clientsConnection.joy_connid ==  param->connect.conn_id ){
                clientsConnection.joy_en = 0;
                clientsConnection.joy_connid = 0xFF;
                xEventGroupClearBits(BLE_event_group, BLECONNECTED_BIT_JOYSTICK);
                ESP_LOGI(GATTS_TABLE_TAG, "JOYSTICK ******S-COLLEGATO******");
            }


            #ifdef EXTENDED_ADVERSING_5
                esp_ble_gap_ext_adv_start(NUM_EXT_ADV_SET, &ext_adv[0]);
            #else
                esp_ble_gap_start_advertising(&adv_params);           
            #endif


           // xEventGroupClearBits(BLE_event_group, BLECONNECTED_BIT);

            BuzMsg.state = 1;
            BuzMsg.timeWaitON = 100;
            BuzMsg.timeWaitOFF = 150;
            xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);

            msgLED.state = 1;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 0;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 1;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 0;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 1;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 0;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
*/

            break;
        case ESP_GATTS_OPEN_EVT:
            break;
        case ESP_GATTS_CANCEL_OPEN_EVT:
            break;
        case ESP_GATTS_CLOSE_EVT:
            break;
        case ESP_GATTS_LISTEN_EVT:
            break;
        case ESP_GATTS_CONGEST_EVT:
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            ESP_LOGI(GATTS_TABLE_TAG, "The number handle = %x",param->add_attr_tab.num_handle);
            if (param->create.status == ESP_GATT_OK){
                if(param->add_attr_tab.num_handle == HRS_IDX_NB) {
                    memcpy(BLE_wheels_handle_table, param->add_attr_tab.handles,
                    sizeof(BLE_wheels_handle_table));
                   esp_ble_gatts_start_service(BLE_wheels_handle_table[IDX_SVC]);
                }else{
                    ESP_LOGE(GATTS_TABLE_TAG, "Create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
                         param->add_attr_tab.num_handle, HRS_IDX_NB);
                }
            }else{
                ESP_LOGE(GATTS_TABLE_TAG, " Create attribute table failed, error code = %x", param->create.status);
            }
        break;
    }

        default:
           break;
    }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            Joystick_profile_tab[JOYSTICK_PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TABLE_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < JOYSTICK_PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == Joystick_profile_tab[idx].gatts_if) {
                if (Joystick_profile_tab[idx].gatts_cb) {
                    Joystick_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}


void BLE_Service_Start(void)
{
    esp_err_t ret;


    clientsConnection.smartphone_en = 0;
    clientsConnection.joy_en = 0;
    clientsConnection.smartphone_connid = 0xFF;
    clientsConnection.joy_connid = 0xFF;

    clientsConnection.numero_connessioni = 0;


    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(GATTS_TABLE_TAG, "%s init bluetooth", __func__);
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gap register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gatts_app_register(ESP_JOYSTICK_APP_ID);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

  
/*  
    uint8_t ic=0;

    while(1){



        uint8_t data[] = {ic,0x44};

        
        
        esp_ble_gatts_send_indicate(Joystick_profile_tab[0].gatts_if, Joystick_profile_tab[0].conn_id, BLE_wheels_handle_table[IDX_CHAR_VAL_A], 2, data, false);

        ic++;


        vTaskDelay(2000 / portTICK_PERIOD_MS);



    }

*/
/*
    while(1){

        ledRGB_SetColor(255,0,0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        ledRGB_SetColor(255,255,0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        ledRGB_SetColor(255,0,255);
        vTaskDelay(2000 / portTICK_PERIOD_MS);







    }

*/






    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    //era commentato
#ifdef BLEPASSORD
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_ONLY; // ESP_BLE_SEC_ENCRYPT_MITM; //ESP_LE_AUTH_REQ_SC_ONLY;                       // ESP_LE_AUTH_REQ_SC_BOND;// ESP_LE_AUTH_REQ_SC_MITM_BOND; // ESP_LE_AUTH_NO_BOND; //  ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;//ESP_IO_CAP_NONE; //ESP_IO_CAP_OUT; //ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    //set static passkey
    uint32_t passkey = 123456;
    uint8_t auth_option =  ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE;   //ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;



/*
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_ONLY;                       // ESP_LE_AUTH_REQ_SC_BOND;// ESP_LE_AUTH_REQ_SC_MITM_BOND; // ESP_LE_AUTH_NO_BOND; //  ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT; //ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    //set static passkey
    uint32_t passkey = 123456;
    uint8_t auth_option =  ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE;   //ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
  */  
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
#endif

    /* If your BLE device acts as a Slave, the init_key means you hope which types of key of the master should distribute to you,
    and the response key means which key you can distribute to the master;
    If your BLE device acts as a master, the response key means you hope which types of key of the slave should distribute to you,
    and the init key means which key you can distribute to the slave. */
    /*
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
    */
    /* Just show how to clear all the bonded devices
     * Delay 30s, clear all the bonded devices
     *
     * vTaskDelay(30000 / portTICK_PERIOD_MS);
     * remove_all_bonded_devices();
     */

    BLE_event_group = xEventGroupCreate();
    
    //BLE_SendNotification_Queue = xQueueCreate( 1, 32 ); //sizeof( uint32_t )
    BLE_SendNotification_Queue = xMessageBufferCreate( 100 );

    xTaskCreate(&Send_BLE_Notification_Task, "BLE Send Telemetry", 4096, NULL, tskIDLE_PRIORITY, NULL);

//per test
    //xTaskCreate(BLE_Send_Data, "SEND", 4096, NULL, tskIDLE_PRIORITY, NULL);


}




void connected(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param){

        if(clientsConnection.numero_connessioni<2){

            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT ID %d",param->connect.conn_id);

            ESP_LOGI(GATTS_TABLE_TAG, "CONNECT ADDDDDRESS= 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x",
                         param->connect.remote_bda[0], param->connect.remote_bda[1], 
                         param->connect.remote_bda[2], param->connect.remote_bda[3],
                         param->connect.remote_bda[4], param->connect.remote_bda[5]);


            if( memcmp(param->connect.remote_bda, MAC_REMOTO_JOYSTICK, 6) == 0 ){
                clientsConnection.joy_en = 1;
                clientsConnection.joy_connid = param->connect.conn_id;

                xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT_JOYSTICK);
                xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT);

                ESP_LOGI(GATTS_TABLE_TAG, "JOYSTICK ******COLLEGATO******");
            }else{ //if( memcmp(param->connect.remote_bda, MAC_REMOTO_TELEMETRIA, 6) == 0 ){
                clientsConnection.smartphone_en = 1;
                clientsConnection.smartphone_connid = param->connect.conn_id;

                xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT_TELEMETRIA);
                xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT);

                ESP_LOGI(GATTS_TABLE_TAG, "SMARTHPHONE ******COLLEGATO******");
            } 

                clientsConnection.numero_connessioni++;

                    //MAC UGUALE ******CONNETTI******

//                    xEventGroupSetBits(BLE_event_group, BLECONNECTED_BIT);

                    BuzMsg.state = 1;
                    BuzMsg.timeWaitON = 90;
                    BuzMsg.timeWaitOFF = 250;
                    xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);

                    //Joystick_profile_tab[JOYSTICK_PROFILE_APP_IDX].conn_id = param->connect.conn_id;
                    
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 0;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 0;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 255;
                    msgLED.timeWait = 1000;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 10;
                    msgLED.timeWait = 10;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);

#ifdef MULTICONNECTION
                //rescanning for multiple connection
            #ifdef EXTENDED_ADVERSING_5
                esp_ble_gap_ext_adv_start(NUM_EXT_ADV_SET, &ext_adv[0]);
            #else
                esp_ble_gap_start_advertising(&adv_params);           
            #endif

#endif

            }else{
                esp_ble_gatts_close(gatts_if, param->connect.conn_id);
            }
}

void disconnected(esp_ble_gatts_cb_param_t *param){
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
 	        //Joystick_profile_tab[JOYSTICK_PROFILE_APP_IDX].conn_id = 0;

            if( clientsConnection.joy_connid ==  param->connect.conn_id ){
                clientsConnection.joy_en = 0;
                clientsConnection.joy_connid = 0xFF;
                xEventGroupClearBits(BLE_event_group, BLECONNECTED_BIT_JOYSTICK);
                ESP_LOGI(GATTS_TABLE_TAG, "JOYSTICK ******S-COLLEGATO******");
            }else { //(clientsConnection.smartphone_connid == param->connect.conn_id ){
                clientsConnection.smartphone_en = 0;
                clientsConnection.smartphone_connid = 0xFF;
                xEventGroupClearBits(BLE_event_group, BLECONNECTED_BIT_TELEMETRIA);
                ESP_LOGI(GATTS_TABLE_TAG, "SMARTHPHONE ******S-COLLEGATO******");
            }
            if(clientsConnection.numero_connessioni>0){
                clientsConnection.numero_connessioni--;
            }

            if(( clientsConnection.smartphone_en == 0 )&&
                ( clientsConnection.joy_en == 0 ))
            {
                    xEventGroupClearBits(BLE_event_group, BLECONNECTED_BIT);
            }

            #ifdef EXTENDED_ADVERSING_5
                esp_ble_gap_ext_adv_start(NUM_EXT_ADV_SET, &ext_adv[0]);
            #else
                esp_ble_gap_start_advertising(&adv_params);           
            #endif



            BuzMsg.state = 1;
            BuzMsg.timeWaitON = 100;
            BuzMsg.timeWaitOFF = 150;
            xQueueSend(buzzer_Queue, &BuzMsg, portMAX_DELAY);

            msgLED.state = 1;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 0;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 1;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 0;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 1;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);
            msgLED.state = 0;
            msgLED.R = 255;
            msgLED.G = 0;
            msgLED.B = 0;
            msgLED.timeWait = 100;
            xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);


            if(( clientsConnection.smartphone_en == 1 )||
                ( clientsConnection.joy_en == 1 ))
            {
                    msgLED.state = 1;
                    msgLED.R = 0;
                    msgLED.G = 0;
                    msgLED.B = 10;
                    msgLED.timeWait = 10;
                    xQueueSend(led_Queue, (void *)&msgLED, portMAX_DELAY);

            }
}









//MAX20 byte !!!!!!!!!!!!!!!!!!!!

void Send_BLE_Notification_Task(void *pvParameter) {
    uint8_t data[101];
  //  uint8_t message_type;
  //  uint8_t message_len;
    size_t xReceivedBytes;

    while(1){

        //if (xQueueReceive(BLE_SendNotification_Queue, &data, portMAX_DELAY) == pdPASS ) {
        xReceivedBytes = xMessageBufferReceive(BLE_SendNotification_Queue,
                                     &data[0], 100 ,portMAX_DELAY);

            ESP_LOGI(GATTS_TABLE_TAG, "SENDDDD DaTa To BLE --> Ricevuto : %d ", xReceivedBytes);
           // ESP_LOGI(GATTS_TABLE_TAG, "task stack: %d", uxTaskGetStackHighWaterMark(NULL));
           // ESP_LOGI(GATTS_TABLE_TAG, "task heap: %d", xPortGetFreeHeapSize());

        if(xReceivedBytes > 1) {

            EventBits_t FLAG_CONNESSIONI = xEventGroupGetBits(BLE_event_group);

            ESP_LOGI(GATTS_TABLE_TAG, "%x", FLAG_CONNESSIONI);


            //if(IS_BLE_CONNECT & BLECONNECTED_BIT_TELEMETRIA){
           if((FLAG_CONNESSIONI & BLECONNECTED_BIT)&&
            (FLAG_CONNESSIONI & BLECONNECTED_BIT_TELEMETRIA))
           {
                ESP_LOGI(GATTS_TABLE_TAG, "SENDDDD DaTa To BLE --> Ricevuto : %d", xReceivedBytes);
    
//                message_type = data[0];
//                message_len = data[1];

                //esp_ble_gatts_send_indicate(Joystick_profile_tab[0].gatts_if, Joystick_profile_tab[0].conn_id, BLE_wheels_handle_table[IDX_CHAR_VAL_TELEMETRIA], 2, data, false);
                //MAX20 byte !!!!!!!!!!!!!!!!!!!!
                esp_ble_gatts_send_indicate(Joystick_profile_tab[0].gatts_if, clientsConnection.smartphone_connid, BLE_wheels_handle_table[IDX_CHAR_VAL_TELEMETRIA], xReceivedBytes, &data[0], false);
            
                ESP_LOGI(GATTS_TABLE_TAG, "-----");

            
            }

        }
    }

}



/*
//long timeTDBGelaspe=0;
//Send 4 byte
void BLE_Send_Data (void *pvParameter){
    size_t xBytesSent;

    struct VESC_DATACAN_CAN_PACKET_STATUS_1 VL1;
    struct VESC_DATACAN_CAN_PACKET_STATUS_1 VL2;
    //uint8_t databuffer[sizeof(VSC_DATA_Left)];
    uint8_t i=0, *bytePtr;


    while(1){

        if(i==0){
            VL1.type_message = 'a';
            VL1.RPM = 158;
            VL1.Current = 1.535;
            VL1.Duty_Motor = 84.2;
            bytePtr = (uint8_t*)&VL1;
            xBytesSent = xMessageBufferSend(BLE_SendNotification_Queue, bytePtr, sizeof(VL1) , 0);
            i = 1;
        }else if(i==1){
            VL2.type_message = 'b';
            VL2.RPM = 350;
            VL2.Current = 1.835;
            VL2.Duty_Motor = 45.5;
            bytePtr = (uint8_t*)&VL2;
            xBytesSent = xMessageBufferSend(BLE_SendNotification_Queue, bytePtr, sizeof(VL2) , 0);
            i = 0;
        }



//            xBytesSent = xMessageBufferSend(BLE_SendNotification_Queue, (uint8_t*)&data[0], 20 , 0);
            ESP_LOGI(GATTS_TABLE_TAG, "*** %d",xBytesSent );

         //   ESP_LOGI(GATTS_TABLE_TAG, "task stack: %d", uxTaskGetStackHighWaterMark(NULL));
         //   ESP_LOGI(GATTS_TABLE_TAG, "task heap: %d", xPortGetFreeHeapSize());

       vTaskDelay(1000 / portTICK_PERIOD_MS);



    }


}


*/