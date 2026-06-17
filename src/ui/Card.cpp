#include "Card.h"
#include "../chart/Chart.h"   // darkenColor, lightenColor
#include "../utils/AntiAlias.h"

// Card 基类

Card::Card(int x, int y, int w, int h, const tstring& title, int radius)
    : Widget(x, y, w, h)
    , m_radius(radius)
    , m_title(title)
    , m_shadowColor(RGB(200, 200, 210))
    , m_bgColor(WHITE)
    , m_titleColor(RGB(50, 50, 80))
{
}

void Card::drawShadow() const
{
    if (m_radius > 0)
        drawRoundRectFill(m_x + 2, m_y + 3, m_w, m_h, m_radius, m_shadowColor);
}

void Card::drawBody() const
{
    drawRoundRectFill(m_x, m_y, m_w, m_h, m_radius, m_bgColor);
}

void Card::drawTitle() const
{
    if (!m_title.empty())
    {
        AA::setTextStyleAA(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD);
        settextcolor(m_titleColor);
        outtextxy(m_x + 14, m_y + 10, m_title.c_str());
    }
}

void Card::draw() const
{
    drawShadow();
    drawBody();
    drawTitle();
}

void Card::setColors(COLORREF shadow, COLORREF bg, COLORREF title)
{
    m_shadowColor = shadow;
    m_bgColor = bg;
    m_titleColor = title;
}

void Card::setTitle(const tstring& t)
{
    m_title = t;
}

// DisplayBox 只读文本展示框

DisplayBox::DisplayBox(int x, int y, int w, int h, const tstring& title, int radius)
    : Card(x, y, w, h, title, radius)
{
}

void DisplayBox::setText(const tstring& t)
{
    m_text = t;
}

void DisplayBox::draw() const
{
    Card::draw();

    if (!m_text.empty())
    {
        AA::setTextStyleAA(18, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD);
        settextcolor(m_titleColor);
        outtextxy(m_x + 14, m_y + 36, m_text.c_str());
    }
}
