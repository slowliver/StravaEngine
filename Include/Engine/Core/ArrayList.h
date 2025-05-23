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
class ArrayListConstIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;
	using pointer = const Type*;
	using reference = const Type&;

public:
	ArrayListConstIterator()
		: m_pointer()
		, m_index(0)
	{}

	explicit ArrayListConstIterator(pointer pointer, UInt32 offset = 0)
		: m_pointer(pointer)
		, m_index(offset)
	{}

	// Begin read ops.
	reference operator*() const
	{
		return *operator->();
	}

	pointer operator->() const
	{
		STRAVA_VERIFY(m_pointer);
		return m_pointer + m_index;
	}

	reference operator[](const difference_type offset) const
	{
		return *(*this + offset);
	}
	// End read ops.

	// Begin forward ops.
	ArrayListConstIterator& operator++()
	{
		STRAVA_VERIFY(m_pointer);
		++m_index;
		return *this;
	}

	ArrayListConstIterator operator++(int)
	{
		ArrayListConstIterator temp = *this;
		++* this;
		return temp;
	}
	// End forward ops.

	// Begin back ops.
	ArrayListConstIterator& operator--()
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index != 0);
		--m_index;
		return *this;
	}

	ArrayListConstIterator operator--(int)
	{
		ArrayListConstIterator temp = *this;
		--* this;
		return temp;
	}
	// End back ops.

	// Begin forward any ops.
	ArrayListConstIterator operator+(const difference_type offset) const
	{
		ArrayListConstIterator temp = *this;
		temp.m_index += offset;
		return temp;
	}

	ArrayListConstIterator& operator+=(const difference_type offset)
	{
		m_index += static_cast<UInt32>(offset);
		return *this;
	}
	// End forward any ops.

	// Begin back any ops.
	ArrayListConstIterator operator-(const difference_type offset) const
	{
		ArrayListConstIterator temp = *this;
		temp.m_index -= offset;
		return temp;
	}
	ArrayListConstIterator& operator-=(const difference_type offset)
	{
		return *this += -offset;
	}
	// End back any ops.

	// Begin difference ops.
	difference_type operator-(const ArrayListConstIterator& right) const
	{
		return static_cast<difference_type>(m_index - right.m_index);
	}
	// End difference ops.

	// Begin comparison ops.
	bool operator==(const ArrayListConstIterator& right) const
	{
		return m_index == right.m_index;
	}

	bool operator!=(const ArrayListConstIterator& right) const
	{
		return !(*this == right);
	}

	bool operator<(const ArrayListConstIterator& right) const
	{
		return m_index < right.m_index;
	}

	bool operator>(const ArrayListConstIterator& right) const
	{
		return right < *this;
	}

	bool operator<=(const ArrayListConstIterator& right) const
	{
		return !(right < *this);
	}

	bool operator>=(const ArrayListConstIterator& right) const
	{
		return !(*this < right);
	}
	// End comparison ops.

private:
	pointer m_pointer;
	UInt32 m_index;
};

template <class Type>
class ArrayListIterator : public ArrayListConstIterator<Type>
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;
	using pointer = Type*;
	using reference = Type&;

public:
	ArrayListIterator()
	{}

	explicit ArrayListIterator(pointer pointer, UInt32 offset = 0)
		: ArrayListConstIterator<Type>(pointer, offset)
	{}

	// Begin write ops.
	reference operator*() const
	{
		return const_cast<reference>(ArrayListConstIterator<Type>::operator*());
	}

	pointer operator->() const
	{
		return const_cast<pointer>(ArrayListConstIterator<Type>::operator->());
	}

	reference operator[](const ptrdiff_t offset) const
	{
		return const_cast<reference>(ArrayListConstIterator<Type>::operator[](offset));
	}
	// End write ops.

	// Begin forward ops.
	ArrayListIterator& operator++()
	{
		ArrayListConstIterator<Type>::operator++();
		return *this;
	}

	ArrayListIterator operator++(int)
	{
		ArrayListIterator temp = *this;
		ArrayListConstIterator<Type>::operator++();
		return temp;
	}
	// End forward ops.

	// Beign back os.
	ArrayListIterator& operator--()
	{
		ArrayListConstIterator<Type>::operator--();
		return *this;
	}

	ArrayListIterator operator--(int)
	{
		ArrayListIterator temp = *this;
		ArrayListConstIterator<Type>::operator--();
		return temp;
	}
	// End back ops.

	// Begin forward any ops.
	ArrayListIterator operator+(const ptrdiff_t offset) const
	{
		ArrayListIterator temp = *this;
		temp += offset;
		return temp;
	}

	ArrayListIterator& operator+=(const ptrdiff_t offset)
	{
		ArrayListConstIterator<Type>::operator+=(offset);
		return *this;
	}
	// End forward any ops.

	// Begin back any ops.
	ArrayListIterator operator-(const ptrdiff_t offset) const
	{
		ArrayListIterator temp = *this;
		temp -= offset;
		return temp;
	}

	ArrayListIterator& operator-=(const ptrdiff_t offset)
	{
		ArrayListConstIterator<Type>::operator-=(offset);
		return *this;
	}
	// End back any ops.
};

template <class Type>
class ArrayList
{
public:
	using value_type = Type;
	using allocator_type = TAllocator<Type>;
	using pointer = std::add_pointer_t<Type>;
	using const_pointer = std::add_pointer_t<const Type>;
	using reference = Type&;
	using const_reference = const Type&;
	using size_type = UInt32;
	using difference_type = std::ptrdiff_t;
	using iterator = ArrayListIterator<Type>;
	using const_iterator = ArrayListConstIterator<Type>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	ArrayList() {}
	explicit ArrayList(UInt32 count) { Resize(count); }
	ArrayList(UInt32 count, const Type& value) { Resize(count, value); }
	template <class IteratorType>
	ArrayList(IteratorType first, IteratorType last) { AddRange(first, last); }
	ArrayList(const ArrayList& arrayList)
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
	ArrayList(ArrayList&& arrayList)
		: m_data(std::move(arrayList.m_data))
		, m_count(std::move(arrayList.m_count))
		, m_capacity(std::move(arrayList.m_capacity))
	{}
	ArrayList(std::initializer_list<Type> initializerList) { AddRange(initializerList); }

	ArrayList& operator=(const ArrayList& arrayList)&
	{
		if (this != &arrayList)
		{
			Clear();
			Reserve(arrayList.GetCapacity());
			AddRange(arrayList);
		}
		return *this;
	}

	ArrayList& operator=(ArrayList&& arrayList)&
	{
		if (this != &arrayList)
		{
			Clear();
			m_data = std::move(arrayList.m_data);
			m_count = std::move(arrayList.m_count);
			m_capacity = std::move(arrayList.m_capacity);
			arrayList.m_data = nullptr;
			arrayList.m_count = 0;
			arrayList.m_capacity = 0;
		}
		return *this;
	}

	iterator begin() { return iterator(m_data, 0); }
	const_iterator begin() const { return const_iterator(m_data, 0); }
	iterator end() { return iterator(m_data, m_count); }
	const_iterator end() const { return const_iterator(m_data, m_count); }
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	const_iterator cbegin() const { return begin(); }
	const_iterator cend() const { return end(); }
	const_reverse_iterator crbegin() const { return rbegin(); }
	const_reverse_iterator crend() const { return rend(); }

	void Reserve(UInt32 capacity)
	{
		if (capacity <= m_capacity)
		{
			return;
		}
		auto allocator = TAllocator<Type>();
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

	void Resize(UInt32 count, const Type& value)
	{
		Reserve(count);
		if (count > m_count)
		{
			for (UInt32 i = m_count; i < count; ++i)
			{
				::new(&m_data[i]) Type(value);
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

	bool IsEmpty() const { return m_count == 0; }

	void TrimToSize()
	{
		if (m_count >= m_capacity)
		{
			return;
		}
		auto allocator = TAllocator<Type>();
		if (m_count == 0)
		{
			if (m_data)
			{
				allocator.Deallocate(m_data, m_capacity);
			}
			m_data = nullptr;
			m_capacity = 0;
			return;
		}
		auto* newData = allocator.Allocate(m_count);
		for (UInt32 i = 0; i < m_count; ++i)
		{
			::new(&newData[i]) Type(std::move(m_data[i]));
			m_data[i].~Type();
		}
		allocator.Deallocate(m_data, m_capacity);
		m_data = newData;
		m_capacity = m_count;
	}

	Type& GetAt(UInt32 index) { return m_data[index]; }
	const Type& GetAt(UInt32 index) const { return m_data[index]; }
	Type& operator[](UInt32 index) { return GetAt(index); }
	const Type& operator[](UInt32 index) const { return GetAt(index); }

	Type* GetData() { return m_data; }
	const Type* GetData() const { return m_data; }

	Type& GetFront() { return m_data[0]; }
	const Type& GetFront() const { return m_data[0]; }
	Type& GetBack() { return m_data[m_count - 1]; }
	const Type& GetBack() const { return m_data[m_count - 1]; }

	template <class... Args>
	Type& Emplace(UInt32 index, Args&&... args)
	{
		const UInt32 oldCount = m_count;
		const UInt32 newCount = m_count + 1;
		if (newCount > m_capacity)
		{
			Reserve(newCount * 3 / 2);
		}
		for (UInt32 i = oldCount; i < index; --i)
		{
			::new(&m_data[i]) Type(std::move(m_data[i - 1]));
		}
		::new(&m_data[index]) Type(std::forward<Args>(args)...);
		m_count = newCount;
		return m_data[index];
	}

	void Insert(UInt32 index, const Type& value) { Emplace(index, value); }
	void Insert(UInt32 index, Type&& value) { Emplace(index, std::move(value)); }

	template <class... Args>
	Type& EmplaceBack(Args&&... args) { return Emplace(m_count, std::forward<Args>(args)...); }

	UInt32 Add(const Type& value) { EmplaceBack(value); return m_count - 1; }
	UInt32 Add(const Type&& value) { EmplaceBack(std::move(value)); return m_count - 1; }

	template <class IteratorType>
	void InsertRange(UInt32 index, IteratorType first, IteratorType last)
	{
		const UInt32 numRangeCount = static_cast<UInt32>(last - first);
		const UInt32 oldCount = m_count;
		const UInt32 newCount = m_count + numRangeCount;
		if (newCount > m_capacity)
		{
			Reserve(newCount * 3 / 2);
		}
#if 0
		Resize(newCount);
		for (UInt32 i = oldCount; i > index; --i)
		{
			m_data[i - 1 + numRangeCount] = std::move(m_data[i - 1]);
		}
#else
		for (UInt32 i = oldCount; i > index; --i)
		{
			::new(&m_data[i - 1 + numRangeCount]) Type(std::move(m_data[i - 1]));
		}
#endif
		for (IteratorType itr = first; itr < last; ++itr)
		{
			const auto offset = static_cast<UInt32>(itr - first);
			::new(&m_data[index + offset]) Type(*itr);
		}
		m_count = newCount;
	}
	void InsertRange(UInt32 index, const ArrayList& other) { InsertRange(index, other.cbegin(), other.cend()); }
	void InsertRange(UInt32 index, ArrayList&& other) { InsertRange(index, other.cbegin(), other.cend()); }
	void InsertRange(UInt32 index, std::initializer_list<Type> other) { InsertRange(index, other.begin(), other.end()); }
	void InsertRange(UInt32 index, Type* other, UInt32 count) { InsertRange(index, &other[0], &other[count]); }
	template <UInt32 k_size>
	void InsertRange(UInt32 index, const Array<Type, k_size>& other) { InsertRange(index, other.cbegin(), other.cend()); }

	template <class IteratorType>
	void AddRange(IteratorType first, IteratorType last) { InsertRange(m_count, first, last); }
	void AddRange(const ArrayList& other) { InsertRange(m_count, other); }
	void AddRange(ArrayList&& other) { InsertRange(m_count, std::move(other)); }
	void AddRange(std::initializer_list<Type> other) { InsertRange(m_count, other); }
	void AddRange(Type* other, UInt32 count) { InsertRange(m_count, other, count); }
	template <UInt32 k_size>
	void AddRange(const Array<Type, k_size>& other) { InsertRange(m_count, other); }

	void Clear() { Resize(0); }

	void Swap(ArrayList other)
	{
		std::swap(m_data, other.m_data);
		std::swap(m_count, other.m_count);
		std::swap(m_capacity, other.m_capacity);
	}

	Int32 IndexOf(const Type& element) const
	{
		Int32 index = -1;
		for (UInt32 i = 0; i < m_count; ++i)
		{
			if (m_data[i] == element)
			{
				index = i;
				break;
			}
		}
		return index;
	}

	bool Contains(const Type& element) const { return (IndexOf(element) != -1); }

	// C++ STL alias
	UInt32 size() const { return GetCount(); }
	UInt32 max_size() const { return GetMaxCount(); }
	void resize(UInt32 count) { Resize(count); }
	void resize(UInt32 count, const Type& value) { Resize(count, value); }
	UInt32 capacity() const { return GetCapacity(); }
	bool empty() const { bool IsEmpty(); }
	void reserve(UInt32 count) { Reserve(count); }
	void shrink_to_fit() { TrimToSize(); }
	Type& at(UInt32 index) { return GetAt(index); }
	const Type& at(UInt32 index) const { return GetAt(index); }
	Type* data() { return GetData(); }
	const Type* data() const { return GetData(); }
	Type& front() { return GetFront(); }
	const Type& front() const { return GetFront(); }
	Type& back() { return GetBack(); }
	const Type& back() const { return GetBack(); }
	void push_back(const Type& value) { Add(value); }
	void push_back(Type&& value) { Add(std::move(value)); }
	template <class... Args> void emplace_back(Args&&... args) { EmplaceBack(std::forward<Args>(args)...); }
	template <class... Args> Type& emplace_back(Args&&... args) { return EmplaceBack(std::forward<Args>(args)...); }
	void pop_back() { Resize(m_count - 1); }
	iterator insert(const_iterator position, const Type& value) { Insert(static_cast<UInt32>(position - cbegin()), value); }
	iterator insert(const_iterator position, Type&& value) { Insert(static_cast<UInt32>(position - cbegin()), std::move(value)); }
	iterator insert(const_iterator position, UInt32 count, const Type& value);
	iterator insert(const_iterator position, std::initializer_list<Type> other) { Insert(static_cast<UInt32>(position - cbegin()), other); }
	template <class IteratorType> iterator insert(IteratorType position, IteratorType first, IteratorType last) { Insert(static_cast<UInt32>(position - cbegin()), first, last); }
	template <class... Args> iterator emplace(const_iterator position, Args&&... args) { Emplace(static_cast<UInt32>(position - cbegin()), std::forward<Args>(args)); }
	void swap(ArrayList other) { Swap(other); }
	void clear() { Clear(); }

private:
	Type* m_data = nullptr;
	UInt32 m_count = 0;
	UInt32 m_capacity = 0;
};
}