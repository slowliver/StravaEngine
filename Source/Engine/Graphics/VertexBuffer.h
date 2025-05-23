#pragma once

#include <memory>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Graphics/Type.h>

namespace StravaEngine::Graphics
{
struct VertexBufferSpec final
{
	Size m_stride;
	Size m_size;
};

class NativeVertexBufferBase
{
public:
	NativeVertexBufferBase() {}
	virtual ~NativeVertexBufferBase() {}
	virtual bool OnCreate(const VertexBufferSpec& vertexBufferSpec, void* vertexData) = 0;
};

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	bool Create(const VertexBufferSpec& vertexBufferSpec, void* vertexData);
	void Release();

	const NativeVertexBufferBase* GetNativeVertexBuffer() const { return m_nativeVertexBuffer.get(); }
	NativeVertexBufferBase* GetNativeVertexBuffer() { return m_nativeVertexBuffer.get(); }

private:
	std::unique_ptr<NativeVertexBufferBase> m_nativeVertexBuffer = nullptr;
	VertexBufferSpec m_spec = {};
	Core::ArrayList<Byte> m_data;
};
}