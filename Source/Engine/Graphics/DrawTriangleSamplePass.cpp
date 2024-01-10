#include "DrawTriangleSamplePass.h"

#include <Engine/Graphics/Renderer.h>
#include "CommandBuffer.h"

extern "C" Byte g_vertexShader[];
extern "C" Size g_vertexShaderSize;
extern "C" Byte g_pixelShader[];
extern "C" Size g_pixelShaderSize;

namespace StravaEngine::Graphics
{
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

	m_vertexShader.Create(Core::ArrayProxy<Byte>(g_vertexShader, g_vertexShaderSize));
	m_pixelShader.Create(Core::ArrayProxy<Byte>(g_pixelShader, g_pixelShaderSize));

//	std::printf(k_vertexAttributeTypeNames[0]);
	return true;
}

void DrawTriangleSamplePass::Terminate()
{}

void DrawTriangleSamplePass::OnRender()
{
	auto* graphicsCmmandBuffer = Renderer::GetInstance()->GetGraphicsCommandBuffer();

	graphicsCmmandBuffer->BeginPass();

	VertexBuffer* vertexBuffers[] = { &m_vertexBuffer };
	graphicsCmmandBuffer->SetVertexBuffers(0, vertexBuffers);

	Viewport viewport = { 0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f };
	graphicsCmmandBuffer->SetViewport(viewport);

	Core::Int32Rect scissor = { 0, 0, 800, 600 };
	graphicsCmmandBuffer->SetScissor(scissor);

	graphicsCmmandBuffer->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	graphicsCmmandBuffer->SetVertexShader(&m_vertexShader);
	graphicsCmmandBuffer->SetPixelShader(&m_pixelShader);

	graphicsCmmandBuffer->Draw(3);
	graphicsCmmandBuffer->Draw(3);

	graphicsCmmandBuffer->EndPass();

//	graphicsCmmandBuffer->Set
//	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}
}