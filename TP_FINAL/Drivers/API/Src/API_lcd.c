/*
 * API_lcd.c
 *
 *  Created on: Apr 20, 2026
 *      Author: Usuario
 */


#include "API_lcd.h"
#include <stdio.h>


static I2C_HandleTypeDef *hlcd_i2c;

/**
 * @brief Envía un comando o dato al LCD en modo 4 bits vía I2C.
 *
 * Divide el byte en dos nibbles (alto y bajo) y genera los pulsos
 * de enable necesarios para que el LCD registre cada parte.
 *
 * @param data Byte a enviar (comando o carácter).
 * @param flags Control de registro (RS = dato / comando).
 *
 * @note Esta función es interna porque implementa el protocolo
 * específico del LCD y no debe ser utilizada directamente por la aplicación.
 */
static void LCD_Send_Internal(uint8_t data, uint8_t flags) {
    uint8_t up = (data & 0xF0) | flags | LCD_BACKLIGHT;
    uint8_t lo = ((data << 4) & 0xF0) | flags | LCD_BACKLIGHT;

    uint8_t data_t[4];
    data_t[0] = up | LCD_ENABLE;
    data_t[1] = up;
    data_t[2] = lo | LCD_ENABLE;
    data_t[3] = lo;

    HAL_I2C_Master_Transmit(hlcd_i2c, LCD_I2C_ADDR, data_t, 4, 100);
    HAL_Delay(1);
}

/**
 * @brief Inicializa el LCD en modo 4 bits usando interfaz I2C.
 *
 * Ejecuta la secuencia de inicialización requerida por el controlador
 * HD44780 para asegurar una correcta configuración del display.
 *
 * @param hi2c Handler del periférico I2C utilizado.
 *
 * @note La secuencia de inicialización es crítica y debe respetar
 * los tiempos especificados en el datasheet para evitar fallos.
 */
void API_LCD_Init(I2C_HandleTypeDef *hi2c) {
	// Se valida el puntero al periférico I2C para evitar inicializar
	// el driver con una referencia inválida, lo que causaría fallas
	// en la comunicación con el LCD.
	if (hi2c == NULL) {
	        return; // No se puede inicializar sin I2C válido
	    }
    hlcd_i2c = hi2c;

    // Secuencia de inicialización modo 4 bits
    HAL_Delay(50);
    LCD_Send_Internal(0x30, 0);
    HAL_Delay(5);
    LCD_Send_Internal(0x30, 0);
    HAL_Delay(1);
    LCD_Send_Internal(0x32, 0);
    HAL_Delay(10);

    // Configuración del display
    LCD_Send_Internal(LCD_FUNCTIONSET | 0x08, 0); // 2 líneas, 5x8 font
    LCD_Send_Internal(LCD_DISPLAYCONTROL | LCD_DISPLAYON, 0);
    LCD_Send_Internal(LCD_ENTRYMODESET | 0x02, 0);
    API_LCD_Clear();
}

/**
 * @brief Envía una cadena de caracteres al LCD.
 *
 * Recorre la cadena carácter por carácter y los envía como datos.
 *
 * @param str Puntero a la cadena a mostrar.
 *
 * @note No realiza control de longitud, por lo que se recomienda
 * enviar cadenas acordes al tamaño del display (ej: 16x2).
 */
void API_LCD_SendString(char *str) {
	// Se verifica que el puntero sea válido para evitar fallos de ejecución

	if (str == NULL) {
	        return;
	    }
    while (*str) {
        LCD_Send_Internal(*str++, LCD_RS);
    }
}

/**
 * @brief Posiciona el cursor en una fila y columna específicas.
 *
 * Traduce la fila y columna a la dirección DDRAM del LCD.
 *
 * @param row Fila (0 o 1).
 * @param col Columna (0 a 15).
 *
 * @note El mapeo de memoria del LCD no es lineal, por eso
 * se utilizan máscaras específicas (0x80, 0xC0).
 */

void API_LCD_SetCursor(uint8_t row, uint8_t col) {
	// Validación de parámetros
	// Se valida el rango de fila y columna para evitar accesos inválidos
	// a la memoria interna del LCD (DDRAM), lo que podría generar
	// comportamientos erráticos en el display.
	    if (row > 1 || col > 15) {
	        return; // Ignoramos valores inválidos
	    }
    uint8_t mask = (row == 0) ? 0x80 : 0xC0;
    LCD_Send_Internal(mask | col, 0);
}

/**
 * @brief Limpia el contenido del display.
 *
 * Envía el comando de clear y espera el tiempo necesario
 * para que el LCD complete la operación.
 *
 * @note El clear es una operación lenta en el LCD, por lo que
 * se incluye un delay para evitar corrupción de datos.
 */
void API_LCD_Clear(void) {
    LCD_Send_Internal(LCD_CLEARDISPLAY, 0);
    HAL_Delay(2);
}


// --- FUNCIONES PARA VISUALIZACIÓN ---

/**
 * @brief Muestra la presión y el valor RAW en el LCD.
 *
 * Presenta la presión en mmHg en la primera línea y el valor
 * crudo del sensor en la segunda línea.
 *
 * @param presion Valor de presión calculado.
 * @param raw Valor crudo del sensor.
 *
 * @note Se agregan espacios al final para sobrescribir
 * caracteres residuales de lecturas anteriores.
 */
void Visualizar_Datos(float presion, int32_t raw) {
    char buf[16];

    // Línea 1: Presión principal
    sprintf(buf, "P: %.1f mmHg   ", presion);
    API_LCD_SetCursor(0, 0);
    API_LCD_SendString(buf);

    // Línea 2: Valor raw (útil para debug)
    sprintf(buf, "R: %ld      ", raw);
    API_LCD_SetCursor(1, 0);
    API_LCD_SendString(buf);
}

/**
 * @brief Muestra un mensaje de error en el LCD.
 *
 * Limpia el display y presenta un mensaje de error en dos líneas.
 *
 * @param mensaje Texto descriptivo del error.
 *
 * @note Permite centralizar la visualización de fallas
 * y mejorar la interacción con el usuario.
 */
void Visualizar_Error(char* mensaje) {
    API_LCD_Clear();
    API_LCD_SetCursor(0, 0);
    API_LCD_SendString("!!! ERROR !!!");
    API_LCD_SetCursor(1, 0);
    API_LCD_SendString(mensaje);
}
