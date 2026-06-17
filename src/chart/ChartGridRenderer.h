#pragma once
#include <easyx.h>

// 绘制 Y 轴水平网格线 + 刻度数值（5 条虚线 + 6 个刻度标签）
// textOffsetX: 刻度文字相对 originX 的水平偏移（负数表示左侧）
void drawYAxisGridAndScale(
    int originX, int originY, int axisRightX, int plotHeight,
    double maxValue, COLORREF gridColor, COLORREF textColor,
    int textOffsetX = -50);
