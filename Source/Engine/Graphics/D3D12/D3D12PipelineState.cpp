#include "D3D12PipelineState.h"

#include <Engine/Core/ArrayList.h>

#include "D3D12Core.h"

// バイト列のときにポインタの比較ではなくバイト列全体をチェックする.
#define STRAVA_D3D12_ENABLE_STRICT_COMPARE_BYTES		0

// バイト列のときにポインタの比較ではなくバイト列全体をチェックする.
#define STRAVA_D3D12_ENABLE_STRICT_COMPARE_STRING		0

#define STRAVA_D3D12_COMPARE(x, y, member)				(x.member == y.member)
#if STRAVA_D3D12_ENABLE_STRICT_COMPARE_STRING
#define STRAVA_D3D12_COMPARE_STRING(x, y, member)		(std::strcmp(x.member, y.member) != 0)
#else
#define STRAVA_D3D12_COMPARE_STRING(x, y, member)		STRAVA_D3D12_COMPARE(x, y, member)
#endif
#if STRAVA_D3D12_ENABLE_STRICT_COMPARE_BYTES
#define STRAVA_D3D12_COMPARE_BYTES(x, y, member, size)	(std::memcmp(x.member, y.member, size) != 0)
#else
#define STRAVA_D3D12_COMPARE_BYTES(x, y, member, size)	STRAVA_D3D12_COMPARE(x, y, member)
#endif
#define STRAVA_D3D12_COMPARE_ARRAY(x, y, member, n)		([&]() -> bool { for (Size i = 0; i < n; ++i) { if (!STRAVA_D3D12_COMPARE(x, y, member[i])) { return false; } } return true; } ())


namespace StravaEngine::Graphics::D3D12
{
D3D12PipelineStateObject::D3D12PipelineStateObject(ID3D12Device* d3d12Device)
{
	HRESULT hr = {};

#if 0
	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature->GetD3D12RootSignature();
	psoDesc.VS = { g_vertexShader, g_vertexShaderSize };
	psoDesc.PS = { g_pixelShader, g_pixelShaderSize };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	hr = d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
#endif
}

D3D12PipelineStateObject::~D3D12PipelineStateObject()
{}

Size D3D12PipelineStateHash::operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& key) const
{
	UInt64 hash = 0;
	// Root Signature
	{
		hash ^= reinterpret_cast<std::uintptr_t>(key.pRootSignature);
	}
	// VS PS DS HS GS
	{
		hash ^= 0x00000000FFFFFFFF & (key.VS.BytecodeLength);
		hash ^= 0xFFFFFFFF00000000 & (key.PS.BytecodeLength	<< 32);
		hash ^= 0x00000000FFFFFFFF & (key.DS.BytecodeLength);
		hash ^= 0xFFFFFFFF00000000 & (key.HS.BytecodeLength	<< 32);
		hash ^= 0x00000000FFFFFFFF & (key.GS.BytecodeLength);
#if !STRAVA_D3D12_ENABLE_STRICT_COMPARE_BYTES
		hash ^= reinterpret_cast<std::uintptr_t>(key.VS.pShaderBytecode);
		hash ^= reinterpret_cast<std::uintptr_t>(key.PS.pShaderBytecode);
		hash ^= reinterpret_cast<std::uintptr_t>(key.DS.pShaderBytecode);
		hash ^= reinterpret_cast<std::uintptr_t>(key.HS.pShaderBytecode);
		hash ^= reinterpret_cast<std::uintptr_t>(key.GS.pShaderBytecode);
#endif
	}
	// Stream Output
	{
		hash ^= key.StreamOutput.NumEntries;
		hash ^= static_cast<UInt64>(key.StreamOutput.NumStrides)		<< 32;
		hash ^= key.StreamOutput.RasterizedStream;
		for (UInt32 i = 0; i < key.StreamOutput.NumEntries; ++i)
		{
			const auto& soDeclaration = key.StreamOutput.pSODeclaration[i];
			hash ^= soDeclaration.Stream;
			hash ^= static_cast<UInt64>(soDeclaration.SemanticIndex)	<< 32;
			hash ^= soDeclaration.StartComponent;								// 8bits
			hash ^= static_cast<UInt64>(soDeclaration.ComponentCount)	<<  8;	// 8bits
			hash ^= static_cast<UInt64>(soDeclaration.OutputSlot)		<< 16;	// 8bits
#if !STRAVA_D3D12_ENABLE_STRICT_COMPARE_STRING
			hash ^= reinterpret_cast<std::uintptr_t>(soDeclaration.SemanticName);
#endif
		}
		for (UInt32 i = 0; i < key.StreamOutput.NumStrides; ++i)
		{
			hash ^= static_cast<UInt64>(key.StreamOutput.pBufferStrides[i]) << ((i % 2 == 0) ? 0 : 32);
		}
	}
	// Blend State
	{
		hash ^= static_cast<UInt64>(key.BlendState.AlphaToCoverageEnable ? 1 : 0);
		hash ^= static_cast<UInt64>(key.BlendState.IndependentBlendEnable ? 1 : 0)	<< 1;
		for (UInt32 i = 0; i < 8; ++i)
		{
			const auto& renderTarget = key.BlendState.RenderTarget[i];
			hash ^= static_cast<UInt64>(renderTarget.BlendEnable ? 1 : 0)			<<  2;	// 1bit
			hash ^= static_cast<UInt64>(renderTarget.LogicOpEnable ? 1 : 0)			<<  3;	// 1bit
			hash ^= static_cast<UInt64>(renderTarget.SrcBlend)						<<  5;	// 5bits
			hash ^= static_cast<UInt64>(renderTarget.DestBlend)						<< 10;	// 5bits
			hash ^= static_cast<UInt64>(renderTarget.BlendOp)						<< 15;	// 3bits
			hash ^= static_cast<UInt64>(renderTarget.SrcBlendAlpha)					<< 18;	// 5bits
			hash ^= static_cast<UInt64>(renderTarget.DestBlendAlpha)				<< 23;	// 5bits
			hash ^= static_cast<UInt64>(renderTarget.BlendOpAlpha)					<< 28;	// 3bits
			hash ^= static_cast<UInt64>(renderTarget.LogicOp)						<< 31;	// 5bits
			hash ^= static_cast<UInt64>(renderTarget.RenderTargetWriteMask)			<< 36;	// 8bits
		}
	}
	// Sample Mask
	{
		hash ^= key.SampleMask;
	}
	// Rasterizer State
	{
		hash ^= static_cast<UInt64>(key.RasterizerState.FillMode);															// 3bits
		hash ^= static_cast<UInt64>(key.RasterizerState.CullMode)													<<  3;	// 3bits
		hash ^= static_cast<UInt64>(key.RasterizerState.FrontCounterClockwise)										<<  6;	// 1bit
		hash ^= static_cast<UInt64>(key.RasterizerState.DepthBias)													<<  7;	// 32bits
		hash ^= static_cast<UInt64>(*reinterpret_cast<const UInt32*>(&key.RasterizerState.DepthBiasClamp));					// 32bits
		hash ^= static_cast<UInt64>(*reinterpret_cast<const UInt32*>(&key.RasterizerState.SlopeScaledDepthBias))	<< 32;	// 32bits
		hash ^= static_cast<UInt64>(key.RasterizerState.DepthClipEnable ? 1 : 0);											// 1bit
		hash ^= static_cast<UInt64>(key.RasterizerState.MultisampleEnable ? 1 : 0)									<<  1;	// 1bit
		hash ^= static_cast<UInt64>(key.RasterizerState.AntialiasedLineEnable ? 1 : 0)								<<  2;	// 1bit
		hash ^= static_cast<UInt64>(key.RasterizerState.ForcedSampleCount)											<< 32;	// 32bits
		hash ^= static_cast<UInt64>(key.RasterizerState.ConservativeRaster);												// 2bits
	}
	// Depth Stencil State
	{
		hash ^= static_cast<UInt64>(key.DepthStencilState.DepthEnable ? 1 : 0);					// 1bit
		hash ^= static_cast<UInt64>(key.DepthStencilState.DepthWriteMask)				<<  1;	// 3bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.DepthFunc)					<<  4;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.StencilEnable ? 1 : 0)		<<  8;	// 1bit
		hash ^= static_cast<UInt64>(key.DepthStencilState.StencilReadMask)				<<  9;	// 8bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.StencilWriteMask)				<< 17;	// 8bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.FrontFace.StencilFailOp)		<< 25;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.FrontFace.StencilDepthFailOp)	<< 29;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.FrontFace.StencilPassOp)		<< 33;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.FrontFace.StencilFunc)		<< 37;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.BackFace.StencilFailOp)		<< 41;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.BackFace.StencilDepthFailOp)	<< 45;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.BackFace.StencilPassOp)		<< 49;	// 4bits
		hash ^= static_cast<UInt64>(key.DepthStencilState.BackFace.StencilFunc)			<< 53;	// 4bits
	}
	// Input Layout
	{
		hash ^= static_cast<UInt64>(key.InputLayout.NumElements);
		for (UInt32 i = 0; i < key.InputLayout.NumElements; ++i)
		{
			const auto& inputElementDesc = key.InputLayout.pInputElementDescs[i];
#if !STRAVA_D3D12_ENABLE_STRICT_COMPARE_STRING
			hash ^= reinterpret_cast<std::uintptr_t>(inputElementDesc.SemanticName);
#endif
			hash ^= static_cast<UInt64>(inputElementDesc.SemanticIndex);
			hash ^= static_cast<UInt64>(inputElementDesc.Format)				<< 32;
			hash ^= static_cast<UInt64>(inputElementDesc.InputSlot);
			hash ^= static_cast<UInt64>(inputElementDesc.AlignedByteOffset)		<< 32;
			hash ^= static_cast<UInt64>(inputElementDesc.InputSlotClass);
			hash ^= static_cast<UInt64>(inputElementDesc.InstanceDataStepRate)	<< 32;
		}
	}
	// IB Strip Cut Value
	{
		hash ^= static_cast<UInt64>(key.IBStripCutValue);
	}
	// Primitive Topology Type
	{
		hash ^= static_cast<UInt64>(key.PrimitiveTopologyType)	<< 32;
	}
	// Num Render Targets
	{
		hash ^= static_cast<UInt64>(key.NumRenderTargets);
	}
	// RTV Formats
	{
		for (UInt32 i = 0; i < Core::Min(key.NumRenderTargets, 8u); ++i)
		{
			hash ^= static_cast<UInt64>(key.RTVFormats[i]) << ((i % 2 == 0) ? 0 : 32);
		}
	}
	// DSV Format
	{
		hash ^= static_cast<UInt64>(key.DSVFormat);
	}
	// Sample Desc
	{
		hash ^= static_cast<UInt64>(key.SampleDesc.Count);
		hash ^= static_cast<UInt64>(key.SampleDesc.Quality)	<< 32;
	}
	// Node Mask
	{
		hash ^= static_cast<UInt64>(key.NodeMask);
	}
	// Cached PSO
	{
		hash ^= static_cast<UInt64>(key.CachedPSO.CachedBlobSizeInBytes);
#if !STRAVA_D3D12_ENABLE_STRICT_COMPARE_BYTES
		hash ^= reinterpret_cast<std::uintptr_t>(key.CachedPSO.pCachedBlob);
#endif
	}
	// Flags
	{
		hash ^= static_cast<UInt64>(key.Flags);
	}
	return hash;
}

STRAVA_FORCE_INLINE static bool operator==(const D3D12_SHADER_BYTECODE& x, const D3D12_SHADER_BYTECODE& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, BytecodeLength) &&
		STRAVA_D3D12_COMPARE_BYTES(x, y, pShaderBytecode, x.BytecodeLength);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_SO_DECLARATION_ENTRY& x, const D3D12_SO_DECLARATION_ENTRY& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, Stream) &&
		STRAVA_D3D12_COMPARE(x, y, SemanticName) &&
		STRAVA_D3D12_COMPARE(x, y, SemanticIndex) &&
		STRAVA_D3D12_COMPARE(x, y, StartComponent) &&
		STRAVA_D3D12_COMPARE(x, y, ComponentCount) &&
		STRAVA_D3D12_COMPARE(x, y, OutputSlot);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_STREAM_OUTPUT_DESC& x, const D3D12_STREAM_OUTPUT_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, NumEntries) &&
		STRAVA_D3D12_COMPARE_ARRAY(x, y, pSODeclaration, x.NumEntries) &&
		STRAVA_D3D12_COMPARE(x, y, NumStrides) &&
		STRAVA_D3D12_COMPARE_ARRAY(x, y, pBufferStrides, x.NumStrides) &&
		STRAVA_D3D12_COMPARE(x, y, RasterizedStream);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_RENDER_TARGET_BLEND_DESC& x, const D3D12_RENDER_TARGET_BLEND_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, BlendEnable) &&
		STRAVA_D3D12_COMPARE(x, y, LogicOpEnable) &&
		STRAVA_D3D12_COMPARE(x, y, SrcBlend) &&
		STRAVA_D3D12_COMPARE(x, y, DestBlend) &&
		STRAVA_D3D12_COMPARE(x, y, BlendOp) &&
		STRAVA_D3D12_COMPARE(x, y, SrcBlendAlpha) &&
		STRAVA_D3D12_COMPARE(x, y, DestBlendAlpha) &&
		STRAVA_D3D12_COMPARE(x, y, BlendOpAlpha) &&
		STRAVA_D3D12_COMPARE(x, y, LogicOp) &&
		STRAVA_D3D12_COMPARE(x, y, RenderTargetWriteMask);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_BLEND_DESC& x, const D3D12_BLEND_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, AlphaToCoverageEnable) &&
		STRAVA_D3D12_COMPARE(x, y, IndependentBlendEnable) &&
		STRAVA_D3D12_COMPARE_ARRAY(x, y, RenderTarget, 8);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_RASTERIZER_DESC& x, const D3D12_RASTERIZER_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, FillMode) &&
		STRAVA_D3D12_COMPARE(x, y, CullMode) &&
		STRAVA_D3D12_COMPARE(x, y, FrontCounterClockwise) &&
		STRAVA_D3D12_COMPARE(x, y, DepthBias) &&
		STRAVA_D3D12_COMPARE(x, y, DepthBiasClamp) &&
		STRAVA_D3D12_COMPARE(x, y, SlopeScaledDepthBias) &&
		STRAVA_D3D12_COMPARE(x, y, DepthClipEnable) &&
		STRAVA_D3D12_COMPARE(x, y, MultisampleEnable) &&
		STRAVA_D3D12_COMPARE(x, y, AntialiasedLineEnable) &&
		STRAVA_D3D12_COMPARE(x, y, ForcedSampleCount) &&
		STRAVA_D3D12_COMPARE(x, y, ConservativeRaster);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_DEPTH_STENCILOP_DESC& x, const D3D12_DEPTH_STENCILOP_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, StencilFailOp) &&
		STRAVA_D3D12_COMPARE(x, y, StencilDepthFailOp) &&
		STRAVA_D3D12_COMPARE(x, y, StencilPassOp) &&
		STRAVA_D3D12_COMPARE(x, y, StencilFunc);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_DEPTH_STENCIL_DESC& x, const D3D12_DEPTH_STENCIL_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, DepthEnable) &&
		STRAVA_D3D12_COMPARE(x, y, DepthWriteMask) &&
		STRAVA_D3D12_COMPARE(x, y, DepthFunc) &&
		STRAVA_D3D12_COMPARE(x, y, StencilEnable) &&
		STRAVA_D3D12_COMPARE(x, y, StencilReadMask) &&
		STRAVA_D3D12_COMPARE(x, y, StencilWriteMask) &&
		STRAVA_D3D12_COMPARE(x, y, FrontFace) &&
		STRAVA_D3D12_COMPARE(x, y, BackFace);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_INPUT_ELEMENT_DESC& x, const D3D12_INPUT_ELEMENT_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE_STRING(x, y, SemanticName) &&
		STRAVA_D3D12_COMPARE(x, y, SemanticIndex) &&
		STRAVA_D3D12_COMPARE(x, y, Format) &&
		STRAVA_D3D12_COMPARE(x, y, InputSlot) &&
		STRAVA_D3D12_COMPARE(x, y, AlignedByteOffset) &&
		STRAVA_D3D12_COMPARE(x, y, InputSlotClass) &&
		STRAVA_D3D12_COMPARE(x, y, InstanceDataStepRate);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_INPUT_LAYOUT_DESC& x, const D3D12_INPUT_LAYOUT_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, NumElements) &&
		STRAVA_D3D12_COMPARE_ARRAY(x, y, pInputElementDescs, x.NumElements);
}
STRAVA_FORCE_INLINE static bool operator==(const DXGI_SAMPLE_DESC& x, const DXGI_SAMPLE_DESC& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, Count) &&
		STRAVA_D3D12_COMPARE(x, y, Quality);
}
STRAVA_FORCE_INLINE static bool operator==(const D3D12_CACHED_PIPELINE_STATE& x, const D3D12_CACHED_PIPELINE_STATE& y)
{
	return
		STRAVA_D3D12_COMPARE(x, y, CachedBlobSizeInBytes) &&
		STRAVA_D3D12_COMPARE_BYTES(x, y, pCachedBlob, x.CachedBlobSizeInBytes);
}

bool D3D12PipelineStateKeyEq::operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& x, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& y) const
{
	return
		STRAVA_D3D12_COMPARE(x, y, pRootSignature) &&
		STRAVA_D3D12_COMPARE(x, y, VS) &&
		STRAVA_D3D12_COMPARE(x, y, PS) &&
		STRAVA_D3D12_COMPARE(x, y, DS) &&
		STRAVA_D3D12_COMPARE(x, y, HS) &&
		STRAVA_D3D12_COMPARE(x, y, GS) &&
		STRAVA_D3D12_COMPARE(x, y, StreamOutput) &&
		STRAVA_D3D12_COMPARE(x, y, BlendState) &&
		STRAVA_D3D12_COMPARE(x, y, SampleMask) &&
		STRAVA_D3D12_COMPARE(x, y, RasterizerState) &&
		STRAVA_D3D12_COMPARE(x, y, DepthStencilState) &&
		STRAVA_D3D12_COMPARE(x, y, InputLayout) &&
		STRAVA_D3D12_COMPARE(x, y, IBStripCutValue) &&
		STRAVA_D3D12_COMPARE(x, y, PrimitiveTopologyType) &&
		STRAVA_D3D12_COMPARE(x, y, NumRenderTargets) &&
		STRAVA_D3D12_COMPARE_ARRAY(x, y, RTVFormats, Core::Min(x.NumRenderTargets, 8u)) &&
		STRAVA_D3D12_COMPARE(x, y, DSVFormat) &&
		STRAVA_D3D12_COMPARE(x, y, SampleDesc) &&
		STRAVA_D3D12_COMPARE(x, y, NodeMask) &&
		STRAVA_D3D12_COMPARE(x, y, CachedPSO) &&
		STRAVA_D3D12_COMPARE(x, y, Flags);
}

std::unique_ptr<D3D12PipelineStateManager, D3D12PipelineStateManager::Deleter> D3D12PipelineStateManager::s_instance = nullptr;

D3D12PipelineStateManager* D3D12PipelineStateManager::CreateInstance()
{
	s_instance.reset(new D3D12PipelineStateManager());
	return GetInstance();
}

D3D12PipelineStateManager* D3D12PipelineStateManager::GetInstance()
{
	return s_instance.get();
}

void D3D12PipelineStateManager::DestroyInstance()
{
	s_instance.reset(nullptr);
}

bool D3D12PipelineStateManager::Initialize()
{
	return true;
}

void D3D12PipelineStateManager::Terminate()
{
	for (auto& pair : m_pipelineStatePool)
	{
		pair.second->Release();
	}
	m_pipelineStatePool.clear();
}

ID3D12PipelineState* D3D12PipelineStateManager::FindOrCreate(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
{
	auto itr = m_pipelineStatePool.find(desc);

	ID3D12PipelineState* pso = nullptr;

	// Create
	if (itr == m_pipelineStatePool.cend())
	{
		auto* d3d12Device = D3D12Core::s_instance->GetD3D12Device();
		HRESULT hr = d3d12Device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			return nullptr;
		}
		m_pipelineStatePool.insert(std::pair<D3D12_GRAPHICS_PIPELINE_STATE_DESC, ID3D12PipelineState*>(desc, pso));
	}

	// Find
	else
	{
		pso = itr->second;
	}

	return pso;
}
}