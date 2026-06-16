#pragma once

// ==================== 公共类型定义 ====================
// 项目中所有 tstring 统一定义于此。
// 显式包含 <windows.h> 确保 TCHAR 在任意 EasyX 版本 / 编译器下都可用，
// 不再依赖 easyx.h 间接拉入 windows.h。

#include <windows.h>
#include <string>

typedef std::basic_string<TCHAR> tstring;
