#include "stdafx.h"
#include "process_message_handler.h"

#include "../renderer/v8_window_function.h"

void CProcessMessageHandler::OnProcessMessageReceived(CefRefPtr<CefProcessMessage> message)
{
	// TODO
	CefString name = message->GetName();
	CefRefPtr<CefListValue> list = message->GetArgumentList();

	if (name == V8WindowFunciotn::winTest)
	{

	}
}
