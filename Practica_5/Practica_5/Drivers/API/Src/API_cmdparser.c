/*
 * API_cmdparser.c
 *
 *  Created on: Abr 2, 2026
 *      Author: Ing. Lucia Guirula
 */

/* Includes ------------------------------------------------------------------*/

#include "API_cmdparser.h"
#include "API_uart.h"
#include "main.h"
#include <string.h>
#include <ctype.h>
#include "stm32f4xx_hal.h"

/* ================= VARIABLES PRIVADAS ================= */

static uint8_t rxChar;                         // carácter recibido
static char cmdBuffer[CMD_MAX_LINE];          // buffer de línea
static uint8_t bufferIndex=0;                     // posición actual

/* ================= FUNCIONES PRIVADAS ================= */

/**
 * @brief Convierte un string a mayúsculas (in-place)
 *
 * Recorre el string carácter por carácter y convierte cada uno
 * a su equivalente en mayúscula utilizando la función toupper().
 *
 * Esta función modifica directamente el contenido del string original.
 *
 * @param str Puntero al string a convertir (debe ser distinto de NULL)
 *
 * @note Se castea a (unsigned char) porque toupper() lo requiere
 *       para evitar comportamiento indefinido con caracteres negativos.
 */
static void toUpperCase(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

/**
 * @brief Ejecuta el comando parseado
 *
 * @param cmd Comando principal
 * @param arg1 Primer argumento
 * @param arg2 Segundo argumento
 *
 * @return cmd_status_t Estado de ejecución
 */
static cmd_status_t cmdExecute(char *cmd, char *arg1, char *arg2)
{
    if (cmd == NULL)
        return CMD_ERR_SYNTAX;

    /* HELP */
    if (strcmp(cmd, "HELP") == 0)
    {
        cmdPrintHelp();
        return CMD_OK;
    }

    /* LED */
    if (strcmp(cmd, "LED") == 0)
    {
        if (arg1 == NULL)
            return CMD_ERR_ARG;

        if (strcmp(arg1, "ON") == 0)
        {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            return CMD_OK;
        }
        else if (strcmp(arg1, "OFF") == 0)
        {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            return CMD_OK;
        }
        else if (strcmp(arg1, "TOGGLE") == 0)
        {
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            return CMD_OK;
        }
        else
        {
            return CMD_ERR_ARG;
        }
    }

    /* STATUS */
    if (strcmp(cmd, "STATUS") == 0)
    {
        GPIO_PinState state = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin);

        if (state == GPIO_PIN_SET)
            uartSendString((uint8_t*)"LED is ON");
        else
            uartSendString((uint8_t*)"LED is OFF");

        return CMD_OK;
    }

    return CMD_ERR_UNKNOWN;
}


/**
 * @brief Imprime el resultado del comando
 *
 * @param status Estado devuelto por cmdExecute
 */
static void cmdPrintStatus(cmd_status_t status)
{
    switch (status)
    {
        case CMD_OK:
            uartSendString((uint8_t*)"OK");
            break;

        case CMD_ERR_OVERFLOW:
            uartSendString((uint8_t*)"ERROR: Overflow");
            break;

        case CMD_ERR_SYNTAX:
            uartSendString((uint8_t*)"ERROR: Syntax");
            break;

        case CMD_ERR_UNKNOWN:
            uartSendString((uint8_t*)"ERROR: Unknown command");
            break;

        case CMD_ERR_ARG:
            uartSendString((uint8_t*)"ERROR: Invalid argument");
            break;
    }
}

/**
 * @brief Procesa una línea completa recibida por UART
 *
 * @param line String con el comando recibido
 */
static void cmdProcessLine(char *line)
{
    /* Ignorar comentarios */
    if (line[0] == '#' ||
       (line[0] == '/' && line[1] == '/'))
    {
        return;
    }

    /* Convertir a mayúsculas */
    toUpperCase(line);

    /* Tokenizar */
    char *cmd  = strtok(line, " ");
    char *arg1 = strtok(NULL, " ");
    char *arg2 = strtok(NULL, " ");

    /* Ejecutar comando */
    cmd_status_t status = cmdExecute(cmd, arg1, arg2);

    /* Mostrar resultado */
    cmdPrintStatus(status);
}


/* ================= FUNCIONES PUBLICAS ================= */

/**
 * @brief Inicializa el módulo parser de comandos.
 *
 * Esta función prepara el parser para comenzar a recibir comandos:
 * - Reinicia el índice del buffer.
 * - Limpia completamente el buffer de recepción.
 *
 * Debe llamarse una única vez al inicio del programa.
 */
void cmdParserInit(void)
{
    bufferIndex = 0;
    memset(cmdBuffer, 0, sizeof(cmdBuffer));
}


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
void cmdPrintHelp(void)
{
    uartSendString((uint8_t*)"Available commands:");
    uartSendString((uint8_t*)"HELP");
    uartSendString((uint8_t*)"LED ON");
    uartSendString((uint8_t*)"LED OFF");
    uartSendString((uint8_t*)"LED TOGGLE");
    uartSendString((uint8_t*)"STATUS");
}

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
void cmdPoll(void)
{
    if (uartReceiveByte(&rxChar))
    {
        if (rxChar == '\r' || rxChar == '\n')
        {
            if (bufferIndex > 0)
            {
                cmdBuffer[bufferIndex] = '\0';

                cmdProcessLine(cmdBuffer);

                bufferIndex = 0;
                memset(cmdBuffer, 0, sizeof(cmdBuffer));
            }
        }
        else
        {
            if (bufferIndex < CMD_MAX_LINE - 1)
            {
                cmdBuffer[bufferIndex++] = rxChar;
            }
            else
            {
                uartSendString((uint8_t*)"ERROR: Overflow\r\n");

                bufferIndex = 0;
                memset(cmdBuffer, 0, sizeof(cmdBuffer));
            }
        }
    }
}
