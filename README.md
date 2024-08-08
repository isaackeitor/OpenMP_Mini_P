Aquí tienes el README actualizado con más detalles sobre la compilación y ejecución de las versiones secuencial y paralela del proyecto:

---

# OpenMP_Mini_P
# Simulación de Ecosistema

Este proyecto simula un ecosistema simple con plantas, herbívoros y carnívoros. La simulación se ejecuta en una cuadrícula de tamaño fijo y se puede ejecutar tanto de manera secuencial como paralelizada utilizando OpenMP. Se utilizan estrategias como omp parallel for,collapse,  omp atomic y la utilización de threads para asegurar que a las operaciones aleatorias sean diferentes en cada hilo.

## Requisitos

- Compilador compatible con C y OpenMP (por ejemplo, `gcc`).
- Sistema operativo compatible con la ejecución de programas en C.

## Estructura del Proyecto

- `secuencial.c`: Código para la simulación secuencial del ecosistema.
- `paraleloOptimizado.c`: Código para la simulación paralela del ecosistema utilizando OpenMP.
- `resultados_secuencial.txt`: Archivo de salida que muestra el estado del ecosistema en varios puntos del tiempo para la simulación secuencial.
- `resultados_paralelo_optimizado.txt`: Archivo de salida que muestra el estado del ecosistema en varios puntos del tiempo para la simulación paralela.

## Compilación y Ejecución

### Simulación Secuencial

Para compilar y ejecutar la simulación secuencial:

```bash
gcc secuencial.c -o secuencial
./secuencial
```

### Simulación Paralela

Para compilar y ejecutar la simulación paralela utilizando OpenMP:

```bash
gcc -fopenmp paraleloOptimizado.c -o paraleloOptimizado
./paralelo
```

## Descripción de la Simulación

La simulación representa un ecosistema en una cuadrícula de tamaño 30x30. Las entidades en el ecosistema incluyen:

- **Plantas**: Pueden reproducirse en celdas adyacentes con una probabilidad del 10% en cada tick. Tienen una probabilidad del 5% de morir.
- **Herbívoros**: Se alimentan de plantas y pueden moverse a celdas adyacentes si no hay plantas en su celda actual. Tienen una probabilidad del 70% de moverse.
- **Carnívoros**: Se alimentan de herbívoros y también pueden moverse a celdas adyacentes si no hay herbívoros en su celda actual. Tienen una probabilidad del 70% de moverse.

La simulación se ejecuta durante un número fijo de ticks, y en cada tick se actualizan las poblaciones de plantas, herbívoros y carnívoros según las reglas mencionadas.

### Variabilidad en la Reproducción

Para añadir realismo, se introduce variabilidad en la reproducción de herbívoros y carnívoros, permitiendo que se reproduzcan o se muevan a celdas adyacentes con probabilidades específicas.

## Resultados

Los resultados de la simulación se almacenan en archivos de texto (`results_secuencial.txt` y `results_parallel.txt`) que muestran el estado del ecosistema en varios puntos del tiempo. Esto incluye el número total de plantas, herbívoros y carnívoros en cada tick, así como una representación gráfica de la cuadrícula.

## Ejemplo de Salida

Un ejemplo de salida para un tick puede verse de la siguiente manera:

```
Tick 10
Total Plants: 120
Total Herbivores: 35
Total Carnivores: 10
Distribución:
P P H C P ...
P H P P P ...
...
```
