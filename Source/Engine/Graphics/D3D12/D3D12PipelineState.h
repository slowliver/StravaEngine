#pragma once

#include <memory>
#include <d3d12.h>
#include <unordered_map>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Graphics::D3D12
{
class D3D12PipelineStateObject
{
public:
	explicit D3D12PipelineStateObject(ID3D12Device* d3d12Device);
	~D3D12PipelineStateObject();

private:
	ID3D12RootSignature* m_d3d12RootSignature = nullptr;
};

struct D3D12PipelineStateHash final
{
	Size operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& key) const;
};

struct D3D12PipelineStateKeyEq final
{
	bool operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& x, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& y) const;
};

class D3D12PipelineStateManager final
{
private:
	friend class Deleter;
	using PipelineStatePool = std::unordered_map
	<
		D3D12_GRAPHICS_PIPELINE_STATE_DESC,
		ID3D12PipelineState*,
		D3D12PipelineStateHash,
		D3D12PipelineStateKeyEq
	>;

public:
	static D3D12PipelineStateManager* CreateInstance();
	static D3D12PipelineStateManager* GetInstance();
	static void DestroyInstance();

	bool Initialize();
	void Terminate();

	ID3D12PipelineState* FindOrCreate(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

private:
	struct Deleter
	{
		void operator()(D3D12PipelineStateManager* thiz) const
		{
			delete thiz;
		}
	};

private:
	D3D12PipelineStateManager() {}
	~D3D12PipelineStateManager() {}

private:
	static std::unique_ptr<D3D12PipelineStateManager, Deleter> s_instance;
	PipelineStatePool m_pipelineStatePool;
};
}