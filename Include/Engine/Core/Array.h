#pragma once

#include <array>
#include <iterator>
#include <algorithm>

#include "CoreMinimal.h"

template <class Type, Size k_size>
class ArrayConstIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;
	using reference = const Type&;
	using pointer = const Type*;

public:
	constexpr ArrayConstIterator()
		: m_pointer()
		, m_index(0)
	{}

	constexpr explicit ArrayConstIterator(pointer pointer, Size offset = 0)
		: m_pointer(pointer)
		, m_index(offset)
	{}

	[[nodiscard]]
	constexpr pointer operator->() const
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index < k_size);
		return m_pointer + m_index;
	}

	[[nodiscard]]
	constexpr reference operator*() const
	{
		return *operator->();
	}

	constexpr ArrayConstIterator& operator++()
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index < k_size);
		++m_index;
		return *this;
	}

	constexpr ArrayConstIterator operator++(int)
	{
		ArrayConstIterator temp = *this;
		++*this;
		return temp;
	}

	constexpr ArrayConstIterator& operator--()
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index != 0);
		--m_index;
		return *this;
	}

	constexpr ArrayConstIterator operator--(int)
	{
		ArrayConstIterator temp = *this;
		--*this;
		return temp;
	}

	constexpr ArrayConstIterator& operator+=(const difference_type offset)
	{
		m_index += static_cast<Size>(offset);
		return *this;
	}

	constexpr ArrayConstIterator& operator-=(const difference_type offset)
	{
		return *this += -offset;
	}

	[[nodiscard]]
	constexpr difference_type operator-(const ArrayConstIterator& right) const
	{
		return static_cast<difference_type>(m_index - right.m_index);
	}

	[[nodiscard]]
	constexpr reference operator[](const difference_type offset) const
	{
		return *(*this + offset);
	}

	[[nodiscard]]
	constexpr bool operator==(const ArrayConstIterator& right) const
	{
		return m_index == right.m_index;
	}

	[[nodiscard]]
	constexpr bool operator<(const ArrayConstIterator& right) const
	{
		return m_index < right.m_index;
	}

	[[nodiscard]]
	constexpr ArrayConstIterator operator+(const difference_type offset) const
	{
		ArrayConstIterator temp = *this;
		temp += offset;
		return temp;
	}

	[[nodiscard]]
	constexpr ArrayConstIterator operator-(const difference_type offset) const
	{
		ArrayConstIterator temp = *this;
		temp -= offset;
		return temp;
	}

#if 0
	[[nodiscard]]
	constexpr ArrayConstIterator operator+(const difference_type offset, ArrayConstIterator next)
	{
		next += offset;
		return next;
	}
#endif

	[[nodiscard]]
	constexpr bool operator!=(const ArrayConstIterator& right) const
	{
		return !(*this == right);
	}

	[[nodiscard]]
	constexpr bool operator>(const ArrayConstIterator& right) const
	{
		return right < *this;
	}

	[[nodiscard]]
	constexpr bool operator<=(const ArrayConstIterator& right) const
	{
		return !(right < *this);
	}

	[[nodiscard]]
	constexpr bool operator>=(const ArrayConstIterator& right) const
	{
		return !(*this < right);
	}

private:
	pointer m_pointer;
	Size m_index;
};

template <class Type, Size k_size>
class ArrayIterator : public ArrayConstIterator<Type, k_size>
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;
	using reference = Type&;
	using pointer = Type*;

public:
	constexpr ArrayIterator()
	{}

	constexpr explicit ArrayIterator(Type* pointer, Size offset = 0)
		: ArrayConstIterator<Type, k_size>(pointer, offset)
	{}

	[[nodiscard]]
	constexpr Type& operator*() const
	{
		return const_cast<Type&>(ArrayConstIterator<Type, k_size>::operator*());
	}

	[[nodiscard]]
	constexpr Type* operator->() const
	{
		return const_cast<Type*>(ArrayConstIterator<Type, k_size>::operator->());
	}

	constexpr ArrayIterator& operator++()
	{
		ArrayConstIterator<Type, k_size>::operator++();
		return *this;
	}

	constexpr ArrayIterator operator++(int)
	{
		ArrayIterator temp = *this;
		ArrayConstIterator<Type, k_size>::operator++();
		return temp;
	}

	constexpr ArrayIterator& operator--()
	{
		ArrayConstIterator<Type, k_size>::operator--();
		return *this;
	}

	constexpr ArrayIterator operator--(int)
	{
		ArrayIterator temp = *this;
		ArrayConstIterator<Type, k_size>::operator--();
		return temp;
	}

	constexpr ArrayIterator& operator+=(const ptrdiff_t offset)
	{
		ArrayConstIterator<Type, k_size>::operator+=(offset);
		return *this;
	}

	[[nodiscard]]
	constexpr ArrayIterator operator+(const ptrdiff_t offset) const
	{
		ArrayIterator temp = *this;
		temp += offset;
		return temp;
	}

#if 0
	[[nodiscard]]
	constexpr ArrayIterator operator+(const ptrdiff_t offset, ArrayIterator next)
	{
		next += offset;
		return next;
	}
#endif

	constexpr ArrayIterator& operator-=(const ptrdiff_t offset)
	{
		ArrayConstIterator<Type, k_size>::operator-=(offset);
		return *this;
	}

	using ArrayConstIterator<Type, k_size>::operator-;

	[[nodiscard]]
	constexpr ArrayIterator operator-(const ptrdiff_t offset) const
	{
		ArrayIterator temp = *this;
		temp -= offset;
		return temp;
	}

	[[nodiscard]]
	constexpr Type& operator[](const ptrdiff_t offset) const
	{
		return const_cast<Type&>(ArrayConstIterator<Type, k_size>::operator[](offset));
	}
};

namespace StravaEngine::Core
{
template <class Type, Size k_size>
class Array
{
public:
	using value_type = Type;
	using size_type = Size;
	using difference_type = ptrdiff_t;
	using pointer = Type*;
	using const_pointer = const Type*;
	using reference = Type&;
	using const_reference = const Type&;
	using iterator = ArrayIterator<Type, k_size>;
	using const_iterator = ArrayConstIterator<Type, k_size>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	[[nodiscard]]
	constexpr reference operator[](Size index)
	{
		STRAVA_VERIFY(index >= 0 && index < k_size);
		return m_data[index];
	}

	[[nodiscard]]
	constexpr const_reference operator[](Size index) const
	{
		STRAVA_VERIFY(index >= 0 && index < k_size);
		return m_data[index];
	}

	constexpr void fill(const Type& value)
	{
		for (Size i = 0; i < k_size; ++i)
		{
			at[i] == value;
		}
	}

	constexpr void swap(Array& other)
	{
		std::swap_ranges(begin(), end(), other.begin());
	}

	[[nodiscard]]
	constexpr iterator begin()
	{
		return iterator(m_data, 0);
	}

	[[nodiscard]]
	constexpr const_iterator begin() const
	{
		return const_iterator(m_data, 0);
	}

	[[nodiscard]]
	constexpr iterator end()
	{
		return iterator(m_data, k_size);
	}

	[[nodiscard]]
	constexpr const_iterator end() const
	{
		return const_iterator(m_data, k_size);
	}

	[[nodiscard]]
	constexpr reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	[[nodiscard]]
	constexpr const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}

	[[nodiscard]]
	constexpr reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	[[nodiscard]]
	constexpr const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	[[nodiscard]]
	constexpr const_iterator cbegin() const
	{
		return begin();
	}

	[[nodiscard]]
	constexpr const_iterator cend() const
	{
		return end();
	}

	[[nodiscard]]
	constexpr const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	[[nodiscard]]
	constexpr const_reverse_iterator crend() const
	{
		return rend();
	}

	[[nodiscard]]
	constexpr size_type size() const
	{
		return k_size;
	}

	[[nodiscard]]
	constexpr size_type max_size() const
	{
		return size();
	}

	[[nodiscard]]
	constexpr bool empty() const
	{
		return false;
	}

	[[nodiscard]]
	constexpr reference at(Size index)
	{
		return m_data[index];
	}

	[[nodiscard]]
	constexpr const_reference at(Size index) const
	{
		return m_data[index];
	}

	[[nodiscard]]
	constexpr reference front()
	{
		return m_data[0];
	}

	[[nodiscard]]
	constexpr const_reference front() const
	{
		return m_data[0];
	}

	[[nodiscard]]
	constexpr reference back()
	{
		return m_data[k_size - 1];
	}

	[[nodiscard]]
	constexpr const_reference back() const
	{
		return m_data[k_size - 1];
	}

	[[nodiscard]]
	constexpr pointer data()
	{
		return m_data;
	}

	[[nodiscard]]
	constexpr const_pointer data() const
	{
		return m_data;
	}

	
public:
	/// <summary>
	/// arrayクラスはpublicな配列メンバ変数を持ち、非トリビアルなコンストラクタを提供しない。
	/// そのため、arrayは集成体の要件を満たす。
	/// これにより、arrayクラスは組み込み配列と同様の初期化構文を使用して初期化を行うことができる。
	/// https://cpprefjp.github.io/reference/array/array/op_initializer.html
	/// </summary>
	Type m_data[k_size];
};

template <class Type, Size k_size>
constexpr Array<std::remove_cv_t<Type>, k_size> ToArray(Type(&a)[k_size])
{
	return Array<std::remove_cv_t<Type>, k_size>(a);
}

template <class Type, Size k_size>
constexpr Array<std::remove_cv_t<Type>, k_size> ToArray(Type(&&a)[k_size])
{
	return Array<std::remove_cv_t<Type>, k_size>(a);
}
}

