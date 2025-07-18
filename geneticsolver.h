#ifndef GENETICSOLVER_H
#define GENETICSOLVER_H

#include <string>

using namespace std;

/*
Genetic Algorithm

A generalizeable framework for apply a genetic algorithm to a problem. Requires user to implement a fitness function in order to evaluate a given member of the population's fitness. Fitness must be able to be evaluated based on the actions dictated by the given character array. Thus, the user must translate the character string into actions that the the system takes in order to solve the problem at hand and evaluate it's fitness.

A character array is chosen to represent the members the data members are single bytes and have a range of 0 to 255 which is more actions than most problems need.

Data Members:
- sizePopulation - the size of the population being simulated in the genetic algorithm
- sizeMember - the length of the character array that represents the members and state-action pairs
    Example: the traditional 1D cellular automata would be represented by 8 characters - so this value is 8
- population - the collection of members of the population who's fitness is to be evaluated
- numActions - the possible number of actions a member can take for any of the given states
    Example: the traditional 1D cellular automata would be represented by 2 possible actions
- actions - list of all possible actions - this allows for the members to be human readable if desired
    Example: the traditional 1D cellular automata might use {'0', '1'} to represent the final value of the bit for the given state
- fitnessVals - an array of the fitness values as evaluated on the current population
- totalFitness - the sum of all the fitness values for the current population
- crossovers - the number of crossovers to be performed when generating new offspring
- mutationRate - the rate of mutations in the population when apply the mutation step 0.0 means no mutations
- totalGens - the number of generations to breed before considering the algorithm to be done
*/

// Some default parameters - chosen with cellular automata in mind
const int GA_DEFAULT_SIZEPOP = 100;
const int GA_DEFAULT_SIZEMEMBER = 8;
const int GA_DEFAULT_NUMACTIONS = 2;
const char GA_DEFAULT_ACTIONS[] = {'0', '1'};
const int GA_DEFAULT_CROSSOVERS = 1;
const int GA_DEFAULT_MUTATION_RATE = 0.1;

class GeneticAlgorithm{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        GeneticAlgorithm();
        GeneticAlgorithm(int sizePopulation, int sizeMembers, int numActions, char* actions, int crossovers, double mutationRate);
        GeneticAlgorithm(const GeneticAlgorithm& otherGA);
        GeneticAlgorithm& operator=(const GeneticAlgorithm& otherGA);
        virtual ~GeneticAlgorithm();

        //---------- GENETIC ALGORITHM I/O ----------
        // Loads a population from the file
        void load(string filename);
        // Saves a populationn to a file
        void save(string filename);

        //---------- GENETIC ALGORITHM FUNCTIONS ----------
        // Initializes the population
        void initPop();
        // Evaluates the fitness of a member of the population
        // Note: use of pure virtual to force implementation
        virtual double fitness(int member) = 0;
        // Train the algorithm for the specified number of generations
        void train(int numGenerations = 1);
        // Evaluate the fitness of the population
        void evalFitness();
        // Choose parents for breeding and create a new population
        void breed();
        // Mutate the children based on the mutation rate
        void mutate();
        // Returns the most fit member index based on the current fitness values, recalculates if desired
        int getMostFit(bool calcFitness);

        //---------- ACCESSORS ----------
        // Makes a copy of the member for external use
        char* getMember(int member);
        // Returns the average fitness - calculates the fitness if necessary
        double getAverageFitness(bool calcFitness);

        //---------- MUTATORS ----------
        void setCrossovers(int crossovers);
        void setMutationRate(double mutationRate);
        void setTotalGens(int totalGens);
    protected:
        // The size of the population
        int sizePopulation;
        // The size of an individual member of the population
        int sizeMembers;
        // The actual population
        char** population;
        // Number of possible actions
        int numActions;
        // List of possible actions
        char* actions;
        // Fitness of the population
        double* fitnessVals;
        // Total fitness
        double totalFitness;
        // Number of crossover points
        int crossovers;
        // Mutation rate
        double mutationRate;
        // Total number of generations
        int totalGens;
    private:
        //---------- PRIVATE UTILITIES ----------
        // Deletes the data in the population array
        void clearPop();

        //---------- DEBUGGING UTILITIES ----------
        // Prints the population when called on this->population
        // Also allows for debugging the breeding algorithm which generates a new population
        void printPop(char** pop);
};

#endif