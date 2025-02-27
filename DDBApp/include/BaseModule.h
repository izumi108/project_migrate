#ifndef BaseModule_h__
#define BaseModule_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	BaseModule.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2015-03-23 11:23:22
// ˵��			:	����ӿ���
/////////////////////////////////////////////////////////////*/
#pragma once

namespace DuiLib
{
	class CWindowUI;
}

// ģ�����
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
	// Returns:   std::vector<CString>* �����б�
	// Qualifier:
	//************************************
	virtual void CommandList(std::vector<CString> *pArray)=0;

	//************************************
	// Method:    ��ѯ����˵��
	// FullName:  BaseModule::CommandExplain
	// Access:    virtual public 
	// Returns:   CDXString ����˵���ַ���
	// Qualifier: 
	// Parameter: CDXString ��������
	//************************************
	virtual LPCTSTR CommandExplain(LPCTSTR cmd)=0;

	//************************************
	// Method:    ִ�й���
	// FullName:  BaseModule::Excute
	// Access:    virtual public 
	// Returns:   int 0 ����ɹ��������������
	// Qualifier:
	// Parameter: CDXString cmd  ������
	// Parameter: vector<string> paramArray  �����б�
	//************************************
	virtual int Excute(LPCTSTR cmd,std::vector<CString> paramArray)=0;
};
#endif // BaseModule_h__