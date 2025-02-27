#ifndef IExit_h__
#define IExit_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	IExit.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:30:36
// 说明		:	退出事件接口
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Singleton_Export.h"

class SINGLETON_API IExit
{
public:
	IExit();
	~IExit();
	//全部模块的保存现有数据时间总和只有3秒
	//所有有界面的模块都必须继承此方法，如无操作则直接return true
	virtual bool OnExit();

};


#endif // IExit_h__
