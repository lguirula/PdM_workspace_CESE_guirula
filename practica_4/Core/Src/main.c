/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "API_delay.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * @brief Estados de la FSM para anti-rebote del botón.
 *
 * Permite detectar flancos ascendentes y descendentes
 * utilizando retardos no bloqueantes para evitar rebotes.
 */
typedef enum{
BUTTON_UP,
BUTTON_FALLING,
BUTTON_DOWN,
BUTTON_RISING,
} debounceState_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/**
 * @brief Selección de ejercicio a ejecutar.
 *
 * Este proyecto contiene múltiples implementaciones correspondientes
 * a distintos puntos de la práctica.
 *
 * Para seleccionar qué ejercicio se desea ejecutar, se debe descomentar
 * SOLO UNA de las siguientes macros:
 *
 * - EJERCICIO_1: Implementación de la máquina de estados de anti-rebote
 *                con control directo del LED.
 *
 * - EJERCICIO_2: Implementación modular del anti-rebote utilizando
 *                API_debounce y API_delay. El LED cambia su frecuencia
 *                de parpadeo entre 100 ms y 500 ms al presionar el botón.
 *
 * IMPORTANTE:
 * - No deben estar activados múltiples ejercicios al mismo tiempo.
 * - El código dentro del main se controla mediante directivas #ifdef.
 */
#define EJERCICIO_2
// #define EJERCICIO_1


#ifdef EJERCICIO_1
#define DEBOUNCE_TIME_MS  40U // Tiempo de anti-rebote en milisegundos
#define BUTTON_PRESSED GPIO_PIN_RESET
#endif


#ifdef EJERCICIO_2
#include "API_debounce.h"
#define TIEMPO_LENTO  500U
#define TIEMPO_RAPIDO 100U
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
#ifdef EJERCICIO_1
static debounceState_t estadoActual;
static delay_t delayRebote;
#endif

#ifdef EJERCICIO_2
delay_t delayLed;
bool estadoFrecuencia = false; // false = lento, true = rápido
#endif
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
#ifdef EJERCIO_1
void debounceFSM_init(void);
void debounceFSM_update(void);
void buttonPressed(void);
void buttonReleased(void);
#endif
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef EJERCICIO_1
/**
 * @brief Inicializa la máquina de estados de anti-rebote.
 * Configura el estado inicial del sistema como BUTTON_UP
 * y prepara el retardo no bloqueante utilizado para validar los cambios de estado del pulsador.
 */
void debounceFSM_init()
{
    estadoActual = BUTTON_UP;
    delayInit(&delayRebote, DEBOUNCE_TIME_MS); // tiempo de anti-rebote en ms
}

/**
 * @brief Acción ejecutada cuando el botón es presionado.
 * Enciende el LED de la placa.
 */
void buttonPressed()
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

/**
 * @brief Acción ejecutada cuando el botón es liberado.
 *
 * Apaga el LED de la placa.
 */
void buttonReleased()
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

/**
 * @brief Actualiza la máquina de estados del pulsador.
 *
 *Se encarga de:
 * - Leer el estado actual del botón
 * - Gestionar las transiciones de la FSM
 * - Aplicar anti-rebote mediante un delay no bloqueante
 * - Ejecutar acciones ante eventos (presión/liberación)
 */
void debounceFSM_update()
{
    // Lectura del estado actual del botón
    GPIO_PinState lectura = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

    switch (estadoActual)
    {
        /**
         * Estado estable: botón liberado
         */
        case BUTTON_UP:
            // Si se detecta presión (posible rebote)
            if (lectura == BUTTON_PRESSED)
            {
                estadoActual = BUTTON_FALLING;
                delayInit(&delayRebote, DEBOUNCE_TIME_MS); // inicia anti-rebote
            }
            break;

        /**
         * Estado transitorio: posible flanco descendente
         */
        case BUTTON_FALLING:
            // Espera a que termine el tiempo de anti-rebote
            if (delayRead(&delayRebote))
            {
                // Si sigue presionado → confirmo evento
                if (lectura == BUTTON_PRESSED)
                {
                    estadoActual = BUTTON_DOWN;
                    buttonPressed(); // evento: botón presionado
                }
                else
                {
                    // Rebote → vuelve al estado anterior
                    estadoActual = BUTTON_UP;
                }
            }
            break;

        /**
         * Estado estable: botón presionado
         */
        case BUTTON_DOWN:
            // Si se detecta liberación (posible rebote)
            if (lectura != BUTTON_PRESSED)
            {
                estadoActual = BUTTON_RISING;
                delayInit(&delayRebote, DEBOUNCE_TIME_MS); // inicia anti-rebote
            }
            break;

        /**
         * Estado transitorio: posible flanco ascendente
         */
        case BUTTON_RISING:
            // Espera a que termine el tiempo de anti-rebote
            if (delayRead(&delayRebote))
            {
                // Si realmente se liberó → confirmo evento
                if (lectura != BUTTON_PRESSED)
                {
                    estadoActual = BUTTON_UP;
                    buttonReleased(); // evento: botón liberado
                }
                else
                {
                    // Rebote → vuelve a presionado
                    estadoActual = BUTTON_DOWN;
                }
            }
            break;
    }
}
#endif

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
#ifdef EJERCICIO_2
  debounceFSM_init();                 // inicializa la FSM del botón
  delayInit(&delayLed, TIEMPO_LENTO); // inicializa el delay del LED
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  while (1)
  {
    /* USER CODE END WHILE */
	#ifdef EJERCICIO_1
	  debounceFSM_update();
	#endif

	#ifdef EJERCICIO_2
	  // actualizar FSM del botón
	      debounceFSM_update();

	      // si hubo una pulsación
	      if (readKey())
	      {
	          // alternar frecuencia
	          estadoFrecuencia = !estadoFrecuencia;

	          if (estadoFrecuencia)
	          {
	              delayWrite(&delayLed, TIEMPO_RAPIDO);
	          }
	          else
	          {
	              delayWrite(&delayLed, TIEMPO_LENTO);
	          }
	      }

	      // control del LED con delay no bloqueante
	      if (delayRead(&delayLed))
	      {
	          HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	      }
	#endif
    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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

#ifdef  USE_FULL_ASSERT
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
