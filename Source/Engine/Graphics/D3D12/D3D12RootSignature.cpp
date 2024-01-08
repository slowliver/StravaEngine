#include "D3D12RootSignature.h"

#include <d3dx12.h>

#include <Engine/Core/ArrayList.h>

namespace StravaEngine::Graphics::D3D12
{
bool D3D12RootSignature::Initialize(ID3D12Device* d3d12Device)
{
	HRESULT hr = {};

	Size descriptorRangeCurrent = 0;
	auto descriptorRangePool = Core::Array<CD3DX12_DESCRIPTOR_RANGE, 32>();
	auto rootParameter = Core::ArrayList<CD3DX12_ROOT_PARAMETER>();

	auto addDescriptorHeap = [&descriptorRangeCurrent, &descriptorRangePool, &rootParameter](D3D12_DESCRIPTOR_RANGE_TYPE type, UINT numDescriptors, D3D12_SHADER_VISIBILITY shaderVisibility)
	{
		descriptorRangePool[descriptorRangeCurrent].Init(type, numDescriptors, 0);
		rootParameter.EmplaceBack(CD3DX12_ROOT_PARAMETER()).InitAsDescriptorTable
		(
			1,
			&descriptorRangePool[descriptorRangeCurrent],
			shaderVisibility
		);
		++descriptorRangeCurrent;
	};

	// Vertex Shader
	{
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, UINT_MAX, D3D12_SHADER_VISIBILITY_VERTEX);
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, D3D12_SHADER_VISIBILITY_VERTEX);
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT_MAX, D3D12_SHADER_VISIBILITY_VERTEX);
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2048, D3D12_SHADER_VISIBILITY_VERTEX);
	}

	// Pixel Shader
	{
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, UINT_MAX, D3D12_SHADER_VISIBILITY_PIXEL);
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, D3D12_SHADER_VISIBILITY_PIXEL);
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT_MAX, D3D12_SHADER_VISIBILITY_PIXEL);
		addDescriptorHeap(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2048, D3D12_SHADER_VISIBILITY_PIXEL);
	}

	auto rootSignatureDesc = CD3DX12_ROOT_SIGNATURE_DESC
	(
		static_cast<UINT>(rootParameter.GetCount()),
		rootParameter.GetData(),
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;

	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	if (FAILED(hr))
	{
		char* ptr = static_cast<char*>(error->GetBufferPointer());
		return false;
	}

	hr = d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_d3d12RootSignature));
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void D3D12RootSignature::Terminate()
{
	if (m_d3d12RootSignature)
	{
		m_d3d12RootSignature->Release();
		m_d3d12RootSignature = nullptr;
	}
}
}