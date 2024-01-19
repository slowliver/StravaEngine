#include <string>
#include <cstdio>

//#include "./../../../External/Microsoft/DirectXShaderCompiler/inc/dxcapi.h"

#if defined(_MSC_VER )
#pragma warning(disable : 4996)
#pragma warning(disable : 6387)
#endif

static FILE* g_input = nullptr;
static FILE* g_output = nullptr;

static void CloseFile(FILE*& file)
{
	if (file)
	{
		std::fclose(file);
		file = nullptr;
	}
}

static void OnError(const char* error)
{
	CloseFile(g_input);
	CloseFile(g_output);
	std::fprintf(stderr, error);
	std::fprintf(stderr, "\n");
	std::exit(-1);
}

[[noreturn]]
static void WriteString(const char* str)
{
	if (std::fputs(str, g_output) < 0)
	{
		OnError("Failed to write to file.");
	}
}

int main(int argc, const char* argv[])
{
	// Example.
	// ShaderBuilder -o <output.cpp> -t <target> -e <entry-point> <input.shader>
	// -t <target>	Target shader type.
	// 	<target>: vs, ps, hs, ds, gs, cs
	// -sm <model>	Target shader model (default 6.6).
	// 	<model>: 6.5, 6.6, 6.7
	if (argc < 2)
	{
		OnError("There is no input file.");
	}

	std::string inputFilePath = argv[argc - 1];
	g_input = std::fopen(inputFilePath.c_str(), "rb");
	if (!g_input)
	{
		OnError("Error.");
	}

	std::string outputFilePath = inputFilePath + ".output.cpp";
	g_output = std::fopen(outputFilePath.c_str(), "w+");
	if (!g_output)
	{
		OnError("Error.");
	}

#define LINE(text) text "\n"
	WriteString
	(
		LINE("#include <Engine/Core/CoreMinimul.h>")
		LINE("")
		LINE("namespace StravaEngine")
		LINE("{")
		LINE("extern \"C\" Byte g_data[] = \n{")
	);
#undef LINE

	static constexpr int k_numColumns = 8;
	for (std::uint32_t i = 0;; ++i)
	{
		char fs[32] = {};
		int c = std::fgetc(g_input);
		if (c == EOF)
		{
			break;
		}
		if (i % k_numColumns == 0)
		{
			WriteString(i != 0 ? "\n\t" : "\t");
		}
		std::sprintf(fs, "(Byte)0x%02x,", c);
		WriteString(fs);
		if (i % k_numColumns != k_numColumns - 1)
		{
			WriteString(" ");
		}
	}

	WriteString("\n};\n} // namespace StravaEngine");

	CloseFile(g_input);
	CloseFile(g_output);

	return 0;
}
