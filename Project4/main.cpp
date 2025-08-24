/**
 * This is a program to learn about Image Loading
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

struct SDLState
{
    SDL_Window *window;
    SDL_Event event;
    SDL_Renderer *renderer; // renderer is used for rendering images and other objects in a window
};

// The cleanup function now takes the whole SDLState struct.
void cleanup(SDLState &state)
{
    std::cout << "Closing the window" << std::endl;
    if (state.window)
        SDL_DestroyWindow(state.window);
    if (state.renderer)
        SDL_DestroyRenderer(state.renderer); // We will now also destroy the renderer on cleanup
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    SDLState state = {nullptr}; // Initialize the window pointer to null

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing SDL3", nullptr);
        return 1;
    }

    int height = 720;
    int width = 1280;

    state.window = SDL_CreateWindow("Hello Window", width, height, 0);

    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing the window", nullptr);
        SDL_Quit();
        return 1;
    }

    /* nullptr makes it so that the object is rendered on the current device
     * Later We will make it so that we can automatically choose the best device, or let user choose the device
     */
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer) // If there was an error creating renderer
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing renderer", nullptr);
        cleanup(state);
        return 1;
    }

    // Load game asset
    SDL_Texture *idle_texture = IMG_LoadTexture(state.renderer, "../data/idle.png");
    SDL_SetTextureScaleMode(idle_texture, SDL_SCALEMODE_NEAREST);

    // Start the game loop
    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&state.event))
        {
            switch (state.event.type)
            {
            case SDL_EVENT_QUIT:
                std::cout << "User clicked on the close window button" << std::endl;
                running = false;
                break;
            }
        }

        // Draw on the window

        // RGBA, this is used to set a color for renderer
        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
        // This is the line that will actually make the renderer white
        SDL_RenderClear(state.renderer);

        SDL_FRect src{
            .x = 0,
            .y = 0,
            .w = 32,
            .h = 32}; // How many pixels do we want to draw on the window

        SDL_FRect dst{
            .x = 0,
            .y = 0,
            .w = 32,
            .h = 32}; // at which source do we want to load the texture at

        // Render Assets
        SDL_RenderTexture(state.renderer, idle_texture, &src, &dst);

        // Now we will swap buffers to present it on window
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyTexture(idle_texture);
    // The cleanup function is called after the game loop finishes.
    cleanup(state);
    return 0;
}
