#pragma once
#include "Chart.h"
#include <easyx.h>
#include <vector>

// ==================== 柱状图类 ====================
// 继承 Chart 抽象基类，实现垂直柱状图的绘制。
// 支持多色柱子（通过调色板循环配色）、Y 轴刻度线和虚线网格。
//
// 布局说明（以 960×580 绘图区域为例）：
//   - 标题：顶部居中
//   - Y 轴：左侧 x=80 处
//   - X 轴：底部 y=chartHeight-70 处
//   - 柱子自动等分绘图宽度
class BarChart : public Chart
{
private:
    COLORREF barColor;                     // 默认柱子颜色（调色板为空时使用）
    COLORREF axisColor;                    // 坐标轴颜色
    COLORREF gridColor;                    // 网格线颜色
    COLORREF textColor;                    // 文字颜色
    std::vector<COLORREF> barColors;       // 多色柱子调色板

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
