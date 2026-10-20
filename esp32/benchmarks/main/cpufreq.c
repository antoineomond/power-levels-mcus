#include "target_configuration.h" 
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_pm.h"

// Benchmark sizes
#define BENCH_PRIME_SIZE 2000
#define BENCH_MAT_SIZE 70
#define BENCH_MAT_SIZE_FLOAT 70
#define BENCH_MAT_SIZE_DOUBLE 50

// Benchmark correct results
#define CORRECT_PRIME 303
#define CORRECT_MAT_MUL 1273131485
#define CORRECT_MAT_MUL_FLOAT_UPPER 21570899.0
#define CORRECT_MAT_MUL_FLOAT_LOWER 21570897.0
#define CORRECT_MAT_MUL_DOUBLE_UPPER 7818683.784350
#define CORRECT_MAT_MUL_DOUBLE_LOWER 7818683.784330

#define EXPE_PIN 5
#define SEED 42

void init_gpio(int pin) {
	gpio_reset_pin(pin);
	gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void set_gpio(int pin, int state) {
  gpio_set_level(pin, state);
}

void disable_gpio(int pin) {
	gpio_reset_pin(pin);
	gpio_set_direction(pin, GPIO_MODE_INPUT);
	gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
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
	set_gpio(EXPE_PIN, 1);
	volatile uint32_t cpt = compute_primes(2, benchmark_size);
	set_gpio(EXPE_PIN, 0);
	return cpt;
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
	set_gpio(EXPE_PIN, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	set_gpio(EXPE_PIN, 0);
		
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
	set_gpio(EXPE_PIN, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	set_gpio(EXPE_PIN, 0);
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
	set_gpio(EXPE_PIN, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	set_gpio(EXPE_PIN, 0);
	
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

void run_benchmarks() {
	vTaskDelay(pdMS_TO_TICKS(20000));
	
	volatile uint32_t res_prime = benchmark_prime(BENCH_PRIME_SIZE);
	vTaskDelay(pdMS_TO_TICKS(100));

	volatile uint32_t res_mat_mul = benchmark_mat_mul(BENCH_MAT_SIZE);
	vTaskDelay(pdMS_TO_TICKS(100));
	
	volatile float res_mat_mul_float = benchmark_mat_mul_float(BENCH_MAT_SIZE_FLOAT);
	vTaskDelay(pdMS_TO_TICKS(100));
	
	volatile double res_mat_mul_double = benchmark_mat_mul_double(BENCH_MAT_SIZE_DOUBLE);
	vTaskDelay(pdMS_TO_TICKS(100));
	
	
	// Verification
	if(res_prime != CORRECT_PRIME 
			|| res_mat_mul != CORRECT_MAT_MUL 
			|| res_mat_mul_float < CORRECT_MAT_MUL_FLOAT_LOWER
			|| res_mat_mul_float > CORRECT_MAT_MUL_FLOAT_UPPER
			|| res_mat_mul_double < CORRECT_MAT_MUL_DOUBLE_LOWER 
			|| res_mat_mul_double > CORRECT_MAT_MUL_DOUBLE_UPPER) {

		configure_led();

		// Activate and turn on LED 
		while(1){
			blink_led();
			vTaskDelay(pdMS_TO_TICKS(1000));
			blink_led();
			vTaskDelay(pdMS_TO_TICKS(1000));
		} // Don't continue execution
	}
}

void disable_uart(void) {
    // Uninstall the driver for UART port 0 (or UART_NUM_1)
    uart_driver_delete(UART_NUM_0);

    // Reset the GPIO pins assigned to UART so they can be re-purposed
    disable_gpio(GPIO_NUM_24); // TX Pin
    disable_gpio(GPIO_NUM_23); // RX Pin
}

void app_main(void) {
  // Keep this delay otherwise random crash
  vTaskDelay(pdMS_TO_TICKS(1000));

	// Expe pin
	init_gpio(5);
	
  // Put all the other gpios in input mode with pull_down resistors
	uint8_t unused_gpios[] = {0, 1, 2, 3, 13, 14, 4, 24, 23, 10, 11, 25, 12, 8, 22, 9, 27, 26};
	for (int i = 0; i < sizeof(unused_gpios)/sizeof(uint8_t); i++) {
		disable_gpio(unused_gpios[i]);
	}
	
  configure_led();
	
	blink_led();
  vTaskDelay(pdMS_TO_TICKS(10000));
	blink_led();
	
	while (true) {
		
		// max current 8mA
		set_cpu_clock(PLL_64M, 64);
		run_benchmarks();
		set_cpu_clock(PLL_96M, 96);
		run_benchmarks();
		set_cpu_clock(RC, 1);
		run_benchmarks();
		
		// max current 22mA
		set_cpu_clock(PLL_64M, 1);
		run_benchmarks();
		set_cpu_clock(PLL_96M, 1);
		run_benchmarks();
	}
}
