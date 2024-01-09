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

class GraphicsCommandBuffer;
class DrawTriangleSamplePass;
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

	GraphicsCommandBuffer* GetGraphicsCommandBuffer() { return m_graphicsCommandBuffer.get(); }
	Core::ArrayList<std::function<void(void)>>& GetResourceQueueCreate() { return m_resourceQueueCreate; }
	Core::ArrayList<std::function<void(void)>>& GetResourceQueueRelease() { return m_resourceQueueRelease; }

private:
	void OnPrepareResource();
	void OnPreRender();
	void OnRender();
	void OnPostRender();
	void OnExecuteCommandBuffer();

private:
	static std::unique_ptr<Renderer> s_instance;
	Core::ArrayList<std::function<void(void)>> m_resourceQueueCreate;
	Core::ArrayList<std::function<void(void)>> m_resourceQueueRelease;
	std::unique_ptr<GraphicsCommandBuffer> m_graphicsCommandBuffer;
	std::unique_ptr<DrawTriangleSamplePass> m_drawTriangleSamplePass;
};
}