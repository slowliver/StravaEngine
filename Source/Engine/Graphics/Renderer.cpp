#include <Windows.h>

#include <Engine/Graphics/Renderer.h>
#include <Engine/Framework/Application.h>

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

bool Renderer::Initialize()
{
	return true;
}

void Renderer::Terminate()
{}

void Renderer::Update()
{

}
}