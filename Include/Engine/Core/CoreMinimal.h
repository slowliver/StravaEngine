#pragma once

#include <cstdint>
#include <cstddef>

#if defined(DEBUG) || defined(_DEBUG)
#define STRAVA_DEBUG 1
#else
#define STRAVA_DEBUG 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#define STRAVA_WINDOWS 1
#else
#define STRAVA_WINDOWS 0
#endif

#define STRAVA_VERIFY(condition)
#define STRAVA_VERIFYM(condition, message)

namespace StravaEngine::Core
{
using Byte = std::byte;
	
using Int8 = std::int8_t;
using UInt8 = std::uint8_t;
using Int16 = std::int16_t;
using UInt16 = std::uint16_t;
using Int32 = std::int32_t;
using UInt32 = std::uint32_t;
using Int64 = std::int64_t;
using UInt64 = std::uint64_t;
using Size = UInt64;

using NativeHandle = UInt64;

template <class Type>
void SafeDelete(Type*& pointer)
{
	delete pointer;
	pointer = nullptr;
}

#if 0
template <class Type, Size k_size>
void SafeDelete(Type(&a)[k_size])
{
	delete a;
}
#endif

template <class Type, Size k_size>
constexpr Size GetCount(const Type(&)[k_size]) { return k_size; }

#if 0
template <Core::Size N>
constexpr Core::Size GetCount(const BoolArray<N>&) { return N; }
#endif

template <class Enum>
constexpr Size GetCount(const Enum) { return ToUnderlying<Enum>(Enum::Count); }

template <class Type>
[[nodiscard]]
constexpr const Type& Min(const Type& lhs, const Type& rhs)
{
	return lhs > rhs ? rhs : lhs;
}

template <class Type>
[[nodiscard]]
constexpr const Type& Max(const Type& lhs, const Type& rhs)
{
	return lhs > rhs ? lhs : rhs;
}
}

using StravaEngine::Core::Byte;

using StravaEngine::Core::Int8;
using StravaEngine::Core::UInt8;
using StravaEngine::Core::Int16;
using StravaEngine::Core::UInt16;
using StravaEngine::Core::Int32;
using StravaEngine::Core::UInt32;
using StravaEngine::Core::Int64;
using StravaEngine::Core::UInt64;
using StravaEngine::Core::Size;

using StravaEngine::Core::NativeHandle;