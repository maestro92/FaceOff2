
#include "../PlatformShared/platform_shared.h"
#include "debug_interface.h"

#include "SDL.h"
#undef main

#include <windows.h>
#include <string>
#include <iostream>

#include "SDL_image.h"

#include "sdl_faceoff_opengl.h"

#include "../GameCode/game_code.h"


bool debugMode;
bool is_game_running;



static DebugTable globalDebugTable_;
DebugTable* globalDebugTable = &globalDebugTable_;

static OpenGLStuff openGL;

struct SDLLoadedCode
{
	bool isValid;
	std::string srcDllFullPath;
	std::string tmpDllFullPath;

	// on linux is void*
	// HMODULE is just the DLL's base address
	// A handle to a module. The is the base address of the module in memory.
	FILETIME dllLastWriteTime;
	HMODULE dllBaseAddress;

	GameUpdateAndRender_t gameUpdateAndRender;
	DebugSystemUpdateAndRender_t debugSystemUpdateAndRender;
};



void PrintFullPath(char * partialPath)
{
	char full[_MAX_PATH];
	if (_fullpath(full, partialPath, _MAX_PATH) != NULL)
		printf("Full path is: %s\n", full);
	else
		printf("Invalid path\n");
}

// This is done using windows API.
FILETIME SDLGetFileLastWriteTime(const char* filename)
{
	FILETIME lastWriteTime = {};
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (GetFileAttributesExA(filename, GetFileExInfoStandard, &data))
	{
		lastWriteTime = data.ftLastWriteTime;
	}

	return lastWriteTime;
}

BitmapInfo SDLLoadPNGFile(char* filename)
{
	BitmapInfo result = {};
	SDL_Surface* image = IMG_Load(filename);
//	SDL_Surface* image = IMG_Load_RW(SDL_RWFromFile("Assets/wall2.png", "rb"), 1);
	
	if (image == nullptr)
	{
		printf("1Unable to load image %s! SDL_image Error: %s\n", filename, IMG_GetError());
	}

	result.width = image->w;
	result.height = image->h;
	result.pitch = image->pitch;

	/*
	cout << "result.dim " << result.width << " " << result.height << endl;
	cout << "result.pitch " << result.pitch << " " << endl;
	cout << "format " << image->format->format << endl;

	cout << "BitsPerPixel " << image->format->BitsPerPixel << endl;
	cout << "BytesPerPixel " << image->format->BytesPerPixel << endl;

	cout << SDL_GetPixelFormatName(image->format->format) << endl;
	*/

	assert(result.pitch == result.width * 4);


	// we are assuming 32 bit. So result.pitch == result.width * 4.
//	int totalByteSize = result.width * result.height * 4;
	int totalByteSize = result.pitch * result.height;

//	cout << "totalByteSize " << totalByteSize << endl;
	result.memory = VirtualAlloc(0, (size_t)totalByteSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memcpy(result.memory, image->pixels, totalByteSize);

	return result;
}



void SDLLoadCode(SDLLoadedCode* loaded_code)
{
	CopyFile(loaded_code->srcDllFullPath.c_str(), loaded_code->tmpDllFullPath.c_str(), FALSE);
	
	DWORD error_code = GetLastError();
	if (error_code != 0)
	{
		std::cout << "Error Copying the dll: " << error_code << std::endl;
	}
	else
	{
		std::cout << "Operation Successful " << std::endl;
	}
	

	loaded_code->dllLastWriteTime = SDLGetFileLastWriteTime(loaded_code->srcDllFullPath.c_str());

	loaded_code->dllBaseAddress = LoadLibraryA(loaded_code->tmpDllFullPath.c_str());
	if (loaded_code->dllBaseAddress)
	{
		// load the function names
		GameUpdateAndRender_t gameUpdateAndRender = (GameUpdateAndRender_t)GetProcAddress(loaded_code->dllBaseAddress, "GameUpdateAndRender");
		
		if (gameUpdateAndRender)
		{
			loaded_code->gameUpdateAndRender = gameUpdateAndRender;
			loaded_code->isValid = (loaded_code->gameUpdateAndRender);
		}
		else
		{
			std::cout << "Error in updateAndRenderFunction GetProcAddress: " << GetLastError() << std::endl;
			loaded_code->isValid = false;
		}


	}
	else
	{
		std::cout << "Error loading the module: " << GetLastError() << std::endl;
	}
	std::cout << "base address " << loaded_code->dllBaseAddress;
}

void SDLUnloadCode(SDLLoadedCode* loaded_code)
{
	if (loaded_code->dllBaseAddress)
	{
		std::cout << "Unloading" << std::endl;
		FreeLibrary(loaded_code->dllBaseAddress);
		loaded_code->dllBaseAddress = 0;
	}
	
	loaded_code->gameUpdateAndRender = NULL;
	loaded_code->debugSystemUpdateAndRender = NULL;
	loaded_code->isValid = false;
}

void SDLReloadCode(SDLLoadedCode* loadedCode)
{
	SDLUnloadCode(loadedCode);
	for (int i = 0; i < 100; i++)
	{
		if (loadedCode->isValid)
		{
			break;
		}

		SDLLoadCode(loadedCode);
		Sleep(100);
	}
}



void RendererEndFrame()
{
	// the sleeping is done here.
	SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
}


bool SDLCheckForCodeChange(SDLLoadedCode* loadedCode)
{
//	FILETIME newDLLWriteTime = SDLGetFileLastWriteTime(loadedCode->dllFullPath.c_str());
		
	FILETIME newDLLWriteTime = SDLGetFileLastWriteTime(loadedCode->srcDllFullPath.c_str());
	bool result = (CompareFileTime(&newDLLWriteTime, &loadedCode->dllLastWriteTime) != 0);

	if (result == true)
	{
		std::cout << "needs to reload" << std::endl;
	}

	return result;
}

uint64_t SDLGetWallClock()
{
	uint64_t result = SDL_GetPerformanceCounter();
	return result;
}

double SDLGetSecondsElapsed(uint64_t start, uint64_t end, uint64_t performanceFrequency )
{
	double result = ((double)(end - start) / (double)performanceFrequency);
	return(result);
}

// virtualAlloc


// buttonState current has the state from the previous frame
// we want to check if the current frame is different
// if the state in the new frame is different, we increase the halfTransitionCount
void SDLProcessKeyboardEvent(GameButtonState* buttonState, bool isDown)
{
	if (buttonState->endedDown != isDown)
	{
		buttonState->endedDown = isDown;
		++buttonState->halfTransitionCount;
	}

	std::cout << "ended down " << buttonState->endedDown << ", halftransition count" << buttonState->halfTransitionCount << std::endl;

}


void SDLProcessPendingEvents(GameInputState* game_input_state)
{
	SDL_Event event;

	for (;;)
	{
		int numPendingEvents = SDL_PollEvent(&event);

		if (numPendingEvents == 0)
		{
			break;
		}

		switch (event.type)
		{
			case SDL_QUIT:
			{
				is_game_running = false;				
			}	break;

			

			case SDL_KEYDOWN:
			{
				SDL_Keycode keyCode = event.key.keysym.sym;
				if (keyCode == SDLK_z)
				{
					break;
				}
				// still want to process the other key code in SDL_KEYUP
			}
			case SDL_KEYUP:
			{
				SDL_Keycode keyCode = event.key.keysym.sym;

				bool altKeyIsDown = (event.key.keysym.mod & KMOD_ALT);
				bool shiftKeyIsDown = (event.key.keysym.mod & KMOD_SHIFT);
				bool isDown = (event.key.state == SDL_PRESSED);

				if (event.key.repeat == 0)
				{
					if (keyCode == SDLK_w)
					{
						SDLProcessKeyboardEvent(&game_input_state->moveForward, isDown);
					}				
					else if (keyCode == SDLK_a)
					{
						SDLProcessKeyboardEvent(&game_input_state->moveLeft, isDown);
					}
					else if (keyCode == SDLK_s)
					{
						SDLProcessKeyboardEvent(&game_input_state->moveBack, isDown);
					}
					else if (keyCode == SDLK_d)
					{
						SDLProcessKeyboardEvent(&game_input_state->moveRight, isDown);
					}
					else if (keyCode == SDLK_z)
					{
						debugMode = !debugMode;
						if (debugMode)
						{
							SDL_ShowCursor(SDL_ENABLE);
						}
						else
						{
							SDL_ShowCursor(SDL_DISABLE);
						}
					}
					else if (keyCode == SDLK_ESCAPE)
					{
						is_game_running = false;
					}
				}
			}	break;

			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{

					}	break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
					{
						std::cout << "SDL_WINDOW_EVENT_FOCUSED_GAINED" << std::endl;
					}	break;
				}
			}

		}
		
	}
}



void syncInputs(GameInputState* old_input_state, GameInputState* new_input_state)
{
	for (int i = 0; i < 4; i++)
	{
		new_input_state->buttons[i].endedDown = old_input_state->buttons[i].endedDown;
	}
}

void debugInputs(GameInputState* new_input_state)
{
	for (int i = 0; i < 1; i++)
	{
		std::cout << new_input_state->buttons[i].endedDown << std::endl;
	}
}



int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO);


	glm::ivec2 windowDimensions = glm::ivec2(800, 640);

	// glm::ivec2(1920, 1080);

	SDL_Window* window = SDL_CreateWindow("FaceOff 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowDimensions.x, windowDimensions.y,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);

	// we start in game mode, so we disable the mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	SDLLoadedCode gameCode = {};
	gameCode.srcDllFullPath = "../Debug/GameCode.dll";
	gameCode.tmpDllFullPath = "../Debug/GameCode_temp.dll";


	gameCode.gameUpdateAndRender = nullptr;
	gameCode.debugSystemUpdateAndRender = nullptr;


	SDLLoadCode(&gameCode);

//	LoadFontTest();

	SDLInitOpenGL(window);
	initApplicationOpenGL(&openGL);
	if (window)
	{
		// Load Renderer 
	
		is_game_running = true;
		SDL_ShowWindow(window);


		uint64 globalPerfCountFrequency = SDL_GetPerformanceFrequency();
		int monitorRefreshHz = 60;
		int displayIndex = SDL_GetWindowDisplayIndex(window);
		SDL_DisplayMode mode = {};
		int displayModeResult = SDL_GetDesktopDisplayMode(displayIndex, &mode);
		if (displayModeResult == 0 && mode.refresh_rate > 1)
		{
			monitorRefreshHz = mode.refresh_rate;
		}
		double gameUpdateHz = (double)monitorRefreshHz;

		// if we are doing 60 fps, this will be 0.016 ms 
		double targetSecondsPerFrame = 1.0 / (double)gameUpdateHz;

		uint64_t lastCounter = SDLGetWallClock();


		GameMemory gameMemory = {};
		gameMemory.permenentStorageSize = Megabytes(256);
		gameMemory.transientStorageSize = Megabytes(256);
		gameMemory.debugStorageSize = Megabytes(256);

		gameMemory.debugTable = globalDebugTable;


		LPVOID baseAddress = 0;

		uint64 totalSize = gameMemory.permenentStorageSize + gameMemory.transientStorageSize + gameMemory.debugStorageSize;
		void* gameMemoryBlock = VirtualAlloc(baseAddress, (size_t)totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		gameMemory.permenentStorage = gameMemoryBlock;
		gameMemory.transientStorage = ((uint8_t*)gameMemory.permenentStorage + gameMemory.permenentStorageSize);
		gameMemory.debugStorage = ((uint8_t*)gameMemory.permenentStorage + gameMemory.permenentStorageSize + gameMemory.transientStorageSize);

		gameMemory.platformAPI.readImageFile = (PlatformReadImageFile)SDLLoadPNGFile;
		gameMemory.platformAPI.allocateTexture = (PlatformAllocateTexture)OpenGLAllocateTexture;
		// gameMemory.platformAPI.allocateTexture2 = (PlatformAllocateTexture2)OpenGLAllocateTexture2;


		uint32_t renderCommandsPushBufferSize = Megabytes(64);
		void* renderCommandsPushBuffer = VirtualAlloc(0, (size_t)renderCommandsPushBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		int maxNumVertex = 65535;
		void* texturedArrayBuffer = VirtualAlloc(0, maxNumVertex * sizeof(TexturedVertex), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		int maxNumBitmaps = 65535 >> 2;
		void* bitmapArrayBuffer = VirtualAlloc(0, maxNumBitmaps * sizeof(LoadedBitmap*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		GameInputState inputs[1] = {};
		GameInputState* newInput = &inputs[0];

		int frame = 0;
		while (is_game_running)
		{
			newInput->dtForFrame = targetSecondsPerFrame;
			

			// process keyboard events
			SDLProcessPendingEvents(newInput);

			// process mouse events

			uint32 mouseState = SDL_GetMouseState(&newInput->mousePos.x, &newInput->mousePos.y);
			newInput->mousePos.y = (float)windowDimensions.y - newInput->mousePos.y;

			Uint32 SDLButtonID[PlatformMouseButton_Count] =
			{
				SDL_BUTTON_LMASK,
				SDL_BUTTON_MMASK,
				SDL_BUTTON_RMASK,
				SDL_BUTTON_X1MASK,
				SDL_BUTTON_X2MASK,
			};

			// didn't fully understand this part
			/*
			for (int i = 0; i < PlatformMouseButton_Count; i++)
			{
				new->MouseButtons[i] = OldInput->MouseButtons[ButtonIndex];
				NewInput->MouseButtons[ButtonIndex].HalfTransitionCount = 0;
				SDLProcessKeyboardEvent(&NewInput->MouseButtons[ButtonIndex],
					MouseState & SDLButtonID[ButtonIndex]);
			}
			*/
			if (!debugMode)
			{
				SDL_WarpMouseInWindow(window, windowDimensions.x / 2, windowDimensions.y / 2);				
			}

			GameRenderCommands gameRenderCommands = {};

			gameRenderCommands.settings.dims = windowDimensions;

			gameRenderCommands.pushBufferBase = (uint8*)renderCommandsPushBuffer;
			gameRenderCommands.maxPushBufferSize = renderCommandsPushBufferSize;
			gameRenderCommands.numRenderGroups = 0;

			gameRenderCommands.maxNumVertex = maxNumVertex;
			gameRenderCommands.masterVertexArray = (TexturedVertex*)texturedArrayBuffer;
			gameRenderCommands.numVertex = 0;

			gameRenderCommands.maxNumBitmaps = maxNumBitmaps;
			gameRenderCommands.masterBitmapArray = (LoadedBitmap**)bitmapArrayBuffer;
			gameRenderCommands.numBitmaps = 0;


			BEGIN_BLOCK("Game Update");
			// there was no way for me to debug this way.
			// gameCode.updateAndRenderFunction(&gameMemory, newInput, &gameRenderCommands);
			//	if (UpdateAndRender != nullptr)
			{
				GameUpdateAndRender(&gameMemory, newInput, &gameRenderCommands, windowDimensions, debugMode);
			}


			END_BLOCK();

			if (SDLCheckForCodeChange(&gameCode))
			{
				SDLReloadCode(&gameCode);
			}


		//	if (Game.DEBUGFrameEnd)
			{
				DebugSystemUpdateAndRender(&gameMemory, newInput, &gameRenderCommands, windowDimensions, debugMode);
			}

			BEGIN_BLOCK("Frame Display");
			/*
			for (int j = 0; j < 100000000; j++)
			{
				int a = 1;
			}
			*/			
			OpenGLRenderCommands(&openGL, &gameRenderCommands, glm::ivec2(0), glm::ivec2(0), windowDimensions);
			RendererEndFrame();
			END_BLOCK();

			uint64_t endCounter = SDLGetWallClock();
			double measuredSecondsPerFrame = SDLGetSecondsElapsed(lastCounter, endCounter, globalPerfCountFrequency);
			
			FRAME_MARKER(measuredSecondsPerFrame);
			lastCounter = endCounter;
			frame++;
		}
	}


	SDL_Quit();
	return(0);
}

