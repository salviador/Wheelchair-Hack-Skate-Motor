#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "wii.h"
#include "wheels.h"


static const char *TAGWII = "WII I2C";

i2c_cmd_handle_t i2chandle;

bool wii_present = false; 

uint8_t nunchuk_data[10];
bool wii_SecurityERROR = false;


bool wii_IS_present(void){
    return wii_present;
}


void wii_setup(void){

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 5,
        .scl_io_num = 6,
        .sda_pullup_en = GPIO_PULLUP_ENABLE, //GPIO_PULLUP_DISABLE, //GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE, //GPIO_PULLUP_DISABLE, //GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };

    ESP_ERROR_CHECK(i2c_param_config(0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(0, I2C_MODE_MASTER, 0, 0, 0));
    ESP_LOGI(TAGWII, "I2C itialized successfully");

   wii_init();
}


void wii_init(void){
    esp_err_t erri;

    wii_present = false;
    
    //Wii Init 1
    i2chandle = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(i2chandle));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xA4, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xF0, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0x55, true));
    ESP_ERROR_CHECK(i2c_master_stop(i2chandle));
    erri = i2c_master_cmd_begin(0, i2chandle, 10 / portTICK_RATE_MS);
    i2c_cmd_link_delete(i2chandle);
    //ESP_LOGI(TAGWII, "%s", esp_err_to_name(erri));
    if(erri != ESP_OK){
        ESP_LOGI(TAGWII, "Wii NoN Presente");
        //wii non presente
        wii_present = false;
        return;
    }

    //Wii Init 2
    i2chandle = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(i2chandle));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xA4, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xFB, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0x00, true));
    ESP_ERROR_CHECK(i2c_master_stop(i2chandle));
    erri = i2c_master_cmd_begin(0, i2chandle, 10 / portTICK_RATE_MS);
    i2c_cmd_link_delete(i2chandle);
    //ESP_LOGI(TAGWII, "%s", esp_err_to_name(erri));
    if(erri != ESP_OK){
        ESP_LOGI(TAGWII, "Wii NoN Presente");
        //wii non presente
        wii_present = false;
        return;
    }


    //Wii Init 3
    i2chandle = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(i2chandle));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xA4 , true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xFA, true));
    ESP_ERROR_CHECK(i2c_master_stop(i2chandle));
    erri = i2c_master_cmd_begin(0, i2chandle, 10 / portTICK_RATE_MS);
    if(erri != ESP_OK){
        ESP_LOGI(TAGWII, "Wii NoN Presente");
        //wii non presente
        wii_present = false;
        return;
    }
    i2c_cmd_link_delete(i2chandle);
    erri = i2c_master_read_from_device(0, 0x52, &nunchuk_data[0], 6, 10 / portTICK_RATE_MS);
    if(erri != ESP_OK){
        ESP_LOGI(TAGWII, "Wii NoN Presente");
        //wii non presente
        wii_present = false;
        return;
    }

    wii_present = true;
    wii_SecurityERROR = false;

    xTaskCreatePinnedToCore(wii_task, "WII", 4096, NULL, tskIDLE_PRIORITY, NULL, tskIDLE_PRIORITY+4);
}


esp_err_t wii_update(struct  WII_VAR *data){
    esp_err_t erri;
    //Wii READ
    i2chandle = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(i2chandle));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0xA4 , true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2chandle, 0x00, true));
    ESP_ERROR_CHECK(i2c_master_stop(i2chandle));
    erri = i2c_master_cmd_begin(0, i2chandle, 10 / portTICK_RATE_MS);
    i2c_cmd_link_delete(i2chandle);
    erri = i2c_master_read_from_device(0, 0x52, &nunchuk_data[0], 6, 10 / portTICK_RATE_MS);

    return erri;
}



void wii_task(void *arg)
{
    struct WII_VAR wiidata;
    struct CHART_data_TX dataToSend;

    wii_SecurityERROR = false;

    ESP_LOGI(TAGWII, "******WIIII TASK*******");

    while(1) {

        if (wii_update(&wiidata) == ESP_OK){
            //Decodifica
            if(!wii_SecurityERROR){
                //Decodifica
                wiidata.x = nunchuk_data[0];
                wiidata.y = nunchuk_data[1];
                wiidata.btn_c = !((nunchuk_data[5] >> 1) & 0x01);
                wiidata.btn_z = !(nunchuk_data[5] & 0x01);
            }else{
                wiidata.x = 127;
                wiidata.y = 127;
                wiidata.btn_z = false;
                wiidata.btn_c = false;
            }
        }else{
            wiidata.x = 127;
            wiidata.y = 127;
            wiidata.btn_z = false;
            wiidata.btn_c = false;
            wii_SecurityERROR = true;
        }

        //mappa 0-255  ---> -1000  +1000
        dataToSend.uJoy_x = map(wiidata.x, 0, 255, -1000, 1000);
        dataToSend.uJoy_y = map(wiidata.y, 0, 255, -1000, 1000);
        dataToSend.uvbattery = 3300;

        if(wiidata.btn_z)
            dataToSend.buttons = dataToSend.buttons | 0x01;
        else
            dataToSend.buttons = dataToSend.buttons & 0xFE;

        if(wiidata.btn_c)
            dataToSend.buttons = dataToSend.buttons | 0x02;
        else
            dataToSend.buttons = dataToSend.buttons & 0xFD;


        //ESP_LOGI("DATA", "%d ,%d ,%d ,%d" ,wiidata.x,wiidata.y,wiidata.btn_c,wiidata.btn_z);

        //ESP_LOGI("DATA", "%d ,%d ,%d ,%d" ,wiidata.x,wiidata.y,dataToSend.uJoy_x,dataToSend.uJoy_y);

        xQueueOverwrite(xQueueJOYData, &dataToSend);

        vTaskDelay(70 / portTICK_PERIOD_MS);
    }
}







