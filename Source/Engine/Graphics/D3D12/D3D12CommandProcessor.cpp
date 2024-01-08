#include "D3D12CommandProcessor.h"

#include <d3dx12.h>

#include "D3D12Core.h"
#include "D3D12TypeTranslator.h"
#include "D3D12VertexBuffer.h"

namespace StravaEngine::Graphics::D3D12
{
struct D3D12CommandProcessor::StateCache final
{
	union DirtyFlag
	{
		UInt64 m_raw;
		struct
		{
			// Input Assembler
			bool m_primitiveTopology	: 1;
			bool m_vertexBuffers		: 1;

			// Rasterizer
			bool m_viewport				: 1;
			bool m_scissor				: 1;
		};
	};
	DirtyFlag m_dirtyFlags = { 0xFFFFFFFFFFFFFFFF };

	// Input Assembler
	PrimitiveTopology m_primitiveTopology = PrimitiveTopology::PointList;

	// Rasterizer
	Viewport m_viewport = Viewport();
	Core::Int32Rect m_scissor = Core::Int32Rect();

	void Reset()
	{
		m_dirtyFlags.m_raw = 0xFFFFFFFFFFFFFFFF;
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
	for (Size i = 0; i < numViews; ++i)
	{
		auto* vertexBuffer = static_cast<D3D12VertexBuffer*>(packet->m_buffers[i]->GetNativeVertexBuffer());
		d3d12VertexBufferViews[i] = vertexBuffer->GetD3D12VertexBufferView();
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

	// Input Assembler
	if (stateCache->m_dirtyFlags.m_primitiveTopology)
	{
		D3D12_PRIMITIVE_TOPOLOGY d3d12PrimitiveTopology = Translator::ToD3D12(stateCache->m_primitiveTopology);
		d3d12GraphicsCommandList->IASetPrimitiveTopology(d3d12PrimitiveTopology);
	}

	// Rasterizer
	if (stateCache->m_dirtyFlags.m_viewport)
	{
		D3D12_VIEWPORT viewport = Translator::ToD3D12(stateCache->m_viewport);
		d3d12GraphicsCommandList->RSSetViewports(1, &viewport);
	}
	if (stateCache->m_dirtyFlags.m_scissor)
	{
		D3D12_RECT scissorRect = Translator::ToD3D12(stateCache->m_scissor);
		d3d12GraphicsCommandList->RSSetScissorRects(1, &scissorRect);
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

		// Input Assembler
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetPrimitiveTopology, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetVertexBuffers, gcbCurrent);

		// Rasterizer
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetViewport, gcbCurrent);
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(SetScissor, gcbCurrent);

		// Draw
		STRAVA_D3D12_COMMAND_PROCESSOR_CASE(Draw, gcbCurrent);

		default:
			break;
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