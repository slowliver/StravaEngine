#pragma once

#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Graphics::D3D12
{
class D3D12RootSignature
{
public:
	D3D12RootSignature() {}
	~D3D12RootSignature() {}

	bool Initialize(ID3D12Device* d3d12Device);
	void Terminate();

	STRAVA_FORCE_INLINE ID3D12RootSignature* GetD3D12RootSignature() { return m_d3d12RootSignature; };

private:
	ID3D12RootSignature* m_d3d12RootSignature = nullptr;
};
}