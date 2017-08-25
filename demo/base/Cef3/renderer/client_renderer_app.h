#pragma once

#include "../common/client_app.h"

class ClientAppRender
	: public ClientApp
	, public CefRenderProcessHandler
{
public:
	
	// CefApp methods:
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE{ return this; }

	//CefRenderProcessHandler methods
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ClientAppRender);
};
