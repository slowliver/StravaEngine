#include "GraphicsContext.h"

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
}

CommandBufferBase::~CommandBufferBase()
{
	auto allocator = Core::Allocator<Byte>();
	allocator.Deallocate(m_begin);
}

Byte* CommandBufferBase::Push(Size commandSize, Size additionalSize)
{
	const auto diff = reinterpret_cast<uintptr_t>(m_end) - reinterpret_cast<uintptr_t>(m_current);
	if (diff < commandSize + additionalSize)
	{
		STRAVA_ASSERT(!"Can't allocate command!")
		return nullptr;
	}
	auto* previousPosition = m_current;
	previousPosition += commandSize + additionalSize;
	return previousPosition;
}

#if 0
void CommandBufferBase::End(Size commandSize, Size additionalSize)
{}
#endif

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
}