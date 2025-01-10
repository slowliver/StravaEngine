#pragma once

#include <initializer_list>
#include <vector>
#include <xutility>
#include <type_traits>

#include <Engine/Core/ArrayList.h>
#include <Engine/Core/NonInstantiatable.h>
#include <Engine/Core/String.h>

namespace StravaEngine::Framework
{
struct Environment : private Core::NonInstantiatable<Environment>
{
	static const Core::ArrayList<Core::String>& GetCommandLineArgs();
};
}
