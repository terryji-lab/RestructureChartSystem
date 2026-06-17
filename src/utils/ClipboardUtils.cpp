#include "ClipboardUtils.h"
#include <tchar.h>

namespace ClipboardUtils {

// 将文本复制到系统剪贴板
// 通过 GlobalAlloc 分配全局内存，适配 Unicode/ANSI 两种编译模式
void copyToClipboard(const tstring& text, HWND hwnd)
{
    if (text.empty() || !OpenClipboard(hwnd)) return;

    EmptyClipboard();

    size_t size = (text.length() + 1) * sizeof(TCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hMem)
    {
        TCHAR* pMem = (TCHAR*)GlobalLock(hMem);
        if (pMem)
        {
            _tcscpy_s(pMem, text.length() + 1, text.c_str());
            GlobalUnlock(hMem);
        }
#ifdef UNICODE
        SetClipboardData(CF_UNICODETEXT, hMem);
#else
        SetClipboardData(CF_TEXT, hMem);
#endif
    }

    CloseClipboard();
}

// 从系统剪贴板读取文本
// 先检查剪贴板格式是否可用，再打开读取
tstring pasteFromClipboard(HWND hwnd)
{
    tstring result;

#ifdef UNICODE
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return result;
#else
    if (!IsClipboardFormatAvailable(CF_TEXT)) return result;
#endif

    if (!OpenClipboard(hwnd)) return result;

#ifdef UNICODE
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
#else
    HANDLE hData = GetClipboardData(CF_TEXT);
#endif

    if (hData)
    {
        TCHAR* pData = (TCHAR*)GlobalLock(hData);
        if (pData)
        {
            result = pData;
            GlobalUnlock(hData);
        }
    }

    CloseClipboard();
    return result;
}

}
