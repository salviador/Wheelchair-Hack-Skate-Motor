#ifndef __LEDRGB_H__
#define __LEDRGB_H__

    struct Led_queue
    {
        uint8_t   state;

        uint8_t   R;
        uint8_t   G;
        uint8_t   B;

        uint16_t  timeWait; //mS
    };

    QueueHandle_t led_Queue;


    void ledRGB_init(void);

    void ledRGB_OFF(void);
    void ledRGB_SetColor(uint32_t red , uint32_t green, uint32_t blue);

    void LedRGB_Task(void *pvParameter);



#endif