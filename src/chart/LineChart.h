#pragma once
#include "Chart.h"
#include <vector>

// 折线图类
class LineChart : public Chart
{
protected:
    COLORREF lineColor;
    COLORREF pointColor;
    COLORREF axisColor;
    COLORREF gridColor;
    COLORREF textColor;

    double getMaxValue() const;
    void drawTitle();
    void drawAxis();
    void drawGrid();

public:
    LineChart(const tstring& chartTitle,
              const std::vector<ChartItem>& chartData,
              int x = 20, int y = 20,
              int w = 960, int h = 580);

    void draw() override;
    void applyTheme(const ColorTheme& theme) override;
};
