#include <stdlib.h>
#include "logger.h"
#include "pso.h"

Logger* logger_create(char *filename) {
    Logger *logger = (Logger*)malloc(sizeof(Logger));
    if (!logger) {
        fprintf(stderr, "Blad alokacji pamieci dla loggera\n");
        return NULL;
    }

    logger->file = fopen(filename, "w");
    if (!logger->file) {
        fprintf(stderr, "Blad otwierania pliku: %s\n", filename);
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
    fprintf(logger->file, ",global_best_x,global_best_y,global_best_value\n");
}

void logger_write_iteration(Logger *logger, int iteration, Swarm *swarm) {
    if (!logger || !logger->file || !swarm) return;

    fprintf(logger->file, "%d", iteration);
    for (int i = 0; i < swarm->num_particles; i++) {
        fprintf(logger->file, ",%.4f,%.4f",
                swarm->particles[i].x, swarm->particles[i].y);
    }

    double gbest_x = swarm->global_best_x;
    double gbest_y = swarm->global_best_y;
    double gbest_value = swarm->global_best_value;

    fprintf(logger->file, ",%lf,%lf,%lf\n", gbest_x, gbest_y, gbest_value);
}