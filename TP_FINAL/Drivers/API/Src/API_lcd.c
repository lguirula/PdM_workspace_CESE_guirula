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
 * @brief Envía un comando o dato al LCD via I2C
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

void API_LCD_Init(I2C_HandleTypeDef *hi2c) {
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

void API_LCD_SendString(char *str) {
    while (*str) {
        LCD_Send_Internal(*str++, LCD_RS);
    }
}

void API_LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t mask = (row == 0) ? 0x80 : 0xC0;
    LCD_Send_Internal(mask | col, 0);
}

void API_LCD_Clear(void) {
    LCD_Send_Internal(LCD_CLEARDISPLAY, 0);
    HAL_Delay(2);
}


// --- FUNCIONES PARA VISUALIZACIÓN ---

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

void Visualizar_Error(char* mensaje) {
    API_LCD_Clear();
    API_LCD_SetCursor(0, 0);
    API_LCD_SendString("!!! ERROR !!!");
    API_LCD_SetCursor(1, 0);
    API_LCD_SendString(mensaje);
}
