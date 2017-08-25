#pragma once

#include "include/cef_client.h"
#include <mutex>

class CCefBrowserUI;

class CefClientEx
	: public CefClient
	, public CefLifeSpanHandler
	, public CefContextMenuHandler
	, public CefLoadHandler
{
public:
	CefClientEx(CCefBrowserUI* _owner);

	void CreateBrowser(HWND hParentWnd, RECT rc, CefString url);
	void CloseBrowser();

	virtual CefRefPtr<CefLifeSpanHandler>		GetLifeSpanHandler()		{ return this; }
	virtual CefRefPtr<CefContextMenuHandler>	GetContextMenuHandler()		{ return this; }
	virtual CefRefPtr<CefLoadHandler>			GetLoadHandler()			{ return this; }

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		WindowOpenDisposition target_disposition,
		bool user_gesture,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) OVERRIDE;

	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model)OVERRIDE;

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser)OVERRIDE;

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(CefClientEx);

	std::mutex		m_mutex;
	CCefBrowserUI*	m_owner;
};


class CCefBrowserUI :public CControlUI
{
	friend CefClientEx;
public:
	CCefBrowserUI(CPaintManagerUI* pPaintManager);
	~CCefBrowserUI();

	virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void	DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void	SetVisible(bool bVisible);
	virtual void	SetInternVisible(bool bVisible);

	virtual bool OnProcessMessageReceived(CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);

	int		GetIdentifier()	const;
	virtual void Navigate2(LPCTSTR url);
	virtual void RunJs(const CefString& js);

protected:

	CPaintManagerUI*		m_pm;
	CefRefPtr<CefBrowser>	m_pBrowser;
	CefRefPtr<CefClientEx>	m_pClient;
	bool					m_has_create;
};