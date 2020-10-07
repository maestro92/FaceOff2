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

	// camera is viewing along -zAxis
	glm::vec3 xAxis;
	glm::vec3 yAxis;
	glm::vec3 zAxis;

	float pitch;
	void SetViewDirection(glm::vec3 viewDirection)
	{
		zAxis = -viewDirection;
	}

	glm::vec3 GetViewDirection()
	{
		return -zAxis;
	}

	// usually you just pass in the gluLookAtMatrix
	void SetOrientation(glm::mat4 cameraMatrix)
	{
		// Hack: Todo, get rid of this extra inverse
		xAxis = glm::vec3(cameraMatrix[0][0], cameraMatrix[0][1], cameraMatrix[0][2]);
		yAxis = glm::vec3(cameraMatrix[1][0], cameraMatrix[1][1], cameraMatrix[1][2]);
		zAxis = glm::vec3(cameraMatrix[2][0], cameraMatrix[2][1], cameraMatrix[2][2]);
	}
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

	bool mouseIsDebugMode;
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




// xyz coordinate system
void PushCoordinateSystem(GameRenderCommands* gameRenderCommands, TransformData* transformData, glm::vec3 origin, glm::vec3 dim)
{
	glm::vec3 xAxisEnd = origin + dim.x * glm::vec3(1, 0, 0);		glm::vec4 xAxisColor = glm::vec4(1, 0, 0, 1);
	glm::vec3 yAxisEnd = origin + dim.y * glm::vec3(0, 1, 0);		glm::vec4 yAxisColor = glm::vec4(0, 1, 0, 1);
	glm::vec3 zAxisEnd = origin + dim.z * glm::vec3(0, 0, 1);		glm::vec4 zAxisColor = glm::vec4(0, 0, 1, 1);

	RenderEntryColoredLines* entry = PushRenderElement(gameRenderCommands, ColoredLines);

	entry->masterVertexArrayOffset = gameRenderCommands->numVertex;
	entry->numLines = 6;

	TexturedVertex* vertexArray = &(gameRenderCommands->masterVertexArray[gameRenderCommands->numVertex]);
	vertexArray[0].position = origin;
	vertexArray[0].color = xAxisColor;

	vertexArray[1].position = xAxisEnd;
	vertexArray[1].color = xAxisColor;

	// y axis
	vertexArray[2].position = origin;
	vertexArray[2].color = yAxisColor;

	vertexArray[3].position = yAxisEnd;
	vertexArray[3].color = yAxisColor;

	// z axis
	vertexArray[4].position = origin;
	vertexArray[4].color = zAxisColor;

	vertexArray[5].position = zAxisEnd;
	vertexArray[5].color = zAxisColor;

	gameRenderCommands->numVertex += 6;
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

glm::mat4 LookAtMatrix(glm::vec3 eye, glm::vec3 direction)
{
	glm::vec3 up = glm::vec3(0, 1, 0);

	glm::mat4 result = glm::mat4(1.0);

	glm::vec3 xaxis = glm::cross(up, direction);
	xaxis = glm::normalize(xaxis);

	glm::vec3 yaxis = glm::cross(direction, xaxis);
	yaxis = glm::normalize(xaxis);

	result[0][0] = xaxis.x;
	result[1][0] = yaxis.x;
	result[2][0] = direction.x;

	result[0][1] = xaxis.y;
	result[1][1] = yaxis.y;
	result[2][1] = direction.y;

	result[0][2] = xaxis.z;
	result[1][2] = yaxis.z;
	result[2][2] = direction.z;
//	result = glm::mat4(1.0);
	return result;
}


// this is copying glm::lookat implementation, only that we are doing the inverse ourselves
glm::mat4 GetCameraMatrix(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up)
{
	glm::vec3 f = glm::normalize(center - eye);
	glm::vec3 u = glm::normalize(up);
	glm::vec3 s = glm::normalize(glm::cross(f, u));
	u = glm::cross(s, f);

	glm::mat4 result = glm::mat4(1.0);
	
	result[0][0] = s.x;
	result[0][1] = s.y;
	result[0][2] = s.z;

	result[1][0] = u.x;
	result[1][1] = u.y;
	result[1][2] = u.z;

	result[2][0] = -f.x;
	result[2][1] = -f.y;
	result[2][2] = -f.z;

	result[3][0] = eye.x;
	result[3][1] = eye.y;
	result[3][2] = eye.z;

	/*
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	
	Result[3][0] = -dot(s, eye);
	Result[3][1] = -dot(u, eye);
	Result[3][2] = dot(f, eye);
	*/
	return result;
}




void WorldTickAndRender(GameState* gameState, GameInputState* gameInputState, GameRenderCommands* gameRenderCommands, glm::ivec2 windowDimensions, bool isDebugMode)
{
	float angleXInDeg = 0;
	float angleYInDeg = 0;

	// float angleXInRad = 0;
	// float angleYInRad = 0;
	CameraEntity* cam = &gameState->camera;

	if (!isDebugMode)
	{
		float dx = gameInputState->mouseX - windowDimensions.x / 2;
		float dy = gameInputState->mouseY - windowDimensions.y / 2;
		
		if (dx != 0)
		{
			cout << "dx " << dx << endl;
		}
		if (dy != 0)
		{
			cout << ">>>>> dy " << dy << endl;
		}
			

		angleXInDeg = dx * 0.05f;
		angleYInDeg = dy * 0.05f;

		if (cam->pitch + angleYInDeg >= 179)
		{
			angleYInDeg = 179 - cam->pitch;
		}

		if (cam->pitch + angleYInDeg <= -179)
		{
			angleYInDeg = -179 - cam->pitch;
		}

		if (angleYInDeg != 0)
		{
			cout << "		angleYInDeg " << angleYInDeg << endl;
		}

		cam->pitch += angleYInDeg;

		if (angleYInDeg != 0)
		{
			cout << "		cam->Pitch " << cam->pitch << endl;
		}

		// angleXInRad = angleXInDeg * 3.14f / 180.0f;
		// angleYInRad = angleYInDeg * 3.14f / 180.0f;

		if (dx != 0)
		{
			cout << "angleXInDeg " << angleXInDeg << endl;
		}
		/*
		if (dy != 0)
		{
			cout << "		angleYInDeg " << angleYInDeg << endl;
		}
		*/
	}




	// rotate around x with dy then rotate around Y with dx
	glm::vec3 newViewDir = glm::vec3(glm::rotate(angleYInDeg, cam->xAxis) *
		glm::rotate(-angleXInDeg, cam->yAxis) * glm::vec4(gameState->camera.GetViewDirection(), 1));

	newViewDir = glm::normalize(newViewDir);
	//	cam->SetViewDirection(newViewDir);


		// cout << "gameState->camera.dir " << gameState->camera.dir << endl;


		// process input
	float stepSize = 0.5f;
	if (gameInputState->moveForward.endedDown)
	{
		gameState->camera.position += stepSize * newViewDir;
	}

	if (gameInputState->moveLeft.endedDown)
	{
		gameState->camera.position += stepSize * GetHorizontalVector(newViewDir, true);
	}

	if (gameInputState->moveRight.endedDown)
	{
		gameState->camera.position += stepSize * GetHorizontalVector(newViewDir, false);
	}

	if (gameInputState->moveBack.endedDown)
	{
		gameState->camera.position += -stepSize * newViewDir;
	}
	// update camera


	// update camera matrix
	glm::vec3 center = gameState->camera.position + newViewDir;
	//	glm::mat4 viewMatrix = glm::lookAt(gameState->camera.position, center, glm::vec3(0, 1, 0));

	glm::vec3 supportUpVector = glm::vec3(0, 1, 0);
	if (glm::dot(newViewDir, supportUpVector) == 1)
	{
		supportUpVector = cam->yAxis;
	}
//	cout << " supportUpVector " << supportUpVector << endl;

	glm::mat4 cameraMatrix = GetCameraMatrix(gameState->camera.position, center, supportUpVector);
	cam->SetOrientation(cameraMatrix);


//	cout << "camera pos " << gameState->camera.position << endl;
//	cout << "view direction " << cam->GetViewDirection() << endl;
//	cout << cameraMatrix << endl;

	TransformData transformData = {};

	glm::mat4 cameraTransform = glm::translate(gameState->camera.position);// *cameraRot;
	float dim = 20;
//	glm::mat4 cameraProj = glm::ortho(-dim, dim, -dim, dim);
	glm::mat4 cameraProj = glm::perspective(45.0f, 800.0f / 640.0f, 0.5f, 1000.0f);

//	transformData.cameraTransform = cameraProj * glm::inverse(cameraTransform);
	transformData.cameraTransform = cameraProj * glm::inverse(cameraMatrix);;

	

	gameRenderCommands->cameraProjectionMatrix = cameraProj;
	gameRenderCommands->cameraTransformMatrix = cameraTransform;
	gameRenderCommands->transformMatrix = transformData.cameraTransform;


	for (int i = 0; i < gameState->entityCount; i++)
	{
		Entity* entity = &gameState->entities[i];

		PushCube(gameRenderCommands, &transformData, entity->pos, glm::vec3(1, 1, 1));
	}

	float scale = 200;
	PushCoordinateSystem(gameRenderCommands, &transformData, glm::vec3(0, 0, 0), glm::vec3(scale, scale, scale));

}


extern "C" __declspec(dllexport) void UpdateAndRender(GameMemory* gameMemory, GameInputState* gameInputState, GameRenderCommands* gameRenderCommands, 
														glm::ivec2 windowDimensions, bool isDebugMode)
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
			gameState->entities[i].pos = glm::vec3(5 - i, 5 - i, 5 - i);
			//			cout << i << ": " << gameState->entities[i].pos << std::endl;
 		}

		gameState->camera = {};
		gameState->camera.position = glm::vec3(0, 5, 20);
		gameState->camera.xAxis = glm::vec3(1.0, 0.0, 0.0);
		gameState->camera.yAxis = glm::vec3(0.0, 1.0, 0.0);
		gameState->camera.zAxis = glm::vec3(0.0, 0.0, 1.0);
		
		gameState->mouseIsDebugMode = false;
		gameState->isInitalized = true;
	}

	WorldTickAndRender(gameState, gameInputState, gameRenderCommands, windowDimensions, isDebugMode);


	// render bitmaps

}


extern "C" __declspec(dllexport) void test1()
{

}


