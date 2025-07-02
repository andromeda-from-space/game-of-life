#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

#include <SDL2/SDL.h>

#include "geneticsolver.h"

// Goal: Use cellular automata and genetic algorithms to determine if a bit string is majority on
// -> If a given bit string has majority on then the result should be all on, otherwise the result should be all off

// Flag values
const char CA_FALSE = '0';
const char CA_TRUE = '1';

// Some graphical constants
// Color for the grid line
const SDL_Color CA_GRID_LINES = {150, 150, 150, 255};
// Color for the tiles that are labeled true
const SDL_Color CA_FALSE_COLOR = {0, 0, 0, 255};
// Color for the tiles that are labeled false
const SDL_Color CA_TRUE_COLOR = {255, 255, 255, 255};
// Pixel size width for the tiles
const int CA_PIXEL_SIZE = 5;

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
        // Generates a new random set of rules
        void genRandomRules();
        // Updates the current domain based on the rules
        void step(bool*& curr, int domainSize);
        // Simulates the domain for the number of steps, returning an array of all of the results
        bool** simulate(bool* start, int domainSize, int numSteps);
        // Attempts to solve the majority on/off problem with the given rule
        // If no conclusion is reached returns -1, otherwise returns the majority
        // Modifies the start array to allow for the user to see the final result
        int majority(bool*& start, int domainSize, int maxSteps);
        
        //---------- MUTATORS ----------
        void setRules(char* newRules);

        //---------- GRAPHICAL REPRESENTATION ----------
        // Create an image of the final result of the rule as applied to the start
        void snapShot(bool* start, int domainSize, int numSteps, int pixelSize = 20);
    private:
        // The rules for simulation
        char* rules;
};

// Majority Solver Genetic Algorithm
// Uses a genetic algotihm to solve the basic nearest neighbor domain cell automata in a periodic domain
// Fitness is the average number of values that match the majority over a pre-specified number of tests. The majority calculation is allowed to run for a pre-specified number of steps before concluding
class MajoritySolverGA : public GeneticAlgorithm {
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        MajoritySolverGA();
        MajoritySolverGA(int sizePopulation, int crossovers, double mutationRate, int numFitnessTests, int domainSize, int maxSteps);
        MajoritySolverGA(const MajoritySolverGA & other);
        MajoritySolverGA& operator=(const MajoritySolverGA & other);
        ~MajoritySolverGA();

        //---------- GENETIC ALGORITHM FUNCTIONS ----------
        // Fitness function for the genetic algorithm
        double fitness(int member);

        //---------- UTILITIES ----------
        // Creates an animation of the given member
        void visualizeMember(int member);

    private:
        static char CA_ACTIONS[2];
        // Cellular Automata framework for evaluating the fitness
        CellularAutomata1D* currAutomata;
        // Number of random strings to test the member on when evaluating the fitness
        int numFitnessTests;
        // Domain size for the cellular automata testing
        int domainSize;
        // The maximum number of steps before the fitness function gives up
        int maxSteps;
};

// Super basic integer that is meant to be positive and wrap around if it exceeds the maximum value
class WrapInt {
    public:
        //---------- CONSTRUCTORS ----------
        WrapInt();
        WrapInt(int val, int max);

        //---------- OPERATIONS ----------
        // Arithemetic assignment
        WrapInt& operator+=(int rhs);
        WrapInt& operator-=(int rhs);
        WrapInt& operator*=(int rhs);
        WrapInt& operator/=(int rhs);
        // Increment
        WrapInt& operator++();
        WrapInt operator++(int dummy);
        // Decrement
        WrapInt& operator--();
        WrapInt operator--(int dummy);
        // Comparisons
        friend bool operator==(const WrapInt& lhs, const WrapInt& rhs);
        friend bool operator==(int lhs, const WrapInt& rhs);
        friend bool operator==(const WrapInt& lhs, int rhs);
        friend bool operator<(const WrapInt& lhs, const WrapInt& rhs);
        friend bool operator<(int lhs, const WrapInt& rhs);
        friend bool operator<(const WrapInt& lhs, int rhs);
        friend bool operator>(const WrapInt& lhs, const WrapInt& rhs);
        friend bool operator>(int lhs, const WrapInt& rhs);
        friend bool operator>(const WrapInt& lhs, int rhs);
        friend bool operator<=(const WrapInt& lhs, const WrapInt& rhs);
        friend bool operator<=(int lhs, const WrapInt& rhs);
        friend bool operator<=(const WrapInt& lhs, int rhs);
        friend bool operator>=(const WrapInt& lhs, const WrapInt& rhs);
        friend bool operator>=(int lhs, const WrapInt& rhs);
        friend bool operator>=(const WrapInt& lhs, int rhs);

        //---------- UTILITIES ----------
        // Enforces the wrap around condition
        void wrapVal();

        //---------- MUTATORS ----------
        void setVal(int newVal);
        void setMax(int newMax);

        //---------- ACCESSORS ----------
        int getVal();
        int getMax();

    private:
        // The value of the integer
        int currVal;
        // The maximum value before it gets wrapped - i.e. the mod space
        int max;
};

// Generalizes the above concept, where instead of just the most immediate left and right neighbor, considers some k neighbors in each direction (k = 1 is the above case)
class CellularAutomata1DGeneral{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        CellularAutomata1DGeneral();
        CellularAutomata1DGeneral(int neighborCount);
        CellularAutomata1DGeneral(int neighborCount, char* rules);
        CellularAutomata1DGeneral(const CellularAutomata1DGeneral & other);
        CellularAutomata1DGeneral& operator=(const CellularAutomata1DGeneral & other);
        ~CellularAutomata1DGeneral();

        //---------- UTILITIES ----------
        // Generates a new random set of rules
        void genRandomRules();
        // Updates the current domain based on the rules
        void step(bool*& curr, int domainSize);
        // Simulates the domain for the number of steps, returning an array of all of the results
        bool** simulate(bool* start, int domainSize, int numSteps);
        // Attempts to solve the majority on/off problem with the given rule
        // If no conclusion is reached returns -1, otherwise returns the majority
        // Modifies the start array to allow for the user to see the final result
        int majority(bool*& start, int domainSize, int maxSteps);

        //---------- MUTATORS ----------
        void setRules(char* newRules);

        //---------- GRAPHICAL REPRESENTATION ----------
        // Create an image of the final result of the rule as applied to the start
        void snapShot(bool* start, int domainSize, int numSteps);
    private:
        // The number of neighbhors to consider in the rule
        int neighborCount;
        // Size of the rules of array
        int numRules;
        // The rules for simulation
        char* rules;
};

#endif