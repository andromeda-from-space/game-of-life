#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

#include "geneticsolver.h"
#include "sdl-basics.h"

// TODO - modify to use character arrays instead of bools for the evaluation of the rules
// TODO - create graphical representations of the results of processing the rules

// Goal: Use cellular automata and genetic algorithms to determine if a bit string is majority on
// -> If a given bit string has majority on then the result should be all on, otherwise the result should be all off

// Flag values
const char CA_FALSE = '0';
const char CA_TRUE = '1';

// Some graphical constants
// Pixel size width for the tiles
const int CA_PIXEL_SIZE = 5;
// Color for the tiles that are labeled true
const SDL_Color CA_FALSE_COLOR = {255, 255, 255, 255};
// Color for the grid line
const SDL_Color CA_GRID_LINES = {150, 150, 150, 255};
// Color for the tiles that are labeled false
const SDL_Color CA_TRUE_COLOR = {255, 234, 0, 255};

// Uses the classic 1D cellular automata definition where the bit and it's immediate neighbors determines it's value at the next step
// Uses a toroidal (i.e. wrap around) domain for the boundary values of the bit string
class CellularAutomata1D{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        CellularAutomata1D();
        CellularAutomata1D(char* rules);
        CellularAutomata1D(const CellularAutomata1D & other);
        CellularAutomata1D& operator=(const CellularAutomata1D & other);
        ~CellularAutomata1D();

        //---------- UTILITIES ----------
        // Simulates the domain for the number of steps, returning an array of all of the results
        bool** simulate(bool* start, int domainSize, int numSteps);
        // Attempts to solve the majority on/off problem - if no conclusion is reached returns -1
        int majority(bool* start, int domainSize, int maxSteps);
        // Updates the current domain based on the rules
        void step(bool* curr, int domainSize);

        //---------- MUTATORS ----------
        void setRules(char* newRules);

        //---------- GRAPHICAL REPRESENTATION ----------
        // Create an image of the final result of the rule as applied to the start
        void snapShot(SDLWindowWrapper & window, bool* start, int domainSize, int numSteps);
        void snapShot(SDLWindowWrapper & window, int rows, int cols, int results);
    private:
        // The rules for simulation
        char rules[8];
};

// TODO - Constructors with parent class
// TODO - fitness function might be a bad metric
//  - alternative : for majority 1 - fitness is the count of 1s, for majority 0 - fitness is the count of 0s - possibly rewrite the majority function with this in mind
class MajoritySolverGA : public GeneticAlgorithm {
    public:
        //---------- GENETIC ALGORITHM FUNCTIONS ----------
        double fitness(int member);
    private:
        // Cellular Automata framework for evaluating the fitness
        CellularAutomata1D currAutomata;
        // Number of random strings to test the member on when evaluating the fitness
        int numFitnessTests;
        // Domain size for the cellular automata testing
        int domainSize;
        // The maximum number of steps before the fitness function gives up
        int maxSteps;
};

// Generalizes the above concept, where instead of just the most immediate left and right neighbor, considers some k neighbors in each direction (k = 1 is the above case)
class CellularAutomata1DGeneral{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        CellularAutomata1DGeneral();
        CellularAutomata1DGeneral(int neighborCount, bool* rules);
        CellularAutomata1DGeneral(const CellularAutomata1DGeneral & other);
        CellularAutomata1DGeneral& operator=(const CellularAutomata1DGeneral & other);
        ~CellularAutomata1DGeneral();

        //---------- UTILITIES ----------
        // Simulates the domain for the number of steps, returning an array of all of the results
        bool** simulate(bool* start, int domainSize, int numSteps);
        // Attempts to solve the majority on/off problem
        int majority(bool* start, int domainSize, int maxSteps);
        // Updates the current domain based on the rules
        void step(bool* curr, int domainSize);
    private:
        // The number of neighbhors to consider in the rule
        int neighborCount;
        // The rules for simulation
        char* rules;
};

#endif