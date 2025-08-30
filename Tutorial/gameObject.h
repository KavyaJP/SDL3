#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <SDL3/SDL.h>

#include "animation.h"

enum class ObjectType
{
    player,
    enemy,
    level
};

struct GameObject
{
    ObjectType type;
    glm::vec2 position, acceleration, velocity;
    float direction;

    std::vector<Animation> animations;
    int currentAnimation;

    SDL_Texture *texture;

    GameObject()
    {
        type = ObjectType::level;
        position = velocity = acceleration = glm::vec2(0, 0);
        direction = 1;
        currentAnimation = -1;
        texture = nullptr;
    }
};