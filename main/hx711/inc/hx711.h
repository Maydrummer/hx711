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
int32_t hx711_read(hx711_t *module);