#include "VertexBuffer.h"

#include <Engine/Graphics/Renderer.h>

#include "D3D12/D3D12VertexBuffer.h"

namespace StravaEngine::Graphics
{
VertexBuffer::VertexBuffer()
	: m_nativeVertexBuffer(new NativeVertexBuffer())
{}

VertexBuffer::~VertexBuffer()
{}

bool VertexBuffer::Create(const VertexBufferSpec& vertexBufferSpec)
{
	m_spec = vertexBufferSpec;
	m_data.AddRange(vertexBufferSpec.m_data, vertexBufferSpec.m_dataSize);
	auto& resourceCreationQueue = Renderer::GetInstance()->GetResourceCreationQueue();
	auto* spec = &m_spec;
	auto* nativeVertexBuffer = m_nativeVertexBuffer.get();
	resourceCreationQueue.Add
	(
		[spec, nativeVertexBuffer]()
		{
			nativeVertexBuffer->OnCreate(*spec);
		}
	);
	return true;
}

}