#pragma once

#include <d3d12.h>

namespace StravaEngine::Graphics::D3D12
{
class D3D12PipelineStateObject
{
public:
	explicit D3D12PipelineStateObject(ID3D12Device* d3d12Device);
	~D3D12PipelineStateObject();

private:
	ID3D12RootSignature* m_d3d12RootSignature = nullptr;
};
}