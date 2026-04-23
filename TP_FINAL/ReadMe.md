# Sistema de Medición de Presión con FSM (STM32)

## Descripción general

Este proyecto implementa un sistema embebido para la **medición de presión utilizando un sensor analógico con salida digital (ADC integrado)**, controlado mediante una **máquina de estados finitos (FSM)**.

El sistema permite:

* Inicializar y configurar el hardware
* Realizar **calibración automática (offset y sensibilidad)**
* Leer presión en tiempo real
* Visualizar los datos en un LCD por I2C

Aunque el sistema actualmente se limita a la adquisición y visualización, está diseñado con una arquitectura modular que permite su extensión a sistemas de **control en lazo cerrado** (por ejemplo, control de presión mediante un motor o bomba).

---

## Arquitectura del proyecto

El código está organizado en una estructura modular dentro de la carpeta:

```
drivers/API/
```

###  Módulos principales

* `API_sensor.c / .h`
  Driver del sensor de presión.
  Se encarga de:

  * Lectura RAW desde el ADC
  * Cálculo de offset (tara)
  * Conversión a mmHg
  * Calibración de sensibilidad

* `API_lcd.c / .h`
  Driver del display LCD vía I2C.
  Permite:

  * Inicialización
  * Escritura de strings
  * Posicionamiento del cursor
  * Visualización de datos y errores

* `API_delay.c / .h`
  Implementación de retardos no bloqueantes para temporización.

* `API_fsm.c / .h`
  Implementación de la **máquina de estados finitos** que gobierna el sistema.

---

## Máquina de estados (FSM)

La FSM organiza la lógica del sistema en distintos estados:

* `ESTADO_INIT`
  Inicialización de hardware y calibración de cero (offset)

* `ESTADO_CALIBRATE_SPAN`
  Calibración de sensibilidad usando una presión conocida (ej: 80 mmHg)

* `ESTADO_IDLE`
  Espera no bloqueante entre lecturas

* `ESTADO_READ_SENSOR`
  Lectura del sensor y cálculo de presión

* `ESTADO_REPORT`
  Visualización de datos en el LCD

* `ESTADO_ERROR`
  Manejo de fallas del sistema

---

## Sensor utilizado

Se utilizó el sensor:

**MPS20N0040D**

Características relevantes:

* Sensor de presión tipo puente resistivo
* Incluye conversión a digital mediante un ADC externo HX710
* Alta sensibilidad para mediciones de presión baja (ideal para aplicaciones biomédicas)

---

## Calibración

La calibración del sensor se realiza en dos etapas:

### 1. Offset (tara)

Se obtiene promediando múltiples lecturas sin presión aplicada:

```
offset = promedio(RAW)
```

---

### 2. Sensibilidad (span)

Se utiliza una presión conocida (por ejemplo 80 mmHg) y se aplica una relación lineal:

```
P = m * RAW_neto
```

donde:

* `RAW_neto = RAW - offset`
* `m = presión_conocida / RAW_neto`

Este modelo corresponde a una aproximación lineal del tipo:

```
P = m·x + b
```

donde en este caso:

* `b = 0` (ya eliminado mediante la tara)

---

## Visualización

El sistema muestra:

* Presión en mmHg
* Valor RAW del sensor (para debug)

El LCD se maneja mediante I2C en modo 4 bits.

---

## Limitaciones actuales

* No se implementó el control de actuadores (motor/bomba)
* No hay filtrado digital avanzado (promedios, IIR, etc.)
* La calibración depende de intervención manual

---

## Trabajo futuro

El sistema está preparado para evolucionar hacia un esquema de control, por ejemplo:

* Control de presión en lazo cerrado
* Regulación mediante bomba o válvula
* Implementación de PID
* Registro de datos

---

## Aplicaciones potenciales

* Simuladores de presión invasiva
* Sistemas biomédicos educativos
* Bancos de prueba de sensores
* Control de presión en sistemas neumáticos

---

## Autor

Ing. Guirula Lucía

---

## Referencias


* https://makersportal.com/blog/2020/6/4/mps20n0040d-pressure-sensor-calibration-with-arduino#calibration=
* https://github.com/bogde/HX711/tree/master
* https://github.com/queuetue/Q2-HX711-Arduino-Library/tree/master
* https://uelectronics.com/producto/mps20n0040d-s-modulo-sensor-de-presion-barometrica/?srsltid=AfmBOoqJwAjtkc8Hip1tc9DWRX5jAsPGLkzuSfvsGkYHUrQn_bdpibtd
