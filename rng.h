#ifndef RNG_H
#define RNG_H

#include <random>
#include <ctime>
#include <limits>

namespace rng {
    // Double distribution between 0.0 and 1.0
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Mersenne Twister for the technique to generate the numbers
    std::mt19937 generator; 

    // Flag for the rng being seeded
    bool seeded = false;

    // Seed the rng
    void seedRNG(){
        generator.seed(std::random_device{}());
        seeded = true;
    }

    // Generate a random number between min and max inclusive
    double genRandDouble(double min, double max){
        return dist(generator) * (max - min) + min;
    }

    // Generate a random number between min and max inclusive
    int genRandInt(int min, int max){
        return ((int) (dist(generator) * INT32_MAX)) % (max - min + 1) + min;
    }
}

#endif