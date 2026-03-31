# Practica 4 - Retardos No Bloqueantes y Anti-Rebote

## Descripción

Este proyecto fue desarrollado sobre la placa **NUCLEO-F446RE** utilizando **STM32CubeIDE**.

El objetivo es implementar:
- Retardos no bloqueantes
- Una máquina de estados finitos (FSM) para anti-rebote de un pulsador
- Modularización del código en APIs reutilizables
- Control de un LED en función de eventos del usuario

---

## Estructura del proyecto
```text
Core/
Drivers/
 └── API/
      ├── Inc/
      │    └── API_delay.h
      │    └── API_debounce.h
      └── Src/
           └── API_delay.c
           └── API_debounce.c
```

---

## Funcionalidades implementadas

### Módulo API_delay

Permite trabajar con retardos no bloqueantes usando la función `HAL_GetTick()`.

Estructura utilizada:

```c
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;
```
##  Módulo API_debounce

Este módulo implementa una **máquina de estados finitos (FSM)** para eliminar el efecto de rebote mecánico del pulsador de la placa.

---

### Estados de la FSM

La máquina de estados está compuesta por los siguientes estados:

- **BUTTON_UP**  
  El botón se encuentra liberado (no presionado). Es el estado inicial del sistema.

- **BUTTON_FALLING**  
  Estado transitorio que indica que se detectó un posible flanco descendente (inicio de presión).  
  Se espera un tiempo de anti-rebote para confirmar la pulsación.

- **BUTTON_DOWN**  
  El botón está presionado de forma estable (rebote ya filtrado).

- **BUTTON_RISING**  
  Estado transitorio que indica que se detectó un posible flanco ascendente (liberación del botón).  
  Se espera nuevamente el tiempo de anti-rebote para confirmar la liberación.

---

###  Funciones implementadas

- `debounceFSM_init()`  
  Inicializa la FSM en el estado `BUTTON_UP` y configura el retardo de anti-rebote.

- `debounceFSM_update()`  
  Actualiza la máquina de estados leyendo el estado del botón.  
  Esta función debe llamarse periódicamente dentro del loop principal.

- `readKey()`  
  Devuelve `true` cuando se detecta una pulsación válida (flanco descendente).  
  Luego de devolver `true`, resetea internamente el evento.

---

## Ejercicio 1

En este ejercicio se implementa la FSM de anti-rebote directamente en `main.c`.

- El LED se controla de forma directa:
  - Se enciende al presionar el botón
  - Se apaga al soltarlo
- No hay modularización (toda la lógica está en el `main`)
- Sirve como primera aproximación al uso de máquinas de estado

---

##  Ejercicio 2

En este ejercicio se modulariza la solución creando el módulo `API_debounce`.

Se integra con el módulo `API_delay` para lograr un sistema más reutilizable y organizado.

###  Funcionamiento

El sistema controla el parpadeo del LED de la siguiente manera:

- Estado inicial:
  - Parpadeo lento (500 ms)

- Cada vez que se presiona el botón:
  - Se alterna entre:
    - 500 ms (lento)
    - 100 ms (rápido)

La detección de la pulsación se realiza mediante la función `readKey()` del módulo debounce.

---

### Anti-rebote

El anti-rebote se implementa utilizando un retardo no bloqueante de:

- **40 ms**

Esto permite:
- Evitar falsas detecciones
- No bloquear la ejecución del programa
- Mantener el sistema reactivo

---

## Tecnologías utilizadas

* Lenguaje C
* STM32CubeIDE
* STM32 HAL
* Microcontrolador STM32F446RE

---
