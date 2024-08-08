#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define GRID_SIZE 30
#define INITIAL_PLANTS 150
#define INITIAL_HERBIVORES 40
#define INITIAL_CARNIVORES 15

typedef struct {
    int plants;
    int herbivores;
    int carnivores;
} Cell;

Cell grid[GRID_SIZE][GRID_SIZE];

void initialize_ecosystem() {
    // Inicializa la cuadrícula
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].plants = 0;
            grid[i][j].herbivores = 0;
            grid[i][j].carnivores = 0;
        }
    }

    // Añadir plantas, herbívoros y carnívoros aleatoriamente
    #pragma omp parallel
    {
        unsigned int thread_seed = (unsigned int)time(NULL) + omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < INITIAL_PLANTS; i++) {
            int x = rand_r(&thread_seed) % GRID_SIZE;
            int y = rand_r(&thread_seed) % GRID_SIZE;
            #pragma omp atomic
            grid[x][y].plants++;
        }

        #pragma omp for
        for (int i = 0; i < INITIAL_HERBIVORES; i++) {
            int x = rand_r(&thread_seed) % GRID_SIZE;
            int y = rand_r(&thread_seed) % GRID_SIZE;
            #pragma omp atomic
            grid[x][y].herbivores++;
        }

        #pragma omp for
        for (int i = 0; i < INITIAL_CARNIVORES; i++) {
            int x = rand_r(&thread_seed) % GRID_SIZE;
            int y = rand_r(&thread_seed) % GRID_SIZE;
            #pragma omp atomic
            grid[x][y].carnivores++;
        }
    }
}

void simulate_tick() {
    #pragma omp parallel
    {
        unsigned int thread_seed = (unsigned int)time(NULL) + omp_get_thread_num();
        #pragma omp for collapse(2)
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                // Actualizar plantas
                if (grid[i][j].plants > 0) {
                    if ((rand_r(&thread_seed) % 100) < 30) {
                        int new_i = (i + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                        int new_j = (j + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                        #pragma omp atomic
                        grid[new_i][new_j].plants++;
                    }
                }

                // Actualizar herbívoros
                if (grid[i][j].herbivores > 0) {
                    if (grid[i][j].plants > 0) {
                        #pragma omp atomic
                        grid[i][j].plants--;
                    } else {
                        if ((rand_r(&thread_seed) % 100) < 70) {
                            int new_i = (i + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                            int new_j = (j + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                            #pragma omp atomic
                            grid[new_i][new_j].herbivores++;
                            #pragma omp atomic
                            grid[i][j].herbivores--;
                        }
                    }
                }

                // Actualizar carnívoros
                if (grid[i][j].carnivores > 0) {
                    if (grid[i][j].herbivores > 0) {
                        #pragma omp atomic
                        grid[i][j].herbivores--;
                    } else {
                        if ((rand_r(&thread_seed) % 100) < 70) {
                            int new_i = (i + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                            int new_j = (j + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                            #pragma omp atomic
                            grid[new_i][new_j].carnivores++;
                            #pragma omp atomic
                            grid[i][j].carnivores--;
                        }
                    }
                }
            }
        }
    }
}

void print_grid(FILE *file) {
    fprintf(file, "Distribución:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // Imprimir las plantas (P), herbívoros (H) y carnívoros (C)
            for (int k = 0; k < grid[i][j].plants; k++) {
                fprintf(file, "P ");
            }
            for (int k = 0; k < grid[i][j].herbivores; k++) {
                fprintf(file, "H ");
            }
            for (int k = 0; k < grid[i][j].carnivores; k++) {
                fprintf(file, "C ");
            }
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
}

void run_simulation(int ticks, FILE *file) {
    for (int t = 0; t < ticks; t++) {
        // Simular el tick
        simulate_tick();

        // Contar el número total de plantas, herbívoros y carnívoros
        int total_plants = 0;
        int total_herbivores = 0;
        int total_carnivores = 0;

        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                total_plants += grid[i][j].plants;
                total_herbivores += grid[i][j].herbivores;
                total_carnivores += grid[i][j].carnivores;
            }
        }

        // Imprimir los totales y la distribución
        fprintf(file, "Tick %d\n", t);
        fprintf(file, "Total Plants: %d\n", total_plants);
        fprintf(file, "Total Herbivores: %d\n", total_herbivores);
        fprintf(file, "Total Carnivores: %d\n", total_carnivores);
        print_grid(file);
        fprintf(file, "\n");
    }
}

int main() {
    srand(time(NULL));
    initialize_ecosystem();

    int ticks = 1000;
    double start, end;

    FILE *file = fopen("resultados_paralelo.txt", "w");
    if (file == NULL) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return 1;
    }

    start = omp_get_wtime();
    run_simulation(ticks, file);
    end = omp_get_wtime();

    printf("Tiempo de ejecución: %f segundos\n", end - start);

    fclose(file);

    return 0;
}
