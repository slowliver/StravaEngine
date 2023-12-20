#pragma once

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Graphics
{
enum class GraphicsAPI : UInt8
{
	Null,
	Direct3D12,
	Vulkan,
	Count
};

#if 0
enum class CPUFrameCount : UInt8
{
	Invalid = 0,
	One = 1,
	Two = 2,
	Three = 3,
	Max = Three,
};

enum class GPUBufferCount : UInt8
{
	Invalid = 0,
	Two = 2,
	Three = 3,
	Max = Three,
};
#endif

enum class PrimitiveTopology : UInt8
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	Count
};

struct Viewport
{
	float m_left = 0.0f;
	float m_top = 0.0f;
	float m_width = 0.0f;
	float m_height = 0.0f;
	float m_minDepth = 0.0f;
	float m_maxDepth = 1.0f;
};

using NativeResouce = std::intptr_t;
}