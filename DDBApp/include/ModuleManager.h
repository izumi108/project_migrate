#ifndef ModuleManager_h__
#define ModuleManager_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ����� :   ModuleManager.h
// ������    :   daviyang35@QQ.com
// ����ʱ�� :   2014-07-05 12:32:45
// ˵��       :   ģ�������
/////////////////////////////////////////////////////////////*/
#pragma once

class BaseModule;
namespace DuiLib
{
    class CContainerUI;
    class CWindowUI;
}

// GetPluginInstance(CFilterProcess,_T("FilterProcess"));
#define  GetPluginInstance(x,y) dynamic_cast<##x*>(Singleton::GetModuleManager()->GetModule(##y))

/************************************************************************/
/* ģ�������                                                   */
/************************************************************************/
class SINGLETON_API ModuleManager
{
    friend class Singleton;
    typedef std::vector<CString> VString;
public:

    // Summary  : ����ģ��
    // Parameter: CContainerUI * pLeftPanel ���������
    // Parameter: CContainerUI * pMainPanel ���������
    //void LoadModules(CContainerUI* pLeftPanel, CContainerUI* pMainPanel);

    // ����ָ��·����ģ��
    // ���ȼ���Ѽ���ʵ�������������������������
    // ����ֵ���Ϊfalse��˵��ʵ�����Ӧ��dll��������
    bool Require(LPCTSTR lpszModuleName);

    // ����ini�ļ�ָ��ģ��
    // ������pWindow ������ʾ��������ָ��
    void RequireModules(LPCTSTR lpszIniFilePath);

    // ��ʼ��UI
    void InitUserInterface(DuiLib::CWindowUI* pWindow);

    // Summary  : ȡ��ģ����
    // Returns  : ģ����
    int GetModuleCount();

    // Summary  : ȡ��ģ��ָ��
    // Returns  : ģ��ָ��b
    // Parameter: int index �ڼ���ģ��
    BaseModule* GetModule(int index)
    {
        return _vecModules.at(index);
    }

    // Summary  : ȡ��ģ��ָ��
    // Returns  : ģ��ָ��
    // Parameter: wstring id ģ��ID
    BaseModule* GetModule(LPCTSTR lpszName);

    // Summary  : ȡ������ģ��ָ���б�
    std::vector<BaseModule*> GetModules();

    // Summary  : ȡ��ָ������ģ��ָ���б�
    std::vector<BaseModule*> GetModulesByFilter(LPCTSTR lpszFilter);

private:
    ModuleManager();
    ~ModuleManager();
#pragma warning(disable:4251)
    std::vector<HMODULE>       _vecDlls;                // DLL���
    std::vector<BaseModule*>   _vecModules;     // ģ��ָ��
    std::vector<BaseModule*> LoadEx(LPCTSTR lpszModulePath);
    void LoadModules(VString &modules);
    CDXString m_strConfigFile;
#pragma warning(default:4251)
};
#endif // ModuleManager_h__