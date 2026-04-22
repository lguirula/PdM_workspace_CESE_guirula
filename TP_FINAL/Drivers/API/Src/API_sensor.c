/*
 * API_sensor.c
 *
 *  Created on: Apr 10, 2026
 *      Author: Ing. Guirula
 */



#include "api_sensor.h"
#include "api_lcd.h"

void API_Sensor_Init(hx710_t *sensor, GPIO_TypeDef* p_clk, uint16_t clk, GPIO_TypeDef* p_data, uint16_t data) {
    if (sensor == NULL) return;

    sensor->port_clk = p_clk;
    sensor->pin_clk = clk;
    sensor->port_data = p_data;
    sensor->pin_data = data;
    sensor->offset = 0;
    sensor->gain_pulses = 1; // Default Ganancia 128

    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);

    // FORZAR RESET DEL SENSOR:
        // Un pulso largo de High en CLK resetea el HX710
    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_SET);
    HAL_Delay(1); // 1ms es una eternidad para el sensor, suficiente para resetearlo
    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);
    HAL_Delay(1);
}

sensor_status_t API_Sensor_ReadRaw(hx710_t *sensor, int32_t *value)
{
    if (sensor == NULL || value == NULL)
        return SENSOR_ERROR;

    // 1. Esperar a que el sensor esté listo (DATA en LOW)
    // Agregamos un pequeño timeout para que no devuelva BUSY por un microsegundo de delay
    uint32_t timeout = 10000;
    while (HAL_GPIO_ReadPin(sensor->port_data, sensor->pin_data) == GPIO_PIN_SET) {
        if (--timeout == 0) return SENSOR_BUSY;
    }

    int32_t raw = 0;

    // 2. Leer los 24 bits uno por uno
    for (int i = 0; i < 24; i++)
    {
        HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_SET);

        // Delay corto (el HX711 aguanta hasta 20MHz, con 50-100 iteraciones sobra)
        for (volatile int d = 0; d < 100; d++);

        raw <<= 1; // Desplazamos para hacer lugar al nuevo bit
        if (HAL_GPIO_ReadPin(sensor->port_data, sensor->pin_data) == GPIO_PIN_SET) {
            raw++;
        }

        HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);
        for (volatile int d = 0; d < 100; d++);
    }

    // 3. Pulso 25 (Setea ganancia 128 para la próxima lectura)
    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++);
    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);
    for (volatile int d = 0; d < 100; d++);

    // 4. Extensión de signo (Fundamental para valores negativos/tara)
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }


    *value = raw;
    return SENSOR_OK;
}

int32_t API_Sensor_GetValue(hx710_t *sensor, uint8_t muestras)
{
    int32_t raw_actual = 0;
    int64_t acumulado = 0; // Usamos 64 bits para evitar desbordamiento en la suma
    uint8_t exitos = 0;

    for (uint8_t i = 0; i < muestras; i++) {
        if (API_Sensor_ReadRaw(sensor, &raw_actual) == SENSOR_OK) {
            acumulado += raw_actual;
            exitos++;
        }
        HAL_Delay(5); // Pequeño respiro entre lecturas
    }

    if (exitos == 0) return 0;

    // Retornamos el promedio menos el offset (el valor neto)
    return (int32_t)(acumulado / exitos) - sensor->offset;
}

/**
 * @brief Autocalibración (Tara)
 * Lee el sensor varias veces y guarda el promedio como offset
 */
void API_Sensor_Calibrate(hx710_t *sensor) {
    int64_t acumulado = 0;
    int32_t lectura = 0;
    uint8_t muestras_ok = 0;

    // Promediamos 20 muestras para un cero bien estable
    for (uint8_t i = 0; i < 20; i++) {
        if (API_Sensor_ReadRaw(sensor, &lectura) == SENSOR_OK) {
            acumulado += lectura;
            muestras_ok++;
        }
        HAL_Delay(20);
    }

    if (muestras_ok > 0) {
        sensor->offset = (int32_t)(acumulado / muestras_ok);
    }
}


float API_Sensor_To_mmHg(hx710_t *sensor, int32_t valor_neto) {
    if (sensor == NULL || sensor->sensitivity == 0.0f) {
        return 0.0f;
    }

    // El valor_neto ya viene sin offset gracias a GetValue
    return (float)valor_neto * sensor->sensitivity;
}
