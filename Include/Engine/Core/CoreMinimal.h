#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <type_traits>

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

#define STRAVA_BREAK do { ::DebugBreak(); } while(false);
#define STRAVA_ASSERT(condition) do { if (!(condition)) { STRAVA_BREAK; } } while(false);

#define STRAVA_VERIFY(condition)
#define STRAVA_VERIFYM(condition, message)

#if STRAVA_WINDOWS
#define STRAVA_FORCE_INLINE __forceinline
#else
#define STRAVA_FORCE_INLINE inline
#endif

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

struct Default {};
constexpr Default k_default;

template <class Type>
struct RectBase
{
	Type m_left;
	Type m_top;
	Type m_right;
	Type m_bottom;
	RectBase& SetWidth(Type width) { m_right = m_left + width; return this; }
	RectBase& SetHeight(Type height) { m_right = m_left + height; return this; }
	Type GetWidth(Type width) const { return m_right - m_left; }
	Type GetHeight(Type height) const { return m_right - m_left; }
};
template <class Type>
inline bool operator==(const RectBase<Type>& x, const RectBase<Type>& y) { return x.m_left == y.m_left && x.m_top == y.m_top && x.m_right == y.m_right && x.m_bottom == y.m_bottom; }
template <class Type>
inline bool operator!=(const RectBase<Type>& x, const RectBase<Type>& y) { return !(x == y); }

using Int32Rect = RectBase<Int32>;

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

template <class Enum>
using UnderlyingType = std::underlying_type_t<Enum>;

template <class Enum>
constexpr UnderlyingType<Enum> ToUnderlying(Enum e) { return static_cast<UnderlyingType<Enum>>(e); }

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

template <class Type>
[[nodiscard]]
constexpr const Type& Clamp(const Type& x, const Type& min, const Type& max)
{
	return Max(Min(x, max), min);
}

STRAVA_FORCE_INLINE void* Memcpy(void* dst, const void* src, Size size)
{
	return std::memcpy(dst, src, size);
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
