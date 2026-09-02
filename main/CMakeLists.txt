idf_component_register(SRCS "test.c"
                              "A7169/A7169.c"
                              "protocol/protocol.c"
                       PRIV_REQUIRES spi_flash esp_driver_gpio esp_driver_uart
                       INCLUDE_DIRS "A7169" "protocol")
