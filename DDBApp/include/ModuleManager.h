#ifndef ModuleManager_h__
#define ModuleManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称 :   ModuleManager.h
// 创建人    :   daviyang35@QQ.com
// 创建时间 :   2014-07-05 12:32:45
// 说明       :   模块管理器
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
/* 模块管理类                                                   */
/************************************************************************/
class SINGLETON_API ModuleManager
{
    friend class Singleton;
    typedef std::vector<CString> VString;
public:

    // Summary  : 加载模块
    // Parameter: CContainerUI * pLeftPanel 左面板容器
    // Parameter: CContainerUI * pMainPanel 主面板容器
    //void LoadModules(CContainerUI* pLeftPanel, CContainerUI* pMainPanel);

    // 加载指定路径的模块
    // 优先检查已加载实例，如果不存在则搜索并导入
    // 返回值如果为false，说明实例与对应的dll都不存在
    bool Require(LPCTSTR lpszModuleName);

    // 加载ini文件指定模块
    // 参数：pWindow 用于显示的主窗口指针
    void RequireModules(LPCTSTR lpszIniFilePath);

    // 初始化UI
    void InitUserInterface(DuiLib::CWindowUI* pWindow);

    // Summary  : 取得模块数
    // Returns  : 模块数
    int GetModuleCount();

    // Summary  : 取得模块指针
    // Returns  : 模块指针b
    // Parameter: int index 第几个模块
    BaseModule* GetModule(int index)
    {
        return _vecModules.at(index);
    }

    // Summary  : 取得模块指针
    // Returns  : 模块指针
    // Parameter: wstring id 模块ID
    BaseModule* GetModule(LPCTSTR lpszName);

    // Summary  : 取得所有模块指针列表
    std::vector<BaseModule*> GetModules();

    // Summary  : 取得指定类型模块指针列表
    std::vector<BaseModule*> GetModulesByFilter(LPCTSTR lpszFilter);

private:
    ModuleManager();
    ~ModuleManager();
#pragma warning(disable:4251)
    std::vector<HMODULE>       _vecDlls;                // DLL句柄
    std::vector<BaseModule*>   _vecModules;     // 模块指针
    std::vector<BaseModule*> LoadEx(LPCTSTR lpszModulePath);
    void LoadModules(VString &modules);
    CDXString m_strConfigFile;
#pragma warning(default:4251)
};
#endif // ModuleManager_h__