#include "rng.h"

namespace rng{
    std::uniform_real_distribution<double> dist;
    std::mt19937 generator;
    bool seeded;
}

void rng::seedRNG(){
    dist = std::uniform_real_distribution<double>(0.0, 1.0);
    generator.seed(std::random_device{}());
    seeded = true;
}

double rng::genRandDouble(double min, double max){
    return dist(generator) * (max - min) + min;
}

int rng::genRandInt(int min, int max){
    return ((int) (dist(generator) * INT32_MAX)) % (max - min + 1) + min;
}