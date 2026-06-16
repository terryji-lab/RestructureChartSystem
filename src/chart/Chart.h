#pragma once
#include <easyx.h>
#include <string>
#include <vector>

typedef std::basic_string<TCHAR> tstring;

struct ChartItem
{
    tstring name;
    double value;
};

// ==================== 颜色主题 ====================
struct ColorTheme
{
    tstring name;
    std::vector<COLORREF> barPalette;
    COLORREF axisColor;
    COLORREF gridColor;
    COLORREF textColor;
    COLORREF bgColor;
    COLORREF cardColor;
    COLORREF btnNormal;
    COLORREF btnHover;
    COLORREF btnPress;
    COLORREF accentColor;
};

// ==================== 颜色工具函数（内联，所有图表类共享）====================
inline COLORREF darkenColor(COLORREF c, int amount)
{
    int r = GetRValue(c) - amount; if (r < 0) r = 0;
    int g = GetGValue(c) - amount; if (g < 0) g = 0;
    int b = GetBValue(c) - amount; if (b < 0) b = 0;
    return RGB(r, g, b);
}

inline COLORREF lightenColor(COLORREF c, int amount)
{
    int r = GetRValue(c) + amount; if (r > 255) r = 255;
    int g = GetGValue(c) + amount; if (g > 255) g = 255;
    int b = GetBValue(c) + amount; if (b > 255) b = 255;
    return RGB(r, g, b);
}

// ==================== 抽象基类 ====================
class Chart
{
protected:
    tstring title;
    std::vector<ChartItem> data;

    int leftX, topY;
    int chartWidth, chartHeight;

public:
    Chart(const tstring& chartTitle,
         const std::vector<ChartItem>& chartData,
         int x, int y, int w, int h);

    void setData(const std::vector<ChartItem>& chartData);
    void setTitle(const tstring& chartTitle);

    virtual void draw() = 0;
    virtual void applyTheme(const ColorTheme& theme) = 0;

    virtual ~Chart();
};
