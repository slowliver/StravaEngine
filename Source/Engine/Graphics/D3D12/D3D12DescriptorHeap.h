#pragma once

#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/BitArray.h>

namespace StravaEngine::Graphics::D3D12
{
class D3D12DescriptorHeapBase
{
public:
	D3D12DescriptorHeapBase() {}
	virtual ~D3D12DescriptorHeapBase();

	virtual bool Initialize(ID3D12Device* d3d12Device, Size numDescriptors, Size numForBinding);
	virtual void Terminate();

	virtual void Allocate();
//	virtual void Free();

	virtual D3D12_GPU_DESCRIPTOR_HANDLE Push(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	virtual D3D12_CPU_DESCRIPTOR_HANDLE** PushMultiple(UInt32 count);

	ID3D12DescriptorHeap* GetD3D12DescriptorHeap() { return m_d3d12DescriptorHeap; }

#if 0
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetD3D12GPUDescriptorHandle() const;
#endif

protected:
	Size m_numDescriptors;
	Size m_numForBinding;
	Size m_numForBindless;
	UInt64 m_cpuBegin;
	UInt64 m_gpuBegin;
#if 0
	UInt64 m_cpuEnd;
	UInt64 m_gpuEnd;
	UInt64 m_cpuEndForBinding;
	UInt64 m_gpuEndForBinding;
#endif
	UInt64 m_offset;
	D3D12_DESCRIPTOR_HEAP_TYPE m_d3d12DescriptorHeapType;
	UInt32 m_d3d12DescriptorSize;
	ID3D12DescriptorHeap* m_d3d12DescriptorHeap = nullptr;
};

class D3D12DescriptorHeapCBVSRVUAV : public D3D12DescriptorHeapBase
{
public:
	D3D12DescriptorHeapCBVSRVUAV();
	virtual ~D3D12DescriptorHeapCBVSRVUAV() override;
};

class D3D12DescriptorHeapSampler : public D3D12DescriptorHeapBase
{
public:
	D3D12DescriptorHeapSampler();
	virtual ~D3D12DescriptorHeapSampler() override;
};

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType, UInt32 k_numDescriptors>
requires (k_numDescriptors > 0) && (k_numDescriptors % 8 == 0) && (k_numDescriptors < UINT32_MAX)
class D3D12CPUDescriptorHeap
{
private:
//	static constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType = d3d12DescriptorHeapType;
//	static constexpr UInt32 k_numDescriptors = numDescriptors;

public:
	D3D12CPUDescriptorHeap() = default;
	~D3D12CPUDescriptorHeap() {}

	bool Initialize(ID3D12Device* d3d12Device);
	void Terminate();



	D3D12_CPU_DESCRIPTOR_HANDLE CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
	D3D12_CPU_DESCRIPTOR_HANDLE CreateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);

private:
	UInt32 m_d3d12DescriptorHandleIncrementSize = 0;
	ID3D12DescriptorHeap* m_d3d12DescriptorHeap = nullptr;
	Size m_offset = 0;
	Core::BitArray<k_numDescriptors> m_usedMask = {};
};

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType, UInt32 k_numDescriptors>
requires (k_numDescriptors > 0) && (k_numDescriptors % 8 == 0) && (k_numDescriptors < UINT32_MAX)
bool D3D12CPUDescriptorHeap<k_d3d12DescriptorHeapType, k_numDescriptors>::Initialize(ID3D12Device* d3d12Device)
{
	HRESULT hr = {};

	m_d3d12DescriptorHandleIncrementSize = d3d12Device->GetDescriptorHandleIncrementSize(k_d3d12DescriptorHeapType);

	D3D12_DESCRIPTOR_HEAP_FLAGS d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if constexpr (k_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ||
		k_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		//		d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
	d3d12DescriptorHeapDesc.Type = k_d3d12DescriptorHeapType;
	d3d12DescriptorHeapDesc.NumDescriptors = k_numDescriptors;
	d3d12DescriptorHeapDesc.Flags = d3d12DescriptorHeapFlags;
	d3d12DescriptorHeapDesc.NodeMask = 0;

	hr = d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&m_d3d12DescriptorHeap));
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
}