#pragma once
#include <easyx.h>

// 圆角矩形填充（用 solidrectangle + solidcircle 组合实现）
inline void drawRoundRectFill(int x, int y, int w, int h, int r, COLORREF color)
{
    if (r <= 0) { setfillcolor(color); solidrectangle(x, y, x + w, y + h); return; }
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    setfillcolor(color);
    solidrectangle(x + r, y + r, x + w - r, y + h - r);
    solidrectangle(x + r, y, x + w - r, y + r);
    solidrectangle(x + r, y + h - r, x + w - r, y + h);
    solidrectangle(x, y + r, x + r, y + h - r);
    solidrectangle(x + w - r, y + r, x + w, y + h - r);
    solidcircle(x + r,     y + r,     r);
    solidcircle(x + w - r, y + r,     r);
    solidcircle(x + r,     y + h - r, r);
    solidcircle(x + w - r, y + h - r, r);
}
