#pragma once
#include "LineChart.h"

// 面积图类
class AreaChart : public LineChart
{
private:
    COLORREF fillColor;

public:
    AreaChart(const tstring& chartTitle,
              const std::vector<ChartItem>& chartData,
              int x = 20, int y = 20,
              int w = 960, int h = 580);

    void draw() override;
    void applyTheme(const ColorTheme& theme) override;
};
