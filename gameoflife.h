#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include "geneticsolver.h"

class GameOfLife{
    public:
        // Adds the organism to the board. Allows for both bool arrays (which match the data structure here) and char arrays which match the genetic algorithm code
        void addOrganism(int orgRows, int orgCols, bool* organism);
        void addOrganism(int orgRows, int orgCols, char* organism);
        // Generates a random board to see if anything interesting pops out
        // Performs a single step of the game of life counting the net number of tiles changed - possibly good for a fitness function
        int step();
    private:
        // The number of rows in the game of life board
        int rows;
        // The number of columns in the game of life board
        int cols;
        // The board itself
        bool** board;
        // Clears the board of all data
        void clearBoard();
};

class GameOfLifeGA : public GeneticAlgorithm {
    
};

#endif