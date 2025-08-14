#include "common.h"
#include <SDL2/SDL.h>

#include "application.h"


int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init() failed\n");
        return 1;
    }    

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // Set depth buffer size

    Application app(argc, argv);
   

    try
    {
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "!!!Exception:" << '\n';
        std::cerr << e.what() << '\n';
    }
    
    
}