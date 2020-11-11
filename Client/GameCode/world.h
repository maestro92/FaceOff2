#pragma once

#include "../PlatformShared/platform_shared.h"
#include "../FaceOff2/memory.h"

struct Entity
{
	glm::vec3 pos;
	glm::vec3 dim;
};

struct World
{
	MemoryArena memoryArena;

	Entity entities[1024];
	int entityCount;
	int maxEntityCount;
};

void initWorld(World* world)
{
	// initlaize the game state  
	world->entityCount = 10;
	world->maxEntityCount = 1024;
	for (int i = 0; i < world->entityCount; i++)
	{
		world->entities[i].pos = glm::vec3(5 - i, 5 - i, 5 - i);
		world->entities[i].dim = glm::vec3(1);
		//			cout << i << ": " << gameState->entities[i].pos << std::endl;
	}
}