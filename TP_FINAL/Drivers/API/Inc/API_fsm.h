/*
 * API_fsm.h
 *
 *  Created on: Apr 10, 2026
 *      Author: Ing. Guirula Lucia
 */

#ifndef API_FSM_H_
#define API_FSM_H_


#include <stdint.h>
#include <stdbool.h>
#include "API_delay.h"

/* --- DEFINES----------------------------------------------------------------------- */
#define TIEMPO_MUESTREO_MS 500  // Tiempo entre lecturas


/* --- PROTOTIPOS PÚBLICOS----------------------------------------------------------- */

void FSM_App_Init(void);
void FSM_App_Update(void);



#endif /* API_FSM_H_ */
