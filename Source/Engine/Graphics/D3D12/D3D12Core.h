#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include <Engine/Graphics/Renderer.h>

#include "./../CommandBuffer.h"

namespace StravaEngine::Graphics::D3D12
{
class D3D12CommandProcessor;
class D3D12RootSignature;
class D3D12PipelineStateManager;
class D3D12Core
{
private:
	static constexpr UInt32 k_frameCount = 3;

public:
	D3D12Core();
	~D3D12Core();
	bool Initialize(const RendererSpec& spec);
	void Terminate();
	void OnPrepareCommandBuffer();
	void OnSubmitCommandBuffer(const GraphicsCommandBuffer& graphicsCommandBuffer);

	ID3D12Device* GetD3D12Device() { return m_d3d12Device; }
	ID3D12CommandAllocator* GetD3D12CommandAllocator(UInt32 index = 0) { return m_commandAllocators[index]; }

	STRAVA_FORCE_INLINE D3D12RootSignature* GetRootSignature() { return m_rootSignature.get(); }

public:
	static std::unique_ptr<D3D12Core> s_instance;

private:
	void WaitForGPU();
	void MoveToNextFrame();

private:
	std::unique_ptr<D3D12CommandProcessor> m_commandProcessor = nullptr;
	std::unique_ptr<D3D12RootSignature> m_rootSignature = nullptr;
	IDXGIFactory7* m_dxgiFactory7 = nullptr;
	IDXGIAdapter4* m_dxgiAdapter4 = nullptr;
	ID3D12Device* m_d3d12Device = nullptr;
	IDXGISwapChain3* m_dxgiSwapChain3 = nullptr;
	ID3D12CommandQueue* m_d3d12CmmandQueue = nullptr;
	ID3D12DescriptorHeap* m_d3d12RTVHeap = nullptr;
	ID3D12Resource* m_renderTargets[k_frameCount] = {};
	ID3D12PipelineState* m_pipelineState = nullptr;
	ID3D12CommandAllocator* m_commandAllocators[k_frameCount] = {};
	UInt32 m_frameIndex = 0;
	HANDLE m_fenceEvent = {};
	ID3D12Fence* m_fence = nullptr;
	UINT64 m_fenceValues[k_frameCount] = {};
	UInt32 m_d3d12RTVDescriptorSize = 0;

};
}