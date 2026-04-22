/*
 * API_delay.c
 *
 *  Created on: Mar 19, 2026
 *      Author: Ing. Lucia Guirula
 */

/* Includes ------------------------------------------------------------------*/
#include "API_delay.h"
#include "stm32f4xx_hal.h"

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief Inicializa la estructura delay
 * @param delay puntero a la estructura delay
 * @param duration duración del retardo en ms
 */
void delayInit( delay_t * delay, tick_t duration ){

	// Validación de parámetros
	    if (delay == NULL || duration == 0){
	        return;
	    }

	delay->duration = duration;
	delay->running = false;
}

/**
 * @brief Verifica si el tiempo del delay se cumplió
 * @param delay puntero a la estructura delay
 * @return true si el tiempo se cumplió, false en caso contrario
 */
bool_t delayRead( delay_t * delay ){

	// Validación de parámetros
	  if (delay == NULL){
	      return false;
	  }

	if(!delay->running) {
		delay->running = true;
		delay->startTime = HAL_GetTick();
	} else {
		tick_t tiempoActual = HAL_GetTick();
		tick_t diferencia = tiempoActual - delay->startTime;

		if(diferencia >= delay->duration){
			delay->running = false;
			return true;
		}
		return false;
	}
	return false;
}


/**
 * @brief Modifica la duración de un delay existente
 * @param delay puntero a la estructura delay
 * @param duration nueva duración en ms
 */
void delayWrite( delay_t * delay, tick_t duration){
	// Validación de parámetros
	    if (delay == NULL || duration == 0){
	        return;
	    }

	delay->duration = duration;
}


/**
 * @brief Devuelve el estado de ejecución del delay
 * @param delay Puntero a la estructura delay_t
 * @return true si el delay está corriendo
 * @return false si no está corriendo o si el puntero es inválido
 */
bool_t delayIsRunning(delay_t * delay)
{
    if (delay == NULL)
    {
        return false;
    }

    return delay->running;
}

/* USER CODE END 0 */
