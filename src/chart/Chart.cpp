#include "Chart.h"

// ==================== Chart 抽象基类实现 ====================
// Chart 是所有图表的公共基类，定义了数据、标题、位置等公共属性。
// 具体的绘制逻辑由各子类（BarChart、PieChart 等）的 draw() 实现。

// 构造函数：初始化图表标题、数据、位置和尺寸
Chart::Chart(const tstring& chartTitle,
             const std::vector<ChartItem>& chartData,
             int x, int y, int w, int h)
    : title(chartTitle), data(chartData)
    , leftX(x), topY(y), chartWidth(w), chartHeight(h) {}

// 设置图表数据（替换现有数据）
void Chart::setData(const std::vector<ChartItem>& chartData)
{
    data = chartData;
}

// 设置图表标题
void Chart::setTitle(const tstring& chartTitle)
{
    title = chartTitle;
}

// 虚析构函数（保证子类正确析构）
Chart::~Chart() {}
