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

    // Añadir plantas aleatoriamente sin permitir múltiples plantas en la misma casilla
    #pragma omp parallel
    {
        unsigned int thread_seed = (unsigned int)time(NULL) + omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < INITIAL_PLANTS; i++) {
            int x, y;
            do {
                x = rand_r(&thread_seed) % GRID_SIZE;
                y = rand_r(&thread_seed) % GRID_SIZE;
            } while (grid[x][y].plants > 0);

            #pragma omp critical
            {
                if (grid[x][y].plants == 0) {
                    grid[x][y].plants = 1;
                }
            }
        }

        // Añadir herbívoros aleatoriamente sin permitir múltiples herbívoros en la misma casilla
        #pragma omp for
        for (int i = 0; i < INITIAL_HERBIVORES; i++) {
            int x, y;
            do {
                x = rand_r(&thread_seed) % GRID_SIZE;
                y = rand_r(&thread_seed) % GRID_SIZE;
            } while (grid[x][y].herbivores > 0);

            #pragma omp critical
            {
                if (grid[x][y].herbivores == 0) {
                    grid[x][y].herbivores = 1;
                }
            }
        }

        // Añadir carnívoros aleatoriamente sin permitir múltiples carnívoros en la misma casilla
        #pragma omp for
        for (int i = 0; i < INITIAL_CARNIVORES; i++) {
            int x, y;
            do {
                x = rand_r(&thread_seed) % GRID_SIZE;
                y = rand_r(&thread_seed) % GRID_SIZE;
            } while (grid[x][y].carnivores > 0);

            #pragma omp critical
            {
                if (grid[x][y].carnivores == 0) {
                    grid[x][y].carnivores = 1;
                }
            }
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
                        #pragma omp critical
                        {
                            if (grid[new_i][new_j].plants == 0) {
                                grid[new_i][new_j].plants++;
                            }
                        }
                    }

                    // Introducir una probabilidad de muerte del 5% para las plantas
                    if ((rand_r(&thread_seed) % 100) < 5) {
                        #pragma omp atomic
                        grid[i][j].plants--;
                    }
                }

                // Actualizar herbívoros
                if (grid[i][j].herbivores > 0) {
                    // Se aumenta la eficiencia de los herbívoros para consumir plantas
                    int plants_eaten = (grid[i][j].plants >= 2) ? 2 : grid[i][j].plants;
                    #pragma omp atomic
                    grid[i][j].plants -= plants_eaten;

                    if (grid[i][j].plants == 0) {
                        if ((rand_r(&thread_seed) % 100) < 30) {
                            int new_i = (i + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                            int new_j = (j + rand_r(&thread_seed) % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                            #pragma omp critical
                            {
                                if (grid[new_i][new_j].herbivores == 0) {
                                    grid[new_i][new_j].herbivores++;
                                    grid[i][j].herbivores--;
                                }
                            }
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
                            #pragma omp critical
                            {
                                if (grid[new_i][new_j].carnivores == 0) {
                                    grid[new_i][new_j].carnivores++;
                                    grid[i][j].carnivores--;
                                }
                            }
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

    int ticks = 3000;
    double start, end;

    FILE *file = fopen("resultados_paralelo_optimizado.txt", "w");
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
