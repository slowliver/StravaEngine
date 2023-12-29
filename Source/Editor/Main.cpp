#include <stdio.h>

#include <windows.h>

#include <stdio.h>

#include <Engine/Core/Array.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Core/ArrayProxy.h>
#include <Engine/Framework/Application.h>
#include <Engine/Core/Diagnostics/Debugger.h>

#include <intrin.h>

using namespace StravaEngine;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdline, int cmdshow)
{
	int a = 1;
	Core::ArrayProxy<const int> ap = Core::ArrayProxy<const int>(a);

	Framework::ApplicationSpec spec;
	spec.m_nativeHandle = (NativeHandle)hInstance;
	spec.m_title = "Title";

	auto application = Framework::Application(spec);

	// Return this part of the WM_QUIT message to Windows.
	return 0;
}