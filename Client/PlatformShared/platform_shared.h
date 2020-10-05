#pragma once
#include <stdint.h>

#include "glm/ext.hpp"

/*
typedef void* (*PlatformAllocateMemory)(int size);
typedef void(*PlatformDeallocateMemory)(void* address, int size);
*/


#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define OffsetOf(type, member) (uintptr_t) &(((type*)0)->member)

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef size_t MemoryIndex;


std::ostream& operator<<(std::ostream& os, glm::vec3 v)
{
	return os << v.x << " " << v.y << " " << v.z;
}

std::ostream& operator<<(std::ostream& os, glm::mat4 m)
{
	return os << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << "\n"
		      << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << "\n"
			  << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << "\n"
	          << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << "\n";;

}

enum GameInputMouseButton
{
	PlatformMouseButton_Left,
	PlatformMouseButton_Middle,
	PlatformMouseButton_Right,
	PlatformMouseButton_Extended0,
	PlatformMouseButton_Extended1,

	PlatformMouseButton_Count,
};

struct PlatformAPI
{

};

struct GameButtonState
{
	int halfTransitionCount;
	bool endedDown;
};

struct GameInputState
{
	double dtForFrame;

	GameButtonState buttons[4];
	struct
	{
		GameButtonState moveForward;
		GameButtonState moveLeft;
		GameButtonState moveRight;
		GameButtonState moveBack;
	};

	GameButtonState mouseButtons[PlatformMouseButton_Count];
	double mouseX, mouseY;
	/*
	// For debugging only;
	double mouseX, mouseY, mouseZ;
	bool shiftDown, altDown, controlDown;
	*/
};

struct GameMemory
{
	uint64 permenentStorageSize;
	void* permenentStorage;

	uint64 transientStorageSize;
	void* transientStorage;

	uint64 debugStorageSize;
	void* debugStorage;
	
	PlatformAPI platformAPI;

};



enum RenderEntryType
{
	RenderEntryType_Clear,
	RenderEntryType_TexturedQuads,
	RenderEntryType_ColoredLines,
};

struct RenderEntryHeader
{
	uint16 type;
};

struct RenderEntryClear
{
	glm::vec4 color;
};

struct RenderEntryTexturedQuads
{
	int numQuads;
	int masterVertexArrayOffset;
};

struct RenderEntryColoredLines
{
	int numLines;
	int masterVertexArrayOffset;
};

struct TexturedVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 color;
};

struct GameRenderCommands
{
	uint8* pushBufferBase;
	uint32 pushBufferSize;	
	uint32 maxPushBufferSize;

	uint32 numElements;

	unsigned int maxNumVertex;
	unsigned int numVertex;
	TexturedVertex* masterVertexArray;

	// hack for now
	// eventually we want to add this to a render group concept
	// instead of per TexturedQuad.
	glm::mat4 cameraProjectionMatrix;
	glm::mat4 cameraTransformMatrix;
	glm::mat4 transformMatrix;

	uint8* CurrentPushBufferAt()
	{
		return pushBufferBase + pushBufferSize;
	}

	bool HasSpaceFor(uint32 size)
	{
		return (pushBufferSize + size) <= maxPushBufferSize;
	}

	void PrintDebug()
	{
		for (int i = 0; i < numVertex; i++)
		{			
			std::cout << "i " << i << ": " << masterVertexArray[i].position << std::endl;
		}
	}

};


// make a type that this function template
typedef void(*UpdateAndRender_t)(GameMemory* gameMemory, 
								GameInputState* gameInput, 
								GameRenderCommands* gameRenderCommands, glm::ivec2 windowDimensions, bool isDebugMode);
typedef void(*test1_t)();
