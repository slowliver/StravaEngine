#pragma once

#include <memory>

#include <Engine/Core/ArrayProxy.h>
#include <Engine/Core/ArrayList.h>

namespace StravaEngine::Graphics
{
class NativeShaderBase
{
public:
	NativeShaderBase() {}
	virtual ~NativeShaderBase() {}
	virtual bool OnCreate(Core::ArrayList<Byte>&& bytes) = 0;
};

class Shader
{
public:
	Shader();
	~Shader();

	bool Create(Core::ArrayProxy<Byte> bytes);

	const NativeShaderBase* GetNativeShader() const { return m_nativeShader.get(); }
	NativeShaderBase* GetNativeShader() { return m_nativeShader.get(); }

	template <class NativeShaderType>
	const NativeShaderType* GetNativeShader() const { return static_cast<const NativeShaderType*>(GetNativeShader()); }
	template <class NativeShaderType>
	NativeShaderType* GetNativeShader() { return static_cast<NativeShaderType*>(GetNativeShader()); }

private:
	std::unique_ptr<NativeShaderBase> m_nativeShader = nullptr;
};

// using ShaderRef = std::shared_ptr<Shader>;
class ShaderFactory
{
public:
	ShaderFactory() {}
	~ShaderFactory() {}

//	bool Create()

private:
	Core::ArrayList<Shader*> m_shaders;
};
}