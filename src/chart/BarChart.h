#pragma once
#include "Chart.h"
#include <easyx.h>
#include <vector>

class BarChart : public Chart
{
private:
    COLORREF barColor;
    COLORREF axisColor;
    COLORREF gridColor;
    COLORREF textColor;
    std::vector<COLORREF> barColors;

private:
    double getMaxValue() const;
    void drawTitle();
    void drawAxis();
    void drawYAxisScale(
        double maxValue,
        int originX,
        int originY,
        int axisRightX,
        int plotHeight
    );

public:
    BarChart(
        const tstring& chartTitle,
        const std::vector<ChartItem>& chartData,
        int x = 20, int y = 20,
        int width = 960, int height = 580
    );

    void draw() override;
    void applyTheme(const ColorTheme& theme) override;
};
