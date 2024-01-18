#pragma once

#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Graphics::D3D12
{
class D3D12DescriptorHeapBase
{
public:
	D3D12DescriptorHeapBase() {}
	virtual ~D3D12DescriptorHeapBase();

	virtual bool Initialize(ID3D12Device* d3d12Device, Size numDescriptors, Size numForBinding);
	virtual void Terminate();

	virtual D3D12_CPU_DESCRIPTOR_HANDLE* Push();
	virtual D3D12_CPU_DESCRIPTOR_HANDLE** PushMultiple(UInt32 count);

protected:
	Size m_numDescriptors;
	Size m_numForBinding;
	Size m_numForBindless;
	UInt64 m_cpuBegin;
	UInt64 m_gpuBegin;
	UInt64 m_cpuEnd;
	UInt64 m_gpuEnd;
	UInt64 m_cpuEndForBinding;
	UInt64 m_gpuEndForBinding;
	UInt64 m_cpuTail;
	UInt64 m_gpuTail;
	D3D12_DESCRIPTOR_HEAP_TYPE m_d3d12DescriptorHeapType;
	UInt32 m_d3d12DescriptorSize;
	ID3D12DescriptorHeap* m_d3d12DescriptorHeap = nullptr;
};

class D3D12DescriptorHeapCBVSRVUAV : public D3D12DescriptorHeapBase
{
public:
	D3D12DescriptorHeapCBVSRVUAV();
	virtual ~D3D12DescriptorHeapCBVSRVUAV() override;

//	bool Initialize(ID3D12Device* d3d12Device);
//	void Terminate();

private:
};

class D3D12DescriptorPool
{
private:
//	static constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType = d3d12DescriptorHeapType;
//	static constexpr Size k_numDescriptors = numDescriptors;

public:
	D3D12DescriptorPool(D3D12_DESCRIPTOR_HEAP_TYPE d3d12DescriptorHeapType, Size numDescriptors);
	~D3D12DescriptorPool() {}

	bool Initialize(ID3D12Device* d3d12Device);
	void Terminate();

	D3D12_CPU_DESCRIPTOR_HANDLE CreateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);

private:
	D3D12_DESCRIPTOR_HEAP_TYPE m_d3d12DescriptorHeapType;
	Size m_numDescriptors;
	Size m_d3d12DescriptorSize = 0;
	ID3D12DescriptorHeap* m_d3d12DescriptorHeap = nullptr;
	Size m_offset = 0;
};
}