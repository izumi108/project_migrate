#ifndef SingletonDefine_h__
#define SingletonDefine_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称 :   SingletonDefine.h
// 创建人    :   daviyang35@QQ.com
// 创建时间 :   2014-07-05 12:15:07
// 说明       :   全局宏定义
/////////////////////////////////////////////////////////////*/
#pragma once
#include <string>

#ifdef SINGLETON_EXPORTS
#define SINGLETON_API __declspec(dllexport)
#else
#define SINGLETON_API __declspec(dllimport)
#endif

#ifdef _UNICODE
typedef std::wstring CDXString;
#else
typedef std::string CDXString;
#endif

typedef std::wstring CDXStringW;
typedef std::string CDXStringA;

#endif // SingletonDefine_h__
