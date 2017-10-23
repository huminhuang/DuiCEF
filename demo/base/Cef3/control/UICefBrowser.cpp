#include "stdafx.h"
#include "UICefBrowser.h"

CCefBrowserUI::CCefBrowserUI()
	: m_bLoadFinish(false)
	, m_pProcessMessageHandler(new CProcessMessageHandler)
	, m_pClientHandler(new CCefClientHandler(this))
{
	
}

CCefBrowserUI::~CCefBrowserUI()
{
	m_pClientHandler->Close();

	if (m_pBrowser != nullptr)
	{
		m_pClientHandler->CloseBrowser(m_pBrowser);
	}
}

void CCefBrowserUI::DoInit()
{
	m_pClientHandler->CreateBrowser(m_pManager->GetPaintWindow(), m_rcItem);
}

void CCefBrowserUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	resize();

	return CControlUI::DoPaint(hDC, rcPaint);
}

void CCefBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("url")) == 0) Navigate2(pstrValue);
	else return CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefBrowserUI::SetVisible(bool bVisible)
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetHost()->SetWindowVisibility(bVisible);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([bVisible, this]{ SetVisible(bVisible); });
	}

	return CControlUI::SetVisible(bVisible);
}

void CCefBrowserUI::SetInternVisible(bool bVisible)
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetHost()->SetWindowVisibility(bVisible);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([bVisible, this]{ SetInternVisible(bVisible); });
	}

	return CControlUI::SetInternVisible(bVisible);
}

void CCefBrowserUI::SetProcessMessageHandler(CefRefPtr<CProcessMessageHandler> pHandler)
{
	m_pProcessMessageHandler = pHandler;
}

void CCefBrowserUI::OnProcessMessageReceived(CefRefPtr<CefProcessMessage> message)
{
	if (m_pProcessMessageHandler != nullptr)
	{
		m_pProcessMessageHandler->OnProcessMessageReceived(message);
	}
}

void CCefBrowserUI::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	m_pBrowser = browser;

	// 执行缓存的任务
	CefCacheTask task;
	while (!m_AfterCreatedCacheTasks.empty())
	{
		task = move(m_AfterCreatedCacheTasks.front());
		m_AfterCreatedCacheTasks.pop();

		task();
	}
}

void CCefBrowserUI::OnLoadStart()
{
	m_bLoadFinish = false;
}

void CCefBrowserUI::OnLoadEnd(int httpStatusCode)
{
	m_bLoadFinish = true;

	// 执行缓存的任务
	CefCacheTask task;
	while (!m_LoadEndCacheTasks.empty())
	{
		task = move(m_LoadEndCacheTasks.front());
		m_LoadEndCacheTasks.pop();

		task();
	}
}

void CCefBrowserUI::OnLoadError(int errorCode, const CefString& errorText, const CefString& failedUrl)
{
	while (!m_LoadEndCacheTasks.empty())
	{
		m_LoadEndCacheTasks.pop();
	}
}

void CCefBrowserUI::Navigate2(CefString url)
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetMainFrame()->LoadURL(url);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([url, this]
		{
			Navigate2(url);
		});
	}
}

void CCefBrowserUI::RunJs(CefString JsCode)
{
	if (m_pBrowser != nullptr)
	{
		if (m_bLoadFinish)
		{
			m_pBrowser->GetMainFrame()->ExecuteJavaScript(JsCode, "", 0);
		}
		else
		{
			m_LoadEndCacheTasks.push([JsCode, this]{ RunJs(JsCode); });
		}
	}
	else
	{
		m_AfterCreatedCacheTasks.push([JsCode, this]{ RunJs(JsCode); });
	}
}

void CCefBrowserUI::resize()
{
	if (m_pBrowser != nullptr)
	{
		POINT pt	= { m_rcItem.left, m_rcItem.top };
		HWND hwnd	= m_pBrowser->GetHost()->GetWindowHandle();

		if (GetWindowStyle(hwnd) & WS_POPUP)
		{
			::ClientToScreen(GetManager()->GetPaintWindow(), &pt);
		}

		::MoveWindow(hwnd
			, pt.x
			, pt.y
			, m_rcItem.right - m_rcItem.left
			, m_rcItem.bottom - m_rcItem.top
			, TRUE);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([this]{ resize(); });
	}
}