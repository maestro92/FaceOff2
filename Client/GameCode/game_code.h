#pragma once

#include "../PlatformShared/platform_shared.h"


#include <iostream>



// define memory for Push style stuff

using namespace std;

/*
#define PushStruct(Arena, type) (type*)PushSize
#define PushArray(Arena, count, type)
*/



struct Entity
{
	glm::dvec3 pos;
};

/*
struct memory_arena
{
	uint8* base;
	MemoryIndex size;
	MemoryIndex used;
};
*/


struct Camera
{
	glm::vec3 position;

};

// This is mirroring the sim_region struct in handmade_sim_region.h
struct GameState
{
	bool isInitalized;
	Entity entities[1024];
	int entityCount;
	int maxEntityCount;

	Camera camera;
};




#define PushRenderElement(gameRenderCommands, type) (type*)PushRenderElement_(gameRenderCommands, RenderEntryType_##type, sizeof(type))
void* PushRenderElement_(GameRenderCommands* commands, RenderEntryType type, uint32 size)
{
	void* result = 0;

	size += sizeof(RenderEntryHeader);
	if (commands->HasSpaceFor(size))
	{
		RenderEntryHeader* header = (RenderEntryHeader*)commands->CurrentPushBufferAt();
		header->type = type;
		result = (uint8*)header + sizeof(*header);

		commands->pushBufferSize += size;
		commands->numElements++;
	}
	else
	{

	}
	return result;
}

void PushCube(glm::dvec3 entityPosition, GameRenderCommands* gameRenderCommands)
{
	RenderEntryCube* cube = PushRenderElement(gameRenderCommands, RenderEntryCube);

}


void WorldTickAndRender(GameState* gameState, GameRenderCommands* gameRenderCommands)
{

	for (int i = 0; i < gameState->entityCount; i++)
	{
		Entity* entity = &gameState->entities[i];

		PushCube(entity->pos, gameRenderCommands);
	}
}


extern "C" __declspec(dllexport) void UpdateAndRender(GameMemory* gameMemory, GameInputState* gameInputState, GameRenderCommands* gameRenderCommands)
{
//	cout << "Update And Render-2" << endl;

	// so all my game state resides in memory
	
	// initalize, just init 20 randome entities

	// allocate memroy for entities

	if (gameInputState->moveForward.endedDown)
	{
		cout << "move forward" << endl;
	}
	if (gameInputState->moveLeft.endedDown)
	{
		cout << "move left" << endl;
	}
	if (gameInputState->moveRight.endedDown)
	{
		cout << "move right" << endl;
	}
	if (gameInputState->moveBack.endedDown)
	{
		cout << "move back" << endl;
	}


	GameState* gameState = (GameState*)gameMemory->permenentStorage;
	if (!gameState->isInitalized)
	{
		// intialize memory arena



		// initlaize the game state  
		gameState->entityCount = 10;
		gameState->maxEntityCount = 1024;
		for (int i = 0; i < gameState->entityCount; i++)
		{
			gameState->entities[i].pos = glm::dvec3(i, 0, i);
		}

		gameState->isInitalized = true;
	}

	WorldTickAndRender(gameState, gameRenderCommands);


	// render bitmaps

}


extern "C" __declspec(dllexport) void test1()
{

}


