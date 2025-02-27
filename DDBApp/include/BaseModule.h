#ifndef BaseModule_h__
#define BaseModule_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	BaseModule.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-03-23 11:23:22
// 说明			:	插件接口类
/////////////////////////////////////////////////////////////*/
#pragma once

namespace DuiLib
{
	class CWindowUI;
}

// 模块基类
class BaseModule
{
public:
	virtual LPCTSTR Identity() =0;
	virtual LPCTSTR Name() = 0;
	virtual LPCTSTR Icon() = 0;
    virtual LPCTSTR Description() = 0;

	virtual void Init(DuiLib::CWindowUI* pWindowUI) =0;
	virtual int LayoutIndex()=0;
    virtual void Show() = 0;
    virtual void Hide(bool isAll=true) = 0;

	//************************************
	// Method:    CommandList
	// FullName:  BaseModule::CommandList
	// Access:    virtual public 
	// Returns:   std::vector<CString>* 功能列表
	// Qualifier:
	//************************************
	virtual void CommandList(std::vector<CString> *pArray)=0;

	//************************************
	// Method:    查询命令说明
	// FullName:  BaseModule::CommandExplain
	// Access:    virtual public 
	// Returns:   CDXString 命令说明字符串
	// Qualifier: 
	// Parameter: CDXString 命令名称
	//************************************
	virtual LPCTSTR CommandExplain(LPCTSTR cmd)=0;

	//************************************
	// Method:    执行功能
	// FullName:  BaseModule::Excute
	// Access:    virtual public 
	// Returns:   int 0 代表成功，其他代表错误
	// Qualifier:
	// Parameter: CDXString cmd  功能名
	// Parameter: vector<string> paramArray  参数列表
	//************************************
	virtual int Excute(LPCTSTR cmd,std::vector<CString> paramArray)=0;
};
#endif // BaseModule_h__