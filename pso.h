#ifndef PSO_H
#define PSO_H

#include "map.h"

// Struktura czastki
typedef struct {
    double x;           // pozycja X
    double y;           // pozycja Y
    double vx;          // prędkość X
    double vy;          // prędkość Y
    double best_x;     // najlepsza znana pozycja X
    double best_y;     // najlepsza znana pozycja Y
    double best_v;     // wartość funkcji celu w pBest
} Particle;

// Struktura opisująca rój cząstek
typedef struct Swarm {
    Particle *particles;  // tablica cząstek
    int num_particles;    // liczba cząstek w roju
    double global_best_x;       // globalne najlepsze X
    double global_best_y;       // globalne najlepsze Y
    double global_best_value;   // wartość funkcji celu w gBest
} Swarm;

// Parametry algorytmu PSO
typedef struct {
    double w;   // waga bezwładności
    double c1;  // wspolczynnik pamieci wlasnek
    double c2;  // współczynnik wiedzy roju
} PSOParams;

// Domyślne wartości parametrów PSO
#define DEFAULT_W  0.5
#define DEFAULT_C1 1.0
#define DEFAULT_C2 1.0
#define DEFAULT_PARTICLES 30
#define DEFAULT_ITERATIONS 100

// Tworzy i inicjalizuje rój cząstek
Swarm* swarm_create(int num_particles, Map *map);

// Zwalnia pamiec
void swarm_free(Swarm *swarm);

// Wykonuje iterację algorytmu PSO
void pso_iterate(Swarm *swarm, Map *map, PSOParams *params);

// Uruchamia pełną symulację PSO
void pso_run(Swarm *swarm, Map *map, PSOParams *params, int iterations, int log_interval, char *log_filename);

// Wczytuje parametry PSO z config
// Zwraca 1 w przypadku sukcesu, 0 w innym przypadkiu
int pso_load_config(char *filename, PSOParams *params);

#endif