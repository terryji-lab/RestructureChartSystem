#include "Pages.h"
#include "chart/BarChart.h"
#include "chart/PieChart.h"
#include "chart/LineChart.h"
#include "chart/AreaChart.h"

// 图表工厂函数，根据 ChartType 枚举创建对应的图表子类实例，并应用主题配色
// 所有图表使用统一的绘图区域：x=80, y=60, w=1040, h=680
std::unique_ptr<Chart> createChart(
    ChartType type,
    const tstring& title,
    const std::vector<ChartItem>& data,
    const ColorTheme& theme
)
{
    std::unique_ptr<Chart> chart;
    switch(type)
    {
    case CHART_BAR:
        chart = std::make_unique<BarChart>(title, data, 80, 60, 1040, 680);
        break;
    case CHART_PIE:
        chart = std::make_unique<PieChart>(title, data, 80, 60, 1040, 680, 280);
        break;
    case CHART_LINE:
        chart = std::make_unique<LineChart>(title, data, 80, 60, 1040, 680);
        break;
    case CHART_AREA:
        chart = std::make_unique<AreaChart>(title, data, 80, 60, 1040, 680);
        break;
    default:
        return nullptr;
    }
    chart->applyTheme(theme);
    return chart;
}
