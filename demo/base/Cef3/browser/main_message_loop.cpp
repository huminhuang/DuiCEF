#include "stdafx.h"

#include "main_message_loop.h"
#include <mutex>

CMainMessageLoopWin *g_instance = nullptr;

CMainMessageLoopWin* CMainMessageLoopWin::Get()
{
	return g_instance;
}

namespace
{
	const TCHAR kWndClass[] = _T("Client_MessageWindow");
	const TCHAR kTaskMessageName[] = _T("Client_CustomTask");


	class CefClosureTask : public CefTask {
	public:
		explicit CefClosureTask(const base::Closure& closure)
			: closure_(closure) {
		}

		// CefTask method
		virtual void Execute() OVERRIDE{
			closure_.Run();
			closure_.Reset();
		}

	private:
		base::Closure closure_;

		IMPLEMENT_REFCOUNTING(CefClosureTask);
		DISALLOW_COPY_AND_ASSIGN(CefClosureTask);
	};
}

HWND CMainMessageLoopWin::CreateMessageWindow(HINSTANCE hInstance)
{

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = MessageWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kWndClass;
	RegisterClassEx(&wc);

	return CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance, 0);
}

LRESULT CALLBACK CMainMessageLoopWin::MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CMainMessageLoopWin* self = (CMainMessageLoopWin*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (self && message == self->_message_id) {
		// Execute the task.
		CefTask* task = reinterpret_cast<CefTask*>(wParam);
		task->Execute();

		// Release the reference added in PostTaskInternal. This will likely result
		// in |task| being deleted.
		task->Release();
	}
	else switch (message) {
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
	DCHECK(g_instance == nullptr);

	g_instance = this;
}

CMainMessageLoopWin::~CMainMessageLoopWin()
{
	DCHECK(g_instance != nullptr);

	g_instance = nullptr;
}

int CMainMessageLoopWin::Run()
{
	DCHECK(RunsTasksOnCurrentThread());

	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	{
		std::lock_guard<std::mutex> locker(_mutex);

		// Create the hidden window for message processing.
		_message_hwnd_ = CreateMessageWindow(hInstance);
		CHECK(_message_hwnd_);

		// Store a pointer to |this| in the window's user data.
		LONG_PTR result = ::SetWindowLongPtr(_message_hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		// Execute any tasks that are currently queued.
		while (!queued_tasks_.empty()) {
			PostTaskInternal(queued_tasks_.front());
			queued_tasks_.pop();
		}
	}

	MSG msg;

	// Run the application message loop.
	while (GetMessage(&msg, NULL, 0, 0)) {
		// Allow processing of dialog messages.
		//if (dialog_hwnd_ && IsDialogMessage(dialog_hwnd_, &msg))
		//	continue;

		if (!CPaintManagerUI::TranslateMessage(&msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	{
		std::lock_guard<std::mutex> locker(_mutex);

		// Destroy the message window.
		DestroyWindow(_message_hwnd_);
		_message_hwnd_ = NULL;
	}

	return static_cast<int>(msg.wParam);
}

void CMainMessageLoopWin::Quit()
{
	// Execute PostQuitMessage(0) on the main thread.
	PostClosure(base::Bind(::PostQuitMessage, 0));
}

void CMainMessageLoopWin::PostTask(CefRefPtr<CefTask> task)
{
	std::lock_guard<std::mutex> locker(_mutex);

	PostTaskInternal(task);
}

void CMainMessageLoopWin::PostClosure(const base::Closure& closure)
{
	PostTask(CefCreateClosureTask(closure));
}

void CMainMessageLoopWin::PostTaskInternal(CefRefPtr<CefTask> task)
{
	ASSERT(!_mutex.try_lock());

	if (!_message_hwnd_) {
		// Queue the task until the message loop starts running.
		queued_tasks_.push(task);
		return;
	}

	// Add a reference that will be released in MessageWndProc.
	task->AddRef();

	// Post the task for execution by the message window.
	PostMessage(_message_hwnd_, _message_id,
		reinterpret_cast<WPARAM>(task.get()), 0);
}

CefRefPtr<CefTask> CMainMessageLoopWin::CefCreateClosureTask(const base::Closure& closure)
{
	return new CefClosureTask(closure);
}