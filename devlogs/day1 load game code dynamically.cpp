1.	So the first thing I tried to do is to load the game code dynamically
this is something I have never done in my previous projects and I want to emulate what Casey did on Handmade hero.

So there is this struct in our code base where we want to keep track of the state of our loaded code.

two key things is the memory address to the dll and the function pointers to the functions that you want.


				struct SDLLoadedCode
				{
					bool isValid;
					string dllFullPath;

					// on linux is void*
					// HMODULE is just the DLL's base address
					// A handle to a module. The is the base address of the module in memory.
					FILETIME dllLastWriteTime;
					HMODULE dllBaseAddress;

	                function pointer 
				};



so My initial attempt was this. I thought that just by having a pointer to the function, you can just call it. 
But apparently there is no way to call it in c?

The thing is that although you have this memory address, the compiler doesnt know how to interpret it. How many arguments
does it take? what is the return type. so by just having void* doesnt work

                struct SDLLoadedCode
                {
                    bool isValid;
                    string dllFullPath;

                    // on linux is void*
                    // HMODULE is just the DLL's base address
                    // A handle to a module. The is the base address of the module in memory.
                    FILETIME dllLastWriteTime;
                    HMODULE dllBaseAddress;

    ----------->    void* updateAndRenderFunction;
                };


so what we want to do is to define a function pointer type that has the function template that you want


so this is what I did   

                typedef void(*UpdateAndRender_t)();

                struct SDLLoadedCode
                {
                    bool isValid;
                    string dllFullPath;

                    // on linux is void*
                    // HMODULE is just the DLL's base address
                    // A handle to a module. The is the base address of the module in memory.
                    FILETIME dllLastWriteTime;
                    HMODULE dllBaseAddress;

                    UpdateAndRender_t updateAndRenderFunction;
                };

Besure not to make the mistake of doing 

                struct SDLLoadedCode
                {
                    bool isValid;
                    string dllFullPath;

                    // on linux is void*
                    // HMODULE is just the DLL's base address
                    // A handle to a module. The is the base address of the module in memory.
                    FILETIME dllLastWriteTime;
                    HMODULE dllBaseAddress;

    ----------->    UpdateAndRender_t* updateAndRenderFunction;
                };

I initially made the mistake of declaring a function pointer to a pointer. UpdateAndRender_t is already a pointer to a function


then in the load code function you can just call GetProcAddress();


                void SDLLoadCode(SDLLoadedCode* loaded_code)
                {
                    loaded_code->dllBaseAddress = LoadLibraryA(loaded_code->dllFullPath.c_str());
                    if (loaded_code->dllBaseAddress)
                    {
                        // load the function names
                        UpdateAndRender_t updateAndRenderFunction = (UpdateAndRender_t)GetProcAddress(loaded_code->dllBaseAddress, "UpdateAndRender");
                        
                        if (updateAndRenderFunction)
                        {
                            loaded_code->updateAndRenderFunction = updateAndRenderFunction;
                        }
                        else
                        {
                            cout << "error";
                            std::cout << GetLastError() << endl;
                            loaded_code->isValid = false;
                        }
                    }
                }

Besure to use GetLastError(); if you GetProcAddress(); was not successful.



On the GameCodeSide, its really important that you do this 



                extern "C" __declspec(dllexport) void UpdateAndRender()
                {
                    cout << "Update And Render2";
                }



if you only just do 

                extern "C" void UpdateAndRender()
                {
                    cout << "Update And Render2";
                }

of you leave out the __declspec(dllexport), this function wont be exported in the GameCode.dll
what that means is that you wll get a error127 from the GetLastError(); function call.

the extern "C" has to do with name manging.
Accordingly to RaymondChen 


        Dllexport of a c++ function will expose the function with C++ name mangling. If C++ name mangling is not desired, 
        either use a .def file (EXPORTS keyword), which will expose the  name without mangling, 
        or declare functions as extern “C”, which will expose the name with C mangling

the idea is that you need to use __declspec(dllexport) to "export from a dll"
https://docs.microsoft.com/en-us/cpp/build/exporting-from-a-dll-using-declspec-dllexport?view=vs-2019#:~:text=You%20can%20export%20data%2C%20functions,export%20decorated%20C%2B%2B%20function%20names.

this keyword adds the export directive to the object file so you do not need to use a .def file.