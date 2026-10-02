idf_component_register(SRCS
"cpufreq.c"
"led.c"
REQUIRES driver esp_driver_gpio
REQUIRES driver esp_driver_uart
REQUIRES led_strip
					INCLUDE_DIRS ".")
