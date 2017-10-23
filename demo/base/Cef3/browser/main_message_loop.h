#pragma once

#include "../common/client_app.h"

#include "include/base/cef_callback.h"
#include "include/base/cef_bind.h"

//
#define CEF_REQUIRE_MAIN_THREAD()   CHECK(_cef_main_win_handler.RunsTasksOnCurrentThread());
#define PostMainThreadTask(task)	_cef_main_win_handler.PostClosure(task)
#define RunsTasksOnMainThread		_cef_main_win_handler.RunsTasksOnCurrentThread()

class CMainMessageLoopWin
{
public:
	CMainMessageLoopWin();
	~CMainMessageLoopWin();

	void PostTask(CefRefPtr<CefTask> task);
	void PostClosure(const base::Closure& closure);
	bool RunsTasksOnCurrentThread() const{ return _thread_id == GetCurrentThreadId(); }

private:

	// Window procedure for the message window.
	static LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void PostTaskInternal(CefRefPtr<CefTask> task);
	CefRefPtr<CefTask> CefCreateClosureTask(const base::Closure& closure);

	HWND	_message_hwnd_;
	UINT	_message_id;
	DWORD	_thread_id;

	DISALLOW_COPY_AND_ASSIGN(CMainMessageLoopWin);
};

extern CMainMessageLoopWin _cef_main_win_handler;
