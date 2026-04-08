#  Práctica 5 – UART y Parser de Comandos con MEF

## Descripción general

En esta práctica se implementan dos módulos principales para un sistema embebido basado en STM32:

* **API_uart**: capa para la comunicación UART utilizando la HAL.
* **API_cmdparser**: parser de comandos basado en una Máquina de Estados Finita (MEF).

El sistema funciona en **modo polling (sin interrupciones ni DMA)** y permite interactuar con la placa mediante una terminal serie (por ejemplo, PuTTY).

---

## Módulo `API_uart`

### Descripción

Este módulo encapsula el uso de la UART (USART2), proporcionando funciones simples para inicializar la comunicación, enviar datos y recibir información.

---

### Funciones implementadas

#### `bool uartInit(void)`

Inicializa la UART con la siguiente configuración:

* Baudrate: 115200
* 8 bits de datos
* Sin paridad
* 1 bit de stop

Además, imprime por la terminal los parámetros de configuración.

**Retorna:**

* `true` → inicialización exitosa
* `false` → error en la inicialización

---

#### `void uartSendString(uint8_t *pstring)`

Envía un string completo (hasta `'\0'`) por UART.

* Verifica que el puntero no sea `NULL`
* Calcula automáticamente el tamaño del string

---

#### `void uartSendStringSize(uint8_t *pstring, uint16_t size)`

Envía una cantidad específica de bytes por UART.

* Valida puntero ≠ NULL
* Valida tamaño entre 1 y 256

---

#### `bool uartReceiveByte(uint8_t *c)`

Recibe un byte por UART en modo polling.

* No bloqueante (timeout corto)
* Ideal para lectura progresiva de comandos

---

## Módulo `API_cmdparser`

### Descripción

Este módulo implementa un parser de comandos utilizando una Máquina de Estados Finita (MEF). Permite recibir texto por UART, interpretarlo y ejecutar acciones sobre el sistema.

---

## Funcionamiento del parser

El parser realiza las siguientes tareas:

1. Lee caracteres desde la UART (uno por vez)
2. Construye un string (línea de comando)
3. Detecta el fin de línea (`\r` o `\n`)
4. Procesa el comando recibido
5. Ejecuta la acción correspondiente

---

## Funciones principales

### `void cmdParserInit(void)`

Inicializa el parser:

* Limpia el buffer de recepción
* Reinicia el índice de escritura

---

### `void cmdPoll(void)`

Función principal del parser (debe llamarse en el `while(1)`).

Responsabilidades:

* Leer caracteres desde UART
* Construir comandos
* Detectar ENTER
* Ejecutar comandos
* Manejar errores de overflow

Implementación no bloqueante

---

### `void cmdPrintHelp(void)`

Imprime por UART la lista de comandos disponibles.

---

##  Comandos implementados

Los comandos no distinguen entre mayúsculas y minúsculas:

| Comando      | Acción                       |
| ------------ | ---------------------------- |
| `HELP`       | Muestra la lista de comandos |
| `LED ON`     | Enciende el LED              |
| `LED OFF`    | Apaga el LED                 |
| `LED TOGGLE` | Invierte el estado del LED   |
| `STATUS`     | Muestra el estado del LED    |

---

## Ejercicio 1 – Capa UART

Se implementa una capa de abstracción sobre la UART que:

* Encapsula funciones de la HAL
* Valida parámetros
* Permite enviar y recibir datos de forma segura

Se verifica su funcionamiento mediante una terminal serie.

---

## Ejercicio 2 – Parser con MEF

Se implementa un parser de comandos progresivo:

### 2.1 – Eco básico

Se recibe un carácter y se reenvía inmediatamente.

### 2.2 – Recepción de línea

Se acumulan caracteres hasta detectar ENTER.

### 2.3 – Parser completo

Se interpretan comandos y se ejecutan acciones sobre el sistema.

---

## Uso en `main.c`

```c
uartInit();
cmdParserInit();

while (1)
{
    cmdPoll();
}
```

---

## Pruebas

Para probar el sistema se utiliza una terminal serie (ej: PuTTY):

Configuración:

* Baudrate: 115200
* 8 bits de datos
* Sin paridad
* 1 bit de stop

Ejemplo de uso:

```
HELP
LED ON
STATUS
LED TOGGLE
```

---

## Consideraciones

* El sistema es completamente **no bloqueante**
* No utiliza interrupciones ni DMA
* El buffer tiene tamaño limitado (`CMD_MAX_LINE`)
* Se manejan errores como:

  * Overflow de buffer
  * Comandos desconocidos
  * Errores de sintaxis

---


