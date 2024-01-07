#include "DrawTriangleSamplePass.h"

#include <Engine/Graphics/Renderer.h>
#include "CommandBuffer.h"

namespace StravaEngine::Graphics
{
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
	spec.m_data = (Byte*)triangleVertices;
	spec.m_dataSize = sizeof(triangleVertices);

	m_vertexBuffer.reset(new VertexBuffer());
	m_vertexBuffer->Create(spec);
//	std::printf(k_vertexAttributeTypeNames[0]);
	return true;
}

void DrawTriangleSamplePass::Terminate()
{}

void DrawTriangleSamplePass::OnRender()
{
	auto* graphicsCmmandBuffer = Renderer::GetInstance()->GetGraphicsCommandBuffer();

	VertexBuffer* vertexBuffers[] = { m_vertexBuffer.get()};
	graphicsCmmandBuffer->SetVertexBuffers(0, vertexBuffers);

	Viewport viewport = { 0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f };
	graphicsCmmandBuffer->SetViewport(viewport);

	Core::Int32Rect scissor = { 0, 0, 800, 600 };
	graphicsCmmandBuffer->SetScissor(scissor);

	graphicsCmmandBuffer->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	graphicsCmmandBuffer->Draw(0, 0, 0, 0);

//	graphicsCmmandBuffer->Set
//	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}
}