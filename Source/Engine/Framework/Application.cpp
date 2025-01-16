#include <Windows.h>

#include <Engine/Framework/Application.h>

#include <Engine/Graphics/Renderer.h>
#include <imgui.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace StravaEngine::Framework
{
static LRESULT Application_WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//	DXSample* pSample = reinterpret_cast<DXSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return 0;
	}

	switch (message)
	{
	case WM_CREATE:
	{
		// Save the DXSample* passed in to CreateWindow.
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	return 0;

	case WM_KEYDOWN:
		//		if (pSample)
		//		{
		//			pSample->OnKeyDown(static_cast<UINT8>(wParam));
		//		}
		return 0;

	case WM_KEYUP:
		//		if (pSample)
		//		{
		//			pSample->OnKeyUp(static_cast<UINT8>(wParam));
		//		}
		return 0;

	case WM_PAINT:
	{
		auto* renderer = Graphics::Renderer::GetInstance();
		renderer->OnUpdate();
	}
		//		if (pSample)
		//		{
		//			pSample->OnUpdate();
		//			pSample->OnRender();
		//		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

Application::Application(const ApplicationSpec& spec)
{
	HINSTANCE hInstance = (HINSTANCE)spec.m_nativeHandle;

	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = Application_WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = "DXSampleClass";

	RegisterClassExA(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(800), static_cast<LONG>(600) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	auto hwnd = CreateWindowExA
	(
		0,
		windowClass.lpszClassName,
		"Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		nullptr
	);

	// Initialize the sample. OnInit is defined in each child-implementation of DXSample.
	// pSample->OnInit();

	ShowWindow(hwnd, SW_SHOW);

	auto* renderer = Graphics::Renderer::CreateInstance();
	Graphics::RendererSpec rendererSpec;
	rendererSpec.m_nativeWindowHandle = (NativeHandle)hwnd;
	renderer->Initialize(rendererSpec);


	// Main sample loop.
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Process any messages in the queue.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//	pSample->OnDestroy();
}
}