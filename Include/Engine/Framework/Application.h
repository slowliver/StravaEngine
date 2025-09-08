#pragma once

#include <string>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Framework
{
struct ApplicationSpec final
{
	NativeHandle m_nativeHandle;
	std::string m_title;
	UInt32 m_windowWidth;
	UInt32 m_windowHeight;
};

class Application
{
public:
	Application() = default;
	virtual ~Application() = default;

	bool Initialize(const ApplicationSpec& spec);
	void Run();
	void Terminate();

};
}