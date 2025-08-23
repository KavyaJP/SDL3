/**
 * This is a program that will open a windows briefly.
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

void cleanup(SDL_Window *win) // We can use this function to clean up any destroy any window
{
    SDL_DestroyWindow(win);
    SDL_Quit(); // Quit the window once the program is done
}

int main(int argc, char *argv[])
{ // We do this because we want SDL to be able to find our main function, thats why we have to give it arguments

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing SDL3", nullptr);
        return 1; // no need to go any further if there is an error in initializing SDL3
    }

    // Height and Width of the window
    int height = 480;
    int width = 640;

    SDL_Window *win = SDL_CreateWindow("Hello Window", width, height, 0); // This will create a window

    if (!win) // To check if the window is actually created
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing the window", nullptr);
        cleanup(win); // So no error happens, for safe side
        return 1;
    }

    cleanup(win);
    return 0;
}