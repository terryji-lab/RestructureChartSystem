#include "PieChart.h"
#include <cmath>
#include <sstream>

const double PI = 3.14159265358979323846;

// ==================== 构造 ====================
PieChart::PieChart(const tstring& chartTitle,
                   const std::vector<ChartItem>& chartData,
                   int x, int y, int w, int h, int r)
    : Chart(chartTitle, chartData, x, y, w, h)
    , m_textColor(BLACK)
{
    radius = r;
    // 圆心位于绘图区中央偏左（右侧留给图例）
    centerX = leftX + chartWidth / 2 - 80;
    centerY = topY + chartHeight / 2;

    // 默认调色板（8 种颜色）
    colors = {
        0xD59B5B, 0x317DED, 0xA5A5A5, 0x00C0FF,
        0x47AD70, 0xC47244, 0x0E489E, 0x636363
    };
}

// ==================== 应用主题配色 ====================
void PieChart::applyTheme(const ColorTheme& theme)
{
    if (!theme.barPalette.empty())
        colors = theme.barPalette;
    m_textColor = theme.textColor;
}

// ==================== 计算数据总和 ====================
double PieChart::getTotal() const
{
    double sum = 0;
    for(size_t i = 0;i < data.size();i++)
        sum += data[i].value;
    return sum;
}

// ==================== 主绘制函数 ====================
void PieChart::draw()
{
    double total = getTotal();

    // 数据为空或总和为零时显示提示
    if(data.empty() || total <= 0)
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        int tw = textwidth(_T("No Data"));
        outtextxy(leftX + (chartWidth - tw) / 2, topY + chartHeight / 2 - 20, _T("No Data"));
        return;
    }

    // 绘制标题（顶部居中）
    settextcolor(m_textColor);
    settextstyle(36,0,_T("Arial"));
    int tw = textwidth(title.c_str());
    outtextxy(leftX + (chartWidth - tw) / 2, topY + 20, title.c_str());

    double startAngle = 0;  // 起始角度（弧度制，从右侧水平方向顺时针）

    // 逐个绘制扇形
    for(size_t i = 0;i < data.size();i++)
    {
        // 计算每个扇区占的比例和结束角度
        double percent = data[i].value / total;
        double endAngle = startAngle + percent * 2 * PI;

        setfillcolor(colors[i % colors.size()]);
        setlinecolor(WHITE);

        // EasyX 的 fillpie：绘制填充椭圆扇形
        fillpie(centerX - radius, centerY - radius,
                centerX + radius, centerY + radius,
                startAngle, endAngle);

        // 在扇形中间位置绘制百分比标签
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

    // 绘制右侧图例
    drawLegend();
}

// ==================== 绘制图例 ====================
// 在饼图右侧绘制颜色块 + 名称 + 数值的图例列表
void PieChart::drawLegend()
{
    int legendX = centerX + radius + 70;   // 图例起始 X（饼图右侧 70px）
    int legendY = centerY - radius;         // 图例起始 Y（与饼图顶部对齐）

    settextstyle(22,0,_T("Arial"));
    settextcolor(m_textColor);

    for(size_t i = 0;i < data.size();i++)
    {
        int ly = legendY + int(i) * 42;    // 每行图例间距 42px

        // 颜色块
        setfillcolor(colors[i % colors.size()]);
        fillrectangle(legendX, ly, legendX + 20, ly + 20);

        // 颜色块边框
        setlinecolor(BLACK);
        rectangle(legendX, ly, legendX + 20, ly + 20);

        // 名称和数值
        TCHAR legendStr[128];
        _stprintf_s(legendStr, _T("%s: %.0f"), data[i].name.c_str(), data[i].value);
        outtextxy(legendX + 30, ly, legendStr);
    }
}
