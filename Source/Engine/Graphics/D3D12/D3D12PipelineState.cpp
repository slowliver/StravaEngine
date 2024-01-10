#include "D3D12PipelineState.h"

#include <Engine/Core/ArrayList.h>

#include "D3D12Core.h"

namespace StravaEngine::Graphics::D3D12
{
D3D12PipelineStateObject::D3D12PipelineStateObject(ID3D12Device* d3d12Device)
{
	HRESULT hr = {};

#if 0
	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature->GetD3D12RootSignature();
	psoDesc.VS = { g_vertexShader, g_vertexShaderSize };
	psoDesc.PS = { g_pixelShader, g_pixelShaderSize };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	hr = d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
#endif
}

D3D12PipelineStateObject::~D3D12PipelineStateObject()
{}

Size D3D12PipelineStateHash::operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& key) const
{
	return 0;
}

bool D3D12PipelineStateKeyEq::operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& x, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& y) const
{
	return (std::memcmp(&x, &y, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC)) == 0);
}

std::unique_ptr<D3D12PipelineStateManager, D3D12PipelineStateManager::Deleter> D3D12PipelineStateManager::s_instance = nullptr;

D3D12PipelineStateManager* D3D12PipelineStateManager::CreateInstance()
{
	s_instance.reset(new D3D12PipelineStateManager());
	return GetInstance();
}

D3D12PipelineStateManager* D3D12PipelineStateManager::GetInstance()
{
	return s_instance.get();
}

void D3D12PipelineStateManager::DestroyInstance()
{
	s_instance.reset(nullptr);
}

bool D3D12PipelineStateManager::Initialize()
{
	return true;
}

void D3D12PipelineStateManager::Terminate()
{
	for (auto& pair : m_pipelineStatePool)
	{
		pair.second->Release();
	}
	m_pipelineStatePool.clear();
}

ID3D12PipelineState* D3D12PipelineStateManager::FindOrCreate(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
{
	auto itr = m_pipelineStatePool.find(desc);

	ID3D12PipelineState* pso = nullptr;

	// Create
	if (itr == m_pipelineStatePool.cend())
	{
		auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();
		HRESULT hr = d3d12Device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			return nullptr;
		}
		m_pipelineStatePool.insert(std::pair<D3D12_GRAPHICS_PIPELINE_STATE_DESC, ID3D12PipelineState*>(desc, pso));
	}

	// Find
	else
	{
		pso = itr->second;
	}

	return pso;
}
}