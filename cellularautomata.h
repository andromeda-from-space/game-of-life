#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

// Goal: Use cellular automata and genetic algorithms to determine if a bit string is majority on
// -> If a given bit string has majority on then the result should be all on, otherwise the result should be all off

// Uses the classic 1D cellular automata definition where the bit and it's immediate neighbors determines it's value at the next step
class CellularAutomata1D{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        CellularAutomata1D();
        CellularAutomata1D(bool* rules);
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
    private:
        // The rules for simulation
        bool rules[8];
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
        bool* rules;
};

#endif