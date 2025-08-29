idf_component_register(SRCS "main.c" "hx711/src/port.c" "hx711/src/hx711.c"
                    PRIV_REQUIRES spi_flash
                    REQUIRES esp_driver_rmt esp_driver_gpio  
                    INCLUDE_DIRS "" "hx711/inc")
