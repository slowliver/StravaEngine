#include <Windows.h>

#include <Engine/Core/StringUtility.h>

namespace StravaEngine::Core
{
String StringUtility::ToString(wchar_t* const src, Size srcLength)
{
	if (!src)
	{
		return String(u8"");
	}

	if (srcLength > INT_MAX)
	{
		return String(u8"");
	}

	if (srcLength == 0)
	{
		srcLength = std::wcslen(src);
	}

	int length = ::WideCharToMultiByte(CP_UTF8, 0, src, static_cast<int>(srcLength), 0, 0, nullptr, nullptr);
	String dst = String(length, 0);
	::WideCharToMultiByte(CP_UTF8, 0, src, static_cast<int>(srcLength), reinterpret_cast<char*>(dst.data()), length, nullptr, nullptr);
//		output_buffer[length] = '\0';
//	if (out_length) { *out_length = length; }
	return dst;
}
}
