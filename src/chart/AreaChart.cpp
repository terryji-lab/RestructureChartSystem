#include "AreaChart.h"
#include "ChartCoordinates.h"
#include "../utils/AntiAlias.h"
#include <sstream>

// 构造函数
AreaChart::AreaChart(const tstring& chartTitle,
                     const std::vector<ChartItem>& chartData,
                     int x, int y, int w, int h)
    : LineChart(chartTitle, chartData, x, y, w, h)
    , fillColor(RGB(0xBB, 0xDD, 0xFF))
{
}

// 应用主题配色
void AreaChart::applyTheme(const ColorTheme& theme)
{
    LineChart::applyTheme(theme);
    fillColor = theme.barPalette.size() > 2
        ? theme.barPalette[2]
        : lightenColor(lineColor, 130);
}

// 主绘制函数：填充区域 + 上方折线 + 数据点
void AreaChart::draw()
{
    if (data.empty())
    {
        settextcolor(RED);
        AA::setTextStyleAA(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    double maxVal = getMaxValue();
    if (maxVal <= 0)
    {
        settextcolor(RED);
        AA::setTextStyleAA(40, 0, _T("Arial"));
        int tw = textwidth(_T("Invalid Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("Invalid Data"));
        return;
    }

    drawTitle();
    drawGrid();
    drawAxis();

    PlotCoords pc = computePlotCoords(data, leftX, topY, chartWidth, chartHeight);
    int n = int(data.size());

    // 填充面积多边形（抗锯齿）
    POINT* pts = new POINT[n + 2];
    pts[0].x = pc.originX;     pts[0].y = pc.originY;
    for (int i = 0; i < n; i++)
    {
        pts[i + 1].x = pc.ptX[i];
        pts[i + 1].y = pc.ptY[i];
    }
    pts[n + 1].x = pc.ptX[n - 1]; pts[n + 1].y = pc.originY;

    AA::fillpolygon(pts, n + 2, fillColor);
    delete[] pts;

    // 画折线（抗锯齿）
    for (int i = 0; i < n - 1; i++)
        AA::line(pc.ptX[i], pc.ptY[i], pc.ptX[i + 1], pc.ptY[i + 1], lineColor, 3);

    // 自适应标签
    int fontSize = 14;
    if (n > 10) { fontSize = 11; }
    int labelStep = 1;
    if (n > 25)
    {
        int spacePerLabel = (n == 1) ? pc.plotWidth : pc.plotWidth / (n - 1);
        AA::setTextStyleAA(fontSize, 0, _T("Arial"));
        int avgTextW = textwidth(_T("MMMMM"));
        labelStep = avgTextW / spacePerLabel + 1;
    }

    // 绘制数据点
    AA::setTextStyleAA(fontSize, 0, _T("Arial"));

    for (int i = 0; i < n; i++)
    {
        int r = (n > 30) ? 2 : 4;
        AA::fillcircle(pc.ptX[i], pc.ptY[i], r, pointColor, axisColor);

        if (labelStep == 1 || i % labelStep == 0)
        {
            TCHAR numStr[16];
            _stprintf_s(numStr, _T("%.0f"), data[i].value);
            settextcolor(textColor);
            outtextxy(pc.ptX[i] - 10, pc.ptY[i] - 20, numStr);
        }

        if (i % labelStep == 0)
        {
            int tw = textwidth(data[i].name.c_str());
            outtextxy(pc.ptX[i] - tw / 2, pc.originY + 8, data[i].name.c_str());
        }
    }

    // Y 轴刻度
    AA::setTextStyleAA(14, 0, _T("Arial"));
    for (int i = 0; i <= 5; i++)
    {
        int sy = pc.originY - pc.plotHeight * i / 5;
        double scaleVal = maxVal * i / 5;
        TCHAR scaleStr[16];
        _stprintf_s(scaleStr, _T("%.0f"), scaleVal);
        outtextxy(pc.originX - 45, sy - 8, scaleStr);
    }
}
