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

	m_cpuBegin = m_d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
	m_gpuBegin = m_d3d12DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr;
	m_cpuEnd = m_cpuBegin + m_d3d12DescriptorSize * m_numDescriptors;
	m_gpuEnd = m_gpuBegin + m_d3d12DescriptorSize * m_numDescriptors;
	m_cpuEndForBinding = m_cpuBegin + m_d3d12DescriptorSize * m_numForBinding;
	m_gpuEndForBinding = m_gpuBegin + m_d3d12DescriptorSize * m_numForBinding;
	m_cpuTail = m_cpuBegin;
	m_gpuTail = m_gpuBegin;

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

D3D12_CPU_DESCRIPTOR_HANDLE* D3D12DescriptorHeapBase::Push()
{
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE** D3D12DescriptorHeapBase::PushMultiple(UInt32 count)
{
	UInt64 current;

	// Ring-buffer “à‚ÉŽû‚Ü‚Á‚Ä‚¢‚é.
	if (m_cpuTail + m_d3d12DescriptorSize * count < m_numForBinding)
	{
		current = m_cpuTail;
		m_cpuTail += m_d3d12DescriptorSize * count;
	}
	// Ring-buffer ‚Ì––’[‚ð’´‚¦‚½.
	else
	{
		current = m_cpuBegin;
		m_cpuBegin = m_cpuBegin + m_d3d12DescriptorSize * count;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = current;
	return nullptr;
}

D3D12DescriptorHeapCBVSRVUAV::D3D12DescriptorHeapCBVSRVUAV()
	: D3D12DescriptorHeapBase()
{
	m_d3d12DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
}

D3D12DescriptorHeapCBVSRVUAV::~D3D12DescriptorHeapCBVSRVUAV()
{}

#if 0
D3D12DescriptorHeapRTV::D3D12DescriptorHeapRTV()
	: D3D12DescriptorHeapBase()
{
	m_d3d12DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
}
#endif

D3D12DescriptorPool::D3D12DescriptorPool(D3D12_DESCRIPTOR_HEAP_TYPE d3d12DescriptorHeapType, Size numDescriptors)
	: m_d3d12DescriptorHeapType(d3d12DescriptorHeapType)
	, m_numDescriptors(numDescriptors)
{}

bool D3D12DescriptorPool::Initialize(ID3D12Device* d3d12Device)
{
	HRESULT hr = {};

	m_d3d12DescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(m_d3d12DescriptorHeapType);

	D3D12_DESCRIPTOR_HEAP_FLAGS d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (m_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ||
		m_d3d12DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		d3d12DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
	d3d12DescriptorHeapDesc.Type = m_d3d12DescriptorHeapType;
	d3d12DescriptorHeapDesc.NumDescriptors = static_cast<UINT>(m_numDescriptors);
	d3d12DescriptorHeapDesc.Flags = d3d12DescriptorHeapFlags;
	d3d12DescriptorHeapDesc.NodeMask = 0;

	hr = d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&m_d3d12DescriptorHeap));
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorPool::CreateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3d12CPUDescriptorHandle = {};
	if (m_d3d12DescriptorHeapType != D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	{
		return d3d12CPUDescriptorHandle;
	}
	if (m_offset >= m_numDescriptors)
	{
		return d3d12CPUDescriptorHandle;
	}
	d3d12CPUDescriptorHandle.ptr = m_d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_d3d12DescriptorSize * m_offset;
	auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();
	d3d12Device->CreateRenderTargetView(resource, desc, d3d12CPUDescriptorHandle);
	m_offset++;
	return d3d12CPUDescriptorHandle;
}

void D3D12DescriptorPool::Terminate()
{
	if (m_d3d12DescriptorHeap)
	{
		m_d3d12DescriptorHeap->Release();
		m_d3d12DescriptorHeap = nullptr;
	}
}
}