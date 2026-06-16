#include "PieChart.h"
#include <cmath>
#include <sstream>

const double PI = 3.14159265358979323846;

PieChart::PieChart(const tstring& chartTitle,
                   const std::vector<ChartItem>& chartData,
                   int x, int y, int w, int h, int r)
    : Chart(chartTitle, chartData, x, y, w, h)
    , m_textColor(BLACK)
{
    radius = r;
    centerX = leftX + chartWidth / 2 - 80;
    centerY = topY + chartHeight / 2;

    // 默认调色板
    colors = {
        0xD59B5B, 0x317DED, 0xA5A5A5, 0x00C0FF,
        0x47AD70, 0xC47244, 0x0E489E, 0x636363
    };
}

void PieChart::applyTheme(const ColorTheme& theme)
{
    if (!theme.barPalette.empty())
        colors = theme.barPalette;
    m_textColor = theme.textColor;
}

double PieChart::getTotal() const
{
    double sum = 0;
    for(size_t i = 0;i < data.size();i++)
        sum += data[i].value;
    return sum;
}

void PieChart::draw()
{
    double total = getTotal();

    if(data.empty() || total <= 0)
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    // Title
    settextcolor(m_textColor);
    settextstyle(36,0,_T("Arial"));
    int tw = textwidth(title.c_str());
    outtextxy(leftX + (chartWidth - tw) / 2, topY + 20, title.c_str());

    double startAngle = 0;

    for(size_t i = 0;i < data.size();i++)
    {
        double percent = data[i].value / total;
        double endAngle = startAngle + percent * 2 * PI;

        setfillcolor(colors[i % colors.size()]);
        setlinecolor(WHITE);

        fillpie(centerX - radius, centerY - radius,
                centerX + radius, centerY + radius,
                startAngle, endAngle);

        // Percentage label
        double midAngle = (startAngle + endAngle) / 2;
        int textX = centerX + int(cos(midAngle) * radius * 0.6);
        int textY = centerY - int(sin(midAngle) * radius * 0.6);

        TCHAR pctStr[16];
        _stprintf_s(pctStr, _T("%.1f%%"), percent * 100);
        settextcolor(m_textColor);
        settextstyle(20,0,_T("Arial"));
        outtextxy(textX - 20, textY - 8, pctStr);

        startAngle = endAngle;
    }

    drawLegend();
}

void PieChart::drawLegend()
{
    int legendX = centerX + radius + 70;
    int legendY = centerY - radius;

    settextstyle(22,0,_T("Arial"));
    settextcolor(m_textColor);

    for(size_t i = 0;i < data.size();i++)
    {
        int ly = legendY + int(i) * 42;

        setfillcolor(colors[i % colors.size()]);
        fillrectangle(legendX, ly, legendX + 20, ly + 20);

        setlinecolor(BLACK);
        rectangle(legendX, ly, legendX + 20, ly + 20);

        TCHAR legendStr[128];
        _stprintf_s(legendStr, _T("%s: %.0f"), data[i].name.c_str(), data[i].value);
        outtextxy(legendX + 30, ly, legendStr);
    }
}
