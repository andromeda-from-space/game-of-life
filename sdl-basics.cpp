#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <sstream>
#include <iostream>

#include "sdl-basics.h"

//--------------------------------------------------------------------
//---------- Exceptions ----------------------------------------------
//--------------------------------------------------------------------
InitFailException::InitFailException(const std::string msg) : message(msg) {}
InitFailException::InitFailException(const char* msg) : message(msg) {}

const char* InitFailException::what() const noexcept{
    return message.c_str();
}

LoadFailException::LoadFailException(const std::string msg) : message(msg) {}
LoadFailException::LoadFailException(const char* msg) : message(msg) {}

const char* LoadFailException::what() const noexcept{
    return message.c_str();
}

RenderFailException::RenderFailException(const std::string msg) : message(msg) {}
RenderFailException::RenderFailException(const char* msg) : message(msg) {}

const char* RenderFailException::what() const noexcept{
    return message.c_str();
}

SDLFailException::SDLFailException(const std::string msg) : message(msg) {}
SDLFailException::SDLFailException(const char* msg) : message(msg) {}

const char* SDLFailException::what() const noexcept{
    return message.c_str();
}

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

SDLWindowWrapper::SDLWindowWrapper(const SDLWindowWrapper & other) : screenWidth(other.screenWidth), screenHeight(other.screenHeight), window(nullptr), renderer(nullptr), useTTF(other.useTTF), useMixer(useMixer), fpsCap(other.fpsCap){
    // Initialize and create the window
    std::stringstream temp;
    temp << SDL_GetWindowTitle(other.window) << " - new";
    init(temp.str());
}

SDLWindowWrapper& SDLWindowWrapper::operator=(const SDLWindowWrapper & other){
    if(this != &other){
        // Copy data
        screenWidth = other.screenWidth;
        screenHeight = other.screenHeight;
        useTTF = other.useTTF;
        useMixer = useMixer;
        fpsCap = other.fpsCap;

        // Initialize and create the window
        std::stringstream temp;
        temp << SDL_GetWindowTitle(other.window) << " - new";
        init(temp.str());
    }
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

SDL_Renderer* SDLWindowWrapper::getRenderer(){
    return renderer;
}

int SDLWindowWrapper::getWidth(){
    return screenWidth;
}

int SDLWindowWrapper::getHeight(){
    return screenHeight;
}

Uint32 SDLWindowWrapper::getWindowPixelFormat(){
    return SDL_GetWindowPixelFormat(window);
}

//---------- PRIVATE UTILITIES ----------
bool SDLWindowWrapper::init(std::string title){
    // Return value
    bool success = true;

    // Error message creation
    std::stringstream errorMessage;

    // Initialize the SDL Subsystem if necessary
    if(SDL_INIT_COUNT == 0){
        if(SDL_Init(SDL_INIT_VIDEO) < 0){
            errorMessage << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
            throw InitFailException(errorMessage.str());
            success = false;
        }
    }
    SDL_INIT_COUNT++;

    // Attempt to create the window
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if(!window){
        errorMessage << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        throw SDLFailException(errorMessage.str());
        success = false;
    }

    // Attempt to create renderer for window
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        errorMessage << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        throw SDLFailException(errorMessage.str());
        success = false;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // Attempt to initialize the SDL_image library
    int imgFlags = IMG_INIT_PNG;
    if(IMG_INIT_COUNT == 0){
        if(!(IMG_Init(imgFlags) & imgFlags)){
            errorMessage << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << "\n";
            throw InitFailException(errorMessage.str());
            success = false;
        }
    }
    IMG_INIT_COUNT++;
    
    // Attempt to initialize the SDL_ttf library
    if(useTTF){
        if(TTF_INIT_COUNT == 0){
            if(TTF_Init() == -1){
                errorMessage << "SDL_TTF could not initialize ! TTF_Error: " << TTF_GetError() << "\n";
                throw InitFailException(errorMessage.str());
                success = false;
            }
        }
        TTF_INIT_COUNT++;
    }

    // Attempt to initialize the SDL_mixer library
    if(useMixer){
        if(MIX_INIT_COUNT == 0){
             if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 ){
                errorMessage << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
                throw InitFailException(errorMessage.str());
                success = false;
             }
        }
        MIX_INIT_COUNT++;
    }

    return success;
}

//--------------------------------------------------------------------
//---------- SDLTextureWrapper ---------------------------------------
//--------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
SDLTextureWrapper::SDLTextureWrapper() : texture(nullptr), width(-1), height(-1), ownTexture(true) {}

SDLTextureWrapper::SDLTextureWrapper(const SDLTextureWrapper& other) : texture(other.texture), width(other.width), height(other.height), ownTexture(false) {}

SDLTextureWrapper& SDLTextureWrapper::operator=(const SDLTextureWrapper& other) {
    if(this != &other){
        // Copy memory
        texture = other.texture;
        width = other.width;
        height = other.height;
        ownTexture = false;
    }
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

    // Error message string stream
    std::stringstream errorMessage;

    // Load image at specified path as a surface
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(!loadedSurface){
        errorMessage << "Unable to load image " << path << "! SDL Error: " << IMG_GetError() << "\n";
        throw LoadFailException(errorMessage.str());
        success = false;
    } else {
        // Set the color key
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        // Convert the loaded image to into a texture
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(!texture){
            errorMessage << "Unable to optimize image " << path << "! SDL Error: " << SDL_GetError() << "\n";
            throw RenderFailException(errorMessage.str());
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
    if(texture && ownTexture){
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

    // Error message string stream
    std::stringstream errorMessage;

    // Delete old texture
    free();

    // Create a surface from the text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    
    // Pull out it's dimensions
    width = textSurface->w;
    height = textSurface->h;

    // Convert Surface to a texture
    if(!textSurface){
        errorMessage << "Unable to render text to surface! SDL_TTF Error: " <<  TTF_GetError() << "\n";
        throw SDLFailException(errorMessage.str());
        success = false;
    } else {
        // Create the texture
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(!texture){
            errorMessage << "Unable to render texture from surface! SDL Error: " << SDL_GetError() << "\n";
            throw RenderFailException(errorMessage.str());
            success = false;
        }
        
        // Clean up
        SDL_FreeSurface(textSurface);
    }

    return success;
}

void SDLTextureWrapper::createBlankTexture(SDL_Renderer* renderer, Uint32 pixelFormat, int width, int height){
    // Create the texture
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    texture = SDL_CreateTexture(renderer, pixelFormat, SDL_TEXTUREACCESS_TARGET, width, height);

    // Set the height
    this->width = width;
    this->height = height;
}

void SDLTextureWrapper::setAsRenderTarget(SDL_Renderer* renderer){
    SDL_SetRenderTarget(renderer, texture);
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

//--------------------------------------------------------------------
//---------- SDLPixelGridRenderer ------------------------------------
//--------------------------------------------------------------------
//---------- CONSTRUCTORS & DESTRUCTOR ----------
SDLPixelGridRenderer::SDLPixelGridRenderer() : title("SDLPixelGridRenderer"), rows(-1), cols(-1), ticksPerFrame(-1), window(nullptr), renderer(nullptr), fpsTimer(SDLTimer()), background(SDLTextureWrapper()), pixelSize(GRID_PIXEL_SIZE), falseColor(GRID_FALSE_COLOR), gridLineColor(GRID_LINES_COLOR), trueColor(GRID_TRUE_COLOR){}

SDLPixelGridRenderer::SDLPixelGridRenderer(std::string title, int rows, int cols) : title(title), rows(rows), cols(cols), ticksPerFrame(-1), window(nullptr), renderer(nullptr), fpsTimer(SDLTimer()), background(SDLTextureWrapper()), pixelSize(GRID_PIXEL_SIZE), falseColor(GRID_FALSE_COLOR), gridLineColor(GRID_LINES_COLOR), trueColor(GRID_TRUE_COLOR) {
    // Run initialization script
    init();
}

SDLPixelGridRenderer::SDLPixelGridRenderer(std::string title, int rows, int cols, SDL_Color gridColor) : rows(rows), cols(cols), ticksPerFrame(-1), window(nullptr), renderer(nullptr), fpsTimer(SDLTimer()), background(SDLTextureWrapper()), gridLineColor(gridColor), falseColor(GRID_FALSE_COLOR), trueColor(GRID_TRUE_COLOR), pixelSize(GRID_PIXEL_SIZE) {
    // Run initialization script
    init();
}

SDLPixelGridRenderer::SDLPixelGridRenderer(std::string title, int rows, int cols, SDL_Color gridColor,SDL_Color falseColor,  SDL_Color trueColor) : rows(rows), cols(cols), ticksPerFrame(-1), window(nullptr), renderer(nullptr), fpsTimer(SDLTimer()), background(SDLTextureWrapper()), gridLineColor(gridColor), falseColor(falseColor), trueColor(trueColor), pixelSize(GRID_PIXEL_SIZE) {
    // Run initialization script
    init();
}

SDLPixelGridRenderer::SDLPixelGridRenderer(std::string title, int rows, int cols, SDL_Color gridColor, SDL_Color falseColor, SDL_Color trueColor, int pixelSize) : rows(rows), cols(cols), ticksPerFrame(-1), window(nullptr), renderer(nullptr), fpsTimer(SDLTimer()), background(SDLTextureWrapper()), gridLineColor(gridColor), falseColor(falseColor), trueColor(trueColor), pixelSize(pixelSize){
    // Run initialization script
    init();
}

SDLPixelGridRenderer::SDLPixelGridRenderer(const SDLPixelGridRenderer& other) : rows(other.rows), cols(other.cols), ticksPerFrame(other.ticksPerFrame), window(nullptr), renderer(nullptr), fpsTimer(SDLTimer()), background(SDLTextureWrapper()), gridLineColor(other.gridLineColor), falseColor(other.falseColor), trueColor(other.trueColor), pixelSize(other.pixelSize){
    // Run initialization script
    init();
}

SDLPixelGridRenderer& SDLPixelGridRenderer::operator=(const SDLPixelGridRenderer& other){
    if(this != &other){
        // Delete the old window
        if(window){
            delete(window);
            window = nullptr;
        }

        // Set the other variables
        title = other.title;
        rows = other.rows;
        cols = other.cols;
        ticksPerFrame = other.ticksPerFrame;
        gridLineColor = other.gridLineColor;
        falseColor = other.falseColor;
        trueColor = other.trueColor;
        pixelSize = other.pixelSize;

        // Re-run initialization
        init();
    }
    return *this;
}

SDLPixelGridRenderer::~SDLPixelGridRenderer(){
    delete(window);
}

//---------- UTILITIES ----------
void SDLPixelGridRenderer::drawBoolGrid(bool** data, bool saveOutput, std::string path){
        // Flag to exit the window
    bool quit = false;
    // SDL Event to track if 'x' has been pressed
    SDL_Event e;
    // Rectangle for filling the pixels
    SDL_Rect fillRect = {0, 0, pixelSize, pixelSize};
    // String stream to make unique file names
    std::stringstream filenameMaker;
    std::string outputFilename;


    // Render the background
    background.render(renderer, 0, 0);

    // Render the "on" tiles
    SDL_SetRenderDrawColor(renderer, trueColor);
    for(int i = 0; i < rows; i++){
        fillRect.y = i * (pixelSize + 1);
        for(int j = 0; j < cols; j++){
            fillRect.x = j * (pixelSize + 1);
            if(data[i][j]){
                SDL_RenderFillRect(renderer, &fillRect);
            }
        }
    }

    // Save the current frame
    if(saveOutput){
        window->saveImg(path);
    }

    // Update the screen
    SDL_RenderPresent(renderer);

    // Create all frames
    while(!quit){
        // Poll events to enable the exit out button
        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }
    }
}

void SDLPixelGridRenderer::animateBoolGrid(bool*** data, int frameCount, int frameRate, bool saveOutput, std::string path){
    // Set ticks per frame
    ticksPerFrame = 1000 / frameRate;

    // Flag to exit the window
    bool quit = false;
    // SDL Event to track if 'x' has been pressed
    SDL_Event e;
    // Rectangle for filling the pixels
    SDL_Rect fillRect = {0, 0, pixelSize, pixelSize};
    // String stream to make unique file names
    std::stringstream filenameMaker;
    std::string outputFilename;
    // The count of ticks since the previous frame
    int currTicks;

    // Create all frames
    for(int k = 0; k < frameCount && !quit; k++){
        // Start the fps timer
        fpsTimer.start();

        // Poll events to enable the exit out button
        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }

        // Render the background
        background.render(renderer, 0, 0);

        // Render the "on" tiles
        SDL_SetRenderDrawColor(renderer, trueColor);
        for(int i = 0; i < rows; i++){
            fillRect.y = i * (pixelSize + 1);
            for(int j = 0; j < cols; j++){
                fillRect.x = j * (pixelSize + 1);
                if(data[k][i][j]){
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }
        }

        // Save the current frame
        if(saveOutput){
            filenameMaker << path << k << ".png";
            outputFilename = filenameMaker.str();
            filenameMaker.str();
            window->saveImg(outputFilename);
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Check the ticks
        currTicks = fpsTimer.getTicks();
        if(currTicks < ticksPerFrame){
            // Wait remaining time
            SDL_Delay(ticksPerFrame - currTicks);
        }
    }
}

void SDLPixelGridRenderer::scrollBoolGrid(bool** data, int frameCount, int frameRate, ScrollDirection direction, bool saveOutput, std::string path){
    // Set ticks per frame
    ticksPerFrame = 1000 / frameRate;

    // Flag to exit the window
    bool quit = false;
    // SDL Event to track if 'x' has been pressed
    SDL_Event e;
    // Rectangle for filling the pixels
    SDL_Rect fillRect = {0, 0, pixelSize, pixelSize};
    // String stream to make unique file names
    std::stringstream filenameMaker;
    std::string outputFilename;
    // The count of ticks since the previous frame
    int currTicks;
    // Current frame
    int currFrame = 0;
    // Current data to render
    bool** currData;
    // Temporary pointer for doing the pointer swap
    bool* temp;
    // Transpose the data if left to right for more efficient rendering
    if(direction == ScrollDirection::LEFT || direction == ScrollDirection::RIGHT){
        currData = new bool*[cols];
        for(int j = 0; j < cols; j++){
            currData[j] = new bool[rows];
            for(int i = 0; i < rows; i++){
                currData[j][i] = data[i][j];
            }
        }
    } else {
        // Otherwise just copy the data
        currData = new bool*[rows];
        for(int i = 0; i < rows; i++){
            currData[i] = new bool[cols];
            for(int j = 0; j < cols; j++){
                currData[i][j] = data[i][j];
            }
        }
    }

    // Create all frames
    while(!quit){
        // Start the fps timer
        fpsTimer.start();

        // Poll events to enable the exit out button
        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }

        // Render the background
        background.render(renderer, 0, 0);

        // Set the render color for the "on" tiles
        SDL_SetRenderDrawColor(renderer, trueColor);
        if(direction == ScrollDirection::UP || direction == ScrollDirection::DOWN){
            // Up/Down direction, Render the "on" tiles
            for(int i = 0; i < rows; i++){
                fillRect.y = i * (pixelSize + 1);
                for(int j = 0; j < cols; j++){
                    fillRect.x = j * (pixelSize + 1);
                    if(currData[i][j]){
                        SDL_RenderFillRect(renderer, &fillRect);
                    }
                }
            }

            // Scroll the grid
            if(direction == ScrollDirection::UP){
                temp = currData[0];
                for(int i = 0; i < rows - 1; i++){
                    currData[i] = currData[i + 1];
                }
                currData[rows - 1] = temp;
            } else {
                temp = currData[rows - 1];
                for(int i = rows - 1; i > 0; i--){
                    currData[i] = currData[i - 1];
                }
                currData[0] = temp;
            }
        } else {
            // Left/Right - use transpose
            for(int j = 0; j < cols; j++){
                fillRect.x = j * (pixelSize + 1);
                for(int i = 0; i < rows; i++){
                    fillRect.y = i * (pixelSize + 1);
                    if(currData[j][i]){
                        SDL_RenderFillRect(renderer, &fillRect);
                    }
                }
            }

            // Scroll the grid
            if(direction == ScrollDirection::LEFT){
                temp = currData[0];
                for(int j = 0; j < cols - 1; j++){
                    currData[j] = currData[j + 1];
                }
                currData[cols - 1] = temp;
            } else {
                temp = currData[cols - 1];
                for(int j = cols - 1; j > 0; j--){
                    currData[j] = currData[j - 1];
                }
                currData[0] = temp;
            }
        }

        // Save the current frame
        if(saveOutput){
            filenameMaker << path << currFrame << ".png";
            outputFilename = filenameMaker.str();
            filenameMaker.str();
            window->saveImg(outputFilename);
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Check the ticks
        currTicks = fpsTimer.getTicks();
        if(currTicks < ticksPerFrame){
            // Wait remaining time
            SDL_Delay(ticksPerFrame - currTicks);
        }

        // Check frames
        if(frameCount > 0){
            if(currFrame == frameCount){
                quit = true;
            }
        }
        currFrame++;
    }
}

void SDLPixelGridRenderer::scrollColorGrid(SDL_Color** data, int frameCount, int frameRate, ScrollDirection direction, bool saveOutput, std::string path){
    // Set ticks per frame
    ticksPerFrame = 1000 / frameRate;

    // Flag to exit the window
    bool quit = false;
    // SDL Event to track if 'x' has been pressed
    SDL_Event e;
    // Rectangle for filling the pixels
    SDL_Rect fillRect = {0, 0, pixelSize, pixelSize};
    // String stream to make unique file names
    std::stringstream filenameMaker;
    std::string outputFilename;
    // The count of ticks since the previous frame
    int currTicks;
    // Current frame
    int currFrame = 0;
    // Current data to render
    SDL_Color** currData;
    // Temporary pointer for doing the pointer swap
    SDL_Color* temp;
    // Transpose the data if left to right for more efficient rendering
    if(direction == ScrollDirection::LEFT || direction == ScrollDirection::RIGHT){
        currData = new SDL_Color*[cols];
        for(int j = 0; j < cols; j++){
            currData[j] = new SDL_Color[rows];
            for(int i = 0; i < rows; i++){
                currData[j][i] = data[i][j];
            }
        }
    } else {
        // Otherwise just copy the data
        currData = new SDL_Color*[rows];
        for(int i = 0; i < rows; i++){
            currData[i] = new SDL_Color[cols];
            for(int j = 0; j < cols; j++){
                currData[i][j] = data[i][j];
            }
        }
    }

    // Create all frames
    while(!quit){
        // Start the fps timer
        fpsTimer.start();

        // Poll events to enable the exit out button
        while( SDL_PollEvent( &e ) ){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
        }

        // Render the background
        background.render(renderer, 0, 0);

        // Set the render color for the "on" tiles
        if(direction == ScrollDirection::UP || direction == ScrollDirection::DOWN){
            // Up/Down direction, Render the "on" tiles
            for(int i = 0; i < rows; i++){
                fillRect.y = i * (pixelSize + 1);
                for(int j = 0; j < cols; j++){
                    fillRect.x = j * (pixelSize + 1);
                    SDL_SetRenderDrawColor(renderer, currData[i][j]);
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }

            // Scroll the grid
            if(direction == ScrollDirection::UP){
                temp = currData[0];
                for(int i = 0; i < rows - 1; i++){
                    currData[i] = currData[i + 1];
                }
                currData[rows - 1] = temp;
            } else {
                temp = currData[rows - 1];
                for(int i = rows - 1; i > 0; i--){
                    currData[i] = currData[i - 1];
                }
                currData[0] = temp;
            }
        } else {
            // Left/Right - use transpose
            for(int j = 0; j < cols; j++){
                fillRect.x = j * (pixelSize + 1);
                for(int i = 0; i < rows; i++){
                    fillRect.y = i * (pixelSize + 1);
                    SDL_SetRenderDrawColor(renderer, currData[j][i]);
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }

            // Scroll the grid
            if(direction == ScrollDirection::LEFT){
                temp = currData[0];
                for(int j = 0; j < cols - 1; j++){
                    currData[j] = currData[j + 1];
                }
                currData[cols - 1] = temp;
            } else {
                temp = currData[cols - 1];
                for(int j = cols - 1; j > 0; j--){
                    currData[j] = currData[j - 1];
                }
                currData[0] = temp;
            }
        }

        // Save the current frame
        if(saveOutput){
            filenameMaker << path << currFrame << ".png";
            outputFilename = filenameMaker.str();
            filenameMaker.str();
            window->saveImg(outputFilename);
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Check the ticks
        currTicks = fpsTimer.getTicks();
        if(currTicks < ticksPerFrame){
            // Wait remaining time
            SDL_Delay(ticksPerFrame - currTicks);
        }

        // Check frames
        if(frameCount > 0){
            if(currFrame == frameCount){
                quit = true;
            }
        }
        currFrame++;
    }
}


//---------- MUTATORS ----------
void SDLPixelGridRenderer::setFalseColor(SDL_Color newColor){
    falseColor = newColor;
    
    // Redraw the background
    drawBackground();
}

void SDLPixelGridRenderer::setGridLineColor(SDL_Color newColor){
    gridLineColor = newColor;

    // Redraw the background
    drawBackground();
}

void SDLPixelGridRenderer::setTrueColor(SDL_Color newColor){
    trueColor = newColor;
}
//---------- PRIVATE UTILITIES ----------
void SDLPixelGridRenderer::init(){
    // Calculate the window width and height
    int windowWidth = cols * (pixelSize + 1) - 1;
    int windowHeight = rows * (pixelSize + 1) - 1;

    // Create the window
    window = new SDLWindowWrapper(windowWidth, windowHeight, title);

    // Pull out the renderer
    renderer = window->getRenderer();

    // Create the texture for the background
    background.createBlankTexture(renderer, window->getWindowPixelFormat(), windowWidth, windowHeight);
    drawBackground();

    // Reset the renderer
    SDL_SetRenderTarget(renderer, NULL);
}

void SDLPixelGridRenderer::drawBackground(){
    // Calculate the window width and height
    int windowWidth = cols * (pixelSize + 1) - 1;
    int windowHeight = rows * (pixelSize + 1) - 1;

    // Create the texture for the background
    background.setAsRenderTarget(renderer);

    // Make the background
    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, falseColor);
    SDL_RenderClear(renderer);

    // Draw the grid lines
    SDL_SetRenderDrawColor(renderer, gridLineColor);
    // Horizontal Grid Lines
    for(int i = 0; i < rows - 1; i++){
        SDL_RenderDrawLine(renderer,
            0, (pixelSize + 1) * (i + 1) - 1,
            windowWidth, (pixelSize + 1) * (i + 1) - 1
        );
    }

    // Vertical grid lines
    for(int i = 0; i < cols - 1; i++){
        SDL_RenderDrawLine(renderer,
            (pixelSize + 1) * (i + 1) - 1, 0,
            (pixelSize + 1) * (i + 1) - 1, windowHeight
        );
    }

    // Render to the texture
    SDL_RenderPresent(renderer);

    // Reset the renderer
    SDL_SetRenderTarget(renderer, NULL);
}

//---------- EXTERNAL FUNCTIONS ----------
int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, const SDL_Color& color){
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int SDL_SetRenderDrawColor(SDL_Renderer*& renderer, SDL_Color& color){
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}