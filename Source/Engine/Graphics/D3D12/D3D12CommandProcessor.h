#pragma once

#include <d3d12.h>

#include "./../CommandBuffer.h"

namespace StravaEngine::Graphics::D3D12
{
class D3D12CommandProcessor final
{
public:
	struct StateCache;

public:
	D3D12CommandProcessor() {}
	~D3D12CommandProcessor() {}

	bool Initialize(ID3D12Device* d3d12Device, ID3D12CommandAllocator* d3d12CommandAllocator);
	void Terminate();

	bool OnPrepareCommandBuffer(ID3D12CommandAllocator* d3d12CommandAllocator);
	void OnSubmitCommandBuffer(const GraphicsCommandBuffer& graphicsCommandBuffer);

	STRAVA_FORCE_INLINE StateCache* GetStateCache() { return m_stateCache; }
	STRAVA_FORCE_INLINE ID3D12GraphicsCommandList* GetD3D12GraphicsCommandList() { return m_d3d12GraphicsCommandList; }

private:
	ID3D12GraphicsCommandList* m_d3d12GraphicsCommandList = nullptr;
	StateCache* m_stateCache = nullptr;

};
}