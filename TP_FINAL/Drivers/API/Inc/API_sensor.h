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

#define HX710_GAIN       128.0f



/* --- TIPOS DE DATOS ---------------------------------------------------------------------------------- */
typedef struct {
    GPIO_TypeDef* port_clk;
    uint16_t pin_clk;
    GPIO_TypeDef* port_data;
    uint16_t pin_data;
    int32_t offset;      // Para la autocalibración (tara)
    uint8_t gain_pulses; // 1 pulso = Ganancia 128 (Canal A)
    float sensitivity;
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
sensor_status_t API_Sensor_Calibrate_Sensitivity(hx710_t *sensor, float presion_conocida);
int32_t API_Sensor_GetValue(hx710_t *sensor, uint8_t muestras);
sensor_status_t API_Sensor_ReadPressure(hx710_t *sensor,int32_t *raw,float *presion);

#endif /*API_SENSOR_H_*/
