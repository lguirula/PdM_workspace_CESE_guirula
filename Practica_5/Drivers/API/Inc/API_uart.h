/*
 * API_uart.h
 *
 *  Created on: Abr 2, 2026
 *      Author: Ing. Lucia Guirula
 */

#ifndef API_UART_H_
#define API_UART_H_

/* Includes ------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>

/* Typedefs ------------------------------------------------------------------*/
typedef bool bool_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Recibe un byte desde la UART en modo polling.
 *
 * Esta función intenta leer un solo byte desde la UART utilizando la HAL.
 * Utiliza un timeout corto para evitar bloquear la ejecución del programa.
 *
 * @param c Puntero donde se almacenará el byte recibido.
 *
 * @return true  Si se recibió correctamente un byte.
 * @return false Si no se recibió ningún dato o si el puntero es NULL.
 *
 * @note Esta función es utilizada por el parser para leer caracteres
 *       de forma incremental sin detener el programa principal.
 */
bool_t uartReceiveByte(uint8_t *c);

/**
 * @brief Inicializa la UART.
 *
 * Configura la UART con los parámetros definidos en el módulo y envía
 * por la terminal serie un mensaje con la configuración aplicada.
 *
 * @return true si la inicialización fue exitosa
 * @return false si ocurrió un error en la inicialización
 */
bool_t uartInit();

/**
 * @brief Envía un string completo por UART.
 *
 * Envía todos los caracteres del string hasta encontrar '\0'.
 *
 * @param pstring Puntero al string a enviar
 */
void uartSendString(uint8_t * pstring);

/**
 * @brief Envía una cantidad específica de bytes por UART.
 *
 * @param pstring Puntero al buffer a enviar
 * @param size Cantidad de bytes a transmitir (1 a 256)
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size);

/**
 * @brief Recibe una cantidad específica de bytes por UART.
 *
 * @param pstring Puntero al buffer donde se almacenan los datos recibidos
 * @param size Cantidad de bytes a recibir (1 a 256)
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);


#endif
