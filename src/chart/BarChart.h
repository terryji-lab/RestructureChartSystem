#pragma once
#include "Chart.h"
#include <easyx.h>
#include <vector>

//柱状图类
class BarChart : public Chart
{
private:
    //各种颜色信息
    COLORREF barColor;                     
    COLORREF axisColor;                    
    COLORREF gridColor;                    
    COLORREF textColor;                    
    std::vector<COLORREF> barColors;       

private:
    // 获取数据中的最大值（用于计算柱高比例）
    double getMaxValue() const;
    // 绘制标题（居中显示在绘图区顶部）
    void drawTitle();
    // 绘制 X 轴和 Y 轴
    void drawAxis();
    // 绘制 Y 轴刻度线和水平网格线
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
