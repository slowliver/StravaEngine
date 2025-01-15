#pragma once

#include <iterator>
#include <algorithm>

#include <Engine/Core/Allocator.h>
#include <Engine/Core/CoreMinimal.h>

#if 0
namespace StravaEngine::Core
{
class BitArrayList
{
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

	BitArrayList();
	explicit BitArrayList(UInt32 count);
#if 0
	BitArrayList(UInt32 count, bool value) { Resize(count, value); }
	template <class IteratorType>
	BitArrayList(IteratorType first, IteratorType last) { AddRange(first, last); }
	BitArrayList(const BitArrayList& arrayList)
		: m_count(arrayList.m_count)
		, m_capacity(arrayList.m_capacity)
	{
		if (arrayList.m_count)
		{
			auto allocator = TAllocator<Type>();
			m_data = allocator.Allocate(m_capacity);
			for (UInt32 i = 0; i < m_count; ++i)
			{
				::new(&m_data[i]) Type(arrayList.m_data[i]);
			}
		}
	}
#endif
	BitArrayList(BitArrayList&& bitArrayList);
#if 0
	BitArrayList(std::initializer_list<bool> initializerList) { AddRange(initializerList); }
#endif

	void fill(bool value);

	void Reserve(UInt32 capacity)
	{
		if (capacity <= m_capacity)
		{
			return;
		}
		auto allocator = Allocator::Allocate(capacity);
		auto* newData = allocator.Allocate(capacity);
		for (UInt32 i = 0; i < m_count; ++i)
		{
			::new(&newData[i]) Type(std::move(m_data[i]));
			m_data[i].~Type();
		}
		if (m_data)
		{
			allocator.Deallocate(m_data);
		}
		m_data = newData;
		m_capacity = capacity;
	}

	UInt32 GetCapacity() const { return m_capacity; }

	UInt32 GetCount() const { return m_count; }

	UInt32 GetMaxCount() const { return UINT64_MAX; }

	void Resize(UInt32 count)
	{
		Reserve(count);
		if (count > m_count)
		{
			for (UInt32 i = m_count; i < count; ++i)
			{
				::new(&m_data[i]) Type();
			}
			m_count = count;
		}
		else if (count < m_count)
		{
			for (UInt32 i = count; i < m_count; ++i)
			{
				m_data[i].~Type();
			}
			m_count = count;
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

	constexpr void SetAt(UInt32 index, bool value)
	{
		m_packedData[index / 8] &= ~UInt8(1 << (index % 8));
		m_packedData[index / 8] |= (value ? 1 : 0) << (index % 8);
	}

	constexpr bool GetAt(UInt32 index) const { return (m_packedData[index / 8] & (1 << (index % 8))) != 0; }
	constexpr bool operator[](UInt32 index) const { return GetAt(index); }
	
private:

private:
	UInt32 m_count = 0;
	UInt32 m_capacity = 0;
	UInt8* m_packedData = nullptr;
};
}
#endif