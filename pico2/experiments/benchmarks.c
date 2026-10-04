#include "target_configuration.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include <stdlib.h>

// Benchmark sizes
#define BENCH_PRIME_SIZE 2000
#define BENCH_MAT_SIZE 200
#define BENCH_MAT_SIZE_DOUBLE 140
#define BENCH_PRIME_SIZE_LPOSC 200
#define BENCH_MAT_SIZE_LPOSC 20
#define BENCH_MAT_SIZE_DOUBLE_LPOSC 14

// Benchmark correct results
#define CORRECT_PRIME 303
#define CORRECT_MAT_MUL 1377790330
#define CORRECT_MAT_MUL_FLOAT_UPPER 513006273.000000
#define CORRECT_MAT_MUL_FLOAT_LOWER 513006271.000000
#define CORRECT_MAT_MUL_DOUBLE_UPPER 176452630.184880
#define CORRECT_MAT_MUL_DOUBLE_LOWER 176452630.184870
#define CORRECT_PRIME_LPOSC 46
#define CORRECT_MAT_MUL_LPOSC 131482027
#define CORRECT_MAT_MUL_FLOAT_UPPER_LPOSC 519851.937600 
#define CORRECT_MAT_MUL_FLOAT_LOWER_LPOSC 519851.937400 
#define CORRECT_MAT_MUL_DOUBLE_UPPER_LPOSC 171011.912417
#define CORRECT_MAT_MUL_DOUBLE_LOWER_LPOSC 171011.912415

// Parameters
#define PLL_MIN_VCO_FREQ_HZ 760*MHZ 
#define PLL_MAX_POSTDIV 7 
#define PLL_DEFAULT_VCO_FREQ_HZ PLL_SYS_VCO_FREQ_HZ 
#define PLL_DEFAULT_POSTDIV1 PLL_SYS_POSTDIV1
#define PLL_DEFAULT_POSTDIV2 PLL_SYS_POSTDIV2

#define ROSC_MAX_DIVIDER 30
#define ROSC_MIN_RANGE ROSC_CTRL_FREQ_RANGE_VALUE_LOW
#define ROSC_MIN_DRIVE_STRENGTH 0x0000
#define ROSC_MAX_DRIVE_STRENGTH 0x7777
#define ROSC_DEFAULT_DIVIDER 8
#define ROSC_DEFAULT_RANGE ROSC_MIN_RANGE
#define ROSC_DEFAULT_DRIVE_STRENGTH ROSC_MIN_DRIVE_STRENGTH

#define LPOSC_MIN_TRIM 0x000
#define LPOSC_MAX_TRIM 0x3f0
#define LPOSC_DEFAULT_TRIM LPOSC_MIN_TRIM

#define VREG_DEFAULT VREG_VOLTAGE_DEFAULT
#define VREG_MIN_PLL VREG_VOLTAGE_0_90
#define VREG_MIN_XOSC_ROSC_LPOSC VREG_VOLTAGE_0_75

#define SEED 42

const int expe_pin = 11;
const uint US = 1000000;
extern float TIME_RATE;

void pull_down_gpios() {
	for (int gpio = 0; gpio < 23; gpio++) {
		if(gpio != expe_pin) {
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

uint32_t benchmark_prime(uint32_t benchmark_size) {
	gpio_put(expe_pin, 1);
	volatile uint32_t cpt = compute_primes(2, benchmark_size);
	gpio_put(expe_pin, 0);
	return cpt;
}

void compute_primes_core1() {
	uint32_t start = multicore_fifo_pop_blocking();
	uint32_t end = multicore_fifo_pop_blocking();
	uint cpt = compute_primes(start, end);
	multicore_fifo_push_blocking(cpt);
}

uint32_t benchmark_prime_multicores(uint benchmark_size) {
	multicore_reset_core1();
	multicore_launch_core1(compute_primes_core1);
	multicore_fifo_push_blocking(benchmark_size/2);
	multicore_fifo_push_blocking(benchmark_size);
	gpio_put(expe_pin, 1);
	uint cpt_core0 = compute_primes(2, benchmark_size/2);
	uint cpt_core1 = multicore_fifo_pop_blocking();
	gpio_put(expe_pin, 0);
	return cpt_core0 + cpt_core1;
}

uint32_t benchmark_mat_mul(uint32_t size) {
	volatile uint32_t checksum = 0;
	srand(SEED);
	// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
	uint32_t *A = malloc(sizeof(uint32_t)*size*size);
	uint32_t *B = malloc(sizeof(uint32_t)*size*size);
	uint32_t *C = malloc(sizeof(uint32_t)*size*size);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			A[size*i + j] = rand() % 256;
			B[size*i + j] = rand() % 256;
			C[size*i + j] = 0;
		}
	}
	gpio_put(expe_pin, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	gpio_put(expe_pin, 0);
		
	// Prevent compiler optimisation and check results
	checksum = 0;
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			checksum = (checksum + C[size*i +j]) % (1<<31);
		}
	}
	free(A);
	free(B);
	free(C);
	return checksum;
}

float benchmark_mat_mul_float(uint32_t size) {
	volatile float checksum = 0;
	srand(SEED);
	// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
	float *A = malloc(sizeof(float)*size*size);
	float *B = malloc(sizeof(float)*size*size);
	float *C = malloc(sizeof(float)*size*size);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			A[size*i + j] = (float)rand()/(float)(RAND_MAX/16); // Generates a float between 0 and 16
			B[size*i + j] = (float)rand()/(float)(RAND_MAX/16);
			C[size*i + j] = 0;
		}
	}
	gpio_put(expe_pin, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	gpio_put(expe_pin, 0);
	// Prevent compiler optimisation and check results
	checksum = 0;
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			checksum = checksum + C[size*i +j];
		}
	}
	free(A);
	free(B);
	free(C);
	return checksum;
}

double benchmark_mat_mul_double(uint32_t size) {
	volatile double checksum = 0;
	srand(SEED);
	// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
	double *A = malloc(sizeof(double)*size*size);
	double *B = malloc(sizeof(double)*size*size);
	double *C = malloc(sizeof(double)*size*size);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			A[size*i + j] = (double)rand()/(double)(RAND_MAX/16);
			B[size*i + j] = (double)rand()/(double)(RAND_MAX/16);
			C[size*i + j] = 0;
		}
	}
	gpio_put(expe_pin, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	gpio_put(expe_pin, 0);
	
	// Prevent compiler optimisation and check results
	checksum = 0;
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			checksum = checksum + C[size*i +j];
		}
	}
	free(A);
	free(B);
	free(C);
	return checksum;
}

void execute_benchmarks(bool islposc) {
	sleep_us((int)(20*US*TIME_RATE));
	
	uint32_t bench_prime_size = islposc ? BENCH_PRIME_SIZE_LPOSC : BENCH_PRIME_SIZE;
	uint32_t bench_mat_size = islposc ? BENCH_MAT_SIZE_LPOSC : BENCH_MAT_SIZE;
	uint32_t bench_mat_double_size = islposc ? BENCH_MAT_SIZE_DOUBLE_LPOSC : BENCH_MAT_SIZE_DOUBLE;
	volatile uint32_t res_prime = benchmark_prime(bench_prime_size);
	sleep_us((int)(100000*TIME_RATE));
	
	volatile uint32_t res_prime_multicores = benchmark_prime_multicores(bench_prime_size);
	sleep_us((int)(100000*TIME_RATE));
	
	volatile uint32_t res_mat_mul = benchmark_mat_mul(bench_mat_size);
	sleep_us((int)(100000*TIME_RATE));
	
	volatile float res_mat_mul_float = benchmark_mat_mul_float(bench_mat_size);
	sleep_us((int)(100000*TIME_RATE));
	
	volatile double res_mat_mul_double = benchmark_mat_mul_double(bench_mat_double_size);
	sleep_us((int)(100000*TIME_RATE));
	
	// Verification
	if(!islposc) {
		if(res_prime != CORRECT_PRIME
				|| res_prime_multicores != CORRECT_PRIME
				|| res_mat_mul != CORRECT_MAT_MUL
				|| res_mat_mul_float < CORRECT_MAT_MUL_FLOAT_LOWER
				|| res_mat_mul_float > CORRECT_MAT_MUL_FLOAT_UPPER
				|| res_mat_mul_double < CORRECT_MAT_MUL_DOUBLE_LOWER
				|| res_mat_mul_double > CORRECT_MAT_MUL_DOUBLE_UPPER) {
		
			gpio_init(PICO_DEFAULT_LED_PIN);
			gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
			gpio_put(PICO_DEFAULT_LED_PIN, 1);
			while(1){} // Don't continue execution
		}
	}
	else {
		if(res_prime != CORRECT_PRIME_LPOSC
				|| res_prime_multicores != CORRECT_PRIME_LPOSC
				|| res_mat_mul != CORRECT_MAT_MUL_LPOSC
				|| res_mat_mul_float < CORRECT_MAT_MUL_FLOAT_LOWER_LPOSC
				|| res_mat_mul_float > CORRECT_MAT_MUL_FLOAT_UPPER_LPOSC
				|| res_mat_mul_double < CORRECT_MAT_MUL_DOUBLE_LOWER_LPOSC
				|| res_mat_mul_double > CORRECT_MAT_MUL_DOUBLE_UPPER_LPOSC) {
		
			gpio_init(PICO_DEFAULT_LED_PIN);
			gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
			gpio_put(PICO_DEFAULT_LED_PIN, 1);
			while(1){} // Don't continue execution
		}
	}
}

void led_blink(uint count) {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	for (int i = 0; i < count; i++) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		sleep_us((int)(TIME_RATE*250000));
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		sleep_us((int)(TIME_RATE*250000));
	}
}

int main() {
	sleep_ms(1000);
	// Set GPIO pin to advertise experiments start and end
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	
	pull_down_gpios();
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	
	sleep_ms(10000);
	
	while(true) {
		// Baseline
		// switch_configuration_from_parameter(&(config){PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true});
		// execute_benchmarks(false);
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
		// switch_configuration_from_parameter(&(config){PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true});
		// execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_85, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_85, true});
		execute_benchmarks(false);
		
		// Min frequency, min voltage
		switch_configuration_from_parameter(&(config){PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true});
		execute_benchmarks(false);
		switch_configuration_from_parameter(&(config){ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_85, true});
		execute_benchmarks(false);
		
		// LPOSC
	 switch_configuration_from_parameter(&(config){LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_DEFAULT, true});
		execute_benchmarks(true);
	 switch_configuration_from_parameter(&(config){LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_80, true});
		execute_benchmarks(true);
	}
}

