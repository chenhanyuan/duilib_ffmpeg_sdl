#pragma once

#include <UIlib.h>
using namespace DuiLib;

#define WM_USER_PLAYING         WM_USER + 1     // ��ʼ�����ļ�
#define WM_USER_POS_CHANGED     WM_USER + 2     // �ļ�����λ�øı�
#define WM_USER_END_REACHED     WM_USER + 3     // �������

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

	LRESULT OnPlaying(HWND hwnd, WPARAM wParam, LPARAM lParam);     // �ļ�ͷ��ȡ��ϣ���ʼ����
	LRESULT OnPosChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);  // ���ȸı䣬�������������Ľ���
	LRESULT OnEndReached(HWND hwnd, WPARAM wParam, LPARAM lParam);  // �ļ��������

private:
	//CVideoPlayer m_player;

	WINDOWPLACEMENT m_OldWndPlacement;
};

