// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO:  在此处引用程序需要的其他头文件
#include "resource.h"
#include "UIlib.h"

using namespace DuiLib;
using namespace Gdiplus;

#include "base/Cef3/cef_util.h"
#include "base/ControlEx/ControlEx.h"

#ifdef _DEBUG
	#pragma comment(lib, "Duilib/DuiLib_d.lib")
#else
	#pragma comment(lib, "Duilib/DuiLib.lib")
#endif

//#include <vld.h>