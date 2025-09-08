#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/Renderer.h>

#include "D3D12/D3D12Texture.h"

namespace StravaEngine::Graphics
{
Texture::Texture()
	: m_nativeTexture(new NativeTexture())
{}

Texture::~Texture()
{}

bool Texture::Create(const TextureSpec& TextureSpec, Core::ArrayProxy<Byte> bytes)
{
	m_spec = TextureSpec;
	auto bytesArray = Core::ArrayList<Byte>(bytes.begin(), bytes.end());
	auto& resourceQueueCreate = Renderer::GetInstance()->GetResourceQueueCreate();
	resourceQueueCreate.Add
	(
		[&spec = m_spec, nativeTexture = m_nativeTexture.get(), data = std::move(bytesArray)]() mutable
		{
			nativeTexture->OnCreate(spec, std::move(data));
		}
	);
	return true;
}

bool Texture::Create1D(UInt32 width, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k1D;
	spec.m_width = width;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

bool Texture::Create1DArray(UInt32 width, UInt32 arrayCount, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k1D;
	spec.m_width = width;
	spec.m_depthOrArrayCount = arrayCount;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

bool Texture::Create2D(UInt32 width, UInt32 height, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

bool Texture::Create2DArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = arrayCount;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

bool Texture::Create2DMultisample(UInt32 width, UInt32 height, Format format, Multisample multisample, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	spec.m_multisample = multisample;
	return Create(spec, bytes);
}

bool Texture::Create2DMultisampleArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, Multisample multisample, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_format = format;
	spec.m_depthOrArrayCount = arrayCount;
	spec.m_mipCount = mipCount;
	spec.m_multisample = multisample;
	return Create(spec, bytes);
}

bool Texture::Create3D(UInt32 width, UInt32 height, UInt32 depth, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k3D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = depth;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

bool Texture::CreateCube(UInt32 width, UInt32 height, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = 6;
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

bool Texture::CreateCubeArray(UInt32 width, UInt32 height, UInt32 cubeCount, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount)
{
	TextureSpec spec = {};
	spec.m_dimension = TextureDimension::k2D;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depthOrArrayCount = 6 * Core::Max(1u, cubeCount);
	spec.m_format = format;
	spec.m_mipCount = mipCount;
	return Create(spec, bytes);
}

void Texture::Release()
{

}
}