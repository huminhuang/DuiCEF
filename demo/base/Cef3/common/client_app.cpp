#include "stdafx.h"

#include "client_app.h"

const char kProcessType[] = "type";
const char kRendererProcess[] = "renderer";

ClientApp::ProcessType ClientApp::GetProcessType()
{
	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	// The command-line flag won't be specified for the browser process.
	if (!command_line->HasSwitch(kProcessType))
		return BrowserProcess;

	//
	if (command_line->GetSwitchValue(kProcessType) == kRendererProcess)
		return RendererProcess;

	return OtherProcess;
}