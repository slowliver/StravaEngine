#include <string>
#include <cstdio>
#include <memory>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <windows.h>
#include <wrl/client.h>

#include "./../../../External/Microsoft/DirectXShaderCompiler/inc/dxcapi.h"

#if defined(_MSC_VER )
#pragma warning(disable : 4996)
#pragma warning(disable : 6387)
#endif

static void OnError(const char* error)
{
	std::fprintf(stderr, error);
	std::fprintf(stderr, "\n");
	std::exit(-1);
}

static void CheckResult(HRESULT hr)
{
	if (FAILED(hr))
	{
		OnError("Failed to process.");
	}
}

static std::wstring GetWideString(const char* multibyteString)
{
	std::wstring wideString;
	const auto count = ::MultiByteToWideChar
	(
		CP_ACP,
		0,
		multibyteString,
		-1,
		NULL,
		0
	);
	wideString.resize(count);
	::MultiByteToWideChar
	(
		CP_ACP,
		0,
		multibyteString,
		-1,
		&wideString[0],
		count
	);
	return wideString;
}

enum class Stage
{
	Unknown,
	Vertex,
	Pixel,
	Hull,
	Domain,
	Geometory,
	Compute,
	Count
};

enum class ShaderModel
{
	Unknonw,
	k6_5,
	k6_6,
	Count
};

struct ShaderCompileArguments
{
	std::string m_inputPathString;
	std::string m_outputPathString;
	std::vector<std::pair<Stage, std::string>> m_stageAndEntryNameStrings;
	std::string m_shaderModelString = "6.5";

	const std::string& GetInputPath() const
	{
		return m_inputPathString;
	}

	const std::string& GetOutputPath() const
	{
		return m_outputPathString;
	}

	const std::vector<std::pair<Stage, std::string>>& GetStageAndEntryNameStrings() const
	{
		return m_stageAndEntryNameStrings;
	}

	ShaderModel GetShaderModel() const
	{
		if
		(
			std::strcmp(m_shaderModelString.c_str(), "6.5") == 0 ||
			std::strcmp(m_shaderModelString.c_str(), "6_5") == 0
		)
		{
			return ShaderModel::k6_5;
		}
		else if
		(
			std::strcmp(m_shaderModelString.c_str(), "6.6") == 0 ||
			std::strcmp(m_shaderModelString.c_str(), "6_6") == 0
		)
		{
			return ShaderModel::k6_6;
		}
		return ShaderModel::Unknonw;
	}
};

int main(int argc, const char* argv[])
{
	using Microsoft::WRL::ComPtr;

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

	ShaderCompileArguments shaderCompileArguments;

	for (size_t i = 1; i < argc; ++i)
	{
		auto keyAndValue = getKeyAndValue(argv[i]);
		const auto& key = keyAndValue.first;
		const auto& value = keyAndValue.second;

		if (i < argc - 1)
		{
			if (key == "--output")
			{
				shaderCompileArguments.m_outputPathString = value;
			}
			else if (key == "--vs-entry-name")
			{
				shaderCompileArguments.m_stageAndEntryNameStrings.emplace_back(Stage::Vertex, value);
			}
			else if (key == "--ps-entry-name")
			{
				shaderCompileArguments.m_stageAndEntryNameStrings.emplace_back(Stage::Pixel, value);
			}
			else if (key == "--hs-entry-name")
			{
				shaderCompileArguments.m_stageAndEntryNameStrings.emplace_back(Stage::Hull, value);
			}
			else if (key == "--ds-entry-name")
			{
				shaderCompileArguments.m_stageAndEntryNameStrings.emplace_back(Stage::Domain, value);
			}
			else if (key == "--gs-entry-name")
			{
				shaderCompileArguments.m_stageAndEntryNameStrings.emplace_back(Stage::Geometory, value);
			}
			else if (key == "--cs-entry-name")
			{
				shaderCompileArguments.m_stageAndEntryNameStrings.emplace_back(Stage::Compute, value);
			}
			else if (key == "--shader-model")
			{
				shaderCompileArguments.m_shaderModelString = value;
			}
		}
		else
		{
			if (!key.starts_with("--"))
			{
				shaderCompileArguments.m_inputPathString = key;
			}
		}
	}

	if (!std::filesystem::exists(std::filesystem::path(shaderCompileArguments.GetInputPath())))
	{
		std::string error = "Input file (" + shaderCompileArguments.GetInputPath() + ") does not exist.";
		OnError(error.c_str());
	}

	HRESULT hr = {};
	ComPtr<IDxcUtils> dxcUtils;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
	CheckResult(hr);

	uint32_t codePage = CP_UTF8;
	ComPtr<IDxcBlobEncoding> source;
	hr = dxcUtils->LoadFile(GetWideString(shaderCompileArguments.GetInputPath().data()).data(), &codePage, source.GetAddressOf());
	CheckResult(hr);

	ComPtr<IDxcCompiler3> dxcCompiler3;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler3.GetAddressOf()));
	CheckResult(hr);

	DxcBuffer dxcBuffer = {};
	dxcBuffer.Ptr = source->GetBufferPointer();
	dxcBuffer.Size = source->GetBufferSize();
	dxcBuffer.Encoding = codePage;

	struct DXCCompilerArguments
	{
		std::wstring m_entryName;
		std::wstring m_profile;
		std::wstring m_stage;
		std::wstring m_shaderModel;

		void SetEntryName(std::string entryName)
		{
			m_entryName = GetWideString(entryName.c_str());
		}

		void SetStage(Stage stage)
		{
			switch (stage)
			{
			case Stage::Vertex:
				m_stage = L"vs";
				break;
			case Stage::Pixel:
				m_stage = L"ps";
				break;
			case Stage::Hull:
				m_stage = L"hs";
				break;
			case Stage::Domain:
				m_stage = L"ds";
				break;
			case Stage::Geometory:
				m_stage = L"gs";
				break;
			case Stage::Compute:
				m_stage = L"cs";
				break;
			}
		};

		void SetShaderModel(ShaderModel shaderModel)
		{
			switch (shaderModel)
			{
			case ShaderModel::k6_5:
				m_shaderModel = L"6_5";
				break;
			case ShaderModel::k6_6:
				m_shaderModel = L"6_6";
				break;
			}
		}

		std::vector<const wchar_t*> BuildArguments()
		{
			std::vector<const wchar_t*> arguments;

			arguments.push_back(L"-E");
			arguments.push_back(m_entryName.c_str());

			m_profile = m_stage + L"_" + m_shaderModel;
			arguments.push_back(L"-T");
			arguments.push_back(m_profile.c_str());

//			arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
//			arguments.push_back(DXC_ARG_DEBUG); //-Zi
			return arguments;
		}
	};

	DXCCompilerArguments dxcCompilerArguments;
	dxcCompilerArguments.SetShaderModel(shaderCompileArguments.GetShaderModel());

	::MessageBoxA(nullptr, "Test", "Test", MB_OK);

	for (const auto& stageAndEntryNameString : shaderCompileArguments.GetStageAndEntryNameStrings())
	{
		dxcCompilerArguments.SetEntryName(stageAndEntryNameString.second);
		dxcCompilerArguments.SetStage(stageAndEntryNameString.first);
#if 0
		// Strip reflection data and pdbs (see later)
		arguments.push_back(L"-Qstrip_debug");
		arguments.push_back(L"-Qstrip_reflect");
#endif
		auto buildArguments = dxcCompilerArguments.BuildArguments();

		ComPtr<IDxcResult> dxcCompileResult;
		hr = dxcCompiler3->Compile(&dxcBuffer, buildArguments.data(), static_cast<uint32_t>(buildArguments.size()), nullptr, IID_PPV_ARGS(dxcCompileResult.GetAddressOf()));
		CheckResult(hr);

		ComPtr<IDxcBlob> dxil;
		hr = dxcCompileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(dxil.GetAddressOf()), nullptr);
		if (SUCCEEDED(hr) && dxil && dxil->GetBufferSize() > 0)
		{
			std::string outputFilePath = shaderCompileArguments.GetInputPath() + "." + stageAndEntryNameString.second + ".cso";
			FILE* output = nullptr;
			output = std::fopen(outputFilePath.c_str(), "wb+");
			if (!output)
			{
				OnError("Error.");
			}
			std::fwrite(dxil->GetBufferPointer(), 1, dxil->GetBufferSize(), output);
			std::fclose(output);
		}
		else
		{
			ComPtr<IDxcBlobUtf8> error;
			hr = dxcCompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(error.GetAddressOf()), nullptr);
			if (SUCCEEDED(hr) && error && error->GetStringLength() > 0)
			{
				OnError(error->GetStringPointer());
			}
		}
	}

	return 0;
}
