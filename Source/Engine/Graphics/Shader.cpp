#include "Shader.h"

#include <Engine/Graphics/Renderer.h>

#include "D3D12/D3D12Shader.h"

namespace StravaEngine::Graphics
{
Shader::Shader()
	: m_nativeShader(new NativeShader())
{}

Shader::~Shader()
{}

bool Shader::Create(Core::ArrayProxy<Byte> byteCode)
{
	auto bytes = Core::ArrayList<Byte>(byteCode.begin(), byteCode.end());
	auto& resourceQueueCreate = Renderer::GetInstance()->GetResourceQueueCreate();
	resourceQueueCreate.Add
	(
		[nativeShader = m_nativeShader.get(), data = bytes]() mutable
		{
			nativeShader->OnCreate(std::move(data));
		}
	);
	return true;
}
}