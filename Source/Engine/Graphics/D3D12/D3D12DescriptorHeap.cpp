#include "D3D12DescriptorHeap.h"

#include <d3dx12.h>

#include <Engine/Core/ArrayList.h>

#include "D3D12Core.h"

namespace StravaEngine::Graphics::D3D12
{
D3D12DescriptorHeapBase::~D3D12DescriptorHeapBase()
{}

bool D3D12DescriptorHeapBase::Initialize(ID3D12Device* d3d12Device, Size numDescriptors, Size numForBinding)
{
	HRESULT hr = {};

	STRAVA_ASSERT(numForBinding <= numDescriptors);

	m_numDescriptors = numDescriptors;
	m_numForBinding = Core::Min(numDescriptors, numForBinding);
	m_numForBindless = m_numDescriptors - m_numForBinding;

	D3D12_DESCRIPTOR_HEAP_FLAGS d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (m_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ||
		m_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
	d3d12DescriptorHeapDesc.Type = m_d3d12DescriptorHeapType;
	d3d12DescriptorHeapDesc.NumDescriptors = static_cast<UINT>(numDescriptors);
	d3d12DescriptorHeapDesc.Flags = d3d12DescriptorHeapFlags;
	d3d12DescriptorHeapDesc.NodeMask = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&m_d3d12DescriptorHeap));
	if (FAILED(hr))
	{
		return false;
	}

	m_d3d12DescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(m_d3d12DescriptorHeapType);

	m_d3d12BaseCPUDescriptor = m_d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3d12BaseGPUDescriptor = m_d3d12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
#if 0
	m_cpuEnd = m_cpuBegin + m_d3d12DescriptorSize * m_numDescriptors;
	m_gpuEnd = m_gpuBegin + m_d3d12DescriptorSize * m_numDescriptors;
	m_cpuEndForBinding = m_cpuBegin + m_d3d12DescriptorSize * m_numForBinding;
	m_gpuEndForBinding = m_gpuBegin + m_d3d12DescriptorSize * m_numForBinding;
	m_cpuTail = m_cpuBegin;
	m_gpuTail = m_gpuBegin;
#endif
	m_offset = 0;

	return true;
}

void D3D12DescriptorHeapBase::Terminate()
{
	if (m_d3d12DescriptorHeap)
	{
		m_d3d12DescriptorHeap->Release();
		m_d3d12DescriptorHeap = nullptr;
	}
}

UInt32 D3D12DescriptorHeapBase::Allocate()
{
	auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();

	UInt32 currentOffset;

	// Ring-buffer 内に収まっている.
	if (m_offset + 1 < m_numForBinding)
	{
		currentOffset = m_offset;
		m_offset += 1;
	}
	// Ring-buffer の末端を超えた.
	else
	{
		currentOffset = 0;
		m_offset = 1;
	}

	return currentOffset;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeapBase::GetCPUDescriptorHandleAt(UInt32 index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_d3d12BaseCPUDescriptor, index, m_d3d12DescriptorSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeapBase::GetGPUDescriptorHandleAt(UInt32 index)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_d3d12BaseGPUDescriptor, index, m_d3d12DescriptorSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeapBase::Push(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();

	UInt64 currentOffset;

	// Ring-buffer 内に収まっている.
	if (m_offset + 1 < m_numForBinding)
	{
		currentOffset = m_offset;
		m_offset += 1;
	}
	// Ring-buffer の末端を超えた.
	else
	{
		currentOffset = 0;
		m_offset = 1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dest = { m_d3d12BaseCPUDescriptor.ptr + m_d3d12DescriptorSize * currentOffset };
	d3d12Device->CopyDescriptorsSimple(1, dest, handle, m_d3d12DescriptorHeapType);

	D3D12_GPU_DESCRIPTOR_HANDLE d3d12GPUDescriptorHandle = { m_d3d12BaseGPUDescriptor.ptr + m_d3d12DescriptorSize * currentOffset };
	return d3d12GPUDescriptorHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE** D3D12DescriptorHeapBase::PushMultiple(UInt32 count)
{
	UInt64 currentOffset;

	// Ring-buffer 内に収まっている.
	if (m_offset + count < m_numForBinding)
	{
		currentOffset = m_offset;
		m_offset += count;
	}
	// Ring-buffer の末端を超えた.
	else
	{
		currentOffset = 0;
		m_offset = count;
	}

//	D3D12_CPU_DESCRIPTOR_HANDLE handle;
//	handle.ptr = current;
	return nullptr;
}

#if 0
D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeapBase::GetD3D12GPUDescriptorHandle() const
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3d12GPUDescriptorHandle = { m_gpuBegin + m_d3d12DescriptorSize * m_offset };
	return d3d12GPUDescriptorHandle;
}
#endif

D3D12DescriptorHeapCBVSRVUAV::D3D12DescriptorHeapCBVSRVUAV()
	: D3D12DescriptorHeapBase()
{
	m_d3d12DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
}

D3D12DescriptorHeapCBVSRVUAV::~D3D12DescriptorHeapCBVSRVUAV()
{}

D3D12DescriptorHeapSampler::D3D12DescriptorHeapSampler()
	: D3D12DescriptorHeapBase()
{
	m_d3d12DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
}

D3D12DescriptorHeapSampler::~D3D12DescriptorHeapSampler()
{}

#if 0
D3D12DescriptorHeapRTV::D3D12DescriptorHeapRTV()
	: D3D12DescriptorHeapBase()
{
	m_d3d12DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
}
#endif
}