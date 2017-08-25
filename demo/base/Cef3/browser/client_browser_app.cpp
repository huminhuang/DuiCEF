#include "stdafx.h"

#include "client_browser_app.h"


void ClientAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
	//查看更多的命令行 http://peter.sh/experiments/chromium-command-line-switches/#disable-gpu-process-prelaunch

	// cef3 调Lantern 的系统代理时.pac 文件时会崩溃
	// 已经确认是此版本Cef的bug
	// 目前先加入此参数，不走代理，不去执行.pac文件。
	command_line->AppendSwitch("no-proxy-server");

	//禁用GPU进程
	//GPU在处理3D图形动画有明显的优势，但目前我们的页面还未涉及3D动画
	//所以暂时禁掉，以节省进程数
	command_line->AppendSwitch("--disable-gpu");

	//设置所有的页面共用一个进程，以节省进程数
	command_line->AppendSwitchWithValue("--renderer-process-limit", "1");

#if 0
	/*推荐使用以下方法*/
	command_line->AppendSwitchWithValue("ppapi-flash-path", "PepperFlash/pepflashplayer.dll");
	command_line->AppendSwitchWithValue("ppapi-flash-version", "22.0.0.192");
	command_line->AppendSwitchWithValue("plugin-policy", "allow");
#endif
}