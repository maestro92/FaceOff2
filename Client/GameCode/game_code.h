#pragma once

#include "../PlatformShared/platform_shared.h"


#include <iostream>



// define memory for Push style stuff

using namespace std;

/*
#define PushStruct(Arena, type) (type*)PushSize
#define PushArray(Arena, count, type)
*/


struct CameraEntity
{
	glm::vec3 position;
	glm::vec3 dir;
};

struct Entity
{
	glm::vec3 pos;
};

/*
struct memory_arena
{
	uint8* base;
	MemoryIndex size;
	MemoryIndex used;
};
*/



// This is mirroring the sim_region struct in handmade_sim_region.h
struct GameState
{
	bool isInitalized;
	Entity entities[1024];
	int entityCount;
	int maxEntityCount;

	CameraEntity camera;
};

struct TransformData
{
	glm::mat4 cameraTransform;
};


#define PushRenderElement(gameRenderCommands, type) (RenderEntry##type*)PushRenderElement_(gameRenderCommands, RenderEntryType_##type, sizeof(RenderEntry##type))
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

// p0 p1 p2 p3 in clock wise order
void PushQuad(GameRenderCommands* gameRenderCommands, TransformData* transformData, int texture,
													glm::vec3 p0, glm::vec2 uv0, glm::vec4 color0,
												    glm::vec3 p1, glm::vec2 uv1, glm::vec4 color1,
													glm::vec3 p2, glm::vec2 uv2, glm::vec4 color2,
													glm::vec3 p3, glm::vec2 uv3, glm::vec4 color3)
{
	RenderEntryTexturedQuads* entry = PushRenderElement(gameRenderCommands, TexturedQuads);

	entry->masterVertexArrayOffset = gameRenderCommands->numVertex;
	entry->numQuads = 1;

	TexturedVertex* vertexArray = &(gameRenderCommands->masterVertexArray[gameRenderCommands->numVertex]);
	vertexArray[0].position = p0;
	vertexArray[0].normal = p0;
	vertexArray[0].uv = uv0;
	vertexArray[0].color = color0;

	vertexArray[1].position = p1;
	vertexArray[1].normal = p1;
	vertexArray[1].uv = uv1;
	vertexArray[1].color = color1;

	// Note that vertexArray[2] and vertexArray[3] has the points swapped since we want to
	// draw this with trangle strips.
	vertexArray[2].position = p3;
	vertexArray[2].normal = p3;
	vertexArray[2].uv = uv3;
	vertexArray[2].color = color3;

	vertexArray[3].position = p2;
	vertexArray[3].normal = p2;
	vertexArray[3].uv = uv2;
	vertexArray[3].color = color2;

	gameRenderCommands->numVertex += 4;
}


void PushCube(GameRenderCommands* gameRenderCommands, TransformData* transformData, glm::vec3 entityPosition, glm::vec3 dim)
{
	// RenderEntryCube* cube = PushRenderElement(gameRenderCommands, RenderEntryCube);

	// push the 6 sides
	glm::vec3 min = entityPosition - dim;
	glm::vec3 max = entityPosition + dim;


	/*
		y
		^		 
	  (-x,y,-z) p4 ------------ p5 (x,y,-z) 
		|		|\              |\
		|		| \             | \
		|		| (-x,y,z)      |  \
		|		|	p0 ------------ p1 (x,y,z)
		|	    p6 -|----------	p7	|
	   (-x,-y,-z)\  |	(x,-y,-z)\	|
		|		  \	|		      \ |
		|		   \|			   \|
		|			p2 ------------ p3 (x,-y,z)
		|         (-x,-y,z) 
		|			
		------------------------------------------> x
	    \
		 \
		  \
		   V z
	*/

	// 4 points on front face 
	glm::vec3 p0 = glm::vec3(min.x, max.y, max.z);
	glm::vec3 p1 = glm::vec3(max.x, max.y, max.z);
	glm::vec3 p2 = glm::vec3(min.x, min.y, max.z);
	glm::vec3 p3 = glm::vec3(max.x, min.y, max.z);

	// 4 points on back face 
	glm::vec3 p4 = glm::vec3(min.x, max.y, min.z);
	glm::vec3 p5 = glm::vec3(max.x, max.y, min.z);
	glm::vec3 p6 = glm::vec3(min.x, min.y, min.z);
	glm::vec3 p7 = glm::vec3(max.x, min.y, min.z);

	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(1, 0);
	glm::vec2 t2 = glm::vec2(0, 1);
	glm::vec2 t3 = glm::vec2(1, 1);

	glm::vec4 c0 = glm::vec4(1, 0, 0, 1);
	glm::vec4 c1 = glm::vec4(0, 1, 0, 1);
	glm::vec4 c2 = glm::vec4(0, 0, 1, 1);
	glm::vec4 c3 = glm::vec4(1, 1, 1, 1);

	// front
	PushQuad(gameRenderCommands, transformData, 0,	p0, t0, c0,
													p1, t1, c1,
													p3, t3, c3,
													p2, t2, c2);
	// top
	PushQuad(gameRenderCommands, transformData, 0,	p4, t0, c0,
													p5, t1, c1,
													p1, t3, c3,
													p0, t2, c2);
	// left 
	PushQuad(gameRenderCommands, transformData, 0,	p4, t0, c0,
													p0, t1, c1,
													p2, t3, c3,
													p6, t2, c2);
	// bottom
	PushQuad(gameRenderCommands, transformData, 0,	p2, t0, c0,
													p3, t1, c1,
													p7, t3, c3,
													p6, t2, c2);
	// right 
	PushQuad(gameRenderCommands, transformData, 0,	p1, t0, c0,
													p5, t1, c1,
													p7, t3, c3,
													p3, t2, c2);
	// back
	PushQuad(gameRenderCommands, transformData, 0,	p5, t0, c0,
													p4, t1, c1,
													p6, t3, c3,
													p7, t2, c2);
}




glm::vec3 GetHorizontalVector(glm::vec3 dir, bool left)
{
	glm::vec3 supportingVector = glm::vec3(0, 1, 0);

	if (glm::dot(dir, supportingVector) == 1)
	{
		supportingVector = glm::vec3(0, 0, 1);
	}

	glm::vec3 result = -glm::cross(dir, supportingVector);
	if (!left)
	{
		result = -result;
	}
	
	return result;
}


void WorldTickAndRender(GameState* gameState, GameInputState* gameInputState, GameRenderCommands* gameRenderCommands)
{
	// process input
	float stepSize = 0.05f;
	if (gameInputState->moveForward.endedDown)
	{
		gameState->camera.position += stepSize * gameState->camera.dir;
	}

	if (gameInputState->moveLeft.endedDown)
	{
		gameState->camera.position += stepSize * GetHorizontalVector(gameState->camera.dir, true);
	}

	if (gameInputState->moveRight.endedDown)
	{
		gameState->camera.position += stepSize * GetHorizontalVector(gameState->camera.dir, false);
	}

	if (gameInputState->moveBack.endedDown)
	{
		gameState->camera.position += -stepSize * gameState->camera.dir;
	}
	cout << "camera pos " << gameState->camera.position << endl;
	// update camera


	// update camera matrix

	TransformData transformData = {};

	glm::mat4 cameraTransform = glm::translate(gameState->camera.position);
	float dim = 20;
//	glm::mat4 cameraProj = glm::ortho(-dim, dim, -dim, dim);
	glm::mat4 cameraProj = glm::perspective(45.0f, 800.0f / 640.0f, 0.5f, 1000.0f);

	transformData.cameraTransform = cameraProj * glm::inverse(cameraTransform);

	gameRenderCommands->cameraProjectionMatrix = cameraProj;
	gameRenderCommands->cameraTransformMatrix = cameraTransform;
	gameRenderCommands->transformMatrix = transformData.cameraTransform;


	for (int i = 0; i < gameState->entityCount; i++)
	{
		Entity* entity = &gameState->entities[i];

		PushCube(gameRenderCommands, &transformData, entity->pos, glm::vec3(1, 1, 1));
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
			gameState->entities[i].pos = glm::dvec3(5 - i, 5 - i, 5 - i);
			//			cout << i << ": " << gameState->entities[i].pos << std::endl;
 		}

		gameState->camera.position = glm::dvec3(0, 5, 20);
		gameState->camera.dir = glm::dvec3(0, 0, -1);

		gameState->isInitalized = true;
	}

	WorldTickAndRender(gameState, gameInputState, gameRenderCommands);


	// render bitmaps

}


extern "C" __declspec(dllexport) void test1()
{

}


