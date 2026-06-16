#include "Card.h"
#include "../chart/Chart.h"   // darkenColor, lightenColor

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

// ==================== PopupCard ====================

PopupCard::PopupCard(int x, int y, int w, int h, const tstring& title)
    : Card(x, y, w, h, title, 0)       // radius=0：直角矩形
    , m_accentColor(RGB(70, 130, 210))
    , m_okBtn(x + (w - 150) / 2, y + h - 68, 150, 42, _T("OK"),
              [this]() { m_shouldClose = true; })
    , m_shouldClose(false)
    , m_isError(false)
{
    m_okBtn.setColorText(RGB(255, 255, 255));
}

void PopupCard::setMessage(const tstring& msg)  { m_message = msg; }
void PopupCard::setDetail(const tstring& detail) { m_detail = detail; }
void PopupCard::setAccentColor(COLORREF c)       { m_accentColor = c; }
void PopupCard::setError(bool isError)           { m_isError = isError; }

void PopupCard::setButtonColors(COLORREF normal, COLORREF hover, COLORREF press)
{
    m_okBtn.setColorNormal(normal);
    m_okBtn.setColorHover(hover);
    m_okBtn.setColorPress(press);
    m_okBtn.setColorBorder(press);
}

void PopupCard::draw() const
{
    int w = m_w, h = m_h;
    int x = m_x, y = m_y;

    // 1. 阴影（偏移 10px）
    setfillcolor(m_shadowColor);
    solidrectangle(x + 10, y + 10, x + w + 10, y + h + 10);

    // 2. 主体背景
    setfillcolor(m_bgColor);
    solidrectangle(x, y, x + w, y + h);

    // 3. 描边
    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 2);
    rectangle(x, y, x + w, y + h);

    // 4. 顶部 accent 色条
    setfillcolor(m_accentColor);
    solidrectangle(x, y, x + w, y + 8);

    // 5. 图标（成功 ✓ / 失败 ✕）
    int cx = x + 70;
    int cy = y + 78;
    setlinecolor(m_accentColor);
    setfillcolor(lightenColor(m_accentColor, 95));
    fillcircle(cx, cy, 28);

    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 4);
    if (m_isError)
    {
        // ✕ 叉号
        line(cx - 12, cy - 12, cx + 12, cy + 12);
        line(cx + 12, cy - 12, cx - 12, cy + 12);
    }
    else
    {
        // ✓ 勾号
        line(cx - 13, cy, cx - 3, cy + 10);
        line(cx - 3, cy + 10, cx + 16, cy - 12);
    }
    setlinestyle(PS_SOLID, 1);

    // 6. 标题
    setbkmode(TRANSPARENT);
    settextcolor(m_titleColor);
    settextstyle(28, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(x + 118, y + 48, m_title.c_str());

    // 7. 副标题
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_NORMAL, false, false, false);
    settextcolor(darkenColor(m_titleColor, 35));
    outtextxy(x + 118, y + 92, m_message.c_str());

    // 8. 详情文字（文件名等）
    settextcolor(m_accentColor);
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(x + 118, y + 122, m_detail.c_str());

    // 9. OK 按钮
    m_okBtn.draw();
}

void PopupCard::showModal()
{
    m_shouldClose = false;

    BeginBatchDraw();
    draw();
    EndBatchDraw();

    ExMessage msg;
    while (!m_shouldClose)
    {
        msg = getmessage(EX_MOUSE | EX_KEY);

        if (msg.message == WM_KEYDOWN)
        {
            if (msg.vkcode == VK_RETURN || msg.vkcode == VK_SPACE || msg.vkcode == VK_ESCAPE)
                break;
        }
        else if (msg.message == WM_MOUSEMOVE)
        {
            if (m_okBtn.handleMouseMove(msg.x, msg.y))
            {
                BeginBatchDraw();
                draw();
                EndBatchDraw();
            }
        }
        else if (msg.message == WM_LBUTTONDOWN)
        {
            if (m_okBtn.handleMouseDown(msg.x, msg.y))
            {
                BeginBatchDraw();
                draw();
                EndBatchDraw();
            }
        }
        else if (msg.message == WM_LBUTTONUP)
        {
            if (m_okBtn.handleMouseUp(msg.x, msg.y))
            {
                BeginBatchDraw();
                draw();
                EndBatchDraw();
            }
        }
    }
}
