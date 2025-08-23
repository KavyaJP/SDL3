/**
 * This is a program to learn about event management
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

void cleanup(SDL_Window *win)
{
    std::cout << "Closing the window" << std::endl;
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main(int argc, char *argv[])
{

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing SDL3", nullptr);
        return 1;
    }

    int height = 480;
    int width = 640;

    SDL_Window *win = SDL_CreateWindow("Hello Window", width, height, 0);

    if (!win)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing the window", nullptr);
        cleanup(win);
        return 1;
    }

    // Start the game loop
    bool running = true;
    while (running)
    {
        SDL_Event event{0}; // This will run in every iteration, and will tell us about which events happened
        // now we will pass the event structure in a while loop
        while (SDL_PollEvent(&event)) // We only pass a pointer so it takes less space in memory
        {
            switch (event.type) // To check which event is happening
            {
            case SDL_EVENT_QUIT: // if the user clicks on the close button of the window
                std::cout << "User clicked on the close window button" << std::endl;
                running = false; // This will break us out of the main game loop, after which the cleanup function will be called
                break;
            }
        }
    }

    cleanup(win);
    return 0;
}