#include "BarChart.h"
#include "ChartGridRenderer.h"
#include "../utils/AntiAlias.h"
#include <sstream>
#include <iomanip>

// 构造函数
BarChart::BarChart(
    const tstring& chartTitle,
    const std::vector<ChartItem>& chartData,
    int x, int y, int width, int height
)
    : Chart(chartTitle, chartData, x, y, width, height)
    , barColor(RGB(91, 141, 190))
    , axisColor(RGB(80, 80, 80))
    , gridColor(RGB(220, 220, 220))
    , textColor(RGB(40, 40, 40))
{
}

// 应用主题配色
void BarChart::applyTheme(const ColorTheme& theme)
{
    barColor  = theme.barPalette.empty() ? RGB(91, 141, 190) : theme.barPalette[0];
    barColors = theme.barPalette;
    axisColor = theme.axisColor;
    gridColor = theme.gridColor;
    textColor = theme.textColor;
}

// 获取数据最大值
double BarChart::getMaxValue() const
{
    if (data.empty()) return 0;
    double maxValue = data[0].value;
    for (size_t i = 1; i < data.size(); i++)
        if (data[i].value > maxValue) maxValue = data[i].value;
    return maxValue;
}

// 绘制标题
void BarChart::drawTitle()
{
    settextcolor(textColor);
    AA::setTextStyleAA(28, 0, _T("Arial"));
    int titleWidth = textwidth(title.c_str());
    outtextxy(leftX + chartWidth / 2 - titleWidth / 2, topY + 20, title.c_str());
}

// 绘制坐标轴
void BarChart::drawAxis()
{
    int originX = leftX + 80;
    int originY = topY + chartHeight - 70;
    int axisTopY = topY + 80;
    int axisRightX = leftX + chartWidth - 50;

    AA::line(originX, axisTopY, originX, originY, axisColor, 2);
    AA::line(originX, originY, axisRightX, originY, axisColor, 2);
}

// 绘制 Y 轴网格线和刻度（委托给 ChartGridRenderer 工具函数）
void BarChart::drawYAxisScale(
    double maxValue,
    int originX,
    int originY,
    int axisRightX,
    int plotHeight
)
{
    drawYAxisGridAndScale(originX, originY, axisRightX, plotHeight,
                          maxValue, gridColor, textColor, -55);
}

// 主绘制函数
void BarChart::draw()
{
    if (data.empty())
    {
        settextcolor(RED);
        AA::setTextStyleAA(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    double maxValue = getMaxValue();
    if (maxValue <= 0)
    {
        settextcolor(RED);
        AA::setTextStyleAA(40, 0, _T("Arial"));
        int tw = textwidth(_T("Invalid Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("Invalid Data"));
        return;
    }

    drawTitle();
    drawAxis();

    int originX = leftX + 80;
    int originY = topY + chartHeight - 70;
    int axisTopY = topY + 80;
    int axisRightX = leftX + chartWidth - 50;
    int plotWidth = axisRightX - originX;
    int plotHeight = originY - axisTopY;

    drawYAxisScale(maxValue, originX, originY, axisRightX, plotHeight);

    int n = int(data.size());
    int gap = plotWidth / (n * 2 + 1);
    int barWidth = gap;
    if (barWidth > 60) barWidth = 60;

    AA::setTextStyleAA(16, 0, _T("Arial"));

    for (int i = 0; i < n; i++)
    {
        int barX1 = originX + gap + i * 2 * gap;
        int barX2 = barX1 + barWidth;

        int barHeight = int(data[i].value / maxValue * plotHeight);
        int barY1 = originY - barHeight;
        int barY2 = originY;

        COLORREF thisBarColor = barColors.empty()
            ? barColor
            : barColors[i % barColors.size()];
        setfillcolor(thisBarColor);
        setlinecolor(thisBarColor);
        solidrectangle(barX1, barY1, barX2, barY2);

        // 数值标签
        tstringstream valueStream;
        valueStream << std::fixed << std::setprecision(0) << data[i].value;
        tstring valueText = valueStream.str();

        settextcolor(textColor);
        int valueTextWidth = textwidth(valueText.c_str());
        int valueTextX = (barX1 + barX2) / 2 - valueTextWidth / 2;
        outtextxy(valueTextX, barY1 - 25, valueText.c_str());

        // X 轴名称标签
        int nameTextWidth = textwidth(data[i].name.c_str());
        int nameTextX = (barX1 + barX2) / 2 - nameTextWidth / 2;
        outtextxy(nameTextX, originY + 12, data[i].name.c_str());
    }
}
