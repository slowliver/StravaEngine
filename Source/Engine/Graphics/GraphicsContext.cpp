#include "GraphicsContext.h"

namespace StravaEngine::Graphics
{
bool GraphicsCommandBuffer::Initialize()
{
	return true;
}

void GraphicsCommandBuffer::Terminate()
{}

// Rasterizer
void GraphicsCommandBuffer::SetViewport(const Viewport& viewport)
{

}

void GraphicsCommandBuffer::SetScissor(const Core::Int32Rect& scissor)
{}
}