#include "BarChart.h"
#include <sstream>
#include <iomanip>

// ==================== 构造 ====================
BarChart::BarChart(
    const tstring& chartTitle,
    const std::vector<ChartItem>& chartData,
    int x, int y, int width, int height
)
    : Chart(chartTitle, chartData, x, y, width, height)
    , barColor(RGB(91, 141, 190))          // 默认蓝色
    , axisColor(RGB(80, 80, 80))           // 深灰坐标轴
    , gridColor(RGB(220, 220, 220))        // 浅灰网格
    , textColor(RGB(40, 40, 40))           // 深色文字
{
}

// ==================== 应用主题配色 ====================
void BarChart::applyTheme(const ColorTheme& theme)
{
    barColor  = theme.barPalette.empty() ? RGB(91, 141, 190) : theme.barPalette[0];
    barColors = theme.barPalette;
    axisColor = theme.axisColor;
    gridColor = theme.gridColor;
    textColor = theme.textColor;
}

// ==================== 获取数据最大值 ====================
double BarChart::getMaxValue() const
{
    if (data.empty())
    {
        return 0;
    }

    double maxValue = data[0].value;

    for (size_t i = 1; i < data.size(); i++)
    {
        if (data[i].value > maxValue)
        {
            maxValue = data[i].value;
        }
    }

    return maxValue;
}

// ==================== 绘制标题 ====================
void BarChart::drawTitle()
{
    settextcolor(textColor);
    settextstyle(28, 0, _T("Arial"));

    int titleWidth = textwidth(title.c_str());
    // 水平居中
    outtextxy(leftX + chartWidth / 2 - titleWidth / 2, topY + 20, title.c_str());
}

// ==================== 绘制坐标轴 ====================
void BarChart::drawAxis()
{
    // 原点位置：X 轴起点在左侧 80px 处，Y 轴起点在底部 70px 上方
    int originX = leftX + 80;
    int originY = topY + chartHeight - 70;

    int axisTopY = topY + 80;
    int axisRightX = leftX + chartWidth - 50;

    setlinecolor(axisColor);
    setlinestyle(PS_SOLID, 2);

    // Y 轴（竖线）
    line(originX, axisTopY, originX, originY);

    // X 轴（横线）
    line(originX, originY, axisRightX, originY);

    setlinestyle(PS_SOLID, 1);
}

// ==================== 绘制 Y 轴刻度与网格线 ====================
void BarChart::drawYAxisScale(
    double maxValue,
    int originX,
    int originY,
    int axisRightX,
    int plotHeight
)
{
    settextstyle(14, 0, _T("Arial"));

    // 绘制 5 条水平网格线（0%, 20%, 40%, 60%, 80%, 100%）
    for (int i = 0; i <= 5; i++)
    {
        int y = originY - plotHeight * i / 5;
        double scaleValue = maxValue * i / 5;

        // 虚线网格线
        setlinecolor(gridColor);
        setlinestyle(PS_DOT, 1);
        line(originX, y, axisRightX, y);

        // 刻度数值
        std::basic_stringstream<TCHAR> ss;
        ss << std::fixed << std::setprecision(0) << scaleValue;
        tstring scaleText = ss.str();

        settextcolor(textColor);
        setlinestyle(PS_SOLID, 1);
        outtextxy(originX - 55, y - 8, scaleText.c_str());
    }
}

// ==================== 主绘制函数 ====================
void BarChart::draw()
{
    // 数据为空时显示提示
    if (data.empty())
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    double maxValue = getMaxValue();

    // 最大值无效时显示提示
    if (maxValue <= 0)
    {
        settextcolor(RED);
        settextstyle(24, 0, _T("Arial"));
        outtextxy(leftX + 50, topY + 50, _T("Invalid data"));
        return;
    }

    drawTitle();
    drawAxis();

    // 计算绘图区域的坐标参数
    int originX = leftX + 80;
    int originY = topY + chartHeight - 70;

    int axisTopY = topY + 80;
    int axisRightX = leftX + chartWidth - 50;

    int plotWidth = axisRightX - originX;
    int plotHeight = originY - axisTopY;

    drawYAxisScale(maxValue, originX, originY, axisRightX, plotHeight);

    int n = int(data.size());

    // 计算柱子间距和宽度
    // 布局方式：| 空隙 | 柱子 | 空隙 | 柱子 | ... |
    // 每个柱位占 2*gap，其中柱子宽 gap，两侧各 gap/2
    int gap = plotWidth / (n * 2 + 1);
    int barWidth = gap;

    // 柱子宽度上限（避免数据量少时柱子过宽）
    if (barWidth > 60)
    {
        barWidth = 60;
    }

    settextstyle(16, 0, _T("Arial"));

    // 逐个绘制柱子
    for (int i = 0; i < n; i++)
    {
        // 柱子水平位置
        int barX1 = originX + gap + i * 2 * gap;
        int barX2 = barX1 + barWidth;

        // 柱子高度（按比例缩放）
        int barHeight = int(data[i].value / maxValue * plotHeight);
        int barY1 = originY - barHeight;
        int barY2 = originY;

        // 选择柱子颜色：调色板不为空则循环使用，否则使用默认单色
        COLORREF thisBarColor = barColors.empty()
            ? barColor
            : barColors[i % barColors.size()];
        setfillcolor(thisBarColor);
        setlinecolor(thisBarColor);
        solidrectangle(barX1, barY1, barX2, barY2);

        // 柱子上方的数值标签
        std::basic_stringstream<TCHAR> valueStream;
        valueStream << std::fixed << std::setprecision(0) << data[i].value;
        tstring valueText = valueStream.str();

        settextcolor(textColor);

        int valueTextWidth = textwidth(valueText.c_str());
        int valueTextX = (barX1 + barX2) / 2 - valueTextWidth / 2;
        int valueTextY = barY1 - 25;

        outtextxy(valueTextX, valueTextY, valueText.c_str());

        // X 轴下方的名称标签
        int nameTextWidth = textwidth(data[i].name.c_str());
        int nameTextX = (barX1 + barX2) / 2 - nameTextWidth / 2;

        outtextxy(nameTextX, originY + 12, data[i].name.c_str());
    }
}
