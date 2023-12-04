#pragma once

#include <array>
#include <iterator>

#include "CoreMinimal.h"

template <class Type, Size k_size>
class ArrayConstIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;

public:
	constexpr ArrayConstIterator()
		: m_pointer()
		, m_index(0)
	{}

	constexpr explicit ArrayConstIterator(const Type* pointer, Size offset = 0)
		: m_pointer(pointer)
		, m_index(offset)
	{}

	[[nodiscard]]
	constexpr const Type* operator->() const
	{
		STRAVA_VERIFY(m_pointer);
		STRAVA_VERIFY(m_index < k_size);
		return m_pointer + m_index;
	}

	[[nodiscard]]
	constexpr const Type& operator*() const
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

	constexpr ArrayConstIterator& operator+=(const ptrdiff_t offset)
	{
		m_index += static_cast<Size>(offset);
		return *this;
	}

	constexpr ArrayConstIterator& operator-=(const ptrdiff_t offset)
	{
		return *this += -offset;
	}

	[[nodiscard]]
	constexpr ptrdiff_t operator-(const ArrayConstIterator& right) const
	{
		return static_cast<ptrdiff_t>(m_index - right.m_index);
	}

	[[nodiscard]]
	constexpr const Type& operator[](const ptrdiff_t offset) const
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
	constexpr ArrayConstIterator operator+(const ptrdiff_t offset) const
	{
		ArrayConstIterator temp = *this;
		temp += offset;
		return temp;
	}

	[[nodiscard]]
	constexpr ArrayConstIterator operator-(const ptrdiff_t offset) const
	{
		ArrayConstIterator temp = *this;
		temp -= offset;
		return temp;
	}

	[[nodiscard]]
	constexpr ArrayConstIterator operator+(const ptrdiff_t offset, ArrayConstIterator next)
	{
		next += offset;
		return next;
	}

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
	const Type* m_pointer;
	Size m_index;
};


template <class Type, Size k_size>
class ArrayIterator : public ArrayConstIterator<Type, k_size>
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Type;
	using difference_type = ptrdiff_t;

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

	[[nodiscard]]
	constexpr ArrayIterator operator+(const ptrdiff_t offset, ArrayIterator next)
	{
		next += offset;
		return next;
	}

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

#if 0
	using iterator = _Array_iterator<_Ty, _Size>;
	using const_iterator = _Array_const_iterator<_Ty, _Size>;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
#endif

#if _HAS_TR1_NAMESPACE
	_DEPRECATE_TR1_NAMESPACE void assign(const _Ty& _Value) {
		_STD fill_n(_Elems, _Size, _Value);
	}
#endif // _HAS_TR1_NAMESPACE

	_CONSTEXPR20 void fill(const _Ty& _Value) {
		_STD fill_n(_Elems, _Size, _Value);
	}

	_CONSTEXPR20 void swap(array& _Other) noexcept(_Is_nothrow_swappable<_Ty>::value) {
		_Swap_ranges_unchecked(_Elems, _Elems + _Size, _Other._Elems);
	}

	_NODISCARD _CONSTEXPR17 iterator begin() noexcept {
		return iterator(_Elems, 0);
	}

	_NODISCARD _CONSTEXPR17 const_iterator begin() const noexcept {
		return const_iterator(_Elems, 0);
	}

	_NODISCARD _CONSTEXPR17 iterator end() noexcept {
		return iterator(_Elems, _Size);
	}

	_NODISCARD _CONSTEXPR17 const_iterator end() const noexcept {
		return const_iterator(_Elems, _Size);
	}

	_NODISCARD _CONSTEXPR17 reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	_NODISCARD _CONSTEXPR17 reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	_NODISCARD _CONSTEXPR17 const_iterator cbegin() const noexcept {
		return begin();
	}

	_NODISCARD _CONSTEXPR17 const_iterator cend() const noexcept {
		return end();
	}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator crend() const noexcept {
		return rend();
	}

	_CONSTEXPR17 _Ty* _Unchecked_begin() noexcept {
		return _Elems;
	}

	_CONSTEXPR17 const _Ty* _Unchecked_begin() const noexcept {
		return _Elems;
	}

	_CONSTEXPR17 _Ty* _Unchecked_end() noexcept {
		return _Elems + _Size;
	}

	_CONSTEXPR17 const _Ty* _Unchecked_end() const noexcept {
		return _Elems + _Size;
	}

	[[nodiscard]] constexpr size_type size() const
	{
		return k_size;
	}

	[[nodiscard]] constexpr size_type max_size() const
	{
		return size();
	}

	_NODISCARD_EMPTY_ARRAY_MEMBER constexpr bool empty() const noexcept {
		return false;
	}

	_NODISCARD _CONSTEXPR17 reference at(size_type _Pos) {
		if (_Size <= _Pos) {
			_Xran();
		}

		return _Elems[_Pos];
	}

	_NODISCARD constexpr const_reference at(size_type _Pos) const {
		if (_Size <= _Pos) {
			_Xran();
		}

		return _Elems[_Pos];
	}

	_NODISCARD _CONSTEXPR17 reference operator[](_In_range_(0, _Size - 1) size_type _Pos) noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
		_STL_VERIFY(_Pos < _Size, "array subscript out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0

		return _Elems[_Pos];
	}

	_NODISCARD constexpr const_reference operator[](_In_range_(0, _Size - 1) size_type _Pos) const noexcept
		/* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
		_STL_VERIFY(_Pos < _Size, "array subscript out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0

		return _Elems[_Pos];
	}

	_NODISCARD _CONSTEXPR17 reference front() noexcept /* strengthened */ {
		return _Elems[0];
	}

	_NODISCARD constexpr const_reference front() const noexcept /* strengthened */ {
		return _Elems[0];
	}

	_NODISCARD _CONSTEXPR17 reference back() noexcept /* strengthened */ {
		return _Elems[_Size - 1];
	}

	_NODISCARD constexpr const_reference back() const noexcept /* strengthened */ {
		return _Elems[_Size - 1];
	}

	_NODISCARD _CONSTEXPR17 _Ty* data() noexcept {
		return _Elems;
	}

	_NODISCARD _CONSTEXPR17 const _Ty* data() const noexcept {
		return _Elems;
	}

	[[noreturn]] void _Xran() const {
		_Xout_of_range("invalid array<T, N> subscript");
	}

	Type m_elements[k_size];
};
}