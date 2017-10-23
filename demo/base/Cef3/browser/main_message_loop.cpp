#include "stdafx.h"

#include "main_message_loop.h"

namespace
{
	const TCHAR kWndClass[] = _T("Client_MessageWindow");
	const TCHAR kTaskMessageName[] = _T("Client_CustomTask");

	class CefClosureTask : public CefTask 
	{
	public:
		explicit CefClosureTask(const base::Closure& closure): closure_(closure) {}

		// CefTask method
		virtual void Execute() OVERRIDE
		{
			closure_.Run();
			closure_.Reset();
		}

	private:
		base::Closure closure_;

		IMPLEMENT_REFCOUNTING(CefClosureTask);
		DISALLOW_COPY_AND_ASSIGN(CefClosureTask);
	};
}

CMainMessageLoopWin _cef_main_win_handler;

LRESULT CALLBACK CMainMessageLoopWin::MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CMainMessageLoopWin* self = (CMainMessageLoopWin*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (self && message == self->_message_id) 
	{
		// Execute the task.
		CefTask* task = reinterpret_cast<CefTask*>(wParam);
		task->Execute();

		// Release the reference added in PostTaskInternal. This will likely result
		// in |task| being deleted.
		task->Release();
	}
	else switch (message) 
	{
	case WM_DESTROY:
		// Clear the reference to |self|.
		LONG_PTR result = ::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

CMainMessageLoopWin::CMainMessageLoopWin()
	: _message_id(RegisterWindowMessage(kTaskMessageName))
	, _thread_id(GetCurrentThreadId())
	, _message_hwnd_(nullptr)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = MessageWndProc;
	wc.hInstance = CPaintManagerUI::GetInstance();
	wc.lpszClassName = kWndClass;
	RegisterClassEx(&wc);

	_message_hwnd_ = CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, CPaintManagerUI::GetInstance(), 0);

	::SetWindowLongPtr(_message_hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

CMainMessageLoopWin::~CMainMessageLoopWin()
{
	::DestroyWindow(_message_hwnd_);
	_message_hwnd_ = NULL;
}

void CMainMessageLoopWin::PostTask(CefRefPtr<CefTask> task)
{
	PostTaskInternal(task);
}

void CMainMessageLoopWin::PostClosure(const base::Closure& closure)
{
	PostTask(CefCreateClosureTask(closure));
}

void CMainMessageLoopWin::PostTaskInternal(CefRefPtr<CefTask> task)
{
	task->AddRef();

	PostMessage(_message_hwnd_, _message_id, reinterpret_cast<WPARAM>(task.get()), 0);
}

CefRefPtr<CefTask> CMainMessageLoopWin::CefCreateClosureTask(const base::Closure& closure)
{
	return new CefClosureTask(closure);
}