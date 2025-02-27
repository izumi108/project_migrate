#include "stdafx.h"
#include "BaseWindow.h"
#include <assert.h>
#include <WindowsX.h>

BaseWindow::BaseWindow(void)
	: m_bSubclassed(false)
	, m_bIsAutoDelete(false)
	, m_OldWndProc(::DefWindowProc)
	, m_hWnd(NULL)
	, m_hPaintDC(NULL)
	, m_bIsDoModal(false)
{
}


BaseWindow::~BaseWindow(void)
{
}

HWND BaseWindow::Create(HWND hwndParent, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle, int x /*= CW_USEDEFAULT*/, int y /*= CW_USEDEFAULT*/, int cx /*= CW_USEDEFAULT*/, int cy /*= CW_USEDEFAULT*/)
{
	if ( GetSuperClassName() != NULL && !RegisterSuperclass() )
		return NULL;
	if ( GetSuperClassName() == NULL && !RegisterWindowClass())
		return NULL;

#ifdef _DEBUG
	m_dwRunningThread = ::GetCurrentThreadId();
#endif // _DEBUG

	m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), lpszWindowName, dwStyle, x, y, cx, cy,
		hwndParent, NULL, m_hInstance, this);
	DWORD dwLastError = ::GetLastError();
	_ASSERTE(m_hWnd!=NULL);
	return m_hWnd;
}

bool BaseWindow::RegisterWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = GetClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = BaseWindow::__WndProc;
	wc.hInstance = m_hInstance;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = GetWindowClassName();
	ATOM ret = ::RegisterClass(&wc);
	_ASSERTE(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LPCTSTR BaseWindow::GetWindowClassName() const
{
	return _T("WindowWnd");
}

UINT BaseWindow::GetClassStyle() const
{
	return NULL;
}

LRESULT BaseWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::CallWindowProc(m_OldWndProc,m_hWnd,uMsg,wParam,lParam);
}

void BaseWindow::OnFinalMessage(HWND hWnd)
{

}

LRESULT CALLBACK BaseWindow::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 窗口默认消息处理函数
	BaseWindow* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		// 设置窗口实例指针到hWnd窗口上
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<BaseWindow*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		pThis->m_hPaintDC = ::GetDC(hWnd);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	} 
	else
	{
		// 非 WM_NCCREATE ，从hWnd中读取窗口实例指针
		pThis = reinterpret_cast<BaseWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
			// 如果当前消息是销毁窗口
			::ReleaseDC(hWnd,pThis->m_hPaintDC);
			::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);

			// 当前窗口是子类化的，恢复原窗口过程
			if( pThis->m_bSubclassed )
				pThis->Unsubclass();
			pThis->m_hWnd = NULL;

			// 窗口销毁完成，用户可以安全销毁窗口实例
			pThis->OnFinalMessage(hWnd);
			if ( !pThis->m_bIsDoModal && pThis->m_bIsAutoDelete )
				delete pThis;
			return lRes;
		}
	}

	if( pThis != NULL )
	{
		// 窗口消息发送到窗口实例中继续处理
		// BaseWindow::HandleMessage 为虚函数
		// CWindowUI::HandleMessage重写实现了消息处理
		return pThis->WindowProc(uMsg, wParam, lParam);
	} 
	else
	{
		// 没有找到窗口实例指针，调用默认处理
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

HWND BaseWindow::Subclass(HWND hWnd)
{
	_ASSERTE(::IsWindow(hWnd));
	_ASSERTE(m_hWnd==NULL);
	m_OldWndProc = SubclassWindow(hWnd, BaseWindow::__WndProc);
	if( m_OldWndProc == NULL )
	{
		// 子类化失败
		return NULL;
	}

	m_bSubclassed = true;
	m_hWnd = hWnd;
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
	return m_hWnd;
}

void BaseWindow::Unsubclass()
{
	_ASSERTE(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) )
		return;
	if( !m_bSubclassed )
		return;
	SubclassWindow(m_hWnd, m_OldWndProc);
	m_OldWndProc = ::DefWindowProc;
	m_bSubclassed = false;
}

HINSTANCE BaseWindow::GetHinstance()
{
	return m_hInstance;
}

void BaseWindow::SetHinstance(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}

void BaseWindow::CenterWindow()
{
	_ASSERTE(::IsWindow(m_hWnd));
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWnd=m_hWnd;
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	if (hWndCenter!=NULL)
		hWnd=hWndCenter;

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if( hWndCenter == NULL )
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if( xLeft < rcArea.left )
		xLeft = rcArea.left;
	else if( xLeft + DlgWidth > rcArea.right )
		xLeft = rcArea.right - DlgWidth;
	if( yTop < rcArea.top )
		yTop = rcArea.top;
	else if( yTop + DlgHeight > rcArea.bottom )
		yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

HWND BaseWindow::GetHWND() const
{
	return m_hWnd;
}

BaseWindow::operator HWND() const
{
	return m_hWnd;
}

void BaseWindow::ShowWindow(int nCmdShow /*= SW_SHOW*/)
{
	_ASSERTE(::IsWindow(m_hWnd));
	if ( ::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd,nCmdShow);
	}
}

void BaseWindow::CloseWindow(UINT nRet /*= IDOK*/)
{
	if (::IsWindow(m_hWnd))
	{
		::PostMessage(m_hWnd,WM_CLOSE,nRet,0);
	}
}

void BaseWindow::SetSmallIcon(HICON hSmallIcon)
{
	_ASSERTE(hSmallIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) ICON_SMALL, (LPARAM) hSmallIcon);
}

void BaseWindow::SetLargeIcon(HICON hLargeIcon)
{
	_ASSERTE(hLargeIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) ICON_BIG, (LPARAM) hLargeIcon);
}

UINT BaseWindow::DoModal()
{
	_ASSERTE(::IsWindow(m_hWnd));
	UINT nRet = 0;
	HWND hWndParent = GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);
	m_bIsDoModal = true;
	MSG msg = { 0 };
	do 
	{
		if ( !::IsWindow(m_hWnd))
			break;
		if ( ! ::GetMessage(&msg, NULL, 0, 0) )
			break;

		if ( msg.hwnd == m_hWnd && msg.message == WM_CLOSE )
		{
			::EnableWindow(hWndParent,TRUE);		// 恢复父窗口所有消息输入
			::SetFocus(hWndParent);
			nRet = msg.wParam;
		}

		if( !::TranslateMessage(&msg) )
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT );

	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);

	if( msg.message == WM_QUIT )
		::PostQuitMessage(msg.wParam);

	if ( m_bIsDoModal && m_bIsAutoDelete)
		delete this;
	return nRet;
}

void BaseWindow::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
	_ASSERTE(::IsWindow(m_hWnd));
	RECT rc = { 0 };
	if( !::GetClientRect(m_hWnd, &rc) ) return;
	if( cx != -1 ) rc.right = cx;
	if( cy != -1 ) rc.bottom = cy;
	if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd),
		(!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)),
		GetWindowExStyle(m_hWnd)) )
		return;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LONG BaseWindow::ModifyStyle(DWORD dwStyleAdd,DWORD dwStyleRemove /*= 0*/)
{
	LONG dwOldStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	LONG styleValue = dwOldStyle;
	styleValue &= ~( dwStyleRemove );
	styleValue |= (dwStyleAdd);
	LONG lRet = ::SetWindowLong(m_hWnd, GWL_STYLE, styleValue);
	return dwOldStyle;
}

LONG BaseWindow::ModifyExStyle(DWORD dwExStyleAdd,DWORD dwExStyleRemove /*= 0*/)
{
	LONG dwOldStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	LONG styleValue = dwOldStyle;
	styleValue &= ~( dwExStyleRemove );
	styleValue |= (dwExStyleAdd);
	LONG lRet = ::SetWindowLong(m_hWnd, GWL_EXSTYLE, styleValue);
	return dwOldStyle;
}

void BaseWindow::EndModal(UINT nRet /*= IDOK*/)
{
	if ( m_bIsDoModal == false)
	{
		// 警告，非模态窗口不应该用这个函数关闭窗口
		_ASSERTE(false);
		return;
	}

	if ( ::IsWindow(m_hWnd) )
	{
		::PostMessage(m_hWnd,WM_CLOSE,nRet,0);
	}
}

LRESULT BaseWindow::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/)
{
	_ASSERTE(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd,uMsg,wParam,lParam);
}

LRESULT BaseWindow::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/)
{
	_ASSERTE(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd,uMsg,wParam,lParam);
}

LPCTSTR BaseWindow::GetSuperClassName() const
{
	return NULL;
}

bool BaseWindow::RegisterSuperclass()
{
	// Get the class information from an existing
	// window so we can subclass it later on...
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	if( !::GetClassInfoEx(NULL, GetSuperClassName(), &wc) )
	{
		if( !::GetClassInfoEx(m_hInstance, GetSuperClassName(), &wc) )
		{
			_ASSERTE(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = BaseWindow::__ControlProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = GetWindowClassName();
	ATOM ret = ::RegisterClassEx(&wc);
	_ASSERTE(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK BaseWindow::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BaseWindow* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<BaseWindow*>(lpcs->lpCreateParams);
		::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
		pThis->m_hWnd = hWnd;
	} 
	else
	{
		pThis = reinterpret_cast<BaseWindow*>(::GetProp(hWnd, _T("WndX")));
		if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			if( pThis->m_bSubclassed )
				pThis->Unsubclass();
			::SetProp(hWnd, _T("WndX"), NULL);
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage(hWnd);
			return lRes;
		}
	}
	if( pThis != NULL )
	{
		return pThis->WindowProc(uMsg, wParam, lParam);
	} 
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

HDC BaseWindow::GetPaintDC() const
{
	return m_hPaintDC;
}

BaseWindow::operator HDC() const
{
	return m_hPaintDC;
}

BOOL BaseWindow::DestroyWindow()
{
	if ( ::IsWindow(m_hWnd))
		return ::DestroyWindow(m_hWnd);

	return FALSE;
}

BOOL BaseWindow::MoveWindow(int x,int y,int width /*= -1*/, int height /*= -1*/,BOOL bRepaint/*= FALSE*/)
{
	if ( !::IsWindow(m_hWnd))
		return FALSE;

	RECT rcClient;
	::GetClientRect(m_hWnd,&rcClient);

	return ::MoveWindow(m_hWnd,
		x,y, 
		width == -1 ? (rcClient.right - rcClient.left) : width ,
		height == -1 ? (rcClient.bottom - rcClient.top) : height,
		bRepaint );
}