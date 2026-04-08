/*
 * API_cmdparser.h
 *
 *  Created on: Abr 2, 2026
 *      Author: Ing. Lucia Guirula
 */

#ifndef API_CMDPARSER_H_
#define API_CMDPARSER_H_

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

/* Defines ------------------------------------------------------------------*/

#define CMD_MAX_LINE 64 // Incluye '\0'
#define CMD_MAX_TOKENS 3  // comando + hasta 2 argumentos

/**
 * @brief Estados de la MEF del parser de comandos
 *
 * Permite recibir caracteres por UART, formar comandos completos
 * y ejecutarlos de manera no bloqueante.
 */
typedef enum {
    CMD_OK = 0,
    CMD_ERR_OVERFLOW,
    CMD_ERR_SYNTAX,
    CMD_ERR_UNKNOWN,
    CMD_ERR_ARG
} cmd_status_t;
/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Inicializa el módulo parser de comandos.
 *
 * Esta función prepara el parser para comenzar a recibir comandos:
 * - Reinicia el índice del buffer.
 * - Limpia completamente el buffer de recepción.
 *
 * Debe llamarse una única vez al inicio del programa.
 */
void cmdParserInit(void);

/**
 * @brief Máquina de estados del parser en modo polling.
 *
 * Esta función debe ser llamada periódicamente desde el bucle principal.
 * Su función es:
 * - Leer caracteres desde la UART (uno por vez).
 * - Construir un string (línea de comando).
 * - Detectar el final de línea (\r o \n).
 * - Procesar el comando recibido.
 *
 * Funcionamiento:
 * 1. Si llega un carácter:
 *    - Si es ENTER → procesa la línea completa.
 *    - Si no → lo guarda en el buffer.
 *
 * 2. Si el buffer se llena:
 *    - Se informa un error de overflow.
 *    - Se reinicia el buffer.
 *
 * @note Esta implementación es no bloqueante y permite que el sistema
 *       continúe ejecutando otras tareas en paralelo.
 */
void cmdPoll(void);

/**
 * @brief Imprime por UART la lista de comandos disponibles.
 *
 * Envía al usuario, a través de la terminal serie, los comandos
 * que el sistema reconoce y puede ejecutar.
 *
 * Comandos disponibles:
 * - HELP        : Muestra esta ayuda
 * - LED ON      : Enciende el LED
 * - LED OFF     : Apaga el LED
 * - LED TOGGLE  : Invierte el estado del LED
 * - STATUS      : Informa el estado actual del LED
 */
void cmdPrintHelp(void);
#endif
