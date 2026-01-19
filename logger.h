#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Forward declaration
typedef struct Swarm Swarm;

// Struktura loggera do zapisywania danych do pliku CSV
typedef struct {
    FILE *file;
} Logger;

// Tworzy logger i otwiera plik do zapisu
Logger* logger_create(char *filename);

// Zamyka plik i zwalnia pamięć
void logger_close(Logger *logger);

// Zapisuje header csv do ktorego zapisujemy
void logger_write_header(Logger *logger, int num_particles);

// Zapisuje dane o pozycjach cząstek dla danej iteracji
void logger_write_iteration(Logger *logger, int iteration, Swarm *swarm);

#endif