#pragma once

#include <memory>
#include <functional>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayProxy.h>
#include <Engine/Graphics/Type.h>
#include "D3D12/D3D12Core.h"

namespace StravaEngine::Graphics
{
#if defined(STRAVA_COMMAND_PACKET)
#undef STRAVA_COMMAND_PACKET
#endif
#if 0
#define STRAVA_COMMAND_PACKET_TYPE(p) \
namespace CommandPacketType	\
{																	\
static constexpr UInt32 p = ;										\
namespace ToString													\
{																	\
	static constexpr const char8_t u8#p;							\
}																	\
struct CommandPacket##p final : public CommandPacket<CommandPacketType::p>
;
#endif
#define STRAVA_COMMAND_PACKET(p) struct CommandPacket##p final : public CommandPacketBase<CommandPacketType::p>

class RenderTexture;
class Shader;
class VertexBuffer;
enum class CommandPacketType : UInt32
{
	// No Operation
	Unknown,
	NOP,

	ClearRenderTarget,

	// Render Pass
	BeginPass,
	EndPass,

	// Input Assembler
	SetPrimitiveTopology,
	SetVertexBuffers,

	// Vertex Shader
	SetVertexShader,

	// Hull Shader

	// Tessellator

	// Domain Shader

	// Geometry Shader

	// Pixel Shader
	SetPixelShader,

	// Stream Output

	// Rasterizer
	SetViewport,
	SetScissor,

	// Output Merger
	SetRenderTargets,

	// Draw
	Draw,

	// Native Command
	SetNativeCommand,

	Count
};

static constexpr const char8_t* k_commandPacketNames[] =
{
	// No Operation
	u8"Unknown",
	u8"NOP",

	u8"ClearRenderTarget",
	
	// Render Pass
	u8"BeginPass",
	u8"EndPass",

	// Input Assembler
	u8"SetPrimitiveTopology",
	u8"SetVertexBuffers",

	// Vertex Shader
	u8"SetVertexShader",

	// Hull Shader

	// Tessellator

	// Domain Shader

	// Geometry Shader

	// Pixel Shader
	u8"SetPixelShader",

	// Stream Output

	// Rasterizer
	u8"SetViewport",
	u8"SetScissor",

	// Output Merger
	u8"SetRenderTargets",

	// Draw
	u8"Draw",

	// Native Command
	u8"SetNativeCommand",
};

static_assert(Core::ToUnderlying(CommandPacketType::Count) == Core::GetCount(k_commandPacketNames));

template <CommandPacketType k_commandPacketType>
struct alignas(sizeof(void*)) CommandPacketBase
{
	static constexpr CommandPacketType k_type = k_commandPacketType;
	constexpr const char8_t* ToString() const { k_commandPacketNames[ToUnderlying(k_type)]; }
	CommandPacketType m_type;
	UInt32 m_size;
};

// Begin No Operation

STRAVA_COMMAND_PACKET(Unknown)
{};

STRAVA_COMMAND_PACKET(NOP)
{};

// End No Operation

STRAVA_COMMAND_PACKET(ClearRenderTarget)
{
	RenderTexture* m_renderTarget;
	float m_color[4];
};

// Begin Render Pass

STRAVA_COMMAND_PACKET(BeginPass)
{};

STRAVA_COMMAND_PACKET(EndPass)
{};

// End Render Pass

// Begin Input Assembler

STRAVA_COMMAND_PACKET(SetPrimitiveTopology)
{
	PrimitiveTopology m_primitiveTopology;
};

STRAVA_COMMAND_PACKET(SetVertexBuffers)
{
	UInt32 m_startSlot;
	UInt32 m_numBuffers;
};

// End Input Assembler

// Begin Vertex Shader

STRAVA_COMMAND_PACKET(SetVertexShader)
{
	Shader* m_vertexShader;
};

// End Vertex Shader

// Begin Pixel Shader

STRAVA_COMMAND_PACKET(SetPixelShader)
{
	Shader* m_pixelShader;
};

// End Pixel Shader

// Begin Rasterizer

STRAVA_COMMAND_PACKET(SetViewport)
{
	Viewport m_viewport;
};

STRAVA_COMMAND_PACKET(SetScissor)
{
	Core::Int32Rect m_scissor;
};

// End Rasterizer

STRAVA_COMMAND_PACKET(SetRenderTargets)
{
	UInt32 m_numRenderTargets;
	RenderTexture* m_renderTargets[8];
};

// Begin Draw

STRAVA_COMMAND_PACKET(Draw)
{
	UInt32 m_vertexCountPerInstance;
	UInt32 m_instanceCount;
	UInt32 m_startVertexLocation;
	UInt32 m_startInstanceLocation;
};

// End Draw

STRAVA_COMMAND_PACKET(SetNativeCommand)
{
//	std::function<void(void)> m_function;
};

class CommandBufferBase
{
public:
	CommandBufferBase() = delete;
	CommandBufferBase(const CommandBufferBase&) = delete;
	explicit CommandBufferBase(Size size);
	virtual ~CommandBufferBase();

	virtual void Reset();

	template <typename T>
	T& Push(const Size additionalSize = 0)
	{
		T& commandPacket = reinterpret_cast<T&>(*Push(sizeof(T), additionalSize));
		commandPacket.m_type = T::k_type;
		commandPacket.m_size = static_cast<UInt32>(std::intptr_t(m_back) - std::intptr_t(&commandPacket));
		return commandPacket;
	}

#if 0
	template <typename T>
	void End(const Size additionalSize = 0)
	{
		End(sizeof(T), additionalSize);
	}
#endif

	Byte* GetBegin() const { return m_begin; }
	Byte* GetEnd() const { return m_end; }
	Byte* GetBack() const { return m_back; }

	// Native Command
	void SetNativeCommand(std::function<void(void)> func);

private:
	Byte* Push(Size commandSize, Size additionalSize);
#if 0
	void End(Size commandSize, Size additionalSize);
#endif

private:
	Size m_size = 0;
	Byte* m_back = nullptr;
	Byte* m_begin = nullptr;
	Byte* m_end = nullptr;
};

class GraphicsCommandBuffer final : public CommandBufferBase
{
public:
	explicit GraphicsCommandBuffer(Size size)
		: CommandBufferBase(size)
	{}
	~GraphicsCommandBuffer() {}

	bool Initialize();
	void Terminate();

	void ClearRenderTarget(RenderTexture* renderTarget, float* color);

	// Render Pass
	void BeginPass();
	void EndPass();

	// Input Assembler
	void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);
	void SetVertexBuffers(UInt8 startSlot, Core::ArrayProxy<VertexBuffer*> buffers);

	// Vertex Shader
	void SetVertexShader(Shader* vertexShader);

	// Pixel Shader
	void SetPixelShader(Shader* pixelShader);

	// Rasterizer
	void SetViewport(const Viewport& viewport);
	void SetScissor(const Core::Int32Rect& scissor);

	// Output Merger
	void SetRenderTargets(Core::ArrayProxy<RenderTexture*> targets);

	// Draw
	void Draw(UInt32 vertexCount);
	void Draw(UInt32 vertexCountPerInstance, UInt32 instanceCount, UInt32 startVertexLocation, UInt32 startInstanceLocation);

//	CommandBuffer* GetCommandBuffer() { return m_commandBuffer; }

private:
//	CommandBuffer* m_commandBuffer = nullptr;
};

#if defined(STRAVA_COMMAND_PACKET)
#undef STRAVA_COMMAND_PACKET
#endif
}