#include "stdint.h"
#include "port.h"

typedef enum
{
    CHANNEL_A_GAIN_128 = 0,
    CHANNEL_B_GAIN_32,
    CHANNEL_A_GAIN_64
}hx711_gain_t;


typedef struct 
{
    /* data */
    uint8_t dt_pin;
    uint8_t clk_pin;
    hx711_gain_t channel_config;
    PortInterface_t *port;
}hx711_t;

int hx711_init(hx711_t *module, PortInterface_t *port, uint8_t dt_pin, uint8_t clk_pin, hx711_gain_t channel_cfg);
void hx711_tare(hx711_t *module, uint8_t samples);
void hx711_calibrate(hx711_t *module, float known_weight, uint8_t samples);
float hx711_get_weight(hx711_t *module,uint8_t samples);
int32_t hx711_get_offset(void);
void hx711_set_offset(int32_t new_offset);
float hx711_get_scale(void);
void hx711_set_scale(float new_scale);
// int32_t hx711_read(hx711_t *module);