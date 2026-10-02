#include "target_configuration.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include <stdlib.h>

// Benchmark sizes
#define BENCH_PRIME_SIZE 5000
#define BENCH_PRIME_SIZE_LPOSC 200
#define BENCH_MAT_SIZE 207
#define NB_ITERATIONS_MAT_MUL 10
#define NB_ITERATIONS_MAT_MUL_LPOSC 1

// Benchmark correct results
#define CORRECT_PRIME 669
#define CORRECT_PRIME_LPOSC 46
#define CORRECT_MAT_MUL 4294967295
#define CORRECT_MAT_MUL_FLOAT_UPPER 0.525 
#define CORRECT_MAT_MUL_FLOAT_LOWER 0.524 
#define CORRECT_MAT_MUL_DOUBLE 0.5241578750190518665164063349948264658451080322265625

const int expe_pin = 11;
const uint US = 1000000;
extern float TIME_RATE;

uint compute_primes(uint start, uint end) {
	volatile uint cpt = 0;
	uint is_prime = 1;
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

uint8_t benchmark_prime(uint benchmark_size) {
	gpio_put(expe_pin, 1);
	volatile uint cpt = compute_primes(2, benchmark_size);
	gpio_put(expe_pin, 0);
	uint8_t correct = 1;
	if(benchmark_size == 200 && cpt != CORRECT_PRIME_LPOSC) {
		correct = 0;
	}
	if(benchmark_size == 5000 && cpt != CORRECT_PRIME) {
		correct = 0;
	}
	return correct;
}

void compute_primes_core1() {
	uint32_t start = multicore_fifo_pop_blocking();
	uint32_t end = multicore_fifo_pop_blocking();
	uint cpt = compute_primes(start, end);
	multicore_fifo_push_blocking(cpt);
}

uint8_t benchmark_prime_multicores(uint benchmark_size) {
	multicore_reset_core1();
	multicore_launch_core1(compute_primes_core1);
	multicore_fifo_push_blocking(benchmark_size/2);
	multicore_fifo_push_blocking(benchmark_size);
	gpio_put(expe_pin, 1);
	uint cpt_core0 = compute_primes(2, benchmark_size/2);
	uint cpt_core1 = multicore_fifo_pop_blocking();
	gpio_put(expe_pin, 0);
	uint8_t correct = 1;
	if(benchmark_size == 200 && cpt_core0 + cpt_core1 != CORRECT_PRIME_LPOSC) {
		correct = 0;
	}
	if(benchmark_size == 5000 && cpt_core0 + cpt_core1 != CORRECT_PRIME) {
		correct = 0;
	}
	return correct;
}

uint8_t benchmark_mat_mul(uint benchmark_size, uint nb_iteration_mat_mul) {
	uint32_t A_value = 1UL<<15;
	uint32_t B_value = 1UL<<15;
	volatile uint8_t correct = 1;
	volatile uint32_t *A = malloc(sizeof(uint32_t)*benchmark_size);
	volatile uint32_t *B = malloc(sizeof(uint32_t)*benchmark_size);
	volatile uint32_t *C = malloc(sizeof(uint32_t)*benchmark_size);
	for (int i = 0; i < benchmark_size; i++) {
		A[i] = A_value;
		B[i] = B_value;
	}
	gpio_put(expe_pin, 1);
	for (int k = 0; k < nb_iteration_mat_mul; k++) {
		for (int i = 0; i < benchmark_size; i++) {
			C[i] = A[i] * B[i];
		}
	}
	gpio_put(expe_pin, 0);
	// Verification
	for (int i = 0; i < benchmark_size; i++) {
		if(C[i] != CORRECT_MAT_MUL) {
			correct = 0;
		}
	}
	free((void*)A);
	free((void*)B);
	free((void*)C);
	return correct;
}

uint8_t benchmark_mat_mul_float(uint benchmark_size, uint nb_iteration_mat_mul) {
	float A_value = 1.23456789;
	float B_value = 1.23456789;
	volatile uint8_t correct = 1;
	volatile float *A = malloc(sizeof(float)*benchmark_size);
	volatile float *B = malloc(sizeof(float)*benchmark_size);
	volatile float *C = malloc(sizeof(float)*benchmark_size);
	for (int i = 0; i < benchmark_size; i++) {
		A[i] = A_value;
		B[i] = B_value;
	}
	gpio_put(expe_pin, 1);
	for (int k = 0; k < nb_iteration_mat_mul; k++) {
		for (int i = 0; i < benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
	}
	gpio_put(expe_pin, 0);
	// Verification
	for (int i = 0; i < benchmark_size; i++) {
		if(C[i] > CORRECT_MAT_MUL_FLOAT_UPPER || C[i] < CORRECT_MAT_MUL_FLOAT_LOWER) {
			correct = 0;
		}
	}
	free((void*)A);
	free((void*)B);
	free((void*)C);
	return correct;
}

uint8_t benchmark_mat_mul_double(uint benchmark_size, uint nb_iteration_mat_mul) {
	double A_value = 1.23456789;
	double B_value = 1.23456789;
	volatile uint8_t correct = 1;
	volatile double *A = malloc(sizeof(double)*benchmark_size);
	volatile double *B = malloc(sizeof(double)*benchmark_size);
	volatile double *C = malloc(sizeof(double)*benchmark_size);
	for (int i = 0; i < benchmark_size; i++) {
		A[i] = A_value;
		B[i] = B_value;
	}
	gpio_put(expe_pin, 1);
	for (int k = 0; k < nb_iteration_mat_mul; k++) {
		for (int i = 0; i < benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
	}
	gpio_put(expe_pin, 0);
	// Verification
	for (int i = 0; i < benchmark_size; i++) {
		if(C[i] > CORRECT_MAT_MUL_FLOAT_UPPER || C[i] < CORRECT_MAT_MUL_FLOAT_LOWER) {
			correct = 0;
		}
	}
	free((void*)A);
	free((void*)B);
	free((void*)C);
	return correct;
}

uint8_t execute_benchmarks(bool clock_source_lposc) {
	// Sleep 10 seconds before starting benchmarks
	sleep_us((int)(10*US*TIME_RATE));
	uint prime_size = clock_source_lposc ? BENCH_PRIME_SIZE_LPOSC : BENCH_PRIME_SIZE;
	uint mat_mul_iters = clock_source_lposc ? NB_ITERATIONS_MAT_MUL_LPOSC : NB_ITERATIONS_MAT_MUL;
	uint results = 0;
	
	results |= benchmark_prime(prime_size) << 1; // Uses one CPU core
	sleep_us((int)(100000*TIME_RATE));
	
	results |= benchmark_prime_multicores(prime_size) << 2; // Uses both cores
	sleep_us((int)(100000*TIME_RATE));
	
	results |= benchmark_mat_mul(BENCH_MAT_SIZE, mat_mul_iters) << 3; // Uses RAM
	sleep_us((int)(100000*TIME_RATE));
	
	results |= benchmark_mat_mul_float(BENCH_MAT_SIZE, mat_mul_iters) << 4; // Uses float co-processor
	sleep_us((int)(100000*TIME_RATE));
	
	results |= benchmark_mat_mul_double(BENCH_MAT_SIZE/2, mat_mul_iters) << 5; // Uses double co-processor
	sleep_us((int)(US*TIME_RATE));
	return results;
}

int main() {
	sleep_ms(100); // For unknown reason, not sleeping here sometimes makes firmware upload
	// Set GPIO pin to advertise experiments start and end
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	
	while(true) {
		// Baseline
		switch_configuration_from_parameter(&(config){PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_DEFAULT, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true});
		execute_benchmarks(false);
		
		// min frequency
		switch_configuration_from_parameter(&(config){PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_DEFAULT, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_DEFAULT, true});
		execute_benchmarks(false);
		
		// min voltage
		switch_configuration_from_parameter(&(config){PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_MIN_PLL, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_80, true});
		execute_benchmarks(false);
		
		// Min frequency, min voltage
		switch_configuration_from_parameter(&(config){PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_MIN_PLL, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_DEFAULT, true});
		execute_benchmarks(false);
		
		// LPOSC
	  switch_configuration_from_parameter(&(config){LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_10, true});
		execute_benchmarks(true);
	  switch_configuration_from_parameter(&(config){LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_80, true});
		execute_benchmarks(true);
	}
}

