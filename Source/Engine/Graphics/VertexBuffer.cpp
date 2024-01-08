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

bool VertexBuffer::Create(const VertexBufferSpec& vertexBufferSpec, void* vertexData)
{
	m_spec = vertexBufferSpec;
//	m_data.AddRange(vertexBufferSpec.m_data, vertexBufferSpec.m_dataSize);
	auto& resourceCreationQueue = Renderer::GetInstance()->GetResourceCreationQueue();
	auto* spec = &m_spec;
	auto* data = new Byte[m_spec.m_size];
	Core::Memcpy(data, vertexData, m_spec.m_size);
	auto* nativeVertexBuffer = m_nativeVertexBuffer.get();
	resourceCreationQueue.Add
	(
		[spec, data, nativeVertexBuffer]()
		{
			nativeVertexBuffer->OnCreate(*spec, data);
			delete[] data;
		}
	);
	return true;
}

}