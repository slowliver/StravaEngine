#include "D3D12Core.h"


namespace StravaEngine::Graphics::D3D12
{
std::unique_ptr<D3D12Core> D3D12Core::s_instance = nullptr;

extern "C" char g_vertexShader[];
extern "C" char g_pixelShader[];

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

		m_d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
	}

	// Create the pipeline state, which includes compiling and loading shaders.
	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
	}

	// Create the command list.
	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(m_commandList->Close());

	// Create the vertex buffer.
	{
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
		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValues[m_frameIndex]++;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForGpu();
	}

	return true;
}

void D3D12Core::Terminate()
{

}
}