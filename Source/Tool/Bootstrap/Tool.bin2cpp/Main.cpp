// Tool.bin2cpp.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

static void PrintError(const char* error)
{
	std::cerr << error << std::endl;
	return;
}

int main(int argc, const char* argv[])
{
	// Example.
	// bin2cpp -o <output.cpp> -eh <output.h> -vn <variable_name> <input.bin>
	if (argc < 2)
	{
		PrintError("There is no input file.");
		return 0xFFFFFFFF;
	}

	std::string inputFilePath = argv[argc - 1];
	std::ifstream inputFileStream(inputFilePath, std::ios_base::in | std::ios_base::binary);
	if (!inputFileStream)
	{
		PrintError("Error.");
		return 0xFFFFFFFF;
	}

	std::string outputFilePath = inputFilePath + ".output.cpp";
	std::ofstream outputFileStream(inputFilePath, std::ios_base::out);
	if (!outputFileStream)
	{
		PrintError("Error.");
		return 0xFFFFFFFF;
	}

	outputFileStream << "extern \"C\" Byte g_data[] = \n{ ";
	const auto outputFileStreamFlags = outputFileStream.flags();
	outputFileStream.setf(std::ios::hex, std::ios::basefield);
	outputFileStream.fill('0');
	char chr = 0;
	for (std::uint32_t i = 0; inputFileStream.get(chr); ++i)
	{
		outputFileStream << "(Byte)0x" << std::setw(2) << chr << ", ";
	}
	std::cout.flags(outputFileStreamFlags);
	outputFileStream << "};" << std::endl;

//	std::cout << inputFilePath << std::endl;
	return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
