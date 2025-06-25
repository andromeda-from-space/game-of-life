#include "geneticsolver.h"
#include "rng.h"

//---------- CONSTRUCTORS & DESTRUCTOR ----------
GeneticAlgorithm::GeneticAlgorithm() : sizePopulation(0), sizeMembers(0), population(nullptr) {
    rng::seedRNG();
}

GeneticAlgorithm::GeneticAlgorithm(int sizePopulation, int sizeMembers) : sizePopulation(sizePopulation), sizeMembers(sizeMembers), population(nullptr) {
    rng::seedRNG();
    initPop();
}

GeneticAlgorithm::GeneticAlgorithm(const GeneticAlgorithm& otherGA) : sizePopulation(otherGA.sizePopulation), sizeMembers(otherGA.sizeMembers) {
    population = new char*[sizePopulation];
    for(int i = 0; i < sizePopulation; i++){
        population[i] = new char[sizeMembers];
        for(int j = 0; j < sizeMembers; j++){
            population[i][j] = otherGA.population[i][j];
        }
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

    }
}

GeneticAlgorithm::~GeneticAlgorithm(){
    clearPop();
}

//---------- GENETIC ALGORITHM I/O ----------
void GeneticAlgorithm::load(string filename){
    // TODO
}

void GeneticAlgorithm::save(string filename){
    // TODO
}

//---------- GENETIC ALGORITHM FUNCTIONS ----------
void GeneticAlgorithm::initPop(){
    // Choose random actions
    for(int i = 0; i < sizePopulation; i++){
        for(int j = 0; j < sizeMembers; j++){

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
            currFitness += fitnessVals[indexParent1];
            indexParent1++;
        }

        //Pick the second parent
        roll = rng::genRandDouble(0.0, totalFitness);
        indexParent1 = 0;
        currFitness = fitnessVals[0];
        while(currFitness < roll){
            currFitness += fitnessVals[indexParent1];
            indexParent1++;
        }

        // Generate crossover points
        crossoverPoints[0] = rng::genRandInt(0, sizeMembers - 1);
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
    //---------- DECLARATIONS ----------
    // Number of mutations to perform
    int numMutations;
    // Index to mutate at
    int mutationIndex;
    // Action to mutate to
    char mutateAction;

    //---------- ALGORITHM ----------
    for(int i = 0; i < sizePopulation; i++){
        numMutations = rng::genRandDouble(0.0, mutationRate) * sizeMembers;
        for(int j = 0; j < numMutations; j++){
            mutationIndex = rng::genRandInt(0, sizeMembers - 1);
            mutateAction = actions[rng::genRandInt(0, numActions)];
            population[i][mutationIndex] = mutateAction;
        }
    }
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