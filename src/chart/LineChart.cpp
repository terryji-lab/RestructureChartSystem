#include "LineChart.h"
#include <sstream>

// ==================== 构造 ====================
LineChart::LineChart(const tstring& chartTitle,
                     const std::vector<ChartItem>& chartData,
                     int x, int y, int w, int h)
    : Chart(chartTitle, chartData, x, y, w, h)
    , lineColor(RGB(0xD5, 0x9B, 0x5B))     // 默认橙棕色折线
    , pointColor(RGB(0x31, 0x7D, 0xED))    // 默认蓝色数据点
    , axisColor(BLACK)
    , gridColor(RGB(0xDC, 0xDC, 0xDC))
    , textColor(BLACK)
{
}

// ==================== 应用主题配色 ====================
void LineChart::applyTheme(const ColorTheme& theme)
{
    lineColor  = theme.barPalette.empty() ? RGB(0xD5, 0x9B, 0x5B) : theme.barPalette[0];
    pointColor = theme.barPalette.size() > 1 ? theme.barPalette[1] : darkenColor(lineColor, 60);
    axisColor  = theme.axisColor;
    gridColor  = theme.gridColor;
    textColor  = theme.textColor;
}

// ==================== 获取数据最大值 ====================
double LineChart::getMaxValue() const
{
    if(data.empty())return 0;
    double maxVal = data[0].value;
    for(size_t i = 1;i < data.size();i++)
    {
        if(data[i].value > maxVal)maxVal = data[i].value;
    }
    return maxVal;
}

// ==================== 绘制标题 ====================
void LineChart::drawTitle()
{
    settextcolor(textColor);
    settextstyle(28,0,_T("Arial"));
    int tw = textwidth(title.c_str());
    outtextxy(leftX + (chartWidth - tw) / 2, topY + 20, title.c_str());
}

// ==================== 绘制坐标轴 ====================
void LineChart::drawAxis()
{
    int originX = leftX + 60;
    int originY = topY + chartHeight - 60;
    int axisTopY = topY + 70;
    int axisRightX = leftX + chartWidth - 40;

    setlinecolor(axisColor);
    setlinestyle(PS_SOLID,2);

    // Y 轴
    line(originX, axisTopY, originX, originY);
    // X 轴
    line(originX, originY, axisRightX, originY);

    setlinestyle(PS_SOLID,1);
}

// ==================== 绘制网格线 ====================
void LineChart::drawGrid()
{
    int originX = leftX + 60;
    int originY = topY + chartHeight - 60;
    int axisTopY = topY + 70;
    int axisRightX = leftX + chartWidth - 40;
    int plotHeight = originY - axisTopY;

    setlinecolor(gridColor);
    setlinestyle(PS_DOT,1);

    // 5 条水平虚线网格
    for(int i = 1;i <= 5;i++)
    {
        int gy = originY - plotHeight * i / 5;
        line(originX, gy, axisRightX, gy);
    }

    setlinestyle(PS_SOLID,1);
}

// ==================== 计算绘图坐标 ====================
// 将数据值映射到屏幕像素坐标，供 draw() 和子类 AreaChart 复用。
// ptX: 数据点在 X 轴上的像素位置（等间距分布）
// ptY: 数据点在 Y 轴上的像素位置（按值和最大值比例映射）
LineChart::PlotCoords LineChart::computePlotCoords() const
{
    PlotCoords pc;
    pc.originX = leftX + 60;
    pc.originY = topY + chartHeight - 60;
    pc.axisTopY = topY + 70;
    pc.axisRightX = leftX + chartWidth - 40;
    pc.plotWidth = pc.axisRightX - pc.originX;
    pc.plotHeight = pc.originY - pc.axisTopY;

    double maxVal = getMaxValue();
    int n = int(data.size());

    pc.ptX.resize(n);
    pc.ptY.resize(n);

    // 等间距分布 X 坐标，按比例计算 Y 坐标
    for(int i = 0;i < n;i++)
    {
        // 只有一个数据点时放在中央
        if(n == 1)pc.ptX[i] = pc.originX + pc.plotWidth / 2;
        else pc.ptX[i] = pc.originX + i * pc.plotWidth / (n - 1);

        // Y 坐标：值越大越靠上（像素坐标越小）
        pc.ptY[i] = pc.originY - int(data[i].value / maxVal * pc.plotHeight);
    }

    return pc;
}

// ==================== 主绘制函数 ====================
void LineChart::draw()
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

    PlotCoords pc = computePlotCoords();
    int n = int(data.size());

    // ── 绘制折线 ──
    setlinecolor(lineColor);
    setlinestyle(PS_SOLID,3);
    for(int i = 0;i < n - 1;i++)
        line(pc.ptX[i], pc.ptY[i], pc.ptX[i + 1], pc.ptY[i + 1]);
    setlinestyle(PS_SOLID,1);

    // ── 自适应标签间距（避免数据量大时标签重叠）──
    int labelStep = 1;       // 标签显示间隔（1=全部显示，2=每隔一个）
    int fontSize = 16;       // 标签字体大小

    // 数据量 > 10 时缩小字体
    if(n > 10){ fontSize = 12; }

    // 数据量 > 25 时，根据可用空间计算标签间隔
    if(n > 25)
    {
        int spacePerLabel = (n == 1) ? pc.plotWidth : pc.plotWidth / (n - 1);
        settextstyle(fontSize, 0, _T("Arial"));
        int avgTextW = textwidth(_T("MMMMM"));  // 估算文本宽度
        labelStep = avgTextW / spacePerLabel + 1;
    }

    // ── 绘制数据点和标签 ──
    settextstyle(fontSize, 0, _T("Arial"));

    for(int i = 0;i < n;i++)
    {
        setfillcolor(pointColor);
        setlinecolor(axisColor);

        // 数据点半径：大数据量时缩小以避免重叠
        int r = (n > 30) ? 2 : 5;
        fillcircle(pc.ptX[i], pc.ptY[i], r);
        circle(pc.ptX[i], pc.ptY[i], r);

        // 数值标签（只在间距足够时显示）
        if(labelStep == 1 || i % labelStep == 0)
        {
            TCHAR numStr[16];
            _stprintf_s(numStr, _T("%.0f"), data[i].value);
            settextcolor(textColor);
            outtextxy(pc.ptX[i] - 10, pc.ptY[i] - 20, numStr);
        }

        // X 轴标签（按步长跳过以避免重叠）
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
