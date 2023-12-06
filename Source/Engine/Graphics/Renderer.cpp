#include <Windows.h>

#include <Engine/Graphics/Renderer.h>
#include "D3D12/D3D12Core.h"

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
	D3D12::D3D12Core::s_instance.reset(new D3D12::D3D12Core());
	D3D12::D3D12Core::s_instance->Initialize(spec);
	return true;
}

void Renderer::Terminate()
{
	D3D12::D3D12Core::s_instance->Terminate();
	D3D12::D3D12Core::s_instance.reset(nullptr);
}

void Renderer::Update()
{

}
}