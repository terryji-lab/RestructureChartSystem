#pragma once
#include <easyx.h>
#include "AntiAlias.h"

// 圆角矩形填充工具函数
// 使用 GDI+ 抗锯齿圆角矩形替代原来的矩形+圆形组合方案
// r 超过宽/高一半时自动截断
inline void drawRoundRectFill(int x, int y, int w, int h, int r, COLORREF color)
{
    AA::roundrect(x, y, w, h, r, color);
}
