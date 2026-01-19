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