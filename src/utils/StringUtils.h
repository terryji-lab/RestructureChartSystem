#pragma once
#include <string>
#include <windows.h>
#include "../common/Types.h"

// 去除 std::string 首尾空白字符
std::string trimString(const std::string& s);

// 将 UTF-8/ANSI std::string 转换为 tstring
// Unicode 编译时使用 MultiByteToWideChar，ANSI 编译时直接返回
tstring stringToTString(const std::string& s);
