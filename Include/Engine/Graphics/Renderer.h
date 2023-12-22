#pragma once

#include <memory>
#include <functional>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>

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

	Core::ArrayList<std::function<void(void)>>& GetResourceCreationQueue() { return m_resourceCreationQueue; }

private:
	static std::unique_ptr<Renderer> s_instance;
	Core::ArrayList<std::function<void(void)>> m_resourceCreationQueue;
	std::unique_ptr<class GraphicsCommandBuffer> m_graphicsCommandBuffer;
	std::unique_ptr<class DrawTriangleSamplePass> m_drawTriangleSamplePass;
};
}