#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "pso.h"
#include "logger.h"

// Generuje losową liczbę z zakresu [0, 1]
static double random_01(void) {
    return (double)rand() / RAND_MAX;
}

// Generuje losową liczbę z zakresu [min, max]
static double random_range(double min, double max) {
    return min + random_01() * (max - min);
}

Swarm* swarm_create(int num_particles, const Map *map) {
    Swarm *swarm = (Swarm*)malloc(sizeof(Swarm));
    if (!swarm) {
        fprintf(stderr, "Błąd: Nie można zaalokować pamięci dla roju\n");
        return NULL;
    }

    swarm->num_particles = num_particles;
    swarm->particles = (Particle*)malloc(num_particles * sizeof(Particle));
    if (!swarm->particles) {
        fprintf(stderr, "Błąd: Nie można zaalokować pamięci dla cząstek\n");
        free(swarm);
        return NULL;
    }

    swarm->gBest_x = 0;
    swarm->gBest_y = 0;
    swarm->gBest_value = -DBL_MAX;

    // Inicjalizacja cząstek w losowych pozycjach na mapie
    for (int i = 0; i < num_particles; i++) {
        Particle *p = &swarm->particles[i];

        // Losowa pozycja na mapie
        p->x = random_range(0, map->width);
        p->y = random_range(0, map->height);

        // Losowa początkowa prędkość (niewielka)
        p->vx = random_range(-1, 1);
        p->vy = random_range(-1, 1);

        // pBest to początkowa pozycja
        p->pBest_x = p->x;
        p->pBest_y = p->y;
        p->pBest_value = map_get_value(map, p->x, p->y);

        // Aktualizacja gBest jeśli to najlepsza pozycja
        if (p->pBest_value > swarm->gBest_value) {
            swarm->gBest_value = p->pBest_value;
            swarm->gBest_x = p->pBest_x;
            swarm->gBest_y = p->pBest_y;
        }
    }

    return swarm;
}

void swarm_free(Swarm *swarm) {
    if (swarm) {
        if (swarm->particles) {
            free(swarm->particles);
        }
        free(swarm);
    }
}

void pso_iterate(Swarm *swarm, const Map *map, const PSOParams *params) {
    for (int i = 0; i < swarm->num_particles; i++) {
        Particle *p = &swarm->particles[i];

        // Losowe współczynniki r1 i r2
        double r1 = random_01();
        double r2 = random_01();

        // Aktualizacja prędkości według wzoru PSO
        // v(t+1) = w * v(t) + c1 * r1 * (pBest - x) + c2 * r2 * (gBest - x)
        p->vx = params->w * p->vx
              + params->c1 * r1 * (p->pBest_x - p->x)
              + params->c2 * r2 * (swarm->gBest_x - p->x);

        p->vy = params->w * p->vy
              + params->c1 * r1 * (p->pBest_y - p->y)
              + params->c2 * r2 * (swarm->gBest_y - p->y);

        // Ograniczenie prędkości (opcjonalne, ale pomocne)
        double max_velocity = 2.0;
        if (p->vx > max_velocity) p->vx = max_velocity;
        if (p->vx < -max_velocity) p->vx = -max_velocity;
        if (p->vy > max_velocity) p->vy = max_velocity;
        if (p->vy < -max_velocity) p->vy = -max_velocity;

        // Aktualizacja pozycji
        // x(t+1) = x(t) + v(t+1)
        p->x += p->vx;
        p->y += p->vy;

        // Obliczenie wartości funkcji celu
        double fitness = map_get_value(map, p->x, p->y);

        // Aktualizacja pBest
        if (fitness > p->pBest_value) {
            p->pBest_value = fitness;
            p->pBest_x = p->x;
            p->pBest_y = p->y;
        }

        // Aktualizacja gBest
        if (fitness > swarm->gBest_value) {
            swarm->gBest_value = fitness;
            swarm->gBest_x = p->x;
            swarm->gBest_y = p->y;
        }
    }
}

void pso_run(Swarm *swarm, const Map *map, const PSOParams *params,
             int iterations, int log_interval, const char *log_filename) {

    // Jeśli logowanie jest włączone, otwórz plik
    Logger *logger = NULL;
    if (log_interval > 0 && log_filename) {
        logger = logger_create(log_filename);
        if (logger) {
            logger_write_header(logger, swarm->num_particles);
        }
    }

    printf("Rozpoczynam symulację PSO...\n");
    printf("Parametry: w=%.2f, c1=%.2f, c2=%.2f\n", params->w, params->c1, params->c2);
    printf("Liczba cząstek: %d, Liczba iteracji: %d\n\n", swarm->num_particles, iterations);

    for (int iter = 0; iter < iterations; iter++) {
        pso_iterate(swarm, map, params);

        // Logowanie co n-tą iterację
        if (logger && log_interval > 0 && (iter % log_interval == 0 || iter == iterations - 1)) {
            logger_write_iteration(logger, iter, swarm);
        }

        // Wypisz postęp co 10% iteracji
        if ((iter + 1) % (iterations / 10 + 1) == 0 || iter == iterations - 1) {
            printf("Iteracja %d/%d: gBest = (%.2f, %.2f), wartość = %.2f\n",
                   iter + 1, iterations, swarm->gBest_x, swarm->gBest_y, swarm->gBest_value);
        }
    }

    if (logger) {
        logger_close(logger);
        printf("\nDane zapisano do pliku: %s\n", log_filename);
    }

    printf("\n=== WYNIK SYMULACJI ===\n");
    printf("Znaleziono źródło sygnału na pozycji: (%.2f, %.2f)\n",
           swarm->gBest_x, swarm->gBest_y);
    printf("Siła sygnału: %.2f\n", swarm->gBest_value);
}

int pso_load_config(const char *filename, PSOParams *params) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Błąd: Nie można otworzyć pliku konfiguracyjnego: %s\n", filename);
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignoruj komentarze i puste linie
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        char key[64];
        double value;
        if (sscanf(line, "%63[^=]=%lf", key, &value) == 2) {
            // Usuń białe znaki z klucza
            char *k = key;
            while (*k == ' ') k++;
            char *end = k + strlen(k) - 1;
            while (end > k && (*end == ' ' || *end == '\n' || *end == '\r')) {
                *end = '\0';
                end--;
            }

            if (strcmp(k, "w") == 0) {
                params->w = value;
            } else if (strcmp(k, "c1") == 0) {
                params->c1 = value;
            } else if (strcmp(k, "c2") == 0) {
                params->c2 = value;
            }
        }
    }

    fclose(file);
    return 1;
}

