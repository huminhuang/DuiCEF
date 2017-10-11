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
	//LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	//styleValue &= ~WS_CAPTION;
	//::SetWindowLong(*this, GWL_STYLE, styleValue|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

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

LRESULT CWndSimpleFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;

default:
	bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}