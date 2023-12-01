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

private:
	using PointerType = const Type*;
	using ReferenceType = const Type&;

    enum { _EEN_SIZE = _Size }; // helper for expression evaluator

public:
	constexpr ArrayConstIterator()
		: m_pointer()
		, m_index(0)
	{}

	constexpr explicit ArrayConstIterator(PointerType pointer, Size Offset = 0)
		: m_pointer(pointer)
		, m_index(Offset)
	{}

	[[nodiscard]] constexpr ReferenceType operator*() const
	{
		return *operator->();
	}

    _NODISCARD _CONSTEXPR17 pointer operator->() const noexcept {
        _STL_VERIFY(_Ptr, "cannot dereference value-initialized array iterator");
        _STL_VERIFY(_Idx < _Size, "cannot dereference out of range array iterator");
        return _Ptr + _Idx;
    }

    _CONSTEXPR17 _Array_const_iterator& operator++() noexcept {
        _STL_VERIFY(_Ptr, "cannot increment value-initialized array iterator");
        _STL_VERIFY(_Idx < _Size, "cannot increment array iterator past end");
        ++_Idx;
        return *this;
    }

    _CONSTEXPR17 _Array_const_iterator operator++(int) noexcept {
        _Array_const_iterator _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    _CONSTEXPR17 _Array_const_iterator& operator--() noexcept {
        _STL_VERIFY(_Ptr, "cannot decrement value-initialized array iterator");
        _STL_VERIFY(_Idx != 0, "cannot decrement array iterator before begin");
        --_Idx;
        return *this;
    }

    _CONSTEXPR17 _Array_const_iterator operator--(int) noexcept {
        _Array_const_iterator _Tmp = *this;
        --*this;
        return _Tmp;
    }

    constexpr void _Verify_offset(const ptrdiff_t _Off) const noexcept {
        if (_Off != 0) {
            _STL_VERIFY(_Ptr, "cannot seek value-initialized array iterator");
        }

        if (_Off < 0) {
            _STL_VERIFY(_Idx >= size_t{0} - static_cast<size_t>(_Off), "cannot seek array iterator before begin");
        }

        if (_Off > 0) {
            _STL_VERIFY(_Size - _Idx >= static_cast<size_t>(_Off), "cannot seek array iterator after end");
        }
    }

    _CONSTEXPR17 _Array_const_iterator& operator+=(const ptrdiff_t _Off) noexcept {
        _Verify_offset(_Off);
        _Idx += static_cast<size_t>(_Off);
        return *this;
    }

    _CONSTEXPR17 _Array_const_iterator& operator-=(const ptrdiff_t _Off) noexcept {
        return *this += -_Off;
    }

    _NODISCARD _CONSTEXPR17 ptrdiff_t operator-(const _Array_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return static_cast<ptrdiff_t>(_Idx - _Right._Idx);
    }

    _NODISCARD _CONSTEXPR17 reference operator[](const ptrdiff_t _Off) const noexcept {
        return *(*this + _Off);
    }

    _NODISCARD _CONSTEXPR17 bool operator==(const _Array_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return _Idx == _Right._Idx;
    }

#if _HAS_CXX20
    _NODISCARD constexpr strong_ordering operator<=>(const _Array_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return _Idx <=> _Right._Idx;
    }
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
    _NODISCARD _CONSTEXPR17 bool operator<(const _Array_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return _Idx < _Right._Idx;
    }
#endif // !_HAS_CXX20

    _CONSTEXPR17 void _Compat(const _Array_const_iterator& _Right) const noexcept { // test for compatible iterator pair
        _STL_VERIFY(_Ptr == _Right._Ptr, "array iterators incompatible");
    }

    using _Prevent_inheriting_unwrap = _Array_const_iterator;

    _NODISCARD constexpr pointer _Unwrapped() const noexcept {
        return _Ptr + _Idx;
    }

    constexpr void _Verify_with(const _Array_const_iterator& _Last) const noexcept {
        // note _Compat check inside operator<=
        _STL_VERIFY(*this <= _Last, "array iterator range transposed");
    }

    constexpr void _Seek_to(pointer _It) noexcept {
        _Idx = static_cast<size_t>(_It - _Ptr);
    }

private:
	PointerType m_pointer;
	Size m_index;

public:
    _NODISCARD _CONSTEXPR17 _Array_const_iterator operator+(const ptrdiff_t _Off) const noexcept {
        _Array_const_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD _CONSTEXPR17 _Array_const_iterator operator-(const ptrdiff_t _Off) const noexcept {
        _Array_const_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND _CONSTEXPR17 _Array_const_iterator operator+(
        const ptrdiff_t _Off, _Array_const_iterator _Next) noexcept {
        _Next += _Off;
        return _Next;
    }

#if !_HAS_CXX20
    _NODISCARD _CONSTEXPR17 bool operator!=(const _Array_const_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }

    _NODISCARD _CONSTEXPR17 bool operator>(const _Array_const_iterator& _Right) const noexcept {
        return _Right < *this;
    }

    _NODISCARD _CONSTEXPR17 bool operator<=(const _Array_const_iterator& _Right) const noexcept {
        return !(_Right < *this);
    }

    _NODISCARD _CONSTEXPR17 bool operator>=(const _Array_const_iterator& _Right) const noexcept {
        return !(*this < _Right);
    }
#endif // !_HAS_CXX20
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