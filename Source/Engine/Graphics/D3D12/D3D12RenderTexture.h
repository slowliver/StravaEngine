#pragma once

#include <memory>
#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>
#include "./../RenderTexture.h"

namespace StravaEngine::Graphics
{
namespace D3D12
{
class D3D12RenderTexture : public NativeRenderTextureBase
{
public:
	D3D12RenderTexture() {}
	virtual ~D3D12RenderTexture() override;
	virtual bool OnCreate(const RenderTextureSpec& vertexBufferSpec) override;

	ID3D12Resource* GetD3D12Resource() { return m_d3d12Resource; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetD3D12CPUDescriptorHandle() { return m_d3d12CPUDescriptorHandle; }

private:
	ID3D12Resource* m_d3d12Resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12CPUDescriptorHandle = {};
//	Core::ArrayList<Byte> m_bytes;
};
}
using NativeRenderTexture = typename D3D12::D3D12RenderTexture;
}