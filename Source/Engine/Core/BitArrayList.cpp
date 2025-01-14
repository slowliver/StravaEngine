#pragma once

#include <Engine/Core/BitArrayList.h>

namespace StravaEngine::Core
{
BitArrayList::BitArrayList()
{}

BitArrayList::BitArrayList(UInt32 count)
{
	Resize(count);
}

BitArrayList::BitArrayList(BitArrayList&& bitArrayList)
	: m_packedData(std::move(bitArrayList.m_packedData))
	, m_count(std::move(bitArrayList.m_count))
	, m_capacity(std::move(bitArrayList.m_capacity))
{}

void BitArrayList::fill(bool value)
{
	for (Int64 i = 0; i < (Int64(m_count) - 1) / 8 + 1; ++i)
	{
		m_packedData[i] = value ? 0xFF : 0x00;
	}
}

void BitArrayList::Reserve(UInt32 capacity)
{
	if (capacity == 0 || capacity / 8 <= m_capacity / 8)
	{
		return;
	}
	auto* newData = Allocator::Allocate((capacity - 1) / 8 + 1);
	for (Int64 i = 0; i < (Int64(m_count) - 1) / 8 + 1; ++i)
	{
		::new(&newData[i]) UInt8(m_packedData[i]);
		m_data[i].~Type();
	}
	if (m_data)
	{
		allocator.Deallocate(m_data);
	}
	m_data = newData;
	m_capacity = capacity;
}


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

