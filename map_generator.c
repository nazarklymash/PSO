// Generator map terenu dla symulatora Misji Ratunkowej PSO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

void print_usage(const char *program_name) {
    printf("Generator map terenu dla PSO\n");
    printf("=============================\n\n");
    printf("Użycie: %s <plik_wyjściowy> [opcje]\n\n", program_name);
    printf("Opcje:\n");
    printf("  -w <liczba>    Szerokość mapy (domyślnie: 20)\n");
    printf("  -h <liczba>    Wysokość mapy (domyślnie: 20)\n");
    printf("  -x <liczba>    Pozycja X źródła sygnału (domyślnie: losowo)\n");
    printf("  -y <liczba>    Pozycja Y źródła sygnału (domyślnie: losowo)\n");
    printf("  -s <liczba>    Siła sygnału maksymalnego (domyślnie: 100.0)\n");
    printf("  -n <liczba>    Poziom szumu 0.0-1.0 (domyślnie: 0.2)\n");
    printf("  --help         Wyświetl tę pomoc\n\n");
    printf("Przykład:\n");
    printf("  %s terrain.txt -w 30 -h 30 -x 15 -y 20 -s 150\n", program_name);
}

int main(int argc, char *argv[]) {
    // Domyślne wartości
    const char *output_file = NULL;
    int width = 20;
    int height = 20;
    double peak_x = -1;  // -1 oznacza losowo
    double peak_y = -1;
    double peak_value = 100.0;
    double noise = 0.2;

    // Parsowanie argumentów
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Pierwszy argument to plik wyjściowy
    int arg_start = 1;
    if (argv[1][0] != '-') {
        output_file = argv[1];
        arg_start = 2;
    }

    // Parsowanie opcji
    for (int i = arg_start; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            height = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-x") == 0 && i + 1 < argc) {
            peak_x = atof(argv[++i]);
        } else if (strcmp(argv[i], "-y") == 0 && i + 1 < argc) {
            peak_y = atof(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            peak_value = atof(argv[++i]);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            noise = atof(argv[++i]);
        } else if (argv[i][0] != '-' && !output_file) {
            output_file = argv[i];
        }
    }

    if (!output_file) {
        fprintf(stderr, "Błąd: Nie podano pliku wyjściowego\n");
        print_usage(argv[0]);
        return 1;
    }

    // Inicjalizacja generatora liczb losowych
    srand((unsigned int)time(NULL));

    // Losowa pozycja źródła jeśli nie podano
    if (peak_x < 0) peak_x = utils_random_range(0, width);
    if (peak_y < 0) peak_y = utils_random_range(0, height);

    printf("Generowanie mapy %dx%d...\n", width, height);
    printf("Źródło sygnału: (%.2f, %.2f)\n", peak_x, peak_y);
    printf("Siła sygnału: %.2f\n", peak_value);
    printf("Poziom szumu: %.2f\n", noise);

    if (utils_generate_map(width, height, peak_x, peak_y, peak_value, noise, output_file)) {
        printf("Mapa zapisana do pliku: %s\n", output_file);
        return 0;
    } else {
        return 1;
    }
}

