#pragma once

#include <Windows.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Array.h>

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

enum class VertexAttributeType : UInt8
{
	Position,
	Normal,
	Tangent,
	Color,
	TexCoord,
	BlendWeight,
	BlendIndices,
	Count
};

constexpr const char* k_vertexAttributeTypeNames[] =
{
	"POSITION",
	"NORMAL",
	"TANGENT",
	"COLOR",
	"TEXCOORD",
	"BLENDWEIGHT",
	"BLENDINDICES",
};

static_assert(Core::ToUnderlying(VertexAttributeType::Count) == Core::GetCount(k_vertexAttributeTypeNames));

enum class VertexAttributeFormat : UInt8
{
	Float32,
	Float16,
	UNorm8,
	SNorm8,
	Count
};

enum class VertexAttributeDataType : UInt8
{
	PerVertex,
	PerInstance,
	Count
};

struct VertexAttribute
{
	VertexAttributeType m_type = VertexAttributeType::Position;
	UInt8 m_index = 0;
	VertexAttributeFormat m_format = VertexAttributeFormat::Float32;
	UInt8 m_dimension = 4;
	VertexAttributeDataType m_dataType = VertexAttributeDataType::PerVertex;
	VertexAttribute() = default;
	VertexAttribute(VertexAttributeType type, UInt8 index, VertexAttributeFormat format, UInt8 dimension, VertexAttributeDataType dataType)
		: m_type(type)
		, m_index(index)
		, m_format(format)
		, m_dimension(dimension)
		, m_dataType(dataType)
	{
		STRAVA_ASSERT(0 < m_dimension && dimension <= 4);
	}
	VertexAttribute(VertexAttributeType type, UInt8 index, VertexAttributeFormat format, UInt8 dimension)
		: VertexAttribute(type, index, format, dimension, VertexAttributeDataType::PerVertex)
	{}
};

template <Size k_attributeCount>
struct VertexLayout : public Core::Array<VertexAttribute, k_attributeCount>
{};

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