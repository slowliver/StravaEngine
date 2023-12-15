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
class ListConstIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;
	using pointer = const Type*;
	using reference = const Type&;

public:
	ListConstIterator()
		: m_pointer()
		, m_index(0)
	{}

	explicit ListConstIterator(pointer pointer, Size offset = 0)
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
	ListConstIterator& operator++()
	{
		STRAVA_VERIFY(m_pointer);
		++m_index;
		return *this;
	}

	ListConstIterator operator++(int)
	{
		ListConstIterator temp = *this;
		++* this;
		return temp;
	}
	// End forward ops.

	// Begin back ops.
	ListConstIterator& operator--()
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index != 0);
		--m_index;
		return *this;
	}

	ListConstIterator operator--(int)
	{
		ListConstIterator temp = *this;
		--* this;
		return temp;
	}
	// End back ops.

	// Begin forward any ops.
	ListConstIterator operator+(const difference_type offset) const
	{
		ListConstIterator temp = *this;
		temp.m_index += offset;
		return temp;
	}

	ListConstIterator& operator+=(const difference_type offset)
	{
		m_index += static_cast<Size>(offset);
		return *this;
	}
	// End forward any ops.

	// Begin back any ops.
	ListConstIterator operator-(const difference_type offset) const
	{
		ListConstIterator temp = *this;
		temp.m_index -= offset;
		return temp;
	}
	ListConstIterator& operator-=(const difference_type offset)
	{
		return *this += -offset;
	}
	// End back any ops.

	// Begin difference ops.
	difference_type operator-(const ListConstIterator& right) const
	{
		return static_cast<difference_type>(m_index - right.m_index);
	}
	// End difference ops.

	// Begin comparison ops.
	bool operator==(const ListConstIterator& right) const
	{
		return m_index == right.m_index;
	}

	bool operator!=(const ListConstIterator& right) const
	{
		return !(*this == right);
	}

	bool operator<(const ListConstIterator& right) const
	{
		return m_index < right.m_index;
	}

	bool operator>(const ListConstIterator& right) const
	{
		return right < *this;
	}

	bool operator<=(const ListConstIterator& right) const
	{
		return !(right < *this);
	}

	bool operator>=(const ListConstIterator& right) const
	{
		return !(*this < right);
	}
	// End comparison ops.

private:
	pointer m_pointer;
	Size m_index;
};

template <class Type>
class ListIterator : public ListConstIterator<Type>
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;
	using pointer = Type*;
	using reference = Type&;

public:
	ListIterator()
	{}

	explicit ListIterator(pointer pointer, Size offset = 0)
		: ListConstIterator<Type>(pointer, offset)
	{}

	// Begin write ops.
	reference operator*() const
	{
		return const_cast<reference>(ListConstIterator<Type>::operator*());
	}

	pointer operator->() const
	{
		return const_cast<pointer>(ListConstIterator<Type>::operator->());
	}

	reference operator[](const ptrdiff_t offset) const
	{
		return const_cast<reference>(ListConstIterator<Type>::operator[](offset));
	}
	// End write ops.

	// Begin forward ops.
	ListIterator& operator++()
	{
		ListConstIterator<Type>::operator++();
		return *this;
	}

	ListIterator operator++(int)
	{
		ListIterator temp = *this;
		ListConstIterator<Type>::operator++();
		return temp;
	}
	// End forward ops.

	// Beign back os.
	ListIterator& operator--()
	{
		ListConstIterator<Type>::operator--();
		return *this;
	}

	ListIterator operator--(int)
	{
		ListIterator temp = *this;
		ListConstIterator<Type>::operator--();
		return temp;
	}
	// End back ops.

	// Begin forward any ops.
	ListIterator operator+(const ptrdiff_t offset) const
	{
		ListIterator temp = *this;
		temp += offset;
		return temp;
	}

	ListIterator& operator+=(const ptrdiff_t offset)
	{
		ListConstIterator<Type>::operator+=(offset);
		return *this;
	}
	// End forward any ops.

	// Begin back any ops.
	ListIterator operator-(const ptrdiff_t offset) const
	{
		ListIterator temp = *this;
		temp -= offset;
		return temp;
	}

	ListIterator& operator-=(const ptrdiff_t offset)
	{
		ListConstIterator<Type>::operator-=(offset);
		return *this;
	}
	// End back any ops.
};

template <class Type>
class List
{
private:
	struct Node final
	{
		Type m_data;
		Node* m_previous = nullptr;
		Node* m_next = nullptr;
	};
public:
	using value_type = Type;
	using allocator_type = Allocator<Type>;
	using pointer = std::add_pointer_t<Type>;
	using const_pointer = std::add_pointer_t<const Type>;
	using reference = Type&;
	using const_reference = const Type&;
	using size_type = Size;
	using difference_type = std::ptrdiff_t;
	using iterator = ListIterator<Type>;
	using const_iterator = ListConstIterator<Type>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	List() {}
	explicit List(Size count) { Resize(count); }
	List(Size count, const Type& value) { Resize(count, value); }
	template <class IteratorType>
	vector(IteratorType first, IteratorType last) { AddRange(first, last); }
	List(const List& List)
		: m_count(List.m_count)
		, m_capacity(List.m_capacity)
	{
		if (List.m_count)
		{
			auto allocator = Allocator<Type>();
			m_data = allocator.allocate(m_capacity);
			for (Size i = 0; i < m_count; ++i)
			{
				::new(&m_data[i]) Type(List.m_data[i]);
			}
		}
	}
	List(List&& List)
		: m_data(std::move(List.m_data))
		, m_count(std::move(List.m_count))
		, m_capacity(std::move(List.m_capacity))
	{}
	List(std::initializer_list<Type> initializerList) { AddRange(initializerList); }

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

	void Reserve(Size capacity)
	{
		if (capacity <= m_capacity)
		{
			return;
		}
		auto allocator = Allocator<Type>();
		auto* newData = allocator.allocate(capacity);
		for (Size i = 0; i < m_count; ++i)
		{
			::new(&newData[i]) Type(std::move(m_data[i]));
			m_data[i].~Type();
		}
		if (m_data)
		{
			allocator.deallocate(m_data);
		}
		m_data = newData;
		m_capacity = capacity;
	}

	Size GetCapacity() const { return m_capacity; }

	Size GetCount() const { return m_count; }

	Size GetMaxCount() const { return UINT64_MAX; }

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

	bool IsEmpty() const { return m_count == 0; }

	void TrimToSize()
	{
		if (m_count >= m_capacity)
		{
			return;
		}
		auto allocator = Allocator<Type>();
		if (m_count == 0)
		{
			if (m_data)
			{
				allocator.deallocate(m_data, m_capacity);
			}
			m_data = nullptr;
			m_capacity = 0;
			return;
		}
		auto* newData = allocator.allocate(m_count);
		for (Size i = 0; i < m_count; ++i)
		{
			::new(&newData[i]) Type(std::move(m_data[i]));
			m_data[i].~Type();
		}
		allocator.deallocate(m_data, m_capacity);
		m_data = newData;
		m_capacity = m_count;
	}

	Type& GetAt(Size index) { return m_data[index]; }
	const Type& GetAt(Size index) const { return m_data[index]; }
	Type& operator[](Size index) { return GetAt(index); }
	const Type& operator[](Size index) const { return GetAt(index); }

	Type* GetData() { return m_data; }
	const Type* GetData() const { return m_data; }

	Type& GetFront() { return m_data[0]; }
	const Type& GetFront() const { return m_data[0]; }
	Type& GetBack() { return m_data[m_count - 1]; }
	const Type& GetBack() const { return m_data[m_count - 1]; }

	template <class... Args>
	Type& Emplace(Size index, Args&&... args)
	{
		const Size oldCount = m_count;
		const Size newCount = m_count + 1;
		if (newCount > m_capacity)
		{
			Reserve(newCount * 3 / 2);
		}
		for (Size i = oldCount; i < index; --i)
		{
			::new(&m_data[i]) Type(std::move(m_data[i - 1]));
		}
		::new(&m_data[index]) Type(std::forward<Args>(args)...);
		m_count = newCount;
		return m_data[index];
	}

	void Insert(Size index, const Type& value) { Emplace(index, value); }
	void Insert(Size index, Type&& value) { Emplace(index, std::move(value)); }

	template <class... Args>
	Type& EmplaceBack(Args&&... args) { return Emplace(m_count, std::forward<Args>(args)...); }

	Size Add(const Type& value) { EmplaceBack(value); return m_count - 1; }
	Size Add(const Type&& value) { return EmplaceBack(std::move(value)); return m_count - 1; }

	template <class IteratorType>
	void InsertRange(Size index, IteratorType first, IteratorType last)
	{
		const Size numRangeCount = static_cast<Size>(last - first);
		const Size oldCount = m_count;
		const Size newCount = m_count + numRangeCount;
		if (newCount > m_capacity)
		{
			Reserve(newCount * 3 / 2);
		}
#if 0
		Resize(newCount);
		for (Size i = oldCount; i > index; --i)
		{
			m_data[i - 1 + numRangeCount] = std::move(m_data[i - 1]);
		}
#else
		for (Size i = oldCount; i > index; --i)
		{
			::new(&m_data[i - 1 + numRangeCount]) Type(std::move(m_data[i - 1]));
		}
#endif
		for (IteratorType itr = first; itr < last; ++itr)
		{
			const auto offset = static_cast<Size>(itr - first);
			::new(&m_data[index + offset]) Type(*itr);
		}
		m_count = newCount;
	}
	void InsertRange(Size index, const List& other) { InsertRange(index, other.cbegin(), other.cend()); }
	void InsertRange(Size index, List&& other) { InsertRange(index, other.cbegin(), other.cend()); }
	void InsertRange(Size index, std::initializer_list<Type> other) { InsertRange(index, other.begin(), other.end()); }
	void InsertRange(Size index, Type* other, Size count) { InsertRange(index, &other[0], &other[count]); }
	template <Size k_size> void InsertRange(Size index, const Array<Type, k_size>& other) { InsertRange(index, other.cbegin(), other.cend()); }

	template <class IteratorType>
	void AddRange(IteratorType first, IteratorType last) { InsertRange(m_count - 1, first, last); }
	void AddRange(const List& other) { InsertRange(m_count - 1, other); }
	void AddRange(List&& other) { InsertRange(m_count - 1, std::move(other)); }
	void AddRange(std::initializer_list<Type> other) { InsertRange(m_count - 1, other); }
	void AddRange(Type* other, Size count) { InsertRange(m_count - 1, other, count); }
	template <Size k_size> void AddRange(const Array<Type, k_size>& other) { InsertRange(m_count - 1, other); }

	void Clear() { Resize(0); }

	void Swap(List other)
	{
		std::swap(m_data, other.m_data);
		std::swap(m_count, other.m_count);
		std::swap(m_capacity, other.m_capacity);
	}

	// C++ STL alias
	Size size() const { return GetCount(); }
	Size max_size() const { return GetMaxCount(); }
	void resize(Size count) { Resize(count); }
	void resize(Size count, const Type& value) { Resize(count, value); }
	Size capacity() const { return GetCapacity(); }
	bool empty() const { bool IsEmpty(); }
	void reserve(Size count) { Reserve(count); }
	void shrink_to_fit() { TrimToSize(); }
	Type& at(Size index) { return GetAt(index); }
	const Type& at(Size index) const { return GetAt(index); }
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
	iterator insert(const_iterator position, const Type& value) { Insert(static_cast<Size>(position - cbegin()), value); }
	iterator insert(const_iterator position, Type&& value) { Insert(static_cast<Size>(position - cbegin()), std::move(value)); }
	iterator insert(const_iterator position, Size count, const Type& value);
	iterator insert(const_iterator position, std::initializer_list<Type> other) { Insert(static_cast<Size>(position - cbegin()), other); }
	template <class IteratorType> iterator insert(IteratorType position, IteratorType first, IteratorType last) { Insert(static_cast<Size>(position - cbegin()), first, last); }
	template <class... Args> iterator emplace(const_iterator position, Args&&... args) { Emplace(static_cast<Size>(position - cbegin()), std::forward<Args>(args)); }
	void swap(List other) { Swap(other); }
	void clear() { Clear(); }

private:
	Type* m_data = nullptr;
	Size m_count = 0;
	Size m_capacity = 0;
};
}