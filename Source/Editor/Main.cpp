#include <stdio.h>

#include <windows.h>

#include <stdio.h>

#include <Engine/Core/Array.h>
#include <Engine/Framework/Application.h>

using namespace StravaEngine;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdline, int cmdshow)
{

	Framework::ApplicationSpec spec;
	spec.m_nativeHandle = (NativeHandle)hInstance;
	spec.m_title = "Title";

	auto application = Framework::Application(spec);

	// Return this part of the WM_QUIT message to Windows.
	return 0;
}