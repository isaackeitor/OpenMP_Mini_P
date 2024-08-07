# OpenMP_Mini_P
# Simulación de Ecosistema

Este proyecto simula un ecosistema simple con plantas, herbívoros y carnívoros. La simulación se ejecuta en una cuadrícula de tamaño fijo y se puede ejecutar tanto de manera secuencial como paralelizada utilizando OpenMP.

## Requisitos

- Compilador compatible con C y OpenMP (por ejemplo, `gcc`).
- Sistema operativo compatible con la ejecución de programas en C.

## Estructura del Proyecto

- `secuencial.c`: Código para la simulación secuencial del ecosistema.
- `paralelo.c`: Código para la simulación paralela del ecosistema utilizando OpenMP.
- `results_secuencial.txt`: Archivo de salida que muestra el estado del ecosistema en varios puntos del tiempo para la simulación secuencial.
- `results_parallel.txt`: Archivo de salida que muestra el estado del ecosistema en varios puntos del tiempo para la simulación paralela.

## Compilación y Ejecución

### Simulación Secuencial

Para compilar y ejecutar la simulación secuencial:

```bash
gcc ecosystem_sequential.c -o ecosystem_sequential
./ecosystem_sequential

