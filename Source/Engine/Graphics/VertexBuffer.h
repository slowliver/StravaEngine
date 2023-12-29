#pragma once

#include <memory>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>

namespace StravaEngine::Graphics
{
struct VertexBufferSpec final
{
	Byte* m_data;
	Size m_sizePerElement;
	Size m_dataSize;
};

class NativeVertexBufferBase
{
public:
	NativeVertexBufferBase() {}
	virtual ~NativeVertexBufferBase() {}
	virtual bool OnCreate(const VertexBufferSpec& vertexBufferSpec) = 0;
};

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();
	bool Create(const VertexBufferSpec& vertexBufferSpec);

private:
	VertexBufferSpec m_spec = {};
	Core::ArrayList<Byte> m_data;
	std::unique_ptr<NativeVertexBufferBase> m_nativeVertexBuffer = nullptr;
};
}