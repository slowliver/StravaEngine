#include "D3D12Texture.h"

#include <d3dx12.h>

#include "D3D12Core.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12TypeTranslator.h"

namespace StravaEngine::Graphics::D3D12
{
D3D12Texture::~D3D12Texture()
{}

// Heap-allocating UpdateSubresources implementation
static inline UINT64 D3D12Texture_UpdateSubresources(
	_In_ ID3D12GraphicsCommandList* pCmdList,
	_In_ ID3D12Resource* pDestinationResource,
	_In_ ID3D12Resource* pIntermediate,
	UINT64 IntermediateOffset,
	_In_range_(0, D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
	_In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
	_In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData) noexcept
{
	UINT64 RequiredSize = 0;
	const auto MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
	if (MemToAlloc > SIZE_MAX)
	{
		return 0;
	}
	void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
	if (pMem == nullptr)
	{
		return 0;
	}
	auto pLayouts = static_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
	auto pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
	auto pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);

#if defined(_MSC_VER) || !defined(_WIN32)
	const auto Desc = pDestinationResource->GetDesc();
#else
	D3D12_RESOURCE_DESC tmpDesc;
	const auto& Desc = *pDestinationResource->GetDesc(&tmpDesc);
#endif
	ID3D12Device* pDevice = nullptr;
	pDestinationResource->GetDevice(IID_PPV_ARGS(&pDevice));
	pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
	pDevice->Release();

	const UINT64 Result = ::UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, pLayouts, pNumRows, pRowSizesInBytes, pSrcData);
	HeapFree(GetProcessHeap(), 0, pMem);
	return Result;
}


bool D3D12Texture::OnCreate(const TextureSpec& textureSpec, Core::ArrayList<Byte>&& bytes)
{
	HRESULT hr = {};

	const D3D12_HEAP_PROPERTIES d3d12HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_HEAP_FLAGS d3d12HeapFlags = D3D12_HEAP_FLAG_NONE;
	d3d12HeapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

	const auto depthOrArrayCount = Core::Max(1u, textureSpec.m_depthOrArrayCount);
	const auto isArray = (depthOrArrayCount > 1);
	const auto mipCount = Core::Max(1u, textureSpec.m_mipCount);
	const bool isMultisample = (textureSpec.m_multisample != Multisample::k1x);
	const bool isDepthStencil = FormatUtility::GetIsDepthStencil(textureSpec.m_format);
	const Format typelessFormat = (!isDepthStencil) ? textureSpec.m_format : FormatUtility::ToTypeless(textureSpec.m_format);

	D3D12_RESOURCE_DESC d3d12ResourceDesc = {};
	d3d12ResourceDesc.Dimension = Translator::ToD3D12::ToResourceDimension(textureSpec.m_dimension);
	d3d12ResourceDesc.Alignment = 0;
	d3d12ResourceDesc.Width = textureSpec.m_width;
	d3d12ResourceDesc.DepthOrArraySize = depthOrArrayCount;
	d3d12ResourceDesc.MipLevels = mipCount;
	d3d12ResourceDesc.Format = Translator::ToDXGI::ToFormat(typelessFormat);
	d3d12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d12ResourceDesc.Flags = (!isDepthStencil) ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	d3d12ResourceDesc.SampleDesc.Count = Core::Min(1 << Core::ToUnderlying(textureSpec.m_multisample), D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT);
	d3d12ResourceDesc.SampleDesc.Quality = 0;
	switch (textureSpec.m_dimension)
	{
	case TextureDimension::k1D:
	{
		d3d12ResourceDesc.Height = 1;
		break;
	}
	case TextureDimension::k2D:
	case TextureDimension::k3D:
	{
		d3d12ResourceDesc.Height = textureSpec.m_height;
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
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_d3d12Resource)
	);
	if (FAILED(hr))
	{
		return false;
	}

	ID3D12Resource* textureUploadHeap = nullptr;
	UInt64 uploadBufferSize = 0;// ::GetRequiredIntermediateSize(m_d3d12Resource, 0, 1);
	d3d12Device->GetCopyableFootprints(&d3d12ResourceDesc, 0, mipCount, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

	const D3D12_HEAP_PROPERTIES d3d12HeapPropertiesForUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const D3D12_RESOURCE_DESC d3d12ResourceDescForUpload = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	hr = d3d12Device->CreateCommittedResource
	(
		&d3d12HeapPropertiesForUpload,
		D3D12_HEAP_FLAG_NONE,
		&d3d12ResourceDescForUpload,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureUploadHeap)
	);
	if (FAILED(hr))
	{
		return false;
	}

	D3D12_SUBRESOURCE_DATA d3d12SubresourceData = {};
	d3d12SubresourceData.pData = bytes.GetData();
	d3d12SubresourceData.RowPitch = textureSpec.m_width * FormatUtility::GetSizePerPixel(textureSpec.m_format);
	d3d12SubresourceData.SlicePitch = d3d12SubresourceData.RowPitch * textureSpec.m_height;

	struct DeferredFunctor final
	{
		struct Arguments final
		{
			Arguments(UInt32 mipCount) : m_mipCount(mipCount), m_subresourceData(new D3D12_SUBRESOURCE_DATA[mipCount]) {}
			~Arguments() { delete[] m_subresourceData; }
			ID3D12Resource* m_src;
			ID3D12Resource* m_dst;
			UInt32 m_mipCount;
			D3D12_SUBRESOURCE_DATA* m_subresourceData;
		};
		static void Execute(void* rawCommandList, void* rawArguments)
		{
			auto* arguments = static_cast<Arguments*>(rawArguments);
			auto* d3d12GraphicsCommandList = static_cast<ID3D12GraphicsCommandList*>(rawCommandList);
			D3D12Texture_UpdateSubresources(d3d12GraphicsCommandList, arguments->m_dst, arguments->m_src, 0, 0, arguments->m_mipCount, arguments->m_subresourceData);
			D3D12_RESOURCE_BARRIER d3d12ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				arguments->m_dst,
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			d3d12GraphicsCommandList->ResourceBarrier(1, &d3d12ResourceBarrier);
			delete arguments;
		}
	};

	auto* arguments = new DeferredFunctor::Arguments(mipCount);
	arguments->m_src = textureUploadHeap;
	arguments->m_dst = m_d3d12Resource;
	for (UInt32 i = 0; i < mipCount; ++i)
	{
		std::memcpy(&arguments->m_subresourceData[i], &d3d12SubresourceData, sizeof(D3D12_SUBRESOURCE_DATA));
	}

	auto* graphicsCmmandBuffer = Renderer::GetInstance()->GetGraphicsCommandBuffer();
	graphicsCmmandBuffer->SetNativeCommand(&DeferredFunctor::Execute, arguments);

	D3D12_SHADER_RESOURCE_VIEW_DESC d3d12ShaderResourceViewDesc = {};
	d3d12ShaderResourceViewDesc.Format = Translator::ToDXGI::ToFormat(textureSpec.m_format);
	d3d12ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
	switch (textureSpec.m_dimension)
	{
	case TextureDimension::k1D:
	{
		if (!isArray)
		{
			d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			d3d12ShaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
			d3d12ShaderResourceViewDesc.Texture1D.MipLevels = mipCount;
			d3d12ShaderResourceViewDesc.Texture1D.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			d3d12ShaderResourceViewDesc.Texture1DArray.MostDetailedMip = 0;
			d3d12ShaderResourceViewDesc.Texture1DArray.MipLevels = mipCount;
			d3d12ShaderResourceViewDesc.Texture1DArray.FirstArraySlice = 0;
			d3d12ShaderResourceViewDesc.Texture1DArray.ArraySize = depthOrArrayCount;
			d3d12ShaderResourceViewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
		}
		break;
	}
	case TextureDimension::k2D:
	{
		if (!isArray)
		{
			if (!isMultisample)
			{
				d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				d3d12ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				d3d12ShaderResourceViewDesc.Texture2D.MipLevels = mipCount;
				d3d12ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
				d3d12ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			}
		}
		else
		{
			if (!isMultisample)
			{
				d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				d3d12ShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
				d3d12ShaderResourceViewDesc.Texture2DArray.MipLevels = mipCount;
				d3d12ShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
				d3d12ShaderResourceViewDesc.Texture2DArray.ArraySize = depthOrArrayCount;
				d3d12ShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
				d3d12ShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
				d3d12ShaderResourceViewDesc.Texture2DMSArray.FirstArraySlice = 0;
				d3d12ShaderResourceViewDesc.Texture2DMSArray.ArraySize = depthOrArrayCount;
			}
		}
		break;
	}
	case TextureDimension::k3D:
	{
		d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		d3d12ShaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
		d3d12ShaderResourceViewDesc.Texture3D.MipLevels = mipCount;
		d3d12ShaderResourceViewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
		break;
	}
	default: // ‚±‚±‚É—ˆ‚½‚ç‚Ü‚¸‚¢.
	{
		STRAVA_ASSERT(0);
		return false;
	}
	}

	auto* descriptorPoolCBVSRVUAV = D3D12Core::s_instance->GetCPUDescriptorHeapCBVSRVUAV();
	if (m_d3d12CPUDescriptorHandle = descriptorPoolCBVSRVUAV->Allocate(); m_d3d12CPUDescriptorHandle.ptr)
	{
		d3d12Device->CreateShaderResourceView(m_d3d12Resource, &d3d12ShaderResourceViewDesc, m_d3d12CPUDescriptorHandle);
	}
	return true;
}
}