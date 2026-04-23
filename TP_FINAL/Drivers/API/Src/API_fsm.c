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

    // Se inicializa el hardware una sola vez al inicio para asegurar
    // que el sensor y el LCD estén en un estado conocido antes de operar.
    // Esto evita lecturas inválidas en los primeros ciclos.
    delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);
    API_LCD_Init(&hi2c1);
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
        	API_LCD_Init(&hi2c1);
        	API_Sensor_Init(&mi_sensor, GPIOA, GPIO_PIN_7, GPIOA, GPIO_PIN_6);


        	HAL_Delay(1000);

        	// Se realiza la tara para eliminar el offset del sensor.
        	// Esto permite que todas las mediciones posteriores sean relativas
        	// al punto de referencia (0 mmHg), mejorando la precisión.
        	API_LCD_Clear();
        	API_LCD_SendString("Calibrando Cero");
        	API_Sensor_Calibrate(&mi_sensor);


        	// Se introduce una espera para permitir al usuario aplicar una presión conocida.
        	// Esta referencia es necesaria para calcular la sensibilidad del sensor.
        	API_LCD_Clear();
        	API_LCD_SendString("Poner 80 mmHg");
        	API_LCD_SetCursor(1,0);
        	API_LCD_SendString("Tiene 5 seg...");
        	HAL_Delay(5000); // Te damos 5 segundos para inflar

        	estado_actual = ESTADO_CALIBRATE_SPAN;
            break;

		case ESTADO_CALIBRATE_SPAN:
			// Se calcula la pendiente de conversión RAW→mmHg utilizando un punto conocido.
			// Esto permite linealizar la medición sin necesidad de conocer
			// los parámetros internos del sensor.
			if (API_Sensor_Calibrate_Sensitivity(&mi_sensor, 80.0f) == SENSOR_OK) {
			        API_LCD_Clear();
			        API_LCD_SendString("Calibrado OK!");
			    } else {
			        API_LCD_Clear();
			        API_LCD_SendString("Error: Usando Def");
			        // Seteamos la que calculamos antes por default para poder ver algo
			        mi_sensor.sensitivity = 0.00001f;
			    }

			HAL_Delay(1000);
			delayInit(&timer_muestreo, TIEMPO_MUESTREO_MS);
			estado_actual = ESTADO_IDLE;
			break;

		case ESTADO_IDLE:
			// Se utiliza un temporizador no bloqueante para definir la frecuencia de muestreo.
			// Esto evita saturar el sensor y permite mantener la FSM responsiva.
            if (delayRead(&timer_muestreo)) {
                estado_actual = ESTADO_READ_SENSOR;
            }
            break;

        case ESTADO_READ_SENSOR:


        	if (API_Sensor_ReadPressure(&mi_sensor, &valor_raw, &presion_mmhg) != SENSOR_OK) {
        	        presion_mmhg = -999;
        	    }

         estado_actual = ESTADO_REPORT;
         break;

            break;

        case ESTADO_REPORT:
        	// Se separa la visualización de la adquisición para evitar bloquear
        	// la lectura del sensor con operaciones lentas como el LCD.
        	API_LCD_Clear();
        	snprintf(buffer, sizeof(buffer), "R:%ld", valor_raw);
        	API_LCD_SetCursor(0,0); API_LCD_SendString(buffer);
        	snprintf(buffer, sizeof(buffer), "P:%.2f mmHg", presion_mmhg);
        	API_LCD_SetCursor(1,0); API_LCD_SendString(buffer);
        	estado_actual = ESTADO_IDLE;

            break;

        case ESTADO_ERROR:
        	// Estado de seguridad que permite detectar fallas del sensor
        	// y evitar el uso de datos inválidos en el sistema.
        	Visualizar_Error("FALLA SENSOR");
            break;

        default:
            estado_actual = ESTADO_ERROR;
            break;
    }
}
