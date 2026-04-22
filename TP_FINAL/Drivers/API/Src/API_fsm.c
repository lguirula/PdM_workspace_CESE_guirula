/*
 * API_fsm.c
 *
 *  Created on: Apr 10, 2026
 *      Author: Ing. Guirula Lucía
 */

#include "API_fsm.h"
#include "API_sensor.h"
#include "API_lcd.h"

/* --- TIPOS PRIVADOS ------------------------------------------------------------------------ */
typedef enum {
	ESTADO_INIT,
    ESTADO_IDLE,
    ESTADO_READ_SENSOR,
    ESTADO_PROCESS,
    ESTADO_REPORT,
    ESTADO_ERROR
} State_t;

/* --- VARIABLES PRIVADAS -------------------------------------------------------------------- */
static State_t estado_actual;
static delay_t timer_muestreo;
static hx710_t mi_sensor;
static int32_t valor_raw;
static float presion_mmhg;
extern I2C_HandleTypeDef hi2c1;

/* --- IMPLEMENTACIÓN -------------------------------------------------------------------------- */

void FSM_App_Init(void) {
    estado_actual = ESTADO_INIT;


    delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);
    API_LCD_Init(&hi2c1); // <-- Agregado
    API_Sensor_Init(&mi_sensor, GPIOA, GPIO_PIN_7, GPIOA, GPIO_PIN_6);
    // Ponemos un valor de sensibilidad por defecto (el que calculamos)
    mi_sensor.sensitivity = 0.000009612f;
    API_LCD_Clear();
    API_LCD_SendString("Estabilizando...");

    // 1. Dale tiempo al sensor para que "despierte" (Vital para HX711/710)
    HAL_Delay(1000);
    // 2. Feedback visual inicial
    API_LCD_Clear();
    API_LCD_SendString("Calibrando...");

    // 3. Autocalibración (Tara)
    API_Sensor_Calibrate(&mi_sensor);

    // 4. Configurar el timer de muestreo
    delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);

    API_LCD_Clear();
    API_LCD_SendString("Sistema Listo");
    HAL_Delay(500);
}

void FSM_App_Update(void) {
	char buffer[16]; // Buffer para el LCD
    switch (estado_actual) {

        case ESTADO_INIT:
        	// 1. Inicializar Hardware
        	API_LCD_Init(&hi2c1); // <-- Agregado
        	API_Sensor_Init(&mi_sensor, GPIOA, GPIO_PIN_7, GPIOA, GPIO_PIN_6);
        	// Ponemos un valor de sensibilidad por defecto (el que calculamos)
        	mi_sensor.sensitivity = 0.000009612f;
        	API_LCD_Clear();
        	API_LCD_SendString("Estabilizando...");

        	// 1. Dale tiempo al sensor para que "despierte" (Vital para HX711/710)
        	HAL_Delay(1000);
        	// 2. Feedback visual inicial
        	API_LCD_Clear();
        	API_LCD_SendString("Calibrando...");

        	// 3. Autocalibración (Tara)
        	API_Sensor_Calibrate(&mi_sensor);

        	// 4. Configurar el timer de muestreo
        	delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);

        	API_LCD_Clear();
        	API_LCD_SendString("Sistema Listo");
        	HAL_Delay(500);

        	estado_actual = ESTADO_IDLE;
            break;

		case ESTADO_IDLE:

            if (delayRead(&timer_muestreo)) {
                estado_actual = ESTADO_READ_SENSOR;
            }
            break;

        case ESTADO_READ_SENSOR:
        	valor_raw = API_Sensor_GetValue(&mi_sensor, 3);
        	// Ejemplo de detección de error (Cable suelto)
        	if (valor_raw == 0) {
        		estado_actual = ESTADO_ERROR;
        	} else {
        		estado_actual = ESTADO_PROCESS;
        	}
            break;

        case ESTADO_PROCESS:
        	presion_mmhg = API_Sensor_To_mmHg(&mi_sensor, valor_raw);

            estado_actual = ESTADO_REPORT;
            break;

        case ESTADO_REPORT:
        	Visualizar_Datos(presion_mmhg, valor_raw);
            estado_actual = ESTADO_IDLE;

            break;

        case ESTADO_ERROR:
        	Visualizar_Error("FALLA SENSOR");
            break;

        default:
            estado_actual = ESTADO_ERROR;
            break;
    }
}
