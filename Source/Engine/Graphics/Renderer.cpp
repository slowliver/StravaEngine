#include <Windows.h>

#include <Engine/Graphics/Renderer.h>

#include "D3D12/D3D12Core.h"
#include "CommandBuffer.h"
#include "DrawTriangleSamplePass.h"

namespace StravaEngine::Graphics
{
std::unique_ptr<Renderer> Renderer::s_instance = nullptr;

Renderer* Renderer::CreateInstance()
{
	s_instance.reset(new Renderer());
	return GetInstance();
}

Renderer* Renderer::GetInstance()
{
	return s_instance.get();
}

void Renderer::DestroyInstance()
{
	s_instance.reset(nullptr);
}

Renderer::Renderer()
{}

bool Renderer::Initialize(const RendererSpec& spec)
{
	bool result = false;

	D3D12::D3D12Core::s_instance.reset(new D3D12::D3D12Core());
	result = D3D12::D3D12Core::s_instance->Initialize(spec);
	if (!result)
	{
		return false;
	}

	m_graphicsCommandBuffer.reset(new GraphicsCommandBuffer(1024 * 512));
	m_graphicsCommandBuffer->Initialize();

	m_drawTriangleSamplePass.reset(new DrawTriangleSamplePass());
	m_drawTriangleSamplePass->Initialize();

	return true;
}

void Renderer::Terminate()
{
	D3D12::D3D12Core::s_instance->Terminate();
	D3D12::D3D12Core::s_instance.reset(nullptr);
}

void Renderer::OnUpdate()
{
	OnPrepareResource();
	OnPreRender();
	OnRender();
	OnPostRender();
	OnExecuteCommandBuffer();
}

void Renderer::OnPrepareResource()
{
	for (auto& resourceCreationFunc : m_resourceCreationQueue)
	{
		resourceCreationFunc();
	}
	// ‚±‚±‚ÅƒLƒ…[‚ðíœ‚·‚é‚×‚«‚È‚Ì‚©‚Í“ä.
	m_resourceCreationQueue.Clear();
}

void Renderer::OnPreRender()
{
	m_graphicsCommandBuffer->Reset();
}

void Renderer::OnRender()
{
	m_drawTriangleSamplePass->OnRender();
}

void Renderer::OnPostRender()
{

}

void Renderer::OnExecuteCommandBuffer()
{
	D3D12::D3D12Core::s_instance->OnPrepareCommandBuffer();
	D3D12::D3D12Core::s_instance->OnSubmitCommandBuffer(*m_graphicsCommandBuffer);
}
}