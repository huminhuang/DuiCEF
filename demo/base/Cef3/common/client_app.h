#pragma once

#include "include/cef_app.h"

class ClientApp
	: public CefApp
{
public:
	enum ProcessType
	{
		BrowserProcess,
		RendererProcess,
		ZygoteProcess,
		OtherProcess,
	};

	//自定义方法
	//获取进程类型
	static ProcessType GetProcessType();

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ClientApp);
};