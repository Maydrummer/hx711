#include "port.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/gpio.h"

/* Configuraciones segund ESP32 C3*/
rmt_channel_handle_t tx_chan = NULL;

static esp_err_t config_rmt_channel(gpio_num_t gpio);
static esp_err_t config_gpio_data(gpio_num_t gpio);
esp_err_t transmit_exact_pulses_efficient(uint32_t pulse_count, uint32_t high_time_us, uint32_t low_time_us);

static esp_err_t config_rmt_channel(gpio_num_t gpio)
{
    esp_err_t ret;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio,                    // Change to your desired GPIO
        .mem_block_symbols = 64,
        .resolution_hz = 1 * 1000 * 1000, // 1 MHz = 1 µs per tick
        .trans_queue_depth = 4,
        .flags.invert_out = false,
        .flags.with_dma = false,
    };
    ret = rmt_new_tx_channel(&tx_chan_config, &tx_chan);
    if(ret!=ESP_OK)
    {   
        return ret;
    }
    ret = rmt_enable(tx_chan);
    if(ret!=ESP_OK)
    {   
        return ret;
    }

    return ESP_OK;
}

static esp_err_t config_gpio_data(gpio_num_t gpio)
{
    esp_err_t ret;
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<<gpio),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    ret = gpio_config(&io_conf);
    if(ret!=ESP_OK)
    {   
        return ret;
    }
    return ESP_OK;
}

esp_err_t transmit_exact_pulses_efficient(uint32_t pulse_count, uint32_t high_time_us, uint32_t low_time_us)
{
    rmt_symbol_word_t single_pulse[2] = {
        {.level0 = 1, .duration0 = high_time_us, .level1 = 1, .duration1 = 0},
        {.level0 = 0, .duration0 = low_time_us, .level1 = 0, .duration1 = 0}
    };

    rmt_copy_encoder_config_t copy_encoder_config = {};
    rmt_encoder_handle_t copy_encoder = NULL;
    rmt_new_copy_encoder(&copy_encoder_config, &copy_encoder);

    rmt_transmit_config_t transmit_config = {
        .loop_count = pulse_count - 1,  // Repetir (n-1) veces
        .flags.eot_level = 0,
    };

    // Transmitir (n-1) pulsos con loop
    esp_err_t ret = rmt_transmit(tx_chan, copy_encoder, single_pulse, 
                                sizeof(single_pulse), &transmit_config);

    rmt_tx_wait_all_done(tx_chan, 0xffffffffUL);
    rmt_del_encoder(copy_encoder);

    return ret;
}

/* Public functions */
int setup_sensor(uint8_t data_pin, uint8_t clock_pin)
{
    int ack = 1;
    esp_err_t ret = config_rmt_channel(clock_pin);
    if(ret!=ESP_OK)
    {
        ack= 0;
        return ack;
    }
    ret = config_gpio_data(data_pin);
    if(ret!=ESP_OK)
    {
        ack= 0;
        return ack;
    }
    return ack;
}

int gpio_state(uint8_t gpio)
{
    return gpio_get_level(gpio);
}


int generate_n_pulses(uint32_t pulse_count, uint32_t high_time_us, uint32_t low_time_us)
{
    int ack = 1;
    esp_err_t ret = transmit_exact_pulses_efficient(pulse_count,high_time_us,low_time_us);
    if(ret!=ESP_OK)
    {
        ack= 0;
        return ack;
    }
    return ack;
}