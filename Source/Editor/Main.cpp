#include <stdio.h>

#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return MessageBoxA(NULL, "hello, world", "caption", 0);
}