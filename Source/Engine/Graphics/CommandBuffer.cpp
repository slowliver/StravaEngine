#include "CommandBuffer.h"

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

void CommandBufferBase::SetNativeCommand(std::function<void(void)> func)
{
	auto& packet = Push<CommandPacketSetNativeCommand>();
	packet.m_function = func;
}

#if 0
void CommandBufferBase::End(Size commandSize, Size additionalSize)
{}
#endif

// Input Assembler
void GraphicsCommandBuffer::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
	auto& packet = Push<CommandPacketSetPrimitiveTopology>();
	packet.m_primitiveTopology = primitiveTopology;
}

void GraphicsCommandBuffer::SetVertexBuffers(UInt8 startSlot, Core::ArrayProxy<VertexBuffer*> buffers, UInt8 offset)
{
	auto& packet = Push<CommandPacketSetVertexBuffers>();
	packet.m_startSlot = startSlot;
	packet.m_buffers = buffers;
	packet.m_offset = offset;
}

// Rasterizer
void GraphicsCommandBuffer::SetViewport(const Viewport& viewport)
{
	auto& packet = Push<CommandPacketSetViewport>();
	packet.m_viewport = viewport;
}

void GraphicsCommandBuffer::SetScissor(const Core::Int32Rect& scissor)
{
	auto& packet = Push<CommandPacketSetScissor>();
	packet.m_scissor = scissor;
}

void GraphicsCommandBuffer::Draw(UInt32 vertexCountPerInstance, UInt32 instanceCount, UInt32 startVertexLocation, UInt32 startInstanceLocation)
{
	auto& packet = Push<CommandPacketDraw>();
	packet.m_vertexCountPerInstance = vertexCountPerInstance;
	packet.m_instanceCount = instanceCount;
	packet.m_startVertexLocation = startVertexLocation;
	packet.m_startInstanceLocation = startInstanceLocation;
}
}