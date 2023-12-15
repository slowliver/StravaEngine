#include <d3d12.h>
#include <dxgi1_6.h>

#include <Engine/Graphics/Renderer.h>

namespace StravaEngine::Graphics::D3D12
{
class D3D12Core
{
private:
	static constexpr UInt32 k_frameCount = 3;

public:
	bool Initialize(const RendererSpec& spec);
	void Terminate();

public:
	static std::unique_ptr<D3D12Core> s_instance;

private:
	IDXGIFactory7* m_dxgiFactory7 = nullptr;
	IDXGIAdapter4* m_dxgiAdapter4 = nullptr;
	ID3D12Device* m_d3d12Device = nullptr;
	IDXGISwapChain3* m_dxgiSwapChain3 = nullptr;
	ID3D12CommandQueue* m_d3d12CmmandQueue = nullptr;
	ID3D12DescriptorHeap* m_d3d12RTVHeap = nullptr;
	ID3D12Resource* m_renderTargets[k_frameCount] = {};
	ID3D12RootSignature* m_rootSignature = nullptr;
	ID3D12PipelineState* m_pipelineState = nullptr;
	ID3D12CommandAllocator* m_commandAllocators[k_frameCount] = {};
	UInt32 m_frameIndex = 0;
	UInt32 m_d3d12RTVDescriptorSize = 0;

};
}