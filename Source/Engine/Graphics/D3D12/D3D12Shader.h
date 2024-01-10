#pragma once

#include <memory>
#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>
#include "./../Shader.h"

namespace StravaEngine::Graphics
{
namespace D3D12
{
class D3D12Shader : public NativeShaderBase
{
public:
	D3D12Shader() {}
	virtual ~D3D12Shader() override;
	virtual bool OnCreate(Core::ArrayList<Byte>&& bytes) override;

	const D3D12_SHADER_BYTECODE& GetShaderByteCode() const { return m_shaderByteCode; }

private:
	Core::ArrayList<Byte> m_bytes;
	D3D12_SHADER_BYTECODE m_shaderByteCode;
};
}
using NativeShader = typename D3D12::D3D12Shader;
}