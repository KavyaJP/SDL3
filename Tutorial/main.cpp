#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <format>

#include "gameObject.h"

// Represents the core components of the SDL application state.
struct SDLState
{
    SDL_Window *window;
    SDL_Event event;
    SDL_Renderer *renderer;
    int width, height, logical_width, logical_height;
    const bool *keys;

    SDLState() : keys(SDL_GetKeyboardState(nullptr)) {}
};

const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 0;
const int MAP_ROWS = 5;
const int MAP_COLUMNS = 50;
const int TILE_SIZE = 32;

struct GameState
{
    std::array<std::vector<GameObject>, 2> layers;
    std::vector<GameObject> backgroundTiles;
    std::vector<GameObject> foregroundTiles;
    int playerIndex;
    SDL_FRect mapViewPort;
    float bg2scroll, bg3scroll, bg4scroll;

    GameState(const SDLState &state)
    {
        playerIndex = -1;
        mapViewPort = {
            .x = 0,
            .y = 0,
            .w = static_cast<float>(state.logical_width),
            .h = static_cast<float>(state.logical_height)};
        bg2scroll = bg3scroll = bg4scroll = 0;
    }

    GameObject &player() { return layers[LAYER_IDX_CHARACTERS][playerIndex]; }
};

struct Resources
{
    const int ANIM_PLAYER_IDLE = 0;
    const int ANIM_PLAYER_RUN = 1;
    const int ANIM_PLAYER_SLIDE = 2;
    std::vector<Animation> playerAnims;

    std::vector<SDL_Texture *> textures;
    SDL_Texture *idle_texture, *run_texture, *brick, *grass, *ground, *panel, *sliding_texture, *background1, *background2, *background3, *background4;

    SDL_Texture *load_texture(SDL_Renderer *renderer, const std::string &filepath)
    {
        // Load game asset
        SDL_Texture *tex = IMG_LoadTexture(renderer, filepath.c_str());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        textures.push_back(tex);
        return tex;
    }

    void load(SDLState &state)
    {
        playerAnims.resize(5);
        playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 1.6f);
        playerAnims[ANIM_PLAYER_RUN] = Animation(4, 0.5f);
        playerAnims[ANIM_PLAYER_SLIDE] = Animation(1, 1.0f);

        idle_texture = load_texture(state.renderer, "../data/idle.png");
        run_texture = load_texture(state.renderer, "../data/run.png");
        sliding_texture = load_texture(state.renderer, "../data/slide.png");
        brick = load_texture(state.renderer, "../data/tiles/brick.png");
        grass = load_texture(state.renderer, "../data/tiles/grass.png");
        ground = load_texture(state.renderer, "../data/tiles/ground.png");
        panel = load_texture(state.renderer, "../data/tiles/panel.png");
        background1 = load_texture(state.renderer, "../data/bg/bg_layer1.png");
        background2 = load_texture(state.renderer, "../data/bg/bg_layer2.png");
        background3 = load_texture(state.renderer, "../data/bg/bg_layer3.png");
        background4 = load_texture(state.renderer, "../data/bg/bg_layer4.png");
    }

    void unload()
    {
        for (SDL_Texture *tex : textures)
        {
            SDL_DestroyTexture(tex);
        }
    }
};

// Function prototypes
void cleanup(SDLState &state);
bool initialise(SDLState &state);
void drawObject(const SDLState &state, GameState &gs, GameObject &obj, float deltaTime);
void update(const SDLState &state, GameState &gs, GameObject &obj, const Resources &res, float deltaTime);
void collisionResponse(const SDLState &state, GameState &gs, const Resources &res, const SDL_FRect &rectA, const SDL_FRect &rectB, const SDL_FRect &rectC, GameObject &a, GameObject &b, float deltaTime);
void checkCollision(const SDLState &state, GameState &gs, const Resources &res, GameObject &a, GameObject &b, float deltaTime);
void createTiles(const SDLState &state, GameState &gs, const Resources &res);
void handleKeyInput(const SDLState &state, GameState &gs, GameObject &obj, SDL_Scancode key, bool keyDown);
void drawParralaxBackground(SDL_Renderer *renderer, SDL_Texture *texture, float xVelocity, float &scrollPos, float scrollFactor, float deltaTime);

int main(int argc, char *argv[])
{
    // Initialize the SDL state structure.
    SDLState state;

    // Set the desired dimensions for the window and the logical rendering area.
    state.height = 720;
    state.width = 1280;
    state.logical_width = 640;
    state.logical_height = 360;

    // Initialise SDL, the window, and the renderer.
    if (!initialise(state))
    {
        return 1;
    }

    Resources res;
    res.load(state);

    // --- GAME DATA ---
    GameState gs(state);
    createTiles(state, gs, res);

    // --- DELTA TIME SETUP ---
    // Get the time at the start of the game.
    uint64_t prevTime = SDL_GetTicks();
    float fps_timer = 0;
    int fps_counter = 0;
    int last_fps = 0;

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

        // --- FPS Averaging Logic ---
        fps_timer += deltaTime;
        fps_counter++;
        if (fps_timer >= 1.0f) // Update the display once per second
        {
            last_fps = fps_counter;
            fps_counter = 0;
            fps_timer -= 1.0f;
        }

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
            case SDL_EVENT_KEY_DOWN:
                handleKeyInput(state, gs, gs.player(), state.event.key.scancode, true);
                break;
            case SDL_EVENT_KEY_UP:
                handleKeyInput(state, gs, gs.player(), state.event.key.scancode, false);
                break;
            }
        }

        // Update all objects
        for (auto &layer : gs.layers)
        {
            for (GameObject &obj : layer)
            {
                update(state, gs, obj, res, deltaTime);
                if (obj.currentAnimation != -1)
                {
                    obj.animations[obj.currentAnimation].step(deltaTime);
                }
            }
        }

        // --- RENDERING LOGIC ---
        // Set the draw color and clear the screen.
        SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
        SDL_RenderClear(state.renderer);

        // Calculating map view point
        gs.mapViewPort.x = (gs.player().position.x + TILE_SIZE / 2) - gs.mapViewPort.w / 2;

        // Draw Background Images
        SDL_RenderTexture(state.renderer, res.background1, nullptr, nullptr);
        drawParralaxBackground(state.renderer, res.background4, gs.player().velocity.x, gs.bg4scroll, 0.1f, deltaTime);
        drawParralaxBackground(state.renderer, res.background3, gs.player().velocity.x, gs.bg3scroll, 0.2f, deltaTime);
        drawParralaxBackground(state.renderer, res.background2, gs.player().velocity.x, gs.bg2scroll, 0.3f, deltaTime);

        // draw background tiles
        for (GameObject &obj : gs.backgroundTiles)
        {
            SDL_FRect dst{
                .x = obj.position.x - gs.mapViewPort.x,
                .y = obj.position.y,
                .w = static_cast<float>(obj.texture->w),
                .h = static_cast<float>(obj.texture->h)};
            SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);
        }

        // draw all objects
        for (auto &layer : gs.layers)
        {
            for (GameObject &obj : layer)
            {
                drawObject(state, gs, obj, deltaTime);
            }
        }

        // draw foreground tiles
        for (GameObject &obj : gs.foregroundTiles)
        {
            SDL_FRect dst{
                .x = obj.position.x - gs.mapViewPort.x,
                .y = obj.position.y,
                .w = static_cast<float>(obj.texture->w),
                .h = static_cast<float>(obj.texture->h)};
            SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);
        }

        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(state.renderer, 5, 5,
                            std::format("state: {}", static_cast<int>(gs.player().data.player.state)).c_str());

        SDL_SetRenderDrawColor(state.renderer, 0, 255, 0, 255);
        SDL_RenderDebugText(state.renderer, state.logical_width - 70, 0,
                            std::format("FPS: {}", last_fps).c_str());

        // Swap the buffers to display the new frame.
        SDL_RenderPresent(state.renderer);

        // --- END OF FRAME ---
        // Set the previous time to the current time for the next frame's calculation.
        prevTime = nowTime;
    }

    // --- CLEANUP AFTER LOOP ---
    res.unload();
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
    state.window = SDL_CreateWindow("Tutorial Game", state.width, state.height, SDL_WINDOW_RESIZABLE);
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

void drawObject(const SDLState &state, GameState &gs, GameObject &obj, float deltaTime)
{
    // Define the source and destination rectangles for rendering.
    const float spriteSize = 32;
    float srcX = obj.currentAnimation != -1 ? obj.animations[obj.currentAnimation].currentFrame() * spriteSize : 0.0f;

    SDL_FRect src{
        .x = srcX,
        .y = 0,
        .w = spriteSize,
        .h = spriteSize};

    SDL_FRect dst{
        .x = obj.position.x - gs.mapViewPort.x,
        .y = obj.position.y,
        .w = spriteSize,
        .h = spriteSize};

    SDL_FlipMode flipMode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // Render the texture to the screen.
    SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipMode);
}

void update(const SDLState &state, GameState &gs, GameObject &obj, const Resources &res, float deltaTime)
{
    if (obj.dynamic)
    {
        // Apply Some Gravity
        obj.velocity += glm::vec2(0, 500) * deltaTime;
    }

    if (obj.type == ObjectType::player)
    {
        float currentDirection = 0;
        float moveAmount = 0;

        // We will do +1 and -1 to make sure that if user has pressed both keys then it will negate each other
        if (state.keys[SDL_SCANCODE_A] || state.keys[SDL_SCANCODE_LEFT]) // Left Key
            currentDirection -= 1;
        if (state.keys[SDL_SCANCODE_D] || state.keys[SDL_SCANCODE_RIGHT]) // Right Direction
            currentDirection += 1;

        // If the user has pressed a key then assign the direction to the player
        if (currentDirection)
            obj.direction = currentDirection;

        switch (obj.data.player.state)
        {
        case PlayerState::idle:
            if (currentDirection)
            {
                obj.data.player.state = PlayerState::running;
            }
            else
            {
                // decelarate
                if (obj.velocity.x)
                {
                    // if the velocity is positive i.e. in the right direction we use a negative factor and vice-versa
                    const float factor = obj.velocity.x > 0 ? -1.5f : 1.5f;
                    float amount = factor * obj.acceleration.x * deltaTime;

                    // if velocity is already less than amount than further decelaration is not possible so we set it to 0
                    if (std::abs(obj.velocity.x) < std::abs(amount))
                        obj.velocity.x = 0;
                    else
                        obj.velocity.x += amount; // amount will be always inverse to velocity because of factor so we add it
                }
            }
            obj.texture = res.idle_texture;
            obj.currentAnimation = res.ANIM_PLAYER_IDLE;
            break;
        case PlayerState::running:
            if (!currentDirection)
            {
                obj.data.player.state = PlayerState::idle;
            }

            // moving in opposite direction will make a sliding
            if (obj.velocity.x * obj.direction < 0 && obj.grounded)
            {
                obj.texture = res.sliding_texture;
                obj.currentAnimation = res.ANIM_PLAYER_SLIDE;
            }
            else
            {
                obj.texture = res.run_texture;
                obj.currentAnimation = res.ANIM_PLAYER_RUN;
            }
            break;
        case PlayerState::jumping:
            obj.texture = res.run_texture;
            obj.currentAnimation = res.ANIM_PLAYER_RUN;
            break;
        }

        // This is to calculate velocity of the object
        obj.velocity += currentDirection * obj.acceleration * deltaTime;

        // If the velocity is greater than max speed than reduce it to max speed
        // we use absolute value because velocity can be negative for currentDirection = -1
        if (std::abs(obj.velocity.x) > obj.maxSpeedX)
            obj.velocity.x = obj.maxSpeedX * currentDirection;
    }
    obj.position += obj.velocity * deltaTime;

    // Handle Collision
    bool foundGround = false;
    for (auto &layers : gs.layers)
    {
        for (GameObject &objB : layers)
        {
            if (&obj != &objB)
            {
                checkCollision(state, gs, res, obj, objB, deltaTime);

                // grounded sensor
                SDL_FRect sensor{
                    .x = obj.position.x + obj.collider.x,
                    .y = obj.position.y + obj.collider.y + obj.collider.h,
                    .w = obj.collider.w,
                    .h = 1};

                SDL_FRect rectB{
                    .x = objB.position.x + objB.collider.x,
                    .y = objB.position.y + objB.collider.y,
                    .w = objB.collider.w,
                    .h = objB.collider.h};

                if (SDL_HasRectIntersectionFloat(&sensor, &rectB))
                {
                    foundGround = true;
                }
            }
        }
    }
    if (obj.grounded != foundGround)
    {
        // We are changing the state
        obj.grounded = foundGround;
        if (foundGround && obj.type == ObjectType::player)
        {
            obj.data.player.state = PlayerState::running;
        }
    }

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(state.renderer, 5, 20,
                        std::format("OBJ Grounded: {}", static_cast<int>(gs.player().grounded)).c_str());

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(state.renderer, 5, 35,
                        std::format("Found Ground: {}", static_cast<int>(foundGround)).c_str());
}

void collisionResponse(const SDLState &state, GameState &gs, const Resources &res, const SDL_FRect &rectA, const SDL_FRect &rectB, const SDL_FRect &rectC, GameObject &a, GameObject &b, float deltaTime)
{
    if (a.type == ObjectType::player)
    {
        switch (b.type)
        {
        case ObjectType::level:
        {
            if (rectC.w < rectC.h)
            {
                // Horizontal Collision
                if (a.velocity.x > 0)
                {
                    // We have a positive velocity, i.e. we are going in the right direction
                    a.position.x -= rectC.w;
                }
                else if (a.velocity.x < 0)
                {
                    // We have a negative velocity, i.e. we are going in the left direction
                    a.position.x += rectC.w;
                }
                a.velocity.x = 0;
            }
            else
            {
                // Vertical Collision
                if (a.velocity.y > 0)
                {
                    // We have a positive velocity, i.e. we are going in the downward direction
                    a.position.y -= rectC.h;
                }
                else if (a.velocity.y < 0)
                {
                    // We have a negative velocity, i.e. we are going in the upward direction
                    a.position.y += rectC.h;
                }
                a.velocity.y = 0;
            }
            break;
        }
        }
    }
}

void checkCollision(const SDLState &state, GameState &gs, const Resources &res, GameObject &a, GameObject &b, float deltaTime)
{
    SDL_FRect rectA{
        .x = a.position.x + a.collider.x,
        .y = a.position.y + a.collider.y,
        .w = a.collider.w,
        .h = a.collider.h};

    SDL_FRect rectB{
        .x = b.position.x + b.collider.x,
        .y = b.position.y + b.collider.y,
        .w = b.collider.w,
        .h = b.collider.h};

    SDL_FRect rectC{0}; // this is the rect that will check for collision

    if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
    {
        // Found intersection
        collisionResponse(state, gs, res, rectA, rectB, rectC, a, b, deltaTime);
    }
}

void createTiles(const SDLState &state, GameState &gs, const Resources &res)
{
    /*
    1 - Ground
    2 - Panel
    3 - Enemy
    4 - Player
    5 - Grass
    6 - Brick
    */
    short map[MAP_ROWS][MAP_COLUMNS] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 4, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };

    short foreground[MAP_ROWS][MAP_COLUMNS] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    short background[MAP_ROWS][MAP_COLUMNS] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    const auto loadMap = [&state, &res, &gs](short layer[MAP_ROWS][MAP_COLUMNS])
    {
        const auto createObject = [&state](int r, int c, SDL_Texture *tex, ObjectType type)
        {
            GameObject o;
            o.type = type;
            o.position = glm::vec2(c * TILE_SIZE, state.logical_height - (MAP_ROWS - r) * TILE_SIZE);
            o.texture = tex;
            o.collider = {.x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};
            return o;
        };

        for (int r = 0; r < MAP_ROWS; r++)
        {
            for (int c = 0; c < MAP_COLUMNS; c++)
            {
                switch (layer[r][c])
                {
                case 1:
                {
                    GameObject o = createObject(r, c, res.ground, ObjectType::level);
                    gs.layers[LAYER_IDX_LEVEL].push_back(o);
                    break;
                }
                case 2:
                {
                    GameObject o = createObject(r, c, res.panel, ObjectType::level);
                    gs.layers[LAYER_IDX_LEVEL].push_back(o);
                    break;
                }
                case 4: // This is the player cases
                {
                    GameObject player;
                    player = createObject(r, c, res.idle_texture, ObjectType::player);
                    player.data.player = PlayerData();
                    player.texture = res.idle_texture;
                    player.animations = res.playerAnims;
                    player.currentAnimation = res.ANIM_PLAYER_IDLE;
                    player.acceleration = glm::vec2(300, 0);
                    player.maxSpeedX = 100;
                    player.dynamic = true;
                    player.collider = {.x = 11, .y = 6, .w = 10, .h = 26};
                    gs.layers[LAYER_IDX_CHARACTERS].push_back(player);
                    gs.playerIndex = gs.layers[LAYER_IDX_CHARACTERS].size() - 1;
                    break;
                }
                case 5:
                {
                    GameObject o = createObject(r, c, res.grass, ObjectType::level);
                    gs.foregroundTiles.push_back(o);
                    break;
                }
                case 6:
                {
                    GameObject o = createObject(r, c, res.brick, ObjectType::level);
                    gs.backgroundTiles.push_back(o);
                    break;
                }
                }
            }
        }
    };
    loadMap(map);
    loadMap(background);
    loadMap(foreground);
    assert(gs.playerIndex != -1);
}

void handleKeyInput(const SDLState &state, GameState &gs, GameObject &obj, SDL_Scancode key, bool keyDown)
{
    const float JumpForce = -200.0f; // this is negative to make the player go up when they jump

    if (obj.type == ObjectType::player)
    {
        switch (obj.data.player.state)
        {
        case PlayerState::idle:
        {
            if (key == SDL_SCANCODE_SPACE && keyDown)
            {
                obj.data.player.state = PlayerState::jumping;
                obj.velocity.y += JumpForce;
            }
            break;
        }
        case PlayerState::running:
        {
            if (key == SDL_SCANCODE_SPACE && keyDown)
            {
                obj.data.player.state = PlayerState::jumping;
                obj.velocity.y += JumpForce;
            }
            break;
        }
        }
    }
}

void drawParralaxBackground(SDL_Renderer *renderer, SDL_Texture *texture, float xVelocity, float &scrollPos, float scrollFactor, float deltaTime)
{
    scrollPos -= xVelocity * scrollFactor * deltaTime;
    if (scrollPos <= -texture->w)
    {
        scrollPos = 0;
    }

    SDL_FRect dst{
        .x = scrollPos,
        .y = 68,
        .w = texture->w * 2.0f, // the width is 2 times because we want to draw the texture twice
        .h = static_cast<float>(texture->h)};

    SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}