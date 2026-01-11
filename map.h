#ifndef MAP_H
#define MAP_H

// Struktura przechowująca mapę terenu
typedef struct {
    int width;      // szerokość mapy
    int height;     // wysokość mapy
    double **data;  // tablica 2D z wartościami sygnału
} Map;

// Wczytuje mapę z pliku
// Zwraca wskaźnik do mapy lub NULL w przypadku błędu
Map* map_load(const char *filename);

// Zwalnia pamięć zajmowaną przez mapę
void map_free(Map *map);

// Pobiera wartość z mapy dla danych współrzędnych (x, y)
// Jeśli współrzędne są poza zakresem, zwraca wartość karną
double map_get_value(const Map *map, double x, double y);

// Wartość karna dla pozycji poza mapą
#define MAP_PENALTY -10000.0

#endif // MAP_H

