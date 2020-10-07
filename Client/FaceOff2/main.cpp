
#include "../PlatformShared/platform_shared.h"

#include "SDL.h"
#undef main

#include <windows.h>
#include <string>
#include <iostream>


#include "sdl_faceoff_opengl.h"

#include "../GameCode/game_code.h"


bool debugMode;
bool is_game_running;

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


enum AssetTypeId
{
	AssetFont,
	NumAssetType,
};



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

	UpdateAndRender_t updateAndRenderFunction;
	test1_t test1Function;
};

struct LoadedBitmap
{
	void* memory;
	int width;
	int height;
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
		UpdateAndRender_t updateAndRenderFunction = (UpdateAndRender_t)GetProcAddress(loaded_code->dllBaseAddress, "UpdateAndRender");
		
		if (updateAndRenderFunction)
		{
			loaded_code->updateAndRenderFunction = updateAndRenderFunction;
			loaded_code->isValid = (loaded_code->updateAndRenderFunction);
		}
		else
		{
			std::cout << "Error in updateAndRenderFunction GetProcAddress: " << GetLastError() << std::endl;
			loaded_code->isValid = false;
		}

		// load the function names
		test1_t test1Function = (test1_t)GetProcAddress(loaded_code->dllBaseAddress, "test1");

		if (test1Function)
		{
			loaded_code->test1Function = test1Function;
		}
		else
		{
			std::cout << "Error in test1Function GetProcAddress: " << GetLastError() << std::endl;
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
	
	loaded_code->updateAndRenderFunction = NULL;
	loaded_code->test1Function = NULL;
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


	gameCode.updateAndRenderFunction = nullptr;


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



		LPVOID baseAddress = 0;

		uint64 totalSize = gameMemory.permenentStorageSize + gameMemory.transientStorageSize + gameMemory.debugStorageSize;
		void* gameMemoryBlock = VirtualAlloc(baseAddress, (size_t)totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		gameMemory.permenentStorage = gameMemoryBlock;
		gameMemory.transientStorage = ((uint8_t*)gameMemory.permenentStorage + gameMemory.permenentStorageSize);
		gameMemory.debugStorage = ((uint8_t*)gameMemory.permenentStorage + gameMemory.permenentStorageSize + gameMemory.transientStorageSize);


		uint32_t renderCommandsPushBufferSize = Megabytes(64);
		void* renderCommandsPushBuffer = VirtualAlloc(0, (size_t)renderCommandsPushBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		int maxNumVertex = 65535;
		void* texturedArrayBuffer = VirtualAlloc(0, maxNumVertex * sizeof(TexturedVertex), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);




		GameInputState inputs[1] = {};
		GameInputState* newInput = &inputs[0];


		while (is_game_running)
		{
			newInput->dtForFrame = targetSecondsPerFrame;
			

			// process keyboard events
			SDLProcessPendingEvents(newInput);

			// process mouse events
			glm::ivec2 mouseP;
			uint32 mouseState = SDL_GetMouseState(&mouseP.x, &mouseP.y);
			newInput->mouseX = (float)mouseP.x;
			newInput->mouseY = (float)windowDimensions.y - mouseP.y;

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
			gameRenderCommands.pushBufferBase = (uint8*)renderCommandsPushBuffer;
			gameRenderCommands.maxPushBufferSize = renderCommandsPushBufferSize;
			gameRenderCommands.numElements = 0;

			gameRenderCommands.maxNumVertex = maxNumVertex;
			gameRenderCommands.masterVertexArray = (TexturedVertex*)texturedArrayBuffer;
			gameRenderCommands.numVertex = 0;

			// gameCode.updateAndRenderFunction(&gameMemory, newInput, &gameRenderCommands);

			UpdateAndRender(&gameMemory, newInput, &gameRenderCommands, windowDimensions, debugMode);

			if (SDLCheckForCodeChange(&gameCode))
			{
				SDLReloadCode(&gameCode);
			}

			OpenGLRenderCommands(&openGL, &gameRenderCommands, glm::ivec2(0), glm::ivec2(0), windowDimensions);

			RendererEndFrame();


			GameInputState *temp = newInput;


			uint64_t endCounter = SDLGetWallClock();
			double measuredSecondsPerFrame = SDLGetSecondsElapsed(lastCounter, endCounter, globalPerfCountFrequency);
			
		//	cout << measuredSecondsPerFrame << endl;
			lastCounter = endCounter;
		}
	}


	SDL_Quit();
	return(0);
}

