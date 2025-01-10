#include <Windows.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Framework/Environment.h>

namespace StravaEngine::Framework
{
static Core::ArrayList<Core::String> g_cachedCommandLineArgs;

const Core::ArrayList<Core::String>& Environment::GetCommandLineArgs()
{
	if (g_cachedCommandLineArgs.IsEmpty())
	{
		int argc;
		auto* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
		if (argv != nullptr)
		{
			for (UInt32 i = 0; i < argc; ++i)
			{
				g_cachedCommandLineArgs.Add(Core::String(argv[i]));
			}
			LocalFree(argv);
		}
	}
	return commandLineArgs;
}