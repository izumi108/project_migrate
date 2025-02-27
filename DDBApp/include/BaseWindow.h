#ifndef BaseWindow_h__
#define BaseWindow_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	BaseWindow.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-03-20 14:03:25
// 说明			:	消息中心消息泵窗口
/////////////////////////////////////////////////////////////*/
#pragma once
#include <Windows.h>

class SINGLETON_API BaseWindow
{
public:
	BaseWindow(void);
	~BaseWindow(void);
public:

	HWND Create(HWND hwndParent, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT);

	LONG ModifyStyle(DWORD dwStyleAdd,DWORD dwStyleRemove = 0);
	LONG ModifyExStyle(DWORD dwExStyleAdd,DWORD dwExStyleRemove = 0);

	HWND Subclass(HWND hWnd);
	void Unsubclass();

	HINSTANCE GetHinstance();
	void SetHinstance(HINSTANCE hInstance);

	HWND GetHWND() const;
	operator HWND() const;

	HDC GetPaintDC() const;
	operator HDC() const;

	virtual void ShowWindow(int nCmdShow = SW_SHOW);
	virtual void CloseWindow(UINT nRet = IDOK);

	virtual UINT DoModal();
	virtual void EndModal(UINT nRet = IDOK);

	virtual BOOL MoveWindow(int x,int y,int width = -1, int height = -1,BOOL bRepaint= FALSE);

	virtual BOOL DestroyWindow();

	void CenterWindow();	// 居中，支持扩展屏幕
	void SetSmallIcon(HICON hSmallIcon);
	void SetLargeIcon(HICON hLargeIcon);

	LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	void ResizeClient(int cx = -1, int cy = -1);

protected:
	bool RegisterWindowClass();		// 用于Window
	bool RegisterSuperclass();			// 用于系统标准控件

	virtual LPCTSTR GetWindowClassName() const;		// 用于Window
	virtual LPCTSTR GetSuperClassName() const;		// 用于系统控件
	virtual UINT GetClassStyle() const;
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnFinalMessage(HWND hWnd);

	// 注册的窗口消息循环
	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// 注册的控件消息循环
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	HWND m_hWnd;
	HDC m_hPaintDC;
	HINSTANCE m_hInstance;
	WNDPROC m_OldWndProc;
	bool m_bSubclassed;
	bool m_bIsAutoDelete;
	bool m_bIsDoModal;
#ifdef _DEBUG
	DWORD m_dwRunningThread;
#endif // _DEBUG
};
#endif // BaseWindow_h__
