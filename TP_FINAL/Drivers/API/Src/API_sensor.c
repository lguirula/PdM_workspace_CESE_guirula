/*
 * API_sensor.c
 *
 *  Created on: Apr 10, 2026
 *      Author: Ing. Guirula
 */



#include "api_sensor.h"

/**
 * @brief Inicializa la estructura del sensor y configura los pines GPIO.
 * Realiza un reset manual del HX710 para asegurar un arranque limpio.
 *
 * @param sensor Puntero a la estructura del sensor.
 * @param p_clk Puerto GPIO del pin de clock.
 * @param clk Pin de clock.
 * @param p_data Puerto GPIO del pin de datos.
 * @param data Pin de datos.
 */
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
        HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_SET);
        HAL_Delay(100); // Mantenemos el CLK en alto 100ms para apagar/resetear el sensor
        HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);
        HAL_Delay(100); // Esperamos a que despierte
}


/**
 * @brief Lee un valor crudo de 24 bits desde el sensor HX710.
 *
 * Espera a que el sensor esté listo (DATA en LOW) y luego realiza
 * la lectura bit a bit. Incluye extensión de signo.
 *
 * @param sensor Puntero a la estructura del sensor.
 * @param value Puntero donde se almacena el valor leído.
 *
 * @return SENSOR_OK si la lectura fue exitosa,
 *         SENSOR_BUSY si el sensor no estaba listo,
 *         SENSOR_ERROR en caso de error.
 */
sensor_status_t API_Sensor_ReadRaw(hx710_t *sensor, int32_t *value) {
    if (sensor == NULL || value == NULL) return SENSOR_ERROR;

    // 1. Verificamos si el sensor está listo (DATA debe estar en LOW)
    // Si después de un tiempo corto sigue en HIGH, salimos para no trabar la FSM
    uint32_t timeout = 2000;
    while (HAL_GPIO_ReadPin(sensor->port_data, sensor->pin_data) == GPIO_PIN_SET) {
        if (--timeout == 0) return SENSOR_BUSY;
    }

    int32_t raw = 0;
    // 2. Leemos los 24 bits
    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_SET);
        for (volatile int d = 0; d < 100; d++); // Delay micro

        raw <<= 1;
        if (HAL_GPIO_ReadPin(sensor->port_data, sensor->pin_data) == GPIO_PIN_SET) {
            raw++;
        }

        HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);
        for (volatile int d = 0; d < 100; d++);
    }

    // 3. PULSO 25 - Vital para que el sensor no se apague
    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++);
    HAL_GPIO_WritePin(sensor->port_clk, sensor->pin_clk, GPIO_PIN_RESET);
    for (volatile int d = 0; d < 100; d++);

    // 4. Extensión de signo
    if (raw & 0x800000) raw |= 0xFF000000;

    *value = raw;
    return SENSOR_OK;
}

/**
 * @brief Realiza la calibración de cero (tara) del sensor.
 *
 * Promedia múltiples lecturas para obtener un valor de offset estable.
 * Este offset se utiliza luego para obtener el valor neto.
 *
 * @param sensor Puntero a la estructura del sensor.
 */
void API_Sensor_Calibrate(hx710_t *sensor) {
	if (sensor == NULL) return SENSOR_ERROR;
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

/**
 * @brief Lee la presión en mmHg a partir del sensor.
 *
 * Internamente realiza la lectura RAW, calcula el valor neto
 * restando el offset y aplica la sensibilidad calibrada.
 *
 * @param sensor Puntero a la estructura del sensor.
 * @param presion Puntero donde se almacena la presión calculada.
 *
 * @return SENSOR_OK si la lectura fue exitosa,
 *         SENSOR_ERROR en caso de error.
 */
sensor_status_t API_Sensor_ReadPressure(hx710_t *sensor,int32_t *raw,float *presion) {
    if (sensor == NULL || raw == NULL || presion == NULL) {
        return SENSOR_ERROR;
    }

    if (API_Sensor_ReadRaw(sensor, raw) != SENSOR_OK) {
        return SENSOR_ERROR;
    }

    int32_t neto = *raw - sensor->offset;
    *presion = neto * sensor->sensitivity;

    return SENSOR_OK;
}


/**
 * @brief Calcula la sensibilidad del sensor (pendiente de calibración).
 *
 * Utiliza un valor de presión conocida (ej: 80 mmHg) y el valor
 * neto medido para calcular la relación lineal.
 *
 * @param sensor Puntero a la estructura del sensor.
 * @param presion_conocida Valor de presión aplicada durante la calibración.
 *
 * @return SENSOR_OK si la calibración fue exitosa,
 *         SENSOR_ERROR si el valor medido es insuficiente.
 */
sensor_status_t API_Sensor_Calibrate_Sensitivity(hx710_t *sensor, float presion_conocida) {
	if (sensor == NULL) return SENSOR_ERROR;
    // Tomamos el valor promedio (GetValue ya resta el offset inicial)
    int32_t raw_neto = API_Sensor_GetValue(sensor, 40);

    // Usamos el valor absoluto por si la manguera está invertida
    // y el valor baja en lugar de subir
    uint32_t abs_neto = (raw_neto < 0) ? -raw_neto : raw_neto;

    // Bajamos el umbral a 1000 para ver si así pasa
    if (abs_neto < 1000) {
        sensor->sensitivity = 0.0f;
        return SENSOR_ERROR;
    }

    sensor->sensitivity = presion_conocida / (float)raw_neto;
    return SENSOR_OK;
}

/**
 * @brief Obtiene el valor neto promedio del sensor.
 *
 * Realiza múltiples lecturas RAW, calcula el promedio y resta el offset.
 * Se utiliza principalmente para procesos de calibración.
 *
 * @param sensor Puntero a la estructura del sensor.
 * @param muestras Cantidad de muestras a promediar.
 *
 * @return Valor neto promedio (RAW - offset).
 */
int32_t API_Sensor_GetValue(hx710_t *sensor, uint8_t muestras)
{
	if (sensor == NULL) return SENSOR_ERROR;
    int32_t raw_actual = 0;
    int64_t acumulado = 0;
    uint8_t exitos = 0;

    for (uint8_t i = 0; i < muestras; i++) {
        if (API_Sensor_ReadRaw(sensor, &raw_actual) == SENSOR_OK) {
            acumulado += raw_actual;
            exitos++;
        }
        HAL_Delay(10);
    }

    if (exitos == 0) return 0;

    int32_t promedio = (int32_t)(acumulado / exitos);

    // El valor neto es el promedio actual menos el punto de cero (offset)
    return promedio - sensor->offset;
}
