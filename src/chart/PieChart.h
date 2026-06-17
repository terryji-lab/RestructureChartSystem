#pragma once
#include "Chart.h"
#include <vector>

// 饼图类
class PieChart : public Chart
{
private:
    int centerX, centerY, radius;
    std::vector<COLORREF> colors;
    COLORREF m_textColor;

    double getTotal() const;
    void drawLegend();

public:
    PieChart(const tstring& chartTitle,
             const std::vector<ChartItem>& chartData,
             int x = 20, int y = 20,
             int w = 960, int h = 580,
             int r = 280);

    void draw() override;
    void applyTheme(const ColorTheme& theme) override;
};
