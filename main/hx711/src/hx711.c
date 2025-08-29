#include "hx711.h"

static uint8_t aditional_pulses = 1;
static int32_t offset ; 
static float scale;

static int32_t _hx711_read_raw(hx711_t *module);

static int32_t _hx711_read_raw(hx711_t *module)
{
    uint32_t count = 0;
    while(!(module->port->data_line_state(module->dt_pin) == 0));
    // if(module->port->data_line_state(module->dt_pin) == 0)
    // {
    for(uint8_t i=0 ; i<24; i++)
    {
        module->port->generate_n_pulses(1,25,0);
        count = count << 1 ;
        if(module->port->data_line_state(module->dt_pin) == 1)
        {
            count = count + 1;
        }
        module->port->generate_n_pulses(1,0,25);
    }
    module->port->generate_n_pulses(aditional_pulses,25,25);
    // }
    
    if (count & 0x800000)  // si el bit 23 (signo) está en 1
    {
        count |= 0xFF000000; // llenar los bits superiores con 1
    }

    return (int32_t) count;
}



int hx711_init(hx711_t *module, PortInterface_t *port,   uint8_t dt_pin, uint8_t clk_pin, hx711_gain_t channel_cfg)
{
    module->dt_pin = dt_pin;
    module->clk_pin = clk_pin;
    module->channel_config = channel_cfg;
    module->port = port;
    if(module->channel_config == CHANNEL_A_GAIN_128)
    {
        aditional_pulses = 1;
    }
    else if(module->channel_config == CHANNEL_B_GAIN_32)
    {
        aditional_pulses = 2;
    }
    else
    {
        aditional_pulses = 3;
    }
    return module->port->init_interface(module->dt_pin,module->clk_pin );
}


void hx711_tare(hx711_t *module, uint8_t samples)
{
    int64_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += _hx711_read_raw(module);
    }
    offset = (int32_t)(sum / samples);
}

void hx711_calibrate(hx711_t *module, float known_weight, uint8_t samples)
{
    int64_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += _hx711_read_raw(module);
    }
    int32_t value = (int32_t)(sum / samples);
    value-=offset;
    scale = (float)value / known_weight;
}

float hx711_get_weight(hx711_t *module,uint8_t samples)
{
    int64_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += _hx711_read_raw(module);
    }
    int32_t value = (int32_t)(sum / samples);
    value-=offset;
    return (float)value / scale;
}

int32_t hx711_get_offset(void)
{
    return offset;
}

void hx711_set_offset(int32_t new_offset)
{
    offset = new_offset;
}

float hx711_get_scale(void)
{
    return scale;
}


void hx711_set_scale(float new_scale)
{
    scale = new_scale;
}