#pragma once
#include "Chart.h"
#include <vector>

// ==================== 折线图类 ====================
class LineChart : public Chart
{
protected:
    COLORREF lineColor;
    COLORREF pointColor;
    COLORREF axisColor;
    COLORREF gridColor;
    COLORREF textColor;

    // 坐标计算结果（供子类 AreaChart 复用）
    struct PlotCoords
    {
        std::vector<int> ptX, ptY;
        int originX, originY;
        int axisTopY, axisRightX;
        int plotWidth, plotHeight;
    };

    double getMaxValue() const;
    void drawTitle();
    void drawAxis();
    void drawGrid();

    // 计算绘图坐标（子类可复用）
    PlotCoords computePlotCoords() const;

public:
    LineChart(const tstring& chartTitle,
              const std::vector<ChartItem>& chartData,
              int x = 20, int y = 20,
              int w = 960, int h = 580);

    void draw() override;
    void applyTheme(const ColorTheme& theme) override;
};
