#pragma once

#include <string>
#include <memory>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Graphics
{
struct RendererSpec final
{
	NativeHandle m_nativeWindowHandle;
};

class Renderer
{
public:
	static Renderer* CreateInstance();
	static Renderer* GetInstance();
	static void DestroyInstance();

private:
	Renderer();

public:
	bool Initialize(const RendererSpec& spec);
	void Terminate();
	void OnUpdate();

private:
	static std::unique_ptr<Renderer> s_instance;
	std::unique_ptr<class GraphicsCommandBuffer> m_graphicsCommandBuffer;
};
}