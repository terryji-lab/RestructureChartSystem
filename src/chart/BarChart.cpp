#include "BarChart.h"
#include <sstream>
#include <iomanip>

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

void BarChart::applyTheme(const ColorTheme& theme)
{
    barColor  = theme.barPalette.empty() ? RGB(91, 141, 190) : theme.barPalette[0];
    barColors = theme.barPalette;
    axisColor = theme.axisColor;
    gridColor = theme.gridColor;
    textColor = theme.textColor;
}

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

void BarChart::drawTitle()
{
    settextcolor(textColor);
    settextstyle(28, 0, _T("Arial"));

    int titleWidth = textwidth(title.c_str());
    outtextxy(leftX + chartWidth / 2 - titleWidth / 2, topY + 20, title.c_str());
}

void BarChart::drawAxis()
{
    int originX = leftX + 80;
    int originY = topY + chartHeight - 70;

    int axisTopY = topY + 80;
    int axisRightX = leftX + chartWidth - 50;

    setlinecolor(axisColor);
    setlinestyle(PS_SOLID, 2);

    // y axis
    line(originX, axisTopY, originX, originY);

    // x axis
    line(originX, originY, axisRightX, originY);

    setlinestyle(PS_SOLID, 1);
}

void BarChart::drawYAxisScale(
    double maxValue,
    int originX,
    int originY,
    int axisRightX,
    int plotHeight
)
{
    settextstyle(14, 0, _T("Arial"));

    for (int i = 0; i <= 5; i++)
    {
        int y = originY - plotHeight * i / 5;
        double scaleValue = maxValue * i / 5;

        // grid line
        setlinecolor(gridColor);
        setlinestyle(PS_DOT, 1);
        line(originX, y, axisRightX, y);

        // scale text
        std::basic_stringstream<TCHAR> ss;
        ss << std::fixed << std::setprecision(0) << scaleValue;
        tstring scaleText = ss.str();

        settextcolor(textColor);
        setlinestyle(PS_SOLID, 1);
        outtextxy(originX - 55, y - 8, scaleText.c_str());
    }
}

void BarChart::draw()
{
    if (data.empty())
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    double maxValue = getMaxValue();

    if (maxValue <= 0)
    {
        settextcolor(RED);
        settextstyle(24, 0, _T("Arial"));
        outtextxy(leftX + 50, topY + 50, _T("Invalid data"));
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

    if (barWidth > 60)
    {
        barWidth = 60;
    }

    settextstyle(16, 0, _T("Arial"));

    for (int i = 0; i < n; i++)
    {
        int barX1 = originX + gap + i * 2 * gap;
        int barX2 = barX1 + barWidth;

        int barHeight = int(data[i].value / maxValue * plotHeight);
        int barY1 = originY - barHeight;
        int barY2 = originY;

        // draw bar — use palette color if available, else single barColor
        COLORREF thisBarColor = barColors.empty()
            ? barColor
            : barColors[i % barColors.size()];
        setfillcolor(thisBarColor);
        setlinecolor(thisBarColor);
        solidrectangle(barX1, barY1, barX2, barY2);

        // value text
        std::basic_stringstream<TCHAR> valueStream;
        valueStream << std::fixed << std::setprecision(0) << data[i].value;
        tstring valueText = valueStream.str();

        settextcolor(textColor);

        int valueTextWidth = textwidth(valueText.c_str());
        int valueTextX = (barX1 + barX2) / 2 - valueTextWidth / 2;
        int valueTextY = barY1 - 25;

        outtextxy(valueTextX, valueTextY, valueText.c_str());

        // x-axis label
        int nameTextWidth = textwidth(data[i].name.c_str());
        int nameTextX = (barX1 + barX2) / 2 - nameTextWidth / 2;

        outtextxy(nameTextX, originY + 12, data[i].name.c_str());
    }
}
