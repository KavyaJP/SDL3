#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

struct SDLState
{
    SDL_Window *window;
    SDL_Event event;
    SDL_Renderer *renderer;
    int width, height, logical_width, logical_height;
};

void cleanup(SDLState &state);
bool initialise(SDLState &state);

int main(int argc, char *argv[])
{

    SDLState state = {nullptr}; // Initialize the window pointer to null

    // This will be te height and width of the actual window Created as well as the logical height and width
    state.height = 720;
    state.width = 1280;
    state.logical_width = 640;
    state.logical_height = 480;

    if (!initialise(state)) // If there is any problem in initialisation we exit the program
    {
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
            case SDL_EVENT_WINDOW_RESIZED:
                state.width = state.event.window.data1;
                state.height = state.event.window.data2;
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

void cleanup(SDLState &state)
{
    std::cout << "Closing the window" << std::endl;
    if (state.window)
        SDL_DestroyWindow(state.window);

    std::cout << "Destroying the renderer" << std::endl;
    if (state.renderer)
        SDL_DestroyRenderer(state.renderer); // We will now also destroy the renderer on cleanup

    std::cout << "Quitting" << std::endl;
    SDL_Quit();
}

bool initialise(SDLState &state)
{
    bool initSuccess = true;
    // This block will help us in detecting run time errors when initialising an SDL Object
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing SDL3", nullptr);
        initSuccess = false;
    }

    // Creating a Widnow, Title | width | Height | SDL Flag
    state.window = SDL_CreateWindow("Hello Window", state.width, state.height, SDL_WINDOW_RESIZABLE);

    // If any runtime error happened when initialsing a window
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing the window", nullptr);
        SDL_Quit();
        initSuccess = false;
    }

    /* Renderer help us render images and objects on the window
     * nullptr makes it so that the object is rendered on the current device
     * Later We will make it so that we can automatically choose the best device, or let user choose the device
     */
    // SDLWindow | name
    state.renderer = SDL_CreateRenderer(state.window, nullptr);

    // If there was an error creating renderer
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing renderer", nullptr);
        cleanup(state);
        initSuccess = false;
    }

    // Configure a logical resolution for game so SDL has to worry about the scaling and not us
    SDL_SetRenderLogicalPresentation(state.renderer, state.logical_width, state.logical_height, SDL_LOGICAL_PRESENTATION_LETTERBOX); // Letterbox makes it so that the game retains the resolution i provided and fills the rest of the area with black color

    return initSuccess;
}