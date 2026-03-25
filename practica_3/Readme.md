# Práctica 3 - Modularización y Retardos No Bloqueantes

## Objetivos

* Implementar retardos no bloqueantes utilizando una base de tiempo (HAL_GetTick)
* Diseñar una API modular reutilizable
* Separar el código en archivos `.h` y `.c`
* Controlar parámetros de entrada en funciones

---

## Funcionalidades implementadas

Se desarrolló el módulo `API_delay` que incluye:

* `delayInit()` → Inicializa el retardo
* `delayRead()` → Verifica si el tiempo se cumplió
* `delayWrite()` → Modifica la duración del retardo
* `delayIsRunning()` → Indica si el retardo está en ejecución

---

## Aplicación

Se implementa un programa que hace parpadear el LED de la placa STM32 NUCLEO-F446RE utilizando retardos no bloqueantes.

---

## Estructura del proyecto

```text
Core/
Drivers/
 └── API/
      ├── Inc/
      │    └── API_delay.h
      └── Src/
           └── API_delay.c
```

---

## Tecnologías utilizadas

* Lenguaje C
* STM32CubeIDE
* STM32 HAL
* Microcontrolador STM32F446RE

---

