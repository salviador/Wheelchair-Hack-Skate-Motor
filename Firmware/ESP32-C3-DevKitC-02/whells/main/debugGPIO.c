#include "debugGPIO.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define GPIO_OUTPUT_DEBUG    3
#define GPIO_OUTPUT_DEBUG_PIN_SEL  (1ULL<<GPIO_OUTPUT_DEBUG)

uint8_t debugGPIOTGL;

void debugGPIO3_setup (void){
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_DEBUG_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_OUTPUT_DEBUG, 0);
    debugGPIOTGL=0;
}

void debugGPIO3_ON (void){
    gpio_set_level(GPIO_OUTPUT_DEBUG, 1);
    debugGPIOTGL=1;
}
void debugGPIO3_OFF (void){
    gpio_set_level(GPIO_OUTPUT_DEBUG, 0);
    debugGPIOTGL=0;
}
void debugGPIO3_TGL (void){
    if(debugGPIOTGL){
        gpio_set_level(GPIO_OUTPUT_DEBUG, 0);
        debugGPIOTGL=0;
    }else{
        gpio_set_level(GPIO_OUTPUT_DEBUG, 1);
        debugGPIOTGL=1;
    }
}