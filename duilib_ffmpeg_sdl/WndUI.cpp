#include "StdAfx.h"
#include "WndUI.h"


CWndUI::CWndUI(void) :
	m_hWnd(NULL)
{
}


CWndUI::~CWndUI(void)
{
}

void CWndUI::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);
	::ShowWindow(m_hWnd, bVisible);
}

void CWndUI::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);
	::ShowWindow(m_hWnd, bVisible);
}

void CWndUI::SetPos(RECT rc, bool bNeedInvalidate)
{
	__super::SetPos(rc);
	::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
}

bool CWndUI::Attach(HWND hWnd)
{
	if (!::IsWindow(hWnd))
		return false;

	m_hWnd = hWnd;
	return true;
}

HWND CWndUI::Detach()
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;
	return hWnd;
}
HWND CWndUI::GetHWnd()
{
	return m_hWnd;
}