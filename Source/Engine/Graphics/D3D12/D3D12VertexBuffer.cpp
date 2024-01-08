#include "D3D12VertexBuffer.h"

#include <d3dx12.h>

#include "D3D12Core.h"

namespace StravaEngine::Graphics::D3D12
{
D3D12VertexBuffer::~D3D12VertexBuffer()
{
	if (m_resource)
	{
		m_resource->Release();
		m_resource = nullptr;
	}
}

bool D3D12VertexBuffer::OnCreate(const VertexBufferSpec& vertexBufferSpec, void* vertexData)
{
	HRESULT hr = {};

	auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();

	D3D12_HEAP_PROPERTIES d3d12HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC vertexBufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSpec.m_size);
	hr = d3d12Device->CreateCommittedResource
	(
		&d3d12HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_resource)
	);
	STRAVA_ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		return false;
	}

	// Copy the triangle data to the vertex buffer.
	Byte* vertexBufferData;
	D3D12_RANGE readRange = CD3DX12_RANGE(0, 0);        // We do not intend to read from this resource on the CPU.
	hr = m_resource->Map(0, &readRange, reinterpret_cast<void**>(&vertexBufferData));
	if (FAILED(hr))
	{
		return false;
	}
	Core::Memcpy(vertexBufferData, vertexData, vertexBufferSpec.m_size);
	m_resource->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_view.BufferLocation = m_resource->GetGPUVirtualAddress();
	m_view.SizeInBytes = static_cast<UINT>(vertexBufferSpec.m_size);
	m_view.StrideInBytes = static_cast<UINT>(vertexBufferSpec.m_stride);

	return true;
}
}