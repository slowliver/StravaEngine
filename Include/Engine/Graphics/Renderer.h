#pragma once

#include <string>
#include <memory>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Graphics
{
struct RendererSpec final
{
	NativeHandle m_nativeHandle;
	std::string m_title;
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
	bool Initialize();
	void Terminate();
	void Update();

private:
	static std::unique_ptr<Renderer> s_instance;
};
}