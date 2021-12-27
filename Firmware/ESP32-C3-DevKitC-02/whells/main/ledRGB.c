#include "driver/rmt.h"
#include "led_strip.h"
#include "ledRGB.h"
#include "esp_log.h"
#include "freertos/queue.h"


static const char *TAGLED = "LED_RGB";

#define RMT_TX_CHANNEL RMT_CHANNEL_0

#define EXAMPLE_CHASE_SPEED_MS (10)
#define CONFIG_EXAMPLE_STRIP_LED_NUMBER 1
#define RMT_GPIO_TX 8


led_strip_t *strip;





void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}


void ledRGB_init(void){
    ESP_LOGI(TAGLED, "ledRGB_initx");

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(8, RMT_CHANNEL_0);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(1, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAGLED, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));

}


void ledRGB_SetColor(uint32_t red , uint32_t green, uint32_t blue){
    ESP_ERROR_CHECK(strip->set_pixel(strip, 0, red, green, blue));
    ESP_ERROR_CHECK(strip->refresh(strip, 100));
}





void ledRGB_OFF(void){
    ESP_ERROR_CHECK(strip->clear(strip, 100));
}




void LedRGB_Task(void *pvParameter) {
    struct Led_queue msg;


//    QueueHandle_t *tled_Queue = ( QueueHandle_t * ) pvParameter;

    ESP_LOGI(TAGLED,"Creo Queque x LED buffer size: %d", sizeof(msg));

    //led_Queue = xQueueCreate(2, sizeof(msg));
    uint8_t   state;
    uint16_t  timeWait;

    ledRGB_init();


    while(1) { 
        if (xQueueReceive(led_Queue, (void *)&msg, portMAX_DELAY) == pdPASS ) {

            ESP_LOGI(TAGLED,"QUEQUE LED RECEIVER, now processed");

            state = msg.state;
            timeWait = msg.timeWait;
            if(state==0){
                ledRGB_OFF();
            }else
                ledRGB_SetColor(msg.R,msg.G,msg.B);
            
            ESP_LOGI(TAGLED,"state: %d, timeWait: %d", state, timeWait);
            vTaskDelay(timeWait / portTICK_RATE_MS);		

            ESP_LOGI(TAGLED, "CPU: %d", xPortGetCoreID());
            ESP_LOGI(TAGLED, "task stack: %d", uxTaskGetStackHighWaterMark(NULL));
            ESP_LOGI(TAGLED, "task heap: %d", xPortGetFreeHeapSize());
            

        }
    }
}