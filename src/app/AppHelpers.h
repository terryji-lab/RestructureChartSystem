#pragma once
#include "chart/Chart.h"
#include "ui/Pages.h"

// 图表类型 → PNG 后缀
const TCHAR* getChartSuffix(ChartType type);

// 超长文本截断（用于弹窗显示）
tstring shortenForPopup(const tstring& text, size_t maxLen);

// 显示主题弹窗（成功/错误提示）
void showPopup(const ColorTheme& theme,
               const tstring& title,
               const tstring& message,
               const tstring& detail = _T(""),
               bool isError = false);

// 弹出导出路径对话框，返回用户是否确认
bool promptExportPath(const ColorTheme& theme,
                      const tstring& title,
                      const tstring& message,
                      const tstring& placeholder,
                      const tstring& defaultPath,
                      tstring& outPath);

// CSV 加载 + 结果弹窗
// 返回 true 表示加载成功，并填充 outData / outTitle
bool tryLoadCSV(const tstring& path,
                std::vector<ChartItem>& outData,
                tstring& outTitle,
                const ColorTheme& theme);
