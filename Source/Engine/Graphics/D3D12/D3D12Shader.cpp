#include "D3D12Shader.h"

#include <d3dx12.h>

#include "D3D12Core.h"

namespace StravaEngine::Graphics::D3D12
{
D3D12Shader::~D3D12Shader()
{}

bool D3D12Shader::OnCreate(Core::ArrayList<Byte>&& bytes)
{
	m_bytes = bytes;
	m_shaderByteCode.pShaderBytecode = m_bytes.GetData();
	m_shaderByteCode.BytecodeLength = m_bytes.GetCount();
	return true;
}
}