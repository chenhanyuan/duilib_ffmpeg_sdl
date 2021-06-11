//#include "stdafx.h"
#include "MainWnd.h"
#include "WndUI.h"
#include "h264Decoder.h"

#include <thread>

void CallbackPlayer(void *data, UINT uMsg)
{
	/*CVideoPlayer *pPlayer = (CVideoPlayer*)data;

	if (pPlayer)
	{
		HWND hWnd = pPlayer->GetHWnd();

		if (::IsWindow(hWnd) && ::IsWindow(::GetParent(hWnd)))
		{
			::PostMessage(::GetParent(hWnd), uMsg, (WPARAM)data, 0);
		}
	}*/
}

void CallbackPlaying(void *data)
{
	CallbackPlayer(data, WM_USER_PLAYING);
}

void CallbackPosChanged(void *data)
{
	CallbackPlayer(data, WM_USER_POS_CHANGED);
}

void CallbackEndReached(void *data)
{
	CallbackPlayer(data, WM_USER_END_REACHED);
}


CMainWnd::CMainWnd(void)
{
}


CMainWnd::~CMainWnd(void)
{
}


LPCTSTR CMainWnd::GetWindowClassName() const
{
	return L"DuiMainWnd";
}

DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return L"MainWindow.xml";
}

DuiLib::CDuiString CMainWnd::GetSkinFolder()
{
	return L"skin";
}

CControlUI * CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("Player")) == 0)
	{
		CWndUI* pUI = new CWndUI();
		HWND hWnd = CreateWindow(_T("#32770"), _T("Player"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), (HMENU)0, NULL, NULL);
		pUI->Attach(hWnd);
		return pUI;
	}
	return NULL;
}

void CMainWnd::InitWindow()
{
	// 根据分辨率自动调节窗口大小
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	//AdaptWindowSize(oMonitor.rcMonitor.right - oMonitor.rcMonitor.left);
	::GetWindowPlacement(*this, &m_OldWndPlacement);

	CWndUI* pWnd = (CWndUI*)m_PaintManager.FindControl(_T("player"));
	if (pWnd)
	{
		//m_player.SetHWnd(pWnd->GetHWnd());
		//m_player.SetCallbackPlaying(CallbackPlaying);
		//m_player.SetCallbackPosChanged(CallbackPosChanged);
		//m_player.SetCallbackEndReached(CallbackEndReached);

		//m_PaintManager.GetInstancePath();

		////m_player.Play("D:\\code2\\code\\DuiVLC\\Debug\\Material Design Motion.mp4");
		//m_player.Play("Material Design Motion.mp4");

		std::thread work = std::thread([&]() {

			decodeFile(pWnd->GetHWnd());
		});
		work.detach();



	}
}


void CMainWnd::Notify(TNotifyUI& msg)
{
	__super::Notify(msg);
}


void CMainWnd::AdaptWindowSize(UINT cxScreen)
{
	int iX = 968, iY = 600;
	int iWidthList = 225, iWidthSearchEdit = 193;
	SIZE szFixSearchBtn = { 201, 0 };

	if (cxScreen <= 1024)      // 800*600  1024*768  
	{
		iX = 775;
		iY = 470;
	}
	else if (cxScreen <= 1280) // 1152*864  1280*800  1280*960  1280*1024
	{
		iX = 968;
		iY = 600;
	}
	else if (cxScreen <= 1366) // 1360*768 1366*768
	{
		iX = 1058;
		iY = 656;
		iWidthList += 21;
		iWidthSearchEdit += 21;
		szFixSearchBtn.cx += 21;
	}
	else                      // 1440*900
	{
		iX = 1224;
		iY = 760;
		iWidthList += 66;
		iWidthSearchEdit += 66;
		szFixSearchBtn.cx += 66;
	}

	::SetWindowPos(m_PaintManager.GetPaintWindow(), NULL, 0, 0, iX, iY, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
	CenterWindow();
}


LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);

	switch (uMsg)
	{
		HANDLE_MSG(*this, WM_DISPLAYCHANGE, OnDisplayChange);
		HANDLE_MSG(*this, WM_GETMINMAXINFO, OnGetMinMaxInfo);

	case WM_USER_PLAYING:
		return OnPlaying(*this, wParam, lParam);
	case WM_USER_POS_CHANGED:
		return OnPosChanged(*this, wParam, lParam);
	case WM_USER_END_REACHED:
		return OnEndReached(*this, wParam, lParam);

	default:
		break;
	}

	return lRes;
}

void CMainWnd::OnDisplayChange(HWND hwnd, UINT bitsPerPixel, UINT cxScreen, UINT cyScreen)
{
	AdaptWindowSize(cxScreen);
}

void CMainWnd::OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
	/*if (m_bFullScreenMode)
	{
		lpMinMaxInfo->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN) + 2 * (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER));
		lpMinMaxInfo->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN) + 2 * (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER));
	}*/
}


LRESULT CMainWnd::OnPlaying(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CMainWnd::OnPosChanged(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CMainWnd::OnEndReached(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	::MessageBox(NULL, L"播放结束...", L"结束", MB_OK);
	return TRUE;
}