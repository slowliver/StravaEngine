#pragma once

#include <memory>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>

namespace StravaEngine::Graphics
{
struct RenderTextureSpec final
{
	TextureDimension m_dimension;
	UInt32 m_width;
	UInt32 m_height;
	UInt32 m_depthOrArrayCount;
	Format m_format;
	UInt32 m_mipCount;
	Multisample m_multisample;
};

class NativeRenderTextureBase
{
public:
	NativeRenderTextureBase() {}
	virtual ~NativeRenderTextureBase() {}
	virtual bool OnCreate(const RenderTextureSpec& vertexBufferSpec) = 0;
};

class RenderTexture
{
public:
	RenderTexture();
	~RenderTexture();

	bool Create(const RenderTextureSpec& renderTextureSpec);
	bool Create1D(UInt32 width, Format format, UInt32 mipCount = 1);
	bool Create1DArray(UInt32 width, UInt32 arrayCount, Format format, UInt32 mipCount = 1);
	bool Create2D(UInt32 width, UInt32 height, Format format, UInt32 mipCount = 1);
	bool Create2DArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, UInt32 mipCount = 1);
	bool Create2DMultisample(UInt32 width, UInt32 height, Format format, Multisample multisample, UInt32 mipCount = 1);
	bool Create2DMultisampleArray(UInt32 width, UInt32 height, UInt32 arrayCount, Format format, Multisample multisample, UInt32 mipCount = 1);
	bool Create3D(UInt32 width, UInt32 height, UInt32 depth, Format format, UInt32 mipCount = 1);
	bool CreateCube(UInt32 width, UInt32 height, Format format, UInt32 mipCount = 1);
	bool CreateCubeArray(UInt32 width, UInt32 height, UInt32 cubeCount, Format format, UInt32 mipCount = 1);
	bool Map2D() { return true; }
	void Release();

	const NativeRenderTextureBase* GetNativeRenderTexture() const { return m_nativeRenderTexture.get(); }
	NativeRenderTextureBase* GetNativeRenderTexture() { return m_nativeRenderTexture.get(); }
	template <class NativeShaderType>
	const NativeShaderType* GetNativeRenderTexture() const { return static_cast<const NativeShaderType*>(GetNativeShader()); }
	template <class NativeShaderType>
	NativeShaderType* GetNativeRenderTexture() { return static_cast<NativeShaderType*>(GetNativeShader()); }

private:
	std::unique_ptr<NativeRenderTextureBase> m_nativeRenderTexture = nullptr;
	RenderTextureSpec m_spec = {};
};
}