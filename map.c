#include <stdio.h>
#include <stdlib.h>
#include "map.h"

Map* map_load(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie można otworzyć pliku mapy");
        return NULL;
    }

    Map *map = (Map*)malloc(sizeof(Map));
    if (!map) {
        fprintf(stderr, "Błąd alokacji pamięci dla mapy\n");
        fclose(file);
        return NULL;
    }

    // Wczytujemy wymiary mapy
    if (fscanf(file, "%d %d", &map->width, &map->height) != 2) {
        fprintf(stderr, "Niepoprawny format pliku mapy\n");
        free(map);
        fclose(file);
        return NULL;
    }

    // Alokacja dwuwymiarnej tablicy data
    map->data = (double**)malloc(map->height * sizeof(double*));
    if (!map->data) {
        fprintf(stderr, "Nie można zaalokować pamięci dla danych mapy\n");
        free(map);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < map->height; i++) {
        map->data[i] = (double*)malloc(map->width * sizeof(double));
        if (!map->data[i]) {
            fprintf(stderr, "Nie można zaalokować pamięci dla wiersza mapy\n");
            // free zaalokowanych wierszy
            for (int j = 0; j < i; j++) {
                free(map->data[j]);
            }
            free(map->data);
            free(map);
            fclose(file);
            return NULL;
        }

        // Czytamy wartości dla wiersza
        for (int j = 0; j < map->width; j++) {
            if (fscanf(file, "%lf", &map->data[i][j]) != 1) {
                fprintf(stderr, "Niepoprawne dane mapy w wierszu %d, kolumnie %d\n", i, j);
                // free reszte zaalokowanego miejsca
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

double map_get_value(Map *map, double x, double y) {
    int col = (int)x;
    int row = (int)y;

    // Sprawdzamy czy pozycja jest w granicach mapy
    if (col < 0 || col >= map->width || row < 0 || row >= map->height) {
        return MAP_PENALTY;  // Kara za wyjście poza mapę
    }

    return map->data[row][col];
}