#include <Windows.h> 
#include "MainWnd.h"

int WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	CMainWnd mainWnd;
	mainWnd.Create(NULL, L"DuiMainWnd", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);

	mainWnd.CenterWindow();
	mainWnd.ShowModal();

	return 0;
}