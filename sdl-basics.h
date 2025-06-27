#ifndef SDL_BASICS_H
#define SDL_BASICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>

// TODO - pass strings by reference
// TODO - deep copying
// TODO - shared pointers
// TODO - more sophisticated text handling
// TODO - create initialization flags to make sure the program doesn't try to double initialize the SDL subsystems, semaphore style model?

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
        SDLWindowWrapper(int width, int height, std::string title, bool useTTF = false, int fpsCap = DEFAULT_SCREEN_FPS);
        SDLWindowWrapper(const SDLWindowWrapper & other);
        SDLWindowWrapper& operator=(const SDLWindowWrapper & other);
        ~SDLWindowWrapper();

        //---------- UTILITIES ----------
        // Saves what's currently in the renderer
        // Note: must be called before SDL_RenderPresent otherwise renderer buffer will clear and there won't be any visual data to save
        void saveImg(std::string path);

        //---------- ACCESSORS ----------
        SDL_Renderer* getRenderer();
        int getWidth();
        int getHeight();
    private:
        // Width of the screen
        int screenWidth;
        // Height of the screen
        int screenHeight;
        // Pointer for the SDL_Window
        SDL_Window* window;
        // Renderer for the engine
        SDL_Renderer* renderer;
        // Boolean for whether or not the TTF subsystem is started
        bool useTTF;
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

#endif