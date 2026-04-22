/*
 * API_sensor.h
 *
 *  Created on: Apr 10, 2026
 *      Author: Ing. Guirula Lucia
 */


#ifndef API_SENSOR_H_
#define API_SENSOR_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
/* --- DEFINES ----------------------------------------------------------------------------------------- */
#define SENSOR_BITS 24
#define VREF_MV          5000.0f    // Milivoltios
#define ADC_MAX_COUNTS   16777216.0f // 2^24
#define HX710_GAIN       128.0f
#define KPA_TO_MMHG      7.50062f


/* --- TIPOS DE DATOS ---------------------------------------------------------------------------------- */
typedef struct {
    GPIO_TypeDef* port_clk;
    uint16_t pin_clk;
    GPIO_TypeDef* port_data;
    uint16_t pin_data;
    int32_t offset;      // Para la autocalibración (tara)
    uint8_t gain_pulses; // 1 pulso = Ganancia 128 (Canal A)
    int32_t sensitivity;
} hx710_t;

typedef enum {
    SENSOR_OK,
    SENSOR_ERROR,
    SENSOR_BUSY
} sensor_status_t;

/* --- PROTOTIPOS PÚBLICOS --- */
void API_Sensor_Init(hx710_t *sensor, GPIO_TypeDef* p_clk, uint16_t clk, GPIO_TypeDef* p_data, uint16_t data);
sensor_status_t API_Sensor_ReadRaw(hx710_t *sensor, int32_t *value);
void API_Sensor_Calibrate(hx710_t *sensor);
int32_t API_Sensor_GetValue(hx710_t *sensor, uint8_t muestras);

#endif /*API_SENSOR_H_*/
