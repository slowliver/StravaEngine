#pragma once

#include <type_traits>

#include <Engine/Core/CoreMinimal.h>

#if STRAVA_WINDOWS
#include <Windows.h>
#endif

namespace StravaEngine::Core
{
namespace Internal
{
template<class Type, Size k_alignment>
struct Allocator
{
	using value_type = Type;
	using propagate_on_container_move_assignment = std::true_type;
	using size_type = Size;
	using difference_type = std::ptrdiff_t;
	using is_always_equal = std::true_type;

	Allocator() {};
	Allocator(const Allocator&) {};
	Allocator(Allocator&&) {};
#if 0
	template<typename OtherType, Size k_alignment2>
	Allocator(const Allocator<OtherType, k_alignment2>&) {};
#endif
	~Allocator() {};

	Allocator& operator=(const Allocator&) {};
#if 0
	template<typename OtherType, Size k_alignment2>
	Allocator& operator=(const Allocator<OtherType, k_alignment2>&) {};
#endif

	[[nodiscard]]
	Type* Allocate(Size count)
	{
#if STRAVA_WINDOWS
		return static_cast<Type*>(::_aligned_malloc(count * sizeof(Type), k_alignment));
#else
#error
#endif
	}

	void Deallocate(Type* pointer)
	{
#if STRAVA_WINDOWS
		::_aligned_free(pointer);
#else
#error
#endif
	}
};

template<class Type1, Size k_alignment1, class Type2, Size k_alignment2>
inline bool operator==(const Allocator<Type1, k_alignment1>&, const Allocator<Type2, k_alignment2>&)
{
	return std::is_same_v<Type1, Type2>&& k_alignment1 == k_alignment2;
}

template<class Type1, Size k_alignment1, class Type2, Size k_alignment2>
inline bool operator!=(const Allocator<Type1, k_alignment1>& lhs, const Allocator<Type2, k_alignment2>& rhs)
{
	return !(lhs == rhs);
}
}

template<class Type, Size k_alignment = alignof(Type) < alignof(void*) ? alignof(void*) : alignof(Type)>
using Allocator = Internal::Allocator<Type, k_alignment>;
}
