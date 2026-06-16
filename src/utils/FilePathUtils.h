#pragma once
#include "chart/Chart.h"
#include <string>

// Utility class for export path handling. It supports both relative and absolute
// paths, adds .png automatically when needed, and creates parent folders before
// saving exported chart images.
class FilePathUtils
{
public:
    static tstring trim(const tstring& text);
    static tstring ensurePngExtension(const tstring& filePath);
    static tstring safeFileName(const tstring& name);
    static tstring joinPath(const tstring& directory, const tstring& fileName);
    static tstring parentDirectory(const tstring& filePath);
    static bool ensureDirectoryExists(const tstring& directory);
    static bool ensureParentDirectoryExists(const tstring& filePath);
    static bool endsWithSeparator(const tstring& path);

    static tstring buildSingleChartPath(const tstring& inputPath,
                                        const tstring& title,
                                        const tstring& suffix);

    static tstring buildExportAllChartPath(const tstring& inputPath,
                                           const tstring& title,
                                           const tstring& suffix);
};
