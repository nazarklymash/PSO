#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "map.h"
#include "pso.h"

// Funkcja wyświetlająca pomoc
void print_usage(const char *program_name) {
    printf("Symulator Misji Ratunkowej - Algorytm PSO\n");
    printf("==========================================\n\n");
    printf("Użycie: %s <plik_mapy> [opcje]\n\n", program_name);
    printf("Opcje:\n");
    printf("  -p <liczba>    Liczba cząstek (dronów) w roju (domyślnie: %d)\n", DEFAULT_PARTICLES);
    printf("  -i <liczba>    Liczba iteracji symulacji (domyślnie: %d)\n", DEFAULT_ITERATIONS);
    printf("  -c <plik>      Plik konfiguracyjny z parametrami PSO\n");
    printf("  -n <liczba>    Co którą iterację zapisywać postępy (0 = brak zapisu, domyślnie: 0)\n");
    printf("  -o <plik>      Nazwa pliku wyjściowego CSV (domyślnie: output.csv)\n");
    printf("  -h             Wyświetl tę pomoc\n\n");
    printf("Przykład:\n");
    printf("  %s terrain.txt -p 50 -i 200 -c config.txt -n 2\n", program_name);
}

int main(int argc, char *argv[]) {
    // Domyślne wartości
    const char *map_file = NULL;
    const char *config_file = NULL;
    const char *output_file = "output.csv";
    int num_particles = DEFAULT_PARTICLES;
    int num_iterations = DEFAULT_ITERATIONS;
    int log_interval = 0;

    // Parsowanie argumentów
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Pierwszy argument to plik mapy (jeśli nie zaczyna się od -)
    int arg_start = 1;
    if (argv[1][0] != '-') {
        map_file = argv[1];
        arg_start = 2;
    }

    // Parsowanie opcji
    for (int i = arg_start; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            num_particles = atoi(argv[++i]);
            if (num_particles <= 0) {
                fprintf(stderr, "Błąd: Liczba cząstek musi być dodatnia\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            num_iterations = atoi(argv[++i]);
            if (num_iterations <= 0) {
                fprintf(stderr, "Błąd: Liczba iteracji musi być dodatnia\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config_file = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            log_interval = atoi(argv[++i]);
            if (log_interval < 0) {
                fprintf(stderr, "Błąd: Interwał logowania nie może być ujemny\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (argv[i][0] != '-' && !map_file) {
            map_file = argv[i];
        } else {
            fprintf(stderr, "Nieznana opcja: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    // Sprawdzenie czy podano plik mapy
    if (!map_file) {
        fprintf(stderr, "Błąd: Nie podano pliku mapy\n");
        print_usage(argv[0]);
        return 1;
    }

    // Inicjalizacja generatora liczb losowych
    srand((unsigned int)time(NULL));

    // Wczytanie mapy
    printf("Wczytywanie mapy z pliku: %s\n", map_file);
    Map *map = map_load(map_file);
    if (!map) {
        return 1;
    }
    printf("Mapa wczytana pomyślnie. Wymiary: %d x %d\n\n", map->width, map->height);

    // Inicjalizacja parametrów PSO
    PSOParams params = {
        .w = DEFAULT_W,
        .c1 = DEFAULT_C1,
        .c2 = DEFAULT_C2
    };

    // Wczytanie konfiguracji jeśli podano plik
    if (config_file) {
        printf("Wczytywanie konfiguracji z pliku: %s\n", config_file);
        if (!pso_load_config(config_file, &params)) {
            fprintf(stderr, "Uwaga: Użyto domyślnych parametrów PSO\n");
        }
    }

    // Utworzenie roju
    Swarm *swarm = swarm_create(num_particles, map);
    if (!swarm) {
        map_free(map);
        return 1;
    }

    // Uruchomienie symulacji
    pso_run(swarm, map, &params, num_iterations, log_interval, output_file);

    // Zwolnienie pamięci
    swarm_free(swarm);
    map_free(map);

    return 0;
}

