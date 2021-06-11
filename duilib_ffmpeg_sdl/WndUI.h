#pragma once

#include <UIlib.h>
using namespace DuiLib;

class CWndUI :public CControlUI
{
public:
	CWndUI(void);
	~CWndUI(void);

	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true);
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
	bool Attach(HWND hWnd);
	HWND Detach();
	HWND GetHWnd();


protected:
	HWND m_hWnd;
};
