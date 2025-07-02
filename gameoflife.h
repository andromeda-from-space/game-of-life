#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include "geneticsolver.h"

//---------- CONSTANTS ----------
// Default board size
const int GAME_OF_LIFE_DEFAULT_ROWS = 10;
const int GAME_OF_LIFE_DEFAULT_COLS = 10;

class GameOfLife{
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        GameOfLife();
        GameOfLife(int rows, int cols);
        GameOfLife(const GameOfLife & other);
        GameOfLife& operator=(const GameOfLife & other);
        ~GameOfLife();

        //---------- UTILITIES ----------
        // Adds the organism to the board. Allows for both bool arrays (which match the data structure here) and char arrays which match the genetic algorithm code
        void addOrganism(int orgRows, int orgCols, bool* organism);
        void addOrganism(int orgRows, int orgCols, char* organism);
        // Generates a random board with chance being the chance (percent as decimal) that a board state starts occupied (true)
        void randomBoard(double chance);
        // Performs a single step of the game of life counting the net number of tiles changed
        int step();
        // Creates a copy of the board overwriting the data - provides write protection
        void getBoardSafe(bool** data);
        // Provides direct access to the board through a copy of the pointer - gives user direct access to the board and is not write safe
        bool** getBoard();
        

        //---------- DEBUGGING UTILITIES ----------
        // Prints out the differences
        friend void diffPrint(ostream& os, const GameOfLife& obj1, const bool* expectedBoard);
        // Prints out as 0s and 1s to an ostream
        friend ostream& operator<<(ostream& os, const GameOfLife& obj);
    protected:
        // The number of rows in the game of life board
        int rows;
        // The number of columns in the game of life board
        int cols;
        // The board itself
        bool** board;

        //---------- PROTECTED UTILITIES ----------        
        // Deletes the board data
        void deleteBoard();
        // Sets all the data in the board to 0
        void resetBoard();
};

enum class GoLFitnessFunction {
    FinalStepTiles,
    AverageChangeTiles,
    CenterOfMassMotion
};

class GameOfLifeGA : public GeneticAlgorithm, public GameOfLife {
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        GameOfLifeGA();
        GameOfLifeGA(int sizePopulation, int sizeMembers, int numActions, char* actions, int crossovers, double mutationRate, int totalGens, int rows, int cols, GoLFitnessFunction fitnessFunc, int maxSteps, int orgRows, int orgCols);
        GameOfLifeGA(const GameOfLifeGA & other);
        GameOfLifeGA& operator=(const GameOfLifeGA & other);
        ~GameOfLifeGA();

        //---------- GENETIC ALGORITHM FUNCTIONS ----------
        // Fitness function for the genetic algorithm
        double fitness(int member);

        //---------- UTILITIES ----------
        // Creates an animation of the given member
        void animateMember(int member, int steps);
    private:
        // Fitness function being used
        GoLFitnessFunction fitnessFunc;
        // Maximum number of steps for the simulation
        int maxSteps;
        // Size of the organisms
        int orgRows;
        int orgCols;

        //---------- PRIVATE UTILITIES ----------
        // Calculates a fitness value for a member based on having the most tiles on at the final time step
        double fitnessMostTiles(int member);
        // Calculates a fitness value for a member based on having the largest average in tiles over the simulation
        double fitnessAverageChangeTiles(int member);
        // Calculates a fitness value for a member based on having the most motion of it's center of mass
        double fitnessCenterOfMassMotion(int member);
};

//---------- EXTERNAL FUNCTIONS ----------
void test_GameOfLife();

#endif