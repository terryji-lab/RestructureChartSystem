#pragma once
#include "chart/Chart.h"
#include <vector>

// ==================== CSV 文件数据读取类 ====================
// 负责从 CSV 文件中解析图表数据。支持 UTF-8 和 ANSI 编码，
// 自动跳过 BOM 和标题行。每行格式为：名称,数值
//
// 使用方式：
//   FileDataReader reader;
//   if (reader.loadFromPath(_T("data.csv"))) {
//       auto data = reader.getData();
//   } else {
//       auto err = reader.getErrorMessage();
//   }
class FileDataReader
{
private:
    tstring filePath;                     // CSV 文件路径
    std::vector<ChartItem> data;          // 解析后的图表数据
    tstring errorMessage;                 // 错误信息（加载失败时设置）

private:
    // 将 UTF-8/ANSI 字符串转换为 TCHAR 字符串
    // Unicode 编译时使用 MultiByteToWideChar，ANSI 编译时直接返回
    static tstring stringToTString(const std::string& s);

public:
    FileDataReader();
    FileDataReader(const tstring& path);

    // 设置 CSV 文件路径（绝对路径，从 GUI 输入框获取）
    void setFilePath(const tstring& path);

    // 获取当前设置的文件路径
    tstring getFilePath() const;

    // 使用当前 filePath 加载并解析 CSV 文件
    // @return 成功返回 true，失败可通过 getErrorMessage() 获取原因
    bool load();

    // 设置路径并立即加载（等同于 setFilePath + load）
    bool loadFromPath(const tstring& path);

    // 获取加载后的图表数据
    std::vector<ChartItem> getData() const;

    // 获取错误信息（仅在 load/loadFromPath 返回 false 时有效）
    tstring getErrorMessage() const;

    // 清空旧数据和路径
    void clear();
};
