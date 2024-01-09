#include "D3D12CommandProcessor.h"

#include <d3dx12.h>

#include "D3D12Core.h"
#include "D3D12TypeTranslator.h"
#include "D3D12VertexBuffer.h"
#include "D3D12RootSignature.h"

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

			// Rasterizer
			bool m_viewport				: 1;
			bool m_scissor				: 1;
		};
	};
	DirtyFlag m_dirtyFlags;

	// Root Signature
	bool m_firstRootSignatureSet;

	// Input Assembler
	PrimitiveTopology m_primitiveTopology;

	// Rasterizer
	Viewport m_viewport;
	Core::Int32Rect m_scissor;

	void Reset()
	{
		m_dirtyFlags.m_raw = 0xFFFFFFFFFFFFFFFF;
		m_firstRootSignatureSet = true;
		m_primitiveTopology = PrimitiveTopology::Unknown;
		m_viewport = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
		m_scissor = { INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX };
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
#define STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(commandPacketType, commandProcessor, packet, pointer)	\
static void D3D12CommandProcessor_on##commandPacketType(D3D12CommandProcessor& commandProcessor, CommandPacket##commandPacketType* packet, Byte* pointer)
#endif

// Render Pass

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(BeginPass, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (stateCache->m_firstRootSignatureSet)
	{
		stateCache->m_firstRootSignatureSet = false;
		stateCache->m_dirtyFlags.m_rootSignature = true;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(EndPass, commandProcessor, packet, pointer)
{}

// Begin Input Assembler

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetPrimitiveTopology, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_primitiveTopology != stateCache->m_primitiveTopology)
	{
		stateCache->m_primitiveTopology = packet->m_primitiveTopology;
		stateCache->m_dirtyFlags.m_primitiveTopology = true;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetVertexBuffers, commandProcessor, packet, pointer)
{
	auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();
	D3D12_VERTEX_BUFFER_VIEW d3d12VertexBufferViews[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	const auto numViews = Core::Min(Core::GetCount(d3d12VertexBufferViews), static_cast<Size>(packet->m_numBuffers));
	auto* vertexBuffers = reinterpret_cast<VertexBuffer**>(pointer + sizeof(CommandPacketSetVertexBuffers));
	for (Size i = 0; i < numViews; ++i)
	{
		auto* vertexBuffer = vertexBuffers[i];
		if (!vertexBuffer)
		{
			continue;
		}
		auto* nativeVertexBuffer = static_cast<D3D12VertexBuffer*>(vertexBuffer->GetNativeVertexBuffer());
		if (!nativeVertexBuffer)
		{
			continue;
		}
		d3d12VertexBufferViews[i] = nativeVertexBuffer->GetD3D12VertexBufferView();
	}
	d3d12GraphicsCommandList->IASetVertexBuffers(packet->m_startSlot, static_cast<UINT>(numViews), d3d12VertexBufferViews);
}

// End Input Assembler

// Begin Rasterizer

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetViewport, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_viewport != stateCache->m_viewport)
	{
		stateCache->m_viewport = packet->m_viewport;
		stateCache->m_dirtyFlags.m_viewport = true;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetScissor, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_scissor != stateCache->m_scissor)
	{
		stateCache->m_scissor = packet->m_scissor;
		stateCache->m_dirtyFlags.m_scissor = true;
	}
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(Draw, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12GraphicsPipelineStateDesc = {};
#if 0
	d3d12GraphicsPipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	d3d12GraphicsPipelineStateDesc.VS = { g_vertexShader, g_vertexShaderSize };
	d3d12GraphicsPipelineStateDesc.PS = { g_pixelShader, g_pixelShaderSize };
	d3d12GraphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	d3d12GraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	d3d12GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3d12GraphicsPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3d12GraphicsPipelineStateDesc.SampleMask = UINT_MAX;
	d3d12GraphicsPipelineStateDesc.NumRenderTargets = 1;
	d3d12GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d12GraphicsPipelineStateDesc.SampleDesc.Count = 1;
#endif

	// Root Signature
	if (stateCache->m_dirtyFlags.m_rootSignature)
	{
		auto* d3d12RootSignature = D3D12Core::s_instance->GetRootSignature()->GetD3D12RootSignature();
		d3d12GraphicsPipelineStateDesc.pRootSignature = d3d12RootSignature;
		d3d12GraphicsCommandList->SetGraphicsRootSignature(d3d12RootSignature);
		stateCache->m_dirtyFlags.m_rootSignature = false;
	}

	// Input Assembler
	if (stateCache->m_dirtyFlags.m_primitiveTopology)
	{
		d3d12GraphicsPipelineStateDesc.PrimitiveTopologyType = Translator::ToD3D12::ToPrimitiveTopologyType(stateCache->m_primitiveTopology);
		D3D12_PRIMITIVE_TOPOLOGY d3d12PrimitiveTopology = Translator::ToD3D12::ToPrimitiveTopology(stateCache->m_primitiveTopology);
		d3d12GraphicsCommandList->IASetPrimitiveTopology(d3d12PrimitiveTopology);
		stateCache->m_dirtyFlags.m_primitiveTopology = false;
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

	d3d12GraphicsCommandList->DrawInstanced(packet->m_vertexCountPerInstance, packet->m_instanceCount, packet->m_startVertexLocation, packet->m_startInstanceLocation);
}

// End Rasterizer

#undef STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK

void D3D12CommandProcessor::OnSubmitCommandBuffer(const GraphicsCommandBuffer& graphicsCommandBuffer)
{
#if !defined(STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK)
#define STRAVA_D3D12_COMMAND_PROCESSOR_CASE(commandPacketType, cb)					\
case CommandPacketType::##commandPacketType:										\
{																					\
	auto* commandPacket = reinterpret_cast<CommandPacket##commandPacketType*>(cb);	\
	D3D12CommandProcessor_on##commandPacketType(*this, commandPacket, cb);			\
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

		// Render Pass
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(BeginPass, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(EndPass, gcbCurrent);

		// Input Assembler
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetPrimitiveTopology, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetVertexBuffers, gcbCurrent);

		// Rasterizer
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetViewport, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetScissor, gcbCurrent);

		// Draw
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(Draw, gcbCurrent);

		default:
		{
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