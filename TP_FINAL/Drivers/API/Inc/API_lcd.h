/*
 * API_lcd.h
 *
 *  Created on: Apr 20, 2026
 *      Author: Usuario
 */



#ifndef API_LCD_H_
#define API_LCD_H_

#include "stm32f4xx_hal.h"

#include <stdint.h>

/* --- CONFIGURACIÓN I2C --- */
#define LCD_I2C_ADDR    (0x27 << 1) // Dirección común 0x27 o 0x3F (desplazada para HAL)

/* --- COMANDOS LCD --- */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME   0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT  0x10
#define LCD_FUNCTIONSET  0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/* --- FLAGS --- */
#define LCD_DISPLAYON 0x04
#define LCD_CURSOROFF 0x00
#define LCD_BLINKOFF  0x00

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04  // Pin Enable
#define LCD_RS        0x01  // Pin Register Select

/* --- PROTOTIPOS PÚBLICOS --- */
void API_LCD_Init(I2C_HandleTypeDef *hi2c);
void API_LCD_SendString(char *str);
void API_LCD_SetCursor(uint8_t row, uint8_t col);
void API_LCD_Clear(void);
void Visualizar_Error(char* mensaje);
void Visualizar_Datos(float presion, int32_t raw);
#endif


