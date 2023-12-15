#include <stdio.h>

#include <windows.h>

#include <stdio.h>

#include <Engine/Core/Array.h>
#include <Engine/Core/ArrayList.h>
#include <Engine/Framework/Application.h>
#include <Engine/Core/Diagnostics/Debugger.h>

#include <intrin.h>

using namespace StravaEngine;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdline, int cmdshow)
{
	struct TestStruct
	{
		int a = 0;
		int b = 121;
		float c = 1024.0f;
		std::string str;
	};

	struct Test
	{
		Test() {
			::OutputDebugStringA("Constructed!");
		}
		~Test() {
			::OutputDebugStringA("Destructed!");
		}
		int a[4];
	};

	Core::ArrayList<TestStruct> list2;
	TestStruct test = { 0, 121, 19.0f, "TestTestTest" };
	list2.EmplaceBack(9, 4, 204.0f, "Hoge");
	list2.EmplaceBack(10, 2, 104.0f, "Hoge2");
	list2.EmplaceBack(11, 6, 304.0f, "Hoge3");
	list2.EmplaceBack(1, 7, 4.0f, "Hoge4");
	list2.Add(test);

	std::vector<TestStruct> list3;
	for (int i = 0; i < 10; ++i)
	{
		TestStruct t;
		t.a = i;
		t.b = i * 2;
		t.c = (float)i * 3;
		t.str = "Hogehoge";
		list3.push_back(t);
	}

	list2.InsertRange(2, list3.begin(), list3.end());

	std::vector<Test> list4;
	list4.push_back(Test());
	list4.push_back(Test());
	list4.push_back(Test());
	list4.insert((list4.begin() + 1), Test());

	STRAVA_ASSERT(0);

	Framework::ApplicationSpec spec;
	spec.m_nativeHandle = (NativeHandle)hInstance;
	spec.m_title = "Title";

	auto application = Framework::Application(spec);

	// Return this part of the WM_QUIT message to Windows.
	return 0;
}