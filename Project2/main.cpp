/**
 * This is a program to learn about event management
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

// We will make a struct named SDLState so whenever we want to use an SDL Component,
// we can just pass this state object around.
struct SDLState
{
    SDL_Window *window;
    SDL_Event event;
    SDL_Renderer *renderer;
}; // Added a semicolon here, which is required after a struct definition.

// The cleanup function now takes the whole SDLState struct.
void cleanup(SDLState &state)
{
    std::cout << "Closing the window" << std::endl;
    // Check if the window exists before trying to destroy it.
    if (state.window)
    {
        SDL_DestroyWindow(state.window);
    }
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    SDLState state = {nullptr}; // Initialize the window pointer to null

    // SDL_Init returns 0 on success and a negative number on failure.
    // The check has been corrected to look for a negative return value.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing SDL3", nullptr);
        return 1;
    }

    int height = 480;
    int width = 640;

    state.window = SDL_CreateWindow("Hello Window", width, height, 0);

    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing the window", nullptr);
        SDL_Quit(); // Just quit SDL if the window fails to create.
        return 1;
    }

    // Start the game loop
    bool running = true;
    while (running)
    {
        // We pass the address of our event object to SDL_PollEvent.
        // SDL will fill this object with event information.
        while (SDL_PollEvent(&state.event))
        {
            // We check the 'type' of the event that occurred.
            switch (state.event.type)
            {
            case SDL_EVENT_QUIT: // If the user clicks on the close button of the window
                std::cout << "User clicked on the close window button" << std::endl;
                running = false; // This will break us out of the main game loop.
                break;
            }
        }
    }

    // The cleanup function is called after the game loop finishes.
    cleanup(state);
    return 0;
}
