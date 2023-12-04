#pragma once

#include <string>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Framework
{
struct ApplicationSpec final
{
	Core::NativeHandle m_nativeHandle;
	std::string m_title;
};

class Application
{
public:
	Application(const ApplicationSpec& spec);
};
}