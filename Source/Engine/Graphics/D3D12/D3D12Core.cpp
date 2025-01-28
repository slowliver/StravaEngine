#include "D3D12Core.h"

#include <d3dx12.h>

#include <Engine/Graphics/Type.h>

#include "D3D12CommandProcessor.h"
#include "D3D12RootSignature.h"
#include "D3D12PipelineState.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12RenderTexture.h"
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>

namespace StravaEngine::Graphics::D3D12
{
std::unique_ptr<D3D12Core> D3D12Core::s_instance = nullptr;

extern "C" Byte g_vertexShader[];
extern "C" Size g_vertexShaderSize;
extern "C" Byte g_pixelShader[];
extern "C" Size g_pixelShaderSize;

STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, VSMain);
STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, PSMain);
STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, VSMainFull);
STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, PSMainFull);

D3D12Core::D3D12Core()
	: m_commandProcessor(new D3D12CommandProcessor())
	, m_rootSignature(new D3D12RootSignature())
	, m_descriptorPoolCBVSRVUAV(new D3D12CPUDescriptorHeapCBVSRVUAV())
	, m_descriptorPoolSampler(new D3D12CPUDescriptorHeapSampler())
	, m_descriptorPoolRTV(new D3D12CPUDescriptorHeapRTV())
	, m_descriptorHeapCBVSRVUAV(new D3D12DescriptorHeapCBVSRVUAV())
	, m_descriptorHeapSampler(new D3D12DescriptorHeapSampler())
{}

D3D12Core::~D3D12Core()
{}

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
	swapChainDesc.BufferCount = k_frameCount;
	swapChainDesc.Width = spec.m_width;
	swapChainDesc.Height = spec.m_height;
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

	// Create descriptor heaps.
	{
		m_descriptorPoolCBVSRVUAV->Initialize(m_d3d12Device, 1024);
		m_descriptorPoolSampler->Initialize(m_d3d12Device, 1024);
		m_descriptorPoolRTV->Initialize(m_d3d12Device, 1024);
		m_descriptorHeapCBVSRVUAV->Initialize(m_d3d12Device, 100, 100);
		m_descriptorHeapSampler->Initialize(m_d3d12Device, 100, 100);
	}

	// Create frame resources.
	{
		// Create a RTV and a command allocator for each frame.
		for (UINT n = 0; n < k_frameCount; n++)
		{
			hr = m_dxgiSwapChain3->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			if (FAILED(hr))
			{
				return false;
			}
			if (m_d3d12RTVHandles[n] = m_descriptorPoolRTV->Allocate(); m_d3d12RTVHandles[n].ptr)
			{
				auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();
				d3d12Device->CreateRenderTargetView(m_renderTargets[n], nullptr, m_d3d12RTVHandles[n]);
			}
		}

		for (UINT n = 0; n < k_frameCount; n++)
		{
			hr = m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n]));
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	{
		bool success = m_rootSignature->Initialize(m_d3d12Device);
		if (!success)
		{
			return false;
		}
	}

	{
		auto* pipelineStateManager = D3D12PipelineStateManager::CreateInstance();
		if (!pipelineStateManager)
		{
			return false;
		}
		bool success = pipelineStateManager->Initialize();
		if (!success)
		{
			return false;
		}
	}

	{
		bool success = m_commandProcessor->Initialize(m_d3d12Device, m_commandAllocators[m_frameIndex]);
		if (!success)
		{
			return false;
		}
	}

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		hr = m_d3d12Device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if (FAILED(hr))
		{
			return false;
		}
		m_fenceValues[m_frameIndex]++;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			hr = (HRESULT_FROM_WIN32(GetLastError()));
			if (FAILED(hr))
			{
				return false;
			}
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForGPU();
	}

	return true;
}

void D3D12Core::OnPrepareCommandBuffer()
{
	HRESULT hr = {};

	// Record all the commands we need to render the scene into the command list.
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	hr = m_commandAllocators[m_frameIndex]->Reset();
	if (FAILED(hr))
	{
		return;
	}

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	{
		bool result = m_commandProcessor->OnPrepareCommandBuffer(m_commandAllocators[m_frameIndex]);
		if (!result)
		{
			return;
		}
	}
}

void D3D12Core::OnSubmitCommandBuffer(const GraphicsCommandBuffer& graphicsCommandBuffer, RenderTexture& finalOutputTexture)
{
	HRESULT hr = {};

	auto* d3d12GraphicsCommandList = m_commandProcessor->GetD3D12GraphicsCommandList();

	m_commandProcessor->OnSubmitCommandBuffer(graphicsCommandBuffer);

	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = finalOutputTexture.GetNativeRenderTexture<D3D12RenderTexture>()->GetD3D12CPUDescriptorHandle();
		d3d12GraphicsCommandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
	}

	ID3D12DescriptorHeap* d3d12DescriptorHeaps[] =
	{
		D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetD3D12DescriptorHeap(),
	};
	d3d12GraphicsCommandList->SetDescriptorHeaps(1, d3d12DescriptorHeaps);
	//	d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable()
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12GraphicsCommandList);

	{
		D3D12_RESOURCE_BARRIER barriers[] =
		{
			CD3DX12_RESOURCE_BARRIER::Transition
			(
				finalOutputTexture.GetNativeRenderTexture<D3D12RenderTexture>()->GetD3D12Resource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_COPY_SOURCE
			),
			CD3DX12_RESOURCE_BARRIER::Transition
			(
				m_renderTargets[m_frameIndex],
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_COPY_DEST
			)
		};
		d3d12GraphicsCommandList->ResourceBarrier(2, barriers);
	}

	{
		auto* dstResource = m_renderTargets[m_frameIndex];
		auto dstLocation = CD3DX12_TEXTURE_COPY_LOCATION(dstResource, 0);
		auto* srcResource = finalOutputTexture.GetNativeRenderTexture<D3D12RenderTexture>()->GetD3D12Resource();
		auto srcLocation = CD3DX12_TEXTURE_COPY_LOCATION(srcResource, 0);
		d3d12GraphicsCommandList->CopyTextureRegion
		(
			&dstLocation,
			0,
			0,
			0,
			&srcLocation,
			nullptr
		);
	}

	{
		D3D12_RESOURCE_BARRIER barriers[] =
		{
			CD3DX12_RESOURCE_BARRIER::Transition
			(
				finalOutputTexture.GetNativeRenderTexture<D3D12RenderTexture>()->GetD3D12Resource(),
				D3D12_RESOURCE_STATE_COPY_SOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			),
			CD3DX12_RESOURCE_BARRIER::Transition
			(
				m_renderTargets[m_frameIndex],
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PRESENT
			)
		};
		d3d12GraphicsCommandList->ResourceBarrier(2, barriers);
	}

#if 0
	// Indicate that the back buffer will be used as a render target.
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_renderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	d3d12GraphicsCommandList->OMSetRenderTargets(1, &m_d3d12RTVHandles[m_frameIndex], FALSE, nullptr);

	ID3D12DescriptorHeap* d3d12DescriptorHeaps[] =
	{
		D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetD3D12DescriptorHeap(),
	};
	d3d12GraphicsCommandList->SetDescriptorHeaps(1, d3d12DescriptorHeaps);
	//	d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable()
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12GraphicsCommandList);

	// Record commands.
//	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
//	d3d12GraphicsCommandList->ClearRenderTargetView(m_d3d12RTVHandles[m_frameIndex], clearColor, 0, nullptr);
//	d3d12GraphicsCommandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_renderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}
#endif

	hr = d3d12GraphicsCommandList->Close();
	if (FAILED(hr))
	{
		return;
	}

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { d3d12GraphicsCommandList };
	m_d3d12CmmandQueue->ExecuteCommandLists(static_cast<UINT>(Core::GetCount(ppCommandLists)), ppCommandLists);

	// Present the frame.
	hr = m_dxgiSwapChain3->Present(1, 0);
	if (FAILED(hr))
	{
		return;
	}

	MoveToNextFrame();
}

void D3D12Core::OnPresent(const GraphicsCommandBuffer& graphicsCommandBuffer)
{
	HRESULT hr = {};

	auto* d3d12GraphicsCommandList = m_commandProcessor->GetD3D12GraphicsCommandList();

	// Indicate that the back buffer will be used as a render target.
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_renderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	d3d12GraphicsCommandList->OMSetRenderTargets(1, &m_d3d12RTVHandles[m_frameIndex], FALSE, nullptr);


	// Indicate that the back buffer will now be used to present.
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_renderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	hr = d3d12GraphicsCommandList->Close();
	if (FAILED(hr))
	{
		return;
	}

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { d3d12GraphicsCommandList };
	m_d3d12CmmandQueue->ExecuteCommandLists(static_cast<UINT>(Core::GetCount(ppCommandLists)), ppCommandLists);

	// Present the frame.
	hr = m_dxgiSwapChain3->Present(1, 0);
	if (FAILED(hr))
	{
		return;
	}

	MoveToNextFrame();
}

void D3D12Core::WaitForGPU()
{
	// Schedule a Signal command in the queue.
	HRESULT hr = {};
	
	hr = m_d3d12CmmandQueue->Signal(m_fence, m_fenceValues[m_frameIndex]);
	if (FAILED(hr))
	{
		return;
	}

	// Wait until the fence has been processed.
	hr = m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent);
	if (FAILED(hr))
	{
		return;
	}

	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	m_fenceValues[m_frameIndex]++;
}

void D3D12Core::MoveToNextFrame()
{
	HRESULT hr = {};

	// Schedule a Signal command in the queue.
	const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
	hr = m_d3d12CmmandQueue->Signal(m_fence, currentFenceValue);

	// Update the frame index.
	m_frameIndex = m_dxgiSwapChain3->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
	{
		hr = m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent);
		if (FAILED(hr))
		{
			return;
		}
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

void D3D12Core::Terminate()
{
	if (m_commandProcessor)
	{
		m_commandProcessor->Terminate();
		m_commandProcessor.reset();
	}
	if (D3D12PipelineStateManager::GetInstance())
	{
		D3D12PipelineStateManager::GetInstance()->Terminate();
		D3D12PipelineStateManager::DestroyInstance();
	}
	if (m_rootSignature)
	{
		m_rootSignature->Terminate();
		m_rootSignature.reset();
	}
	if (m_descriptorHeapSampler)
	{
		m_descriptorHeapSampler->Terminate();
		m_descriptorHeapSampler.reset();
	}
	if (m_descriptorHeapCBVSRVUAV)
	{
		m_descriptorHeapCBVSRVUAV->Terminate();
		m_descriptorHeapCBVSRVUAV.reset();
	}
	m_descriptorPoolRTV->Terminate();
	m_descriptorPoolSampler->Terminate();
	m_descriptorPoolCBVSRVUAV->Terminate();
}
}