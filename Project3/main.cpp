/**
 * This is a program to learn about Renderer
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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
        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255); // RGBA, this is used to set a color for renderer
        SDL_RenderClear(state.renderer);                            // This is the line that will actually make the renderer white

        // Now we will swap buffers to present it on window
        SDL_RenderPresent(state.renderer);

        SDL_Vertex vertices[3];

        // Define the first vertex (e.g., top)
        vertices[0].position.x = 320.0f;
        vertices[0].position.y = 100.0f;
        vertices[0].color.r = 0;
        vertices[0].color.g = 0;
        vertices[0].color.b = 0;
        vertices[0].color.a = 255;

        // Define the second vertex (e.g., bottom-left)
        vertices[1].position.x = 120.0f;
        vertices[1].position.y = 400.0f;
        vertices[1].color.r = 0;
        vertices[1].color.g = 0;
        vertices[1].color.b = 0;
        vertices[1].color.a = 255;

        // Define the third vertex (e.g., bottom-right)
        vertices[2].position.x = 520.0f;
        vertices[2].position.y = 400.0f;
        vertices[2].color.r = 0;
        vertices[2].color.g = 0;
        vertices[2].color.b = 0;
        vertices[2].color.a = 255;

        int indices[3] = {0, 1, 2}; // Indices for the triangle
        SDL_RenderGeometry(state.renderer, nullptr, vertices, 3, indices, 3);
        SDL_RenderPresent(state.renderer);
    }

    // The cleanup function is called after the game loop finishes.
    cleanup(state);
    return 0;
}
