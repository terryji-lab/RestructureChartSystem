#pragma once

// 公共类型定义，自适应字符类型，让项目在Unicode和ANSI编译模式下都能运行
// 项目中所有 tstring 统一定义于此。
// 显式包含 <windows.h> 确保 TCHAR 在任意 EasyX 版本 / 编译器下都可用，
// 不再依赖 easyx.h 间接拉入 windows.h。

#include <windows.h>
#include <string>
#include <sstream>

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;
