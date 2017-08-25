#include "StdAfx.h"
#include "client_v8handler.h"

bool V8Handler::Execute( const CefString& name, CefRefPtr<CefV8Value> object, \
							const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception )
{
	CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create( name );

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> args = msg->GetArgumentList();

	// Populate the argument values.

	int _size = arguments.size();

	args->SetSize(_size);

	for (int i = 0; i<_size; i++)
	{
		if (arguments[i]->IsString())
		{
			args->SetString(i, arguments[i]->GetStringValue());
		}
		else if (arguments[i]->IsInt())
		{
			args->SetInt(i, arguments[i]->GetIntValue());
		}
		else if (arguments[i]->IsDouble())
		{
			args->SetDouble(i, arguments[i]->GetDoubleValue());
		}
		else if (arguments[i]->IsBool())
		{
			args->SetBool(i, arguments[i]->GetBoolValue());
		}
		else
		{
			args->SetString(i, arguments[i]->GetStringValue());
		}
	}

	context->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
