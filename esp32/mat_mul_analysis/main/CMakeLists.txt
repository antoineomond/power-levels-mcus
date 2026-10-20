idf_component_register(SRCS
"main.c"
"led.c"
"mat_mul_asm.S"
REQUIRES driver esp_driver_gpio
REQUIRES driver esp_driver_uart
REQUIRES led_strip
					INCLUDE_DIRS ".")
