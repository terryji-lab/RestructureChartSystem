#include "FileDataReader.h"
#include "StringUtils.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cctype>
#include <windows.h>
#include <tchar.h>

// 构造/析构

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

// CSV 文件加载与解析
// 流程：打开文件 → 跳过 BOM → 逐行按逗号分割 → 解析数字 → 存入 data 向量
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
        if (line.empty()) continue;

        // 按逗号分割
        std::stringstream ss(line);

        std::string name;
        std::string valueStr;

        if (!getline(ss, name, ',')) continue;
        if (!getline(ss, valueStr)) continue;

        name = trimString(name);
        valueStr = trimString(valueStr);

        // 跳过空字段
        if (name.empty() || valueStr.empty()) continue;

        // 尝试将值字段解析为浮点数
        char* endPtr = NULL;
        double value = std::strtod(valueStr.c_str(), &endPtr);

        // 如果解析失败（endPtr 指向字符串开头），说明这是标题行
        if (endPtr == valueStr.c_str()) continue;

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
