#pragma once

#include <initializer_list>
#include <vector>
#include <xutility>
#include <type_traits>

#include <Engine/Core/Array.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Allocator.h>

namespace StravaEngine::Core
{
template <class Type>
class ArrayProxy
{
public:
	ArrayProxy() : m_pointer(nullptr), m_count(0) {}
	ArrayProxy(std::nullptr_t) : m_pointer(nullptr), m_count(0) {}
	ArrayProxy(Type& value) : m_pointer(&value), m_count(1) {}
	ArrayProxy(Type&&) = delete;
	template <class OtherType = Type, std::enable_if_t<std::is_const_v<OtherType>, std::nullptr_t> = nullptr>
	ArrayProxy(std::remove_const_t<Type>& value) : m_pointer(&value), m_count(1) {}
	template <class OtherType = Type, std::enable_if_t<std::is_const_v<OtherType>, std::nullptr_t> = nullptr>
	ArrayProxy(std::remove_const_t<Type>&&) = delete;
	ArrayProxy(Type* pointer, Size count) : m_pointer(pointer), m_count(count) {}
	template <class OtherType = Type, std::enable_if_t<std::is_const_v<OtherType>, std::nullptr_t> = nullptr>
	ArrayProxy(std::remove_const_t<OtherType>* pointer, Size count) : m_pointer(pointer), m_count(count) {}
	ArrayProxy(std::initializer_list<Type>& list) : m_pointer(list.begin()), m_count(list.size()) {}
	ArrayProxy(std::initializer_list<Type>&&) = delete;
	template <class OtherType = Type, std::enable_if_t<std::is_const_v<OtherType>, std::nullptr_t> = nullptr>
	ArrayProxy(std::initializer_list<std::remove_const_t<OtherType>> list) : m_pointer(list.begin()), m_count(list.size()) {}
	template <class OtherType = Type, std::enable_if_t<std::is_const_v<OtherType>, std::nullptr_t> = nullptr>
	ArrayProxy(std::initializer_list<std::remove_const_t<OtherType>>&&) = delete;
	template <Size k_count>
	ArrayProxy(Type (&pointer)[k_count]) : m_pointer(pointer) : m_count(k_count) {}
	template <Size k_count>
	ArrayProxy(Type&&[k_count]) = delete;
	template <class OtherType = Type, Size k_count, std::enable_if_t<std::is_const_v<OtherType>, std::nullptr_t> = nullptr>
	ArrayProxy(std::remove_const_t<OtherType>& pointer[k_count]) : m_pointer(pointer) : m_count(k_count) {}
	template <std::size_t k_count, class OtherType = Type, std::enable_if_t<std::is_const_v<OtherType>::value, std::nullptr_t> = nullptr>
	ArrayProxy(std::remove_const_t<OtherType>&&[k_count]) = delete;
	template <class OtherType, std::enable_if_t<std::is_convertible_v<decltype(std::declval<OtherType>().data()), Type*> && std::is_convertible_v<decltype(std::declval<OtherType>().size()), Size>, std::nullptr_t> = nullptr>
	ArrayProxy(OtherType& other) : m_pointer(other.data()) : m_count(other.size()) {}
	template <class OtherType, std::enable_if_t<std::is_convertible_v<decltype(std::declval<OtherType>().data()), Type*>&& std::is_convertible_v<decltype(std::declval<OtherType>().size()), Size>, std::nullptr_t> = nullptr>
	ArrayProxy(OtherType&& other) = delete;
	
	Size GetCount() const { return m_count; }
	bool IsEmpty() const { return m_count == 0; }
	const Type& GetAt(Size index) const { STRAVA_ASSERT(index >= 0 && index < m_count); return m_pointer[index]; }
	const Type& operator[](Size index) const { return GetAt(index); }
	const Type* GetData() const { return m_data; }
	Type& GetFront() { return m_data[0]; }
	const Type& GetFront() const { return m_data[0]; }
	Type& GetBack() { return m_data[m_count - 1]; }
	const Type& GetBack() const { return m_data[m_count - 1]; }

	// C++ STL alias
	const Type* begin() const { return m_pointer; }
	const Type* end() const { return m_pointer + m_count; }
	const Type& front() const { return *m_pointer; }
	const Type& back() const { return *(m_pointer + m_count - 1); }
	bool empty() const { return IsEmpty(); }
	Size size() const { return GetCount(); }
	Type* data() const { return GetData(); }
	const Type& at(Size index) const { return GetAt(index); }

private:
	Type* m_pointer = nullptr;
	Size m_count = 0;
};

template <class Type>
class StridedArrayProxy
{
public:
	StridedArrayProxy(ArrayProxy<Type> arrayProxy, Size stride) : m_arrayProxy(arrayProxy), m_stride(stride) {}
	StridedArrayProxy(ArrayProxy<Type> arrayProxy) : StridedArrayProxy(arrayProxy, sizeof(Type)) {}
	StridedArrayProxy(std::nullptr_t) : StridedArrayProxy(nullptr, 0) {}
	StridedArrayProxy() : StridedArrayProxy(nullptr, 0) {}

	StridedArrayProxy& operator=(ArrayProxy<Type>& arrayProxy) { m_arrayProxy = arrayProxy; m_stride = sizeof(Type); return *this; }

	Size GetCount() const { return m_arrayProxy.GetCount(); }
	bool IsEmpty() const { return m_arrayProxy.IsEmpty(); }
	const Type& GetAt(Size index) const { return m_arrayProxy.GetAt(index); }
	const Type& operator[](Size index) const { return m_arrayProxy.GetAt(index); }
	const Type* GetData() const { return m_arrayProxy.GetData(); }
	Type& GetFront() { return m_arrayProxy.GetFront(); }
	const Type& GetFront() const { return m_arrayProxy.GetFront(); }
	Type& GetBack() { return m_arrayProxy.GetBack(); }
	const Type& GetBack() const { return m_arrayProxy.GetBack(); }
	Size GetStride() const { return m_stride; }

	// C++ STL alias
	const Type* begin() const { return m_arrayProxy.begin(); }
	const Type* end() const { return m_arrayProxy.end(); }
	const Type& front() const { return m_arrayProxy.front(); }
	const Type& back() const { return m_arrayProxy.back(); }
	bool empty() const { return IsEmpty(); }
	Size size() const { return GetCount(); }
	Type* data() const { return GetData(); }
	const Type& at(Size index) const { return GetAt(index); }

private:
	ArrayProxy<Type> m_arrayProxy = ArrayProxy<Size>();
	Size m_stride = sizeof(Type);
};
}