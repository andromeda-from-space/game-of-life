#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <sstream>

#include "sdl-basics.h"

//--------------------------------------------------------------------
//---------- SDLWindowWrapper ----------------------------------------
//--------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
SDLWindowWrapper::SDLWindowWrapper() : screenWidth(DEFAULT_SCREEN_WIDTH), screenHeight(DEFAULT_SCREEN_HEIGHT), window(nullptr), renderer(nullptr), useTTF(true), fpsCap(DEFAULT_SCREEN_TICKS_PER_FRAME) {
    init("SDL Window");
}

SDLWindowWrapper::SDLWindowWrapper(int width, int height, std::string title, bool useTTF, int fpsCap) : screenWidth(width), screenHeight(height), window(nullptr), renderer(nullptr), useTTF(useTTF), fpsCap(fpsCap) {
    init(title);
}

SDLWindowWrapper::SDLWindowWrapper(const SDLWindowWrapper & other) : screenWidth(other.screenWidth), screenHeight(other.screenHeight), window(nullptr), renderer(nullptr), useTTF(other.useTTF), fpsCap(other.fpsCap){
    std::stringstream temp;
    temp << SDL_GetWindowTitle(other.window) << " - new";
    init(temp.str());
}

SDLWindowWrapper& SDLWindowWrapper::operator=(const SDLWindowWrapper & other){
    // TODO
    return *this;
}

SDLWindowWrapper::~SDLWindowWrapper(){
    // Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;

    // Quit SDL Subsystems
    IMG_Quit();
    SDL_Quit();
    if(useTTF){
        TTF_Quit();
    }
}

//---------- UTILITIES ----------
void SDLWindowWrapper::saveImg(std::string path){
    // Get the window surface
    SDL_Surface* currSurface = SDL_GetWindowSurface(window);
    // Save
    IMG_SavePNG(currSurface, path.c_str());
}

//---------- PRIVATE UTILITIES ----------
bool SDLWindowWrapper::init(std::string title){
    // TODO - Make code more compact - early returns make else blocks unnecessary
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        // TODO - exceptions?
        return false;
    } else {
        // Create the window
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        if(!window){
            fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
            // TODO - exceptions?
            return false;
        } else {
            // Create renderer for window
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            // Turn off vsync for Lesson 25
            //renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
            if(!renderer)
            {
                fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                return false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

                // Initialize the SDL_image library
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)){
                    fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    // TODO - exceptions?
                    return false;
                } else {
                    if(useTTF && TTF_Init() == -1){
                        fprintf(stderr, "SDL_TTF could not initialize ! TTF_Error: %s\n", TTF_GetError());
                        // TODO - exceptions?
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

//--------------------------------------------------------------------
//---------- SDLTextureWrapper ---------------------------------------
//--------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
SDLTextureWrapper::SDLTextureWrapper() : texture(nullptr), width(-1), height(-1){}

SDLTextureWrapper::SDLTextureWrapper(const SDLTextureWrapper& other) : texture(nullptr), width(-1), height(-1){
    // TODO
}

SDLTextureWrapper& SDLTextureWrapper::operator=(const SDLTextureWrapper& other) {
    // TODO
    return *this;
}

SDLTextureWrapper::~SDLTextureWrapper(){
    free();
}

//---------- UTILITIES ----------
bool SDLTextureWrapper::loadFromFile(SDL_Renderer* renderer, std::string path){
    // Remove anything that has already been loaded in
    free();

    // Flag for loading success
    bool success = true;

    // Load image at specified path as a surface
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(!loadedSurface){
        fprintf(stderr, "Unable to load image %s! SDL Error: %s\n", path.c_str(), IMG_GetError());
        // TODO - exception handling
        success = false;
    } else {
        // Set the color key
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        // Convert the loaded image to into a texture
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(!texture){
            fprintf(stderr, "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            // TODO - exception handling
            success = false;
        } else {
            // Get the image dimensions
            width = loadedSurface->w;
            height = loadedSurface->h;
        }
        SDL_FreeSurface( loadedSurface );
    }

    return success;
}

void SDLTextureWrapper::free(){
    if(texture){
        // Destroy the texture
        SDL_DestroyTexture(texture);
        texture = nullptr;

        // Set the width and the height to nonsense values
        width = -1;
        height = -1;
    }
}

void SDLTextureWrapper::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
    // Render the image into the correct location
    SDL_Rect renderQuad = {x, y, width, height};

    // Use the clipping rectangle to extract the sprite
    if(clip){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to the screen
    SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip );
}

void SDLTextureWrapper::setColor(Uint8 red, Uint8 green, Uint8 blue){
    SDL_SetTextureColorMod(texture, red, green, blue);
}

void SDLTextureWrapper::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(texture, alpha);
}

void SDLTextureWrapper::setBlendMode(SDL_BlendMode blending){
    SDL_SetTextureBlendMode(texture, blending);
}

bool SDLTextureWrapper::createTextTexture(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color& color){
    // Flag for if creating the texture was successful
    bool success = true;

    // Delete old texture
    free();

    // Create a surface from the text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    
    // Pull out it's dimensions
    width = textSurface->w;
    height = textSurface->h;

    // Convert Surface to a texture
    if(!textSurface){
        fprintf(stderr, "Unable to render text to surface! SDL_TTF Error: %s\n", TTF_GetError());
        success = false;
    } else {
        // Create the texture
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(!texture){
            fprintf(stderr, "Unable to render texture from surface! SDL Error: %s\n", SDL_GetError());
            // TODO - Exceptions?
            success = false;
        }
        
        // Clean up
        SDL_FreeSurface(textSurface);
    }

    return success;
}

//---------- ACCESSORS ----------
int SDLTextureWrapper::getWidth(){
    return width;
}

int SDLTextureWrapper::getHeight(){
    return height;
}

//--------------------------------------------------------------------
//---------- SDLTimer ------------------------------------------------
//--------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
SDLTimer::SDLTimer() : startTicks(0), pausedTicks(0), paused(false), started(false) {}

//---------- UTILITIES ----------
void SDLTimer::start(){
    started = true;
    startTicks = SDL_GetTicks();

    paused = false;
    pausedTicks = 0;
}

void SDLTimer::stop(){
    started = false;
    startTicks = 0;
    
    paused = false;
    pausedTicks = 0;
}

void SDLTimer::pause(){
    if(started && !paused){
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
    
}

void SDLTimer::unpause(){
    if(started && paused){
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

Uint32 SDLTimer::getTicks(){
    // Return value
    Uint32 time = 0;

    if(started){
        if(paused){
            // Return the value of when the timer was paused
            time = pausedTicks;
        } else {
            // Otherwise return the difference in time between now and when the timer was started
            time = SDL_GetTicks() - startTicks;
        }
    }

    return time;
}

bool SDLTimer::isStarted(){
    return started;
}

bool SDLTimer::isPaused(){
    return paused;
}