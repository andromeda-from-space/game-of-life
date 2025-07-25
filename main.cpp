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
#include "cellularautomata.h"
#include "gameoflife.h"
#include "rng.h"

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
//std::cerr << "DEBUG: " << __FILE__ << " line " << __LINE__ << "\n";

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

void test_drawBoolGrid(){
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

    // Render the grid
    SDLPixelGridRenderer test = SDLPixelGridRenderer("Test Single Frame", rows, cols);
    test.drawBoolGrid(grid, false, "");

    // Clean up the grid
    for(int i = 0; i < rows; i++){
        delete[](grid[i]);
        grid[i] = nullptr;
    }
    delete[](grid);
}

void test_animateBoolGrid(){
    // Initialize test data
    int rows = 7;
    int cols = 7;
    int frames = 12;
    int frameRate = 5;
    
    // Make the grid
    bool*** grid = new bool**[frames];

    // First frame
    grid[0] = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid[0][i] = new bool[7];
        for(int j = 0; j < cols; j++){
            if((i * cols + j) % 2 == 0){
                grid[0][i][j] = false;
            } else {
                grid[0][i][j] = true;
            }
        }
    }

    // Second frame
    grid[1] = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid[1][i] = new bool[7];
        for(int j = 0; j < cols; j++){
            grid[1][i][j] = false;
        }
    }

    // Third frame
    grid[2] = new bool*[7];
    for(int i = 0; i < rows; i++){
        grid[2][i] = new bool[7];
        for(int j = 0; j < cols; j++){
            if((i * cols + j) % 2 == 0){
                grid[2][i][j] = true;
            } else {
                grid[2][i][j] = false;
            }
        }
    }

    // Fourth frame
    grid[3] = grid[1];

    // Repeat
    for(int k = 3; k < frames; k++){
        grid[k] = grid[k % 4]; 
    }
    
    // Animate
    SDLPixelGridRenderer test = SDLPixelGridRenderer("Test Animated", rows, cols);
    test.animateBoolGrid(grid, frames, frameRate, false, "");

    // Cleanup
    // Delete the grids
    for(int i = 0; i < rows; i++){
        delete[](grid[0][i]);
        grid[0][i] = nullptr;
        delete[](grid[1][i]);
        grid[1][i] = nullptr;
        delete[](grid[2][i]);
        grid[2][i] = nullptr;
    }

    delete[](grid[0]);
    grid[0] = nullptr;
    delete[](grid[1]);
    grid[1] = nullptr;
    delete[](grid[2]);
    grid[2] = nullptr;

    delete[](grid);
}

void test_scrollBoolGrid(){
    // Generate the data
    int rows = 10;
    int cols = 10;
    int frameCount = 20;
    int frameRate = 5;

    bool** pixelArt = new bool*[rows];
    for(int i = 0; i < rows; i++){
        pixelArt[i] = new bool[cols];
        for(int j = 0; j < cols; j++){
            pixelArt[i][j] = i >= j;
        }
    }

    // Scroll
    SDLPixelGridRenderer test = SDLPixelGridRenderer("Test Scrolling", rows, cols);
    test.scrollBoolGrid(pixelArt, frameCount, frameRate, ScrollDirection::LEFT, false, "");

    // Clean up
    for(int i = 0; i < rows; i++){
        delete[](pixelArt[i]);
        pixelArt[i] = nullptr;
    }
    delete[](pixelArt);
}

void test_scrollColorGrid(){
    // Generate the data
    int rows = 10;
    int cols = 10;
    int frameCount = 20;
    int frameRate = 5;

    SDL_Color** pixelArt = new SDL_Color*[rows];
    for(int i = 0; i < rows; i++){
        pixelArt[i] = new SDL_Color[cols];
        for(int j = 0; j < cols; j++){
            pixelArt[i][j] = {(Uint8)(i * 25), (Uint8)(j * 25), 0, 255};
        }
    }

    // Scroll
    SDLPixelGridRenderer test = SDLPixelGridRenderer("Test Single Frame", rows, cols, {0, 0, 0, 255});
    test.scrollColorGrid(pixelArt, frameCount, frameRate, ScrollDirection::LEFT, false, "");

    // Clean up
    for(int i = 0; i < rows; i++){
        delete[](pixelArt[i]);
        pixelArt[i] = nullptr;
    }
    delete[](pixelArt);
}

void test_cellularAutomata1D(){
    char rules[8] = {'1', '1', '1', '0', '0', '0', '0', '1'};
    CellularAutomata1D example = CellularAutomata1D(rules);

    // Initialize starting point
    int domainSize = 31;
    bool start[domainSize];
    for(int i = 0; i < domainSize; i ++){
        start[i] = true;
    }
    start[15] = false;
    int numSteps = 15;

    // Generate the snapshot
    example.snapShot(start, domainSize, numSteps);
}

void test_cellularAutomata1DGeneral(){
    // Initialize with rule 30 - see https://mathworld.wolfram.com/CellularAutomaton.html
    // Dark mode made the graphics weird
    char rules30[8] = {'1', '1', '1', '0', '0', '0', '0', '1'};
    CellularAutomata1DGeneral example = CellularAutomata1DGeneral(1, rules30);

    // Initialize starting point
    int domainSize = 63;
    bool start[domainSize];
    for(int i = 0; i < domainSize; i ++){
        start[i] = true;
    }
    start[32] = false;
    int numSteps = 31;

    // Generate the snapshot
    example.snapShot(start, domainSize, numSteps);

    // Repeat with a random ruleset
    example = CellularAutomata1DGeneral(2);
    example.snapShot(start, domainSize, numSteps);
}

void test_WrapInt(){
    int max = 7;
    WrapInt wrapInt = WrapInt(0, max);

    wrapInt.setVal(5);
    wrapInt += 5;
    cout << "Actual: " << wrapInt.getVal()  << " Expected: 3\n";
    
    wrapInt.setVal(3);
    wrapInt -= 5;
    cout << "Actual: " << wrapInt.getVal()<< " Expected: 5\n";
    
    wrapInt.setVal(3);
    wrapInt += max;
    wrapInt.wrapVal();
    cout << "Actual: " << wrapInt.getVal()<< " Expected: 3\n";

    wrapInt.setVal(5);
    wrapInt -= max;
    wrapInt.wrapVal();
    cout << "Actual: " << wrapInt.getVal() << " Expected: 5\n";

    wrapInt.setVal(5);
    wrapInt -= 2 * max;
    cout << "Actual: " << wrapInt.getVal() << " Expected: 5\n";
}

void test_majority(){
    // Problem constants
    int domainSize = 8;
    int numSteps = 5;
    
    // Rule
    char rules[8] = {'0', '1', '0', '1', '0', '1', '0', '1'};
    CellularAutomata1D example = CellularAutomata1D(rules);

    // Test with majority
    bool* start = new bool[domainSize];
    for(int i = 0; i < domainSize; i++){
        start[i] = rng::genRandDouble(0.0, 1.0) < 0.5;
    }
    bool* startCopy = new bool[domainSize];
    for(int i = 0; i < domainSize; i++){
        startCopy[i] = start[i];
    }

    // Run majority
    int result = example.majority(start, domainSize, numSteps);
    std::cout << "Result of applying rule: ";
    for(int i = 0; i < domainSize; i++){
        std::cout << start[i];
    }
    std::cout << "\n";
    std::cout << "Result from majority(): " << result << "\n";

    // Show snapshot
    example.snapShot(startCopy, domainSize, numSteps);

    // Try a different example
    for(int i = 0; i < 8; i++){
        rules[i] = '0';
    }
    example.setRules(rules);

    for(int i = 0; i < domainSize; i++){
        start[i] = rng::genRandDouble(0.0, 1.0) < 0.5;
    }
    for(int i = 0; i < domainSize; i++){
        startCopy[i] = start[i];
    }

    // Run majority
    result = example.majority(start, domainSize, numSteps);
    std::cout << "Result of applying rule: ";
    for(int i = 0; i < domainSize; i++){
        std::cout << start[i];
    }
    std::cout << "\n";
    std::cout << "Result from majority(): " << result << "\n";

    // Show snapshot
    example.snapShot(startCopy, domainSize, numSteps);

    // Cleanup
    delete[](start);
    delete[](startCopy);
}

void experiment0_CellularAutomata(){
    // Values for the genetic solver
    // The size of the population
    int sizePopulation = 100;
    // The number of crossovers
    int crossovers = 1;
    // The mutation rate
    double mutationRate = 0.1;
    // The number of attempts to try and get the majority
    int numFitnessTests = 5;
    // The domain size
    int domainSize = 32;
    // Maximum number of steps to attempt to blackout the whole domain
    int maxSteps = 100;
    // Number of generations to train
    int numGens = 1;

    // Create the solver
    MajoritySolverGA solver = MajoritySolverGA(sizePopulation, crossovers, mutationRate, numFitnessTests, domainSize, maxSteps);

    // Run the solver
    solver.train(numGens);

    // Get the most fit member
    int mostFit = solver.getMostFit(false);

    // See what it can make
    solver.visualizeMember(mostFit);
}

void experiment1_GameOfLife(){
    // TODO
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
    cerr << "\t\t3 - test pixel grid single frame code\n";
    cerr << "\t\t4 - test pixel grid animation code\n";
    cerr << "\t\t5 - test scrolling boolean pixel art code.\n";
    cerr << "\t\t6 - test scrolling color pixel art code.\n";
    cerr << "\t\t7 - test code for the GameOfLife class.\n";
    cerr << "\t\t8 - test the basic cellular automaton code.\n";
    cerr << "\t\t9 - test the generalized cellular automaton code.\n";
    cerr << "\t\t10 - test the WrapInt Class.\n";
    cerr << "\t\t11 - test the majority function in the basic cellular automaton.\n";
    // Run code
    cerr << "\t-r # - experiment mode with options:\n";
    cerr << "\t\t0 - trains cellular automata to solve the majority problem.\n";
    cerr << "\t\t1 - trains organisms using one of the various fitness functions.\n";
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
            test_drawBoolGrid(); // PASSED
            break;
        case 4:
            test_animateBoolGrid(); // PASSED
            break;
        case 5:
            test_scrollBoolGrid();  // PASSED
            break;
        case 6:
            test_scrollColorGrid(); // PASSED 
            break;
        case 7:
            test_GameOfLife();  // PASSED
            break;
        case 8:
            test_cellularAutomata1D();  // PASSED
            break;
        case 9:
            test_cellularAutomata1DGeneral(); // PASSED
            break;
        case 10:
            test_WrapInt(); // PASSED
            break;
        case 11:
            test_majority(); // PASSED
            break;
        default:
            cerr << "Invalid testing code. See help menu (-h)\n";
            break;
    }
}

// Run the code to generate the data desired
void runOptions(int runFlag){
    switch(runFlag){
        case 0:
            experiment0_CellularAutomata();
            break;
        case 1:
            experiment1_GameOfLife();
            break;
        default:
            cerr << "Invalid experiment code. See help menu (-h)\n";
            break;
    }
}

//---------- MAIN ----------
int main(int argc, char* argv[]){
    // See the rng
    rng::seedRNG();

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