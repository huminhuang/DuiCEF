#include "stdafx.h"

#include "UICefBrowser.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "../browser/main_message_loop.h"

CefRefPtr<CefClientEx> CefClientEx::Create(CCefBrowserUI* owner)
{
	CefRefPtr<CefClientEx> instance(new CefClientEx);

	instance->m_pOwner = owner;
	instance->m_pSelf = instance;

	return instance;
}

void CefClientEx::CreateBrowser(HWND hParentWnd, CefString url)
{
	REQUIRE_MAIN_THREAD;

	CefWindowInfo info;
	CefBrowserSettings settings;
	info.SetAsPopup(hParentWnd, _T(""));
	info.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	CefBrowserHost::CreateBrowser(info, m_pSelf, url, settings, NULL);
}

void CefClientEx::CloseBrowser()
{
	REQUIRE_MAIN_THREAD;

	m_pSelf->Release();
	m_pOwner = nullptr;
}

bool CefClientEx::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	WindowOpenDisposition target_disposition,
	bool user_gesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access)
{
	// 禁止创建新窗口
	return true;
}

void CefClientEx::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	if (!RunsTasksOnMainThread)
	{
		PostMainThreadTask(base::Bind(&CefClientEx::OnLoadEnd, m_pSelf, browser, frame, httpStatusCode)); return;
	}

	REQUIRE_MAIN_THREAD;
	m_pOwner->OnLoadEnd(httpStatusCode);
}

void CefClientEx::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	if (!RunsTasksOnMainThread)
	{
		PostMainThreadTask(base::Bind(&CefClientEx::OnAfterCreated, m_pSelf, browser)); return;
	}
	
	REQUIRE_MAIN_THREAD;

	if (m_pOwner != nullptr)
	{
		m_pOwner->OnAfterCreated(browser);
	}
	else
	{
		browser->GetHost()->CloseBrowser(true);
	}
}

CCefBrowserUI::CCefBrowserUI(CPaintManagerUI* ppm)
{
	m_pClient = CefClientEx::Create(this);
	m_pClient->CreateBrowser(ppm->GetPaintWindow(), /*_T("about:blank")*/_T("www.huya.com"));
}

CCefBrowserUI::~CCefBrowserUI()
{
	GetManager()->RemoveMessageFilter(this);

	m_pClient->CloseBrowser();

	if (m_pBrowser.get())
	{
		m_pBrowser->GetHost()->CloseBrowser(true);
	}
}

void CCefBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("url")) == 0) LoadURL(pstrValue);
	else if (_tcscmp(pstrName, _T("alpha")) == 0) SetAlpha(_ttoi(pstrValue));
	else return CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefBrowserUI::DoInit()
{
	GetManager()->AddMessageFilter(this);
}

void CCefBrowserUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CControlUI::DoPaint(hDC, rcPaint);

	OnResize();
}

void CCefBrowserUI::SetVisible(bool bVisible)
{
	if (m_pBrowser.get())
	{
		m_pBrowser->GetHost()->SetWindowVisibility(bVisible);
	}
	else
	{
		m_CreateCacheTasks.push([bVisible, this]{ SetVisible(bVisible); });
	}

	return CControlUI::SetVisible(bVisible);
}

void CCefBrowserUI::SetInternVisible(bool bVisible)
{
	if (m_pBrowser.get())
	{
		m_pBrowser->GetHost()->SetWindowVisibility(bVisible);
	}
	else
	{
		m_CreateCacheTasks.push([bVisible, this]{ SetInternVisible(bVisible); });
	}

	return CControlUI::SetInternVisible(bVisible);
}

int CCefBrowserUI::GetIdentifier() const
{
	return m_pBrowser.get() ? m_pBrowser->GetIdentifier() : -1;
}

void CCefBrowserUI::LoadURL(CefString url, const std::function<void(void)>& fnLoadEndTask)
{
	if (m_pBrowser.get())
	{
		m_pBrowser->GetMainFrame()->LoadURL(url);
	}
	else
	{
		m_CreateCacheTasks.push([url, fnLoadEndTask, this]{ LoadURL(url, fnLoadEndTask); });
	}
}

void CCefBrowserUI::ExecuteJS(CefString JsCode)
{
	if (m_pBrowser.get())
	{
		m_pBrowser->GetMainFrame()->ExecuteJavaScript(JsCode, "", 0);
	}
	else
	{
		m_CreateCacheTasks.push([JsCode, this]{ ExecuteJS(JsCode); });
	}
}

void CCefBrowserUI::SetAlpha(UINT8 uAlpha)
{
	if (m_pBrowser.get())
	{
		HWND hWnd = m_pBrowser->GetHost()->GetWindowHandle();

		::SetWindowLong(hWnd, GWL_EXSTYLE, ::GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hWnd, 0, uAlpha, LWA_ALPHA);
	}
	else
	{
		m_CreateCacheTasks.push([uAlpha, this]{ SetAlpha(uAlpha); });
	}
}

LRESULT CCefBrowserUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	switch (uMsg)
	{
	case WM_WINDOWPOSCHANGED:
	case WM_MOVE:
	{
		OnResize();
	}
	default:
		break;
	}
	return 0;
}

void CCefBrowserUI::OnResize()
{
	if (m_pBrowser != nullptr)
	{
		POINT pt = { m_rcItem.left, m_rcItem.top };
		::ClientToScreen(GetManager()->GetPaintWindow(), &pt);

		::MoveWindow(m_pBrowser->GetHost()->GetWindowHandle()
			, pt.x
			, pt.y
			, m_rcItem.right - m_rcItem.left
			, m_rcItem.bottom - m_rcItem.top
			, false);
	}
	else
	{
		m_CreateCacheTasks.push([this]{ OnResize(); });
	}
}

void CCefBrowserUI::OnLoadEnd(int httpStatusCode)
{

}

void CCefBrowserUI::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	m_pBrowser = browser;

	while (!m_CreateCacheTasks.empty())
	{
		std::function<void(void)> task = move(m_CreateCacheTasks.front());
		m_CreateCacheTasks.pop();
		task();
	}
}