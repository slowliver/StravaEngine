#pragma once

#include <Engine/Core/Allocator.h>

#if ST_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace StravaEngine::Core
{
void* Allocator::AlignedAllocate(Size size, Size align)
{
#if ST_PLATFORM_WINDOWS
	return ::_aligned_malloc(size, align);
#else
#error
#endif
}

void* Allocator::Allocate(Size size)
{
	return AlignedAllocate(size, sizeof(void*));
}

void Allocator::Free(void* pointer)
{
#if ST_PLATFORM_WINDOWS
	::_aligned_free(pointer);
#else
#error
#endif
}
}

#if 0
void* operator new(Size size)
{
	return StravaEngine::Core::Allocator::Allocate(size);
}
#endif
