#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <sstream>
#include <iostream>

#include "sdl-basics.h"

//--------------------------------------------------------------------
//---------- SDLWindowWrapper ----------------------------------------
//--------------------------------------------------------------------
// Initialize the static memory
int SDLWindowWrapper::SDL_INIT_COUNT = 0;
int SDLWindowWrapper::IMG_INIT_COUNT = 0;
int SDLWindowWrapper::TTF_INIT_COUNT = 0;
int SDLWindowWrapper::MIX_INIT_COUNT = 0;

//---------- CONSTRUCTORS & DESTRUCTOR ----------
SDLWindowWrapper::SDLWindowWrapper() : screenWidth(DEFAULT_SCREEN_WIDTH), screenHeight(DEFAULT_SCREEN_HEIGHT), window(nullptr), renderer(nullptr), useTTF(true), useMixer(false), fpsCap(DEFAULT_SCREEN_TICKS_PER_FRAME) {
    init("SDL Window");
}

SDLWindowWrapper::SDLWindowWrapper(int width, int height, std::string title, bool useTTF, bool useMixer, int fpsCap) : screenWidth(width), screenHeight(height), window(nullptr), renderer(nullptr), useTTF(useTTF), useMixer(useMixer), fpsCap(fpsCap) {
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
    SDL_INIT_COUNT--;
    if(SDL_INIT_COUNT == 0){
        SDL_Quit();
    }
    
    IMG_INIT_COUNT--;
    if(IMG_INIT_COUNT == 0){
        IMG_Quit();
    }

    if(useTTF){
        TTF_INIT_COUNT--;
        if(TTF_INIT_COUNT == 0){
            TTF_Quit();
        }
    }
    if(useMixer){
        MIX_INIT_COUNT--;
        if(MIX_INIT_COUNT == 0){
            Mix_Quit();
        }
    }
}

//---------- UTILITIES ----------
void SDLWindowWrapper::saveImg(std::string path){
    // Render what's in the renderer to a temporary surface
    SDL_Surface* saveSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, saveSurface->format->format, saveSurface->pixels, saveSurface->pitch);

    // Save
    IMG_SavePNG(saveSurface, path.c_str());

    // Clean up
    SDL_FreeSurface(saveSurface);
}

SDL_Renderer* SDLWindowWrapper::getRenderer(){
    return renderer;
}

int SDLWindowWrapper::getWidth(){
    return screenWidth;
}

int SDLWindowWrapper::getHeight(){
    return screenHeight;
}

//---------- PRIVATE UTILITIES ----------
bool SDLWindowWrapper::init(std::string title){
    // Initialize the SDL Subsystem if necessary
    if(SDL_INIT_COUNT == 0){
        if(SDL_Init(SDL_INIT_VIDEO) < 0){
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
            // TODO - exceptions?
            return false;
        }
    }
    SDL_INIT_COUNT++;

    // Attempt to create the window
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if(!window){
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        // TODO - exceptions?
        return false;
    }

    // Attempt to create renderer for window
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        // TODO - exceptions?
        return false;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // Attempt to initialize the SDL_image library
    int imgFlags = IMG_INIT_PNG;
    if(IMG_INIT_COUNT == 0){
        if(!(IMG_Init(imgFlags) & imgFlags)){
            std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << "\n";
            // TODO - exceptions?
            return false;
        }
    }
    IMG_INIT_COUNT++;
    
    // Attempt to initialize the SDL_ttf library
    if(useTTF){
        if(TTF_INIT_COUNT == 0){
            if(TTF_Init() == -1){
                std::cerr << "SDL_TTF could not initialize ! TTF_Error: " << TTF_GetError() << "\n";
                // TODO - exceptions?
                return false;
            }
        }
        TTF_INIT_COUNT++;
    }

    // Attempt to initialize the SDL_mixer library
    if(useMixer){
        if(MIX_INIT_COUNT == 0){
             if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 ){
                std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
                // TODO - exceptions?
                return false;
             }
        }
        MIX_INIT_COUNT++;
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
        std::cerr << "Unable to load image " << path << "! SDL Error: " << IMG_GetError() << "\n";
        // TODO - exception handling
        success = false;
    } else {
        // Set the color key
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        // Convert the loaded image to into a texture
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(!texture){
            std::cerr << "Unable to optimize image " << path << "! SDL Error: " << SDL_GetError() << "\n";
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
        std::cerr << "Unable to render text to surface! SDL_TTF Error: " <<  TTF_GetError() << "\n";
        success = false;
    } else {
        // Create the texture
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(!texture){
            std::cerr << "Unable to render texture from surface! SDL Error: " << SDL_GetError() << "\n";
            // TODO - Exceptions?
            success = false;
        }
        
        // Clean up
        SDL_FreeSurface(textSurface);
    }

    return success;
}

//---------- ACCESSORS ----------
int SDLWindowWrapper::getSDLInitCount(){
    return SDL_INIT_COUNT;
}

int SDLWindowWrapper::getIMGInitCount(){
    return IMG_INIT_COUNT;
}

int SDLWindowWrapper::getTTFInitCount(){
    return TTF_INIT_COUNT;
}

int SDLWindowWrapper::getMixerInitCount(){
    return MIX_INIT_COUNT;
}

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

//---------- EXTERNAL FUNCTIONS ----------
int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, const SDL_Color& color){
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, SDL_Color& color){
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void drawPixelGrid(std::string& title, int rows, int cols, bool** data, SDLWindowWrapper* window, int frameRate, SDLTextureWrapper* background, std::string* backgroundOut, std::string* saveOutput){
    // Frame rate timer
    SDLTimer fpsTimer;
    fpsTimer.start();

    // Flag: if the background texture was provided or not
    bool backgroundProvided = background == nullptr;
    // Flag: if the background image should be deleted
    bool deleteTempImage = backgroundOut == nullptr;
    if(deleteTempImage){
        backgroundOut = new std::string("temp_background.png");
    }

    // Calculate the screen dimensions
    int screenWidth = cols * (GRID_PIXEL_SIZE + 1) - 1;
    int screenHeight = rows * (GRID_PIXEL_SIZE + 1) - 1;

    // Create the window and get the renderer
    if(!window){
        window = new SDLWindowWrapper(screenWidth, screenHeight, title);
    }
    SDL_Renderer* renderer = window->getRenderer();

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, GRID_FALSE_COLOR);
    SDL_RenderClear(renderer);

    // Check for pre-rendered bakground and/or make one
    if(!background){
        background = new SDLTextureWrapper();

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, GRID_FALSE_COLOR);
        SDL_RenderClear(renderer);

        // Draw the grid lines
        SDL_SetRenderDrawColor(renderer, GRID_TRUE_COLOR);
        // Horizontal Grid Lines
        for(int i = 0; i < rows - 1; i++){
            SDL_RenderDrawLine(renderer,
                0, (GRID_PIXEL_SIZE + 1) * (i + 1) - 1,
                screenWidth, (GRID_PIXEL_SIZE + 1) * (i + 1) - 1
            );
        }

        // Vertical grid lines
        for(int i = 0; i < cols - 1; i++){
            SDL_RenderDrawLine(renderer,
                (GRID_PIXEL_SIZE + 1) * (i + 1) - 1, 0,
                (GRID_PIXEL_SIZE + 1) * (i + 1) - 1, screenHeight
            );
        }

        // Create the background grid
        window->saveImg(*backgroundOut);
    
        // Load the background image
        background->loadFromFile(renderer, *backgroundOut);
    }
    
    // SDL_Event to track when to quit
    SDL_Event e;
    // Continue until the window is exited out of
    bool quit = false;
    // Rectangle for filling in the individual value
    SDL_Rect fillRect = {0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE};

    // Count of the ticks in the frame
    int frameTicks;

    // Main loop for SDL for rendering
    while(!quit){
        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }

        // Render the tiles
        SDL_SetRenderDrawColor(renderer, GRID_TRUE_COLOR);
        for(int i = 0; i < rows; i++){
            fillRect.x = i * (GRID_PIXEL_SIZE + 1);
            for(int j = 0; j < cols; j++){
                fillRect.y = j * (GRID_PIXEL_SIZE + 1);
                if(data[i][j]){
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }
        }

        if(frameRate > 0){
            int ticksPerFrame = 1000 / frameRate;
            // Enforce the frame rate cap
            frameTicks = fpsTimer.getTicks();
            if( frameTicks < ticksPerFrame ){
                // Wait remaining time
                SDL_Delay( ticksPerFrame - frameTicks );
            }
            quit = true;
        }
    }

    // Clean up
    if(frameRate < 0){
        delete(window);
    }
    if(!backgroundProvided){
        delete(background);
    }
    if(deleteTempImage){
        std::remove(backgroundOut->c_str());
        delete(backgroundOut);
    }
}