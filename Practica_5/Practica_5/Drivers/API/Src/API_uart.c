/*
 * API_uart.c
 *
 *  Created on: Abr 2, 2026
 *      Author: Ing. Lucia Guirula
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"


/* Variable privada del módulo */
static UART_HandleTypeDef huart2;
static const timeout = 100;
static char newLine[] = "\n\r";

/* Functions -------------------------------------------------------*/
bool uartInit(){

		char buffer[120];
		/* Configuración de la UART */
	    huart2.Instance = USART2;
	    huart2.Init.BaudRate = 115200;
	    huart2.Init.WordLength = UART_WORDLENGTH_8B;
	    huart2.Init.StopBits = UART_STOPBITS_1;
	    huart2.Init.Parity = UART_PARITY_NONE;
	    huart2.Init.Mode = UART_MODE_TX_RX;
	    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	    //Error_Handler();
	    if (HAL_UART_Init(&huart2) != HAL_OK)
	    {
	        return false;
	    }


	 /* Mensaje de configuración */
	    /*
	     * Si la inicialización fue exitosa, se envían por UART
	     * los parámetros de configuración para verificación.
	     */

	   snprintf(buffer, sizeof(buffer),
			   "UART CONFIG:\r\n"
			   "Baudrate: %lu\r\n"
	           "WordLength: %lu\r\n"
	           "StopBits: %lu\r\n"
	           "Parity: %lu\r\n"
	           "Mode: %lu\r\n"
	           "HwFlowCtl: %lu\r\n"
	           "OverSampling: %lu\r\n\r\n",
	           huart2.Init.BaudRate,
	           huart2.Init.WordLength,
	           huart2.Init.StopBits,
	           huart2.Init.Parity,
	           huart2.Init.Mode,
	           huart2.Init.HwFlowCtl,
	           huart2.Init.OverSampling);

	 HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), timeout);
	 return true;

}

/*
 * @brief Envía un string completo por UART (hasta '\0').
 *
 * @param pstring Puntero al string a enviar.
 *
 * @note La función calcula automáticamente la longitud del string.
 *       Agrega un salto de línea al final.
 */
void uartSendString(uint8_t * pstring){
	/* Validación de puntero */
	if (pstring == NULL)
	    return;

	/* Obtener longitud del string */
	uint16_t size = strlen((char*)pstring);

	/* Transmitir string */
	if (HAL_UART_Transmit(&huart2, pstring, size, timeout) != HAL_OK)
		return;

	/* Enviar salto de línea */
	HAL_UART_Transmit(&huart2, (uint8_t *)newLine , 2, timeout);

}

/*
 * @brief Envía una cantidad específica de bytes por UART.
 *
 * @param pstring Puntero al buffer a enviar.
 * @param size Cantidad de bytes a transmitir (1 a 256).
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size){

	/* Validación de parámetros */
	if (pstring == NULL)
	   return;

	if (size == 0 || size > 256)
	   return;
	/* Transmitir datos */
	if (HAL_UART_Transmit(&huart2, pstring, size, timeout) != HAL_OK)
	   return;
	/* Salto de línea*/
	HAL_UART_Transmit(&huart2, (uint8_t *)newLine, 2, timeout);

}

/*
 * @brief Recibe una cantidad específica de bytes por UART.
 *
 * @param pstring Puntero al buffer donde se almacenarán los datos.
 * @param size Cantidad de bytes a recibir (1 a 256).
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size){

	/* Validación de parámetros */
	if (pstring == NULL)
	        return;

	if (size == 0 || size > 256)
	        return;
	/* Recepción de datos */
	if (HAL_UART_Receive(&huart2, pstring, size, timeout) != HAL_OK)
	        return;


}

bool uartReceiveByte(uint8_t *c)
{
    if (c == NULL)
        return false;

    if (HAL_UART_Receive(&huart2, c, 1, 10) == HAL_OK)
        return true;

    return false;
}
