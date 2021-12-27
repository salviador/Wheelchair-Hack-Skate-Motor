

#ifndef __BUZZER_H__
#define  __BUZZER_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "sdkconfig.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include "esp_spi_flash.h"
    #include <esp_types.h>
    #include "esp_intr.h"    


    void buzzer_setup(void);

    void buzzer_beep(uint8_t n_beep, uint16_t  time);






#endif

