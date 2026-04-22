/*
 * API_delay.h
 *
 *  Created on: Mar 19, 2026
 *      Author: Ing. Lucia Guirula
 */

#ifndef API_DELAY_H_
#define API_DELAY_H_

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

/* Typedefs ------------------------------------------------------------------*/

typedef uint32_t tick_t;
typedef bool bool_t;


typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;
/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Inicializa la estructura delay
 * @param delay puntero a delay
 * @param duration duración en ms
 */
void delayInit( delay_t * delay, tick_t duration );

/**
 * @brief Verifica si el delay se cumplió
 * @param delay puntero a delay
 * @return true si el tiempo se cumplió
 */
bool_t delayRead( delay_t * delay );

/**
 * @brief Devuelve el estado de ejecución del delay
 * @param delay Puntero a la estructura delay_t
 * @return true si el delay está corriendo
 * @return false si no está corriendo o si el puntero es inválido
 */
bool_t delayIsRunning( delay_t  * delay);

/**
 * @brief Modifica la duración del delay
 * @param delay puntero a delay
 * @param duration nueva duración
 */
void delayWrite( delay_t * delay, tick_t duration );



#endif /* API_DELAY_H_ */
