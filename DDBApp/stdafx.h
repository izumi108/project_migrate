// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN					// �� Windows ͷ���ų�����ʹ�õ�����
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// Windows ͷ�ļ�:
#include <windows.h>
#include <WTypes.h>

// C ����ʱͷ�ļ�
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

// ��������ͷ�ļ�
#include "../Global/Macros.h"

#include "Message.h"
#include "GroupMessage.h"
#include "BaseThread.h"
#include "BaseTimer.h"

//////////////////////////////////////////////////////////////////////////
// �������ܿ�
#include "DDBUtils.h"
#pragma comment(lib,"DDBUtils.lib")

