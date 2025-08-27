#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

// Represents the core components of the SDL application state.
struct SDLState
{
    SDL_Window *window;
    SDL_Event event;
    SDL_Renderer *renderer;
    int width, height, logical_width, logical_height;
};

// Function prototypes
void cleanup(SDLState &state);
bool initialise(SDLState &state);

int main(int argc, char *argv[])
{
    // Initialize the SDL state structure.
    SDLState state = {nullptr};

    // Set the desired dimensions for the window and the logical rendering area.
    state.height = 720;
    state.width = 1280;
    state.logical_width = 640;
    state.logical_height = 480;

    // Initialise SDL, the window, and the renderer.
    if (!initialise(state))
    {
        return 1;
    }

    // Load game asset
    SDL_Texture *idle_texture = IMG_LoadTexture(state.renderer, "../data/idle.png");
    if (!idle_texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to load texture: ../data/idle.png", state.window);
        cleanup(state);
        return 1;
    }
    SDL_SetTextureScaleMode(idle_texture, SDL_SCALEMODE_NEAREST);

    // --- GAME DATA & INPUT SETUP ---
    // Get a pointer to the keyboard state array. This is a snapshot of the keyboard.
    const bool *keys = SDL_GetKeyboardState(nullptr);
    float playerX = 0;
    float floor = state.logical_height;

    // --- DELTA TIME SETUP ---
    // Get the time at the start of the game.
    uint64_t prevTime = SDL_GetTicks();

    // Start the main game loop.
    bool running = true;
    while (running)
    {
        // --- DELTA TIME CALCULATION ---
        // Get the current time at the beginning of the frame.
        uint64_t nowTime = SDL_GetTicks();
        // Calculate the time elapsed since the last frame, in seconds.
        // We use 1000.0f to force a floating-point division.
        float deltaTime = (nowTime - prevTime) / 1000.0f;

        // Process all pending events in the queue.
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

        // --- HANDLE PLAYER INPUT ---
        // Define player speed in pixels per second.
        float moveAmountX = 0;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) // left
            moveAmountX += -75;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) // right
            moveAmountX += 75;

        // Update player position based on speed and delta time.
        playerX += moveAmountX * deltaTime;

        // --- RENDERING LOGIC ---
        // Set the draw color and clear the screen.
        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
        SDL_RenderClear(state.renderer);

        // Define the source and destination rectangles for rendering.
        const float spriteSize = 32;
        SDL_FRect src{
            .x = 0,
            .y = 0,
            .w = spriteSize,
            .h = spriteSize};

        SDL_FRect dst{
            .x = playerX,
            .y = floor - spriteSize,
            .w = spriteSize,
            .h = spriteSize};

        // Render the texture to the screen.
        SDL_RenderTexture(state.renderer, idle_texture, &src, &dst);

        // Swap the buffers to display the new frame.
        SDL_RenderPresent(state.renderer);

        // --- END OF FRAME ---
        // Set the previous time to the current time for the next frame's calculation.
        prevTime = nowTime;
    }

    // --- CLEANUP AFTER LOOP ---
    SDL_DestroyTexture(idle_texture);
    cleanup(state);
    return 0;
}

/**
 * @brief Releases all SDL resources in the reverse order of creation.
 * @param state The current SDL application state.
 */
void cleanup(SDLState &state)
{
    std::cout << "Destroying the renderer" << std::endl;
    if (state.renderer)
        SDL_DestroyRenderer(state.renderer);

    std::cout << "Closing the window" << std::endl;
    if (state.window)
        SDL_DestroyWindow(state.window);

    // In the latest SDL3, SDL_Quit handles all subsystem cleanup.
    std::cout << "Quitting SDL" << std::endl;
    SDL_Quit();
}

/**
 * @brief Initializes SDL and its subsystems, creates a window and a renderer.
 * @param state The SDL application state to initialize.
 * @return True on success, false on failure.
 */
bool initialise(SDLState &state)
{
    // Initialize the SDL video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing SDL3", nullptr);
        return false;
    }

    // Create the main application window.
    state.window = SDL_CreateWindow("Hello Window", state.width, state.height, SDL_WINDOW_RESIZABLE);
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing the window", nullptr);
        cleanup(state);
        return false;
    }

    // Create a renderer for the window.
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error Initializing renderer", nullptr);
        cleanup(state);
        return false;
    }

    // Configure a logical resolution for the game.
    SDL_SetRenderLogicalPresentation(state.renderer, state.logical_width, state.logical_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return true;
}
