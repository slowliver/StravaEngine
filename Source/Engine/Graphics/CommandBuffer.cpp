#include <Engine/Graphics/CommandBuffer.h>

#include <Engine/Core/Allocator.h>

namespace StravaEngine::Graphics
{
bool GraphicsCommandBuffer::Initialize()
{
	return true;
}

void GraphicsCommandBuffer::Terminate()
{}

CommandBufferBase::CommandBufferBase(Size size)
{
	auto allocator = Core::Allocator<Byte>();
	m_begin = allocator.Allocate(size);
	m_end = m_begin + size;
	m_back = m_begin;
}

CommandBufferBase::~CommandBufferBase()
{
	auto allocator = Core::Allocator<Byte>();
	allocator.Deallocate(m_begin);
}

void CommandBufferBase::Reset()
{
	m_back = m_begin;
}

Byte* CommandBufferBase::Push(Size commandSize, Size additionalSize)
{
	const auto diff = reinterpret_cast<uintptr_t>(m_end) - reinterpret_cast<uintptr_t>(m_back);
	if (diff < commandSize + additionalSize)
	{
		STRAVA_ASSERT(!"Can't allocate command!")
			return nullptr;
	}
	auto* previousPosition = m_back;
	m_back += commandSize + additionalSize;
	return previousPosition;
}

#define STRAVA_COMMAND_BUFFER_FUNC(type, p)				\
static_assert(std::is_trivial_v<CommandPacket##p>);		\
void type##CommandBuffer::p
#define STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(p)			\
STRAVA_COMMAND_BUFFER_FUNC(Graphics, p)

void CommandBufferBase::SetNativeCommand(void (*function)(void*, void*), void* arguments)
{
	auto& packet = Push<CommandPacketSetNativeCommand>();
	packet.m_arguments = arguments;
	packet.m_function = function;
}

#if 0
void CommandBufferBase::End(Size commandSize, Size additionalSize)
{}
#endif

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(ClearRenderTarget)(RenderTexture* renderTarget, float* color)
{
	auto& packet = Push<CommandPacketClearRenderTarget>();
	packet.m_renderTarget = renderTarget;
	packet.m_color[0] = color[0];
	packet.m_color[1] = color[1];
	packet.m_color[2] = color[2];
	packet.m_color[3] = color[3];
}

// Render Pass
STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(BeginPass)()
{
	auto& packet = Push<CommandPacketBeginPass>();
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(EndPass)()
{
	auto& packet = Push<CommandPacketEndPass>();
}

// Input Assembler
STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetPrimitiveTopology)(PrimitiveTopology primitiveTopology)
{
	auto& packet = Push<CommandPacketSetPrimitiveTopology>();
	packet.m_primitiveTopology = primitiveTopology;
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetVertexBuffers)(UInt8 startSlot, Core::ArrayProxy<VertexBuffer*> buffers)
{
	auto& packet = Push<CommandPacketSetVertexBuffers>(sizeof(VertexBuffer*) * buffers.GetCount());
	packet.m_startSlot = startSlot;
	packet.m_numBuffers = static_cast<UInt32>(buffers.GetCount());
	auto** vertexBuffers = reinterpret_cast<VertexBuffer**>(reinterpret_cast<Byte*>(&packet) + sizeof(CommandPacketSetVertexBuffers));
	for (Size i = 0; i < packet.m_numBuffers; ++i)
	{
		vertexBuffers[i] = buffers[i];
	}
}

// Vertex Shader
STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetVertexShader)(Shader* vertexShader)
{
	auto& packet = Push<CommandPacketSetVertexShader>();
	packet.m_vertexShader = vertexShader;
}

// Pixel Shader
STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetPixelShader)(Shader* pixelShader)
{
	auto& packet = Push<CommandPacketSetPixelShader>();
	packet.m_pixelShader = pixelShader;
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetPSShaderResources)(Texture* texture)
{
	auto& packet = Push<CommandPacketSetPSShaderResources>();
	packet.m_texture = texture;
}

// Rasterizer
STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetViewport)(const Viewport& viewport)
{
	auto& packet = Push<CommandPacketSetViewport>();
	packet.m_viewport = viewport;
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetScissor)(const Core::Int32Rect& scissor)
{
	auto& packet = Push<CommandPacketSetScissor>();
	packet.m_scissor = scissor;
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(SetRenderTargets)(Core::ArrayProxy<RenderTexture*> targets)
{
	const auto numRenderTargets = Core::Min(static_cast<UInt32>(targets.GetCount()), 8u);
	auto& packet = Push<CommandPacketSetRenderTargets>();
	packet.m_numRenderTargets = numRenderTargets;
	for (Size i = 0; i < packet.m_numRenderTargets; ++i)
	{
		packet.m_renderTargets[i] = targets[i];
	}
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(Draw)(UInt32 vertexCount)
{
	Draw(vertexCount, 1, 0, 0);
}

STRAVA_GRAPHICS_COMMAND_BUFFER_FUNC(Draw)(UInt32 vertexCountPerInstance, UInt32 instanceCount, UInt32 startVertexLocation, UInt32 startInstanceLocation)
{
	auto& packet = Push<CommandPacketDraw>();
	packet.m_vertexCountPerInstance = vertexCountPerInstance;
	packet.m_instanceCount = instanceCount;
	packet.m_startVertexLocation = startVertexLocation;
	packet.m_startInstanceLocation = startInstanceLocation;
}
}