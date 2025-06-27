#ifndef GENETICSOLVER_H
#define GENETICSOLVER_H

#include <string>

using namespace std;

class GeneticAlgorithm{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        GeneticAlgorithm();
        GeneticAlgorithm(int sizePopulation, int sizeMembers);
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
        virtual double fitness(int member) = 0;
        // Train the algorithm for the specified number of generations
        void train(int numGenerations);
        // Evaluate the fitness of the population
        void evalFitness();
        // Choose parents for breeding and create a new population
        void breed();
        // Mutate the children based on the mutation rate
        void mutate();
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
        //---------- PRIVATE UTITLITIES ----------
        // Deletes the data in the population array
        void clearPop();
};

#endif