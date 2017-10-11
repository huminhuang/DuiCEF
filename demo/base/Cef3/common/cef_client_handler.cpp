#include "stdafx.h"

#include "cef_client_handler.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "../browser/main_message_loop.h"
#include "../control/UICefBrowser.h"

void CCefClientHandler::CreateBrowser(CefString url, CefWindowInfo& info)
{
	CEF_REQUIRE_MAIN_THREAD();

	CefBrowserSettings settings;

	CefBrowserHost::CreateBrowser(info, this, url, settings, NULL);
}

void CCefClientHandler::Close()
{
	CEF_REQUIRE_MAIN_THREAD();

	m_pOwner = nullptr;
}

bool CCefClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser
	, CefProcessId source_process
	, CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();

	CefRefPtr<CefProcessMessage> msg = message->Copy();
	exec_on_main_thread([this, msg]
	{
		if (m_pOwner != nullptr)
		{
			m_pOwner->OnProcessMessageReceived(msg);
		}
	});

	return true;
}

void CCefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	exec_on_main_thread([this, browser]
	{
		if (m_pOwner != nullptr)
		{
			m_pOwner->OnAfterCreated(browser);
		}
		else
		{
			browser->GetHost()->CloseBrowser(true);
		}
	});
}

void CCefClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser
	, CefRefPtr<CefFrame> frame
	, CefRefPtr<CefContextMenuParams> params
	, CefRefPtr<CefMenuModel> model)
{
	//≤ªœ‘ æ”“º¸≤Àµ•
#ifndef _DEBUG
	for (int i = 0; i < model->GetCount(); i++)
	{
		model->RemoveAt(i);
	}
#endif
}

void CCefClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();

	exec_on_main_thread([this]
	{
		m_pOwner->OnLoadStart();
	});
}

void CCefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser
	, CefRefPtr<CefFrame> frame
	, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();

	exec_on_main_thread([this, httpStatusCode]
	{
		m_pOwner->OnLoadEnd(httpStatusCode);
	});
}

void CCefClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser
	, CefRefPtr<CefFrame> frame
	, ErrorCode errorCode
	, const CefString& errorText
	, const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	exec_on_main_thread([this, errorCode, errorText, failedUrl]
	{
		m_pOwner->OnLoadError(errorCode, errorText, failedUrl);
	});
}

bool CCefClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask)
{
	CEF_REQUIRE_UI_THREAD();

	// Forbid dragging of link URLs.
	if (mask & DRAG_OPERATION_LINK)
		return true;

	return false;
}

void CCefClientHandler::exec_on_main_thread(std::function<void(void)> task)
{
	if (m_pOwner == nullptr)
		return;

	if (!RunsTasksOnMainThread)
	{
		PostMainThreadTask(base::Bind(&CCefClientHandler::exec_on_main_thread, this, task)); 
	}
	else
	{
		task();
	}
}
