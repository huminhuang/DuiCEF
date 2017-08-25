#pragma once

#include "../common/client_app.h"

class ClientAppBrowser
	: public ClientApp
	, public CefBrowserProcessHandler
{
public:
	
	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE{ return this; }
	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;

	// CefBrowserProcessHandler methods:
	//

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ClientAppBrowser);
};
