#pragma once
#include "Player.h"
#include "Gate.h"
#include "JumpBall.h"
#include "Spike.h"

class CollisionManager
{
public:
	CollisionManager(Player* player, std::vector <Spike>* inObstacles, std::vector<Gate>* inGates, std::vector<JumpBall>* inJumpBalls, int* plane);
	bool spikeCollision();
	bool ballCollision();
	std::optional<Type> gateCollision();

private:
	Player* player;
	std::vector<Spike>* spike;
	std::vector<Gate>* gates;
	std::vector<JumpBall>* jumpBalls;
	int* currentPlane;
};