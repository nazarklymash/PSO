#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

double utils_random_01(void) {
    return (double)rand() / RAND_MAX;
}

double utils_random_range(double min, double max) {
    return min + utils_random_01() * (max - min);
}

int utils_generate_map(int width, int height,
                       double peak_x, double peak_y,
                       double peak_value, double noise,
                       const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Błąd: Nie można utworzyć pliku mapy: %s\n", filename);
        return 0;
    }

    // Zapisz wymiary
    fprintf(file, "%d %d\n", width, height);

    // Generuj wartości sygnału
    // Sygnał słabnie proporcjonalnie do kwadratu odległości od źródła
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double dx = x - peak_x;
            double dy = y - peak_y;
            double distance = sqrt(dx * dx + dy * dy);

            // Siła sygnału słabnie z odległością
            double signal = peak_value / (1.0 + distance * distance * 0.1);

            // Dodaj szum
            signal += utils_random_range(-noise, noise) * peak_value * 0.1;

            // Zapewnij że sygnał nie jest ujemny
            if (signal < 0) signal = 0;

            fprintf(file, "%.2f", signal);
            if (x < width - 1) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

