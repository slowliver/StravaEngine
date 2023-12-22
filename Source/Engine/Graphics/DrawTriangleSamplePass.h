#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Graphics/Type.h>

#include "VertexBuffer.h"

namespace StravaEngine::Graphics
{
class DrawTriangleSamplePass
{
public:
	DrawTriangleSamplePass() {}
	~DrawTriangleSamplePass() {}

	bool Initialize();
	void Terminate();
	void OnRender();

private:
	std::unique_ptr<VertexBuffer> m_vertexBuffer = nullptr;

};
}