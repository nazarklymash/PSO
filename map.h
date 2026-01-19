#ifndef MAP_H
#define MAP_H

// Struktura mapy terenu
typedef struct {
    int width;      // szerokość mapy
    int height;     // wysokość mapy
    double **data;  // tablica 2D z wartościami sygnalu
} Map;

// Wczytuje mapę z pliku
// Zwraca wskaźnik do mapy lub NULL w przypadku erroru
Map* map_load(char *filename);

// Zwalnia pamięć
void map_free(Map *map);

// Pobiera wartość z mapy dla (x, y)
// Jeśli współrzędne są poza zakresem, zwraca karę
double map_get_value(Map *map, double x, double y);

// Wartość karna dla pozycji poza mapą
#define MAP_PENALTY -10000.0

#endif