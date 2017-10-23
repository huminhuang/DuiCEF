#pragma once

#include "../common/client_app.h"

#include "include/base/cef_callback.h"
#include "include/base/cef_bind.h"

//
#define CEF_REQUIRE_MAIN_THREAD()		CHECK(CEF_RUN_ON_MAIN_THREAD());
#define CEF_POST_MAIN_THREAD_TASK(task)	_cef_main_win_handler.PostClosure(task)
#define CEF_RUN_ON_MAIN_THREAD()		_cef_main_win_handler.RunsTasksOnCurrentThread()

class CMainMessageLoopWin
{
public:
	CMainMessageLoopWin();
	~CMainMessageLoopWin();

	void PostTask(CefRefPtr<CefTask> task);
	void PostClosure(const base::Closure& closure);
	bool RunsTasksOnCurrentThread() const{ return _thread_id == GetCurrentThreadId(); }

private:

	static LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void PostTaskInternal(CefRefPtr<CefTask> task);
	CefRefPtr<CefTask> CefCreateClosureTask(const base::Closure& closure);

	HWND	_message_hwnd_;
	UINT	_message_id;
	DWORD	_thread_id;

	DISALLOW_COPY_AND_ASSIGN(CMainMessageLoopWin);
};

extern CMainMessageLoopWin _cef_main_win_handler;
