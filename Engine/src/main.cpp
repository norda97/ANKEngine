#include "pch.h"
#include "Core/WinWrapper.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#ifdef TS_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	HWND hWnd;

	WinWrapper::init(hInstance, nCmdShow, &hWnd, 500, 400, L"TechniqueSandbox");

	while (WinWrapper::handleMessage())
	{

	}

	return 0;
}
