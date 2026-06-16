#include "FilePathUtils.h"
#include <windows.h>
#include <tchar.h>
#include <cctype>

static bool isPathSeparator(TCHAR ch)
{
    return ch == _T('\\') || ch == _T('/');
}

static tstring normalizeSeparators(tstring path)
{
    for (size_t i = 0; i < path.length(); ++i)
    {
        if (path[i] == _T('/')) path[i] = _T('\\');
    }
    return path;
}

static bool hasPngExtension(const tstring& path)
{
    if (path.length() < 4) return false;
    tstring ext = path.substr(path.length() - 4);
    for (size_t i = 0; i < ext.length(); ++i)
        ext[i] = (TCHAR)_totlower(ext[i]);
    return ext == _T(".png");
}

static bool directoryExists(const tstring& directory)
{
    if (directory.empty()) return true;
    DWORD attrs = GetFileAttributes(directory.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool isDriveRoot(const tstring& path)
{
    return path.length() == 3 && path[1] == _T(':') && isPathSeparator(path[2]);
}

tstring FilePathUtils::trim(const tstring& text)
{
    size_t begin = 0;
    while (begin < text.length() && (text[begin] == _T(' ') || text[begin] == _T('\t') || text[begin] == _T('\r') || text[begin] == _T('\n')))
        ++begin;

    size_t end = text.length();
    while (end > begin && (text[end - 1] == _T(' ') || text[end - 1] == _T('\t') || text[end - 1] == _T('\r') || text[end - 1] == _T('\n')))
        --end;

    return text.substr(begin, end - begin);
}

tstring FilePathUtils::ensurePngExtension(const tstring& filePath)
{
    tstring path = trim(filePath);
    if (path.empty() || endsWithSeparator(path)) return path;
    if (hasPngExtension(path)) return path;
    return path + _T(".png");
}

tstring FilePathUtils::safeFileName(const tstring& name)
{
    tstring result = trim(name);
    if (result.empty()) result = _T("Chart");

    const tstring invalid = _T("\\/:*?\"<>|");
    for (size_t i = 0; i < result.length(); ++i)
    {
        if (invalid.find(result[i]) != tstring::npos)
            result[i] = _T('_');
    }
    return result;
}

tstring FilePathUtils::joinPath(const tstring& directory, const tstring& fileName)
{
    tstring dir = trim(directory);
    tstring file = trim(fileName);
    if (dir.empty()) return file;
    if (file.empty()) return dir;
    if (endsWithSeparator(dir)) return dir + file;
    return dir + _T("\\") + file;
}

tstring FilePathUtils::parentDirectory(const tstring& filePath)
{
    tstring path = normalizeSeparators(trim(filePath));
    if (path.empty()) return _T("");

    size_t pos = path.find_last_of(_T("\\/"));
    if (pos == tstring::npos) return _T("");

    if (pos == 2 && path.length() >= 3 && path[1] == _T(':'))
        return path.substr(0, 3);

    if (pos == 0) return path.substr(0, 1);
    return path.substr(0, pos);
}

bool FilePathUtils::ensureDirectoryExists(const tstring& directory)
{
    tstring path = normalizeSeparators(trim(directory));
    if (path.empty()) return true;

    while (path.length() > 1 && isPathSeparator(path[path.length() - 1]) && !isDriveRoot(path))
        path.erase(path.length() - 1);

    if (directoryExists(path)) return true;

    tstring current;
    size_t pos = 0;

    if (path.length() >= 2 && path[1] == _T(':'))
    {
        current = path.substr(0, 2);
        pos = 2;
        if (pos < path.length() && isPathSeparator(path[pos]))
        {
            current += _T("\\");
            ++pos;
        }
    }
    else if (!path.empty() && isPathSeparator(path[0]))
    {
        current = _T("\\");
        pos = 1;
    }

    while (pos < path.length())
    {
        while (pos < path.length() && isPathSeparator(path[pos])) ++pos;
        if (pos >= path.length()) break;

        size_t next = path.find(_T('\\'), pos);
        tstring part = (next == tstring::npos) ? path.substr(pos) : path.substr(pos, next - pos);

        if (!current.empty() && !endsWithSeparator(current)) current += _T("\\");
        current += part;

        if (!directoryExists(current))
        {
            if (!CreateDirectory(current.c_str(), NULL))
            {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS) return false;
            }
        }

        if (next == tstring::npos) break;
        pos = next + 1;
    }

    return directoryExists(path);
}

bool FilePathUtils::ensureParentDirectoryExists(const tstring& filePath)
{
    return ensureDirectoryExists(parentDirectory(filePath));
}

bool FilePathUtils::endsWithSeparator(const tstring& path)
{
    if (path.empty()) return false;
    return isPathSeparator(path[path.length() - 1]);
}

tstring FilePathUtils::buildSingleChartPath(const tstring& inputPath,
                                            const tstring& title,
                                            const tstring& suffix)
{
    tstring path = trim(inputPath);
    tstring defaultName = safeFileName(title) + suffix;

    if (path.empty()) return _T("");
    if (endsWithSeparator(path)) return joinPath(path, defaultName);
    return ensurePngExtension(path);
}

tstring FilePathUtils::buildExportAllChartPath(const tstring& inputPath,
                                               const tstring& title,
                                               const tstring& suffix)
{
    tstring path = trim(inputPath);
    tstring fileName = safeFileName(title) + suffix;

    if (path.empty()) return _T("");
    if (endsWithSeparator(path)) return joinPath(path, fileName);

    if (hasPngExtension(path))
    {
        tstring base = path.substr(0, path.length() - 4);
        return base + suffix;
    }

    return joinPath(path, fileName);
}
