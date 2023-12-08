#pragma once

#include <Engine/Core/CoreMinimal.h>

#if defined(STRAVA_LOG)
#define STRAVA_LOG_INIT(path)				::StravaEngine::Kernel::Internal::Initialize(path)
#define STRAVA_LOG_PRINT_INFORMATION(...)	::StravaEngine::Kernel::Internal::Print(::StravaEngine::Kernel::Internal::Level::Information, __FILE__, __LINE__, ##__VA_ARGS__)
#define STRAVA_LOG_PRINT_WARNING(...)		::StravaEngine::Kernel::Internal::Print(::StravaEngine::Kernel::Internal::Level::Warning, __FILE__, __LINE__, ##__VA_ARGS__)
#define STRAVA_LOG_PRINT_ERROR(...)			::StravaEngine::Kernel::Internal::Print(::StravaEngine::Kernel::Internal::Level::Error, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define STRAVA_LOG_INIT(...)				((void)0)
#define STRAVA_LOG_PRINT_INFORMATION(...)	((void)0)
#define STRAVA_LOG_PRINT_WARNING(...)		((void)0)
#define STRAVA_LOG_PRINT_ERROR(...)			((void)0)
#endif

namespace StravaEngine::Core::Diagnostics
{
class Debugger
{
public:
	enum class LogLevel
	{
		Information,
		Warning,
		Error,
		Count
	};
	static void Break();
	static void Log(LogLevel logLevel, const char* file, UInt32 line, const char* fmt, ...);
};
}

