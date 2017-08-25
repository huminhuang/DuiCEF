#include "stdafx.h"

#include "WndSimpleFrame.h"

//////////////////////////////////////////////////////////////////////////
CWndSimpleFrame::CWndSimpleFrame(void)
{
}

CWndSimpleFrame::~CWndSimpleFrame(void)
{

}

void CWndSimpleFrame::OnFinalMessage(HWND /*hWnd*/)
{
	PostQuitMessage(IDOK);

	delete this;
}

void CWndSimpleFrame::Init()
{

}

void CWndSimpleFrame::OnPrepare()
{
	
}

void CWndSimpleFrame::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit"))
		OnPrepare();
	else if (msg.sType == _T("click"))
	{
		LPCTSTR pszCtrlName = msg.pSender->GetName().GetData();
		if (_tcscmp(pszCtrlName, _T("btn_sysmin")) == 0)
		{
			OnBtnMin();
		}
		else if (_tcscmp(pszCtrlName, _T("btn_sysclose")) == 0)
		{
			OnBtnClose();
		}
	}
}

void CWndSimpleFrame::OnBtnMin()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CWndSimpleFrame::OnBtnClose()
{
	Close(IDOK);
}

LRESULT CWndSimpleFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	m_pm.Init( m_hWnd);

	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T("WndSimpleFrame.xml"), (UINT)0, &cb, &m_pm);
	ASSERT(pRoot);
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CWndSimpleFrame::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::IsIconic(*this)) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CWndSimpleFrame::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if (!::IsZoomed(*this)) {
		RECT rcSizeBox = m_pm.GetSizeBox();
		if (pt.y < rcClient.top + rcSizeBox.top) {
			if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
			return HTTOP;
		}
		else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
			if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
		if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
	}

	RECT rcCaption = m_pm.GetCaptionRect();
	if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
		if (pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0)
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CWndSimpleFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{ 
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) 
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		//rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left-1, rcWnd.top-1, rcWnd.right+1, rcWnd.bottom+1, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
		 
	}
	
	bHandled = FALSE;
	return 0;
}

LRESULT CWndSimpleFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;

default:
	bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}