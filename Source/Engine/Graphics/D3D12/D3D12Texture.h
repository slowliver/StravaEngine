#pragma once

#include <memory>
#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>
#include <Engine/Graphics/Texture.h>

namespace StravaEngine::Graphics
{
namespace D3D12
{
class D3D12Texture : public NativeTextureBase
{
public:
	D3D12Texture() {}
	virtual ~D3D12Texture() override;
	virtual bool OnCreate(const TextureSpec& textureSpec, Core::ArrayList<Byte>&& bytes) override;

	ID3D12Resource* GetD3D12Resource() { return m_d3d12Resource; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetD3D12CPUDescriptorHandle() { return m_d3d12CPUDescriptorHandle; }

private:
	ID3D12Resource* m_d3d12Resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12CPUDescriptorHandle = {};
//	Core::ArrayList<Byte> m_bytes;
};
}
using NativeTexture = typename D3D12::D3D12Texture;
}