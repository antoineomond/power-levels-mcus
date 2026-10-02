add_executable(benchmarks
        benchmarks.c
				target_configuration.h)
target_link_libraries(benchmarks
				hardware_gpio
				pico_time
				pico_stdlib
				pico_multicore
				hardware_pll
				hardware_xosc
				hardware_powman
				hardware_ticks
				hardware_watchdog
				hardware_sleep
				hardware_rosc
				hardware_gpio)
pico_enable_stdio_usb(benchmarks 0)
pico_enable_stdio_uart(benchmarks 0)
pico_add_extra_outputs(benchmarks)
