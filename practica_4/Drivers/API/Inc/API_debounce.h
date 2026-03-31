/*
 * API_debounce.h
 *
 *  Created on: Mar 29, 2026
 *      Author: Ing. Lucia Guirula
 */

#ifndef API_DEBOUNCE_H_
#define API_DEBOUNCE_H_

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

/* Typedefs ------------------------------------------------------------------*/
typedef bool bool_t;

/* Function prototypes -------------------------------------------------------*/
void debounceFSM_init(void); // @brief Inicializa la máquina de estados de anti-rebote.
void debounceFSM_update(void); //@brief Actualiza la máquina de estados del pulsador.
bool_t readKey(void); //@brief Devuelve si se presionó la tecla

#endif
