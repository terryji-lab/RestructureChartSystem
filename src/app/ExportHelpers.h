#pragma once
#include "chart/Chart.h"
#include "ui/Pages.h"

// 导出当前图表（带路径对话框和结果提示）
void exportCurrentChart(const ColorTheme& theme,
                        ChartType currentChart,
                        const tstring& loadedTitle,
                        const std::vector<ChartItem>& loadedData);

// 批量导出全部 4 种图表
void exportAllCharts(const ColorTheme& theme,
                     const tstring& loadedTitle,
                     const std::vector<ChartItem>& loadedData);
