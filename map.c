#include <stdio.h>
#include <stdlib.h>
#include "map.h"

Map* map_load(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Błąd: Nie można otworzyć pliku mapy: %s\n", filename);
        return NULL;
    }

    Map *map = (Map*)malloc(sizeof(Map));
    if (!map) {
        fprintf(stderr, "Błąd: Nie można zaalokować pamięci dla mapy\n");
        fclose(file);
        return NULL;
    }

    // Wczytaj wymiary mapy
    if (fscanf(file, "%d %d", &map->width, &map->height) != 2) {
        fprintf(stderr, "Błąd: Niepoprawny format pliku mapy\n");
        free(map);
        fclose(file);
        return NULL;
    }

    // Alokacja tablicy 2D
    map->data = (double**)malloc(map->height * sizeof(double*));
    if (!map->data) {
        fprintf(stderr, "Błąd: Nie można zaalokować pamięci dla danych mapy\n");
        free(map);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < map->height; i++) {
        map->data[i] = (double*)malloc(map->width * sizeof(double));
        if (!map->data[i]) {
            fprintf(stderr, "Błąd: Nie można zaalokować pamięci dla wiersza mapy\n");
            // Zwolnij już zaalokowane wiersze
            for (int j = 0; j < i; j++) {
                free(map->data[j]);
            }
            free(map->data);
            free(map);
            fclose(file);
            return NULL;
        }

        // Wczytaj wartości dla wiersza
        for (int j = 0; j < map->width; j++) {
            if (fscanf(file, "%lf", &map->data[i][j]) != 1) {
                fprintf(stderr, "Błąd: Niepoprawne dane mapy w wierszu %d, kolumnie %d\n", i, j);
                // Zwolnij pamięć
                for (int k = 0; k <= i; k++) {
                    free(map->data[k]);
                }
                free(map->data);
                free(map);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return map;
}

void map_free(Map *map) {
    if (map) {
        if (map->data) {
            for (int i = 0; i < map->height; i++) {
                free(map->data[i]);
            }
            free(map->data);
        }
        free(map);
    }
}

double map_get_value(const Map *map, double x, double y) {
    // Konwersja współrzędnych ciągłych na indeksy dyskretne
    int col = (int)x;
    int row = (int)y;

    // Sprawdzenie czy pozycja jest w granicach mapy
    if (col < 0 || col >= map->width || row < 0 || row >= map->height) {
        return MAP_PENALTY;  // Kara za wyjście poza mapę
    }

    return map->data[row][col];
}

