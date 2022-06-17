#ifndef __WII_H__
#define  __WII_H__

    #include <stdio.h>


    struct  WII_VAR
    {
        uint8_t x;
        uint8_t y;
        bool btn_z;
        bool btn_c;
    };
    






    void wii_setup(void);

    void wii_init(void);

    bool wii_IS_present(void);

    esp_err_t wii_update(struct  WII_VAR *data);

    void wii_task(void *arg);



#endif

