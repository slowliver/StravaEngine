#pragma once

#include <type_traits>

#include <Engine/Core/CoreMinimal.h>

#if ST_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace StravaEngine::Core
{
struct Allocator
{
//	using value_type = Type;
	using propagate_on_container_move_assignment = std::true_type;
	using size_type = Size;
	using difference_type = std::ptrdiff_t;
	using is_always_equal = std::true_type;

	Allocator() = delete;
	Allocator(const Allocator&) = delete;
	~Allocator() = delete;
	Allocator& operator =(const Allocator&) = delete;
	void* operator new(Size) = delete;
	void* operator new(Size, void* ptr) = delete;
	void* operator new[](Size) = delete;
	void* operator new[](Size, void* ptr) = delete;
	void operator delete(void*) = delete;
	void operator delete(void*, void*) = delete;
	void operator delete[](void*) = delete;
	void operator delete[](void*, void*) = delete;

	[[nodiscard]]
	static void* AlignedAllocate(Size size, Size align);

	[[nodiscard]]
	static void* Allocate(Size size);

	static void Free(void* pointer);
};

namespace Internal
{
template<class Type, Size k_alignment>
struct TAllocator
{
	using value_type = Type;
	using propagate_on_container_move_assignment = std::true_type;
	using size_type = Size;
	using difference_type = std::ptrdiff_t;
	using is_always_equal = std::true_type;

	TAllocator() {};
	TAllocator(const Allocator&) {};
	TAllocator(Allocator&&) {};
#if 0
	template<typename OtherType, Size k_alignment2>
	TAllocator(const Allocator<OtherType, k_alignment2>&) {};
#endif
	~TAllocator() {};

	TAllocator& operator=(const TAllocator&) {};
#if 0
	template<typename OtherType, Size k_alignment2>
	Allocator& operator=(const Allocator<OtherType, k_alignment2>&) {};
#endif

	[[nodiscard]]
	Type* Allocate(Size count)
	{
#if ST_PLATFORM_WINDOWS
		return static_cast<Type*>(::_aligned_malloc(count * sizeof(Type), k_alignment));
#else
#error
#endif
	}

	void Deallocate(Type* pointer)
	{
#if ST_PLATFORM_WINDOWS
		::_aligned_free(pointer);
#else
#error
#endif
	}
};

template<class Type1, Size k_alignment1, class Type2, Size k_alignment2>
inline bool operator==(const TAllocator<Type1, k_alignment1>&, const TAllocator<Type2, k_alignment2>&)
{
	return std::is_same_v<Type1, Type2>&& k_alignment1 == k_alignment2;
}

template<class Type1, Size k_alignment1, class Type2, Size k_alignment2>
inline bool operator!=(const TAllocator<Type1, k_alignment1>& lhs, const TAllocator<Type2, k_alignment2>& rhs)
{
	return !(lhs == rhs);
}
}

template<class Type, Size k_alignment = alignof(Type) < alignof(void*) ? alignof(void*) : alignof(Type)>
using TAllocator = Internal::TAllocator<Type, k_alignment>;
}

#if 0
[[nodiscard]]
void* operator new(Size size);
#endif
