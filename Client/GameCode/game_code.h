#pragma once

#include "../PlatformShared/platform_shared.h"
#include "debug_interface.h"
#include "memory.h"
#include "../FaceOff2/asset.h"
#include "debug.h"

#include <iostream>



// define memory for Push style stuff

using namespace std;


// typedef void(*PlatformLoadTexture)(GameAssets* gameAssets, BitmapId bitmapId);

glm::vec4 COLOR_WHITE = glm::vec4(1, 1, 1, 1);

static PlatformAPI platformAPI;

static FontId debugFontId;
static LoadedFont* debugLoadedFont;
static glm::mat4 globalDebugCameraMat;



static DebugTable* GlobalDebugTable;
int MAX_DEBUG_EVENT_ARRAY_COUNT = 8;

struct CameraEntity
{
	// consider storing these 4 as a matrix?
	// then add accessors
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





// This is mirroring the sim_region struct in handmade_sim_region.h
struct GameState
{
	bool isInitalized;
	Entity entities[1024];
	int entityCount;
	int maxEntityCount;

	CameraEntity camera;

	bool mouseIsDebugMode;

	MemoryArena memoryArena;
};

struct TransientState
{
	bool isInitalized;
	MemoryArena memoryArena;
	GameAssets* assets;

};




#define PushRenderElement(gameRenderCommands, type) (RenderGroupEntry##type*)PushRenderElement_(gameRenderCommands, RenderGroupEntryType_##type, sizeof(RenderGroupEntry##type))
void* PushRenderElement_(GameRenderCommands* commands, RenderGroupEntryType type, uint32 size)
{
	void* result = 0;

	size += sizeof(RenderEntryHeader);
	if (commands->HasSpaceFor(size))
	{
		RenderEntryHeader* header = (RenderEntryHeader*)commands->CurrentPushBufferAt();
		header->type = type;
		result = (uint8*)header + sizeof(*header);

		commands->pushBufferSize += size;
		commands->numRenderGroups++;
	}
	else
	{

	}


	return result;
}

// p0 p1 p2 p3 in clock wise order
void PushQuad_Core(GameRenderCommands* gameRenderCommands, RenderGroup* renderGroup, LoadedBitmap* bitmap,
															glm::vec3 p0, glm::vec2 uv0, glm::vec4 color0,
															glm::vec3 p1, glm::vec2 uv1, glm::vec4 color1,
															glm::vec3 p2, glm::vec2 uv2, glm::vec4 color2,
															glm::vec3 p3, glm::vec2 uv3, glm::vec4 color3)
{
	if (gameRenderCommands->HasSpaceForVertex(4))
	{
		RenderGroupEntryTexturedQuads* entry = renderGroup->quads;
		entry->numQuads++;

		assert(gameRenderCommands->numBitmaps < gameRenderCommands->maxNumBitmaps);
		int index = gameRenderCommands->numBitmaps;
		gameRenderCommands->masterBitmapArray[index] = bitmap;
		gameRenderCommands->numBitmaps++;


	//	cout << "entry masterBitmapArray " << index << " " << bitmap->textureHandle << endl;
	//	cout << "gameRenderCommands->numBitmaps " << gameRenderCommands->numBitmaps << endl;

		int index2 = gameRenderCommands->numVertex;
		TexturedVertex* vertexArray = &(gameRenderCommands->masterVertexArray[index2]);
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
}


enum AlignmentMode
{
	Centered,
	Left,
	Right,
	Top,
	Bottom
};

void PushBitmap(GameRenderCommands* gameRenderCommands, 
					RenderGroup* renderGroup, 
					LoadedBitmap* bitmap, 
					glm::vec4 color, 
					glm::vec3 position, 
					glm::vec3 halfDim, AlignmentMode hAlignment, AlignmentMode vAlignment)
{
	glm::vec3 min = position;
	glm::vec3 max = position;
	switch (hAlignment)
	{
		case AlignmentMode::Centered:
			min.x -= halfDim.x;
			max.x += halfDim.x;
			break;
		case AlignmentMode::Left:
			max.x += halfDim.x * 2;
			break;
		case AlignmentMode::Right:
			min.x -= halfDim.x * 2;
			break;
		default:
			break;
	}

	switch (vAlignment)
	{
		case AlignmentMode::Centered:
			min.y -= halfDim.y;
			max.y += halfDim.y;
			break;
		case AlignmentMode::Top:
			min.y -= halfDim.y * 2;
			break;
		case AlignmentMode::Bottom:
			max.y += halfDim.y * 2;
			break;
		default:
			break;
	}

	glm::vec3 p0 = glm::vec3(min.x, max.y, max.z);
	glm::vec3 p1 = glm::vec3(max.x, max.y, max.z);
	glm::vec3 p2 = glm::vec3(min.x, min.y, max.z);
	glm::vec3 p3 = glm::vec3(max.x, min.y, max.z);

	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(1, 0);
	glm::vec2 t2 = glm::vec2(0, 1);
	glm::vec2 t3 = glm::vec2(1, 1);

	glm::vec4 c0 = color;
	glm::vec4 c1 = color;
	glm::vec4 c2 = color;
	glm::vec4 c3 = color;


	// front
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p0, t0, c0,
															p1, t1, c1,
															p3, t3, c3,
															p2, t2, c2);
}



void PushCube(GameRenderCommands* gameRenderCommands, RenderGroup* renderGroup, LoadedBitmap* bitmap, glm::vec4 color, glm::vec3 entityPosition, glm::vec3 dim)
{
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

	glm::vec4 c0 = color;
	glm::vec4 c1 = color;
	glm::vec4 c2 = color;
	glm::vec4 c3 = color;

	// front
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p0, t0, c0,
															p1, t1, c1,
															p3, t3, c3,
															p2, t2, c2);
	// top
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p4, t0, c0,
															p5, t1, c1,
															p1, t3, c3,
															p0, t2, c2);
	// left 
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p4, t0, c0,
															p0, t1, c1,
															p2, t3, c3,
															p6, t2, c2);
	// bottom
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p2, t0, c0,
															p3, t1, c1,
															p7, t3, c3,
															p6, t2, c2);
	// right 
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p1, t0, c0,
															p5, t1, c1,
															p7, t3, c3,
															p3, t2, c2);
	// back
	PushQuad_Core(gameRenderCommands, renderGroup, bitmap,	p5, t0, c0,
															p4, t1, c1,
															p6, t3, c3,
															p7, t2, c2);
}	




// xyz coordinate system
void PushCoordinateSystem(GameRenderCommands* gameRenderCommands, RenderGroup* group, LoadedBitmap* bitmap, glm::vec3 origin, glm::vec3 dim)
{
	glm::vec3 xAxisEnd = origin + dim.x * glm::vec3(1, 0, 0);		glm::vec4 xAxisColor = glm::vec4(1, 0, 0, 1);
	glm::vec3 yAxisEnd = origin + dim.y * glm::vec3(0, 1, 0);		glm::vec4 yAxisColor = glm::vec4(0, 1, 0, 1);
	glm::vec3 zAxisEnd = origin + dim.z * glm::vec3(0, 0, 1);		glm::vec4 zAxisColor = glm::vec4(0, 0, 1, 1);

	glm::vec3 xAxisMiddlePos = (xAxisEnd + origin) / 2.0f;
	glm::vec3 yAxisMiddlePos = (yAxisEnd + origin) / 2.0f;
	glm::vec3 zAxisMiddlePos = (zAxisEnd + origin) / 2.0f;

//	xAxisColor = COLOR_WHITE;
//	yAxisColor = COLOR_WHITE;
//	zAxisColor = COLOR_WHITE;

	float cubeThickness = 0.05f;
	PushCube(gameRenderCommands, group, bitmap, xAxisColor, xAxisMiddlePos, glm::vec3(dim.x / 2, cubeThickness, cubeThickness));
	PushCube(gameRenderCommands, group, bitmap, yAxisColor, yAxisMiddlePos, glm::vec3(cubeThickness, dim.y / 2, cubeThickness));
	PushCube(gameRenderCommands, group, bitmap, zAxisColor, zAxisMiddlePos, glm::vec3(cubeThickness, cubeThickness, dim.z / 2));

/*
		glm::vec3 xAxisEnd = origin + dim.x * glm::vec3(1, 0, 0);		glm::vec4 xAxisColor = glm::vec4(1, 0, 0, 1);
		glm::vec3 yAxisEnd = origin + dim.y * glm::vec3(0, 1, 0);		glm::vec4 yAxisColor = glm::vec4(0, 1, 0, 1);
		glm::vec3 zAxisEnd = origin + dim.z * glm::vec3(0, 0, 1);		glm::vec4 zAxisColor = glm::vec4(0, 0, 1, 1);

		RenderEntryColoredLines* entry = PushRenderElement(gameRenderCommands, ColoredLines);
		entry->renderSetup = *renderSetup;
		entry->masterVertexArrayOffset = gameRenderCommands->numVertex;
		entry->masterBitmapArrayOffset = gameRenderCommands->numBitmaps;
		entry->numLinePoints = 6;

		assert(gameRenderCommands->numBitmaps < gameRenderCommands->maxNumBitmaps);
		int index = gameRenderCommands->numBitmaps;
		gameRenderCommands->masterBitmapArray[index] = bitmap;
		gameRenderCommands->numBitmaps++;


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
*/
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




void WorldTickAndRender(GameState* gameState, GameAssets* gameAssets, 
						GameInputState* gameInputState, GameRenderCommands* gameRenderCommands, glm::ivec2 windowDimensions, bool isDebugMode)
{
	float angleXInDeg = 0;
	float angleYInDeg = 0;

	// float angleXInRad = 0;
	// float angleYInRad = 0;
	CameraEntity* cam = &gameState->camera;

	if (!isDebugMode)
	{
		float dx = gameInputState->mousePos.x - windowDimensions.x / 2;
		float dy = gameInputState->mousePos.y - windowDimensions.y / 2;
		
		/*
		if (dx != 0)
		{
			cout << "dx " << dx << endl;
		}
		if (dy != 0)
		{
			cout << ">>>>> dy " << dy << endl;
		}
			*/

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

		/*
		if (angleYInDeg != 0)
		{
			cout << "		angleYInDeg " << angleYInDeg << endl;
		}
		*/
		cam->pitch += angleYInDeg;

		/*
		if (angleYInDeg != 0)
		{
			cout << "		cam->Pitch " << cam->pitch << endl;
		}
		*/
		// angleXInRad = angleXInDeg * 3.14f / 180.0f;
		// angleYInRad = angleYInDeg * 3.14f / 180.0f;

		/*
		if (dx != 0)
		{
			cout << "angleXInDeg " << angleXInDeg << endl;
		}
		*/
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
	glm::vec3 supportUpVector = glm::vec3(0, 1, 0);
	if (glm::dot(newViewDir, supportUpVector) == 1)
	{
		supportUpVector = cam->yAxis;
	}


	glm::mat4 cameraMatrix = GetCameraMatrix(gameState->camera.position, center, supportUpVector);
	cam->SetOrientation(cameraMatrix);

	globalDebugCameraMat = cameraMatrix;

	glm::mat4 cameraTransform = glm::translate(gameState->camera.position);// *cameraRot;
	float dim = 20;
	glm::mat4 cameraProj = glm::perspective(45.0f, 800.0f / 640.0f, 0.5f, 1000.0f);


	// We start a render setup
	RenderGroup group = {};

	RenderSetup renderSetup = {};
//	renderSetup.cameraProjectionMatrix = cameraProj;
//	renderSetup.cameraTransformMatrix = cameraTransform;
	renderSetup.transformMatrix = cameraProj * glm::inverse(cameraMatrix);

	group.quads = PushRenderElement(gameRenderCommands, TexturedQuads);
	*group.quads = {};
	group.quads->masterVertexArrayOffset = gameRenderCommands->numVertex;
	group.quads->masterBitmapArrayOffset = gameRenderCommands->numBitmaps;
	group.quads->renderSetup = renderSetup;




	for (int i = 0; i < gameState->entityCount; i++)
	{
		Entity* entity = &gameState->entities[i];

		BitmapId bitmapID2 = GetFirstBitmapIdFrom(gameAssets, AssetFamilyType::Wall);
		LoadedBitmap* bitmap2 = GetBitmap(gameAssets, bitmapID2);
		PushCube(gameRenderCommands, &group, bitmap2, COLOR_WHITE, entity->pos, glm::vec3(1, 1, 1));
	}

	float scale = 200;
	BitmapId bitmapID = GetFirstBitmapIdFrom(gameAssets, AssetFamilyType::Default);
	LoadedBitmap* bitmap = GetBitmap(gameAssets, bitmapID);
	PushCoordinateSystem(gameRenderCommands, &group, bitmap, glm::vec3(0, 0, 0), glm::vec3(scale, scale, scale));
}


extern DebugTable* globalDebugTable;


extern "C" __declspec(dllexport) void GameUpdateAndRender(GameMemory* gameMemory, GameInputState* gameInputState, GameRenderCommands* gameRenderCommands,
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

	globalDebugTable = gameMemory->debugTable;


	GameState* gameState = (GameState*)gameMemory->permenentStorage;
	if (!gameState->isInitalized)
	{
		// intialize memory arena
		platformAPI = gameMemory->platformAPI;


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


		uint8* base = (uint8*)gameMemory->permenentStorage + sizeof(GameState);
		MemoryIndex size = gameMemory->permenentStorageSize - sizeof(GameState);
		gameState->memoryArena.Init(base, size);

		gameState->isInitalized = true;


		// CallOpenGL To Load Textures
	}

	TransientState* transientState = (TransientState*)gameMemory->transientStorage;
	if (!transientState->isInitalized)
	{
		uint8* base = (uint8*)gameMemory->transientStorage + sizeof(TransientState);
		MemoryIndex size = gameMemory->transientStorageSize - sizeof(TransientState);
		transientState->memoryArena.Init(base, size);

		transientState->assets = PushStruct(&transientState->memoryArena, GameAssets);
		AllocateGameAssets(&transientState->memoryArena, transientState->assets);

		debugFontId = { GetFirstAssetIdFrom(transientState->assets, AssetFamilyType::Enum::Font) };
		debugLoadedFont = GetFont(transientState->assets, debugFontId);


		transientState->isInitalized = true;
	}

	/*
	for (int i = 0; i < 200; i++)
	{
		int a = 1;
		std::cout << "nice " << std::endl;
	}
	*/

	WorldTickAndRender(gameState, transientState->assets, gameInputState, gameRenderCommands, windowDimensions, isDebugMode);
}



void AllocateDebugFrame(DebugFrame* debugFrame, MemoryArena* memoryArena)
{
	debugFrame->beginClock = 0;
	debugFrame->endClock = 0;
	debugFrame->wallSecondsElapsed = 0.0f;
	debugFrame->rootProfileNode = new ProfileNode(-1);

	std::cout << "debugFrame->rootProfileNode " << debugFrame->rootProfileNode << std::endl;
}


void AllocateDebugFrames(DebugState* debugState)
{
	debugState->frames = PushArray(&debugState->collationArena, MAX_DEBUG_EVENT_ARRAY_COUNT, DebugFrame);
	debugState->numFrames = 0;
	debugState->maxFrames = MAX_DEBUG_EVENT_ARRAY_COUNT;
	debugState->collationFrame = 0;

	// allocating DebugFrame
	for (int i = 0; i < MAX_DEBUG_EVENT_ARRAY_COUNT; i++)
	{
		AllocateDebugFrame(&debugState->frames[i], &debugState->collationArena);
	}
}


void RestartCollation(DebugState* debugState)
{
	debugState->numFrames = 0;
	debugState->collationFrame = 0;
}

void RenderProfileBars(DebugState* debugState, GameRenderCommands* gameRenderCommands, 
						RenderGroup* renderGroup, GameAssets* gameAssets, glm::ivec2 mousePos)
{

	// we have lanes for our threads 
	// handmade hero day254 24:52
	float halfWidth = gameRenderCommands->settings.dims.x / 2.0f;
	float halfHeight = gameRenderCommands->settings.dims.y / 2.0f;

	BitmapId bitmapID = GetFirstBitmapIdFrom(gameAssets, AssetFamilyType::Default);
	LoadedBitmap* defaultBitmap = GetBitmap(gameAssets, bitmapID);

	glm::vec3 profileRectDim = glm::vec3(200, 100, 0);
	glm::vec3 profileRectMin = glm::vec3(-halfWidth, halfHeight - profileRectDim.y, 0);
	glm::vec3 profileRectMax = profileRectMin + profileRectDim;
	// background
	PushBitmap(gameRenderCommands, renderGroup, defaultBitmap, glm::vec4(0, 0, 0.25, 0.25), profileRectMin,
													glm::vec3(profileRectDim.x / 2.0, profileRectDim.y / 2.0, 0), AlignmentMode::Left, AlignmentMode::Bottom);

	//cout << "RenderProfileBars " << endl;
	if (debugState->mostRecentFrame != NULL)
	{
		debugState->mostRecentFrame->PrintDebug();


		ProfileNode* root = debugState->mostRecentFrame->rootProfileNode;

		float frameSpan = root->duration;
		float pixelSpan = profileRectDim.x;

		float scale = 0.0f;
		if (frameSpan > 0)
		{
			scale = pixelSpan / frameSpan;
		}


		uint32 numLanes = debugState->threads.size();
		float laneHeight = 0.0f;
		if (numLanes > 0)
		{
			laneHeight = profileRectDim.y / numLanes;
		}



		glm::vec4 colors[] = {
			glm::vec4(1,0,0,1),
			glm::vec4(0,1,0,1),
			glm::vec4(0,0,1,1),

			glm::vec4(1,1,0,1),
			glm::vec4(0,1,1,1),
			glm::vec4(1,0,1,1)
		};


		cout << "root->children " << root->children.size() << endl;
		// the more recent ones are at the top
		for (uint32 i = 0; i < root->children.size(); i++)
		{
			ProfileNode* node = root->children[i];
			glm::vec4 color = colors[i % ArrayCount(colors)];
			
			glm::vec3 rectMin, rectMax;

			rectMin.x = profileRectMin.x + scale * node->parentRelativeClock;
			rectMax.x = rectMin.x + scale * node->duration;

			uint32 laneIndex = 0;
			rectMin.y = profileRectMax.y - (laneIndex + 1) * laneHeight;
			rectMax.y = profileRectMax.y - laneIndex * laneHeight;
			
			glm::vec3 dim = rectMax - rectMin;

			float zOffset = 1;

			std::cout << "rectMin " << rectMin << std::endl;
			std::cout << "rectMax " << rectMax << std::endl;

			std::cout << "dim " << dim << std::endl;
		//	std::cout << "node->parentRelativeClock " << node->parentRelativeClock;
		//	std::cout << "node->duration " << node->duration;

			PushBitmap(gameRenderCommands, renderGroup, defaultBitmap, color, glm::vec3(rectMin.x, rectMin.y, zOffset),
					glm::vec3(dim.x / 2.0, dim.y / 2.0, 0), AlignmentMode::Left, AlignmentMode::Bottom);
		
			// if mouse in region
		
		}
	}

}


extern "C" __declspec(dllexport) void DebugSystemUpdateAndRender(GameMemory* gameMemory, 
																GameInputState* gameInputState, 
																GameRenderCommands* gameRenderCommands,
																glm::ivec2 windowDimensions, bool isDebugMode)
{
	DebugState* debugState = (DebugState*)gameMemory->debugStorage;
	if (!debugState->isInitalized)
	{
		uint64 collationArenaSize = Megabytes(32);

		uint8* debugArenaBase = (uint8*)gameMemory->debugStorage + sizeof(DebugState);
		MemoryIndex debugArenaSize = gameMemory->debugStorageSize - sizeof(DebugState) - collationArenaSize;
		debugState->debugArena.Init(debugArenaBase, debugArenaSize);

		debugState->renderGroup = PushStruct(&debugState->debugArena, RenderGroup);

		uint8* collationArenaBase = (uint8*)gameMemory->debugStorage + sizeof(DebugState) + debugArenaSize;
		debugState->collationArena.Init(collationArenaBase, collationArenaSize);

		AllocateDebugFrames(debugState);
		debugState->threads = std::vector<DebugThread>();		
		debugState->debugElements = std::vector<DebugElement>();

		debugState->isInitalized = true;
	}

	GameState* gameState = (GameState*)gameMemory->permenentStorage;
	TransientState* transientState = (TransientState*)gameMemory->transientStorage;


	float halfWidth = gameRenderCommands->settings.dims.x / 2.0f;
	float halfheight = gameRenderCommands->settings.dims.y / 2.0f;

	glm::mat4 cameraProj = glm::ortho(-halfWidth, halfWidth, -halfheight, halfheight);


	// We start a render setup
	RenderGroup group = {};

	RenderSetup renderSetup = {};
	renderSetup.transformMatrix = cameraProj;// *glm::inverse(globalDebugCameraMat);



	group.quads = PushRenderElement(gameRenderCommands, TexturedQuads);
	*group.quads = {};
	group.quads->masterVertexArrayOffset = gameRenderCommands->numVertex;
	group.quads->masterBitmapArrayOffset = gameRenderCommands->numBitmaps;
	group.quads->renderSetup = renderSetup;

	// how big do we want char to be displayed
	const float DEBUG_CHAR_BITMAP_SCALE = 1;

	string s = "abcdefghijlmnopqrstuvwxyz 123456789\nABCDEFGHIJLMNOPQRSTUVWXYZ";
	//string s = "Hej ab";
	
	int ascent = 0;
	int descent = 0;
	int lineGap = 0;
	stbtt_GetFontVMetrics(&debugLoadedFont->fontInfo, &ascent, &descent, &lineGap);
	float scale = stbtt_ScaleForPixelHeight(&debugLoadedFont->fontInfo, FONT_SCALE);

	int lineGapBetweenNextBaseline = (ascent - descent + lineGap);
	int scaledLineGap = (int)(lineGapBetweenNextBaseline * scale);

	float xPos = -halfWidth;
//	int yBaselinePos = halfheight - (int)(ascent * scale);

	int yBaselinePos = 0; // halfheight - (int)(ascent * scale);

	// This is essentially following the example from stb library
	for (int i = 0; i < s.size(); i++)
	{
		int advance, leftSideBearing;
		stbtt_GetCodepointHMetrics(&debugLoadedFont->fontInfo, s[i], &advance, &leftSideBearing);

		GlyphId glyphID = GetGlyph(transientState->assets, debugLoadedFont, s[i]);
		LoadedGlyph* glyphBitmap = GetGlyph(transientState->assets, glyphID);

		if (s[i] == '\n')
		{
			xPos = -halfWidth;
			yBaselinePos -= scaledLineGap;
		}
		else
		{
			float height = DEBUG_CHAR_BITMAP_SCALE * glyphBitmap->bitmap.height;
			float width = glyphBitmap->bitmap.width / (float)glyphBitmap->bitmap.height * height;

			int x = xPos + glyphBitmap->bitmapXYOffsets.x;
			int y = yBaselinePos - glyphBitmap->bitmapXYOffsets.y;

			glm::vec3 leftTopPos = glm::vec3(x, y, 0);

			PushBitmap(gameRenderCommands, &group, &glyphBitmap->bitmap, COLOR_WHITE, leftTopPos, glm::vec3(width / 2.0, height / 2.0, 0), AlignmentMode::Left, AlignmentMode::Top);

			
			xPos += (advance * scale);
			if (i < s.size())
				xPos += scale * stbtt_GetCodepointKernAdvance(&debugLoadedFont->fontInfo, s[i], s[i+1]);
		}
	}


	uint64 arrayIndex_eventIndex = globalDebugTable->eventArrayIndex_EventIndex;

	// get the top 32 bit
	uint32 eventArrayIndex = arrayIndex_eventIndex >> 32;
	// we want the ladder 32 bit
	uint32 numEvents = arrayIndex_eventIndex & 0xFFFFFFFF;
	cout << "		before eventArrayIndex " << eventArrayIndex << ", numEvents " << numEvents << endl;

	// we change the array we want to write to
	uint64 newEventArrayIndex = !eventArrayIndex;
	globalDebugTable->eventArrayIndex_EventIndex = (uint64)(newEventArrayIndex << 32);

	uint32 eventArrayIndex2 = globalDebugTable->eventArrayIndex_EventIndex >> 32;
	uint32 numEvents2 = globalDebugTable->eventArrayIndex_EventIndex & 0xFFFFFFFF;
	cout << "		after new eventArrayIndex " << eventArrayIndex2 << ", numEvents " << numEvents2 << endl;


	// one debug event array is almost a frame worth of stuff

	if (debugState->numFrames >= MAX_DEBUG_EVENT_ARRAY_COUNT)
	{
		RestartCollation(debugState);
	}

	// Assuming we aren't recording debugEvents multithreadedly
	cout << "		before eventArrayIndex " << eventArrayIndex << ", numEvents " << numEvents << endl;
	ProcessDebugEvents(debugState, globalDebugTable->events[eventArrayIndex], numEvents);

	// Render Debug stuff

	RenderProfileBars(debugState, gameRenderCommands, &group, transientState->assets, gameInputState->mousePos);

}
