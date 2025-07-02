//--DEBUG--
#include <iostream>
//--END DEBUG--
#include <fstream>

#include "geneticsolver.h"
#include "rng.h"

//---------- CONSTRUCTORS & DESTRUCTOR ----------
GeneticAlgorithm::GeneticAlgorithm() : sizePopulation(GA_DEFAULT_SIZEPOP), sizeMembers(GA_DEFAULT_SIZEMEMBER), population(nullptr), numActions(GA_DEFAULT_NUMACTIONS), actions(nullptr), fitnessVals(nullptr), totalFitness(0), crossovers(GA_DEFAULT_CROSSOVERS), mutationRate(GA_DEFAULT_MUTATION_RATE), totalGens(0) {
    // Copy actions
    actions = new char[numActions];
    for(int i = 0; i < numActions; i++){
        actions[i] = GA_DEFAULT_ACTIONS[i];
    }
    
    // Allocate memory for the fitness values
    fitnessVals = new double[sizePopulation];

    // Seed the RNG
    rng::seedRNG();

    // Initialize the population
    initPop();
}

GeneticAlgorithm::GeneticAlgorithm(int sizePopulation, int sizeMembers, int numActions, char* actions, int crossovers, double mutationRate) : sizePopulation(sizePopulation), sizeMembers(sizeMembers), population(nullptr), numActions(numActions), actions(nullptr), fitnessVals(nullptr), crossovers(crossovers), mutationRate(mutationRate), totalGens(0) {
    // Deep copy the actions
    this->actions = new char[numActions];
    for(int i = 0; i < numActions; i++){
        this->actions[i] = actions[i];
    }

    // Allocate memory for the fitness values
    fitnessVals = new double[sizePopulation];

    // Seed the RNG
    rng::seedRNG();

    // Initialize the population
    initPop();
}

GeneticAlgorithm::GeneticAlgorithm(const GeneticAlgorithm& otherGA) : sizePopulation(otherGA.sizePopulation), sizeMembers(otherGA.sizeMembers), population(nullptr), numActions(otherGA.numActions), actions(nullptr), fitnessVals(nullptr), totalFitness(otherGA.totalFitness), crossovers(otherGA.crossovers), mutationRate(otherGA.mutationRate), totalGens(otherGA.totalGens) {
    // Copy the population
    population = new char*[sizePopulation];
    for(int i = 0; i < sizePopulation; i++){
        population[i] = new char[sizeMembers];
        for(int j = 0; j < sizeMembers; j++){
            population[i][j] = otherGA.population[i][j];
        }
    }

    // Copy the actions list
    actions = new char[numActions];
    for(int i = 0; i < numActions; i++){
        actions[i] = otherGA.actions[i];
    }

    // Copy the fitness values
    fitnessVals = new double[sizePopulation];
    for(int i = 0; i < sizePopulation; i++){
        fitnessVals[i] = otherGA.fitnessVals[i];
    }
} 

GeneticAlgorithm& GeneticAlgorithm::operator=(const GeneticAlgorithm& otherGA) {
    if(this != &otherGA){
        // Clean up population if necessary
        clearPop();

        // Copy the sizes
        sizePopulation = otherGA.sizePopulation;
        sizeMembers = otherGA.sizeMembers;
        
        // Copy the population
        population = new char*[sizePopulation];
        for(int i = 0; i < sizePopulation; i++){
            population[i] = new char[sizeMembers];
            for(int j = 0; j < sizeMembers; j++){
                population[i][j] = otherGA.population[i][j];
            }
        }

        // Copy the actions
        numActions = otherGA.numActions;
        if(actions){
            delete[](actions);
            actions = nullptr;
        }
        actions = new char[numActions];
        for(int i = 0; i < numActions; i++){
            actions[i] = otherGA.actions[i];
        }

        // Copy the fitness values
        if(fitnessVals){
            delete[](fitnessVals);
            fitnessVals = nullptr;
        }
        fitnessVals = new double[sizePopulation];
        for(int i = 0; i < sizePopulation; i++){
            fitnessVals[i] = otherGA.fitnessVals[i];
        }
        totalFitness = otherGA.totalFitness;

        // Copy other values
        crossovers = otherGA.crossovers;
        mutationRate = otherGA.mutationRate;
        totalGens = otherGA.totalGens;
    }
    return *this;
}

GeneticAlgorithm::~GeneticAlgorithm(){
    // Delete the population array
    clearPop();

    // Delete the actions array
    delete[](actions);
    actions = nullptr;

    // Delete the fitness array
    delete[](fitnessVals);
    fitnessVals = nullptr;
}

//---------- GENETIC ALGORITHM I/O ----------
void GeneticAlgorithm::load(string filename){
    // Open the file
    ifstream gaFile;
    gaFile.open(filename);

    // Read the file contents
    string temp;
    vector<string> lines;
    gaFile >> temp;
    while(!gaFile.eof()){
        lines.push_back(temp);
        gaFile >> temp;
    }

    // Close the file
    gaFile.close();

    // Clear the old data
    if(population){
        clearPop();
    }

    if(fitnessVals){
        delete[](fitnessVals);
        fitnessVals = nullptr;
        totalFitness = 0;
    }

    if(actions){
        delete[](actions);
        actions = nullptr;
    }

    // Read the data and set the values
    sizePopulation = lines.size();
    sizeMembers = lines[0].length();
    population = new char*[sizePopulation];
    for(int i = 0; i < sizePopulation; i++){
        population[i] = new char[sizeMembers];
    }

    // Read the unique actions out of the given population and copy the values
    string actionsRead;
    int count = 0;
    bool found;
    for(auto itr = lines.begin(); itr != lines.end(); itr++){
        for(size_t i = 0; i < itr->length(); i++){
            // Copy the character from the line into the population
            population[count][i] = (*itr)[i];

            // Check the possible actions array for the given character
            found = false;
            for(size_t j = 0; j < actionsRead.length(); j++){
                if(actionsRead[j] == (*itr)[i]){
                    found = true;
                }
            }
            if(!found){
                actionsRead += (*itr)[i];
            }
        }
        count++;
    }

    // Copy the values of the actions
    numActions = actionsRead.length();
    actions = new char[numActions];
    for(int i = 0; i < numActions; i++){
        actions[i] = actionsRead[i];
    }

    // Allocate space for the fitness array
    fitnessVals = new double[sizePopulation];
}

void GeneticAlgorithm::save(string filename){
    // Open the file
    ofstream gaFile;
    gaFile.open(filename);

    // Save the contents to the file
    for(int j = 0; j < sizeMembers; j++){
        gaFile << population[0][j];
    }
    for(int i = 1; i < sizePopulation; i++){
        gaFile << "\n";
        for(int j = 0; j < sizeMembers; j++){
            gaFile << population[i][j];
        }
    }

    // Close the file
    gaFile.close();
}

//---------- GENETIC ALGORITHM FUNCTIONS ----------
void GeneticAlgorithm::initPop(){
    // Random value
    double roll;
    // The index of the chosen action
    int chosenAction;

    // Reset the population array
    if(population){
        clearPop();
    }
    population = new char*[sizePopulation];
    for(int i = 0; i < sizePopulation; i++){
        population[i] = new char[sizeMembers];
    }

    // Choose random actions
    for(int i = 0; i < sizePopulation; i++){
        for(int j = 0; j < sizeMembers; j++){
            roll = rng::genRandDouble(0.0, 1.0);
            chosenAction = roll * numActions;
            population[i][j] = actions[chosenAction];
        }
    }
}

void GeneticAlgorithm::train(int numGenerations){
    // Track the generations
    int currGeneration = 0;
    totalGens += numGenerations;
    
    // Run the basic Genetic Algorithm training loop
    while(currGeneration < numGenerations){
        // Evaluate fitness
        evalFitness();
        // Breed
        breed();
        // Mutate
        mutate();
        // Next Generation
        currGeneration++;
    }
}

void GeneticAlgorithm::evalFitness(){
    // Evaluate the fitness on each member of the population and track the total
    totalFitness = 0.0;
    for(int i = 0; i < sizePopulation; i++){
        fitnessVals[i] = fitness(i);
        totalFitness += fitnessVals[i];
    }
}

void GeneticAlgorithm::breed(){
    //---------- DECLARATIONS ----------
    // Index of parent 1
    int indexParent1;
    // Index of parent 2
    int indexParent2;
    // Real number roll
    double roll;
    // Fitness value to track which parent should be selected
    double currFitness;
    // The crossover points
    int crossoverPoints[crossovers];
    // The current crossover point
    int currCrossoverPoint;
    // Index to track crossover
    int index;
    // Temporary variable for swapping the integers into the right place
    int tempSwap;
    // Initialize the new population
    char** newPopulation = new char*[sizePopulation];
    for(int i = 0; i < sizePopulation; i++){
        newPopulation[i] = new char[sizeMembers];
    }

    //---------- ALGORITHM ----------
    // Pick parents and generate the offspring
    for(int i = 0; i < sizePopulation; i++){
        // Pick the first parent
        roll = rng::genRandDouble(0.0, totalFitness);
        indexParent1 = 0;
        currFitness = fitnessVals[0];
        while(currFitness < roll){
            indexParent1++;
            currFitness += fitnessVals[indexParent1];
        }

        //Pick the second parent
        roll = rng::genRandDouble(0.0, totalFitness);
        indexParent2 = 0;
        currFitness = fitnessVals[0];
        while(currFitness < roll){
            indexParent2++;
            currFitness += fitnessVals[indexParent2];
        }

        // Generate crossover points
        crossoverPoints[0] = rng::genRandInt(1, sizeMembers - 1);
        for(int j = 1; j < crossovers; j++){
            currCrossoverPoint = rng::genRandInt(0, sizeMembers - 1);
            for(int k = 0; k < j; k++){
                if(currCrossoverPoint < crossoverPoints[k]){
                    tempSwap = crossoverPoints[k];
                    crossoverPoints[k] = currCrossoverPoint;
                    currCrossoverPoint = tempSwap;
                }
            }
            crossoverPoints[j] = currCrossoverPoint;
        }

        // Perform crossover
        index = 0;
        for(int j = 0; j < crossovers; j++){
            // Copy from the first parent
            while(index < crossoverPoints[j]){
                newPopulation[i][index] = population[indexParent1][index];
                index++;
            }
            
            // Swap parents for crossover
            tempSwap = indexParent2;
            indexParent2 = indexParent1;
            indexParent1 = tempSwap;
        }
        // Copy remaining actions
        while(index < sizeMembers){
            newPopulation[i][index] = population[indexParent1][index];
            index++;
        }
    }

    // Pointer shuffle
    clearPop();
    population = newPopulation;
}

void GeneticAlgorithm::mutate(){
    bool fastFlag = (mutationRate * ((double) sizeMembers)) > 1;
    if(fastFlag){
        //---------- LARGE MEMBER ALGORITHM ----------
        // Number of mutations
        int numMutations;

        // Perform mutations
        for(int i = 0; i < sizePopulation; i++){
            numMutations = rng::genRandDouble(0.0, mutationRate);
            for(int j = 0; j < numMutations; j++){
                population[i][rng::genRandInt(0, sizePopulation - 1)] = actions[rng::genRandInt(0, numActions - 1)];
            }
        }
        // Note: this algorithm changes the definition of the mutation rate a little, as you can only mutate UP to the mutationRate * sizeMembers
    } else {
        //---------- SMALL MEMBER ALGORITHM ----------
        // For each member, randomly mutate each character based on the mutation, choosing from the set of available actions
        for(int i = 0; i < sizePopulation; i++){
            for(int j = 0; j < sizeMembers; j++){
                if(rng::genRandDouble(0.0, 1.0) < mutationRate){
                    population[i][j] = actions[rng::genRandInt(0, numActions - 1)];
                }
            }
        }
    }
}

int GeneticAlgorithm::getMostFit(bool calcFitness){
    // Calculate the fitness if desired
    if(calcFitness){
        evalFitness();
    }

    // Find the maximum
    int maxIndex = 0;
    double maxFitness = fitnessVals[0];
    for(int i = 1; i < sizePopulation; i++){
        if(fitnessVals[i] > maxFitness){
            maxIndex = i;
            maxFitness = fitnessVals[i];
        }
    }

    return maxIndex;
}

//---------- ACCESSORS ----------
char* GeneticAlgorithm::getMember(int member){
    char* memArr = new char[sizeMembers];
    for(int i = 0; i < sizeMembers; i++){
        memArr[i] = population[member][i];
    }
    return memArr;
}

double GeneticAlgorithm::getAverageFitness(bool calcFitness){
    // Calculate the fitness if necessary
    if(calcFitness){
        evalFitness();
    }

    // Returns the average total fitness
    return totalFitness / ((double) sizePopulation);
}

//---------- MUTATORS ----------
void GeneticAlgorithm::setCrossovers(int crossovers){
    this->crossovers = crossovers;
}

void GeneticAlgorithm::setMutationRate(double mutationRate){
    this->mutationRate = mutationRate;
}

void GeneticAlgorithm::setTotalGens(int totalGens){
    this->totalGens = totalGens;
}


//---------- PRIVATE UTITLITIES ----------
void GeneticAlgorithm::clearPop(){
    if(population){
        for(int i = 0; i < sizePopulation; i++){
            delete[](population[i]);
            population[i] = nullptr;
        }
        delete[](population);
    }
}

//---------- DEBUGGING UTILITIES ----------
void GeneticAlgorithm::printPop(char** pop){
    for(int i = 0; i < sizePopulation; i++){
        std::cerr << "Member " << i << ": ";
        for(int j = 0; j < sizeMembers; j++){
            std::cerr << pop[i][j] << " ";
        }
        std::cerr << "\n";
    }
}