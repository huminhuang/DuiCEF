#include "stdafx.h"

#include "base/Cef3/browser/main_message_loop.h"
#include "base/Cef3/common/cef_main.h"
#include "main/WndSimpleFrame.h"

template <class T>
void CreateMainWnd(LPCTSTR pstrName)
{
	CWindowWnd *mainFrame = new T;
	mainFrame->Create(NULL, pstrName
		, WS_OVERLAPPEDWINDOW
		, WS_EX_OVERLAPPEDWINDOW
		, 0, 0, 1000, 500);
	mainFrame->CenterWindow();
	mainFrame->ShowWindow();
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{ 
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ------------------cef初始化------------------
	CCefMainHandler CefMainHandler;
	if (!CefMainHandler.Initialize(hInstance)) return -1;

	// ------------------duilib初始化------------------
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("Skin\\"));
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
	{
		return -1;
	}
	
	// ------------------显示主窗口------------------
	CreateMainWnd<CWndSimpleFrame>(_T("SimpleWnd"));


	// ------------------开启消息循环------------------
	CefMainHandler.RunMessageLoop();
	

	// ------------------结束关闭资源------------------
	::CoUninitialize();
	
	return 0;
}
