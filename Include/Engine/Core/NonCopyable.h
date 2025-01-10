#pragma once

namespace StravaEngine::Core
{
template <class Type>
class NonCopyable
{
protected:
	constexpr NonCopyable() = default;
	constexpr ~NonCopyable() = default;
	constexpr NonCopyable(const NonCopyable&) = delete;
	constexpr Type& operator= (const Type&) = delete;
};
}
