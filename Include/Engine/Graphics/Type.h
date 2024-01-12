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

namespace Internal
{
struct FormatSpecTable
{
	Size m_sizePerPixel;
	struct BitsPerChannel
	{
		// Color
		UInt8 m_r;
		UInt8 m_g;
		UInt8 m_b;
		UInt8 m_a;

		// Exp
		UInt8 m_e;

		// Depth Stencil
		UInt8 m_d;
		UInt8 m_s;
	} m_bitsPerChannel;
	bool m_compressed;
};
constexpr FormatSpecTable k_formatSpecTable[] =
{
	// Unknown
	{  0, {  0,  0,  0,  0,  0,  0,  0 }, false },	// Unknown

	// Color 8bis
	{  1, {  8,  0,  0,  0,  0,  0,  0 }, false },	// R8_Typeless
	{  1, {  8,  0,  0,  0,  0,  0,  0 }, false },	// R8_UNorm
	{  1, {  8,  0,  0,  0,  0,  0,  0 }, false },	// R8_SNorm
	{  1, {  8,  0,  0,  0,  0,  0,  0 }, false },	// R8_UInt
	{  1, {  8,  0,  0,  0,  0,  0,  0 }, false },	// R8_SInt

	// Color 16bits
	{  2, { 16,  0,  0,  0,  0,  0,  0 }, false },	// R16_Typeless
	{  2, { 16,  0,  0,  0,  0,  0,  0 }, false },	// R16_SFloat
	{  2, { 16,  0,  0,  0,  0,  0,  0 }, false },	// R16_UNorm
	{  2, { 16,  0,  0,  0,  0,  0,  0 }, false },	// R16_SNorm
	{  2, { 16,  0,  0,  0,  0,  0,  0 }, false },	// R16_UInt
	{  2, { 16,  0,  0,  0,  0,  0,  0 }, false },	// R16_SInt
	{  2, {  8,  8,  0,  0,  0,  0,  0 }, false },	// R8G8_Typeless
	{  2, {  8,  8,  0,  0,  0,  0,  0 }, false },	// R8G8_UNorm
	{  2, {  8,  8,  0,  0,  0,  0,  0 }, false },	// R8G8_SNorm
	{  2, {  8,  8,  0,  0,  0,  0,  0 }, false },	// R8G8_UInt
	{  2, {  8,  8,  0,  0,  0,  0,  0 }, false },	// R8G8_SInt
	{  2, {  9,  9,  9,  0,  5,  0,  0 }, false },	// R9G9B9E5_SharedExp

	// Special Color 16bits
	{  2, {  5,  6,  5,  0,  0,  0,  0 }, false },	// B5G6R5_UNorm
	{  2, {  5,  5,  5,  1,  0,  0,  0 }, false },	// B5G5R5A1_UNorm
	{  2, {  4,  4,  4,  4,  0,  0,  0 }, false },	// B4G4R4A4_UNorm

	// Depth 16bits
	{  2, {  0,  0,  0,  0,  0, 16,  0 }, false },	// D16_UNorm

	// Color 32bits
	{  4, { 32,  0,  0,  0,  0,  0,  0 }, false },	// R32_Typeless
	{  4, { 32,  0,  0,  0,  0,  0,  0 }, false },	// R32_SFloat
	{  4, { 32,  0,  0,  0,  0,  0,  0 }, false },	// R32_UInt
	{  4, { 32,  0,  0,  0,  0,  0,  0 }, false },	// R32_SInt
	{  4, { 24,  8,  0,  0,  0,  0,  0 }, false },	// R24G8_Typeless
	{  4, { 24,  0,  0,  0,  0,  0,  0 }, false },	// R24_UNorm_X8_Typeless
	{  4, {  0,  8,  0,  0,  0,  0,  0 }, false },	// X24_Typeless_G8_UInt
	{  4, { 16, 16,  0,  0,  0,  0,  0 }, false },	// R16G16_Typeless
	{  4, { 16, 16,  0,  0,  0,  0,  0 }, false },	// R16G16_SFloat
	{  4, { 16, 16,  0,  0,  0,  0,  0 }, false },	// R16G16_UNorm
	{  4, { 16, 16,  0,  0,  0,  0,  0 }, false },	// R16G16_SNorm
	{  4, { 16, 16,  0,  0,  0,  0,  0 }, false },	// R16G16_UInt
	{  4, { 16, 16,  0,  0,  0,  0,  0 }, false },	// R16G16_SInt
	{  4, { 11, 11, 10,  0,  0,  0,  0 }, false },	// R11G11B10_UFloat
	{  4, { 10, 10, 10,  2,  0,  0,  0 }, false },	// R10G10B10A2_Typeless
	{  4, { 10, 10, 10,  2,  0,  0,  0 }, false },	// R10G10B10A2_UNorm
	{  4, { 10, 10, 10,  2,  0,  0,  0 }, false },	// R10G10B10A2_UInt
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// R8G8B8A8_Typeless
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// R8G8B8A8_UNorm
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// R8G8B8A8_SNorm
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// R8G8B8A8_UNormSRGB
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// R8G8B8A8_UInt
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// R8G8B8A8_SInt

	// Special Color 32bits
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// B8G8R8A8_Typeless
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// B8G8R8A8_UNorm
	{  4, {  8,  8,  8,  8,  0,  0,  0 }, false },	// B8G8R8A8_UNormSRGB
	{  4, {  8,  8,  8,  0,  0,  0,  0 }, false },	// B8G8R8X8_Typeless
	{  4, {  8,  8,  8,  0,  0,  0,  0 }, false },	// B8G8R8X8_UNorm
	{  4, {  8,  8,  8,  0,  0,  0,  0 }, false },	// B8G8R8X8_UNormSRGB
	{  4, {  8,  8,  8,  0,  0,  0,  0 }, false },	// R8G8B8G8_UNorm
	{  4, {  8,  8,  8,  0,  0,  0,  0 }, false },	// G8R8G8B8_UNorm

	// Depth 32bits
	{  4, {  0,  0,  0,  0,  0, 32,  0 }, false },	// D32_SFloat
	{  4, {  0,  0,  0,  0,  0, 24,  8 }, false },	// D24_UNorm_S8_UInt

	// Color 64bits
	{ 16, { 32, 32,  0,  0,  0,  0,  0 }, false },	// R32G32_Typeless
	{ 16, { 32, 32,  0,  0,  0,  0,  0 }, false },	// R32G32_SFloat
	{ 16, { 32, 32,  0,  0,  0,  0,  0 }, false },	// R32G32_UInt
	{ 16, { 32, 32,  0,  0,  0,  0,  0 }, false },	// R32G32_SInt
	{ 16, { 32,  8,  0,  0,  0,  0,  0 }, false },	// R32G8X24_Typeless
	{ 16, { 32,  0,  0,  0,  0,  0,  0 }, false },	// R32_SFloat_X8X24_Typeless
	{ 16, { 32,  8,  0,  0,  0,  0,  0 }, false },	// X32_Typeless_G8X24_UInt
	{ 16, { 16, 16, 16, 16,  0,  0,  0 }, false },	// R16G16B16A16_Typeless
	{ 16, { 16, 16, 16, 16,  0,  0,  0 }, false },	// R16G16B16A16_SFloat
	{ 16, { 16, 16, 16, 16,  0,  0,  0 }, false },	// R16G16B16A16_UNorm
	{ 16, { 16, 16, 16, 16,  0,  0,  0 }, false },	// R16G16B16A16_SNorm
	{ 16, { 16, 16, 16, 16,  0,  0,  0 }, false },	// R16G16B16A16_UInt
	{ 16, { 16, 16, 16, 16,  0,  0,  0 }, false },	// R16G16B16A16_SInt

	// Depth 64bits
	{ 16, {  0,  0,  0,  0,  0, 32,  8 }, false },	// D32_SFloat_S8X24_UInt

	// Color 96bits
	{ 24, { 32, 32, 32,  0,  0,  0,  0 }, false },	// R32G32B32_Typeless
	{ 24, { 32, 32, 32,  0,  0,  0,  0 }, false },	// R32G32B32_SFloat
	{ 24, { 32, 32, 32,  0,  0,  0,  0 }, false },	// R32G32B32_UInt
	{ 24, { 32, 32, 32,  0,  0,  0,  0 }, false },	// R32G32B32_SInt

	// Color 128bits
	{ 24, { 32, 32, 32, 32,  0,  0,  0 }, false },	// R32G32B32A32_Typeless
	{ 24, { 32, 32, 32, 32,  0,  0,  0 }, false },	// R32G32B32A32_SFloat
	{ 24, { 32, 32, 32, 32,  0,  0,  0 }, false },	// R32G32B32A32_UInt
	{ 24, { 32, 32, 32, 32,  0,  0,  0 }, false },	// R32G32B32A32_SInt

	// Block Compression BC1
	{  0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC1_Typeless
	{  0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC1_UNorm
	{  0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC1_UNormSRGB

	// Block Compression BC2
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC2_Typeless
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC2_UNorm
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC2_UNormSRGB

	// Block Compression BC3
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC3_Typeless
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC3_UNorm
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC3_UNormSRGB

	// Block Compression BC4
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC4_Typeless
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC4_UNorm
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC4_SNorm

	// Block Compression BC5
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC5_Typeless
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC5_UNorm
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC5_SNorm

	// Block Compression BC6(H)
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC6_Typeless
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC6_UFloat
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC6_SFloat

	// Block Compression BC7
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC7_Typeless
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC7_UNorm
	{ 0, {  0,  0,  0,  0,  0,  0,  0 },  true },	// BC7_UNormSRGB
};
}

struct Format
{
	enum Raw : UInt32
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

		// Color 64bits
		R32G32_Typeless				= 55,
		R32G32_SFloat				= 56,
		R32G32_UInt					= 57,
		R32G32_SInt					= 58,
		R32G8X24_Typeless			= 59,
		R32_SFloat_X8X24_Typeless	= 60,
		X32_Typeless_G8X24_UInt		= 61,
		R16G16B16A16_Typeless		= 62,
		R16G16B16A16_SFloat			= 63,
		R16G16B16A16_UNorm			= 64,
		R16G16B16A16_SNorm			= 65,
		R16G16B16A16_UInt			= 66,
		R16G16B16A16_SInt			= 67,

		// Depth 64bits
		D32_SFloat_S8X24_UInt		= 68,

		// Color 96bits
		R32G32B32_Typeless			= 69,
		R32G32B32_SFloat			= 70,
		R32G32B32_UInt				= 71,
		R32G32B32_SInt				= 72,

		// Color 128bits
		R32G32B32A32_Typeless		= 73,
		R32G32B32A32_SFloat			= 74,
		R32G32B32A32_UInt			= 75,
		R32G32B32A32_SInt			= 76,

		// Block Compression BC1
		BC1_Typeless				= 77,
		BC1_UNorm					= 78,
		BC1_UNormSRGB				= 79,

		// Block Compression BC2
		BC2_Typeless				= 80,
		BC2_UNorm					= 81,
		BC2_UNormSRGB				= 82,

		// Block Compression BC3
		BC3_Typeless				= 83,
		BC3_UNorm					= 84,
		BC3_UNormSRGB				= 85,

		// Block Compression BC4
		BC4_Typeless				= 86,
		BC4_UNorm					= 87,
		BC4_SNorm					= 88,

		// Block Compression BC5
		BC5_Typeless				= 89,
		BC5_UNorm					= 90,
		BC5_SNorm					= 91,

		// Block Compression BC6(H)
		BC6_Typeless				= 92,
		BC6_UFloat					= 93,
		BC6_SFloat					= 94,

		// Block Compression BC7
		BC7_Typeless				= 95,
		BC7_UNorm					= 96,
		BC7_UNormSRGB				= 97,

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
	static_assert(Core::GetCount(Internal::k_formatSpecTable) == static_cast<Size>(Raw::Count));
	STRAVA_FORCE_INLINE static Size GetSizePerPixelOf(Raw type) { return Internal::k_formatSpecTable[type].m_sizePerPixel; }
	STRAVA_FORCE_INLINE static Size GetBitsOfROf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_r; }
	STRAVA_FORCE_INLINE static Size GetBitsOfGOf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_g; }
	STRAVA_FORCE_INLINE static Size GetBitsOfBOf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_b; }
	STRAVA_FORCE_INLINE static Size GetBitsOfAOf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_a; }
	STRAVA_FORCE_INLINE static Size GetBitsOfExponentOf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_e; }
	STRAVA_FORCE_INLINE static Size GetBitsOfDepthOf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_d; }
	STRAVA_FORCE_INLINE static Size GetBitsOfStencilOf(Raw type) { return Internal::k_formatSpecTable[type].m_bitsPerChannel.m_s; }
	STRAVA_FORCE_INLINE Size GetSizePerPixel() const { return GetSizePerPixelOf(static_cast<Raw>(m_raw)); }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfR() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_r; }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfG() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_g; }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfB() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_b; }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfA() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_a; }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfExponent() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_e; }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfDepth() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_d; }
	STRAVA_FORCE_INLINE UInt8 GetBitsOfStencil() const { return Internal::k_formatSpecTable[static_cast<Raw>(m_raw)].m_bitsPerChannel.m_s; }
	UInt32 m_raw;
};

enum class TextureDimension : UInt8
{
	Unknown,
	k1D,
	k1DArray,
	k2D,
	k2DArray,
	k2DMultisample,
	k2DMultisampleArray,
	k3D,
	Cube,
	CubeArray,
	Count
};

enum class Multisample : UInt8
{
	k1x,
	k2x,
	k4x,
	k8x,
	k16x,
	Count
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