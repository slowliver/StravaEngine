#include <Engine/Core/Diagnostics/Debugger.h>

#include <Windows.h>

namespace StravaEngine::Core::Diagnostics
{
void Debugger::Break()
{
	::DebugBreak();
}

static constexpr const char* k_logLevelToStrings[] =
{
	"Information",
	"Warning",
	"Error"
};

static constexpr Size k_debugPrintBufferMax = 4096;

#if 0
void Debugger::Log(LogLevel logLevel, const char* file, UInt32 line, const char* fmt, ...)
{
	char buffer1[1024] = {};
	char buffer2[k_debugPrintBufferMax] = {};
	// Make buffer1
	{
#if defined(STRAVA_WIN)
		SYSTEMTIME systemTime = {};
		::GetLocalTime(&systemTime);
		std::snprintf
		(
			buffer1,
			GetCount(buffer1),
			"%s(%u): [%s] %04d/%02d/%02d %02d:%02d:%02d: ",
			file,
			line,
			k_levelToStrings[ToUnderlying(logLevel)],
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond
		);
#else
		auto time = std::time(nullptr);
		auto localTime = std::localtime(&time);
		snprintf
		(
			buffer1,
			GetCount(buffer1),
			"[%s] %04d/%02d/%02d %02d:%02d:%02d %s %d: ",
			k_levelToStrings[UnderlyingCast(logLevel)],
			localTime->tm_year + 1900,
			localTime->tm_mon + 1,
			localTime->tm_mday,
			localTime->tm_hour,
			localTime->tm_min,
			localTime->tm_sec,
			file,
			line
		);
#endif
	}
	// Make buffer2
	{
		std::va_list va;
		va_start(va, fmt);
		std::vsnprintf(buffer2, k_debugPrintBufferMax, fmt, va);
		va_end(va);
	}
	// Make log string.
	auto log = String(buffer1) + String(buffer2) + String("\n");
#if defined(STRAVA_WIN)
	{
		LockGuard lockForDebugPrint(g_mutexForDebugPrint);
		::OutputDebugStringA(log.c_str());
	}
#endif
	{
		GetLogStream() << log << std::flush;
	}
}

	struct LogStreamDeleter
	{
		void operator()(std::ofstream* logStream) const
		{
			if (logStream && logStream->is_open())
			{
				logStream->close();
			}
			delete logStream;
		}
	};

String g_logFilePath;
UniquePointer<std::ofstream, LogStreamDeleter> g_logStream;
Mutex g_mutexForDebugPrint;

std::ofstream& GetLogStream(void)
{
	LockGuard lockForDebugPrint(g_mutexForDebugPrint);
	if (!g_logStream)
	{
		g_logStream.reset(new std::ofstream());
	}
	if (!g_logStream->is_open())
	{
		g_logStream->open(g_logFilePath, std::ios::out/* | std::ios::app */);
	}
	return *g_logStream;
}
}

void Initialize(const StringView path)
{
	LockGuard lockForDebugPrint(g_mutexForDebugPrint);
	if (!g_logStream || !g_logStream->is_open())
	{
		g_logFilePath = path;
		g_logStream.reset(new std::ofstream());
		g_logStream->open(g_logFilePath.c_str(), std::ios::out/* | std::ios::app */);
	}
	else
	{
		STRAVA_LOG_PRINT_ERROR("DebugInit has already called or DebugPrint has already called.");
	}
}

void Print(const Internal::Level logLevel, const Char* file, const UInt32 line, const Char* fmt, ...)
{

}
#endif
}