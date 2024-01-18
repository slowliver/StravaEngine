#include "D3D12RenderTexture.h"

#include <d3dx12.h>

#include "D3D12Core.h"
#include "D3D12DescriptorHeap.h"
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

	const auto depthOrArrayCount = Core::Max(1u, renderTextureSpec.m_depthOrArrayCount);
	const auto isArray = (depthOrArrayCount > 1);
	const auto mipCount = Core::Max(1u, renderTextureSpec.m_mipCount);
	const bool isMultisample = (renderTextureSpec.m_multisample != Multisample::k1x);
	const bool isDepthStencil = FormatUtility::GetIsDepthStencil(renderTextureSpec.m_format);
	const Format typelessFormat = (!isDepthStencil) ? renderTextureSpec.m_format : FormatUtility::ToTypeless(renderTextureSpec.m_format);

	D3D12_RESOURCE_DESC d3d12ResourceDesc = {};
	d3d12ResourceDesc.Dimension = Translator::ToD3D12::ToResourceDimension(renderTextureSpec.m_dimension);
	d3d12ResourceDesc.Alignment = 0;
	d3d12ResourceDesc.Width = renderTextureSpec.m_width;
	d3d12ResourceDesc.DepthOrArraySize = depthOrArrayCount;
	d3d12ResourceDesc.MipLevels = mipCount;
	d3d12ResourceDesc.Format = Translator::ToDXGI::ToFormat(typelessFormat);
	d3d12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d12ResourceDesc.Flags = (!isDepthStencil) ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	d3d12ResourceDesc.SampleDesc.Count = Core::Min(1 << Core::ToUnderlying(renderTextureSpec.m_multisample), D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT);
	d3d12ResourceDesc.SampleDesc.Quality = 0;
	switch (renderTextureSpec.m_dimension)
	{
	case TextureDimension::k1D:
	{
		d3d12ResourceDesc.Height = 1;
		break;
	}
	case TextureDimension::k2D:
	case TextureDimension::k3D:
	{
		d3d12ResourceDesc.Height = renderTextureSpec.m_height;
		break;
	}
	default: // ‚±‚±‚É—ˆ‚½‚ç‚Ü‚¸‚¢.
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

	D3D12_RENDER_TARGET_VIEW_DESC d3d12RenderTargetViewDesc = {};
	d3d12RenderTargetViewDesc.Format = Translator::ToDXGI::ToFormat(renderTextureSpec.m_format);
	switch (renderTextureSpec.m_dimension)
	{
	case TextureDimension::k1D:
	{
		if (!isArray)
		{
			d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
			d3d12RenderTargetViewDesc.Texture1D.MipSlice = 0;
		}
		else
		{
			d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
			d3d12RenderTargetViewDesc.Texture1DArray.MipSlice = 0;
			d3d12RenderTargetViewDesc.Texture1DArray.FirstArraySlice = 0;
			d3d12RenderTargetViewDesc.Texture1DArray.ArraySize = depthOrArrayCount;
		}
		break;
	}
	case TextureDimension::k2D:
	{
		if (!isArray)
		{
			if (!isMultisample)
			{
				d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				d3d12RenderTargetViewDesc.Texture2D.MipSlice = 0;
				d3d12RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
			}
			else
			{
				d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			}
		}
		else
		{
			if (!isMultisample)
			{
				d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				d3d12RenderTargetViewDesc.Texture2DArray.MipSlice = 0;
				d3d12RenderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
				d3d12RenderTargetViewDesc.Texture2DArray.ArraySize = depthOrArrayCount;
				d3d12RenderTargetViewDesc.Texture2DArray.PlaneSlice = 0;
			}
			else
			{
				d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
				d3d12RenderTargetViewDesc.Texture2DMSArray.FirstArraySlice = 0;
				d3d12RenderTargetViewDesc.Texture2DMSArray.ArraySize = depthOrArrayCount;
			}
		}
		break;
	}
	case TextureDimension::k3D:
	{
		d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
		d3d12RenderTargetViewDesc.Texture3D.MipSlice = 0;
		d3d12RenderTargetViewDesc.Texture3D.FirstWSlice = 0;
		d3d12RenderTargetViewDesc.Texture3D.WSize = depthOrArrayCount;
		break;
	}
	default: // ‚±‚±‚É—ˆ‚½‚ç‚Ü‚¸‚¢.
	{
		STRAVA_ASSERT(0);
		return false;
	}
	}

	auto* descriptorPoolRTV = D3D12Core::s_instance->GetDescriptorPoolRTV();
	m_d3d12CPUDescriptorHandle = descriptorPoolRTV->CreateRenderTargetView(m_d3d12Resource, &d3d12RenderTargetViewDesc);
	return true;
}
}