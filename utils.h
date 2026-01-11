#ifndef UTILS_H
#define UTILS_H

// Generuje losową liczbę z zakresu [0, 1]
double utils_random_01(void);

// Generuje losową liczbę z zakresu [min, max]
double utils_random_range(double min, double max);

// Generuje mapę terenu i zapisuje ją do pliku
// width, height - wymiary mapy
// peak_x, peak_y - współrzędne maksimum sygnału
// peak_value - wartość maksimum
// noise - poziom szumu (0.0 - brak szumu, 1.0 - dużo szumu)
// filename - nazwa pliku do zapisu
// Zwraca 1 w przypadku sukcesu, 0 w przypadku błędu
int utils_generate_map(int width, int height,
                       double peak_x, double peak_y,
                       double peak_value, double noise,
                       const char *filename);

#endif // UTILS_H

