#pragma once

#include <type_traits>

#include <Engine/Core/CoreMinimal.h>

#if STRAVA_WINDOWS
#include <Windows.h>
#endif

namespace StravaEngine::Core
{
template<class Type, Size k_alignment = alignof(Type)>
struct DefaultAlignedAllocator
{
	using value_type = Type;
	using propagate_on_container_move_assignment = std::true_type;
	using size_type = Size;
	using difference_type = std::ptrdiff_t;
	using is_always_equal = std::true_type;

	DefaultAlignedAllocator() = default;
	DefaultAlignedAllocator(const DefaultAlignedAllocator&) = default;
	template<typename OtherValueType>
	DefaultAlignedAllocator(const DefaultAlignedAllocator<OtherValueType, k_alignment>&) = default;
	~DefaultAlignedAllocator() = default;

	DefaultAlignedAllocator& operator=(const DefaultAlignedAllocator&) = default;

	[[nodiscard]]
	Type* allocate(size_type n)
	{
#if STRAVA_WINDOWS
		return static_cast<Type*>(::_aligned_malloc(n * sizeof(Type), k_alignment));
#else
#error
#endif
	}

	void deallocate(Type* p, size_type n)
	{
#if STRAVA_WINDOWS
		::_aligned_free(p);
#else
#error
#endif
	}
};

template<class Type1, Size k_alignment1, class Type2, Size k_alignment2>
inline bool operator==(const DefaultAlignedAllocator<Type1, k_alignment1>&, const DefaultAlignedAllocator<Type2, k_alignment2>&)
{
	return std::is_same_v<Type1, Type2> && alignment1 == alignment2;
}

template<class Type1, Size k_alignment1, class Type2, Size k_alignment2>
inline bool operator!=(const DefaultAlignedAllocator<Type1, k_alignment1>&, const DefaultAlignedAllocator<Type2, k_alignment2>&)
{
	return !(lhs == rhs);
}
}
