/*
 * API_debounce.h
 *
 *  Created on: Mar 29, 2026
 *      Author: Ing. Lucia Guirula
 */

/* Includes ------------------------------------------------------------------*/
#include "API_debounce.h"
#include "API_delay.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/* Defines ------------------------------------------------------------------*/
#define DEBOUNCE_TIME_MS 40U
#define BUTTON_PRESSED   GPIO_PIN_RESET

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* Tipos privados */
typedef enum{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RISING
} debounceState_t;

/* Variables privadas */
static debounceState_t estadoActual;
static delay_t delayRebote;
static bool_t keyPressed = false; // flag de evento

/**
 * @brief Inicializa la FSM de anti-rebote
 */
void debounceFSM_init(void)
{
    estadoActual = BUTTON_UP;
    delayInit(&delayRebote, DEBOUNCE_TIME_MS);
    keyPressed = false;
}

/**
 * @brief Actualiza la FSM del pulsador
 *
 * Detecta flanco descendente (presión) y genera evento.
 */
void debounceFSM_update(void)
{
    GPIO_PinState lectura = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

    switch (estadoActual)
    {
        case BUTTON_UP:
            if (lectura == BUTTON_PRESSED)
            {
                estadoActual = BUTTON_FALLING;
                delayInit(&delayRebote, DEBOUNCE_TIME_MS);
            }
            break;

        case BUTTON_FALLING:
            if (delayRead(&delayRebote))
            {
                if (lectura == BUTTON_PRESSED)
                {
                    estadoActual = BUTTON_DOWN;
                    keyPressed = true; // evento detectado
                }
                else
                {
                    estadoActual = BUTTON_UP;
                }
            }
            break;

        case BUTTON_DOWN:
            if (lectura != BUTTON_PRESSED)
            {
                estadoActual = BUTTON_RISING;
                delayInit(&delayRebote, DEBOUNCE_TIME_MS);
            }
            break;

        case BUTTON_RISING:
            if (delayRead(&delayRebote))
            {
                if (lectura != BUTTON_PRESSED)
                {
                    estadoActual = BUTTON_UP;
                }
                else
                {
                    estadoActual = BUTTON_DOWN;
                }
            }
            break;
    }
}

/**
 * @brief Devuelve si se presionó la tecla
 *
 * @return true si hubo un evento de presión.
 *         false en caso contrario.
 *
 * Si devuelve true, resetea el estado interno.
 */
bool_t readKey(void)
{
    if (keyPressed)
    {
        keyPressed = false; // termina el evento
        return true;
    }
    return false;
}

/* USER CODE END 0 */
