/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/gpio.h"


#define GPIO_CLOCK        0
#define GPIO_DATA         1


/* INIT */

// Global channel handle
rmt_channel_handle_t tx_chan = NULL;

// Configuration
rmt_tx_channel_config_t tx_chan_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .gpio_num = 0,                    // Change to your desired GPIO
    .mem_block_symbols = 64,
    .resolution_hz = 1 * 1000 * 1000, // 1 MHz = 1 µs per tick
    .trans_queue_depth = 4,
    .flags.invert_out = false,
    .flags.with_dma = false,
};

gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = (1ULL<<GPIO_DATA),
    .pull_down_en = 0,
    .pull_up_en = 0,
};


// Function to transmit exact number of pulses
// Versión optimizada para memoria (si necesitas muchos pulsos)
esp_err_t transmit_exact_pulses_efficient(uint32_t pulse_count, uint32_t high_time_us, uint32_t low_time_us)
{
    // Crear un solo pulso y usar loop_count - 1
    // Luego añadir el último pulso manualmente
    
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

    rmt_tx_wait_all_done(tx_chan, portMAX_DELAY);
    rmt_del_encoder(copy_encoder);

    return ret;
}

void prueba_lectura(void)
{
    uint32_t count = 0;
    if(gpio_get_level(GPIO_DATA) == 0)
    {
        for(uint8_t i=0 ; i<24; i++)
        {
            transmit_exact_pulses_efficient(1, 25, 0);
            count = count << 1 ;
            if (gpio_get_level(GPIO_DATA) == 1)
            {
                count = count + 1;
            }
            transmit_exact_pulses_efficient(1, 0, 25);
        }
        transmit_exact_pulses_efficient(1, 25, 25);
    }
    ESP_LOGI("main","LECTURA: %ld", count);
}

void app_main(void)
{
    // Initialize RMT TX channel
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    
    while (1) {
        prueba_lectura();
        // if(gpio_get_level(GPIO_DATA) == 0)
        // {
        //     for(uint8_t i=0 ; i<25; i++)
        //     {
        //         transmit_exact_pulses_efficient(1, 25, 25);
        //     }
        // }
        // Example: Transmit 5 pulses with 50µs high time and 50µs low time (50% duty cycle)
        
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}