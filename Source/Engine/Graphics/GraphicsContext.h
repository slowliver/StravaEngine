#include <Engine/Core/CoreMinimal.h>
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
#define STRAVA_COMMAND_PACKET(p) struct CommandPacket##p final : public CommandPacket<CommandPacketType::p>

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
};

//static_assert(Core::ToUnderlying(CommandPacketType::Count) == Core::GetCount(k_commandPacketNames));

template <CommandPacketType commandPacketType>
struct CommandPacket
{
	static constexpr CommandPacketType k_commandPacketType = commandPacketType;
	CommandPacketType m_commandPacketType = commandPacketType;
//	const char8_t ToString() const { k_commandPacketNames[ToUnderlying(k_commandPacketType)]; }
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

#if 0
STRAVA_COMMAND_PACKET(SetVertexBuffers)
{
	Kernel::Size m_startSlot = 0;
	Kernel::Size m_numBuffers = 0;
	NativeResouce* m_buffers = nullptr;
};
#endif

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

class CommandBufferBase
{

};

class GraphicsCommandBuffer final : public CommandBufferBase
{
public:
	GraphicsCommandBuffer() {}
	~GraphicsCommandBuffer() {}

	bool Initialize();
	void Terminate();

	void Reset();

//	void ClearRenderTarget(NativeResouce* const renderTarget, const Kernel::Color clearColor);

	// Input Assembler
//	void SetPrimitiveTopology(const PrimitiveTopology primitiveTopology);
//	void SetVertexBuffers(const PrimitiveTopology primitiveTopology);

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