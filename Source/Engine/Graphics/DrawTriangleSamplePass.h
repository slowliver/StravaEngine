#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Graphics/Type.h>

#include "VertexBuffer.h"
#include "RenderTexture.h"
#include "Shader.h"
#include "Texture.h"

namespace StravaEngine::Graphics
{
class DrawTriangleSamplePass
{
public:
	DrawTriangleSamplePass();
	~DrawTriangleSamplePass() {}

	bool Initialize();
	void Terminate();
	void OnRender();

private:
	VertexBuffer m_vertexBuffer;
	RenderTexture m_renderTexture;
	Texture m_testTexture;
	Shader m_vertexShader;
	Shader m_pixelShader;
	Shader m_fullTriangleVertexShader;
	Shader m_fullTrianglePixelShader;
};
}