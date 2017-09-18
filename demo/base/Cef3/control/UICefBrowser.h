#pragma once

#include "include/cef_client.h"
#include <functional>
#include <queue>

class CCefBrowserUI;

class CefClientEx
	: public CefClient
	, public CefLifeSpanHandler
	, public CefContextMenuHandler
	, public CefLoadHandler
{
public:
	static CefRefPtr<CefClientEx> Create(CCefBrowserUI* owner);

	void CreateBrowser(HWND hParentWnd, CefString url);
	void CloseBrowser();

	virtual CefRefPtr<CefLifeSpanHandler>		GetLifeSpanHandler()		{ return this; }
	virtual CefRefPtr<CefContextMenuHandler>	GetContextMenuHandler()		{ return this; }
	virtual CefRefPtr<CefLoadHandler>			GetLoadHandler()			{ return this; }

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

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode);

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);

protected:
	IMPLEMENT_REFCOUNTING(CefClientEx);

	CCefBrowserUI*	m_pOwner;
	CefRefPtr<CefClientEx> m_pSelf;
};

class CCefBrowserUI :public CControlUI, public IMessageFilterUI
{
	friend CefClientEx;
public:
	CCefBrowserUI(CPaintManagerUI* ppm);
	~CCefBrowserUI();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void DoInit();
	void DoPaint(HDC hDC, const RECT& rcPaint);

	void SetVisible(bool bVisible);
	void SetInternVisible(bool bVisible);

	int	GetIdentifier()	const;
	void LoadURL(CefString url, const std::function<void(void)>& fnLoadEndTask = nullptr);
	void ExecuteJS(CefString JsCode);
	void SetAlpha(UINT8 uAlpha);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
	void OnResize();

	void OnLoadEnd(int httpStatusCode);
	void OnAfterCreated(CefRefPtr<CefBrowser> browser);

private:
	CefRefPtr<CefClientEx>	m_pClient;
	CefRefPtr<CefBrowser>	m_pBrowser;

	std::queue<std::function<void(void)>> m_CreateCacheTasks;
};