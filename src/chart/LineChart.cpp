#include "LineChart.h"
#include "ChartCoordinates.h"
#include <sstream>

// 构造函数
LineChart::LineChart(const tstring& chartTitle,
                     const std::vector<ChartItem>& chartData,
                     int x, int y, int w, int h)
    : Chart(chartTitle, chartData, x, y, w, h)
    , lineColor(RGB(0xD5, 0x9B, 0x5B))
    , pointColor(RGB(0x31, 0x7D, 0xED))
    , axisColor(BLACK)
    , gridColor(RGB(0xDC, 0xDC, 0xDC))
    , textColor(BLACK)
{
}

// 应用主题配色
void LineChart::applyTheme(const ColorTheme& theme)
{
    lineColor  = theme.barPalette.empty() ? RGB(0xD5, 0x9B, 0x5B) : theme.barPalette[0];
    pointColor = theme.barPalette.size() > 1 ? theme.barPalette[1] : darkenColor(lineColor, 60);
    axisColor  = theme.axisColor;
    gridColor  = theme.gridColor;
    textColor  = theme.textColor;
}

// 获取数据最大值
double LineChart::getMaxValue() const
{
    if (data.empty()) return 0;
    double maxVal = data[0].value;
    for (size_t i = 1; i < data.size(); i++)
        if (data[i].value > maxVal) maxVal = data[i].value;
    return maxVal;
}

// 绘制标题
void LineChart::drawTitle()
{
    settextcolor(textColor);
    settextstyle(28, 0, _T("Arial"));
    int tw = textwidth(title.c_str());
    outtextxy(leftX + (chartWidth - tw) / 2, topY + 20, title.c_str());
}

// 绘制坐标轴
void LineChart::drawAxis()
{
    int originX = leftX + 60;
    int originY = topY + chartHeight - 60;
    int axisTopY = topY + 70;
    int axisRightX = leftX + chartWidth - 40;

    setlinecolor(axisColor);
    setlinestyle(PS_SOLID, 2);

    line(originX, axisTopY, originX, originY);
    line(originX, originY, axisRightX, originY);

    setlinestyle(PS_SOLID, 1);
}

// 绘制网格线
void LineChart::drawGrid()
{
    int originX = leftX + 60;
    int originY = topY + chartHeight - 60;
    int axisTopY = topY + 70;
    int axisRightX = leftX + chartWidth - 40;
    int plotHeight = originY - axisTopY;

    setlinecolor(gridColor);
    setlinestyle(PS_DOT, 1);

    for (int i = 1; i <= 5; i++)
    {
        int gy = originY - plotHeight * i / 5;
        line(originX, gy, axisRightX, gy);
    }

    setlinestyle(PS_SOLID, 1);
}

// 主绘制函数
void LineChart::draw()
{
    if (data.empty())
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    double maxVal = getMaxValue();
    if (maxVal <= 0)
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("Invalid Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("Invalid Data"));
        return;
    }

    drawTitle();
    drawGrid();
    drawAxis();

    PlotCoords pc = computePlotCoords(data, leftX, topY, chartWidth, chartHeight);
    int n = int(data.size());

    // 绘制折线
    setlinecolor(lineColor);
    setlinestyle(PS_SOLID, 3);
    for (int i = 0; i < n - 1; i++)
        line(pc.ptX[i], pc.ptY[i], pc.ptX[i + 1], pc.ptY[i + 1]);
    setlinestyle(PS_SOLID, 1);

    // 自适应标签间距
    int labelStep = 1;
    int fontSize = 16;

    if (n > 10) { fontSize = 12; }

    if (n > 25)
    {
        int spacePerLabel = (n == 1) ? pc.plotWidth : pc.plotWidth / (n - 1);
        settextstyle(fontSize, 0, _T("Arial"));
        int avgTextW = textwidth(_T("MMMMM"));
        labelStep = avgTextW / spacePerLabel + 1;
    }

    // 绘制数据点和标签
    settextstyle(fontSize, 0, _T("Arial"));

    for (int i = 0; i < n; i++)
    {
        setfillcolor(pointColor);
        setlinecolor(axisColor);

        int r = (n > 30) ? 2 : 5;
        fillcircle(pc.ptX[i], pc.ptY[i], r);
        circle(pc.ptX[i], pc.ptY[i], r);

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
    settextstyle(14, 0, _T("Arial"));
    for (int i = 0; i <= 5; i++)
    {
        int sy = pc.originY - pc.plotHeight * i / 5;
        double scaleVal = maxVal * i / 5;
        TCHAR scaleStr[16];
        _stprintf_s(scaleStr, _T("%.0f"), scaleVal);
        outtextxy(pc.originX - 45, sy - 8, scaleStr);
    }
}
