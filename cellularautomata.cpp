//--DEBUG--
#include <iostream>
//--END DEBUG--
#include "cellularautomata.h"
#include "rng.h"
#include "sdl-basics.h"

//-------------------------------------------------------------------------------------
//---------- CellularAutomata1D -------------------------------------------------------
//-------------------------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
CellularAutomata1D::CellularAutomata1D() : rules(nullptr){
    // Randomly select rules based on coin flip
    rules = new char[8];
    for(int i = 0; i < 8; i++){
        (rng::genRandDouble(0.0, 1.0) > 0.5) ? rules[i] = CA_TRUE : rules[i] = CA_FALSE;
    }
}

CellularAutomata1D::CellularAutomata1D(char* rules) : rules(nullptr){
    this->rules = new char[8];
    for(int i = 0; i < 8; i++){
        this->rules[i] = rules[i];
    }
}

CellularAutomata1D::CellularAutomata1D(const CellularAutomata1D & other) : rules(nullptr){
    this->rules = new char[8];
    for(int i = 0; i < 8; i++){
        rules[i] = other.rules[i];
    }
}

CellularAutomata1D& CellularAutomata1D::operator=(const CellularAutomata1D & other){
    // Check for self-assigment
    if(this != &other){
        if(!rules){
            this->rules = new char[8];
        }

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
void CellularAutomata1D::step(bool*& curr, int domainSize){
    // Rule to apply for the current neighborhood of points
    int ruleVal;
    // The end result of applying the rules
    bool* next = new bool[domainSize];

    // Left side
    ruleVal = curr[domainSize - 1] * 4 + curr[0] * 2 + curr[1];
    next[0] = rules[ruleVal] == CA_TRUE;
    
    // Internals
    for(int i = 1; i < domainSize - 1; i++){
        ruleVal = curr[i - 1] * 4 + curr[i] * 2 + curr[i + 1];
        next[i] = rules[ruleVal] == CA_TRUE;
    }

    // Right side
    ruleVal = curr[domainSize - 2] * 4 + curr[domainSize - 1] * 2 + curr[0];
    next[domainSize - 1] = rules[ruleVal] == CA_TRUE;

    // Pointer shuffle
    delete[](curr);
    curr = next;
}

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

int CellularAutomata1D::majority(bool*& start, int domainSize, int maxSteps){
    // Simulate for either the maximum number of steps or until the domain has stabilized into all on or off
    int currStep = 0;
    bool done = false;
    while(!done && currStep < maxSteps){
        // Perform a step
        step(start, domainSize);
        
        // Check if done
        done = true;
        for(int i = 0; i < domainSize - 1 && done; i++){
            if(start[i] != start[i + 1]){
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
        return start[0];
    }
}

//---------- MUTATORS ----------
void CellularAutomata1D::setRules(char* newRules){
    for(int i = 0; i < 8; i++){
        rules[i] = newRules[i];
    }
}

void CellularAutomata1D::snapShot(bool* start, int domainSize, int numSteps){
    // Generate the data
    bool** data = simulate(start, domainSize, numSteps);

    // Draw the snapshot
    SDLPixelGridRenderer pixelRenderer = SDLPixelGridRenderer("1D Cellular Automata", numSteps + 1, domainSize, CA_FALSE_COLOR, CA_GRID_LINES, CA_TRUE_COLOR);
    pixelRenderer.drawBoolGrid(data, false, "");
}

//-------------------------------------------------------------------------------------
//---------- MajoritySolverGA ---------------------------------------------------------
//-------------------------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
MajoritySolverGA::MajoritySolverGA() : GeneticAlgorithm(), currAutomata(nullptr), numFitnessTests(-1), domainSize(-1), maxSteps(-1) {}

MajoritySolverGA::MajoritySolverGA(int sizePopulation, int sizeMembers, int numActions, char* actions, int crossovers, double mutationRate, int totalGens, int numFitnessTests, int domainSize, int maxSteps) : GeneticAlgorithm(sizePopulation, sizeMembers, numActions, actions,crossovers, mutationRate, totalGens), currAutomata(nullptr), numFitnessTests(numFitnessTests), domainSize(domainSize), maxSteps(maxSteps) {}

MajoritySolverGA::MajoritySolverGA(const MajoritySolverGA & other) : GeneticAlgorithm(other), currAutomata(nullptr), numFitnessTests(other.numFitnessTests), domainSize(other.domainSize), maxSteps(other.maxSteps) {}

MajoritySolverGA& MajoritySolverGA::operator=(const MajoritySolverGA & other) {
    if(this != &other){
        // Set genetic algorithm variables
        this->GeneticAlgorithm::operator=(other);

        // Clean up old cellular automata placeholder
        if(currAutomata){
            delete(currAutomata);
            currAutomata = nullptr;
        }

        // Set other variables
        numFitnessTests = other.numFitnessTests;
        domainSize = other.domainSize;
        maxSteps = other.maxSteps;
    }
    return *this;
}

MajoritySolverGA::~MajoritySolverGA() {
    if(currAutomata){
        delete(currAutomata);
        currAutomata = nullptr;
    }
};

//---------- GENETIC ALGORITHM FUNCTIONS ----------
double MajoritySolverGA::fitness(int member){
    if(!currAutomata){
        currAutomata = new CellularAutomata1D(population[member]);
    } else {
        currAutomata->setRules(population[member]);
    }

    // Randomly generate bit strings and evaluate
    double fitness = (double) numFitnessTests;
    // The bit string to test the cellular automata on
    bool* start = new bool[domainSize];
    // Count of initial trueValues
    int totalTrue;
    // The majority value
    bool majority;
    // The value returned by majority
    int eval;
    // Current number of true values
    int currTrue;


    // Attempt to classify the random starting points
    for(int i = 0; i < numFitnessTests; i++){
        totalTrue = 0;
        // Generate a random starting point
        for(int j = 0; j < domainSize; j++){
            if(rng::genRandDouble(0.0, 1.0) > 0.5){
                start[j] = true;
                totalTrue++;
            } else {
                start[j] = false;
            }
        }
        // Evaluate which is the majority
        majority = totalTrue >= domainSize / 2 ? true : false;

        // Apply the rules
        eval = currAutomata->majority(start, domainSize, maxSteps);

        // Calculate the fitness for this trial
        if(eval >= 0){
            if(majority == start[0]){
                // Got the correct result
                fitness += domainSize;
            } else {
                // Got the opposite result it should have
                fitness += 0;
            }    
        } else {
            // Calculate the fitness update otherwise
            currTrue = 0;
            for(int i = 0; i < domainSize; i++){
                currTrue += start[i];
            }
            fitness += (majority ? currTrue : (domainSize - currTrue));
        }
    }

    // Clean up
    delete[](start);

    // Return the fitness value
    return fitness / ((double) numFitnessTests);
}

//---------- UTILITIES ----------
void MajoritySolverGA::animateMember(int member){
    // TODO
}

//-----------------------------------------------------------------------------
//---------- WrapInt ----------------------------------------------------------
//-----------------------------------------------------------------------------
//---------- CONSTRUCTORS ----------
WrapInt::WrapInt() : currVal(0), max(0){}

WrapInt::WrapInt(int val, int max) : currVal(val), max(max) {
    wrapVal();
}

//---------- OPERATIONS ----------
WrapInt& WrapInt::operator+=(int rhs){
    currVal += rhs;
    wrapVal();
}

WrapInt& WrapInt::operator-=(int rhs){
    currVal -= rhs;
    wrapVal();
}

WrapInt& WrapInt::operator*=(int rhs){
    currVal *= rhs;
    wrapVal();
}

WrapInt& WrapInt::operator++(){
    currVal = currVal + 1 == max ? 0 : currVal + 1;
}

WrapInt WrapInt::operator++(int dummy){
    return WrapInt(currVal + 1 == max ? 0 : currVal + 1, max);
}

WrapInt& WrapInt::operator--(){
    currVal = currVal - 1 < 0 ? max - 1 : currVal - 1;
}

WrapInt WrapInt::operator--(int dummy){
    return WrapInt(currVal - 1 < 0 ? max - 1 : currVal - 1, max);
}

bool operator==(const WrapInt& lhs, const WrapInt& rhs){

}

bool operator==(int lhs, const WrapInt& rhs){

}

bool operator==(const WrapInt& lhs, int rhs){

}

bool operator<(const WrapInt& lhs, const WrapInt& rhs){

}

bool operator<(int lhs, const WrapInt& rhs){

}

bool operator<(const WrapInt& lhs, int rhs){

}

bool operator>(const WrapInt& lhs, const WrapInt& rhs){

}

bool operator>(int lhs, const WrapInt& rhs){

}

bool operator>(const WrapInt& lhs, int rhs){

}

bool operator<=(const WrapInt& lhs, const WrapInt& rhs){

}

bool operator<=(int lhs, const WrapInt& rhs){

}

bool operator<=(const WrapInt& lhs, int rhs){
    
}

bool operator>=(const WrapInt& lhs, const WrapInt& rhs){
    return lhs.currVal >= rhs.currVal;
}

bool operator>=(int lhs, const WrapInt& rhs){
    return lhs >= rhs.currVal;
}

bool operator>=(const WrapInt& lhs, int rhs){
    return lhs.currVal >= rhs;
}

//---------- UTILITIES ----------
void WrapInt::wrapVal() {
    if(currVal >= max){
        currVal = currVal % max;
    } else if(currVal < 0){
        currVal -= ((currVal / max) - 1) * max;
        currVal = currVal % max;
    }
}

//---------- MUTATORS ----------
void WrapInt::setVal(int newVal){
    currVal = newVal;
    wrapVal();
}

void WrapInt::setMax(int newMax){
    max = newMax;
}

//---------- ACCESSORS ----------
int WrapInt::getVal(){
    return currVal;
}

//-----------------------------------------------------------------------------
//---------- CellularAutomata1DGeneral ----------------------------------------
//-----------------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
CellularAutomata1DGeneral::CellularAutomata1DGeneral() : neighborCount(0), numRules(0), rules(nullptr) {
    // Count of the neighbors in each direction - default is k = 1
    neighborCount = 1;
    // The total number of rules
    numRules = 2;
    for(int i = 0; i < 2 * neighborCount; i++){
        numRules *= 2;
    }

    // Randomly select rules based on coin flip
    rules = new char[numRules];
    for(int i = 0; i < numRules; i++){
        (rng::genRandDouble(0.0, 1.0) > 0.5) ? rules[i] = CA_TRUE : rules[i] = CA_FALSE;
    }
}

CellularAutomata1DGeneral::CellularAutomata1DGeneral(int neighborCount) : neighborCount(neighborCount), numRules(0), rules(nullptr) {
    // The total number of rules
    numRules = 2;
    for(int i = 0; i < 2 * neighborCount; i++){
        numRules *= 2;
    }

    // Randomly select rules based on coin flip
    this->rules = new char[numRules];
    for(int i = 0; i < numRules; i++){
        (rng::genRandDouble(0.0, 1.0) > 0.5) ? rules[i] = CA_TRUE : rules[i] = CA_FALSE;
    }
}

CellularAutomata1DGeneral::CellularAutomata1DGeneral(int neighborCount, char* rules) : neighborCount(neighborCount), numRules(0), rules(nullptr){
    // The total number of rules
    numRules = 2;
    for(int i = 0; i < 2 * neighborCount; i++){
        numRules *= 2;
    }

    // Copy the provided rules
    this->rules = new char[numRules];
    for(int i = 0; i < numRules; i++){
        this->rules[i] = rules[i];
    }
}

CellularAutomata1DGeneral::CellularAutomata1DGeneral(const CellularAutomata1DGeneral & other) : neighborCount(other.neighborCount), numRules(other.numRules), rules(nullptr){
    // Copy the provided rules
    this->rules = new char[numRules];
    for(int i = 0; i < numRules; i++){
        this->rules[i] = other.rules[i];
    }
}

CellularAutomata1DGeneral& CellularAutomata1DGeneral::operator=(const CellularAutomata1DGeneral & other){
    // Check for self assignment
    if(this != &other){
        // Clean up old rules
        if(rules){
            delete[](rules);
            rules = nullptr;
        }

        // Set other values
        neighborCount = other.neighborCount;
        numRules = other.numRules;

        // Copy the rules
        rules = new char[numRules];
        for(int i = 0; i < numRules; i++){
            rules[i] = other.rules[i];
        }
    }
    return *this;
}

CellularAutomata1DGeneral::~CellularAutomata1DGeneral(){
    if(rules){
        delete[](rules);
        rules = nullptr;
    }
}

//---------- UTILITIES ----------
void CellularAutomata1DGeneral::step(bool*& curr, int domainSize){
    // Rule to apply for the current neighborhood of points
    int ruleVal;
    // The current multiplier
    int currMult;
    // The end result of applying the rules
    bool* next = new bool[domainSize];

    // Create a wrapping index value
    WrapInt wrapIndex = WrapInt(0, domainSize);
    
    // Update all values
    for(int i = 0; i < domainSize; i++){
        // Initial value
        wrapIndex.setVal(i + neighborCount);

        // Calculate the local rule value to use
        ruleVal = 0;
        currMult = 1;
        for(int j = 0; j < 2 * neighborCount + 1; j++){
            ruleVal += currMult * curr[wrapIndex.getVal()];
            currMult *= 2;
            wrapIndex -= 1;
        }
        next[i] = rules[ruleVal] == CA_TRUE;
    }

    // Pointer shuffle
    delete[](curr);
    curr = next;
}

bool** CellularAutomata1DGeneral::simulate(bool* start, int domainSize, int numSteps){
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

int CellularAutomata1DGeneral::majority(bool*& start, int domainSize, int maxSteps){
    // Simulate for either the maximum number of steps or until the domain has stabilized into all on or off
    int currStep = 0;
    bool done = false;
    while(!done && currStep < maxSteps){
        // Perform a step
        step(start, domainSize);
        
        // Check if done
        done = true;
        for(int i = 0; i < domainSize - 1 && done; i++){
            if(start[i] != start[i + 1]){
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
        return start[0];
    }
}

//---------- MUTATORS ----------
void CellularAutomata1DGeneral::setRules(char* newRules){
    for(int i = 0; i < numRules; i++){
        rules[i] = newRules[i];
    }
}

//---------- GRAPHICAL REPRESENTATION ----------
// Create an image of the final result of the rule as applied to the start
void CellularAutomata1DGeneral::snapShot(bool* start, int domainSize, int numSteps){
    // Generate the data
    bool** data = simulate(start, domainSize, numSteps);

    // Draw the snapshot
    SDLPixelGridRenderer pixelRenderer = SDLPixelGridRenderer("General 1D Cellular Automata", numSteps + 1, domainSize, CA_FALSE_COLOR, CA_GRID_LINES, CA_TRUE_COLOR);
    pixelRenderer.drawBoolGrid(data, false, "");
}