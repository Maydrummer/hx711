#include "stdint.h"

typedef struct
{
    int (*init_interface)(uint8_t data_pin, uint8_t clock_pin);
    int (*data_line_state)(uint8_t gpio);
    int (*generate_n_pulses)(uint32_t pulse_count, uint32_t high_time_us, uint32_t low_time_us);
}PortInterface_t;

extern PortInterface_t ESP32_C3_Port; 
// int setup_sensor(uint8_t data_pin, uint8_t clock_pin);
// int gpio_state(uint8_t gpio);
// int generate_n_pulses(uint32_t pulse_count, uint32_t high_time_us, uint32_t low_time_us);