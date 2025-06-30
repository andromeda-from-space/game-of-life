#include "sdl-basics.h"

// Super basic testing for low level coding issues
int main(int argc, char* argv[]){
    // Rendering to a texture as opposed to a window
    SDLWindowWrapper window = SDLWindowWrapper(640, 480, "Render to Texture Test");

    return 0;
}