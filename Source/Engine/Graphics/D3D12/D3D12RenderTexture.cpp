#include "D3D12RenderTexture.h"

#include <d3dx12.h>

#include "D3D12Core.h"
#include "D3D12TypeTranslator.h"

namespace StravaEngine::Graphics::D3D12
{
D3D12RenderTexture::~D3D12RenderTexture()
{}

bool D3D12RenderTexture::OnCreate(const RenderTextureSpec& renderTextureSpec)
{
	HRESULT hr = {};

	D3D12_HEAP_PROPERTIES d3d12HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_HEAP_FLAGS d3d12HeapFlags = D3D12_HEAP_FLAG_NONE;
	d3d12HeapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

	const bool isDepthStencil = renderTextureSpec.m_format.GetIsDepthStencil();
	Format format = (!isDepthStencil) ? renderTextureSpec.m_format : renderTextureSpec.m_format.ToTypeless();

	D3D12_RESOURCE_DESC d3d12ResourceDesc;
	d3d12ResourceDesc.Dimension = Translator::ToD3D12::ToResourceDimension(renderTextureSpec.m_dimension);
	d3d12ResourceDesc.Alignment = 0;
	d3d12ResourceDesc.Width = renderTextureSpec.m_width;
	d3d12ResourceDesc.MipLevels = Core::Max(1u, renderTextureSpec.m_mipCount);
	d3d12ResourceDesc.Format = Translator::ToDXGI::ToFormat(format);
	d3d12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d12ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	d3d12ResourceDesc.SampleDesc.Count = Core::Min(1 << Core::ToUnderlying(renderTextureSpec.m_multisample), D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT);
	d3d12ResourceDesc.SampleDesc.Quality = 0;
	switch (renderTextureSpec.m_dimension)
	{
	case TextureDimension::k1D:
	{
		d3d12ResourceDesc.Height = 1;
		d3d12ResourceDesc.DepthOrArraySize = 1;
		break;
	}
	case TextureDimension::k2D:
	case TextureDimension::k3D:
	{
		d3d12ResourceDesc.Height = renderTextureSpec.m_height;
		d3d12ResourceDesc.DepthOrArraySize = Core::Max(1u, renderTextureSpec.m_depthOrArrayCount);
		break;
	}
	default: // ‚±‚±‚É—ˆ‚½‚ç‚Ü‚¸‚¢
	{
		STRAVA_ASSERT(0);
		return false;
	}
	}
	
	auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();
	hr = d3d12Device->CreateCommittedResource
	(
		&d3d12HeapProperties,
		d3d12HeapFlags,
		&d3d12ResourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(&m_d3d12Resource)
	);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
}