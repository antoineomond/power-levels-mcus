#include "target_configuration.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include <stdlib.h>

#define BENCH_PRIME_SIZE 2000

const int expe_pin = 11;
const int expe_pin_multicore = 12;

void pull_down_gpios() {
	for (int gpio = 0; gpio < 23; gpio++) {
		if(gpio != expe_pin && gpio != expe_pin_multicore) {
			gpio_set_dir(gpio, 0);
			gpio_pull_down(gpio);
		}
	}
	gpio_set_dir(25, 0);
	gpio_pull_down(25);
	gpio_set_dir(26, 0);
	gpio_pull_down(26);
	gpio_set_dir(27, 0);
	gpio_pull_down(27);
	gpio_set_dir(28, 0);
	gpio_pull_down(28);
}

uint32_t compute_primes(uint32_t start, uint32_t end) {
	volatile uint32_t cpt = 0;
	uint32_t is_prime = 1;
	for (int k = start; k < end; k++) {
		is_prime = 1;
		for (int i = 2; i	< k; i++) {
			if (k%i==0) {
				is_prime = 0;
			}
		}
		if(is_prime == 1) {
			cpt += 1;
		}
	}
	return cpt;
}

void compute_primes_core1_measured() {
	uint32_t start = multicore_fifo_pop_blocking();
	uint32_t end = multicore_fifo_pop_blocking();
	gpio_put(expe_pin_multicore, 1);
	uint cpt = compute_primes(start, end);
	gpio_put(expe_pin_multicore, 0);
	multicore_fifo_push_blocking(cpt);
	sleep_ms(100);
}

uint32_t benchmark_prime_multicores_measured(uint benchmark_size) {
	multicore_reset_core1();
	multicore_launch_core1(compute_primes_core1_measured);
	multicore_fifo_push_blocking(benchmark_size/2);
	multicore_fifo_push_blocking(benchmark_size);
	gpio_put(expe_pin, 1);
	uint cpt_core0 = compute_primes(2, benchmark_size/2);
	gpio_put(expe_pin, 0);
	sleep_ms(10);
	gpio_put(expe_pin, 1);
	uint cpt_core1 = multicore_fifo_pop_blocking();
	gpio_put(expe_pin, 0);
	sleep_ms(10);
	return cpt_core0 + cpt_core1;
}

int main() {
	sleep_ms(1000);
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	gpio_init(expe_pin_multicore);
	gpio_set_dir(expe_pin_multicore, GPIO_OUT);
	
	pull_down_gpios();
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	
	sleep_ms(10000);
	
	switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_DEFAULT, true});
	while(true) {
		benchmark_prime_multicores_measured(BENCH_PRIME_SIZE);
		sleep_ms(2000);
	}
}

