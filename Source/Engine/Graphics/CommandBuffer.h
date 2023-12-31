#pragma once

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

class VertexBuffer;

enum class CommandPacketType : UInt32
{
	// No Operation.
	NOP,

	ClearRenderTarget,

	// Input Assembler
	SetPrimitiveTopology,
	SetVertexBuffers,

	// Vertex Shader

	// Hull Shader

	// Tessellator

	// Domain

	// Geometry

	// Stream Output

	// Rasterizer
	SetViewport,
	SetScissor,

	// Output Merger
	SetRenderTargets,

	// Native Command
	SetNativeCommand,

	Count
};

static constexpr const char8_t* k_commandPacketNames[] =
{
	// No Operation.
	u8"NOP",

	u8"ClearRenderTarget",

	// Input Assembler
	u8"SetPrimitiveTopology",
	u8"SetVertexBuffers",

	// Vertex Shader

	// Hull Shader

	// Tessellator

	// Domain

	// Geometry

	// Stream Output

	// Rasterizer
	u8"SetViewport",
	u8"SetScissor",

	// Pixel Shader

	// Output Merger
	u8"SetRenderTargets",

	// Native Command
	u8"SetNativeCommand",
};

static_assert(Core::ToUnderlying(CommandPacketType::Count) == Core::GetCount(k_commandPacketNames));

template <CommandPacketType k_commandPacketType>
struct alignas(sizeof(void*)) CommandPacketBase
{
	static constexpr CommandPacketType k_type = k_commandPacketType;
	constexpr const char8_t* ToString() const { k_commandPacketNames[ToUnderlying(k_type)]; }
	const CommandPacketType m_type = k_commandPacketType;
};

#if 0
STRAVA_COMMAND_PACKET(ClearRenderTarget)
{
	NativeResouce m_renderTarget = nullptr;
	Kernel::Color m_clearColor = Kernel::Color();
};
#endif

STRAVA_COMMAND_PACKET(SetPrimitiveTopology)
{
	PrimitiveTopology m_primitiveTopology = PrimitiveTopology::TriangleList;
};

STRAVA_COMMAND_PACKET(SetVertexBuffers)
{
	UInt8 m_startSlot = 0;
	UInt8 m_numBuffers = 0;
	Core::ArrayProxy<VertexBuffer*> m_buffers = nullptr;
	UInt8 m_offset = 0;
};

STRAVA_COMMAND_PACKET(SetViewport)
{
	Viewport m_viewport = Viewport();
};

STRAVA_COMMAND_PACKET(SetScissor)
{
	Core::Int32Rect m_scissor = Core::Int32Rect();
};

#if 0
STRAVA_COMMAND_PACKET(SetRenderTargets)
{
	Kernel::Size m_numRenderTargets = 0;
	NativeResouce m_renderTargets[8] = {};
};
#endif

STRAVA_COMMAND_PACKET(SetNativeCommand)
{
	std::function<void(void)> m_function = nullptr;
};

class CommandBufferBase
{
public:
	CommandBufferBase() = delete;
	CommandBufferBase(const CommandBufferBase&) = delete;
	explicit CommandBufferBase(Size size);
	virtual ~CommandBufferBase();

	void Reset() {}

	template <typename T>
	T& Push(const Size additionalSize = 0)
	{
		return reinterpret_cast<T&>(*Push(sizeof(T), additionalSize));
	}

#if 0
	template <typename T>
	void End(const Size additionalSize = 0)
	{
		End(sizeof(T), additionalSize);
	}
#endif

	Byte* GetBegin() { return m_begin; }

	// Native Command
	void SetNativeCommand(std::function<void(void)> func);

private:
	Byte* Push(Size commandSize, Size additionalSize);
#if 0
	void End(Size commandSize, Size additionalSize);
#endif

private:
	Size m_size = 0;
	Byte* m_current = nullptr;
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

	void Reset() {}

//	void ClearRenderTarget(NativeResouce* const renderTarget, const Kernel::Color clearColor);

	// Input Assembler
	void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);
	void SetVertexBuffers(UInt8 startSlot, Core::ArrayProxy<VertexBuffer*> buffers, UInt8 offset = 0);

	// Rasterizer
	void SetViewport(const Viewport& viewport);
	void SetScissor(const Core::Int32Rect& scissor);

	// Output Merger
//	void SetRenderTargets(const Kernel::Size numRenderTargets, NativeResouce* const renderTargets);

//	CommandBuffer* GetCommandBuffer() { return m_commandBuffer; }

private:
//	CommandBuffer* m_commandBuffer = nullptr;
};

#if defined(STRAVA_COMMAND_PACKET)
#undef STRAVA_COMMAND_PACKET
#endif
}