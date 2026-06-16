#include "FileDataReader.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cctype>
#include <windows.h>
#include <tchar.h>

// ==================== 去除字符串首尾空白 ====================
// 用于清洗 CSV 中每个字段的前后空格
static std::string trimString(const std::string& s)
{
    size_t start = 0;

    // 跳过前导空白
    while (start < s.size() && std::isspace((unsigned char)s[start]))
    {
        start++;
    }

    size_t end = s.size();

    // 跳过尾部空白
    while (end > start && std::isspace((unsigned char)s[end - 1]))
    {
        end--;
    }

    return s.substr(start, end - start);
}

// ==================== 构造/析构 ====================

FileDataReader::FileDataReader()
{
    filePath = _T("");
    errorMessage = _T("");
}

FileDataReader::FileDataReader(const tstring& path)
{
    filePath = path;
    errorMessage = _T("");
}

void FileDataReader::setFilePath(const tstring& path)
{
    filePath = path;
}

tstring FileDataReader::getFilePath() const
{
    return filePath;
}

// ==================== 字符串编码转换 ====================
// 将 std::string（UTF-8 或 ANSI）转换为 tstring（Unicode 时为 wstring）
// 优先尝试 UTF-8 编码，失败时回退到系统默认 ANSI 代码页
tstring FileDataReader::stringToTString(const std::string& s)
{
#ifdef UNICODE
    if (s.empty())
    {
        return _T("");
    }

    // 先尝试 UTF-8 编码
    int size = MultiByteToWideChar(
        CP_UTF8,
        0,
        s.c_str(),
        -1,
        NULL,
        0
    );

    UINT codePage = CP_UTF8;

    // 如果 UTF-8 转换失败，使用系统默认代码页（ANSI）
    if (size <= 0)
    {
        codePage = CP_ACP;
        size = MultiByteToWideChar(
            CP_ACP,
            0,
            s.c_str(),
            -1,
            NULL,
            0
        );
    }

    if (size <= 0)
    {
        return _T("");
    }

    // size 包含了结尾的 '\0'，所以实际字符数是 size-1
    std::wstring result(size - 1, L'\0');

    MultiByteToWideChar(
        codePage,
        0,
        s.c_str(),
        -1,
        &result[0],
        size
    );

    return result;
#else
    // ANSI 编译时无需转换，直接返回
    return s;
#endif
}

// ==================== CSV 文件加载与解析 ====================
// 解析流程：
//   1. 以二进制模式打开文件
//   2. 跳过 UTF-8 BOM（0xEF 0xBB 0xBF）
//   3. 逐行读取，按逗号分割为 name 和 value
//   4. 尝试将 value 解析为数字，解析失败则跳过该行（标题行）
//   5. 将有效的 name-value 对存入 data 向量
bool FileDataReader::load()
{
    data.clear();
    errorMessage = _T("");

    if (filePath.empty())
    {
        errorMessage = _T("File path is empty.");
        return false;
    }

    // 以二进制模式打开（避免 CRT 对换行符的处理影响判断）
    FILE* fp = _tfopen(filePath.c_str(), _T("rb"));

    if (fp == NULL)
    {
        errorMessage = _T("Failed to open file. Please check the file path.");
        return false;
    }

    char buffer[1024];
    bool firstLine = true;  // 第一行需要特殊处理 BOM

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        std::string line = buffer;

        // 去除行尾换行符（\n 或 \r\n）
        while (!line.empty() &&
            (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
        {
            line.erase(line.size() - 1);
        }

        // 处理 UTF-8 BOM（仅第一行）
        if (firstLine)
        {
            firstLine = false;

            if (line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF &&
                (unsigned char)line[1] == 0xBB &&
                (unsigned char)line[2] == 0xBF)
            {
                line = line.substr(3);  // 去掉 BOM 标记
            }
        }

        // 去除首尾空格
        line = trimString(line);

        // 跳过空行
        if (line.empty())
        {
            continue;
        }

        // 按逗号分割
        std::stringstream ss(line);

        std::string name;
        std::string valueStr;

        if (!getline(ss, name, ','))
        {
            continue;
        }

        if (!getline(ss, valueStr))
        {
            continue;
        }

        name = trimString(name);
        valueStr = trimString(valueStr);

        // 跳过空字段
        if (name.empty() || valueStr.empty())
        {
            continue;
        }

        // 尝试将值字段解析为浮点数
        char* endPtr = NULL;
        double value = std::strtod(valueStr.c_str(), &endPtr);

        // 如果解析失败（endPtr 指向字符串开头），说明这是标题行
        // 例如：name,value 这一行会被跳过
        if (endPtr == valueStr.c_str())
        {
            continue;
        }

        // 构造 ChartItem 并添加到结果集
        ChartItem item;
        item.name = stringToTString(name);
        item.value = value;

        data.push_back(item);
    }

    fclose(fp);

    // 检查是否有有效数据
    if (data.empty())
    {
        errorMessage = _T("No valid chart data found in CSV file.");
        return false;
    }

    return true;
}

bool FileDataReader::loadFromPath(const tstring& path)
{
    setFilePath(path);
    return load();
}

std::vector<ChartItem> FileDataReader::getData() const
{
    return data;
}

tstring FileDataReader::getErrorMessage() const
{
    return errorMessage;
}

void FileDataReader::clear()
{
    filePath = _T("");
    errorMessage = _T("");
    data.clear();
}
