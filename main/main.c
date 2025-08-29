/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hx711.h"

#define GPIO_CLOCK        0
#define GPIO_DATA         1
#define SAMPLES           20
#define PESO_CONOCIDO     213.0f
#define GAIN_AND_CHANNEL  CHANNEL_A_GAIN_128

hx711_t sensor_1;
float data;
void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(5000));
    hx711_init(&sensor_1, &ESP32_C3_Port, GPIO_DATA, GPIO_CLOCK, GAIN_AND_CHANNEL);
    ESP_LOGI("MAIN","Procediendo a encerar...");
    hx711_tare(&sensor_1, SAMPLES);
    ESP_LOGI("MAIN","Balanza encerada con el offset %ld", hx711_get_offset());
    /* Calibracion*/
    ESP_LOGI("MAIN","Procediendo a calibrar en 3 seg..");
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI("MAIN","Coloque peso conocido ahora..");
    hx711_calibrate(&sensor_1,PESO_CONOCIDO, SAMPLES);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI("MAIN","Balanza calibrada con escala %.2f", hx711_get_scale());

    while (1) {
        data = hx711_get_weight(&sensor_1, SAMPLES);
        ESP_LOGI("MAIN", "Value: %.2f gramos", data);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}