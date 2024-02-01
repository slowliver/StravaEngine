// Tool.bin2cpp.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <cstdio>
#include <cinttypes>
#include <cstring>

#include <xxhash_cx.h>

int main(int argc, const char* argv[])
{
	constexpr const char* k_default = "";

	const char* data = k_default;
	std::size_t length = 0;

	if (argc > 1)
	{
		data = argv[1];
		length = std::strlen(argv[1]);
	}

	std::uint64_t hash = xxhash::xxh64(data, length, 0);
	std::printf("%" PRIu64, hash);

	return 0;
}
