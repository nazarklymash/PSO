#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "pso.h"
#include "logger.h"
#include "utils.h"

Swarm* swarm_create(int num_particles, Map *map) {
    Swarm *swarm = (Swarm*)malloc(sizeof(Swarm));
    if (!swarm) {
        fprintf(stderr, "Blad alokacji pamięci dla roju\n");
        return NULL;
    }

    swarm->num_particles = num_particles;
    swarm->particles = (Particle*)malloc(num_particles * sizeof(Particle));

    if (!swarm->particles) {
        fprintf(stderr, "Blad alokacji pamięci dla cząstek\n");
        free(swarm);
        return NULL;
    }

    swarm->global_best_x = 0;
    swarm->global_best_y = 0;
    swarm->global_best_value = -DBL_MAX;

    // Inicjalizacja cząstek w losowych pozycjach na mapie
    for (int i = 0; i < num_particles; i++) {
        Particle *p = &swarm->particles[i];

        // Losowa pozycja na mapie
        p->x = utils_random_range(0, map->width);
        p->y = utils_random_range(0, map->height);

        // Losowa początkowa prędkość
        p->vx = utils_random_range(-1, 1);
        p->vy = utils_random_range(-1, 1);

        // best_x / best_y to początkowa pozycja
        p->best_x = p->x;
        p->best_y = p->y;
        p->best_v = map_get_value(map, p->x, p->y);

        // Aktualizacja gBest jeśli to najlepsza pozycja
        if (p->best_v > swarm->global_best_value) {
            swarm->global_best_value = p->best_v;
            swarm->global_best_x = p->best_x;
            swarm->global_best_y = p->best_y;
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

void pso_iterate(Swarm *swarm, Map *map, PSOParams *params) {
    for (int i = 0; i < swarm->num_particles; i++) {
        Particle *p = &swarm->particles[i];

        // Losowe współczynniki r1 i r2
        double r1 = utils_random_01();
        double r2 = utils_random_01();

        // Aktualizacja prędkości godnie ze wzorem PSO
        // v(t+1) = w * v(t) + c1 * r1 * (pBest - x) + c2 * r2 * (gBest - x)
        p->vx = params->w * p->vx + params->c1 * r1 * (p->best_x - p->x) + params->c2 * r2 * (swarm->global_best_x - p->x);

        p->vy = params->w * p->vy + params->c1 * r1 * (p->best_y - p->y) + params->c2 * r2 * (swarm->global_best_y - p->y);

        // Ograniczenie prędkości
        double max_velocity = 2.0;
        if (p->vx > max_velocity) 
            p->vx = max_velocity;
        if (p->vx < -max_velocity) 
            p->vx = -max_velocity;
        if (p->vy > max_velocity) 
            p->vy = max_velocity;
        if (p->vy < -max_velocity) 
            p->vy = -max_velocity;

        // Aktualizacja pozycji
        // x(t+1) = x(t) + v(t+1)
        p->x += p->vx;
        p->y += p->vy;

        // Obliczenie wartości funkcji celu
        double fitness = map_get_value(map, p->x, p->y);

        // Aktualizacja best_x best_y i best_v
        if (fitness > p->best_v) {
            p->best_v = fitness;
            p->best_x = p->x;
            p->best_y = p->y;
        }

        // Aktualizacja global_best -ow
        if (fitness > swarm->global_best_value) {
            swarm->global_best_value = fitness;
            swarm->global_best_x = p->x;
            swarm->global_best_y = p->y;
        }
    }
}

void pso_run(Swarm *swarm, Map *map, PSOParams *params, int iterations, int log_interval, char *log_filename) {

    // Jezeli logger istnieje to inicjalizujemy go
    Logger *logger = NULL;
    if (log_interval > 0 && log_filename) {
        logger = logger_create(log_filename);
        if (logger) {
            logger_write_header(logger, swarm->num_particles);
        }
    }

    printf("Parametry algorytmu: w=%lf, c1=%lf, c2=%lf\n", params->w, params->c1, params->c2);
    printf("Liczba cząstek: %d, Liczba iteracji: %d\n", swarm->num_particles, iterations);

    for (int i = 0; i < iterations; i++) {
        pso_iterate(swarm, map, params);

        // Logowanie kazdą iterację
        if (logger && log_interval > 0 && (i % log_interval == 0 || i == iterations - 1)) {
            logger_write_iteration(logger, i, swarm);
        }

        // Wypisz postęp co 0.1 od ilosci iteracji
        if ((i + 1) % (iterations / 10 + 1) == 0 || i == iterations - 1) {
            printf("Iteracja %d/%d: gBest = (%lf, %lf), wartość = %lf\n", i + 1, iterations, swarm->global_best_x, swarm->global_best_y, swarm->global_best_value);
        }
    }

    if (logger) {
        logger_close(logger);
        printf("\nDane zapisano do pliku %s\n", log_filename);
    }

    printf("\n\n");

    printf("Znaleziono źródło sygnału na pozycji: (%lf, %lf)\n", swarm->global_best_x, swarm->global_best_y);
    printf("Siła sygnału: %lf\n", swarm->global_best_value);
}

int pso_load_config( char *filename, PSOParams *params) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie można otworzyć pliku konfiguracyjnego");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignorujemy komentarze i puste linie
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        char key[64];
        double value;

        // Pobieramy maks 63 znaki dopoki nie napotkamy = i przypisujemy do key, a reszte do value
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