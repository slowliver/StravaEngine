#include <stdio.h>

#include <windows.h>

#include <stdio.h>

#include <Engine/Core/Array.h>
#include <Engine/Framework/Application.h>

using namespace StravaEngine;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdline, int cmdshow)
{
	Core::Array<int, 8> myArray;

	int i = 0;
	for (auto& elem : myArray)
	{
		elem = i++;
	}

	for (auto& elem : myArray)
	{
		char bytes[128];
		sprintf_s(bytes, "%d\n", elem);
		OutputDebugStringA(bytes);
	}

	Framework::ApplicationSpec spec;
	spec.m_nativeHandle = (NativeHandle)hInstance;
	spec.m_title = "Title";

	auto application = Framework::Application(spec);

	// Return this part of the WM_QUIT message to Windows.
	return 0;
}