#pragma once

#include <cstdint>

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