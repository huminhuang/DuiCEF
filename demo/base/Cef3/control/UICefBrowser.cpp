#include "stdafx.h"

#include "UICefBrowser.h"

#include "base/Cef3/common/cef_protocol.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

CefClientEx::CefClientEx(CCefBrowserUI* _owner)
	: m_owner(_owner)
{

}

void CefClientEx::CreateBrowser(HWND hParentWnd, RECT rc, CefString url)
{
	CefWindowInfo info;
	CefBrowserSettings settings;
	info.SetAsChild(hParentWnd, rc);

	CefBrowserHost::CreateBrowser(info, this, url, settings, NULL);
}

void CefClientEx::CloseBrowser()
{
	std::lock_guard<std::mutex> _locker(m_mutex);
	if (m_owner != nullptr)
	{
		if (m_owner->m_pBrowser.get())
		{
			m_owner->m_pBrowser->GetHost()->CloseBrowser(true);
		}
		m_owner = nullptr;
	}
}

bool CefClientEx::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();

	if (m_owner != nullptr && browser->GetIdentifier() == m_owner->GetIdentifier())
	{
		return m_owner->OnProcessMessageReceived(source_process, message);
	}

	return false;
}

bool CefClientEx::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{
	// 禁止创建新窗口
	return true;
}

void CefClientEx::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	cef_context_menu_type_flags_t flag = params->GetTypeFlags();
	if (flag & CM_TYPEFLAG_PAGE)
	{
		model->SetLabel(MENU_ID_STOPLOAD, L"停止加载");
		model->SetLabel(MENU_ID_REDO, L"重复");
	}
	if ((flag & CM_TYPEFLAG_EDITABLE) || (flag & CM_TYPEFLAG_SELECTION))
	{
		model->SetLabel(MENU_ID_UNDO, L"撤销");
		//model->SetLabel(MENU_ID_REDO, L"重做");
		model->SetLabel(MENU_ID_CUT, L"剪切");
		model->SetLabel(MENU_ID_COPY, L"复制");
		model->SetLabel(MENU_ID_PASTE, L"粘贴");
		model->SetLabel(MENU_ID_DELETE, L"删除");
		model->SetLabel(MENU_ID_SELECT_ALL, L"全选");
	}
	model->Remove(MENU_ID_PRINT);
#ifndef _DEBUG
	model->Remove(MENU_ID_VIEW_SOURCE);
#endif
	model->Remove(MENU_ID_BACK);
	model->Remove(MENU_ID_FORWARD);
	model->Remove(MENU_ID_REDO);
	if (MENUITEMTYPE_SEPARATOR == model->GetTypeAt(0))
		model->RemoveAt(0);
#if 0
	model->AddItem(MENU_ID_RELOAD, L"刷新");
#endif

	//不显示右键菜单
#ifndef _DEBUG
	for (int i = 0; i < model->GetCount(); i++)
	{
		model->RemoveAt(i);
	}
#endif
}

void CefClientEx::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	std::lock_guard<std::mutex> _locker(m_mutex);
	if (m_owner != nullptr)
	{
		m_owner->OnAfterCreated(browser);
	}
}

void CefClientEx::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();

	// TODO
}

CCefBrowserUI::CCefBrowserUI(CPaintManagerUI* pPaintManager)
	: m_pClient(new CefClientEx(this))
	, m_has_create(false)
	, m_pm(pPaintManager)
{

}


CCefBrowserUI::~CCefBrowserUI()
{
	m_pClient->CloseBrowser();
}

void CCefBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("homepage")) == 0) Navigate2(pstrValue);
	else return CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefBrowserUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if (IsVisible() && m_pBrowser.get())
	{
		HWND hBrowserWnd = m_pBrowser->GetHost()->GetWindowHandle();

		::MoveWindow(hBrowserWnd
			, m_rcItem.left
			, m_rcItem.top
			, m_rcItem.right - m_rcItem.left
			, m_rcItem.bottom - m_rcItem.top
			, true);

		if (!::IsWindowVisible(hBrowserWnd))
		{
			::ShowWindow(hBrowserWnd, IsVisible());
		}
	}

	return CControlUI::DoPaint(hDC, rcPaint);
}

void CCefBrowserUI::SetVisible(bool bVisible)
{
	if (m_pBrowser.get())
	{
		::ShowWindow(m_pBrowser->GetHost()->GetWindowHandle(), bVisible);
	}

	return CControlUI::SetVisible(bVisible);
}

void CCefBrowserUI::SetInternVisible(bool bVisible)
{
	if (m_pBrowser.get())
	{
		::ShowWindow(m_pBrowser->GetHost()->GetWindowHandle(), bVisible);
	}

	return CControlUI::SetInternVisible(bVisible);
}

bool CCefBrowserUI::OnProcessMessageReceived(CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	bool bRet = false;

	if (PID_RENDERER == source_process)
	{
		bRet = true;

		CefString name = message->GetName();
		CefRefPtr<CefListValue> list = message->GetArgumentList();

		if (name == CefProtocol::kTestFun)
		{
			// TODO
		}
	}

	return bRet;
}

void CCefBrowserUI::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	m_pBrowser = browser;
	
	//刷新界面
	RECT rcClient = { 0 };
	HWND hwnd = m_pm->GetPaintWindow();

	::GetClientRect(hwnd, &rcClient);
	::InvalidateRect(hwnd, &rcClient, true);
}

void CCefBrowserUI::Navigate2(LPCTSTR url)
{
	if (!m_has_create)
	{
		m_has_create = true;
		m_pClient->CreateBrowser(m_pm->GetPaintWindow(), m_rcItem, url);
	}

	if (m_pBrowser.get())
	{
		m_pBrowser->GetMainFrame()->LoadURL(CefString(url));
	}
}

void CCefBrowserUI::RunJs(const CefString& js)
{
	if (m_pBrowser.get())
	{
		m_pBrowser->GetMainFrame()->ExecuteJavaScript(js, m_pBrowser->GetMainFrame()->GetURL(), 0);
	}
}

int CCefBrowserUI::GetIdentifier() const
{
	if (m_pBrowser.get())
	{
		return m_pBrowser->GetIdentifier();
	}

	return -1;
}






