#ifndef RNG_H
#define RNG_H

#include <random>
#include <ctime>
#include <limits>

namespace rng{
    // Double distribution between 0.0 and 1.0
    extern std::uniform_real_distribution<double> dist;

    // Mersenne Twister for the technique to generate the numbers
    extern std::mt19937 generator; 

    // Flag for the rng being seeded
    extern bool seeded;

    // Seed the rng
    void seedRNG();

    // Generate a random number between min and max inclusive
    double genRandDouble(double min, double max);

    // Generate a random number between min and max inclusive
    int genRandInt(int min = 0.0, int max = 1.0);
}

#endif