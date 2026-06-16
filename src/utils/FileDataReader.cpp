#include "FileDataReader.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cctype>
#include <windows.h>
#include <tchar.h>

static std::string trimString(const std::string& s)
{
    size_t start = 0;

    while (start < s.size() && std::isspace((unsigned char)s[start]))
    {
        start++;
    }

    size_t end = s.size();

    while (end > start && std::isspace((unsigned char)s[end - 1]))
    {
        end--;
    }

    return s.substr(start, end - start);
}

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

tstring FileDataReader::stringToTString(const std::string& s)
{
#ifdef UNICODE
    if (s.empty())
    {
        return _T("");
    }

    int size = MultiByteToWideChar(
        CP_UTF8,
        0,
        s.c_str(),
        -1,
        NULL,
        0
    );

    UINT codePage = CP_UTF8;

    // If UTF-8 fails, use system default code page
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
    return s;
#endif
}

bool FileDataReader::load()
{
    data.clear();
    errorMessage = _T("");

    if (filePath.empty())
    {
        errorMessage = _T("File path is empty.");
        return false;
    }

    FILE* fp = _tfopen(filePath.c_str(), _T("rb"));

    if (fp == NULL)
    {
        errorMessage = _T("Failed to open file. Please check the file path.");
        return false;
    }

    char buffer[1024];
    bool firstLine = true;

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        std::string line = buffer;

        // Remove line break
        while (!line.empty() &&
            (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
        {
            line.erase(line.size() - 1);
        }

        // Remove UTF-8 BOM
        if (firstLine)
        {
            firstLine = false;

            if (line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF &&
                (unsigned char)line[1] == 0xBB &&
                (unsigned char)line[2] == 0xBF)
            {
                line = line.substr(3);
            }
        }

        line = trimString(line);

        if (line.empty())
        {
            continue;
        }

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

        if (name.empty() || valueStr.empty())
        {
            continue;
        }

        char* endPtr = NULL;
        double value = std::strtod(valueStr.c_str(), &endPtr);

        // Skip header line, such as: name,value
        if (endPtr == valueStr.c_str())
        {
            continue;
        }

        ChartItem item;
        item.name = stringToTString(name);
        item.value = value;

        data.push_back(item);
    }

    fclose(fp);

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
