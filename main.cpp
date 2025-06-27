#include <filesystem>
#include <string>
#include <SDL2/SDL.h>

#include "sdl-basics.h"
#include "geneticsolver.h"
#include "gameoflife.h"

// Debugging function
//fprintf(stderr, "DEBUG: %s line %d\n", __FILE__, __LINE__);

// Creating the video in ffmpeg
//ffmpeg -f image2 -framerate 5 -i frame%d.png -vcodec libx264 -crf 22 video.mp4


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

// Wrapper function to make SetRenderDrawColor a little less annoying
int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, const SDL_Color& color){
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void debug_pulsarVisualization(){
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

int main(){
    /*
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
    */

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

    return 0;
}