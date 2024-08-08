#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 10
#define INITIAL_PLANTS 150
#define INITIAL_HERBIVORES 40
#define INITIAL_CARNIVORES 15

typedef struct {
    int plants;
    int herbivores;
    int carnivores;
} Cell;

typedef struct {
    int x;
    int y;
} ActiveCell;

Cell grid[GRID_SIZE][GRID_SIZE];
ActiveCell active_cells[GRID_SIZE * GRID_SIZE];
int num_active_cells = 0;

// Función para inicializar el ecosistema
void initialize_ecosystem() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].plants = 0;
            grid[i][j].herbivores = 0;
            grid[i][j].carnivores = 0;
        }
    }

    // Añadir plantas, herbívoros y carnívoros aleatoriamente
    for (int i = 0; i < INITIAL_PLANTS; i++) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        grid[x][y].plants++;
    }

    for (int i = 0; i < INITIAL_HERBIVORES; i++) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        grid[x][y].herbivores++;
    }

    for (int i = 0; i < INITIAL_CARNIVORES; i++) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        grid[x][y].carnivores++;
    }

    // Inicializar lista de celdas activas
    num_active_cells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].plants > 0 || grid[i][j].herbivores > 0 || grid[i][j].carnivores > 0) {
                active_cells[num_active_cells++] = (ActiveCell){i, j};
            }
        }
    }
}

// Función para simular un tick
void simulate_tick() {
    Cell new_grid[GRID_SIZE][GRID_SIZE];
    
    // Copia el estado actual al nuevo estado
    for (int i = 0; i < num_active_cells; i++) {
        int x = active_cells[i].x;
        int y = active_cells[i].y;
        new_grid[x][y] = grid[x][y];
    }

    for (int i = 0; i < num_active_cells; i++) {
        int x = active_cells[i].x;
        int y = active_cells[i].y;

        // Actualizar plantas
        if (grid[x][y].plants > 0) {
            if ((rand() % 100) < 30) {
                int new_x = (x + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                int new_y = (y + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                new_grid[new_x][new_y].plants++;
            }
        }
        
        // Actualizar herbívoros
        if (grid[x][y].herbivores > 0) {
            if (grid[x][y].plants > 0) {
                new_grid[x][y].plants--;
            } else {
                int new_x = (x + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                int new_y = (y + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                new_grid[new_x][new_y].herbivores++;
                new_grid[x][y].herbivores--;
            }
        }
        
        // Actualizar carnívoros
        if (grid[x][y].carnivores > 0) {
            if (grid[x][y].herbivores > 0) {
                new_grid[x][y].herbivores--;
            } else {
                int new_x = (x + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                int new_y = (y + rand() % 3 - 1 + GRID_SIZE) % GRID_SIZE;
                new_grid[new_x][new_y].carnivores++;
                new_grid[x][y].carnivores--;
            }
        }
    }

    // Copiar el nuevo estado al estado actual
    for (int i = 0; i < num_active_cells; i++) {
        int x = active_cells[i].x;
        int y = active_cells[i].y;
        grid[x][y] = new_grid[x][y];
    }

    // Actualizar lista de celdas activas
    num_active_cells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].plants > 0 || grid[i][j].herbivores > 0 || grid[i][j].carnivores > 0) {
                active_cells[num_active_cells++] = (ActiveCell){i, j};
            }
        }
    }
}

// Función para imprimir la cuadrícula
void print_grid(FILE *file) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fprintf(file, "P:%d H:%d C:%d | ", grid[i][j].plants, grid[i][j].herbivores, grid[i][j].carnivores);
        }
        fprintf(file, "\n");
    }
}

// Función para ejecutar la simulación
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
    clock_t start, end;
    double cpu_time_used;

    FILE *file = fopen("results_secuencial.txt", "w");
    if (file == NULL) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return 1;
    }

    start = clock();
    run_simulation(ticks, file);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución: %f segundos\n", cpu_time_used);

    fclose(file);

    return 0;
}
