#include <filesystem>
#include <string>
#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
//-- BEGIN UNIX ONLY--
#include <unistd.h>
// Code that is not portable will be marked as much as possible
// unistd.h is a Unix only library
//-- END UNIX ONLY--

#include "sdl-basics.h"
#include "geneticsolver.h"
#include "gameoflife.h"

// TODO - fix test functions
// TODO - Test Pixel Grid Class

//---------- TESTING FUNCTION CONSTANTS ----------
// Default constants for the solver class
// Directory to save the output into
const std::string GOLS_OUTPUT_DIR = "";
// Population size for the genetic solver
const int GOLS_POP_SIZE = 100;
// Size of the organisms to put in the Game Of Life
const int GOLS_ORG_ROWS = 13;
const int GOLS_ORG_COLS = 13;
// Size of the simulation box
const int GOLS_SIM_ROWS = (GOLS_ORG_ROWS % 2) == 0 ? 2 * GOLS_ORG_ROWS : 2 * GOLS_ORG_ROWS + 1;
const int GOLS_SIM_COLS = (GOLS_ORG_COLS % 2) == 0 ? 2 * GOLS_ORG_COLS : 2 * GOLS_ORG_COLS + 1;
// Size, in pixels, of the sides of the squares in the grid
const int GOLS_RENDER_SIZE = 20;
// Color of the active tiles
const SDL_Color GOLS_BACKGROUND = {255, 255, 255, 255};
const SDL_Color GOLS_GRID_LINES = {150, 150, 150, 255};
const SDL_Color GOLS_ORG_COLOR = {255, 234, 0, 255};
// Background PNG name
const std::string GOLS_BACKGROUND_PNG = "background.png";
// Maximum frame rate
const int GOLS_FPS = 5;
const int GOLS_TICKS_PER_FRAME = 1000 / GOLS_FPS;

// Debugging function
// cerr << "DEBUG: " << __FILE__ << " line " << __LINE__ << "\n";

// Creating the video in ffmpeg
//ffmpeg -f image2 -framerate 5 -i frame%d.png -vcodec libx264 -crf 22 video.mp4

void renderOrganism(int orgRows, int orgCols, bool* org, int simRows, int simCols, int totFrames){
    //---------- GRAPHICAL REPRESENTATION ----------
    //---------- INITIALIZATION ----------
    // Calculate the
    int screenWidth = simCols * (GOLS_RENDER_SIZE + 1) - 1;
    int screenHeight = simRows * (GOLS_RENDER_SIZE + 1) - 1;

    // Initialize the window
    SDLWindowWrapper window = SDLWindowWrapper(screenWidth, screenHeight, "Game of Life Solver");
    SDL_Renderer* renderer = window.getRenderer();

    // Draw the background screen
    SDL_SetRenderDrawColor(renderer, GOLS_BACKGROUND);
    SDL_RenderClear(renderer);

    // Draw the grid lines
    SDL_SetRenderDrawColor(renderer, GOLS_GRID_LINES);
    // Horizontal Grid Lines
    for(int i = 0; i < simRows - 1; i++){
        SDL_RenderDrawLine(renderer,
            0, (GOLS_RENDER_SIZE + 1) * (i + 1) - 1,
            screenWidth, (GOLS_RENDER_SIZE + 1) * (i + 1) - 1
        );
    }

    // Vertical grid lines
    for(int i = 0; i < simCols - 1; i++){
        SDL_RenderDrawLine(renderer,
            (GOLS_RENDER_SIZE + 1) * (i + 1) - 1, 0,
            (GOLS_RENDER_SIZE + 1) * (i + 1) - 1, screenHeight
        );
    }

    // Save to a png
    window.saveImg(GOLS_OUTPUT_DIR + GOLS_BACKGROUND_PNG);

    // Load into the texture
    SDLTextureWrapper background = SDLTextureWrapper();
    background.loadFromFile(renderer, GOLS_OUTPUT_DIR + GOLS_BACKGROUND_PNG);

    //---------- SIMULATION ----------
    GameOfLife gameSim = GameOfLife(simRows, simCols);
    gameSim.addOrganism(orgRows, orgCols, org);
    
    // Loop for video of simulation
    // The board for visualization
    bool** board;

    // SDL_Rect for rendering the tiles
    SDL_Rect fillRect = {0, 0, GOLS_RENDER_SIZE, GOLS_RENDER_SIZE};

    // Events for SDL to process
    SDL_Event e;
    
    // Frame rate timer
    SDLTimer fpsTimer;

    // Count of the ticks in the frame
    int frameTicks;

    // Frame count for creating a set of pngs to make a video
    int frameCount = 0;

    // File name for frames
    std::string framePrefix = "./video/frame";
    std::string frameSuffix = ".png";
    std::string filename;

    // Continue until the window is exited out of
    bool quit = false;
    while(!quit && frameCount < totFrames){
        // Start the timer
        fpsTimer.start();

        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }
        
        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the background
        background.render(renderer, 0, 0);

        // Render the tiles
        board = gameSim.getBoard();
        SDL_SetRenderDrawColor(renderer, GOLS_ORG_COLOR);
        for(int i = 0; i < simRows; i++){
            fillRect.y = i * (GOLS_RENDER_SIZE + 1);
            for(int j = 0; j < simCols; j++){
                fillRect.x = j * (GOLS_RENDER_SIZE + 1);
                if(board[i][j]){
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }
        }

        // Save the frame
        filename = framePrefix + to_string(frameCount) + frameSuffix;
        window.saveImg(filename);

        // Flip the screen
        SDL_RenderPresent(renderer);

        // Timestep in the Game of Life sim
        gameSim.step();

        // Increment frame count
        frameCount++;

        // Enforce the frame rate cap
        frameTicks = fpsTimer.getTicks();
        if( frameTicks < GOLS_TICKS_PER_FRAME ){
            // Wait remaining time
            SDL_Delay( GOLS_TICKS_PER_FRAME - frameTicks );
        }
    }
}

//---------- ADDITIONAL TESTING FUNCTIONS ----------
void test_pulsar(){
    // The oscillating pulsar
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

    int simRows = 17;
    int simCols = 17;

    //---------- GRAPHICAL REPRESENTATION ----------
    //---------- INITIALIZATION ----------
    // Calculate the
    int screenWidth = simCols * (GOLS_RENDER_SIZE + 1) - 1;
    int screenHeight = simRows * (GOLS_RENDER_SIZE + 1) - 1;

    // Initialize the window
    SDLWindowWrapper window = SDLWindowWrapper(screenWidth, screenHeight, "Game of Life Solver");
    SDL_Renderer* renderer = window.getRenderer();

    // Draw the background screen
    SDL_SetRenderDrawColor(renderer, GOLS_BACKGROUND);
    SDL_RenderClear(renderer);

    // Draw the grid lines
    SDL_SetRenderDrawColor(renderer, GOLS_GRID_LINES);
    // Horizontal Grid Lines
    for(int i = 0; i < simRows - 1; i++){
        SDL_RenderDrawLine(renderer,
            0, (GOLS_RENDER_SIZE + 1) * (i + 1) - 1,
            screenWidth, (GOLS_RENDER_SIZE + 1) * (i + 1) - 1
        );
    }

    // Vertical grid lines
    for(int i = 0; i < simCols - 1; i++){
        SDL_RenderDrawLine(renderer,
            (GOLS_RENDER_SIZE + 1) * (i + 1) - 1, 0,
            (GOLS_RENDER_SIZE + 1) * (i + 1) - 1, screenHeight
        );
    }

    // Save to a png
    window.saveImg(GOLS_OUTPUT_DIR + GOLS_BACKGROUND_PNG);

    // Load into the texture
    SDLTextureWrapper background = SDLTextureWrapper();
    background.loadFromFile(renderer, GOLS_OUTPUT_DIR + GOLS_BACKGROUND_PNG);

    //---------- SIMULATION ----------
    GameOfLife gameSim = GameOfLife(simRows, simCols);
    gameSim.addOrganism(pulsarRows, pulsarCols, pulsar);
    
    // Loop for video of simulation
    // The board for visualization
    bool** board;

    // SDL_Rect for rendering the tiles
    SDL_Rect fillRect = {0, 0, GOLS_RENDER_SIZE, GOLS_RENDER_SIZE};

    // Events for SDL to process
    SDL_Event e;
    
    // Frame rate timer
    SDLTimer fpsTimer;

    // Count of the ticks in the frame
    int frameTicks;

    // Frame count for creating a set of pngs to make a video
    int frameCount = 0;

    // File name for frames
    std::string framePrefix = "./video/frame";
    std::string frameSuffix = ".png";
    std::string filename;

    // Continue until the window is exited out of
    bool quit = false;
    while(!quit && frameCount < 20){
        // Start the timer
        fpsTimer.start();

        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }
        
        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the background
        background.render(renderer, 0, 0);

        // Render the tiles
        board = gameSim.getBoard();
        SDL_SetRenderDrawColor(renderer, GOLS_ORG_COLOR);
        for(int i = 0; i < simRows; i++){
            fillRect.x = i * (GOLS_RENDER_SIZE + 1);
            for(int j = 0; j < simCols; j++){
                fillRect.y = j * (GOLS_RENDER_SIZE + 1);
                if(board[i][j]){
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }
        }

        // Save the frame
        filename = framePrefix + to_string(frameCount) + frameSuffix;
        window.saveImg(filename);

        // Flip the screen
        SDL_RenderPresent(renderer);

        // Timestep in the Game of Life sim
        gameSim.step();

        // Increment frame count
        frameCount++;

        // Enforce the frame rate cap
        frameTicks = fpsTimer.getTicks();
        if( frameTicks < GOLS_TICKS_PER_FRAME ){
            // Wait remaining time
            SDL_Delay( GOLS_TICKS_PER_FRAME - frameTicks );
        }
    }
}

void test_square(){
    // Square
    int squareRows = 4;
    int squareCols = 4;
    bool square[] = {
        1, 0, 0, 1,
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 0, 0, 1
    };
    int simRows = 4;
    int simCols = 4;
    int totFrames = 10;
    renderOrganism(squareRows, squareCols, square, simRows, simCols, totFrames);
}

void test_glider(){
    // Glider
    int gliderRows = 3;
    int gliderCols = 3;
    bool glider[] = {
        0, 0, 1,
        1, 0, 1,
        0, 1, 1
    };

    // Simulations size
    int simRows = 5;
    int simCols = 5;

    // Total number of frames for the video
    int totFrames = 40;

    // Simulate
    renderOrganism(gliderRows, gliderCols, glider, simRows, simCols, totFrames);
}

void test_drawPixelGrid_SingleFrame(){
    // Initialize test data
    int rows = 7;
    int cols = 7;
    bool** grid = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid[i] = new bool[7];
    }

    // Set to true and false
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if((i * cols + j) % 2 == 0){
                grid[i][j] = false;
            } else {
                grid[i][j] = true;
            }
        }
    }

    // Call drawPixelGrid
    std::string title = "Test: Still";
    drawPixelGrid(title, rows, cols, grid);

    // Clean up the grid
    for(int i = 0; i < rows; i++){
        delete[](grid[i]);
        grid[i] = nullptr;
    }
    delete[](grid);
}

void test_drawPixelGrid_Animated(){
    // Initialize test data
    int rows = 7;
    int cols = 7;
    
    // First Grid
    bool** grid0 = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid0[i] = new bool[7];
        for(int j = 0; j < cols; j++){
            if((i * cols + j) % 2 == 0){
                grid0[i][j] = false;
            } else {
                grid0[i][j] = true;
            }
        }
    }

    // Second Grid
    bool** grid1 = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid1[i] = new bool[7];
        for(int j = 0; j < cols; j++){
            grid1[i][j] = false;
        }
    }

    // Third Grid
    bool** grid2 = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid2[i] = new bool[7];
        for(int j = 0; j < cols; j++){
            if((i * cols + j) % 2 == 0){
                grid2[i][j] = true;
            } else {
                grid2[i][j] = false;
            }
        }
    }

    // Call drawPixelGrid
    std::string title = "Test: Animated";

    // Save file name stufff
    std::string* saveFilename = new std::string();;
    std::string prefix = "video/frame";
    std::string suffix = ".png";
    std::stringstream saveFilenameMaker; 
    saveFilenameMaker << prefix << 0 << suffix;
    *saveFilename = saveFilenameMaker.str();

    // Window
    SDLWindowWrapper* window = nullptr;

    // Frame rate
    int frameRate = 5;

    // Background file name
    std::string* backgroundFilename = new std::string();
    *backgroundFilename = "temp_background.png";
    
    // Make the first frame
    drawPixelGrid(title, rows, cols, grid0, saveFilename, window, frameRate, nullptr, backgroundFilename);

    if(window){
        cerr << "Pointer working as expected.";
    } else {
        cerr << "Not as expected - use a reference or change the spec.";
    }

    // Grab the renderer from the window
    SDL_Renderer* renderer = window->getRenderer();

    // Load the second background into a texture
    SDLTextureWrapper* backgroundTexture = new SDLTextureWrapper();
    backgroundTexture->loadFromFile(renderer, *backgroundFilename);

    // The current grid
    bool** currGrid;
    int modVal;

    // Loop for the video
    for(int i = 0; i < 10; i++){
        // Make a new file name
        saveFilenameMaker.str(std::string());
        saveFilenameMaker << prefix << i << suffix;
        *saveFilename = saveFilenameMaker.str();

        // Four frame cycle
        modVal = i % 4;
        if(modVal == 0){
            currGrid = grid1;
        } else if(modVal == 1){
            currGrid = grid2;
        } else if(modVal == 2){
            currGrid = grid1;
        } else{
            currGrid = grid0;
        }

        // Update the frames
        cerr << "DEBUG: " << __FILE__ << " line " << __LINE__ << "\n";
        drawPixelGrid(title, rows, cols, currGrid, saveFilename, window, frameRate, backgroundTexture, backgroundFilename);
    }
    // The frame rate might get a little wonky, because enough of this is done independent of all the main loop. Might be worth encapsulating it into it's own class.

    // Cleanup
    // Delete the grids
    for(int i = 0; i < rows; i++){
        delete[](grid0[i]);
        grid0[0] = nullptr;
        delete[](grid1[i]);
        grid1[0] = nullptr;
        delete[](grid2[i]);
        grid2[0] = nullptr;
    }
    delete[](grid0);
    delete[](grid1);
    delete[](grid2);
    
    // Delete the background PNG
    std::remove(backgroundFilename->c_str());

    // Delete the strings
    delete(backgroundFilename);
    delete(saveFilename);

    // Delete the SDL data
    delete(backgroundTexture);
    delete(window);    
}

void test_scrolling(){
    // Generate the data
    int rows = 10;
    int cols = 10;
    SDL_Color** pixelArt = new SDL_Color*[rows];
    for(int i = 0; i < rows; i++){
        pixelArt[i] = new SDL_Color[cols];
        for(int j = 0; j < cols; j++){
            pixelArt[i][j] = {(Uint8)(i * 25), (Uint8)(j * 25), 0, 255};
        }
    }

    // Scroll
    scrolling(rows, cols, pixelArt);
}

//---------- COMMAND LINE ARGUMENT FUNCTIONS ----------
// Prints the help menu
void printHelpMenu(){
    cerr << "game-of-life Help Menu:\n";
    cerr << "\t-h - print help menu\n";
    // Test code
    cerr << "\t-t # - testing mode with options:\n";
    cerr << "\t\t0 - test \"pulsar\" in basic domain\n";
    cerr << "\t\t1 - test basic animated organism - still life \"square\"\n";
    cerr << "\t\t2 - test basic animated organism - \"glider\"\n";
    cerr << "\t\t3 - test code for creating a single still image\n";
    cerr << "\t\t4 - test code that wraps the animation as an example of how to use it efficiently\n";
    cerr << "\t\t5 - test code for the GameOfLife class.\n";
    cerr << "\r\t6 - test scrolling pixel art code.\n";
    // Run code
    cerr << "\t-r # - experiment mode with options:\n";
    // TODO - update the help menu
}

// Processes the testing
void testOptions(int testFlag){
    switch(testFlag){
        case 0:
            test_pulsar();  // PASSED
            break;
        case 1:
            test_square();  // PASSED
            break;
        case 2:
            test_glider();  // PASSED
            break;
        case 3:
            test_drawPixelGrid_SingleFrame();   // PASSED
            break;
        case 4:
            test_drawPixelGrid_Animated();
            break;
        case 5:
            test_GameOfLife();  // PASSED
            break;
        case 6:
            test_scrolling();   // PASSED
            break;
        case 7:
            cerr << "Not implemented\n";
            break;
        default:
            cerr << "Invalid testing code. See help menu (-h)\n";
            break;
    }
}

// Run the code to generate the data desired
void runOptions(int runFlag){
    cerr << "Not implemented\n";
    // TODO
    switch(runFlag){
        case 0:
            break;
        case 1:
            break;
        default:
            cerr << "Invalid experiment code. See help menu (-h)\n";
            break;
    }
}

//---------- MAIN ----------
int main(int argc, char* argv[]){
    // Command line options
    const char* CMD_OPTIONS = "ht:r:";
    // The current option
    char opt;

    //-- BEGIN UNIX ONLY--
    //---------- PROCESS COMMAND LINE ARGUMENTS ----------
    while((opt = getopt(argc, argv, CMD_OPTIONS)) != -1){
        switch(opt){
            case 'h':
                printHelpMenu();
                exit(EXIT_SUCCESS);
                break;
            case 't':
                testOptions(atoi(optarg));
                exit(EXIT_SUCCESS);
                break;
            case 'r':
                runOptions(atoi(optarg));
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }
    //-- END UNIX ONLY--

    // Default behavior
    runOptions(0);

    return 0;
}