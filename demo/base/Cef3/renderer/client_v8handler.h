#pragma once

#include "include/cef_v8.h"

class V8Handler: public CefV8Handler
{
public:

	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, \
		const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(V8Handler);
};

