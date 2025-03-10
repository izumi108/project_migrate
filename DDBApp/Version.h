/****************************************************************
版本号生成规则：
    MajorVersion.MinorVersion.FixedVersion.SVNVersion
MajorVersion "3"
MinorVersion "2"
FixedVersion "0"
SVNVersion   "SVN版本号" 假设发布时SVN版本号为189
    3.2.0.189

MajorVersion 主版本号，大的版本重构发布
MinorVersion 次版本号，小的模块重构，功能修改等
FixedVersion 修订版本号，外发,Bug修复+N  N最小为1
SVNVersion   这个版本号自动管理，无需介入

发布日志：


****************************************************************/
#pragma once
#include "../../base/Global/VersionDef.h"

// 来自公共引用头文件
//#define VER_GLOBAL_CompanyName "成都网丁科技有限公司"
//#define VER_GLOBAL_LegalCopyright "Copyright (C) 2009-2015.成都网丁科技有限公司"

#define VER_CompanyName         VER_GLOBAL_CompanyName
#define VER_LegalCopyright      VER_GLOBAL_LegalCopyright
#define VER_FileDescription     "DLL功能扩展"

#define MajorVersion    1
#define MinorVersion    1
#define FixedVersion    3

