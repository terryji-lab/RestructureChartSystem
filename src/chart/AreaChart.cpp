#include "AreaChart.h"
#include <sstream>

// ==================== 构造 ====================
AreaChart::AreaChart(const tstring& chartTitle,
                     const std::vector<ChartItem>& chartData,
                     int x, int y, int w, int h)
    : LineChart(chartTitle, chartData, x, y, w, h)
    , fillColor(RGB(0xBB, 0xDD, 0xFF))     // 默认浅蓝色填充
{
}

// ==================== 应用主题配色 ====================
void AreaChart::applyTheme(const ColorTheme& theme)
{
    LineChart::applyTheme(theme);  // 复用父类 LineChart 的配色逻辑
    fillColor = theme.barPalette.size() > 2
        ? theme.barPalette[2]                         // 使用调色板第 3 个颜色
        : lightenColor(lineColor, 130);               // 回退：折线颜色变浅
}

// ==================== 主绘制函数 ====================
// 面积图 = 填充区域 + 上方折线 + 数据点
// 先绘制填充多边形，再在其上叠加折线和数据点。
void AreaChart::draw()
{
    // 数据为空时显示提示
    if(data.empty())
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    double maxVal = getMaxValue();
    if(maxVal <= 0)
    {
        settextcolor(RED);
        settextstyle(24,0,_T("Arial"));
        outtextxy(leftX + 50, topY + 50, _T("Invalid Data"));
        return;
    }

    drawTitle();
    drawGrid();
    drawAxis();

    // 复用基类 LineChart 的坐标计算
    PlotCoords pc = computePlotCoords();
    int n = int(data.size());

    // ── 填充面积（在画折线之前绘制，避免覆盖）──
    // 构建多边形：原点 → 数据点序列 → 右下角 → 闭合
    POINT* pts = new POINT[n + 2];
    pts[0].x = pc.originX;     pts[0].y = pc.originY;      // 左下角原点
    for(int i = 0;i < n;i++)
    {
        pts[i + 1].x = pc.ptX[i];
        pts[i + 1].y = pc.ptY[i];
    }
    pts[n + 1].x = pc.ptX[n - 1]; pts[n + 1].y = pc.originY; // 右下角

    setfillcolor(fillColor);     // 使用主题填充色
    setlinecolor(fillColor);     // 边线与填充同色（消除接缝）
    fillpolygon(pts, n + 2);
    delete[] pts;

    // ── 画折线（在填充之上，使其可见）──
    setlinecolor(lineColor);
    setlinestyle(PS_SOLID,3);
    for(int i = 0;i < n - 1;i++)
        line(pc.ptX[i], pc.ptY[i], pc.ptX[i + 1], pc.ptY[i + 1]);
    setlinestyle(PS_SOLID,1);

    // ── 自适应标签 ──
    int fontSize = 14;
    if(n > 10){ fontSize = 11; }
    int labelStep = 1;
    if(n > 25)
    {
        int spacePerLabel = (n == 1) ? pc.plotWidth : pc.plotWidth / (n - 1);
        settextstyle(fontSize, 0, _T("Arial"));
        int avgTextW = textwidth(_T("MMMMM"));
        labelStep = avgTextW / spacePerLabel + 1;
    }

    // ── 绘制数据点 ──
    settextstyle(fontSize, 0, _T("Arial"));

    for(int i = 0;i < n;i++)
    {
        setfillcolor(pointColor);
        setlinecolor(axisColor);

        int r = (n > 30) ? 2 : 4;
        fillcircle(pc.ptX[i], pc.ptY[i], r);
        circle(pc.ptX[i], pc.ptY[i], r);

        // 数值标签
        if(labelStep == 1 || i % labelStep == 0)
        {
            TCHAR numStr[16];
            _stprintf_s(numStr, _T("%.0f"), data[i].value);
            settextcolor(textColor);
            outtextxy(pc.ptX[i] - 10, pc.ptY[i] - 20, numStr);
        }

        // X 轴标签
        if(i % labelStep == 0)
        {
            int tw = textwidth(data[i].name.c_str());
            outtextxy(pc.ptX[i] - tw / 2, pc.originY + 8, data[i].name.c_str());
        }
    }

    // ── Y 轴刻度 ──
    settextstyle(14,0,_T("Arial"));
    for(int i = 0;i <= 5;i++)
    {
        int sy = pc.originY - pc.plotHeight * i / 5;
        double scaleVal = maxVal * i / 5;

        TCHAR scaleStr[16];
        _stprintf_s(scaleStr, _T("%.0f"), scaleVal);
        outtextxy(pc.originX - 45, sy - 8, scaleStr);
    }
}
