#pragma once

#include <iterator>
#include <algorithm>
#include <memory>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Core
{
template <class Type, bool k_isThreadSafe = true>
class SharedPointer
{
public:
	std::shared_ptr<T>
	SharedPointer() {}
	SharedPointer(nullptr_t) {}
	explicit SharedPointer(Type* pointer)
		: m_pointer(pointer)
		, m_refCount(new Int64(1))
	{}
	SharedPointer(const SharedPointer& other)
		: m_pointer(other.m_pointer)
		, m_refCount(other.m_refCount)
	{
		++(*m_refCount);
	}
	SharedPointer(SharedPointer&& other)
		: m_pointer(other.m_pointer)
		, m_refCount(other.m_refCount)
	{
		other.m_pointer = nullptr;
		other.m_refCount = nullptr;
	}

public:
	/// <summary>
	/// array�N���X��public�Ȕz�񃁃��o�ϐ��������A��g���r�A���ȃR���X�g���N�^��񋟂��Ȃ��B
	/// ���̂��߁Aarray�͏W���̗̂v���𖞂����B
	/// ����ɂ��Aarray�N���X�͑g�ݍ��ݔz��Ɠ��l�̏������\�����g�p���ď��������s�����Ƃ��ł���B
	/// https://cpprefjp.github.io/reference/array/array/op_initializer.html
	/// </summary>
	Type* m_pointer = nullptr;
	Int64* m_refCount = nullptr;
};
}

