#pragma once

#include <iterator>
#include <algorithm>

#include <Engine/Core/CoreMinimal.h>

namespace StravaEngine::Core
{
#if 0
template <Size k_size>
class BitArrayConstIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = bool;
	using difference_type = ptrdiff_t;
	using pointer = const Type*;
	using reference = const Type&;

public:
	constexpr BitArrayConstIterator()
		: m_pointer()
		, m_index(0)
	{}

	constexpr explicit BitArrayConstIterator(pointer pointer, Size offset = 0)
		: m_pointer(pointer)
		, m_index(offset)
	{}

	// Begin read ops.
	[[nodiscard]]
	constexpr reference operator*() const
	{
		return *operator->();
	}

	[[nodiscard]]
	constexpr pointer operator->() const
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index < k_size);
		return m_pointer + m_index;
	}

	[[nodiscard]]
	constexpr reference operator[](const difference_type offset) const
	{
		return *(*this + offset);
	}
	// End read ops.

	// Begin forward ops.
	constexpr BitArrayConstIterator& operator++()
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index < k_size);
		++m_index;
		return *this;
	}

	constexpr BitArrayConstIterator operator++(int)
	{
		ArrayConstIterator temp = *this;
		++*this;
		return temp;
	}
	// End forward ops.

	// Begin back ops.
	constexpr BitArrayConstIterator& operator--()
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index != 0);
		--m_index;
		return *this;
	}

	constexpr BitArrayConstIterator operator--(int)
	{
		BitArrayConstIterator temp = *this;
		--*this;
		return temp;
	}
	// End back ops.

	// Begin forward any ops.
	[[nodiscard]]
	constexpr BitArrayConstIterator operator+(const difference_type offset) const
	{
		BitArrayConstIterator temp = *this;
		temp.m_index += offset;
		return temp;
	}

	constexpr ArrayConstIterator& operator+=(const difference_type offset)
	{
		m_index += static_cast<Size>(offset);
		return *this;
	}
	// End forward any ops.

	// Begin back any ops.
	[[nodiscard]]
	constexpr ArrayConstIterator operator-(const difference_type offset) const
	{
		ArrayConstIterator temp = *this;
		temp.m_index -= offset;
		return temp;
	}
	constexpr ArrayConstIterator& operator-=(const difference_type offset)
	{
		return *this += -offset;
	}
	// End back any ops.

	// Begin difference ops.
	[[nodiscard]]
	constexpr difference_type operator-(const ArrayConstIterator& right) const
	{
		return static_cast<difference_type>(m_index - right.m_index);
	}
	// End difference ops.

	// Begin comparison ops.
	[[nodiscard]]
	constexpr bool operator==(const ArrayConstIterator& right) const
	{
		return m_index == right.m_index;
	}

	[[nodiscard]]
	constexpr bool operator!=(const ArrayConstIterator& right) const
	{
		return !(*this == right);
	}

	[[nodiscard]]
	constexpr bool operator<(const ArrayConstIterator& right) const
	{
		return m_index < right.m_index;
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
	// End comparison ops.

private:
	pointer m_pointer;
	Size m_index;
};

template <Size k_size>
class BitArrayIterator : public BitArrayConstIterator<Type, k_size>
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = bool;
	using difference_type = ptrdiff_t;
	using pointer = Type*;
	using reference = Type&;

public:
	constexpr BitArrayIterator()
	{}

	constexpr explicit BitArrayIterator(pointer pointer, Size offset = 0)
		: BitArrayConstIterator<Type, k_size>(pointer, offset)
	{}

	// Begin write ops.
	[[nodiscard]]
	constexpr reference operator*() const
	{
		return const_cast<reference>(ArrayConstIterator<Type, k_size>::operator*());
	}

	[[nodiscard]]
	constexpr pointer operator->() const
	{
		return const_cast<pointer>(ArrayConstIterator<Type, k_size>::operator->());
	}

	[[nodiscard]]
	constexpr reference operator[](const ptrdiff_t offset) const
	{
		return const_cast<reference>(ArrayConstIterator<Type, k_size>::operator[](offset));
	}
	// End write ops.

	// Begin forward ops.
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
	// End forward ops.

	// Beign back os.
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
	// End back ops.

	// Begin forward any ops.
	[[nodiscard]]
	constexpr ArrayIterator operator+(const ptrdiff_t offset) const
	{
		ArrayIterator temp = *this;
		temp += offset;
		return temp;
	}

	constexpr ArrayIterator& operator+=(const ptrdiff_t offset)
	{
		ArrayConstIterator<Type, k_size>::operator+=(offset);
		return *this;
	}
	// End forward any ops.

	// Begin back any ops.
	[[nodiscard]]
	constexpr ArrayIterator operator-(const ptrdiff_t offset) const
	{
		ArrayIterator temp = *this;
		temp -= offset;
		return temp;
	}

	constexpr ArrayIterator& operator-=(const ptrdiff_t offset)
	{
		ArrayConstIterator<Type, k_size>::operator-=(offset);
		return *this;
	}
	// End back any ops.
};
#endif

template <UInt32 k_count>
requires (k_count > 0)
class BitArray
{
private:
	static constexpr UInt32 k_countOfPackedData = (k_count - 1) / 8 + 1;

public:
	using value_type = bool;
	using size_type = Size;
	using difference_type = ptrdiff_t;
	using pointer = bool*;
	using const_pointer = const bool*;
	using reference = bool&;
	using const_reference = const bool&;
#if 0
	using iterator = BitArrayIterator<Type, k_size>;
	using const_iterator = ArrayConstIterator<Type, k_size>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
#endif

	constexpr void fill(bool value)
	{
		for (UInt32 i = 0; i < k_countOfPackedData; ++i)
		{
			m_packedData[i] = value ? 0xFF : 0x00;
		}
	}

#if 0
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
#endif

	[[nodiscard]]
	constexpr bool All() const
	{
		bool result = true;
		for (UInt32 i = 0; i < k_countOfPackedData; ++i)
		{
			if (m_packedData[i] != 0xFF)
			{
				result = false;
				break;
			}
		}
		return result;
	}

	[[nodiscard]]
	constexpr bool Any() const
	{
		bool result = false;
		for (UInt32 i = 0; i < k_countOfPackedData; ++i)
		{
			if (m_packedData[i] != 0x00)
			{
				result = true;
				break;
			}
		}
		return result;
	}


	[[nodiscard]]
	constexpr UInt32 GetCount() const { return k_count; }

	[[nodiscard]]
	constexpr UInt32 GetMaxCount() const { return GetCount(); }

	constexpr void SetAt(UInt32 index, bool value)
	{
		m_packedData[index / 8] &= ~UInt8(1 << (index % 8));
		m_packedData[index / 8] |= (value ? 1 : 0) << (index % 8);
	}

	[[nodiscard]]
	constexpr bool GetAt(UInt32 index) const { return (m_packedData[index / 8] & (1 << (index % 8))) != 0; }
	[[nodiscard]]
	constexpr bool operator[](UInt32 index) const { return GetAt(index); }

	// C++ STL alias
	[[nodiscard]]
	constexpr UInt32 size() const { return GetCount(); }
	[[nodiscard]]
	constexpr UInt32 max_size() const { return GetMaxCount(); }
	
private:
	UInt8  m_packedData[k_countOfPackedData] = {};
};
}

