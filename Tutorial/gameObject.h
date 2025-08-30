#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <SDL3/SDL.h>

#include "animation.h"

enum class PlayerState
{
    idle,
    running,
    jumping
};

struct PlayerData
{
    PlayerState state;

    PlayerData()
    {
        state = PlayerState::idle;
    }
};

struct EnemyData
{
};

struct LevelData
{
};

union ObjectData
{
    PlayerData player;
    LevelData level;
    EnemyData enemy;
};

enum class ObjectType
{
    player,
    enemy,
    level
};

struct GameObject
{
    ObjectType type;
    ObjectData data;
    glm::vec2 position, acceleration, velocity;

    float direction;
    float maxSpeedX;

    std::vector<Animation> animations;
    int currentAnimation;

    SDL_Texture *texture;

    GameObject() : data{.level = LevelData()}
    {
        type = ObjectType::level;
        position = velocity = acceleration = glm::vec2(0, 0);
        direction = 1;
        maxSpeedX = 0;
        currentAnimation = -1;
        texture = nullptr;
    }
};