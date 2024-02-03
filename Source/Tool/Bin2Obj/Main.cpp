// Tool.bin2cpp.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <string>
#include <algorithm>
#include <filesystem>
#include <cstdio>

#include <windows.h>

#if defined(_MSC_VER )
#pragma warning(disable : 4996)
#pragma warning(disable : 6387)
#endif

static void OnError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::vfprintf(stderr, format, args);
	va_end(args);
	std::fprintf(stderr, "\n");
	std::exit(-1);
}

struct Bin2ObjArguments
{
	std::string m_inputPathString;
	std::string m_outputDirectoryString;
	std::string m_variableNameString = "data";
	bool m_suppressObj = false;

	const std::string& GetInputPath() const { return m_inputPathString; }

	const std::string GetInputFilename() const
	{
		std::string filename = std::string(m_inputPathString);
		std::replace(filename.begin(), filename.end(), '\\', '/');
		filename = filename.substr(filename.find_last_of('/') + 1);
		filename = filename.substr(0, filename.find_last_of('.'));
		return filename;
	}

	const std::string GetOutputDirectory() const
	{
		return std::filesystem::path(std::filesystem::weakly_canonical(m_outputDirectoryString).string() + "/").make_preferred().string();
	}

	const std::string& GetVariableName() const { return m_variableNameString; }

	bool GetSuppressObj() const { return m_suppressObj; }
};

int main(int argc, const char* argv[])
{
	// Example.
	// bin2cpp -o <output.cpp> -eh <output.h> -vn <variable_name> <input.bin>
	if (argc < 2)
	{
		OnError("There is no input file.");
	}

	auto getKeyAndValue = [=](std::string arg) -> std::pair<std::string, std::string>
	{
		std::pair<std::string, std::string> keyAndValue;
		auto& key = keyAndValue.first;
		auto& value = keyAndValue.second;

		// Key
		key.resize(arg.size());
		std::transform(arg.cbegin(), arg.cend(), key.begin(), tolower);
		const auto s = arg.find_first_of('=');
		key = std::string(key, 0, s);

		// Value
		if (s != std::string::npos)
		{
			value = std::string(arg, s + 1);
		}

		return keyAndValue;
	};

	Bin2ObjArguments bin2ObjArguments;

	for (size_t i = 1; i < argc; ++i)
	{
		auto keyAndValue = getKeyAndValue(argv[i]);
		const auto& key = keyAndValue.first;
		const auto& value = keyAndValue.second;

		if (i < argc - 1)
		{
			if (key == "--output-directory")
			{
				bin2ObjArguments.m_outputDirectoryString = value.ends_with("/") ? value : value + "/";
			}
			else if (key == "--variable-name")
			{
				bin2ObjArguments.m_variableNameString = value;
			}
			else if (key == "--suppress-obj")
			{
				bin2ObjArguments.m_suppressObj = true;
			}
		}
		else
		{
			if (!key.starts_with("--"))
			{
				bin2ObjArguments.m_inputPathString = key;
			}
		}
	}

	auto verifyVariableName = [](const std::string& variableName) -> bool
	{
		for (size_t i = 0; i < variableName.size(); ++i)
		{
			auto c = variableName.data()[i];
			if (c >= '0' && c <= '9')
			{
				continue;
			}
			else if (c >= 'A' && c <= 'Z')
			{
				continue;
			}
			else if (c == '_')
			{
				continue;
			}
			else if (c >= 'a' && c <= 'z')
			{
				continue;
			}
			return false;
		}
		return true;
	};
	if (!verifyVariableName(bin2ObjArguments.GetVariableName()))
	{
		OnError("The variable name (%s) is an illegal format.", bin2ObjArguments.GetVariableName().c_str());
	}

	struct ScopedFILEPtr
	{
		ScopedFILEPtr() = delete;
		explicit ScopedFILEPtr(FILE* file) : m_file(file) {}
		~ScopedFILEPtr() { if (m_file) { std::fclose(m_file); } }
		bool IsValid() const { return m_file != nullptr; }
		FILE* operator*() const { return m_file; }
		FILE* m_file = nullptr;
	};

	ScopedFILEPtr input = ScopedFILEPtr(std::fopen(bin2ObjArguments.GetInputPath().c_str(), "rb"));
	if (!input.IsValid())
	{
		OnError("Input file (%s) is not found.", bin2ObjArguments.GetInputPath().c_str());
	}

	if (!std::filesystem::exists(bin2ObjArguments.GetOutputDirectory()))
	{
		std::filesystem::create_directories(bin2ObjArguments.GetOutputDirectory());
	}

	std::string cppPath = bin2ObjArguments.GetOutputDirectory() + bin2ObjArguments.GetInputFilename() + ".cpp";

	// Write cpp file.
	{
		ScopedFILEPtr cppOutput = ScopedFILEPtr(std::fopen(cppPath.c_str(), "w+"));
		if (!cppOutput.IsValid())
		{
			OnError("Output file (%s) cannot create.", cppPath.c_str());
		}

		auto writeString = [&cppPath,&cppOutput](const char* str)
		{
			if (std::fputs(str, *cppOutput) < 0)
			{
				OnError("Failed to write to file (%s).", cppPath.c_str());
			}
		};
		writeString
		(
			"namespace StravaEngine\n"
			"{\n"
			"extern \"C\" unsigned char g_"
		);
		writeString(bin2ObjArguments.GetVariableName().c_str());
		writeString("[] = \n{\n");

		static constexpr int k_numColumns = 8;
		for (std::uint32_t i = 0;; ++i)
		{
			char fs[32] = {};
			int c = std::fgetc(*input);
			if (c == EOF)
			{
				break;
			}
			if (i % k_numColumns == 0)
			{
				writeString(i != 0 ? "\n\t" : "\t");
			}
			std::sprintf(fs, "0x%02x,", c);
			writeString(fs);
			if (i % k_numColumns != k_numColumns - 1)
			{
				writeString(" ");
			}
		}

		writeString("\n};\n} // namespace StravaEngine");
	}

	// Write obj file.
	if (!bin2ObjArguments.GetSuppressObj())
	{
		char pathToExe[MAX_PATH];
		if (!GetModuleFileNameA(NULL, pathToExe, MAX_PATH))
		{
			OnError("Cannot detect current Bin2Obj path.");
		}

		std::string pathToCL = std::string(pathToExe) + "\\..\\..\\..\\Package\\Microsoft\\MSVC\\cl.exe";
		pathToCL = std::filesystem::path(std::filesystem::weakly_canonical(pathToCL)).make_preferred().string();
		if (!std::filesystem::exists(pathToCL))
		{
			OnError("%s is not found.", pathToCL.c_str());
		}

		SECURITY_ATTRIBUTES securityAttributes = {};
		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.lpSecurityDescriptor = NULL;
		securityAttributes.bInheritHandle = TRUE;
		HANDLE readPipeHandle = {};
		HANDLE writePipeHandle = {};
		if (!::CreatePipe(&readPipeHandle, &writePipeHandle, &securityAttributes, 0))
		{
			OnError("A pipe creation failed.");
		}

		STARTUPINFOA startupinfo = {};
		startupinfo.cb = sizeof(STARTUPINFOA);
		startupinfo.dwFlags = STARTF_USESTDHANDLES;
		startupinfo.hStdOutput = writePipeHandle;
		startupinfo.hStdError = writePipeHandle;
		PROCESS_INFORMATION processInformation = {};

		std::string objPath = bin2ObjArguments.GetOutputDirectory() + bin2ObjArguments.GetInputFilename() + ".obj";

		std::string clArgumentsString = "";
		clArgumentsString += "\"" + pathToCL + "\" ";
		clArgumentsString += "/O2 /std:c++20 /nologo /c ";
		clArgumentsString += "/Fo\"" + objPath + "\" ";
		clArgumentsString += "\"" + cppPath + "\"";

		char* clArguments = static_cast<char*>(alloca(clArgumentsString.size() + 1));
		std::strcpy(clArguments, clArgumentsString.c_str());
		auto processSuccessed = CreateProcessA
		(
			NULL,                 // 実行可能モジュールの名前
			clArguments,                      // コマンドラインの文字列
			NULL, // セキュリティ記述子
			NULL,  // セキュリティ記述子
			FALSE,                      // ハンドルの継承オプション
			0,                     // 作成のフラグ
			NULL,                      // 新しい環境ブロック
			NULL,                // カレントディレクトリの名前
			&startupinfo,               // スタートアップ情報
			&processInformation // プロセス情報
		);

		if (!processSuccessed)
		{
			OnError("%s cannot start.", pathToCL.c_str());
		}

		WaitForSingleObject(processInformation.hProcess, INFINITE);

		DWORD exitCode;
		GetExitCodeProcess(processInformation.hProcess, &exitCode);
		if (exitCode != 0)
		{
			OnError("An error occurred with writing an .obj file.");
		}

		// ハンドルを閉じる
		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);
	}

	return 0;
}
