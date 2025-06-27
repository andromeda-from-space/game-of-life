#include "cellularautomata.h"
#include "rng.h"

//---------------------------------------- CellularAutomata1D ----------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
CellularAutomata1D::CellularAutomata1D(){
    // Randomly select rules based on coin flip
    for(int i = 0; i < 8; i++){
        if(rng::genRandDouble(0.0, 1.0) > 0.5){
            rules[i] = true;
        } else {
            rules[i] = false;
        }
    }
}

CellularAutomata1D::CellularAutomata1D(bool* rules){
    for(int i = 0; i < 8; i++){
        this->rules[i] = rules[i];
    }
}

CellularAutomata1D::CellularAutomata1D(const CellularAutomata1D & other){
    for(int i = 0; i < 8; i++){
        rules[i] = other.rules[i];
    }
}

CellularAutomata1D& CellularAutomata1D::operator=(const CellularAutomata1D & other){
    // Check for self-assigment
    if(this != &other){
        for(int i = 0; i < 8; i++){
            rules[i] = other.rules[i];
        }
    }
    return *this;
}

CellularAutomata1D::~CellularAutomata1D(){
    delete[](rules);
}

//---------- UTILITIES ----------
bool** CellularAutomata1D::simulate(bool* start, int domainSize, int numSteps){
    // Create the output domain
    bool** output = new bool*[numSteps + 1];
    for(int i = 0; i < numSteps + 1; i++){
        output[i] = new bool[domainSize];
    }

    // Copy the start into the output
    for(int i = 0; i < domainSize; i++){
        output[0][i] = start[i];
    }

    // Run simulation for all steps
    int countSteps = 0;
    while(countSteps < numSteps){
        // Copy the previous step
        for(int i = 0; i < domainSize; i++){
            output[countSteps + 1][i] = output[countSteps][i];
        }

        // Run a step
        step(output[countSteps + 1], domainSize);

        // Increment
        countSteps++;
    }
    
    return output;
}

int CellularAutomata1D::majority(bool* start, int domainSize, int maxSteps){
    // create a local copy of the starting point
    bool* curr = new bool[domainSize];
    
    // Simulate for either the maximum number of steps or until the domain has stabilized into all on or off
    int currStep = 0;
    bool done = false;
    while(!done && currStep < maxSteps){
        // Perform a step
        step(curr, domainSize);
        
        // Check if done
        done = true;
        for(int i = 0; i < domainSize - 1 && done; i++){
            if(curr[i] != curr[i + 1]){
                done  = false;
            }
        }

        // Increment
        currStep++;
    }

    // Check if algorithm completed
    if(!done && currStep < maxSteps){
        return -1;
    } else {
        return curr[0];
    }
}

void CellularAutomata1D::step(bool* curr, int domainSize){
    // Rule to apply for the current neighborhood of points
    int ruleVal;
    // The end result of applying the rules
    bool* next = new bool[domainSize];

    // Left side
    ruleVal = curr[domainSize - 1] * 4 + curr[0] * 2 + curr[1];
    next[0] = rules[ruleVal];
    
    // Internals
    for(int i = 1; i < domainSize - 1; i++){
        ruleVal = curr[i - 1] * 4 + curr[i] * 2 + curr[i + 1];
        next[i] = rules[ruleVal];
    }

    // Right side
    ruleVal = curr[domainSize - 2] * 4 + curr[domainSize - 1] * 2 + curr[0];
    next[domainSize - 1] = rules[ruleVal];

    // Pointer shuffle
    delete[](curr);
    curr = next;
}

//---------------------------------------- CellularAutomata1DGeneral ----------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
CellularAutomata1DGeneral::CellularAutomata1DGeneral() {
    // Count of the neighbors in each direction - default is k = 1
    neighborCount = 1;
    // The total number of rules
    int ruleCount = 2;
    for(int i = 0; i < 2 * neighborCount; i++){
        ruleCount *= 2;
    }

    // Randomly select rules based on coin flip
    for(int i = 0; i < 8; i++){
        if(rng::genRandDouble(0.0, 1.0) > 0.5){
            rules[i] = true;
        } else {
            rules[i] = false;
        }
    }
}

CellularAutomata1DGeneral::CellularAutomata1DGeneral(int neighborCount, bool* rules){
    // TODO
}

CellularAutomata1DGeneral::CellularAutomata1DGeneral(const CellularAutomata1DGeneral & other){
    // TODO
}

CellularAutomata1DGeneral& CellularAutomata1DGeneral::operator=(const CellularAutomata1DGeneral & other){
    // TODO
    return *this;
}

CellularAutomata1DGeneral::~CellularAutomata1DGeneral(){
    // TODO
}

//---------- UTILITIES ----------
bool** CellularAutomata1DGeneral::simulate(bool* start, int domainSize, int numSteps){
    // TODO
    return nullptr;
}

int CellularAutomata1DGeneral::majority(bool* start, int domainSize, int maxSteps){
    // TODO
    return -1;
}

void CellularAutomata1DGeneral::step(bool* curr, int domainSize){
    // TODO
}