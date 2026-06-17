#include "ChartCoordinates.h"

PlotCoords computePlotCoords(
    const std::vector<ChartItem>& data,
    int leftX, int topY, int chartWidth, int chartHeight)
{
    PlotCoords pc;
    pc.originX = leftX + 60;
    pc.originY = topY + chartHeight - 60;
    pc.axisTopY = topY + 70;
    pc.axisRightX = leftX + chartWidth - 40;
    pc.plotWidth = pc.axisRightX - pc.originX;
    pc.plotHeight = pc.originY - pc.axisTopY;

    // 计算数据最大值
    double maxVal = 0;
    if (!data.empty())
    {
        maxVal = data[0].value;
        for (size_t i = 1; i < data.size(); i++)
            if (data[i].value > maxVal) maxVal = data[i].value;
    }

    int n = int(data.size());
    pc.ptX.resize(n);
    pc.ptY.resize(n);

    for (int i = 0; i < n; i++)
    {
        if (n == 1)
            pc.ptX[i] = pc.originX + pc.plotWidth / 2;
        else
            pc.ptX[i] = pc.originX + i * pc.plotWidth / (n - 1);

        pc.ptY[i] = pc.originY - int(data[i].value / maxVal * pc.plotHeight);
    }

    return pc;
}
