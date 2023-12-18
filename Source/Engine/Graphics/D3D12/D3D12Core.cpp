#include "D3D12Core.h"

#include <d3dx12.h>

namespace StravaEngine::Graphics::D3D12
{
std::unique_ptr<D3D12Core> D3D12Core::s_instance = nullptr;

extern "C" Byte g_vertexShader[];
extern "C" Size g_vertexShaderSize;
extern "C" Byte g_pixelShader[];
extern "C" Size g_pixelShaderSize;

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

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = k_frameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_d3d12RTVHeap));
		if (FAILED(hr))
		{
			return false;
		}
		m_d3d12RTVDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = { m_d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart() };

		// Create a RTV and a command allocator for each frame.
		for (UINT n = 0; n < k_frameCount; n++)
		{
			hr = m_dxgiSwapChain3->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			if (FAILED(hr))
			{
				return false;
			}
			m_d3d12Device->CreateRenderTargetView(m_renderTargets[n], nullptr, rtvHandle);
			rtvHandle.ptr += m_d3d12RTVDescriptorSize;
			hr = m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n]));
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	// Create an empty root signature.
	{
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.NumParameters = 0;
		rootSignatureDesc.pParameters = nullptr;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* signature = nullptr;
		ID3DBlob* error = nullptr;

		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		if (FAILED(hr))
		{
			return false;
		}

		hr = m_d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
		if (FAILED(hr))
		{
			return false;
		}
	}

	// Create the pipeline state, which includes compiling and loading shaders.
	{
		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = m_rootSignature;
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
		hr = m_d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
		if (FAILED(hr))
		{
			return false;
		}
	}

	// Create the command list.
	hr = m_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex], m_pipelineState, IID_PPV_ARGS(&m_commandList));
	if (FAILED(hr))
	{
		return false;
	}

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	hr = m_commandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	struct Vertex
	{
		float position[3];
		float color[4];
	};

	// Create the vertex buffer.
	{
		float m_aspectRatio = 16.0f / 9.0f;
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		D3D12_HEAP_PROPERTIES d3d12HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC vertexBufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		hr = m_d3d12Device->CreateCommittedResource
		(
			&d3d12HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&vertexBufferResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)
		);
		if (FAILED(hr))
		{
			return false;
		}

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		hr = m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (FAILED(hr))
		{
			return false;
		}
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
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

void D3D12Core::OnUpdate()
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
	hr = m_commandList->Reset(m_commandAllocators[m_frameIndex], m_pipelineState);
	if (FAILED(hr))
	{
		return;
	}

	// Set necessary state.
	m_commandList->SetGraphicsRootSignature(m_rootSignature);

	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, 800.0f, 600.0f);
	D3D12_RECT scissorRect = CD3DX12_RECT(0, 0, 800, 600);
	m_commandList->RSSetViewports(1, &viewport);
	m_commandList->RSSetScissorRects(1, &scissorRect);

	// Indicate that the back buffer will be used as a render target.
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_renderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		m_commandList->ResourceBarrier(1, &barrier);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_d3d12RTVDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_renderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		m_commandList->ResourceBarrier(1, &barrier);
	}

	hr = m_commandList->Close();
	if (FAILED(hr))
	{
		return;
	}

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList };
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

}
}