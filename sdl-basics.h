#ifndef SDL_BASICS_H
#define SDL_BASICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>

// TODO - pass strings by const reference
// TODO - deep copying
// TODO - shared pointers
// TODO - more sophisticated text handling

// TODO - implement scrollColorGrid

// Default Screen Sizes
const int DEFAULT_SCREEN_WIDTH = 640;
const int DEFAULT_SCREEN_HEIGHT = 480;

// Frame rate cap variables
const int DEFAULT_SCREEN_FPS = 60;
const int DEFAULT_SCREEN_TICKS_PER_FRAME = 1000 / DEFAULT_SCREEN_FPS;

class SDLWindowWrapper {
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        SDLWindowWrapper();
        SDLWindowWrapper(int width, int height, std::string title, bool useTTF = false, bool useMixer = false, int fpsCap = DEFAULT_SCREEN_FPS);
        SDLWindowWrapper(const SDLWindowWrapper & other);
        SDLWindowWrapper& operator=(const SDLWindowWrapper & other);
        ~SDLWindowWrapper();

        //---------- UTILITIES ----------
        // Saves what's currently in the renderer
        // Note: must be called before SDL_RenderPresent otherwise renderer buffer will clear and there won't be any visual data to save
        void saveImg(std::string path);

        //---------- ACCESSORS ----------
        int getSDLInitCount();
        int getIMGInitCount();
        int getTTFInitCount();
        int getMixerInitCount();
        SDL_Renderer* getRenderer();
        int getWidth();
        int getHeight();
    private:
        //---------- STATIC PRIVATE MEMBERS ----------
        // Count of instances of the class using the SDL Library
        static int SDL_INIT_COUNT;
        // Count of instances of the class using the SDL_image subsystem
        static int IMG_INIT_COUNT;
        // Count of instances of the class using the SDL_ttf subsystem
        static int TTF_INIT_COUNT;
        // Count of the instances of the class using the SDL_mixer subsystem
        static int MIX_INIT_COUNT;

        //---------- PRIVATE MEMBERS ----------
        // Width of the screen
        int screenWidth;
        // Height of the screen
        int screenHeight;
        // Pointer for the SDL_Window
        SDL_Window* window;
        // Renderer for the engine
        SDL_Renderer* renderer;
        // Flag for whether or not the TTF subsystem is used
        bool useTTF;
        // Flag for whether or not the Mixer subsystem is used
        bool useMixer;
        // Frame rate cap
        int fpsCap;

        //---------- PRIVATE UTILITIES ----------
        // Initialize the SDL subsystems for use
        bool init(std::string title);
};

/*
SDLTextureWrapper

Intended as a support class for the SDLWindowWrapper Class. Performs very basic 2D texture rendering using the GPU accelerated rendering provided by SDL.
*/
class SDLTextureWrapper {
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        SDLTextureWrapper();
        SDLTextureWrapper(const SDLTextureWrapper& other);
        SDLTextureWrapper& operator=(const SDLTextureWrapper& other);
        ~SDLTextureWrapper();

        //---------- UTILITIES ----------
        // Loads an image from a file
        bool loadFromFile(SDL_Renderer* renderer, std::string path);
        // Deallocates the loaded image
        void free();
        // Renders the tecture at a given point with the provided renderer
        void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE );
        // Performs color modulation which multiplies the colors in the texture by the fraction val / 255
        void setColor(Uint8 red, Uint8 green, Uint8 blue);
        // Set the value of the alpha channel
        void setAlpha(Uint8 alpha);
        // Set the blend mode of the texture
        void setBlendMode(SDL_BlendMode blending);
        // Create a textuyre for some text
        bool createTextTexture(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color& color);
        // Render to this texture
        friend void renderToTexture(SDL_Renderer* renderer, SDLTextureWrapper& textureWrapper);

        //---------- ACCESSORS ----------
        int getWidth();
        int getHeight();
        
        //---------- MUTATORS ----------
    private:
        // Texture pointer
        SDL_Texture* texture;
        // Image size
        int width;
        int height;
};

/*
SDLTimer

Uses the SDL Library to create a timer that can be started stopped and paused. Since the SDL_GetTicks reads the time from the start of the program, this allows for the program to ignore time when paused.
*/
class SDLTimer {
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        SDLTimer();

        //---------- UTILITIES ----------
        // Start the timer - resets the timer
        void start();
        // Stop the timer - resest the timer in unstarted state
        void stop();
        // Pause the timer
        void pause();
        // Unpause the timer
        void unpause();
        // Gets the timer's time
        Uint32 getTicks();
        // Checks the status of the timer
        bool isStarted();
        bool isPaused();

    private:
        // The clock time when the timer started
        Uint32 startTicks;

        // The ticks stored when the timer was paused
        Uint32 pausedTicks;

        // Flag for the timer being paused
        bool paused;
        // Flag for the timer being started
        bool started;
};

// Some default graphical constants
// Pixel size width for the tiles
const int GRID_PIXEL_SIZE = 20;
// Color for the tiles that are labeled true
const SDL_Color GRID_FALSE_COLOR = {255, 255, 255, 255};
// Color for the grid line
const SDL_Color GRID_LINES_COLOR = {150, 150, 150, 255};
// Color for the tiles that are labeled false
const SDL_Color GRID_TRUE_COLOR = {255, 234, 0, 255};

enum class ScrollDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class SDLPixelGridRenderer {
    public:
        //---------- CONSTRUCTORS & DESTRUCTOR ----------
        SDLPixelGridRenderer();
        SDLPixelGridRenderer(std::string title, int rows, int cols);
        SDLPixelGridRenderer(const SDLPixelGridRenderer& other);
        SDLPixelGridRenderer& operator=(const SDLPixelGridRenderer& other);
        ~SDLPixelGridRenderer();
        

        //---------- UTILITIES ----------
        // Draws a still of the provided data
        void drawBoolGrid(bool** data, bool saveOutput, std::string path);
        // Animates provided data
        void animateBoolGrid(bool*** data, int frameCount, int frameRate, bool saveOutput, std::string path);
        // Scrolls the provided data in the direction provided
        void scrollBoolGrid(bool** data, int frameCount, int frameRate, ScrollDirection direction, bool saveOutput, std::string path);
        // Scrolls the provided data in the direction provided
        void scrollColorGrid(SDL_Color** data, int frameCount, int frameRate, ScrollDirection direction, bool saveOutput, std::string path);

        //---------- MUTATORS ----------
        void setFalseColor(SDL_Color newColor);
        void setGridLineColor(SDL_Color newColor);
        void setTrueColor(SDL_Color newColor);
    private:
        // Number of rows in the pixel grid
        int rows;
        // Number of columns in the pixel grid
        int cols;
        // Ticks per frame for the 
        int ticksPerFrame;
        // The window to render in
        SDLWindowWrapper* window;
        // The renderer
        SDL_Renderer* renderer;
        // Timer for the fps
        SDLTimer fpsTimer;
        // Texture to hold the background image
        SDLTextureWrapper background;
        // "Pixel" size
        int pixelSize;
        // Color of "false" in the boolean data
        SDL_Color falseColor;
        // Color of the gridlines
        SDL_Color gridLineColor;
        // Color of "true" in the boolean data
        SDL_Color trueColor;

        //---------- PRIVATE UTILITIES ----------
        void drawBackground();
};

//---------- EXTERNAL FUNCTIONS ----------
// Overload SDL_SetRenderDrawColor so that it just takes an SDL_Color
int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, const SDL_Color& color);
int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, SDL_Color& color);

#endif