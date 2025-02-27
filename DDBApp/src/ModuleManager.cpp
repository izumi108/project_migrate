/********************************************************************
    created:    2012/12/27 15:40
    filename:   ModuleManager.cpp
    author:     QiuChang

    purpose:    模块管理类
*********************************************************************/
#include "stdafx.h"
#include "ModuleManager.h"
#include "BaseModule.h"
#include "File/IniFile.h"

typedef void*   PMODULE;
typedef int (*GetModulesCount)();
typedef void* (*CreateModule)(int count);
typedef void* (*ReleaseModule)(PMODULE p);

//void ModuleManager::LoadModules(DuiLib::CContainerUI* pLeftPanel, DuiLib::CContainerUI* pMainPanel)
//{
// 加载模块逻辑
/*
1.打开ini配置文件，从指定节中读取依赖模块列表
2.将读取到的dll文件路径存储到vector
3.遍历vector数据，加载模块dll
4.加载指定模块中的Resouce（暂未实现）
*/

bool ModuleManager::Require(LPCTSTR lpszModuleName)
{
    CDXString strModuleName(lpszModuleName);
    size_t nPos = strModuleName.rfind(_T('/'));
    CDXString strFileName;
    CDXString strPath;

    if(nPos != -1)
    {
        strFileName = strModuleName.substr(nPos + 1);
        strPath = strModuleName.substr(0, nPos + 1);
    }
    else
    {
        strFileName = lpszModuleName;
    }

    if(! strPath.empty())
    {
        BOOL bRet = ::SetDllDirectory(strPath.c_str());
        int i = 0;
    }

    BaseModule* pRequireModule = GetModule(strFileName.c_str());

    if(pRequireModule)
    {
        return true;
    }

    if(this->LoadEx(strFileName.c_str()).size() == 0)
    {
        // 加载Dll返回的导出接口数量为0，加载dll失败了
        return false;
    }

    return true;
}

//  LOG->Write(Log::LOG_DEBUG, L"Load Moduels start");
//
//      // EXE路径
//      TCHAR szPath[MAX_PATH];
//      TCHAR szDllsPath[MAX_PATH];
//      ::GetModuleFileName(NULL, szPath, MAX_PATH);
//      TCHAR *pLastBias = NULL;
//      if (pLastBias = _tcsrchr(szPath, '\\'))
//          *(pLastBias + 1) = '\0';
//      _tcscpy(szDllsPath,szPath);
//      _tcscat(szDllsPath,_T("Dlls\\"));
//
//      vector<wstring> folers;
//      vector<wstring> files;
//
//      CFileOperation::FindAllFolderInDir(szDllsPath,folers);
//
//      if(!folers.empty())
//      {
//          wstring strModules = CFileOperation::GetIniFileValue(CONFIG(_strOptionIni), L"MainModule", L"modules");
//          for (vector<wstring>::iterator item=folers.begin();item!=folers.end();++item)
//          {
//              wstring strFolderName = CFileOperation::GetFileName(*item);
//              if (strModules.find(CStrOperation::toLowerW(strFolderName)) == wstring::npos)
//                  continue;
//
//              LOG->Write(Log::LOG_DEBUG, L"Load Moduels folders %s",(*item).c_str());
//
//              CFileOperation::FindAllFileInDir(*item, files, _T("dll"));
//              for(vector<wstring>::iterator it = files.begin(); it != files.end(); ++it)
//              {
//                  vector<BaseModule*> modules=Load(*item,*it);
//                  if(modules.empty())
//                  {
//                      LOG->Write(Log::LOG_DEBUG, L"Load Moduels empty");
//                      continue;
//                  }
//
//                  for (vector<BaseModule*>::iterator i=modules.begin();i!=modules.end();++i)
//                  {
//                      NetModule* pModule = dynamic_cast<NetModule*>(*i);
//                      if (!pModule)
//                      {
//                          LOG->Write(Log::LOG_DEBUG, L"Load Moduel is NULL");
//                          continue;
//                      }
//
//                      LoadResource(*item, pModule);
//                      //用户区域模块需特殊处理
//                      if(pModule->Id()!=_T("UserCenter"))
//                          pModule->Init(pLeftPanel, pMainPanel);
//                  }
//              }
//          }
//      }
//
//      LOG->Write(Log::LOG_DEBUG, L"Load Moduels start");
//}

void ModuleManager::RequireModules(LPCTSTR lpszIniFilePath)
{
    m_strConfigFile = lpszIniFilePath;
    DDBUtils::CIniFile iniFile(m_strConfigFile.c_str());
    CString strValue;
    VString modules;

    strValue = iniFile.GetString(_T("Base"), _T("modules"));
    modules = DDBAPI_SplitStr(strValue, _T(';'));
    LoadModules(modules);

    strValue = iniFile.GetString(_T("Plugin"), _T("modules"));
    modules = DDBAPI_SplitStr(strValue, _T(';'));
    LoadModules(modules);

    strValue = iniFile.GetString(_T("Submodule"), _T("modules"));
    modules = DDBAPI_SplitStr(strValue, _T(';'));
    LoadModules(modules);
}

void ModuleManager::InitUserInterface(DuiLib::CWindowUI* pWindow)
{
    size_t nCount = _vecModules.size();

    for(size_t i = 0; i < nCount; ++i)
    {
        BaseModule* pModule = _vecModules[i];
        pModule->Init(pWindow);
    }
}

void ModuleManager::LoadModules(VString &modules)
{
    size_t nModuleCount = modules.size();

    if(nModuleCount > 0)
    {
        for(size_t i = 0; i < nModuleCount; ++i)
        {
            this->LoadEx(modules[i].GetString());
        }
    }
}

int ModuleManager::GetModuleCount()
{
    return _vecModules.size();
}

ModuleManager::ModuleManager()
{
}

ModuleManager::~ModuleManager()
{
    for(size_t i = 0; i < _vecDlls.size(); i++)
    {
        HMODULE dll = _vecDlls[i];

        if(dll)
        {
            ReleaseModule release = (ReleaseModule)GetProcAddress(dll, "ReleaseModule");

            if(release)
            {
                release(_vecModules[i]);
            }
        }
    }
}

BaseModule* ModuleManager::GetModule(LPCTSTR lpszName)
{
    if(lpszName == NULL || lpszName[0] == '\0')
    {
        return NULL;
    }

    size_t nCompareCount = _tcslen(lpszName);

    for(vector<BaseModule*>::iterator it = _vecModules.begin(); it != _vecModules.end(); it++)
    {
        BaseModule* module = *it;

        if(module)
        {
            LPCTSTR lpszIdentity = module->Identity();

            if(_tcsncicmp(lpszName, lpszIdentity, nCompareCount) == 0)
            {
                return module;
            }
        }
    }

    return NULL;
}

std::vector<BaseModule*> ModuleManager::LoadEx(LPCTSTR lpszModulePath)
{
    vector<BaseModule*> tempMoudles;

    HMODULE hDll = ::LoadLibrary(lpszModulePath);

    if(hDll)
    {
        GetModulesCount func = (GetModulesCount)::GetProcAddress(hDll, "GetModulesCount");

        if(func)
        {
            int count = func();

            for(int index = 0; index < count; ++index)
            {
                CreateModule func = (CreateModule)::GetProcAddress(hDll, "CreateModule");

                if(!func)
                {
                    continue;
                }

                BaseModule* pModule = reinterpret_cast<BaseModule*>(func(index));

                if(!pModule)
                {
                    continue;
                }

                tempMoudles.push_back(pModule);

                _vecDlls.push_back(hDll);
                _vecModules.push_back(pModule);
            }
        }
    }

    return tempMoudles;
}

vector<BaseModule*> ModuleManager::GetModules()
{
    return _vecModules;
}

std::vector<BaseModule*> ModuleManager::GetModulesByFilter(LPCTSTR lpszFilter)
{
    vector<BaseModule*> filterModules;
    size_t nModulesCount = _vecModules.size();
    size_t nCompareCount = _tcslen(lpszFilter);

    for(size_t i = 0; i < nModulesCount; ++i)
    {
        BaseModule* pModule = _vecModules[i];
        LPCTSTR lpszIdentity = pModule->Identity();

        if(_tcsncicmp(lpszFilter, lpszIdentity, nCompareCount) == 0)
        {
            // 命中的模块
            filterModules.push_back(pModule);
        }
    }

    return filterModules;
}

