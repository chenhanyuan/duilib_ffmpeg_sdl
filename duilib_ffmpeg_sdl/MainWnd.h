#pragma once

#include <UIlib.h>
using namespace DuiLib;

#define WM_USER_PLAYING         WM_USER + 1     // 开始播放文件
#define WM_USER_POS_CHANGED     WM_USER + 2     // 文件播放位置改变
#define WM_USER_END_REACHED     WM_USER + 3     // 播放完毕

class CMainWnd :public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);

	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void Notify(TNotifyUI& msg);

	void AdaptWindowSize(UINT cxScreen);

	void OnDisplayChange(HWND hwnd, UINT bitsPerPixel, UINT cxScreen, UINT cyScreen);
	void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);

	LRESULT OnPlaying(HWND hwnd, WPARAM wParam, LPARAM lParam);     // 文件头读取完毕，开始播放
	LRESULT OnPosChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);  // 进度改变，播放器传回来的进度
	LRESULT OnEndReached(HWND hwnd, WPARAM wParam, LPARAM lParam);  // 文件播放完毕

private:
	//CVideoPlayer m_player;

	WINDOWPLACEMENT m_OldWndPlacement;
};

