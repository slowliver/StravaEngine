#pragma once

#include <initializer_list>
#include <vector>
#include <xutility>

#include <Engine/Core/Array.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Allocator.h>

namespace StravaEngine::Core
{
template <class Type>
class ArrayList
{
public:
	using value_type = Type;
	using allocator_type = Allocator<Type>;
	using pointer = std::add_pointer_t<Type>;
	using const_pointer = std::add_pointer_t<const Type>;
	using reference = Type&;
	using const_reference = const Type&;
	using size_type = Size;
	using difference_type = std::ptrdiff_t;
#if 0
	using iterator = ArrayIterator<Type, k_size>;
	using const_iterator = ArrayConstIterator<Type, k_size>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
#endif

	ArrayList()
	{
//		std::vector a = std::vector();
	}
#if 0
	explicit ArrayList(Size size);
	ArrayList(Size size, const Type& value);
	template <class IteratorType>
	vector(IteratorType first, IteratorType last);
	ArrayList(const ArrayList& arrayList);
	ArrayList(ArrayList&& arrayList);
	ArrayList(std::initializer_list<Type> initializerList);
#endif

	void Reserve(Size count)
	{
		if (count > m_capacity)
		{
			auto allocator = Allocator<Type>();
			auto* newData = allocator.allocate(count);
			for (Size i = 0; i < m_count; ++i)
			{
				::new(&newData[i]) Type(std::move(m_data[i]));
				m_data[i].~Type();
			}
			if (m_data)
			{
				allocator.deallocate(m_data, m_capacity);
			}
			m_data = newData;
			m_capacity = count;
		}
	}

	Size GetCapacity() const
	{
		return m_capacity;
	}

	Size GetCount() const
	{
		return m_count;
	}

	Size GetMaxCount() const
	{
		return UINT64_MAX;
	}

	void Resize(Size count)
	{
		Reserve(count);
		if (count > m_count)
		{
			for (Size i = m_count; i < count; ++i)
			{
				::new(&m_data[i]) Type();
			}
			m_count = count;
		}
		else if (count < m_count)
		{
			for (Size i = count; i < m_count; ++i)
			{
				m_data[i].~Type();
			}
			m_count = count;
		}
	}

	void Resize(Size count, const Type& value)
	{
		Reserve(count);
		if (count > m_count)
		{
			for (Size i = m_count; i < count; ++i)
			{
				::new(&m_data[i]) Type(value);
			}
			m_count = count;
		}
		else if (count < m_count)
		{
			for (Size i = count; i < m_count; ++i)
			{
				m_data[i].~Type();
			}
			m_count = count;
		}
	}

	[[nodiscard]]
	bool IsEmpty() const
	{
		return m_count == 0;
	}

	void TrimToSize()
	{
		assert(0);
	}

	// C++ STL alias
	Size size() const { return GetCount(); }
	Size max_size() const { return GetMaxCount(); }
	Size max_size() const { return GetMaxCount(); }
	void resize(Size count) { Resize(count); }
	void resize(Size count, const Type& value) { Resize(count, value); }
	Size capacity() const { return GetCapacity(); }
	[[nodiscard]]
	bool empty() const { bool IsEmpty(); }
	void reserve(Size count) { Reserve(count); }

private:
	Type* m_data = nullptr;
	Size m_count = 0;
	Size m_capacity = 0;
};
}