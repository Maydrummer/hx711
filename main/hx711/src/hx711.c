#include "hx711.h"

static uint8_t aditional_pulses = 1;

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

int32_t hx711_read(hx711_t *module)
{
    uint32_t count = 0;
    if(module->port->data_line_state(module->dt_pin) == 0)
    {
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
    }
    
    if (count & 0x800000)  // si el bit 23 (signo) está en 1
    {
        count |= 0xFF000000; // llenar los bits superiores con 1
    }

    return (int32_t) count;
}

