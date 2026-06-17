#include "ChartGridRenderer.h"
#include <tchar.h>
#include <cstdio>

void drawYAxisGridAndScale(
    int originX, int originY, int axisRightX, int plotHeight,
    double maxValue, COLORREF gridColor, COLORREF textColor,
    int textOffsetX)
{
    settextstyle(14, 0, _T("Arial"));

    for (int i = 0; i <= 5; i++)
    {
        int y = originY - plotHeight * i / 5;
        double scaleValue = maxValue * i / 5;

        // 水平虚线网格
        setlinecolor(gridColor);
        setlinestyle(PS_DOT, 1);
        line(originX, y, axisRightX, y);

        // 刻度数值
        TCHAR scaleStr[16];
        _stprintf_s(scaleStr, _T("%.0f"), scaleValue);
        settextcolor(textColor);
        setlinestyle(PS_SOLID, 1);
        outtextxy(originX + textOffsetX, y - 8, scaleStr);
    }
}
