#include "RenderTexture.h"

#include <Engine/Graphics/Renderer.h>

#include "D3D12/D3D12RenderTexture.h"

namespace StravaEngine::Graphics
{
RenderTexture::RenderTexture()
	: m_nativeRenderTexture(new NativeRenderTexture())
{}

RenderTexture::~RenderTexture()
{}

bool RenderTexture::Create(const RenderTextureSpec& renderTextureSpec)
{
	m_spec = renderTextureSpec;
	auto& resourceQueueCreate = Renderer::GetInstance()->GetResourceQueueCreate();
	resourceQueueCreate.Add
	(
		[&spec = m_spec, &nativeRenderTexture = m_nativeRenderTexture]()
		{
			nativeRenderTexture->OnCreate(spec);
		}
	);
	return true;
}

bool RenderTexture::Create1D(UInt32 width, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k1D;
	spec.m_width = width;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

bool RenderTexture::Create1DArray(UInt32 width, UInt32 arrayCount, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k1D;
	spec.m_width = width;
	spec.m_depthOrArrayCount = arrayCount;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

bool RenderTexture::Create2D(UInt32 width, UInt32 height, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

bool RenderTexture::Create2DArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = arrayCount;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

bool RenderTexture::Create2DMultisample(UInt32 width, UInt32 height, Format format, Multisample multisample, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	spec.m_multisample = multisample;
	return Create(spec);
}

bool RenderTexture::Create2DMultisampleArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, Multisample multisample, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_format = format;
	spec.m_depthOrArrayCount = arrayCount;
	spec.m_mipCount = mipCount;
	spec.m_multisample = multisample;
	return Create(spec);
}

bool RenderTexture::Create3D(UInt32 width, UInt32 height, UInt32 depth, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k3D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = depth;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

bool RenderTexture::CreateCube(UInt32 width, UInt32 height, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = 6;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

bool RenderTexture::CreateCubeArray(UInt32 width, UInt32 height, UInt32 cubeCount, Format format, UInt32 mipCount)
{
	RenderTextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = 6 * Core::Max(1u, cubeCount);
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec);
}

void RenderTexture::Release()
{

}
}