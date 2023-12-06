#include "D3D12Core.h"

static UInt32 FrameCount = 3;

namespace StravaEngine::Graphics::D3D12
{
std::unique_ptr<D3D12Core> D3D12Core::s_instance = nullptr;

bool D3D12Core::Initialize(const RendererSpec& spec)
{
	HRESULT hr = {};

	UInt32 dxgiFactoryFlags = 0;

#if STRAVA_DEBUG
	{
		ID3D12Debug* debugController = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		if (SUCCEEDED(hr))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
		else
		{
//			STRAVA_LOG_PRINT_WARNING("Failed to init with debug layer enabled.");
		}
		if (debugController)
		{
			debugController->Release();
			debugController = nullptr;
		}
	}
#endif

	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory7));
	if (FAILED(hr))
	{
//		STRAVA_LOG_PRINT_WARNING("Failed to create IDXGIFactory7.");
		return false;
	}

	for (UInt32 adapterIndex = 0; m_dxgiFactory7->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_dxgiAdapter4)) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1 = {};
		m_dxgiAdapter4->GetDesc1(&dxgiAdapterDesc1);

		if (dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			m_dxgiAdapter4->Release();
			m_dxgiAdapter4 = nullptr;
			continue;
		}

		hr = D3D12CreateDevice(m_dxgiAdapter4, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			break;
		}
		else
		{
			m_dxgiAdapter4->Release();
			m_dxgiAdapter4 = nullptr;
			continue;
		}
	}

	hr = D3D12CreateDevice(m_dxgiAdapter4, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_d3d12Device));
	if (FAILED(hr))
	{
//		STRAVA_LOG_PRINT_ERROR("Failed to create d3d12 device.");
		return false;
	}

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_d3d12CmmandQueue));
	if (FAILED(hr))
	{
		return false;
	}

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = 800;
	swapChainDesc.Height = 600;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* dxgiSwapChain1 = nullptr;
	hr = m_dxgiFactory7->CreateSwapChainForHwnd
	(
		m_d3d12CmmandQueue,        // Swap chain needs the queue so that it can force a flush on it.
		(HWND)spec.m_nativeWindowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		&dxgiSwapChain1
	);
	if (FAILED(hr))
	{
		return false;
	}

	// This sample does not support fullscreen transitions.
	hr = m_dxgiFactory7->MakeWindowAssociation((HWND)spec.m_nativeWindowHandle, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
	{
		return false;
	}

	hr = dxgiSwapChain1->QueryInterface(&m_dxgiSwapChain3);
	dxgiSwapChain1->Release();
	dxgiSwapChain1 = nullptr;
	if (FAILED(hr))
	{
		return false;
	}

	m_frameIndex = m_dxgiSwapChain3->GetCurrentBackBufferIndex();

#if 0
	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV and a command allocator for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);

			ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
		}
	}
#endif

	return true;
}

void D3D12Core::Terminate()
{

}
}