# Práctica 2: Retardos no bloqueantes y control de LED

## Objetivo

Implementar un módulo de software para el manejo de retardos no bloqueantes y utilizarlo para controlar el parpadeo de un LED en una placa de desarrollo.

---

## 📋 Consigna

La práctica se divide en tres partes:

### Punto 1

Implementar un módulo de retardos no bloqueantes utilizando una estructura `delay_t` y las siguientes funciones:

* `delayInit`
* `delayRead`
* `delayWrite`

El módulo debe utilizar `HAL_GetTick()` como base de tiempo.

---

### Punto 2

Utilizar el módulo desarrollado para hacer parpadear un LED con el siguiente comportamiento:

* 100 ms encendido
* 100 ms apagado

---

### Punto 3 

Implementar un patrón de parpadeo:

* 5 ciclos de 1 segundo
* 5 ciclos de 200 ms
* 5 ciclos de 100 ms

Utilizando un arreglo para definir los períodos.

---

## Implementación

Se implementó un delay no bloqueante basado en la comparación de marcas de tiempo con `HAL_GetTick()`.

El funcionamiento del sistema se basa en:

* Inicializar el retardo sin comenzar la cuenta (`delayInit`)
* Consultar periódicamente si el tiempo se cumplió (`delayRead`)
* Permitir modificar la duración del retardo (`delayWrite`)

---

## Lógica de funcionamiento

El programa principal utiliza un enfoque no bloqueante dentro del `while(1)`:

1. Se evalúa el estado del delay
2. Se alterna el estado del LED
3. Se cuentan los ciclos de parpadeo
4. Se cambia el período según el patrón definido

El patrón se implementa mediante vectores:

```c
uint32_t periodos[] = {1000, 200, 100};
uint8_t repeticiones[] = {5, 5, 5};
```

Para poder modificar fácilmente los tiempos sin alterar la lógica del programa.

---

##  Hardware utilizado

* Placa: NUCLEO-F446RE
* LED: LD2 (GPIOA PIN 5)






