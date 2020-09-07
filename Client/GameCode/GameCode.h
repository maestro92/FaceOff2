#pragma once


#include <iostream>

using namespace std;

extern "C" __declspec(dllexport) void UpdateAndRender()
{
	cout << "Update And Render2";
}


