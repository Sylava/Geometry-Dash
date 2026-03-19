#include "CollisionManager.h"
#include <iostream>

CollisionManager::CollisionManager(Player* inPlayer, std::vector<Spike>* inObstacles, std::vector<Gate>* inGates, std::vector<JumpBall>* inJumpBalls, int* plane)
{
    player = inPlayer;
    spike = inObstacles;
    gates = inGates;
    jumpBalls = inJumpBalls;
    currentPlane = plane;
}

bool CollisionManager::spikeCollision()
{
    for (auto& spike : *spike)
    {
        if (spike.getPlane() != *currentPlane)
            continue;

        if (player->getBounds().findIntersection(spike.getBounds()))
            return true;
    }
    return false;
}

std::optional<Type> CollisionManager::gateCollision()
{
    for (Gate& gate : *gates)
    {
        if (gate.getPlane() != *currentPlane)
            continue;

        if (player->getBounds().findIntersection(gate.getBounds()))
        {
            return gate.type;
        }
    }
    return std::nullopt;
}

bool CollisionManager::ballCollision()
{
    for (auto& ball : *jumpBalls)
    {
        if (ball.getPlane() != *currentPlane)
            continue;

        if (player->getBounds().findIntersection(ball.getBounds()))
            return true;
    }
    return false;
}