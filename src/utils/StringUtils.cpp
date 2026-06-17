#include "StringUtils.h"
#include <cctype>

// 去除字符串首尾空白
std::string trimString(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start]))
        start++;

    size_t end = s.size();
    while (end > start && std::isspace((unsigned char)s[end - 1]))
        end--;

    return s.substr(start, end - start);
}

// 将 std::string 转换为 tstring
tstring stringToTString(const std::string& s)
{
#ifdef UNICODE
    if (s.empty()) return _T("");

    int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    UINT codePage = CP_UTF8;

    if (size <= 0)
    {
        codePage = CP_ACP;
        size = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);
    }

    if (size <= 0) return _T("");

    std::wstring result(size - 1, L'\0');
    MultiByteToWideChar(codePage, 0, s.c_str(), -1, &result[0], size);
    return result;
#else
    return s;
#endif
}
