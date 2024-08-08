#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h> // Incluir esta cabecera para usar strcat

#define GRID_SIZE 10
#define INITIAL_PLANTS 150
#define INITIAL_HERBIVORES 40
#define INITIAL_CARNIVORES 15

typedef struct {
    int plants;
    int herbivores;
    int carnivores;
} Cell;

Cell grid[GRID_SIZE][GRID_SIZE];
Cell new_grid[GRID_SIZE][GRID_SIZE];

void initialize_ecosystem() {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].plants = 0;
            grid[i][j].herbivores = 0;
            grid[i][j].carnivores = 0;
        }
    }

    unsigned int seed = (unsigned int)time(NULL);
    #pragma omp parallel
    {
        unsigned int thread_seed = seed + omp_get_thread_num();

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
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            new_grid[i][j] = grid[i][j];
        }
    }

    #pragma omp parallel for collapse(2) schedule(auto)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].plants > 0) {
                if ((rand() % 100) < 30) {
                    int new_i = (i + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                    int new_j = (j + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                    #pragma omp atomic
                    new_grid[new_i][new_j].plants++;
                }
            }
        }
    }

    #pragma omp parallel for collapse(2) schedule(auto)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].herbivores > 0) {
                if (grid[i][j].plants > 0) {
                    #pragma omp atomic
                    new_grid[i][j].plants--;
                } else {
                    int new_i = (i + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                    int new_j = (j + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                    #pragma omp atomic
                    new_grid[new_i][new_j].herbivores++;
                    #pragma omp atomic
                    new_grid[i][j].herbivores--;
                }
            }
        }
    }

    #pragma omp parallel for collapse(2) schedule(auto)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].carnivores > 0) {
                if (grid[i][j].herbivores > 0) {
                    #pragma omp atomic
                    new_grid[i][j].herbivores--;
                } else {
                    int new_i = (i + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                    int new_j = (j + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                    #pragma omp atomic
                    new_grid[new_i][new_j].carnivores++;
                    #pragma omp atomic
                    new_grid[i][j].carnivores--;
                }
            }
        }
    }

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }
}

void print_grid(FILE *file) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fprintf(file, "P:%d H:%d C:%d | ", grid[i][j].plants, grid[i][j].herbivores, grid[i][j].carnivores);
        }
        fprintf(file, "\n");
    }
}

void run_simulation(int ticks, FILE *file) {
    for (int t = 0; t < ticks; t++) {
        simulate_tick();
        fprintf(file, "Tick %d\n", t);
        print_grid(file);
        fprintf(file, "\n");
    }
}

int main() {
    srand(time(NULL));
    initialize_ecosystem();

    int ticks = 10;
    double start, end;
    double cpu_time_used;

    FILE *file = fopen("results_parallel_optimized.txt", "w");
    if (file == NULL) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return 1;
    }

    start = omp_get_wtime();
    run_simulation(ticks, file);
    end = omp_get_wtime();

    cpu_time_used = end - start;
    printf("Tiempo de ejecución: %f segundos\n", cpu_time_used);

    fclose(file);

    return 0;
}
