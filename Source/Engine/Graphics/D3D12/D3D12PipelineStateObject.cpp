#include "D3D12PipelineStateObject.h"

#include <d3dx12.h>

#include <Engine/Core/ArrayList.h>

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
}