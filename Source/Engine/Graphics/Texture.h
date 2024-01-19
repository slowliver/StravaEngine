#pragma once

#include <memory>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayProxy.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>

namespace StravaEngine::Graphics
{
struct TextureSpec final
{
	TextureDimension m_dimension;
	UInt32 m_width;
	UInt32 m_height;
	UInt32 m_depthOrArrayCount;
	Format m_format;
	UInt32 m_mipCount;
	Multisample m_multisample;
};

class NativeTextureBase
{
public:
	NativeTextureBase() {}
	virtual ~NativeTextureBase() {}
	virtual bool OnCreate(const TextureSpec& textureSpec, Core::ArrayList<Byte>&& bytes) = 0;
};

class Texture
{
public:
	Texture();
	~Texture();

	bool Create(const TextureSpec& textureSpec, Core::ArrayProxy<Byte> bytes);
	bool Create1D(UInt32 width, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Create1DArray(UInt32 width, UInt32 arrayCount, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Create2D(UInt32 width, UInt32 height, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Create2DArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Create2DMultisample(UInt32 width, UInt32 height, Format format, Multisample multisample, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Create2DMultisampleArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, Multisample multisample, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Create3D(UInt32 width, UInt32 height, UInt32 depth, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool CreateCube(UInt32 width, UInt32 height, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool CreateCubeArray(UInt32 width, UInt32 height, UInt32 cubeCount, Format format, Core::ArrayProxy<Byte> bytes, UInt32 mipCount = 1);
	bool Map2D() { return true; }
	void Release();

	const NativeTextureBase* GetNativeTexture() const { return m_nativeTexture.get(); }
	NativeTextureBase* GetNativeTexture() { return m_nativeTexture.get(); }
	template <class NativeShaderType>
	const NativeShaderType* GetNativeTexture() const { return static_cast<const NativeShaderType*>(GetNativeTexture()); }
	template <class NativeShaderType>
	NativeShaderType* GetNativeTexture() { return static_cast<NativeShaderType*>(GetNativeTexture()); }

private:
	std::unique_ptr<NativeTextureBase> m_nativeTexture = nullptr;
	TextureSpec m_spec = {};
};
}