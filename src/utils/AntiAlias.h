#pragma once
// =========================================================================
//  AntiAlias.h  —  GDI+ 抗锯齿绘图工具（头文件唯一实现）
//
//  使用方式：
//    1. 在 #include 之前定义 GDIPVER  (见下方)
//    2. 调用 AA::Init() 初始化 GDI+（一次即可）
//    3. 用 AA::line / AA::circle / ... 替换 EasyX 同名函数
//    4. 程序退出前调用 AA::Shutdown()
//
//  原理：
//    通过获取 EasyX 当前工作 IMAGE 的 HDC，创建 GDI+ Graphics 对象，
//    启用 SmoothingModeAntiAlias 进行矢量绘图。
//    文字依旧走 EasyX 的 outtextxy，但字体 QUALITY 改为 CLEARTYPE。
// =========================================================================

#ifndef ANTIALIAS_H_INCLUDED
#define ANTIALIAS_H_INCLUDED

// GDI+ 需要在 <windows.h> 之后、自身头文件之前设置最低版本
#ifndef GDIPVER
#define GDIPVER 0x0110
#endif

#include <easyx.h>
#include <windows.h>

// 避免 GDI+ 的 min/max 宏冲突
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>   // std::min / std::max (GDI+ headers may need)

#pragma warning(push)
#pragma warning(disable: 4458)  // GDI+ 头内部 shadow 警告
#include <gdiplus.h>
#pragma warning(pop)

#pragma comment(lib, "gdiplus.lib")

#include <cmath>
#include <vector>

// =========================================================================
namespace AA {
// =========================================================================

// ---------- GDI+ 生命周期管理 ----------

inline ULONG_PTR& _Token() { static ULONG_PTR t = 0; return t; }

inline void Init()
{
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&_Token(), &input, nullptr);
}

inline void Shutdown()
{
    if (_Token()) { Gdiplus::GdiplusShutdown(_Token()); _Token() = 0; }
}

// ---------- 内部：获取当前 HDC ----------

inline HDC _GetDC()
{
    // GetImageHDC(NULL) 返回当前工作图像的 HDC（屏幕或离屏 IMAGE）
    return GetImageHDC(NULL);
}

// ---------- COLORREF → GDI+ Color ----------

inline Gdiplus::Color _ToColor(COLORREF c, BYTE alpha = 255)
{
    return Gdiplus::Color(alpha, GetRValue(c), GetGValue(c), GetBValue(c));
}

// =========================================================================
//  抗锯齿绘图 API
// =========================================================================

// ---- 直线 ----
inline void line(int x1, int y1, int x2, int y2,
                 COLORREF color, int width = 1)
{
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen pen(_ToColor(color), (Gdiplus::REAL)width);
    g.DrawLine(&pen, x1, y1, x2, y2);
}

// ---- 圆（描边）----
inline void circle(int cx, int cy, int r, COLORREF color, int width = 1)
{
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen pen(_ToColor(color), (Gdiplus::REAL)width);
    g.DrawEllipse(&pen, cx - r, cy - r, r * 2, r * 2);
}

// ---- 填充圆 ----
inline void fillcircle(int cx, int cy, int r,
                       COLORREF fillCol, COLORREF borderCol, int borderW = 1)
{
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(_ToColor(fillCol));
    g.FillEllipse(&brush, cx - r, cy - r, r * 2, r * 2);
    if (borderW > 0)
    {
        Gdiplus::Pen pen(_ToColor(borderCol), (Gdiplus::REAL)borderW);
        g.DrawEllipse(&pen, cx - r, cy - r, r * 2, r * 2);
    }
}

// ---- 仅填充圆（无边框）----
inline void solidcircle(int cx, int cy, int r, COLORREF color)
{
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(_ToColor(color));
    g.FillEllipse(&brush, cx - r, cy - r, r * 2, r * 2);
}

// ---- 圆弧 ----
// startAngle / sweepAngle 单位为弧度（与 EasyX arc 相同约定）
inline void arc(int left, int top, int right, int bottom,
                double startRad, double endRad,
                COLORREF color, int width = 1)
{
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen pen(_ToColor(color), (Gdiplus::REAL)width);

    // EasyX arc: startAngle / endAngle 均为弧度
    // GDI+ DrawArc: startAngle / sweepAngle 均为角度（度）
    double startDeg = -startRad * 180.0 / 3.14159265358979323846;
    double endDeg   = -endRad   * 180.0 / 3.14159265358979323846;
    double sweepDeg = endDeg - startDeg;

    g.DrawArc(&pen, left, top, right - left, bottom - top,
              (Gdiplus::REAL)startDeg, (Gdiplus::REAL)sweepDeg);
}

// ---- 填充饼图扇形 ----
// startAngle / endAngle 为弧度（与 EasyX fillpie 相同约定）
inline void fillpie(int left, int top, int right, int bottom,
                    double startRad, double endRad,
                    COLORREF fillCol, COLORREF borderCol = 0xFFFFFFFF)
{
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    double startDeg = -startRad * 180.0 / 3.14159265358979323846;
    double endDeg   = -endRad   * 180.0 / 3.14159265358979323846;
    double sweepDeg = endDeg - startDeg;

    Gdiplus::SolidBrush brush(_ToColor(fillCol));
    g.FillPie(&brush, left, top, right - left, bottom - top,
              (Gdiplus::REAL)startDeg, (Gdiplus::REAL)sweepDeg);

    if (borderCol != 0xFFFFFFFF)
    {
        Gdiplus::Pen pen(_ToColor(borderCol), 1.0f);
        g.DrawPie(&pen, left, top, right - left, bottom - top,
                  (Gdiplus::REAL)startDeg, (Gdiplus::REAL)sweepDeg);
    }
}

// ---- 填充多边形 ----
inline void fillpolygon(const POINT* pts, int count,
                        COLORREF fillCol, COLORREF borderCol = 0xFFFFFFFF)
{
    if (count < 2) return;
    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    std::vector<Gdiplus::Point> gpts(count);
    for (int i = 0; i < count; ++i)
    {
        gpts[i].X = pts[i].x;
        gpts[i].Y = pts[i].y;
    }

    Gdiplus::SolidBrush brush(_ToColor(fillCol));
    g.FillPolygon(&brush, gpts.data(), count);

    if (borderCol != 0xFFFFFFFF)
    {
        Gdiplus::Pen pen(_ToColor(borderCol), 1.0f);
        g.DrawPolygon(&pen, gpts.data(), count);
    }
}

// ---- 圆角矩形 (填充 + 可选描边) ----
inline void roundrect(int x, int y, int w, int h, int r,
                      COLORREF fillCol,
                      COLORREF borderCol = 0xFFFFFFFF, int borderW = 1)
{
    if (r <= 0) { setfillcolor(fillCol); solidrectangle(x, y, x + w, y + h); return; }
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    HDC hdc = _GetDC();
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // 构建圆角矩形 GraphicsPath
    Gdiplus::GraphicsPath path;
    int d = r * 2;
    path.AddArc(x,         y,         d, d, 180, 90);   // 左上
    path.AddArc(x + w - d, y,         d, d, 270, 90);   // 右上
    path.AddArc(x + w - d, y + h - d, d, d, 0,   90);   // 右下
    path.AddArc(x,         y + h - d, d, d, 90,  90);   // 左下
    path.CloseFigure();

    Gdiplus::SolidBrush brush(_ToColor(fillCol));
    g.FillPath(&brush, &path);

    if (borderCol != 0xFFFFFFFF && borderW > 0)
    {
        Gdiplus::Pen pen(_ToColor(borderCol), (Gdiplus::REAL)borderW);
        g.DrawPath(&pen, &path);
    }
}

// =========================================================================
//  字体抗锯齿（ClearType）辅助
// =========================================================================

// 增强版 settextstyle：在 EasyX 的基础上设置 CLEARTYPE_QUALITY
inline void setTextStyleAA(int height, int width, LPCTSTR face,
                           int escapement = 0, int orientation = 0,
                           int weight = FW_DONTCARE,
                           bool italic = false, bool underline = false,
                           bool strikeout = false)
{
    LOGFONT lf = {};
    lf.lfHeight      = height;    // 保持传入的值（正值为字元高度，负值为字符高度）
    lf.lfWidth       = width;
    lf.lfEscapement  = escapement;
    lf.lfOrientation = orientation;
    lf.lfWeight      = weight;
    lf.lfItalic      = italic;
    lf.lfUnderline   = underline;
    lf.lfStrikeOut   = strikeout;
    lf.lfCharSet     = DEFAULT_CHARSET;
    lf.lfQuality     = CLEARTYPE_QUALITY;
    _tcscpy_s(lf.lfFaceName, face);
    settextstyle(&lf);
}

// =========================================================================
}  // namespace AA
// =========================================================================

#endif // ANTIALIAS_H_INCLUDED
