#include "DrawTriangleSamplePass.h"

#include <Engine/Graphics/Type.h>
#include <Engine/Graphics/Renderer.h>
#include <Engine/Graphics/CommandBuffer.h>

#include <Engine/../../External/masyos/xxhash_cx/include/xxhash_cx.h>

STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, VSMain);
STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, PSMain);
STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, VSMainFull);
STRAVA_SHADER_DECLARATION(D3D12::Shader, Shader, PSMainFull);

namespace StravaEngine::Graphics
{
static Byte* DrawTriangleSamplePass_CreateTexture()
{
	static constexpr UInt32 width = 128;
	static constexpr UInt32 height = 128;
	static UInt32 data[width * height];
	for (UInt32 x = 0; x < width; ++x)
	{
		for (UInt32 y = 0; y < height; ++y)
		{
			UInt32 color = 0;
			color |= (UInt32)(((float)x / width) * 0xFF);
			color |= (UInt32)(((float)y / height) * 0xFF) << 8;
			color |= 0xFF000000;
			data[x + y * width] = color;
		}
	}
	return reinterpret_cast<Byte*>(data);
}

DrawTriangleSamplePass::DrawTriangleSamplePass()
{}

bool DrawTriangleSamplePass::Initialize()
{
	auto vertexLayout =
	{
		VertexAttribute(VertexAttributeType::Position, 0, VertexAttributeFormat::Float32, 3),
		VertexAttribute(VertexAttributeType::Color, 0, VertexAttributeFormat::Float32, 4),
	};

	struct Vertex
	{
		float position[3];
		float color[4];
	};

	float m_aspectRatio = 16.0f / 9.0f;
	// Define the geometry for a triangle.
	Vertex triangleVertices[] =
	{
		{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	VertexBufferSpec spec;
	spec.m_size = sizeof(triangleVertices);
	spec.m_stride = sizeof(Vertex);

	m_vertexBuffer.Create(spec, triangleVertices);

	m_renderTexture.Create2D(800, 600, Format::R8G8B8A8_UNorm);

	m_testTexture.Create2D(128, 128, Format::R8G8B8A8_UNorm, Core::ArrayProxy<Byte>(DrawTriangleSamplePass_CreateTexture(), 128 * 128 * 4), 1);
	
	m_vertexShader.Create(Core::ArrayProxy<Byte>(reinterpret_cast<Byte*>(STRAVA_GET_SHADER_BINARY_ARRAY(D3D12::Shader, Shader, VSMain)), STRAVA_GET_SHADER_BINARY_SIZE(D3D12::Shader, Shader, VSMain)));
	m_pixelShader.Create(Core::ArrayProxy<Byte>(reinterpret_cast<Byte*>(STRAVA_GET_SHADER_BINARY_ARRAY(D3D12::Shader, Shader, PSMain)), STRAVA_GET_SHADER_BINARY_SIZE(D3D12::Shader, Shader, PSMain)));

	m_fullTriangleVertexShader.Create(Core::ArrayProxy<Byte>(reinterpret_cast<Byte*>(STRAVA_GET_SHADER_BINARY_ARRAY(D3D12::Shader, Shader, VSMainFull)), STRAVA_GET_SHADER_BINARY_SIZE(D3D12::Shader, Shader, VSMainFull)));
	m_fullTrianglePixelShader.Create(Core::ArrayProxy<Byte>(reinterpret_cast<Byte*>(STRAVA_GET_SHADER_BINARY_ARRAY(D3D12::Shader, Shader, PSMainFull)), STRAVA_GET_SHADER_BINARY_SIZE(D3D12::Shader, Shader, PSMainFull)));

//	std::printf(k_vertexAttributeTypeNames[0]);
	return true;
}

void DrawTriangleSamplePass::Terminate()
{}

void DrawTriangleSamplePass::OnRender(GraphicsCommandBufferRef graphicsCmmandBuffer)
{
	float color[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
	graphicsCmmandBuffer.ClearRenderTarget(&m_renderTexture, color);

	RenderTexture* rts[] = {&m_renderTexture};
	graphicsCmmandBuffer.SetRenderTargets(rts);

	graphicsCmmandBuffer.BeginPass();
	{
		VertexBuffer* vertexBuffers[] = { &m_vertexBuffer };
		graphicsCmmandBuffer.SetVertexBuffers(0, vertexBuffers);

		Viewport viewport = { 0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f };
		graphicsCmmandBuffer.SetViewport(viewport);

		Core::Int32Rect scissor = { 0, 0, 800, 600 };
		graphicsCmmandBuffer.SetScissor(scissor);

		graphicsCmmandBuffer.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

		graphicsCmmandBuffer.SetVertexShader(&m_fullTriangleVertexShader);
		graphicsCmmandBuffer.SetPixelShader(&m_fullTrianglePixelShader);
		graphicsCmmandBuffer.SetPSShaderResources(&m_testTexture);

		graphicsCmmandBuffer.Draw(3);

		viewport = { 0.0f, 0.0f, 400.0f, 300.0f, 0.0f, 1.0f };
		graphicsCmmandBuffer.SetViewport(viewport);

		scissor = { 0, 0, 400, 300 };
		graphicsCmmandBuffer.SetScissor(scissor);
		graphicsCmmandBuffer.Draw(3);
	}
	graphicsCmmandBuffer.EndPass();

//	graphicsCmmandBuffer->Set
//	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}
}