#pragma once

/*
struct DebugElement
{
	char* GUID;
	char* name;
	DebugElement* nextInHash;
};
*/

struct OpenDebugBlock
{
	uint32 startingFrameIndex;
	DebugEvent* openingEvent;

	// if stored in the free list linked list in DebugState->headFreeBlock
	// next stores the next one 

	// if this node is being used, next is storing the parent 
	OpenDebugBlock* next;
};

struct DebugFrameRegion
{
	DebugEvent* debugEvent;
	uint64 cycleCount;
};

#define MAX_REGIONS_PER_FRAME 2*4096
struct DebugFrame
{
	uint64 beginClock;
	uint64 endClock;
	uint32 wallSecondsElapsed;

	uint32 numRegions;
	DebugFrameRegion* regions;
};




struct DebugThread
{
	// LinkedList of OpenBlocks, this is implemented as a stack
	OpenDebugBlock* headOpenBlock;

	bool HasOpenBlocks()
	{
		return headOpenBlock != NULL;
	}
};

struct DebugState
{
	bool isInitalized;
	MemoryArena debugArena;
	DebugFrame* collationFrame;


	// Things for collation
	MemoryArena collationArena;

	uint32 numFrames;
	DebugFrame* frames;


	// a linked list of threads.
	// we are using a linked list for all the threads.
	DebugThread* headThread;

	// a linked list of free blocks. When DebugThreads wants a new OpenDebugBlock
	// you request it from here.
	OpenDebugBlock* headFreeBlock;
};



void initDebugFrame(DebugFrame* debugFrame, uint64 beginClock, MemoryArena* memoryArena)
{
	debugFrame->beginClock = beginClock;
	debugFrame->endClock = 0;
	debugFrame->numRegions = 0;
	debugFrame->regions = PushArray(memoryArena, MAX_REGIONS_PER_FRAME, DebugFrameRegion);
	debugFrame->wallSecondsElapsed = 0.0f;
}


DebugThread* TryGetOrCreateDebugThread(DebugState* debugState, int threadId)
{
	return NULL;
}



bool IsMatchingDebugEvents(const DebugEvent& lhs, const DebugEvent& rhs)
{
	return lhs.threadId == rhs.threadId;
}



void ProcessDebugEvents(DebugState* debugState, DebugEvent* debugEventsArray, uint32 numEvents)
{
	// Reset the debug table, start the pingpong 

	// move all of it into my debug storage

	for (int i = 0; i < numEvents; i++)
	{
		DebugEvent* event = &debugEventsArray[i];

		if (event->type == DebugEventType::FrameMarker)
		{
			// if we already frame 
			if (debugState->collationFrame)
			{
				debugState->collationFrame->endClock = event->clock;

				debugState->numFrames++;
			}


			debugState->collationFrame = &debugState->frames[debugState->numFrames];
			initDebugFrame(debugState->collationFrame, event->clock, &debugState->collationArena);
		}
		else if (debugState->collationFrame != NULL)
		{
			uint32 frameIndex = debugState->numFrames - 1;
			int threadId = 1;
			DebugThread* thread = TryGetOrCreateDebugThread(debugState, threadId);
			uint64 relativeClock = event->clock - debugState->collationFrame->beginClock;

			if (event->type == DebugEventType::BeginBlock)
			{
				OpenDebugBlock* debugBlock = debugState->headFreeBlock;
				if (debugBlock)
				{
					debugState->headFreeBlock = debugBlock->next;
				}
				else
				{
					debugBlock = PushStruct(&debugState->collationArena, OpenDebugBlock);
					debugBlock->next = NULL;
				}

				debugBlock->startingFrameIndex = frameIndex;
				debugBlock->openingEvent = event;
				debugBlock->next = thread->headOpenBlock;
				thread->headOpenBlock = debugBlock;

			}
			else if (event->type == DebugEventType::EndBlock)
			{
				assert(debugState->collationFrame);
				uint32 frameIndex = debugState->numFrames - 1;

				uint64 relativeClock = event->clock - debugState->collationFrame->beginClock;

				if (thread->HasOpenBlocks())
				{
					OpenDebugBlock* matchingBlock = thread->headOpenBlock;
					DebugEvent* openingEvent = matchingBlock->openingEvent;

					if (IsMatchingDebugEvents(*openingEvent, *event))
					{
						if (matchingBlock->startingFrameIndex = frameIndex)
						{
							// add a debug region
						}
					}
					else
					{

					}

					// recycle the current OpenDebugBlock					
					thread->headOpenBlock = thread->headOpenBlock->next;
					matchingBlock->next = debugState->headFreeBlock;
					debugState->headFreeBlock = matchingBlock;
				}
			}
		}
	}
}


