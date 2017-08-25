#include "stdafx.h"

#include "client_renderer_app.h"
#include "client_v8handler.h"

#include "../common/cef_protocol.h"

void ClientAppRender::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// Create an instance of my CefV8Handler object.
	CefRefPtr<CefV8Handler> handler = new V8Handler();

	for (const auto& funName : CefProtocol::fun_name_set)
	{

		// Create the "myfunc" function.
		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(funName, handler);

		// Add the "myfunc" function to the "window" object.
		object->SetValue(funName, func, V8_PROPERTY_ATTRIBUTE_NONE);
	}

}

