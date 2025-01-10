#pragma once

#include <iterator>
#include <algorithm>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/String.h>
#include <Engine/Core/NonInstantiatable.h>

namespace StravaEngine::Core
{
struct StringUtility : private NonInstantiatable<StringUtility>
{
	static String ToString(wchar_t* const src, Size srcLength = 0);
};
}
