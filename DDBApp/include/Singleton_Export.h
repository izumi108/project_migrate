#ifndef SingletonDefine_h__
#define SingletonDefine_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ����� :   SingletonDefine.h
// ������    :   daviyang35@QQ.com
// ����ʱ�� :   2014-07-05 12:15:07
// ˵��       :   ȫ�ֺ궨��
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
