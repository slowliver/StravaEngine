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
	virtual bool OnCreate(Core::ArrayList<Byte>&& byteCode) = 0;
};

class Shader
{
public:
	Shader();
	~Shader();
	bool Create(Core::ArrayProxy<Byte> byteCode);

private:
	std::unique_ptr<NativeShaderBase> m_nativeShader = nullptr;
};
}