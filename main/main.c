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
#define GAIN_AND_CHANNEL  CHANNEL_A_GAIN_128

hx711_t sensor_1;
uint32_t data;
void app_main(void)
{
    hx711_init(&sensor_1, &ESP32_C3_Port, GPIO_DATA, GPIO_CLOCK, GAIN_AND_CHANNEL);
    while (1) {
        data = hx711_read(&sensor_1);
        ESP_LOGI("MAIN", "Value: %ld", data);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}