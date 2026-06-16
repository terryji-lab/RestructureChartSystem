#include "Chart.h"

Chart::Chart(const tstring& chartTitle,
             const std::vector<ChartItem>& chartData,
             int x, int y, int w, int h)
    : title(chartTitle), data(chartData)
    , leftX(x), topY(y), chartWidth(w), chartHeight(h) {}

void Chart::setData(const std::vector<ChartItem>& chartData)
{
    data = chartData;
}

void Chart::setTitle(const tstring& chartTitle)
{
    title = chartTitle;
}

Chart::~Chart() {}
