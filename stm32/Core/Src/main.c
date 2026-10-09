/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "target_configuration.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Benchmark sizes
#define BENCH_PRIME_SIZE 2000
#define BENCH_MAT_SIZE 100
#define BENCH_MAT_SIZE_DOUBLE 70

// Benchmark correct results
#define CORRECT_PRIME 303
#define CORRECT_MAT_MUL 1141371108
#define CORRECT_MAT_MUL_FLOAT 63883268.0 
#define CORRECT_MAT_MUL_DOUBLE_LOWER 21570919.092350 
#define CORRECT_MAT_MUL_DOUBLE_UPPER 21570919.092360 

#define SEED 42

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init_pull_downs(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void MX_GPIO_Init_pull_downs(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA6 PA7 PA8
                           PA9 PA10 PA11 PA12
                           PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB12 PB13 PB14 PB15
                           PB3 PB4 PB5 PB6
                           PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
	volatile uint32_t cpt = compute_primes(2, benchmark_size);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				C[size*i + j] += A[size*i + k] * B[size*k + j];
			}
		}
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
	
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
	HAL_Delay(20000);
	
	volatile uint32_t res_prime = benchmark_prime(BENCH_PRIME_SIZE);
	HAL_Delay(100);
	
	volatile uint32_t res_mat_mul = benchmark_mat_mul(BENCH_MAT_SIZE);
	HAL_Delay(100);
	
	volatile float res_mat_mul_float = benchmark_mat_mul_float(BENCH_MAT_SIZE);
	HAL_Delay(100);
	
	volatile double res_mat_mul_double = benchmark_mat_mul_double(BENCH_MAT_SIZE_DOUBLE);
	HAL_Delay(100);
	
	// Verification
	if(res_prime != CORRECT_PRIME 
			|| res_mat_mul != CORRECT_MAT_MUL 
			|| res_mat_mul_float != CORRECT_MAT_MUL_FLOAT
			|| res_mat_mul_double < CORRECT_MAT_MUL_DOUBLE_LOWER 
			|| res_mat_mul_double > CORRECT_MAT_MUL_DOUBLE_UPPER) {
	
		// Activate and turn on LED 
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		while(1){} // Don't continue execution
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  MX_GPIO_Init_pull_downs();
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */
	while(1) {
		// max current: 6mA
		SystemClock_Config_HSI_16MHz();
		run_benchmarks();
		SystemClock_Config_HSE_25MHz();
		run_benchmarks();
		SystemClock_Config_HSI_1MHz();
		run_benchmarks();
		SystemClock_Config_HSE_156250000Hz();
		run_benchmarks();
		SystemClock_Config_PLL_1MHz();
		run_benchmarks();
		SystemClock_Config_PLL_1MHz_minvreg();
		run_benchmarks();
		
		// max current 12mA
		SystemClock_Config_PLL_64MHz_default();
		run_benchmarks();
		SystemClock_Config_PLL_64MHz_minvreg();
		run_benchmarks();
	}

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,  */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
