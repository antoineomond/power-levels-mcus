#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_system.h"
#include "soc/rtc.h"
#include "esp_log.h"
#include "led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_pm.h"

typedef enum CLK_SRC {
  XTAL=1, PLL_64M=2, PLL_96M=3, RC=4
 } CLK_SRC;

// Benchmark sizes
#define BENCH_PRIME_SIZE 200
#define BENCH_MAT_SIZE 140
#define BENCH_MAT_SIZE_DOUBLE 100

// Benchmark correct results
#define CORRECT_PRIME 303
#define CORRECT_MAT_MUL 1721141896
#define CORRECT_MAT_MUL_FLOAT_UPPER 176452289.0
#define CORRECT_MAT_MUL_FLOAT_LOWER 176452287.0
#define CORRECT_MAT_MUL_DOUBLE_UPPER 63883357.365418
#define CORRECT_MAT_MUL_DOUBLE_LOWER 63883357.365416

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
	
	volatile float res_mat_mul_float = benchmark_mat_mul_float(BENCH_MAT_SIZE);
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

		// Activate and turn on LED 
		blink_led();
		while(1){} // Don't continue execution
	}
}

bool set_cpu_clock(CLK_SRC source, uint32_t divider) {
  rtc_cpu_freq_config_t setup;

  if (source == XTAL){
	setup.source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
	setup.source = SOC_CPU_CLK_SRC_XTAL;
  }
  else if (source == PLL_64M){
	setup.source_freq_mhz = 64;
	setup.source = SOC_CPU_CLK_SRC_FLASH_PLL;
  }
  else if (source == PLL_96M){
	setup.source_freq_mhz = 96;
	setup.source = SOC_CPU_CLK_SRC_PLL;
  }
  else if (source == RC){
	setup.source_freq_mhz = 8;
	setup.source = SOC_CPU_CLK_SRC_RC_FAST;
  } else {
	return 0;
  }

  if (divider < 1)
	return 0;

  setup.div = divider;
  setup.freq_mhz = (setup.source_freq_mhz + divider/2) / divider;

  rtc_clk_cpu_freq_set_config(&setup);

  return 1;
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

  // Set cpu freq
	//set_cpu_clock(XTAL, 1);

	// Expe pin
	init_gpio(5);
  configure_led();
	
  // Put all the other gpios in input mode with pull_down resistors
	uint8_t unused_gpios[] = {0, 1, 2, 3, 13, 14, 4, 10, 11, 25, 12, 22, 27, 26};
	for (int i = 0; i < sizeof(unused_gpios)/sizeof(uint8_t); i++) {
		disable_gpio(unused_gpios[i]);
	}
	
	//blink_led();
  //vTaskDelay(pdMS_TO_TICKS(1000));
	//blink_led();
	
	//disable_uart();
  //vTaskDelay(pdMS_TO_TICKS(1000));
	//
	//blink_led();
  //vTaskDelay(pdMS_TO_TICKS(1000));
	//blink_led();

	// Configure power management to lock the frequency safely
	//esp_pm_config_t pm_config = {
	//		.max_freq_mhz = 32, // Your target XTAL frequency
	//		.min_freq_mhz = 8,
	//		.light_sleep_enable = false
	//};

	//// This safely scales the voltage up first, then alters the clock tree
	//ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
  //vTaskDelay(pdMS_TO_TICKS(3000));
	
	// Baseline
	//set_cpu_clock(XTAL, 1);
  //vTaskDelay(pdMS_TO_TICKS(1000));
	//
	//blink_led();
  //vTaskDelay(pdMS_TO_TICKS(1000));
	//blink_led();
	
  configure_led();
	
	blink_led();
  vTaskDelay(pdMS_TO_TICKS(20000));
	blink_led();
	
	while (true) {
		//run_benchmarks();
		set_cpu_clock(PLL_64M, 1);
		run_benchmarks();
		set_cpu_clock(PLL_96M, 1);
		run_benchmarks();
		set_cpu_clock(RC, 1);
		run_benchmarks();
		
		// Minimum freq >1MHz
		//set_cpu_clock(XTAL, 32);
		//run_benchmarks();
		set_cpu_clock(PLL_64M, 64);
		run_benchmarks();
		set_cpu_clock(PLL_96M, 96);
		run_benchmarks();
		//set_cpu_clock(RC, 8);
		//run_benchmarks();
		//set_cpu_clock(XTAL, 1);
		
		// Minimum freq
		//set_cpu_clock(XTAL, 255);
		//run_benchmarks();
		//set_cpu_clock(PLL_64M, 255);
		//run_benchmarks();
		//set_cpu_clock(PLL_96M, 255);
		//run_benchmarks();
		//set_cpu_clock(RC, 255);
		//run_benchmarks();
	}
}
