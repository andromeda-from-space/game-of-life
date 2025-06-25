#include "gameoflife.h"

void GameOfLife::addOrganism(int orgRows, int orgCols, bool* organism){
    // Copy the organism into roughly the center of the board
    int rowPad = (rows - orgRows) / 2;
    int colPad = (cols - orgCols) / 2;
    int index = 0;
    for(int i = 0; i < orgRows; i++){
        for(int j = 0; j <orgCols; j++){
            board[rowPad + i][colPad + j] = organism[index];
            index++;
        }
    }
}

void GameOfLife::addOrganism(int orgRows, int orgCols, char* organism){
    // Copy the organism into roughly the center of the board
    int rowPad = (rows - orgRows) / 2;
    int colPad = (cols - orgCols) / 2;
    int index = 0;
    for(int i = 0; i < orgRows; i++){
        for(int j = 0; j <orgCols; j++){
            board[rowPad + i][colPad + j] = (bool) organism[index];
            index++;
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
        if(count == 2 && board[0][0]){
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
    if(count == 2 && board[0][0]){
        nextBoard[0][cols - 1] = true;
    } else if(count == 3){
        nextBoard[0][cols - 1] = true;
    }

    // Internals
    for(int i = 1; i < rows - 1; i++){
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
                if(count == 2 && board[0][0]){
                    nextBoard[i][j] = true;
                } else if(count == 3){
                    nextBoard[i][j] = true;
                }
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
    if(count == 2 && board[0][0]){
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
        if(count == 2 && board[0][0]){
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
    if(count == 2 && board[0][0]){
        nextBoard[rows - 1][0] = true;
    } else if(count == 3){
        nextBoard[rows - 1][0] = true;
    }

    // Count the number of changes
    count = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            count += board[i][j] != nextBoard[i][j];
        }
    }

    // Pointer shuffle
    clearBoard();
    board = nextBoard;

    return count;
}

void GameOfLife::clearBoard(){
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