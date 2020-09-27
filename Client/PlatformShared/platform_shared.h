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

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef size_t MemoryIndex;



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
	RenderEntryType_RenderEntryClear,
	RenderEntryType_RenderEntryCube,
	RenderEntryType_CoordinateSystem,
};

struct RenderEntryHeader
{
	uint16 type;
};

struct RenderEntryClear
{
	glm::vec4 color;
};

struct RenderEntryCube
{
	glm::vec3 position;
};

struct RenderEntryCoordinateSystem
{
	glm::vec3 position;
	// orientation?
};

struct GameRenderCommands
{
	uint8* pushBufferBase;
	uint32 pushBufferSize;	
	uint32 maxPushBufferSize;

	uint32 numElements;

	uint8* CurrentPushBufferAt()
	{
		return pushBufferBase + pushBufferSize;
	}

	bool HasSpaceFor(uint32 size)
	{
		return (pushBufferSize + size) <= maxPushBufferSize;
	}
};


// make a type that this function template
typedef void(*UpdateAndRender_t)(GameMemory* gameMemory, GameInputState* gameInput, GameRenderCommands* gameRenderCommands);
typedef void(*test1_t)();
