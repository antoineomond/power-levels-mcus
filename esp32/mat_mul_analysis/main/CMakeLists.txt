idf_component_register(SRCS
"main.c"
"led.c"
"mat_mul_asm.S"
"mat_mul_asm_nomem.S"
"mat_mul_asm_noalloc.S"
"mat_mul_asm_nomul.S"
"mat_mul_asm_mod.S"
"mat_mul_float_asm.S"
"mat_mul_float_asm_nosf.S"
REQUIRES driver esp_driver_gpio
REQUIRES driver esp_driver_uart
REQUIRES led_strip
					INCLUDE_DIRS ".")
