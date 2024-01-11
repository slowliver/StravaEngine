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

enum class Format : UInt32
{
	// Unknown
	Unknown						=  0,

	// Color 8bis
	R8_Typeless					=  1,
	R8_UNorm					=  2,
	R8_SNorm					=  3,
	R8_UInt						=  4,
	R8_SInt						=  5,

	// Color 16bits
	R16_Typeless				=  6,
	R16_SFloat					=  7,
	R16_UNorm					=  8,
	R16_SNorm					=  9,
	R16_UInt					= 10,
	R16_SInt					= 11,
	R8G8_Typeless				= 12,
	R8G8_UNorm					= 13,
	R8G8_SNrom					= 14,
	R8G8_UInt					= 15,
	R8G8_SInt					= 16,
	R9G9B9E5_SharedExp			= 17,

	// Special Color 16bits
	B5G6R5_UNorm				= 18,
	B5G5R5A1_UNorm				= 19,
	B4G4R4A4_UNorm				= 20,

	// Depth 16bits
	D16_UNorm					= 21,

	// Color 32bits
	R32_Typeless				= 22,
	R32_SFloat					= 23,
	R32_UInt					= 24,
	R32_SInt					= 25,
	R24G8_Typeless				= 26,
	R24_UNorm_X8_Typeless		= 27,
	X24_Typeless_G8_UInt		= 28,
	R16G16_Typeless				= 29,
	R16G16_SFloat				= 30,
	R16G16_UNorm				= 31,
	R16G16_SNorm				= 32,
	R16G16_UInt					= 33,
	R16G16_SInt					= 34,
	R11G11B10_UFloat			= 35,
	R10G10B10A2_Typeless		= 36,
	R10G10B10A2_UNorm			= 37,
	R10G10B10A2_UInt			= 38,
	R8G8B8A8_Typeless			= 39,
	R8G8B8A8_UNorm				= 40,
	R8G8B8A8_SNorm				= 41,
	R8G8B8A8_UNormSRGB			= 42,
	R8G8B8A8_UInt				= 43,
	R8G8B8A8_SInt				= 44,

	// Special Color 32bits
	B8G8R8A8_Typeless			= 45,
	B8G8R8A8_UNorm				= 46,
	B8G8R8A8_UNormSRGB			= 47,
	B8G8R8X8_Typeless			= 48,
	B8G8R8X8_UNorm				= 49,
	B8G8R8X8_UNormSRGB			= 50,
	R8G8B8G8_UNorm				= 51,	// like YVUV
	G8R8G8B8_UNorm				= 52,	// like YUYV (aka YUY2)

	// Depth 32bits
	D32_SFloat					= 53,
	D24_UNorm_S8_UInt			= 54,

	// Color 48bits
	R32G32B32_Typeless			= 55,
	R32G32B32_SFloat			= 56,
	R32G32B32_UInt				= 57,
	R32G32B32_SInt				= 58,

	// Color 64bits
	R32G32_Typeless				= 59,
	R32G32_SFloat				= 60,
	R32G32_UInt					= 61,
	R32G32_SInt					= 62,
	R32G8X24_Typeless			= 63,
	R32_SFloat_X8X24_Typeless	= 64,
	X32_Typeless_G8X24_UInt		= 65,
	R16G16B16A16_Typeless		= 66,
	R16G16B16A16_SFloat			= 67,
	R16G16B16A16_UNorm			= 68,
	R16G16B16A16_SNorm			= 69,
	R16G16B16A16_UInt			= 70,
	R16G16B16A16_SInt			= 71,

	// Depth 64bits
	D32_SFloat_S8X24_UInt		= 72,

	// Color 128bits
	R32G32B32A32_Typeless		= 73,
	R32G32B32A32_SFloat			= 74,
	R32G32B32A32_UInt			= 75,
	R32G32B32A32_SInt			= 75,

	// Block Compression BC1
	BC1_Typeless				= 76,
	BC1_UNorm					= 77,
	BC1_UNormSRGB				= 78,

	// Block Compression BC2
	BC2_Typeless				= 79,
	BC2_UNorm					= 80,
	BC2_UNormSRGB				= 81,

	// Block Compression BC3
	BC3_Typeless				= 82,
	BC3_UNorm					= 83,
	BC3_UNormSRGB				= 84,

	// Block Compression BC4
	BC4_Typeless				= 85,
	BC4_UNorm					= 86,
	BC4_SNorm					= 87,

	// Block Compression BC5
	BC5_Typeless				= 88,
	BC5_UNorm					= 89,
	BC5_SNorm					= 90,

	// Block Compression BC6(H)
	BC6_Typeless				= 91,
	BC6_UHalf					= 92,
	BC6_SHalf					= 93,

	// Block Compression BC7
	BC7_Typeless				= 94,
	BC7_UNorm					= 95,
	BC7_UNormSRGB				= 96,

#if 0
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
	DXGI_FORMAT_FORCE_UINT = 0xffffffff
#endif
	Count						= BC7_UNormSRGB + 1
};

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
	Unknown,
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	Count
};

struct Viewport
{
	float m_left;
	float m_top;
	float m_width;
	float m_height;
	float m_minDepth;
	float m_maxDepth;
};
inline bool operator==(const Viewport& x, const Viewport& y)
{
	return
		x.m_left == y.m_left &&
		x.m_top == y.m_top &&
		x.m_width == y.m_width &&
		x.m_height == y.m_height &&
		x.m_minDepth == y.m_minDepth &&
		x.m_maxDepth == y.m_maxDepth;
}
inline bool operator!=(const Viewport& x, const Viewport& y) { return !(x == y); }

using NativeResouce = std::intptr_t;
}