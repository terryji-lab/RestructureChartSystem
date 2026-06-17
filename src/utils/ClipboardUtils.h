#pragma once
#include <windows.h>
#include "../common/Types.h"

// Win32 剪贴板操作工具函数
namespace ClipboardUtils {

// 将文本写入系统剪贴板
void copyToClipboard(const tstring& text, HWND hwnd);

// 从系统剪贴板读取文本
tstring pasteFromClipboard(HWND hwnd);

}
