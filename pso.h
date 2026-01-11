#ifndef PSO_H
#define PSO_H

#include "map.h"

// Struktura opisująca pojedynczą cząstkę (drona)
typedef struct {
    double x;           // bieżąca pozycja X
    double y;           // bieżąca pozycja Y
    double vx;          // prędkość w kierunku X
    double vy;          // prędkość w kierunku Y
    double pBest_x;     // najlepsza znana pozycja X tej cząstki
    double pBest_y;     // najlepsza znana pozycja Y tej cząstki
    double pBest_value; // wartość funkcji celu w pBest
} Particle;

// Struktura opisująca rój cząstek
typedef struct Swarm {
    Particle *particles;  // tablica cząstek
    int num_particles;    // liczba cząstek w roju
    double gBest_x;       // globalne najlepsze X
    double gBest_y;       // globalne najlepsze Y
    double gBest_value;   // wartość funkcji celu w gBest
} Swarm;

// Parametry algorytmu PSO
typedef struct {
    double w;   // waga bezwładności
    double c1;  // współczynnik poznawczy (pamięć własna)
    double c2;  // współczynnik społeczny (wiedza roju)
} PSOParams;

// Domyślne wartości parametrów PSO
#define DEFAULT_W  0.5
#define DEFAULT_C1 1.0
#define DEFAULT_C2 1.0
#define DEFAULT_PARTICLES 30
#define DEFAULT_ITERATIONS 100

// Tworzy i inicjalizuje rój cząstek
Swarm* swarm_create(int num_particles, const Map *map);

// Zwalnia pamięć roju
void swarm_free(Swarm *swarm);

// Wykonuje jedną iterację algorytmu PSO
void pso_iterate(Swarm *swarm, const Map *map, const PSOParams *params);

// Uruchamia pełną symulację PSO
void pso_run(Swarm *swarm, const Map *map, const PSOParams *params,
             int iterations, int log_interval, const char *log_filename);

// Wczytuje parametry PSO z pliku konfiguracyjnego
// Zwraca 1 w przypadku sukcesu, 0 w przypadku błędu
int pso_load_config(const char *filename, PSOParams *params);

#endif // PSO_H

