#include "Card.h"

// ==================== Card ====================

Card::Card(int x, int y, int w, int h, const tstring& title, int radius)
    : m_x(x), m_y(y), m_w(w), m_h(h)
    , m_radius(radius)
    , m_title(title)
    , m_shadowColor(RGB(200, 200, 210))
    , m_bgColor(WHITE)
    , m_titleColor(RGB(50, 50, 80))
{
}

void Card::draw() const
{
    // 阴影
    if (m_radius > 0)
        drawRoundRectFill(m_x + 2, m_y + 3, m_w, m_h, m_radius, m_shadowColor);

    // 主体背景
    drawRoundRectFill(m_x, m_y, m_w, m_h, m_radius, m_bgColor);

    // 标题
    if (!m_title.empty())
    {
        settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
        settextcolor(m_titleColor);
        outtextxy(m_x + 14, m_y + 10, m_title.c_str());
    }
}

bool Card::contains(int mx, int my) const
{
    return mx >= m_x && mx <= m_x + m_w
        && my >= m_y && my <= m_y + m_h;
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

// ==================== DisplayBox ====================

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
    Card::draw();  // 背景 + 标题

    if (!m_text.empty())
    {
        settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);
        settextcolor(m_titleColor);
        outtextxy(m_x + 14, m_y + 36, m_text.c_str());
    }
}
