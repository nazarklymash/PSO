#include <stdlib.h>
#include "logger.h"
#include "pso.h"

Logger* logger_create(const char *filename) {
    Logger *logger = (Logger*)malloc(sizeof(Logger));
    if (!logger) {
        fprintf(stderr, "Błąd: Nie można zaalokować pamięci dla loggera\n");
        return NULL;
    }

    logger->file = fopen(filename, "w");
    if (!logger->file) {
        fprintf(stderr, "Błąd: Nie można otworzyć pliku do zapisu: %s\n", filename);
        free(logger);
        return NULL;
    }

    return logger;
}

void logger_close(Logger *logger) {
    if (logger) {
        if (logger->file) {
            fclose(logger->file);
        }
        free(logger);
    }
}

void logger_write_header(Logger *logger, int num_particles) {
    if (!logger || !logger->file) return;

    fprintf(logger->file, "iteration");
    for (int i = 0; i < num_particles; i++) {
        fprintf(logger->file, ",p%d_x,p%d_y", i, i);
    }
    fprintf(logger->file, ",gBest_x,gBest_y,gBest_value\n");
}

void logger_write_iteration(Logger *logger, int iteration, const Swarm *swarm) {
    if (!logger || !logger->file || !swarm) return;

    fprintf(logger->file, "%d", iteration);
    for (int i = 0; i < swarm->num_particles; i++) {
        fprintf(logger->file, ",%.4f,%.4f",
                swarm->particles[i].x, swarm->particles[i].y);
    }
    fprintf(logger->file, ",%.4f,%.4f,%.4f\n",
            swarm->gBest_x, swarm->gBest_y, swarm->gBest_value);
}

