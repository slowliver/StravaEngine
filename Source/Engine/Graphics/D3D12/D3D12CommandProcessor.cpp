#include "D3D12CommandProcessor.h"

#include <d3dx12.h>

#include "D3D12Core.h"
#include "D3D12TypeTranslator.h"
#include "D3D12VertexBuffer.h"
#include "D3D12RenderTexture.h"
#include "D3D12RootSignature.h"
#include "D3D12Shader.h"
#include "D3D12PipelineState.h"

extern "C" Byte g_vertexShader[];
extern "C" Size g_vertexShaderSize;
extern "C" Byte g_pixelShader[];
extern "C" Size g_pixelShaderSize;

namespace StravaEngine::Graphics::D3D12
{
struct D3D12CommandProcessor::StateCache final
{
	union DirtyFlag
	{
		UInt64 m_raw;
		struct
		{
			// Root Signature
			bool m_rootSignature		: 1;

			// Input Assembler
			bool m_primitiveTopology	: 1;
			bool m_vertexBuffers		: 1;

			// Vertex Shader
			bool m_vertexShader			: 1;

			// Pixel Shader
			bool m_pixelShader			: 1;

			// Rasterizer
			bool m_viewport				: 1;
			bool m_scissor				: 1;

			// Output Merger
			bool m_renderTargets		: 1;
		};
	};
	DirtyFlag m_dirtyFlags;

	// Render Pass
	bool m_onPass;

	// Root Signature
	bool m_firstRootSignatureSet;

	// Input Assembler
	PrimitiveTopology m_primitiveTopology;

	// Vertex Shader
	Shader* m_vertexShader;

	// Pixel Shader
	Shader* m_pixelShader;

	// Rasterizer
	Viewport m_viewport;
	Core::Int32Rect m_scissor;

	// Output Merger
	UInt8 m_numRenderTargets;
	RenderTexture* m_renderTargets[8];

	void Reset()
	{
		m_dirtyFlags.m_raw = 0xFFFFFFFFFFFFFFFF;
		m_onPass = false;
		m_firstRootSignatureSet = true;
		m_primitiveTopology = PrimitiveTopology::Unknown;
		m_vertexShader = nullptr;
		m_pixelShader = nullptr;
		m_viewport = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
		m_scissor = { INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX };
		m_numRenderTargets = 0;
		std::memset(m_renderTargets, 0, sizeof(m_renderTargets));
	}
};

bool D3D12CommandProcessor::Initialize(ID3D12Device* d3d12Device, ID3D12CommandAllocator* d3d12CommandAllocator)
{
	HRESULT hr = {};

	hr = d3d12Device->CreateCommandList
	(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		d3d12CommandAllocator,
		nullptr,
		IID_PPV_ARGS(&m_d3d12GraphicsCommandList)
	);
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_d3d12GraphicsCommandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	m_stateCache = new StateCache();

	return true;
}

void D3D12CommandProcessor::Terminate()
{
	if (m_d3d12GraphicsCommandList)
	{
		m_d3d12GraphicsCommandList->Release();
		m_d3d12GraphicsCommandList = nullptr;
	}

	delete m_stateCache;
	m_stateCache = nullptr;
}

#if !defined(STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK)
#define STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(commandPacketType, commandProcessor, packet)	\
static void D3D12CommandProcessor_on##commandPacketType(D3D12CommandProcessor& commandProcessor, const CommandPacket##commandPacketType* packet)
#endif

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(ClearRenderTarget, commandProcessor, packet)
{
	auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();
	auto* renderTexture = packet->m_renderTarget->GetNativeRenderTexture<D3D12RenderTexture>();
	if (!renderTexture)
	{
		return;
	}
	const auto d3d12CPUDescriptorHandle = renderTexture->GetD3D12CPUDescriptorHandle();
	if (d3d12CPUDescriptorHandle.ptr)
	{
		d3d12GraphicsCommandList->ClearRenderTargetView
		(
			d3d12CPUDescriptorHandle,
			packet->m_color,
			0,
			nullptr
		);
	}
}

// Render Pass

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(BeginPass, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (stateCache->m_onPass)
	{
		STRAVA_ASSERT(!stateCache->m_onPass);
		return;
	}
	stateCache->m_onPass = true;
	if (stateCache->m_firstRootSignatureSet)
	{
		stateCache->m_firstRootSignatureSet = false;
		stateCache->m_dirtyFlags.m_rootSignature = true;
	}
	if (stateCache->m_dirtyFlags.m_renderTargets)
	{
		auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[8];
		for (Size i = 0; i < stateCache->m_numRenderTargets; ++i)
		{
			auto* renderTexture = stateCache->m_renderTargets[i]->GetNativeRenderTexture<D3D12RenderTexture>();
			renderTargetHandles[i] = renderTexture->GetD3D12CPUDescriptorHandle();
		}
		// Depth Stencil ‚Í‚Ð‚Æ‚Ü‚¸–³Ž‹.
		d3d12GraphicsCommandList->OMSetRenderTargets(stateCache->m_numRenderTargets, renderTargetHandles, FALSE, nullptr);
		stateCache->m_dirtyFlags.m_renderTargets = false;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(EndPass, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (!stateCache->m_onPass)
	{
		STRAVA_ASSERT(stateCache->m_onPass);
		return;
	}
	stateCache->m_onPass = false;
}

// Begin Input Assembler

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetPrimitiveTopology, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_primitiveTopology != stateCache->m_primitiveTopology)
	{
		stateCache->m_primitiveTopology = packet->m_primitiveTopology;
		stateCache->m_dirtyFlags.m_primitiveTopology = true;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetVertexBuffers, commandProcessor, packet)
{
	auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();
	D3D12_VERTEX_BUFFER_VIEW d3d12VertexBufferViews[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	const auto numViews = Core::Min(Core::GetCount(d3d12VertexBufferViews), static_cast<Size>(packet->m_numBuffers));
	const auto** vertexBuffers = reinterpret_cast<const VertexBuffer**>(reinterpret_cast<const std::intptr_t>(packet) + sizeof(CommandPacketSetVertexBuffers));
	for (Size i = 0; i < numViews; ++i)
	{
		auto* vertexBuffer = vertexBuffers[i];
		if (!vertexBuffer)
		{
			continue;
		}
		auto* nativeVertexBuffer = static_cast<const D3D12VertexBuffer*>(vertexBuffer->GetNativeVertexBuffer());
		if (!nativeVertexBuffer)
		{
			continue;
		}
		d3d12VertexBufferViews[i] = nativeVertexBuffer->GetD3D12VertexBufferView();
	}
	d3d12GraphicsCommandList->IASetVertexBuffers(packet->m_startSlot, static_cast<UINT>(numViews), d3d12VertexBufferViews);
}

// End Input Assembler

// Begin Vertex Shader

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetVertexShader, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_vertexShader != stateCache->m_vertexShader)
	{
		stateCache->m_vertexShader = packet->m_vertexShader;
		stateCache->m_dirtyFlags.m_vertexShader = true;
	}
}

// End Vertex Shader

// Begin Pixel Shader

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetPixelShader, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_pixelShader != stateCache->m_pixelShader)
	{
		stateCache->m_pixelShader = packet->m_pixelShader;
		stateCache->m_dirtyFlags.m_pixelShader = true;
	}
}

// End Pixel Shader

// Begin Rasterizer

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetViewport, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_viewport != stateCache->m_viewport)
	{
		stateCache->m_viewport = packet->m_viewport;
		stateCache->m_dirtyFlags.m_viewport = true;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetScissor, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_scissor != stateCache->m_scissor)
	{
		stateCache->m_scissor = packet->m_scissor;
		stateCache->m_dirtyFlags.m_scissor = true;
	}
}

// End Rasterizer

// Begin Output Merger

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetRenderTargets, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (stateCache->m_onPass)
	{
		STRAVA_ASSERT(!stateCache->m_onPass);
		return;
	}
	const auto numRenderTargets = Core::Min(8u, static_cast<UInt32>(packet->m_numRenderTargets));
	bool isAnyChanged = false;
	if (numRenderTargets != stateCache->m_numRenderTargets)
	{
		stateCache->m_numRenderTargets = numRenderTargets;
		isAnyChanged = true;
	}
	for (Size i = 0; i < numRenderTargets; ++i)
	{
		if (packet->m_renderTargets[i] != stateCache->m_renderTargets[i])
		{
			stateCache->m_renderTargets[i] = packet->m_renderTargets[i];
			isAnyChanged = true;
		}
	}
	if (isAnyChanged)
	{
		stateCache->m_dirtyFlags.m_renderTargets = true;
	}
}

// End Output Merger

// Begin Draw

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(Draw, commandProcessor, packet)
{
	auto* stateCache = commandProcessor.GetStateCache();
	auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12GraphicsPipelineStateDesc = {};
	d3d12GraphicsPipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	d3d12GraphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	d3d12GraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	d3d12GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3d12GraphicsPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3d12GraphicsPipelineStateDesc.SampleMask = UINT_MAX;
	d3d12GraphicsPipelineStateDesc.NumRenderTargets = 1;
	d3d12GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d12GraphicsPipelineStateDesc.SampleDesc.Count = 1;

	bool setPipelineState = false;

	// Root Signature
	if (stateCache->m_dirtyFlags.m_rootSignature)
	{
		auto* d3d12RootSignature = D3D12Core::s_instance->GetRootSignature()->GetD3D12RootSignature();
		d3d12GraphicsPipelineStateDesc.pRootSignature = d3d12RootSignature;
		d3d12GraphicsCommandList->SetGraphicsRootSignature(d3d12RootSignature);
		stateCache->m_dirtyFlags.m_rootSignature = false;
		setPipelineState = true;
	}

	// Vertex Shader
	if (stateCache->m_dirtyFlags.m_vertexShader)
	{
		auto* vertexShader = stateCache->m_vertexShader;
		if (vertexShader)
		{
			auto* nativeVertexShader = vertexShader->GetNativeShader<D3D12Shader>();
			if (nativeVertexShader)
			{
				d3d12GraphicsPipelineStateDesc.VS = nativeVertexShader->GetShaderByteCode();
			}
		}
		stateCache->m_dirtyFlags.m_vertexShader = false;
		setPipelineState = true;
	}

	// Pixel Shader
	if (stateCache->m_dirtyFlags.m_pixelShader)
	{
		auto* pixelShader = stateCache->m_pixelShader;
		if (pixelShader)
		{
			auto* nativePixelShader = pixelShader->GetNativeShader<D3D12Shader>();
			if (nativePixelShader)
			{
				d3d12GraphicsPipelineStateDesc.PS = nativePixelShader->GetShaderByteCode();
			}
		}
		stateCache->m_dirtyFlags.m_pixelShader = false;
		setPipelineState = true;
	}
	
	// Input Assembler
	if (stateCache->m_dirtyFlags.m_primitiveTopology)
	{
		d3d12GraphicsPipelineStateDesc.PrimitiveTopologyType = Translator::ToD3D12::ToPrimitiveTopologyType(stateCache->m_primitiveTopology);
		D3D12_PRIMITIVE_TOPOLOGY d3d12PrimitiveTopology = Translator::ToD3D12::ToPrimitiveTopology(stateCache->m_primitiveTopology);
		d3d12GraphicsCommandList->IASetPrimitiveTopology(d3d12PrimitiveTopology);
		stateCache->m_dirtyFlags.m_primitiveTopology = false;
		setPipelineState = true;
	}

	// Rasterizer
	if (stateCache->m_dirtyFlags.m_viewport)
	{
		D3D12_VIEWPORT viewport = Translator::ToD3D12::ToViewport(stateCache->m_viewport);
		d3d12GraphicsCommandList->RSSetViewports(1, &viewport);
		stateCache->m_dirtyFlags.m_viewport = false;
	}
	if (stateCache->m_dirtyFlags.m_scissor)
	{
		D3D12_RECT scissorRect = Translator::ToD3D12::ToRect(stateCache->m_scissor);
		d3d12GraphicsCommandList->RSSetScissorRects(1, &scissorRect);
		stateCache->m_dirtyFlags.m_scissor = false;
	}

	if (setPipelineState)
	{
		auto* pipelineStateManager = D3D12PipelineStateManager::GetInstance();
		auto* pso = pipelineStateManager->FindOrCreate(d3d12GraphicsPipelineStateDesc);
		if (pso)
		{
			d3d12GraphicsCommandList->SetPipelineState(pso);
		}
		else
		{
			STRAVA_ASSERT(0);
		}
	}

	d3d12GraphicsCommandList->DrawInstanced(packet->m_vertexCountPerInstance, packet->m_instanceCount, packet->m_startVertexLocation, packet->m_startInstanceLocation);
}

// End Draw

#undef STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK

void D3D12CommandProcessor::OnSubmitCommandBuffer(const GraphicsCommandBuffer& graphicsCommandBuffer)
{
#if !defined(STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK)
#define STRAVA_D3D12_COMMAND_PROCESSOR_CASE(commandPacketType, cb)					\
case CommandPacketType::##commandPacketType:										\
{																					\
	auto* commandPacket = reinterpret_cast<CommandPacket##commandPacketType*>(cb);	\
	D3D12CommandProcessor_on##commandPacketType(*this, commandPacket);				\
	cb += commandPacket->m_size;													\
	break;																			\
}
#endif
	auto* gcbCurrent = graphicsCommandBuffer.GetBegin();
	auto* gcbBack = graphicsCommandBuffer.GetBack();
	while (gcbCurrent != gcbBack)
	{
		auto* commandPacketType = reinterpret_cast<CommandPacketType*>(gcbCurrent);
		switch (*commandPacketType)
		{
#if 0
		case CommandPacketType::ClearRenderTarget:
		{
			reinterpret_cast<CommandPacketClear*>(gcbCurrent);
			D3D12CommandProcessor_onClearRenderTarget()
				break;
		}
#endif
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(ClearRenderTarget, gcbCurrent);

		// Render Pass
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(BeginPass, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(EndPass, gcbCurrent);

		// Vertex Shader
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetVertexShader, gcbCurrent);

		// Pixel Shader
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetPixelShader, gcbCurrent);

		// Input Assembler
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetPrimitiveTopology, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetVertexBuffers, gcbCurrent);

		// Rasterizer
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetViewport, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetScissor, gcbCurrent);

		// Output Merger
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetRenderTargets, gcbCurrent);

		// Draw
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(Draw, gcbCurrent);

		default:
		{
			STRAVA_ASSERT(!"Unimplemeted command packet is required!");
			auto* commandPacket = reinterpret_cast<CommandPacketUnknown*>(gcbCurrent);
			gcbCurrent += commandPacket->m_size;
			break;
		}
		}
	}
#if 0
	HRESULT hr = {};
	hr = m_d3d12GraphicsCommandList->Close();
	if (FAILED(hr))
	{
		return;
	}
#endif
}

bool D3D12CommandProcessor::OnPrepareCommandBuffer(ID3D12CommandAllocator* d3d12CommandAllocator)
{
	m_stateCache->Reset();

	HRESULT hr = {};
	hr = m_d3d12GraphicsCommandList->Reset(d3d12CommandAllocator, nullptr);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
}