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
	ESTADO_CALIBRATE_SPAN,
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
    API_LCD_Clear();
    API_LCD_SendString("Inicializando");

    delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);
    HAL_Delay(1000);

}

void FSM_App_Update(void) {
	char buffer[16]; // Buffer para el LCD
    switch (estado_actual) {

        case ESTADO_INIT:
        	// 1. Inicializar Hardware
        	API_LCD_Init(&hi2c1); // <-- Agregado
        	API_Sensor_Init(&mi_sensor, GPIOA, GPIO_PIN_7, GPIOA, GPIO_PIN_6);
        	// Ponemos un valor de sensibilidad por defecto (el que calculamos)

        	HAL_Delay(1000);

        	// 1. Tara (Cero mmHg) - Asegúrate de no tocar el sensor
        	API_LCD_Clear();
        	API_LCD_SendString("Calibrando Cero");
        	API_Sensor_Calibrate(&mi_sensor); // Esta función ya resta el offset


        	// 2. Pasamos al estado de esperar los 80mmHg
        	API_LCD_Clear();
        	API_LCD_SendString("Poner 80 mmHg");
        	API_LCD_SetCursor(1,0);
        	API_LCD_SendString("Tiene 5 seg...");
        	HAL_Delay(5000); // Te damos 5 segundos para inflar

        	estado_actual = ESTADO_CALIBRATE_SPAN;
            break;

		case ESTADO_CALIBRATE_SPAN:
			/*if (API_Sensor_Calibrate_Sensitivity(&mi_sensor, 80.0f) == SENSOR_OK) {
			        API_LCD_Clear();
			        API_LCD_SendString("Calibrado OK!");
			        HAL_Delay(1000);
			        delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);
			        estado_actual = ESTADO_IDLE;
			} else {
			        // Si entra acá es porque el RAW no cambió al inflar
			        estado_actual = ESTADO_ERROR;
			    }*/
			if (API_Sensor_Calibrate_Sensitivity(&mi_sensor, 80.0f) == SENSOR_OK) {
			        API_LCD_Clear();
			        API_LCD_SendString("Calibrado OK!");
			    } else {
			        API_LCD_Clear();
			        API_LCD_SendString("Error: Usando Def");
			        // Seteamos la que calculamos antes por default para poder ver algo
			        mi_sensor.sensitivity = 0.00096f;
			    }
			char buffer[16];
			snprintf(buffer, sizeof(buffer), "S:%.6f", mi_sensor.sensitivity);
			API_LCD_Clear();
			API_LCD_SendString(buffer);
			HAL_Delay(2000);
			HAL_Delay(1000);
			delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);
			estado_actual = ESTADO_IDLE;
			break;

		case ESTADO_IDLE:

            if (delayRead(&timer_muestreo)) {
                estado_actual = ESTADO_READ_SENSOR;
            }
            break;

        case ESTADO_READ_SENSOR:
        	// 1. Intentamos una lectura simple (sin promedios pesados por ahora)
        	    sensor_status_t status = API_Sensor_ReadRaw(&mi_sensor, &valor_raw);

        	    if (status == SENSOR_OK) {
        	        int32_t valor_neto = valor_raw - mi_sensor.offset;
        	        presion_mmhg = (float)valor_neto * mi_sensor.sensitivity ;
        	    } else {
        	        // Para debug:
        	        presion_mmhg = -999;
        	    }
        	    estado_actual = ESTADO_REPORT;
            break;

        case ESTADO_REPORT:
        	//Visualizar_Datos(presion_mmhg, valor_raw);
        	snprintf(buffer, sizeof(buffer), "R:%ld", valor_raw);
        	API_LCD_SetCursor(0,0);
        	API_LCD_SendString(buffer);

        	snprintf(buffer, sizeof(buffer), "P:%.2f", presion_mmhg);
        	API_LCD_SetCursor(1,0);
        	API_LCD_SendString(buffer);
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
