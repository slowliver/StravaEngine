#include <stdio.h>

#include <windows.h>

#include <Engine/Framework/Application.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdline, int cmdshow)
{
	StravaEngine::Framework::ApplicationSpec spec;
	spec.m_nativeHandle = (NativeHandle)hInstance;
	spec.m_title = "Title";

	auto application = StravaEngine::Framework::Application(spec);

	// Return this part of the WM_QUIT message to Windows.
	return 0;
}