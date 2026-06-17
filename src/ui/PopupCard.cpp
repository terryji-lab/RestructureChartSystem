#include "PopupCard.h"
#include "../chart/Chart.h"   // darkenColor, lightenColor

PopupCard::PopupCard(int x, int y, int w, int h, const tstring& title)
    : Card(x, y, w, h, title, 0)       // radius=0：直角矩形（弹窗风格）
    , m_accentColor(RGB(70, 130, 210)) // 默认强调色蓝色
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
    // 1. 阴影（偏移 10px，比普通 Card 更重，突出弹窗层次感）
    setfillcolor(m_shadowColor);
    solidrectangle(m_x + 10, m_y + 10, m_x + m_w + 10, m_y + m_h + 10);

    // 2. 主体背景
    Card::drawBody();

    // 3. 描边（2px 强调色边框）
    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 2);
    rectangle(m_x, m_y, m_x + m_w, m_y + m_h);

    // 4. 顶部 accent 色条
    setfillcolor(m_accentColor);
    solidrectangle(m_x, m_y, m_x + m_w, m_y + 8);

    // 5. 图标（成功 ✓ / 失败 ✕）
    int cx = m_x + 70;
    int cy = m_y + 78;
    setlinecolor(m_accentColor);
    setfillcolor(lightenColor(m_accentColor, 95));
    fillcircle(cx, cy, 28);

    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 4);
    if (m_isError)
    {
        line(cx - 12, cy - 12, cx + 12, cy + 12);
        line(cx + 12, cy - 12, cx - 12, cy + 12);
    }
    else
    {
        line(cx - 13, cy, cx - 3, cy + 10);
        line(cx - 3, cy + 10, cx + 16, cy - 12);
    }
    setlinestyle(PS_SOLID, 1);

    // 6. 标题
    setbkmode(TRANSPARENT);
    settextcolor(m_titleColor);
    settextstyle(28, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(m_x + 118, m_y + 48, m_title.c_str());

    // 7. 副标题/消息
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_NORMAL, false, false, false);
    settextcolor(darkenColor(m_titleColor, 35));
    outtextxy(m_x + 118, m_y + 92, m_message.c_str());

    // 8. 详情文字
    settextcolor(m_accentColor);
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(m_x + 118, m_y + 122, m_detail.c_str());

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
