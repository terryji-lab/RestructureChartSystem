#include "Chart.h"

//构造函数
Chart::Chart(const tstring& chartTitle,
             const std::vector<ChartItem>& chartData,
             int x, int y, int w, int h)
    : title(chartTitle), data(chartData)
    , leftX(x), topY(y), chartWidth(w), chartHeight(h) {}

//设置数据
void Chart::setData(const std::vector<ChartItem>& chartData)
{
    data = chartData;
}

//设置标题
void Chart::setTitle(const tstring& chartTitle)
{
    title = chartTitle;
}

//析构函数
Chart::~Chart() {}
