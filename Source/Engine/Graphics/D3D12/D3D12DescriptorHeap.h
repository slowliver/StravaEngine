#pragma once

#include <d3d12.h>
#include <bitset>
#include <d3dx12.h>

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

constexpr UInt32 k_invalidCPUDescriptorHeapIndex = 0xFFFFFFFF;

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType>
//requires (k_numDescriptors > 0) && (k_numDescriptors % 8 == 0) && (k_numDescriptors < UINT32_MAX)
class D3D12CPUDescriptorHeap
{
private:
//	static constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType = d3d12DescriptorHeapType;
//	static constexpr UInt32 k_numDescriptors = numDescriptors;

public:
	D3D12CPUDescriptorHeap() = default;
	~D3D12CPUDescriptorHeap() {}

	bool Initialize(ID3D12Device* d3d12Device, UInt32 numDescriptors);
	void Terminate();

	[[nodiscard]]
	D3D12_CPU_DESCRIPTOR_HANDLE Allocate();
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE index);

	Int64 GetIndexFrom(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

private:
	UInt32 m_d3d12DescriptorHandleIncrementSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12BaseDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
	ID3D12DescriptorHeap* m_d3d12DescriptorHeap = nullptr;
	UInt32 m_numDescriptors = 0;
	UInt32 m_marker = 0;
	Core::BitArray<1024> m_usedMask = {};
};

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType>
inline bool D3D12CPUDescriptorHeap<k_d3d12DescriptorHeapType>::Initialize(ID3D12Device* d3d12Device, UInt32 numDescriptors)
{
	HRESULT hr = {};

	if (numDescriptors > m_usedMask.GetCount())
	{
		STRAVA_ASSERT(0);
		return false;
	}

	m_d3d12DescriptorHandleIncrementSize = d3d12Device->GetDescriptorHandleIncrementSize(k_d3d12DescriptorHeapType);
	m_numDescriptors = numDescriptors;

	D3D12_DESCRIPTOR_HEAP_FLAGS d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if constexpr (k_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ||
		k_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		//		d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
	d3d12DescriptorHeapDesc.Type = k_d3d12DescriptorHeapType;
	d3d12DescriptorHeapDesc.NumDescriptors = numDescriptors;
	d3d12DescriptorHeapDesc.Flags = d3d12DescriptorHeapFlags;
	d3d12DescriptorHeapDesc.NodeMask = 0;

	hr = d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&m_d3d12DescriptorHeap));
	if (FAILED(hr))
	{
		return false;
	}

	m_d3d12BaseDescriptor = m_d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	return true;
}

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType>
inline void D3D12CPUDescriptorHeap<k_d3d12DescriptorHeapType>::Terminate()
{
	STRAVA_ASSERT(!m_usedMask.Any());
	if (m_d3d12DescriptorHeap)
	{
		m_d3d12DescriptorHeap->Release();
		m_d3d12DescriptorHeap = nullptr;
	}
}

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType>
inline D3D12_CPU_DESCRIPTOR_HANDLE D3D12CPUDescriptorHeap<k_d3d12DescriptorHeapType>::Allocate()
{
#if 0
	if (m_offset >= m_numDescriptors)
	{
		return d3d12CPUDescriptorHandle;
	}
#endif
	UInt32 loopCount = 0;
	while (m_usedMask[m_marker])
	{
		if (loopCount >= m_numDescriptors)
		{
			STRAVA_ASSERT(0);
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
		}
		m_marker = (m_marker + 1) % m_numDescriptors;
		++loopCount;
	}
	m_usedMask.SetAt(m_marker, true);
	m_marker = (m_marker + 1) % m_numDescriptors;
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_d3d12BaseDescriptor, m_marker, m_d3d12DescriptorHandleIncrementSize);
}

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType>
inline void D3D12CPUDescriptorHeap<k_d3d12DescriptorHeapType>::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	const UInt32 index = GetIndexFrom(handle);
	if (index < 0 || index >= m_numDescriptors)
	{
		STRAVA_ASSERT(0);
		return;
	}
	m_usedMask.SetAt(index, false);
}

template <D3D12_DESCRIPTOR_HEAP_TYPE k_d3d12DescriptorHeapType>
inline Int64 D3D12CPUDescriptorHeap<k_d3d12DescriptorHeapType>::GetIndexFrom(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{
	return (Int64(handle.ptr) - Int64(m_d3d12BaseDescriptor.ptr)) / m_d3d12DescriptorHandleIncrementSize;
}

class D3D12CPUDescriptorHeapCBVSRVUAV : public D3D12CPUDescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>
{};

class D3D12CPUDescriptorHeapSampler : public D3D12CPUDescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER>
{};

class D3D12CPUDescriptorHeapRTV : public D3D12CPUDescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>
{};
}