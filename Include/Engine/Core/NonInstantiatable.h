#pragma once

#include <Engine/Core/NonCopyable.h>

namespace StravaEngine::Core
{
template <class Type>
class NonInstantiatable : private NonCopyable<NonInstantiatable<Type>>
{
protected:
	constexpr NonInstantiatable() = delete;
#if 0
	template <class... Args>
	constexpr NonInstantiatable(Args...) = delete;
#endif
};
}
