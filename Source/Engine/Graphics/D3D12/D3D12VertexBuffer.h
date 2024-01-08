#pragma once

#include <memory>
#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>
#include "./../VertexBuffer.h"

namespace StravaEngine::Graphics
{
namespace D3D12
{
class D3D12VertexBuffer : public NativeVertexBufferBase
{
public:
	D3D12VertexBuffer() {}
	virtual ~D3D12VertexBuffer() override;
	virtual bool OnCreate(const VertexBufferSpec& vertexBufferSpec, void* vertexData) override;

	ID3D12Resource* GetD3D12Resource() { return m_resource; }
	const D3D12_VERTEX_BUFFER_VIEW& GetD3D12VertexBufferView() const { return m_view; }

private:
	ID3D12Resource* m_resource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_view = {};
};
}
using NativeVertexBuffer = typename D3D12::D3D12VertexBuffer;
}