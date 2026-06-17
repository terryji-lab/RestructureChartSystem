#pragma once
#include <vector>
#include "Chart.h"

// 折线图/面积图坐标计算结果
struct PlotCoords
{
    std::vector<int> ptX, ptY;
    int originX, originY;
    int axisTopY, axisRightX;
    int plotWidth, plotHeight;
};

// 计算折线图/面积图的数据点屏幕坐标
// 将数据值按比例映射到像素坐标，X 轴等间距分布
PlotCoords computePlotCoords(
    const std::vector<ChartItem>& data,
    int leftX, int topY, int chartWidth, int chartHeight);
