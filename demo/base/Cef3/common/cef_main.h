#pragma once

#include "../browser/client_browser_app.h"
#include "../renderer/client_renderer_app.h"

class CCefMainHandler: public CefBase
{
public:

	bool Initialize(HINSTANCE hInstance, BOOL _multi_threaded_message_loop = TRUE)
	{
		m_multi_threaded_message_loop = _multi_threaded_message_loop;

		CefMainArgs args(hInstance);

		// 获取进程类型
		ClientApp::ProcessType processType = ClientApp::GetProcessType();

		// 根据进程来创建不同的CefApp实例
		CefRefPtr<CefApp> app;

		// browser进程：创建ClientAppBrowser实例
		if (processType == ClientApp::ProcessType::BrowserProcess)
		{
			app = new ClientAppBrowser;
		}
		// render进程：创建ClientAppRender实例
		else if (processType == ClientApp::ProcessType::RendererProcess)
		{
			app = new ClientAppRender;
		}
		// 其他进程：创建ClientApp实例
		else
		{
			app = new ClientApp;
		}

		// 子进程在这里阻塞，直到退出
		int exit_code = CefExecuteProcess(args, app.get(), NULL);
		if (exit_code >= 0)
		{
			exit(exit_code);
		}

		CefSettings settings;
		CefSettingsTraits::init(&settings);
		settings.log_severity = LOGSEVERITY_DISABLE;
		settings.multi_threaded_message_loop = _multi_threaded_message_loop;
		CefString(&settings.locale).FromWString(L"zh-CN");

		return CefInitialize(args, settings, app.get(), NULL);
	}

	void RunMessageLoop()
	{
		if (m_multi_threaded_message_loop)
		{
			m_main_message_loop_win.Run();
		}
		else
		{
			CefRunMessageLoop();
		}
	}

	~CCefMainHandler()
	{
		CefShutdown();
	}

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefMainHandler);

public:
	BOOL m_multi_threaded_message_loop;
	CMainMessageLoopWin m_main_message_loop_win;
};