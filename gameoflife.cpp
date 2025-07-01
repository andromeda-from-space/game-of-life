#include <iostream>
#include <sstream>

#include "rng.h"
#include "gameoflife.h"
#include "sdl-basics.h"

//-------------------------------------------------------------------------------------
//---------- GameOfLife ---------------------------------------------------------------
//-------------------------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
GameOfLife::GameOfLife() : rows(GAME_OF_LIFE_DEFAULT_ROWS), cols(GAME_OF_LIFE_DEFAULT_COLS), board(nullptr) {
    rng::seedRNG();
    resetBoard();
}

GameOfLife::GameOfLife(int rows, int cols) : rows(rows), cols(cols), board(nullptr)  {
    rng::seedRNG();
    resetBoard();
}

GameOfLife::GameOfLife(const GameOfLife & other) : rows(other.rows), cols(other.cols), board(nullptr)  {
    // TODO
}

GameOfLife& GameOfLife::operator=(const GameOfLife & other){
    // TODO
    return *this;
}

GameOfLife::~GameOfLife(){
    deleteBoard();
}

//---------- UTILITIES ----------
void GameOfLife::addOrganism(int orgRows, int orgCols, bool* organism){
    // Reset the board
    resetBoard();

    // Copy the organism into roughly the center of the board
    int rowPad = (rows - orgRows) / 2;
    int colPad = (cols - orgCols) / 2;
    int index = 0;
    for(int i = 0; i < orgRows; i++){
        for(int j = 0; j < orgCols; j++){
            board[rowPad + i][colPad + j] = organism[index];
            index++;
        }
    }
}

void GameOfLife::addOrganism(int orgRows, int orgCols, char* organism){
    // Reset the board
    resetBoard();

    // Copy the organism into roughly the center of the board
    int rowPad = (rows - orgRows) / 2;
    int colPad = (cols - orgCols) / 2;
    int index = 0;
    for(int i = 0; i < orgRows; i++){
        for(int j = 0; j < orgCols; j++){
            board[rowPad + i][colPad + j] = (bool) organism[index];
            index++;
        }
    }
}

void GameOfLife::randomBoard(double chance){
    // Roll for determining if a square is on
    double roll;
    
    // Allocate memory for the board if necessary
    if(!board){
        board = new bool*[rows];
        for(int i = 0; i < rows; i++){
            board[i] = new bool[cols];
        }
    }

    // Fill out the board
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            roll = rng::genRandDouble(0., 1.0);
            if(roll < chance){
                board[i][j] = true;
            }
        }
    }
}

int GameOfLife::step(){
    // Count of neighbors
    int count;
    // Board generated this step
    bool** nextBoard = new bool*[rows];
    for(int i = 0; i < rows; i++){
        nextBoard[i] = new bool[cols];
        for(int j = 0; j < cols; j++){
            nextBoard[i][j] = false;
        }
    }

    // Apply rules of Conway's Game of Life with wrapping
    // Upper Left Corner
    count = 0;
    count += board[rows - 1][cols - 1]; // Upper Left
    count += board[rows - 1][0];        // Upper Middle
    count += board[rows - 1][1];        // Upper Right
    count += board[0][cols - 1];        // Left
    count += board[0][1];               // Right
    count += board[1][cols - 1];        // Lower Left
    count += board[1][0];               // Lower Middle
    count += board[1][1];               // Lower Right

    // Apply rules based on neighbor count
    if(count == 2 && board[0][0]){
        nextBoard[0][0] = true;
    } else if(count == 3){
        nextBoard[0][0] = true;
    }

    
    // Top Row Middle
    for(int j = 1; j < cols - 1; j++){
        count = 0;
        count += board[rows - 1][j - 1];    // Upper Left
        count += board[rows - 1][j];        // Upper Middle
        count += board[rows - 1][j + 1];    // Upper Right
        count += board[0][j - 1];           // Left
        count += board[0][j + 1];           // Right
        count += board[1][j - 1];           // Lower Left
        count += board[1][j];               // Lower Middle
        count += board[1][j + 1];           // Lower Right

        // Apply rules based on neighbor count
        if(count == 2 && board[0][j]){
            nextBoard[0][j] = true;
        } else if(count == 3){
            nextBoard[0][j] = true;
        }
    }

    // Upper Right Corner
    count = 0;
    count += board[rows - 1][cols - 2]; // Upper Left
    count += board[rows - 1][cols - 1]; // Upper Middle
    count += board[rows - 1][0];        // Upper Right
    count += board[0][cols - 2];        // Left
    count += board[0][0];               // Right
    count += board[1][cols - 2];        // Lower Left
    count += board[1][cols - 1];        // Lower Middle
    count += board[1][0];               // Lower Right

    // Apply rules based on neighbor count
    if(count == 2 && board[0][rows - 1]){
        nextBoard[0][cols - 1] = true;
    } else if(count == 3){
        nextBoard[0][cols - 1] = true;
    }

    // Between first and last row
    for(int i = 1; i < rows - 1; i++){
        // Left column
        count = 0;
        count += board[i - 1][cols - 1];    // Upper Left
        count += board[i - 1][0];           // Upper Middle
        count += board[i - 1][1];           // Upper Right
        count += board[i][cols - 1];        // Left
        count += board[i][1];               // Right
        count += board[i + 1][cols - 1];    // Lower Left
        count += board[i + 1][0];           // Lower Middle
        count += board[i + 1][1];           // Lower Right

        // Apply rules based on neighbor count
        if(count == 2 && board[i][0]){
            nextBoard[i][0] = true;
        } else if(count == 3){
            nextBoard[i][0] = true;
        }

        // Internals
        for(int j = 1; j < cols - 1; j++){
            count = 0;
            count += board[i - 1][j - 1];   // Upper Left
            count += board[i - 1][j];       // Upper Middle
            count += board[i - 1][j + 1];   // Upper Right
            count += board[i][j - 1];       // Left
            count += board[i][j + 1];       // Right
            count += board[i + 1][j - 1];   // Lower Left
            count += board[i + 1][j];       // Lower Middle
            count += board[i + 1][j + 1];   // Lower Right

            // Apply rules based on neighbor count
            if(count == 2 && board[i][j]){
                nextBoard[i][j] = true;
            } else if(count == 3){
                nextBoard[i][j] = true;
            }
        }

        // Right Column
        count = 0;
        count += board[i - 1][cols - 2];    // Upper Left
        count += board[i - 1][cols - 1];    // Upper Middle
        count += board[i - 1][0];           // Upper Right
        count += board[i][cols - 2];        // Left
        count += board[i][0];               // Right
        count += board[i + 1][cols - 2];    // Lower Left
        count += board[i + 1][cols - 1];    // Lower Middle
        count += board[i + 1][0];           // Lower Right

        // Apply rules based on neighbor count
        if(count == 2 && board[i][cols - 1]){
            nextBoard[i][cols - 1] = true;
        } else if(count == 3){
            nextBoard[i][cols - 1] = true;
        }
    }

    // Lower Left Corner
    count = 0;
    count += board[rows - 2][cols - 1]; // Upper Left
    count += board[rows - 2][0];        // Upper Middle
    count += board[rows - 2][1];        // Upper Right
    count += board[rows - 1][cols - 1]; // Left
    count += board[rows - 1][1];        // Right
    count += board[0][cols - 1];        // Lower Left
    count += board[0][0];               // Lower Middle
    count += board[0][1];               // Lower Right

    // Apply rules based on neighbor count
    if(count == 2 && board[rows - 1][0]){
        nextBoard[rows - 1][0] = true;
    } else if(count == 3){
        nextBoard[rows - 1][0] = true;
    }

    // Bottom Row Middle
    for(int j = 1; j < cols - 1; j++){
        count = 0;
        count += board[rows - 2][j - 1];    // Upper Left
        count += board[rows - 2][j];        // Upper Middle
        count += board[rows - 2][j + 1];    // Upper Right
        count += board[rows - 1][j - 1];    // Left
        count += board[rows - 1][j + 1];    // Right
        count += board[0][j - 1];           // Lower Left
        count += board[0][j];               // Lower Middle
        count += board[0][j + 1];           // Lower Right

        // Apply rules based on neighbor count
        if(count == 2 && board[rows - 1][j]){
            nextBoard[rows - 1][j] = true;
        } else if(count == 3){
            nextBoard[rows - 1][j] = true;
        }
    }

    // Lower Right Corner
    count = 0;
    count += board[rows - 2][cols - 2]; // Upper Left
    count += board[rows - 2][cols - 1]; // Upper Middle
    count += board[rows - 2][0];        // Upper Right
    count += board[rows - 1][cols - 2]; // Left
    count += board[rows - 1][0];        // Right
    count += board[0][cols - 2];        // Lower Left
    count += board[0][cols - 1];        // Lower Middle
    count += board[0][0];               // Lower Right

    // Apply rules based on neighbor count
    if(count == 2 && board[rows - 1][cols - 1]){
        nextBoard[rows - 1][cols - 1] = true;
    } else if(count == 3){
        nextBoard[rows - 1][cols - 1] = true;
    }

    // Count the number of changes
    count = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            count += board[i][j] != nextBoard[i][j];
        }
    }

    // Pointer shuffle
    deleteBoard();
    board = nextBoard;

    return count;
}

void GameOfLife::getBoardSafe(bool** data){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            data[i][j] = board[i][j];
        }
    }
}

bool** GameOfLife::getBoard(){
    return board;
}

//---------- DEBUGGING UTILITIES ----------
void diffPrint(ostream& os, const GameOfLife& obj1, const bool* expectedBoard){
    int index = 0;
    for(int i = 0; i < obj1.rows; i++){
        for(int j = 0; j < obj1.cols; j++){
            if(obj1.board[i][j] != expectedBoard[index]){
                os << "X";
            } else {
                os << "O";
            }
            index++;
        }
        os << "\n";
    }
}

ostream& operator<<(ostream& os, const GameOfLife& obj){
    for(int i = 0; i < obj.rows; i++){
        for(int j = 0; j < obj.cols; j++){
            if(obj.board[i][j]){
                os << "1";
            } else {
                os << "0";
            }
        }
        os << "\n";
    }
    return os;
}

//---------- PRIVATE UTILITIES ----------
void GameOfLife::resetBoard(){
    // Check if board has been initialized
    if(!board){
        // Allocate memory if needed
        board = new bool*[rows];
        for(int i = 0; i < rows; i++){
            board[i] = new bool[cols];
            for(int j = 0; j < cols; j++){
                board[i][j] = false;
            }
        }
    } else {
        // Otherwise set all to zero
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                board[i][j] = false;
            }
        }
    }
}

void GameOfLife::deleteBoard(){
    // Check for nullptr and clean up the memory
    if(board){
        for(int i = 0; i < rows; i++){
            delete[](board[i]);
            board[i] = nullptr;
        }
        delete[](board);
        board = nullptr;
    }
}

//-------------------------------------------------------------------------------------
//---------- GameOfLifeGA -------------------------------------------------------------
//-------------------------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
GameOfLifeGA::GameOfLifeGA() : GeneticAlgorithm(), GameOfLife(), fitnessFunc(GoLFitnessFunction::FinalStepTiles), maxSteps(-1), orgRows(-1), orgCols(-1) {}

GameOfLifeGA::GameOfLifeGA(int sizePopulation, int sizeMembers, int numActions, char* actions, int crossovers, double mutationRate, int totalGens, int rows, int cols, GoLFitnessFunction fitnessFunc, int maxSteps, int orgRows, int orgCols) : GeneticAlgorithm(sizePopulation, sizeMembers, numActions, actions,crossovers, mutationRate, totalGens), GameOfLife(rows, cols), fitnessFunc(fitnessFunc), maxSteps(maxSteps), orgRows(orgRows), orgCols(orgCols){}

GameOfLifeGA::GameOfLifeGA(const GameOfLifeGA & other) : GeneticAlgorithm(other), GameOfLife(other), fitnessFunc(other.fitnessFunc), maxSteps(other.maxSteps), orgRows(other.orgRows), orgCols(other.orgCols) {}

GameOfLifeGA& GameOfLifeGA::operator=(const GameOfLifeGA & other){
    // Check for self-assignment
    if(this != &other){
        // Perform operator= operations on base class components
        this->GeneticAlgorithm::operator=(other);
        this->GameOfLife::operator=(other);

        // Do other assignments
        fitnessFunc = other.fitnessFunc;
        maxSteps = other.maxSteps;
        orgRows = other.orgRows;
        orgCols = other.orgCols;
    }
    return *this;
}

GameOfLifeGA::~GameOfLifeGA(){}

//---------- GENETIC ALGORITHM FUNCTIONS ----------
double GameOfLifeGA::fitness(int member){
    if(fitnessFunc == GoLFitnessFunction::FinalStepTiles){
        return fitnessMostTiles(member);
    } else if(fitnessFunc == GoLFitnessFunction::AverageChangeTiles){
        return fitnessAverageChangeTiles(member);
    } else if(fitnessFunc == GoLFitnessFunction::CenterOfMassMotion){
        return fitnessCenterOfMassMotion(member);
    } else {
        std::cerr << "Error: invalid fitness function.\n";
        return 0.0;
    }
}

//---------- UTILITIES ----------
void GameOfLifeGA::animateMember(int member, int steps){
    // Reset the board
    resetBoard();

    // Add the organism
    addOrganism(orgRows, orgCols, population[member]);

    // Generate the frames
    bool*** frameData = new bool**[steps + 1];
    frameData[0] = new bool*[rows];
    for(int i = 0; i < rows; i++){
        frameData[0][i] = new bool[cols];
        for(int j = 0; j < cols; j++){
            frameData[0][i][j] = board[i][j];
        }
    }

    for(int k = 0; k < steps; k++){
        step();
        getBoardSafe(frameData[k + 1]); 
    }

    // Animate
    std::stringstream sstream;
    sstream << "Game of Life GA, member = " << member;
    std::string title = sstream.str();
    SDLPixelGridRenderer animation = SDLPixelGridRenderer(title, rows, cols);
    animation.animateBoolGrid(frameData, steps + 1, 5, false, "");
}

//---------- PRIVATE UTILITIES ----------
double GameOfLifeGA::fitnessMostTiles(int member){
    // Reset the board
    resetBoard();

    // Add the organism in
    addOrganism(orgRows, orgCols, population[member]);

    // Step the game forward
    for(int i = 0; i < maxSteps; i++){
        step();
    }

    // Count all the tiles that are on
    double fitness = 0.0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(board[i][j]){
                fitness += 1.0;
            }
        }
    }

    return fitness;
}

double GameOfLifeGA::fitnessAverageChangeTiles(int member){
    // Reset the board
    resetBoard();

    // Add the organism in
    addOrganism(orgRows, orgCols, population[member]);

    // Step the game forward
    double fitness = 0.0;
    for(int i = 0; i < maxSteps; i++){
        fitness += step();
    }
    return fitness / ((double) maxSteps);
}

double GameOfLifeGA::fitnessCenterOfMassMotion(int member){
    // Reset the board
    resetBoard();

    // Add the organism in
    addOrganism(orgRows, orgCols, population[member]);

    // Calculate the center of mass
    double numerXCoM;
    double numerYCoM;
    double denomCoM;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(board[i][j]){
                numerYCoM += (i + 0.5);
                numerXCoM += (j + 0.5);
                denomCoM += 1.0;
            }
        }
    }
    double oldXCoM = numerXCoM / denomCoM;
    double oldYCoM = numerYCoM / denomCoM;
    double newXCoM = 0.0;
    double newYCoM = 0.0;
    double delX;
    double delY;

    // Step the game forward
    double fitness = 0.0;
    for(int k = 0; k < maxSteps; k++){
        // Perform step
        step();

        // Calculate the new center of mass
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                if(board[i][j]){
                    numerYCoM += (i + 0.5);
                    numerXCoM += (j + 0.5);
                    denomCoM += 1.0;
                }
            }
        }
        newXCoM = numerXCoM / denomCoM;
        newYCoM = numerYCoM / denomCoM;
        
        // Calculate the differences
        delX = newXCoM - oldXCoM;
        delY = newYCoM - oldYCoM;

        // Update fitness
        fitness += sqrt(delX * delX + delY * delY);

        // Cycle new to old
        oldXCoM = newXCoM;
        oldYCoM = newYCoM;
    }
    return fitness / ((double) maxSteps);
}

//---------- EXTERNAL FUNCTIONS ----------
void test_GameOfLife(){
    // Start a Game of Life with a random board
    GameOfLife game = GameOfLife(17, 17);
    
    
    // Some example organisms
    // Still lifes
    int blockRows = 2;
    int blockCols = 2;
    bool block[] = {
        1, 1,
        1, 1
    };

    // Add the organism and simulate
    game.addOrganism(blockRows, blockCols, block);
    cout << "---------------\n";
    cout << game;
    for(int i = 0; i < 5; i++){
        game.step();
        cout << "---------------\n";
        cout << game;
    }
    cout << "---------------\n";
    
    // Oscillators
    int pulsarRows = 13;
    int pulsarCols = 13;
    bool pulsar[] = {
        0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
        0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0
    };

    // Add the organism and simulate
    game.addOrganism(pulsarRows, pulsarCols, pulsar);
    cout << "---------------\n";
    cout << game;
    for(int i = 0; i < 5; i++){
        game.step();
        cout << "---------------\n";
        cout << game;
    }
    cout << "---------------\n";
    
    // Spaceships
    int gliderRows = 3;
    int gliderCols = 3;
    bool glider[] = {
        0, 0, 1,
        1, 0, 1,
        0, 1, 1
    };

    // Add the organism and simulate
    game.addOrganism(gliderRows, gliderCols, glider);
    cout << "---------------\n";
    cout << game;
    for(int i = 0; i < 6; i++){
        game.step();
        cout << "---------------\n";
        cout << game;
    }
    cout << "---------------\n";
}