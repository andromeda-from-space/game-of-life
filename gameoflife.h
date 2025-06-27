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
        // Performs a single step of the game of life counting the net number of tiles changed - possibly good for a fitness function
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
    private:
        // The number of rows in the game of life board
        int rows;
        // The number of columns in the game of life board
        int cols;
        // The board itself
        bool** board;

        //---------- PRIVATE UTILITIES ----------
        // Sets all the data in the board to 0
        void resetBoard();
        // Deletes the board data
        void deleteBoard();
};

class GameOfLifeGA : public GeneticAlgorithm {
    
};

void test_GameOfLife();

#endif