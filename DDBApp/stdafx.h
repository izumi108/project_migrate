// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN					// 从 Windows 头中排除极少使用的资料
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// Windows 头文件:
#include <windows.h>
#include <WTypes.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// ATL
#include <atlstr.h>
#include <atltime.h>
#include <atltypes.h>

// STL
#include <hash_map>
#include <hash_set>
#include <vector>
#include <map>
#include <list>
#include <string>
using namespace std;

// 公共引用头文件
#include "../Global/Macros.h"

#include "Message.h"
#include "GroupMessage.h"
#include "BaseThread.h"
#include "BaseTimer.h"

//////////////////////////////////////////////////////////////////////////
// 基础功能库
#include "DDBUtils.h"
#pragma comment(lib,"DDBUtils.lib")

