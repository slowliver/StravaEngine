#include "D3D12CommandProcessor.h"
#include "D3D12CommandProcessor.h"

#include <d3dx12.h>

#include "D3D12Core.h"

namespace StravaEngine::Graphics::D3D12
{
struct D3D12CommandProcessor::StateCache final
{
	union DirtyFlag
	{
		UInt64 m_raw;
		struct
		{
			bool m_viewport : 1;
			bool m_scissor  : 1;
		};
	};
	DirtyFlag m_dirtyFlags = { 0xFFFFFFFFFFFFFFFF };
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
static Byte* D3D12CommandProcessor_on##commandPacketType(D3D12CommandProcessor& commandProcessor, CommandPacket##commandPacketType* packet, Byte* pointer)
#endif

// Begin Input Assembler

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetPrimitiveTopology, commandProcessor, packet, pointer)
{
	return pointer + sizeof(CommandPacketSetPrimitiveTopology);
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetVertexBuffers, commandProcessor, packet, pointer)
{
	return pointer + sizeof(CommandPacketSetVertexBuffers);
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
	return pointer + sizeof(CommandPacketSetViewport);
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(SetScissor, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	if (packet->m_scissor != stateCache->m_scissor)
	{
		stateCache->m_scissor = packet->m_scissor;
		stateCache->m_dirtyFlags.m_scissor = true;
	}
	return pointer + sizeof(CommandPacketSetScissor);
}

STRAVA_D3D12_COMMAND_PROCESSOR_CALLBACK(Draw, commandProcessor, packet, pointer)
{
	auto* stateCache = commandProcessor.GetStateCache();
	auto* d3d12GraphicsCommandList = commandProcessor.GetD3D12GraphicsCommandList();
	if (stateCache->m_dirtyFlags.m_viewport)
	{
		D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT
		(
			stateCache->m_viewport.m_left,
			stateCache->m_viewport.m_left + stateCache->m_viewport.m_width,
			stateCache->m_viewport.m_top,
			stateCache->m_viewport.m_top + stateCache->m_viewport.m_height,
			stateCache->m_viewport.m_minDepth,
			stateCache->m_viewport.m_maxDepth
		);
		d3d12GraphicsCommandList->RSSetViewports(1, &viewport);
	}
	if (stateCache->m_dirtyFlags.m_scissor)
	{
		D3D12_RECT scissorRect = CD3DX12_RECT
		(
			stateCache->m_scissor.m_left,
			stateCache->m_scissor.m_top,
			stateCache->m_scissor.m_right,
			stateCache->m_scissor.m_bottom
		);
		d3d12GraphicsCommandList->RSSetScissorRects(1, &scissorRect);
	}
	return pointer + sizeof(CommandPacketDraw);
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
	cb = D3D12CommandProcessor_on##commandPacketType(*this, commandPacket, cb);		\
	break;																			\
}
#endif
	auto* gcbCurrent = graphicsCommandBuffer.GetBegin();
	auto* gcbEnd = graphicsCommandBuffer.GetEnd();
	while (gcbCurrent != gcbEnd)
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
	HRESULT hr = {};
	hr = m_d3d12GraphicsCommandList->Reset(d3d12CommandAllocator, nullptr);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
}