#pragma once
#include <easyx.h>

// ==================== 圆角矩形填充工具函数 ====================
// 由于 EasyX 没有原生的圆角矩形绘制函数，这里通过组合
// solidrectangle（矩形）和 solidcircle（圆形）来模拟圆角效果。
// 绘制顺序：先画中间矩形主体，再画四条边，最后画四个角上的圆。
//
// 参数：
//   x, y  - 左上角坐标
//   w, h  - 矩形宽高
//   r     - 圆角半径（超过宽/高一半时自动截断）
//   color - 填充颜色
inline void drawRoundRectFill(int x, int y, int w, int h, int r, COLORREF color)
{
    if (r <= 0) { setfillcolor(color); solidrectangle(x, y, x + w, y + h); return; }
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    setfillcolor(color);
    // 中间矩形主体
    solidrectangle(x + r, y + r, x + w - r, y + h - r);
    // 上边条
    solidrectangle(x + r, y, x + w - r, y + r);
    // 下边条
    solidrectangle(x + r, y + h - r, x + w - r, y + h);
    // 左边条
    solidrectangle(x, y + r, x + r, y + h - r);
    // 右边条
    solidrectangle(x + w - r, y + r, x + w, y + h - r);
    // 四个角的圆形填充
    solidcircle(x + r,     y + r,     r);
    solidcircle(x + w - r, y + r,     r);
    solidcircle(x + r,     y + h - r, r);
    solidcircle(x + w - r, y + h - r, r);
}
