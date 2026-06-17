#pragma once
#include "chart/Chart.h"
#include <string>

// 文件路径工具类，处理导出路径的拼接、规范化、后缀添加和目录创建
// 支持绝对路径和相对路径，自动补全 .png 后缀，并递归创建父目录
class FilePathUtils
{
public:
    // 去除首尾空白
    static tstring trim(const tstring& text);
    // 确保路径以 .png 结尾，否则自动追加
    static tstring ensurePngExtension(const tstring& filePath);
    // 将名称中的非法文件名字符替换为下划线
    static tstring safeFileName(const tstring& name);
    // 拼接目录和文件名
    static tstring joinPath(const tstring& directory, const tstring& fileName);
    // 获取路径的父目录
    static tstring parentDirectory(const tstring& filePath);
    // 确保目录存在（递归创建）
    static bool ensureDirectoryExists(const tstring& directory);
    // 确保父目录存在
    static bool ensureParentDirectoryExists(const tstring& filePath);
    // 判断路径是否以分隔符结尾
    static bool endsWithSeparator(const tstring& path);

    // 构建单图表导出路径：若输入为目录则拼接文件名，否则直接使用
    static tstring buildSingleChartPath(const tstring& inputPath,
                                        const tstring& title,
                                        const tstring& suffix);

    // 构建批量导出路径：输入为目录时拼接文件名，输入为 .png 时插入后缀
    static tstring buildExportAllChartPath(const tstring& inputPath,
                                           const tstring& title,
                                           const tstring& suffix);
};
